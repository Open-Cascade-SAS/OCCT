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

// A fillet or chamfer that ends at a vertex with two tangent consecutive edges: the end of the
// stripe is intersected with the face beyond the tangent pair, and the edge that bounds that face
// at the stripe end has no pcurve on it. The stripe end point is then projected on that face; the
// builder declines when the point is farther from the face than 10 * 1.e-4.

#include <BOPAlgo_ArgumentAnalyzer.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepFilletAPI_MakeChamfer.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <BRepGProp.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <Geom2d_Curve.hxx>
#include <GeomAbs_CurveType.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <GProp_GProps.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopLoc_Location.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

#include <gtest/gtest.h>

#include <cmath>
#include <tuple>
#include <vector>

namespace
{
constexpr double THE_Size = 1.0;

//! Block of 60 x 30 x 100 with a roof on the part of its top face where y is above -15: a half
//! cylinder of radius 30 with its axis along Y, or a box. The edge between the top face of the
//! block and the end face of the roof is concave and ends on the two side walls of the block.
//! The walls are tangent to the half cylinder along the edge that continues the top edge of the
//! wall. The order of the arguments of the fuse decides which face of the filleted edge OCCT
//! treats as the first one.
TopoDS_Shape makeBlockWithRoof(const bool theCylindricalRoof, const bool theRoofFirst)
{
  const TopoDS_Shape aBlock =
    BRepPrimAPI_MakeBox(gp_Pnt(-30.0, -30.0, 0.0), 60.0, 30.0, 100.0).Shape();
  const gp_Ax2 aRoofAxes(gp_Pnt(0.0, -15.0, 100.0), gp_Dir(0.0, 1.0, 0.0), gp_Dir(-1.0, 0.0, 0.0));
  const TopoDS_Shape aRoof =
    theCylindricalRoof ? BRepPrimAPI_MakeCylinder(aRoofAxes, 30.0, 15.0, M_PI).Shape()
                       : BRepPrimAPI_MakeBox(gp_Pnt(-30.0, -15.0, 100.0), 60.0, 15.0, 30.0).Shape();
  return theRoofFirst ? BRepAlgoAPI_Fuse(aRoof, aBlock).Shape()
                      : BRepAlgoAPI_Fuse(aBlock, aRoof).Shape();
}

//! Like makeBlockWithRoof(true, ...), but the part of the top face in front of the roof slopes down
//! toward the front by theSlope per unit length, so the edge before the vertex leaves the line of
//! the edge after it. The axis of the roof is theCrease below the top face, so the roof meets the
//! walls at an angle and the stripe end point is off the roof surface by about
//! theSlope * theCrease / sqrt(900 + theCrease^2) mm (a tangent roof would give the square of
//! that).
TopoDS_Shape makeSlopedBlockWithRoof(const double theSlope,
                                     const double theCrease,
                                     const bool   theRoofFirst)
{
  BRepBuilderAPI_MakePolygon aProfile;
  aProfile.Add(gp_Pnt(-30.0, -30.0, 0.0));
  aProfile.Add(gp_Pnt(-30.0, 0.0, 0.0));
  aProfile.Add(gp_Pnt(-30.0, 0.0, 100.0));
  aProfile.Add(gp_Pnt(-30.0, -15.0, 100.0));
  aProfile.Add(gp_Pnt(-30.0, -30.0, 100.0 - 15.0 * theSlope));
  aProfile.Close();
  const TopoDS_Shape aBlock =
    BRepPrimAPI_MakePrism(BRepBuilderAPI_MakeFace(aProfile.Wire()).Face(), gp_Vec(60.0, 0.0, 0.0))
      .Shape();
  const double       aRadius = std::sqrt(900.0 + theCrease * theCrease);
  const gp_Ax2       aRoofAxes(gp_Pnt(0.0, -15.0, 100.0 - theCrease),
                         gp_Dir(0.0, 1.0, 0.0),
                         gp_Dir(-1.0, 0.0, 0.0));
  const TopoDS_Shape aRoof =
    BRepAlgoAPI_Common(
      BRepPrimAPI_MakeCylinder(aRoofAxes, aRadius, 15.0).Shape(),
      BRepPrimAPI_MakeBox(gp_Pnt(-40.0, -15.0, 100.0), 80.0, 15.0, aRadius + 5.0).Shape())
      .Shape();
  return theRoofFirst ? BRepAlgoAPI_Fuse(aRoof, aBlock).Shape()
                      : BRepAlgoAPI_Fuse(aBlock, aRoof).Shape();
}

//! Returns a straight edge of the concave edge between the top of the block and the roof, the
//! one that ends on the wall x = 30.
TopoDS_Edge findConcaveEdge(const TopoDS_Shape& theShape)
{
  for (TopExp_Explorer anExp(theShape, TopAbs_EDGE); anExp.More(); anExp.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(anExp.Current());
    BRepAdaptor_Curve  aCurve(anEdge);
    const gp_Pnt       aFirst = aCurve.Value(aCurve.FirstParameter());
    const gp_Pnt       aLast  = aCurve.Value(aCurve.LastParameter());
    if (aCurve.GetType() == GeomAbs_Line && std::abs(aFirst.Y() + 15.0) < 1.e-7
        && std::abs(aLast.Y() + 15.0) < 1.e-7 && std::abs(aFirst.Z() - 100.0) < 1.e-7
        && std::abs(aLast.Z() - 100.0) < 1.e-7 && std::max(aFirst.X(), aLast.X()) > 29.9)
    {
      return anEdge;
    }
  }
  return TopoDS_Edge();
}

//! Volume of the shape.
double volumeOf(const TopoDS_Shape& theShape)
{
  GProp_GProps aProps;
  BRepGProp::VolumeProperties(theShape, aProps);
  return aProps.Mass();
}

//! Number of faces of the shape.
int faceCount(const TopoDS_Shape& theShape)
{
  int aCount = 0;
  for (TopExp_Explorer anExp(theShape, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    ++aCount;
  }
  return aCount;
}

//! Largest tolerance of the vertices of the shape.
double maxVertexTolerance(const TopoDS_Shape& theShape)
{
  double aMax = 0.0;
  for (TopExp_Explorer anExp(theShape, TopAbs_VERTEX); anExp.More(); anExp.Next())
  {
    aMax = std::max(aMax, BRep_Tool::Tolerance(TopoDS::Vertex(anExp.Current())));
  }
  return aMax;
}

//! Checks a blend of the concave edge: valid solid, tolerances of the input, no fault found by the
//! boolean analyzer, and the volume the blend adds along the 60 long edge. The measured volumes
//! are within 0.004 of the analytic value, the tolerance is 0.02.
void expectExactBlend(const TopoDS_Shape& theInput,
                      const TopoDS_Shape& theResult,
                      const double        theAddedVolume)
{
  EXPECT_TRUE(BRepCheck_Analyzer(theResult).IsValid());
  EXPECT_LT(maxVertexTolerance(theResult), 1.e-3);
  BOPAlgo_ArgumentAnalyzer anAnalyzer;
  anAnalyzer.SetShape1(theResult);
  anAnalyzer.SelfInterMode() = true;
  anAnalyzer.SmallEdgeMode() = true;
  anAnalyzer.Perform();
  EXPECT_FALSE(anAnalyzer.HasFaulty());
  EXPECT_NEAR(volumeOf(theResult) - volumeOf(theInput), theAddedVolume, 0.02);
}

//! Volume a fillet of radius THE_Size adds along a concave right angle edge of length 60.
constexpr double THE_FilletVolume = (1.0 - M_PI / 4.0) * THE_Size * THE_Size * 60.0;

//! Volume a chamfer of distance THE_Size adds along a concave right angle edge of length 60.
constexpr double THE_ChamferVolume = 0.5 * THE_Size * THE_Size * 60.0;

//! Fillets the concave edge of the block with a roof and checks the result.
void checkFillet(const bool theCylindricalRoof, const bool theRoofFirst)
{
  const TopoDS_Shape aShape = makeBlockWithRoof(theCylindricalRoof, theRoofFirst);
  ASSERT_TRUE(BRepCheck_Analyzer(aShape).IsValid());
  const TopoDS_Edge anEdge = findConcaveEdge(aShape);
  ASSERT_FALSE(anEdge.IsNull());

  BRepFilletAPI_MakeFillet aFillet(aShape);
  aFillet.Add(THE_Size, anEdge);
  aFillet.Build();
  ASSERT_TRUE(aFillet.IsDone());
  expectExactBlend(aShape, aFillet.Shape(), THE_FilletVolume);
}

//! Chamfers the concave edge of the block with a roof and checks the result.
void checkChamfer(const bool theCylindricalRoof, const bool theRoofFirst)
{
  const TopoDS_Shape aShape = makeBlockWithRoof(theCylindricalRoof, theRoofFirst);
  ASSERT_TRUE(BRepCheck_Analyzer(aShape).IsValid());
  const TopoDS_Edge anEdge = findConcaveEdge(aShape);
  ASSERT_FALSE(anEdge.IsNull());

  BRepFilletAPI_MakeChamfer aChamfer(aShape);
  aChamfer.Add(THE_Size, anEdge);
  aChamfer.Build();
  ASSERT_TRUE(aChamfer.IsDone());
  expectExactBlend(aShape, aChamfer.Shape(), THE_ChamferVolume);
}

//! Block and roof revolved about the Z axis over 90 degrees: the walls are cylinders and the roof
//! is a half torus tangent to the outer wall along its top circle. The concave edge is the radius
//! at 45 degrees where the roof starts, 30 long.
TopoDS_Shape makeRevolvedBlockWithTorusRoof()
{
  const gp_Ax1               anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  BRepBuilderAPI_MakePolygon aSection;
  aSection.Add(gp_Pnt(30.0, 0.0, 0.0));
  aSection.Add(gp_Pnt(60.0, 0.0, 0.0));
  aSection.Add(gp_Pnt(60.0, 0.0, 100.0));
  aSection.Add(gp_Pnt(30.0, 0.0, 100.0));
  aSection.Close();
  const TopoDS_Shape aBlock =
    BRepPrimAPI_MakeRevol(BRepBuilderAPI_MakeFace(aSection.Wire()).Face(), anAxis, M_PI / 2)
      .Shape();
  BRepBuilderAPI_MakeWire aHalfDisc;
  aHalfDisc.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(30.0, 0.0, 100.0), gp_Pnt(60.0, 0.0, 100.0)));
  aHalfDisc.Add(BRepBuilderAPI_MakeEdge(
    GC_MakeArcOfCircle(gp_Pnt(60.0, 0.0, 100.0), gp_Pnt(45.0, 0.0, 115.0), gp_Pnt(30.0, 0.0, 100.0))
      .Value()));
  const TopoDS_Shape aSector =
    BRepPrimAPI_MakeRevol(BRepBuilderAPI_MakeFace(aHalfDisc.Wire()).Face(), anAxis, M_PI / 4)
      .Shape();
  gp_Trsf aRotation;
  aRotation.SetRotation(anAxis, M_PI / 4);
  const TopoDS_Shape aRoof = BRepBuilderAPI_Transform(aSector, aRotation, true).Shape();
  return BRepAlgoAPI_Fuse(aBlock, aRoof).Shape();
}

