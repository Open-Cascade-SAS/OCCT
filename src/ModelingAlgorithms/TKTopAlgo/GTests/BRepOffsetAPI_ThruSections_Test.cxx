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
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <BSplCLib.hxx>
#include <Geom_BSplineCurve.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Pnt.hxx>

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
  BRepOffsetAPI_ThruSections aLoft1(Standard_True, Standard_True);
  aLoft1.AddWire(aBottomPolygon1.Wire());
  aLoft1.AddWire(aTopPolygon1.Wire());
  aLoft1.Build();

  // Create second loft (ThruSections)
  BRepOffsetAPI_ThruSections aLoft2(Standard_True, Standard_True);
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
Handle(Geom_BSplineCurve) createBSplineCurve(const std::vector<double>& thePoles,
                                             const std::vector<double>& theKnots)
{
  const int aKnotSeqSize = static_cast<int>(theKnots.size());
  const int aDegree      = 3; // cubic spline
  const int aNbPoles     = aKnotSeqSize - aDegree - 1;

  TColgp_Array1OfPnt aPoles(1, aNbPoles);
  int                anIdx = 0;
  for (int i = 1; i <= aNbPoles; ++i)
  {
    aPoles(i) = gp_Pnt(thePoles[anIdx], thePoles[anIdx + 1], thePoles[anIdx + 2]);
    anIdx += 3;
  }

  TColStd_Array1OfReal aKnotSeq(1, aKnotSeqSize);
  for (int i = 1; i <= aKnotSeqSize; ++i)
  {
    aKnotSeq(i) = theKnots[i - 1];
  }

  TColStd_Array1OfReal    aKnots(1, BSplCLib::KnotsLength(aKnotSeq, false));
  TColStd_Array1OfInteger aMults(1, aKnots.Upper());
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
    Handle(Geom_BSplineCurve) aCurve = createBSplineCurve(aSection.first, aSection.second);
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
