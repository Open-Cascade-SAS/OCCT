#include <gtest/gtest.h>

#include <Convert_EllipseToBSplineCurve.hxx>
#include <Convert_ParameterisationType.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Pnt2d.hxx>

TEST(Convert_EllipseToBSplineCurveTest, FullEllipse_TgtThetaOver2)
{
  const gp_Elips2d anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0, 3.0);
  const Convert_EllipseToBSplineCurve aConv(anElips, Convert_TgtThetaOver2);

  EXPECT_TRUE(aConv.IsPeriodic());
  EXPECT_EQ(aConv.Degree(), 2);
  EXPECT_GT(aConv.NbPoles(), 0);
  EXPECT_GT(aConv.NbKnots(), 0);

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

TEST(Convert_EllipseToBSplineCurveTest, Arc_TgtThetaOver2)
{
  const gp_Elips2d anElips(gp_Ax2d(gp_Pnt2d(1.0, 1.0), gp_Dir2d(1.0, 0.0)), 4.0, 2.0);
  const double aU1 = M_PI / 4.0;
  const double aU2 = 3.0 * M_PI / 2.0;
  const Convert_EllipseToBSplineCurve aConv(anElips, aU1, aU2, Convert_TgtThetaOver2);

  EXPECT_FALSE(aConv.IsPeriodic());
  EXPECT_GT(aConv.NbPoles(), 0);
  EXPECT_GT(aConv.NbKnots(), 0);

  // Verify first pole matches ellipse point at U1
  const NCollection_Array1<gp_Pnt2d>& aPoles   = aConv.Poles();
  const double                         aMajR    = anElips.MajorRadius();
  const double                         aMinR    = anElips.MinorRadius();
  const gp_Pnt2d                       aCenter  = anElips.Location();
  const gp_Dir2d                       aXDir    = anElips.XAxis().Direction();
  const gp_Dir2d                       aYDir    = anElips.YAxis().Direction();

  const double aFirstX = aCenter.X() + aMajR * std::cos(aU1) * aXDir.X()
                        + aMinR * std::sin(aU1) * aYDir.X();
  const double aFirstY = aCenter.Y() + aMajR * std::cos(aU1) * aXDir.Y()
                        + aMinR * std::sin(aU1) * aYDir.Y();
  EXPECT_NEAR(aPoles(1).X(), aFirstX, 1.0e-10);
  EXPECT_NEAR(aPoles(1).Y(), aFirstY, 1.0e-10);

  // Verify last pole matches ellipse point at U2
  const double aLastX = aCenter.X() + aMajR * std::cos(aU2) * aXDir.X()
                       + aMinR * std::sin(aU2) * aYDir.X();
  const double aLastY = aCenter.Y() + aMajR * std::cos(aU2) * aXDir.Y()
                       + aMinR * std::sin(aU2) * aYDir.Y();
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

TEST(Convert_EllipseToBSplineCurveTest, FullEllipse_RationalC1)
{
  const gp_Elips2d anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 3.0, 1.0);
  const Convert_EllipseToBSplineCurve aConv(anElips, Convert_RationalC1);

  EXPECT_TRUE(aConv.IsPeriodic());
  EXPECT_GT(aConv.NbPoles(), 0);
  EXPECT_GT(aConv.NbKnots(), 0);

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

TEST(Convert_EllipseToBSplineCurveTest, WeightsArePositive)
{
  const gp_Elips2d anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0, 3.0);
  const Convert_EllipseToBSplineCurve aConv(anElips, Convert_TgtThetaOver2);

  const NCollection_Array1<double>& aWeights = aConv.Weights();
  for (int i = 1; i <= aConv.NbPoles(); ++i)
  {
    EXPECT_GT(aWeights(i), 0.0);
  }
}
