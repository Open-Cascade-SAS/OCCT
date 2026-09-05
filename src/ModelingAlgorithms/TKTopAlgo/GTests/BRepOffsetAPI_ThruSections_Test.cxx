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

#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepGProp.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <BRep_Tool.hxx>
#include <BSplCLib.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Surface.hxx>
#include <GeomConvert.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GProp_GProps.hxx>
#include <gce_MakeCirc.hxx>
#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pln.hxx>
#include <gp_Trsf.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_List.hxx>
#include <Precision.hxx>
#include <Standard_Integer.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ShapeMapHasher.hxx>

// Test OCC10006: BRepOffsetAPI_ThruSections loft operation with Boolean fusion
TEST(BRepOffsetAPI_ThruSections_Test, OCC10006_LoftAndFusion)
{
  // Define bottom and top polygon coordinates for first loft
  double aBottomPoints1[12] = {10, -10, 0, 100, -10, 0, 100, -100, 0, 10, -100, 0};
  double aTopPoints1[12]    = {0, 0, 10, 100, 0, 10, 100, -100, 10, 0, -100, 10};

  // Define bottom and top polygon coordinates for second loft
  double aBottomPoints2[12] = {0, 0, 10.00, 100, 0, 10.00, 100, -100, 10.00, 0, -100, 10.00};
  double aTopPoints2[12]    = {0, 0, 250, 100, 0, 250, 100, -100, 250, 0, -100, 250};

  // Create polygons
  BRepBuilderAPI_MakePolygon aBottomPolygon1, aTopPolygon1, aBottomPolygon2, aTopPolygon2;
  gp_Pnt                     aTmpPnt;

  for (int i = 0; i < 4; i++)
  {
    aTmpPnt.SetCoord(aBottomPoints1[3 * i], aBottomPoints1[3 * i + 1], aBottomPoints1[3 * i + 2]);
    aBottomPolygon1.Add(aTmpPnt);

    aTmpPnt.SetCoord(aTopPoints1[3 * i], aTopPoints1[3 * i + 1], aTopPoints1[3 * i + 2]);
    aTopPolygon1.Add(aTmpPnt);

    aTmpPnt.SetCoord(aBottomPoints2[3 * i], aBottomPoints2[3 * i + 1], aBottomPoints2[3 * i + 2]);
    aBottomPolygon2.Add(aTmpPnt);

    aTmpPnt.SetCoord(aTopPoints2[3 * i], aTopPoints2[3 * i + 1], aTopPoints2[3 * i + 2]);
    aTopPolygon2.Add(aTmpPnt);
  }

  // Close polygons
  aBottomPolygon1.Close();
  aTopPolygon1.Close();
  aBottomPolygon2.Close();
  aTopPolygon2.Close();

  // Create first loft (ThruSections)
  BRepOffsetAPI_ThruSections aLoft1(true, true);
  aLoft1.AddWire(aBottomPolygon1.Wire());
  aLoft1.AddWire(aTopPolygon1.Wire());
  aLoft1.Build();

  // Create second loft (ThruSections)
  BRepOffsetAPI_ThruSections aLoft2(true, true);
  aLoft2.AddWire(aBottomPolygon2.Wire());
  aLoft2.AddWire(aTopPolygon2.Wire());
  aLoft2.Build();

  // Verify that loft operations succeeded
  EXPECT_FALSE(aLoft1.Shape().IsNull()) << "First loft operation should produce a valid shape";
  EXPECT_FALSE(aLoft2.Shape().IsNull()) << "Second loft operation should produce a valid shape";

  // Perform Boolean fusion of the two lofted shapes
  BRepAlgoAPI_Fuse aFusion(aLoft1.Shape(), aLoft2.Shape());

  // Verify that fusion operation succeeded
  EXPECT_FALSE(aFusion.Shape().IsNull())
    << "Boolean fusion of lofted shapes should produce a valid shape";
}

namespace
{
//! Helper function to create a B-spline curve from poles and knot sequence.
//! @param thePoles Array of 3D pole coordinates (x1,y1,z1, x2,y2,z2, ...)
//! @param theKnots Knot sequence for cubic B-spline
//! @return Handle to the created B-spline curve
occ::handle<Geom_BSplineCurve> createBSplineCurve(const std::vector<double>& thePoles,
                                                  const std::vector<double>& theKnots)
{
  const int aKnotSeqSize = static_cast<int>(theKnots.size());
  const int aDegree      = 3; // cubic spline
  const int aNbPoles     = aKnotSeqSize - aDegree - 1;

  NCollection_Array1<gp_Pnt> aPoles(1, aNbPoles);
  int                        anIdx = 0;
  for (int i = 1; i <= aNbPoles; ++i)
  {
    aPoles(i) = gp_Pnt(thePoles[anIdx], thePoles[anIdx + 1], thePoles[anIdx + 2]);
    anIdx += 3;
  }

  NCollection_Array1<double> aKnotSeq(1, aKnotSeqSize);
  for (int i = 1; i <= aKnotSeqSize; ++i)
  {
    aKnotSeq(i) = theKnots[i - 1];
  }

  NCollection_Array1<double> aKnots(1, BSplCLib::KnotsLength(aKnotSeq, false));
  NCollection_Array1<int>    aMults(1, aKnots.Upper());
  BSplCLib::Knots(aKnotSeq, aKnots, aMults);

  return new Geom_BSplineCurve(aPoles, aKnots, aMults, aDegree, false);
}
} // namespace

