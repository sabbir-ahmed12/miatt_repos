import os
import sys
from pathlib import Path
import itk

REQUIRED_PYTHON = (3, 10)
REQUIRED_ITK = "5.4.5"

python_ok = sys.version_info[:2] == REQUIRED_PYTHON
itk_ok = itk.__version__ == REQUIRED_ITK
precommit_hook = Path(".git") / "hooks" / "pre-commit"
precommit_ok = precommit_hook.is_file() and os.access(precommit_hook, os.X_OK)


print(
    f"Python version : {sys.version.split()[0]}  {'OK' if python_ok else 'FAIL'} (required 3.10.x)"
)
print(
    f"ITK version    : {itk.__version__}  {'OK' if itk_ok else 'FAIL'} (required {REQUIRED_ITK})"
)
print(
    f"pre-commit     : {'OK' if precommit_ok else 'FAIL'} (run: pixi run pre-commit install)"
)
