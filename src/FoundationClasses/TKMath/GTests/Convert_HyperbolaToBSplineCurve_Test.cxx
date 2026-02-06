#include <gtest/gtest.h>

#include <Convert_HyperbolaToBSplineCurve.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Pnt2d.hxx>

TEST(Convert_HyperbolaToBSplineCurveTest, BasicConversion)
{
  const gp_Hypr2d aHypr(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 3.0, 2.0);
  const double aU1 = -1.0;
  const double aU2 = 1.0;
  const Convert_HyperbolaToBSplineCurve aConv(aHypr, aU1, aU2);

  EXPECT_FALSE(aConv.IsPeriodic());
  EXPECT_EQ(aConv.Degree(), 2);
  EXPECT_GT(aConv.NbPoles(), 0);
  EXPECT_GT(aConv.NbKnots(), 0);

  // Verify first pole matches hyperbola point at U1
  const NCollection_Array1<gp_Pnt2d>& aPoles   = aConv.Poles();
  const double                         aMajR    = aHypr.MajorRadius();
  const double                         aMinR    = aHypr.MinorRadius();
  const gp_Pnt2d                       aCenter  = aHypr.Location();
  const gp_Dir2d                       aXDir    = aHypr.XAxis().Direction();
  const gp_Dir2d                       aYDir    = aHypr.YAxis().Direction();

  const double aFirstX = aCenter.X() + aMajR * std::cosh(aU1) * aXDir.X()
                        + aMinR * std::sinh(aU1) * aYDir.X();
  const double aFirstY = aCenter.Y() + aMajR * std::cosh(aU1) * aXDir.Y()
                        + aMinR * std::sinh(aU1) * aYDir.Y();
  EXPECT_NEAR(aPoles(1).X(), aFirstX, 1.0e-10);
  EXPECT_NEAR(aPoles(1).Y(), aFirstY, 1.0e-10);

  // Verify last pole matches hyperbola point at U2
  const double aLastX = aCenter.X() + aMajR * std::cosh(aU2) * aXDir.X()
                       + aMinR * std::sinh(aU2) * aYDir.X();
  const double aLastY = aCenter.Y() + aMajR * std::cosh(aU2) * aXDir.Y()
                       + aMinR * std::sinh(aU2) * aYDir.Y();
  EXPECT_NEAR(aPoles(aConv.NbPoles()).X(), aLastX, 1.0e-10);
  EXPECT_NEAR(aPoles(aConv.NbPoles()).Y(), aLastY, 1.0e-10);

  // Verify all weights are positive
  const NCollection_Array1<double>& aWeights = aConv.Weights();
  for (int i = 1; i <= aConv.NbPoles(); ++i)
  {
    EXPECT_GT(aWeights(i), 0.0) << "Weight at index " << i << " is not positive";
  }

  // Verify knots are monotonically increasing
  const NCollection_Array1<double>& aKnots = aConv.Knots();
  for (int i = 2; i <= aConv.NbKnots(); ++i)
  {
    EXPECT_GT(aKnots(i), aKnots(i - 1)) << "Knots not monotonically increasing at index " << i;
  }

  // Verify multiplicities are within valid range
  const NCollection_Array1<int>& aMults = aConv.Multiplicities();
  for (int i = 1; i <= aConv.NbKnots(); ++i)
  {
    EXPECT_GT(aMults(i), 0);
    EXPECT_LE(aMults(i), aConv.Degree() + 1);
  }
}

TEST(Convert_HyperbolaToBSplineCurveTest, LargeRange)
{
  const gp_Hypr2d aHypr(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 1.0, 1.0);
  const double aU1 = -2.0;
  const double aU2 = 2.0;
  const Convert_HyperbolaToBSplineCurve aConv(aHypr, aU1, aU2);

  EXPECT_FALSE(aConv.IsPeriodic());
  EXPECT_GT(aConv.NbPoles(), 0);
  EXPECT_GT(aConv.NbKnots(), 0);

  // Verify first pole matches hyperbola point at U1
  const NCollection_Array1<gp_Pnt2d>& aPoles   = aConv.Poles();
  const double                         aMajR    = aHypr.MajorRadius();
  const double                         aMinR    = aHypr.MinorRadius();
  const gp_Pnt2d                       aCenter  = aHypr.Location();
  const gp_Dir2d                       aXDir    = aHypr.XAxis().Direction();
  const gp_Dir2d                       aYDir    = aHypr.YAxis().Direction();

  const double aFirstX = aCenter.X() + aMajR * std::cosh(aU1) * aXDir.X()
                        + aMinR * std::sinh(aU1) * aYDir.X();
  const double aFirstY = aCenter.Y() + aMajR * std::cosh(aU1) * aXDir.Y()
                        + aMinR * std::sinh(aU1) * aYDir.Y();
  EXPECT_NEAR(aPoles(1).X(), aFirstX, 1.0e-10);
  EXPECT_NEAR(aPoles(1).Y(), aFirstY, 1.0e-10);

  // Verify last pole matches hyperbola point at U2
  const double aLastX = aCenter.X() + aMajR * std::cosh(aU2) * aXDir.X()
                       + aMinR * std::sinh(aU2) * aYDir.X();
  const double aLastY = aCenter.Y() + aMajR * std::cosh(aU2) * aXDir.Y()
                       + aMinR * std::sinh(aU2) * aYDir.Y();
  EXPECT_NEAR(aPoles(aConv.NbPoles()).X(), aLastX, 1.0e-10);
  EXPECT_NEAR(aPoles(aConv.NbPoles()).Y(), aLastY, 1.0e-10);

  // Verify all weights are positive
  const NCollection_Array1<double>& aWeights = aConv.Weights();
  for (int i = 1; i <= aConv.NbPoles(); ++i)
  {
    EXPECT_GT(aWeights(i), 0.0) << "Weight at index " << i << " is not positive";
  }

  // Verify knots are monotonically increasing
  const NCollection_Array1<double>& aKnots = aConv.Knots();
  for (int i = 2; i <= aConv.NbKnots(); ++i)
  {
    EXPECT_GT(aKnots(i), aKnots(i - 1)) << "Knots not monotonically increasing at index " << i;
  }
}

TEST(Convert_HyperbolaToBSplineCurveTest, WeightsArePositive)
{
  const gp_Hypr2d aHypr(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 3.0, 2.0);
  const Convert_HyperbolaToBSplineCurve aConv(aHypr, -1.0, 1.0);

  const NCollection_Array1<double>& aWeights = aConv.Weights();
  for (int i = 1; i <= aConv.NbPoles(); ++i)
  {
    EXPECT_GT(aWeights(i), 0.0);
  }
}