// Test case: ThruSections with B-spline profiles having different numbers of poles.
// This tests the scenario where closed B-spline curves with varying control point counts
// are used as sections for lofting. The algorithm should handle profiles with different
// numbers of edges by making them compatible.
TEST(BRepOffsetAPI_ThruSections_Test, BSplineProfilesWithDifferentPoleCount)
{
  // Section 1: 31 poles at Z=0 (closed curve - first pole equals last pole)
  // clang-format off
  std::vector<double> aPoles1 = {
    0.90194,-0.49457,0, 1.10106097848166,-0.688963767263419,0,
    1.04668209568152,-1.01716787534971,0, 1.56914438377061,-1.59514757777645,0,
    2.07302273729763,-3.97023193652984,0, 2.13699279206564,-4.57936580650492,0,
    2.13184140893145,-7.00174300027487,0, 1.23454863419269,-8.49296048646617,0,
    0.718149935438171,-9.43440624002067,0, -2.02215956293932,-12.0782514208969,0,
    -3.44515644568264,-13.0526565862391,0, -8.61587484541011,-15.7788974963508,0,
    -12.8309565197945,-17.0127003407937,0, -18.9761857527559,-18.432946752008,0,
    -20.6360623744536,-18.7727324951302,0, -21.2130359371451,-19.0009746915956,0,
    -22.2663547655952,-18.9176136571682,0, -22.1384066610996,-17.5929745686915,0,
    -21.7954793416826,-18.0629411635879,0, -14.4780597089423,-15.0594484542493,0,
    -10.8386889762912,-13.1323128331196,0, -6.48208180266811,-9.4594741816569,0,
    -5.28863405147604,-8.21363647074186,0, -2.96204585602755,-5.08386133056186,0,
    -2.54058250866771,-4.20341306384795,0, -1.61736353367643,-2.66854172662003,0,
    -1.36012191071392,-0.569755235503059,0, -1.02752582768388,-0.306835878255118,0,
    0.166466683785465,0.162094918019513,0, 0.0600379384518613,0.327344971788155,0,
    0.90194,-0.49457,0
  };
  std::vector<double> aKnots1 = {
    0,0,0,0, 0.0161348522840521, 0.0807608132035811, 0.131338840314679,
    0.147844505833154, 0.16533568293713, 0.250607290759724, 0.267449419858552,
    0.322628298230969, 0.341027430676258, 0.36078046495376, 0.420324490838906,
    0.440253145913864, 0.458870520105636, 0.492435831240754, 0.509078502457839,
    0.525715578369451, 0.577446072383669, 0.597241246236863, 0.656453700919606,
    0.676155157762965, 0.694541842118733, 0.749834839741989, 0.766726765513781,
    0.834662561102041, 0.868900689429001, 0.88456999851039, 0.931780340251946,
    1,1,1,1
  };
  // clang-format on

  // Section 2: 31 poles at Z=14
  // clang-format off
  std::vector<double> aPoles2 = {
    1.01818,-0.45183,14, 1.23027550376089,-0.635831979821499,14,
    1.22298178591092,-0.993247942942908,14, 1.9247594565888,-1.68138513003473,14,
    2.78453675046566,-4.24062036175799,14, 2.92887039780498,-4.88871135922063,14,
    3.21530512178939,-7.44536648753893,14, 2.43464600049467,-9.01717535389148,14,
    1.99774717699481,-10.0099731707865,14, -0.580430194841712,-12.811667114046,14,
    -1.96280131040106,-13.8497247564759,14, -7.0770142802295,-16.7706677743417,14,
    -11.3526244425869,-18.1132238752247,14, -17.6322830947984,-19.667828036594,14,
    -19.3359506088904,-20.0397164345371,14, -19.9363407564968,-20.2618454245171,14,
    -20.9958867740652,-20.2431659322596,14, -20.9472778652509,-18.9170304109807,14,
    -20.5705540974871,-19.3684485145849,14, -13.1692875695181,-16.2161494354514,14,
    -9.51921634418429,-14.2082484638036,14, -5.35937404690226,-10.3089201842049,14,
    -4.47798441471647,-9.23875034192191,14, -3.21879123246707,-7.6110600629437,14,
    -2.18689883513949,-5.31367859264728,14, -1.97348761746698,-4.58783119756755,14,
    -1.33574873445926,-3.01168469104978,14, -1.71865526796731,-0.0846344433001956,14,
    0.269132653252561,0.122297343391581,14, 0.122686280710628,0.325049350788693,14,
    1.01818,-0.45183,14
  };
  std::vector<double> aKnots2 = {
    0,0,0,0, 0.0161348522840521, 0.0807608132035811, 0.131338840314679,
    0.147844505833154, 0.16533568293713, 0.250607290759724, 0.267449419858552,
    0.322628298230969, 0.341027430676258, 0.36078046495376, 0.420324490838906,
    0.440253145913864, 0.458870520105636, 0.492435831240754, 0.509078502457839,
    0.525715578369451, 0.577446072383669, 0.597241246236863, 0.656453700919606,
    0.676155157762965, 0.694541842118733, 0.749834839741989, 0.766726765513781,
    0.834662561102041, 0.868900689429001, 0.88456999851039, 0.931780340251946,
    1,1,1,1
  };
  // clang-format on

  // Section 3: 31 poles at Z=28
  // clang-format off
  std::vector<double> aPoles3 = {
    1.1315,-0.412,28, 1.35620967405692,-0.586767016287049,28,
    1.40346173744982,-0.904848944467879,28, 2.42203198448737,-2.18621379588629,28,
    3.32033694639709,-3.92656269181969,28, 3.51212155543691,-4.32855460347163,28,
    4.24437254399706,-7.7411305703627,28, 3.58837706744425,-9.38948673956861,28,
    3.23381768225783,-10.4288340403835,28, 0.827297282546986,-13.3806847722879,28,
    -0.511092431459457,-14.4804531232202,28, -5.56121110916677,-17.5918940081576,28,
    -9.89839343105239,-19.0417230070618,28, -16.3191628184718,-20.7281818875723,28,
    -18.069202037559,-21.1317454056832,28, -18.6952658436501,-21.3501485512683,28,
    -19.7594163372563,-21.3890021828607,28, -19.7825452812777,-20.0662589434646,28,
    -19.3742511072047,-20.4997648812408,28, -11.8843871512175,-17.1947066102013,28,
    -8.22814484532529,-15.0978242272864,28, -4.27266763395487,-10.9862396235634,28,
    -3.46908164394296,-9.8527181618549,28, -2.33410525489297,-8.12592678971571,28,
    -1.5676761966055,-5.65380588039358,28, -1.45500694833621,-4.86219190572022,28,
    -1.20491779902222,-2.99088321180665,28, -1.71473210788581,-0.11550194997251,28,
    0.339193684817586,0.130842094688643,28, 0.185038304272247,0.324106655338832,28,
    1.1315,-0.411999999999999,28
  };
  std::vector<double> aKnots3 = {
    0,0,0,0, 0.0161348522840521, 0.0807608132035811, 0.131338840314679,
    0.147844505833154, 0.16533568293713, 0.250607290759724, 0.267449419858552,
    0.322628298230969, 0.341027430676258, 0.36078046495376, 0.420324490838906,
    0.440253145913864, 0.458870520105636, 0.492435831240754, 0.509078502457839,
    0.525715578369451, 0.577446072383669, 0.597241246236863, 0.656453700919606,
    0.676155157762965, 0.694541842118733, 0.749834839741989, 0.766726765513781,
    0.834662561102041, 0.868900689429001, 0.88456999851039, 0.931780340251946,
    1,1,1,1
  };
  // clang-format on

  // Section 4: 31 poles at Z=42
  // clang-format off
  std::vector<double> aPoles4 = {
    1.23665,-0.38035,42, 1.47168196581999,-0.547085468398438,42,
    1.59588093039332,-0.976910285968177,42, 2.37359942123034,-1.62852397162461,42,
    3.88462097653375,-4.06747989117973,42, 4.08076351079016,-4.41252552269171,42,
    5.13530795520565,-7.91791038549566,42, 4.5942068788067,-9.62828143393584,42,
    4.31440651520895,-10.7043545807689,42, 2.06395892933396,-13.7788094086647,42,
    0.765564553906658,-14.9299915868171,42, -4.22675671524901,-18.2024292050444,42,
    -8.62128212326746,-19.7458598662774,42, -15.1728828553279,-21.5499125868338,42,
    -16.9659291749178,-21.9822055955299,42, -17.6161839495155,-22.1994311185674,42,
    -18.6836955455801,-22.2848113284216,42, -18.7654460877162,-20.9681980423141,42,
    -18.33029903925,-21.3862202927921,42, -10.7621667068909,-17.9386762111952,42,
    -7.10729220900608,-15.7553189784139,42, -3.34515330946479,-11.4622305098615,42,
    -2.61347538922171,-10.2767882581575,42, -1.59293712529107,-8.46932612376041,42,
    -1.06722790049871,-5.86343234527305,42, -1.05069435531815,-5.16187378699568,42,
    -0.946257221489481,-3.39477994610666,42, -1.94338925491643,-0.26811859733049,42,
    0.402165057047551,0.139888420742186,42, 0.248044898503808,0.320982408483093,42,
    1.23665,-0.380349999999999,42
  };
  std::vector<double> aKnots4 = {
    0,0,0,0, 0.0161348522840521, 0.0807608132035811, 0.131338840314679,
    0.147844505833154, 0.16533568293713, 0.250607290759724, 0.267449419858552,
    0.322628298230969, 0.341027430676258, 0.36078046495376, 0.420324490838906,
    0.440253145913864, 0.458870520105636, 0.492435831240754, 0.509078502457839,
    0.525715578369451, 0.577446072383669, 0.597241246236863, 0.656453700919606,
    0.676155157762965, 0.694541842118733, 0.749834839741989, 0.766726765513781,
    0.834662561102041, 0.868900689429001, 0.88456999851039, 0.931780340251946,
    1,1,1,1
  };
  // clang-format on

  // Section 5: 33 poles at Z=56 (different number of poles!)
  // clang-format off
  std::vector<double> aPoles5 = {
    1.38958,-0.30093,56, 2.43615912287256,-0.909728724567382,56,
    3.0281955825442,-1.8572582903678,56, 3.65762773796813,-2.54529310782012,56,
    4.96146733383277,-4.12983817908431,56, 5.23021328351262,-4.48294463637656,56,
    6.97411718250412,-8.12140889198172,56, 6.70106429787142,-9.9518367568647,56.0000000000001,
    6.61799594372847,-10.831499083977,56, 5.83042424901388,-13.1305423440654,56,
    4.4856942305868,-14.6753503685142,56, 3.526218495628,-15.6885280970105,56,
    -1.30359786741575,-19.3108973511269,56, -5.83186375110243,-21.0521342727528,56,
    -12.7369662411789,-23.0728272909214,56, -14.6499746822398,-23.5541553891022,56,
    -15.5393756779425,-23.8164232092995,56, -16.7857626486778,-22.971990212193,56,
    -15.8148711149702,-22.7381053237965,56, -14.1582028215773,-22.0884170074466,56,
    -8.2848005862047,-19.3490481837665,56, -4.53645198453342,-17.0504864772276,56,
    -1.10904288834323,-12.471539082827,56, -0.522426290934783,-11.1995642443973,56,
    0.262420398047911,-9.25463182654568,56, 0.213867071381995,-6.39113351333029,56,
    0.0369897362998156,-5.60398557321822,56, -0.344006372425831,-3.85342683471391,56,
    -1.68576438677131,-1.32767756558911,56, -1.05109637156753,-0.635077640089276,56,
    0.456139976122501,0.188083742774127,56, 0.318806380192129,0.321942747786321,56,
    1.38958,-0.30093,56
  };
  std::vector<double> aKnots5 = {
    0,0,0,0, 0.0668554777677272, 0.0834666756318599, 0.100984720713562,
    0.151909931618998, 0.169593414001346, 0.254704249388902, 0.27136091562454,
    0.289520682572814, 0.325734361487981, 0.343874098775056, 0.363388151948866,
    0.422534909356546, 0.442420378784176, 0.461023580422867, 0.494592323781788,
    0.527437648618005, 0.560332403705056, 0.578743404285102, 0.598421903816054,
    0.65700259163225, 0.676406731087284, 0.694519638867019, 0.730942734487404,
    0.749396334888904, 0.766418578227312, 0.818448937788905, 0.836068871481316,
    0.8848958398016, 0.931598977689477, 1,1,1,1
  };
  // clang-format on

  // Create wires from B-spline curves
  std::vector<std::pair<std::vector<double>, std::vector<double>>> aSections = {{aPoles1, aKnots1},
                                                                                {aPoles2, aKnots2},
                                                                                {aPoles3, aKnots3},
                                                                                {aPoles4, aKnots4},
                                                                                {aPoles5, aKnots5}};

  BRepOffsetAPI_ThruSections aThruSections(true, false, 1.0e-06);

  for (const auto& aSection : aSections)
  {
    occ::handle<Geom_BSplineCurve> aCurve = createBSplineCurve(aSection.first, aSection.second);
    ASSERT_FALSE(aCurve.IsNull()) << "Failed to create B-spline curve";

    BRepBuilderAPI_MakeEdge aMakeEdge(aCurve);
    ASSERT_TRUE(aMakeEdge.IsDone()) << "Failed to create edge from B-spline curve";

    BRepBuilderAPI_MakeWire aMakeWire(aMakeEdge.Edge());
    ASSERT_TRUE(aMakeWire.IsDone()) << "Failed to create wire from edge";

    aThruSections.AddWire(aMakeWire.Wire());
  }

  // Build should not throw - this is the regression test
  ASSERT_NO_THROW(aThruSections.Build())
    << "ThruSections should not throw 'profiles are inconsistent' for valid B-spline profiles";

  // Verify the result
  EXPECT_TRUE(aThruSections.IsDone())
    << "ThruSections should complete successfully for closed B-spline profiles "
       "with different pole counts";

  if (aThruSections.IsDone())
  {
    EXPECT_FALSE(aThruSections.Shape().IsNull()) << "ThruSections should produce a valid shape";
  }
}

