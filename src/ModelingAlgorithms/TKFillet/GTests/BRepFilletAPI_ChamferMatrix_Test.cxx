// Copyright (c) 2026 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.

#include <BRepAdaptor_Curve.hxx>
#include <BRepTools.hxx>
#include <tuple>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Check.hxx>
#include <ChFi3d_ChBuilder.hxx>
#include <limits>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepFilletAPI_MakeChamfer.hxx>
#include <BRepGProp.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepPrimAPI_MakeTorus.hxx>
#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <Bnd_Box.hxx>
#include <GProp_GProps.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_List.hxx>
#include <Precision.hxx>
#include <Standard_Failure.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <cmath>
#include <sstream>
#include <string>
#include <vector>

namespace
{
enum class SurfaceFamily
{
  PlanePlane,
  PlaneCylinder,
  PlaneCone,
  PlaneSphere,
  PlaneTorus,
  PlaneExtrusion,
  PlaneRevolution,
  PlaneBSpline
};

enum class ChamferMode
{
  EqualDistance,
  TwoDistances,
  DistanceAngle
};

struct MatrixCase
{
  SurfaceFamily Family;
  ChamferMode   Mode;
};

struct EdgeContext
{
  TopoDS_Edge Edge;
  TopoDS_Face FirstFace;
  TopoDS_Face SecondFace;
};

const char* familyName(const SurfaceFamily theFamily)
{
  switch (theFamily)
  {
    case SurfaceFamily::PlanePlane:
      return "PlanePlane";
    case SurfaceFamily::PlaneCylinder:
      return "PlaneCylinder";
    case SurfaceFamily::PlaneCone:
      return "PlaneCone";
    case SurfaceFamily::PlaneSphere:
      return "PlaneSphere";
    case SurfaceFamily::PlaneTorus:
      return "PlaneTorus";
    case SurfaceFamily::PlaneExtrusion:
      return "PlaneExtrusion";
    case SurfaceFamily::PlaneRevolution:
      return "PlaneRevolution";
    case SurfaceFamily::PlaneBSpline:
      return "PlaneBSpline";
  }
  return "Unknown";
}

const char* modeName(const ChamferMode theMode)
{
  switch (theMode)
  {
    case ChamferMode::EqualDistance:
      return "EqualDistance";
    case ChamferMode::TwoDistances:
      return "TwoDistances";
    case ChamferMode::DistanceAngle:
      return "DistanceAngle";
  }
  return "Unknown";
}

TopoDS_Shape makeFamilyShape(const SurfaceFamily theFamily)
{
  switch (theFamily)
  {
    case SurfaceFamily::PlanePlane:
      return BRepPrimAPI_MakeBox(20.0, 18.0, 16.0).Shape();
    case SurfaceFamily::PlaneCylinder:
      return BRepPrimAPI_MakeCylinder(8.0, 20.0).Shape();
    case SurfaceFamily::PlaneCone:
      return BRepPrimAPI_MakeCone(9.0, 5.0, 20.0).Shape();
    case SurfaceFamily::PlaneSphere:
      return BRepPrimAPI_MakeSphere(10.0, -55.0 * M_PI / 180.0, 50.0 * M_PI / 180.0).Shape();
    case SurfaceFamily::PlaneTorus:
      return BRepPrimAPI_MakeTorus(12.0, 3.0, -2.0 * M_PI / 3.0, 2.0 * M_PI / 3.0, 3.0 * M_PI / 2.0)
        .Shape();
    case SurfaceFamily::PlaneExtrusion: {
      NCollection_Array1<gp_Pnt> aPoints(1, 4);
      aPoints.SetValue(1, gp_Pnt(0, 0, 0));
      aPoints.SetValue(2, gp_Pnt(3, 1, 0));
      aPoints.SetValue(3, gp_Pnt(7, -1, 0));
      aPoints.SetValue(4, gp_Pnt(10, 0, 0));
      const occ::handle<Geom_BSplineCurve> aCurve = GeomAPI_PointsToBSpline(aPoints).Curve();
      BRepBuilderAPI_MakeWire              aWire;
      aWire.Add(BRepBuilderAPI_MakeEdge(aCurve));
      aWire.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(10, 0, 0), gp_Pnt(10, 5, 0)));
      aWire.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(10, 5, 0), gp_Pnt(0, 5, 0)));
      aWire.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(0, 5, 0), gp_Pnt(0, 0, 0)));
      return BRepPrimAPI_MakePrism(BRepBuilderAPI_MakeFace(aWire.Wire()), gp_Vec(0, 0, 12)).Shape();
    }
    case SurfaceFamily::PlaneRevolution: {
      NCollection_Array1<gp_Pnt> aPoints(1, 4);
      aPoints.SetValue(1, gp_Pnt(4, 0, 0));
      aPoints.SetValue(2, gp_Pnt(5, 0, 3));
      aPoints.SetValue(3, gp_Pnt(4.5, 0, 7));
      aPoints.SetValue(4, gp_Pnt(4, 0, 10));
      const occ::handle<Geom_BSplineCurve> aCurve = GeomAPI_PointsToBSpline(aPoints).Curve();
      BRepBuilderAPI_MakeWire              aWire;
      aWire.Add(BRepBuilderAPI_MakeEdge(aCurve));
      aWire.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(4, 0, 10), gp_Pnt(0, 0, 10)));
      aWire.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(0, 0, 10), gp_Pnt(0, 0, 0)));
      aWire.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(0, 0, 0), gp_Pnt(4, 0, 0)));
      return BRepPrimAPI_MakeRevol(BRepBuilderAPI_MakeFace(aWire.Wire()),
                                   gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)),
                                   2.0 * M_PI,
                                   true)
        .Shape();
    }
    case SurfaceFamily::PlaneBSpline: {
      BRepOffsetAPI_ThruSections aLoft(true, false);
      for (const std::array<double, 2>& aSection : {std::array<double, 2>{0.0, 0.0},
                                                    std::array<double, 2>{6.0, 1.0},
                                                    std::array<double, 2>{12.0, 0.0}})
      {
        const double               aZ      = aSection[0];
        const double               anInset = aSection[1];
        BRepBuilderAPI_MakePolygon aPolygon;
        aPolygon.Add(gp_Pnt(-5 + anInset, -4, aZ));
        aPolygon.Add(gp_Pnt(5 + anInset, -4, aZ));
        aPolygon.Add(gp_Pnt(5 - anInset, 4, aZ));
        aPolygon.Add(gp_Pnt(-5 - anInset, 4, aZ));
        aPolygon.Close();
        aLoft.AddWire(aPolygon.Wire());
      }
      aLoft.Build();
      return aLoft.Shape();
    }
  }
  return TopoDS_Shape();
}

