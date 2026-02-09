#include <gtest/gtest.h>

#include <Convert_CircleToBSplineCurve.hxx>
#include <Convert_ParameterisationType.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>

TEST(Convert_CircleToBSplineCurveTest, FullCircle_TgtThetaOver2)
{
  const gp_Circ2d aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0);
  const Convert_CircleToBSplineCurve aConv(aCirc, Convert_TgtThetaOver2);

  EXPECT_EQ(aConv.Degree(), 2);
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

TEST(Convert_CircleToBSplineCurveTest, FullCircle_RationalC1)
{
  const gp_Circ2d aCirc(gp_Ax2d(gp_Pnt2d(1.0, 2.0), gp_Dir2d(1.0, 0.0)), 3.0);
  const Convert_CircleToBSplineCurve aConv(aCirc, Convert_RationalC1);

  EXPECT_TRUE(aConv.IsPeriodic());
  EXPECT_GT(aConv.NbPoles(), 0);

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

TEST(Convert_CircleToBSplineCurveTest, Arc_TgtThetaOver2)
{
  const gp_Circ2d aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 1.0);
  const double aU1 = M_PI / 6.0;
  const double aU2 = 5.0 * M_PI / 3.0;
  const Convert_CircleToBSplineCurve aConv(aCirc, aU1, aU2, Convert_TgtThetaOver2);

  EXPECT_FALSE(aConv.IsPeriodic());
  EXPECT_EQ(aConv.Degree(), 2);
  EXPECT_GT(aConv.NbPoles(), 0);

  // Verify first pole matches conic point at U1
  const NCollection_Array1<gp_Pnt2d>& aPoles = aConv.Poles();
  const double                         aR     = aCirc.Radius();
  const gp_Pnt2d                       aCenter = aCirc.Location();
  const gp_Dir2d                       aXDir   = aCirc.XAxis().Direction();
  const gp_Dir2d                       aYDir   = aCirc.YAxis().Direction();

  const double aFirstX = aCenter.X() + aR * (std::cos(aU1) * aXDir.X() + std::sin(aU1) * aYDir.X());
  const double aFirstY = aCenter.Y() + aR * (std::cos(aU1) * aXDir.Y() + std::sin(aU1) * aYDir.Y());
  EXPECT_NEAR(aPoles(1).X(), aFirstX, 1.0e-10);
  EXPECT_NEAR(aPoles(1).Y(), aFirstY, 1.0e-10);

  // Verify last pole matches conic point at U2
  const double aLastX = aCenter.X() + aR * (std::cos(aU2) * aXDir.X() + std::sin(aU2) * aYDir.X());
  const double aLastY = aCenter.Y() + aR * (std::cos(aU2) * aXDir.Y() + std::sin(aU2) * aYDir.Y());
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

TEST(Convert_CircleToBSplineCurveTest, Arc_QuasiAngular)
{
  const gp_Circ2d aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 2.0);
  const double aU1 = 0.0;
  const double aU2 = M_PI;
  const Convert_CircleToBSplineCurve aConv(aCirc, aU1, aU2, Convert_QuasiAngular);

  EXPECT_FALSE(aConv.IsPeriodic());
  EXPECT_EQ(aConv.Degree(), 6);
  EXPECT_GT(aConv.NbPoles(), 0);

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

TEST(Convert_CircleToBSplineCurveTest, Arc_Polynomial)
{
  const gp_Circ2d aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 1.0);
  const double aU1 = 0.0;
  const double aU2 = M_PI * 0.5;
  const Convert_CircleToBSplineCurve aConv(aCirc, aU1, aU2, Convert_Polynomial);

  EXPECT_FALSE(aConv.IsPeriodic());
  EXPECT_EQ(aConv.Degree(), 7);
  EXPECT_EQ(aConv.NbPoles(), 8);
}

TEST(Convert_CircleToBSplineCurveTest, WeightsArePositive)
{
  const gp_Circ2d aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 1.0);
  const Convert_CircleToBSplineCurve aConv(aCirc, Convert_TgtThetaOver2);

  const NCollection_Array1<double>& aWeights = aConv.Weights();
  for (int i = 1; i <= aConv.NbPoles(); ++i)
  {
    EXPECT_GT(aWeights(i), 0.0);
  }
}
