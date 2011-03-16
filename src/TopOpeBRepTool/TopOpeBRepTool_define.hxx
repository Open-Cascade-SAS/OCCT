// File:	TopOpeBRepTool_define.hxx
// Created:	Wed Oct  8 11:39:45 1997
// Author:	Prestataire Xuan PHAM PHU
//		<xpu@poulopox.paris1.matra-dtv.fr>

#ifndef _TopOpeBRepTool_define_HeaderFile
#define _TopOpeBRepTool_define_HeaderFile

#include <TopAbs_ShapeEnum.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs_State.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_IndexedMapOfOrientedShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_DataMapOfShapeInteger.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeInteger.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfOrientedShapeInteger.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapOfOrientedShapeInteger.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TCollection_AsciiString.hxx>

#define MTAse TopAbs_ShapeEnum
#define MTAo TopAbs_Orientation
#define MTAs TopAbs_State
#define MTTmiomos TopTools_MapIteratorOfMapOfShape
#define MTTmos TopTools_MapOfShape
#define MTTliolos TopTools_ListIteratorOfListOfShape
#define MTTlos TopTools_ListOfShape
#define MTTimos TopTools_IndexedMapOfShape
#define MTTimoos TopTools_IndexedMapOfOrientedShape
#define MTTdmoss TopTools_DataMapOfShapeShape
#define MTTdmosi TopTools_DataMapOfShapeInteger
#define MTTdmoosi TopTools_DataMapOfOrientedShapeInteger
#define MTTdmiodmoss TopTools_DataMapIteratorOfDataMapOfShapeShape
#define MTTdmiodmosi TopTools_DataMapIteratorOfDataMapOfShapeInteger
#define MTTdmoslos TopTools_DataMapOfShapeListOfShape
#define MTTdmiodmoslos TopTools_DataMapIteratorOfDataMapOfShapeListOfShape
#define MTTidmoslos TopTools_IndexedDataMapOfShapeListOfShape
#define MTTdmiodmoosi TopTools_DataMapIteratorOfDataMapOfOrientedShapeInteger
#define MTs TopoDS_Shape
#define MTf TopoDS_Face
#define MTe TopoDS_Edge
#define MTv TopoDS_Vertex
#define Msr Standard_Real
#define Msi Standard_Integer
#define Msb Standard_Boolean
#define Msf Standard_False
#define Mst Standard_True
#define Mso Standard_OStream
#define Mtcas TCollection_AsciiString

#define MTLsc  TopOpeBRepTool_ShapeClassifier
#define MTLpsc TopOpeBRepTool_PShapeClassifier
#define MTLoct TopOpeBRepTool_OutCurveType
#define MTLc    TopOpeBRepTool_connexity

#endif
