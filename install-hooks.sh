#!/bin/bash

command -v clang-format
if [[ $? -ne 0 ]]; then
  echo "Attempting to install clang-format..."
  if [[ "$OSTYPE" == "darwin"* ]]; then
    brew install clang-format
  fi
  if [[ "$OSTYPE" == "linux"* ]]; then
    apt install build-essential
  fi
fi

cat <<EOF > .git/hooks/pre-commit
#!/bin/bash
echo "Applying auto formatting..."
clang-format -i -style=Microsoft *.h *.cpp
EOF

chmod +x .git/hooks/pre-commit

echo "Installed pre-commit hook."
