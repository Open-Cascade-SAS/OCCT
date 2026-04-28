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

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepGProp.hxx>
#include <BRepOffsetAPI_MakePipeShell.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <Geom_Plane.hxx>
#include <GProp_GProps.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <Law_Linear.hxx>
#include <Precision.hxx>
#include <Standard_Handle.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Wire.hxx>
#include <TopExp_Explorer.hxx>

#include <cmath>

namespace
{

//! Computes the total linear length of a wire.
double wireLength(const TopoDS_Wire& theWire)
{
  GProp_GProps aProps;
  BRepGProp::LinearProperties(theWire, aProps);
  return aProps.Mass();
}

//! Builds a bent tube solid (wallSolid) and an inner gas solid (gasSolid).
//! Mirrors the OCC332 Draw command algorithm:
//!   - A circular cross-section profile is swept along an arc spine
//!   - A linear scaling law varies the inner radius from radius_l to radius_r
//!   - The outer radius is offset by wall_thickness throughout
//!
//! @param[in]  theWallThickness  wall thickness of the tube
//! @param[in]  theDia1           inner diameter at start of bend
//! @param[in]  theDia2           inner diameter at end of bend
//! @param[in]  theMajorRadius    radius of the bend arc (spine radius)
//! @param[in]  theLength         arc length along the spine
//! @param[out] theWallSolid      resulting tube wall solid
//! @param[out] theGasSolid       resulting inner gas solid
//! @return false if any construction step fails
bool buildBentTube(double        theWallThickness,
                   double        theDia1,
                   double        theDia2,
                   double        theMajorRadius,
                   double        theLength,
                   TopoDS_Solid& theWallSolid,
                   TopoDS_Shape& theGasSolid)
{
  const double aBendAngle = theLength / theMajorRadius;
  if (aBendAngle >= M_PI)
    return false;

  const double aRadiusL = theDia1 / 2.0;
  const double aRadiusR = theDia2 / 2.0;

  gp_Ax2 anOrigin(gp_Pnt(5000.0, -300.0, 1000.0), gp_Dir(0.0, -1.0, -1.0));

  gp_Pln  aCirc1Plane(anOrigin.Location(), anOrigin.Direction());
  gp_Circ aFaceCircle(anOrigin, aRadiusL);
  gp_Circ anOutFaceCircle(anOrigin, aRadiusL + theWallThickness);

  gp_Pnt aCircCenter = anOrigin.Location().Translated(theMajorRadius * anOrigin.XDirection());
  gp_Ax1 aCircAxis(aCircCenter, anOrigin.YDirection());
  gp_Pln aCirc2Plane = aCirc1Plane.Rotated(aCircAxis, aBendAngle);

  gp_Ax2  aSpineAxis(aCircCenter, anOrigin.YDirection(), -anOrigin.XDirection());
  gp_Circ aSpineCircle(aSpineAxis, theMajorRadius);

  TopoDS_Wire aWire1    = BRepBuilderAPI_MakeWire(BRepBuilderAPI_MakeEdge(aFaceCircle)).Wire();
  TopoDS_Wire anOutWire = BRepBuilderAPI_MakeWire(BRepBuilderAPI_MakeEdge(anOutFaceCircle)).Wire();

  occ::handle<Geom_Curve> aSpineCurve(
    GC_MakeArcOfCircle(aSpineCircle, 0.0, aBendAngle, true).Value());

  occ::handle<Law_Linear> aLaw1 = new Law_Linear();
  occ::handle<Law_Linear> aLaw2 = new Law_Linear();
  aLaw1->Set(aSpineCurve->FirstParameter(), 1.0, aSpineCurve->LastParameter(), aRadiusR / aRadiusL);
  aLaw2->Set(aSpineCurve->FirstParameter(),
             1.0,
             aSpineCurve->LastParameter(),
             (aRadiusR + theWallThickness) / (aRadiusL + theWallThickness));

  BRepBuilderAPI_MakeEdge aMkEdge(aSpineCurve);
  if (!aMkEdge.IsDone())
    return false;
  TopoDS_Wire aSpineWire = BRepBuilderAPI_MakeWire(aMkEdge.Edge()).Wire();

  BRepOffsetAPI_MakePipeShell aMkPipe1(aSpineWire);
  aMkPipe1.SetTolerance(1.0e-8, 1.0e-8, 1.0e-6);
  aMkPipe1.SetLaw(aWire1, aLaw1, false, false);
  aMkPipe1.Build();
  if (!aMkPipe1.IsDone())
    return false;

  BRepOffsetAPI_MakePipeShell aMkPipe2(aSpineWire);
  aMkPipe2.SetTolerance(1.0e-8, 1.0e-8, 1.0e-6);
  aMkPipe2.SetLaw(anOutWire, aLaw2, false, false);
  aMkPipe2.Build();
  if (!aMkPipe2.IsDone())
    return false;

  // Build opening face at start of bend (annular ring)
  BRepBuilderAPI_MakeFace aMkFace;
  occ::handle<Geom_Plane> aPlane1 = new Geom_Plane(aCirc1Plane);
  aMkFace.Init(aPlane1, false, Precision::Confusion());
  aMkFace.Add(TopoDS::Wire(aMkPipe2.FirstShape()));
  aMkFace.Add(TopoDS::Wire(aMkPipe1.FirstShape().Reversed()));
  if (!aMkFace.IsDone())
    return false;
  TopoDS_Face aFace1 = aMkFace.Face();

  // Build opening face at end of bend (annular ring)
  occ::handle<Geom_Plane> aPlane2 = new Geom_Plane(aCirc2Plane);
  aMkFace.Init(aPlane2, false, Precision::Confusion());
  aMkFace.Add(TopoDS::Wire(aMkPipe2.LastShape()));
  aMkFace.Add(TopoDS::Wire(aMkPipe1.LastShape().Reversed()));
  if (!aMkFace.IsDone())
    return false;
  TopoDS_Face aFace2 = aMkFace.Face();

  // Assemble tube shell from inner pipe face, outer pipe face, and two annular ring faces
  TopoDS_Shell aTubeShell;
  BRep_Builder aBuilder;
  aBuilder.MakeShell(aTubeShell);

  TopExp_Explorer aFaceExp(aMkPipe1.Shape(), TopAbs_FACE);
  if (aFaceExp.More())
    aBuilder.Add(aTubeShell, aFaceExp.Current().Reversed());

  // Extract the gas solid before modifying mkPipe1 to a solid
  aMkPipe1.MakeSolid();
  theGasSolid = aMkPipe1.Shape();

  aFaceExp.Init(aMkPipe2.Shape(), TopAbs_FACE);
  if (aFaceExp.More())
    aBuilder.Add(aTubeShell, aFaceExp.Current());

  aBuilder.Add(aTubeShell, aFace1.Reversed());
  aBuilder.Add(aTubeShell, aFace2);
  aTubeShell.Closed(BRep_Tool::IsClosed(aTubeShell));

  aBuilder.MakeSolid(theWallSolid);
  aBuilder.Add(theWallSolid, aTubeShell);

  return true;
}

} // anonymous namespace

