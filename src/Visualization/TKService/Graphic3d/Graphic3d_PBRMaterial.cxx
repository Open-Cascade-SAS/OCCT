// Author: Ilya Khramov
// Copyright (c) 2019 OPEN CASCADE SAS
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

#include <Graphic3d_PBRMaterial.hxx>

#include <Graphic3d_MaterialDefinitionError.hxx>

#include <limits>

//=================================================================================================

float Graphic3d_PBRMaterial::RoughnessFromSpecular(const Quantity_Color& theSpecular,
                                                                const double   theShiness)
{
  double aRoughnessFactor = 1.0 - theShiness;
  // double aSpecIntens = theSpecular.Light() * theSpecular;
  const double aSpecIntens =
    theSpecular.Red() * 0.2125 + theSpecular.Green() * 0.7154 + theSpecular.Blue() * 0.0721;
  if (aSpecIntens < 0.1)
  {
    // low specular intensity should produce a rough material even if shininess is high
    aRoughnessFactor *= (1.0 - aSpecIntens);
  }
  return (float)aRoughnessFactor;
}

//=================================================================================================

Graphic3d_PBRMaterial::Graphic3d_PBRMaterial()
    : myColor(0.f, 0.f, 0.f, 1.f),
      myMetallic(0.f),
      myRoughness(1.f),
      myEmission(0.f),
      myIOR(1.5f)
{
}

//=================================================================================================

Graphic3d_PBRMaterial::Graphic3d_PBRMaterial(const Graphic3d_BSDF& theBSDF)
{
  SetBSDF(theBSDF);
}

//=================================================================================================

void Graphic3d_PBRMaterial::SetMetallic(float theMetallic)
{
  Graphic3d_MaterialDefinitionError_Raise_if(
    theMetallic < 0.f || theMetallic > 1.f,
    "'metallic' parameter of PBR material must be in range [0, 1]") myMetallic = theMetallic;
}

//=================================================================================================

float Graphic3d_PBRMaterial::Roughness(float theNormalizedRoughness)
{
  return theNormalizedRoughness * (1.f - MinRoughness()) + MinRoughness();
}

//=================================================================================================

void Graphic3d_PBRMaterial::SetRoughness(float theRoughness)
{
  Graphic3d_MaterialDefinitionError_Raise_if(
    theRoughness < 0.f || theRoughness > 1.f,
    "'roughness' parameter of PBR material must be in range [0, 1]") myRoughness = theRoughness;
}

//=================================================================================================

void Graphic3d_PBRMaterial::SetIOR(float theIOR)
{
  Graphic3d_MaterialDefinitionError_Raise_if(
    theIOR < 1.f || theIOR > 3.f,
    "'IOR' parameter of PBR material must be in range [1, 3]") myIOR = theIOR;
}

//=================================================================================================

void Graphic3d_PBRMaterial::SetColor(const Quantity_ColorRGBA& theColor)
{
  myColor.SetRGB(theColor.GetRGB());
  SetAlpha(theColor.Alpha());
}

//=================================================================================================

void Graphic3d_PBRMaterial::SetColor(const Quantity_Color& theColor)
{
  myColor.SetRGB(theColor);
}

//=================================================================================================

void Graphic3d_PBRMaterial::SetAlpha(float theAlpha)
{
  Graphic3d_MaterialDefinitionError_Raise_if(
    theAlpha < 0.f || theAlpha > 1.f,
    "'alpha' parameter of PBR material must be in range [0, 1]") myColor.SetAlpha(theAlpha);
}

//=================================================================================================

void Graphic3d_PBRMaterial::SetEmission(const NCollection_Vec3<float>& theEmission)
{
  Graphic3d_MaterialDefinitionError_Raise_if(
    theEmission.r() < 0.f || theEmission.g() < 0.f || theEmission.b() < 0.f,
    "all components of 'emission' parameter of PBR material must be greater than 0") myEmission =
    theEmission;
}

//=================================================================================================

void Graphic3d_PBRMaterial::SetBSDF(const Graphic3d_BSDF& theBSDF)
{
  SetEmission(theBSDF.Le);

  if (theBSDF.Absorption != NCollection_Vec4<float>(0.f))
  {
    SetMetallic(0.f);
    SetColor(Quantity_Color(theBSDF.Absorption.rgb()));
    if (theBSDF.FresnelCoat.FresnelType() == Graphic3d_FM_DIELECTRIC)
    {
      SetIOR(theBSDF.FresnelCoat.Serialize().y());
      SetRoughness(0.f);
      SetAlpha(theBSDF.Absorption.a() * 4.f);
    }
    return;
  }

  if (theBSDF.FresnelBase.FresnelType() == Graphic3d_FM_CONSTANT
      && theBSDF.Kt != NCollection_Vec3<float>(0.f))
  {
    SetIOR(1.f);
    SetRoughness(1.f);
    SetMetallic(0.f);
    SetColor(Quantity_Color(theBSDF.Kt));
    SetAlpha(1.f - (theBSDF.Kt.r() + theBSDF.Kt.g() + theBSDF.Kt.b()) / 3.f);
    return;
  }

  SetRoughness(sqrtf(theBSDF.Ks.w()));
  if (theBSDF.FresnelBase.FresnelType() == Graphic3d_FM_DIELECTRIC
      || theBSDF.FresnelBase.FresnelType() == Graphic3d_FM_CONSTANT)
  {
    SetIOR(1.5f);
    SetColor(Quantity_Color(theBSDF.Kd));
    SetMetallic(0.f);
  }
  else if (theBSDF.FresnelBase.FresnelType() == Graphic3d_FM_SCHLICK)
  {
    SetColor(Quantity_Color(theBSDF.FresnelBase.Serialize().rgb()));
    SetMetallic(1.f);
  }
  else
  {
    SetColor(Quantity_Color(theBSDF.Ks.rgb()));
    SetMetallic(1.f);
  }
}

