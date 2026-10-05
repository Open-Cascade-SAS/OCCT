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

#include "BOPTest_Utilities.pxx"

#include <algorithm>
#include <cmath>

#include <BOPAlgo_MakeConnected.hxx>
#include <BOPAlgo_MakePeriodic.hxx>
#include <BOPAlgo_MakerVolume.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_NurbsConvert.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepGProp.hxx>
#include <BRepTools_History.hxx>

#include <GProp_GProps.hxx>

#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_TrimmedCurve.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_Map.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

namespace
{
int CountShapes(const TopoDS_Shape& theShape, const TopAbs_ShapeEnum theType)
{
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMap;
  TopExp::MapShapes(theShape, aMap);

  int aCount = 0;
  for (int anIndex = 1; anIndex <= aMap.Extent(); ++anIndex)
  {
    if (aMap(anIndex).ShapeType() == theType)
    {
      ++aCount;
    }
  }
  return aCount;
}

int CountShapes(const NCollection_List<TopoDS_Shape>& theShapes, const TopAbs_ShapeEnum theType)
{
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMap;
  for (NCollection_List<TopoDS_Shape>::Iterator anIterator(theShapes); anIterator.More();
       anIterator.Next())
  {
    TopExp::MapShapes(anIterator.Value(), aMap);
  }

  int aCount = 0;
  for (int anIndex = 1; anIndex <= aMap.Extent(); ++anIndex)
  {
    if (aMap(anIndex).ShapeType() == theType)
    {
      ++aCount;
    }
  }
  return aCount;
}

TopoDS_Compound MakeCompound(const NCollection_List<TopoDS_Shape>& theShapes)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  for (NCollection_List<TopoDS_Shape>::Iterator anIterator(theShapes); anIterator.More();
       anIterator.Next())
  {
    aBuilder.Add(aCompound, anIterator.Value());
  }
  return aCompound;
}

void ExpectShapeCounts(const TopoDS_Shape& theShape,
                       const int           theVertices,
                       const int           theEdges,
                       const int           theWires,
                       const int           theFaces,
                       const int           theShells,
                       const int           theSolids)
{
  EXPECT_FALSE(theShape.IsNull());
  if (theShape.IsNull())
  {
    return;
  }
  EXPECT_TRUE(BRepCheck_Analyzer(theShape, true, false, false).IsValid());
  EXPECT_EQ(CountShapes(theShape, TopAbs_VERTEX), theVertices);
  EXPECT_EQ(CountShapes(theShape, TopAbs_EDGE), theEdges);
  EXPECT_EQ(CountShapes(theShape, TopAbs_WIRE), theWires);
  EXPECT_EQ(CountShapes(theShape, TopAbs_FACE), theFaces);
  EXPECT_EQ(CountShapes(theShape, TopAbs_SHELL), theShells);
  EXPECT_EQ(CountShapes(theShape, TopAbs_SOLID), theSolids);
}

void ExpectWireFaceShellSolidCounts(const TopoDS_Shape& theShape,
                                    const int           theWires,
                                    const int           theFaces,
                                    const int           theShells,
                                    const int           theSolids)
{
  EXPECT_FALSE(theShape.IsNull());
  if (theShape.IsNull())
  {
    return;
  }
  EXPECT_TRUE(BRepCheck_Analyzer(theShape, true, false, false).IsValid());
  EXPECT_EQ(CountShapes(theShape, TopAbs_WIRE), theWires);
  EXPECT_EQ(CountShapes(theShape, TopAbs_FACE), theFaces);
  EXPECT_EQ(CountShapes(theShape, TopAbs_SHELL), theShells);
  EXPECT_EQ(CountShapes(theShape, TopAbs_SOLID), theSolids);
}

void ExpectValidShape(const TopoDS_Shape& theShape)
{
  EXPECT_FALSE(theShape.IsNull());
  if (!theShape.IsNull())
  {
    EXPECT_TRUE(BRepCheck_Analyzer(theShape, true, false, false).IsValid());
  }
}

void ExpectMaterials(const NCollection_List<TopoDS_Shape>& theActual,
                     const TopoDS_Shape&                   theExpected1,
                     const TopoDS_Shape&                   theExpected2 = TopoDS_Shape())
{
  const int anExpectedExtent = theExpected2.IsNull() ? 1 : 2;
  EXPECT_EQ(theActual.Extent(), anExpectedExtent);
  for (NCollection_List<TopoDS_Shape>::Iterator anIterator(theActual); anIterator.More();
       anIterator.Next())
  {
    const bool isExpected = anIterator.Value().IsSame(theExpected1)
                            || (!theExpected2.IsNull() && anIterator.Value().IsSame(theExpected2));
    EXPECT_TRUE(isExpected);
  }
}

void ExpectEqualProperties(const TopoDS_Shape& theFirst,
                           const TopoDS_Shape& theSecond,
                           const bool          theSkipShared)
{
  GProp_GProps aFirstProperties;
  GProp_GProps aSecondProperties;
  BRepGProp::LinearProperties(theFirst, aFirstProperties, theSkipShared);
  BRepGProp::LinearProperties(theSecond, aSecondProperties, theSkipShared);
  EXPECT_NEAR(aFirstProperties.Mass(),
              aSecondProperties.Mass(),
              1.0e-6 * std::max(1.0, std::abs(aSecondProperties.Mass())));

  aFirstProperties  = GProp_GProps();
  aSecondProperties = GProp_GProps();
  BRepGProp::SurfaceProperties(theFirst, aFirstProperties, 1.0, theSkipShared);
  BRepGProp::SurfaceProperties(theSecond, aSecondProperties, 1.0, theSkipShared);
  EXPECT_NEAR(aFirstProperties.Mass(),
              aSecondProperties.Mass(),
              1.0e-6 * std::max(1.0, std::abs(aSecondProperties.Mass())));

  aFirstProperties  = GProp_GProps();
  aSecondProperties = GProp_GProps();
  BRepGProp::VolumeProperties(theFirst, aFirstProperties, 1.0, false, theSkipShared);
  BRepGProp::VolumeProperties(theSecond, aSecondProperties, 1.0, false, theSkipShared);
  EXPECT_NEAR(aFirstProperties.Mass(),
              aSecondProperties.Mass(),
              1.0e-6 * std::max(1.0, std::abs(aSecondProperties.Mass())));
}

NCollection_List<TopoDS_Face> GetFaces(const TopoDS_Shape& theShape);

