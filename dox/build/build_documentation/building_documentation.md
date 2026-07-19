Build Documentation {#build_upgrade__building_documentation}
=================

To generate HTML documentation from sources contained in *dox* subdirectory, 
you need to have Doxygen 1.8.4 (or above) installed on your system.
Tcl/Tk is required when configuring a full OCCT build with DRAW enabled.

OCCT documentation is generated via CMake targets. Enable `-DBUILD_DOC_Overview=ON` 
when configuring the build, then build the desired target:

To generate Overview documentation:

    cmake --build . --target Overview

To generate Reference manual:

    cmake --build . --target RefMan

To generate all documentation:

    cmake --build . --target doc

See @ref occt_contribution__documentation for prerequisites and details on OCCT documentation system.