// Test OCC895: BRepOffsetAPI_ThruSections with two circular arc wires.
// Regression test for incorrect computation of mutual orientations of wire segments
// that caused a twisted surface to be created.
TEST(BRepOffsetAPI_ThruSections_Test, OCC895_TwoCircularArcWires_NoTwist)
{
  // Build the two wires using circular arcs at different positions,
  // reproducing exactly the OCC895 DRAW command with angle=5, reverse=0, order=0.
  const double aRad   = 1.0;
  const double aAngle = 5.0 * M_PI / 180.0;

  // Wire 1: arc from a circle whose axis is rotated 5 degrees around Z
  gp_Pnt aCenter1(0, 10, 0);
  gp_Ax2 aAxis1(aCenter1, -gp::DY(), gp::DX());
  aAxis1.Rotate(gp_Ax1(aCenter1, gp::DZ()), aAngle);

  gce_MakeCirc aMakeCirc1(aAxis1, aRad);
  ASSERT_TRUE(aMakeCirc1.IsDone());
  GC_MakeArcOfCircle aMakeArc1(aMakeCirc1.Value(), 0, M_PI / 2, true);
  ASSERT_TRUE(aMakeArc1.IsDone());
  const occ::handle<Geom_TrimmedCurve>& aArc1 = aMakeArc1.Value();

  BRepBuilderAPI_MakeEdge aMakeEdge1(aArc1, aArc1->StartPoint(), aArc1->EndPoint());
  ASSERT_TRUE(aMakeEdge1.IsDone());
  BRepBuilderAPI_MakeWire aMakeWire1(aMakeEdge1.Edge());
  ASSERT_TRUE(aMakeWire1.IsDone());
  const TopoDS_Wire& aWire1 = aMakeWire1.Wire();

  // Wire 2: arc from a circle at a different center with fixed axis
  gp_Pnt aCenter2(10, 0, 0);
  gp_Ax2 aAxis2(aCenter2, -gp::DX(), gp::DZ());

  gce_MakeCirc aMakeCirc2(aAxis2, aRad);
  ASSERT_TRUE(aMakeCirc2.IsDone());
  GC_MakeArcOfCircle aMakeArc2(aMakeCirc2.Value(), 0, M_PI / 2, true);
  ASSERT_TRUE(aMakeArc2.IsDone());
  const occ::handle<Geom_TrimmedCurve>& aArc2 = aMakeArc2.Value();

  BRepBuilderAPI_MakeEdge aMakeEdge2(aArc2, aArc2->StartPoint(), aArc2->EndPoint());
  ASSERT_TRUE(aMakeEdge2.IsDone());
  BRepBuilderAPI_MakeWire aMakeWire2(aMakeEdge2.Edge());
  ASSERT_TRUE(aMakeWire2.IsDone());
  const TopoDS_Wire& aWire2 = aMakeWire2.Wire();

  // Build ThruSections shell with order=0: wire2 first, then wire1
  BRepOffsetAPI_ThruSections aThruSect(false, true);
  aThruSect.AddWire(aWire2);
  aThruSect.AddWire(aWire1);
  aThruSect.Build();

  ASSERT_TRUE(aThruSect.IsDone()) << "ThruSections must succeed";
  ASSERT_FALSE(aThruSect.Shape().IsNull()) << "ThruSections must produce a non-null shape";

  // Verify surface area is approximately 18.1614 (reference value from DRAW test)
  GProp_GProps aProps;
  BRepGProp::SurfaceProperties(aThruSect.Shape(), aProps);
  EXPECT_NEAR(aProps.Mass(), 18.1614, 0.01) << "Surface area should be approximately 18.1614";
}