void ExpectModifiedMaterials(BOPAlgo_MakeConnected&                theConnected,
                             const NCollection_List<TopoDS_Shape>& theModified,
                             const TopoDS_Shape&                   thePositive1,
                             const TopoDS_Shape&                   thePositive2,
                             const TopoDS_Shape&                   theNegative1,
                             const TopoDS_Shape&                   theNegative2 = TopoDS_Shape())
{
  const TopoDS_Shape                  aModifiedShape = MakeCompound(theModified);
  const NCollection_List<TopoDS_Face> aModifiedFaces = GetFaces(aModifiedShape);
  ASSERT_FALSE(aModifiedFaces.IsEmpty());

  bool hasPositiveMaterials = false;
  bool hasNegativeMaterials = false;
  for (NCollection_List<TopoDS_Face>::Iterator anIterator(aModifiedFaces); anIterator.More();
       anIterator.Next())
  {
    const NCollection_List<TopoDS_Shape>& aPositive =
      theConnected.MaterialsOnPositiveSide(anIterator.Value());
    const NCollection_List<TopoDS_Shape>& aNegative =
      theConnected.MaterialsOnNegativeSide(anIterator.Value());
    if (!aPositive.IsEmpty())
    {
      hasPositiveMaterials = true;
      ExpectMaterials(aPositive, thePositive1, thePositive2);
    }
    if (!aNegative.IsEmpty())
    {
      hasNegativeMaterials = true;
      ExpectMaterials(aNegative, theNegative1, theNegative2);
    }
  }

  EXPECT_TRUE(hasPositiveMaterials);
  EXPECT_EQ(hasNegativeMaterials, !theNegative1.IsNull());
}

TopoDS_Shape MakePeriodicityProfile()
{
  BRepBuilderAPI_MakePolygon aPolygon;
  aPolygon.Add(gp_Pnt(0.0, 0.0, 0.0));
  aPolygon.Add(gp_Pnt(10.0, 0.0, 0.0));
  aPolygon.Add(gp_Pnt(10.0, 0.0, 10.0));
  aPolygon.Add(gp_Pnt(5.0, 0.0, 10.0));
  aPolygon.Add(gp_Pnt(5.0, 0.0, 5.0));
  aPolygon.Add(gp_Pnt(0.0, 0.0, 5.0));
  aPolygon.Close();

  const TopoDS_Face aFace = BRepBuilderAPI_MakeFace(aPolygon.Wire());
  return BRepPrimAPI_MakePrism(aFace, gp_Vec(0.0, 5.0, 0.0)).Shape();
}

NCollection_List<TopoDS_Face> GetFaces(const TopoDS_Shape& theShape)
{
  NCollection_List<TopoDS_Face>                          aFaces;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMap;
  aMap.Add(theShape);
  for (TopExp_Explorer anExplorer(theShape, TopAbs_FACE); anExplorer.More(); anExplorer.Next())
  {
    if (aMap.Add(anExplorer.Current()))
    {
      aFaces.Append(TopoDS::Face(anExplorer.Current()));
    }
  }
  return aFaces;
}

NCollection_List<TopoDS_Edge> GetEdges(const TopoDS_Shape& theShape)
{
  NCollection_List<TopoDS_Edge>                          anEdges;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMap;
  aMap.Add(theShape);
  for (TopExp_Explorer anExplorer(theShape, TopAbs_EDGE); anExplorer.More(); anExplorer.Next())
  {
    if (aMap.Add(anExplorer.Current()))
    {
      anEdges.Append(TopoDS::Edge(anExplorer.Current()));
    }
  }
  return anEdges;
}

TopoDS_Face GetFace(const TopoDS_Shape& theShape, const int theIndex)
{
  int anIndex = 0;
  for (TopExp_Explorer anExplorer(theShape, TopAbs_FACE); anExplorer.More(); anExplorer.Next())
  {
    if (anIndex++ == theIndex)
    {
      return TopoDS::Face(anExplorer.Current());
    }
  }
  return TopoDS_Face();
}

occ::handle<Geom_Curve> BasisCurve(const TopoDS_Edge& theEdge)
{
  double                  aFirst = 0.0;
  double                  aLast  = 0.0;
  occ::handle<Geom_Curve> aCurve = BRep_Tool::Curve(theEdge, aFirst, aLast);
  while (!aCurve.IsNull())
  {
    const occ::handle<Geom_TrimmedCurve> aTrimmed = occ::down_cast<Geom_TrimmedCurve>(aCurve);
    if (aTrimmed.IsNull())
    {
      break;
    }
    aCurve = aTrimmed->BasisCurve();
  }
  return aCurve;
}

const char* CurveTypeName(const TopoDS_Edge& theEdge)
{
  const occ::handle<Geom_Curve> aCurve = BasisCurve(theEdge);
  return aCurve.IsNull() ? nullptr : aCurve->DynamicType()->Name();
}

bool IsBSplineCurve(const TopoDS_Edge& theEdge)
{
  return !occ::down_cast<Geom_BSplineCurve>(BasisCurve(theEdge)).IsNull();
}

occ::handle<Geom_Surface> BasisSurface(const TopoDS_Face& theFace)
{
  occ::handle<Geom_Surface> aSurface = BRep_Tool::Surface(theFace);
  while (!aSurface.IsNull())
  {
    const occ::handle<Geom_RectangularTrimmedSurface> aTrimmed =
      occ::down_cast<Geom_RectangularTrimmedSurface>(aSurface);
    if (aTrimmed.IsNull())
    {
      break;
    }
    aSurface = aTrimmed->BasisSurface();
  }
  return aSurface;
}

const char* SurfaceTypeName(const TopoDS_Face& theFace)
{
  const occ::handle<Geom_Surface> aSurface = BasisSurface(theFace);
  return aSurface.IsNull() ? nullptr : aSurface->DynamicType()->Name();
}

bool IsBSplineSurface(const TopoDS_Face& theFace)
{
  return !occ::down_cast<Geom_BSplineSurface>(BasisSurface(theFace)).IsNull();
}

occ::handle<BRepTools_History> MakeVolume(BOPAlgo_MakerVolume&                 theMaker,
                                          const TopoDS_Shape&                  theModifiedShape,
                                          const NCollection_List<TopoDS_Face>& theFaces)
{
  NCollection_List<TopoDS_Shape> anArguments;
  anArguments.Append(theModifiedShape);
  for (NCollection_List<TopoDS_Face>::Iterator anIterator(theFaces); anIterator.More();
       anIterator.Next())
  {
    anArguments.Append(anIterator.Value());
  }

  theMaker.SetArguments(anArguments);
  theMaker.SetToFillHistory(true);
  theMaker.SetFuzzyValue(Precision::Confusion());
  theMaker.SetNonDestructive(false);
  theMaker.SetIntersect(true);
  theMaker.SetRunParallel(false);
  theMaker.Perform();
  return new BRepTools_History(anArguments, theMaker);
}

