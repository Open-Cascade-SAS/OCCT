#======================================================================
#
# Purpose:   Defines macros identifying current version of Open CASCADE
#
#  OCC_VERSION_MAJOR       : (integer) number identifying major version
#  OCC_VERSION_MINOR       : (integer) number identifying minor version
#  OCC_VERSION_MAINTENANCE : (integer) number identifying maintenance version
#  OCC_VERSION_DEVELOPMENT : (string)  if defined, indicates development or modified version
#                              in case of release, comment the line with setting this variable
#
#  Sample values of OCC_VERSION_DEVELOPMENT:
#  - "dev" for development version between releases
#  - "beta..." or "rc..." for beta releases or release candidates
#  - "project..." for version containing project-specific fixes
#
#  CMake variable USE_GIT_REVISION (bool) will be used to extend the result
#    version string with Git revision number in case of development version is not empty.
#======================================================================

set (OCC_VERSION_MAJOR 7)
set (OCC_VERSION_MINOR 8)
set (OCC_VERSION_MAINTENANCE 2)
set (OCC_VERSION_DEVELOPMENT "dev")
