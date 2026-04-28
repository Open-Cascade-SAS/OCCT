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
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepGProp.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <GProp_GProps.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Solid.hxx>

//! Runs OCC817 test: creates hollow box (30x30x30 outer minus 10x10x10 inner),
//! grids the bounding box into cells of theMeshDelta size, computes
//! BRepAlgoAPI_Common of the solid with each cell, and returns
//! the ratio of accumulated meshed volume to original volume.
//! Returns -1.0 on failure.
static double runHollowBoxMeshTest(double theMeshDelta)
{
  constexpr double aDelt = 5.0 * Precision::Confusion();

  // Create outer box solid
  const TopoDS_Solid aFullSolid = BRepPrimAPI_MakeBox(gp_Pnt(0, 0, 0), 30.0, 30.0, 30.0).Solid();

  // Create inner box solid
  const TopoDS_Solid anInnerSolid =
    BRepPrimAPI_MakeBox(gp_Pnt(10, 10, 10), 10.0, 10.0, 10.0).Solid();

  // Cut inner from outer
  BRepAlgoAPI_Cut aCut(aFullSolid, anInnerSolid);
  if (!aCut.IsDone())
    return -1.0;

  // Extract the single solid from the cut result
  TopoDS_Solid    aCutSolid;
  int             aNbSolids = 0;
  TopExp_Explorer anExp;
  for (anExp.Init(aCut.Shape(), TopAbs_SOLID); anExp.More(); anExp.Next())
  {
    const TopoDS_Solid& aSol = TopoDS::Solid(anExp.Current());
    if (!aSol.IsNull())
    {
      aCutSolid = aSol;
      aNbSolids++;
    }
  }
  if (aNbSolids != 1)
    return -1.0;

  // Calculate original volume
  GProp_GProps aVProps;
  BRepGProp::VolumeProperties(aCutSolid, aVProps);
  const double anOriginalVolume = aVProps.Mass();
  if (anOriginalVolume <= 0.0)
    return -1.0;

  // Build bounding box and extend by small delta
  Bnd_Box aBndBox;
  BRepBndLib::Add(aCutSolid, aBndBox);
  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBndBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
  aXmin -= aDelt;
  aYmin -= aDelt;
  aZmin -= aDelt;
  aXmax += aDelt;
  aYmax += aDelt;
  aZmax += aDelt;

  // Grid the bounding box
  int aNx = (int)((aXmax - aXmin) / theMeshDelta);
  if (aNx <= 0)
    aNx = 1;
  int aNy = (int)((aYmax - aYmin) / theMeshDelta);
  if (aNy <= 0)
    aNy = 1;
  int aNz = (int)((aZmax - aZmin) / theMeshDelta);
  if (aNz <= 0)
    aNz = 1;

  const double aStepX     = (aXmax - aXmin) / aNx;
  const double aStepY     = (aYmax - aYmin) / aNy;
  const double aStepZ     = (aZmax - aZmin) / aNz;
  const int    aNbSubvols = aNx * aNy * aNz;

  NCollection_Array1<TopoDS_Shape> aSubvols(0, aNbSubvols - 1);
  NCollection_Array1<double>       aSubvolVols(0, aNbSubvols - 1);

  // Build grid cells
  int l = 0;
  for (int i = 0; i < aNx; i++)
  {
    for (int j = 0; j < aNy; j++)
    {
      for (int k = 0; k < aNz; k++)
      {
        const gp_Pnt aPnt(aXmin + i * aStepX, aYmin + j * aStepY, aZmin + k * aStepZ);
        aSubvols.SetValue(l, BRepPrimAPI_MakeBox(aPnt, aStepX, aStepY, aStepZ).Solid());
        BRepGProp::VolumeProperties(aSubvols(l), aVProps);
        aSubvolVols.SetValue(l, aVProps.Mass());
        l++;
      }
    }
  }

  // Compute common of solid with each grid cell, accumulate volumes
  double anAccumulated = 0.0;
  for (l = 0; l < aNbSubvols; l++)
  {
    const TopoDS_Shape aCopySolid = BRepBuilderAPI_Copy(aCutSolid).Shape();
    BRepAlgoAPI_Common aCommon(aCopySolid, aSubvols(l));
    if (!aCommon.IsDone())
      continue;

    int          aNbCommon = 0;
    TopoDS_Shape aFoundSolid;
    for (anExp.Init(aCommon.Shape(), TopAbs_SOLID); anExp.More(); anExp.Next())
    {
      const TopoDS_Solid& aSol = TopoDS::Solid(anExp.Current());
      if (!aSol.IsNull())
      {
        aFoundSolid = aSol;
        aNbCommon++;
      }
    }
    if (aNbCommon == 1)
    {
      BRepGProp::VolumeProperties(aFoundSolid, aVProps);
      const double aVol = aVProps.Mass();
      if (aVol > 0.0 && aVol <= aSubvolVols(l))
        anAccumulated += aVol;
    }
  }

  return anAccumulated / anOriginalVolume;
}

