#include <gtest/gtest.h>

#include <Convert_CylinderToBSplineSurface.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>

TEST(Convert_CylinderToBSplineSurfaceTest, FullCylinder)
{
  const gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 3.0);
  const Convert_CylinderToBSplineSurface aConv(aCyl, 0.0, 10.0);

  EXPECT_EQ(aConv.UDegree(), 2);
  EXPECT_EQ(aConv.VDegree(), 1);
  EXPECT_TRUE(aConv.IsUPeriodic());
  EXPECT_FALSE(aConv.IsVPeriodic());
  EXPECT_GT(aConv.NbUPoles(), 0);
  EXPECT_GT(aConv.NbVPoles(), 0);
}

TEST(Convert_CylinderToBSplineSurfaceTest, TrimmedCylinder)
{
  const gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 2.0);
  const double aU1 = 0.0, aU2 = M_PI;
  const double aV1 = -5.0, aV2 = 5.0;
  const Convert_CylinderToBSplineSurface aConv(aCyl, aU1, aU2, aV1, aV2);

  EXPECT_FALSE(aConv.IsUPeriodic());
  EXPECT_FALSE(aConv.IsVPeriodic());
  EXPECT_GT(aConv.NbUPoles(), 0);
  EXPECT_GT(aConv.NbVPoles(), 0);
}

TEST(Convert_CylinderToBSplineSurfaceTest, WeightsArePositive)
{
  const gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 1.0);
  const Convert_CylinderToBSplineSurface aConv(aCyl, 0.0, 1.0);

  const NCollection_Array2<double>& aWeights = aConv.Weights();
  for (int i = 1; i <= aConv.NbUPoles(); ++i)
  {
    for (int j = 1; j <= aConv.NbVPoles(); ++j)
    {
      EXPECT_GT(aWeights(i, j), 0.0);
    }
  }
}

TEST(Convert_CylinderToBSplineSurfaceTest, KnotsAreMonotonic)
{
  const gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 1.0);
  const Convert_CylinderToBSplineSurface aConv(aCyl, 0.0, 1.0);

  const NCollection_Array1<double>& aUKnots = aConv.UKnots();
  for (int i = 2; i <= aConv.NbUKnots(); ++i)
  {
    EXPECT_GT(aUKnots(i), aUKnots(i - 1));
  }
  const NCollection_Array1<double>& aVKnots = aConv.VKnots();
  for (int i = 2; i <= aConv.NbVKnots(); ++i)
  {
    EXPECT_GT(aVKnots(i), aVKnots(i - 1));
  }
}
