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

#include <BRep_Builder.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_NurbsConvert.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepGProp.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepTools.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BinTools.hxx>
#include <Bnd_Box.hxx>
#include <Bnd_OBB.hxx>
#include <GProp_GProps.hxx>
#include <GProp_PrincipalProps.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_Map.hxx>
#include <OSD_Parallel.hxx>
#include <Poly_Triangulation.hxx>
#include <TopExp_Explorer.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <gp_Pnt.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Geom_Circle.hxx>
#include <gp_Ax2.hxx>

#include <gtest/gtest.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <sstream>
#include <string>

namespace
{

// Write a box to a BRep string once, to be reused by all threads.
std::string MakeBoxBrepString()
{
  BRepPrimAPI_MakeBox aBox(10.0, 20.0, 30.0);
  EXPECT_FALSE(aBox.Shape().IsNull());

  std::ostringstream anOss;
  BRepTools::Write(aBox.Shape(), anOss);
  EXPECT_FALSE(anOss.str().empty());
  return anOss.str();
}

// Read a shape from a BRep buffer. Each call uses its own stream.
bool ReadFromBuffer(const std::string& theData, TopoDS_Shape& theShape)
{
  std::istringstream anIss(theData);
  BRep_Builder       aBuilder;
  BRepTools::Read(theShape, anIss, aBuilder);
  return !theShape.IsNull();
}

const char* Bug30269BrepData()
{
  return R"OCCT(DBRep_DrawableShape

CASCADE Topology V1, (c) Matra-Datavision
Locations 0
Curve2ds 0
Curves 2
1 8111.3 2939 9014.2 0.23 5.2e-07 0.97
1 9996.2 2940 8568.4 -0.229 3.64e-07 -0.97
Polygon3D 0
PolygonOnTriangulations 0
Surfaces 2
1 9056.7 2940 8807.3 1.29e-07 -1 5e-07 -1.26e-29 -5.03e-07 -1 1 1.288e-07 -6.48e-14
1 10000 1519 8584.45 -0.97 -8.47e-08 0.23 0.23 0 0.97 -8.24e-08 1 1.95e-08
Triangulations 0

TShapes 10
Ve
1e-07
8111.30 2940 9014.2
0 0

0101101
*
Ve
1e-07
9996.21 2940 8568.4
0 0

0101101
*
Ed
 2.22e-16 0 0 0
0

0101000
+10 0 -9 0 *
Ve
1e-07
8115 2940 9030
0 0

0101101
*
Ed
 1e-07 0 0 0
1  1 0 0 16.5
0

0101000
+10 0 -7 0 *
Ve
1e-07
10000 2940 8584.5
0 0

0101101
*
Ed
 2.22e-16 0 0 0
0

0101000
+7 0 -5 0 *
Ed
 1e-07 0 0 0
1  2 0 -16.5 0
4 C0 1 0 2 0
0

0101000
+5 0 -9 0 *
Wi

0101000
-8 0 +6 0 +4 0 +3 0 *
Fa
0  1e-07 1 0

0101000
-2 0 *

-1 4
)OCCT";
}

