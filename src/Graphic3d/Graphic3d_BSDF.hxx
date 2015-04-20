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
  : myFresnelType (Graphic3d_FM_CONSTANT)
  {
    // ideal specular reflector
    myFresnelData = Graphic3d_Vec3 (0.f, 1.f, 0.f);
  }

  //! Creates Schlick's approximation of Fresnel factor.
  static Graphic3d_Fresnel CreateSchlick (const Graphic3d_Vec3& theSpecularColor)
  {
    return Graphic3d_Fresnel (Graphic3d_FM_SCHLICK, theSpecularColor);
  }

  //! Creates Fresnel factor for constant reflection.
  static Graphic3d_Fresnel CreateConstant (const Standard_ShortReal theReflection)
  {
    return Graphic3d_Fresnel (Graphic3d_FM_CONSTANT, Graphic3d_Vec3 (0.f, 1.f, theReflection));
  }

  //! Creates Fresnel factor for physical-based dielectric model.
  static Graphic3d_Fresnel CreateDielectric (Standard_ShortReal theRefractionIndex)
  {
    return Graphic3d_Fresnel (Graphic3d_FM_DIELECTRIC, Graphic3d_Vec3 (0.f, theRefractionIndex, 0.f));
  }

  //! Creates Fresnel factor for physical-based conductor model.
  static Graphic3d_Fresnel CreateConductor (Standard_ShortReal theRefractionIndex,
                                            Standard_ShortReal theAbsorptionIndex)
  {
    return Graphic3d_Fresnel (Graphic3d_FM_CONDUCTOR, Graphic3d_Vec3 (0.f, theRefractionIndex, theAbsorptionIndex));
  }

  //! Creates Fresnel factor for physical-based conductor model (spectral version).
  Standard_EXPORT static Graphic3d_Fresnel CreateConductor (const Graphic3d_Vec3& theRefractionIndex,
                                                            const Graphic3d_Vec3& theAbsorptionIndex);

public:

  //! Returns serialized representation of Fresnel factor.
  Standard_EXPORT Graphic3d_Vec4 Serialize() const;

  //! Performs comparison of two objects describing Fresnel factor.
  bool operator== (const Graphic3d_Fresnel& theOther) const
  {
    return myFresnelType == theOther.myFresnelType
        && myFresnelData == theOther.myFresnelData;
  }

protected:

  //! Creates new Fresnel reflectance factor.
  Graphic3d_Fresnel (Graphic3d_FresnelModel theType, const Graphic3d_Vec3& theData)
  : myFresnelType (theType),
    myFresnelData (theData)
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
class Graphic3d_BSDF
{
public:

  //! Weight of the Lambertian BRDF.
  Graphic3d_Vec3 Kd;

  //! Weight of the reflection BRDF.
  Graphic3d_Vec3 Kr;

  //! Weight of the transmission BTDF.
  Graphic3d_Vec3 Kt;

  //! Weight of the Blinn's glossy BRDF.
  Graphic3d_Vec3 Ks;

  //! Self-emitted radiance.
  Graphic3d_Vec3 Le;

  //! Parameters of Fresnel reflectance.
  Graphic3d_Fresnel Fresnel;

  //! Roughness (exponent) of Blinn's BRDF.
  Standard_ShortReal Roughness;

  //! Absorption color of transparent media.
  Graphic3d_Vec3 AbsorptionColor;

  //! Absorption intensity of transparent media.
  Standard_ShortReal AbsorptionCoeff;

public:

  //! Creates BSDF describing diffuse (Lambertian) surface.
  static Standard_EXPORT Graphic3d_BSDF CreateDiffuse (const Graphic3d_Vec3& theWeight);

  //! Creates BSDF describing polished metallic-like surface.
  static Standard_EXPORT Graphic3d_BSDF CreateMetallic (const Graphic3d_Vec3&    theWeight,
                                                        const Graphic3d_Fresnel& theFresnel,
                                                        const Standard_ShortReal theRoughness);

  //! Creates BSDF describing transparent object.
  //! Transparent BSDF models simple transparency without
  //! refraction (the ray passes straight through the surface).
  static Standard_EXPORT Graphic3d_BSDF CreateTransparent (const Graphic3d_Vec3&    theWeight,
                                                           const Graphic3d_Vec3&    theAbsorptionColor,
                                                           const Standard_ShortReal theAbsorptionCoeff);

  //! Creates BSDF describing glass-like object.
  //! Glass-like BSDF mixes refraction and reflection effects at
  //! grazing angles using physically-based Fresnel dielectric model.
  static Standard_EXPORT Graphic3d_BSDF CreateGlass (const Graphic3d_Vec3&    theWeight,
                                                     const Graphic3d_Vec3&    theAbsorptionColor,
                                                     const Standard_ShortReal theAbsorptionCoeff,
                                                     const Standard_ShortReal theRefractionIndex);

public:

  //! Creates uninitialized BSDF.
  Graphic3d_BSDF()
  {
    Roughness = AbsorptionCoeff = 0.f;
  }

  //! Normalizes BSDF components.
  Standard_EXPORT void Normalize();

  //! Performs mixing of two BSDFs.
  Graphic3d_BSDF& operator+ (const Graphic3d_BSDF& theOther)
  {
    Kd += theOther.Kd;
    Kr += theOther.Kr;
    Kt += theOther.Kt;
    Ks += theOther.Ks;
    Le += theOther.Le;

    return *this;
  }

  //! Performs comparison of two BSDFs.
  bool operator== (const Graphic3d_BSDF& theOther) const
  {
    return Kd              == theOther.Kd
        && Kr              == theOther.Kr
        && Kt              == theOther.Kt
        && Ks              == theOther.Ks
        && Le              == theOther.Le
        && Fresnel         == theOther.Fresnel
        && Roughness       == theOther.Roughness
        && AbsorptionCoeff == theOther.AbsorptionCoeff
        && AbsorptionColor == theOther.AbsorptionColor;
  }

};

#endif // _Graphic3d_BSDF_HeaderFile
