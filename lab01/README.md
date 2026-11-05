# Lab Assignment

This lab assignment sets up a Pixi environment for creating an ITK Python module for medical image processing.

** THERE ARE ERRORS IN THE DOCUMENTATION BELOW **  Do not blindly copy-n-paste the commands to complete this
assignment.  You need to understand the commands, and adapt the hints below to work for your environment.

## Key Elements

- [ ] Do NOT allow the home account to be used for pixi install.
   - [ ] The entire pixi cache and install directories should be in /nfsscratch/Users/${LOGNAME}.
   - [ ] Ensure that environment variables redirect all Pixi defaults to a specified directory
- [ ] Small Python program that verifies the Python version is 3.10 and ITK version is 5.4.5
- [ ] Pixi env shall provide
  - [ ] Python 3.10
  - [ ] Ruff
  - [ ] Black
  - [ ] pre-commit
  - [ ] ITK version 5.4.5

## Setup Steps

### Step 0 — Initialize a git repository

Pre-commit hooks require a git repository to be present. If you are starting from scratch:

```sh
git init
git add .
git commit -m "Initial commit"
```

If you cloned this repository, this step is already done.

### Step 1 — Redirect Pixi away from your home directory

Before running any Pixi commands, set these environment variables. This prevents Pixi from writing its cache and toolchain into `~/.pixi`.

```sh

# local user directory
export PREFERRED_STORAGE=/nfsscratch/Users/${LOGNAME}

#PIXI INSTALL
export PIXI_HOME=${PREFERRED_STORAGE}/.pixi
export PIXI_CACHE_DIR=/opt/Users/${LOGNAME}/.pixi/cache
export XDG_CACHE_HOME=${PREFERRED_STORAGE}/.cache
export UV_CACHE_DIR=$XDG_CACHE_HOME/uv
```

Add these to your shell profile (`~/.zshrc` or `~/.bashrc`) to make them persistent across sessions, then reload:

```sh
source ~/.zshrc   # or ~/.bashrc
```

Verify the variables are set:

```sh
echo $PIXI_HOME
echo $PIXI_CACHE_DIR
```

### Step 2 - Install pixi

Read instructions for installing pixi from https://pixi.prefix.dev/latest/installation

### Step 3 — Install the Pixi environment

With the environment variables set, install all project dependencies declared in `pixi.toml`:

```sh
pixi install
```

This will:
- Resolve compatible versions of all packages for your platform
- Download the binaries from conda-forge and PyPI
- Create a `pixi.lock` file pinning exact versions
- Create a local `.pixi/` environment inside the project directory

### Step 4 — Install pre-commit hooks

Install the git hooks so that Black and Ruff run automatically on every commit:

```sh
pixi run pre-commit install
```

This installs the hooks defined in `.pre-commit-config.yaml` into `.git/hooks/pre-commit`. From this point on, every `git commit` will automatically:
- Fix trailing whitespace and missing newlines
- Check for YAML syntax errors and merge conflict markers
- Format code with Black
- Lint and auto-fix with Ruff

To run the hooks manually against all files without committing:

```sh
pixi run pre-commit run --all-files
```

### Step 5 — Verify the environment

Run the verification script to confirm Python, ITK, and pre-commit are all correctly set up:

```sh
pixi run verify
```

All three checks should show `OK`. If pre-commit shows `FAIL`, re-run Step 3.

### Step 6 — Activate the environment (optional)

To drop into an interactive shell with the environment active:

```sh
pixi shell
```

## Common Pixi Commands

| Command | Description |
|---------|-------------|
| `pixi install` | Install all dependencies from `pixi.toml` |
| `pixi run <task>` | Run a named task inside the environment |
| `pixi shell` | Activate the environment in your current shell |
| `pixi list` | List installed packages and their versions |
| `pixi clean` | Remove the local `.pixi/` environment |