const char* Bug29939BrepData()
{
  return R"OCCT(DBRep_DrawableShape

CASCADE Topology V1, (c) Matra-Datavision
Locations 6
1
-6.93889390391e-18               1               0  -266.829932547
             -1 -6.93889390391e-18               0               0
              0               0               1 8.881784197e-16
1
              1 -1.23204712323214e-17 3.48118418796994e-18  -357.513195371
1.23204706275e-17 0.999999999999985 1.73740064720997e-07 5.87091965856e-15
-3.48118632852936e-18 -1.73740064720997e-07 0.999999999999985 1.00245277978e-14
1
              1 -3.49249707682408e-17 -1.73723591341045e-16    613.47435459
3.30382668395332e-18 0.983888906493258 -0.178780926498047  -24.4029550353
1.77168632948486e-16 0.178780926498047 0.983888906493258   17.0096765133
1
-0.891601662261103 -0.296724731883325 0.34205395676999   289.371401338
0.321472550183128 -0.946772534430101 0.0166483490940992   -855.09721987
0.318907314641721 0.12480465350928 0.939532821741973  -233.868191989
2  1 1 2 1 3 1 4 1 0
2  4 -1 3 -1 2 -1 1 -1 0
Curve2ds 8
1 3.1415926535885261 -3.1415926537565504 -9.6123430370698398e-15 1
1 2.6415926535897927 3.1415926535897829 1 3.227912221389625e-15
1 -3.641592653589794 3.1415926535899144 1 -2.1627771834064215e-14
1 -3.641592653589794 3.1415926535897447 1 8.2694421717546571e-15
1 -3.641592653589794 3.1415926535897984 1 -2.5444437451706449e-15
1 -3.6415926535891083 3.1415926535895933 1 4.5496352846831891e-14
1 0 -3.1415926537565433 0 1
1 -3.6415926535884808 1.5707963267948022 1 1.8093822187836176e-14
Curves 9
2 84.858320332900007 -63 155.33211345500001 -0.87758256189037243 0 0.4794255386042035 -0.4794255386042035 0 -0.87758256189037243 -0 -1 0 2
2 -5.6843418860799994e-14 -63 4.2632564145599997e-14 -0 1 0 0 0 1 1 0 -0 175
7 0 0  4 5 2  9.51279659315 -64.469009257300002 174.74125643599999  10.8275887261 -63.750060296000001 174.66968003599999  12.261430838000001 -63.2501700292 174.57846249100001  13.7577343107 -62.999999975000001 174.464825784  15.252255180300001 -63 174.33407214900001
 0 5 8.4865807349600004 5
2 -5.6843418860799994e-14 -63 4.2632564145599997e-14 -0 1 0 0 0 1 1 0 -0 175
2 -5.6843418860799994e-14 -63 4.2632564145599997e-14 -0 1 0 0 0 1 1 0 -0 175
2 -5.6843418860799994e-14 -63 4.2632564145599997e-14 -0 1 0 0 0 1 1 0 -0 175
2 -5.6843418860799994e-14 -63 4.2632564145599997e-14 -0 1 0 0 0 1 1 0 -0 175
2 -84.858320332900007 -63 -155.33211345500001 0.87758256189037243 -0 -0.4794255386042035 0.4794255386042035 0 0.87758256189037243 0 -1 0 2
2 -4.2632564145599997e-14 -61 -2.8421709430399997e-14 0 1 0 0 -0 1 1 0 -0 177
Polygon3D 0
PolygonOnTriangulations 0
Surfaces 1
5 -5.6843418860799994e-14 -63 4.2632564145599997e-14 -0 1 0 -0.4794255386042035 0 -0.87758256189037243 -0.87758256189037243 -0 0.4794255386042035 177 2
Triangulations 0

TShapes 20
Ve
1.00005e-07
446.609600903292 -747.182655462685 -128.403390746598
0 0

0101101
*
Ve
1e-07
447.487711636221 -749.041771994926 -130.345710818059
0 0

0101101
*
Ed
 1e-07 1 1 0
1  1 0 4.71238898051187 6.28318530717959
2  1 1 0 4.71238898051187 6.28318530717959
0

0101000
+20 6 -19 6 *
Ve
0.000801323308506101
439.731321696259 -808.930183254921 -91.6608968795304
0 0

0101101
*
Ed
 1e-07 1 1 0
1  2 0 0.0872664625995157 0.499999999999316
2  2 1 0 0.0872664625995157 0.499999999999316
0

0101000
+17 6 -19 6 *
Ve
9.50082811303135e-06
439.731308365217 -808.930245083447 -91.6608718249203
0 0

0101101
*
Ed
 1e-07 1 1 0
1  3 0 8.48648424147889 8.48658045180264
0

0101000
+15 6 -17 6 *
Ve
0.000801323588114315
400.97481659698 -922.047565623353 -85.9891266596981
0 0

0101101
*
Ed
 1e-07 1 1 0
1  4 0 5.672320068985 6.37045138103039
2  3 1 0 5.672320068985 6.37045138103039
0

0101000
+13 6 -15 6 *
Ve
0.000801323173512029
346.92570979233 -1004.00637567207 -154.481647187039
0 0

0101101
*
Ed
 1e-07 1 1 0
1  5 0 4.974188368183 5.672320068985
2  4 1 0 4.974188368183 5.672320068985
0

0101000
+11 6 -13 6 *
Ve
0.000801323604481778
302.874179046209 -1016.45717530592 -265.090046896853
0 0

0101101
*
Ed
 1e-07 1 1 0
1  6 0 4.27605666738522 4.974188368183
2  5 1 0 4.27605666738522 4.974188368183
0

0101000
+9 6 -11 6 *
Ve
1e-07
289.096508106169 -961.871772388525 -358.63416338841
0 0

0101101
*
Ed
 1e-07 1 1 0
1  7 0 3.64159265358911 4.27605666738522
2  6 1 0 3.64159265358911 4.27605666738522
0

0101000
+7 6 -9 6 *
Ve
1.00005e-07
286.408212189964 -962.444998719012 -359.300854202584
0 0

0101101
*
Ed
 1e-07 1 1 0
1  8 0 4.71238898038469 6.28318530717959
2  7 1 0 4.71238898038469 6.28318530717959
0

0101000
+5 6 -7 6 *
Ed
 1.00005e-07 1 1 0
1  9 0 3.64159265358848 6.78318530718743
2  8 1 0 3.64159265358848 6.78318530718743
0

0101000
+5 6 -20 6 *
Wi

0101100
+18 0 -16 0 -14 0 -12 0 -10 0 -8 0 -6 0 -4 0 +3 0 *
Fa
0  1e-07 1 0

1101000
+2 0 *

-1 5
0
)OCCT";
}

TopoDS_Shape RoundTripShape(const TopoDS_Shape& theShape,
                            const bool          theBinary,
                            const bool          theWithTriangles,
                            const int           theVersion)
{
  std::stringstream aStream;
  if (theBinary)
  {
    BinTools::Write(theShape,
                    aStream,
                    theWithTriangles,
                    false,
                    static_cast<BinTools_FormatVersion>(theVersion));
  }
  else
  {
    BRepTools::Write(theShape,
                     aStream,
                     theWithTriangles,
                     false,
                     static_cast<TopTools_FormatVersion>(theVersion));
  }
  EXPECT_TRUE(aStream.good());
  EXPECT_FALSE(aStream.str().empty());

  aStream.seekg(0);
  TopoDS_Shape aRestoredShape;
  if (theBinary)
  {
    BinTools::Read(aRestoredShape, aStream);
  }
  else
  {
    BRepTools::Read(aRestoredShape, aStream, BRep_Builder());
  }
  EXPECT_FALSE(aRestoredShape.IsNull());
  return aRestoredShape;
}

