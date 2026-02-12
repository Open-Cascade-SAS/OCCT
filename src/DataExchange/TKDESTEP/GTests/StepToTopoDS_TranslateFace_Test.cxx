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

#include <gtest/gtest.h>

#include <BRep_Tool.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_HArray2.hxx>
#include <Poly_Triangulation.hxx>
#include <StepData_Factors.hxx>
#include <StepToTopoDS_NMTool.hxx>
#include <StepToTopoDS_Tool.hxx>
#include <StepToTopoDS_TranslateFace.hxx>
#include <StepVisual_ComplexTriangulatedFace.hxx>
#include <StepVisual_ComplexTriangulatedSurfaceSet.hxx>
#include <StepVisual_CoordinatesList.hxx>
#include <StepVisual_FaceOrSurface.hxx>
#include <StepVisual_TriangulatedFace.hxx>
#include <StepVisual_TriangulatedSurfaceSet.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopLoc_Location.hxx>
#include <Transfer_TransientProcess.hxx>
#include <gp_XYZ.hxx>

namespace
{

// Helper: create a CoordinatesList with given points.
static occ::handle<StepVisual_CoordinatesList> createCoords(
  const NCollection_Array1<gp_XYZ>& thePoints)
{
  occ::handle<NCollection_HArray1<gp_XYZ>> aPoints =
    new NCollection_HArray1<gp_XYZ>(thePoints.Lower(), thePoints.Upper());
  for (int i = thePoints.Lower(); i <= thePoints.Upper(); ++i)
  {
    aPoints->SetValue(i, thePoints.Value(i));
  }
  occ::handle<StepVisual_CoordinatesList> aCoords = new StepVisual_CoordinatesList();
  aCoords->Init(new TCollection_HAsciiString("coords"), aPoints);
  return aCoords;
}

// Helper: create a 2D int array for triangles (NRows x 3).
static occ::handle<NCollection_HArray2<int>> createTriangles(
  const NCollection_Array1<NCollection_Array1<int>>& theTriData)
{
  const int                             aNbTri     = theTriData.Length();
  occ::handle<NCollection_HArray2<int>> aTriangles = new NCollection_HArray2<int>(1, aNbTri, 1, 3);
  for (int i = 1; i <= aNbTri; ++i)
  {
    const NCollection_Array1<int>& aTri = theTriData.Value(theTriData.Lower() + i - 1);
    aTriangles->SetValue(i, 1, aTri.Value(aTri.Lower()));
    aTriangles->SetValue(i, 2, aTri.Value(aTri.Lower() + 1));
    aTriangles->SetValue(i, 3, aTri.Value(aTri.Lower() + 2));
  }
  return aTriangles;
}

// Helper: create normals array (NRows x 3).
static occ::handle<NCollection_HArray2<double>> createNormals(
  const NCollection_Array1<gp_XYZ>& theNorms)
{
  const int                                aNbNorms = theNorms.Length();
  occ::handle<NCollection_HArray2<double>> aNormals =
    new NCollection_HArray2<double>(1, aNbNorms, 1, 3);
  for (int i = 1; i <= aNbNorms; ++i)
  {
    const gp_XYZ& aN = theNorms.Value(theNorms.Lower() + i - 1);
    aNormals->SetValue(i, 1, aN.X());
    aNormals->SetValue(i, 2, aN.Y());
    aNormals->SetValue(i, 3, aN.Z());
  }
  return aNormals;
}

// Helper: extract Poly_Triangulation from translated face result.
static occ::handle<Poly_Triangulation> getMesh(const StepToTopoDS_TranslateFace& theTranslator)
{
  EXPECT_TRUE(theTranslator.IsDone());
  const TopoDS_Face& aFace = TopoDS::Face(theTranslator.Value());
  TopLoc_Location    aLoc;
  return BRep_Tool::Triangulation(aFace, aLoc);
}

// Fixture providing common setup for all tests.
class StepToTopoDS_TranslateFaceTest : public testing::Test
{
protected:
  void SetUp() override
  {
    occ::handle<Transfer_TransientProcess> aTP = new Transfer_TransientProcess();
    NCollection_DataMap<occ::handle<StepShape_TopologicalRepresentationItem>, TopoDS_Shape> aMap;
    myTool.Init(aMap, aTP);
    myNMTool.SetActive(false);
  }