TopoDS_Shape makeTaperedArmCylinder(const double theCylinderRadius, const double theArmHalfWidth)
{
  // In the YZ section a rectangular arm overlaps the lower part of a circular boss.  The arm
  // flank meets the circle at an angle controlled by halfWidth / radius: the limit tends to a
  // tangent join as the ratio tends to one.  Extruding both along X reproduces the issue's long
  // chamfer edge and curved termination without importing any degenerate upstream features.
  const TopoDS_Shape anArm =
    BRepPrimAPI_MakeBox(gp_Pnt(0.0, -theArmHalfWidth, -8.0 * theCylinderRadius),
                        0.5 * theCylinderRadius,
                        2.0 * theArmHalfWidth,
                        8.0 * theCylinderRadius)
      .Shape();
  const TopoDS_Shape aBoss =
    BRepPrimAPI_MakeCylinder(gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0)),
                             theCylinderRadius,
                             0.5 * theCylinderRadius)
      .Shape();
  BRepAlgoAPI_Fuse aFuse(anArm, aBoss);
  aFuse.SimplifyResult();
  return aFuse.Shape();
}

EdgeContext findTaperedArmEdge(const TopoDS_Shape& theShape,
                               const double        theCylinderRadius,
                               const double        theArmHalfWidth,
                               const bool          thePositiveSide)
{
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    anEdgeFaceMap;
  TopExp::MapShapesAndAncestors(theShape, TopAbs_EDGE, TopAbs_FACE, anEdgeFaceMap);
  const double aTargetY = thePositiveSide ? theArmHalfWidth : -theArmHalfWidth;
  for (int anEdgeIndex = 1; anEdgeIndex <= anEdgeFaceMap.Extent(); ++anEdgeIndex)
  {
    const TopoDS_Edge anEdge = TopoDS::Edge(anEdgeFaceMap.FindKey(anEdgeIndex));
    if (BRepAdaptor_Curve(anEdge).GetType() != GeomAbs_Line)
    {
      continue;
    }
    TopoDS_Vertex aFirstVertex;
    TopoDS_Vertex aLastVertex;
    TopExp::Vertices(anEdge, aFirstVertex, aLastVertex, true);
    if (aFirstVertex.IsNull() || aLastVertex.IsNull())
    {
      continue;
    }
    const gp_Pnt aFirst = BRep_Tool::Pnt(aFirstVertex);
    const gp_Pnt aLast  = BRep_Tool::Pnt(aLastVertex);
    if (std::abs(aFirst.X()) > Precision::Confusion()
        || std::abs(aLast.X()) > Precision::Confusion()
        || std::abs(aFirst.Y() - aTargetY) > Precision::Confusion()
        || std::abs(aLast.Y() - aTargetY) > Precision::Confusion()
        || std::min(aFirst.Z(), aLast.Z()) > -7.9 * theCylinderRadius)
    {
      continue;
    }
    const NCollection_List<TopoDS_Shape>& aFaces = anEdgeFaceMap.FindFromIndex(anEdgeIndex);
    if (aFaces.Size() == 2)
    {
      return {anEdge, TopoDS::Face(aFaces.First()), TopoDS::Face(aFaces.Last())};
    }
  }
  return {};
}

std::pair<GeomAbs_SurfaceType, GeomAbs_SurfaceType> expectedSurfaceTypes(
  const SurfaceFamily theFamily)
{
  switch (theFamily)
  {
    case SurfaceFamily::PlanePlane:
      return {GeomAbs_Plane, GeomAbs_Plane};
    case SurfaceFamily::PlaneCylinder:
      return {GeomAbs_Plane, GeomAbs_Cylinder};
    case SurfaceFamily::PlaneCone:
      return {GeomAbs_Plane, GeomAbs_Cone};
    case SurfaceFamily::PlaneSphere:
      return {GeomAbs_Plane, GeomAbs_Sphere};
    case SurfaceFamily::PlaneTorus:
      return {GeomAbs_Plane, GeomAbs_Torus};
    case SurfaceFamily::PlaneExtrusion:
      return {GeomAbs_Plane, GeomAbs_SurfaceOfExtrusion};
    case SurfaceFamily::PlaneRevolution:
      return {GeomAbs_Plane, GeomAbs_SurfaceOfRevolution};
    case SurfaceFamily::PlaneBSpline:
      return {GeomAbs_Plane, GeomAbs_BSplineSurface};
  }
  return {GeomAbs_OtherSurface, GeomAbs_OtherSurface};
}

EdgeContext findFamilyEdge(const TopoDS_Shape& theShape,
                           const SurfaceFamily theFamily,
                           const bool          thePreferShortest = false)
{
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    anEdgeFaceMap;
  TopExp::MapShapesAndAncestors(theShape, TopAbs_EDGE, TopAbs_FACE, anEdgeFaceMap);
  const auto  anExpected = expectedSurfaceTypes(theFamily);
  EdgeContext aBestContext;
  double      aBestLength = RealLast();

  for (TopExp_Explorer anExp(theShape, TopAbs_EDGE); anExp.More(); anExp.Next())
  {
    const TopoDS_Edge anEdge = TopoDS::Edge(anExp.Current());
    if (!anEdgeFaceMap.Contains(anEdge) || anEdgeFaceMap.FindFromKey(anEdge).Size() < 2)
    {
      continue;
    }
    const NCollection_List<TopoDS_Shape>& aFaces      = anEdgeFaceMap.FindFromKey(anEdge);
    const TopoDS_Face                     aFirst      = TopoDS::Face(aFaces.First());
    const TopoDS_Face                     aSecond     = TopoDS::Face(aFaces.Last());
    const GeomAbs_SurfaceType             aFirstType  = BRepAdaptor_Surface(aFirst).GetType();
    const GeomAbs_SurfaceType             aSecondType = BRepAdaptor_Surface(aSecond).GetType();
    if ((aFirstType == anExpected.first && aSecondType == anExpected.second)
        || (aFirstType == anExpected.second && aSecondType == anExpected.first))
    {
      GProp_GProps aProperties;
      BRepGProp::LinearProperties(anEdge, aProperties);
      if (aProperties.Mass() >= aBestLength)
      {
        continue;
      }
      aBestLength = aProperties.Mass();
      if (aSecondType == GeomAbs_Plane && aFirstType != GeomAbs_Plane)
      {
        aBestContext = {anEdge, aSecond, aFirst};
      }
      else
      {
        aBestContext = {anEdge, aFirst, aSecond};
      }
      if (!thePreferShortest)
      {
        return aBestContext;
      }
    }
  }
  return aBestContext;
}

double shapeVolume(const TopoDS_Shape& theShape)
{
  GProp_GProps aProperties;
  BRepGProp::VolumePropertiesGK(theShape, aProperties, 1.e-8, true, true);
  return aProperties.Mass();
}