int NbTriangles(const TopoDS_Shape& theShape)
{
  int aNbTriangles = 0;
  for (TopExp_Explorer anExplorer(theShape, TopAbs_FACE); anExplorer.More(); anExplorer.Next())
  {
    TopLoc_Location                       aLocation;
    const occ::handle<Poly_Triangulation> aTriangulation =
      BRep_Tool::Triangulation(TopoDS::Face(anExplorer.Current()), aLocation);
    if (!aTriangulation.IsNull())
    {
      aNbTriangles += aTriangulation->NbTriangles();
    }
  }
  return aNbTriangles;
}

int NbSubShapes(const TopoDS_Shape& theShape, const TopAbs_ShapeEnum theType)
{
  int aNbShapes = 0;
  for (TopExp_Explorer anExplorer(theShape, theType); anExplorer.More(); anExplorer.Next())
  {
    ++aNbShapes;
  }
  return aNbShapes;
}

void ExpectEquivalentShape(const TopoDS_Shape& theExpected,
                           const TopoDS_Shape& theActual,
                           const int           theExpectedTriangles)
{
  ASSERT_FALSE(theExpected.IsNull());
  ASSERT_FALSE(theActual.IsNull());
  EXPECT_EQ(theActual.ShapeType(), theExpected.ShapeType());
  EXPECT_EQ(NbSubShapes(theActual, TopAbs_SOLID), NbSubShapes(theExpected, TopAbs_SOLID));
  EXPECT_EQ(NbSubShapes(theActual, TopAbs_SHELL), NbSubShapes(theExpected, TopAbs_SHELL));
  EXPECT_EQ(NbSubShapes(theActual, TopAbs_FACE), NbSubShapes(theExpected, TopAbs_FACE));
  EXPECT_EQ(NbSubShapes(theActual, TopAbs_EDGE), NbSubShapes(theExpected, TopAbs_EDGE));
  EXPECT_EQ(NbSubShapes(theActual, TopAbs_VERTEX), NbSubShapes(theExpected, TopAbs_VERTEX));
  EXPECT_EQ(NbTriangles(theActual), theExpectedTriangles);

  GProp_GProps anExpectedVolume;
  GProp_GProps anActualVolume;
  BRepGProp::VolumeProperties(theExpected, anExpectedVolume);
  BRepGProp::VolumeProperties(theActual, anActualVolume);
  EXPECT_NEAR(anActualVolume.Mass(), anExpectedVolume.Mass(), 1.0e-7);

  GProp_GProps anExpectedSurface;
  GProp_GProps anActualSurface;
  BRepGProp::SurfaceProperties(theExpected, anExpectedSurface);
  BRepGProp::SurfaceProperties(theActual, anActualSurface);
  EXPECT_NEAR(anActualSurface.Mass(), anExpectedSurface.Mass(), 1.0e-7);
}

TopoDS_Shape MakeEmptyShape(const TopAbs_ShapeEnum theType)
{
  BRep_Builder aBuilder;
  switch (theType)
  {
    case TopAbs_VERTEX: {
      TopoDS_Vertex aShape;
      aBuilder.MakeVertex(aShape);
      return aShape;
    }
    case TopAbs_EDGE: {
      TopoDS_Edge aShape;
      aBuilder.MakeEdge(aShape);
      return aShape;
    }
    case TopAbs_WIRE: {
      TopoDS_Wire aShape;
      aBuilder.MakeWire(aShape);
      return aShape;
    }
    case TopAbs_FACE: {
      TopoDS_Face aShape;
      aBuilder.MakeFace(aShape);
      return aShape;
    }
    case TopAbs_SHELL: {
      TopoDS_Shell aShape;
      aBuilder.MakeShell(aShape);
      return aShape;
    }
    case TopAbs_SOLID: {
      TopoDS_Solid aShape;
      aBuilder.MakeSolid(aShape);
      return aShape;
    }
    case TopAbs_COMPSOLID: {
      TopoDS_CompSolid aShape;
      aBuilder.MakeCompSolid(aShape);
      return aShape;
    }
    case TopAbs_COMPOUND: {
      TopoDS_Compound aShape;
      aBuilder.MakeCompound(aShape);
      return aShape;
    }
    default:
      return TopoDS_Shape();
  }
}

TopoDS_Face UniqueFace(const TopoDS_Shape& theShape, const int theIndex)
{
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aFaces;
  TopExp::MapShapes(theShape, TopAbs_FACE, aFaces);
  return theIndex > 0 && theIndex <= aFaces.Extent() ? TopoDS::Face(aFaces(theIndex))
                                                     : TopoDS_Face();
}

TopoDS_Face MakeCircleFace(const gp_Pnt& theCenter, const gp_Dir& theNormal, const double theRadius)
{
  const occ::handle<Geom_Circle> aCircle = new Geom_Circle(gp_Ax2(theCenter, theNormal), theRadius);
  const TopoDS_Edge              aEdge   = BRepBuilderAPI_MakeEdge(aCircle).Edge();
  const TopoDS_Wire              anWire  = BRepBuilderAPI_MakeWire(aEdge).Wire();
  return BRepBuilderAPI_MakeFace(anWire).Face();
}

TopoDS_Face CutAndSelectFirstFace(const TopoDS_Face& theFace, const TopoDS_Face& theTool)
{
  BRepAlgoAPI_Cut aCut(theFace, theTool);
  aCut.Build();
  return aCut.IsDone() ? UniqueFace(aCut.Shape(), 1) : TopoDS_Face();
}

