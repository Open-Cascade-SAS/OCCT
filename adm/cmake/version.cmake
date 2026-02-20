#======================================================================
#
# Purpose:   Defines macros identifying current version of Open CASCADE
#
#  OCC_VERSION_MAJOR       : (integer) number identifying major version
#  OCC_VERSION_MINOR       : (integer) number identifying minor version
#  OCC_VERSION_MAINTENANCE : (integer) number identifying maintenance version
#  OCC_VERSION_DEVELOPMENT : (string)  if defined, indicates development or modified version
#                              in case of release, remove the value
#
#  Sample values of OCC_VERSION_DEVELOPMENT:
#  - "dev" for development version between releases
#  - "beta..." or "rc..." for beta releases or release candidates
#  - "project..." for version containing project-specific fixes
#
#  For development version git commit hash can be added to the version string
#======================================================================

set (OCC_VERSION_MAJOR 8 )
set (OCC_VERSION_MINOR 0 )
set (OCC_VERSION_MAINTENANCE 0 )
set (OCC_VERSION_DEVELOPMENT "rc4" )