void expectMaterialRemoved(const TopoDS_Shape& theResult, const TopoDS_Shape& theInput)
{
  const double anInputVolume = shapeVolume(theInput);
  EXPECT_LT(shapeVolume(theResult), anInputVolume - std::max(1.0e-12, anInputVolume * 1.0e-10))
    << "a successful chamfer must not return an unchanged input solid";
}

double maximumTolerance(const TopoDS_Shape& theShape)
{
  double aMaximum = 0.0;
  for (TopExp_Explorer anExp(theShape, TopAbs_VERTEX); anExp.More(); anExp.Next())
  {
    aMaximum = std::max(aMaximum, BRep_Tool::Tolerance(TopoDS::Vertex(anExp.Current())));
  }
  for (TopExp_Explorer anExp(theShape, TopAbs_EDGE); anExp.More(); anExp.Next())
  {
    aMaximum = std::max(aMaximum, BRep_Tool::Tolerance(TopoDS::Edge(anExp.Current())));
  }
  for (TopExp_Explorer anExp(theShape, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    aMaximum = std::max(aMaximum, BRep_Tool::Tolerance(TopoDS::Face(anExp.Current())));
  }
  return aMaximum;
}

void expectClosedValidSolid(const TopoDS_Shape& theResult,
                            const TopoDS_Shape& theInput,
                            const double        theScale           = 1.0,
                            const bool          theRemovesMaterial = true)
{
  ASSERT_FALSE(theResult.IsNull());
  BRepCheck_Analyzer anAnalyzer(theResult, true, false, true);
  EXPECT_TRUE(anAnalyzer.IsValid()) << "the builder reported success with an invalid result";
  EXPECT_TRUE(BRepAlgoAPI_Check(theResult).IsValid());

  int aSolidCount = 0;
  for (TopExp_Explorer anExp(theResult, TopAbs_SOLID); anExp.More(); anExp.Next())
  {
    ++aSolidCount;
  }
  EXPECT_EQ(aSolidCount, 1) << "a single-solid input must remain a single solid";

  int aShellCount = 0;
  for (TopExp_Explorer anExp(theResult, TopAbs_SHELL); anExp.More(); anExp.Next())
  {
    ++aShellCount;
    EXPECT_TRUE(BRep_Tool::IsClosed(TopoDS::Shell(anExp.Current())))
      << "successful fillet/chamfer result contains an unconnected shell";
  }
  EXPECT_EQ(aShellCount, 1);

  const double anInputVolume  = shapeVolume(theInput);
  const double aResultVolume  = shapeVolume(theResult);
  const double aVolumeEpsilon = 64. * std::numeric_limits<double>::epsilon() * anInputVolume;
  EXPECT_GT(aResultVolume, 0.0);
  if (theRemovesMaterial)
  {
    EXPECT_LT(aResultVolume, anInputVolume - aVolumeEpsilon);
  }
  else
  {
    EXPECT_GT(aResultVolume, anInputVolume + aVolumeEpsilon);
  }

  EXPECT_LE(maximumTolerance(theResult), 2.01e-4 * std::max(1.0, theScale))
    << "topology must not be joined by inflating tolerances";

  Bnd_Box anInputBox;
  Bnd_Box aResultBox;
  BRepBndLib::AddOptimal(theInput, anInputBox, false, true);
  BRepBndLib::AddOptimal(theResult, aResultBox, false, true);
  double anIXMin, anIYMin, anIZMin, anIXMax, anIYMax, anIZMax;
  double aRXMin, aRYMin, aRZMin, aRXMax, aRYMax, aRZMax;
  anInputBox.Get(anIXMin, anIYMin, anIZMin, anIXMax, anIYMax, anIZMax);
  aResultBox.Get(aRXMin, aRYMin, aRZMin, aRXMax, aRYMax, aRZMax);
  const double aBoundTolerance =
    std::max(Precision::Confusion(), maximumTolerance(theInput) + maximumTolerance(theResult));
  EXPECT_GE(aRXMin, anIXMin - aBoundTolerance);
  EXPECT_GE(aRYMin, anIYMin - aBoundTolerance);
  EXPECT_GE(aRZMin, anIZMin - aBoundTolerance);
  EXPECT_LE(aRXMax, anIXMax + aBoundTolerance);
  EXPECT_LE(aRYMax, anIYMax + aBoundTolerance);
  EXPECT_LE(aRZMax, anIZMax + aBoundTolerance);
}

TopoDS_Shape buildChamfer(const TopoDS_Shape& theShape,
                          const EdgeContext&  theContext,
                          const ChamferMode   theMode,
                          const double        theScale         = 1.0,
                          const bool          theUseSecondFace = false)
{
  BRepFilletAPI_MakeChamfer aChamfer(theShape);
  const TopoDS_Face&        aReferenceFace =
    theUseSecondFace ? theContext.SecondFace : theContext.FirstFace;
  switch (theMode)
  {
    case ChamferMode::EqualDistance:
      aChamfer.Add(0.5 * theScale, theContext.Edge);
      break;
    case ChamferMode::TwoDistances:
      if (theUseSecondFace)
      {
        aChamfer.Add(0.65 * theScale, 0.35 * theScale, theContext.Edge, aReferenceFace);
      }
      else
      {
        aChamfer.Add(0.35 * theScale, 0.65 * theScale, theContext.Edge, aReferenceFace);
      }
      break;
    case ChamferMode::DistanceAngle:
      aChamfer.AddDA(0.25 * theScale, M_PI / 4.0, theContext.Edge, aReferenceFace);
      break;
  }
  aChamfer.Build();
  return aChamfer.IsDone() ? aChamfer.Shape() : TopoDS_Shape();
}

TopoDS_Shape buildTwoDistanceChamfer(const TopoDS_Shape& theShape,
                                     const EdgeContext&  theContext,
                                     const double        theFirstDistance,
                                     const double        theSecondDistance,
                                     const bool          theUseSecondFace)
{
  BRepFilletAPI_MakeChamfer aChamfer(theShape);
  if (theUseSecondFace)
  {
    aChamfer.Add(theSecondDistance, theFirstDistance, theContext.Edge, theContext.SecondFace);
  }
  else
  {
    aChamfer.Add(theFirstDistance, theSecondDistance, theContext.Edge, theContext.FirstFace);
  }
  aChamfer.Build();
  return aChamfer.IsDone() ? aChamfer.Shape() : TopoDS_Shape();
}

class ChamferSurfaceModeMatrix : public testing::TestWithParam<MatrixCase>
{
};

std::string matrixCaseName(const testing::TestParamInfo<MatrixCase>& theInfo)
{
  return std::string(familyName(theInfo.param.Family)) + "_" + modeName(theInfo.param.Mode);
}

//=================================================================================================

TopoDS_Shape readCornerShape()
{
  const std::string aFile = __FILE__;
  const std::string aPath =
    aFile.substr(0, aFile.find_last_of("/\\") + 1) + "data/bug1177_corner.brep";
  TopoDS_Shape aShape;
  BRep_Builder aBuilder;
  if (!BRepTools::Read(aShape, aPath.c_str(), aBuilder))
  {
    return {};
  }
  return aShape;
}
} // namespace

