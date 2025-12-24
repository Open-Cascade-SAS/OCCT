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

#include <BRep_Tool.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepCheck_ListOfStatus.hxx>
#include <BRepCheck_Result.hxx>
#include <BRepPrimAPI_MakeTorus.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <gp_Ax2.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Map.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_ShapeMapHasher.hxx>

#include <gtest/gtest.h>

namespace
{

//! Helper function to create partial torus and check validity.
//! @param theR1 Major radius
//! @param theR2 Minor radius
//! @param theAngle1 First angle on the minor circle (V parameter)
//! @param theAngle2 Second angle on the minor circle (V parameter)
//! @param theAngle Angle around the major circle (U parameter), default 2*PI
//! @return true if the resulting shape is valid
bool createAndCheckPartialTorus(double theR1,
                                double theR2,
                                double theAngle1,
                                double theAngle2,
                                double theAngle = 2.0 * M_PI)
{
  gp_Ax2                anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  BRepPrimAPI_MakeTorus aMakeTorus(anAxis, theR1, theR2, theAngle1, theAngle2, theAngle);
  TopoDS_Shape          aShape = aMakeTorus.Shape();

  if (!aMakeTorus.IsDone() || aShape.IsNull())
  {
    return false;
  }

  BRepCheck_Analyzer anAnalyzer(aShape);
  return anAnalyzer.IsValid();
}

} // namespace