void MakeBoxPeriodic(BOPAlgo_MakePeriodic& theMaker,
                     const TopoDS_Shape&   theShape,
                     const double          theXPeriod,
                     const double          theYPeriod,
                     const double          theZPeriod,
                     const bool            theIsInputTrimmedX = true,
                     const bool            theIsInputTrimmedY = true,
                     const bool            theIsInputTrimmedZ = true,
                     const double          theXFirst          = 0.0,
                     const double          theYFirst          = 0.0,
                     const double          theZFirst          = 0.0,
                     const bool            theSetTrimmed      = true)
{
  theMaker.SetShape(theShape);
  theMaker.MakeXPeriodic(true, theXPeriod);
  theMaker.MakeYPeriodic(true, theYPeriod);
  theMaker.MakeZPeriodic(true, theZPeriod);
  if (theSetTrimmed)
  {
    theMaker.SetTrimmed(0, theIsInputTrimmedX, theXFirst);
    theMaker.SetTrimmed(1, theIsInputTrimmedY, theYFirst);
    theMaker.SetTrimmed(2, theIsInputTrimmedZ, theZFirst);
  }
  theMaker.SetRunParallel(false);
  theMaker.Perform();
}

BOPAlgo_MakePeriodic::PeriodicityParams MakeXYZPeriodicity(const double theXPeriod,
                                                           const double theYPeriod,
                                                           const double theZPeriod)
{
  BOPAlgo_MakePeriodic::PeriodicityParams aParams;
  aParams.myPeriodic[0] = true;
  aParams.myPeriodic[1] = true;
  aParams.myPeriodic[2] = true;
  aParams.myPeriod[0]   = theXPeriod;
  aParams.myPeriod[1]   = theYPeriod;
  aParams.myPeriod[2]   = theZPeriod;
  return aParams;
}

void PerformConnected(BOPAlgo_MakeConnected&                theConnected,
                      const NCollection_List<TopoDS_Shape>& theArguments)
{
  theConnected.SetArguments(theArguments);
  theConnected.SetRunParallel(false);
  theConnected.Perform();
}
} // namespace

// Equivalent to tests/boolean/periodicity/A1.
TEST(BOPAlgo_MakePeriodicTest, A1_BoxWithTrimmedPeriodicity_RepeatsWithExpectedTopology)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();

  BOPAlgo_MakePeriodic aMaker;
  MakeBoxPeriodic(aMaker, aBox, 5.0, 8.0, 12.0, false, false, false, 2.0, 1.0, -1.0);

  ASSERT_FALSE(aMaker.HasErrors());
  const TopoDS_Shape& aPeriodicShape = aMaker.Shape();
  ASSERT_FALSE(aPeriodicShape.IsNull());
  ExpectShapeCounts(aPeriodicShape, 8, 12, 6, 6, 1, 1);
  EXPECT_NEAR(BOPTest_Utilities::GetSurfaceArea(aPeriodicShape), 340.0, Precision::Confusion());
  EXPECT_NEAR(BOPTest_Utilities::GetVolume(aPeriodicShape), 400.0, Precision::Confusion());

  aMaker.XRepeat(5);
  aMaker.YRepeat(5);
  aMaker.ZRepeat(5);
  const TopoDS_Shape& aRepeatedShape = aMaker.RepeatedShape();
  ASSERT_FALSE(aRepeatedShape.IsNull());
  ExpectShapeCounts(aRepeatedShape, 588, 1302, 936, 936, 216, 216);
  EXPECT_NEAR(BOPTest_Utilities::GetSurfaceArea(aRepeatedShape), 73440.0, Precision::Confusion());
  EXPECT_NEAR(BOPTest_Utilities::GetVolume(aRepeatedShape), 86400.0, Precision::Confusion());
}