// The valid Pocket precursor of FreeCAD #30886 isolates the chamfer corner;
// the later, invalid Fillet feature from that document is deliberately not used.
class BRepFilletAPI_ChamferCorner
    : public testing::TestWithParam<std::tuple<double, double, int, int, int>>
{
};

TEST_P(BRepFilletAPI_ChamferCorner, Build_CurvedLivingEdge_PreservesLocalTermination)
{
  const auto [aDistance, aScale, aPlacement, aSupport, aPeriods] = GetParam();
  TopoDS_Shape aSource                                           = readCornerShape();
  BRep_Builder aBuilder;
  ASSERT_FALSE(aSource.IsNull());
  ASSERT_TRUE(BRepCheck_Analyzer(aSource, true, false, true).IsValid());
  // Reparameterize periodic faces without changing any 3D geometry. Both seam
  // p-curves must move together, including domains outside the principal period.
  if (aPeriods != 0)
  {
    std::vector<occ::handle<Geom2d_Curve>> aShiftedCurves;
    for (TopExp_Explorer aFaceIt(aSource, TopAbs_FACE); aFaceIt.More(); aFaceIt.Next())
    {
      const TopoDS_Face         aFace = TopoDS::Face(aFaceIt.Current().Oriented(TopAbs_FORWARD));
      const BRepAdaptor_Surface aSurface(aFace);
      if (!aSurface.IsUPeriodic() && !aSurface.IsVPeriodic())
      {
        continue;
      }
      const gp_Vec2d aShift(aSurface.IsUPeriodic() ? aPeriods * aSurface.UPeriod() : 0.,
                            aSurface.IsVPeriodic() ? aPeriods * aSurface.VPeriod() : 0.);
      NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aFaceEdges;
      TopExp::MapShapes(aFace, TopAbs_EDGE, aFaceEdges);
      for (const TopoDS_Shape& anEdgeShape : aFaceEdges)
      {
        const TopoDS_Edge aForward = TopoDS::Edge(anEdgeShape.Oriented(TopAbs_FORWARD));
        double            aFirst = 0., aLast = 0.;
        const auto        aPCurve = BRep_Tool::CurveOnSurface(aForward, aFace, aFirst, aLast);
        ASSERT_FALSE(aPCurve.IsNull());
        // Adjacent faces may share the same surface and edge representation.
        if (std::find(aShiftedCurves.begin(), aShiftedCurves.end(), aPCurve)
            != aShiftedCurves.end())
        {
          continue;
        }
        auto aShifted = occ::down_cast<Geom2d_Curve>(aPCurve->Translated(aShift));
        aShiftedCurves.push_back(aShifted);
        if (BRep_Tool::IsClosed(aForward, aFace))
        {
          const auto aReverse =
            BRep_Tool::CurveOnSurface(TopoDS::Edge(aForward.Reversed()), aFace, aFirst, aLast);
          ASSERT_FALSE(aReverse.IsNull());
          auto aShiftedReverse = occ::down_cast<Geom2d_Curve>(aReverse->Translated(aShift));
          aShiftedCurves.push_back(aShiftedReverse);
          aBuilder.UpdateEdge(aForward,
                              aShifted,
                              aShiftedReverse,
                              aFace,
                              BRep_Tool::Tolerance(aForward));
        }
        else
        {
          aBuilder.UpdateEdge(aForward, aShifted, aFace, BRep_Tool::Tolerance(aForward));
        }
      }
      BRepTools::UpdateFaceUVPoints(aFace);
    }
    ASSERT_TRUE(BRepCheck_Analyzer(aSource, true, false, true).IsValid());
  }
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> anEdges;
  TopExp::MapShapes(aSource, TopAbs_EDGE, anEdges);
  ASSERT_GE(anEdges.Extent(), 31);
  const TopoDS_Edge anEdge = TopoDS::Edge(anEdges.FindKey(31));
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    anEdgeFaces;
  TopExp::MapShapesAndAncestors(aSource, TopAbs_EDGE, TopAbs_FACE, anEdgeFaces);
  const auto& aSupports = anEdgeFaces.FindFromKey(anEdge);
  ASSERT_EQ(aSupports.Extent(), 2);
  const TopoDS_Face aReference = TopoDS::Face(aSupport == 1 ? aSupports.First() : aSupports.Last());
  gp_Trsf           aScaleTransform;
  aScaleTransform.SetScale(gp_Pnt(), aScale);
  gp_Trsf aPlacementTransform;
  if (aPlacement == 1)
  {
    aPlacementTransform.SetRotation(gp_Ax1(gp_Pnt(), gp_Dir(1, 2, 3)), 0.61);
    aPlacementTransform.SetTranslationPart(gp_Vec(17, -23, 11));
  }
  else if (aPlacement == 2)
  {
    aPlacementTransform.SetMirror(gp_Ax2(gp_Pnt(), gp_Dir(0, 1, 0)));
  }
  const gp_Trsf            aTransform = aPlacementTransform.Multiplied(aScaleTransform);
  BRepBuilderAPI_Transform aMoved(aSource, aTransform, true);
  ASSERT_TRUE(aMoved.IsDone());
  ASSERT_TRUE(BRepCheck_Analyzer(aMoved.Shape(), true, false, true).IsValid());
  BRepFilletAPI_MakeChamfer aChamfer(aMoved.Shape());
  if (aSupport == 0)
  {
    aChamfer.Add(aDistance * aScale, TopoDS::Edge(aMoved.ModifiedShape(anEdge)));
  }
  else
  {
    aChamfer.Add(aDistance * aScale,
                 aDistance * aScale,
                 TopoDS::Edge(aMoved.ModifiedShape(anEdge)),
                 TopoDS::Face(aMoved.ModifiedShape(aReference)));
  }
  ASSERT_NO_THROW(aChamfer.Build());
  ASSERT_TRUE(aChamfer.IsDone());
  const TopoDS_Shape& aResult = aChamfer.Shape();
  EXPECT_TRUE(BRepCheck_Analyzer(aResult, true, false, true).IsValid());
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aSolids;
  TopExp::MapShapes(aResult, TopAbs_SOLID, aSolids);
  EXPECT_EQ(aSolids.Extent(), 1);
  for (TopExp_Explorer anIt(aResult, TopAbs_SHELL); anIt.More(); anIt.Next())
  {
    EXPECT_TRUE(BRep_Tool::IsClosed(TopoDS::Shell(anIt.Current())));
  }
  EXPECT_LT(maximumTolerance(aResult), 0.05 * aScale);
  expectMaterialRemoved(aResult, aMoved.Shape());
  for (const double aSide : {-1., 1.})
  {
    const gp_Pnt anExpected       = gp_Pnt(aDistance, aSide * 8., -6.).Transformed(aTransform);
    double       aNearestDistance = RealLast();
    for (TopExp_Explorer anIt(aResult, TopAbs_VERTEX); anIt.More(); anIt.Next())
    {
      aNearestDistance =
        std::min(aNearestDistance,
                 BRep_Tool::Pnt(TopoDS::Vertex(anIt.Current())).Distance(anExpected));
    }
    EXPECT_LE(aNearestDistance, 1.e-5 * aScale)
      << "both cylinder living edges must align with their adjacent chamfer endpoints";
  }
}

