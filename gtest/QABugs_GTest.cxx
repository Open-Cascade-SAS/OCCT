#include <gtest/gtest.h>
#include "TestDrawHelper.hxx"

#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

// Original function from QABugs_20.cxx
// We can either copy it here or link against the original object file if possible.
// For this example, we copy it to demonstrate the wrapper pattern.
static Standard_Integer OCC31294(Draw_Interpretor& di, Standard_Integer, const char**)
{
  BRepBuilderAPI_MakeVertex mkVert(gp_Pnt(0., 0., 0.));
  BRepBuilderAPI_MakeVertex mkDummy(gp_Pnt(0., 0., 0.));
  BRepPrimAPI_MakePrism     mkPrism(mkVert.Shape(), gp_Vec(0., 0., 1.));

  Standard_Integer nbgen   = mkPrism.Generated(mkVert.Shape()).Extent();
  Standard_Integer nbdummy = mkPrism.Generated(mkDummy.Shape()).Extent();

  if (nbgen != 1 || nbdummy != 0)
  {
    di << "Error: wrong generated list \n";
    return 1; // Return 1 on failure
  }

  return 0; // Return 0 on success
}

TEST(QABugs, OCC31294)
{
  // Define arguments as they would appear in Tcl
  // Command: OCC31294
  std::vector<std::string> args = {"OCC31294"};

  RUN_DRAW_COMMAND(OCC31294, args);
}
