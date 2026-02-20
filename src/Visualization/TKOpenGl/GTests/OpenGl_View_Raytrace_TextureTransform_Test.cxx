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

#include <Graphic3d_TextureParams.hxx>
#include <BVH_Types.hxx>
#include <Standard_Handle.hxx>

#include <cmath>
#include <gtest/gtest.h>

namespace
{
constexpr float THE_EPS = 1.0e-6f;

BVH_Mat4f buildTextureTransformForTest(const occ::handle<Graphic3d_TextureParams>& theParams)
{
  BVH_Mat4f aMatrix;
  aMatrix.InitIdentity();
  if (theParams.IsNull())
  {
    return aMatrix;
  }

  const float aScaleX       = theParams->Scale().x();
  const float aScaleY       = theParams->Scale().y();
  aMatrix.ChangeValue(0, 0) = aScaleX;
  aMatrix.ChangeValue(1, 1) = aScaleY;

  const NCollection_Vec2<float> aTrans = -theParams->Translation();
  aMatrix.ChangeValue(0, 3)            = aScaleX * aTrans.x();
  aMatrix.ChangeValue(1, 3)            = aScaleY * aTrans.y();

  const float aAngle = -theParams->Rotation() * static_cast<float>(M_PI / 180.0);
  const float aSin   = std::sin(aAngle);
  const float aCos   = std::cos(aAngle);

  BVH_Mat4f aRotationMat;
  aRotationMat.SetValue(0, 0, aCos);
  aRotationMat.SetValue(1, 1, aCos);
  aRotationMat.SetValue(0, 1, -aSin);
  aRotationMat.SetValue(1, 0, aSin);

  return aMatrix * aRotationMat;
}
} // namespace

TEST(OpenGl_View_Raytrace_TextureTransformTest, IdentityParams)
{
  occ::handle<Graphic3d_TextureParams> aParams = new Graphic3d_TextureParams();
  const BVH_Mat4f                      aMat    = buildTextureTransformForTest(aParams);

  EXPECT_NEAR(aMat.GetValue(0, 0), 1.0f, THE_EPS);
  EXPECT_NEAR(aMat.GetValue(1, 1), 1.0f, THE_EPS);
  EXPECT_NEAR(aMat.GetValue(2, 2), 1.0f, THE_EPS);
  EXPECT_NEAR(aMat.GetValue(3, 3), 1.0f, THE_EPS);
  EXPECT_NEAR(aMat.GetValue(0, 3), 0.0f, THE_EPS);
  EXPECT_NEAR(aMat.GetValue(1, 3), 0.0f, THE_EPS);
}

TEST(OpenGl_View_Raytrace_TextureTransformTest, NonUniformScaleOnly)
{
  occ::handle<Graphic3d_TextureParams> aParams = new Graphic3d_TextureParams();
  aParams->SetScale(NCollection_Vec2<float>(2.0f, 3.0f));

  const BVH_Mat4f aMat = buildTextureTransformForTest(aParams);
  EXPECT_NEAR(aMat.GetValue(0, 0), 2.0f, THE_EPS);
  EXPECT_NEAR(aMat.GetValue(1, 1), 3.0f, THE_EPS);
  EXPECT_NEAR(aMat.GetValue(0, 3), 0.0f, THE_EPS);
  EXPECT_NEAR(aMat.GetValue(1, 3), 0.0f, THE_EPS);
}

TEST(OpenGl_View_Raytrace_TextureTransformTest, TranslationOnly)
{
  occ::handle<Graphic3d_TextureParams> aParams = new Graphic3d_TextureParams();
  aParams->SetTranslation(NCollection_Vec2<float>(4.0f, -5.0f));

  const BVH_Mat4f aMat = buildTextureTransformForTest(aParams);
  EXPECT_NEAR(aMat.GetValue(0, 3), -4.0f, THE_EPS);
  EXPECT_NEAR(aMat.GetValue(1, 3), 5.0f, THE_EPS);
}

TEST(OpenGl_View_Raytrace_TextureTransformTest, ScaleTranslationRotationComposition)
{
  occ::handle<Graphic3d_TextureParams> aParams = new Graphic3d_TextureParams();
  aParams->SetScale(NCollection_Vec2<float>(2.0f, 3.0f));
  aParams->SetTranslation(NCollection_Vec2<float>(1.0f, 2.0f));
  aParams->SetRotation(90.0f);

  const BVH_Mat4f aMat = buildTextureTransformForTest(aParams);

  EXPECT_NEAR(aMat.GetValue(0, 0), 0.0f, THE_EPS);
  EXPECT_NEAR(aMat.GetValue(0, 1), 2.0f, THE_EPS);
  EXPECT_NEAR(aMat.GetValue(1, 0), -3.0f, THE_EPS);
  EXPECT_NEAR(aMat.GetValue(1, 1), 0.0f, THE_EPS);
  EXPECT_NEAR(aMat.GetValue(0, 3), -2.0f, THE_EPS);
  EXPECT_NEAR(aMat.GetValue(1, 3), -6.0f, THE_EPS);
}

TEST(OpenGl_View_Raytrace_TextureTransformTest, RotationSignConvention)
{
  occ::handle<Graphic3d_TextureParams> aParamsPos = new Graphic3d_TextureParams();
  occ::handle<Graphic3d_TextureParams> aParamsNeg = new Graphic3d_TextureParams();
  aParamsPos->SetRotation(45.0f);
  aParamsNeg->SetRotation(-45.0f);

  const BVH_Mat4f aMatPos = buildTextureTransformForTest(aParamsPos);
  const BVH_Mat4f aMatNeg = buildTextureTransformForTest(aParamsNeg);

  EXPECT_NEAR(aMatPos.GetValue(0, 1), -aMatNeg.GetValue(0, 1), THE_EPS);
  EXPECT_NEAR(aMatPos.GetValue(1, 0), -aMatNeg.GetValue(1, 0), THE_EPS);
}