INSTANTIATE_TEST_SUITE_P(CurvedLivingEdges,
                         BRepFilletAPI_ChamferCorner,
                         testing::Combine(testing::Values(0.5, 1.0, 2.0),
                                          testing::Values(0.1, 1.0, 10.0),
                                          testing::Values(0, 1, 2),
                                          testing::Values(0, 1, 2),
                                          testing::Values(-2, 0, 2)));

class BRepFilletAPI_ChamferCornerContours : public testing::TestWithParam<std::tuple<int, bool>>
{
};

TEST_P(BRepFilletAPI_ChamferCornerContours, Build_IndependentContours_UsesOwnSupportFaces)
{
  const auto [aSupport, toReverse] = GetParam();
  const TopoDS_Shape aSource       = readCornerShape();
  ASSERT_FALSE(aSource.IsNull());
  ASSERT_TRUE(BRepCheck_Analyzer(aSource, true, false, true).IsValid());
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> anEdges;
  TopExp::MapShapes(aSource, TopAbs_EDGE, anEdges);
  const TopoDS_Edge anEdge = TopoDS::Edge(anEdges(31));
  gp_Trsf           aTransform;
  aTransform.SetTranslation(gp_Vec(0., 50., 0.));
  BRepBuilderAPI_Transform aMoved(aSource, aTransform, true);
  ASSERT_TRUE(aMoved.IsDone());
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aSource);
  aBuilder.Add(aCompound, aMoved.Shape());
  ASSERT_TRUE(BRepCheck_Analyzer(aCompound, true, false, true).IsValid());
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    anEdgeFaces;
  TopExp::MapShapesAndAncestors(aCompound, TopAbs_EDGE, TopAbs_FACE, anEdgeFaces);
  std::array<TopoDS_Edge, 2> aContours = {anEdge, TopoDS::Edge(aMoved.ModifiedShape(anEdge))};
  if (toReverse)
  {
    std::swap(aContours[0], aContours[1]);
  }
  BRepFilletAPI_MakeChamfer aChamfer(aCompound);
  for (const TopoDS_Edge& aContour : aContours)
  {
    if (aSupport == 0)
    {
      aChamfer.Add(1., aContour);
    }
    else
    {
      const auto& aFaces = anEdgeFaces.FindFromKey(aContour);
      aChamfer.Add(1., 1., aContour, TopoDS::Face(aSupport == 1 ? aFaces.First() : aFaces.Last()));
    }
  }
  ASSERT_NO_THROW(aChamfer.Build());
  ASSERT_TRUE(aChamfer.IsDone());
  const TopoDS_Shape& aResult = aChamfer.Shape();
  EXPECT_TRUE(BRepCheck_Analyzer(aResult, true, false, true).IsValid());
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aSolids;
  TopExp::MapShapes(aResult, TopAbs_SOLID, aSolids);
  ASSERT_EQ(aSolids.Extent(), 2);
  EXPECT_NEAR(shapeVolume(aSolids(1)), shapeVolume(aSolids(2)), shapeVolume(aSource) * 1.e-7);
  expectMaterialRemoved(aResult, aCompound);
}

INSTANTIATE_TEST_SUITE_P(IndependentContours,
                         BRepFilletAPI_ChamferCornerContours,
                         testing::Combine(testing::Values(0, 1, 2), testing::Bool()));

TEST_P(ChamferSurfaceModeMatrix,
       Build_SurfaceFamiliesAndChamferModes_ProducesClosedValidContainedSolid)
{
  const MatrixCase   aCase   = GetParam();
  const TopoDS_Shape anInput = makeFamilyShape(aCase.Family);
  ASSERT_FALSE(anInput.IsNull());
  ASSERT_TRUE(BRepCheck_Analyzer(anInput, true, false, true).IsValid());
  const bool preferShortest =
    (aCase.Family == SurfaceFamily::PlaneTorus && aCase.Mode == ChamferMode::DistanceAngle)
    || (aCase.Family == SurfaceFamily::PlaneExtrusion && aCase.Mode == ChamferMode::TwoDistances);
  const EdgeContext aContext = findFamilyEdge(anInput, aCase.Family, preferShortest);
  ASSERT_FALSE(aContext.Edge.IsNull()) << "surface-pair edge is absent from the fixture";

  TopoDS_Shape aResult;
  ASSERT_NO_THROW(aResult = buildChamfer(anInput, aContext, aCase.Mode));
  ASSERT_FALSE(aResult.IsNull()) << "the surface/mode matrix case did not build";
  // The long torus/plane intersection is concave; its bevel adds material.
  expectClosedValidSolid(aResult,
                         anInput,
                         1.,
                         aCase.Family != SurfaceFamily::PlaneTorus || preferShortest);
  if (aCase.Family <= SurfaceFamily::PlaneTorus)
  {
    EXPECT_TRUE(BRepCheck_Analyzer(aResult, true, false, true).IsValid())
      << "analytic surface cases must also pass exact geometric validation";
  }
}