  StepToTopoDS_Tool   myTool;
  StepToTopoDS_NMTool myNMTool;
};

} // namespace

// Test: TriangulatedFace with direct node indexing (no pnindex).
TEST_F(StepToTopoDS_TranslateFaceTest, TriangulatedFace_DirectNodes)
{
  NCollection_Array1<gp_XYZ> aPoints(1, 4);
  aPoints.SetValue(1, gp_XYZ(0.0, 0.0, 0.0));
  aPoints.SetValue(2, gp_XYZ(1.0, 0.0, 0.0));
  aPoints.SetValue(3, gp_XYZ(1.0, 1.0, 0.0));
  aPoints.SetValue(4, gp_XYZ(0.0, 1.0, 0.0));

  NCollection_Array1<NCollection_Array1<int>> aTriData(1, 2);
  aTriData.ChangeValue(1) = NCollection_Array1<int>(0, 2);
  aTriData.ChangeValue(1).SetValue(0, 1);
  aTriData.ChangeValue(1).SetValue(1, 2);
  aTriData.ChangeValue(1).SetValue(2, 3);
  aTriData.ChangeValue(2) = NCollection_Array1<int>(0, 2);
  aTriData.ChangeValue(2).SetValue(0, 1);
  aTriData.ChangeValue(2).SetValue(1, 3);
  aTriData.ChangeValue(2).SetValue(2, 4);

  occ::handle<StepVisual_TriangulatedFace> aTF = new StepVisual_TriangulatedFace();
  aTF->Init(new TCollection_HAsciiString("face"),
            createCoords(aPoints),
            4,
            nullptr,
            false,
            StepVisual_FaceOrSurface(),
            nullptr,
            createTriangles(aTriData));

  bool                       aHasGeom = false;
  StepToTopoDS_TranslateFace aTranslator(aTF, myTool, myNMTool, false, aHasGeom);

  occ::handle<Poly_Triangulation> aMesh = getMesh(aTranslator);
  ASSERT_FALSE(aMesh.IsNull());
  EXPECT_EQ(aMesh->NbNodes(), 4);
  EXPECT_EQ(aMesh->NbTriangles(), 2);
  EXPECT_FALSE(aMesh->HasNormals());

  EXPECT_NEAR(aMesh->Node(1).X(), 0.0, 1e-12);
  EXPECT_NEAR(aMesh->Node(2).X(), 1.0, 1e-12);
  EXPECT_NEAR(aMesh->Node(3).Y(), 1.0, 1e-12);

  int aN1, aN2, aN3;
  aMesh->Triangle(1).Get(aN1, aN2, aN3);
  EXPECT_EQ(aN1, 1);
  EXPECT_EQ(aN2, 2);
  EXPECT_EQ(aN3, 3);
}

