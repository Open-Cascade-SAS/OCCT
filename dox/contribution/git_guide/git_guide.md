Guide to using Git for OCCT development {#occt_contribution__git_guide}
=================================

@tableofcontents 

@section occt_gitguide_1 Overview

@subsection occt_gitguide_1_1 Purpose

This document provides a practical introduction to Git for OCCT contributors.
OCCT development is hosted on GitHub at https://github.com/Open-Cascade-SAS/OCCT.

For experienced Git users, sections 1 and 3 are sufficient to get started.
Please also read the @ref occt_contribution__contribution_workflow "Contribution Workflow" for the overall process.

@subsection occt_gitguide_1_2 Repository URL

The official OCCT Git repository:

    https://github.com/Open-Cascade-SAS/OCCT

Clone via HTTPS:

    git clone https://github.com/Open-Cascade-SAS/OCCT.git

Or via SSH:

    git clone git@github.com:Open-Cascade-SAS/OCCT.git

@subsection occt_gitguide_1_3 Repository Content

- **master** branch: the current development version. Official releases are marked by tags.
- **IR** branch: the weekly integration branch where contributions are merged after review and CI/CD checks.
- Topic branches for feature development, bug fixes, and improvements.

@subsection occt_gitguide_1_4 Quick Rules

- Configure your Git user name and email so that commits are properly attributed:
  ~~~~
  git config --global user.name "Your Full Name"
  git config --global user.email "your@email.com"
  ~~~~
- Create a fork of the repository and push your branches to your fork.
- Submit changes via Pull Requests (PRs).
- Branches that pass review and CI/CD checks are merged by maintainers (typically squashed into a single commit per PR).

@section occt_gitguide_2 Setting Up Git

@subsection occt_gitguide_2_1 Prerequisites

- Install Git from https://git-scm.com/downloads
- On Windows, Git for Windows is recommended, optionally with TortoiseGit for GUI support.
- On Linux and macOS, use your system package manager or the Git packages recommended by your distribution.
- Configure your identity before creating commits:

    ~~~~
    git config --global user.name "Your Full Name"
    git config --global user.email "your@email.com"
    ~~~~

- On Windows, keep line endings predictable for cross-platform patches:

    ~~~~
    git config --global core.autocrlf true
    git config --global core.safecrlf true
    ~~~~

If you use TortoiseGit or another graphical client, configure the same user name, email address, and SSH client in that tool.

@subsection occt_gitguide_2_2 Authentication

GitHub supports both HTTPS and SSH remotes.
HTTPS is usually the simplest option for cloning and pushing through Git Credential Manager.
SSH is convenient when you already use SSH keys:

~~~~
ssh-keygen -t ed25519 -C "your@email.com"
~~~~

Add the generated public key to your GitHub account, then check access with:

~~~~
ssh -T git@github.com
~~~~

@subsection occt_gitguide_2_3 Fork and Clone

1. Fork the repository on GitHub: click **Fork** at https://github.com/Open-Cascade-SAS/OCCT
2. Clone your fork:

    ~~~~
    git clone https://github.com/YOUR_USERNAME/OCCT.git
    cd OCCT
    ~~~~

3. Add the upstream repository as a remote:

    ~~~~
    git remote add upstream https://github.com/Open-Cascade-SAS/OCCT.git
    ~~~~

@section occt_gitguide_3 Development Workflow

@subsection occt_gitguide_3_1 Create a Branch

Always create branches from an up-to-date master:

~~~~
git checkout master
git pull upstream master
git checkout -b my-feature-branch
~~~~

Use any descriptive name for your branch (e.g., `fix-empty-shape-crash`, `step-ap242-ed4`).

@subsection occt_gitguide_3_2 Make Changes

Develop your change, following @ref occt_contribution__coding_rules "OCCT Coding Rules".
Format your code using clang-format with the configuration provided in the repository.

@subsection occt_gitguide_3_3 Commit

Commit your changes with a meaningful message:

~~~~
git add .
git commit -m "Modeling - Fix crash in BRepAlgoAPI_Fuse on null shapes"
~~~~

The first line should follow the `Group - Summary` format (see @ref occt_contribution__contribution_workflow "Contribution Workflow").
Provide a detailed description in the commit body if needed.

@subsection occt_gitguide_3_4 Push and Create a Pull Request

Push your branch to your fork:

~~~~
git push origin my-feature-branch
~~~~

Then create a Pull Request on GitHub:

- Go to your fork on GitHub
- Click **New Pull Request**
- Set the base to `Open-Cascade-SAS/OCCT/IR` and compare to your branch
- Start with a **Draft PR** to indicate work in progress
- When ready, click **Ready for Review**

@subsection occt_gitguide_3_5 CI/CD and Review

The CI/CD pipeline runs automatically on every push:
- Build checks (Windows, Linux, macOS)
- Code style verification (clang-format)
- DRAW test suite

Results appear in the GitHub Actions tab of the PR.

A reviewer examines the changes and provides feedback as PR comments.
Address feedback by pushing new commits to the same branch.
CI/CD re-runs automatically.

@subsection occt_gitguide_3_6 Merge

Once approved and all checks pass, a maintainer merges the PR.
PRs are typically squashed into a single commit on IR.

@subsection occt_gitguide_3_7 Keep Your Fork Updated

Regularly sync your fork with upstream:

~~~~
git checkout master
git pull upstream master
git push origin master
~~~~

@section occt_gitguide_4 Rebasing

If your branch falls behind IR, rebase it:

~~~~
git checkout my-feature-branch
git pull --rebase upstream IR
~~~~

If conflicts occur, resolve them manually, then:

~~~~
git add .
git rebase --continue
~~~~

After rebasing, force-push to your fork:

~~~~
git push --force origin my-feature-branch
~~~~

**Note:** Do not force-push if others are working on the same branch.

@section occt_gitguide_5 Cleanup

After your PR is merged, delete the local and remote branch:

~~~~
git checkout IR
git branch -d my-feature-branch
git push origin --delete my-feature-branch
~~~~

Regularly prune stale remote tracking branches:

~~~~
git fetch --prune upstream
~~~~
