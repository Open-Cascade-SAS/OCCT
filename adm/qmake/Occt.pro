# This is an experimental Solution project for building entire OCCT framework using qmake.
# It can be also used for just source code navigation in Qt Creator without actually building OCCT.
# Note, as this is an experimental project, compiler flags might differ from official builds - use it on your own risk!
# Building OCCT using CMake is a preferred solution.
#
# This project GENERATES sub-projects (OCCT modules) dynamically, so to start using it:
# - Launch genconf, select Project Format "Qt Project" and configure dependencies; this will also generate "custom.auto.pri".
# - Launch genproj to fill in "inc" folder with links to header files.
# - Open project in Qt Creator, and call "Run qmake".
# - Close the project in Qt Creator, open it again so that you should see the sub-modules in Project tree; call "Run qmake" again.
# - "Run qmake" and perform Build.
TEMPLATE = subdirs
exists(custom.auto.pri) { include(custom.auto.pri) }
exists(custom.pri)      { include(custom.pri) }

OTHER_FILES += OccModule.pro.in \
               OcctHeaderLink.hxx.in \
               OccToolkit.pro.in \
               OccCppConfig.pri \
               OccModule.pri \
               OccToolkit.pri \
               custom.pri.template

# Iterate over Modules and generate sub-projects
aSolModuleList = $$cat(../MODULES, lines)
for (aSolModuleIter, aSolModuleList) {
  aSolModule = $$first($$list($$aSolModuleIter))
  eval(occtkgen_$${aSolModule}.input  = $$_PRO_FILE_PWD_/OccModule.pro.in)
  eval(occtkgen_$${aSolModule}.output = $$_PRO_FILE_PWD_/$${aSolModule}/$${aSolModule}.pro)
  eval(occtkgen_$${aSolModule}.config = verbatim)
  eval(QMAKE_SUBSTITUTES += occtkgen_$${aSolModule})
  SUBDIRS += $${aSolModule}
}

# These dependencies are manually defined
ModelingData.depends = FoundationClasses
ModelingAlgorithms.depends = FoundationClasses ModelingData
Visualization.depends = FoundationClasses ModelingData ModelingAlgorithms
ApplicationFramework.depends = FoundationClasses ModelingData ModelingAlgorithms Visualization
DataExchange.depends = FoundationClasses ModelingData ModelingAlgorithms ApplicationFramework
Draw.depends = FoundationClasses ModelingData ModelingAlgorithms ApplicationFramework DataExchange Visualization
