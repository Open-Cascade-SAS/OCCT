// Copyright (c) 2026 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <gtest/gtest.h>

#include <BSplSLib.hxx>
#include <Convert_TorusToBSplineSurface.hxx>
#include <gp_Torus.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>

TEST(Convert_TorusToBSplineSurfaceTest, FullTorus)
{
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 5.0, 2.0);
  const Convert_TorusToBSplineSurface aConv(aTorus);

  EXPECT_EQ(aConv.UDegree(), 2);
  EXPECT_EQ(aConv.VDegree(), 2);
  EXPECT_TRUE(aConv.IsUPeriodic());
  EXPECT_TRUE(aConv.IsVPeriodic());
  EXPECT_GT(aConv.NbUPoles(), 0);
  EXPECT_GT(aConv.NbVPoles(), 0);
}

TEST(Convert_TorusToBSplineSurfaceTest, TrimmedUV)
{
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 5.0, 2.0);
  const double   aU1 = 0.0, aU2 = M_PI;
  const double   aV1 = 0.0, aV2 = M_PI;
  const Convert_TorusToBSplineSurface aConv(aTorus, aU1, aU2, aV1, aV2);

  EXPECT_FALSE(aConv.IsUPeriodic());
  EXPECT_FALSE(aConv.IsVPeriodic());
  EXPECT_GT(aConv.NbUPoles(), 0);
  EXPECT_GT(aConv.NbVPoles(), 0);
}

TEST(Convert_TorusToBSplineSurfaceTest, UTrimmed)
{
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 5.0, 2.0);
  const Convert_TorusToBSplineSurface aConv(aTorus, 0.0, M_PI, true);

  EXPECT_FALSE(aConv.IsUPeriodic());
  EXPECT_TRUE(aConv.IsVPeriodic());
}

TEST(Convert_TorusToBSplineSurfaceTest, VTrimmed)
{
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 5.0, 2.0);
  const Convert_TorusToBSplineSurface aConv(aTorus, 0.0, M_PI, false);

  EXPECT_TRUE(aConv.IsUPeriodic());
  EXPECT_FALSE(aConv.IsVPeriodic());
}

TEST(Convert_TorusToBSplineSurfaceTest, WeightsArePositive)
{
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 5.0, 2.0);
  const Convert_TorusToBSplineSurface aConv(aTorus);

  const NCollection_Array2<double>& aWeights = aConv.Weights();
  for (int i = 1; i <= aConv.NbUPoles(); ++i)
  {
    for (int j = 1; j <= aConv.NbVPoles(); ++j)
    {
      EXPECT_GT(aWeights(i, j), 0.0);
    }
  }
}

TEST(Convert_TorusToBSplineSurfaceTest, KnotsAreMonotonic)
{
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 5.0, 2.0);
  const Convert_TorusToBSplineSurface aConv(aTorus);

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

TEST(Convert_TorusToBSplineSurfaceTest, GeometricVerification)
{
  const double                        aMajorRadius = 5.0;
  const double                        aMinorRadius = 2.0;
  const gp_Ax3                        anAx3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  const gp_Torus                      aTorus(anAx3, aMajorRadius, aMinorRadius);
  const Convert_TorusToBSplineSurface aConv(aTorus, 0.0, M_PI, 0.0, M_PI);

  const double                      aTol  = 1.0e-10;
  const NCollection_Array1<double>& aUK   = aConv.UKnots();
  const NCollection_Array1<double>& aVK   = aConv.VKnots();
  const double                      aUMin = aUK(aUK.Lower()), aUMax = aUK(aUK.Upper());
  const double                      aVMin = aVK(aVK.Lower()), aVMax = aVK(aVK.Upper());

  for (int i = 0; i <= 4; ++i)
  {
    const double aU = aUMin + i * (aUMax - aUMin) / 4.0;
    for (int j = 0; j <= 4; ++j)
    {
      const double aV = aVMin + j * (aVMax - aVMin) / 4.0;

      gp_Pnt aPnt;
      BSplSLib::D0(aU,
                   aV,
                   0,
                   0,
                   aConv.Poles(),
                   &aConv.Weights(),
                   aConv.UKnots(),
                   aConv.VKnots(),
                   &aConv.UMultiplicities(),
                   &aConv.VMultiplicities(),
                   aConv.UDegree(),
                   aConv.VDegree(),
                   true,
                   true,
                   false,
                   false,
                   aPnt);

      // Verify the point lies on the torus:
      // (sqrt(x^2 + y^2) - R_major)^2 + z^2 = R_minor^2
      const double aRxy    = std::sqrt(aPnt.X() * aPnt.X() + aPnt.Y() * aPnt.Y());
      const double aDistSq = (aRxy - aMajorRadius) * (aRxy - aMajorRadius) + aPnt.Z() * aPnt.Z();
      EXPECT_NEAR(aDistSq, aMinorRadius * aMinorRadius, aTol)
        << "Point not on torus at U=" << aU << " V=" << aV;
    }
  }
}