namespace
{
TopoDS_Wire makeCircleWire(const gp_Pnt& theCenter, const double theRadius)
{
  BRepBuilderAPI_MakeEdge anEdge(gp_Circ(gp_Ax2(theCenter, gp::DZ()), theRadius));
  return BRepBuilderAPI_MakeWire(anEdge.Edge()).Wire();
}

TopoDS_Wire makeRationalCircleWire(const gp_Pnt& theCenter, const double theRadius)
{
  const occ::handle<Geom_Circle> aCircle = new Geom_Circle(gp_Ax2(theCenter, gp::DZ()), theRadius);
  const occ::handle<Geom_BSplineCurve> aBSpline =
    GeomConvert::CurveToBSplineCurve(aCircle, Convert_TgtThetaOver2);
  return BRepBuilderAPI_MakeWire(BRepBuilderAPI_MakeEdge(aBSpline).Edge()).Wire();
}

TopoDS_Wire makeRectangleWire(const double theHalfExtent, const double theZ)
{
  BRepBuilderAPI_MakePolygon aPolygon;
  aPolygon.Add(gp_Pnt(-theHalfExtent, -theHalfExtent, theZ));
  aPolygon.Add(gp_Pnt(theHalfExtent, -theHalfExtent, theZ));
  aPolygon.Add(gp_Pnt(theHalfExtent, theHalfExtent, theZ));
  aPolygon.Add(gp_Pnt(-theHalfExtent, theHalfExtent, theZ));
  aPolygon.Close();
  return aPolygon.Wire();
}

TopoDS_Wire makeSplitRectangleWire(const double theHalfExtent, const double theZ)
{
  BRepBuilderAPI_MakePolygon aPolygon;
  aPolygon.Add(gp_Pnt(-theHalfExtent, -theHalfExtent, theZ));
  aPolygon.Add(gp_Pnt(0.0, -theHalfExtent, theZ));
  aPolygon.Add(gp_Pnt(theHalfExtent, -theHalfExtent, theZ));
  aPolygon.Add(gp_Pnt(theHalfExtent, 0.0, theZ));
  aPolygon.Add(gp_Pnt(theHalfExtent, theHalfExtent, theZ));
  aPolygon.Add(gp_Pnt(0.0, theHalfExtent, theZ));
  aPolygon.Add(gp_Pnt(-theHalfExtent, theHalfExtent, theZ));
  aPolygon.Add(gp_Pnt(-theHalfExtent, 0.0, theZ));
  aPolygon.Close();
  return aPolygon.Wire();
}

occ::handle<Geom_Surface> firstLoftSurface(const TopoDS_Shape& theShape,
                                           TopLoc_Location&    theLocation)
{
  TopExp_Explorer anExplorer(theShape, TopAbs_FACE);
  if (!anExplorer.More())
  {
    return nullptr;
  }
  return BRep_Tool::Surface(TopoDS::Face(anExplorer.Current()), theLocation);
}

gp_Vec loftEndpointDerivative(const TopoDS_Shape& theShape, const bool theFirst)
{
  TopLoc_Location                 aLocation;
  const occ::handle<Geom_Surface> aSurface = firstLoftSurface(theShape, aLocation);
  if (aSurface.IsNull())
  {
    return gp_Vec();
  }
  double aUFirst, aULast, aVFirst, aVLast;
  aSurface->Bounds(aUFirst, aULast, aVFirst, aVLast);
  gp_Pnt aPoint;
  gp_Vec aDU, aDV;
  aSurface->D1(0.37 * aUFirst + 0.63 * aULast, theFirst ? aVFirst : aVLast, aPoint, aDU, aDV);
  aDV.Transform(aLocation.Transformation());
  return aDV;
}

double loftEndpointNormalCurvature(const TopoDS_Shape& theShape,
                                   const bool          theFirst,
                                   const double        theUFraction = 0.63)
{
  TopLoc_Location                 aLocation;
  const occ::handle<Geom_Surface> aSurface = firstLoftSurface(theShape, aLocation);
  if (aSurface.IsNull())
  {
    return RealLast();
  }
  double aUFirst, aULast, aVFirst, aVLast;
  aSurface->Bounds(aUFirst, aULast, aVFirst, aVLast);
  gp_Pnt aPoint;
  gp_Vec aDU, aDV, aDUU, aDVV, aDUV;
  aSurface->D2((1.0 - theUFraction) * aUFirst + theUFraction * aULast,
               theFirst ? aVFirst : aVLast,
               aPoint,
               aDU,
               aDV,
               aDUU,
               aDVV,
               aDUV);
  aDU.Transform(aLocation.Transformation());
  aDV.Transform(aLocation.Transformation());
  aDVV.Transform(aLocation.Transformation());
  gp_Vec aNormal = aDU.Crossed(aDV);
  if (aNormal.SquareMagnitude() <= gp::Resolution() || aDV.SquareMagnitude() <= gp::Resolution())
  {
    return RealLast();
  }
  aNormal.Normalize();
  return aNormal.Dot(aDVV) / aDV.SquareMagnitude();
}

std::vector<gp_Vec> loftEndpointDerivatives(const TopoDS_Shape& theShape, const bool theFirst)
{
  std::vector<gp_Vec> aDerivatives;
  for (TopExp_Explorer anExplorer(theShape, TopAbs_FACE); anExplorer.More(); anExplorer.Next())
  {
    TopLoc_Location                 aLocation;
    const occ::handle<Geom_Surface> aSurface =
      BRep_Tool::Surface(TopoDS::Face(anExplorer.Current()), aLocation);
    if (aSurface.IsNull())
    {
      continue;
    }
    double aUFirst, aULast, aVFirst, aVLast;
    aSurface->Bounds(aUFirst, aULast, aVFirst, aVLast);
    gp_Pnt aPoint;
    gp_Vec aDU, aDV;
    aSurface->D1(0.37 * aUFirst + 0.63 * aULast, theFirst ? aVFirst : aVLast, aPoint, aDU, aDV);
    aDV.Transform(aLocation.Transformation());
    aDerivatives.push_back(aDV);
  }
  return aDerivatives;
}

std::vector<double> loftEndpointNormalCurvatures(const TopoDS_Shape& theShape, const bool theFirst)
{
  std::vector<double> aCurvatures;
  for (TopExp_Explorer anExplorer(theShape, TopAbs_FACE); anExplorer.More(); anExplorer.Next())
  {
    TopLoc_Location                 aLocation;
    const occ::handle<Geom_Surface> aSurface =
      BRep_Tool::Surface(TopoDS::Face(anExplorer.Current()), aLocation);
    if (aSurface.IsNull())
    {
      continue;
    }
    double aUFirst, aULast, aVFirst, aVLast;
    aSurface->Bounds(aUFirst, aULast, aVFirst, aVLast);
    gp_Pnt aPoint;
    gp_Vec aDU, aDV, aDUU, aDVV, aDUV;
    aSurface->D2(0.37 * aUFirst + 0.63 * aULast,
                 theFirst ? aVFirst : aVLast,
                 aPoint,
                 aDU,
                 aDV,
                 aDUU,
                 aDVV,
                 aDUV);
    aDU.Transform(aLocation.Transformation());
    aDV.Transform(aLocation.Transformation());
    aDVV.Transform(aLocation.Transformation());
    gp_Vec aNormal = aDU.Crossed(aDV);
    if (aNormal.SquareMagnitude() <= gp::Resolution() || aDV.SquareMagnitude() <= gp::Resolution())
    {
      aCurvatures.push_back(RealLast());
      continue;
    }
    aNormal.Normalize();
    aCurvatures.push_back(aNormal.Dot(aDVV) / aDV.SquareMagnitude());
  }
  return aCurvatures;
}

void expectSameDirection(const gp_Vec& theActual, const gp_Dir& theExpected)
{
  ASSERT_GT(theActual.SquareMagnitude(), gp::Resolution());
  EXPECT_GT(theActual.Dot(gp_Vec(theExpected)), 0.0);
  EXPECT_LT(theActual.Crossed(gp_Vec(theExpected)).Magnitude() / theActual.Magnitude(), 1.0e-7);
}

struct EdgeSupportedWire
{
  TopoDS_Wire                             Wire;
  BRepOffsetAPI_ThruSections::EdgeFaceMap Supports;
};

int freeEdgeCount(const TopoDS_Shape& theShape)
{
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    anEdgeFaces;
  TopExp::MapShapesAndAncestors(theShape, TopAbs_EDGE, TopAbs_FACE, anEdgeFaces);
  int aCount = 0;
  for (int anIndex = 1; anIndex <= anEdgeFaces.Extent(); ++anIndex)
  {
    if (anEdgeFaces.FindFromIndex(anIndex).Size() == 1)
    {
      ++aCount;
    }
  }
  return aCount;
}

int faceCount(const TopoDS_Shape& theShape)
{
  int aCount = 0;
  for (TopExp_Explorer anExplorer(theShape, TopAbs_FACE); anExplorer.More(); anExplorer.Next())
  {
    ++aCount;
  }
  return aCount;
}

TopoDS_Wire makeToleranceCloseOpenRectangle(const double theZ)
{
  BRepBuilderAPI_MakeVertex aMakeFirst(gp_Pnt(-5.0, -5.0, theZ));
  BRepBuilderAPI_MakeVertex aMakeSecond(gp_Pnt(5.0, -5.0, theZ));
  BRepBuilderAPI_MakeVertex aMakeThird(gp_Pnt(5.0, 5.0, theZ));
  BRepBuilderAPI_MakeVertex aMakeFourth(gp_Pnt(-5.0, 5.0, theZ));
  // Use a distinct seam vertex whose offset is accepted by both vertex tolerances.
  BRepBuilderAPI_MakeVertex aMakeClosing(gp_Pnt(-5.0, -5.0 + 5.0e-7, theZ));
  TopoDS_Vertex             aFirst   = aMakeFirst.Vertex();
  TopoDS_Vertex             aSecond  = aMakeSecond.Vertex();
  TopoDS_Vertex             aThird   = aMakeThird.Vertex();
  TopoDS_Vertex             aFourth  = aMakeFourth.Vertex();
  TopoDS_Vertex             aClosing = aMakeClosing.Vertex();
  BRep_Builder              aBuilder;
  aBuilder.UpdateVertex(aFirst, 1.0e-6);
  aBuilder.UpdateVertex(aClosing, 1.0e-6);

  TopoDS_Wire aWire;
  aBuilder.MakeWire(aWire);
  aBuilder.Add(aWire, BRepBuilderAPI_MakeEdge(aFirst, aSecond).Edge());
  aBuilder.Add(aWire, BRepBuilderAPI_MakeEdge(aSecond, aThird).Edge());
  aBuilder.Add(aWire, BRepBuilderAPI_MakeEdge(aThird, aFourth).Edge());
  aBuilder.Add(aWire, BRepBuilderAPI_MakeEdge(aFourth, aClosing).Edge());
  return aWire;
}

struct C1SupportedWire
{
  TopoDS_Wire                      Wire;
  TopoDS_Face                      Support;
  occ::handle<Geom_BSplineSurface> Surface;
};

C1SupportedWire makeC1SupportedWire()
{
  NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 6);
  const double               aZ[] = {0.0, 0.0, 0.0, 1.0, 2.0, 3.0};
  for (int aUIndex = 1; aUIndex <= 2; ++aUIndex)
  {
    for (int aVIndex = 1; aVIndex <= 6; ++aVIndex)
    {
      aPoles(aUIndex, aVIndex) = gp_Pnt(aUIndex - 1.0, aVIndex - 1.0, aZ[aVIndex - 1]);
    }
  }
  NCollection_Array1<double> aUKnots(1, 2), aVKnots(1, 3);
  aUKnots(1) = 0.0;
  aUKnots(2) = 1.0;
  aVKnots(1) = 0.0;
  aVKnots(2) = 0.5;
  aVKnots(3) = 1.0;
  NCollection_Array1<int> aUMultiplicities(1, 2), aVMultiplicities(1, 3);
  aUMultiplicities(1) = 2;
  aUMultiplicities(2) = 2;
  aVMultiplicities(1) = 4;
  aVMultiplicities(2) = 2;
  aVMultiplicities(3) = 4;

  C1SupportedWire aResult;
  aResult.Surface =
    new Geom_BSplineSurface(aPoles, aUKnots, aVKnots, aUMultiplicities, aVMultiplicities, 1, 3);
  aResult.Support = BRepBuilderAPI_MakeFace(aResult.Surface, Precision::Confusion()).Face();
  aResult.Wire =
    BRepBuilderAPI_MakeWire(BRepBuilderAPI_MakeEdge(aResult.Surface->VIso(0.5)).Edge()).Wire();
  return aResult;
}

EdgeSupportedWire makeBoxTopWireWithSideSupports()
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(gp_Pnt(-5.0, -5.0, -5.0), 10.0, 10.0, 5.0).Shape();
  TopoDS_Face        aTopFace;
  for (TopExp_Explorer aFaceExplorer(aBox, TopAbs_FACE); aFaceExplorer.More(); aFaceExplorer.Next())
  {
    const TopoDS_Face& aFace = TopoDS::Face(aFaceExplorer.Current());
    GProp_GProps       aProperties;
    BRepGProp::SurfaceProperties(aFace, aProperties);
    if (std::abs(aProperties.CentreOfMass().Z()) < Precision::Confusion())
    {
      aTopFace = aFace;
      break;
    }
  }
  if (aTopFace.IsNull())
  {
    return {};
  }

  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    anEdgeFaces;
  TopExp::MapShapesAndAncestors(aBox, TopAbs_EDGE, TopAbs_FACE, anEdgeFaces);

  EdgeSupportedWire aResult;
  aResult.Wire = BRepTools::OuterWire(aTopFace);
  for (BRepTools_WireExplorer anEdgeExplorer(aResult.Wire); anEdgeExplorer.More();
       anEdgeExplorer.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgeExplorer.Current());
    if (!anEdgeFaces.Contains(anEdge))
    {
      return {};
    }
    TopoDS_Face aSideFace;
    for (NCollection_List<TopoDS_Shape>::Iterator aFaceIterator(anEdgeFaces.FindFromKey(anEdge));
         aFaceIterator.More();
         aFaceIterator.Next())
    {
      if (!aFaceIterator.Value().IsSame(aTopFace))
      {
        if (!aSideFace.IsNull())
        {
          return {};
        }
        aSideFace = TopoDS::Face(aFaceIterator.Value());
      }
    }
    if (aSideFace.IsNull())
    {
      return {};
    }
    aResult.Supports.Bind(anEdge, aSideFace);
  }
  return aResult;
}
} // namespace

