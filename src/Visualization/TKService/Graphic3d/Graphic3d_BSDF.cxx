// Created on: 2015-01-19
// Created by: Denis BOGOLEPOV
// Copyright (c) 2014 OPEN CASCADE SAS
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

#include <Graphic3d_BSDF.hxx>

#include <Graphic3d_PBRMaterial.hxx>

#include <algorithm>

//=================================================================================================

NCollection_Vec4<float> Graphic3d_Fresnel::Serialize() const
{
  NCollection_Vec4<float> aData = NCollection_Vec4<float>(myFresnelData, 0.f);

  if (myFresnelType != Graphic3d_FM_SCHLICK)
  {
    aData.x() = -static_cast<float>(myFresnelType);
  }

  return aData;
}

//=================================================================================================

inline float fresnelNormal(float theN, float theK)
{
  return ((theN - 1.f) * (theN - 1.f) + theK * theK) / ((theN + 1.f) * (theN + 1.f) + theK * theK);
}

//=================================================================================================

Graphic3d_Fresnel Graphic3d_Fresnel::CreateConductor(
  const NCollection_Vec3<float>& theRefractionIndex,
  const NCollection_Vec3<float>& theAbsorptionIndex)
{
  const NCollection_Vec3<float> aFresnel(
    fresnelNormal(theRefractionIndex.x(), theAbsorptionIndex.x()),
    fresnelNormal(theRefractionIndex.y(), theAbsorptionIndex.y()),
    fresnelNormal(theRefractionIndex.z(), theAbsorptionIndex.z()));

  return Graphic3d_Fresnel(Graphic3d_FM_SCHLICK, aFresnel);
}

//=================================================================================================

void Graphic3d_Fresnel::DumpJson(Standard_OStream& theOStream, int theDepth) const {
  OCCT_DUMP_CLASS_BEGIN(theOStream, Graphic3d_Fresnel)

    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myFresnelType)
      OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myFresnelData)}

//=================================================================================================

Graphic3d_BSDF::Graphic3d_BSDF()
    : Ks(NCollection_Vec3<float>(0.f), 1.f)
{
  FresnelCoat = Graphic3d_Fresnel::CreateConstant(0.f);
  FresnelBase = Graphic3d_Fresnel::CreateConstant(1.f);
}

// =======================================================================
// function : operator==
// purpose  :
// =======================================================================
bool Graphic3d_BSDF::operator==(const Graphic3d_BSDF& theOther) const
{
  return Kc == theOther.Kc && Kd == theOther.Kd && Kt == theOther.Kt && Ks == theOther.Ks
         && Le == theOther.Le && Absorption == theOther.Absorption
         && FresnelCoat == theOther.FresnelCoat && FresnelBase == theOther.FresnelBase;
}

//=================================================================================================

void Graphic3d_BSDF::Normalize()
{
  float aMax = 0.f;

  for (int aChannelID = 0; aChannelID < 3; ++aChannelID)
  {
    aMax = std::max(aMax, Kd[aChannelID] + Ks[aChannelID] + Kt[aChannelID]);
  }

  if (aMax > 1.f)
  {
    for (int aChannelID = 0; aChannelID < 3; ++aChannelID)
    {
      Kd[aChannelID] /= aMax;
      Ks[aChannelID] /= aMax;
      Kt[aChannelID] /= aMax;
    }
  }
}

//=================================================================================================

Graphic3d_BSDF Graphic3d_BSDF::CreateDiffuse(const NCollection_Vec3<float>& theWeight)
{
  Graphic3d_BSDF aBSDF;

  aBSDF.Kd = theWeight;

  return aBSDF;
}

//=================================================================================================

Graphic3d_BSDF Graphic3d_BSDF::CreateMetallic(const NCollection_Vec3<float>& theWeight,
                                              const Graphic3d_Fresnel&       theFresnel,
                                              const float                    theRoughness)
{
  Graphic3d_BSDF aBSDF;

  aBSDF.FresnelBase = theFresnel;

  // Selecting between specular and glossy
  // BRDF depending on the given roughness
  aBSDF.Ks = NCollection_Vec4<float>(theWeight, theRoughness);

  return aBSDF;
}