//! Returns the straight edge of makeRevolvedBlockWithTorusRoof() that is the concave edge.
TopoDS_Edge findRadialEdge(const TopoDS_Shape& theShape)
{
  for (TopExp_Explorer anExp(theShape, TopAbs_EDGE); anExp.More(); anExp.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(anExp.Current());
    BRepAdaptor_Curve  aCurve(anEdge);
    const gp_Pnt       aFirst = aCurve.Value(aCurve.FirstParameter());
    const gp_Pnt       aLast  = aCurve.Value(aCurve.LastParameter());
    if (aCurve.GetType() == GeomAbs_Line && std::abs(aFirst.Z() - 100.0) < 1.e-7
        && std::abs(aLast.Z() - 100.0) < 1.e-7
        && std::abs(std::atan2(aFirst.Y(), aFirst.X()) - M_PI / 4) < 1.e-7
        && std::abs(std::atan2(aLast.Y(), aLast.X()) - M_PI / 4) < 1.e-7
        && aFirst.Distance(aLast) > 29.0)
    {
      return anEdge;
    }
  }
  return TopoDS_Edge();
}

//! Copy of theShape in which the cylindrical face has a Geom_RectangularTrimmedSurface on the same
//! cylinder, with the parameter range of the face. The pcurves of its edges are attached to it.
TopoDS_Shape withTrimmedCylinder(const TopoDS_Shape& theShape)
{
  const TopoDS_Shape aCopy = BRepBuilderAPI_Copy(theShape, true).Shape();
  BRep_Builder       aBuilder;
  for (TopExp_Explorer anExp(aCopy, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    const TopoDS_Face& aFace = TopoDS::Face(anExp.Current());
    if (BRepAdaptor_Surface(aFace).GetType() != GeomAbs_Cylinder)
    {
      continue;
    }
    double aUMin, aUMax, aVMin, aVMax;
    BRepTools::UVBounds(aFace, aUMin, aUMax, aVMin, aVMax);
    TopLoc_Location                 aLocation;
    const occ::handle<Geom_Surface> aTrimmed =
      new Geom_RectangularTrimmedSurface(BRep_Tool::Surface(aFace, aLocation),
                                         aUMin,
                                         aUMax,
                                         aVMin,
                                         aVMax);
    std::vector<std::tuple<TopoDS_Edge, occ::handle<Geom2d_Curve>, double, double>> aPCurves;
    for (TopExp_Explorer anEdges(aFace, TopAbs_EDGE); anEdges.More(); anEdges.Next())
    {
      const TopoDS_Edge&        anEdge = TopoDS::Edge(anEdges.Current());
      double                    aFirst, aLast;
      occ::handle<Geom2d_Curve> aPCurve = BRep_Tool::CurveOnSurface(anEdge, aFace, aFirst, aLast);
      aPCurves.emplace_back(anEdge, aPCurve, aFirst, aLast);
    }
    aBuilder.UpdateFace(aFace, aTrimmed, aLocation, BRep_Tool::Tolerance(aFace));
    for (const auto& [anEdge, aPCurve, aFirst, aLast] : aPCurves)
    {
      aBuilder.UpdateEdge(anEdge, aPCurve, aTrimmed, aLocation, BRep_Tool::Tolerance(anEdge));
      aBuilder.Range(anEdge, aTrimmed, aLocation, aFirst, aLast);
    }
  }
  return aCopy;
}

//! Fillets or chamfers theEdge with THE_Size. Returns false if the builder declines.
bool blend(const TopoDS_Shape& theShape,
           const TopoDS_Edge&  theEdge,
           const bool          theChamfer,
           TopoDS_Shape&       theResult)
{
  if (theChamfer)
  {
    BRepFilletAPI_MakeChamfer aChamfer(theShape);
    aChamfer.Add(THE_Size, theEdge);
    aChamfer.Build();
    theResult = aChamfer.IsDone() ? aChamfer.Shape() : TopoDS_Shape();
    return aChamfer.IsDone();
  }
  BRepFilletAPI_MakeFillet aFillet(theShape);
  aFillet.Add(THE_Size, theEdge);
  aFillet.Build();
  theResult = aFillet.IsDone() ? aFillet.Shape() : TopoDS_Shape();
  return aFillet.IsDone();
}

//! Blends the concave edge of the sloped block with THE_Size, as a chamfer or a fillet. Checks that
//! the builder declines, or that it builds a valid solid that has more faces than the input and the
//! vertex tolerance of the input. A slope of 1e-5 or more can leave tiny edges in the result
//! whatever the roof, so the case that must build uses a smaller one and stays valid.
void checkSloped(const double theSlope,
                 const double theCrease,
                 const bool   theChamfer,
                 const bool   theExpectDone)
{
  for (const bool aRoofFirst : {false, true})
  {
    const TopoDS_Shape aShape = makeSlopedBlockWithRoof(theSlope, theCrease, aRoofFirst);
    ASSERT_TRUE(BRepCheck_Analyzer(aShape).IsValid());
    const TopoDS_Edge anEdge = findConcaveEdge(aShape);
    ASSERT_FALSE(anEdge.IsNull());
    TopoDS_Shape aResult;
    ASSERT_EQ(blend(aShape, anEdge, theChamfer, aResult), theExpectDone);
    if (theExpectDone)
    {
      EXPECT_TRUE(BRepCheck_Analyzer(aResult).IsValid());
      EXPECT_GT(faceCount(aResult), faceCount(aShape));
      EXPECT_LT(maxVertexTolerance(aResult), 1.e-3);
    }
  }
}

//! Blends the concave edge of the block with the torus roof and checks the result; the blend is
//! 30 long.
void checkTorusRoof(const bool theChamfer)
{
  const TopoDS_Shape aShape = makeRevolvedBlockWithTorusRoof();
  ASSERT_TRUE(BRepCheck_Analyzer(aShape).IsValid());
  const TopoDS_Edge anEdge = findRadialEdge(aShape);
  ASSERT_FALSE(anEdge.IsNull());
  TopoDS_Shape aResult;
  ASSERT_TRUE(blend(aShape, anEdge, theChamfer, aResult));
  expectExactBlend(aShape, aResult, 0.5 * (theChamfer ? THE_ChamferVolume : THE_FilletVolume));
}

//! Blends the concave edge of the block with a roof whose cylindrical face has a trimmed surface.
void checkTrimmedRoof(const bool theChamfer)
{
  for (const bool aRoofFirst : {false, true})
  {
    const TopoDS_Shape aShape = withTrimmedCylinder(makeBlockWithRoof(true, aRoofFirst));
    ASSERT_TRUE(BRepCheck_Analyzer(aShape).IsValid());
    const TopoDS_Edge anEdge = findConcaveEdge(aShape);
    ASSERT_FALSE(anEdge.IsNull());
    TopoDS_Shape aResult;
    ASSERT_TRUE(blend(aShape, anEdge, theChamfer, aResult));
    expectExactBlend(aShape, aResult, theChamfer ? THE_ChamferVolume : THE_FilletVolume);
  }
}
} // namespace

