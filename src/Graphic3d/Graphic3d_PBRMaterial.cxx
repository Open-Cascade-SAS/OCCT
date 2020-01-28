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

// =======================================================================
// function : RoughnessFromSpecular
// purpose  :
// =======================================================================
Standard_ShortReal Graphic3d_PBRMaterial::RoughnessFromSpecular (const Quantity_Color& theSpecular,
                                                                 const Standard_Real theShiness)
{
  Standard_Real aRoughnessFactor = 1.0 - theShiness;
  //Standard_Real aSpecIntens = theSpecular.Light() * theSpecular;
  const Standard_Real aSpecIntens = theSpecular.Red()   * 0.2125
                                  + theSpecular.Green() * 0.7154
                                  + theSpecular.Blue()  * 0.0721;
  if (aSpecIntens < 0.1)
  {
    // low specular intensity should produce a rough material even if shininess is high
    aRoughnessFactor *= (1.0 - aSpecIntens);
  }
  return (Standard_ShortReal )aRoughnessFactor;
}

// =======================================================================
// function : Constructor
// purpose  :
// =======================================================================
Graphic3d_PBRMaterial::Graphic3d_PBRMaterial ()
: myColor     (0.f, 0.f, 0.f, 1.f),
  myMetallic  (0.f),
  myRoughness (1.f),
  myEmission  (0.f),
  myIOR       (1.5f)
{}

// =======================================================================
// function : Constructor
// purpose  :
// =======================================================================
Graphic3d_PBRMaterial::Graphic3d_PBRMaterial (const Graphic3d_BSDF& theBSDF)
{
  SetBSDF (theBSDF);
}

// =======================================================================
// function : SetMetallic
// purpose  :
// =======================================================================
void Graphic3d_PBRMaterial::SetMetallic (Standard_ShortReal theMetallic)
{
  Graphic3d_MaterialDefinitionError_Raise_if (theMetallic < 0.f || theMetallic > 1.f,
    "'metallic' parameter of PBR material must be in range [0, 1]")
  myMetallic = theMetallic;
}

// =======================================================================
// function : Roughness
// purpose  :
// =======================================================================
Standard_ShortReal Graphic3d_PBRMaterial::Roughness (Standard_ShortReal theNormalizedRoughness)
{
  return theNormalizedRoughness * (1.f - MinRoughness()) + MinRoughness();
}

// =======================================================================
// function : SetRoughness
// purpose  :
// =======================================================================
void Graphic3d_PBRMaterial::SetRoughness (Standard_ShortReal theRoughness)
{
  Graphic3d_MaterialDefinitionError_Raise_if (theRoughness < 0.f || theRoughness > 1.f,
    "'roughness' parameter of PBR material must be in range [0, 1]")
  myRoughness = theRoughness;
}

// =======================================================================
// function : SetIOR
// purpose  :
// =======================================================================
void Graphic3d_PBRMaterial::SetIOR (Standard_ShortReal theIOR)
{
  Graphic3d_MaterialDefinitionError_Raise_if (theIOR < 1.f || theIOR > 3.f,
    "'IOR' parameter of PBR material must be in range [1, 3]")
  myIOR = theIOR;
}

// =======================================================================
// function : SetColor
// purpose  :
// =======================================================================
void Graphic3d_PBRMaterial::SetColor (const Quantity_ColorRGBA& theColor)
{
  myColor.SetRGB (theColor.GetRGB());
  SetAlpha (theColor.Alpha());
}

// =======================================================================
// function : SetColor
// purpose  :
// =======================================================================
void Graphic3d_PBRMaterial::SetColor (const Quantity_Color& theColor)
{
  myColor.SetRGB (theColor);
}

// =======================================================================
// function : SetAlpha
// purpose  :
// =======================================================================
void Graphic3d_PBRMaterial::SetAlpha (Standard_ShortReal theAlpha)
{
  Graphic3d_MaterialDefinitionError_Raise_if (theAlpha < 0.f || theAlpha > 1.f,
    "'alpha' parameter of PBR material must be in range [0, 1]")
  myColor.SetAlpha (theAlpha);
}

// =======================================================================
// function : SetEmission
// purpose  :
// =======================================================================
void Graphic3d_PBRMaterial::SetEmission (const Graphic3d_Vec3& theEmission)
{
  Graphic3d_MaterialDefinitionError_Raise_if (theEmission.r() < 0.f
                                           || theEmission.g() < 0.f
                                           || theEmission.b() < 0.f,
    "all components of 'emission' parameter of PBR material must be greater than 0")
  myEmission = theEmission;
}

