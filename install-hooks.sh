#!/bin/bash

if [[ "$OSTYPE" == "darwin"* ]]; then
  clang_format_command="clang-format"
  clang_tidy_command="run-clang-tidy"
  command -v clang_format_command
  if [[ $? -ne 0 ]]; then
    echo "Attempting to install clang-format..."
    brew install clang-format
  fi

  command -v clang_tidy_command
  if [[ $? -ne 0 ]]; then
    echo "Attempting to install clang-tidy..."
    brew install clang-tidy
  fi

  command -v "bear"
  if [[ $? -ne 0 ]]; then
    brew install bear;
  fi
fi
if [[ "$OSTYPE" == "linux"* ]]; then
  clang_format_command="clang-format-11"
  clang_tidy_command="run-clang-tidy-11"
  command -v $clang_format_command
  if [[ $? -ne 0 ]]; then
    echo "Attempting to install clang-format..."
    sudo apt-get update && sudo bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)" && sudo apt-get install -y clang-format-11
    command -v $clang_tidy_command
    if [[ $? -ne 0 ]]; then
      echo "Attempting to install clang-tidy..."
      sudo apt-get install -y clang-tidy-11
    fi
  else
    command -v $clang_tidy_command
    if [[ $? -ne 0 ]]; then
      echo "Attempting to install clang-tidy..."
      sudo apt-get update && sudo bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)" && sudo apt-get install -y clang-tidy-11
    fi
  fi
    command -v "bear"
    if [[ $? -ne 0 ]]; then
      sudo apt-get update;
      sudo apt-get install bear;
    fi
fi
make clean
bear make

cat <<EOF > .git/hooks/pre-commit
#!/bin/sh

FILES=\$(git diff --cached --name-only --diff-filter=ACMR "*.h" "*.cpp" | sed 's| |\\ |g')
[ -z "\$FILES" ] && exit 0

echo "Applying auto formatting..."

# Format all changed files
echo "\$FILES" | xargs $clang_format_command -i -style=file

# Check all files for non white space formatting
$clang_tidy_command

# Add back the modified files to staging
echo "\$FILES" | xargs git add
EOF

chmod +x .git/hooks/pre-commit

echo "Installed pre-commit hook."
