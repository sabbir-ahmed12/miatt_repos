# Lab 01 Quiz — Pixi Environment Setup

Answer each question in 3–5 sentences. Focus on demonstrating understanding rather than reciting commands.

---

**Q1. Why must Pixi's home and cache directories be redirected away from `~/.pixi` on a shared computing system? What would happen if every user on a multi-user research cluster left Pixi in its default location?**

- On a shared computing system, each user gets a limited space in the home directory. While setting up environments, Pixi unpacks hundreds of thousands of tiny text and header files into `~/.pixi`. So, the home directory will run out space quickly and the user will not be able work properly. Moreover, on most research clusters, home directories are shared across all login and compute nodes using a Network File System (NFS). NFS is optimized for low-latency access to small, infrequently changing files. So, if so many users try to run jobs at a time, and different compute nodes try to read files and load the associated shared object files to memory over NFS, it can slow down or feeze the system.
---

**Q2. The `pixi.toml` file pins specific platforms (e.g., `linux-64`, `osx-arm64`). Explain what role the platform list plays and why a research project that runs on both a Mac laptop and a Linux HPC cluster needs to declare multiple platforms.**

- The platforms list directs Pixi's dependency solver to pre-calculate and lock down exact package versions for all specified operating systems simultaneously. So, for a project spanning a Mac laptop and a Linux cluster, declaring both `osx-arm64` and `linux-64` guarantees that compatible versions of all dependencies exist for both environments, preventing version drift. Therefore, it allows the lockfile to capture OS-specific sub-dependencies—like Linux-specific C compilers or Apple-specific frameworks—so they are seamlessly resolved the moment the machine is switched. Furthermore, it helps defining target-specific dependencies, such as installing heavy GPU toolkits exclusively for the Linux cluster while keeping the local Mac environment clean and lightweight.
---

**Q3. The lab requires Python 3.10 and ITK 5.4.5 — not "the latest." Why is pinning exact versions important in a scientific computing context? Give a concrete example of what could go wrong if versions float freely.**

- The exact version is important for reproducibility of results. For example - if I've used a particular algorithm for a task in my research using ITK 5.4.5 version and used the results in a paper. After some months if I need to reproduce the results, and if I use a different version of ITK, it might happen that the algorithm that I've used in 5.4.5 is updated to use a different solver or different parameters by default in the newer version. So, it'll generate different results or output which is not expected.
---

**Q4. Describe the difference between `pixi.toml` and `pixi.lock`. Why does the lock file matter for reproducibility?**

- `pixi.toml` is the human-edited configuration file where a project’s dependencies are declared, version ranges, and environments. In contrast, `pixi.lock` is an auto-generated file that records the exact resolved versions of all dependencies (including transitive ones) for a specific platform. The lock file matters for reproducibility because it ensures that anyone installing the project gets the exact same dependency versions, avoiding differences caused by version updates or resolver changes. Without it, two environments created at different times could behave differently even with the same `pixi.toml`.
---

**Q5. The README deliberately contains errors and warns against blind copy-paste. From a research-computing perspective, why is it dangerous to execute environment-setup commands without understanding them? Relate your answer to the concept of a reproducible research workflow.**

- Blindly executing environment-setup commands is dangerous because I may unknowingly install incorrect dependencies, overwrite existing configurations, or introduce subtle version mismatches that affect results. In research computing, this can lead to non-reproducible outcomes, where the same code produces different results across systems or over time. It also makes debugging difficult, since I don’t understand what was actually installed or configured. A reproducible workflow requires deliberate control over environments, dependencies, and versions—something that is undermined when commands are run without understanding.
---

**Q6. Explain how pre-commit hooks (Black, Ruff) fit into the Pixi environment. Why is it valuable to have code formatting and linting run automatically before every commit, rather than relying on researchers to run these tools manually?**

- Pre-commit hooks like Black and Ruff are installed inside the Pixi-managed environment, so they use the exact versions specified in the project’s dependencies. This ensures that formatting and linting are consistent across all contributors and match the reproducible environment defined by Pixi. When configured as pre-commit hooks, these tools automatically run before each commit, checking and fixing code without requiring manual intervention. This automation is valuable because it eliminates human error and inconsistency. Also, researchers might forget to run the tools or use different versions locally. By enforcing formatting and linting at commit time, the codebase stays clean, consistent, and easier to reproduce and review.
---

**Q7. The environment variables `PIXI_HOME`, `PIXI_CACHE_DIR`, `XDG_CACHE_HOME`, and `UV_CACHE_DIR` each control a different storage location. In your own words, explain why there are multiple cache-related variables instead of just one, and what each one governs.**

- There are multiple cache-related variables because different tools and layers of the environment manage their own data, and separating them avoids conflicts while improving performance and organization. `PIXI_HOME` controls where Pixi stores its core metadata and environments, while `PIXI_CACHE_DIR` is specifically for Pixi’s downloaded packages and build artifacts. `XDG_CACHE_HOME` is a broader, cross-application standard that many tools follow to decide where general cache files should live. `UV_CACHE_DIR` is dedicated to the UV package manager’s cache, keeping its Python package downloads separate. Having separate caches makes the system more modular and reproducible, since each tool can manage, clear, or reuse its own cache independently without affecting others.
---

**Q8. Imagine you are starting a new medical-imaging research project that must be reproducible two years from now and runnable by collaborators on different operating systems. Based on what you learned in this lab, outline the first five setup steps you would take and justify each one.**

- **Use a Robust Environment Setup Tool:** There are different tools to setup environment to use different scientific libraries such as - pip, conda, uv, pixi. I want to a choose a tool which is robust to different setup environments. For example - most of the projects I find on GitHub use pip to setup environment. However, when I try to setup the same environment on HPC, it throws build error because `pip` doesn't use binaries unlike `conda`. I came to know that `pixi` is robust in this case and it can solve such dependency issues better than uv as it uses both PyPI and conda-forge.

- **Initialize Git Repository:** Version control via Git is non-negotiable for tracking changes, reviewing code, and maintaining a history of why certain decisions were made.

- **Standardize Project Directory:** Maintaining a good project structure helps people navigate the project easily. For example - keeping all the codes in `src` directory, notebooks in a different directory or data in a different directory.

- **Setup Automated Formatting and Linting Tools:** Enforcing consistent formatting and linting automatically ensures code quality and readability across collaborators.

- **Document the Workflow:** I would clearly document how to install the environment, run experiments, reproduce results and how to contribute to the project etc. Good documentation is essential for future reproducibility.