// =======================================================================
// function : SetBSDF
// purpose  :
// =======================================================================
void Graphic3d_PBRMaterial::SetBSDF (const Graphic3d_BSDF& theBSDF)
{
  SetEmission (theBSDF.Le);

  if (theBSDF.Absorption != Graphic3d_Vec4(0.f))
  {
    SetMetallic (0.f);
    SetColor (Quantity_Color (theBSDF.Absorption.rgb()));
    if (theBSDF.FresnelCoat.FresnelType() == Graphic3d_FM_DIELECTRIC)
    {
      SetIOR (theBSDF.FresnelCoat.Serialize().y());
      SetRoughness (0.f);
      SetAlpha (theBSDF.Absorption.a() * 4.f);
    }
    return;
  }

  if (theBSDF.FresnelBase.FresnelType() == Graphic3d_FM_CONSTANT
   && theBSDF.Kt != Graphic3d_Vec3(0.f))
  {
    SetIOR (1.f);
    SetRoughness (1.f);
    SetMetallic (0.f);
    SetColor (Quantity_Color (theBSDF.Kt));
    SetAlpha (1.f - (theBSDF.Kt.r() + theBSDF.Kt.g() + theBSDF.Kt.b()) / 3.f);
    return;
  }

  SetRoughness(sqrtf (theBSDF.Ks.w()));
  if (theBSDF.FresnelBase.FresnelType() == Graphic3d_FM_DIELECTRIC
   || theBSDF.FresnelBase.FresnelType() == Graphic3d_FM_CONSTANT)
  {
    SetIOR (1.5f);
    SetColor (Quantity_Color (theBSDF.Kd));
    SetMetallic (0.f);
  }
  else if (theBSDF.FresnelBase.FresnelType() == Graphic3d_FM_SCHLICK)
  {
    SetColor (Quantity_Color (theBSDF.FresnelBase.Serialize().rgb()));
    SetMetallic (1.f);
  }
  else
  {
    SetColor (Quantity_Color (theBSDF.Ks.rgb()));
    SetMetallic (1.f);
  }
}

// =======================================================================
// function : GenerateEnvLUT
// purpose  :
// =======================================================================
void Graphic3d_PBRMaterial::GenerateEnvLUT (const Handle(Image_PixMap)& theLUT,
                                            unsigned int theNbIntegralSamples)
{
  if (theLUT->Format() != Image_Format_RGF)
  {
    throw Standard_ProgramError("LUT pix map format for PBR LUT generation must be Image_Format_RGF");
  }

  for (unsigned int y = 0; y < theLUT->SizeY(); ++y)
  {
    Standard_ShortReal aRoughness = Roughness(y / Standard_ShortReal(theLUT->SizeY() - 1));

    for (unsigned int x = 0; x < theLUT->SizeX(); ++x)
    {
      Standard_ShortReal aCosV = x / Standard_ShortReal(theLUT->SizeX() - 1);
      Graphic3d_Vec3 aView = lutGenView (aCosV);
      unsigned int aCount = 0;
      Graphic3d_Vec2 aResult = Graphic3d_Vec2 (0.f);
      for (unsigned int i = 0; i < theNbIntegralSamples; ++i)
      {
        Graphic3d_Vec2 aHammersleyPoint = lutGenHammersley (i, theNbIntegralSamples);
        Graphic3d_Vec3 aHalf = lutGenImportanceSample (aHammersleyPoint, aRoughness);
        Graphic3d_Vec3 aLight = lutGenReflect (aView, aHalf);
        if (aLight.z() >= 0.f)
        {
          ++aCount;
          Standard_ShortReal aCosVH = aView.Dot (aHalf);
          Standard_ShortReal aGeometryFactor = lutGenGeometryFactor (aLight.z(),
                                                                     aCosV,
                                                                     aRoughness);
          Standard_ShortReal anIntermediateResult = 1.f - aCosVH;
          anIntermediateResult *= anIntermediateResult;
          anIntermediateResult *= anIntermediateResult;
          anIntermediateResult *= 1.f - aCosVH;

          aResult.x() += aGeometryFactor * (aCosVH / aHalf.z()) * (1.f - anIntermediateResult);
          aResult.y() += aGeometryFactor * (aCosVH / aHalf.z()) * anIntermediateResult;
        }
      }

      aResult = aResult / Standard_ShortReal(theNbIntegralSamples);
      theLUT->ChangeValue<Graphic3d_Vec2> (theLUT->SizeY() - 1 - y, x) = aResult;
    }
  }
}

