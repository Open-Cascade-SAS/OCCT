# Open CASCADE Technology

Open CASCADE Technology (OCCT) is a software development platform providing services for 3D surface and solid modeling, CAD data exchange, and visualization. Most of OCCT functionality is available in the form of C++ libraries. OCCT is ideal for developing software dealing with 3D modeling (CAD), manufacturing/measuring (CAM), or numerical simulation (CAE).

## License

Open CASCADE Technology is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License version 2.1 as published by the Free Software Foundation, with a special exception defined in the file `OCCT_LGPL_EXCEPTION.txt`. Consult the file `LICENSE_LGPL_21.txt` included in the OCCT distribution for the complete text of the license.

Alternatively, Open CASCADE Technology may be used under the terms of the Open CASCADE commercial license or a contractual agreement.

**Note:** Open CASCADE Technology is provided on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND. The entire risk related to any use of the OCCT code and materials is on you. See the license text for a formal disclaimer.

## Packaging

You can receive certified versions of OCCT code in different packages:

- **Snapshot of Git repository:** Contains C++ header and source files of OCCT, documentation sources, build scripts, and CMake project files.
- **Complete source archive:** Contains all sources of OCCT, generated HTML and PDF documentation, and ready-to-use projects for building on all officially supported platforms.
- **Binary package (platform-specific):** In addition to the complete source archive, it includes binaries of OCCT and third-party libraries built on one platform. This package allows using OCCT immediately after installation.

Download OCCT release packages from [GitHub Releases](https://github.com/Open-Cascade-SAS/OCCT/releases).

You can also find OCCT pre-installed on your system or install it from packages provided by a third party. Note that packaging and functionality of such versions can be different from certified releases. Please consult the documentation accompanying your version for details.

## Documentation

Documentation is available at the following links:
- [Guides and overview](https://occt3d.com/dev/doc/overview/html/index.html)
- [C++ API reference](https://occt3d.com/dev/doc/refman/html/index.html)

If your package includes HTML documentation, open `doc/overview/html/index.html` for the guides or `doc/refman/html/index.html` for the C++ API reference.

If HTML documentation is not available in your package, you can:

- **Generate it from sources:** Install CMake and Doxygen 1.8.4 or later. Enable `BUILD_DOC_Overview` for guides and `BUILD_DOC_RefMan` for the API reference. Set `3RDPARTY_DOXYGEN_EXECUTABLE` if Doxygen is not found. Build `Overview` for guides, `RefMan` for the API reference, or `doc` for all enabled documentation targets. Tcl/Tk is required for a full build with DRAW enabled.
- **Read documentation in source plain text (Markdown) format** found in the subfolder `dox` or [GitHub Wiki](https://github.com/Open-Cascade-SAS/OCCT/wiki).

See [dox/build/build_documentation/building_documentation.md](dox/build/build_documentation/building_documentation.md) or [Building Documentation](https://occt3d.com/dev/doc/overview/html/build_upgrade__building_documentation.html) for details.

## Building

In most cases, you need to rebuild OCCT on your platform (OS, compiler) before using it in your project to ensure binary compatibility.

Consult the file [dox/build/build_occt/building_occt.md](dox/build/build_occt/building_occt.md) or [Building OCCT](https://occt3d.com/dev/doc/overview/html/build_upgrade__building_occt.html) or [Building OCCT Wiki](https://github.com/Open-Cascade-SAS/OCCT/wiki/build_upgrade) for instructions on building OCCT from sources on supported platforms.

## Version

The current version of OCCT can be found in the file [`adm/cmake/version.cmake`](adm/cmake/version.cmake).

## Development

### Bug Tracker
- [GitHub Issues](https://github.com/Open-Cascade-SAS/OCCT/issues)
- [Historical issue archive (read-only)](https://occt3d.com/dev/tickets/)

For the contribution workflow and CLA request instructions, see [Contributing to OCCT](.github/CONTRIBUTING.md) or the [online contribution guide](https://occt3d.com/dev/doc/overview/html/contribution.html).

### Forum and Discussions
- [Historical forum archive (read-only)](https://occt3d.com/dev/)
- [GitHub Discussions](https://github.com/Open-Cascade-SAS/OCCT/discussions)