echo "Applying auto formatting..." >> .git/hooks/pre-commit
echo "clang-format -i -style=Microsoft *.h *.cpp" >> .git/hooks/pre-commit