void ExpectBoxUvBounds(const TopoDS_Face& theFace)
{
  double aUMin = 0.0, aUMax = 0.0, aVMin = 0.0, aVMax = 0.0;
  BRepTools::UVBounds(theFace, aUMin, aUMax, aVMin, aVMax);
  EXPECT_NEAR(aUMin, 0.0, 1.e-10);
  EXPECT_NEAR(aUMax, 1500.0, 1.e-10);
  EXPECT_NEAR(aVMin, 0.0, 1.e-10);
  EXPECT_NEAR(aVMax, 1500.0, 1.e-10);
}

} // namespace

// Parallel BRep reading must produce shapes with identical volume properties.
// The underlying bug was that Strtod() shared a single Bigint memory pool
// across all threads, causing random mis-parses of numeric values in .brep
// files when read concurrently.
TEST(BRepTools_Test, ParallelRead_IdenticalMass)
{
  const std::string aBrepData = MakeBoxBrepString();

  // Single-threaded reference
  TopoDS_Shape aRefShape;
  ASSERT_TRUE(ReadFromBuffer(aBrepData, aRefShape));
  GProp_GProps aRefProps;
  BRepGProp::VolumeProperties(aRefShape, aRefProps);
  const double aRefMass = std::abs(aRefProps.Mass());
  ASSERT_GT(aRefMass, 0.0);

  constexpr int aNbReads = 200;

  NCollection_Array1<TopoDS_Shape> aShapes(0, aNbReads - 1);
  std::atomic<int>                 aFailCount{0};

  OSD_Parallel::For(0, aNbReads, [&](int theIndex) {
    TopoDS_Shape aShape;
    if (!ReadFromBuffer(aBrepData, aShape))
    {
      aFailCount.fetch_add(1, std::memory_order_relaxed);
      return;
    }
    aShapes(theIndex) = aShape;
  });

  EXPECT_EQ(aFailCount.load(), 0) << "Some parallel reads failed";

  constexpr double aEps = 1.0e-6;
  for (int i = 0; i < aNbReads; ++i)
  {
    ASSERT_FALSE(aShapes(i).IsNull()) << "Shape " << i << " is null";
    GProp_GProps aProps;
    BRepGProp::VolumeProperties(aShapes(i), aProps);
    const double aMass = std::abs(aProps.Mass());
    EXPECT_NEAR(aMass, aRefMass, aEps)
      << "Shape " << i << " mass " << aMass << " != reference " << aRefMass;
  }
}

// Repeated parallel runs to catch intermittent failures.
TEST(BRepTools_Test, ParallelRead_RepeatedRuns)
{
  const std::string aBrepData = MakeBoxBrepString();

  TopoDS_Shape aRefShape;
  ASSERT_TRUE(ReadFromBuffer(aBrepData, aRefShape));
  GProp_GProps aRefProps;
  BRepGProp::VolumeProperties(aRefShape, aRefProps);
  const double aRefMass = std::abs(aRefProps.Mass());

  constexpr int    aNbReads = 100;
  constexpr int    aNbRuns  = 5;
  constexpr double aEps     = 1.0e-6;

  for (int aRun = 0; aRun < aNbRuns; ++aRun)
  {
    NCollection_Array1<TopoDS_Shape> aShapes(0, aNbReads - 1);
    std::atomic<int>                 aFailCount{0};

    OSD_Parallel::For(0, aNbReads, [&](int theIndex) {
      TopoDS_Shape aShape;
      if (!ReadFromBuffer(aBrepData, aShape))
      {
        aFailCount.fetch_add(1, std::memory_order_relaxed);
        return;
      }
      aShapes(theIndex) = aShape;
    });

    EXPECT_EQ(aFailCount.load(), 0) << "Run " << aRun << ": some reads failed";

    for (int i = 0; i < aNbReads; ++i)
    {
      ASSERT_FALSE(aShapes(i).IsNull()) << "Run " << aRun << ", shape " << i << " is null";
      GProp_GProps aProps;
      BRepGProp::VolumeProperties(aShapes(i), aProps);
      const double aMass = std::abs(aProps.Mass());
      EXPECT_NEAR(aMass, aRefMass, aEps) << "Run " << aRun << ", shape " << i << " mass mismatch";
    }
  }
}

// bugs/moddata_3/bug29723: all supported BRep format versions preserve NURBS geometry.
TEST(BRepTools_Test, ModDataBug_29723_FormatVersions)
{
  const TopoDS_Shape          aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();
  BRepBuilderAPI_NurbsConvert aConverter(aBox);
  const TopoDS_Shape          aNurbsBox = aConverter.Shape();
  ASSERT_FALSE(aNurbsBox.IsNull());

  const TopoDS_Shape anAsciiVersion1 =
    RoundTripShape(aNurbsBox, false, true, TopTools_FormatVersion_VERSION_1);
  const TopoDS_Shape anAsciiVersion2 =
    RoundTripShape(aNurbsBox, false, true, TopTools_FormatVersion_VERSION_2);
  const TopoDS_Shape aBinaryVersion1 =
    RoundTripShape(aNurbsBox, true, true, BinTools_FormatVersion_VERSION_1);
  const TopoDS_Shape aBinaryVersion2 =
    RoundTripShape(aNurbsBox, true, true, BinTools_FormatVersion_VERSION_2);
  const TopoDS_Shape aBinaryVersion3 =
    RoundTripShape(aNurbsBox, true, true, BinTools_FormatVersion_VERSION_3);

  ExpectEquivalentShape(aNurbsBox, anAsciiVersion1, 0);
  ExpectEquivalentShape(aNurbsBox, anAsciiVersion2, 0);
  ExpectEquivalentShape(aNurbsBox, aBinaryVersion1, 0);
  ExpectEquivalentShape(aNurbsBox, aBinaryVersion2, 0);
  ExpectEquivalentShape(aNurbsBox, aBinaryVersion3, 0);
}

