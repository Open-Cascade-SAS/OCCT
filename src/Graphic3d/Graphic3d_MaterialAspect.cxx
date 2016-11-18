// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <Graphic3d_MaterialAspect.hxx>

#include <Graphic3d_MaterialDefinitionError.hxx>
#include <Standard_Assert.hxx>
#include <Standard_OutOfRange.hxx>

namespace
{
  //! Raw material for defining list of standard materials
  struct RawMaterial
  {
    const char*              StringName;
    Graphic3d_BSDF           BSDF;
    Quantity_Color           Colors   [Graphic3d_TypeOfReflection_NB];
    Standard_ShortReal       ColorCoef[Graphic3d_TypeOfReflection_NB];
    Standard_ShortReal       TransparencyCoef;
    Standard_ShortReal       RefractionIndex;
    Standard_ShortReal       Shininess;
    Graphic3d_TypeOfMaterial MaterialType;
    Graphic3d_NameOfMaterial MaterialName;
    Standard_Boolean         ReflActivity[Graphic3d_TypeOfReflection_NB];

    RawMaterial (Graphic3d_NameOfMaterial theName, const char* theStringName);

  };

  //! Name list of standard materials (defined within enumeration).
  static const RawMaterial THE_MATERIALS[] =
  {
    RawMaterial (Graphic3d_NOM_BRASS,       "Brass"),
    RawMaterial (Graphic3d_NOM_BRONZE,      "Bronze"),
    RawMaterial (Graphic3d_NOM_COPPER,      "Copper"),
    RawMaterial (Graphic3d_NOM_GOLD,        "Gold"),
    RawMaterial (Graphic3d_NOM_PEWTER,      "Pewter"),
    RawMaterial (Graphic3d_NOM_PLASTER,     "Plastered"),
    RawMaterial (Graphic3d_NOM_PLASTIC,     "Plastified"),
    RawMaterial (Graphic3d_NOM_SILVER,      "Silver"),
    RawMaterial (Graphic3d_NOM_STEEL,       "Steel"),
    RawMaterial (Graphic3d_NOM_STONE,       "Stone"),
    RawMaterial (Graphic3d_NOM_SHINY_PLASTIC, "Shiny_plastified"),
    RawMaterial (Graphic3d_NOM_SATIN,       "Satined"),
    RawMaterial (Graphic3d_NOM_METALIZED,   "Metalized"),
    RawMaterial (Graphic3d_NOM_NEON_GNC,    "Ionized"),
    RawMaterial (Graphic3d_NOM_CHROME,      "Chrome"),
    RawMaterial (Graphic3d_NOM_ALUMINIUM,   "Aluminium"),
    RawMaterial (Graphic3d_NOM_OBSIDIAN,    "Obsidian"),
    RawMaterial (Graphic3d_NOM_NEON_PHC,    "Neon"),
    RawMaterial (Graphic3d_NOM_JADE,        "Jade"),
    RawMaterial (Graphic3d_NOM_CHARCOAL,    "Charcoal"),
    RawMaterial (Graphic3d_NOM_WATER,       "Water"),
    RawMaterial (Graphic3d_NOM_GLASS,       "Glass"),
    RawMaterial (Graphic3d_NOM_DIAMOND,     "Diamond"),
    RawMaterial (Graphic3d_NOM_TRANSPARENT, "Transparent"),
    RawMaterial (Graphic3d_NOM_DEFAULT,     "Default"),
    RawMaterial (Graphic3d_NOM_UserDefined, "UserDefined")
  };
}

