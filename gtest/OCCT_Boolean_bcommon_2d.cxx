#include <gtest/gtest.h>

// OCCT Core
#include <OSD_Environment.hxx>
#include <Precision.hxx>
#include <TCollection_AsciiString.hxx>

// Topology
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <BRep_Tool.hxx>

// Boolean operations
#include <BRepAlgoAPI_Common.hxx>

// GProp for properties
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>

#include "OCCT_TestUtils.hxx"

/**
 * @brief Helper to check properties (Length).
 */
static void CheckLength(const TopoDS_Shape& theShape, Standard_Real theExpectedLength, Standard_Real theTol = 1e-3)
{
    GProp_GProps aProps;
    BRepGProp::LinearProperties(theShape, aProps);
    Standard_Real aLen = aProps.Mass();
    if (theExpectedLength < 0) {
        ASSERT_LT(aLen, Precision::Confusion()) << "Expected empty length, but got " << aLen;
    } else {
        ASSERT_NEAR(aLen, theExpectedLength, theTol);
    }
}

/**
 * @brief Helper to count shapes.
 */
static void CheckNbShapes(const TopoDS_Shape& theShape,
                          TopAbs_ShapeEnum    theType,
                          Standard_Integer    theExpectedCount)
{
  TopTools_IndexedMapOfShape aMap;
  // 使用 MapShapes 会自动去重，只统计唯一的 TShape
  TopExp::MapShapes(theShape, theType, aMap);

  Standard_Integer aCount = aMap.Extent();
  ASSERT_EQ(aCount, theExpectedCount) << "Shape count mismatch for type " << theType;
}

// Suite: OCCT_Boolean_bcommon_2d

TEST(OCCT_Boolean_bcommon_2d, A8)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_1_shell.brep", a)) << "Missing case_1_shell.brep";
    ASSERT_TRUE(LoadTestData("case_1_edge2.brep", b)) << "Missing case_1_edge2.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, -1); // empty
    CheckNbShapes(result, TopAbs_VERTEX, 0);
    CheckNbShapes(result, TopAbs_EDGE, 0);
}

TEST(OCCT_Boolean_bcommon_2d, A9)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_1_shell.brep", a)) << "Missing case_1_shell.brep";
    ASSERT_TRUE(LoadTestData("case_1_edge4.brep", b)) << "Missing case_1_edge4.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 471.239);
    CheckNbShapes(result, TopAbs_VERTEX, 2);
    CheckNbShapes(result, TopAbs_EDGE, 1);
}

TEST(OCCT_Boolean_bcommon_2d, B1)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_1_shell.brep", a)) << "Missing case_1_shell.brep";
    ASSERT_TRUE(LoadTestData("case_1_wire3.brep", b)) << "Missing case_1_wire3.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 942.478);
    CheckNbShapes(result, TopAbs_VERTEX, 2);
    CheckNbShapes(result, TopAbs_EDGE, 2);
}

TEST(OCCT_Boolean_bcommon_2d, B2)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_2_solid_r.brep", a)) << "Missing case_2_solid_r.brep";
    ASSERT_TRUE(LoadTestData("case_2_edge1.brep", b)) << "Missing case_2_edge1.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 847.007);
    CheckNbShapes(result, TopAbs_VERTEX, 14);
    CheckNbShapes(result, TopAbs_EDGE, 7);
}

TEST(OCCT_Boolean_bcommon_2d, B3)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_2_solid_r.brep", a)) << "Missing case_2_solid_r.brep";
    ASSERT_TRUE(LoadTestData("case_2_edge2.brep", b)) << "Missing case_2_edge2.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 847.007);
    CheckNbShapes(result, TopAbs_VERTEX, 14);
    CheckNbShapes(result, TopAbs_EDGE, 7);
}

TEST(OCCT_Boolean_bcommon_2d, B4)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_2_solid_r.brep", a)) << "Missing case_2_solid_r.brep";
    ASSERT_TRUE(LoadTestData("case_2_edge3.brep", b)) << "Missing case_2_edge3.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 753.315);
    CheckNbShapes(result, TopAbs_VERTEX, 6);
    CheckNbShapes(result, TopAbs_EDGE, 3);
}

TEST(OCCT_Boolean_bcommon_2d, B5)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_2_solid_r.brep", a)) << "Missing case_2_solid_r.brep";
    ASSERT_TRUE(LoadTestData("case_2_edge4.brep", b)) << "Missing case_2_edge4.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 753.315);
    CheckNbShapes(result, TopAbs_VERTEX, 6);
    CheckNbShapes(result, TopAbs_EDGE, 3);
}

TEST(OCCT_Boolean_bcommon_2d, B6)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_2_solid_r.brep", a)) << "Missing case_2_solid_r.brep";
    ASSERT_TRUE(LoadTestData("case_2_wire1.brep", b)) << "Missing case_2_wire1.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 874.0);
    CheckNbShapes(result, TopAbs_VERTEX, 18);
    CheckNbShapes(result, TopAbs_EDGE, 11);
}

TEST(OCCT_Boolean_bcommon_2d, B7)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_2_solid_r.brep", a)) << "Missing case_2_solid_r.brep";
    ASSERT_TRUE(LoadTestData("case_2_wire2.brep", b)) << "Missing case_2_wire2.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 874.0);
    CheckNbShapes(result, TopAbs_VERTEX, 18);
    CheckNbShapes(result, TopAbs_EDGE, 11);
}

TEST(OCCT_Boolean_bcommon_2d, B8)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_2_solid_r.brep", a)) << "Missing case_2_solid_r.brep";
    ASSERT_TRUE(LoadTestData("case_2_wire3.brep", b)) << "Missing case_2_wire3.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 846.821);
    CheckNbShapes(result, TopAbs_VERTEX, 9);
    CheckNbShapes(result, TopAbs_EDGE, 6);
}

TEST(OCCT_Boolean_bcommon_2d, B9)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_2_solid_r.brep", a)) << "Missing case_2_solid_r.brep";
    ASSERT_TRUE(LoadTestData("case_2_wire4.brep", b)) << "Missing case_2_wire4.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 768.597);
    CheckNbShapes(result, TopAbs_VERTEX, 9);
    CheckNbShapes(result, TopAbs_EDGE, 6);
}