// Test: TriangulatedFace with pnindex remapping.
TEST_F(StepToTopoDS_TranslateFaceTest, TriangulatedFace_WithPnindex)
{
  // 4 points defined but pnindex selects only 3 of them.
  NCollection_Array1<gp_XYZ> aPoints(1, 4);
  aPoints.SetValue(1, gp_XYZ(0.0, 0.0, 0.0));
  aPoints.SetValue(2, gp_XYZ(10.0, 0.0, 0.0));
  aPoints.SetValue(3, gp_XYZ(10.0, 10.0, 0.0));
  aPoints.SetValue(4, gp_XYZ(0.0, 10.0, 0.0));

  // Pnindex: mesh nodes 1,2,3 map to coordinate indices 2,3,4.
  occ::handle<NCollection_HArray1<int>> aPnindex = new NCollection_HArray1<int>(1, 3);
  aPnindex->SetValue(1, 2);
  aPnindex->SetValue(2, 3);
  aPnindex->SetValue(3, 4);

  NCollection_Array1<NCollection_Array1<int>> aTriData(1, 1);
  aTriData.ChangeValue(1) = NCollection_Array1<int>(0, 2);
  aTriData.ChangeValue(1).SetValue(0, 1);
  aTriData.ChangeValue(1).SetValue(1, 2);
  aTriData.ChangeValue(1).SetValue(2, 3);

  occ::handle<StepVisual_TriangulatedFace> aTF = new StepVisual_TriangulatedFace();
  aTF->Init(new TCollection_HAsciiString("face"),
            createCoords(aPoints),
            4,
            nullptr,
            false,
            StepVisual_FaceOrSurface(),
            aPnindex,
            createTriangles(aTriData));

  bool                       aHasGeom = false;
  StepToTopoDS_TranslateFace aTranslator(aTF, myTool, myNMTool, false, aHasGeom);

  occ::handle<Poly_Triangulation> aMesh = getMesh(aTranslator);
  ASSERT_FALSE(aMesh.IsNull());
  EXPECT_EQ(aMesh->NbNodes(), 3);
  EXPECT_EQ(aMesh->NbTriangles(), 1);

  // Node 1 should be coordinate 2 (10,0,0).
  EXPECT_NEAR(aMesh->Node(1).X(), 10.0, 1e-12);
  EXPECT_NEAR(aMesh->Node(1).Y(), 0.0, 1e-12);
  // Node 2 should be coordinate 3 (10,10,0).
  EXPECT_NEAR(aMesh->Node(2).X(), 10.0, 1e-12);
  EXPECT_NEAR(aMesh->Node(2).Y(), 10.0, 1e-12);
  // Node 3 should be coordinate 4 (0,10,0).
  EXPECT_NEAR(aMesh->Node(3).X(), 0.0, 1e-12);
  EXPECT_NEAR(aMesh->Node(3).Y(), 10.0, 1e-12);
}

// Test: TriangulatedSurfaceSet with direct nodes (no pnindex).
TEST_F(StepToTopoDS_TranslateFaceTest, TriangulatedSurfaceSet_DirectNodes)
{
  NCollection_Array1<gp_XYZ> aPoints(1, 3);
  aPoints.SetValue(1, gp_XYZ(0.0, 0.0, 0.0));
  aPoints.SetValue(2, gp_XYZ(2.0, 0.0, 0.0));
  aPoints.SetValue(3, gp_XYZ(1.0, 2.0, 0.0));

  NCollection_Array1<NCollection_Array1<int>> aTriData(1, 1);
  aTriData.ChangeValue(1) = NCollection_Array1<int>(0, 2);
  aTriData.ChangeValue(1).SetValue(0, 1);
  aTriData.ChangeValue(1).SetValue(1, 2);
  aTriData.ChangeValue(1).SetValue(2, 3);

  occ::handle<StepVisual_TriangulatedSurfaceSet> aTSS = new StepVisual_TriangulatedSurfaceSet();
  aTSS->Init(new TCollection_HAsciiString("surfset"),
             createCoords(aPoints),
             3,
             nullptr,
             nullptr,
             createTriangles(aTriData));

  StepToTopoDS_TranslateFace aTranslator(aTSS, myTool, myNMTool);

  occ::handle<Poly_Triangulation> aMesh = getMesh(aTranslator);
  ASSERT_FALSE(aMesh.IsNull());
  EXPECT_EQ(aMesh->NbNodes(), 3);
  EXPECT_EQ(aMesh->NbTriangles(), 1);

  EXPECT_NEAR(aMesh->Node(1).X(), 0.0, 1e-12);
  EXPECT_NEAR(aMesh->Node(2).X(), 2.0, 1e-12);
  EXPECT_NEAR(aMesh->Node(3).Y(), 2.0, 1e-12);
}

