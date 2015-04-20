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
    aData.x() = -static_cast<Standard_ShortReal> (myFresnelType);
  }

  return aData;
}

// =======================================================================
// function : fresnelNormal
// purpose  :
// =======================================================================
inline Standard_ShortReal fresnelNormal (Standard_ShortReal theN,
                                         Standard_ShortReal theK)
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
  return Graphic3d_Fresnel (Graphic3d_FM_SCHLICK,
    Graphic3d_Vec3 (fresnelNormal (theRefractionIndex.x(), theAbsorptionIndex.x()),
                    fresnelNormal (theRefractionIndex.y(), theAbsorptionIndex.y()),
                    fresnelNormal (theRefractionIndex.z(), theAbsorptionIndex.z())));
}

// =======================================================================
// function : Normalize
// purpose  :
// =======================================================================
void Graphic3d_BSDF::Normalize()
{
  Standard_ShortReal aMax = std::max (Kd.x() + Ks.x() + Kr.x() + Kt.x(),
                            std::max (Kd.y() + Ks.y() + Kr.y() + Kt.y(),
                                      Kd.z() + Ks.z() + Kr.z() + Kt.z()));

  if (aMax > 1.f)
  {
    Kd /= aMax;
    Ks /= aMax;
    Kr /= aMax;
    Ks /= aMax;
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
Graphic3d_BSDF Graphic3d_BSDF::CreateMetallic (const Graphic3d_Vec3&    theWeight,
                                               const Graphic3d_Fresnel& theFresnel,
                                               const Standard_ShortReal theRoughness)
{
  Graphic3d_BSDF aBSDF;

  aBSDF.Roughness = theRoughness;

  // Selecting between specular and glossy
  // BRDF depending on the given roughness
  if (aBSDF.Roughness > 0.f)
  {
    aBSDF.Ks = theWeight;
  }
  else
  {
    aBSDF.Kr = theWeight;
  }

  aBSDF.Fresnel = theFresnel;

  return aBSDF;
}

// =======================================================================
// function : CreateTransparent
// purpose  :
// =======================================================================
Graphic3d_BSDF Graphic3d_BSDF::CreateTransparent (const Graphic3d_Vec3&    theWeight,
                                                  const Graphic3d_Vec3&    theAbsorptionColor,
                                                  const Standard_ShortReal theAbsorptionCoeff)
{
  Graphic3d_BSDF aBSDF;

  aBSDF.Kt = theWeight;

  aBSDF.AbsorptionColor = theAbsorptionColor;
  aBSDF.AbsorptionCoeff = theAbsorptionCoeff;

  aBSDF.Fresnel = Graphic3d_Fresnel::CreateConstant (0.f);

  return aBSDF;
}

// =======================================================================
// function : CreateGlass
// purpose  :
// =======================================================================
Graphic3d_BSDF Graphic3d_BSDF::CreateGlass (const Graphic3d_Vec3&    theWeight,
                                            const Graphic3d_Vec3&    theAbsorptionColor,
                                            const Standard_ShortReal theAbsorptionCoeff,
                                            const Standard_ShortReal theRefractionIndex)
{
  Graphic3d_BSDF aBSDF;

  aBSDF.Kt = theWeight;

  aBSDF.AbsorptionColor = theAbsorptionColor;
  aBSDF.AbsorptionCoeff = theAbsorptionCoeff;

  aBSDF.Fresnel = Graphic3d_Fresnel::CreateDielectric (theRefractionIndex);

  return aBSDF;
}