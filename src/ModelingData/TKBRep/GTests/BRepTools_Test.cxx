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