// bugs/moddata_3/bug31946: BRep ASCII/binary writers honor triangle options.
TEST(BRepTools_Test, ModDataBug_31946_TriangleOptions)
{
  const TopoDS_Shape       aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();
  BRepMesh_IncrementalMesh aMesher(aBox, 0.1);
  ASSERT_EQ(NbTriangles(aBox), 12);

  const TopoDS_Shape anAsciiWithTriangles =
    RoundTripShape(aBox, false, true, TopTools_FormatVersion_CURRENT);
  const TopoDS_Shape anAsciiWithoutTriangles =
    RoundTripShape(aBox, false, false, TopTools_FormatVersion_CURRENT);
  const TopoDS_Shape aBinaryWithTriangles =
    RoundTripShape(aBox, true, true, BinTools_FormatVersion_CURRENT);
  const TopoDS_Shape aBinaryWithoutTriangles =
    RoundTripShape(aBox, true, false, BinTools_FormatVersion_CURRENT);

  ExpectEquivalentShape(aBox, anAsciiWithTriangles, 12);
  ExpectEquivalentShape(aBox, anAsciiWithoutTriangles, 0);
  ExpectEquivalentShape(aBox, aBinaryWithTriangles, 12);
  ExpectEquivalentShape(aBox, aBinaryWithoutTriangles, 0);
}

// bugs/demo/bug25344_4: a BRep shape remains valid after stream save/restore.
TEST(BRepTools_Test, DemoBug_25344_4_SaveRestoreShapeType)
{
  const TopoDS_Shape aSource = BRepPrimAPI_MakeBox(1.0, 1.0, 1.0).Shape();
  const TopoDS_Shape aRestored =
    RoundTripShape(aSource, false, true, TopTools_FormatVersion_VERSION_2);
  ASSERT_FALSE(aRestored.IsNull());
  EXPECT_EQ(aRestored.ShapeType(), aSource.ShapeType());
  EXPECT_EQ(NbSubShapes(aRestored, TopAbs_SOLID), 1);
  EXPECT_EQ(NbSubShapes(aRestored, TopAbs_FACE), 6);
}

// demo/draw/binpersist_1: a generated box survives binary BRep stream storage with its
// bounding box, topology, validity, and global properties intact.
TEST(BRepTools_Test, DemoBinPersist_1_BinaryStreamRoundTrip)
{
  const TopoDS_Shape aSource =
    BRepPrimAPI_MakeBox(gp_Pnt(10.0, 20.0, 30.0), 100.0, 200.0, 300.0).Shape();
  ASSERT_FALSE(aSource.IsNull());

  std::stringstream aStream;
  BinTools::Write(aSource, aStream, true, false, BinTools_FormatVersion_CURRENT);
  ASSERT_TRUE(aStream.good());
  ASSERT_FALSE(aStream.str().empty());

  aStream.seekg(0);
  TopoDS_Shape aRestored;
  BinTools::Read(aRestored, aStream);
  ASSERT_FALSE(aRestored.IsNull());

  Bnd_Box aSourceBounds;
  Bnd_Box aRestoredBounds;
  BRepBndLib::Add(aSource, aSourceBounds);
  BRepBndLib::Add(aRestored, aRestoredBounds);
  const Bnd_Box::Limits anExpected = aSourceBounds.Get();
  const Bnd_Box::Limits anActual   = aRestoredBounds.Get();
  EXPECT_NEAR(anActual.Xmin, anExpected.Xmin, 1.0e-9);
  EXPECT_NEAR(anActual.Ymin, anExpected.Ymin, 1.0e-9);
  EXPECT_NEAR(anActual.Zmin, anExpected.Zmin, 1.0e-9);
  EXPECT_NEAR(anActual.Xmax, anExpected.Xmax, 1.0e-9);
  EXPECT_NEAR(anActual.Ymax, anExpected.Ymax, 1.0e-9);
  EXPECT_NEAR(anActual.Zmax, anExpected.Zmax, 1.0e-9);

  EXPECT_TRUE(BRepCheck_Analyzer(aRestored).IsValid());
  EXPECT_EQ(NbSubShapes(aRestored, TopAbs_SOLID), NbSubShapes(aSource, TopAbs_SOLID));
  EXPECT_EQ(NbSubShapes(aRestored, TopAbs_SHELL), NbSubShapes(aSource, TopAbs_SHELL));
  EXPECT_EQ(NbSubShapes(aRestored, TopAbs_FACE), NbSubShapes(aSource, TopAbs_FACE));
  EXPECT_EQ(NbSubShapes(aRestored, TopAbs_EDGE), NbSubShapes(aSource, TopAbs_EDGE));
  EXPECT_EQ(NbSubShapes(aRestored, TopAbs_VERTEX), NbSubShapes(aSource, TopAbs_VERTEX));

  GProp_GProps aSourceLinear;
  GProp_GProps aRestoredLinear;
  BRepGProp::LinearProperties(aSource, aSourceLinear);
  BRepGProp::LinearProperties(aRestored, aRestoredLinear);
  EXPECT_NEAR(aRestoredLinear.Mass(), aSourceLinear.Mass(), 1.0e-7);
  EXPECT_DOUBLE_EQ(aSourceLinear.Mass(), 4800.0);

  GProp_GProps aSourceSurface;
  GProp_GProps aRestoredSurface;
  BRepGProp::SurfaceProperties(aSource, aSourceSurface);
  BRepGProp::SurfaceProperties(aRestored, aRestoredSurface);
  EXPECT_NEAR(aRestoredSurface.Mass(), aSourceSurface.Mass(), 1.0e-7);
  EXPECT_DOUBLE_EQ(aSourceSurface.Mass(), 220000.0);

  GProp_GProps aSourceVolume;
  GProp_GProps aRestoredVolume;
  BRepGProp::VolumeProperties(aSource, aSourceVolume);
  BRepGProp::VolumeProperties(aRestored, aRestoredVolume);
  EXPECT_NEAR(aRestoredVolume.Mass(), aSourceVolume.Mass(), 1.0e-7);
  EXPECT_DOUBLE_EQ(aSourceVolume.Mass(), 6000000.0);
}