TEST(OCCT_Boolean_bcommon_2d, C1)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_2_shell_r.brep", a)) << "Missing case_2_shell_r.brep";
    ASSERT_TRUE(LoadTestData("case_2_edge1.brep", b)) << "Missing case_2_edge1.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 847.007);
    CheckNbShapes(result, TopAbs_VERTEX, 14);
    CheckNbShapes(result, TopAbs_EDGE, 7);
}

TEST(OCCT_Boolean_bcommon_2d, C2)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_2_shell_r.brep", a)) << "Missing case_2_shell_r.brep";
    ASSERT_TRUE(LoadTestData("case_2_edge2.brep", b)) << "Missing case_2_edge2.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 847.007);
    CheckNbShapes(result, TopAbs_VERTEX, 14);
    CheckNbShapes(result, TopAbs_EDGE, 7);
}

TEST(OCCT_Boolean_bcommon_2d, C3)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_2_shell_r.brep", a)) << "Missing case_2_shell_r.brep";
    ASSERT_TRUE(LoadTestData("case_2_edge3.brep", b)) << "Missing case_2_edge3.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 753.315);
    CheckNbShapes(result, TopAbs_VERTEX, 6);
    CheckNbShapes(result, TopAbs_EDGE, 3);
}

TEST(OCCT_Boolean_bcommon_2d, C4)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_2_shell_r.brep", a)) << "Missing case_2_shell_r.brep";
    ASSERT_TRUE(LoadTestData("case_2_wire1.brep", b)) << "Missing case_2_wire1.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 874.0);
    CheckNbShapes(result, TopAbs_VERTEX, 18);
    CheckNbShapes(result, TopAbs_EDGE, 11);
}

TEST(OCCT_Boolean_bcommon_2d, C5)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_2_shell_r.brep", a)) << "Missing case_2_shell_r.brep";
    ASSERT_TRUE(LoadTestData("case_2_wire2.brep", b)) << "Missing case_2_wire2.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, -1); // empty
    CheckNbShapes(result, TopAbs_VERTEX, 0);
    CheckNbShapes(result, TopAbs_EDGE, 0);
}

TEST(OCCT_Boolean_bcommon_2d, C6)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_2_shell_r.brep", a)) << "Missing case_2_shell_r.brep";
    ASSERT_TRUE(LoadTestData("case_2_wire3.brep", b)) << "Missing case_2_wire3.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 846.821);
    CheckNbShapes(result, TopAbs_VERTEX, 9);
    CheckNbShapes(result, TopAbs_EDGE, 6);
}

TEST(OCCT_Boolean_bcommon_2d, C7)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_2_face_r.brep", a)) << "Missing case_2_face_r.brep";
    ASSERT_TRUE(LoadTestData("case_2_edge1.brep", b)) << "Missing case_2_edge1.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 847.007);
    CheckNbShapes(result, TopAbs_VERTEX, 14);
    CheckNbShapes(result, TopAbs_EDGE, 7);
}

TEST(OCCT_Boolean_bcommon_2d, C8)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_2_face_r.brep", a)) << "Missing case_2_face_r.brep";
    ASSERT_TRUE(LoadTestData("case_2_edge3.brep", b)) << "Missing case_2_edge3.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 753.315);
    CheckNbShapes(result, TopAbs_VERTEX, 6);
    CheckNbShapes(result, TopAbs_EDGE, 3);
}

TEST(OCCT_Boolean_bcommon_2d, C9)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_2_face_r.brep", a)) << "Missing case_2_face_r.brep";
    ASSERT_TRUE(LoadTestData("case_2_wire1.brep", b)) << "Missing case_2_wire1.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 874.0);
    CheckNbShapes(result, TopAbs_VERTEX, 18);
    CheckNbShapes(result, TopAbs_EDGE, 11);
}

TEST(OCCT_Boolean_bcommon_2d, D1)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_2_face_r.brep", a)) << "Missing case_2_face_r.brep";
    ASSERT_TRUE(LoadTestData("case_2_wire3.brep", b)) << "Missing case_2_wire3.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 846.821);
    CheckNbShapes(result, TopAbs_VERTEX, 9);
    CheckNbShapes(result, TopAbs_EDGE, 6);
}

TEST(OCCT_Boolean_bcommon_2d, D2)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_3_solid.brep", a)) << "Missing case_3_solid.brep";
    ASSERT_TRUE(LoadTestData("case_3_edge1.brep", b)) << "Missing case_3_edge1.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 345.975);
    CheckNbShapes(result, TopAbs_VERTEX, 8);
    CheckNbShapes(result, TopAbs_EDGE, 7);
}

TEST(OCCT_Boolean_bcommon_2d, D3)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_3_solid.brep", a)) << "Missing case_3_solid.brep";
    ASSERT_TRUE(LoadTestData("case_3_edge2.brep", b)) << "Missing case_3_edge2.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 345.975);
    CheckNbShapes(result, TopAbs_VERTEX, 8);
    CheckNbShapes(result, TopAbs_EDGE, 7);
}

TEST(OCCT_Boolean_bcommon_2d, D4)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_3_solid.brep", a)) << "Missing case_3_solid.brep";
    ASSERT_TRUE(LoadTestData("offset_wire_034.brep", b)) << "Missing offset_wire_034.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 345.975);
    CheckNbShapes(result, TopAbs_VERTEX, 8);
    CheckNbShapes(result, TopAbs_EDGE, 7);
}

TEST(OCCT_Boolean_bcommon_2d, D5)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_3_solid.brep", a)) << "Missing case_3_solid.brep";
    ASSERT_TRUE(LoadTestData("case_3_wire2.brep", b)) << "Missing case_3_wire2.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 345.975);
    CheckNbShapes(result, TopAbs_VERTEX, 14);
    CheckNbShapes(result, TopAbs_EDGE, 13);
}

TEST(OCCT_Boolean_bcommon_2d, D6)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_3_solid.brep", a)) << "Missing case_3_solid.brep";
    ASSERT_TRUE(LoadTestData("case_3_wire3.brep", b)) << "Missing case_3_wire3.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 345.975);
    CheckNbShapes(result, TopAbs_VERTEX, 8);
    CheckNbShapes(result, TopAbs_EDGE, 7);
}

