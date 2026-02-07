#include <gtest/gtest.h>

#include <BSplSLib.hxx>
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

TEST(Convert_CylinderToBSplineSurfaceTest, GeometricVerification)
{
  const double aRadius = 3.0;
  const double aV1 = -5.0, aV2 = 5.0;
  const gp_Ax3 anAx3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  const gp_Cylinder aCyl(anAx3, aRadius);
  const Convert_CylinderToBSplineSurface aConv(aCyl, 0.0, M_PI, aV1, aV2);

  const double aTol = 1.0e-10;
  // Sample 5x5 grid of BSpline parameters within the domain
  const NCollection_Array1<double>& aUK = aConv.UKnots();
  const NCollection_Array1<double>& aVK = aConv.VKnots();
  const double aUMin = aUK(aUK.Lower()), aUMax = aUK(aUK.Upper());
  const double aVMin = aVK(aVK.Lower()), aVMax = aVK(aVK.Upper());

  for (int i = 0; i <= 4; ++i)
  {
    const double aU = aUMin + i * (aUMax - aUMin) / 4.0;
    for (int j = 0; j <= 4; ++j)
    {
      const double aV = aVMin + j * (aVMax - aVMin) / 4.0;

      gp_Pnt aPnt;
      BSplSLib::D0(aU, aV, 0, 0,
                   aConv.Poles(), &aConv.Weights(),
                   aConv.UKnots(), aConv.VKnots(),
                   &aConv.UMultiplicities(), &aConv.VMultiplicities(),
                   aConv.UDegree(), aConv.VDegree(),
                   true, false, false, false, aPnt);

      // Verify the point lies on the cylinder: distance from Z axis = R
      const double aDistFromAxis = std::sqrt(aPnt.X() * aPnt.X() + aPnt.Y() * aPnt.Y());
      EXPECT_NEAR(aDistFromAxis, aRadius, aTol)
        << "Point not on cylinder at U=" << aU << " V=" << aV;
      // Verify Z is within the V range
      EXPECT_GE(aPnt.Z(), aV1 - aTol);
      EXPECT_LE(aPnt.Z(), aV2 + aTol);
    }
  }
}
