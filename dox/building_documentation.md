﻿# Build Documentation <a id="build_upgrade__building_documentation" class="anchor"></a>

To generate HTML documentation from sources contained in *dox* subdirectory, 
you need to have Tcl and Doxygen 1.8.5 (or above) installed on your system.

Use script **gendoc** (batch file on Windows, shell script on Linux / Mac OSX) located in `adm` directory to generate documentation.

To generate Overview documentation:

    cmd> gendoc -overview

To generate Reference manual:

    cmd> gendoc -refman

Run this command without arguments to get help on supported options.

See [Link](contribution#occt_contribution__documentation) for prerequisites and details on OCCT documentation system.
