// Created on: 1999-11-02
// Created by: Peter KURNEV
// Copyright (c) 1999 Matra Datavision
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

#ifndef _TopOpeBRepBuild_Tools_HeaderFile
#define _TopOpeBRepBuild_Tools_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Standard_Integer.hxx>
#include <TopOpeBRepDS_IndexedDataMapOfShapeWithState.hxx>
#include <TopAbs_State.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopOpeBRepDS_DataMapOfShapeState.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_IndexedMapOfOrientedShape.hxx>
#include <TopTools_IndexedDataMapOfShapeShape.hxx>
class TopoDS_Shape;
class TopOpeBRepTool_ShapeClassifier;
class TopoDS_Face;
class TopoDS_Edge;
class gp_Vec;
class TopoDS_Wire;


//! Auxiliary  methods  used  in  TopOpeBRepBuild_Builder1  class
class TopOpeBRepBuild_Tools 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static void FindState (const TopoDS_Shape& aVertex, const TopAbs_State aState, const TopAbs_ShapeEnum aShapeEnum, const TopTools_IndexedDataMapOfShapeListOfShape& aMapVertexEdges, TopTools_MapOfShape& aMapProcessedVertices, TopOpeBRepDS_DataMapOfShapeState& aMapVs);
  
  Standard_EXPORT static void PropagateState (const TopOpeBRepDS_DataMapOfShapeState& aSplEdgesState, const TopTools_IndexedMapOfShape& anEdgesToRestMap, const TopAbs_ShapeEnum aShapeEnum1, const TopAbs_ShapeEnum aShapeEnum2, TopOpeBRepTool_ShapeClassifier& aShapeClassifier, TopOpeBRepDS_IndexedDataMapOfShapeWithState& aMapOfShapeWithState, const TopTools_MapOfShape& anUnkStateShapes);
  
  Standard_EXPORT static TopAbs_State FindStateThroughVertex (const TopoDS_Shape& aShape, TopOpeBRepTool_ShapeClassifier& aShapeClassifier, TopOpeBRepDS_IndexedDataMapOfShapeWithState& aMapOfShapeWithState, const TopTools_MapOfShape& anAvoidSubshMap);
  
  Standard_EXPORT static void PropagateStateForWires (const TopTools_IndexedMapOfShape& aFacesToRestMap, TopOpeBRepDS_IndexedDataMapOfShapeWithState& aMapOfShapeWithState);
  
  Standard_EXPORT static void SpreadStateToChild (const TopoDS_Shape& aShape, const TopAbs_State aState, TopOpeBRepDS_IndexedDataMapOfShapeWithState& aMapOfShapeWithState);
  
  Standard_EXPORT static void FindState1 (const TopoDS_Shape& anEdge, const TopAbs_State aState, const TopTools_IndexedDataMapOfShapeListOfShape& aMapEdgesFaces, TopTools_MapOfShape& aMapProcessedVertices, TopOpeBRepDS_DataMapOfShapeState& aMapVs);
  
  Standard_EXPORT static void FindState2 (const TopoDS_Shape& anEdge, const TopAbs_State aState, const TopTools_IndexedDataMapOfShapeListOfShape& aMapEdgesFaces, TopTools_MapOfShape& aMapProcessedEdges, TopOpeBRepDS_DataMapOfShapeState& aMapVs);
  
  Standard_EXPORT static Standard_Boolean GetAdjacentFace (const TopoDS_Shape& aFaceObj, const TopoDS_Shape& anEObj, const TopTools_IndexedDataMapOfShapeListOfShape& anEdgeFaceMap, TopoDS_Shape& anAdjFaceObj);
  
  Standard_EXPORT static void GetNormalToFaceOnEdge (const TopoDS_Face& aFObj, const TopoDS_Edge& anEdgeObj, gp_Vec& aDirNormal);
  
  //! This  function  used  to  compute  normal  in  point  which  is  located
  //! near  the  point  with  param  UV    (used  for  computation  of  normals  where  the  normal  in  the  point UV  equal  to  zero).
  Standard_EXPORT static void GetNormalInNearestPoint (const TopoDS_Face& aFace, const TopoDS_Edge& anEdge, gp_Vec& aNormal);
  
  Standard_EXPORT static Standard_Boolean GetTangentToEdgeEdge (const TopoDS_Face& aFObj, const TopoDS_Edge& anEdgeObj, const TopoDS_Edge& aOriEObj, gp_Vec& aTangent);
  
  Standard_EXPORT static Standard_Boolean GetTangentToEdge (const TopoDS_Edge& anEdgeObj, gp_Vec& aTangent);
  
  //! Recompute  PCurves  of  the  all  edges  from  the  wire  on  the  <toFace>
  Standard_EXPORT static void UpdatePCurves (const TopoDS_Wire& aWire, const TopoDS_Face& fromFace, const TopoDS_Face& toFace);
  
  //! recompute  PCurves  of  the  closing  (SIM  ,  with 2  PCurves)  edge on  the  NewFace
  Standard_EXPORT static void UpdateEdgeOnPeriodicalFace (const TopoDS_Edge& aEdgeToUpdate, const TopoDS_Face& OldFace, const TopoDS_Face& NewFace);
  
  //! recompute  PCurve  of  the    edge on  the  NewFace
  Standard_EXPORT static void UpdateEdgeOnFace (const TopoDS_Edge& aEdgeToUpdate, const TopoDS_Face& OldFace, const TopoDS_Face& NewFace);
  
  Standard_EXPORT static Standard_Boolean IsDegEdgesTheSame (const TopoDS_Shape& anE1, const TopoDS_Shape& anE2);
  
  //! test  if  <oldFace>  does  not  contain  INTERNAL  or  EXTERNAL  edges
  //! and  remove  such  edges  in  case  of  its  presence.  The  result  is  stored  in  <corrFace>
  Standard_EXPORT static void NormalizeFace (const TopoDS_Shape& oldFace, TopoDS_Shape& corrFace);
  
  //! test  if  UV  representation  of  <oldFace>  is  good  (i.e.  face  is  closed  in  2d).
  //! if  face  is  not  closed  ,  this  method  will  try  to  close  such  face  and  will
  //! return  corrected  edges  in  the  <aMapOfCorrect2dEdges>.  Parameter  <aSourceShapes>
  //! used  to  fix  the  edge  (or  wires)  which  should  be  correct  (Corrector  used  it  as  a  start  shapes).
  //! NOTE  :  Parameter  corrFace  doesn't  mean  anything.  If  you  want  to use  this  method  ,  rebuild  resulting  face
  //! after  by  yourself  using  corrected  edges.
  Standard_EXPORT static void CorrectFace2d (const TopoDS_Shape& oldFace, TopoDS_Shape& corrFace, const TopTools_IndexedMapOfOrientedShape& aSourceShapes, TopTools_IndexedDataMapOfShapeShape& aMapOfCorrect2dEdges);
  
  Standard_EXPORT static void CorrectTolerances (const TopoDS_Shape& aS, const Standard_Real aTolMax = 0.0001);
  
  Standard_EXPORT static void CorrectCurveOnSurface (const TopoDS_Shape& aS, const Standard_Real aTolMax = 0.0001);
  
  Standard_EXPORT static void CorrectPointOnCurve (const TopoDS_Shape& aS, const Standard_Real aTolMax = 0.0001);
  
  //! Checks if <theFace> has the properly closed in 2D boundary(ies)
  Standard_EXPORT static Standard_Boolean CheckFaceClosed2d (const TopoDS_Face& theFace);




protected:





private:





};







#endif // _TopOpeBRepBuild_Tools_HeaderFile
