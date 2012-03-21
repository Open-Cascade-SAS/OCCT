// Created on: 1997-09-24
// Created by: Jean Yves LEBEY
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
