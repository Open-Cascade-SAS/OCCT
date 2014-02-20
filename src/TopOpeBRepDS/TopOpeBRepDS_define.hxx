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

#include <TopOpeBRepTool_define.hxx>

#include <TopOpeBRepDS_ListIteratorOfListOfInterference.hxx>
#include <TopOpeBRepDS_ListOfInterference.hxx>
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

#define MDSlioloi TopOpeBRepDS_ListIteratorOfListOfInterference
#define MDSloi TopOpeBRepDS_ListOfInterference
#define MDShi Handle(TopOpeBRepDS_Interference)
#define MDShssi Handle(TopOpeBRepDS_ShapeShapeInterference)
#define MDShcpi Handle(TopOpeBRepDS_CurvePointInterference)
#define MDScpi TopOpeBRepDS_CurvePointInterference
#define MDShevi Handle(TopOpeBRepDS_EdgeVertexInterference)
#define MDSevi TopOpeBRepDS_EdgeVertexInterference
#define MDShfei Handle(TopOpeBRepDS_FaceEdgeInterference)
#define MDShsci Handle(TopOpeBRepDS_SurfaceCurveInterference)
#define MDScux TopOpeBRepDS_CurveExplorer
#define MDScud TopOpeBRepDS_CurveData
#define MDScu TopOpeBRepDS_Curve
#define MDSpox TopOpeBRepDS_PointExplorer
#define MDSpod TopOpeBRepDS_PointData
#define MDSpo TopOpeBRepDS_Point
#define MDSii TopOpeBRepDS_InterferenceIterator
#define MDShds Handle(TopOpeBRepDS_HDataStructure)
#define MDSds TopOpeBRepDS_DataStructure
#define MDSpds TopOpeBRepDS_PDataStructure
#define MDSk TopOpeBRepDS_Kind
#define MDSc TopOpeBRepDS_Config
#define MDSt TopOpeBRepDS_Transition

#define MDSsd TopOpeBRepDS_ShapeData
#define MDSmosd TopOpeBRepDS_MapOfShapeData
#define MDSitl TopOpeBRepDS_InterferenceTool

#endif
