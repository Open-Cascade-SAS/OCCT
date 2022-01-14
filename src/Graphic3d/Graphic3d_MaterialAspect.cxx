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
#include <Standard_OutOfRange.hxx>

namespace
{
  //! Raw material for defining list of standard materials
  struct RawMaterial
  {
    const char*              StringName;
    Graphic3d_BSDF           BSDF;
    Graphic3d_PBRMaterial    PBRMaterial;
    Quantity_Color           Colors[Graphic3d_TypeOfReflection_NB];
    Standard_ShortReal       TransparencyCoef;
    Standard_ShortReal       RefractionIndex;
    Standard_ShortReal       Shininess;
    Standard_ShortReal       AmbientCoef;  //!< coefficient for Graphic3d_MaterialAspect::SetColor()
    Standard_ShortReal       DiffuseCoef;  //!< coefficient for Graphic3d_MaterialAspect::SetColor()
    Graphic3d_TypeOfMaterial MaterialType;
    Graphic3d_NameOfMaterial MaterialName;

    RawMaterial (Graphic3d_NameOfMaterial theName, const char* theStringName);

  };

  //! Name list of standard materials (defined within enumeration).
  static const RawMaterial THE_MATERIALS[] =
  {
    RawMaterial (Graphic3d_NameOfMaterial_Brass,       "Brass"),
    RawMaterial (Graphic3d_NameOfMaterial_Bronze,      "Bronze"),
    RawMaterial (Graphic3d_NameOfMaterial_Copper,      "Copper"),
    RawMaterial (Graphic3d_NameOfMaterial_Gold,        "Gold"),
    RawMaterial (Graphic3d_NameOfMaterial_Pewter,      "Pewter"),
    RawMaterial (Graphic3d_NameOfMaterial_Plastered,   "Plastered"),
    RawMaterial (Graphic3d_NameOfMaterial_Plastified,  "Plastified"),
    RawMaterial (Graphic3d_NameOfMaterial_Silver,      "Silver"),
    RawMaterial (Graphic3d_NameOfMaterial_Steel,       "Steel"),
    RawMaterial (Graphic3d_NameOfMaterial_Stone,       "Stone"),
    RawMaterial (Graphic3d_NameOfMaterial_ShinyPlastified, "Shiny_plastified"),
    RawMaterial (Graphic3d_NameOfMaterial_Satin,       "Satined"),
    RawMaterial (Graphic3d_NameOfMaterial_Metalized,   "Metalized"),
    RawMaterial (Graphic3d_NameOfMaterial_Ionized,     "Ionized"),
    RawMaterial (Graphic3d_NameOfMaterial_Chrome,      "Chrome"),
    RawMaterial (Graphic3d_NameOfMaterial_Aluminum,    "Aluminium"),
    RawMaterial (Graphic3d_NameOfMaterial_Obsidian,    "Obsidian"),
    RawMaterial (Graphic3d_NameOfMaterial_Neon,        "Neon"),
    RawMaterial (Graphic3d_NameOfMaterial_Jade,        "Jade"),
    RawMaterial (Graphic3d_NameOfMaterial_Charcoal,    "Charcoal"),
    RawMaterial (Graphic3d_NameOfMaterial_Water,       "Water"),
    RawMaterial (Graphic3d_NameOfMaterial_Glass,       "Glass"),
    RawMaterial (Graphic3d_NameOfMaterial_Diamond,     "Diamond"),
    RawMaterial (Graphic3d_NameOfMaterial_Transparent, "Transparent"),
    RawMaterial (Graphic3d_NameOfMaterial_DEFAULT,     "Default"),
    RawMaterial (Graphic3d_NameOfMaterial_UserDefined, "UserDefined")
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
  AmbientCoef     (0.25f),
  DiffuseCoef     (1.0f),
  MaterialType    (Graphic3d_MATERIAL_ASPECT),
  MaterialName    (theName)
{
  switch (theName)
  {
    case Graphic3d_NameOfMaterial_Plastified:
      MaterialType = Graphic3d_MATERIAL_ASPECT;

      Shininess = 0.0078125f;
      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.25f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.24f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.004896f));
      Colors[Graphic3d_TOR_EMISSION] = Quantity_Color (Graphic3d_Vec3 (0.0f));

      BSDF.Kd = Graphic3d_Vec3 (0.2f);
      BSDF.Ks = Graphic3d_Vec4 (0.00784314f, 0.00784314f, 0.00784314f, 0.25f);
      BSDF.Normalize();

      break;
    case Graphic3d_NameOfMaterial_ShinyPlastified:
      MaterialType = Graphic3d_MATERIAL_ASPECT;

      Shininess = 1.00f;
      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.22f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.50f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (1.0f));
      Colors[Graphic3d_TOR_EMISSION] = Quantity_Color (Graphic3d_Vec3 (0.0f));

