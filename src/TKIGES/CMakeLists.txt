project(TKIGES)

set (TOOLKIT_MODULES
  IGESData
  IGESFile
  IGESBasic
  IGESGraph
  IGESGeom
  IGESDimen
  IGESDraw
  IGESSolid
  IGESDefs
  IGESAppli
  IGESConvGeom
  IGESSelect
  IGESToBRep
  GeomToIGES
  Geom2dToIGES
  BRepToIGES
  BRepToIGESBRep
  IGESControl
)

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)
