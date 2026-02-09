#include <gtest/gtest.h>

#include <Convert_ParabolaToBSplineCurve.hxx>
#include <ElCLib.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Pnt2d.hxx>
#include <BSplCLib.hxx>
#include <NCollection_Array1.hxx>

namespace
{
void checkParabolaPoint(const Convert_ParabolaToBSplineCurve& theConv,
                        const gp_Parab2d&                     theParab,
                        const double                          theParam)
{
  const NCollection_Array1<gp_Pnt2d>& aPoles   = theConv.Poles();
  const NCollection_Array1<double>&   aWeights  = theConv.Weights();
  const NCollection_Array1<double>&   aKnots    = theConv.Knots();
  const NCollection_Array1<int>&      aMults    = theConv.Multiplicities();
  gp_Pnt2d aBSPnt;
  BSplCLib::D0(theParam, 0, theConv.Degree(), theConv.IsPeriodic(),
               aPoles, &aWeights, aKnots, &aMults, aBSPnt);

  // Use ElCLib to evaluate the reference point on the parabola
  const gp_Pnt2d aExpPnt = ElCLib::Value(theParam, theParab);
  const double aExpX = aExpPnt.X();
  const double aExpY = aExpPnt.Y();

  EXPECT_NEAR(aBSPnt.X(), aExpX, 1.0e-10);
  EXPECT_NEAR(aBSPnt.Y(), aExpY, 1.0e-10);
}
} // namespace

TEST(Convert_ParabolaToBSplineCurveTest, BasicConversion)
{
  const gp_Parab2d aParab(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 1.0);
  const double aU1 = -2.0;
  const double aU2 = 2.0;
  const Convert_ParabolaToBSplineCurve aConv(aParab, aU1, aU2);

  EXPECT_FALSE(aConv.IsPeriodic());
  EXPECT_EQ(aConv.Degree(), 2);
  EXPECT_GT(aConv.NbPoles(), 0);
  EXPECT_GT(aConv.NbKnots(), 0);

  for (int i = 0; i <= 4; ++i)
  {
    const double aParam = aU1 + i * (aU2 - aU1) / 4.0;
    checkParabolaPoint(aConv, aParab, aParam);
  }
}

TEST(Convert_ParabolaToBSplineCurveTest, SmallRange)
{
  const gp_Parab2d aParab(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 0.5);
  const double aU1 = -0.5;
  const double aU2 = 0.5;
  const Convert_ParabolaToBSplineCurve aConv(aParab, aU1, aU2);

  for (int i = 0; i <= 4; ++i)
  {
    const double aParam = aU1 + i * (aU2 - aU1) / 4.0;
    checkParabolaPoint(aConv, aParab, aParam);
  }
}

TEST(Convert_ParabolaToBSplineCurveTest, AllWeightsAreOne)
{
  // Parabola conversion produces non-rational (polynomial) BSpline
  const gp_Parab2d aParab(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 1.0);
  const Convert_ParabolaToBSplineCurve aConv(aParab, -1.0, 1.0);

  const NCollection_Array1<double>& aWeights = aConv.Weights();
  for (int i = aWeights.Lower(); i <= aWeights.Upper(); ++i)
  {
    EXPECT_NEAR(aWeights(i), 1.0, 1.0e-15);
  }
}
