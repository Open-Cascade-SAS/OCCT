Contribution Workflow {#occt_contribution__contribution_workflow}
====================================
@tableofcontents 

@section occt_contribution_intro Introduction

The purpose of this document is to describe the standard workflow for processing contributions to OCCT.

OCCT development is hosted on GitHub at https://github.com/Open-Cascade-SAS/OCCT.
All contributions are submitted as Pull Requests (PRs) through the GitHub interface.

@subsection occt_contribution_intro_cla Contributor License Agreement

Before contributing, you must sign the Contributor License Agreement (CLA).
See the [CLA submission form](https://dev.opencascade.org/get_involed/cla_submission_form) for details.

@subsection occt_contribution_intro_pr PR Title Convention

PR titles should follow the pattern `<Group> - <Summary>`, for example:

    Modeling - Modify surface evaluation
    Visualization - Fix crash on empty shape display
    Data Exchange - Add support for STEP AP242 edition 4

The group identifies the OCCT module or component affected by the change.
The summary should be a short, imperative description of what the change does.

@section occt_contribution_workflow Standard Pull Request Workflow

@subsection occt_contribution_workflow_overview Overview

The contribution workflow follows these steps:

1. **Fork and Branch**: Fork the OCCT repository and create a feature branch.
2. **Develop**: Implement the change following @ref occt_contribution__coding_rules "Coding Rules".
3. **Test**: Run existing tests to ensure no regressions. Create a test case when applicable (see @ref occt_contribution__tests "Automated Test System").
4. **Create Draft PR**: Submit a Draft Pull Request on GitHub. CI/CD tests will run automatically.
5. **Ready for Review**: When the change is complete and CI/CD passes, mark the PR as "Ready for Review".
6. **Code Review**: A maintainer reviews the change. Feedback is provided as PR comments.
7. **Address Feedback**: If changes are requested, update the PR. The CI/CD will re-run.
8. **Merge**: Once approved and all checks pass, the PR is merged.

@subsection occt_contribution_workflow_draft Draft Pull Request

Start with a Draft PR to signal that the work is in progress. CI/CD pipelines will run on every push, providing early feedback on build and test results.

Create the PR from your fork's feature branch to the `IR` branch of the official repository.

@subsection occt_contribution_workflow_review Code Review

When the Draft PR is ready, mark it as "Ready for Review":

- A reviewer will examine the changes for correctness, compliance with @ref occt_contribution__coding_rules "Coding Rules", and completeness.
- Review comments are posted as PR comments. Use GitHub's "Request changes" or "Approve" features.
- Address review feedback by pushing additional commits to the branch. CI/CD re-runs automatically.

@subsection occt_contribution_workflow_merge Merge

After approval and successful CI/CD checks, the PR is merged by a maintainer.
PRs are typically squashed into a single commit on the IR branch.

@subsection occt_contribution_workflow_ci CI/CD Testing

The following automated checks run on every PR:
- Build on multiple platforms (Windows, Linux, macOS)
- Coding style verification (clang-format)
- DRAW test suite execution

Results are visible in the GitHub Actions tab of the PR.

@subsection occt_contribution_workflow_branch Branch Naming

There are no restrictions on branch names. Choose a descriptive name that reflects your change (e.g., `fix-empty-shape-crash`, `add-step-ap242-ed4`).

@subsection occt_contribution_workflow_commit Commit Messages

Commit messages should be descriptive. The recommended format:

    Modeling - Fix crash in BRepAlgoAPI_Fuse on empty shapes

The commit description should explain:
- **Problem**: What was wrong
- **Change**: What was modified and why
- **Result**: The expected behavior after the fix

@subsection occt_contribution_workflow_code Requirements to the Code

The change should comply with OCCT @ref occt_contribution__coding_rules "Coding Rules".
Re-formatting of unrelated code should be avoided unless done in a dedicated PR.

@subsection occt_contribution_workflow_test Providing a Test Case

For functional changes, a test case should be created (unless an existing test already covers it).
See @ref testmanual_intro_quick_create "Creating a New Test" for instructions.

Test data files (CAD models, etc.) should be attached to the GitHub Issue or PR.

@subsection occt_contribution_workflow_doc Updating Documentation

If the change affects documented functionality, the corresponding documentation in the `dox/` directory should be updated in the same PR.

Changes that break API compatibility must be documented in @ref occt__upgrade "Upgrade from previous OCCT versions".

@section occt_contribution_issue_attrs Reporting Issues

Open issues through the GitHub web interface using the provided issue templates:

- **Bug Report** - for reproducible defects. The template asks for a description, expected and actual behavior, a sample script or code, OS, compiler, bitness and OCCT version.
- **Feature Request** - for new functionality or enhancements. The template asks for a description, use case, expected benefits and any additional context.

Templates apply the appropriate labels automatically; there is no need to set them by hand. Maintainers may add further classification labels (component, priority, etc.) during triage.

@section occt_contribution_nonstd Additional Notes

@subsection occt_contribution_nonstd_rebase Updating Branches

If your branch falls behind IR, rebase it:

    git fetch upstream
    git rebase upstream/IR

Force-push the updated branch:

    git push --force origin <your-branch-name>

@subsection occt_contribution_nonstd_feedback Handling Feedback

Review feedback is provided as PR comments. Address each comment by:
- Pushing a fix commit to the same branch
- Replying to the comment thread explaining the resolution
- Resolving the conversation when done

@subsection occt_contribution_nonstd_relate Related Issues

If your PR relates to an existing GitHub issue, use "Closes #123" or "Fixes #456" in the PR description to automatically link and close it on merge.