//=================================================================================================

Graphic3d_BSDF Graphic3d_BSDF::CreateTransparent(const NCollection_Vec3<float>& theWeight,
                                                 const NCollection_Vec3<float>& theAbsorptionColor,
                                                 const float                    theAbsorptionCoeff)
{
  Graphic3d_BSDF aBSDF;

  // Create Fresnel parameters for the coat layer;
  // set it to 0 value to simulate ideal refractor
  aBSDF.FresnelCoat = Graphic3d_Fresnel::CreateConstant(0.f);

  aBSDF.Kt = theWeight;

  // Link reflection and transmission coefficients
  aBSDF.Kc.r() = aBSDF.Kt.r();
  aBSDF.Kc.g() = aBSDF.Kt.g();
  aBSDF.Kc.b() = aBSDF.Kt.b();

  aBSDF.Absorption = NCollection_Vec4<float>(theAbsorptionColor, theAbsorptionCoeff);

  return aBSDF;
}

//=================================================================================================

Graphic3d_BSDF Graphic3d_BSDF::CreateGlass(const NCollection_Vec3<float>& theWeight,
                                           const NCollection_Vec3<float>& theAbsorptionColor,
                                           const float                    theAbsorptionCoeff,
                                           const float                    theRefractionIndex)
{
  Graphic3d_BSDF aBSDF;

  // Create Fresnel parameters for the coat layer
  aBSDF.FresnelCoat = Graphic3d_Fresnel::CreateDielectric(theRefractionIndex);

  aBSDF.Kt = theWeight;

  aBSDF.Kc.r() = aBSDF.Kt.r();
  aBSDF.Kc.g() = aBSDF.Kt.g();
  aBSDF.Kc.b() = aBSDF.Kt.b();

  aBSDF.Absorption = NCollection_Vec4<float>(theAbsorptionColor, theAbsorptionCoeff);

  return aBSDF;
}

//=================================================================================================

Graphic3d_BSDF Graphic3d_BSDF::CreateMetallicRoughness(const Graphic3d_PBRMaterial& thePbr)
{
  const NCollection_Vec3<float> aDiff =
    (NCollection_Vec3<float>)thePbr.Color().GetRGB() * thePbr.Alpha();
  const float aRougness2 = thePbr.NormalizedRoughness() * thePbr.NormalizedRoughness();

  Graphic3d_BSDF aBsdf;
  aBsdf.Le = thePbr.Emission();
  if (thePbr.IOR() > 1.0f && thePbr.Alpha() < 1.0f && thePbr.Metallic() <= ShortRealEpsilon())
  {
    aBsdf.FresnelCoat = Graphic3d_Fresnel::CreateDielectric(thePbr.IOR());
    aBsdf.Kt          = NCollection_Vec3<float>(1.0f);
    aBsdf.Kc.r()      = aBsdf.Kt.r();
    aBsdf.Kc.g()      = aBsdf.Kt.g();
    aBsdf.Kc.b()      = aBsdf.Kt.b();
    aBsdf.Absorption.SetValues(thePbr.Color().GetRGB(), thePbr.Alpha() * 0.25f);
  }
  else
  {
    aBsdf.FresnelBase = Graphic3d_Fresnel::CreateSchlick(aDiff * thePbr.Metallic());
    aBsdf.Ks.SetValues(NCollection_Vec3<float>(thePbr.Alpha()), aRougness2);
    aBsdf.Kt = NCollection_Vec3<float>(1.0f - thePbr.Alpha());
    aBsdf.Kd = aDiff * (1.0f - thePbr.Metallic());
  }

  return aBsdf;
}

//=================================================================================================

void Graphic3d_BSDF::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_CLASS_BEGIN(theOStream, Graphic3d_BSDF)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &Kc)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &Kd)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &Ks)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &Kt)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &Le)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &Absorption)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &FresnelCoat)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &FresnelBase)
}