TEST(OCCT_Boolean_bcommon_2d, D7)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_3_solid.brep", a)) << "Missing case_3_solid.brep";
    ASSERT_TRUE(LoadTestData("case_3_wire4.brep", b)) << "Missing case_3_wire4.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 345.975);
    CheckNbShapes(result, TopAbs_VERTEX, 8);
    CheckNbShapes(result, TopAbs_EDGE, 7);
}

TEST(OCCT_Boolean_bcommon_2d, D8)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_3_solid.brep", a)) << "Missing case_3_solid.brep";
    ASSERT_TRUE(LoadTestData("case_3_wire5.brep", b)) << "Missing case_3_wire5.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 292.732);
    CheckNbShapes(result, TopAbs_VERTEX, 4);
    CheckNbShapes(result, TopAbs_EDGE, 4);
}

TEST(OCCT_Boolean_bcommon_2d, D9)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_3_shell.brep", a)) << "Missing case_3_shell.brep";
    ASSERT_TRUE(LoadTestData("case_3_edge1.brep", b)) << "Missing case_3_edge1.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 345.975);
    CheckNbShapes(result, TopAbs_VERTEX, 8);
    CheckNbShapes(result, TopAbs_EDGE, 7);
}

TEST(OCCT_Boolean_bcommon_2d, E1)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_3_shell.brep", a)) << "Missing case_3_shell.brep";
    ASSERT_TRUE(LoadTestData("case_3_edge2.brep", b)) << "Missing case_3_edge2.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 345.975);
    CheckNbShapes(result, TopAbs_VERTEX, 8);
    CheckNbShapes(result, TopAbs_EDGE, 7);
}

TEST(OCCT_Boolean_bcommon_2d, E2)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_3_shell.brep", a)) << "Missing case_3_shell.brep";
    ASSERT_TRUE(LoadTestData("offset_wire_034.brep", b)) << "Missing offset_wire_034.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 345.975);
    CheckNbShapes(result, TopAbs_VERTEX, 8);
    CheckNbShapes(result, TopAbs_EDGE, 7);
}

TEST(OCCT_Boolean_bcommon_2d, E3)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_3_shell.brep", a)) << "Missing case_3_shell.brep";
    ASSERT_TRUE(LoadTestData("case_3_wire2.brep", b)) << "Missing case_3_wire2.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 345.975);
    CheckNbShapes(result, TopAbs_VERTEX, 14);
    CheckNbShapes(result, TopAbs_EDGE, 13);
}

TEST(OCCT_Boolean_bcommon_2d, E4)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_3_shell.brep", a)) << "Missing case_3_shell.brep";
    ASSERT_TRUE(LoadTestData("case_3_wire3.brep", b)) << "Missing case_3_wire3.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 345.975);
    CheckNbShapes(result, TopAbs_VERTEX, 8);
    CheckNbShapes(result, TopAbs_EDGE, 7);
}

TEST(OCCT_Boolean_bcommon_2d, E5)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_3_shell.brep", a)) << "Missing case_3_shell.brep";
    ASSERT_TRUE(LoadTestData("case_3_wire4.brep", b)) << "Missing case_3_wire4.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 345.975);
    CheckNbShapes(result, TopAbs_VERTEX, 8);
    CheckNbShapes(result, TopAbs_EDGE, 7);
}

TEST(OCCT_Boolean_bcommon_2d, E6)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_3_shell.brep", a)) << "Missing case_3_shell.brep";
    ASSERT_TRUE(LoadTestData("case_3_wire5.brep", b)) << "Missing case_3_wire5.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 292.732);
    CheckNbShapes(result, TopAbs_VERTEX, 4);
    CheckNbShapes(result, TopAbs_EDGE, 4);
}

TEST(OCCT_Boolean_bcommon_2d, E7)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_3_face.brep", a)) << "Missing case_3_face.brep";
    ASSERT_TRUE(LoadTestData("case_3_edge1.brep", b)) << "Missing case_3_edge1.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 345.975);
    CheckNbShapes(result, TopAbs_VERTEX, 8);
    CheckNbShapes(result, TopAbs_EDGE, 7);
}

TEST(OCCT_Boolean_bcommon_2d, E8)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_3_face.brep", a)) << "Missing case_3_face.brep";
    ASSERT_TRUE(LoadTestData("offset_wire_034.brep", b)) << "Missing offset_wire_034.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 345.975);
    CheckNbShapes(result, TopAbs_VERTEX, 8);
    CheckNbShapes(result, TopAbs_EDGE, 7);
}

TEST(OCCT_Boolean_bcommon_2d, E9)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_3_face.brep", a)) << "Missing case_3_face.brep";
    ASSERT_TRUE(LoadTestData("case_3_wire2.brep", b)) << "Missing case_3_wire2.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 345.975);
    CheckNbShapes(result, TopAbs_VERTEX, 14);
    CheckNbShapes(result, TopAbs_EDGE, 13);
}

TEST(OCCT_Boolean_bcommon_2d, F1)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_3_face.brep", a)) << "Missing case_3_face.brep";
    ASSERT_TRUE(LoadTestData("case_3_wire5.brep", b)) << "Missing case_3_wire5.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 66.3661);
    CheckNbShapes(result, TopAbs_VERTEX, 2);
    CheckNbShapes(result, TopAbs_EDGE, 1);
}

TEST(OCCT_Boolean_bcommon_2d, F2)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_3_wire.brep", a)) << "Missing case_3_wire.brep";
    ASSERT_TRUE(LoadTestData("case_3_edge1.brep", b)) << "Missing case_3_edge1.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 345.975);
    CheckNbShapes(result, TopAbs_VERTEX, 8);
    CheckNbShapes(result, TopAbs_EDGE, 7);
}

TEST(OCCT_Boolean_bcommon_2d, F3)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_3_wire.brep", a)) << "Missing case_3_wire.brep";
    ASSERT_TRUE(LoadTestData("offset_wire_034.brep", b)) << "Missing offset_wire_034.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 345.975);
    CheckNbShapes(result, TopAbs_VERTEX, 8);
    CheckNbShapes(result, TopAbs_EDGE, 7);
}