// =======================================================================
// function : RawMaterial
// purpose  :
// =======================================================================
RawMaterial::RawMaterial (Graphic3d_NameOfMaterial theName, const char* theStringName)
: StringName      (theStringName),
  BSDF            (Graphic3d_BSDF::CreateDiffuse (Graphic3d_Vec3 (0.0f))),
  TransparencyCoef(0.0f),
  RefractionIndex (1.0f),
  Shininess       (0.039f),
  MaterialType    (Graphic3d_MATERIAL_ASPECT),
  MaterialName    (theName)
{
  ReflActivity[Graphic3d_TOR_AMBIENT]  = Standard_True;
  ReflActivity[Graphic3d_TOR_DIFFUSE]  = Standard_True;
  ReflActivity[Graphic3d_TOR_SPECULAR] = Standard_True;
  ReflActivity[Graphic3d_TOR_EMISSION] = Standard_False;

  ColorCoef[Graphic3d_TOR_AMBIENT]  = 0.3f;
  ColorCoef[Graphic3d_TOR_DIFFUSE]  = 0.65f;
  ColorCoef[Graphic3d_TOR_SPECULAR] = 0.0f;
  ColorCoef[Graphic3d_TOR_EMISSION] = 0.0f;

  Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.2f, 0.2f, 0.2f));
  Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.2f, 0.2f, 0.2f));
  Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (1.0f, 1.0f, 1.0f));
  switch (theName)
  {
    case Graphic3d_NOM_PLASTIC:
      Shininess                         = 0.0078125f;
      ColorCoef[Graphic3d_TOR_AMBIENT]  = 0.50f;
      ColorCoef[Graphic3d_TOR_DIFFUSE]  = 0.24f;
      ColorCoef[Graphic3d_TOR_SPECULAR] = 0.06f;

      BSDF.Kd = static_cast<Graphic3d_Vec3> (Colors[Graphic3d_TOR_DIFFUSE]);
      BSDF.Ks = Graphic3d_Vec4 (0.00784314f, 0.00784314f, 0.00784314f, 0.25f);
      BSDF.Normalize();
      break;
    case Graphic3d_NOM_SHINY_PLASTIC:
      Shininess                         = 1.00f;
      ColorCoef[Graphic3d_TOR_AMBIENT]  = 0.44f;
      ColorCoef[Graphic3d_TOR_DIFFUSE]  = 0.50f;
      ColorCoef[Graphic3d_TOR_SPECULAR] = 1.00f;

      BSDF.Kd = static_cast<Graphic3d_Vec3> (Colors[Graphic3d_TOR_DIFFUSE]);
      BSDF.Ks = Graphic3d_Vec4 (0.145f, 0.145f, 0.145f, 0.17f);
      BSDF.Normalize();
      break;
    case Graphic3d_NOM_SATIN:
      Shininess                         = 0.09375f;
      ColorCoef[Graphic3d_TOR_AMBIENT]  = 0.33f;
      ColorCoef[Graphic3d_TOR_DIFFUSE]  = 0.40f;
      ColorCoef[Graphic3d_TOR_SPECULAR] = 0.44f;

      BSDF.Kd = Graphic3d_Vec3 (0.2f);
      BSDF.Ks = Graphic3d_Vec4 (0.6f);

      break;
    case Graphic3d_NOM_NEON_GNC:
      Shininess                         = 0.05f;
      ColorCoef[Graphic3d_TOR_AMBIENT]  = 1.00f;
      ColorCoef[Graphic3d_TOR_DIFFUSE]  = 1.00f;
      ColorCoef[Graphic3d_TOR_SPECULAR] = 0.62f;
      ColorCoef[Graphic3d_TOR_EMISSION] = 1.00f;
      ReflActivity[Graphic3d_TOR_AMBIENT]  = Standard_False;
      ReflActivity[Graphic3d_TOR_EMISSION] = Standard_True;

      BSDF.Kd = Graphic3d_Vec3 (0.0f);
      BSDF.Ks = Graphic3d_Vec4 (0.5f, 0.5f, 0.5f, 0.f);
      BSDF.Le = static_cast<Graphic3d_Vec3> (Colors[Graphic3d_TOR_DIFFUSE]);
      BSDF.FresnelBase = Graphic3d_Fresnel::CreateDielectric (1.5f);
      break;
    case Graphic3d_NOM_METALIZED:
      Shininess                         = 0.13f;
      ColorCoef[Graphic3d_TOR_AMBIENT]  = 0.90f;
      ColorCoef[Graphic3d_TOR_DIFFUSE]  = 0.47f;
      ColorCoef[Graphic3d_TOR_SPECULAR] = 0.45f;
      ReflActivity[Graphic3d_TOR_AMBIENT] = Standard_False;
      {
        const Graphic3d_Vec3 aColor = (Graphic3d_Vec3 )Colors[Graphic3d_TOR_DIFFUSE];
        BSDF = Graphic3d_BSDF::CreateMetallic (Graphic3d_Vec3 (0.985f, 0.985f, 0.985f),
          Graphic3d_Fresnel::CreateSchlick (aColor), 0.045f);
      }
      break;
    // Ascending Compatibility physical materials. The same definition is taken as in the next constructor.
    case Graphic3d_NOM_BRASS:
      MaterialType = Graphic3d_MATERIAL_PHYSIC;

      Shininess                         = 0.65f;
      ColorCoef[Graphic3d_TOR_AMBIENT]  = 1.00f;
      ColorCoef[Graphic3d_TOR_DIFFUSE]  = 1.00f;
      ColorCoef[Graphic3d_TOR_SPECULAR] = 1.00f;

      BSDF = Graphic3d_BSDF::CreateMetallic (Graphic3d_Vec3 (0.985f, 0.985f, 0.985f),
        Graphic3d_Fresnel::CreateSchlick (Graphic3d_Vec3 (0.58f, 0.42f, 0.20f)), 0.045f);

      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.329f, 0.224f, 0.027f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.780f, 0.569f, 0.114f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.992f, 0.941f, 0.808f));
      break;
    case Graphic3d_NOM_BRONZE:
      MaterialType = Graphic3d_MATERIAL_PHYSIC;

      Shininess                         = 0.65f;
      ColorCoef[Graphic3d_TOR_AMBIENT]  = 1.00f;
      ColorCoef[Graphic3d_TOR_DIFFUSE]  = 1.00f;
      ColorCoef[Graphic3d_TOR_SPECULAR] = 1.00f;

      BSDF = Graphic3d_BSDF::CreateMetallic (Graphic3d_Vec3 (0.985f, 0.985f, 0.985f),
        Graphic3d_Fresnel::CreateSchlick (Graphic3d_Vec3 (0.65f, 0.35f, 0.15f)), 0.045f);

      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.213f, 0.128f, 0.054f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.714f, 0.428f, 0.181f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.590f, 0.408f, 0.250f));
      break;
    case Graphic3d_NOM_COPPER:
      MaterialType = Graphic3d_MATERIAL_PHYSIC;

      Shininess                         = 0.65f;
      ColorCoef[Graphic3d_TOR_AMBIENT]  = 1.00f;
      ColorCoef[Graphic3d_TOR_DIFFUSE]  = 1.00f;
      ColorCoef[Graphic3d_TOR_SPECULAR] = 1.00f;

      BSDF = Graphic3d_BSDF::CreateMetallic (Graphic3d_Vec3 (0.985f, 0.985f, 0.985f),
        Graphic3d_Fresnel::CreateSchlick (Graphic3d_Vec3 (0.955008f, 0.637427f, 0.538163f)), 0.045f);

      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.191f, 0.074f, 0.023f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.604f, 0.270f, 0.083f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.950f, 0.640f, 0.540f));
      break;
    case Graphic3d_NOM_GOLD:
      MaterialType = Graphic3d_MATERIAL_PHYSIC;

      Shininess                         = 0.80f;
      ColorCoef[Graphic3d_TOR_AMBIENT]  = 1.00f;
      ColorCoef[Graphic3d_TOR_DIFFUSE]  = 1.00f;
      ColorCoef[Graphic3d_TOR_SPECULAR] = 1.00f;

      BSDF = Graphic3d_BSDF::CreateMetallic (Graphic3d_Vec3 (0.985f, 0.985f, 0.985f),
        Graphic3d_Fresnel::CreateSchlick (Graphic3d_Vec3 (1.000000f, 0.765557f, 0.336057f)), 0.045f);

      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.300f, 0.230f, 0.095f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.752f, 0.580f, 0.100f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (1.000f, 0.710f, 0.290f));
      break;
    case Graphic3d_NOM_PEWTER:
      MaterialType = Graphic3d_MATERIAL_PHYSIC;

      Shininess                         = 0.50f;
      ColorCoef[Graphic3d_TOR_AMBIENT]  = 1.00f;
      ColorCoef[Graphic3d_TOR_DIFFUSE]  = 1.00f;
      ColorCoef[Graphic3d_TOR_SPECULAR] = 1.00f;

      BSDF = Graphic3d_BSDF::CreateMetallic (Graphic3d_Vec3 (0.985f, 0.985f, 0.985f),
        Graphic3d_Fresnel::CreateConductor (1.8800f, 3.4900f), 0.045f);

      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.106f, 0.059f, 0.114f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.427f, 0.471f, 0.541f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.333f, 0.333f, 0.522f));
      break;
    case Graphic3d_NOM_PLASTER:
      Shininess                         = 0.01f;
      ColorCoef[Graphic3d_TOR_AMBIENT]  = 0.26f;
      ColorCoef[Graphic3d_TOR_DIFFUSE]  = 0.75f;
      ColorCoef[Graphic3d_TOR_SPECULAR] = 0.05f;

      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.192f, 0.192f, 0.192f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.508f, 0.508f, 0.508f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.508f, 0.508f, 0.508f));

      BSDF.Kd = Graphic3d_Vec3 (0.482353f, 0.482353f, 0.482353f);

      break;
    case Graphic3d_NOM_SILVER:
      MaterialType = Graphic3d_MATERIAL_PHYSIC;

      Shininess                         = 0.75f;
      ColorCoef[Graphic3d_TOR_AMBIENT]  = 1.00f;
      ColorCoef[Graphic3d_TOR_DIFFUSE]  = 1.00f;
      ColorCoef[Graphic3d_TOR_SPECULAR] = 1.00f;

      BSDF = Graphic3d_BSDF::CreateMetallic (Graphic3d_Vec3 (0.985f, 0.985f, 0.985f),
        Graphic3d_Fresnel::CreateSchlick (Graphic3d_Vec3 (0.971519f, 0.959915f, 0.915324f)), 0.045f);

      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.275f, 0.275f, 0.250f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.630f, 0.630f, 0.630f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.950f, 0.930f, 0.880f));
      break;
    case Graphic3d_NOM_STEEL:
      MaterialType = Graphic3d_MATERIAL_PHYSIC;

      Shininess                         = 0.90f;
      ColorCoef[Graphic3d_TOR_AMBIENT]  = 1.00f;
      ColorCoef[Graphic3d_TOR_DIFFUSE]  = 1.00f;
      ColorCoef[Graphic3d_TOR_SPECULAR] = 1.00f;

      BSDF = Graphic3d_BSDF::CreateMetallic (Graphic3d_Vec3 (0.985f, 0.985f, 0.985f),
        Graphic3d_Fresnel::CreateConductor (Graphic3d_Vec3 (2.90f, 2.80f, 2.53f), Graphic3d_Vec3 (3.08f, 2.90f, 2.74f)), 0.045f);

      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.150f, 0.150f, 0.180f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.500f, 0.510f, 0.520f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.560f, 0.570f, 0.580f));
      break;
    case Graphic3d_NOM_STONE:
      MaterialType = Graphic3d_MATERIAL_PHYSIC;

      Shininess                         = 0.17f;
      ColorCoef[Graphic3d_TOR_AMBIENT]  = 0.19f;
      ColorCoef[Graphic3d_TOR_DIFFUSE]  = 0.75f;
      ColorCoef[Graphic3d_TOR_SPECULAR] = 0.08f;

      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (1.00f, 0.8f, 0.62f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (1.00f, 0.8f, 0.62f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.98f, 1.0f, 0.60f));

      BSDF.Kd = Graphic3d_Vec3 (0.243137f, 0.243137f, 0.243137f);
      BSDF.Ks = Graphic3d_Vec4 (0.00392157f, 0.00392157f, 0.00392157f, 0.5f);

      break;
    // Ascending Compatibility of physical materials. Takes the same definition as in the next constructor. New materials
    case Graphic3d_NOM_CHROME:
      MaterialType = Graphic3d_MATERIAL_PHYSIC;

      Shininess                         = 0.90f;
      ColorCoef[Graphic3d_TOR_AMBIENT]  = 1.00f;
      ColorCoef[Graphic3d_TOR_DIFFUSE]  = 1.00f;
      ColorCoef[Graphic3d_TOR_SPECULAR] = 1.00f;

      BSDF = Graphic3d_BSDF::CreateMetallic (Graphic3d_Vec3 (0.985f, 0.985f, 0.985f),
        Graphic3d_Fresnel::CreateSchlick (Graphic3d_Vec3 (0.549585f, 0.556114f, 0.554256f)), 0.045f);

      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.200f, 0.200f, 0.225f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.550f, 0.550f, 0.550f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.975f, 0.975f, 0.975f));
      break;
    case Graphic3d_NOM_ALUMINIUM:
      MaterialType = Graphic3d_MATERIAL_PHYSIC;

      Shininess                         = 0.75f;
      ColorCoef[Graphic3d_TOR_AMBIENT]  = 1.00f;
      ColorCoef[Graphic3d_TOR_DIFFUSE]  = 1.00f;
      ColorCoef[Graphic3d_TOR_SPECULAR] = 1.00f;

      BSDF = Graphic3d_BSDF::CreateMetallic (Graphic3d_Vec3 (0.985f, 0.985f, 0.985f),
        Graphic3d_Fresnel::CreateSchlick (Graphic3d_Vec3 (0.913183f, 0.921494f, 0.924524f)), 0.045f);

      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.300f, 0.300f, 0.300f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.600f, 0.600f, 0.600f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.910f, 0.920f, 0.920f));
      break;
    case Graphic3d_NOM_NEON_PHC:
      MaterialType = Graphic3d_MATERIAL_PHYSIC;

      Shininess                         = 0.05f;
      ColorCoef[Graphic3d_TOR_AMBIENT]  = 1.00f;
      ColorCoef[Graphic3d_TOR_DIFFUSE]  = 1.00f;
      ColorCoef[Graphic3d_TOR_SPECULAR] = 0.62f;
      ColorCoef[Graphic3d_TOR_EMISSION] = 0.90f;
      ReflActivity[Graphic3d_TOR_AMBIENT]  = Standard_False;
      ReflActivity[Graphic3d_TOR_DIFFUSE]  = Standard_False;
      ReflActivity[Graphic3d_TOR_EMISSION] = Standard_True;

      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (1.0f, 1.0f, 1.0f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (1.0f, 1.0f, 1.0f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (1.0f, 1.0f, 1.0f));
      Colors[Graphic3d_TOR_EMISSION] = Quantity_Color (Graphic3d_Vec3 (0.0f, 1.0f, 0.46f));

      BSDF.Kd = Graphic3d_Vec3 (0.0f);
      BSDF.Ks = Graphic3d_Vec4 (0.5f, 0.5f, 0.5f, 0.f);
      BSDF.Le = Graphic3d_Vec3 (0.0f, 1.0f, 0.46f);
      BSDF.FresnelBase = Graphic3d_Fresnel::CreateDielectric (1.5f);
      break;
    case Graphic3d_NOM_OBSIDIAN:
      MaterialType = Graphic3d_MATERIAL_PHYSIC;

      Shininess                         = 0.3f;
      ColorCoef[Graphic3d_TOR_AMBIENT]  = 1.0f;
      ColorCoef[Graphic3d_TOR_DIFFUSE]  = 1.0f;
      ColorCoef[Graphic3d_TOR_SPECULAR] = 1.0f;

      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.054f, 0.050f, 0.066f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.183f, 0.170f, 0.225f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.333f, 0.329f, 0.346f));

      BSDF.Kd = Graphic3d_Vec3 (0.023f, 0.f, 0.023f);
      BSDF.Ks = Graphic3d_Vec4 (0.0156863f, 0.0156863f, 0.0156863f, 0.1f);
      break;
    case Graphic3d_NOM_JADE:
      MaterialType = Graphic3d_MATERIAL_PHYSIC;

      Shininess                         = 0.10f;
      ColorCoef[Graphic3d_TOR_AMBIENT]  = 1.00f;
      ColorCoef[Graphic3d_TOR_DIFFUSE]  = 1.00f;
      ColorCoef[Graphic3d_TOR_SPECULAR] = 1.00f;

      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.135f, 0.223f, 0.158f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.540f, 0.890f, 0.630f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.316f, 0.316f, 0.316f));

      BSDF.FresnelBase = Graphic3d_Fresnel::CreateDielectric (1.5f);
      BSDF.Kd = Graphic3d_Vec3 (0.208658f, 0.415686f, 0.218401f);
      BSDF.Ks = Graphic3d_Vec4 (0.611765f, 0.611765f, 0.611765f, 0.06f);
      break;
    case Graphic3d_NOM_CHARCOAL:
      MaterialType = Graphic3d_MATERIAL_PHYSIC;

      Shininess                         = 0.01f;
      ColorCoef[Graphic3d_TOR_AMBIENT]  = 1.00f;
      ColorCoef[Graphic3d_TOR_DIFFUSE]  = 1.00f;
      ColorCoef[Graphic3d_TOR_SPECULAR] = 1.00f;

      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.050f, 0.050f, 0.050f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.150f, 0.150f, 0.150f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.000f, 0.000f, 0.000f));

      BSDF.Kd = Graphic3d_Vec3 (0.02f, 0.02f, 0.02f);
      BSDF.Ks = Graphic3d_Vec4 (0.1f, 0.1f, 0.1f, 0.3f);
      break;
    case Graphic3d_NOM_WATER:
      MaterialType = Graphic3d_MATERIAL_PHYSIC;

      Shininess                         = 0.90f;
      ColorCoef[Graphic3d_TOR_AMBIENT]  = 1.00f;
      ColorCoef[Graphic3d_TOR_DIFFUSE]  = 1.00f;
      ColorCoef[Graphic3d_TOR_SPECULAR] = 1.00f;
      RefractionIndex  = 1.33f;
      BSDF             = Graphic3d_BSDF::CreateGlass (Graphic3d_Vec3 (1.f),
                                                      Graphic3d_Vec3 (0.7f, 0.75f, 0.85f),
                                                      0.05f,
                                                      RefractionIndex);
      TransparencyCoef = 0.80f;

      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.450f, 0.450f, 0.475f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.050f, 0.050f, 0.075f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.380f, 0.380f, 0.380f));
      break;
    case Graphic3d_NOM_GLASS:
      MaterialType = Graphic3d_MATERIAL_PHYSIC;

      Shininess                         = 0.50f;
      ColorCoef[Graphic3d_TOR_AMBIENT]  = 1.00f;
      ColorCoef[Graphic3d_TOR_DIFFUSE]  = 1.00f;
      ColorCoef[Graphic3d_TOR_SPECULAR] = 1.00f;
      RefractionIndex  = 1.62f;
      BSDF             = Graphic3d_BSDF::CreateGlass (Graphic3d_Vec3 (1.f),
                                                      Graphic3d_Vec3 (0.75f, 0.95f, 0.9f),
                                                      0.05f,
                                                      RefractionIndex);
      TransparencyCoef = 0.80f;

      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.550f, 0.575f, 0.575f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.050f, 0.075f, 0.075f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.920f, 0.920f, 0.920f));
      break;
    case Graphic3d_NOM_DIAMOND:
      MaterialType = Graphic3d_MATERIAL_PHYSIC;

      Shininess                         = 0.90f;
      ColorCoef[Graphic3d_TOR_AMBIENT]  = 1.00f;
      ColorCoef[Graphic3d_TOR_DIFFUSE]  = 1.00f;
      ColorCoef[Graphic3d_TOR_SPECULAR] = 1.00f;
      RefractionIndex  = 2.42f;
      BSDF             = Graphic3d_BSDF::CreateGlass (Graphic3d_Vec3 (1.f),
                                                      Graphic3d_Vec3 (0.95f, 0.95f, 0.95f),
                                                      0.05f,
                                                      RefractionIndex);
      TransparencyCoef = 0.80f;

      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.550f, 0.550f, 0.550f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.100f, 0.100f, 0.100f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.970f, 0.970f, 0.970f));
      break;

    case Graphic3d_NOM_TRANSPARENT:
      MaterialType = Graphic3d_MATERIAL_PHYSIC;

      Shininess                         = 0.90f;
      ColorCoef[Graphic3d_TOR_AMBIENT]  = 1.00f;
      ColorCoef[Graphic3d_TOR_DIFFUSE]  = 1.00f;
      ColorCoef[Graphic3d_TOR_SPECULAR] = 1.00f;
      RefractionIndex = 1.0f;

      BSDF.Kd = Graphic3d_Vec3 (0.1f);
      BSDF.Kt = Graphic3d_Vec3 (0.9f);
      BSDF.FresnelBase = Graphic3d_Fresnel::CreateConstant (0.0f);
      TransparencyCoef = 0.80f;

      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.550f, 0.550f, 0.550f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.100f, 0.100f, 0.100f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.970f, 0.970f, 0.970f));
      break;
    case Graphic3d_NOM_UserDefined:
      break;
    case Graphic3d_NOM_DEFAULT:
      break;
  }
}

