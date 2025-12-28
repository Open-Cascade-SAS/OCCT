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

#ifndef _Graphic3d_PBRMaterial_HeaderFile
#define _Graphic3d_PBRMaterial_HeaderFile

#include <Image_PixMap.hxx>
#include <Graphic3d_BSDF.hxx>
#include <NCollection_Vec2.hxx>
#include <Standard_TypeDef.hxx>
#include <NCollection_Vec3.hxx>
#include <Standard_TypeDef.hxx>
#include <Quantity_Color.hxx>

//! Class implementing Metallic-Roughness physically based material definition
class Graphic3d_PBRMaterial
{
public:
  //! Creates new physically based material in Metallic-Roughness system.
  //! 'metallic' parameter is 0 by default.
  //! 'roughness' parameter is 1 by default.
  //! 'color' parameter is (0, 0, 0) by default.
  //! 'alpha' parameter is 1 by default.
  //! 'IOR' parameter is 1.5 by default.
  //! 'emission' parameter is (0, 0, 0) by default.
  Standard_EXPORT Graphic3d_PBRMaterial();

  //! Creates new physically based material in Metallic-Roughness system from Graphic3d_BSDF.
  Standard_EXPORT Graphic3d_PBRMaterial(const Graphic3d_BSDF& theBSDF);

  //! Returns material's metallic coefficient in [0, 1] range.
  //! 1 for metals and 0 for dielectrics.
  //! It is preferable to be exactly 0 or 1. Average values are needed for textures mixing in
  //! shader.
  float Metallic() const { return myMetallic; }

  //! Modifies metallic coefficient of material in [0, 1] range.
  Standard_EXPORT void SetMetallic(float theMetallic);

  //! Maps roughness from [0, 1] to [MinRoughness, 1] for calculations.
  Standard_EXPORT static float Roughness(float theNormalizedRoughness);

  //! Returns real value of roughness in [MinRoughness, 1] range for calculations.
  float Roughness() const { return Roughness(myRoughness); }

  //! Returns roughness mapping parameter in [0, 1] range.
  //! Roughness is defined in [0, 1] for handful material settings
  //! and is mapped to [MinRoughness, 1] for calculations.
  float NormalizedRoughness() const { return myRoughness; }

  //! Modifies roughness coefficient of material in [0, 1] range.
  Standard_EXPORT void SetRoughness(float theRoughness);

  //! Returns index of refraction in [1, 3] range.
  float IOR() const { return myIOR; }

  //! Modifies index of refraction in [1, 3] range.
  //! In practice affects only on non-metal materials reflection possibilities.
  Standard_EXPORT void SetIOR(float theIOR);

  //! Returns albedo color with alpha component of material.
  const Quantity_ColorRGBA& Color() const { return myColor; }

  //! Modifies albedo color with alpha component.
  Standard_EXPORT void SetColor(const Quantity_ColorRGBA& theColor);

  //! Modifies only albedo color.
  Standard_EXPORT void SetColor(const Quantity_Color& theColor);

  //! Returns alpha component in range [0, 1].
  float Alpha() const { return myColor.Alpha(); };

  //! Modifies alpha component.
  Standard_EXPORT void SetAlpha(float theAlpha);

  //! Returns light intensity emitted by material.
  //! Values are greater or equal 0.
  NCollection_Vec3<float> Emission() const { return myEmission; }

  //! Modifies light intensity emitted by material.
  Standard_EXPORT void SetEmission(const NCollection_Vec3<float>& theEmission);

  //! Generates material in Metallic-Roughness system from Graphic3d_BSDF.
  Standard_EXPORT void SetBSDF(const Graphic3d_BSDF& theBSDF);

public:
  //! PBR materials comparison operator.
  bool operator==(const Graphic3d_PBRMaterial& theOther) const
  {
    return (myMetallic == theOther.myMetallic) && (myRoughness == theOther.myRoughness)
           && (myIOR == theOther.myIOR) && (myColor == theOther.myColor)
           && (myEmission == theOther.myEmission);
  }

public:
  //! Generates 2D look up table of scale and bias for fresnell zero coefficient.
  //! It is needed for calculation reflectance part of environment lighting.
  //! @param[out]  theLUT table storage (must be Image_Format_RGF).
  //! @param[in]  theNbIntegralSamples number of importance samples in hemisphere integral
  //! calculation for every table item.
  Standard_EXPORT static void GenerateEnvLUT(const occ::handle<Image_PixMap>& theLUT,
                                             unsigned int theNbIntegralSamples = 1024);

