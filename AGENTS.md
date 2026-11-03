# AGENTS.md

Rules for people and coding agents working in this repository.

## Owned fork contract

This repository is the user's public fork [paulnamalomba/OCCT](https://github.com/paulnamalomba/OCCT) of Open CASCADE Technology (OCCT). SESKAFEA's frontend geometry workers (kernel, STEP and IGES translation) build OCCT from an exact commit of this fork. The commit is pinned with the SHA-256 of its GitHub archive in the frontend's `cmake/FrontendDependencies.json`. All intentional OCCT source changes for SESKAFEA belong here as reviewed commits, never as patch files hidden in consumer build scripts. Publish a fork commit before a consumer pins it.

**Required (user, 3 November 2026):** the fork is public and independently maintained, and upstream `Open-Cascade-SAS/OCCT` is a read/fetch reference only:

- Do not configure a push URL for upstream.
- Do not push branches or tags upstream, and do not open pull requests, issues or comments there.
- GitHub records this repository as a fork, so tools may default to the upstream base. Every push, and any pull request the user asks for, must explicitly target `paulnamalomba/OCCT`. Create pull requests only when the user asks.

### Branches

- `native/v8.0.1` starts from upstream tag `V8_0_1` (`b8f597c677811d1f9f4d8a97f5ae2825c0353a42`, 2026-07-30), whose source archive has SHA-256 `dba62b81078dd43cec23feba89432be301582341001edad1b93342ad8bda35ea`. It carries the SESKAFEA changes as separate commits.
- A later upstream release gets a new `native/v<release>` branch. Each change is re-applied and re-qualified there; a version number never implies compatibility.
- Never rewrite or force-push a commit that a consumer has pinned.

### Change rules

- **Required:** one reviewed change per commit. The message states the defect, its source location, the remedy and the evidence that tested both the raw defect and the remedy.
- **Required:** keep each change minimal and local. Preserve OCCT's public API and ABI unless the user decides otherwise, and keep upstream naming and style in OCCT sources.
- **Required:** a change is adopted only after it has an independent review and the consumer's acceptance gates pass on the fork commit. A zero exit is never success evidence by itself.
- Keep upstream's version macros (`OCC_VERSION_*`) unchanged. The internal version below is separate.

### Internal version (Proposed)

`OCCT_INTERNAL_VERSION` holds the fork's four-part identity, `<breaking>.<major>.<minor>.<build>`, seeded unbuilt at `8.0.1.0`. A build is qualifying when the consumer's acceptance builds this fork commit from its archive and passes its required checks. The first qualifying build advances the version to `8.0.1.1`. Major, minor and build each range from 0 to 9, and overflow carries (`8.0.1.9 -> 8.0.2.0`), as in the MS-MPI fork. Failed, configure-only and documentation-only work does not advance it. Notes are Markdown, grouped under `.commits/<breaking>.<major>.<minor>.md`. The user may re-seed or change this scheme.

### Change register

| Commit subject | Files | Evidence |
| --- | --- | --- |
| Throw from AllocateOptimal when malloc fails | `TKernel/Standard/Standard.cxx` | SESKAFEA X1 checked-allocation controls; frontend C1.0c pin |
| Return null for null STEP vectors and directions | `TKDESTEP/StepToGeom/StepToGeom.cxx` | SESKAFEA X3 P1 (inline-vector `0xC0000005`) |
| Widen the STEP and IGES real write buffers | `StepData_StepWriter.cxx`, `IGESData_IGESWriter.cxx` | SESKAFEA X3 P2 (`0xC0000409` on 17-digit reals) |
| Bound Interface_FloatWriter formatting | `TKXSBase/Interface/Interface_FloatWriter.cxx` | SESKAFEA X3 P3 (truncated zero suppression, format overflow) |

## Shared instruction entry points

`CLAUDE.md`, `GEMINI.md` and `ANTIGRAVITY_INSTRUCTIONS.md` reference `@AGENTS.md`; keep substantive rules here. Upstream's README and coding rules remain in force except where this file supersedes them.