TEST(OCCT_Boolean_bcommon_2d, F4)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_3_wire.brep", a)) << "Missing case_3_wire.brep";
    ASSERT_TRUE(LoadTestData("case_3_wire2.brep", b)) << "Missing case_3_wire2.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 345.975);
    CheckNbShapes(result, TopAbs_VERTEX, 14);
    CheckNbShapes(result, TopAbs_EDGE, 13);
}

TEST(OCCT_Boolean_bcommon_2d, F5)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_3_wire.brep", a)) << "Missing case_3_wire.brep";
    ASSERT_TRUE(LoadTestData("case_3_wire5.brep", b)) << "Missing case_3_wire5.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 66.3661);
    CheckNbShapes(result, TopAbs_VERTEX, 2);
    CheckNbShapes(result, TopAbs_EDGE, 1);
}

TEST(OCCT_Boolean_bcommon_2d, F6)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_4_solid.brep", a)) << "Missing case_4_solid.brep";
    ASSERT_TRUE(LoadTestData("case_4_edge1.brep", b)) << "Missing case_4_edge1.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 1231.36, 0.01); 
    CheckNbShapes(result, TopAbs_VERTEX, 2);
    CheckNbShapes(result, TopAbs_EDGE, 1);
}

TEST(OCCT_Boolean_bcommon_2d, F7)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_4_solid.brep", a)) << "Missing case_4_solid.brep";
    ASSERT_TRUE(LoadTestData("case_4_edge2.brep", b)) << "Missing case_4_edge2.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 1231.36, 0.01);
    CheckNbShapes(result, TopAbs_VERTEX, 2);
    CheckNbShapes(result, TopAbs_EDGE, 1);
}

TEST(OCCT_Boolean_bcommon_2d, F8)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_4_solid.brep", a)) << "Missing case_4_solid.brep";
    ASSERT_TRUE(LoadTestData("case_4_edge3.brep", b)) << "Missing case_4_edge3.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 375.748);
    CheckNbShapes(result, TopAbs_VERTEX, 2);
    CheckNbShapes(result, TopAbs_EDGE, 1);
}

TEST(OCCT_Boolean_bcommon_2d, F9)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_4_solid.brep", a)) << "Missing case_4_solid.brep";
    ASSERT_TRUE(LoadTestData("case_4_wire1.brep", b)) << "Missing case_4_wire1.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 2553.95);
    CheckNbShapes(result, TopAbs_VERTEX, 3);
    CheckNbShapes(result, TopAbs_EDGE, 3);
}

TEST(OCCT_Boolean_bcommon_2d, G1)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_4_solid.brep", a)) << "Missing case_4_solid.brep";
    ASSERT_TRUE(LoadTestData("case_4_wire2.brep", b)) << "Missing case_4_wire2.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 2553.95);
    CheckNbShapes(result, TopAbs_VERTEX, 3);
    CheckNbShapes(result, TopAbs_EDGE, 3);
}

TEST(OCCT_Boolean_bcommon_2d, G2)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_4_solid.brep", a)) << "Missing case_4_solid.brep";
    ASSERT_TRUE(LoadTestData("case_4_wire3.brep", b)) << "Missing case_4_wire3.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 633.071);
    CheckNbShapes(result, TopAbs_VERTEX, 9);
    CheckNbShapes(result, TopAbs_EDGE, 7);
}

TEST(OCCT_Boolean_bcommon_2d, G3)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_4_solid.brep", a)) << "Missing case_4_solid.brep";
    ASSERT_TRUE(LoadTestData("case_4_wire4.brep", b)) << "Missing case_4_wire4.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 633.071);
    CheckNbShapes(result, TopAbs_VERTEX, 9);
    CheckNbShapes(result, TopAbs_EDGE, 7);
}

TEST(OCCT_Boolean_bcommon_2d, G9)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_4_face.brep", a)) << "Missing case_4_face.brep";
    ASSERT_TRUE(LoadTestData("case_4_edge3.brep", b)) << "Missing case_4_edge3.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 474.547);
    CheckNbShapes(result, TopAbs_VERTEX, 4);
    CheckNbShapes(result, TopAbs_EDGE, 2);
}

TEST(OCCT_Boolean_bcommon_2d, H1)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_4_face.brep", a)) << "Missing case_4_face.brep";
    ASSERT_TRUE(LoadTestData("case_4_wire1.brep", b)) << "Missing case_4_wire1.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 1245.29, 0.01);
    CheckNbShapes(result, TopAbs_VERTEX, 10);
    CheckNbShapes(result, TopAbs_EDGE, 5);
}

TEST(OCCT_Boolean_bcommon_2d, H2)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_4_face.brep", a)) << "Missing case_4_face.brep";
    ASSERT_TRUE(LoadTestData("case_4_wire3.brep", b)) << "Missing case_4_wire3.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 1150.78, 0.01);
    CheckNbShapes(result, TopAbs_VERTEX, 15);
    CheckNbShapes(result, TopAbs_EDGE, 10);
}

TEST(OCCT_Boolean_bcommon_2d, H3)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_5_solid.brep", a)) << "Missing case_5_solid.brep";
    ASSERT_TRUE(LoadTestData("case_4_edge1.brep", b)) << "Missing case_4_edge1.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 887.185);
    CheckNbShapes(result, TopAbs_VERTEX, 2);
    CheckNbShapes(result, TopAbs_EDGE, 1);
}

TEST(OCCT_Boolean_bcommon_2d, H4)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_5_solid.brep", a)) << "Missing case_5_solid.brep";
    ASSERT_TRUE(LoadTestData("case_5_edge2.brep", b)) << "Missing case_5_edge2.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 887.185);
    CheckNbShapes(result, TopAbs_VERTEX, 2);
    CheckNbShapes(result, TopAbs_EDGE, 1);
}

TEST(OCCT_Boolean_bcommon_2d, H5)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_5_solid.brep", a)) << "Missing case_5_solid.brep";
    ASSERT_TRUE(LoadTestData("case_4_edge3.brep", b)) << "Missing case_4_edge3.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 829.541);
    CheckNbShapes(result, TopAbs_VERTEX, 2);
    CheckNbShapes(result, TopAbs_EDGE, 1);
}

