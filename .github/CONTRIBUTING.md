# Contributing to OCCT

## Contribute Code

In order to contribute code to OCCT you must sign the Contributor License Agreement (CLA).

- [Introduction: What is CLA and Why It Is Needed?](CLA_SIGNING.md)
- [Contribution License Agreement](CLA.md)
- [CLA Submission Form](https://dev.opencascade.org/get_involed/cla_submission_form)

### Steps to Submit Your Contribution

1. **Fork the repository** on GitHub: https://github.com/Open-Cascade-SAS/OCCT
2. **Create a feature branch** from an up-to-date `master`. Any descriptive branch name is fine.
3. **Develop your change** following the [OCCT Coding Rules](https://dev.opencascade.org/doc/overview/html/occt_contribution__coding_rules.html). Format the source with `clang-format` using the `.clang-format` configuration shipped in the repository.
4. **Test locally**. Run the existing DRAW test suite and `OpenCascadeGTest` tests; create a test case for the change when applicable.
5. **Open a Draft Pull Request** against `Open-Cascade-SAS/OCCT:master`. CI/CD pipelines run on every push and report build, style and test results.
6. **Mark the PR Ready for Review** once CI is green. A maintainer will review the change and request updates as needed.
7. **Address review feedback** by pushing additional commits to the same branch; CI re-runs automatically.
8. After approval the PR is merged (typically squashed) by a maintainer.

PR titles follow the convention `Group - Summary`, for example `Modeling - Fix crash in BRepAlgoAPI_Fuse on null shapes`.

See the [Contribution Workflow](https://dev.opencascade.org/doc/overview/html/occt_contribution__contribution_workflow.html) and [Git Guide](https://dev.opencascade.org/doc/overview/html/occt_contribution__git_guide.html) for full details.

## Contribute Ideas

Every big thing starts with an idea. We appreciate your vision on how to enhance Open CASCADE Technology. Share your thoughts on the [OCCT development forum](https://dev.opencascade.org/forums) or [GitHub Discussions](https://github.com/Open-Cascade-SAS/OCCT/discussions), and submit bug reports or feature requests via [GitHub Issues](https://github.com/Open-Cascade-SAS/OCCT/issues) using the provided issue templates.

- **Forum**: [OCCT development forum](https://dev.opencascade.org/forums)
- **Discussions**: [GitHub Discussions](https://github.com/Open-Cascade-SAS/OCCT/discussions)
- **Reporting Issues**: [GitHub Issues](https://github.com/Open-Cascade-SAS/OCCT/issues)

## Contribute Knowledge

Know a lot about OCCT? You can help educate other OCCT users by writing OCCT-related articles or blog posts, creating samples, examples or tutorials, and even by writing a book about OCCT. If you would like us to share your content via official OCCT resources, please [contact us](https://dev.opencascade.org/webform/contact_us).

## Contribute Documentation and Tutorials

Do you have an idea on how to make OCCT documentation easier for new users or more exhaustive for professionals? Want to help with proofreading and technical writing? Translating OCCT documentation and materials into your native language is also very much appreciated. Submit documentation improvements as [Pull Requests](https://github.com/Open-Cascade-SAS/OCCT/pulls) against files under `dox/`, or open a [GitHub Issue](https://github.com/Open-Cascade-SAS/OCCT/issues) describing what you would like to see improved.

## Contribute to the Community

At any community interaction point, we value your support in starting forum topics or replying to other users' posts, joining Open CASCADE social networks, participating in GitHub or Stack Overflow projects, and just spreading the word about OCCT. Welcome to our community!