      BSDF.Kd = Graphic3d_Vec3 (0.2f);
      BSDF.Ks = Graphic3d_Vec4 (0.145f, 0.145f, 0.145f, 0.17f);
      BSDF.Normalize();
      break;
    case Graphic3d_NameOfMaterial_Satin:
      MaterialType = Graphic3d_MATERIAL_ASPECT;

      Shininess = 0.09375f;
      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.165f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.40f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.162647f));
      Colors[Graphic3d_TOR_EMISSION] = Quantity_Color (Graphic3d_Vec3 (0.0f));

      BSDF.Kd = Graphic3d_Vec3 (0.2f);
      BSDF.Ks = Graphic3d_Vec4 (0.6f);

      break;
    case Graphic3d_NameOfMaterial_Ionized:
      MaterialType = Graphic3d_MATERIAL_ASPECT;

      Shininess = 0.05f;
      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.0f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (1.0f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.342392f));
      Colors[Graphic3d_TOR_EMISSION] = Quantity_Color (Graphic3d_Vec3 (1.0f));

      BSDF.Kd = Graphic3d_Vec3 (0.0f);
      BSDF.Ks = Graphic3d_Vec4 (0.5f, 0.5f, 0.5f, 0.f);
      BSDF.Le = static_cast<Graphic3d_Vec3> (Colors[Graphic3d_TOR_DIFFUSE]);
      BSDF.FresnelBase = Graphic3d_Fresnel::CreateDielectric (1.5f);
      break;
    case Graphic3d_NameOfMaterial_Metalized:
      MaterialType = Graphic3d_MATERIAL_ASPECT;

      Shininess = 0.13f;
      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.0f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.47f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.170645f));
      Colors[Graphic3d_TOR_EMISSION] = Quantity_Color (Graphic3d_Vec3 (0.0f));

      BSDF = Graphic3d_BSDF::CreateMetallic (Graphic3d_Vec3 (0.985f, 0.985f, 0.985f),
                                             Graphic3d_Fresnel::CreateSchlick (Graphic3d_Vec3 (0.2f)), 0.045f);
      break;
    case Graphic3d_NameOfMaterial_Brass:
      MaterialType = Graphic3d_MATERIAL_PHYSIC;

      BSDF = Graphic3d_BSDF::CreateMetallic (Graphic3d_Vec3 (0.985f, 0.985f, 0.985f),
        Graphic3d_Fresnel::CreateSchlick (Graphic3d_Vec3 (0.58f, 0.42f, 0.20f)), 0.045f);

      Shininess = 0.65f;
      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.088428f, 0.041081f, 0.002090f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.570482f, 0.283555f, 0.012335f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.992f, 0.941f, 0.808f));
      Colors[Graphic3d_TOR_EMISSION] = Quantity_Color (Graphic3d_Vec3 (0.0f));
      break;
    case Graphic3d_NameOfMaterial_Bronze:
      MaterialType = Graphic3d_MATERIAL_PHYSIC;

      BSDF = Graphic3d_BSDF::CreateMetallic (Graphic3d_Vec3 (0.985f, 0.985f, 0.985f),
        Graphic3d_Fresnel::CreateSchlick (Graphic3d_Vec3 (0.65f, 0.35f, 0.15f)), 0.045f);

      Shininess = 0.65f;
      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.037301f, 0.014931f, 0.004305f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.468185f, 0.153344f, 0.027491f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.590f, 0.408f, 0.250f));
      Colors[Graphic3d_TOR_EMISSION] = Quantity_Color (Graphic3d_Vec3 (0.0f));
      break;
    case Graphic3d_NameOfMaterial_Copper:
      MaterialType = Graphic3d_MATERIAL_PHYSIC;

      BSDF = Graphic3d_BSDF::CreateMetallic (Graphic3d_Vec3 (0.985f, 0.985f, 0.985f),
        Graphic3d_Fresnel::CreateSchlick (Graphic3d_Vec3 (0.955008f, 0.637427f, 0.538163f)), 0.045f);

      Shininess = 0.65f;
      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.030370f, 0.006451f, 0.001780f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.323236f, 0.059254f, 0.007584f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.950f, 0.640f, 0.540f));
      Colors[Graphic3d_TOR_EMISSION] = Quantity_Color (Graphic3d_Vec3 (0.0f));
      break;
    case Graphic3d_NameOfMaterial_Gold:
      MaterialType = Graphic3d_MATERIAL_PHYSIC;

      BSDF = Graphic3d_BSDF::CreateMetallic (Graphic3d_Vec3 (0.985f, 0.985f, 0.985f),
        Graphic3d_Fresnel::CreateSchlick (Graphic3d_Vec3 (1.000000f, 0.765557f, 0.336057f)), 0.045f);

      Shininess = 0.80f;
      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.073239f, 0.043234f, 0.009264f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.525643f, 0.295700f, 0.010023f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (1.000f, 0.710f, 0.290f));
      Colors[Graphic3d_TOR_EMISSION] = Quantity_Color (Graphic3d_Vec3 (0.0f));
      break;
    case Graphic3d_NameOfMaterial_Pewter:
      MaterialType = Graphic3d_MATERIAL_PHYSIC;

      BSDF = Graphic3d_BSDF::CreateMetallic (Graphic3d_Vec3 (0.985f, 0.985f, 0.985f),
        Graphic3d_Fresnel::CreateConductor (1.8800f, 3.4900f), 0.045f);

      Shininess = 0.50f;
      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.010979f, 0.004795f, 0.012335f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.152583f, 0.188174f, 0.253972f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.333f, 0.333f, 0.522f));
      Colors[Graphic3d_TOR_EMISSION] = Quantity_Color (Graphic3d_Vec3 (0.0f));
      break;
    case Graphic3d_NameOfMaterial_Plastered:
      MaterialType = Graphic3d_MATERIAL_ASPECT;

      Shininess = 0.01f;
      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.13f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.75f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.003936f));
      Colors[Graphic3d_TOR_EMISSION] = Quantity_Color (Graphic3d_Vec3 (0.0f));

      BSDF.Kd = Graphic3d_Vec3 (0.482353f, 0.482353f, 0.482353f);

      break;
    case Graphic3d_NameOfMaterial_Silver:
      MaterialType = Graphic3d_MATERIAL_PHYSIC;

      BSDF = Graphic3d_BSDF::CreateMetallic (Graphic3d_Vec3 (0.985f, 0.985f, 0.985f),
        Graphic3d_Fresnel::CreateSchlick (Graphic3d_Vec3 (0.971519f, 0.959915f, 0.915324f)), 0.045f);

      Shininess = 0.75f;
      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.061465f, 0.061465f, 0.050876f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.354692f, 0.354692f, 0.354692f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.950f, 0.930f, 0.880f));
      Colors[Graphic3d_TOR_EMISSION] = Quantity_Color (Graphic3d_Vec3 (0.0f));
      break;
    case Graphic3d_NameOfMaterial_Steel:
      MaterialType = Graphic3d_MATERIAL_PHYSIC;

      BSDF = Graphic3d_BSDF::CreateMetallic (Graphic3d_Vec3 (0.985f, 0.985f, 0.985f),
        Graphic3d_Fresnel::CreateConductor (Graphic3d_Vec3 (2.90f, 2.80f, 2.53f), Graphic3d_Vec3 (3.08f, 2.90f, 2.74f)), 0.045f);

      Shininess = 0.90f;
      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.019607f, 0.019607f, 0.027212f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.214041f, 0.223414f, 0.233022f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.560f, 0.570f, 0.580f));
      Colors[Graphic3d_TOR_EMISSION] = Quantity_Color (Graphic3d_Vec3 (0.0f));
      break;
    case Graphic3d_NameOfMaterial_Stone:
      MaterialType = Graphic3d_MATERIAL_PHYSIC;

      // special case for SetColor()
      AmbientCoef = 0.19f * 0.25f;
      DiffuseCoef = 0.75f;

      Shininess = 0.17f;
      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.030074f, 0.020069f, 0.013011f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.522522f, 0.318547f, 0.183064f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.98f, 1.0f, 0.60f) * 0.08f);
      Colors[Graphic3d_TOR_EMISSION] = Quantity_Color (Graphic3d_Vec3 (0.0f));

      BSDF.Kd = Graphic3d_Vec3 (0.243137f, 0.243137f, 0.243137f);
      BSDF.Ks = Graphic3d_Vec4 (0.00392157f, 0.00392157f, 0.00392157f, 0.5f);

      break;
    case Graphic3d_NameOfMaterial_Chrome:
      MaterialType = Graphic3d_MATERIAL_PHYSIC;

      BSDF = Graphic3d_BSDF::CreateMetallic (Graphic3d_Vec3 (0.985f, 0.985f, 0.985f),
        Graphic3d_Fresnel::CreateSchlick (Graphic3d_Vec3 (0.549585f, 0.556114f, 0.554256f)), 0.045f);

      Shininess = 0.90f;
      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.033105f, 0.033105f, 0.041436f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.263273f, 0.263273f, 0.263273f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.975f, 0.975f, 0.975f));
      Colors[Graphic3d_TOR_EMISSION] = Quantity_Color (Graphic3d_Vec3 (0.0f));
      break;
    case Graphic3d_NameOfMaterial_Aluminum:
      MaterialType = Graphic3d_MATERIAL_PHYSIC;

      BSDF = Graphic3d_BSDF::CreateMetallic (Graphic3d_Vec3 (0.985f, 0.985f, 0.985f),
        Graphic3d_Fresnel::CreateSchlick (Graphic3d_Vec3 (0.913183f, 0.921494f, 0.924524f)), 0.045f);

      Shininess = 0.75f;
      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.073239f, 0.073239f, 0.073239f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.318547f, 0.318547f, 0.318547f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.910f, 0.920f, 0.920f));
      Colors[Graphic3d_TOR_EMISSION] = Quantity_Color (Graphic3d_Vec3 (0.0f));
      break;
    case Graphic3d_NameOfMaterial_Neon:
      MaterialType = Graphic3d_MATERIAL_PHYSIC;

      Shininess = 0.05f;
      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.0f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.0f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.62f));
      Colors[Graphic3d_TOR_EMISSION] = Quantity_Color (Graphic3d_Vec3 (0.0f, 0.787412f, 0.142892f));

      BSDF.Kd = Graphic3d_Vec3 (0.0f);
      BSDF.Ks = Graphic3d_Vec4 (0.5f, 0.5f, 0.5f, 0.f);
      BSDF.Le = Graphic3d_Vec3 (0.0f, 1.0f, 0.46f);
      BSDF.FresnelBase = Graphic3d_Fresnel::CreateDielectric (1.5f);
      break;
    case Graphic3d_NameOfMaterial_Obsidian:
      MaterialType = Graphic3d_MATERIAL_PHYSIC;

      Shininess = 0.3f;
      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.004305f, 0.003936f, 0.005532f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.028053f, 0.024515f, 0.041436f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.333f, 0.329f, 0.346f));
      Colors[Graphic3d_TOR_EMISSION] = Quantity_Color (Graphic3d_Vec3 (0.0f));

      BSDF.Kd = Graphic3d_Vec3 (0.023f, 0.f, 0.023f);
      BSDF.Ks = Graphic3d_Vec4 (0.0156863f, 0.0156863f, 0.0156863f, 0.1f);
      break;
    case Graphic3d_NameOfMaterial_Jade:
      MaterialType = Graphic3d_MATERIAL_PHYSIC;

      Shininess = 0.10f;
      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.016338f, 0.040729f, 0.021493f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.252950f, 0.767769f, 0.354692f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.316f, 0.316f, 0.316f));
      Colors[Graphic3d_TOR_EMISSION] = Quantity_Color (Graphic3d_Vec3 (0.0f));

      BSDF.FresnelBase = Graphic3d_Fresnel::CreateDielectric (1.5f);
      BSDF.Kd = Graphic3d_Vec3 (0.208658f, 0.415686f, 0.218401f);
      BSDF.Ks = Graphic3d_Vec4 (0.611765f, 0.611765f, 0.611765f, 0.06f);
      break;
    case Graphic3d_NameOfMaterial_Charcoal:
      MaterialType = Graphic3d_MATERIAL_PHYSIC;

      Shininess = 0.01f;
      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.003936f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.019607f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.0f));
      Colors[Graphic3d_TOR_EMISSION] = Quantity_Color (Graphic3d_Vec3 (0.0f));

      BSDF.Kd = Graphic3d_Vec3 (0.02f, 0.02f, 0.02f);
      BSDF.Ks = Graphic3d_Vec4 (0.1f, 0.1f, 0.1f, 0.3f);
      break;
    case Graphic3d_NameOfMaterial_Water:
      MaterialType = Graphic3d_MATERIAL_PHYSIC;

      RefractionIndex  = 1.33f;
      BSDF             = Graphic3d_BSDF::CreateGlass (Graphic3d_Vec3 (1.f),
                                                      Graphic3d_Vec3 (0.7f, 0.75f, 0.85f),
                                                      0.05f,
                                                      RefractionIndex);
      TransparencyCoef = 0.80f;

      Shininess = 0.90f;
      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.170645f, 0.170645f, 0.191627f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.003936f, 0.003936f, 0.006571f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.380f, 0.380f, 0.380f));
      Colors[Graphic3d_TOR_EMISSION] = Quantity_Color (Graphic3d_Vec3 (0.0f));
      break;
    case Graphic3d_NameOfMaterial_Glass:
      MaterialType = Graphic3d_MATERIAL_PHYSIC;

      RefractionIndex  = 1.62f;
      BSDF             = Graphic3d_BSDF::CreateGlass (Graphic3d_Vec3 (1.f),
                                                      Graphic3d_Vec3 (0.75f, 0.95f, 0.9f),
                                                      0.05f,
                                                      RefractionIndex);
      TransparencyCoef = 0.80f;

      Shininess = 0.50f;
      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.263273f, 0.290143f, 0.290143f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.003936f, 0.006571f, 0.006571f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.920f, 0.920f, 0.920f));
      Colors[Graphic3d_TOR_EMISSION] = Quantity_Color (Graphic3d_Vec3 (0.0f));
      break;
    case Graphic3d_NameOfMaterial_Diamond:
      MaterialType = Graphic3d_MATERIAL_PHYSIC;

      RefractionIndex  = 2.42f;
      BSDF             = Graphic3d_BSDF::CreateGlass (Graphic3d_Vec3 (1.f),
                                                      Graphic3d_Vec3 (0.95f, 0.95f, 0.95f),
                                                      0.05f,
                                                      RefractionIndex);
      TransparencyCoef = 0.80f;

      Shininess = 0.90f;
      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.263273f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.010023f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.970f));
      Colors[Graphic3d_TOR_EMISSION] = Quantity_Color (Graphic3d_Vec3 (0.0f));
      break;

    case Graphic3d_NameOfMaterial_Transparent:
      MaterialType = Graphic3d_MATERIAL_PHYSIC;

      RefractionIndex = 1.0f;

      BSDF.Kd = Graphic3d_Vec3 (0.1f);
      BSDF.Kt = Graphic3d_Vec3 (0.9f);
      BSDF.FresnelBase = Graphic3d_Fresnel::CreateConstant (0.0f);
      TransparencyCoef = 0.80f;

      Shininess = 0.90f;
      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.263273f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.010023f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.970f));
      Colors[Graphic3d_TOR_EMISSION] = Quantity_Color (Graphic3d_Vec3 (0.0f));
      break;
    case Graphic3d_NameOfMaterial_UserDefined:
      MaterialType = Graphic3d_MATERIAL_PHYSIC;
      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.1f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.6f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.2f));
      Colors[Graphic3d_TOR_EMISSION] = Quantity_Color (Graphic3d_Vec3 (0.0f));
      break;
    case Graphic3d_NameOfMaterial_DEFAULT:
      MaterialType = Graphic3d_MATERIAL_ASPECT;

      Colors[Graphic3d_TOR_AMBIENT]  = Quantity_Color (Graphic3d_Vec3 (0.15f));
      Colors[Graphic3d_TOR_DIFFUSE]  = Quantity_Color (Graphic3d_Vec3 (0.65f));
      Colors[Graphic3d_TOR_SPECULAR] = Quantity_Color (Graphic3d_Vec3 (0.0f));
      Colors[Graphic3d_TOR_EMISSION] = Quantity_Color (Graphic3d_Vec3 (0.0f));
      break;
  }
  PBRMaterial.SetBSDF (BSDF);
}