// Test: TriangulatedSurfaceSet with pnindex remapping.
TEST_F(StepToTopoDS_TranslateFaceTest, TriangulatedSurfaceSet_WithPnindex)
{
  NCollection_Array1<gp_XYZ> aPoints(1, 4);
  aPoints.SetValue(1, gp_XYZ(0.0, 0.0, 0.0));
  aPoints.SetValue(2, gp_XYZ(5.0, 0.0, 0.0));
  aPoints.SetValue(3, gp_XYZ(5.0, 5.0, 0.0));
  aPoints.SetValue(4, gp_XYZ(0.0, 5.0, 0.0));

  // Pnindex selects 3 out of 4 coordinates (indices 1, 3, 4).
  occ::handle<NCollection_HArray1<int>> aPnindex = new NCollection_HArray1<int>(1, 3);
  aPnindex->SetValue(1, 1);
  aPnindex->SetValue(2, 3);
  aPnindex->SetValue(3, 4);

  NCollection_Array1<NCollection_Array1<int>> aTriData(1, 1);
  aTriData.ChangeValue(1) = NCollection_Array1<int>(0, 2);
  aTriData.ChangeValue(1).SetValue(0, 1);
  aTriData.ChangeValue(1).SetValue(1, 2);
  aTriData.ChangeValue(1).SetValue(2, 3);

  occ::handle<StepVisual_TriangulatedSurfaceSet> aTSS = new StepVisual_TriangulatedSurfaceSet();
  aTSS->Init(new TCollection_HAsciiString("surfset"),
             createCoords(aPoints),
             4,
             nullptr,
             aPnindex,
             createTriangles(aTriData));

  StepToTopoDS_TranslateFace aTranslator(aTSS, myTool, myNMTool);

  occ::handle<Poly_Triangulation> aMesh = getMesh(aTranslator);
  ASSERT_FALSE(aMesh.IsNull());
  EXPECT_EQ(aMesh->NbNodes(), 3);

  // Node 1 = coord 1 (0,0,0).
  EXPECT_NEAR(aMesh->Node(1).X(), 0.0, 1e-12);
  // Node 2 = coord 3 (5,5,0).
  EXPECT_NEAR(aMesh->Node(2).X(), 5.0, 1e-12);
  EXPECT_NEAR(aMesh->Node(2).Y(), 5.0, 1e-12);
  // Node 3 = coord 4 (0,5,0).
  EXPECT_NEAR(aMesh->Node(3).X(), 0.0, 1e-12);
  EXPECT_NEAR(aMesh->Node(3).Y(), 5.0, 1e-12);
}

// Test: TriangulatedFace with per-node normals.
TEST_F(StepToTopoDS_TranslateFaceTest, TriangulatedFace_WithNormals)
{
  NCollection_Array1<gp_XYZ> aPoints(1, 3);
  aPoints.SetValue(1, gp_XYZ(0.0, 0.0, 0.0));
  aPoints.SetValue(2, gp_XYZ(1.0, 0.0, 0.0));
  aPoints.SetValue(3, gp_XYZ(0.0, 1.0, 0.0));

  NCollection_Array1<gp_XYZ> aNorms(1, 3);
  aNorms.SetValue(1, gp_XYZ(0.0, 0.0, 1.0));
  aNorms.SetValue(2, gp_XYZ(0.0, 0.0, 1.0));
  aNorms.SetValue(3, gp_XYZ(0.0, 0.0, 1.0));

  NCollection_Array1<NCollection_Array1<int>> aTriData(1, 1);
  aTriData.ChangeValue(1) = NCollection_Array1<int>(0, 2);
  aTriData.ChangeValue(1).SetValue(0, 1);
  aTriData.ChangeValue(1).SetValue(1, 2);
  aTriData.ChangeValue(1).SetValue(2, 3);

  occ::handle<StepVisual_TriangulatedFace> aTF = new StepVisual_TriangulatedFace();
  aTF->Init(new TCollection_HAsciiString("face"),
            createCoords(aPoints),
            3,
            createNormals(aNorms),
            false,
            StepVisual_FaceOrSurface(),
            nullptr,
            createTriangles(aTriData));

  bool                       aHasGeom = false;
  StepToTopoDS_TranslateFace aTranslator(aTF, myTool, myNMTool, false, aHasGeom);

  occ::handle<Poly_Triangulation> aMesh = getMesh(aTranslator);
  ASSERT_FALSE(aMesh.IsNull());
  EXPECT_TRUE(aMesh->HasNormals());

  const gp_Dir aNormal = aMesh->Normal(1);
  EXPECT_NEAR(aNormal.Z(), 1.0, 1e-12);
}

