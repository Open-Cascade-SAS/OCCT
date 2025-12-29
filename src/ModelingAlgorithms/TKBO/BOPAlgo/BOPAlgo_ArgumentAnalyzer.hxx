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
#include <BOPAlgo_Algo.hxx>
#include <BOPAlgo_Operation.hxx>
#include <BOPAlgo_CheckResult.hxx>
#include <TopAbs_ShapeEnum.hxx>

//! check the validity of argument(s) for Boolean Operations
class BOPAlgo_ArgumentAnalyzer : public BOPAlgo_Algo
{
public:
  DEFINE_STANDARD_ALLOC

  //! empty constructor
  Standard_EXPORT BOPAlgo_ArgumentAnalyzer();
  Standard_EXPORT ~BOPAlgo_ArgumentAnalyzer() override;

  //! sets object shape
  Standard_EXPORT void SetShape1(const TopoDS_Shape& TheShape);

  //! sets tool shape
  Standard_EXPORT void SetShape2(const TopoDS_Shape& TheShape);

  //! returns object shape;
  Standard_EXPORT const TopoDS_Shape& GetShape1() const;

  //! returns tool shape
  Standard_EXPORT const TopoDS_Shape& GetShape2() const;

  //! returns ref
  Standard_EXPORT BOPAlgo_Operation& OperationType();

  //! returns ref
  Standard_EXPORT bool& StopOnFirstFaulty();

  //! Returns (modifiable) mode
  //! that means checking types of shapes.
  bool& ArgumentTypeMode();

  //! Returns (modifiable) mode that means
  //! checking of self-intersection of shapes.
  bool& SelfInterMode();

  //! Returns (modifiable) mode that means
  //! checking of small edges.
  bool& SmallEdgeMode();

  //! Returns (modifiable) mode that means
  //! checking of possibility to split or rebuild faces.
  bool& RebuildFaceMode();

  //! Returns (modifiable) mode that means
  //! checking of tangency between subshapes.
  bool& TangentMode();

  //! Returns (modifiable) mode that means
  //! checking of problem of merging vertices.
  bool& MergeVertexMode();

  //! Returns (modifiable) mode that means
  //! checking of problem of merging edges.
  bool& MergeEdgeMode();

  //! Returns (modifiable) mode that means
  //! checking of problem of continuity of the shape.
  bool& ContinuityMode();

  //! Returns (modifiable) mode that means
  //! checking of problem of invalid curve on surface.
  bool& CurveOnSurfaceMode();

  //! performs analysis
  Standard_EXPORT void Perform(
    const Message_ProgressRange& theRange = Message_ProgressRange()) override;

  //! result of test
  Standard_EXPORT bool HasFaulty() const;

  //! returns a result of test
  Standard_EXPORT const NCollection_List<BOPAlgo_CheckResult>& GetCheckResult() const;

protected:
  //! Prepares data;
  Standard_EXPORT void Prepare();

  Standard_EXPORT void TestTypes();

  Standard_EXPORT void TestSelfInterferences(const Message_ProgressRange& theRange);

  Standard_EXPORT void TestSmallEdge();

  Standard_EXPORT void TestRebuildFace();

  Standard_EXPORT void TestTangent();

  Standard_EXPORT void TestMergeSubShapes(const TopAbs_ShapeEnum theType);

  Standard_EXPORT void TestMergeVertex();

  Standard_EXPORT void TestMergeEdge();

  Standard_EXPORT void TestContinuity();

  Standard_EXPORT void TestCurveOnSurface();

private:
  TopoDS_Shape                          myShape1;
  TopoDS_Shape                          myShape2;
  bool                                  myStopOnFirst;
  BOPAlgo_Operation                     myOperation;
  bool                                  myArgumentTypeMode;
  bool                                  mySelfInterMode;
  bool                                  mySmallEdgeMode;
  bool                                  myRebuildFaceMode;
  bool                                  myTangentMode;
  bool                                  myMergeVertexMode;
  bool                                  myMergeEdgeMode;
  bool                                  myContinuityMode;
  bool                                  myCurveOnSurfaceMode;
  bool                                  myEmpty1;
  bool                                  myEmpty2;
  NCollection_List<BOPAlgo_CheckResult> myResult;
};

#include <BOPAlgo_ArgumentAnalyzer.lxx>

#endif // _BOPAlgo_ArgumentAnalyzer_HeaderFile
