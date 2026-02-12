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

#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_Aspects.hxx>
#include <Graphic3d_MaterialAspect.hxx>

// Test: default shading model is DEFAULT.
TEST(Graphic3d_AspectsTest, ShadingModel_Default)
{
  occ::handle<Graphic3d_Aspects> anAspect = new Graphic3d_Aspects();
  EXPECT_EQ(anAspect->ShadingModel(), Graphic3d_TypeOfShadingModel_DEFAULT);
}

// Test: explicit UNLIT shading model is preserved.
TEST(Graphic3d_AspectsTest, ShadingModel_ExplicitUnlit)
{
  occ::handle<Graphic3d_Aspects> anAspect = new Graphic3d_Aspects();
  anAspect->SetShadingModel(Graphic3d_TypeOfShadingModel_Unlit);
  EXPECT_EQ(anAspect->ShadingModel(), Graphic3d_TypeOfShadingModel_Unlit);
}

// Test: shading model is independent of material reflection properties.
// Regression test: the removed optimization in OpenGl_Aspects implicitly forced UNLIT
// when material had no reflection properties (all colors BLACK).
TEST(Graphic3d_AspectsTest, ShadingModel_IndependentOfMaterial)
{
  occ::handle<Graphic3d_Aspects> anAspect = new Graphic3d_Aspects();

  // Set a non-UNLIT shading model
  anAspect->SetShadingModel(Graphic3d_TypeOfShadingModel_Phong);

  // Set material with all reflection colors to BLACK (no reflection)
  Graphic3d_MaterialAspect aMat;
  aMat.SetAmbientColor(Quantity_NOC_BLACK);
  aMat.SetDiffuseColor(Quantity_NOC_BLACK);
  aMat.SetSpecularColor(Quantity_NOC_BLACK);
  aMat.SetEmissiveColor(Quantity_NOC_BLACK);
  anAspect->SetFrontMaterial(aMat);

  // Shading model must remain Phong - material should NOT affect shading model
  EXPECT_EQ(anAspect->ShadingModel(), Graphic3d_TypeOfShadingModel_Phong);
}

// Test: each shading model value can be set and retrieved.
TEST(Graphic3d_AspectsTest, ShadingModel_AllValues)
{
  occ::handle<Graphic3d_Aspects> anAspect = new Graphic3d_Aspects();

  anAspect->SetShadingModel(Graphic3d_TypeOfShadingModel_Unlit);
  EXPECT_EQ(anAspect->ShadingModel(), Graphic3d_TypeOfShadingModel_Unlit);

  anAspect->SetShadingModel(Graphic3d_TypeOfShadingModel_PhongFacet);
  EXPECT_EQ(anAspect->ShadingModel(), Graphic3d_TypeOfShadingModel_PhongFacet);

  anAspect->SetShadingModel(Graphic3d_TypeOfShadingModel_Gouraud);
  EXPECT_EQ(anAspect->ShadingModel(), Graphic3d_TypeOfShadingModel_Gouraud);

  anAspect->SetShadingModel(Graphic3d_TypeOfShadingModel_Phong);
  EXPECT_EQ(anAspect->ShadingModel(), Graphic3d_TypeOfShadingModel_Phong);

  anAspect->SetShadingModel(Graphic3d_TypeOfShadingModel_Pbr);
  EXPECT_EQ(anAspect->ShadingModel(), Graphic3d_TypeOfShadingModel_Pbr);

  anAspect->SetShadingModel(Graphic3d_TypeOfShadingModel_PbrFacet);
  EXPECT_EQ(anAspect->ShadingModel(), Graphic3d_TypeOfShadingModel_PbrFacet);
}

// Test: explicit UNLIT with zero-material is the correct way to achieve flat coloring.
// This replaces the old pattern of setting all material colors to BLACK
// and relying on the implicit UNLIT forcing in OpenGl_Aspects.
TEST(Graphic3d_AspectsTest, ShadingModel_ExplicitUnlitWithColor)
{
  occ::handle<Graphic3d_Aspects> anAspect = new Graphic3d_Aspects();
  anAspect->SetShadingModel(Graphic3d_TypeOfShadingModel_Unlit);
  anAspect->SetInteriorColor(Quantity_NOC_AZURE);

  EXPECT_EQ(anAspect->ShadingModel(), Graphic3d_TypeOfShadingModel_Unlit);
  EXPECT_EQ(anAspect->InteriorColor(), Quantity_Color(Quantity_NOC_AZURE));
}