INSTANTIATE_TEST_SUITE_P(
  AnalyticSurfaceFamilies,
  ChamferSurfaceModeMatrix,
  testing::Values(MatrixCase{SurfaceFamily::PlanePlane, ChamferMode::EqualDistance},
                  MatrixCase{SurfaceFamily::PlanePlane, ChamferMode::TwoDistances},
                  MatrixCase{SurfaceFamily::PlanePlane, ChamferMode::DistanceAngle},
                  MatrixCase{SurfaceFamily::PlaneCylinder, ChamferMode::EqualDistance},
                  MatrixCase{SurfaceFamily::PlaneCylinder, ChamferMode::TwoDistances},
                  MatrixCase{SurfaceFamily::PlaneCylinder, ChamferMode::DistanceAngle},
                  MatrixCase{SurfaceFamily::PlaneCone, ChamferMode::EqualDistance},
                  MatrixCase{SurfaceFamily::PlaneCone, ChamferMode::TwoDistances},
                  MatrixCase{SurfaceFamily::PlaneCone, ChamferMode::DistanceAngle},
                  MatrixCase{SurfaceFamily::PlaneSphere, ChamferMode::EqualDistance},
                  MatrixCase{SurfaceFamily::PlaneSphere, ChamferMode::TwoDistances},
                  MatrixCase{SurfaceFamily::PlaneSphere, ChamferMode::DistanceAngle},
                  MatrixCase{SurfaceFamily::PlaneTorus, ChamferMode::EqualDistance},
                  MatrixCase{SurfaceFamily::PlaneTorus, ChamferMode::TwoDistances},
                  MatrixCase{SurfaceFamily::PlaneTorus, ChamferMode::DistanceAngle},
                  MatrixCase{SurfaceFamily::PlaneExtrusion, ChamferMode::EqualDistance},
                  MatrixCase{SurfaceFamily::PlaneExtrusion, ChamferMode::TwoDistances},
                  MatrixCase{SurfaceFamily::PlaneExtrusion, ChamferMode::DistanceAngle},
                  MatrixCase{SurfaceFamily::PlaneRevolution, ChamferMode::EqualDistance},
                  MatrixCase{SurfaceFamily::PlaneRevolution, ChamferMode::TwoDistances},
                  MatrixCase{SurfaceFamily::PlaneRevolution, ChamferMode::DistanceAngle},
                  MatrixCase{SurfaceFamily::PlaneBSpline, ChamferMode::EqualDistance},
                  MatrixCase{SurfaceFamily::PlaneBSpline, ChamferMode::TwoDistances},
                  MatrixCase{SurfaceFamily::PlaneBSpline, ChamferMode::DistanceAngle}),
  matrixCaseName);

TEST(BRepFilletAPI_ChamferMatrixTest, Build_SwappedAsymmetricReferenceFaces_ProducesEquivalentSolid)
{
  for (const SurfaceFamily aFamily : {SurfaceFamily::PlanePlane,
                                      SurfaceFamily::PlaneCylinder,
                                      SurfaceFamily::PlaneCone,
                                      SurfaceFamily::PlaneSphere,
                                      SurfaceFamily::PlaneTorus,
                                      SurfaceFamily::PlaneExtrusion,
                                      SurfaceFamily::PlaneRevolution,
                                      SurfaceFamily::PlaneBSpline})
  {
    SCOPED_TRACE(familyName(aFamily));
    const TopoDS_Shape anInput = makeFamilyShape(aFamily);
    const EdgeContext  aContext =
      findFamilyEdge(anInput, aFamily, aFamily == SurfaceFamily::PlaneExtrusion);
    const TopoDS_Shape aForward =
      buildChamfer(anInput, aContext, ChamferMode::TwoDistances, 1.0, false);
    const TopoDS_Shape aReverse =
      buildChamfer(anInput, aContext, ChamferMode::TwoDistances, 1.0, true);
    ASSERT_FALSE(aForward.IsNull());
    ASSERT_FALSE(aReverse.IsNull());
    expectClosedValidSolid(aForward, anInput, 1., aFamily != SurfaceFamily::PlaneTorus);
    expectClosedValidSolid(aReverse, anInput, 1., aFamily != SurfaceFamily::PlaneTorus);
    EXPECT_NEAR(shapeVolume(aForward), shapeVolume(aReverse), shapeVolume(anInput) * 1.0e-8);
    // Equal volume alone does not establish equivalent chamfer geometry. Use the existing
    // Boolean facility to check that neither reference ordering leaves additional material.
    BRepAlgoAPI_Cut aForwardOnly(aForward, aReverse);
    BRepAlgoAPI_Cut aReverseOnly(aReverse, aForward);
    ASSERT_TRUE(aForwardOnly.IsDone());
    ASSERT_TRUE(aReverseOnly.IsDone());
    EXPECT_NEAR(shapeVolume(aForwardOnly.Shape()), 0.0, shapeVolume(anInput) * 1.0e-8);
    EXPECT_NEAR(shapeVolume(aReverseOnly.Shape()), 0.0, shapeVolume(anInput) * 1.0e-8);
  }
}

TEST(BRepFilletAPI_ChamferMatrixTest,
     Build_TaperedArmCylinderContactAngles_ProducesClosedValidSolid)
{
  const double aRadius   = 10.0;
  const double aDistance = 0.5;
  // Ratios approach, but deliberately do not equal, the tangent limit.  This sweeps ordinary
  // transverse joins, shallow joins, and the numerically sensitive near-tangent corner while all
  // inputs remain exact, closed, valid solids.
  for (const double aWidthRatio : {0.20, 0.50, 0.80, 0.95, 0.995})
  {
    const double       aHalfWidth = aRadius * aWidthRatio;
    const TopoDS_Shape anInput    = makeTaperedArmCylinder(aRadius, aHalfWidth);
    SCOPED_TRACE("width/radius=" + std::to_string(aWidthRatio));
    ASSERT_FALSE(anInput.IsNull());
    ASSERT_TRUE(BRepCheck_Analyzer(anInput, true, false, true).IsValid());

    for (const bool isPositiveSide : {false, true})
    {
      const EdgeContext aBaseContext =
        findTaperedArmEdge(anInput, aRadius, aHalfWidth, isPositiveSide);
      ASSERT_FALSE(aBaseContext.Edge.IsNull());
      for (const bool isReversed : {false, true})
      {
        EdgeContext aContext = aBaseContext;
        if (isReversed)
        {
          aContext.Edge.Reverse();
        }
        SCOPED_TRACE(std::string(isPositiveSide ? "positive" : "negative")
                     + (isReversed ? "/reversed" : "/forward"));

        BRepFilletAPI_MakeChamfer aSymmetric(anInput);
        aSymmetric.Add(aDistance, aContext.Edge);
        aSymmetric.Build();
        ASSERT_TRUE(aSymmetric.IsDone());
        expectClosedValidSolid(aSymmetric.Shape(), anInput);
        expectMaterialRemoved(aSymmetric.Shape(), anInput);

        const TopoDS_Shape aFirstReference =
          buildTwoDistanceChamfer(anInput, aContext, aDistance, aDistance, false);
        const TopoDS_Shape aSecondReference =
          buildTwoDistanceChamfer(anInput, aContext, aDistance, aDistance, true);
        ASSERT_FALSE(aFirstReference.IsNull());
        ASSERT_FALSE(aSecondReference.IsNull());
        expectClosedValidSolid(aFirstReference, anInput);
        expectClosedValidSolid(aSecondReference, anInput);
        expectMaterialRemoved(aFirstReference, anInput);
        expectMaterialRemoved(aSecondReference, anInput);
        const double aVolumeTolerance = shapeVolume(anInput) * 1.0e-8;
        EXPECT_NEAR(shapeVolume(aFirstReference), shapeVolume(aSecondReference), aVolumeTolerance);
        EXPECT_NEAR(shapeVolume(aFirstReference),
                    shapeVolume(aSymmetric.Shape()),
                    aVolumeTolerance);
      }
    }
  }
}