// bugs/moddata_1/bug20616: every empty BRep topology type survives DRAW-style storage.
TEST(BRepTools_Test, ModDataBug_20616_EmptyShapeSaveRestore)
{
  NCollection_Array1<TopAbs_ShapeEnum> aTypes(1, 8);
  aTypes(1) = TopAbs_VERTEX;
  aTypes(2) = TopAbs_EDGE;
  aTypes(3) = TopAbs_WIRE;
  aTypes(4) = TopAbs_FACE;
  aTypes(5) = TopAbs_SHELL;
  aTypes(6) = TopAbs_SOLID;
  aTypes(7) = TopAbs_COMPSOLID;
  aTypes(8) = TopAbs_COMPOUND;

  for (NCollection_Array1<TopAbs_ShapeEnum>::Iterator anIterator(aTypes); anIterator.More();
       anIterator.Next())
  {
    const TopoDS_Shape aSource = MakeEmptyShape(anIterator.Value());
    ASSERT_FALSE(aSource.IsNull());

    std::stringstream aStream;
    BRepTools::Write(aSource, aStream);
    ASSERT_TRUE(aStream.good());
    ASSERT_FALSE(aStream.str().empty());

    aStream.seekg(0);
    TopoDS_Shape aRestored;
    BRep_Builder aBuilder;
    BRepTools::Read(aRestored, aStream, aBuilder);
    ASSERT_FALSE(aRestored.IsNull());
    EXPECT_EQ(aRestored.ShapeType(), anIterator.Value());
  }
}

// bugs/modalg_7/optimal_bndbox_exception: an embedded BRep shape is safe for AddOptimal.
TEST(BRepTools_Test, ModalgBug_32236_OptimalBoundingBox)
{
  const char* aShapeData = R"BREP(CASCADE Topology V1, (c) Matra-Datavision
Locations 0
Curve2ds 4
1 -1.3333333333333335 0 0 1
1 0 -1.6666666666666667 1 0
1 1.6666666666666665 0 0 1
1 0 1.3333333333333333 1 0
Curves 0
Polygon3D 0
PolygonOnTriangulations 0
Surfaces 1
9 0 0 0 0 3 3 4 4 2 2 0 3 1.9999999999999998  0 1.6666666666666665 1.9999999999999998  0 0.3333333333333332 1.9999999999999996  0 -0.99999999999999989 2
0 2.9999999999999991 0.66666666666666718  0 1.6666666666666659 0.66666666666666741  0 0.3333333333333332 0.66666666666666663  0 -0.99999999999999956 0.66666666666666663
0 3.0000000000000009 -0.66666666666666718  0 1.6666666666666676 -0.66666666666666752  0 0.33333333333333331 -0.66666666666666652  0 -1.0000000000000002 -0.66666666666666685
0 3.0000000000000004 -2  0 1.6666666666666667 -2  0 0.33333333333333326 -1.9999999999999996  0 -1 -2

-1.3333333333333335 4
2.6666666666666665 4

-2.666666666666667 4
1.3333333333333333 4

Triangulations 0

TShapes 8
Ve
4.000004
0 -1 -1
0 0

0101101
*
Ed
 4.000004 1 1 1
2  1 1 0 -1.66666666666667 1.33333333333333
0

0101000
+8 0 -8 0 *
Ed
 4.000004 1 1 1
2  2 1 0 -1.33333333333333 1.66666666666667
0

0101000
+8 0 -8 0 *
Ed
 4.000004 1 1 1
2  3 1 0 -1.66666666666667 1.33333333333333
0

0101000
+8 0 -8 0 *
Ed
 4.000004 1 1 1
2  4 1 0 -1.33333333333333 1.66666666666667
0

0101000
+8 0 -8 0 *
Wi

0101100
-7 0 +6 0 +5 0 -4 0 *
Fa
0  1e-07 1 0

0101000
+3 0 *
Sh

1101000
+2 0 *

+1 0
)BREP";

  std::istringstream aStream(aShapeData);
  BRep_Builder       aBuilder;
  TopoDS_Shape       aShape;
  BRepTools::Read(aShape, aStream, aBuilder);
  ASSERT_FALSE(aShape.IsNull());

  Bnd_Box aBox;
  EXPECT_NO_THROW(BRepBndLib::AddOptimal(aShape, aBox, true, false));
  EXPECT_FALSE(aBox.IsVoid());
}