// Test: TriangulatedFace with single normal shared by all nodes.
TEST_F(StepToTopoDS_TranslateFaceTest, TriangulatedFace_SingleNormal)
{
  NCollection_Array1<gp_XYZ> aPoints(1, 3);
  aPoints.SetValue(1, gp_XYZ(0.0, 0.0, 0.0));
  aPoints.SetValue(2, gp_XYZ(1.0, 0.0, 0.0));
  aPoints.SetValue(3, gp_XYZ(0.0, 1.0, 0.0));

  NCollection_Array1<gp_XYZ> aNorms(1, 1);
  aNorms.SetValue(1, gp_XYZ(0.0, 1.0, 0.0));

  NCollection_Array1<NCollection_Array1<int>> aTriData(1, 1);
  aTriData.ChangeValue(1) = NCollection_Array1<int>(0, 2);
  aTriData.ChangeValue(1).SetValue(0, 1);
  aTriData.ChangeValue(1).SetValue(1, 2);
  aTriData.ChangeValue(1).SetValue(2, 3);

  occ::handle<StepVisual_TriangulatedFace> aTF = new StepVisual_TriangulatedFace();
  aTF->Init(new TCollection_HAsciiString("face"),
            createCoords(aPoints),
            3,
            createNormals(aNorms),
            false,
            StepVisual_FaceOrSurface(),
            nullptr,
            createTriangles(aTriData));

  bool                       aHasGeom = false;
  StepToTopoDS_TranslateFace aTranslator(aTF, myTool, myNMTool, false, aHasGeom);

  occ::handle<Poly_Triangulation> aMesh = getMesh(aTranslator);
  ASSERT_FALSE(aMesh.IsNull());
  EXPECT_TRUE(aMesh->HasNormals());

  // All 3 nodes should share the same normal (0,1,0).
  for (int i = 1; i <= 3; ++i)
  {
    const gp_Dir aNormal = aMesh->Normal(i);
    EXPECT_NEAR(aNormal.X(), 0.0, 1e-12);
    EXPECT_NEAR(aNormal.Y(), 1.0, 1e-12);
    EXPECT_NEAR(aNormal.Z(), 0.0, 1e-12);
  }
}

// Test: Length factor scaling of coordinates.
TEST_F(StepToTopoDS_TranslateFaceTest, TriangulatedSurfaceSet_LengthFactor)
{
  NCollection_Array1<gp_XYZ> aPoints(1, 3);
  aPoints.SetValue(1, gp_XYZ(0.0, 0.0, 0.0));
  aPoints.SetValue(2, gp_XYZ(1.0, 0.0, 0.0));
  aPoints.SetValue(3, gp_XYZ(0.0, 1.0, 0.0));

  NCollection_Array1<NCollection_Array1<int>> aTriData(1, 1);
  aTriData.ChangeValue(1) = NCollection_Array1<int>(0, 2);
  aTriData.ChangeValue(1).SetValue(0, 1);
  aTriData.ChangeValue(1).SetValue(1, 2);
  aTriData.ChangeValue(1).SetValue(2, 3);

  occ::handle<StepVisual_TriangulatedSurfaceSet> aTSS = new StepVisual_TriangulatedSurfaceSet();
  aTSS->Init(new TCollection_HAsciiString("surfset"),
             createCoords(aPoints),
             3,
             nullptr,
             nullptr,
             createTriangles(aTriData));

  StepData_Factors aFactors;
  aFactors.InitializeFactors(25.4, 1.0, 1.0);

  StepToTopoDS_TranslateFace aTranslator(aTSS, myTool, myNMTool, aFactors);

  occ::handle<Poly_Triangulation> aMesh = getMesh(aTranslator);
  ASSERT_FALSE(aMesh.IsNull());

  // Coordinates should be scaled by length factor 25.4.
  EXPECT_NEAR(aMesh->Node(2).X(), 25.4, 1e-10);
  EXPECT_NEAR(aMesh->Node(3).Y(), 25.4, 1e-10);
}