TEST(OCCT_Boolean_bcommon_2d, H6)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_5_solid.brep", a)) << "Missing case_5_solid.brep";
    ASSERT_TRUE(LoadTestData("case_4_wire1.brep", b)) << "Missing case_4_wire1.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 1747.73);
    CheckNbShapes(result, TopAbs_VERTEX, 4);
    CheckNbShapes(result, TopAbs_EDGE, 2);
}

TEST(OCCT_Boolean_bcommon_2d, H7)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_5_solid.brep", a)) << "Missing case_5_solid.brep";
    ASSERT_TRUE(LoadTestData("case_4_wire2.brep", b)) << "Missing case_4_wire2.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 1747.73);
    CheckNbShapes(result, TopAbs_VERTEX, 4);
    CheckNbShapes(result, TopAbs_EDGE, 2);
}

TEST(OCCT_Boolean_bcommon_2d, H8)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_5_solid.brep", a)) << "Missing case_5_solid.brep";
    ASSERT_TRUE(LoadTestData("case_4_wire3.brep", b)) << "Missing case_4_wire3.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 1808.02, 0.01);
    CheckNbShapes(result, TopAbs_VERTEX, 12);
    CheckNbShapes(result, TopAbs_EDGE, 11);
}

TEST(OCCT_Boolean_bcommon_2d, H9)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_5_solid.brep", a)) << "Missing case_5_solid.brep";
    ASSERT_TRUE(LoadTestData("case_5_wire4.brep", b)) << "Missing case_5_wire4.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 1808.02, 0.01);
    CheckNbShapes(result, TopAbs_VERTEX, 12);
    CheckNbShapes(result, TopAbs_EDGE, 11);
}

TEST(OCCT_Boolean_bcommon_2d, I1)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_5_solid.brep", a)) << "Missing case_5_solid.brep";
    ASSERT_TRUE(LoadTestData("case_5_wire5.brep", b)) << "Missing case_5_wire5.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 3000.52, 0.01);
    CheckNbShapes(result, TopAbs_VERTEX, 2);
    CheckNbShapes(result, TopAbs_EDGE, 2);
}

TEST(OCCT_Boolean_bcommon_2d, I2)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_5_shell.brep", a)) << "Missing case_5_shell.brep";
    ASSERT_TRUE(LoadTestData("case_4_edge1.brep", b)) << "Missing case_4_edge1.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 887.185);
    CheckNbShapes(result, TopAbs_VERTEX, 2);
    CheckNbShapes(result, TopAbs_EDGE, 1);
}

TEST(OCCT_Boolean_bcommon_2d, I3)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_5_shell.brep", a)) << "Missing case_5_shell.brep";
    ASSERT_TRUE(LoadTestData("case_4_edge3.brep", b)) << "Missing case_4_edge3.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 829.541);
    CheckNbShapes(result, TopAbs_VERTEX, 2);
    CheckNbShapes(result, TopAbs_EDGE, 1);
}

TEST(OCCT_Boolean_bcommon_2d, I4)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_5_shell.brep", a)) << "Missing case_5_shell.brep";
    ASSERT_TRUE(LoadTestData("case_4_wire1.brep", b)) << "Missing case_4_wire1.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 1747.73);
    CheckNbShapes(result, TopAbs_VERTEX, 4);
    CheckNbShapes(result, TopAbs_EDGE, 2);
}

TEST(OCCT_Boolean_bcommon_2d, I5)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_5_shell.brep", a)) << "Missing case_5_shell.brep";
    ASSERT_TRUE(LoadTestData("case_4_wire3.brep", b)) << "Missing case_4_wire3.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 1808.02, 0.01);
    CheckNbShapes(result, TopAbs_VERTEX, 12);
    CheckNbShapes(result, TopAbs_EDGE, 11);
}

TEST(OCCT_Boolean_bcommon_2d, I6)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_5_shell.brep", a)) << "Missing case_5_shell.brep";
    ASSERT_TRUE(LoadTestData("case_5_wire5.brep", b)) << "Missing case_5_wire5.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 3000.52, 0.01);
    CheckNbShapes(result, TopAbs_VERTEX, 2);
    CheckNbShapes(result, TopAbs_EDGE, 2);
}

TEST(OCCT_Boolean_bcommon_2d, I7)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_5_face.brep", a)) << "Missing case_5_face.brep";
    ASSERT_TRUE(LoadTestData("case_4_edge1.brep", b)) << "Missing case_4_edge1.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 887.185);
    CheckNbShapes(result, TopAbs_VERTEX, 2);
    CheckNbShapes(result, TopAbs_EDGE, 1);
}

TEST(OCCT_Boolean_bcommon_2d, I8)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_5_face.brep", a)) << "Missing case_5_face.brep";
    ASSERT_TRUE(LoadTestData("case_4_edge3.brep", b)) << "Missing case_4_edge3.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 829.541);
    CheckNbShapes(result, TopAbs_VERTEX, 2);
    CheckNbShapes(result, TopAbs_EDGE, 1);
}

TEST(OCCT_Boolean_bcommon_2d, I9)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_5_face.brep", a)) << "Missing case_5_face.brep";
    ASSERT_TRUE(LoadTestData("case_4_wire1.brep", b)) << "Missing case_4_wire1.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 1747.73);
    CheckNbShapes(result, TopAbs_VERTEX, 4);
    CheckNbShapes(result, TopAbs_EDGE, 2);
}

TEST(OCCT_Boolean_bcommon_2d, J1)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_5_face.brep", a)) << "Missing case_5_face.brep";
    ASSERT_TRUE(LoadTestData("case_4_wire3.brep", b)) << "Missing case_4_wire3.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 1808.02, 0.01);
    CheckNbShapes(result, TopAbs_VERTEX, 12);
    CheckNbShapes(result, TopAbs_EDGE, 11);
}