TEST(BRepFilletAPI_MakeFilletTest, EndOnTangentEdgePair_FirstFaceBesideRoof_BuildsExactFillet)
{
  checkFillet(true, false);
}

TEST(BRepFilletAPI_MakeFilletTest, EndOnTangentEdgePair_SecondFaceBesideRoof_BuildsExactFillet)
{
  checkFillet(true, true);
}

TEST(BRepFilletAPI_MakeFilletTest, EndOnPlainWall_BoxRoof_BuildsExactFillet)
{
  checkFillet(false, false);
  checkFillet(false, true);
}

TEST(BRepFilletAPI_MakeChamferTest, EndOnTangentEdgePair_FirstFaceBesideRoof_BuildsExactChamfer)
{
  checkChamfer(true, false);
}

TEST(BRepFilletAPI_MakeChamferTest, EndOnTangentEdgePair_SecondFaceBesideRoof_BuildsExactChamfer)
{
  checkChamfer(true, true);
}

TEST(BRepFilletAPI_MakeChamferTest, EndOnPlainWall_BoxRoof_BuildsExactChamfer)
{
  checkChamfer(false, false);
  checkChamfer(false, true);
}

TEST(BRepFilletAPI_MakeFilletTest, EndOnTangentEdgePair_EndPointNearFace_BuildsValidSolid)
{
  checkSloped(5.e-6, 100.0, false, true);
}

