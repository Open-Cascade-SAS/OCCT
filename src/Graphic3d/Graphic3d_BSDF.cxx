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

#include <algorithm>

// =======================================================================
// function : Serialize
// purpose  :
// =======================================================================
Graphic3d_Vec4 Graphic3d_Fresnel::Serialize() const
{
  Graphic3d_Vec4 aData = Graphic3d_Vec4 (myFresnelData, 0.f);

  if (myFresnelType != Graphic3d_FM_SCHLICK)
  {
    aData.x() = -static_cast<float> (myFresnelType);
  }

  return aData;
}

// =======================================================================
// function : fresnelNormal
// purpose  :
// =======================================================================
inline float fresnelNormal (float theN,
                            float theK)
{
  return ((theN - 1.f) * (theN - 1.f) + theK * theK) /
         ((theN + 1.f) * (theN + 1.f) + theK * theK);
}

// =======================================================================
// function : CreateConductor
// purpose  :
// =======================================================================
Graphic3d_Fresnel Graphic3d_Fresnel::CreateConductor (const Graphic3d_Vec3& theRefractionIndex,
                                                      const Graphic3d_Vec3& theAbsorptionIndex)
{
  const Graphic3d_Vec3 aFresnel (fresnelNormal (theRefractionIndex.x(), theAbsorptionIndex.x()),
                                 fresnelNormal (theRefractionIndex.y(), theAbsorptionIndex.y()),
                                 fresnelNormal (theRefractionIndex.z(), theAbsorptionIndex.z()));

  return Graphic3d_Fresnel (Graphic3d_FM_SCHLICK, aFresnel);
}

// =======================================================================
// function : Graphic3d_BSDF
// purpose  :
// =======================================================================
Graphic3d_BSDF::Graphic3d_BSDF()
{
  FresnelCoat = Graphic3d_Fresnel::CreateConstant (0.f);
  FresnelBase = Graphic3d_Fresnel::CreateConstant (1.f);
}

// =======================================================================
// function : operator==
// purpose  :
// =======================================================================
bool Graphic3d_BSDF::operator== (const Graphic3d_BSDF& theOther) const
{
  return Kc          == theOther.Kc
      && Kd          == theOther.Kd
      && Kt          == theOther.Kt
      && Ks          == theOther.Ks
      && Le          == theOther.Le
      && Absorption  == theOther.Absorption
      && FresnelCoat == theOther.FresnelCoat
      && FresnelBase == theOther.FresnelBase;
}

// =======================================================================
// function : Normalize
// purpose  :
// =======================================================================
void Graphic3d_BSDF::Normalize()
{
  float aMax = 0.f;

  for (int aChannelID = 0; aChannelID < 3; ++aChannelID)
  {
    aMax = std::max (aMax, Kd[aChannelID] + Ks[aChannelID] + Kt[aChannelID]);
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

// =======================================================================
// function : CreateDiffuse
// purpose  :
// =======================================================================
Graphic3d_BSDF Graphic3d_BSDF::CreateDiffuse (const Graphic3d_Vec3& theWeight)
{
  Graphic3d_BSDF aBSDF;

  aBSDF.Kd = theWeight;

  return aBSDF;
}

// =======================================================================
// function : CreateMetallic
// purpose  :
// =======================================================================
Graphic3d_BSDF Graphic3d_BSDF::CreateMetallic (const Graphic3d_Vec3& theWeight, const Graphic3d_Fresnel& theFresnel, const float theRoughness)
{
  Graphic3d_BSDF aBSDF;

  aBSDF.FresnelBase = theFresnel;

  // Selecting between specular and glossy
  // BRDF depending on the given roughness
  aBSDF.Ks = Graphic3d_Vec4 (theWeight, theRoughness);

  return aBSDF;
}

// =======================================================================
// function : CreateTransparent
// purpose  :
// =======================================================================
Graphic3d_BSDF Graphic3d_BSDF::CreateTransparent (const Graphic3d_Vec3& theWeight,
                                                  const Graphic3d_Vec3& theAbsorptionColor,
                                                  const float           theAbsorptionCoeff)
{
  Graphic3d_BSDF aBSDF;

  // Create Fresnel parameters for the coat layer;
  // set it to 0 value to simulate ideal refractor
  aBSDF.FresnelCoat = Graphic3d_Fresnel::CreateConstant (0.f);

  aBSDF.Kt = theWeight;

  // Link reflection and transmission coefficients
  aBSDF.Kc.r() = aBSDF.Kt.r();
  aBSDF.Kc.g() = aBSDF.Kt.g();
  aBSDF.Kc.b() = aBSDF.Kt.b();

  aBSDF.Absorption = Graphic3d_Vec4 (theAbsorptionColor,
                                     theAbsorptionCoeff);

  return aBSDF;
}

// =======================================================================
// function : CreateGlass
// purpose  :
// =======================================================================
Graphic3d_BSDF Graphic3d_BSDF::CreateGlass (const Graphic3d_Vec3& theWeight,
                                            const Graphic3d_Vec3& theAbsorptionColor,
                                            const float           theAbsorptionCoeff,
                                            const float           theRefractionIndex)
{
  Graphic3d_BSDF aBSDF;

  // Create Fresnel parameters for the coat layer
  aBSDF.FresnelCoat = Graphic3d_Fresnel::CreateDielectric (theRefractionIndex);

  aBSDF.Kt = theWeight;

  aBSDF.Kc.r() = aBSDF.Kt.r();
  aBSDF.Kc.g() = aBSDF.Kt.g();
  aBSDF.Kc.b() = aBSDF.Kt.b();

  aBSDF.Absorption = Graphic3d_Vec4 (theAbsorptionColor,
                                     theAbsorptionCoeff);

  return aBSDF;
}