TEST(BRepFilletAPI_ChamferMatrixTest, Build_RigidMirrorAndScaleTransforms_PreservesValidity)
{
  struct TransformCase
  {
    const char* Name;
    gp_Trsf     Transform;
    double      Scale;
  };

  gp_Trsf aRigid;
  aRigid.SetRotation(gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(1, 2, 3)), 0.73);
  aRigid.SetTranslationPart(gp_Vec(17, -11, 23));
  gp_Trsf aMirror;
  aMirror.SetMirror(gp_Ax2(gp_Pnt(2, -3, 1), gp_Dir(1, 1, 2)));
  gp_Trsf aSmallScale;
  aSmallScale.SetScale(gp_Pnt(0, 0, 0), 1.0e-2);
  gp_Trsf aLargeScale;
  aLargeScale.SetScale(gp_Pnt(0, 0, 0), 1.0e3);

  const std::array<TransformCase, 4> aTransforms = {{{"Rigid", aRigid, 1.0},
                                                     {"Mirror", aMirror, 1.0},
                                                     {"Scale1eMinus2", aSmallScale, 1.0e-2},
                                                     {"Scale1e3", aLargeScale, 1.0e3}}};
  for (const SurfaceFamily aFamily : {SurfaceFamily::PlanePlane,
                                      SurfaceFamily::PlaneCylinder,
                                      SurfaceFamily::PlaneCone,
                                      SurfaceFamily::PlaneSphere,
                                      SurfaceFamily::PlaneTorus,
                                      SurfaceFamily::PlaneExtrusion,
                                      SurfaceFamily::PlaneRevolution,
                                      SurfaceFamily::PlaneBSpline})
  {
    const TopoDS_Shape anInput  = makeFamilyShape(aFamily);
    const EdgeContext  aContext = findFamilyEdge(anInput, aFamily);
    ASSERT_FALSE(aContext.Edge.IsNull());
    for (const TransformCase& aTransformCase : aTransforms)
    {
      SCOPED_TRACE(std::string(familyName(aFamily)) + "/" + aTransformCase.Name);
      BRepBuilderAPI_Transform aTransform(anInput, aTransformCase.Transform, true);
      ASSERT_TRUE(aTransform.IsDone());
      EdgeContext aTransformedContext;
      aTransformedContext.Edge       = TopoDS::Edge(aTransform.ModifiedShape(aContext.Edge));
      aTransformedContext.FirstFace  = TopoDS::Face(aTransform.ModifiedShape(aContext.FirstFace));
      aTransformedContext.SecondFace = TopoDS::Face(aTransform.ModifiedShape(aContext.SecondFace));
      ASSERT_FALSE(aTransformedContext.Edge.IsNull());
      const TopoDS_Shape aTransformedInput = aTransform.Shape();
      const TopoDS_Shape aResult           = buildChamfer(aTransformedInput,
                                                aTransformedContext,
                                                ChamferMode::EqualDistance,
                                                aTransformCase.Scale);
      ASSERT_FALSE(aResult.IsNull());
      expectClosedValidSolid(aResult,
                             aTransformedInput,
                             aTransformCase.Scale,
                             aFamily != SurfaceFamily::PlaneTorus);
    }
  }
}

TEST(BRepFilletAPI_ChamferMatrixTest, Build_ReversedEdgeOrientationAndOrder_PreservesVolume)
{
  const TopoDS_Shape       anInput = BRepPrimAPI_MakeBox(20.0, 18.0, 16.0).Shape();
  std::vector<TopoDS_Edge> anEdges;
  for (TopExp_Explorer anExp(anInput, TopAbs_EDGE); anExp.More(); anExp.Next())
  {
    anEdges.push_back(TopoDS::Edge(anExp.Current()));
  }
  ASSERT_GE(anEdges.size(), 7u);

  BRepFilletAPI_MakeChamfer aForward(anInput);
  aForward.Add(0.5, anEdges[0]);
  aForward.Add(0.5, anEdges[6]);
  aForward.Build();
  ASSERT_TRUE(aForward.IsDone());
  expectClosedValidSolid(aForward.Shape(), anInput);

  BRepFilletAPI_MakeChamfer aReverse(anInput);
  aReverse.Add(0.5, TopoDS::Edge(anEdges[6].Reversed()));
  aReverse.Add(0.5, TopoDS::Edge(anEdges[0].Reversed()));
  aReverse.Build();
  ASSERT_TRUE(aReverse.IsDone());
  expectClosedValidSolid(aReverse.Shape(), anInput);
  EXPECT_NEAR(shapeVolume(aForward.Shape()),
              shapeVolume(aReverse.Shape()),
              shapeVolume(anInput) * 1.e-8);
}

TEST(BRepFilletAPI_ChamferMatrixTest, Build_AnalyticEdgesAtScale1eMinus3_ProducesClosedValidSolid)
{
  gp_Trsf aScaleTransform;
  aScaleTransform.SetScale(gp_Pnt(0, 0, 0), 1.0e-3);
  for (const SurfaceFamily aFamily :
       {SurfaceFamily::PlanePlane, SurfaceFamily::PlaneCylinder, SurfaceFamily::PlaneCone})
  {
    SCOPED_TRACE(familyName(aFamily));
    const TopoDS_Shape       anInput  = makeFamilyShape(aFamily);
    const EdgeContext        aContext = findFamilyEdge(anInput, aFamily);
    BRepBuilderAPI_Transform aTransform(anInput, aScaleTransform, true);
    ASSERT_TRUE(aTransform.IsDone());
    const TopoDS_Shape aTransformedInput = aTransform.Shape();
    const TopoDS_Edge  aTransformedEdge  = TopoDS::Edge(aTransform.ModifiedShape(aContext.Edge));
    BRepFilletAPI_MakeChamfer aChamfer(aTransformedInput);
    aChamfer.Add(5.0e-4, aTransformedEdge);
    aChamfer.Build();
    ASSERT_TRUE(aChamfer.IsDone());
    expectClosedValidSolid(aChamfer.Shape(), aTransformedInput, 1.0e-3);
  }
}