  //! Compute material roughness from common material (specular color + shininess).
  //! @param[in] theSpecular  specular color
  //! @param[in] theShiness   normalized shininess coefficient within [0..1] range
  //! @return roughness within [0..1] range
  Standard_EXPORT static float RoughnessFromSpecular(const Quantity_Color& theSpecular,
                                                                  const double   theShiness);

  //! Compute material metallicity from common material (specular color).
  //! @param[in] theSpecular  specular color
  //! @return metallicity within [0..1] range
  static float MetallicFromSpecular(const Quantity_Color& theSpecular)
  {
    return ((NCollection_Vec3<float>)theSpecular).maxComp();
  }

public:
  //! Roughness cannot be 0 in real calculations, so it returns minimal achievable level of
  //! roughness in practice
  static float MinRoughness() { return 0.01f; }

public:
  //! Shows how much times less samples can be used in certain roughness value specular IBL map
  //! generation in compare with samples number for map with roughness of 1. Specular IBL maps with
  //! less roughness values have higher resolution but require less samples for the same quality of
  //! baking. So that reducing samples number is good strategy to improve performance of baking. The
  //! samples number for specular IBL map with roughness of 1 (the maximum possible samples number)
  //! is expected to be defined as baking parameter. Samples number for other roughness values can
  //! be calculated by multiplication origin samples number by this factor.
  //! @param theProbability value from 0 to 1 controlling strength of samples reducing.
  //! Bigger values result in slower reduction to provide better quality but worse performance.
  //! Value of 1 doesn't affect at all so that 1 will be returned (it can be used to disable
  //! reduction strategy).
  //! @param theRoughness roughness value of current generated specular IBL map (from 0 to 1).
  //! @return factor to calculate number of samples for current specular IBL map baking.
  //! Be aware! It has no obligation to return 1 in case of roughness of 1.
  //! Be aware! It produces poor quality with small number of origin samples. In that case it is
  //! recommended to be disabled.
  Standard_EXPORT static float SpecIBLMapSamplesFactor(
    float theProbability,
    float theRoughness);

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const;

private:
  //! Calculates geometry factor of Cook-Torrance BRDF using Smith formula.
  static float lutGenGeometryFactor(float theCosL,
                                                 float theCosV,
                                                 float theRoughness);

  //! Generates quasi-random point from Hammersley set.
  //! @param theNumber number of point
  //! @param theCount size of Hammersley set
  static NCollection_Vec2<float> lutGenHammersley(unsigned int theNumber, unsigned int theCount);

  //! Generates only cosine theta of direction in spherical coordinates system
  //! according to micro facet distribution function from Cook-Torrance BRDF.
  static float lutGenImportanceSampleCosTheta(
    float theHammerslayPointComponent,
    float theRoughness);

  //! Generates direction using point from Hammersley set
  //! according to micro facet distribution function from Cook-Torrance BRDF.
  static NCollection_Vec3<float> lutGenImportanceSample(const NCollection_Vec2<float>& theHammerslayPoint,
                                               float    theRoughness);

  //! Generates vector using cosine of angle between up vector (normal in hemisphere)
  //! and desired vector.
  //! x component for resulting vector will be zero.
  static NCollection_Vec3<float> lutGenView(float theCosV);

  //! Returns reflected vector according axis.
  //! @param theVector vector is needed to be reflected.
  //! @param theAxis axis of reflection.
  static NCollection_Vec3<float> lutGenReflect(const NCollection_Vec3<float>& theVector,
                                      const NCollection_Vec3<float>& theAxis);

private:
  Quantity_ColorRGBA myColor;     //!< albedo color with alpha component [0, 1]
  float myMetallic;  //!< metallic coefficient of material [0, 1]
  float myRoughness; //!< roughness coefficient of material [0, 1]
  NCollection_Vec3<float>     myEmission;  //!< light intensity emitted by material [>= 0]
  float myIOR;       //!< index of refraction [1, 3]
};

#endif // _Graphic3d_PBRMaterial_HeaderFile
