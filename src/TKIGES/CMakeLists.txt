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

include(${OCCT_SOURCE_DIR}/adm/cmake/occt_toolkit.cmake)
