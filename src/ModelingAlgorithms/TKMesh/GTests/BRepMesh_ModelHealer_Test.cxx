// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <gtest/gtest.h>

#include <BRep_Tool.hxx>
#include <BRepMesh_Context.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <IMeshData_Edge.hxx>
#include <IMeshData_Face.hxx>
#include <IMeshData_Model.hxx>
#include <IMeshData_PCurve.hxx>
#include <IMeshData_Status.hxx>
#include <IMeshData_Wire.hxx>
#include <IMeshTools_MeshBuilder.hxx>
#include <Message_ProgressRange.hxx>
#include <Poly_Triangulation.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

//=================================================================================================
// BRepMesh_ModelHealer::fixFaceBoundaries connects the end points of consecutive edges of each
// wire, reading them as GetPoint(0) and GetPoint(ParametersNb() - 1) of their pcurves.
// BRepMesh_EdgeDiscret catches a Standard_Failure raised while tessellating an edge, marks the
// edge with IMeshData_Failure and lets the meshing continue, leaving the pcurves of that edge
// without any point. The healer must not try to connect such an edge.
//=================================================================================================

namespace
{
//! Returns True if the wires of the face contain the given edge.
bool isBoundedBy(const IMeshData::IFaceHandle& theDFace, const IMeshData::IEdgeHandle& theDEdge)
{
  for (int aWireIt = 0; aWireIt < theDFace->WiresNb(); ++aWireIt)
  {
    const IMeshData::IWireHandle& aDWire = theDFace->GetWire(aWireIt);
    for (int aEdgeIt = 0; aEdgeIt < aDWire->EdgesNb(); ++aEdgeIt)
    {
      if (aDWire->GetEdge(aEdgeIt) == theDEdge.get())
      {
        return true;
      }
    }
  }
  return false;
}

//! Returns True if the face carries a triangulation.
bool hasTriangulation(const TopoDS_Face& theFace)
{
  TopLoc_Location aLoc;
  return !BRep_Tool::Triangulation(theFace, aLoc).IsNull();
}
} // namespace

//! Every edge of the box fails to be discretized: BRepMesh_CurveTessellator rejects the default
//! IMeshTools_Parameters::MinSize, which only BRepMesh_IncrementalMesh replaces by a valid one.
//! The meshing must end with all faces reported as failed instead of crashing in the healer.
TEST(BRepMesh_ModelHealerTest, AllEdgesUndiscretized_FacesFailWithoutCrash)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10., 10., 10.).Shape();

  occ::handle<BRepMesh_Context> aContext = new BRepMesh_Context();
  aContext->SetShape(aBox);
  aContext->ChangeParameters().Deflection = 0.1;
  aContext->ChangeParameters().CleanModel = false;
  ASSERT_LE(aContext->GetParameters().MinSize, 0.0);

  IMeshTools_MeshBuilder aBuilder(aContext);
  aBuilder.Perform(Message_ProgressRange());

  const occ::handle<IMeshData_Model>& aModel = aContext->GetModel();
  ASSERT_FALSE(aModel.IsNull());
  ASSERT_EQ(aModel->FacesNb(), 6);
  for (int anEdgeIt = 0; anEdgeIt < aModel->EdgesNb(); ++anEdgeIt)
  {
    ASSERT_TRUE(aModel->GetEdge(anEdgeIt)->IsSet(IMeshData_Failure))
      << "edge " << anEdgeIt << " is expected to fail its discretization";
  }

  for (int aFaceIt = 0; aFaceIt < aModel->FacesNb(); ++aFaceIt)
  {
    const IMeshData::IFaceHandle& aDFace = aModel->GetFace(aFaceIt);
    EXPECT_TRUE(aDFace->IsSet(IMeshData_Failure)) << "face " << aFaceIt;
    EXPECT_FALSE(hasTriangulation(aDFace->GetFace())) << "face " << aFaceIt;
  }
}

//! One edge of the box fails to be discretized, the others succeed. Only the two faces bounded
//! by that edge must be reported as failed; the four others must still be meshed.
TEST(BRepMesh_ModelHealerTest, OneEdgeUndiscretized_OnlyAdjacentFacesFail)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10., 10., 10.).Shape();

  occ::handle<BRepMesh_Context> aContext = new BRepMesh_Context();
  aContext->SetShape(aBox);
  aContext->ChangeParameters().Deflection = 0.1;
  aContext->ChangeParameters().MinSize    = 0.01;
  aContext->ChangeParameters().CleanModel = false;

  ASSERT_TRUE(aContext->BuildModel());
  ASSERT_TRUE(aContext->DiscretizeEdges());

  const occ::handle<IMeshData_Model>& aModel = aContext->GetModel();
  ASSERT_EQ(aModel->FacesNb(), 6);

  // Leave the edge as BRepMesh_EdgeDiscret does when its tessellation raises an exception.
  const IMeshData::IEdgeHandle& aFailedEdge = aModel->GetEdge(0);
  aFailedEdge->Clear(false);
  aFailedEdge->SetStatus(IMeshData_Failure);
  ASSERT_EQ(aFailedEdge->GetPCurve(0)->ParametersNb(), 0);

  ASSERT_TRUE(aContext->HealModel());
  ASSERT_TRUE(aContext->PreProcessModel());
  aContext->DiscretizeFaces(Message_ProgressRange());
  aContext->PostProcessModel();

  int aNbFailedFaces = 0;
  for (int aFaceIt = 0; aFaceIt < aModel->FacesNb(); ++aFaceIt)
  {
    const IMeshData::IFaceHandle& aDFace = aModel->GetFace(aFaceIt);
    if (isBoundedBy(aDFace, aFailedEdge))
    {
      ++aNbFailedFaces;
      EXPECT_TRUE(aDFace->IsSet(IMeshData_Failure)) << "face " << aFaceIt;
      EXPECT_FALSE(hasTriangulation(aDFace->GetFace())) << "face " << aFaceIt;
    }
    else
    {
      EXPECT_FALSE(aDFace->IsSet(IMeshData_Failure)) << "face " << aFaceIt;
      EXPECT_TRUE(hasTriangulation(aDFace->GetFace())) << "face " << aFaceIt;
    }
  }
  EXPECT_EQ(aNbFailedFaces, 2);
}