TEST(OCCT_Boolean_bcommon_2d, K8)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_7_solid.brep", a)) << "Missing case_7_solid.brep";
    ASSERT_TRUE(LoadTestData("case_7_edge1.brep", b)) << "Missing case_7_edge1.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 482.392);
    CheckNbShapes(result, TopAbs_VERTEX, 6);
    CheckNbShapes(result, TopAbs_EDGE, 3);
}

TEST(OCCT_Boolean_bcommon_2d, K9)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_7_solid.brep", a)) << "Missing case_7_solid.brep";
    ASSERT_TRUE(LoadTestData("case_7_edge2.brep", b)) << "Missing case_7_edge2.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 482.392);
    CheckNbShapes(result, TopAbs_VERTEX, 6);
    CheckNbShapes(result, TopAbs_EDGE, 3);
}

TEST(OCCT_Boolean_bcommon_2d, L1)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_7_solid.brep", a)) << "Missing case_7_solid.brep";
    ASSERT_TRUE(LoadTestData("case_7_edge3.brep", b)) << "Missing case_7_edge3.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 520.393);
    CheckNbShapes(result, TopAbs_VERTEX, 6);
    CheckNbShapes(result, TopAbs_EDGE, 3);
}

TEST(OCCT_Boolean_bcommon_2d, L2)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_7_solid.brep", a)) << "Missing case_7_solid.brep";
    ASSERT_TRUE(LoadTestData("case_7_edge4.brep", b)) << "Missing case_7_edge4.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 520.393);
    CheckNbShapes(result, TopAbs_VERTEX, 6);
    CheckNbShapes(result, TopAbs_EDGE, 3);
}

TEST(OCCT_Boolean_bcommon_2d, L3)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_7_solid.brep", a)) << "Missing case_7_solid.brep";
    ASSERT_TRUE(LoadTestData("case_7_wire1.brep", b)) << "Missing case_7_wire1.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 482.392);
    CheckNbShapes(result, TopAbs_VERTEX, 6);
    CheckNbShapes(result, TopAbs_EDGE, 3);
}

TEST(OCCT_Boolean_bcommon_2d, L4)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_7_solid.brep", a)) << "Missing case_7_solid.brep";
    ASSERT_TRUE(LoadTestData("case_7_wire2.brep", b)) << "Missing case_7_wire2.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 482.392);
    CheckNbShapes(result, TopAbs_VERTEX, 6);
    CheckNbShapes(result, TopAbs_EDGE, 3);
}

TEST(OCCT_Boolean_bcommon_2d, L5)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_7_solid.brep", a)) << "Missing case_7_solid.brep";
    ASSERT_TRUE(LoadTestData("case_7_wire3.brep", b)) << "Missing case_7_wire3.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 520.393);
    CheckNbShapes(result, TopAbs_VERTEX, 6);
    CheckNbShapes(result, TopAbs_EDGE, 3);
}

TEST(OCCT_Boolean_bcommon_2d, L6)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_7_solid.brep", a)) << "Missing case_7_solid.brep";
    ASSERT_TRUE(LoadTestData("case_7_wire4.brep", b)) << "Missing case_7_wire4.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 520.393);
    CheckNbShapes(result, TopAbs_VERTEX, 6);
    CheckNbShapes(result, TopAbs_EDGE, 3);
}

TEST(OCCT_Boolean_bcommon_2d, L7)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_7_shell.brep", a)) << "Missing case_7_shell.brep";
    ASSERT_TRUE(LoadTestData("case_7_edge1.brep", b)) << "Missing case_7_edge1.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 482.392);
    CheckNbShapes(result, TopAbs_VERTEX, 6);
    CheckNbShapes(result, TopAbs_EDGE, 3);
}

TEST(OCCT_Boolean_bcommon_2d, L8)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_7_shell.brep", a)) << "Missing case_7_shell.brep";
    ASSERT_TRUE(LoadTestData("case_7_edge3.brep", b)) << "Missing case_7_edge3.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 520.393);
    CheckNbShapes(result, TopAbs_VERTEX, 6);
    CheckNbShapes(result, TopAbs_EDGE, 3);
}

TEST(OCCT_Boolean_bcommon_2d, L9)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_7_shell.brep", a)) << "Missing case_7_shell.brep";
    ASSERT_TRUE(LoadTestData("case_7_wire1.brep", b)) << "Missing case_7_wire1.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 482.392);
    CheckNbShapes(result, TopAbs_VERTEX, 6);
    CheckNbShapes(result, TopAbs_EDGE, 3);
}

TEST(OCCT_Boolean_bcommon_2d, M1)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_7_shell.brep", a)) << "Missing case_7_shell.brep";
    ASSERT_TRUE(LoadTestData("case_7_wire3.brep", b)) << "Missing case_7_wire3.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 520.393);
    CheckNbShapes(result, TopAbs_VERTEX, 6);
    CheckNbShapes(result, TopAbs_EDGE, 3);
}

TEST(OCCT_Boolean_bcommon_2d, M2)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_7_face.brep", a)) << "Missing case_7_face.brep";
    ASSERT_TRUE(LoadTestData("case_7_edge1.brep", b)) << "Missing case_7_edge1.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 482.392);
    CheckNbShapes(result, TopAbs_VERTEX, 6);
    CheckNbShapes(result, TopAbs_EDGE, 3);
}

TEST(OCCT_Boolean_bcommon_2d, M3)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_7_face.brep", a)) << "Missing case_7_face.brep";
    ASSERT_TRUE(LoadTestData("case_7_edge3.brep", b)) << "Missing case_7_edge3.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 520.393);
    CheckNbShapes(result, TopAbs_VERTEX, 6);
    CheckNbShapes(result, TopAbs_EDGE, 3);
}

TEST(OCCT_Boolean_bcommon_2d, M4)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_7_face.brep", a)) << "Missing case_7_face.brep";
    ASSERT_TRUE(LoadTestData("case_7_wire1.brep", b)) << "Missing case_7_wire1.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 482.392);
    CheckNbShapes(result, TopAbs_VERTEX, 6);
    CheckNbShapes(result, TopAbs_EDGE, 3);
}

