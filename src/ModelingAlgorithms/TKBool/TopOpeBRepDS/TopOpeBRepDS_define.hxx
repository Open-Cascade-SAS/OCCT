// Created on: 1997-09-24
// Created by: Jean Yves LEBEY
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

#ifndef _TopOpeBRepDS_define_HeaderFile
#define _TopOpeBRepDS_define_HeaderFile

#include <TopAbs_ShapeEnum.hxx>

#include <TopAbs_Orientation.hxx>

#include <TopAbs_State.hxx>

#include <TopoDS_Shape.hxx>

#include <TopTools_ShapeMapHasher.hxx>

#include <NCollection_Map.hxx>

#include <NCollection_List.hxx>

#include <NCollection_IndexedMap.hxx>

#include <NCollection_DataMap.hxx>

#include <Standard_Integer.hxx>

#include <NCollection_IndexedDataMap.hxx>

#include <TopoDS_Face.hxx>

#include <TopoDS_Edge.hxx>

#include <TopoDS_Vertex.hxx>

#include <TCollection_AsciiString.hxx>

#include <TopOpeBRepDS_Interference.hxx>

#include <NCollection_List.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <NCollection_List.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_ShapeShapeInterference.hxx>
#include <TopOpeBRepDS_CurvePointInterference.hxx>
#include <TopOpeBRepDS_EdgeVertexInterference.hxx>
#include <TopOpeBRepDS_FaceEdgeInterference.hxx>
#include <TopOpeBRepDS_InterferenceIterator.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_PDataStructure.hxx>
#include <TopOpeBRepDS_DataStructure.hxx>
#include <TopOpeBRepDS_Kind.hxx>
#include <TopOpeBRepDS_Config.hxx>
#include <TopOpeBRepDS_Transition.hxx>

#define MDSlioloi NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator
#define MDSloi NCollection_List<occ::handle<TopOpeBRepDS_Interference>>
#define MDShi occ::handle<TopOpeBRepDS_Interference>
#define MDShssi occ::handle<TopOpeBRepDS_ShapeShapeInterference>
#define MDShcpi occ::handle<TopOpeBRepDS_CurvePointInterference>
#define MDScpi TopOpeBRepDS_CurvePointInterference
#define MDShevi occ::handle<TopOpeBRepDS_EdgeVertexInterference>
#define MDSevi TopOpeBRepDS_EdgeVertexInterference
#define MDShfei occ::handle<TopOpeBRepDS_FaceEdgeInterference>
#define MDShsci occ::handle<TopOpeBRepDS_SurfaceCurveInterference>
#define MDScux TopOpeBRepDS_CurveExplorer
#define MDScud TopOpeBRepDS_CurveData
#define MDScu TopOpeBRepDS_Curve
#define MDSpox TopOpeBRepDS_PointExplorer
#define MDSpod TopOpeBRepDS_PointData
#define MDSpo TopOpeBRepDS_Point
#define MDSii TopOpeBRepDS_InterferenceIterator
#define MDShds occ::handle<TopOpeBRepDS_HDataStructure>
#define MDSds TopOpeBRepDS_DataStructure
#define MDSpds TopOpeBRepDS_PDataStructure
#define MDSk TopOpeBRepDS_Kind
#define MDSc TopOpeBRepDS_Config
#define MDSt TopOpeBRepDS_Transition

#define MDSsd TopOpeBRepDS_ShapeData
#define MDSmosd NCollection_IndexedDataMap<TopoDS_Shape, TopOpeBRepDS_ShapeData, TopTools_ShapeMapHasher>
#define MDSitl TopOpeBRepDS_InterferenceTool

#endif