// =======================================================================
// function : Graphic3d_MaterialAspect
// purpose  :
// =======================================================================
Graphic3d_MaterialAspect::Graphic3d_MaterialAspect()
: myRequestedMaterialName (Graphic3d_NOM_DEFAULT)
{
  init (Graphic3d_NOM_DEFAULT);
}

// =======================================================================
// function : Graphic3d_MaterialAspect
// purpose  :
// =======================================================================
Graphic3d_MaterialAspect::Graphic3d_MaterialAspect (const Graphic3d_NameOfMaterial theName)
: myRequestedMaterialName (theName)
{
  init (theName);
}

// =======================================================================
// function : init
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::init (const Graphic3d_NameOfMaterial theName)
{
  const RawMaterial& aMat = THE_MATERIALS[theName];
  myBSDF       = aMat.BSDF;
  myStringName = aMat.StringName;
  myColors[Graphic3d_TOR_AMBIENT]     = aMat.Colors[Graphic3d_TOR_AMBIENT];
  myColors[Graphic3d_TOR_DIFFUSE]     = aMat.Colors[Graphic3d_TOR_DIFFUSE];
  myColors[Graphic3d_TOR_SPECULAR]    = aMat.Colors[Graphic3d_TOR_SPECULAR];
  myColors[Graphic3d_TOR_EMISSION]    = aMat.Colors[Graphic3d_TOR_EMISSION];
  myColorCoef[Graphic3d_TOR_AMBIENT]  = aMat.ColorCoef[Graphic3d_TOR_AMBIENT];
  myColorCoef[Graphic3d_TOR_DIFFUSE]  = aMat.ColorCoef[Graphic3d_TOR_DIFFUSE];
  myColorCoef[Graphic3d_TOR_SPECULAR] = aMat.ColorCoef[Graphic3d_TOR_SPECULAR];
  myColorCoef[Graphic3d_TOR_EMISSION] = aMat.ColorCoef[Graphic3d_TOR_EMISSION];
  myTransparencyCoef = aMat.TransparencyCoef;
  myRefractionIndex  = aMat.RefractionIndex;
  myShininess        = aMat.Shininess;
  myMaterialType     = aMat.MaterialType;
  myMaterialName     = theName;
  myRequestedMaterialName = theName;
  myReflActivity[Graphic3d_TOR_AMBIENT]  = aMat.ReflActivity[Graphic3d_TOR_AMBIENT];
  myReflActivity[Graphic3d_TOR_DIFFUSE]  = aMat.ReflActivity[Graphic3d_TOR_DIFFUSE];
  myReflActivity[Graphic3d_TOR_SPECULAR] = aMat.ReflActivity[Graphic3d_TOR_SPECULAR];
  myReflActivity[Graphic3d_TOR_EMISSION] = aMat.ReflActivity[Graphic3d_TOR_EMISSION];
}