// Equivalent to tests/boolean/periodicity/A2.
TEST(BOPAlgo_MakePeriodicTest, A2_ProfilePeriodicity_HistoryAndRepetitionAreConsistent)
{
  const TopoDS_Shape aShape = MakePeriodicityProfile();

  BOPAlgo_MakePeriodic aMaker;
  aMaker.SetShape(aShape);
  aMaker.MakeXPeriodic(true, 10.0);
  aMaker.MakeZPeriodic(true, 10.0);
  aMaker.MakeYPeriodic(true, 5.0);
  aMaker.SetRunParallel(false);
  aMaker.Perform();

  ASSERT_FALSE(aMaker.HasErrors());
  const TopoDS_Shape& aPeriodicShape = aMaker.Shape();
  ASSERT_FALSE(aPeriodicShape.IsNull());
  ExpectShapeCounts(aPeriodicShape, 16, 24, 10, 10, 1, 1);
  EXPECT_NEAR(BOPTest_Utilities::GetSurfaceArea(aPeriodicShape), 350.0, Precision::Confusion());
  EXPECT_NEAR(BOPTest_Utilities::GetVolume(aPeriodicShape), 375.0, Precision::Confusion());
  ASSERT_FALSE(aMaker.History().IsNull());

  const BRepTools_History&            aHistory = *aMaker.History();
  const NCollection_List<TopoDS_Face> aFaces   = GetFaces(aShape);
  ASSERT_GE(aFaces.Extent(), 3);

  for (int anIndex = 0; anIndex < 2; ++anIndex)
  {
    const TopoDS_Face aFace = GetFace(aShape, anIndex);
    ASSERT_FALSE(aFace.IsNull());
    const NCollection_List<TopoDS_Shape>& aModified = aHistory.Modified(aFace);
    EXPECT_EQ(CountShapes(aModified, TopAbs_VERTEX), 6);
    EXPECT_EQ(CountShapes(aModified, TopAbs_EDGE), 7);
    EXPECT_EQ(CountShapes(aModified, TopAbs_WIRE), 2);
    EXPECT_EQ(CountShapes(aModified, TopAbs_FACE), 2);

    const TopoDS_Shape                  aModifiedShape = MakeCompound(aModified);
    const NCollection_List<TopoDS_Face> aModifiedFaces = GetFaces(aModifiedShape);
    ASSERT_EQ(aModifiedFaces.Extent(), 2);
    NCollection_List<TopoDS_Shape> aTwins;
    for (NCollection_List<TopoDS_Face>::Iterator anIterator(aModifiedFaces); anIterator.More();
         anIterator.Next())
    {
      const NCollection_List<TopoDS_Shape>& aFaceTwins = aMaker.GetTwins(anIterator.Value());
      for (NCollection_List<TopoDS_Shape>::Iterator aTwinIterator(aFaceTwins); aTwinIterator.More();
           aTwinIterator.Next())
      {
        aTwins.Append(aTwinIterator.Value());
      }
    }
    EXPECT_EQ(CountShapes(aTwins, TopAbs_FACE), 1);
  }

  aMaker.XRepeat(1);
  aMaker.YRepeat(2);
  const TopoDS_Shape& aRepeatedShape = aMaker.RepeatedShape();
  ASSERT_FALSE(aRepeatedShape.IsNull());
  ExpectShapeCounts(aRepeatedShape, 56, 102, 53, 53, 6, 6);
  EXPECT_NEAR(BOPTest_Utilities::GetSurfaceArea(aRepeatedShape), 2100.0, Precision::Confusion());
  EXPECT_NEAR(BOPTest_Utilities::GetVolume(aRepeatedShape), 2250.0, Precision::Confusion());

  const TopoDS_Face aBottomFace = GetFace(aShape, 0);
  const TopoDS_Face aTopFace    = GetFace(aShape, 2);
  ASSERT_FALSE(aBottomFace.IsNull());
  ASSERT_FALSE(aTopFace.IsNull());
  const NCollection_List<TopoDS_Shape>& aBottomGenerated = aHistory.Generated(aBottomFace);
  EXPECT_EQ(CountShapes(aBottomGenerated, TopAbs_VERTEX), 20);
  EXPECT_EQ(CountShapes(aBottomGenerated, TopAbs_EDGE), 31);
  EXPECT_EQ(CountShapes(aBottomGenerated, TopAbs_WIRE), 12);
  EXPECT_EQ(CountShapes(aBottomGenerated, TopAbs_FACE), 12);
  const TopoDS_Shape aBottomGeneratedShape = MakeCompound(aBottomGenerated);
  EXPECT_NEAR(BOPTest_Utilities::GetSurfaceArea(aBottomGeneratedShape),
              300.0,
              Precision::Confusion());
  ExpectValidShape(aBottomGeneratedShape);

  const NCollection_List<TopoDS_Shape>& aTopGenerated = aHistory.Generated(aTopFace);
  EXPECT_EQ(CountShapes(aTopGenerated, TopAbs_VERTEX), 16);
  EXPECT_EQ(CountShapes(aTopGenerated, TopAbs_EDGE), 20);
  EXPECT_EQ(CountShapes(aTopGenerated, TopAbs_WIRE), 6);
  EXPECT_EQ(CountShapes(aTopGenerated, TopAbs_FACE), 6);
  const TopoDS_Shape aTopGeneratedShape = MakeCompound(aTopGenerated);
  EXPECT_NEAR(BOPTest_Utilities::GetSurfaceArea(aTopGeneratedShape), 150.0, Precision::Confusion());
  ExpectValidShape(aTopGeneratedShape);

  for (NCollection_List<TopoDS_Shape>::Iterator anIterator(aTopGenerated); anIterator.More();
       anIterator.Next())
  {
    const NCollection_List<TopoDS_Shape>& aTwins = aMaker.GetTwins(anIterator.Value());
    EXPECT_EQ(CountShapes(aTwins, TopAbs_WIRE), 11);
    EXPECT_EQ(CountShapes(aTwins, TopAbs_FACE), 11);
    const TopoDS_Shape aTwinsShape = MakeCompound(aTwins);
    EXPECT_NEAR(BOPTest_Utilities::GetSurfaceArea(aTwinsShape), 275.0, Precision::Confusion());
    ExpectValidShape(aTwinsShape);
  }

  aMaker.ClearRepetitions();
  EXPECT_TRUE(aHistory.Generated(aBottomFace).IsEmpty());
}

// Equivalent to tests/boolean/periodicity/A3.
TEST(BOPAlgo_MakePeriodicTest, A3_PeriodicCylinderCut_RepeatedCutHasExpectedTopology)
{
  const TopoDS_Shape aBox = BOPTest_Utilities::CreateBox(gp_Pnt(0.0, 0.0, 0.0), 100.0, 100.0, 1.0);
  const TopoDS_Shape aCylinder =
    BOPTest_Utilities::Translate(BOPTest_Utilities::CreateCylinder(2.0, 2.0), 2.5, 2.5, 0.0);

  BOPAlgo_MakePeriodic aMaker;
  aMaker.SetShape(aCylinder);
  aMaker.MakeXPeriodic(true, 5.0);
  aMaker.MakeYPeriodic(true, 5.0);
  aMaker.SetXTrimmed(false, 0.0);
  aMaker.SetYTrimmed(false, 0.0);
  aMaker.SetRunParallel(false);
  aMaker.Perform();
  ASSERT_FALSE(aMaker.HasErrors());
  aMaker.XRepeat(19);
  aMaker.YRepeat(19);

  BRepAlgoAPI_Cut aCut(aBox, aMaker.RepeatedShape());
  ASSERT_TRUE(aCut.IsDone());
  const TopoDS_Shape& aResult = aCut.Shape();
  ASSERT_FALSE(aResult.IsNull());
  ExpectShapeCounts(aResult, 808, 1212, 1206, 406, 1, 1);
  EXPECT_NEAR(BOPTest_Utilities::GetSurfaceArea(aResult), 15373.5, 15373.5 * 0.01);
  EXPECT_NEAR(BOPTest_Utilities::GetVolume(aResult), 4973.45, 4973.45 * 0.01);
}

// Equivalent to tests/boolean/periodicity/A4.
TEST(BOPAlgo_MakePeriodicTest, A4_PeriodicCutResult_RepeatsInBothDirections)
{
  const TopoDS_Shape aBox = BOPTest_Utilities::CreateBox(gp_Pnt(0.0, 0.0, 0.0), 5.0, 5.0, 1.0);
  const TopoDS_Shape aCylinder =
    BOPTest_Utilities::Translate(BOPTest_Utilities::CreateCylinder(2.0, 2.0), 2.5, 2.5, 0.0);
  BRepAlgoAPI_Cut aCut(aBox, aCylinder);
  ASSERT_TRUE(aCut.IsDone());

  BOPAlgo_MakePeriodic aMaker;
  aMaker.SetShape(aCut.Shape());
  aMaker.MakeXPeriodic(true, 5.0);
  aMaker.MakeYPeriodic(true, 5.0);
  aMaker.SetRunParallel(false);
  aMaker.Perform();
  ASSERT_FALSE(aMaker.HasErrors());
  aMaker.XRepeat(-9);
  aMaker.XRepeat(1);
  aMaker.YRepeat(-9);
  aMaker.YRepeat(1);

  const TopoDS_Shape& aResult = aMaker.RepeatedShape();
  ASSERT_FALSE(aResult.IsNull());
  ExpectShapeCounts(aResult, 1682, 3321, 2840, 2040, 400, 400);
  EXPECT_NEAR(BOPTest_Utilities::GetSurfaceArea(aResult), 22973.5, 22973.5 * 0.01);
  EXPECT_NEAR(BOPTest_Utilities::GetVolume(aResult), 4973.45, 4973.45 * 0.01);
}