// =======================================================================
// function : SpecIBLMapSamplesFactor
// purpose  :
// =======================================================================
Standard_ShortReal Graphic3d_PBRMaterial::SpecIBLMapSamplesFactor (Standard_ShortReal theProbability,
                                                                   Standard_ShortReal theRoughness)
{
  return acosf (lutGenImportanceSampleCosTheta (theProbability, theRoughness)) * 2.f / Standard_ShortReal(M_PI);
}

// =======================================================================
// function : lutGenGeometryFactor
// purpose  :
// =======================================================================
Standard_ShortReal Graphic3d_PBRMaterial::lutGenGeometryFactor (Standard_ShortReal theCosL,
                                                                Standard_ShortReal theCosV,
                                                                Standard_ShortReal theRoughness)
{
  Standard_ShortReal aK = theRoughness * theRoughness * 0.5f;

  Standard_ShortReal aGeometryFactor = theCosL;
  aGeometryFactor /= theCosL * (1.f - aK) + aK;
  aGeometryFactor /= theCosV * (1.f - aK) + aK;

  return aGeometryFactor;
}

// =======================================================================
// function : lutGenHammersley
// purpose  :
// =======================================================================
Graphic3d_Vec2 Graphic3d_PBRMaterial::lutGenHammersley (unsigned int theNumber, unsigned int theCount)
{
  Standard_ShortReal aPhi2 = 0.f;
  for (unsigned int i = 0; i < sizeof(unsigned int) * 8; ++i)
  {
    if ((theNumber >> i) == 0)
    {
      break;
    }
    aPhi2 += ((theNumber >> i) & 1) / Standard_ShortReal(1 << (i + 1));
  }

  return Graphic3d_Vec2(theNumber / Standard_ShortReal(theCount), aPhi2);
}

// =======================================================================
// function : lutGenImportanceSampleCosTheta
// purpose  :
// =======================================================================
Standard_ShortReal Graphic3d_PBRMaterial::lutGenImportanceSampleCosTheta (Standard_ShortReal theHammersleyPointComponent,
                                                                          Standard_ShortReal theRoughness)
{
  Standard_ShortReal aQuadRoughness = theRoughness * theRoughness;
  aQuadRoughness *= aQuadRoughness;

  Standard_ShortReal aTmp = 1.f + (aQuadRoughness - 1.f) * theHammersleyPointComponent;

  if (aTmp != 0.f)
  {
    return sqrtf ((1.f - theHammersleyPointComponent) / aTmp);
  }
  else
  {
    return 0.f;
  }
}

// =======================================================================
// function : lutGenImportanceSample
// purpose  :
// =======================================================================
Graphic3d_Vec3 Graphic3d_PBRMaterial::lutGenImportanceSample (const Graphic3d_Vec2 &theHammerslayPoint,
                                                              Standard_ShortReal   theRoughness)
{
  Standard_ShortReal aPhi = 2.f * Standard_ShortReal(M_PI) * theHammerslayPoint.y();

  Standard_ShortReal aCosTheta = lutGenImportanceSampleCosTheta (theHammerslayPoint.x(), theRoughness);
  Standard_ShortReal aSinTheta = sqrtf (1.f - aCosTheta * aCosTheta);

  return Graphic3d_Vec3(aSinTheta * cosf (aPhi),
                        aSinTheta * sinf (aPhi),
                        aCosTheta);
}

// =======================================================================
// function : lutGenView
// purpose  :
// =======================================================================
Graphic3d_Vec3 Graphic3d_PBRMaterial::lutGenView (Standard_ShortReal theCosV)
{
  return Graphic3d_Vec3(0.f, sqrtf(1.f - theCosV * theCosV), theCosV);
}

// =======================================================================
// function : lutGenReflect
// purpose  :
// =======================================================================
Graphic3d_Vec3 Graphic3d_PBRMaterial::lutGenReflect (const Graphic3d_Vec3 &theVector,
                                                     const Graphic3d_Vec3 &theAxis)
{
  return theAxis * theAxis.Dot(theVector) * 2.f - theVector;
}

//=======================================================================
//function : DumpJson
//purpose  : 
//=======================================================================
void Graphic3d_PBRMaterial::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_CLASS_BEGIN (theOStream, Graphic3d_PBRMaterial)

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myColor)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myMetallic)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myRoughness)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myEmission)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myIOR)
}