// Test: ComplexTriangulatedFace with triangle strips.
TEST_F(StepToTopoDS_TranslateFaceTest, ComplexTriangulatedFace_TriangleStrips)
{
  NCollection_Array1<gp_XYZ> aPoints(1, 4);
  aPoints.SetValue(1, gp_XYZ(0.0, 0.0, 0.0));
  aPoints.SetValue(2, gp_XYZ(1.0, 0.0, 0.0));
  aPoints.SetValue(3, gp_XYZ(0.0, 1.0, 0.0));
  aPoints.SetValue(4, gp_XYZ(1.0, 1.0, 0.0));

  // Triangle strip: [1, 2, 3, 4] produces 2 triangles.
  occ::handle<NCollection_HArray1<int>> aStrip = new NCollection_HArray1<int>(1, 4);
  aStrip->SetValue(1, 1);
  aStrip->SetValue(2, 2);
  aStrip->SetValue(3, 3);
  aStrip->SetValue(4, 4);

  occ::handle<NCollection_HArray1<occ::handle<Standard_Transient>>> aStrips =
    new NCollection_HArray1<occ::handle<Standard_Transient>>(1, 1);
  aStrips->SetValue(1, aStrip);

  occ::handle<StepVisual_ComplexTriangulatedFace> aCTF = new StepVisual_ComplexTriangulatedFace();
  aCTF->Init(new TCollection_HAsciiString("complex_face"),
             createCoords(aPoints),
             4,
             nullptr,
             false,
             StepVisual_FaceOrSurface(),
             nullptr,
             aStrips,
             nullptr);

  bool                       aHasGeom = false;
  StepToTopoDS_TranslateFace aTranslator(aCTF, myTool, myNMTool, false, aHasGeom);

  occ::handle<Poly_Triangulation> aMesh = getMesh(aTranslator);
  ASSERT_FALSE(aMesh.IsNull());
  EXPECT_EQ(aMesh->NbNodes(), 4);
  EXPECT_EQ(aMesh->NbTriangles(), 2);
}

// Test: ComplexTriangulatedFace with triangle fans.
TEST_F(StepToTopoDS_TranslateFaceTest, ComplexTriangulatedFace_TriangleFans)
{
  NCollection_Array1<gp_XYZ> aPoints(1, 5);
  aPoints.SetValue(1, gp_XYZ(0.0, 0.0, 0.0));
  aPoints.SetValue(2, gp_XYZ(1.0, 0.0, 0.0));
  aPoints.SetValue(3, gp_XYZ(1.0, 1.0, 0.0));
  aPoints.SetValue(4, gp_XYZ(0.0, 1.0, 0.0));
  aPoints.SetValue(5, gp_XYZ(-1.0, 0.0, 0.0));

  // Triangle fan: [1, 2, 3, 4, 5] with center at vertex 1 produces 3 triangles.
  occ::handle<NCollection_HArray1<int>> aFan = new NCollection_HArray1<int>(1, 5);
  aFan->SetValue(1, 1);
  aFan->SetValue(2, 2);
  aFan->SetValue(3, 3);
  aFan->SetValue(4, 4);
  aFan->SetValue(5, 5);

  occ::handle<NCollection_HArray1<occ::handle<Standard_Transient>>> aFans =
    new NCollection_HArray1<occ::handle<Standard_Transient>>(1, 1);
  aFans->SetValue(1, aFan);

  occ::handle<StepVisual_ComplexTriangulatedFace> aCTF = new StepVisual_ComplexTriangulatedFace();
  aCTF->Init(new TCollection_HAsciiString("complex_face"),
             createCoords(aPoints),
             5,
             nullptr,
             false,
             StepVisual_FaceOrSurface(),
             nullptr,
             nullptr,
             aFans);

  bool                       aHasGeom = false;
  StepToTopoDS_TranslateFace aTranslator(aCTF, myTool, myNMTool, false, aHasGeom);

  occ::handle<Poly_Triangulation> aMesh = getMesh(aTranslator);
  ASSERT_FALSE(aMesh.IsNull());
  EXPECT_EQ(aMesh->NbNodes(), 5);
  EXPECT_EQ(aMesh->NbTriangles(), 3);
}