// Equivalent to tests/boolean/periodicity/A5.
TEST(BOPAlgo_MakePeriodicTest, A5_NurbsEdges_PeriodicTwinsPreserveCurveType)
{
  const TopoDS_Shape                  aBox    = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  const NCollection_List<TopoDS_Face> aFaces  = GetFaces(aBox);
  const NCollection_List<TopoDS_Edge> anEdges = GetEdges(aBox);

  for (NCollection_List<TopoDS_Edge>::Iterator anEdgeIterator(anEdges); anEdgeIterator.More();
       anEdgeIterator.Next())
  {
    const TopoDS_Edge&          anEdge = anEdgeIterator.Value();
    BRepBuilderAPI_NurbsConvert aConverter;
    aConverter.Perform(anEdge);
    ASSERT_TRUE(aConverter.IsDone());
    const TopoDS_Shape aNurbsEdge = aConverter.Shape();
    ASSERT_FALSE(aNurbsEdge.IsNull());

    BOPAlgo_MakerVolume                  aVolume;
    const occ::handle<BRepTools_History> aVolumeHistory = MakeVolume(aVolume, aNurbsEdge, aFaces);
    ASSERT_FALSE(aVolume.HasErrors());
    const TopoDS_Shape& aSolid = aVolume.Shape();
    ASSERT_FALSE(aSolid.IsNull());
    ExpectValidShape(aSolid);

    const NCollection_List<TopoDS_Shape>& aModified = aVolumeHistory->Modified(aNurbsEdge);
    ASSERT_EQ(CountShapes(aModified, TopAbs_EDGE), 1);
    const TopoDS_Edge aModifiedEdge = TopoDS::Edge(aModified.First());
    EXPECT_TRUE(IsBSplineCurve(aModifiedEdge));

    BOPAlgo_MakePeriodic aMaker;
    aMaker.Clear();
    MakeBoxPeriodic(aMaker, aSolid, 10.0, 10.0, 10.0, true, true, true, 0.0, 0.0, 0.0, false);
    ASSERT_FALSE(aMaker.HasErrors());
    ASSERT_FALSE(aMaker.History().IsNull());

    const NCollection_List<TopoDS_Shape>& aPeriodicModified =
      aMaker.History()->Modified(aModifiedEdge);
    ASSERT_EQ(CountShapes(aPeriodicModified, TopAbs_EDGE), 1);
    const TopoDS_Edge aPeriodicModifiedEdge = TopoDS::Edge(aPeriodicModified.First());
    const char*       aPeriodicCurveType    = CurveTypeName(aPeriodicModifiedEdge);
    ASSERT_NE(aPeriodicCurveType, nullptr);

    const NCollection_List<TopoDS_Shape>& aTwins = aMaker.GetTwins(aPeriodicModifiedEdge);
    EXPECT_EQ(CountShapes(aTwins, TopAbs_EDGE), 2);
    for (NCollection_List<TopoDS_Shape>::Iterator anIterator(aTwins); anIterator.More();
         anIterator.Next())
    {
      const char* aTwinCurveType = CurveTypeName(TopoDS::Edge(anIterator.Value()));
      ASSERT_NE(aTwinCurveType, nullptr);
      EXPECT_STREQ(aTwinCurveType, aPeriodicCurveType);
    }
  }
}

// Equivalent to tests/boolean/periodicity/A6.
TEST(BOPAlgo_MakePeriodicTest, A6_NurbsFaces_PeriodicTwinsPreserveGeometryTypes)
{
  const TopoDS_Shape                  aBox   = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  const NCollection_List<TopoDS_Face> aFaces = GetFaces(aBox);

  for (NCollection_List<TopoDS_Face>::Iterator aFaceIterator(aFaces); aFaceIterator.More();
       aFaceIterator.Next())
  {
    const TopoDS_Face&          aFace = aFaceIterator.Value();
    BRepBuilderAPI_NurbsConvert aConverter;
    aConverter.Perform(aFace);
    ASSERT_TRUE(aConverter.IsDone());
    const TopoDS_Shape aNurbsFace = aConverter.Shape();
    ASSERT_FALSE(aNurbsFace.IsNull());

    BOPAlgo_MakerVolume                  aVolume;
    const occ::handle<BRepTools_History> aVolumeHistory = MakeVolume(aVolume, aNurbsFace, aFaces);
    ASSERT_FALSE(aVolume.HasErrors());
    const TopoDS_Shape& aSolid = aVolume.Shape();
    ASSERT_FALSE(aSolid.IsNull());
    ExpectValidShape(aSolid);

    const NCollection_List<TopoDS_Shape>& aModified = aVolumeHistory->Modified(aNurbsFace);
    ASSERT_EQ(CountShapes(aModified, TopAbs_FACE), 1);
    const TopoDS_Face aModifiedFace = TopoDS::Face(aModified.First());
    EXPECT_TRUE(IsBSplineSurface(aModifiedFace));

    BOPAlgo_MakePeriodic aMaker;
    MakeBoxPeriodic(aMaker, aSolid, 10.0, 10.0, 10.0, true, true, true, 0.0, 0.0, 0.0, false);
    ASSERT_FALSE(aMaker.HasErrors());
    ASSERT_FALSE(aMaker.History().IsNull());

    const NCollection_List<TopoDS_Shape>& aPeriodicModified =
      aMaker.History()->Modified(aModifiedFace);
    ASSERT_EQ(CountShapes(aPeriodicModified, TopAbs_FACE), 1);
    const TopoDS_Face aPeriodicModifiedFace = TopoDS::Face(aPeriodicModified.First());
    const char*       aPeriodicSurfaceType  = SurfaceTypeName(aPeriodicModifiedFace);
    ASSERT_NE(aPeriodicSurfaceType, nullptr);

    const NCollection_List<TopoDS_Shape>& aFaceTwins = aMaker.GetTwins(aPeriodicModifiedFace);
    EXPECT_EQ(CountShapes(aFaceTwins, TopAbs_FACE), 1);
    for (NCollection_List<TopoDS_Shape>::Iterator anIterator(aFaceTwins); anIterator.More();
         anIterator.Next())
    {
      const char* aTwinSurfaceType = SurfaceTypeName(TopoDS::Face(anIterator.Value()));
      ASSERT_NE(aTwinSurfaceType, nullptr);
      EXPECT_STREQ(aTwinSurfaceType, aPeriodicSurfaceType);
    }

    const NCollection_List<TopoDS_Edge> aModifiedEdges = GetEdges(aPeriodicModifiedFace);
    for (NCollection_List<TopoDS_Edge>::Iterator anEdgeIterator(aModifiedEdges);
         anEdgeIterator.More();
         anEdgeIterator.Next())
    {
      const TopoDS_Edge& aModifiedEdge      = anEdgeIterator.Value();
      const char*        aModifiedCurveType = CurveTypeName(aModifiedEdge);
      ASSERT_NE(aModifiedCurveType, nullptr);
      const NCollection_List<TopoDS_Shape>& anEdgeTwins = aMaker.GetTwins(aModifiedEdge);
      EXPECT_EQ(CountShapes(anEdgeTwins, TopAbs_EDGE), 2);
      for (NCollection_List<TopoDS_Shape>::Iterator anIterator(anEdgeTwins); anIterator.More();
           anIterator.Next())
      {
        const char* aTwinCurveType = CurveTypeName(TopoDS::Edge(anIterator.Value()));
        ASSERT_NE(aTwinCurveType, nullptr);
        EXPECT_STREQ(aTwinCurveType, aModifiedCurveType);
      }
    }
  }
}

