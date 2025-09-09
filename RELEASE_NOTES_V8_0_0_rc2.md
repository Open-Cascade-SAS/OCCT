# Open CASCADE Technology Version 8.0.0 Release Candidate 2

Open Cascade is delighted to announce the release of `Open CASCADE Technology version 8.0.0 Release Candidate 2` to the public.

## Overview

Version `V8_0_0_rc2` is a candidate release incorporating over 80 improvements and bug fixes compared to version V8_0_0_rc1, bringing the total improvements since version 7.9.0 to over 130 changes.

## What is a Release Candidate

A Release Candidate is a tag on the master branch that has completed all test rounds and is stable to use.
Release candidates progress in parallel with maintenance releases, with the difference that maintenance releases remain binary compatible with minor releases and cannot include most improvements.
The cycle for a release candidate is planned to be 5-10 weeks, while maintenance releases occur once per quarter.

## What's New in OCCT 8.0.0-rc2

### Core

* Upgraded minimum C++ version requirement to C++17 [#537](https://github.com/Open-Cascade-SAS/OCCT/pull/537)
* Optimized gp_Vec, gp_Vec2d, gp_XY, and gp_XYZ classes for better performance [#578](https://github.com/Open-Cascade-SAS/OCCT/pull/578)
* Reworked atomic and Standard_Condition implementation [#598](https://github.com/Open-Cascade-SAS/OCCT/pull/598)
* Optimized NCollection_Array1 with type-specific improvements [#608](https://github.com/Open-Cascade-SAS/OCCT/pull/608)
* Reworked math_DoubleTab to use NCollection container [#607](https://github.com/Open-Cascade-SAS/OCCT/pull/607)
* Fixed WinAPI resource leaks [#625](https://github.com/Open-Cascade-SAS/OCCT/pull/625)
* Fixed include brackets type issues [#635](https://github.com/Open-Cascade-SAS/OCCT/pull/635)
* Geom package copy optimization [#645](https://github.com/Open-Cascade-SAS/OCCT/pull/645)

### Build System and Configuration

* Added comprehensive VCPKG support with multiple features [#618](https://github.com/Open-Cascade-SAS/OCCT/pull/618), [#637](https://github.com/Open-Cascade-SAS/OCCT/pull/637), [#638](https://github.com/Open-Cascade-SAS/OCCT/pull/638)
* Added VCPKG port opencascade with TclTk and GTest support [#580](https://github.com/Open-Cascade-SAS/OCCT/pull/580), [#616](https://github.com/Open-Cascade-SAS/OCCT/pull/616)
* Implemented flexible project root configuration [#641](https://github.com/Open-Cascade-SAS/OCCT/pull/641)
* Fixed build config file validation issues [#647](https://github.com/Open-Cascade-SAS/OCCT/pull/647)
* Disabled GLTF build without RapidJSON [#646](https://github.com/Open-Cascade-SAS/OCCT/pull/646)
* Fixed link errors on macOS when not building using vcpkg [#609](https://github.com/Open-Cascade-SAS/OCCT/pull/609)
* Fixed CSF variable overwriting issues [#561](https://github.com/Open-Cascade-SAS/OCCT/pull/561)
* Fixed paths to 3rd-party in cmake configuration [#523](https://github.com/Open-Cascade-SAS/OCCT/pull/523)
* Fixed ARCH detection for older 32-bit Macs [#626](https://github.com/Open-Cascade-SAS/OCCT/pull/626)
* Removed unused CMake scripts and dependencies [#644](https://github.com/Open-Cascade-SAS/OCCT/pull/644), [#581](https://github.com/Open-Cascade-SAS/OCCT/pull/581)
* Fixed samples CMake configuration [#643](https://github.com/Open-Cascade-SAS/OCCT/pull/643)

### Modeling

* Implemented new Helix Toolkit [#648](https://github.com/Open-Cascade-SAS/OCCT/pull/648)
* Added option to not build history in BRepFill_PipeShell [#632](https://github.com/Open-Cascade-SAS/OCCT/pull/632)
* Fixed GeomFill_CorrectedFrenet hanging in some cases [#630](https://github.com/Open-Cascade-SAS/OCCT/pull/630)
* Fixed infinite loop in Simplifying Fuse operation [#557](https://github.com/Open-Cascade-SAS/OCCT/pull/557)
* Fixed Bnd_BoundSortBox::Compare failures in some cases [#518](https://github.com/Open-Cascade-SAS/OCCT/pull/518)
* Optimized General Fuse (BOPAlgo_PaveFiller) [#514](https://github.com/Open-Cascade-SAS/OCCT/pull/514)
* Fixed BRepFilletAPI_MakeFillet segfault with two curves and rim [#532](https://github.com/Open-Cascade-SAS/OCCT/pull/532)
* Fixed mismatch between projected point and parameter in ShapeAnalysis_Curve [#600](https://github.com/Open-Cascade-SAS/OCCT/pull/600)

### Shape Healing

* Implemented reusing Surface Analysis for Wire fixing [#565](https://github.com/Open-Cascade-SAS/OCCT/pull/565)

### Visualization

* Enhanced FFmpeg Compatibility Layer and updated Video Recorder [#582](https://github.com/Open-Cascade-SAS/OCCT/pull/582)
* Fixed binary PPM image comment writing in Image_AlienPixMap [#413c08272b](https://github.com/Open-Cascade-SAS/OCCT/commit/413c08272b)
* Updated Graphic3d_Aspects::PolygonOffsets documentation [#519](https://github.com/Open-Cascade-SAS/OCCT/pull/519)
* Marked Immediate Mode rendering methods as deprecated in AIS_InteractiveContext [#521](https://github.com/Open-Cascade-SAS/OCCT/pull/521)

### Data Exchange

* Fixed GLTF indices parsing during array processing [#602](https://github.com/Open-Cascade-SAS/OCCT/pull/602)
* Implemented non-uniform scaling in GLTF Import [#503](https://github.com/Open-Cascade-SAS/OCCT/pull/503)
* Fixed GLTF saving edges when Merge Faces is enabled [#554](https://github.com/Open-Cascade-SAS/OCCT/pull/554)
* Changed GLTF export line type to LINE_STRIP [#535](https://github.com/Open-Cascade-SAS/OCCT/pull/535)
* Fixed missing GDT values in STP Import [#617](https://github.com/Open-Cascade-SAS/OCCT/pull/617)
* Preserved control directives in Step Export [#601](https://github.com/Open-Cascade-SAS/OCCT/pull/601)
* Ignored unit factors during tessellation export [#577](https://github.com/Open-Cascade-SAS/OCCT/pull/577)
* Applied scaling transformation in Step Export [#513](https://github.com/Open-Cascade-SAS/OCCT/pull/513)
* Fixed missing Model Curves in IGES Export transfer cache [#483](https://github.com/Open-Cascade-SAS/OCCT/pull/483)
* Fixed XCAFDoc_Editor::RescaleGeometry not rescaling translation of roots reference [#529](https://github.com/Open-Cascade-SAS/OCCT/pull/529)
* Fixed facets with empty normals handling in RWObj_Reader [#520](https://github.com/Open-Cascade-SAS/OCCT/pull/520)
* Added conversion utilities for STEP geometrical and visual enumerations [#545](https://github.com/Open-Cascade-SAS/OCCT/pull/545)
* Added missing headers [#530](https://github.com/Open-Cascade-SAS/OCCT/pull/530)
* Optimized StepData_StepReaderData [#543](https://github.com/Open-Cascade-SAS/OCCT/pull/543)
* Optimized entity graph evaluating [#562](https://github.com/Open-Cascade-SAS/OCCT/pull/562)
* Removed GLTF files from XDEDRAW [#649](https://github.com/Open-Cascade-SAS/OCCT/pull/649)
* Removed unused dependencies from TKXDEDRAW [#650](https://github.com/Open-Cascade-SAS/OCCT/pull/650)

### Testing

* Updated GitHub Actions to use latest versions [#640](https://github.com/Open-Cascade-SAS/OCCT/pull/640)
* Updated workflows to use Windows 2025 for builds and tests [#579](https://github.com/Open-Cascade-SAS/OCCT/pull/579)
* Fixed skipped daily IR on macOS [#628](https://github.com/Open-Cascade-SAS/OCCT/pull/628)
* Added performance summary posting to PR [#612](https://github.com/Open-Cascade-SAS/OCCT/pull/612)
* Fixed master validation workflow [#611](https://github.com/Open-Cascade-SAS/OCCT/pull/611)
* Added daily vcpkg package validation [#605](https://github.com/Open-Cascade-SAS/OCCT/pull/605)
* Updated samples C++ version [#606](https://github.com/Open-Cascade-SAS/OCCT/pull/606)
* Removed extra GitHub jobs [#594](https://github.com/Open-Cascade-SAS/OCCT/pull/594)
* Added Draw testing cleanup [#595](https://github.com/Open-Cascade-SAS/OCCT/pull/595)
* Added ASCII code validation [#593](https://github.com/Open-Cascade-SAS/OCCT/pull/593)
* Migrated PR actions to VCPKG-based [#587](https://github.com/Open-Cascade-SAS/OCCT/pull/587)
* Added compilation on Clang without PCH [#540](https://github.com/Open-Cascade-SAS/OCCT/pull/540)
* Enabled IR integration concurrency [#531](https://github.com/Open-Cascade-SAS/OCCT/pull/531), [#536](https://github.com/Open-Cascade-SAS/OCCT/pull/536)
* Fixed GCC bug workaround for NCollection_LocalArrayTest [#549](https://github.com/Open-Cascade-SAS/OCCT/pull/549)
* Added target branch comparison for repo validation [#524](https://github.com/Open-Cascade-SAS/OCCT/pull/524)

### Draw and Tools

* Fixed vcomputehlr misprint leading to error if no Viewer [#526](https://github.com/Open-Cascade-SAS/OCCT/pull/526)
* Updated DrawDefault script to handle missing directory cases [#542](https://github.com/Open-Cascade-SAS/OCCT/pull/542)

### Documentation

* Added missing description to HLRBRep_HLRToShape methods [#525](https://github.com/Open-Cascade-SAS/OCCT/pull/525)
* Added Copilot instructions for OCCT development [#589](https://github.com/Open-Cascade-SAS/OCCT/pull/589)

## How to Upgrade

There are no critical breaking changes at the API level, however note the following:

* **C++17 Requirement**: The minimum C++ version has been upgraded to C++17. Ensure your compiler supports this standard.
* **Deprecated Methods**: Some Immediate Mode rendering methods in AIS_InteractiveContext have been marked as deprecated.

Migration should proceed smoothly for most applications.

## Performance Improvements

This release includes several performance optimizations:
* Optimized gp_Vec, gp_Vec2d, gp_XY, and gp_XYZ classes
* General Fuse algorithm optimization
* Geom package copy optimization
* StepData_StepReaderData optimization
* NCollection_Array1 type-specific optimizations

## What's Changed

**Full list of changes between V8_0_0_rc1 and V8_0_0_rc2:**

* Coding - Bump version to 8.0.0-rc2
* Coding - Remove unused dependencies from TKXDEDRAW [#650](https://github.com/Open-Cascade-SAS/OCCT/pull/650)
* Build - Remove GLTF files from XDEDRAW [#649](https://github.com/Open-Cascade-SAS/OCCT/pull/649)
* Modeling - Implement new Helix Toolkit [#648](https://github.com/Open-Cascade-SAS/OCCT/pull/648)
* Configuration, CMake - Build config file is invalid [#647](https://github.com/Open-Cascade-SAS/OCCT/pull/647)
* Configuration, CMake - Disable build GLTF without RapidJSON [#646](https://github.com/Open-Cascade-SAS/OCCT/pull/646)
* Coding - Geom package Copy optimisation [#645](https://github.com/Open-Cascade-SAS/OCCT/pull/645)
* CMake - Remove unused CMake script [#644](https://github.com/Open-Cascade-SAS/OCCT/pull/644)
* Samples - CMake configuration fix [#643](https://github.com/Open-Cascade-SAS/OCCT/pull/643)
* Configuration, CMake - Flexible project root [#641](https://github.com/Open-Cascade-SAS/OCCT/pull/641)
* Testing - Update GitHub Actions to use latest versions [#640](https://github.com/Open-Cascade-SAS/OCCT/pull/640)
* Configuration - Add VCPKG port opencascade [#638](https://github.com/Open-Cascade-SAS/OCCT/pull/638)
* Configuration - VCPKG extend feature list [#637](https://github.com/Open-Cascade-SAS/OCCT/pull/637)
* Configuration - Add vcpkg layout support [#618](https://github.com/Open-Cascade-SAS/OCCT/pull/618)

... and 60+ additional improvements and bug fixes.

**Full Changelog**: https://github.com/Open-Cascade-SAS/OCCT/compare/V8_0_0_rc1...V8_0_0_rc2