//=================================================================================================

// Bug OCC332: BRepOffsetAPI_MakePipeShell with linear scaling law along a bent tube.
// Verifies that:
//  - Both wallSolid and gasSolid are geometrically valid shapes
//  - wallSolid contains 4 circular cross-section wires matching expected circumferences:
//      pi*dia1, pi*(dia1+2*wall), pi*dia2, pi*(dia2+2*wall)
//    within 0.1% tolerance (as validated in the original TCL test).
TEST(BRepOffsetAPI_MakePipeShellTest, Bug332_BentTubeWithScalingLaw)
{
  const double aWallThickness = 10.0;
  const double aDia1          = 30.0;
  const double aDia2          = 50.0;
  const double aMajorRadius   = 100.0;
  const double aLength        = 200.0;

  TopoDS_Solid aWallSolid;
  TopoDS_Shape aGasSolid;
  const bool   aBuilt =
    buildBentTube(aWallThickness, aDia1, aDia2, aMajorRadius, aLength, aWallSolid, aGasSolid);
  ASSERT_TRUE(aBuilt) << "buildBentTube construction failed";

  // Validate both solids
  EXPECT_TRUE(BRepCheck_Analyzer(aWallSolid).IsValid()) << "wallSolid is not valid";
  EXPECT_TRUE(BRepCheck_Analyzer(aGasSolid).IsValid()) << "gasSolid is not valid";

  // Collect all wire lengths in wallSolid
  NCollection_Sequence<double> aWireLengths;
  for (TopExp_Explorer anExp(aWallSolid, TopAbs_WIRE); anExp.More(); anExp.Next())
  {
    const double aLen = wireLength(TopoDS::Wire(anExp.Current()));
    if (aLen > Precision::Confusion())
      aWireLengths.Append(aLen);
  }

  // Expected circular circumferences from the TCL test (pi * diameter):
  //   Wire4 in TCL: pi * dia1                       = pi * 30 (inner start, radius=15)
  //   Wire3 in TCL: pi * (dia1 + 2*wall)            = pi * 50 (outer start, radius=25)
  //   Wire6 in TCL: pi * dia2                       = pi * 50 (inner end, radius=25)
  //   Wire5 in TCL: pi * (dia2 + 2*wall)            = pi * 70 (outer end, radius=35)
  const double aPi      = M_PI;
  const double aExpLen1 = aPi * aDia1;                          // pi*30  ~ 94.25
  const double aExpLen2 = aPi * (aDia1 + 2.0 * aWallThickness); // pi*50  ~ 157.08
  // aExpLen3 = pi*dia2 = pi*50, which equals aExpLen2 when dia2/2 == dia1/2 + wall
  const double aExpLen4 = aPi * (aDia2 + 2.0 * aWallThickness); // pi*70  ~ 219.91

  const double aTol = 0.001; // 0.1% relative tolerance

  auto countMatching = [&](double theExpected) -> int {
    int aCount = 0;
    for (const double aLen : aWireLengths)
    {
      if (std::abs(aLen - theExpected) / theExpected <= aTol)
        ++aCount;
    }
    return aCount;
  };

  EXPECT_GE(countMatching(aExpLen1), 1)
    << "No wire found with circumference pi*dia1 = " << aExpLen1;
  // aExpLen2 and aExpLen3 are identical (dia2/2 == dia1/2 + wall), so expect at least 2 matches
  EXPECT_GE(countMatching(aExpLen2), 2)
    << "Expected 2 wires with circumference pi*50 = " << aExpLen2;
  EXPECT_GE(countMatching(aExpLen4), 1)
    << "No wire found with circumference pi*(dia2+2*wall) = " << aExpLen4;
}
