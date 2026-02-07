#include <gtest/gtest.h>

#include <BSplSLib.hxx>
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

TEST(Convert_SphereToBSplineSurfaceTest, GeometricVerification)
{
  const double aRadius = 5.0;
  const gp_Ax3 anAx3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  const gp_Sphere aSphere(anAx3, aRadius);
  // Use trimmed sphere to avoid degenerate poles
  const Convert_SphereToBSplineSurface aConv(aSphere, 0.0, M_PI, -M_PI / 4.0, M_PI / 4.0);

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
                   true, true, false, false, aPnt);

      // Verify the point lies on the sphere: x^2 + y^2 + z^2 = R^2
      const double aDistFromOrigin = aPnt.Distance(gp_Pnt(0.0, 0.0, 0.0));
      EXPECT_NEAR(aDistFromOrigin, aRadius, aTol)
        << "Point not on sphere at U=" << aU << " V=" << aV;
    }
  }
}
