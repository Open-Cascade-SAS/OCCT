Open CASCADE Technology
=======================

This directory contains sources of Open CASCADE Technology (OCCT), a software
development platform providing services for 3D surface and solid modeling, CAD 
data exchange, and visualization. Most of OCCT functionality is available in 
the form of C++ libraries. OCCT can be best applied in development of software 
dealing with 3D modeling (CAD), manufacturing / measuring (CAM) or numerical 
simulation (CAE).

License
-------

Open CASCADE Technology is free software; you can redistribute it and / or 
modify it under the terms of the GNU Lesser General Public version 2.1 as 
published by the Free Software Foundation, with special exception defined in 
the file OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included
in OCCT distribution for complete text of the license.

Alternatively, Open CASCADE Technology may be used under the terms of Open 
CASCADE commercial license or contractual agreement.

Note that Open CASCADE Technology is provided on an "AS IS" basis, WITHOUT 
WARRANTY OF ANY KIND. The entire risk related to any use of the OCCT code and 
materials is on you. See the license text for formal disclaimer.

Packaging
---------

You can receive certified version of OCCT code in different packages.

- Snapshot of Git repository: contains C++ header and source files of OCCT,
  documentation sources, build scripts, and CMake project files.

- Complete source archive: contains all sources of OCCT, generated HTML and PDF
  documentation, and ready-to-use projects for building on all officially 
  supported platforms.

- Binary package (platform-specific): in addition to complete source archive, 
  it includes binaries of OCCT and third-party libraries built on one platform. 
  This package allows using OCCT immediately after installation.

Certified versions of OCCT can be downloaded from http://www.opencascade.com

You can also find OCCT pre-installed on your system, or install it from 
packages provided by a third party. Note that packaging and functionality
of such versions can be different from certified releases. Please consult 
documentation accompanying your version for details.

Documentation
-------------

Open file doc/html/index.html to browse HTML documentation.

If HTML documentation is not available in your package, you can:

- Generate it from sources. 

  You need to have Tcl and Doxygen 1.8.4 (or above) installed on your system.
  and accessible in your environment (check environment variable PATH).
  Use batch file *gendoc.bat* on Windows or Bash script *gendoc* on Linux
  or OS X to (re)generate documentation.

- Read documentation in source plain text (MarkDown) format found in 
  subfolder *dox*

See *dox/dev_guides/documentation/documentation.md* for details.

Building
--------

In most cases you need to rebuild OCCT on your platform (OS, compiler) before
using it in your project, to ensure binary compatibility.

Consult the file *dox/dev_guides/building/building.md* for instructions on
building OCCT from sources on supported platforms.

Version
-------

The current version of OCCT can be consulted in the file
*src/Standard/Standard_Version.hxx*

Development
-----------

For information regarding OCCT code development please consult the official 
OCCT Collaborative Development Portal:
http://dev.opencascade.org