TEST(OCCT_Boolean_bcommon_2d, M5)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_7_face.brep", a)) << "Missing case_7_face.brep";
    ASSERT_TRUE(LoadTestData("case_7_wire3.brep", b)) << "Missing case_7_wire3.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 520.393);
    CheckNbShapes(result, TopAbs_VERTEX, 6);
    CheckNbShapes(result, TopAbs_EDGE, 3);
}

TEST(OCCT_Boolean_bcommon_2d, M6)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_8_solid_repaired.brep", a)) << "Missing case_8_solid_repaired.brep";
    ASSERT_TRUE(LoadTestData("case_8_wire1.brep", b)) << "Missing case_8_wire1.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 181.079);
    CheckNbShapes(result, TopAbs_VERTEX, 3);
    CheckNbShapes(result, TopAbs_EDGE, 2);
}

TEST(OCCT_Boolean_bcommon_2d, M7)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_8_solid_repaired.brep", a)) << "Missing case_8_solid_repaired.brep";
    ASSERT_TRUE(LoadTestData("case_8_wire2.brep", b)) << "Missing case_8_wire2.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 49.966);
    CheckNbShapes(result, TopAbs_VERTEX, 4);
    CheckNbShapes(result, TopAbs_EDGE, 2);
}

TEST(OCCT_Boolean_bcommon_2d, M8)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_8_solid_repaired.brep", a)) << "Missing case_8_solid_repaired.brep";
    ASSERT_TRUE(LoadTestData("case_8_wire3.brep", b)) << "Missing case_8_wire3.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 3.83899);
    CheckNbShapes(result, TopAbs_VERTEX, 2);
    CheckNbShapes(result, TopAbs_EDGE, 1);
}

TEST(OCCT_Boolean_bcommon_2d, M9)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_8_solid_repaired.brep", a)) << "Missing case_8_solid_repaired.brep";
    ASSERT_TRUE(LoadTestData("case_8_wire4.brep", b)) << "Missing case_8_wire4.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 36.0874);
    CheckNbShapes(result, TopAbs_VERTEX, 7);
    CheckNbShapes(result, TopAbs_EDGE, 4);
}

TEST(OCCT_Boolean_bcommon_2d, N1)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_8_solid_repaired.brep", a)) << "Missing case_8_solid_repaired.brep";
    ASSERT_TRUE(LoadTestData("case_8_wire5.brep", b)) << "Missing case_8_wire5.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 126.81);
    CheckNbShapes(result, TopAbs_VERTEX, 6);
    CheckNbShapes(result, TopAbs_EDGE, 4);
}

TEST(OCCT_Boolean_bcommon_2d, N2)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_8_solid_repaired.brep", a)) << "Missing case_8_solid_repaired.brep";
    ASSERT_TRUE(LoadTestData("case_8_wire6.brep", b)) << "Missing case_8_wire6.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, -1); // empty
    CheckNbShapes(result, TopAbs_VERTEX, 0);
    CheckNbShapes(result, TopAbs_EDGE, 0);
}

TEST(OCCT_Boolean_bcommon_2d, N3)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_9_wire.brep", a)) << "Missing case_9_wire.brep";
    ASSERT_TRUE(LoadTestData("case_9_edge1.brep", b)) << "Missing case_9_edge1.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 143.787);
    CheckNbShapes(result, TopAbs_VERTEX, 2);
    CheckNbShapes(result, TopAbs_EDGE, 1);
}

TEST(OCCT_Boolean_bcommon_2d, N4)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_9_wire.brep", a)) << "Missing case_9_wire.brep";
    ASSERT_TRUE(LoadTestData("case_9_edge2.brep", b)) << "Missing case_9_edge2.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, -1); // empty
    CheckNbShapes(result, TopAbs_VERTEX, 0);
    CheckNbShapes(result, TopAbs_EDGE, 0);
}

TEST(OCCT_Boolean_bcommon_2d, N5)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_9_wire.brep", a)) << "Missing case_9_wire.brep";
    ASSERT_TRUE(LoadTestData("case_9_edge3.brep", b)) << "Missing case_9_edge3.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 143.787);
    CheckNbShapes(result, TopAbs_VERTEX, 2);
    CheckNbShapes(result, TopAbs_EDGE, 1);
}

TEST(OCCT_Boolean_bcommon_2d, N6)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_9_wire.brep", a)) << "Missing case_9_wire.brep";
    ASSERT_TRUE(LoadTestData("case_9_edge4.brep", b)) << "Missing case_9_edge4.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, -1); // empty
    CheckNbShapes(result, TopAbs_VERTEX, 0);
    CheckNbShapes(result, TopAbs_EDGE, 0);
}

TEST(OCCT_Boolean_bcommon_2d, N7)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_9_wire.brep", a)) << "Missing case_9_wire.brep";
    ASSERT_TRUE(LoadTestData("case_9_edge5.brep", b)) << "Missing case_9_edge5.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 79.9002);
    CheckNbShapes(result, TopAbs_VERTEX, 2);
    CheckNbShapes(result, TopAbs_EDGE, 1);
}

TEST(OCCT_Boolean_bcommon_2d, N8)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_9_wire.brep", a)) << "Missing case_9_wire.brep";
    ASSERT_TRUE(LoadTestData("case_9_wire1.brep", b)) << "Missing case_9_wire1.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 143.787);
    CheckNbShapes(result, TopAbs_VERTEX, 2);
    CheckNbShapes(result, TopAbs_EDGE, 1);
}

TEST(OCCT_Boolean_bcommon_2d, N9)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_9_wire.brep", a)) << "Missing case_9_wire.brep";
    ASSERT_TRUE(LoadTestData("case_9_wire2.brep", b)) << "Missing case_9_wire2.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 143.787);
    CheckNbShapes(result, TopAbs_VERTEX, 2);
    CheckNbShapes(result, TopAbs_EDGE, 1);
}

TEST(OCCT_Boolean_bcommon_2d, O3)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_9_wire_a.brep", a)) << "Missing case_9_wire_a.brep";
    ASSERT_TRUE(LoadTestData("case_9_edge2.brep", b)) << "Missing case_9_edge2.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, -1); // empty
    CheckNbShapes(result, TopAbs_VERTEX, 0);
    CheckNbShapes(result, TopAbs_EDGE, 0);
}