TEST(BRepOffsetAPI_ThruSections_Test, PlanarNormal_TwoSections_MatchesDirections)
{
  BRepOffsetAPI_ThruSections aLoft(false, false, 1.0e-7);
  aLoft.AddWire(makeRationalCircleWire(gp::Origin(), 5.0));
  aLoft.AddWire(makeRationalCircleWire(gp_Pnt(1.0, 0.0, 10.0), 7.0));
  aLoft.SetFirstSectionNormal();
  aLoft.SetLastSectionNormal();
  aLoft.Build();

  ASSERT_TRUE(aLoft.IsDone());
  EXPECT_EQ(aLoft.GetStatus(), BRepFill_ThruSectionErrorStatus_Done);
  expectSameDirection(loftEndpointDerivative(aLoft.Shape(), true), gp::DZ());
  expectSameDirection(loftEndpointDerivative(aLoft.Shape(), false), gp::DZ());

  TopLoc_Location                        aLocation;
  const occ::handle<Geom_Surface>        aSurface = firstLoftSurface(aLoft.Shape(), aLocation);
  const occ::handle<Geom_BSplineSurface> aBSpline = occ::down_cast<Geom_BSplineSurface>(aSurface);
  ASSERT_FALSE(aBSpline.IsNull());
  EXPECT_TRUE(aBSpline->IsVRational());
  EXPECT_EQ(aBSpline->VDegree(), 3);

  BRepOffsetAPI_ThruSections aOneSidedLoft(false, false, 1.0e-7);
  aOneSidedLoft.AddWire(makeRationalCircleWire(gp::Origin(), 5.0));
  aOneSidedLoft.AddWire(makeRationalCircleWire(gp_Pnt(1.0, 0.0, 10.0), 7.0));
  aOneSidedLoft.SetFirstSectionNormal();
  aOneSidedLoft.Build();

  ASSERT_TRUE(aOneSidedLoft.IsDone());
  expectSameDirection(loftEndpointDerivative(aOneSidedLoft.Shape(), true), gp::DZ());
}

TEST(BRepOffsetAPI_ThruSections_Test, EndpointTangents_IndependentDirections_MatchesDirections)
{
  const gp_Dir aFirstDirection = gp::DZ();
  const gp_Dir aLastDirection(1.0, 0.0, 1.0);

  BRepOffsetAPI_ThruSections aLoft(false, false, 1.0e-7);
  aLoft.AddWire(makeCircleWire(gp::Origin(), 5.0));
  aLoft.AddWire(makeCircleWire(gp_Pnt(0.5, 0.0, 5.0), 6.0));
  aLoft.AddWire(makeCircleWire(gp_Pnt(2.0, 0.0, 10.0), 7.0));
  aLoft.SetFirstSectionTangent(aFirstDirection);
  aLoft.SetLastSectionTangent(aLastDirection);
  aLoft.Build();

  ASSERT_TRUE(aLoft.IsDone());
  expectSameDirection(loftEndpointDerivative(aLoft.Shape(), true), aFirstDirection);
  expectSameDirection(loftEndpointDerivative(aLoft.Shape(), false), aLastDirection);
}

TEST(BRepOffsetAPI_ThruSections_Test, PlanarG2_TwoSections_MatchesZeroCurvature)
{
  BRepOffsetAPI_ThruSections aLoft(false, false, 1.0e-7);
  aLoft.AddWire(makeCircleWire(gp::Origin(), 5.0));
  aLoft.AddWire(makeCircleWire(gp_Pnt(1.0, 0.0, 10.0), 7.0));
  aLoft.SetFirstSectionNormal(GeomAbs_G2);
  aLoft.SetLastSectionNormal(GeomAbs_G2);
  aLoft.Build();

  ASSERT_TRUE(aLoft.IsDone());
  EXPECT_EQ(aLoft.FirstSectionContinuity(), GeomAbs_G2);
  EXPECT_EQ(aLoft.LastSectionContinuity(), GeomAbs_G2);
  expectSameDirection(loftEndpointDerivative(aLoft.Shape(), true), gp::DZ());
  expectSameDirection(loftEndpointDerivative(aLoft.Shape(), false), gp::DZ());
  EXPECT_NEAR(loftEndpointNormalCurvature(aLoft.Shape(), true), 0.0, 1.0e-7);
  EXPECT_NEAR(loftEndpointNormalCurvature(aLoft.Shape(), false), 0.0, 1.0e-7);
}

TEST(BRepOffsetAPI_ThruSections_Test, PlanarG2_ManySections_MatchesZeroCurvature)
{
  BRepOffsetAPI_ThruSections aLoft(false, false, 1.0e-7);
  for (int anIndex = 0; anIndex < 8; ++anIndex)
  {
    aLoft.AddWire(makeCircleWire(gp_Pnt(0.2 * anIndex, 0.0, 3.0 * anIndex), 5.0 + 0.4 * anIndex));
  }
  aLoft.SetFirstSectionNormal(GeomAbs_G2);
  aLoft.Build();

  ASSERT_TRUE(aLoft.IsDone());
  expectSameDirection(loftEndpointDerivative(aLoft.Shape(), true), gp::DZ());
  EXPECT_NEAR(loftEndpointNormalCurvature(aLoft.Shape(), true), 0.0, 1.0e-7);
}

TEST(BRepOffsetAPI_ThruSections_Test, PlanarG2_SolidLoft_BuildsValidSolid)
{
  BRepOffsetAPI_ThruSections aLoft(true, false, 1.0e-7);
  aLoft.AddWire(makeRationalCircleWire(gp::Origin(), 5.0));
  aLoft.AddWire(makeRationalCircleWire(gp_Pnt(0.0, 0.0, 5.0), 6.0));
  aLoft.SetFirstSectionNormal(GeomAbs_G2);
  aLoft.SetLastSectionNormal(GeomAbs_G2);
  aLoft.Build();

  ASSERT_TRUE(aLoft.IsDone());
  ASSERT_FALSE(aLoft.Shape().IsNull());
  EXPECT_EQ(aLoft.Shape().ShapeType(), TopAbs_SOLID);
  EXPECT_TRUE(BRepCheck_Analyzer(aLoft.Shape()).IsValid());
}

TEST(BRepOffsetAPI_ThruSections_Test, G2Parameterization_NonUniformSections_ChangesKnots)
{
  const auto buildSurface = [](const Approx_ParametrizationType theParametrization) {
    const double               aZ[] = {0.0, 0.5, 1.5, 4.0, 8.0, 13.0, 19.0, 28.0};
    BRepOffsetAPI_ThruSections aLoft(false, false, 1.0e-7);
    for (int anIndex = 0; anIndex < 8; ++anIndex)
    {
      aLoft.AddWire(makeCircleWire(gp_Pnt(0.0, 0.0, aZ[anIndex]), 5.0 + 0.3 * anIndex));
    }
    aLoft.SetParType(theParametrization);
    aLoft.SetFirstSectionNormal(GeomAbs_G2);
    aLoft.Build();
    if (!aLoft.IsDone())
    {
      return occ::handle<Geom_BSplineSurface>();
    }
    TopLoc_Location aLocation;
    return occ::down_cast<Geom_BSplineSurface>(firstLoftSurface(aLoft.Shape(), aLocation));
  };

  const occ::handle<Geom_BSplineSurface> aUniform     = buildSurface(Approx_IsoParametric);
  const occ::handle<Geom_BSplineSurface> aChord       = buildSurface(Approx_ChordLength);
  const occ::handle<Geom_BSplineSurface> aCentripetal = buildSurface(Approx_Centripetal);
  ASSERT_FALSE(aUniform.IsNull());
  ASSERT_FALSE(aChord.IsNull());
  ASSERT_FALSE(aCentripetal.IsNull());
  ASSERT_GT(aUniform->NbVKnots(), 2);
  ASSERT_EQ(aUniform->NbVKnots(), aChord->NbVKnots());
  ASSERT_EQ(aUniform->NbVKnots(), aCentripetal->NbVKnots());
  EXPECT_GT(std::abs(aUniform->VKnot(2) - aChord->VKnot(2)), 1.0e-3);
  EXPECT_GT(std::abs(aUniform->VKnot(2) - aCentripetal->VKnot(2)), 1.0e-3);
  EXPECT_GT(std::abs(aChord->VKnot(2) - aCentripetal->VKnot(2)), 1.0e-3);
}

TEST(BRepOffsetAPI_ThruSections_Test, G2ChordLength_NearCoincidentSections_Builds)
{
  BRepOffsetAPI_ThruSections aLoft(false, false, 1.0e-7);
  aLoft.AddWire(makeCircleWire(gp::Origin(), 5.0));
  aLoft.AddWire(makeCircleWire(gp_Pnt(0.0, 0.0, 1.0e-6), 5.000001));
  aLoft.AddWire(makeCircleWire(gp_Pnt(0.0, 0.0, 5.0), 6.0));
  aLoft.SetParType(Approx_ChordLength);
  aLoft.SetFirstSectionNormal(GeomAbs_G2);
  aLoft.Build();

  ASSERT_TRUE(aLoft.IsDone());
  EXPECT_EQ(aLoft.GetStatus(), BRepFill_ThruSectionErrorStatus_Done);
  EXPECT_TRUE(BRepCheck_Analyzer(aLoft.Shape()).IsValid());
  EXPECT_NEAR(loftEndpointNormalCurvature(aLoft.Shape(), true), 0.0, 1.0e-6);
}