TEST(BRepFilletAPI_MakeChamferTest, EndOnTangentEdgePair_EndPointNearFace_BuildsValidSolid)
{
  checkSloped(5.e-6, 100.0, true, true);
}

TEST(BRepFilletAPI_MakeFilletTest, EndOnTangentEdgePair_EndPointFarFromFace_IsDeclined)
{
  checkSloped(0.005, 30.0, false, false);
}

TEST(BRepFilletAPI_MakeChamferTest, EndOnTangentEdgePair_EndPointFarFromFace_IsDeclined)
{
  checkSloped(0.005, 30.0, true, false);
}

TEST(BRepFilletAPI_MakeFilletTest, EndOnTangentEdgePair_TorusRoof_BuildsExactFillet)
{
  checkTorusRoof(false);
}

TEST(BRepFilletAPI_MakeChamferTest, EndOnTangentEdgePair_TorusRoof_BuildsExactChamfer)
{
  checkTorusRoof(true);
}

TEST(BRepFilletAPI_MakeFilletTest, EndOnTangentEdgePair_TrimmedRoofSurface_BuildsExactFillet)
{
  checkTrimmedRoof(false);
}

TEST(BRepFilletAPI_MakeChamferTest, EndOnTangentEdgePair_TrimmedRoofSurface_BuildsExactChamfer)
{
  checkTrimmedRoof(true);
}
