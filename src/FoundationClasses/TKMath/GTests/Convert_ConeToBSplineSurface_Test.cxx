#include <gtest/gtest.h>

#include <Convert_ConeToBSplineSurface.hxx>
#include <gp_Cone.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array2.hxx>

TEST(Convert_ConeToBSplineSurfaceTest, FullCone)
{
  const gp_Cone aCone(gp_Ax3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), M_PI / 6.0, 3.0);
  const Convert_ConeToBSplineSurface aConv(aCone, 0.0, 10.0);

  EXPECT_EQ(aConv.UDegree(), 2);
  EXPECT_EQ(aConv.VDegree(), 1);
  EXPECT_TRUE(aConv.IsUPeriodic());
  EXPECT_FALSE(aConv.IsVPeriodic());
  EXPECT_GT(aConv.NbUPoles(), 0);
  EXPECT_GT(aConv.NbVPoles(), 0);
}

TEST(Convert_ConeToBSplineSurfaceTest, TrimmedCone)
{
  const gp_Cone aCone(gp_Ax3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), M_PI / 4.0, 2.0);
  const double aU1 = 0.0, aU2 = M_PI;
  const double aV1 = 0.0, aV2 = 5.0;
  const Convert_ConeToBSplineSurface aConv(aCone, aU1, aU2, aV1, aV2);

  EXPECT_FALSE(aConv.IsUPeriodic());
  EXPECT_FALSE(aConv.IsVPeriodic());
}

TEST(Convert_ConeToBSplineSurfaceTest, WeightsArePositive)
{
  const gp_Cone aCone(gp_Ax3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), M_PI / 6.0, 1.0);
  const Convert_ConeToBSplineSurface aConv(aCone, 0.0, 5.0);

  const NCollection_Array2<double>& aWeights = aConv.Weights();
  for (int i = 1; i <= aConv.NbUPoles(); ++i)
  {
    for (int j = 1; j <= aConv.NbVPoles(); ++j)
    {
      EXPECT_GT(aWeights(i, j), 0.0);
    }
  }
}