// Equivalent to tests/boolean/mkconnected/A1.
TEST(BOPAlgo_MakePeriodicTest, MakeConnected_A1_PeriodicConnectedSolids)
{
  const TopoDS_Shape aProfile = MakePeriodicityProfile();
  const TopoDS_Shape aBox1    = aProfile;
  const TopoDS_Shape aBox2    = BRepPrimAPI_MakeBox(gp_Pnt(0.0, 0.0, 5.0), 5.0, 5.0, 5.0).Shape();
  const TopoDS_Shape aBox3    = BRepPrimAPI_MakeBox(gp_Pnt(0.0, 5.0, 0.0), 10.0, 5.0, 10.0).Shape();

  NCollection_List<TopoDS_Shape> anArguments;
  anArguments.Append(aBox3);
  anArguments.Append(aBox1);
  anArguments.Append(aBox2);
  BOPAlgo_MakeConnected aConnected;
  PerformConnected(aConnected, anArguments);
  ASSERT_FALSE(aConnected.HasErrors());
  ExpectShapeCounts(aConnected.Shape(), 18, 31, 17, 17, 3, 3);
  EXPECT_NEAR(BOPTest_Utilities::GetSurfaceArea(aConnected.Shape()), 900.0, Precision::Confusion());
  EXPECT_NEAR(BOPTest_Utilities::GetVolume(aConnected.Shape()), 1000.0, Precision::Confusion());
  ASSERT_FALSE(aConnected.History().IsNull());
  EXPECT_EQ(CountShapes(aConnected.GetModified(aBox3), TopAbs_FACE), 7);

  aConnected.MakePeriodic(MakeXYZPeriodicity(10.0, 10.0, 10.0));
  ASSERT_FALSE(aConnected.PeriodicityTool().HasErrors());
  ExpectShapeCounts(aConnected.PeriodicShape(), 26, 42, 20, 20, 3, 3);
  EXPECT_EQ(CountShapes(aConnected.GetModified(aBox1), TopAbs_FACE), 10);
  EXPECT_EQ(CountShapes(aConnected.GetModified(aBox2), TopAbs_VERTEX),
            CountShapes(aBox2, TopAbs_VERTEX));
  EXPECT_EQ(CountShapes(aConnected.GetModified(aBox3), TopAbs_FACE), 8);

  const TopoDS_Face aMaterialFace = GetFace(aBox3, 2);
  ASSERT_FALSE(aMaterialFace.IsNull());
  const TopoDS_Shape aModifiedMaterialShape = MakeCompound(aConnected.GetModified(aMaterialFace));
  const NCollection_List<TopoDS_Face> aModifiedMaterialFaces = GetFaces(aModifiedMaterialShape);
  ASSERT_FALSE(aModifiedMaterialFaces.IsEmpty());
  NCollection_List<TopoDS_Shape> aPositiveMaterials;
  NCollection_List<TopoDS_Shape> aNegativeMaterials;
  for (NCollection_List<TopoDS_Face>::Iterator anIterator(aModifiedMaterialFaces);
       anIterator.More();
       anIterator.Next())
  {
    const NCollection_List<TopoDS_Shape>& aPositive =
      aConnected.MaterialsOnPositiveSide(anIterator.Value());
    for (NCollection_List<TopoDS_Shape>::Iterator aMaterialIterator(aPositive);
         aMaterialIterator.More();
         aMaterialIterator.Next())
    {
      aPositiveMaterials.Append(aMaterialIterator.Value());
    }

    const NCollection_List<TopoDS_Shape>& aNegative =
      aConnected.MaterialsOnNegativeSide(anIterator.Value());
    for (NCollection_List<TopoDS_Shape>::Iterator aMaterialIterator(aNegative);
         aMaterialIterator.More();
         aMaterialIterator.Next())
    {
      aNegativeMaterials.Append(aMaterialIterator.Value());
    }
  }
  ASSERT_FALSE(aPositiveMaterials.IsEmpty());
  ASSERT_FALSE(aNegativeMaterials.IsEmpty());
  NCollection_List<TopoDS_Shape> anExpectedPositiveMaterials;
  anExpectedPositiveMaterials.Append(aBox1);
  anExpectedPositiveMaterials.Append(aBox2);
  NCollection_List<TopoDS_Shape> anExpectedNegativeMaterials;
  anExpectedNegativeMaterials.Append(aBox3);
  ExpectEqualProperties(MakeCompound(aPositiveMaterials),
                        MakeCompound(anExpectedPositiveMaterials),
                        true);
  ExpectEqualProperties(MakeCompound(aNegativeMaterials),
                        MakeCompound(anExpectedNegativeMaterials),
                        true);
}

