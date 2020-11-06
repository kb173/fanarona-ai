#!/bin/bash

command -v clang-format
if [[ $? -ne 0 ]]; then
  echo "Attempting to install clang-format..."
  if [[ "$OSTYPE" == "darwin"* ]]; then
    brew install clang-format
  fi
  if [[ "$OSTYPE" == "linux"* ]]; then
    apt-get update && sudo bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)" && sudo apt-get install -y clang-format-11
  fi
fi

cat <<EOF > .git/hooks/pre-commit
#!/bin/sh

FILES=\$(git diff --cached --name-only --diff-filter=ACMR "*.h" "*.cpp" | sed 's| |\\ |g')
[ -z "\$FILES" ] && exit 0

echo "Applying auto formatting..."

# Format all changed files
echo "\$FILES" | xargs clang-format -i -style=file

# Add back the modified files to staging
echo "\$FILES" | xargs git add
EOF

chmod +x .git/hooks/pre-commit

echo "Installed pre-commit hook."
