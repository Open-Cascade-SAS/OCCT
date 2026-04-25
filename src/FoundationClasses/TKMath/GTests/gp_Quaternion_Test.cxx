// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <gp_Ax2.hxx>
#include <gp_Mat.hxx>
#include <gp_Quaternion.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <Precision.hxx>

#include <cmath>

// Test OCC25574: gp_Quaternion Euler angle conversion is consistent and correct
// for all supported Euler sequences, including YawPitchRoll.
TEST(gp_QuaternionTest, OCC25574_EulerAnglesConsistency)
{
  const char* aNames[] = {"Extrinsic_XYZ", "Extrinsic_XZY", "Extrinsic_YZX", "Extrinsic_YXZ",
                          "Extrinsic_ZXY", "Extrinsic_ZYX", "Intrinsic_XYZ", "Intrinsic_XZY",
                          "Intrinsic_YZX", "Intrinsic_YXZ", "Intrinsic_ZXY", "Intrinsic_ZYX",
                          "Extrinsic_XYX", "Extrinsic_XZX", "Extrinsic_YZY", "Extrinsic_YXY",
                          "Extrinsic_ZYZ", "Extrinsic_ZXZ", "Intrinsic_XYX", "Intrinsic_XZX",
                          "Intrinsic_YZY", "Intrinsic_YXY", "Intrinsic_ZXZ", "Intrinsic_ZYZ"};

  gp_Quaternion aQuat;
  aQuat.Set(0.06766916507860499, 0.21848101129786085, 0.11994599260380681, 0.9660744746954637);

  gp_Mat aRinv = aQuat.GetMatrix().Inverted();
  gp_Mat aI;
  aI.SetIdentity();

  // Check round-trip consistency: GetEulerAngles then SetEulerAngles should reproduce the matrix
  for (int i = gp_Extrinsic_XYZ; i <= gp_Intrinsic_ZYZ; i++)
  {
    double alpha, beta, gamma;
    aQuat.GetEulerAngles(gp_EulerSequence(i), alpha, beta, gamma);

    gp_Quaternion aQuat2;
    aQuat2.SetEulerAngles(gp_EulerSequence(i), alpha, beta, gamma);

    gp_Mat aR    = aQuat2.GetMatrix();
    gp_Mat aDiff = aR * aRinv - aI;
    EXPECT_LE(aDiff.Determinant(), 1e-5)
      << "Round-trip failed for Euler sequence " << aNames[i - gp_Extrinsic_XYZ];
  }
}

// Test OCC25574: Each Euler angle rotation around a named axis should not change
// a point lying on that axis.
TEST(gp_QuaternionTest, OCC25574_EulerAxisRotationPreservesAxis)
{
  const char* aNames[] = {"Extrinsic_XYZ", "Extrinsic_XZY", "Extrinsic_YZX", "Extrinsic_YXZ",
                          "Extrinsic_ZXY", "Extrinsic_ZYX", "Intrinsic_XYZ", "Intrinsic_XZY",
                          "Intrinsic_YZX", "Intrinsic_YXZ", "Intrinsic_ZXY", "Intrinsic_ZYX",
                          "Extrinsic_XYX", "Extrinsic_XZX", "Extrinsic_YZY", "Extrinsic_YXY",
                          "Extrinsic_ZYZ", "Extrinsic_ZXZ", "Intrinsic_XYX", "Intrinsic_XZX",
                          "Intrinsic_YZY", "Intrinsic_YXY", "Intrinsic_ZXZ", "Intrinsic_ZYZ"};

  for (int i = gp_Extrinsic_XYZ; i <= gp_Intrinsic_ZYZ; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      // Determine axis index from the sequence name (X=0, Y=1, Z=2)
      const int anAxis = aNames[i - gp_Extrinsic_XYZ][10 + j] - 'X';
      ASSERT_GE(anAxis, 0);
      ASSERT_LE(anAxis, 2);

      // 90-degree rotation around the j-th axis of the sequence
      double anAngles[3] = {0., 0., 0.};
      anAngles[j]        = 0.5 * M_PI;

      gp_Quaternion q2;
      q2.SetEulerAngles(gp_EulerSequence(i), anAngles[0], anAngles[1], anAngles[2]);

      // Unit vector on the rotation axis
      gp_XYZ v(0., 0., 0.);
      v.SetCoord(anAxis + 1, 1.);

      gp_Trsf aT;
      aT.SetRotation(q2);
      gp_XYZ v2 = v;
      aT.Transforms(v2);

      EXPECT_LE((v - v2).SquareModulus(), Precision::SquareConfusion())
        << "Rotation around axis should not move a point on that axis. Sequence: "
        << aNames[i - gp_Extrinsic_XYZ] << ", angle index: " << j;
    }
  }
}