// =======================================================================
// function : IncreaseShine
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::IncreaseShine (const Standard_ShortReal theDelta)
{
  const Standard_ShortReal anOldShine = myShininess;
  myShininess = myShininess + myShininess * theDelta / 100.0f;
  if (myShininess > 1.0f || myShininess < 0.0f)
  {
    myShininess = anOldShine;
  }
}

// =======================================================================
// function : SetMaterialType
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::SetMaterialType (const Graphic3d_TypeOfMaterial theType)
{
  myMaterialType = theType;
  if (theType != myMaterialType)
  {
    setUserMaterial();
  }
}

// =======================================================================
// function : SetAmbient
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::SetAmbient (const Standard_ShortReal theValue)
{
  if (theValue < 0.0f
   || theValue > 1.0f)
  {
    throw Graphic3d_MaterialDefinitionError("Bad value for SetAmbient < 0. or > 1.0");
  }

  myColorCoef[Graphic3d_TOR_AMBIENT] = theValue;
  if (myReflActivity[Graphic3d_TOR_AMBIENT]
   && myMaterialType == Graphic3d_MATERIAL_PHYSIC)
  {
    setUserMaterial();
  }
}

// =======================================================================
// function : SetColor
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::SetColor (const Quantity_Color& theColor)
{
  const Standard_ShortReal anAmbientCoeff = 0.25f;
  myColors[Graphic3d_TOR_AMBIENT].SetValues (theColor.Red()   * anAmbientCoeff,
                                             theColor.Green() * anAmbientCoeff,
                                             theColor.Blue()  * anAmbientCoeff, Quantity_TOC_RGB);
  myColors[Graphic3d_TOR_DIFFUSE] =  theColor;
  if (myReflActivity[Graphic3d_TOR_AMBIENT]
   && myMaterialType == Graphic3d_MATERIAL_PHYSIC)
  {
    setUserMaterial();
  }
}