// Equivalent to tests/boolean/mkconnected/A2.
TEST(BOPAlgo_MakePeriodicTest, MakeConnected_A2_PeriodicAndRepeatedConnectedSolids)
{
  BRepBuilderAPI_MakePolygon aPolygon;
  aPolygon.Add(gp_Pnt(3.0, 0.0, 6.0));
  aPolygon.Add(gp_Pnt(3.0, 3.0, 6.0));
  aPolygon.Add(gp_Pnt(0.0, 3.0, 6.0));
  aPolygon.Add(gp_Pnt(0.0, 4.0, 6.0));
  aPolygon.Add(gp_Pnt(4.0, 4.0, 6.0));
  aPolygon.Add(gp_Pnt(4.0, 0.0, 6.0));
  aPolygon.Close();
  const TopoDS_Shape aPrism =
    BRepPrimAPI_MakePrism(BRepBuilderAPI_MakeFace(aPolygon.Wire()).Face(), gp_Vec(0.0, 0.0, 4.0))
      .Shape();

  const TopoDS_Shape aBox1 = BRepPrimAPI_MakeBox(10.0, 10.0, 5.0).Shape();
  const TopoDS_Shape aBox2 = BRepPrimAPI_MakeBox(gp_Pnt(0.0, 0.0, 5.0), 10.0, 10.0, 1.0).Shape();
  const TopoDS_Shape aBox3 = BRepPrimAPI_MakeBox(gp_Pnt(0.0, 0.0, 6.0), 3.0, 3.0, 4.0).Shape();
  NCollection_List<TopoDS_Shape> anArguments;
  anArguments.Append(aBox1);
  anArguments.Append(aBox2);
  anArguments.Append(aBox3);
  anArguments.Append(aPrism);
  const TopoDS_Shape aBox5 = BRepPrimAPI_MakeBox(gp_Pnt(0.0, 9.0, 6.0), 4.0, 1.0, 4.0).Shape();
  const TopoDS_Shape aBox6 = BRepPrimAPI_MakeBox(gp_Pnt(9.0, 9.0, 6.0), 1.0, 1.0, 4.0).Shape();
  const TopoDS_Shape aBox7 = BRepPrimAPI_MakeBox(gp_Pnt(9.0, 0.0, 6.0), 1.0, 3.0, 4.0).Shape();
  anArguments.Append(aBox5);
  anArguments.Append(aBox6);
  anArguments.Append(aBox7);

  BOPAlgo_MakeConnected aConnected;
  PerformConnected(aConnected, anArguments);
  ASSERT_FALSE(aConnected.HasErrors());
  ExpectShapeCounts(aConnected.Shape(), 46, 79, 41, 41, 7, 7);
  EXPECT_NEAR(BOPTest_Utilities::GetSurfaceArea(aConnected.Shape()), 888.0, Precision::Confusion());
  EXPECT_NEAR(BOPTest_Utilities::GetVolume(aConnected.Shape()), 696.0, Precision::Confusion());

  BOPAlgo_MakePeriodic::PeriodicityParams aParams;
  aParams.myPeriodic[0] = true;
  aParams.myPeriodic[1] = true;
  aParams.myPeriod[0]   = 10.0;
  aParams.myPeriod[1]   = 10.0;
  aConnected.MakePeriodic(aParams);
  ASSERT_FALSE(aConnected.PeriodicityTool().HasErrors());
  ExpectShapeCounts(aConnected.PeriodicShape(), 49, 83, 42, 42, 7, 7);
  EXPECT_NEAR(BOPTest_Utilities::GetSurfaceArea(aConnected.PeriodicShape()),
              888.0,
              Precision::Confusion());
  EXPECT_NEAR(BOPTest_Utilities::GetVolume(aConnected.PeriodicShape()),
              696.0,
              Precision::Confusion());

  const TopoDS_Face aMaterialFace = GetFace(aBox6, 3);
  ASSERT_FALSE(aMaterialFace.IsNull());
  ExpectModifiedMaterials(aConnected,
                          aConnected.GetModified(aMaterialFace),
                          aBox6,
                          TopoDS_Shape(),
                          TopoDS_Shape());

  ASSERT_FALSE(aConnected.History().IsNull());
  const TopoDS_Shape aModifiedBox5 = MakeCompound(aConnected.GetModified(aBox5));
  ExpectShapeCounts(aModifiedBox5, 10, 15, 7, 7, 1, 1);
  EXPECT_NEAR(BOPTest_Utilities::GetSurfaceArea(aModifiedBox5), 48.0, Precision::Confusion());
  EXPECT_NEAR(BOPTest_Utilities::GetVolume(aModifiedBox5), 16.0, Precision::Confusion());

  aConnected.RepeatShape(0, -1);
  aConnected.RepeatShape(0, 1);
  aConnected.RepeatShape(1, -1);
  aConnected.RepeatShape(1, 1);
  ExpectShapeCounts(aConnected.PeriodicShape(), 496, 947, 564, 564, 112, 112);
  EXPECT_NEAR(BOPTest_Utilities::GetSurfaceArea(aConnected.PeriodicShape()),
              14208.0,
              Precision::Confusion());
  EXPECT_NEAR(BOPTest_Utilities::GetVolume(aConnected.PeriodicShape()),
              11136.0,
              Precision::Confusion());
  ExpectModifiedMaterials(aConnected,
                          aConnected.GetModified(aMaterialFace),
                          aBox6,
                          TopoDS_Shape(),
                          aBox7);

  aParams.Clear();
  aParams.myPeriodic[0]    = true;
  aParams.myPeriodic[1]    = true;
  aParams.myPeriod[0]      = 12.0;
  aParams.myPeriod[1]      = 12.0;
  aParams.myIsTrimmed[0]   = false;
  aParams.myIsTrimmed[1]   = false;
  aParams.myPeriodFirst[0] = -1.0;
  aParams.myPeriodFirst[1] = -1.0;
  aConnected.MakePeriodic(aParams);
  ASSERT_FALSE(aConnected.PeriodicityTool().HasErrors());
  ExpectShapeCounts(aConnected.PeriodicShape(), 46, 79, 41, 41, 7, 7);
  ExpectEqualProperties(aConnected.Shape(), aConnected.PeriodicShape(), true);
  aConnected.RepeatShape(0, -1);
  aConnected.RepeatShape(0, 1);
  aConnected.RepeatShape(1, -1);
  aConnected.RepeatShape(1, 1);
  ExpectShapeCounts(aConnected.PeriodicShape(), 736, 1264, 656, 656, 112, 112);
  EXPECT_NEAR(BOPTest_Utilities::GetSurfaceArea(aConnected.PeriodicShape()),
              14208.0,
              Precision::Confusion());
  EXPECT_NEAR(BOPTest_Utilities::GetVolume(aConnected.PeriodicShape()),
              11136.0,
              Precision::Confusion());
  ExpectModifiedMaterials(aConnected,
                          aConnected.GetModified(aMaterialFace),
                          aBox6,
                          TopoDS_Shape(),
                          TopoDS_Shape());
}