// OCC817: BRepAlgoAPI_Common result accuracy with hollow box using mesh_delta=10.
TEST(BRepAlgoAPI_CutTest, HollowBox_VolumeAccuracy_Delta10)
{
  const double aRatio = runHollowBoxMeshTest(10.0);
  ASSERT_GE(aRatio, 0.0) << "Cut or Common operation failed";
  EXPECT_NEAR(aRatio, 1.0, 0.001) << "Accumulated meshed volume differs from original by > 0.1%";
}

// OCC817: BRepAlgoAPI_Common result accuracy with hollow box using mesh_delta=15.
TEST(BRepAlgoAPI_CutTest, HollowBox_VolumeAccuracy_Delta15)
{
  const double aRatio = runHollowBoxMeshTest(15.0);
  ASSERT_GE(aRatio, 0.0) << "Cut or Common operation failed";
  EXPECT_NEAR(aRatio, 1.0, 0.001) << "Accumulated meshed volume differs from original by > 0.1%";
}

// OCC817: BRepAlgoAPI_Common result accuracy with hollow box using mesh_delta=30.
TEST(BRepAlgoAPI_CutTest, HollowBox_VolumeAccuracy_Delta30)
{
  const double aRatio = runHollowBoxMeshTest(30.0);
  ASSERT_GE(aRatio, 0.0) << "Cut or Common operation failed";
  EXPECT_NEAR(aRatio, 1.0, 0.001) << "Accumulated meshed volume differs from original by > 0.1%";
}

// OCC817: hollow box has correct surface area and valid shape.
// Outer 30x30x30 has area 6*900=5400; inner 10x10x10 hole adds 6*100=600; total=6000.
TEST(BRepAlgoAPI_CutTest, HollowBox_SurfaceAreaAndValidity)
{
  const TopoDS_Solid aFullSolid = BRepPrimAPI_MakeBox(gp_Pnt(0, 0, 0), 30.0, 30.0, 30.0).Solid();
  const TopoDS_Solid anInnerSolid =
    BRepPrimAPI_MakeBox(gp_Pnt(10, 10, 10), 10.0, 10.0, 10.0).Solid();

  BRepAlgoAPI_Cut aCut(aFullSolid, anInnerSolid);
  ASSERT_TRUE(aCut.IsDone());

  // Extract solid
  TopoDS_Solid    aCutSolid;
  TopExp_Explorer anExp;
  for (anExp.Init(aCut.Shape(), TopAbs_SOLID); anExp.More(); anExp.Next())
    aCutSolid = TopoDS::Solid(anExp.Current());
  ASSERT_FALSE(aCutSolid.IsNull());

  // Surface area: outer box 6*900=5400, inner box adds 6*100=600 => total=6000
  GProp_GProps aSProps;
  BRepGProp::SurfaceProperties(aCutSolid, aSProps);
  EXPECT_NEAR(aSProps.Mass(), 6000.0, 6.0); // 0.1% tolerance

  BRepCheck_Analyzer aChecker(aCutSolid);
  EXPECT_TRUE(aChecker.IsValid());
}
