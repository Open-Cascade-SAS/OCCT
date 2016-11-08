// Created on: 2004-09-03
// Created by: Oleg FEDYAEV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

#ifndef _BOPAlgo_ArgumentAnalyzer_HeaderFile
#define _BOPAlgo_ArgumentAnalyzer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>
#include <Standard_Boolean.hxx>
#include <BOPAlgo_Operation.hxx>
#include <BOPAlgo_ListOfCheckResult.hxx>
#include <Standard_Real.hxx>
#include <BOPCol_DataMapOfShapeReal.hxx>
#include <BOPAlgo_Algo.hxx>
#include <TopAbs_ShapeEnum.hxx>
class TopoDS_Shape;


//! check the validity of argument(s) for Boolean Operations
class BOPAlgo_ArgumentAnalyzer  : public BOPAlgo_Algo
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! empty constructor
  Standard_EXPORT BOPAlgo_ArgumentAnalyzer();
Standard_EXPORT virtual ~BOPAlgo_ArgumentAnalyzer();
  
  //! sets object shape
  Standard_EXPORT void SetShape1 (const TopoDS_Shape& TheShape);
  
  //! sets tool shape
  Standard_EXPORT void SetShape2 (const TopoDS_Shape& TheShape);
  
  //! returns object shape;
  Standard_EXPORT const TopoDS_Shape& GetShape1() const;
  
  //! returns tool shape
  Standard_EXPORT const TopoDS_Shape& GetShape2() const;
  
  //! returns ref
  Standard_EXPORT BOPAlgo_Operation& OperationType();
  
  //! returns ref
  Standard_EXPORT Standard_Boolean& StopOnFirstFaulty();
  
  //! Returns (modifiable) mode
  //! that means checking types of shapes.
    Standard_Boolean& ArgumentTypeMode();
  
  //! Returns (modifiable) mode that means
  //! checking of self-intersection of shapes.
    Standard_Boolean& SelfInterMode();
  
  //! Returns (modifiable) mode that means
  //! checking of small edges.
    Standard_Boolean& SmallEdgeMode();
  
  //! Returns (modifiable) mode that means
  //! checking of possibility to split or rebuild faces.
    Standard_Boolean& RebuildFaceMode();
  
  //! Returns (modifiable) mode that means
  //! checking of tangency between subshapes.
    Standard_Boolean& TangentMode();
  
  //! Returns (modifiable) mode that means
  //! checking of problem of merging vertices.
    Standard_Boolean& MergeVertexMode();
  
  //! Returns (modifiable) mode that means
  //! checking of problem of merging edges.
    Standard_Boolean& MergeEdgeMode();
  
  //! Returns (modifiable) mode that means
  //! checking of problem of continuity of the shape.
    Standard_Boolean& ContinuityMode();
  
  //! Returns (modifiable) mode that means
  //! checking of problem of invalid curve on surface.
    Standard_Boolean& CurveOnSurfaceMode();
  
  //! performs analysis
  Standard_EXPORT void Perform();
  
  //! result of test
  Standard_EXPORT Standard_Boolean HasFaulty() const;
  
  //! returns a result of test
  Standard_EXPORT const BOPAlgo_ListOfCheckResult& GetCheckResult() const;
  

protected:

  
  //! Prepares data;
  Standard_EXPORT void Prepare();
  
  Standard_EXPORT void TestTypes();
  
  Standard_EXPORT void TestSelfInterferences();
  
  Standard_EXPORT void TestSmallEdge();
  
  Standard_EXPORT void TestRebuildFace();
  
  Standard_EXPORT void TestTangent();
  
  Standard_EXPORT void TestMergeSubShapes (const TopAbs_ShapeEnum theType);
  
  Standard_EXPORT void TestMergeVertex();
  
  Standard_EXPORT void TestMergeEdge();
  
  Standard_EXPORT void TestContinuity();
  
  Standard_EXPORT void TestCurveOnSurface();
  

private:


  TopoDS_Shape myShape1;
  TopoDS_Shape myShape2;
  Standard_Boolean myStopOnFirst;
  BOPAlgo_Operation myOperation;
  Standard_Boolean myArgumentTypeMode;
  Standard_Boolean mySelfInterMode;
  Standard_Boolean mySmallEdgeMode;
  Standard_Boolean myRebuildFaceMode;
  Standard_Boolean myTangentMode;
  Standard_Boolean myMergeVertexMode;
  Standard_Boolean myMergeEdgeMode;
  Standard_Boolean myContinuityMode;
  Standard_Boolean myCurveOnSurfaceMode;
  Standard_Boolean myEmpty1;
  Standard_Boolean myEmpty2;
  BOPAlgo_ListOfCheckResult myResult;

};


#include <BOPAlgo_ArgumentAnalyzer.lxx>


#endif // _BOPAlgo_ArgumentAnalyzer_HeaderFile