// Test: Null input produces no result.
TEST_F(StepToTopoDS_TranslateFaceTest, NullInput_TessellatedFace)
{
  occ::handle<StepVisual_TessellatedFace> aNullTF;
  bool                                    aHasGeom = false;
  StepToTopoDS_TranslateFace              aTranslator;
  aTranslator.Init(aNullTF, myTool, myNMTool, false, aHasGeom);
  EXPECT_FALSE(aTranslator.IsDone());
}

// Test: Null input for TessellatedSurfaceSet.
TEST_F(StepToTopoDS_TranslateFaceTest, NullInput_TessellatedSurfaceSet)
{
  occ::handle<StepVisual_TessellatedSurfaceSet> aNullTSS;
  StepToTopoDS_TranslateFace                    aTranslator;
  aTranslator.Init(aNullTSS, myTool, myNMTool);
  EXPECT_FALSE(aTranslator.IsDone());
}

// Test: ComplexTriangulatedFace with strip containing degenerate triangles (duplicate indices).
TEST_F(StepToTopoDS_TranslateFaceTest, ComplexTriangulatedFace_DegenerateStrip)
{
  NCollection_Array1<gp_XYZ> aPoints(1, 4);
  aPoints.SetValue(1, gp_XYZ(0.0, 0.0, 0.0));
  aPoints.SetValue(2, gp_XYZ(1.0, 0.0, 0.0));
  aPoints.SetValue(3, gp_XYZ(0.0, 1.0, 0.0));
  aPoints.SetValue(4, gp_XYZ(1.0, 1.0, 0.0));

  // Strip with degenerate: [1, 2, 3, 3] - the second triangle (2,3,3) is degenerate.
  occ::handle<NCollection_HArray1<int>> aStrip = new NCollection_HArray1<int>(1, 4);
  aStrip->SetValue(1, 1);
  aStrip->SetValue(2, 2);
  aStrip->SetValue(3, 3);
  aStrip->SetValue(4, 3);

  occ::handle<NCollection_HArray1<occ::handle<Standard_Transient>>> aStrips =
    new NCollection_HArray1<occ::handle<Standard_Transient>>(1, 1);
  aStrips->SetValue(1, aStrip);

  occ::handle<StepVisual_ComplexTriangulatedFace> aCTF = new StepVisual_ComplexTriangulatedFace();
  aCTF->Init(new TCollection_HAsciiString("complex_face"),
             createCoords(aPoints),
             4,
             nullptr,
             false,
             StepVisual_FaceOrSurface(),
             nullptr,
             aStrips,
             nullptr);

  bool                       aHasGeom = false;
  StepToTopoDS_TranslateFace aTranslator(aCTF, myTool, myNMTool, false, aHasGeom);

  occ::handle<Poly_Triangulation> aMesh = getMesh(aTranslator);
  ASSERT_FALSE(aMesh.IsNull());
  // Only 1 non-degenerate triangle from strip [1,2,3,3].
  EXPECT_EQ(aMesh->NbTriangles(), 1);
}