TEST(BRepFilletAPI_ChamferMatrixTest,
     Build_ShortEdgesAndThresholdPerturbations_ProducesClosedValidSolid)
{
  for (const double aThickness : {1.0e-3, 1.0e-2, 1.0e-1, 1.0})
  {
    SCOPED_TRACE(aThickness);
    const TopoDS_Shape anInput  = BRepPrimAPI_MakeBox(20.0, 18.0, aThickness).Shape();
    const EdgeContext  aContext = findFamilyEdge(anInput, SurfaceFamily::PlanePlane);
    ASSERT_FALSE(aContext.Edge.IsNull());
    BRepFilletAPI_MakeChamfer aChamfer(anInput);
    aChamfer.Add(0.2 * aThickness, aContext.Edge);
    aChamfer.Build();
    ASSERT_TRUE(aChamfer.IsDone());
    expectClosedValidSolid(aChamfer.Shape(), anInput, std::max(1.0e-3, aThickness));
  }

  const TopoDS_Shape anInput  = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  const EdgeContext  aContext = findFamilyEdge(anInput, SurfaceFamily::PlanePlane);
  for (const double aDistance : {1.0e-4, 1.0e-3, 0.1, 4.9, 4.999})
  {
    SCOPED_TRACE(aDistance);
    // This is a geometric-scale test, not a test of the public API's fixed
    // 1.e-4 approximation budget. Keep that budget below the smallest feature.
    ChFi3d_ChBuilder aChamfer(anInput);
    aChamfer.SetParams(1.e-2, 1.e-7, 1.e-8, 1.e-7, 1.e-8, 1.e-3);
    aChamfer.Add(aDistance, aContext.Edge);
    aChamfer.Compute();
    ASSERT_TRUE(aChamfer.IsDone());
    expectClosedValidSolid(aChamfer.Shape(), anInput);
    GProp_GProps aLength;
    BRepGProp::LinearProperties(aContext.Edge, aLength);
    EXPECT_NEAR(shapeVolume(aChamfer.Shape()),
                1000. - .5 * aDistance * aDistance * aLength.Mass(),
                1.e-9);
  }
}

TEST(BRepFilletAPI_ChamferMatrixTest, Build_ComplexCorners_ProducesValidSolidWhenSuccessful)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  const TopoDS_Shape aCylinder =
    BRepPrimAPI_MakeCylinder(gp_Ax2(gp_Pnt(5.0, 0.0, 5.0), gp_Dir(0.0, 1.0, 0.0)), 3.0, 10.0)
      .Shape();
  BRepAlgoAPI_Fuse aFuse(aBox, aCylinder);
  ASSERT_TRUE(aFuse.IsDone());
  const TopoDS_Shape aFused = aFuse.Shape();
  ASSERT_TRUE(BRepCheck_Analyzer(aFused, true, false, true).IsValid());

  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    aVertexFaceMap;
  TopExp::MapShapesAndAncestors(aFused, TopAbs_VERTEX, TopAbs_FACE, aVertexFaceMap);

  int aComplexAttemptCount = 0;
  int aSuccessCount        = 0;
  for (TopExp_Explorer anEdgeExp(aFused, TopAbs_EDGE); anEdgeExp.More(); anEdgeExp.Next())
  {
    const TopoDS_Edge anEdge    = TopoDS::Edge(anEdgeExp.Current());
    bool              isComplex = false;
    for (TopExp_Explorer aVertexExp(anEdge, TopAbs_VERTEX); aVertexExp.More(); aVertexExp.Next())
    {
      if (aVertexFaceMap.Contains(aVertexExp.Current())
          && aVertexFaceMap.FindFromKey(aVertexExp.Current()).Size() >= 3)
      {
        isComplex = true;
      }
    }
    if (!isComplex)
    {
      continue;
    }
    ++aComplexAttemptCount;
    BRepFilletAPI_MakeChamfer aChamfer(aFused);
    aChamfer.Add(0.25, anEdge);
    try
    {
      aChamfer.Build();
    }
    catch (const Standard_Failure&)
    {
      continue;
    }
    if (aChamfer.IsDone())
    {
      ++aSuccessCount;
      expectClosedValidSolid(aChamfer.Shape(), aFused);
    }
  }
  EXPECT_GT(aComplexAttemptCount, 0);
  EXPECT_GT(aSuccessCount, 0) << "the complex-corner sweep did not exercise a successful path";
}

TEST(BRepFilletAPI_ChamferMatrixTest, Build_SeededTransformAndDistanceSweep_ProducesValidSolid)
{
  // A fixed generator makes every failure exactly reproducible while sampling points between the
  // hand-picked matrix values.  Keep the arithmetic local instead of std::uniform_distribution,
  // whose mapping is not required to be identical across standard-library implementations.
  std::uint64_t aState   = UINT64_C(0x30886c0ffee12345);
  const auto    nextUnit = [&aState]() {
    aState = aState * UINT64_C(6364136223846793005) + UINT64_C(1442695040888963407);
    return static_cast<double>(aState >> 11) * (1.0 / 9007199254740992.0);
  };

  const std::array<SurfaceFamily, 8> aFamilies = {{SurfaceFamily::PlanePlane,
                                                   SurfaceFamily::PlaneCylinder,
                                                   SurfaceFamily::PlaneCone,
                                                   SurfaceFamily::PlaneSphere,
                                                   SurfaceFamily::PlaneTorus,
                                                   SurfaceFamily::PlaneExtrusion,
                                                   SurfaceFamily::PlaneRevolution,
                                                   SurfaceFamily::PlaneBSpline}};
  for (int anIteration = 0; anIteration < 64; ++anIteration)
  {
    const SurfaceFamily aFamily =
      aFamilies[static_cast<std::size_t>(anIteration) % aFamilies.size()];
    const TopoDS_Shape anInput  = makeFamilyShape(aFamily);
    const EdgeContext  aContext = findFamilyEdge(anInput, aFamily);
    ASSERT_FALSE(aContext.Edge.IsNull());

    const double aScale = std::pow(10.0, -2.0 + 4.0 * nextUnit());
    gp_Dir       aDirection(0.2 + nextUnit(), 0.2 + nextUnit(), 0.2 + nextUnit());
    gp_Trsf      aRotation;
    aRotation.SetRotation(gp_Ax1(gp_Pnt(0, 0, 0), aDirection), 2.0 * M_PI * nextUnit());
    gp_Trsf aScaleTransform;
    aScaleTransform.SetScale(gp_Pnt(0, 0, 0), aScale);
    const gp_Trsf            aTransformValue = aRotation.Multiplied(aScaleTransform);
    BRepBuilderAPI_Transform aTransform(anInput, aTransformValue, true);
    ASSERT_TRUE(aTransform.IsDone());

    const TopoDS_Shape aTransformedInput = aTransform.Shape();
    const TopoDS_Edge  aTransformedEdge  = TopoDS::Edge(aTransform.ModifiedShape(aContext.Edge));
    const double       aDistance         = aScale * (0.05 + 0.45 * nextUnit());
    SCOPED_TRACE(std::string("seed=0x30886c0ffee12345 iteration=") + std::to_string(anIteration)
                 + " family=" + familyName(aFamily) + " scale=" + std::to_string(aScale)
                 + " distance=" + std::to_string(aDistance));

    BRepFilletAPI_MakeChamfer aChamfer(aTransformedInput);
    aChamfer.Add(aDistance, aTransformedEdge);
    aChamfer.Build();
    ASSERT_TRUE(aChamfer.IsDone());
    expectClosedValidSolid(aChamfer.Shape(),
                           aTransformedInput,
                           aScale,
                           aFamily != SurfaceFamily::PlaneTorus);
  }
}