// =======================================================================
// function : SetAmbientColor
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::SetAmbientColor (const Quantity_Color& theColor)
{
  myColors[Graphic3d_TOR_AMBIENT] = theColor;
  if (myReflActivity[Graphic3d_TOR_AMBIENT]
   && myMaterialType == Graphic3d_MATERIAL_PHYSIC)
  {
    setUserMaterial();
  }
}

// =======================================================================
// function : SetDiffuseColor
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::SetDiffuseColor (const Quantity_Color& theColor)
{
  myColors[Graphic3d_TOR_DIFFUSE] = theColor;
  if (myReflActivity[Graphic3d_TOR_DIFFUSE]
   && myMaterialType == Graphic3d_MATERIAL_PHYSIC)
  {
    setUserMaterial();
  }
}

// =======================================================================
// function : SetSpecularColor
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::SetSpecularColor (const Quantity_Color& theColor)
{
  myColors[Graphic3d_TOR_SPECULAR] = theColor;
  if (myReflActivity[Graphic3d_TOR_SPECULAR]
   && myMaterialType == Graphic3d_MATERIAL_PHYSIC)
  {
    setUserMaterial();
  }
}

// =======================================================================
// function : SetEmissiveColor
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::SetEmissiveColor (const Quantity_Color& theColor)
{
  myColors[Graphic3d_TOR_EMISSION] = theColor;
  if (myReflActivity[Graphic3d_TOR_EMISSION]
   && myMaterialType == Graphic3d_MATERIAL_PHYSIC)
  {
    setUserMaterial();
  }
}