// Test: shading model survives material changes on AspectFillArea3d.
// Verifies the pattern used in updated MeshVS/AIS code:
// create aspect, set material, then set shading model.
TEST(Graphic3d_AspectsTest, ShadingModel_SurvivesMaterialChange)
{
  occ::handle<Graphic3d_AspectFillArea3d> anAspect = new Graphic3d_AspectFillArea3d();
  anAspect->SetShadingModel(Graphic3d_TypeOfShadingModel_Phong);

  // Change material multiple times
  Graphic3d_MaterialAspect aMat1(Graphic3d_NameOfMaterial_Plastified);
  anAspect->SetFrontMaterial(aMat1);
  EXPECT_EQ(anAspect->ShadingModel(), Graphic3d_TypeOfShadingModel_Phong);

  Graphic3d_MaterialAspect aMat2(Graphic3d_NameOfMaterial_DEFAULT);
  anAspect->SetBackMaterial(aMat2);
  EXPECT_EQ(anAspect->ShadingModel(), Graphic3d_TypeOfShadingModel_Phong);

  // Zero material should also not affect shading model
  Graphic3d_MaterialAspect aZeroMat;
  aZeroMat.SetAmbientColor(Quantity_NOC_BLACK);
  aZeroMat.SetDiffuseColor(Quantity_NOC_BLACK);
  aZeroMat.SetSpecularColor(Quantity_NOC_BLACK);
  aZeroMat.SetEmissiveColor(Quantity_NOC_BLACK);
  anAspect->SetFrontMaterial(aZeroMat);
  EXPECT_EQ(anAspect->ShadingModel(), Graphic3d_TypeOfShadingModel_Phong);
}

// Test: SetShadingModel after SetFrontMaterial preserves the model.
// This is the corrected pattern: set UNLIT explicitly after material.
TEST(Graphic3d_AspectsTest, ShadingModel_SetAfterMaterial)
{
  occ::handle<Graphic3d_AspectFillArea3d> anAspect = new Graphic3d_AspectFillArea3d();

  Graphic3d_MaterialAspect aMat(Graphic3d_NameOfMaterial_Plastified);
  anAspect->SetFrontMaterial(aMat);
  anAspect->SetShadingModel(Graphic3d_TypeOfShadingModel_Unlit);

  EXPECT_EQ(anAspect->ShadingModel(), Graphic3d_TypeOfShadingModel_Unlit);
}

// Test: material reflection modes are independent of shading model.
TEST(Graphic3d_AspectsTest, ReflectionMode_IndependentOfShadingModel)
{
  occ::handle<Graphic3d_Aspects> anAspect = new Graphic3d_Aspects();

  // Set Phong shading with a material that has no reflections
  anAspect->SetShadingModel(Graphic3d_TypeOfShadingModel_Phong);

  Graphic3d_MaterialAspect aMat;
  aMat.SetAmbientColor(Quantity_NOC_BLACK);
  aMat.SetDiffuseColor(Quantity_NOC_BLACK);
  aMat.SetSpecularColor(Quantity_NOC_BLACK);
  aMat.SetEmissiveColor(Quantity_NOC_BLACK);
  anAspect->SetFrontMaterial(aMat);

  // Verify material has no reflection
  const Graphic3d_MaterialAspect& aStoredMat = anAspect->FrontMaterial();
  EXPECT_FALSE(aStoredMat.ReflectionMode(Graphic3d_TOR_AMBIENT));
  EXPECT_FALSE(aStoredMat.ReflectionMode(Graphic3d_TOR_DIFFUSE));
  EXPECT_FALSE(aStoredMat.ReflectionMode(Graphic3d_TOR_SPECULAR));
  EXPECT_FALSE(aStoredMat.ReflectionMode(Graphic3d_TOR_EMISSION));

  // But shading model is still Phong (not implicitly forced to UNLIT)
  EXPECT_EQ(anAspect->ShadingModel(), Graphic3d_TypeOfShadingModel_Phong);
}

// Test: PBR shading model is NOT affected by zero Phong material.
// Regression: the old code would force UNLIT even for PBR shading
// if Phong material had no reflections.
TEST(Graphic3d_AspectsTest, ShadingModel_PbrNotAffectedByPhongMaterial)
{
  occ::handle<Graphic3d_Aspects> anAspect = new Graphic3d_Aspects();
  anAspect->SetShadingModel(Graphic3d_TypeOfShadingModel_Pbr);

  Graphic3d_MaterialAspect aMat;
  aMat.SetAmbientColor(Quantity_NOC_BLACK);
  aMat.SetDiffuseColor(Quantity_NOC_BLACK);
  aMat.SetSpecularColor(Quantity_NOC_BLACK);
  aMat.SetEmissiveColor(Quantity_NOC_BLACK);
  anAspect->SetFrontMaterial(aMat);

  // PBR model must be preserved even with zero Phong material
  EXPECT_EQ(anAspect->ShadingModel(), Graphic3d_TypeOfShadingModel_Pbr);
}
