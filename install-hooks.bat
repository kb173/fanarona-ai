echo "FILES=$(git diff --cached --name-only --diff-filter=ACMR ""*.h"" ""*.cpp"" | sed 's| |\\ |g')" > .git/hooks/pre-commit
echo "echo ""Applying auto formatting...""" >> .git/hooks/pre-commit
echo "echo ""$FILES"" | xargs clang-format -i -style=file" >> .git/hooks/pre-commit
echo "echo ""$FILES"" | xargs git add" >> .git/hooks/pre-commit