// =======================================================================
// function : SetDiffuse
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::SetDiffuse (const Standard_ShortReal theValue)
{
  if (theValue < 0.0f
   || theValue > 1.0f)
  {
    throw Graphic3d_MaterialDefinitionError("Bad value for SetDiffuse < 0. or > 1.0");
  }

  myColorCoef[Graphic3d_TOR_DIFFUSE] = theValue;
  if (myReflActivity[Graphic3d_TOR_DIFFUSE]
   && myMaterialType == Graphic3d_MATERIAL_PHYSIC)
  {
    setUserMaterial();
  }
}

// =======================================================================
// function : SetEmissive
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::SetEmissive (const Standard_ShortReal theValue)
{
  if (theValue < 0.0f
   || theValue > 1.0f)
  {
    throw Graphic3d_MaterialDefinitionError("Bad value for SetEmissive < 0. or > 1.0");
  }

  myColorCoef[Graphic3d_TOR_EMISSION] = theValue;
  if (myReflActivity[Graphic3d_TOR_DIFFUSE]
   && myMaterialType == Graphic3d_MATERIAL_PHYSIC)
  {
    setUserMaterial();
  }
}

// =======================================================================
// function : SetReflectionMode
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::SetReflectionMode (const Graphic3d_TypeOfReflection theType,
                                                  const Standard_Boolean theValue)
{
  myReflActivity[theType] = theValue;
  if (myMaterialType == Graphic3d_MATERIAL_PHYSIC)
  {
    setUserMaterial();
  }
}