//=================================================================================================

void Graphic3d_PBRMaterial::GenerateEnvLUT(const occ::handle<Image_PixMap>& theLUT,
                                           unsigned int                theNbIntegralSamples)
{
  if (theLUT->Format() != Image_Format_RGF)
  {
    throw Standard_ProgramError(
      "LUT pix map format for PBR LUT generation must be Image_Format_RGF");
  }

  for (unsigned int y = 0; y < theLUT->SizeY(); ++y)
  {
    float aRoughness = Roughness(y / float(theLUT->SizeY() - 1));

    for (unsigned int x = 0; x < theLUT->SizeX(); ++x)
    {
      float aCosV   = x / float(theLUT->SizeX() - 1);
      NCollection_Vec3<float>     aView   = lutGenView(aCosV);
      NCollection_Vec2<float>     aResult = NCollection_Vec2<float>(0.f);
      for (unsigned int i = 0; i < theNbIntegralSamples; ++i)
      {
        NCollection_Vec2<float> aHammersleyPoint = lutGenHammersley(i, theNbIntegralSamples);
        NCollection_Vec3<float> aHalf            = lutGenImportanceSample(aHammersleyPoint, aRoughness);
        NCollection_Vec3<float> aLight           = lutGenReflect(aView, aHalf);
        if (aLight.z() >= 0.f)
        {
          float aCosVH          = aView.Dot(aHalf);
          float aGeometryFactor = lutGenGeometryFactor(aLight.z(), aCosV, aRoughness);
          float anIntermediateResult = 1.f - aCosVH;
          anIntermediateResult *= anIntermediateResult;
          anIntermediateResult *= anIntermediateResult;
          anIntermediateResult *= 1.f - aCosVH;

          aResult.x() += aGeometryFactor * (aCosVH / aHalf.z()) * (1.f - anIntermediateResult);
          aResult.y() += aGeometryFactor * (aCosVH / aHalf.z()) * anIntermediateResult;
        }
      }

      aResult = aResult / float(theNbIntegralSamples);
      theLUT->ChangeValue<NCollection_Vec2<float>>(theLUT->SizeY() - 1 - y, x) = aResult;
    }
  }
}

//=================================================================================================

float Graphic3d_PBRMaterial::SpecIBLMapSamplesFactor(float theProbability,
                                                                  float theRoughness)
{
  return acosf(lutGenImportanceSampleCosTheta(theProbability, theRoughness)) * 2.f
         / float(M_PI);
}

//=================================================================================================

float Graphic3d_PBRMaterial::lutGenGeometryFactor(float theCosL,
                                                               float theCosV,
                                                               float theRoughness)
{
  float aK = theRoughness * theRoughness * 0.5f;

  float aGeometryFactor = theCosL;
  aGeometryFactor /= theCosL * (1.f - aK) + aK;
  aGeometryFactor /= theCosV * (1.f - aK) + aK;

  return aGeometryFactor;
}

//=================================================================================================

NCollection_Vec2<float> Graphic3d_PBRMaterial::lutGenHammersley(unsigned int theNumber,
                                                       unsigned int theCount)
{
  float aPhi2 = 0.f;
  for (unsigned int i = 0; i < sizeof(unsigned int) * 8; ++i)
  {
    if ((theNumber >> i) == 0)
    {
      break;
    }
    aPhi2 += ((theNumber >> i) & 1) / float(1 << (i + 1));
  }

  return NCollection_Vec2<float>(theNumber / float(theCount), aPhi2);
}

//=================================================================================================

float Graphic3d_PBRMaterial::lutGenImportanceSampleCosTheta(
  float theHammersleyPointComponent,
  float theRoughness)
{
  float aQuadRoughness = theRoughness * theRoughness;
  aQuadRoughness *= aQuadRoughness;

  float aTmp = 1.f + (aQuadRoughness - 1.f) * theHammersleyPointComponent;

  if (aTmp != 0.f)
  {
    return sqrtf((1.f - theHammersleyPointComponent) / aTmp);
  }
  else
  {
    return 0.f;
  }
}

//=================================================================================================

NCollection_Vec3<float> Graphic3d_PBRMaterial::lutGenImportanceSample(
  const NCollection_Vec2<float>& theHammerslayPoint,
  float    theRoughness)
{
  float aPhi = 2.f * float(M_PI) * theHammerslayPoint.y();

  float aCosTheta =
    lutGenImportanceSampleCosTheta(theHammerslayPoint.x(), theRoughness);
  float aSinTheta = sqrtf(1.f - aCosTheta * aCosTheta);

  return NCollection_Vec3<float>(aSinTheta * cosf(aPhi), aSinTheta * sinf(aPhi), aCosTheta);
}

//=================================================================================================

NCollection_Vec3<float> Graphic3d_PBRMaterial::lutGenView(float theCosV)
{
  return NCollection_Vec3<float>(0.f, sqrtf(1.f - theCosV * theCosV), theCosV);
}

//=================================================================================================

NCollection_Vec3<float> Graphic3d_PBRMaterial::lutGenReflect(const NCollection_Vec3<float>& theVector,
                                                    const NCollection_Vec3<float>& theAxis)
{
  return theAxis * theAxis.Dot(theVector) * 2.f - theVector;
}

//=================================================================================================

void Graphic3d_PBRMaterial::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_CLASS_BEGIN(theOStream, Graphic3d_PBRMaterial)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myColor)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myMetallic)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myRoughness)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myEmission)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIOR)
}
