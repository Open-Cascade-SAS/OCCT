// Created on: 1997-10-08
// Created by: Prestataire Xuan PHAM PHU
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

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