// Test full torus creation (no angle parameters)
TEST(BRepPrimAPI_MakeTorusTest, FullTorus)
{
  const double R1 = 10.0;
  const double R2 = 2.0;

  BRepPrimAPI_MakeTorus aMakeTorus(R1, R2);
  TopoDS_Shape          aShape = aMakeTorus.Shape();

  ASSERT_TRUE(aMakeTorus.IsDone()) << "Full torus creation failed";
  ASSERT_FALSE(aShape.IsNull()) << "Resulting shape is null";

  BRepCheck_Analyzer anAnalyzer(aShape);
  EXPECT_TRUE(anAnalyzer.IsValid()) << "Full torus shape is not valid";

  // A full torus should have exactly 1 face
  int aFaceCount = 0;
  for (TopExp_Explorer anExp(aShape, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    aFaceCount++;
  }
  EXPECT_EQ(aFaceCount, 1) << "Expected 1 face for full torus";
}

// Test partial torus with only U angle (slice around Z axis)
TEST(BRepPrimAPI_MakeTorusTest, PartialTorusAngleOnly)
{
  const double R1    = 10.0;
  const double R2    = 2.0;
  const double angle = M_PI; // Half torus around Z axis

  BRepPrimAPI_MakeTorus aMakeTorus(R1, R2, angle);
  TopoDS_Shape          aShape = aMakeTorus.Shape();

  ASSERT_TRUE(aMakeTorus.IsDone()) << "Partial torus (angle only) creation failed";
  ASSERT_FALSE(aShape.IsNull()) << "Resulting shape is null";

  BRepCheck_Analyzer anAnalyzer(aShape);
  EXPECT_TRUE(anAnalyzer.IsValid()) << "Partial torus (angle only) shape is not valid";

  // Should have: 1 lateral + 2 planar (start/end sides)
  int aFaceCount = 0;
  for (TopExp_Explorer anExp(aShape, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    aFaceCount++;
  }
  EXPECT_EQ(aFaceCount, 3) << "Expected 3 faces for partial torus with angle only";
}

// Test that the lateral face surface has correct parameterization
TEST(BRepPrimAPI_MakeTorusTest, LateralFaceParameterization)
{
  const double R1 = 5.0;
  const double R2 = 1.0;

  BRepPrimAPI_MakeTorus aMakeTorus(R1, R2);
  TopoDS_Shape          aShape = aMakeTorus.Shape();
  ASSERT_TRUE(aMakeTorus.IsDone());

  TopExp_Explorer anExp(aShape, TopAbs_FACE);
  ASSERT_TRUE(anExp.More()) << "No faces found";

  const TopoDS_Face&           aFace      = TopoDS::Face(anExp.Current());
  Handle(Geom_Surface)         aSurface   = BRep_Tool::Surface(aFace);
  Handle(Geom_ToroidalSurface) aTorusSurf = Handle(Geom_ToroidalSurface)::DownCast(aSurface);

  ASSERT_FALSE(aTorusSurf.IsNull()) << "Lateral face is not a toroidal surface";

  EXPECT_NEAR(aTorusSurf->MajorRadius(), R1, 1e-10);
  EXPECT_NEAR(aTorusSurf->MinorRadius(), R2, 1e-10);

  // At V = PI/2 (top of tube), Z should be +R2
  gp_Pnt aPnt;
  aTorusSurf->D0(0.0, M_PI_2, aPnt);
  EXPECT_NEAR(aPnt.Z(), R2, 1e-10) << "At V=PI/2, Z should be +MinorRadius";

  // At V = -PI/2 (bottom of tube), Z should be -R2
  aTorusSurf->D0(0.0, -M_PI_2, aPnt);
  EXPECT_NEAR(aPnt.Z(), -R2, 1e-10) << "At V=-PI/2, Z should be -MinorRadius";
}

// =============================================================================
// Tests for partial torus with V parameter range (angle1, angle2)
// These test various angle ranges to verify which configurations produce valid shapes.
// =============================================================================

// Working case: V range symmetric around 0 (-PI/2 to +PI/2)
// Bug 33133 original report - this case actually works
TEST(BRepPrimAPI_MakeTorusTest, PartialTorus_SymmetricAroundZero)
{
  const double R1     = 1.0;
  const double R2     = 0.1;
  const double angle1 = -M_PI_2; // -90 degrees
  const double angle2 = M_PI_2;  // +90 degrees
  const double angle  = 2.094;   // ~120 degrees

  gp_Ax2                anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  BRepPrimAPI_MakeTorus aMakeTorus(anAxis, R1, R2, angle1, angle2, angle);
  TopoDS_Shape          aShape = aMakeTorus.Shape();

  ASSERT_TRUE(aMakeTorus.IsDone()) << "Torus creation failed";
  ASSERT_FALSE(aShape.IsNull()) << "Resulting shape is null";

  BRepCheck_Analyzer anAnalyzer(aShape);
  EXPECT_TRUE(anAnalyzer.IsValid()) << "Symmetric partial torus should be valid";

  // Count faces: 1 lateral + 2 top/bottom + 2 start/end = 5
  int aFaceCount = 0;
  for (TopExp_Explorer anExp(aShape, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    aFaceCount++;
  }
  EXPECT_EQ(aFaceCount, 5) << "Expected 5 faces for partial torus with sides";
}

// Working case: Smaller symmetric range (-45 to +45 degrees)
TEST(BRepPrimAPI_MakeTorusTest, PartialTorus_SmallSymmetricRange)
{
  const double angle1 = -M_PI / 4.0; // -45 degrees
  const double angle2 = M_PI / 4.0;  // +45 degrees

  bool isValid = createAndCheckPartialTorus(10.0, 2.0, angle1, angle2, M_PI / 4.0);
  EXPECT_TRUE(isValid) << "Small symmetric range (-45 to +45) should produce valid shape";
}

// Bug 23612: V range from 90 to 270 degrees (positive values, not crossing 0)
// This test verifies the fix for inverted height ordering in BRepPrim_OneAxis
TEST(BRepPrimAPI_MakeTorusTest, PartialTorus_Bug23612)
{
  const double R1     = 10.0;
  const double R2     = 2.0;
  const double angle1 = M_PI_2;       // 90 degrees
  const double angle2 = 3.0 * M_PI_2; // 270 degrees
  const double angle  = M_PI / 4.0;   // 45 degrees

  gp_Ax2                anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  BRepPrimAPI_MakeTorus aMakeTorus(anAxis, R1, R2, angle1, angle2, angle);
  TopoDS_Shape          aShape = aMakeTorus.Shape();

  ASSERT_TRUE(aMakeTorus.IsDone()) << "Torus creation failed";
  ASSERT_FALSE(aShape.IsNull()) << "Resulting shape is null";

  BRepCheck_Analyzer anAnalyzer(aShape);
  // Fixed: V range (90, 270) now produces valid shape with correct height ordering handling
  EXPECT_TRUE(anAnalyzer.IsValid())
    << "Bug 23612 fixed: V range (90, 270) should produce valid shape";
}

// V range from 0 to 180 degrees (top half of minor circle)
// This is an "equal heights" case: sin(0) = sin(180) = 0
// Known limitation: Equal heights cause degenerate AxisEdge topology,
// resulting in UnorientableShape. This is separate from Bug 23612.
TEST(BRepPrimAPI_MakeTorusTest, PartialTorus_ZeroTo180)
{
  const double angle1 = 0.0;
  const double angle2 = M_PI; // 180 degrees

  bool isValid = createAndCheckPartialTorus(10.0, 2.0, angle1, angle2, M_PI / 4.0);
  // Known limitation: equal heights at endpoints cause topology issues
  // TODO: This could be fixed by using a different topology when heights are equal
  EXPECT_FALSE(isValid) << "Known limitation: equal heights (sin(0)=sin(180)=0) not supported";
}

// V range from -180 to 0 degrees (bottom half of minor circle)
// This is an "equal heights" case: sin(-180) = sin(0) = 0
// Known limitation: see PartialTorus_ZeroTo180
TEST(BRepPrimAPI_MakeTorusTest, PartialTorus_Minus180ToZero)
{
  const double angle1 = -M_PI; // -180 degrees
  const double angle2 = 0.0;

  bool isValid = createAndCheckPartialTorus(10.0, 2.0, angle1, angle2, M_PI / 4.0);
  // Known limitation: equal heights at endpoints cause topology issues
  EXPECT_FALSE(isValid) << "Known limitation: equal heights (sin(-180)=sin(0)=0) not supported";
}

// V range from 180 to 360 degrees (bottom half of minor circle, alternate params)
// This is an "equal heights" case: sin(180) = sin(360) = 0
// Known limitation: see PartialTorus_ZeroTo180
TEST(BRepPrimAPI_MakeTorusTest, PartialTorus_180To360)
{
  const double angle1 = M_PI;     // 180 degrees
  const double angle2 = 2 * M_PI; // 360 degrees

  bool isValid = createAndCheckPartialTorus(10.0, 2.0, angle1, angle2, M_PI / 4.0);
  // Known limitation: equal heights at endpoints cause topology issues
  EXPECT_FALSE(isValid) << "Known limitation: equal heights (sin(180)=sin(360)=0) not supported";
}

// V range with both positive values not crossing 0 (+45 to +135 degrees)
// This is an "equal heights" case: sin(45) = sin(135) ≈ 0.707
// Known limitation: see PartialTorus_ZeroTo180
TEST(BRepPrimAPI_MakeTorusTest, PartialTorus_PositiveRange)
{
  const double angle1 = M_PI / 4.0;       // 45 degrees
  const double angle2 = 3.0 * M_PI / 4.0; // 135 degrees

  bool isValid = createAndCheckPartialTorus(10.0, 2.0, angle1, angle2, M_PI / 4.0);
  // Known limitation: equal heights at endpoints cause topology issues
  EXPECT_FALSE(isValid) << "Known limitation: equal heights (sin(45)=sin(135)) not supported";
}

// V range with both negative values not crossing 0 (-135 to -45 degrees)
// This is an "equal heights" case: sin(-135) = sin(-45) ≈ -0.707
// Known limitation: see PartialTorus_ZeroTo180
TEST(BRepPrimAPI_MakeTorusTest, PartialTorus_NegativeRange)
{
  const double angle1 = -3.0 * M_PI / 4.0; // -135 degrees
  const double angle2 = -M_PI / 4.0;       // -45 degrees

  bool isValid = createAndCheckPartialTorus(10.0, 2.0, angle1, angle2, M_PI / 4.0);
  // Known limitation: equal heights at endpoints cause topology issues
  EXPECT_FALSE(isValid) << "Known limitation: equal heights (sin(-135)=sin(-45)) not supported";
}

// V range asymmetric but crossing 0 (-30 to +60 degrees)
// This works because the range stays within (-PI/2, PI/2)
TEST(BRepPrimAPI_MakeTorusTest, PartialTorus_AsymmetricCrossingZero)
{
  const double angle1 = -M_PI / 6.0; // -30 degrees
  const double angle2 = M_PI / 3.0;  // +60 degrees

  bool isValid = createAndCheckPartialTorus(10.0, 2.0, angle1, angle2, M_PI / 4.0);
  EXPECT_TRUE(isValid) << "Asymmetric range crossing 0 but within (-PI/2, PI/2) should be valid";
}

// Small positive range (+30 to +60 degrees)
// This works because the range stays within (-PI/2, PI/2)
TEST(BRepPrimAPI_MakeTorusTest, PartialTorus_SmallPositiveRange)
{
  const double angle1 = M_PI / 6.0; // 30 degrees
  const double angle2 = M_PI / 3.0; // 60 degrees

  bool isValid = createAndCheckPartialTorus(10.0, 2.0, angle1, angle2, M_PI / 4.0);
  EXPECT_TRUE(isValid) << "Small positive range within (-PI/2, PI/2) should be valid";
}

// =============================================================================
// Root cause analysis test
// =============================================================================

//! This test documents the root cause of bugs 23612/33133:
//! BRepPrim_OneAxis assumes MeridianValue(VMax).Y() > MeridianValue(VMin).Y(),
//! i.e., that the "top" (max V parameter) is geometrically above the "bottom".
//! For circular meridians (torus), this is only true for certain V ranges.
//!
//! The meridian circle parameterization for torus:
//!   X(V) = Major + Minor * cos(V)  [distance from axis]
//!   Y(V) = Minor * sin(V)          [height/Z coordinate]
//!
//! Working ranges: Y increases as V increases (e.g., -90° to +90°)
//! Failing ranges: Y decreases as V increases (e.g., 90° to 270°)
//!                 or Y is equal at VMin and VMax (e.g., 45° to 135°)
TEST(BRepPrimAPI_MakeTorusTest, RootCause_HeightOrderingAssumption)
{
  const double R1 = 10.0;
  const double R2 = 2.0;

  // For a torus meridian: Y(V) = R2 * sin(V)
  auto computeHeight = [R2](double angleRad) { return R2 * std::sin(angleRad); };

  // Case 1: Working range (-90° to +90°)
  // Y(-90°) = -2, Y(+90°) = +2 => Y(VMax) > Y(VMin) ✓
  {
    const double vMin = -M_PI_2;
    const double vMax = M_PI_2;
    double       yMin = computeHeight(vMin);
    double       yMax = computeHeight(vMax);
    EXPECT_GT(yMax, yMin) << "Working case: Y(VMax) should be > Y(VMin)";

    bool isValid = createAndCheckPartialTorus(R1, R2, vMin, vMax, M_PI / 4.0);
    EXPECT_TRUE(isValid) << "Range with correct height ordering should produce valid shape";
  }

  // Case 2: Previously failing range (90° to 270°) - Bug 23612 (FIXED)
  // Y(90°) = +2, Y(270°) = -2 => Y(VMax) < Y(VMin) - height ordering is inverted
  // Fix: BRepPrim_OneAxis now uses VTopGeometric/VBottomGeometric and adjusts
  // edge vertex 'first' flags and wire 'reversed' flags accordingly
  {
    const double vMin = M_PI_2;       // 90°
    const double vMax = 3.0 * M_PI_2; // 270°
    double       yMin = computeHeight(vMin);
    double       yMax = computeHeight(vMax);
    EXPECT_LT(yMax, yMin) << "Height ordering is inverted: Y(VMax) < Y(VMin)";

    bool isValid = createAndCheckPartialTorus(R1, R2, vMin, vMax, M_PI / 4.0);
    // Fixed: BRepPrim_OneAxis now correctly handles inverted height ordering
    EXPECT_TRUE(isValid) << "Bug 23612 fixed: inverted height ordering now handled correctly";
  }

  // Case 3: Equal heights (45° to 135°) - Known Limitation
  // Y(45°) ≈ 1.41, Y(135°) ≈ 1.41 => Heights are equal at endpoints
  // The range covers distinct geometric regions (arc from 45° to 135°), but the
  // resulting topology has a degenerate AxisEdge (zero length) which cannot form
  // an orientable shell with the current face/wire structure.
  {
    const double vMin = M_PI / 4.0;       // 45°
    const double vMax = 3.0 * M_PI / 4.0; // 135°
    double       yMin = computeHeight(vMin);
    double       yMax = computeHeight(vMax);
    EXPECT_NEAR(yMax, yMin, 1e-10) << "Heights at VMin and VMax are equal";

    bool isValid = createAndCheckPartialTorus(R1, R2, vMin, vMax, M_PI / 4.0);
    // Known limitation: equal heights cause degenerate AxisEdge topology
    // This is a separate issue from Bug 23612 (inverted heights)
    EXPECT_FALSE(isValid) << "Known limitation: equal heights not yet supported";
  }
}

// Diagnostic test to understand what BRepCheck_Analyzer is reporting
TEST(BRepPrimAPI_MakeTorusTest, DiagnosticTest_Bug23612)
{
  const double R1     = 10.0;
  const double R2     = 2.0;
  const double angle1 = M_PI_2;       // 90 degrees
  const double angle2 = 3.0 * M_PI_2; // 270 degrees
  const double angle  = M_PI / 4.0;   // 45 degrees

  gp_Ax2                anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  BRepPrimAPI_MakeTorus aMakeTorus(anAxis, R1, R2, angle1, angle2, angle);
  TopoDS_Shape          aShape = aMakeTorus.Shape();

  ASSERT_TRUE(aMakeTorus.IsDone()) << "Torus creation failed";
  ASSERT_FALSE(aShape.IsNull()) << "Resulting shape is null";

  BRepCheck_Analyzer anAnalyzer(aShape);
  if (!anAnalyzer.IsValid())
  {
    std::cout << "\nDiagnostic for Bug23612 (V range 90-270):" << std::endl;

    // Check edges for issues
    int edgeNum = 0;
    for (TopExp_Explorer anExp(aShape, TopAbs_EDGE); anExp.More(); anExp.Next())
    {
      const TopoDS_Edge&       anEdge  = TopoDS::Edge(anExp.Current());
      Handle(BRepCheck_Result) aResult = anAnalyzer.Result(anEdge);
      if (!aResult.IsNull())
      {
        const BRepCheck_ListOfStatus& aStatusList = aResult->Status();
        for (BRepCheck_ListOfStatus::Iterator it(aStatusList); it.More(); it.Next())
        {
          if (it.Value() != BRepCheck_NoError)
          {
            std::cout << "  Edge " << edgeNum << ": Status = " << static_cast<int>(it.Value())
                      << std::endl;
          }
        }
      }
      edgeNum++;
    }

    // Check vertices for issues
    int vertNum = 0;
    for (TopExp_Explorer anExp(aShape, TopAbs_VERTEX); anExp.More(); anExp.Next())
    {
      const TopoDS_Vertex&     aVertex = TopoDS::Vertex(anExp.Current());
      Handle(BRepCheck_Result) aResult = anAnalyzer.Result(aVertex);
      if (!aResult.IsNull())
      {
        const BRepCheck_ListOfStatus& aStatusList = aResult->Status();
        for (BRepCheck_ListOfStatus::Iterator it(aStatusList); it.More(); it.Next())
        {
          if (it.Value() != BRepCheck_NoError)
          {
            gp_Pnt p = BRep_Tool::Pnt(aVertex);
            std::cout << "  Vertex " << vertNum << " at (" << p.X() << ", " << p.Y() << ", "
                      << p.Z() << "): Status = " << static_cast<int>(it.Value()) << std::endl;
          }
        }
      }
      vertNum++;
    }

    // Check faces for issues
    int faceNum = 0;
    for (TopExp_Explorer anExp(aShape, TopAbs_FACE); anExp.More(); anExp.Next())
    {
      const TopoDS_Face&       aFace   = TopoDS::Face(anExp.Current());
      Handle(BRepCheck_Result) aResult = anAnalyzer.Result(aFace);
      if (!aResult.IsNull())
      {
        const BRepCheck_ListOfStatus& aStatusList = aResult->Status();
        for (BRepCheck_ListOfStatus::Iterator it(aStatusList); it.More(); it.Next())
        {
          if (it.Value() != BRepCheck_NoError)
          {
            std::cout << "  Face " << faceNum << ": Status = " << static_cast<int>(it.Value())
                      << std::endl;
          }
        }
      }
      faceNum++;
    }
  }
  // This test just outputs diagnostic info, actual assertion is in other tests
}

// Diagnostic test for equal heights case (0, 180)
TEST(BRepPrimAPI_MakeTorusTest, DiagnosticTest_EqualHeights)
{
  const double R1     = 10.0;
  const double R2     = 2.0;
  const double angle1 = 0;          // 0 degrees
  const double angle2 = M_PI;       // 180 degrees
  const double angle  = M_PI / 4.0; // 45 degrees

  gp_Ax2                anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  BRepPrimAPI_MakeTorus aMakeTorus(anAxis, R1, R2, angle1, angle2, angle);
  TopoDS_Shape          aShape = aMakeTorus.Shape();

  ASSERT_TRUE(aMakeTorus.IsDone()) << "Torus creation failed";
  ASSERT_FALSE(aShape.IsNull()) << "Resulting shape is null";

  // Print heights
  std::cout << "\nDiagnostic for Equal Heights (V range 0-180):" << std::endl;
  std::cout << "  Height at VMin (0): " << R2 * std::sin(angle1) << std::endl;
  std::cout << "  Height at VMax (180): " << R2 * std::sin(angle2) << std::endl;

  // Count and print unique vertices
  std::cout << "  Unique vertices:" << std::endl;
  NCollection_Map<TopoDS_Vertex, TopTools_ShapeMapHasher> uniqueVerts;
  for (TopExp_Explorer anExp(aShape, TopAbs_VERTEX); anExp.More(); anExp.Next())
  {
    const TopoDS_Vertex& aVert = TopoDS::Vertex(anExp.Current());
    if (uniqueVerts.Add(aVert))
    {
      gp_Pnt p = BRep_Tool::Pnt(aVert);
      std::cout << "    V" << uniqueVerts.Size() - 1 << ": (" << p.X() << ", " << p.Y() << ", "
                << p.Z() << ")" << std::endl;
    }
  }
  std::cout << "  Total unique vertices: " << uniqueVerts.Size() << std::endl;

  // Print wire info for each face
  int faceNum = 0;
  for (TopExp_Explorer faceExp(aShape, TopAbs_FACE); faceExp.More(); faceExp.Next())
  {
    const TopoDS_Face& aFace = TopoDS::Face(faceExp.Current());
    std::cout << "  Face " << faceNum << " wires:" << std::endl;
    for (TopExp_Explorer wireExp(aFace, TopAbs_WIRE); wireExp.More(); wireExp.Next())
    {
      std::cout << "    Wire edges:" << std::endl;
      for (TopExp_Explorer edgeExp(wireExp.Current(), TopAbs_EDGE); edgeExp.More(); edgeExp.Next())
      {
        const TopoDS_Edge& anEdge = TopoDS::Edge(edgeExp.Current());
        TopoDS_Vertex      v1, v2;
        TopExp::Vertices(anEdge, v1, v2);
        gp_Pnt p1 = BRep_Tool::Pnt(v1);
        gp_Pnt p2 = BRep_Tool::Pnt(v2);
        std::cout << "      (" << p1.X() << "," << p1.Y() << "," << p1.Z() << ") -> (" << p2.X()
                  << "," << p2.Y() << "," << p2.Z() << ")" << std::endl;
      }
    }
    faceNum++;
  }

  BRepCheck_Analyzer anAnalyzer(aShape);
  if (!anAnalyzer.IsValid())
  {
    std::cout << "Shape is INVALID" << std::endl;

    // Check edges for issues
    int edgeNum = 0;
    for (TopExp_Explorer anExp(aShape, TopAbs_EDGE); anExp.More(); anExp.Next())
    {
      const TopoDS_Edge&       anEdge  = TopoDS::Edge(anExp.Current());
      Handle(BRepCheck_Result) aResult = anAnalyzer.Result(anEdge);
      if (!aResult.IsNull())
      {
        const BRepCheck_ListOfStatus& aStatusList = aResult->Status();
        for (BRepCheck_ListOfStatus::Iterator it(aStatusList); it.More(); it.Next())
        {
          if (it.Value() != BRepCheck_NoError)
          {
            double             f, l;
            Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, f, l);
            std::cout << "  Edge " << edgeNum << " (range " << f << " to " << l
                      << "): Status = " << static_cast<int>(it.Value()) << std::endl;
          }
        }
      }
      edgeNum++;
    }

    // Check faces for issues
    faceNum = 0;
    for (TopExp_Explorer anExp(aShape, TopAbs_FACE); anExp.More(); anExp.Next())
    {
      const TopoDS_Face&       aFace   = TopoDS::Face(anExp.Current());
      Handle(BRepCheck_Result) aResult = anAnalyzer.Result(aFace);
      if (!aResult.IsNull())
      {
        const BRepCheck_ListOfStatus& aStatusList = aResult->Status();
        for (BRepCheck_ListOfStatus::Iterator it(aStatusList); it.More(); it.Next())
        {
          if (it.Value() != BRepCheck_NoError)
          {
            std::cout << "  Face " << faceNum << ": Status = " << static_cast<int>(it.Value())
                      << std::endl;
          }
        }
      }
      faceNum++;
    }
  }
  else
  {
    std::cout << "Shape is VALID" << std::endl;
  }
}