// Test OCC25574: Extrinsic and intrinsic Euler sequences produce compatible matrices.
TEST(gp_QuaternionTest, OCC25574_ExtrinsicIntrinsicCorrespondence)
{
  const double alpha = 0.1517461713131;
  const double beta  = 1.5162198410141;
  const double gamma = 1.9313156236541;

  const gp_EulerSequence aPairs[][2] = {{gp_Extrinsic_XYZ, gp_Intrinsic_ZYX},
                                        {gp_Extrinsic_XZY, gp_Intrinsic_YZX},
                                        {gp_Extrinsic_YZX, gp_Intrinsic_XZY},
                                        {gp_Extrinsic_YXZ, gp_Intrinsic_ZXY},
                                        {gp_Extrinsic_ZXY, gp_Intrinsic_YXZ},
                                        {gp_Extrinsic_ZYX, gp_Intrinsic_XYZ}};
  const char* aPairNames[] = {"XYZ/ZYX", "XZY/YZX", "YZX/XZY", "YXZ/ZXY", "ZXY/YXZ", "ZYX/XYZ"};

  gp_Quaternion aQuat;
  for (int i = 0; i < 6; i++)
  {
    aQuat.SetEulerAngles(aPairs[i][0], alpha, beta, gamma);

    double alpha2, beta2, gamma2;
    aQuat.GetEulerAngles(aPairs[i][1], gamma2, beta2, alpha2);

    EXPECT_NEAR(alpha, alpha2, 1e-5) << "alpha mismatch for pair " << aPairNames[i];
    EXPECT_NEAR(beta, beta2, 1e-5) << "beta mismatch for pair " << aPairNames[i];
    EXPECT_NEAR(gamma, gamma2, 1e-5) << "gamma mismatch for pair " << aPairNames[i];
  }
}

// Test OCC25574 (YawPitchRoll): applying three sequential rotations and recovering
// angles via gp_YawPitchRoll must reproduce the original angles.
TEST(gp_QuaternionTest, OCC25574_YawPitchRollRoundTrip)
{
  const gp_Ax2   aWorld;
  const double   aAlpha = 0.0;
  const double   aBeta  = -35.0 / 180.0 * M_PI;
  const double   aGamma = 90.0 / 180.0 * M_PI;

  // Build the rotated frame step by step (yaw-pitch-roll convention)
  const gp_Quaternion aRotZ(aWorld.Direction(), aAlpha);
  const gp_Vec        aRotY    = aRotZ.Multiply(aWorld.YDirection());
  const gp_Vec        aRotX    = aRotZ.Multiply(aWorld.XDirection());

  const gp_Quaternion aRotYaw(aRotY, aBeta);
  const gp_Vec        aRotZ2  = aRotYaw.Multiply(aWorld.Direction());
  const gp_Vec        aRotX2  = aRotYaw.Multiply(aRotX);

  const gp_Quaternion aRotRoll(aRotX2, aGamma);
  const gp_Vec        aRotZ3  = aRotRoll.Multiply(aRotZ2);

  const gp_Ax2 aResult(gp_Pnt(0., 0., 0.), aRotZ3, aRotX2);

  gp_Trsf aTransformation;
  aTransformation.SetDisplacement(gp_Ax2(), aResult);

  double aComputedAlpha, aComputedBeta, aComputedGamma;
  aTransformation.GetRotation().GetEulerAngles(gp_YawPitchRoll,
                                               aComputedAlpha,
                                               aComputedBeta,
                                               aComputedGamma);

  EXPECT_NEAR(aAlpha, aComputedAlpha, 1e-5) << "YawPitchRoll alpha mismatch";
  EXPECT_NEAR(aBeta, aComputedBeta, 1e-5) << "YawPitchRoll beta mismatch";
  EXPECT_NEAR(aGamma, aComputedGamma, 1e-5) << "YawPitchRoll gamma mismatch";
}

// Test OCC25574 (issue 25946): gp_Intrinsic_ZYX and gp_Extrinsic_XYZ yield the
// same Euler angles in reversed order.
TEST(gp_QuaternionTest, OCC25574_IntrinsicZYX_vs_ExtrinsicXYZ)
{
  gp_Quaternion aQuat;
  aQuat.Set(0.06766916507860499, 0.21848101129786085, 0.11994599260380681, 0.9660744746954637);

  double aAlpha, aBeta, aGamma;
  aQuat.GetEulerAngles(gp_Intrinsic_ZYX, aAlpha, aBeta, aGamma);

  double aAlpha2, aBeta2, aGamma2;
  aQuat.GetEulerAngles(gp_Extrinsic_XYZ, aAlpha2, aBeta2, aGamma2);

  EXPECT_NEAR(aAlpha, aGamma2, 1e-5) << "Intrinsic ZYX alpha should equal Extrinsic XYZ gamma";
  EXPECT_NEAR(aBeta, aBeta2, 1e-5) << "beta should match";
  EXPECT_NEAR(aGamma, aAlpha2, 1e-5) << "Intrinsic ZYX gamma should equal Extrinsic XYZ alpha";
}
