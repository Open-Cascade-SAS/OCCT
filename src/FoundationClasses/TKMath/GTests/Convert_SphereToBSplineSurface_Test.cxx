#include <gtest/gtest.h>

#include <Convert_SphereToBSplineSurface.hxx>
#include <gp_Sphere.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>

TEST(Convert_SphereToBSplineSurfaceTest, FullSphere)
{
  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 5.0);
  const Convert_SphereToBSplineSurface aConv(aSphere);

  EXPECT_EQ(aConv.UDegree(), 2);
  EXPECT_EQ(aConv.VDegree(), 2);
  EXPECT_TRUE(aConv.IsUPeriodic());
  EXPECT_FALSE(aConv.IsVPeriodic());
  EXPECT_GT(aConv.NbUPoles(), 0);
  EXPECT_GT(aConv.NbVPoles(), 0);
  EXPECT_GT(aConv.NbUKnots(), 0);
  EXPECT_GT(aConv.NbVKnots(), 0);

  // Check all weights are non-negative (sphere poles may have zero weights)
  const NCollection_Array2<double>& aWeights = aConv.Weights();
  for (int i = 1; i <= aConv.NbUPoles(); ++i)
  {
    for (int j = 1; j <= aConv.NbVPoles(); ++j)
    {
      EXPECT_GE(aWeights(i, j), 0.0);
    }
  }
}

TEST(Convert_SphereToBSplineSurfaceTest, TrimmedUV)
{
  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 3.0);
  const double aU1 = 0.0, aU2 = M_PI;
  const double aV1 = -M_PI / 4.0, aV2 = M_PI / 4.0;
  const Convert_SphereToBSplineSurface aConv(aSphere, aU1, aU2, aV1, aV2);

  EXPECT_FALSE(aConv.IsUPeriodic());
  EXPECT_FALSE(aConv.IsVPeriodic());
  EXPECT_GT(aConv.NbUPoles(), 0);
  EXPECT_GT(aConv.NbVPoles(), 0);
}

TEST(Convert_SphereToBSplineSurfaceTest, UTrimmed)
{
  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 2.0);
  const Convert_SphereToBSplineSurface aConv(aSphere, 0.0, M_PI, true);

  EXPECT_FALSE(aConv.IsUPeriodic());
}

TEST(Convert_SphereToBSplineSurfaceTest, VTrimmed)
{
  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 2.0);
  const Convert_SphereToBSplineSurface aConv(aSphere, -M_PI / 4.0, M_PI / 4.0, false);

  EXPECT_TRUE(aConv.IsUPeriodic());
}

TEST(Convert_SphereToBSplineSurfaceTest, KnotsAreMonotonic)
{
  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 1.0);
  const Convert_SphereToBSplineSurface aConv(aSphere);

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
