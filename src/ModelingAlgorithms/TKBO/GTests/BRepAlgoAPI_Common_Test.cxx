#include "BOPTest_Utilities.pxx"

//==================================================================================================
// BOPCommon Simple Tests - migrating from /tests/boolean/bopcommon_simple/
//==================================================================================================

class BOPCommonSimpleTest : public BRepAlgoAPI_TestBase
{
};

// Test bopcommon_simple/A1: box b1 0 0 0 1 1 1; box b2 0 0 0 1 1 1; bop b1 b2; bopcommon result;
// checkprops result -s 6
TEST_F(BOPCommonSimpleTest, IdenticalBoxes_A1)
{
  const TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  const TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);

  const TopoDS_Shape aResult = PerformCommon(aBox1, aBox2);
  ValidateResult(aResult, 6.0);
}