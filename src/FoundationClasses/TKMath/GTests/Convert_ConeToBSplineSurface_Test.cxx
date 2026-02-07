#include <gtest/gtest.h>

#include <BSplSLib.hxx>
#include <Convert_ConeToBSplineSurface.hxx>
#include <gp_Cone.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
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

TEST(Convert_ConeToBSplineSurfaceTest, GeometricVerification)
{
  const double aSemiAngle = M_PI / 6.0;
  const double aRadius    = 3.0;
  const gp_Ax3 anAx3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  const gp_Cone aCone(anAx3, aSemiAngle, aRadius);
  const Convert_ConeToBSplineSurface aConv(aCone, 0.0, M_PI, 0.0, 10.0);

  const double aTol = 1.0e-10;
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

      // Verify the point lies on the cone:
      // For a cone with axis Z, semi-angle a, reference radius R:
      // r(z) = R + z * tan(a), distance from axis = r
      const double aDistFromAxis = std::sqrt(aPnt.X() * aPnt.X() + aPnt.Y() * aPnt.Y());
      const double aExpectedR    = aRadius + aPnt.Z() * std::tan(aSemiAngle);
      EXPECT_NEAR(aDistFromAxis, aExpectedR, aTol)
        << "Point not on cone at U=" << aU << " V=" << aV;
    }
  }
}
