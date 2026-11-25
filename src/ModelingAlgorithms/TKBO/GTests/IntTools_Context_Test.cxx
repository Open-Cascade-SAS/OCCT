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

#include <BRepAdaptor_Surface.hxx>
#include <BRepBuilderAPI_NurbsConvert.hxx>
#include <BRep_Tool.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <IntCurveSurface_PolyhedronCache.hxx>
#include <IntTools_Context.hxx>
#include <IntTools_FClass2d.hxx>
#include <NCollection_DynamicArray.hxx>
#include <TopAbs_State.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>

#include <gtest/gtest.h>

namespace
{
//! Faces of a cylinder of radius 2 and height 5 along Z
class IntTools_ContextTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    for (TopExp_Explorer anExp(BRepPrimAPI_MakeCylinder(2., 5.).Shape(), TopAbs_FACE); anExp.More();
         anExp.Next())
    {
      const TopoDS_Face& aFace = TopoDS::Face(anExp.Current());
      if (BRepAdaptor_Surface(aFace).GetType() == GeomAbs_Cylinder)
      {
        myLateral = aFace;
      }
      else if (myPlane1.IsNull())
      {
        myPlane1 = aFace;
      }
      else
      {
        myPlane2 = aFace;
      }
    }
    ASSERT_FALSE(myLateral.IsNull());
    ASSERT_FALSE(myPlane2.IsNull());
  }

  TopoDS_Face myLateral;
  TopoDS_Face myPlane1;
  TopoDS_Face myPlane2;
};
} // namespace

//=================================================================================================

TEST_F(IntTools_ContextTest, ClearFaceCache_ReleasesDataOfTheFaceOnly)
{
  occ::handle<IntTools_Context> aContext = new IntTools_Context();
  const gp_Pnt                  aPoint(1., 0.5, 2.5);
  GeomAPI_ProjectPointOnSurf&   aProj = aContext->ProjPS(myLateral);
  aProj.Perform(aPoint);
  ASSERT_TRUE(aProj.IsDone());
  ASSERT_GT(aProj.NbPoints(), 0);
  const double aDistance = aProj.LowerDistance();
  double       aU, aV;
  aProj.LowerDistanceParameters(aU, aV);
  const TopAbs_State aState = aContext->FClass2d(myLateral).Perform(gp_Pnt2d(aU, aV));
  aContext->SurfaceAdaptor(myLateral);
  aContext->BndBox(myLateral);
  aContext->Hatcher(myLateral);
  aContext->ProjPS(myPlane1);
  EXPECT_EQ(aContext->NbPOnSProjectors(), 2);

  // The orientation of the face does not matter
  aContext->ClearFaceCache(TopoDS::Face(myLateral.Reversed()));
  EXPECT_EQ(aContext->NbPOnSProjectors(), 1);
  // Releasing again, or releasing a face without cached data, does nothing
  aContext->ClearFaceCache(myLateral);
  aContext->ClearFaceCache(myPlane2);
  EXPECT_EQ(aContext->NbPOnSProjectors(), 1);

  // The data is computed again on demand, with the same results
  GeomAPI_ProjectPointOnSurf& aProjNew = aContext->ProjPS(myLateral);
  aProjNew.Perform(aPoint);
  ASSERT_TRUE(aProjNew.IsDone());
  ASSERT_GT(aProjNew.NbPoints(), 0);
  EXPECT_EQ(aProjNew.LowerDistance(), aDistance);
  double aUNew, aVNew;
  aProjNew.LowerDistanceParameters(aUNew, aVNew);
  EXPECT_EQ(aUNew, aU);
  EXPECT_EQ(aVNew, aV);
  EXPECT_EQ(aContext->FClass2d(myLateral).Perform(gp_Pnt2d(aU, aV)), aState);
  EXPECT_EQ(aContext->NbPOnSProjectors(), 2);
}

//=================================================================================================

TEST_F(IntTools_ContextTest, ClearPOnSProjectors_ReleasesAllProjectors)
{
  occ::handle<IntTools_Context> aContext = new IntTools_Context();
  EXPECT_EQ(aContext->NbPOnSProjectors(), 0);
  for (const TopoDS_Face& aFace : {myLateral, myPlane1, myPlane2})
  {
    aContext->ProjPS(aFace).Perform(gp_Pnt(1., 0.5, 2.5));
  }
  EXPECT_EQ(aContext->NbPOnSProjectors(), 3);

  aContext->ClearPOnSProjectors();
  EXPECT_EQ(aContext->NbPOnSProjectors(), 0);

  GeomAPI_ProjectPointOnSurf& aProj = aContext->ProjPS(myLateral);
  aProj.Perform(gp_Pnt(3., 0., 1.));
  ASSERT_TRUE(aProj.IsDone());
  ASSERT_GT(aProj.NbPoints(), 0);
  EXPECT_NEAR(aProj.LowerDistance(), 1., 1.e-9);
  EXPECT_EQ(aContext->NbPOnSProjectors(), 1);
}