TEST(BRepOffsetAPI_ThruSections_Test, SupportFace_G1FirstSection_MatchesTangentPlane)
{
  const TopoDS_Wire aBoundary = makeRationalCircleWire(gp::Origin(), 5.0);
  const TopoDS_Face aSupport  = BRepBuilderAPI_MakeFace(gp_Pln(gp::XOY()), aBoundary).Face();

  BRepOffsetAPI_ThruSections aLoft(false, false, 1.0e-7);
  aLoft.AddWire(BRepTools::OuterWire(aSupport));
  aLoft.AddWire(makeRationalCircleWire(gp_Pnt(0.0, 0.0, 5.0), 6.0));
  aLoft.SetFirstSectionSupport(aSupport, 1.0e-3);
  aLoft.Build();

  ASSERT_TRUE(aLoft.IsDone());
  const gp_Vec aDerivative = loftEndpointDerivative(aLoft.Shape(), true);
  ASSERT_GT(aDerivative.SquareMagnitude(), gp::Resolution());
  EXPECT_LT(std::abs(aDerivative.Dot(gp::DZ())) / aDerivative.Magnitude(), 1.0e-7);

  BRepOffsetAPI_ThruSections aReverseLoft(false, false, 1.0e-7);
  aReverseLoft.AddWire(makeRationalCircleWire(gp_Pnt(0.0, 0.0, 5.0), 6.0));
  aReverseLoft.AddWire(BRepTools::OuterWire(aSupport));
  aReverseLoft.SetLastSectionSupport(aSupport, 1.0e-3);
  aReverseLoft.Build();

  ASSERT_TRUE(aReverseLoft.IsDone());
  const gp_Vec aLastDerivative = loftEndpointDerivative(aReverseLoft.Shape(), false);
  ASSERT_GT(aLastDerivative.SquareMagnitude(), gp::Resolution());
  EXPECT_LT(std::abs(aLastDerivative.Dot(gp::DZ())) / aLastDerivative.Magnitude(), 1.0e-7);
}

TEST(BRepOffsetAPI_ThruSections_Test, SupportFace_G1_C1SurfaceDoesNotRequireD2)
{
  const C1SupportedWire aSupportedWire = makeC1SupportedWire();
  ASSERT_FALSE(aSupportedWire.Wire.IsNull());
  ASSERT_FALSE(aSupportedWire.Support.IsNull());
  ASSERT_FALSE(aSupportedWire.Surface.IsNull());
  ASSERT_EQ(aSupportedWire.Surface->Continuity(), GeomAbs_C1);

  const gp_Pnt            aFirstPoint = aSupportedWire.Surface->Value(0.0, 0.5);
  const gp_Pnt            aLastPoint  = aSupportedWire.Surface->Value(1.0, 0.5);
  BRepBuilderAPI_MakeEdge aMakeAdjacent(aFirstPoint.Translated(gp_Vec(0.0, 0.0, 5.0)),
                                        aLastPoint.Translated(gp_Vec(0.0, 0.0, 5.0)));

  BRepOffsetAPI_ThruSections aLoft(false, false, 1.0e-7);
  aLoft.AddWire(aSupportedWire.Wire);
  aLoft.AddWire(BRepBuilderAPI_MakeWire(aMakeAdjacent.Edge()).Wire());
  // Curvature tolerance is intentionally zero: it is irrelevant to a G1 constraint.
  aLoft.SetFirstSectionSupport(aSupportedWire.Support, GeomAbs_G1, 1.0e-3, 0.0);
  aLoft.Build();

  ASSERT_TRUE(aLoft.IsDone());
  const gp_Vec aDerivative = loftEndpointDerivative(aLoft.Shape(), true);
  ASSERT_GT(aDerivative.SquareMagnitude(), gp::Resolution());
  gp_Pnt aPoint;
  gp_Vec aDU, aDV;
  aSupportedWire.Surface->D1(0.63, 0.5, aPoint, aDU, aDV);
  const gp_Vec aNormal = aDU.Crossed(aDV);
  ASSERT_GT(aNormal.SquareMagnitude(), gp::Resolution());
  EXPECT_LT(std::abs(aDerivative.Dot(aNormal))
              / std::sqrt(aDerivative.SquareMagnitude() * aNormal.SquareMagnitude()),
            1.0e-7);
}

TEST(BRepOffsetAPI_ThruSections_Test, SupportFace_G1MultiEdgeSection_Builds)
{
  const TopoDS_Wire aBoundary = makeRectangleWire(5.0, 0.0);
  const TopoDS_Face aSupport  = BRepBuilderAPI_MakeFace(gp_Pln(gp::XOY()), aBoundary).Face();

  BRepOffsetAPI_ThruSections aLoft(false, false, 1.0e-7);
  aLoft.AddWire(BRepTools::OuterWire(aSupport));
  aLoft.AddWire(makeRectangleWire(6.0, 5.0));
  aLoft.SetFirstSectionSupport(aSupport, 1.0e-3);
  aLoft.Build();

  EXPECT_TRUE(aLoft.IsDone());
  EXPECT_EQ(aLoft.GetStatus(), BRepFill_ThruSectionErrorStatus_Done);
}

TEST(BRepOffsetAPI_ThruSections_Test, IndividualSupports_G1AndG2_BuildsAtBothEnds)
{
  const EdgeSupportedWire aSupportedWire = makeBoxTopWireWithSideSupports();
  ASSERT_FALSE(aSupportedWire.Wire.IsNull());
  ASSERT_EQ(aSupportedWire.Supports.Extent(), 4);

  BRepOffsetAPI_ThruSections aG1Loft(false, false, 1.0e-7);
  aG1Loft.AddWire(aSupportedWire.Wire);
  aG1Loft.AddWire(makeRectangleWire(6.0, 5.0));
  aG1Loft.SetFirstSectionSupports(aSupportedWire.Supports, GeomAbs_G1, 1.0e-3);
  aG1Loft.Build();

  ASSERT_TRUE(aG1Loft.IsDone());
  EXPECT_EQ(aG1Loft.GetStatus(), BRepFill_ThruSectionErrorStatus_Done);
  const std::vector<gp_Vec> aG1Derivatives = loftEndpointDerivatives(aG1Loft.Shape(), true);
  ASSERT_EQ(aG1Derivatives.size(), 4u);
  for (const gp_Vec& aDerivative : aG1Derivatives)
  {
    expectSameDirection(aDerivative, gp::DZ());
  }

  BRepOffsetAPI_ThruSections aG2Loft(false, false, 1.0e-7);
  aG2Loft.AddWire(aSupportedWire.Wire);
  aG2Loft.AddWire(makeRectangleWire(6.0, 5.0));
  aG2Loft.SetFirstSectionSupports(aSupportedWire.Supports, GeomAbs_G2, 1.0e-3);
  aG2Loft.Build();

  ASSERT_TRUE(aG2Loft.IsDone());
  EXPECT_EQ(aG2Loft.GetStatus(), BRepFill_ThruSectionErrorStatus_Done);
  const std::vector<gp_Vec> aG2Derivatives = loftEndpointDerivatives(aG2Loft.Shape(), true);
  ASSERT_EQ(aG2Derivatives.size(), 4u);
  for (const gp_Vec& aDerivative : aG2Derivatives)
  {
    expectSameDirection(aDerivative, gp::DZ());
  }
  const std::vector<double> aFirstCurvatures = loftEndpointNormalCurvatures(aG2Loft.Shape(), true);
  ASSERT_EQ(aFirstCurvatures.size(), 4u);
  for (const double aCurvature : aFirstCurvatures)
  {
    EXPECT_NEAR(aCurvature, 0.0, 1.0e-7);
  }

  BRepOffsetAPI_ThruSections aLastLoft(false, false, 1.0e-7);
  aLastLoft.AddWire(makeRectangleWire(6.0, -5.0));
  aLastLoft.AddWire(aSupportedWire.Wire);
  aLastLoft.SetLastSectionSupports(aSupportedWire.Supports, GeomAbs_G2, 1.0e-3);
  aLastLoft.Build();

  ASSERT_TRUE(aLastLoft.IsDone());
  EXPECT_EQ(aLastLoft.GetStatus(), BRepFill_ThruSectionErrorStatus_Done);
  const std::vector<gp_Vec> aLastDerivatives = loftEndpointDerivatives(aLastLoft.Shape(), false);
  ASSERT_EQ(aLastDerivatives.size(), 4u);
  for (const gp_Vec& aDerivative : aLastDerivatives)
  {
    expectSameDirection(aDerivative, gp::DZ());
  }
  const std::vector<double> aLastCurvatures =
    loftEndpointNormalCurvatures(aLastLoft.Shape(), false);
  ASSERT_EQ(aLastCurvatures.size(), 4u);
  for (const double aCurvature : aLastCurvatures)
  {
    EXPECT_NEAR(aCurvature, 0.0, 1.0e-7);
  }
}

TEST(BRepOffsetAPI_ThruSections_Test, IndividualSupports_CompatibilitySplits_PropagatesMappings)
{
  const EdgeSupportedWire aSupportedWire = makeBoxTopWireWithSideSupports();
  ASSERT_FALSE(aSupportedWire.Wire.IsNull());

  BRepOffsetAPI_ThruSections aLoft(false, false, 1.0e-7);
  aLoft.AddWire(aSupportedWire.Wire);
  aLoft.AddWire(makeSplitRectangleWire(6.0, 5.0));
  aLoft.SetFirstSectionSupports(aSupportedWire.Supports, GeomAbs_G1, 1.0e-3);
  aLoft.Build();

  ASSERT_TRUE(aLoft.IsDone());
  EXPECT_EQ(aLoft.GetStatus(), BRepFill_ThruSectionErrorStatus_Done);
}

