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

### Internal Version And Build Records

**Required (user, 10 November 2026):** owned dependency forks follow the established fork practice of the [MS-MPI fork](https://github.com/paulnamalomba/microsoft-mpi-native-windows). This section applies that practice to this fork.

`OCCT_INTERNAL_VERSION` is the fork's authoritative internal four-part version, independent of upstream's `OCC_VERSION_*` macros:

```text
<breaking>.<major>.<minor>.<build-number>
```

- The seed is `8.0.1.0`: upstream release 8.0.1 plus build zero. It is unbuilt, and it is not a release or a passing-build claim. The first qualifying build advances it to `8.0.1.1`. Build zero after a later rollover can identify a successful build.
- Major, minor and build each range from 0 through 9. A successful build increments the fourth field. Overflow carries to the next field and resets the following fields to zero: `8.0.1.9 -> 8.0.2.0`, `8.0.9.9 -> 8.1.0.0`, `8.9.9.9 -> 9.0.0.0`. Coordinate allocation across branches, and never publish different artifacts under the same complete identity.
- A build qualifies when the consumer's acceptance builds this fork commit from its pinned archive and its required checks pass. Record the actual scope. Failed, cancelled, skipped, configure-only, documentation-only and no-op work does not advance any field.
- Explicit breaking, major or minor decisions select a candidate family. They do not change `OCCT_INTERNAL_VERSION` until an accepted build confirms that candidate.
- Do not encode the internal identity into upstream's `OCC_VERSION_*` macros.
- Group notes by the first three fields in Markdown: `.commits/<breaking>.<major>.<minor>.md`, so the 8.0.1 family shares [.commits/8.0.1.md](.commits/8.0.1.md). Use the `create-commit-notes` skill at seed initialization and at every minor, major or breaking change, including carries. Append every accepted build. Never collapse or rewrite earlier transitions.
- Each accepted build gets `.commits/builds/<version>.json` and a family-note record. The build record holds `accepted`, `status`, `source_commit` (the 40-character fork commit that the consumer built), checks with results, artifact or archive hashes and remaining limits. Persist the accepted version and its record in the same change. This record layout is Proposed.
- `.commits/manage-repo.ps1` reads `OCCT_INTERNAL_VERSION` and never changes it. Any `-Version` must equal it. Mutating actions require an attached branch, a clean tree and `-Yes`. Pushes target only `paulnamalomba/OCCT`. Tags use the `native/v<internal version>` namespace and are never replaced. Run `pwsh -NoProfile -File .commits/manage-repo.ps1 -Action check` for its self-checks, and add `-WhatIf` to preview a mutating action.
### Change register

| Commit subject | Files | Evidence |
| --- | --- | --- |
| Throw from AllocateOptimal when malloc fails | `TKernel/Standard/Standard.cxx` | SESKAFEA X1 checked-allocation controls; frontend C1.0c pin |
| Return null for null STEP vectors and directions | `TKDESTEP/StepToGeom/StepToGeom.cxx` | SESKAFEA X3 P1 (inline-vector `0xC0000005`) |
| Widen the STEP and IGES real write buffers | `StepData_StepWriter.cxx`, `IGESData_IGESWriter.cxx` | SESKAFEA X3 P2 (`0xC0000409` on 17-digit reals) |
| Bound Interface_FloatWriter formatting | `TKXSBase/Interface/Interface_FloatWriter.cxx` | SESKAFEA X3 P3 (truncated zero suppression, format overflow) |

## Shared instruction entry points

`CLAUDE.md`, `GEMINI.md` and `ANTIGRAVITY_INSTRUCTIONS.md` reference `@AGENTS.md`; keep substantive rules here. Upstream's README and coding rules remain in force except where this file supersedes them.