// =======================================================================
// function : SetSpecular
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::SetSpecular (const Standard_ShortReal theValue)
{
  if (theValue < 0.0f
   || theValue > 1.0f)
  {
    throw Graphic3d_MaterialDefinitionError("Bad value for SetSpecular < 0. or > 1.0");
  }

  myColorCoef[Graphic3d_TOR_SPECULAR] = theValue;
  if (myReflActivity[Graphic3d_TOR_SPECULAR]
   && myMaterialType == Graphic3d_MATERIAL_PHYSIC)
  {
    setUserMaterial();
  }
}

// =======================================================================
// function : SetTransparency
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::SetTransparency (const Standard_ShortReal theValue)
{
  if (theValue < 0.0f
   || theValue > 1.0f)
  {
    throw Graphic3d_MaterialDefinitionError("Bad value for SetTransparency < 0. or > 1.0");
  }

  myTransparencyCoef = theValue;
}

// =======================================================================
// function : SetRefractionIndex
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::SetRefractionIndex (const Standard_ShortReal theValue)
{
  if (theValue < 1.0f)
  {
    throw Graphic3d_MaterialDefinitionError("Bad value for refraction index < 1.0");
  }

  myRefractionIndex = theValue;
}

// =======================================================================
// function : SetShininess
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::SetShininess (const Standard_ShortReal theValue)
{
  if (theValue < 0.0f
   || theValue > 1.0f)
  {
    throw Graphic3d_MaterialDefinitionError("Bad value for Shininess < 0. or > 1.0");
  }

  myShininess = theValue;
  setUserMaterial();
}