// bugs/modalg_7/bug30269: a BRep face with missing 2D curves must not crash
// the ordinary, optimal, or oriented bounding-box calculations.
TEST(BRepTools_Test, ModalgBug_30269_MissingCurve2dBounding)
{
  TopoDS_Shape aShape;
  ASSERT_TRUE(ReadFromBuffer(Bug30269BrepData(), aShape));

  Bnd_Box aRegularBox;
  Bnd_Box anOptimalBox;
  Bnd_OBB anOrientedBox;
  EXPECT_NO_THROW(BRepBndLib::Add(aShape, aRegularBox));
  EXPECT_NO_THROW(BRepBndLib::AddOptimal(aShape, anOptimalBox));
  EXPECT_NO_THROW(BRepBndLib::AddOBB(aShape, anOrientedBox));
  EXPECT_FALSE(aRegularBox.IsVoid());
  EXPECT_FALSE(anOptimalBox.IsVoid());
  EXPECT_FALSE(anOrientedBox.IsVoid());
}

// bugs/modalg_7/bug29939: global properties must tolerate the same missing
// Curve2d records without raising Standard_NullObject.
TEST(BRepTools_Test, ModalgBug_29939_MissingCurve2dProperties)
{
  TopoDS_Shape aShape;
  ASSERT_TRUE(ReadFromBuffer(Bug29939BrepData(), aShape));

  GProp_GProps aSurfaceProperties;
  GProp_GProps aVolumeProperties;
  EXPECT_NO_THROW(BRepGProp::SurfaceProperties(aShape, aSurfaceProperties));
  EXPECT_NO_THROW(BRepGProp::VolumeProperties(aShape, aVolumeProperties));
  EXPECT_TRUE(std::isfinite(aSurfaceProperties.Mass()));
  EXPECT_TRUE(std::isfinite(aVolumeProperties.Mass()));
}

namespace
{
static void ExpectSamePrincipalProperties(const GProp_GProps& theExact,
                                          const GProp_GProps& theTriangulated)
{
  EXPECT_NEAR(theExact.Mass(), theTriangulated.Mass(), 1.0e-7);

  GProp_PrincipalProps anExactPrincipal       = theExact.PrincipalProperties();
  GProp_PrincipalProps aTriangulatedPrincipal = theTriangulated.PrincipalProperties();
  double               anExactIx              = 0.0;
  double               anExactIy              = 0.0;
  double               anExactIz              = 0.0;
  double               aTriangulatedIx        = 0.0;
  double               aTriangulatedIy        = 0.0;
  double               aTriangulatedIz        = 0.0;
  anExactPrincipal.Moments(anExactIx, anExactIy, anExactIz);
  aTriangulatedPrincipal.Moments(aTriangulatedIx, aTriangulatedIy, aTriangulatedIz);
  EXPECT_NEAR(anExactIx, aTriangulatedIx, 1.0e-7);
  EXPECT_NEAR(anExactIy, aTriangulatedIy, 1.0e-7);
  EXPECT_NEAR(anExactIz, aTriangulatedIz, 1.0e-7);
}

static void CompareExactAndTriangulatedProperties(const TopoDS_Shape& theShape,
                                                  const int           thePropertyType)
{
  GProp_GProps anExactProperties;
  GProp_GProps aTriangulatedProperties;
  switch (thePropertyType)
  {
    case 0:
      BRepGProp::LinearProperties(theShape, anExactProperties, false, false);
      BRepGProp::LinearProperties(theShape, aTriangulatedProperties, false, true);
      break;
    case 1:
      BRepGProp::SurfaceProperties(theShape, anExactProperties, false, false);
      BRepGProp::SurfaceProperties(theShape, aTriangulatedProperties, false, true);
      break;
    default:
      BRepGProp::VolumeProperties(theShape, anExactProperties, false, false, false);
      BRepGProp::VolumeProperties(theShape, aTriangulatedProperties, false, false, true);
      break;
  }
  ExpectSamePrincipalProperties(anExactProperties, aTriangulatedProperties);
}
} // namespace

// bugs/modalg_7/bug29734: global properties computed from planar face
// triangulations must agree with the exact-geometry calculations.
TEST(BRepTools_Test, ModalgBug_29734_ExactAndTriangulatedProperties)
{
  const TopoDS_Shape aFirstBox  = BRepPrimAPI_MakeBox(1.0, 2.0, 3.0).Shape();
  const TopoDS_Shape aSecondBox = BRepPrimAPI_MakeBox(3.0, 2.0, 1.0).Shape();

  gp_Trsf aTranslation;
  aTranslation.SetTranslation(gp_Vec(0.5, 0.5, 0.5));
  const TopoDS_Shape aTranslatedBox = BRepBuilderAPI_Transform(aSecondBox, aTranslation).Shape();

  gp_Trsf aRotation;
  aRotation.SetRotation(gp_Ax1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 1.0, 1.0)), M_PI / 6.0);
  const TopoDS_Shape aRotatedBox = BRepBuilderAPI_Transform(aTranslatedBox, aRotation).Shape();
  const TopoDS_Shape aFusedShape = BRepAlgoAPI_Fuse(aFirstBox, aRotatedBox).Shape();
  ASSERT_FALSE(aFusedShape.IsNull());

  BRepMesh_IncrementalMesh aMesh(aFusedShape, 0.01);
  ASSERT_TRUE(aMesh.IsDone());
  EXPECT_TRUE(BRepCheck_Analyzer(aFusedShape).IsValid());

  CompareExactAndTriangulatedProperties(aFusedShape, 0);
  CompareExactAndTriangulatedProperties(aFusedShape, 1);
  CompareExactAndTriangulatedProperties(aFusedShape, 2);
}

