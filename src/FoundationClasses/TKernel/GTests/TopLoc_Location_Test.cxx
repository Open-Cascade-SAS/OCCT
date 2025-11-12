// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <TopLoc_Location.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRep_Tool.hxx>
#include <gp.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_Pnt.hxx>
#include <OSD_Parallel.hxx>

#include <gtest/gtest.h>

#include <atomic>
#include <vector>

namespace
{
//! Functor for testing concurrent access to TopLoc_Location::Transformation()
struct TopLocTransformFunctor
{
  TopLocTransformFunctor(const std::vector<TopoDS_Shape>& theShapeVec)
      : myShapeVec(&theShapeVec),
        myIsRaceDetected(0)
  {
  }

  void operator()(size_t i) const
  {
    if (!myIsRaceDetected)
    {
      const TopoDS_Vertex& aVertex = TopoDS::Vertex(myShapeVec->at(i));
      gp_Pnt               aPoint  = BRep_Tool::Pnt(aVertex);
      if (aPoint.X() != static_cast<double>(i))
      {
        ++myIsRaceDetected;
      }
    }
  }

  const std::vector<TopoDS_Shape>* myShapeVec;
  mutable std::atomic<int>         myIsRaceDetected;
};
} // namespace

TEST(TopLoc_Location_Test, OCC25545_ConcurrentTransformationAccess)
{
  // Bug OCC25545: TopLoc_Location::Transformation() provokes data races
  // This test verifies that concurrent access to TopLoc_Location::Transformation()
  // does not cause data races or incorrect geometry results

  // Place vertices in a vector, giving the i-th vertex the
  // transformation that translates it on the vector (i,0,0) from the origin
  Standard_Integer             n = 1000;
  std::vector<TopoDS_Shape>    aShapeVec(n);
  std::vector<TopLoc_Location> aLocVec(n);
  TopoDS_Shape                 aShape = BRepBuilderAPI_MakeVertex(gp::Origin());
  aShapeVec[0]                        = aShape;

  for (Standard_Integer i = 1; i < n; ++i)
  {
    gp_Trsf aTrsf;
    aTrsf.SetTranslation(gp_Vec(1, 0, 0));
    aLocVec[i]   = aLocVec[i - 1] * aTrsf;
    aShapeVec[i] = aShape.Moved(aLocVec[i]);
  }

  // Evaluator function will access vertices geometry concurrently
  TopLocTransformFunctor aFunc(aShapeVec);

  // Process concurrently
  OSD_Parallel::For(0, n, aFunc);

  // Verify no data race was detected
  EXPECT_EQ(aFunc.myIsRaceDetected, 0)
    << "Data race detected in concurrent TopLoc_Location::Transformation() access";
}