// Equivalent to tests/boolean/mkconnected/A5.
TEST(BOPAlgo_MakePeriodicTest, MakeConnected_A5_PeriodicRepetitionCanBeCleared)
{
  const TopoDS_Shape aBox1 = BRepPrimAPI_MakeBox(5.0, 5.0, 10.0).Shape();
  const TopoDS_Shape aBox2 = BRepPrimAPI_MakeBox(gp_Pnt(5.0, 0.0, 0.0), 5.0, 5.0, 5.0).Shape();
  const TopoDS_Shape aBox3 = BRepPrimAPI_MakeBox(gp_Pnt(0.0, 5.0, 0.0), 5.0, 5.0, 5.0).Shape();
  NCollection_List<TopoDS_Shape> anArguments;
  anArguments.Append(aBox1);
  anArguments.Append(aBox2);
  anArguments.Append(aBox3);

  BOPAlgo_MakeConnected aConnected;
  PerformConnected(aConnected, anArguments);
  ASSERT_FALSE(aConnected.HasErrors());
  ExpectValidShape(aConnected.Shape());
  EXPECT_EQ(CountShapes(aConnected.Shape(), TopAbs_WIRE), 18);
  EXPECT_EQ(CountShapes(aConnected.Shape(), TopAbs_FACE), 18);
  EXPECT_EQ(CountShapes(aConnected.Shape(), TopAbs_SHELL), 3);
  EXPECT_EQ(CountShapes(aConnected.Shape(), TopAbs_SOLID), 3);

  const TopoDS_Face aMaterialFace = GetFace(aBox2, 1);
  ASSERT_FALSE(aMaterialFace.IsNull());

  BOPAlgo_MakePeriodic::PeriodicityParams aParams;
  aParams.myPeriodic[0] = true;
  aParams.myPeriodic[1] = true;
  aParams.myPeriod[0]   = 10.0;
  aParams.myPeriod[1]   = 10.0;
  aConnected.MakePeriodic(aParams);
  ASSERT_FALSE(aConnected.PeriodicityTool().HasErrors());
  ExpectValidShape(aConnected.PeriodicShape());
  EXPECT_EQ(CountShapes(aConnected.PeriodicShape(), TopAbs_WIRE), 20);
  EXPECT_EQ(CountShapes(aConnected.PeriodicShape(), TopAbs_FACE), 20);
  EXPECT_EQ(CountShapes(aConnected.PeriodicShape(), TopAbs_SHELL), 3);
  EXPECT_EQ(CountShapes(aConnected.PeriodicShape(), TopAbs_SOLID), 3);

  const NCollection_List<TopoDS_Shape>& aModifiedMaterial = aConnected.GetModified(aMaterialFace);
  const TopoDS_Shape                    aModifiedMaterialShape = MakeCompound(aModifiedMaterial);
  const NCollection_List<TopoDS_Face>   aModifiedMaterialFaces = GetFaces(aModifiedMaterialShape);
  ASSERT_FALSE(aModifiedMaterialFaces.IsEmpty());
  const TopoDS_Face& aModifiedFace = aModifiedMaterialFaces.First();
  ExpectMaterials(aConnected.MaterialsOnPositiveSide(aModifiedFace), aBox2);
  const NCollection_List<TopoDS_Shape>& aFaceTwins =
    aConnected.PeriodicityTool().GetTwins(aModifiedFace);
  NCollection_List<TopoDS_Shape> aNegativeMaterials;
  for (NCollection_List<TopoDS_Shape>::Iterator anIterator(aFaceTwins); anIterator.More();
       anIterator.Next())
  {
    const NCollection_List<TopoDS_Shape>& aMaterials =
      aConnected.MaterialsOnNegativeSide(anIterator.Value());
    for (NCollection_List<TopoDS_Shape>::Iterator aMaterialIterator(aMaterials);
         aMaterialIterator.More();
         aMaterialIterator.Next())
    {
      aNegativeMaterials.Append(aMaterialIterator.Value());
    }
  }
  ExpectMaterials(aNegativeMaterials, aBox1);

  aConnected.RepeatShape(0, 1);
  aConnected.RepeatShape(1, 1);
  ExpectValidShape(aConnected.PeriodicShape());
  EXPECT_EQ(CountShapes(aConnected.PeriodicShape(), TopAbs_WIRE), 76);
  EXPECT_EQ(CountShapes(aConnected.PeriodicShape(), TopAbs_FACE), 76);
  EXPECT_EQ(CountShapes(aConnected.PeriodicShape(), TopAbs_SHELL), 12);
  EXPECT_EQ(CountShapes(aConnected.PeriodicShape(), TopAbs_SOLID), 12);
  ASSERT_FALSE(aConnected.History().IsNull());
  EXPECT_EQ(CountShapes(aConnected.History()->Generated(aMaterialFace), TopAbs_FACE), 4);
  aConnected.ClearRepetitions();
  ExpectWireFaceShellSolidCounts(aConnected.PeriodicShape(), 20, 20, 3, 3);

  const NCollection_List<TopoDS_Face> aClearedModifiedFaces =
    GetFaces(MakeCompound(aConnected.GetModified(aMaterialFace)));
  ASSERT_FALSE(aClearedModifiedFaces.IsEmpty());
  ExpectMaterials(aConnected.MaterialsOnPositiveSide(aClearedModifiedFaces.First()), aBox2);

  aConnected.RepeatShape(1, 1);
  ExpectValidShape(aConnected.PeriodicShape());
  EXPECT_EQ(CountShapes(aConnected.PeriodicShape(), TopAbs_WIRE), 39);
  EXPECT_EQ(CountShapes(aConnected.PeriodicShape(), TopAbs_FACE), 39);
  EXPECT_EQ(CountShapes(aConnected.PeriodicShape(), TopAbs_SHELL), 6);
  EXPECT_EQ(CountShapes(aConnected.PeriodicShape(), TopAbs_SOLID), 6);
  const NCollection_List<TopoDS_Face> aFinalModifiedFaces =
    GetFaces(MakeCompound(aConnected.GetModified(aMaterialFace)));
  ASSERT_FALSE(aFinalModifiedFaces.IsEmpty());
  const NCollection_List<TopoDS_Shape>& aFinalTwins =
    aConnected.PeriodicityTool().GetTwins(aFinalModifiedFaces.First());
  EXPECT_EQ(CountShapes(aFinalTwins, TopAbs_FACE), 3);
}