// bugs/demo/bug23130: maxtolerance reports all box subshape counts and their
// default tolerances consistently on every supported platform.
TEST(BRepTools_Test, DemoBug_23130_BoxMaxTolerance)
{
  const TopoDS_Shape     aBox               = BRepPrimAPI_MakeBox(100.0, 100.0, 100.0).Shape();
  const TopAbs_ShapeEnum aTypes[]           = {TopAbs_FACE, TopAbs_EDGE, TopAbs_VERTEX};
  const int              anExpectedCounts[] = {6, 12, 8};

  for (int aTypeIndex = 0; aTypeIndex < 3; ++aTypeIndex)
  {
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aShapeMap;
    double                                                 aMinTolerance = 1.0e100;
    double                                                 aMaxTolerance = -1.0;
    for (TopExp_Explorer anExplorer(aBox, aTypes[aTypeIndex]); anExplorer.More(); anExplorer.Next())
    {
      double aTolerance = 0.0;
      switch (aTypes[aTypeIndex])
      {
        case TopAbs_FACE:
          aTolerance = BRep_Tool::Tolerance(TopoDS::Face(anExplorer.Current()));
          break;
        case TopAbs_EDGE:
          aTolerance = BRep_Tool::Tolerance(TopoDS::Edge(anExplorer.Current()));
          break;
        case TopAbs_VERTEX:
          aTolerance = BRep_Tool::Tolerance(TopoDS::Vertex(anExplorer.Current()));
          break;
        default:
          break;
      }
      aShapeMap.Add(anExplorer.Current());
      aMinTolerance = (std::min)(aMinTolerance, aTolerance);
      aMaxTolerance = (std::max)(aMaxTolerance, aTolerance);
    }

    EXPECT_EQ(aShapeMap.Extent(), anExpectedCounts[aTypeIndex]);
    EXPECT_NEAR(aMinTolerance, 1.0e-7, 1.0e-7);
    EXPECT_NEAR(aMaxTolerance, 1.0e-7, 1.0e-7);
  }
}

// Migrated from tests/bugs/moddata_3/bug27356. Clean() must remove a free
// edge's 3D polygon while leaving the topological edge itself usable.
TEST(BRepTools_Test, ModDataBug_27356_CleanRemovesFreeEdgePolygon3d)
{
  const TopoDS_Edge anEdge =
    BRepBuilderAPI_MakeEdge(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(1.0, 0.0, 0.0)).Edge();
  ASSERT_FALSE(anEdge.IsNull());

  BRepMesh_IncrementalMesh aMesher(anEdge, 0.01);
  ASSERT_TRUE(aMesher.IsDone());

  TopLoc_Location aLocation;
  ASSERT_FALSE(BRep_Tool::Polygon3D(anEdge, aLocation).IsNull());

  BRepTools::Clean(anEdge);

  EXPECT_TRUE(BRep_Tool::Polygon3D(anEdge, aLocation).IsNull());
}

// Migrated from tests/bugs/modalg_5/bug24404.  UV bounds of the selected box
// face must remain its full parametric limits after the successive cuts.
TEST(BRepTools_Test, ModalgBug_24404_BoxFaceUVBounds)
{
  const TopoDS_Shape aBox  = BRepPrimAPI_MakeBox(1500.0, 1500.0, 1500.0).Shape();
  TopoDS_Face        aFace = UniqueFace(aBox, 4);
  ASSERT_FALSE(aFace.IsNull());
  ExpectBoxUvBounds(aFace);

  const gp_Dir aNormal(0.0, 1.0, 0.0);
  aFace = CutAndSelectFirstFace(
    aFace,
    MakeCircleFace(gp_Pnt(1361.60462531413, 1500.0, 275.105307765905), aNormal, 182.781239888725));
  ASSERT_FALSE(aFace.IsNull());
  ExpectBoxUvBounds(aFace);

  aFace = CutAndSelectFirstFace(
    aFace,
    MakeCircleFace(gp_Pnt(398.623694869499, 1500.0, 5.77182937332096), aNormal, 181.948898616306));
  ASSERT_FALSE(aFace.IsNull());
  ExpectBoxUvBounds(aFace);

  aFace = CutAndSelectFirstFace(
    aFace,
    MakeCircleFace(gp_Pnt(66.2457890511211, 1500.0, 832.500519113239), aNormal, 176.393473166564));
  ASSERT_FALSE(aFace.IsNull());
  ExpectBoxUvBounds(aFace);

  aFace = CutAndSelectFirstFace(
    aFace,
    MakeCircleFace(gp_Pnt(1177.73545803307, 1500.0, 1406.03245550006), aNormal, 150.768085993996));
  ASSERT_FALSE(aFace.IsNull());
  ExpectBoxUvBounds(aFace);
}