// =======================================================================
// function : Graphic3d_MaterialAspect
// purpose  :
// =======================================================================
Graphic3d_MaterialAspect::Graphic3d_MaterialAspect()
: myRequestedMaterialName (Graphic3d_NameOfMaterial_DEFAULT)
{
  init (Graphic3d_NameOfMaterial_DEFAULT);
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
  myBSDF        = aMat.BSDF;
  myPBRMaterial = aMat.PBRMaterial;
  myStringName  = aMat.StringName;
  myColors[Graphic3d_TOR_AMBIENT]     = aMat.Colors[Graphic3d_TOR_AMBIENT];
  myColors[Graphic3d_TOR_DIFFUSE]     = aMat.Colors[Graphic3d_TOR_DIFFUSE];
  myColors[Graphic3d_TOR_SPECULAR]    = aMat.Colors[Graphic3d_TOR_SPECULAR];
  myColors[Graphic3d_TOR_EMISSION]    = aMat.Colors[Graphic3d_TOR_EMISSION];
  myTransparencyCoef = aMat.TransparencyCoef;
  myRefractionIndex  = aMat.RefractionIndex;
  myShininess        = aMat.Shininess;
  myMaterialType     = aMat.MaterialType;
  myMaterialName     = theName;
  myRequestedMaterialName = theName;
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
// function : SetColor
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::SetColor (const Quantity_Color& theColor)
{
  if (myMaterialType == Graphic3d_MATERIAL_ASPECT)
  {
    return;
  }

  myPBRMaterial.SetColor (theColor);

  const RawMaterial& aSrcMat = THE_MATERIALS[myRequestedMaterialName];
  const Quantity_Color anAmbient((Graphic3d_Vec3 )theColor * aSrcMat.AmbientCoef);
  const Quantity_Color aDiffuse ((Graphic3d_Vec3 )theColor * aSrcMat.DiffuseCoef);
  if (myMaterialName != Graphic3d_NameOfMaterial_UserDefined
   && (!myColors[Graphic3d_TOR_AMBIENT].IsEqual (anAmbient)
    || !myColors[Graphic3d_TOR_DIFFUSE].IsEqual (aDiffuse)))
  {
    setUserMaterial();
  }
  myColors[Graphic3d_TOR_AMBIENT] = anAmbient;
  myColors[Graphic3d_TOR_DIFFUSE] = aDiffuse;
}

// =======================================================================
// function : SetAmbientColor
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::SetAmbientColor (const Quantity_Color& theColor)
{
  if (myMaterialType == Graphic3d_MATERIAL_PHYSIC
  &&  myMaterialName != Graphic3d_NameOfMaterial_UserDefined
  && !myColors[Graphic3d_TOR_AMBIENT].IsEqual (theColor))
  {
    setUserMaterial();
  }
  myColors[Graphic3d_TOR_AMBIENT] = theColor;
}

// =======================================================================
// function : SetDiffuseColor
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::SetDiffuseColor (const Quantity_Color& theColor)
{
  if (myMaterialType == Graphic3d_MATERIAL_PHYSIC
  &&  myMaterialName != Graphic3d_NameOfMaterial_UserDefined
  && !myColors[Graphic3d_TOR_DIFFUSE].IsEqual (theColor))
  {
    setUserMaterial();
  }
  myColors[Graphic3d_TOR_DIFFUSE] = theColor;
}

// =======================================================================
// function : SetSpecularColor
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::SetSpecularColor (const Quantity_Color& theColor)
{
  if (myMaterialType == Graphic3d_MATERIAL_PHYSIC
  &&  myMaterialName != Graphic3d_NameOfMaterial_UserDefined
  && !myColors[Graphic3d_TOR_SPECULAR].IsEqual (theColor))
  {
    setUserMaterial();
  }
  myColors[Graphic3d_TOR_SPECULAR] = theColor;
}

// =======================================================================
// function : SetEmissiveColor
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::SetEmissiveColor (const Quantity_Color& theColor)
{
  if (myMaterialType == Graphic3d_MATERIAL_PHYSIC
  &&  myMaterialName != Graphic3d_NameOfMaterial_UserDefined
  && !myColors[Graphic3d_TOR_EMISSION].IsEqual (theColor))
  {
    setUserMaterial();
  }
  myColors[Graphic3d_TOR_EMISSION] = theColor;
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
  myPBRMaterial.SetAlpha (1.0f - theValue);
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

  if (myShininess != theValue)
  {
    myShininess = theValue;
    setUserMaterial();
  }
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
Standard_Boolean Graphic3d_MaterialAspect::MaterialFromName (const Standard_CString theName,
                                                             Graphic3d_NameOfMaterial& theMat)
{
  TCollection_AsciiString aName (theName);
  aName.LowerCase();
  aName.Capitalize();
  const Standard_Integer aNbMaterials = Graphic3d_MaterialAspect::NumberOfMaterials();
  for (Standard_Integer aMatIter = 0; aMatIter <= aNbMaterials; ++aMatIter)
  {
    const RawMaterial& aMat = THE_MATERIALS[aMatIter];
    if (aName == aMat.StringName)
    {
      theMat = Graphic3d_NameOfMaterial(aMatIter);
      return Standard_True;
    }
  }

  // parse aliases
  if (aName == "Plastic")            // Plastified
  {
    theMat = Graphic3d_NameOfMaterial_Plastified;
    return Standard_True;
  }
  else if (aName == "Shiny_plastic") // Shiny_plastified
  {
    theMat = Graphic3d_NameOfMaterial_ShinyPlastified;
    return Standard_True;
  }
  else if (aName == "Plaster")       // Plastered
  {
    theMat = Graphic3d_NameOfMaterial_Plastered;
    return Standard_True;
  }
  else if (aName == "Satin")         // Satined
  {
    theMat = Graphic3d_NameOfMaterial_Satin;
    return Standard_True;
  }
  else if (aName == "Neon_gnc")      // Ionized
  {
    theMat = Graphic3d_NameOfMaterial_Ionized;
    return Standard_True;
  }
  else if (aName == "Neon_phc") // Neon
  {
    theMat = Graphic3d_NameOfMaterial_Neon;
    return Standard_True;
  }
  return Standard_False;
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

//=======================================================================
//function : DumpJson
//purpose  : 
//=======================================================================
void Graphic3d_MaterialAspect::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_CLASS_BEGIN (theOStream, Graphic3d_MaterialAspect)

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myBSDF)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myPBRMaterial)
  OCCT_DUMP_FIELD_VALUE_STRING (theOStream, myStringName)

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myColors[Graphic3d_TOR_AMBIENT])
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myColors[Graphic3d_TOR_DIFFUSE])
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myColors[Graphic3d_TOR_SPECULAR])
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myColors[Graphic3d_TOR_EMISSION])

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myTransparencyCoef)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myRefractionIndex)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myShininess)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myMaterialType)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myMaterialName)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myRequestedMaterialName)
}
