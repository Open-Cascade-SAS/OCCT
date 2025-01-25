// Created on: 2015-01-15
// Created by: Danila ULYANOV
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

#ifndef _Graphic3d_BSDF_HeaderFile
#define _Graphic3d_BSDF_HeaderFile

#include <Graphic3d_Vec3.hxx>
#include <Graphic3d_Vec4.hxx>

class Graphic3d_PBRMaterial;

//! Type of the Fresnel model.
enum Graphic3d_FresnelModel
{
  Graphic3d_FM_SCHLICK    = 0,
  Graphic3d_FM_CONSTANT   = 1,
  Graphic3d_FM_CONDUCTOR  = 2,
  Graphic3d_FM_DIELECTRIC = 3
};

//! Describes Fresnel reflectance parameters.
class Graphic3d_Fresnel
{
public:
  //! Creates uninitialized Fresnel factor.
  Graphic3d_Fresnel()
      : myFresnelType(Graphic3d_FM_CONSTANT)
  {
    // ideal specular reflector
    myFresnelData = Graphic3d_Vec3(0.f, 1.f, 0.f);
  }

  //! Creates Schlick's approximation of Fresnel factor.
  static Graphic3d_Fresnel CreateSchlick(const Graphic3d_Vec3& theSpecularColor)
  {
    return Graphic3d_Fresnel(Graphic3d_FM_SCHLICK, theSpecularColor);
  }

  //! Creates Fresnel factor for constant reflection.
  static Graphic3d_Fresnel CreateConstant(const Standard_ShortReal theReflection)
  {
    return Graphic3d_Fresnel(Graphic3d_FM_CONSTANT, Graphic3d_Vec3(0.f, 1.f, theReflection));
  }

  //! Creates Fresnel factor for physical-based dielectric model.
  static Graphic3d_Fresnel CreateDielectric(Standard_ShortReal theRefractionIndex)
  {
    return Graphic3d_Fresnel(Graphic3d_FM_DIELECTRIC, Graphic3d_Vec3(0.f, theRefractionIndex, 0.f));
  }

  //! Creates Fresnel factor for physical-based conductor model.
  static Graphic3d_Fresnel CreateConductor(Standard_ShortReal theRefractionIndex,
                                           Standard_ShortReal theAbsorptionIndex)
  {
    return Graphic3d_Fresnel(Graphic3d_FM_CONDUCTOR,
                             Graphic3d_Vec3(0.f, theRefractionIndex, theAbsorptionIndex));
  }

  //! Creates Fresnel factor for physical-based conductor model (spectral version).
  Standard_EXPORT static Graphic3d_Fresnel CreateConductor(
    const Graphic3d_Vec3& theRefractionIndex,
    const Graphic3d_Vec3& theAbsorptionIndex);

public:
  //! Returns serialized representation of Fresnel factor.
  Standard_EXPORT Graphic3d_Vec4 Serialize() const;

  //! Performs comparison of two objects describing Fresnel factor.
  bool operator==(const Graphic3d_Fresnel& theOther) const
  {
    return myFresnelType == theOther.myFresnelType && myFresnelData == theOther.myFresnelData;
  }

  //! Returns type of Fresnel.
  Graphic3d_FresnelModel FresnelType() const { return myFresnelType; }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

protected:
  //! Creates new Fresnel reflectance factor.
  Graphic3d_Fresnel(Graphic3d_FresnelModel theType, const Graphic3d_Vec3& theData)
      : myFresnelType(theType),
        myFresnelData(theData)
  {
    //
  }

private:
  //! Type of Fresnel approximation.
  Graphic3d_FresnelModel myFresnelType;

  //! Serialized parameters of specific approximation.
  Graphic3d_Vec3 myFresnelData;
};

//! Describes material's BSDF (Bidirectional Scattering Distribution Function) used
//! for physically-based rendering (in path tracing engine). BSDF is represented as
//! weighted mixture of basic BRDFs/BTDFs (Bidirectional Reflectance (Transmittance)
//! Distribution Functions).
//!
//! NOTE: OCCT uses two-layer material model. We have base diffuse, glossy, or transmissive
//! layer, covered by one glossy/specular coat. In the current model, the layers themselves
//! have no thickness; they can simply reflect light or transmits it to the layer under it.
//! We use actual BRDF model only for direct reflection by the coat layer. For transmission
//! through this layer, we approximate it as a flat specular surface.
class Graphic3d_BSDF
{
public:
  //! Weight of coat specular/glossy BRDF.
  Graphic3d_Vec4 Kc;

  //! Weight of base diffuse BRDF.
  Graphic3d_Vec3 Kd;

  //! Weight of base specular/glossy BRDF.
  Graphic3d_Vec4 Ks;

  //! Weight of base specular/glossy BTDF.
  Graphic3d_Vec3 Kt;

  //! Radiance emitted by the surface.
  Graphic3d_Vec3 Le;

  //! Volume scattering color/density.
  Graphic3d_Vec4 Absorption;

  //! Parameters of Fresnel reflectance of coat layer.
  Graphic3d_Fresnel FresnelCoat;

  //! Parameters of Fresnel reflectance of base layer.
  Graphic3d_Fresnel FresnelBase;

public:
  //! Creates BSDF describing diffuse (Lambertian) surface.
  static Standard_EXPORT Graphic3d_BSDF CreateDiffuse(const Graphic3d_Vec3& theWeight);

  //! Creates BSDF describing polished metallic-like surface.
  static Standard_EXPORT Graphic3d_BSDF CreateMetallic(const Graphic3d_Vec3&    theWeight,
                                                       const Graphic3d_Fresnel& theFresnel,
                                                       const Standard_ShortReal theRoughness);

  //! Creates BSDF describing transparent object.
  //! Transparent BSDF models simple transparency without
  //! refraction (the ray passes straight through the surface).
  static Standard_EXPORT Graphic3d_BSDF
    CreateTransparent(const Graphic3d_Vec3&    theWeight,
                      const Graphic3d_Vec3&    theAbsorptionColor,
                      const Standard_ShortReal theAbsorptionCoeff);

  //! Creates BSDF describing glass-like object.
  //! Glass-like BSDF mixes refraction and reflection effects at
  //! grazing angles using physically-based Fresnel dielectric model.
  static Standard_EXPORT Graphic3d_BSDF CreateGlass(const Graphic3d_Vec3&    theWeight,
                                                    const Graphic3d_Vec3&    theAbsorptionColor,
                                                    const Standard_ShortReal theAbsorptionCoeff,
                                                    const Standard_ShortReal theRefractionIndex);

  //! Creates BSDF from PBR metallic-roughness material.
  static Standard_EXPORT Graphic3d_BSDF
    CreateMetallicRoughness(const Graphic3d_PBRMaterial& thePbr);

public:
  //! Creates uninitialized BSDF.
  Standard_EXPORT Graphic3d_BSDF();

  //! Normalizes BSDF components.
  Standard_EXPORT void Normalize();

  //! Performs comparison of two BSDFs.
  Standard_EXPORT bool operator==(const Graphic3d_BSDF& theOther) const;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;
};

#endif // _Graphic3d_BSDF_HeaderFile
