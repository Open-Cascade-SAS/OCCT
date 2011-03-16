// File:	TopOpeBRepDS_define.hxx
// Created:	Wed Sep 24 15:50:19 1997
// Author:	Jean Yves LEBEY
//		<jyl@bistrox.paris1.matra-dtv.fr>

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
