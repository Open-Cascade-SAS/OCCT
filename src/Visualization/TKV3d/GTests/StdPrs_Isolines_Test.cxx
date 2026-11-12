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

#include <Bnd_Box.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Graphic3d_ArrayOfPrimitives.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HSequence.hxx>
#include <NCollection_List.hxx>
#include <Poly_Triangle.hxx>
#include <Poly_Triangulation.hxx>
#include <Precision.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Standard_Real.hxx>
#include <Standard_Type.hxx>
#include <StdPrs_Isolines.hxx>
#include <StdPrs_WFShape.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <gp.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>

#include <array>
#include <cmath>
#include <sstream>

namespace
{
using Polylines = NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>>;

//==================================================================================================

TopoDS_Face makeBoundarylessFace(const occ::handle<Geom_Surface>& theSurface)
{
  // Unlike BRepBuilderAPI_MakeFace, the low-level builder does not create
  // natural boundary wires. This reproduces an empty STEP face bounds list.
  TopoDS_Face aFace;
  BRep_Builder().MakeFace(aFace, theSurface, Precision::Confusion());
  return aFace;
}

//==================================================================================================

occ::handle<Prs3d_Drawer> makeDrawer()
{
  occ::handle<Prs3d_Drawer> aDrawer = new Prs3d_Drawer;
  aDrawer->SetupOwnDefaults();
  aDrawer->UIsoAspect()->SetNumber(3);
  aDrawer->VIsoAspect()->SetNumber(3);
  aDrawer->SetIsoOnPlane(true);
  return aDrawer;
}

//==================================================================================================

std::array<occ::handle<Geom_Surface>, 3> unboundedSurfaces()
{
  const gp_Ax3 anAxis(gp::Origin(), gp::DZ());
  return {new Geom_CylindricalSurface(anAxis, 3.0),
          new Geom_Plane(anAxis),
          new Geom_ConicalSurface(anAxis, 0.3, 3.0)};
}

//==================================================================================================

Bnd_Box isolineBounds(const TopoDS_Face& theFace, const occ::handle<Prs3d_Drawer>& theDrawer)
{
  Polylines aU, aV;
  StdPrs_Isolines::Add(theFace, theDrawer, 0.1, aU, aV);
  Bnd_Box aBox;
  for (const Polylines* aLines : {&aU, &aV})
  {
    for (const auto& aLine : *aLines)
    {
      for (int aPoint = 1; aPoint <= aLine->Length(); ++aPoint)
      {
        const gp_Pnt& aPnt = aLine->Value(aPoint);
        EXPECT_TRUE(std::isfinite(aPnt.X()));
        EXPECT_TRUE(std::isfinite(aPnt.Y()));
        EXPECT_TRUE(std::isfinite(aPnt.Z()));
        aBox.Add(aPnt);
      }
    }
  }
  return aBox;
}

//==================================================================================================

TCollection_AsciiString shapeBytes(const TopoDS_Shape& theShape)
{
  std::ostringstream aStream;
  BRepTools::Write(theShape, aStream);
  return TCollection_AsciiString(aStream.str().c_str());
}

//==================================================================================================

occ::handle<Poly_Triangulation> makeTriangulation()
{
  occ::handle<Poly_Triangulation> aMesh = new Poly_Triangulation(4, 2, true);
  aMesh->SetNode(1, gp_Pnt(-10.0, -10.0, 0.0));
  aMesh->SetNode(2, gp_Pnt(10.0, -10.0, 0.0));
  aMesh->SetNode(3, gp_Pnt(10.0, 10.0, 0.0));
  aMesh->SetNode(4, gp_Pnt(-10.0, 10.0, 0.0));
  aMesh->SetUVNode(1, gp_Pnt2d(-10.0, -10.0));
  aMesh->SetUVNode(2, gp_Pnt2d(10.0, -10.0));
  aMesh->SetUVNode(3, gp_Pnt2d(10.0, 10.0));
  aMesh->SetUVNode(4, gp_Pnt2d(-10.0, 10.0));
  aMesh->SetTriangle(1, Poly_Triangle(1, 2, 3));
  aMesh->SetTriangle(2, Poly_Triangle(1, 3, 4));
  return aMesh;
}
} // namespace

TEST(StdPrs_IsolinesTest, BoundarylessUnboundedFacesHaveNoAutomaticIsolines)
{
  for (const auto& aSurface : unboundedSurfaces())
  {
    SCOPED_TRACE(aSurface->DynamicType()->Name());
    const TopoDS_Face aFace = makeBoundarylessFace(aSurface);
    ASSERT_FALSE(TopExp_Explorer(aFace, TopAbs_WIRE).More());
    TopLoc_Location aLocation;
    ASSERT_TRUE(BRep_Tool::Triangulation(aFace, aLocation).IsNull());
    const TCollection_AsciiString aBefore = shapeBytes(aFace);
    occ::handle<Prs3d_Drawer>     aDrawer = makeDrawer();
    // Missing triangulation falls back to the same surface isoline algorithm.
    for (bool aToUseTriangulation : {false, true})
    {
      aDrawer->SetIsoOnTriangulation(aToUseTriangulation);
      EXPECT_TRUE(isolineBounds(aFace, aDrawer).IsVoid());
    }
    EXPECT_EQ(shapeBytes(aFace), aBefore);
  }
}

