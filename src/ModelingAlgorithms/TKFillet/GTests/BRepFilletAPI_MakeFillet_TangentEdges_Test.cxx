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
// at the stripe end has no pcurve on it.

#include <BOPAlgo_ArgumentAnalyzer.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepFilletAPI_MakeChamfer.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <BRepGProp.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GProp_GProps.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

#include <cmath>

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
//! boolean analyzer, and the volume the blend adds along the 60 long edge. The ends of the fillet
//! on the cylindrical roof leave the walls by up to 0.02 over the radius, so the tolerance on the
//! volume is 0.02.
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