// =======================================================================
// function : MaterialName
// purpose  :
// =======================================================================
Standard_CString Graphic3d_MaterialAspect::MaterialName (const Standard_Integer theRank)
{
  if (theRank < 1 || theRank > NumberOfMaterials())
  {
    throw Standard_OutOfRange("BAD index of material");
  }
  const RawMaterial& aMat = THE_MATERIALS[theRank - 1];
  return aMat.StringName;
}

// =======================================================================
// function : MaterialFromName
// purpose  :
// =======================================================================
Graphic3d_NameOfMaterial Graphic3d_MaterialAspect::MaterialFromName (const Standard_CString theName)
{
  TCollection_AsciiString aName (theName);
  aName.LowerCase();
  aName.Capitalize();
  const Standard_Integer aNbMaterials = Graphic3d_MaterialAspect::NumberOfMaterials();
  for (Standard_Integer aMatIter = 0; aMatIter < aNbMaterials; ++aMatIter)
  {
    const RawMaterial& aMat = THE_MATERIALS[aMatIter];
    if (aName == aMat.StringName)
    {
      return Graphic3d_NameOfMaterial(aMatIter);
    }
  }

  // parse aliases
  if (aName == "Plastic")            // Plastified
  {
    return Graphic3d_NOM_PLASTIC;
  }
  else if (aName == "Shiny_plastic") // Shiny_plastified
  {
    return Graphic3d_NOM_SHINY_PLASTIC;
  }
  else if (aName == "Plaster")       // Plastered
  {
    return Graphic3d_NOM_PLASTER;
  }
  else if (aName == "Satin")         // Satined
  {
    return Graphic3d_NOM_SATIN;
  }
  else if (aName == "Neon_gnc")      // Ionized
  {
    return Graphic3d_NOM_NEON_GNC;
  }
  else if (aName == "Neon_phc") // Neon
  {
    return Graphic3d_NOM_NEON_PHC;
  }
  return Graphic3d_NOM_DEFAULT;
}

// =======================================================================
// function : MaterialType
// purpose  :
// =======================================================================
Graphic3d_TypeOfMaterial Graphic3d_MaterialAspect::MaterialType (const Standard_Integer theRank)
{
  if (theRank < 1 || theRank > NumberOfMaterials())
  {
    throw Standard_OutOfRange("BAD index of material");
  }
  const RawMaterial& aMat = THE_MATERIALS[theRank - 1];
  return aMat.MaterialType;
}