TEST(StdPrs_IsolinesTest, InfiniteSurfaceDisplayCanBeExplicitlyEnabled)
{
  for (const auto& aSurface : unboundedSurfaces())
  {
    SCOPED_TRACE(aSurface->DynamicType()->Name());
    const TopoDS_Face         aFace   = makeBoundarylessFace(aSurface);
    occ::handle<Prs3d_Drawer> aDrawer = makeDrawer();
    aDrawer->SetMaximalParameterValue(64.0);
    aDrawer->SetIsoOnUnboundedFace(true);
    const Bnd_Box aBox = isolineBounds(aFace, aDrawer);
    ASSERT_FALSE(aBox.IsVoid());
    EXPECT_FALSE(aBox.IsOpen());
    // The existing infinite-curve extent setting remains effective.
    EXPECT_GT(aBox.CornerMax().Distance(aBox.CornerMin()), 32.0);
    EXPECT_LT(aBox.CornerMax().Distance(aBox.CornerMin()), 256.0);
    aDrawer->SetIsoOnUnboundedFace(false);
    EXPECT_TRUE(isolineBounds(aFace, aDrawer).IsVoid());
  }
}

TEST(StdPrs_IsolinesTest, EmptyTriangulationDoesNotEnableUnboundedIsolines)
{
  const TopoDS_Face aFace = makeBoundarylessFace(new Geom_CylindricalSurface(gp::XOY(), 3.0));
  BRep_Builder().UpdateFace(aFace, new Poly_Triangulation);
  TopLoc_Location aLocation;
  ASSERT_FALSE(BRep_Tool::Triangulation(aFace, aLocation).IsNull());
  ASSERT_EQ(BRep_Tool::Triangulation(aFace, aLocation)->NbTriangles(), 0);
  EXPECT_TRUE(isolineBounds(aFace, makeDrawer()).IsVoid());
}

TEST(StdPrs_IsolinesTest, UnboundedFaceSettingIsInheritedAndReset)
{
  const TopoDS_Face aFace = makeBoundarylessFace(new Geom_CylindricalSurface(gp::XOY(), 3.0));
  occ::handle<Prs3d_Drawer> aParent = makeDrawer();
  occ::handle<Prs3d_Drawer> aChild  = new Prs3d_Drawer;
  aParent->SetMaximalParameterValue(64.0);
  aChild->SetLink(aParent);
  EXPECT_FALSE(aChild->IsoOnUnboundedFace());
  EXPECT_FALSE(aChild->HasOwnIsoOnUnboundedFace());
  EXPECT_TRUE(isolineBounds(aFace, aChild).IsVoid());
  aParent->SetIsoOnUnboundedFace(true);
  EXPECT_TRUE(aChild->IsoOnUnboundedFace());
  EXPECT_FALSE(isolineBounds(aFace, aChild).IsVoid());
  aChild->SetIsoOnUnboundedFace(false);
  EXPECT_TRUE(aChild->HasOwnIsoOnUnboundedFace());
  EXPECT_FALSE(aChild->IsoOnUnboundedFace());
  EXPECT_TRUE(isolineBounds(aFace, aChild).IsVoid());
  aChild->UnsetOwnIsoOnUnboundedFace();
  EXPECT_TRUE(aChild->IsoOnUnboundedFace());
  EXPECT_FALSE(aChild->HasOwnIsoOnUnboundedFace());
  EXPECT_FALSE(isolineBounds(aFace, aChild).IsVoid());
  aChild->SetIsoOnUnboundedFace(false);
  aChild->ClearLocalAttributes();
  EXPECT_TRUE(aChild->IsoOnUnboundedFace());
  EXPECT_FALSE(aChild->HasOwnIsoOnUnboundedFace());
  EXPECT_FALSE(isolineBounds(aFace, aChild).IsVoid());
}

TEST(StdPrs_IsolinesTest, BoundarylessFiniteNaturalDomainsAreRetained)
{
  const gp_Ax3               anAxis(gp::Origin(), gp::DZ());
  NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 2);
  aPoles(1, 1)                                             = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(1, 2)                                             = gp_Pnt(0.0, 10.0, 0.0);
  aPoles(2, 1)                                             = gp_Pnt(10.0, 0.0, 2.0);
  aPoles(2, 2)                                             = gp_Pnt(10.0, 10.0, 2.0);
  const std::array<occ::handle<Geom_Surface>, 4> aSurfaces = {
    new Geom_SphericalSurface(anAxis, 3.0),
    new Geom_ToroidalSurface(anAxis, 5.0, 1.0),
    new Geom_BezierSurface(aPoles),
    new Geom_RectangularTrimmedSurface(new Geom_Plane(anAxis), -5.0, 5.0, -7.0, 7.0)};
  for (const auto& aSurface : aSurfaces)
  {
    SCOPED_TRACE(aSurface->DynamicType()->Name());
    const TopoDS_Face aFace = makeBoundarylessFace(aSurface);
    ASSERT_FALSE(TopExp_Explorer(aFace, TopAbs_WIRE).More());
    const TCollection_AsciiString aBefore = shapeBytes(aFace);
    const Bnd_Box                 aBox    = isolineBounds(aFace, makeDrawer());
    ASSERT_FALSE(aBox.IsVoid());
    EXPECT_FALSE(aBox.IsOpen());
    EXPECT_EQ(shapeBytes(aFace), aBefore);
  }
}