//=================================================================================================

TEST_F(IntTools_ContextTest, PolyhedronCache_IsKeptPerFaceAndReleasedWithTheFace)
{
  occ::handle<IntTools_Context>    aContext = new IntTools_Context();
  IntCurveSurface_PolyhedronCache& aCache   = aContext->PolyhedronCache(myLateral);
  EXPECT_EQ(&aContext->PolyhedronCache(myLateral), &aCache);
  EXPECT_NE(&aContext->PolyhedronCache(myPlane1), &aCache);

  const occ::handle<BRepAdaptor_Surface> aSurf = new BRepAdaptor_Surface(myLateral);
  aCache.Polyhedron(aSurf, 20, 20, 0., 0., 1., 1.);
  EXPECT_EQ(aContext->PolyhedronCache(myLateral).NbPolyhedra(), 1);

  aContext->ClearFaceCache(myLateral);
  EXPECT_EQ(aContext->PolyhedronCache(myLateral).NbPolyhedra(), 0);

  // The polyhedra are released when there are more faces than the limit
  aContext->PolyhedronCache(myLateral).Polyhedron(aSurf, 20, 20, 0., 0., 1., 1.);
  aContext->ClearPOnSProjectors(2);
  EXPECT_EQ(aContext->PolyhedronCache(myLateral).NbPolyhedra(), 1);
  aContext->PolyhedronCache(myPlane2);
  aContext->ClearPOnSProjectors(2);
  EXPECT_EQ(aContext->PolyhedronCache(myLateral).NbPolyhedra(), 0);
}

//=================================================================================================

// The projectors released to be reused, initialized again for other faces, give the same
// results as new projectors
TEST_F(IntTools_ContextTest, ClearPOnSProjectors_ReusedProjectorsGiveSameResults)
{
  BRepBuilderAPI_NurbsConvert aConverter(BRepPrimAPI_MakeCylinder(2., 5.).Shape());
  ASSERT_TRUE(aConverter.IsDone());
  NCollection_DynamicArray<TopoDS_Face> aFaces;
  for (TopExp_Explorer anExp(aConverter.Shape(), TopAbs_FACE); anExp.More(); anExp.Next())
  {
    aFaces.Append(TopoDS::Face(anExp.Current()));
  }
  ASSERT_EQ(aFaces.Length(), 3);

  occ::handle<IntTools_Context> aContext = new IntTools_Context();
  for (int aPass = 0; aPass < 2; ++aPass)
  {
    for (const TopoDS_Face& aFace : aFaces)
    {
      for (const gp_Pnt& aPoint : {gp_Pnt(1., 0.5, 2.5), gp_Pnt(3., -1., 4.), gp_Pnt(0.2, 0.1, 6.)})
      {
        GeomAPI_ProjectPointOnSurf& aProj = aContext->ProjPS(aFace);
        aProj.Perform(aPoint);

        double                     aU1, aU2, aV1, aV2;
        GeomAPI_ProjectPointOnSurf aNew;
        aContext->UVBounds(aFace, aU1, aU2, aV1, aV2);
        aNew.Init(BRep_Tool::Surface(aFace), aU1, aU2, aV1, aV2, 1.e-12);
        aNew.SetExtremaFlag(Extrema_ExtFlag_MIN);
        aNew.Perform(aPoint);

        ASSERT_EQ(aProj.IsDone(), aNew.IsDone());
        ASSERT_EQ(aProj.NbPoints(), aNew.NbPoints());
        if (aNew.NbPoints() > 0)
        {
          EXPECT_EQ(aProj.LowerDistance(), aNew.LowerDistance());
          double aU, aV, aUNew, aVNew;
          aProj.LowerDistanceParameters(aU, aV);
          aNew.LowerDistanceParameters(aUNew, aVNew);
          EXPECT_EQ(aU, aUNew);
          EXPECT_EQ(aV, aVNew);
        }
      }
      // Release the projector to be reused for the next face
      aContext->ClearPOnSProjectors(0, true);
      EXPECT_EQ(aContext->NbPOnSProjectors(), 0);
    }
  }
}