TEST(OCCT_Boolean_bcommon_2d, O4)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_9_wire_a.brep", a)) << "Missing case_9_wire_a.brep";
    ASSERT_TRUE(LoadTestData("case_9_edge3.brep", b)) << "Missing case_9_edge3.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, -1); // empty
    CheckNbShapes(result, TopAbs_VERTEX, 0);
    CheckNbShapes(result, TopAbs_EDGE, 0);
}

TEST(OCCT_Boolean_bcommon_2d, O5)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_9_wire_a.brep", a)) << "Missing case_9_wire_a.brep";
    ASSERT_TRUE(LoadTestData("case_9_edge4.brep", b)) << "Missing case_9_edge4.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, -1); // empty
    CheckNbShapes(result, TopAbs_VERTEX, 0);
    CheckNbShapes(result, TopAbs_EDGE, 0);
}

TEST(OCCT_Boolean_bcommon_2d, O6)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_9_wire_a.brep", a)) << "Missing case_9_wire_a.brep";
    ASSERT_TRUE(LoadTestData("case_9_edge5.brep", b)) << "Missing case_9_edge5.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, -1); // empty
    CheckNbShapes(result, TopAbs_VERTEX, 0);
    CheckNbShapes(result, TopAbs_EDGE, 0);
}

TEST(OCCT_Boolean_bcommon_2d, O7)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_9_wire_a.brep", a)) << "Missing case_9_wire_a.brep";
    ASSERT_TRUE(LoadTestData("case_9_wire1.brep", b)) << "Missing case_9_wire1.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 143.787);
    CheckNbShapes(result, TopAbs_VERTEX, 2);
    CheckNbShapes(result, TopAbs_EDGE, 1);
}

TEST(OCCT_Boolean_bcommon_2d, O8)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_9_wire_a.brep", a)) << "Missing case_9_wire_a.brep";
    ASSERT_TRUE(LoadTestData("case_9_wire2.brep", b)) << "Missing case_9_wire2.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, -1); // empty
    CheckNbShapes(result, TopAbs_VERTEX, 0);
    CheckNbShapes(result, TopAbs_EDGE, 0);
}

TEST(OCCT_Boolean_bcommon_2d, O9)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_9_wire_a.brep", a)) << "Missing case_9_wire_a.brep";
    ASSERT_TRUE(LoadTestData("case_9_wire3.brep", b)) << "Missing case_9_wire3.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, -1); // empty
    CheckNbShapes(result, TopAbs_VERTEX, 0);
    CheckNbShapes(result, TopAbs_EDGE, 0);
}

TEST(OCCT_Boolean_bcommon_2d, P1)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_9_edge.brep", a)) << "Missing case_9_edge.brep";
    ASSERT_TRUE(LoadTestData("case_9_edge1.brep", b)) << "Missing case_9_edge1.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 143.787);
    CheckNbShapes(result, TopAbs_VERTEX, 2);
    CheckNbShapes(result, TopAbs_EDGE, 1);
}

TEST(OCCT_Boolean_bcommon_2d, P2)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_9_edge.brep", a)) << "Missing case_9_edge.brep";
    ASSERT_TRUE(LoadTestData("case_9_edge2.brep", b)) << "Missing case_9_edge2.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, -1); // empty
    CheckNbShapes(result, TopAbs_VERTEX, 0);
    CheckNbShapes(result, TopAbs_EDGE, 0);
}

TEST(OCCT_Boolean_bcommon_2d, P3)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_9_edge.brep", a)) << "Missing case_9_edge.brep";
    ASSERT_TRUE(LoadTestData("case_9_edge3.brep", b)) << "Missing case_9_edge3.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, -1); // empty
    CheckNbShapes(result, TopAbs_VERTEX, 0);
    CheckNbShapes(result, TopAbs_EDGE, 0);
}

TEST(OCCT_Boolean_bcommon_2d, P4)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_9_edge.brep", a)) << "Missing case_9_edge.brep";
    ASSERT_TRUE(LoadTestData("case_9_edge4.brep", b)) << "Missing case_9_edge4.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, -1); // empty
    CheckNbShapes(result, TopAbs_VERTEX, 0);
    CheckNbShapes(result, TopAbs_EDGE, 0);
}

TEST(OCCT_Boolean_bcommon_2d, P5)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_9_edge.brep", a)) << "Missing case_9_edge.brep";
    ASSERT_TRUE(LoadTestData("case_9_edge5.brep", b)) << "Missing case_9_edge5.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, -1); // empty
    CheckNbShapes(result, TopAbs_VERTEX, 0);
    CheckNbShapes(result, TopAbs_EDGE, 0);
}

TEST(OCCT_Boolean_bcommon_2d, P6)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_9_edge.brep", a)) << "Missing case_9_edge.brep";
    ASSERT_TRUE(LoadTestData("case_9_wire1.brep", b)) << "Missing case_9_wire1.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, 143.787);
    CheckNbShapes(result, TopAbs_VERTEX, 2);
    CheckNbShapes(result, TopAbs_EDGE, 1);
}

TEST(OCCT_Boolean_bcommon_2d, P7)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_9_edge.brep", a)) << "Missing case_9_edge.brep";
    ASSERT_TRUE(LoadTestData("case_9_wire2.brep", b)) << "Missing case_9_wire2.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, -1); // empty
    CheckNbShapes(result, TopAbs_VERTEX, 0);
    CheckNbShapes(result, TopAbs_EDGE, 0);
}

TEST(OCCT_Boolean_bcommon_2d, P8)
{
    TopoDS_Shape a, b;
    ASSERT_TRUE(LoadTestData("case_9_edge.brep", a)) << "Missing case_9_edge.brep";
    ASSERT_TRUE(LoadTestData("case_9_wire3.brep", b)) << "Missing case_9_wire3.brep";

    BRepAlgoAPI_Common anAlgo(b, a);
    ASSERT_TRUE(anAlgo.IsDone());
    TopoDS_Shape result = anAlgo.Shape();

    CheckLength(result, -1); // empty
    CheckNbShapes(result, TopAbs_VERTEX, 0);
    CheckNbShapes(result, TopAbs_EDGE, 0);
}