TEST(StdPrs_IsolinesTest, BoundedFacesOnInfiniteSupportAreRetained)
{
  const gp_Ax3            anAxis(gp::Origin(), gp::DZ());
  BRepBuilderAPI_MakeFace aCylinderMaker(new Geom_CylindricalSurface(anAxis, 3.0),
                                         0.0,
                                         2.0 * M_PI,
                                         -5.0,
                                         5.0,
                                         Precision::Confusion());
  BRepBuilderAPI_MakeFace aPlaneMaker(new Geom_Plane(anAxis),
                                      -1.0e8,
                                      1.0e8,
                                      -1.0e8,
                                      1.0e8,
                                      Precision::Confusion());
  ASSERT_TRUE(aCylinderMaker.IsDone());
  ASSERT_TRUE(aPlaneMaker.IsDone());
  const std::array<TopoDS_Face, 2> aFaces = {aCylinderMaker.Face(), aPlaneMaker.Face()};
  for (const TopoDS_Face& aFace : aFaces)
  {
    ASSERT_TRUE(TopExp_Explorer(aFace, TopAbs_WIRE).More());
    const Bnd_Box aBox = isolineBounds(aFace, makeDrawer());
    EXPECT_FALSE(aBox.IsVoid());
    EXPECT_FALSE(aBox.IsOpen());
  }
}

TEST(StdPrs_IsolinesTest, TriangulatedInfiniteSurfaceIsRetained)
{
  const TopoDS_Face                     aFace = makeBoundarylessFace(new Geom_Plane(gp::XOY()));
  const occ::handle<Poly_Triangulation> aMesh = makeTriangulation();
  BRep_Builder().UpdateFace(aFace, aMesh);
  const TCollection_AsciiString aBefore = shapeBytes(aFace);
  occ::handle<Prs3d_Drawer>     aDrawer = makeDrawer();
  aDrawer->UIsoAspect()->SetNumber(1);
  aDrawer->VIsoAspect()->SetNumber(1);
  // A usable mesh also exempts on-surface isolines from the fallback policy.
  aDrawer->SetMaximalParameterValue(64.0);
  aDrawer->SetIsoOnTriangulation(false);
  EXPECT_FALSE(isolineBounds(aFace, aDrawer).IsVoid());
  aDrawer->SetIsoOnTriangulation(true);
  const Bnd_Box aBox = isolineBounds(aFace, aDrawer);
  ASSERT_FALSE(aBox.IsVoid());
  ASSERT_FALSE(aBox.IsOpen());
  const auto [aMinX, aMaxX, aMinY, aMaxY, aMinZ, aMaxZ] = aBox.Get();
  EXPECT_GE(aMinX, -10.0 - Precision::Confusion());
  EXPECT_GE(aMinY, -10.0 - Precision::Confusion());
  EXPECT_LE(aMaxX, 10.0 + Precision::Confusion());
  EXPECT_LE(aMaxY, 10.0 + Precision::Confusion());
  EXPECT_NEAR(aMinZ, 0.0, Precision::Confusion());
  EXPECT_NEAR(aMaxZ, 0.0, Precision::Confusion());
  TopLoc_Location aLocation;
  EXPECT_EQ(BRep_Tool::Triangulation(aFace, aLocation), aMesh);
  EXPECT_EQ(shapeBytes(aFace), aBefore);
}

TEST(StdPrs_IsolinesTest, MeshOnlyBoundaryAndOrdinaryEdgeAreRetained)
{
  TopoDS_Face aMeshFace;
  BRep_Builder().MakeFace(aMeshFace, makeTriangulation());
  const occ::handle<Graphic3d_ArrayOfPrimitives> aMeshEdges =
    StdPrs_WFShape::AddEdgesOnTriangulation(aMeshFace);
  ASSERT_FALSE(aMeshEdges.IsNull());
  EXPECT_EQ(aMeshEdges->VertexNumber(), 8);

  BRepBuilderAPI_MakeEdge anEdgeMaker(gp_Pnt(-1.0e8, 0.0, 0.0), gp_Pnt(1.0e8, 0.0, 0.0));
  ASSERT_TRUE(anEdgeMaker.IsDone());
  const occ::handle<Graphic3d_ArrayOfPrimitives> aWire =
    StdPrs_WFShape::AddAllEdges(anEdgeMaker.Edge(), makeDrawer());
  ASSERT_FALSE(aWire.IsNull());
  ASSERT_EQ(aWire->VertexNumber(), 2);
  EXPECT_DOUBLE_EQ(aWire->Vertice(1).Distance(aWire->Vertice(2)), 2.0e8);
}
