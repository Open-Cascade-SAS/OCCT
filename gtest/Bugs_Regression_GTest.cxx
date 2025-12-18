#include <gtest/gtest.h>

// OCCT Foundation
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <Precision.hxx>

// OCCT Geometry
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom2d_Parabola.hxx>
#include <gp_Parab2d.hxx>
#include <GCE2d_MakeParabola.hxx>

// OCCT Modeling
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <GeomFill_SectionGenerator.hxx>
#include <GeomFill_AppSurf.hxx>
#include <GeomFill_Line.hxx>

#include <cmath>

/**
 * @brief Regression test for bug 13904.
 * Verifies that appsurf (GeomFill_AppSurf) doesn't throw exceptions 
 * when filling between a circle and an ellipse.
 */
TEST(Bugs, Bug13904_SurfaceFilling)
{
  // 1. Create geometry
  gp_Circ c_gp(gp_Ax2(gp_Pnt(0, 100, 100), gp_Dir(0, 1, 1)), 50);
  Handle(Geom_Circle) c = new Geom_Circle(c_gp);
  
  gp_Elips el_gp(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 1)), 100, 50);
  Handle(Geom_Ellipse) el = new Geom_Ellipse(el_gp);

  // 2. Setup Section Generator
  GeomFill_SectionGenerator Section;
  Section.AddCurve(c);
  Section.AddCurve(el);
  Section.Perform(Precision::PConfusion());

  // 3. Perform Approximation
  Handle(GeomFill_Line) Line = new GeomFill_Line(2);
  Standard_Integer NbIt = 0;
  GeomFill_AppSurf App(3, 8, Precision::Confusion(), Precision::PConfusion(), NbIt);

  // Verification: No exception should be thrown
  ASSERT_NO_THROW(App.Perform(Line, Section));
  EXPECT_TRUE(App.IsDone());
}

/**
 * @brief Regression test for OCC31294.
 * Verifies generated shapes extent in prism operation.
 */
TEST(QABugs, OCC31294_PrismGeneration)
{
  BRepBuilderAPI_MakeVertex mkVert(gp_Pnt(0., 0., 0.));
  BRepBuilderAPI_MakeVertex mkDummy(gp_Pnt(0., 0., 0.));
  BRepPrimAPI_MakePrism     mkPrism(mkVert.Shape(), gp_Vec(0., 0., 1.));

  EXPECT_EQ(mkPrism.Generated(mkVert.Shape()).Extent(), 1);
  EXPECT_EQ(mkPrism.Generated(mkDummy.Shape()).Extent(), 0);
}

/**
 * @brief Regression test for OCC28829.
 * Verifies floating point behavior (NaN expected for sqrt of negative).
 */
TEST(QABugs, OCC28829_FPE_Handling)
{
  double val = std::sqrt(-1.0);
  EXPECT_TRUE(std::isnan(val));
}

/**
 * @brief Regression test for OCC26747_1.
 * Verifies parabola creation using directrix and focus.
 */
TEST(QABugs, OCC26747_1_ParabolaCreation)
{
  gp_Ax2d aAxes(gp_Pnt2d(0.0, 3.0), gp_Dir2d(0.0, 1.0));
  gp_Pnt2d aFocusPoint(1.0, 3.0);
  
  GCE2d_MakeParabola aPrb(aAxes, aFocusPoint, Standard_True);
  ASSERT_TRUE(aPrb.IsDone());
  
  EXPECT_NEAR(aPrb.Value()->Parab2d().Focal(), 0.5, 1.0e-12);
  gp_Pnt2d aVert = aPrb.Value()->Parab2d().Location();
  EXPECT_NEAR(aVert.X(), 0.5, 1.0e-12);
  EXPECT_NEAR(aVert.Y(), 3.0, 1.0e-12);
}