TEST(BRepOffsetAPI_ThruSections_Test, IndividualSupports_CompatibilitySplits_PreserveInputTolerance)
{
  EdgeSupportedWire aSupportedWire = makeBoxTopWireWithSideSupports();
  ASSERT_FALSE(aSupportedWire.Wire.IsNull());

  BRep_Builder aBuilder;
  for (TopExp_Explorer anExplorer(aSupportedWire.Wire, TopAbs_VERTEX); anExplorer.More();
       anExplorer.Next())
  {
    aBuilder.UpdateVertex(TopoDS::Vertex(anExplorer.Current()), 1.0e-6);
  }

  BRepOffsetAPI_ThruSections::EdgeFaceMap aDetachedSupports;
  for (BRepOffsetAPI_ThruSections::EdgeFaceMap::Iterator anIterator(aSupportedWire.Supports);
       anIterator.More();
       anIterator.Next())
  {
    const TopoDS_Face   aSupport = TopoDS::Face(anIterator.Value());
    BRepAdaptor_Surface anAdaptor(aSupport);
    const double        aU = 0.5 * (anAdaptor.FirstUParameter() + anAdaptor.LastUParameter());
    const double        aV = 0.5 * (anAdaptor.FirstVParameter() + anAdaptor.LastVParameter());
    gp_Pnt              aPoint;
    gp_Vec              aDU, aDV;
    anAdaptor.D1(aU, aV, aPoint, aDU, aDV);
    gp_Vec aNormal = aDU.Crossed(aDV);
    ASSERT_GT(aNormal.SquareMagnitude(), gp::Resolution());
    aNormal.Normalize();

    gp_Trsf aTranslation;
    aTranslation.SetTranslation(aNormal.Multiplied(5.0e-7));
    BRepBuilderAPI_Copy aCopy(aSupport);
    aDetachedSupports.Bind(anIterator.Key(), aCopy.Shape().Moved(TopLoc_Location(aTranslation)));
  }

  BRepOffsetAPI_ThruSections aLoft(false, false, 1.0e-6);
  aLoft.AddWire(aSupportedWire.Wire);
  aLoft.AddWire(makeSplitRectangleWire(6.0, 5.0));
  aLoft.SetFirstSectionSupports(aDetachedSupports, GeomAbs_G1, 1.0e-3);
  aLoft.Build();

  ASSERT_TRUE(aLoft.IsDone());
  EXPECT_EQ(aLoft.GetStatus(), BRepFill_ThruSectionErrorStatus_Done);
}

TEST(BRepOffsetAPI_ThruSections_Test, ConstrainedClosedSections_CompatibilitySplitsShareSeam)
{
  for (const GeomAbs_Shape aContinuity : {GeomAbs_G1, GeomAbs_G2})
  {
    BRepOffsetAPI_ThruSections aLoft(false, false, 1.0e-6);
    aLoft.AddWire(makeRectangleWire(5.0, 0.0));
    aLoft.AddWire(makeSplitRectangleWire(6.0, 5.0));
    aLoft.SetLastSectionNormal(aContinuity);
    aLoft.Build();

    ASSERT_TRUE(aLoft.IsDone());
    ASSERT_GT(faceCount(aLoft.Shape()), 0);
    EXPECT_EQ(freeEdgeCount(aLoft.Shape()), 2 * faceCount(aLoft.Shape()));
    EXPECT_TRUE(BRepCheck_Analyzer(aLoft.Shape()).IsValid());
  }
}

TEST(BRepOffsetAPI_ThruSections_Test, ConstrainedToleranceCloseOpenSections_RemainOpen)
{
  for (const GeomAbs_Shape aContinuity : {GeomAbs_G1, GeomAbs_G2})
  {
    BRepOffsetAPI_ThruSections aLoft(false, false, 1.0e-6);
    aLoft.AddWire(makeToleranceCloseOpenRectangle(0.0));
    aLoft.AddWire(makeToleranceCloseOpenRectangle(5.0));
    aLoft.SetLastSectionNormal(aContinuity);
    aLoft.Build();

    ASSERT_TRUE(aLoft.IsDone());
    ASSERT_GT(faceCount(aLoft.Shape()), 0);
    EXPECT_EQ(freeEdgeCount(aLoft.Shape()), 2 * faceCount(aLoft.Shape()) + 2);
    EXPECT_TRUE(BRepCheck_Analyzer(aLoft.Shape()).IsValid());
  }
}

TEST(BRepOffsetAPI_ThruSections_Test, IndividualSupports_CoincidentDetachedFaces_ProjectsEdges)
{
  const EdgeSupportedWire aSupportedWire = makeBoxTopWireWithSideSupports();
  ASSERT_FALSE(aSupportedWire.Wire.IsNull());

  BRepOffsetAPI_ThruSections::EdgeFaceMap aDetachedSupports;
  for (BRepOffsetAPI_ThruSections::EdgeFaceMap::Iterator anIterator(aSupportedWire.Supports);
       anIterator.More();
       anIterator.Next())
  {
    BRepBuilderAPI_Copy aCopy(anIterator.Value());
    aDetachedSupports.Bind(anIterator.Key(), aCopy.Shape());
  }

  BRepOffsetAPI_ThruSections aLoft(false, false, 1.0e-7);
  aLoft.AddWire(aSupportedWire.Wire);
  aLoft.AddWire(makeRectangleWire(6.0, 5.0));
  aLoft.SetFirstSectionSupports(aDetachedSupports, GeomAbs_G2, 1.0e-3);
  aLoft.Build();

  ASSERT_TRUE(aLoft.IsDone());
  EXPECT_EQ(aLoft.GetStatus(), BRepFill_ThruSectionErrorStatus_Done);
  EXPECT_NEAR(loftEndpointNormalCurvature(aLoft.Shape(), true), 0.0, 1.0e-7);
}

TEST(BRepOffsetAPI_ThruSections_Test, SupportFace_DetachedProfileInsideHole_IsRejected)
{
  const TopoDS_Wire       anOuterWire = makeRectangleWire(10.0, 0.0);
  const TopoDS_Wire       aHoleWire   = makeCircleWire(gp::Origin(), 3.0);
  BRepBuilderAPI_MakeFace aMakeSupport(gp_Pln(gp::XOY()), anOuterWire);
  aMakeSupport.Add(TopoDS::Wire(aHoleWire.Reversed()));
  ASSERT_TRUE(aMakeSupport.IsDone());

  BRepOffsetAPI_ThruSections aLoft(false, false, 1.0e-7);
  aLoft.AddWire(makeCircleWire(gp::Origin(), 1.0));
  aLoft.AddWire(makeCircleWire(gp_Pnt(0.0, 0.0, 5.0), 2.0));
  aLoft.SetFirstSectionSupport(aMakeSupport.Face(), GeomAbs_G1, 1.0e-3);
  aLoft.Build();

  EXPECT_FALSE(aLoft.IsDone());
  EXPECT_EQ(aLoft.GetStatus(), BRepFill_ThruSectionErrorStatus_InvalidBoundaryConstraint);
}

TEST(BRepOffsetAPI_ThruSections_Test, SupportFace_DetachedParallelPlane_IsRejected)
{
  const TopoDS_Face aDetachedSupport =
    BRepBuilderAPI_MakeFace(gp_Pln(gp_Pnt(0.0, 0.0, 20.0), gp::DZ()), -10.0, 10.0, -10.0, 10.0)
      .Face();

  for (const GeomAbs_Shape aContinuity : {GeomAbs_G1, GeomAbs_G2})
  {
    BRepOffsetAPI_ThruSections aLoft(false, false, 1.0e-7);
    aLoft.AddWire(makeCircleWire(gp::Origin(), 5.0));
    aLoft.AddWire(makeCircleWire(gp_Pnt(0.0, 0.0, 5.0), 6.0));
    aLoft.SetFirstSectionSupport(aDetachedSupport, aContinuity, 1.0e-3, 1.0e-3);
    aLoft.Build();

    EXPECT_FALSE(aLoft.IsDone());
    EXPECT_EQ(aLoft.GetStatus(), BRepFill_ThruSectionErrorStatus_InvalidBoundaryConstraint);
  }
}

TEST(BRepOffsetAPI_ThruSections_Test, CylindricalSupport_G1FirstSection_MatchesTangentPlane)
{
  const TopoDS_Shape aCylinder = BRepPrimAPI_MakeCylinder(5.0, 5.0).Shape();
  TopoDS_Face        aSupport;
  for (TopExp_Explorer aFaceExplorer(aCylinder, TopAbs_FACE); aFaceExplorer.More();
       aFaceExplorer.Next())
  {
    const TopoDS_Face& aFace = TopoDS::Face(aFaceExplorer.Current());
    if (BRepAdaptor_Surface(aFace).GetType() == GeomAbs_Cylinder)
    {
      aSupport = aFace;
      break;
    }
  }
  ASSERT_FALSE(aSupport.IsNull());

  TopoDS_Edge aBottomEdge, aTopEdge;
  for (TopExp_Explorer anEdgeExplorer(aSupport, TopAbs_EDGE); anEdgeExplorer.More();
       anEdgeExplorer.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgeExplorer.Current());
    BRepAdaptor_Curve  aCurve(anEdge);
    if (aCurve.GetType() != GeomAbs_Circle)
    {
      continue;
    }
    const double aZ = aCurve.Value(aCurve.FirstParameter()).Z();
    if (std::abs(aZ) < Precision::Confusion())
    {
      aBottomEdge = anEdge;
    }
    else if (std::abs(aZ - 5.0) < Precision::Confusion())
    {
      aTopEdge = anEdge;
    }
  }
  ASSERT_FALSE(aBottomEdge.IsNull());
  ASSERT_FALSE(aTopEdge.IsNull());

  BRepOffsetAPI_ThruSections aFirstLoft(false, false, 1.0e-7);
  aFirstLoft.AddWire(BRepBuilderAPI_MakeWire(aBottomEdge).Wire());
  aFirstLoft.AddWire(makeCircleWire(gp_Pnt(0.0, 0.0, 5.0), 6.0));
  aFirstLoft.SetFirstSectionSupport(aSupport, 1.0e-3);
  aFirstLoft.Build();

  EXPECT_TRUE(aFirstLoft.IsDone());
  EXPECT_EQ(aFirstLoft.GetStatus(), BRepFill_ThruSectionErrorStatus_Done);

  BRepOffsetAPI_ThruSections aLastLoft(false, false, 1.0e-7);
  aLastLoft.AddWire(makeCircleWire(gp_Pnt(0.0, 0.0, 0.0), 6.0));
  aLastLoft.AddWire(BRepBuilderAPI_MakeWire(aTopEdge).Wire());
  aLastLoft.SetLastSectionSupport(aSupport, 1.0e-3);
  aLastLoft.Build();

  EXPECT_TRUE(aLastLoft.IsDone());
  EXPECT_EQ(aLastLoft.GetStatus(), BRepFill_ThruSectionErrorStatus_Done);
}

TEST(BRepOffsetAPI_ThruSections_Test, SphericalSupport_G2CurvatureTolerance_PreservedAfterProfiling)
{
  const TopoDS_Shape aHemisphere = BRepPrimAPI_MakeSphere(5.0, -0.5 * M_PI, 0.0).Shape();
  TopoDS_Face        aSupport;
  for (TopExp_Explorer aFaceExplorer(aHemisphere, TopAbs_FACE); aFaceExplorer.More();
       aFaceExplorer.Next())
  {
    const TopoDS_Face& aFace = TopoDS::Face(aFaceExplorer.Current());
    if (BRepAdaptor_Surface(aFace).GetType() == GeomAbs_Sphere)
    {
      aSupport = aFace;
      break;
    }
  }
  ASSERT_FALSE(aSupport.IsNull());

  TopoDS_Edge anEquator;
  for (TopExp_Explorer anEdgeExplorer(aSupport, TopAbs_EDGE); anEdgeExplorer.More();
       anEdgeExplorer.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgeExplorer.Current());
    BRepAdaptor_Curve  aCurve(anEdge);
    if (aCurve.GetType() == GeomAbs_Circle
        && std::abs(aCurve.Value(aCurve.FirstParameter()).Z()) < Precision::Confusion())
    {
      anEquator = anEdge;
      break;
    }
  }
  ASSERT_FALSE(anEquator.IsNull());

  BRepOffsetAPI_ThruSections aLoft(false, false, 1.0e-7);
  aLoft.AddWire(BRepBuilderAPI_MakeWire(anEquator).Wire());
  aLoft.AddWire(makeCircleWire(gp_Pnt(0.0, 0.0, 2.0), 5.5));
  aLoft.AddWire(makeCircleWire(gp_Pnt(0.0, 0.0, 5.0), 6.5));
  aLoft.SetFirstSectionSupport(aSupport, GeomAbs_G2, 1.0e-3, 1.0e-4);
  aLoft.Build();

  ASSERT_TRUE(aLoft.IsDone());
  EXPECT_EQ(aLoft.GetStatus(), BRepFill_ThruSectionErrorStatus_Done);
  EXPECT_EQ(aLoft.FirstSectionContinuity(), GeomAbs_G2);
  for (int aSample = 0; aSample <= 32; ++aSample)
  {
    const double aFraction = static_cast<double>(aSample) / 32.0;
    EXPECT_NEAR(std::abs(loftEndpointNormalCurvature(aLoft.Shape(), true, aFraction)), 0.2, 2.0e-3)
      << "at profile fraction " << aFraction;
  }

  BRepOffsetAPI_ThruSections aStrictLoft(false, false, 1.0e-7);
  aStrictLoft.AddWire(BRepBuilderAPI_MakeWire(anEquator).Wire());
  aStrictLoft.AddWire(makeCircleWire(gp_Pnt(0.0, 0.0, 2.0), 5.5));
  aStrictLoft.AddWire(makeCircleWire(gp_Pnt(0.0, 0.0, 5.0), 6.5));
  aStrictLoft.SetFirstSectionSupport(aSupport, GeomAbs_G2, 1.0e-3, 1.0e-8);
  aStrictLoft.Build();
  ASSERT_TRUE(aStrictLoft.IsDone());
  for (int aSample = 0; aSample <= 32; ++aSample)
  {
    const double aFraction = static_cast<double>(aSample) / 32.0;
    EXPECT_NEAR(std::abs(loftEndpointNormalCurvature(aStrictLoft.Shape(), true, aFraction)),
                0.2,
                1.0e-7)
      << "at profile fraction " << aFraction;
  }
}

TEST(BRepOffsetAPI_ThruSections_Test, BoundaryConstraints_RuledAndVariational_RejectsOptions)
{
  const TopoDS_Wire aFirst = makeCircleWire(gp::Origin(), 5.0);
  const TopoDS_Wire aLast  = makeCircleWire(gp_Pnt(0.0, 0.0, 5.0), 6.0);

  BRepOffsetAPI_ThruSections aRuledLoft(false, true);
  aRuledLoft.AddWire(aFirst);
  aRuledLoft.AddWire(aLast);
  aRuledLoft.SetFirstSectionTangent(gp::DZ());
  aRuledLoft.Build();
  EXPECT_FALSE(aRuledLoft.IsDone());
  EXPECT_EQ(aRuledLoft.GetStatus(), BRepFill_ThruSectionErrorStatus_IncompatibleOptions);

  BRepOffsetAPI_ThruSections aSmoothedLoft;
  aSmoothedLoft.AddWire(aFirst);
  aSmoothedLoft.AddWire(aLast);
  aSmoothedLoft.SetSmoothing(true);
  aSmoothedLoft.SetFirstSectionTangent(gp::DZ());
  aSmoothedLoft.Build();
  EXPECT_FALSE(aSmoothedLoft.IsDone());
  EXPECT_EQ(aSmoothedLoft.GetStatus(), BRepFill_ThruSectionErrorStatus_IncompatibleOptions);

  BRepOffsetAPI_ThruSections anInvalidSupportLoft;
  anInvalidSupportLoft.AddWire(aFirst);
  anInvalidSupportLoft.AddWire(aLast);
  anInvalidSupportLoft.SetFirstSectionSupport(TopoDS_Face());
  anInvalidSupportLoft.Build();
  EXPECT_FALSE(anInvalidSupportLoft.IsDone());
  EXPECT_EQ(anInvalidSupportLoft.GetStatus(),
            BRepFill_ThruSectionErrorStatus_InvalidBoundaryConstraint);

  const TopoDS_Face aPlanarSupport =
    BRepBuilderAPI_MakeFace(gp_Pln(gp::XOY()), -10.0, 10.0, -10.0, 10.0).Face();
  BRepOffsetAPI_ThruSections anInvalidCurvatureToleranceLoft;
  anInvalidCurvatureToleranceLoft.AddWire(aFirst);
  anInvalidCurvatureToleranceLoft.AddWire(aLast);
  anInvalidCurvatureToleranceLoft.SetFirstSectionSupport(aPlanarSupport, GeomAbs_G2, 1.0e-3, 0.0);
  anInvalidCurvatureToleranceLoft.Build();
  EXPECT_FALSE(anInvalidCurvatureToleranceLoft.IsDone());
  EXPECT_EQ(anInvalidCurvatureToleranceLoft.GetStatus(),
            BRepFill_ThruSectionErrorStatus_InvalidBoundaryConstraint);
}

TEST(BRepOffsetAPI_ThruSections_Test, VariationalSolver_NoBoundaryConstraint_Builds)
{
  BRepOffsetAPI_ThruSections aLoft(false, false, 1.0e-7);
  aLoft.AddWire(makeRectangleWire(5.0, 0.0));
  aLoft.AddWire(makeRectangleWire(5.5, 2.5));
  aLoft.AddWire(makeRectangleWire(6.0, 5.0));
  aLoft.SetSmoothing(true);
  aLoft.Build();

  ASSERT_TRUE(aLoft.IsDone());
  EXPECT_EQ(aLoft.GetStatus(), BRepFill_ThruSectionErrorStatus_Done);
  EXPECT_FALSE(aLoft.Shape().IsNull());
}

TEST(BRepOffsetAPI_ThruSections_Test, BoundaryConstraint_PunctualSection_ReturnsInvalidStatus)
{
  BRepOffsetAPI_ThruSections aLoft;
  aLoft.AddVertex(BRepBuilderAPI_MakeVertex(gp::Origin()).Vertex());
  aLoft.AddWire(makeCircleWire(gp_Pnt(0.0, 0.0, 5.0), 6.0));
  aLoft.SetFirstSectionTangent(gp::DZ());
  aLoft.Build();

  EXPECT_FALSE(aLoft.IsDone());
  EXPECT_EQ(aLoft.GetStatus(), BRepFill_ThruSectionErrorStatus_InvalidBoundaryConstraint);

  const TopoDS_Wire aWire = makeCircleWire(gp_Pnt(0.0, 0.0, 5.0), 6.0);
  TopExp_Explorer   anEdgeExplorer(aWire, TopAbs_EDGE);
  ASSERT_TRUE(anEdgeExplorer.More());
  BRepOffsetAPI_ThruSections::EdgeFaceMap aSupports;
  aSupports.Bind(anEdgeExplorer.Current(),
                 BRepBuilderAPI_MakeFace(gp_Pln(gp::XOY()), -10.0, 10.0, -10.0, 10.0).Face());

  BRepOffsetAPI_ThruSections anEdgeSupportedLoft;
  anEdgeSupportedLoft.AddVertex(BRepBuilderAPI_MakeVertex(gp::Origin()).Vertex());
  anEdgeSupportedLoft.AddWire(aWire);
  anEdgeSupportedLoft.SetFirstSectionSupports(aSupports);
  EXPECT_NO_THROW(anEdgeSupportedLoft.Build());
  EXPECT_FALSE(anEdgeSupportedLoft.IsDone());
  EXPECT_EQ(anEdgeSupportedLoft.GetStatus(),
            BRepFill_ThruSectionErrorStatus_InvalidBoundaryConstraint);
}
