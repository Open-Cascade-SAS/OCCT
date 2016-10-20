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
#include <Quantity_Color.hxx>
#include <Standard_Assert.hxx>
#include <Standard_OutOfRange.hxx>

// =======================================================================
// function : Graphic3d_MaterialAspect
// purpose  :
// =======================================================================
Graphic3d_MaterialAspect::Graphic3d_MaterialAspect()
: myDiffuseColor (0.2, 0.2, 0.2, Quantity_TOC_RGB),
  myAmbientColor (0.2, 0.2, 0.2, Quantity_TOC_RGB)
{
  myRequestedMaterialName = Graphic3d_NOM_DEFAULT;
  Init (myRequestedMaterialName);
}

// =======================================================================
// function : Graphic3d_MaterialAspect
// purpose  :
// =======================================================================
Graphic3d_MaterialAspect::Graphic3d_MaterialAspect (const Graphic3d_NameOfMaterial theName)
: myDiffuseColor (0.2, 0.2, 0.2, Quantity_TOC_RGB),
  myAmbientColor (0.2, 0.2, 0.2, Quantity_TOC_RGB)
{
  myRequestedMaterialName = theName;
  Init (myRequestedMaterialName);
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::Init (const Graphic3d_NameOfMaterial theName)
{
  myMaterialType     = Graphic3d_MATERIAL_ASPECT;
  myTransparencyCoef = 0.0f;
  myRefractionIndex  = 1.0f;
  myDiffuseActivity  = Standard_True;
  myDiffuseCoef      = 0.65f;
  myAmbientActivity  = Standard_True;
  myAmbientCoef      = 0.3f;
  mySpecularActivity = Standard_True;
  mySpecularCoef     = 0.0f;
  myEmissiveActivity = Standard_False;
  myEmissiveCoef     = 0.0f;
  myEnvReflexion     = 0.0f;
  myShininess        = 0.039f;
  myDiffuseColor .SetValues (0.2, 0.2, 0.2, Quantity_TOC_RGB);
  mySpecularColor.SetValues (1.0, 1.0, 1.0, Quantity_TOC_RGB);
  myMaterialName     = theName;

  myBSDF = Graphic3d_BSDF::CreateDiffuse (Graphic3d_Vec3 (0.0f));

  Standard_Integer index = Standard_Integer (theName);
  if (index < NumberOfMaterials())
  {
    myStringName = MaterialName (index + 1);
  }

  switch (theName)
  {
    case Graphic3d_NOM_PLASTIC:
      myShininess    = Standard_ShortReal (0.0078125);
      myAmbientCoef  = Standard_ShortReal (0.5);
      myDiffuseCoef  = Standard_ShortReal (0.24);
      mySpecularCoef = Standard_ShortReal (0.06);

      myBSDF.Kd = Graphic3d_Vec3 (static_cast<Standard_ShortReal> (myDiffuseColor.Red()),
                                  static_cast<Standard_ShortReal> (myDiffuseColor.Green()),
                                  static_cast<Standard_ShortReal> (myDiffuseColor.Blue()));
      myBSDF.Ks = Graphic3d_Vec3 (0.00784314f, 0.00784314f, 0.00784314f);
      myBSDF.Normalize();
      myBSDF.Roughness = 0.25f;
      break;
    case Graphic3d_NOM_SHINY_PLASTIC:
      myShininess    = Standard_ShortReal (1.0);
      myAmbientCoef  = Standard_ShortReal (0.44);
      myDiffuseCoef  = Standard_ShortReal (0.5);
      mySpecularCoef = Standard_ShortReal (1.0);

      myBSDF.Kd = Graphic3d_Vec3 (static_cast<Standard_ShortReal> (myDiffuseColor.Red()),
                                  static_cast<Standard_ShortReal> (myDiffuseColor.Green()),
                                  static_cast<Standard_ShortReal> (myDiffuseColor.Blue()));
      myBSDF.Ks = Graphic3d_Vec3 (0.145f, 0.145f, 0.145f);
      myBSDF.Normalize();
      myBSDF.Roughness = 0.17f;
      break;
    case Graphic3d_NOM_SATIN :
      myShininess    = Standard_ShortReal (0.09375);
      myAmbientCoef  = Standard_ShortReal (0.33);
      myDiffuseCoef  = Standard_ShortReal (0.4);
      mySpecularCoef = Standard_ShortReal (0.44);

      myBSDF.Kd = Graphic3d_Vec3 (0.2f);
      myBSDF.Ks = Graphic3d_Vec3 (0.6f);
      myBSDF.Roughness = 0.6f;

      break;
    case Graphic3d_NOM_NEON_GNC:
      myShininess    = Standard_ShortReal (0.05);
      myAmbientCoef  = Standard_ShortReal (1.0);
      myDiffuseCoef  = Standard_ShortReal (1.0);
      mySpecularCoef = Standard_ShortReal (0.62);
      myEmissiveCoef = Standard_ShortReal (1.0);
      myEmissiveActivity = Standard_True;
      myAmbientActivity  = Standard_False;

      myBSDF.Kd = Graphic3d_Vec3 (0.0f);
      myBSDF.Kr = Graphic3d_Vec3 (0.5f);
      myBSDF.Le = Graphic3d_Vec3 (static_cast<Standard_ShortReal> (myDiffuseColor.Red()),
                                  static_cast<Standard_ShortReal> (myDiffuseColor.Green()),
                                  static_cast<Standard_ShortReal> (myDiffuseColor.Blue()));
      myBSDF.Fresnel = Graphic3d_Fresnel::CreateDielectric (1.5f);
      break;
    case Graphic3d_NOM_METALIZED:
      myShininess    = Standard_ShortReal (0.13);
      myAmbientCoef  = Standard_ShortReal (0.9);
      myDiffuseCoef  = Standard_ShortReal (0.47);
      mySpecularCoef = Standard_ShortReal (0.45);
      myAmbientActivity  = Standard_False;

      {
        Graphic3d_Vec3 aColor (static_cast<Standard_ShortReal> (myDiffuseColor.Red()),
                               static_cast<Standard_ShortReal> (myDiffuseColor.Green()),
                               static_cast<Standard_ShortReal> (myDiffuseColor.Blue()));

        myBSDF = Graphic3d_BSDF::CreateMetallic (Graphic3d_Vec3 (0.985f, 0.985f, 0.985f),
          Graphic3d_Fresnel::CreateSchlick (aColor), 0.045f);
      }
      break;
    // Ascending Compatibility physical materials. The same definition is taken as in the next constructor.
    case Graphic3d_NOM_BRASS:
      myMaterialType = Graphic3d_MATERIAL_PHYSIC;

      myShininess    = 0.65f;
      myAmbientCoef  = 1.00f;
      myDiffuseCoef  = 1.00f;
      mySpecularCoef = 1.00f;

      myBSDF = Graphic3d_BSDF::CreateMetallic (Graphic3d_Vec3 (0.985f, 0.985f, 0.985f),
        Graphic3d_Fresnel::CreateSchlick (Graphic3d_Vec3 (0.58f, 0.42f, 0.20f)), 0.045f);

      // Color resulting from ambient
      myAmbientColor .SetValues (0.329f, 0.224f, 0.027f, Quantity_TOC_RGB);
      // Color resulting from dispersed
      myDiffuseColor .SetValues (0.780f, 0.569f, 0.114f, Quantity_TOC_RGB);
      // Color resulting from specular
      mySpecularColor.SetValues (0.992f, 0.941f, 0.808f, Quantity_TOC_RGB);
      break;
    case Graphic3d_NOM_BRONZE:
      myMaterialType = Graphic3d_MATERIAL_PHYSIC;

      myShininess    = 0.65f;
      myAmbientCoef  = 1.00f;
      myDiffuseCoef  = 1.00f;
      mySpecularCoef = 1.00f;

      myBSDF = Graphic3d_BSDF::CreateMetallic (Graphic3d_Vec3 (0.985f, 0.985f, 0.985f),
        Graphic3d_Fresnel::CreateSchlick (Graphic3d_Vec3 (0.65f, 0.35f, 0.15f)), 0.045f);

      // Color resulting from ambient
      myAmbientColor .SetValues (0.213f, 0.128f, 0.054f, Quantity_TOC_RGB);
      // Color resulting from dispersed
      myDiffuseColor .SetValues (0.714f, 0.428f, 0.181f, Quantity_TOC_RGB);
      // Color resulting from specular
      mySpecularColor.SetValues (0.590f, 0.408f, 0.250f, Quantity_TOC_RGB);
      break;
    case Graphic3d_NOM_COPPER:
      myMaterialType = Graphic3d_MATERIAL_PHYSIC;

      myShininess    = 0.65f;
      myAmbientCoef  = 1.00f;
      myDiffuseCoef  = 1.00f;
      mySpecularCoef = 1.00f;

      myBSDF = Graphic3d_BSDF::CreateMetallic (Graphic3d_Vec3 (0.985f, 0.985f, 0.985f),
        Graphic3d_Fresnel::CreateSchlick (Graphic3d_Vec3 (0.955008f, 0.637427f, 0.538163f)), 0.045f);

      // Color resulting from ambient
      myAmbientColor .SetValues (0.191f, 0.074f, 0.023f, Quantity_TOC_RGB);
      // Color resulting from dispersed
      myDiffuseColor .SetValues (0.604f, 0.270f, 0.083f, Quantity_TOC_RGB);
      // Color resulting from specular
      mySpecularColor.SetValues (0.950f, 0.640f, 0.540f, Quantity_TOC_RGB);
      break;
    case Graphic3d_NOM_GOLD:
      myMaterialType = Graphic3d_MATERIAL_PHYSIC;

      myShininess    = 0.80f;
      myAmbientCoef  = 1.00f;
      myDiffuseCoef  = 1.00f;
      mySpecularCoef = 1.00f;

      myBSDF = Graphic3d_BSDF::CreateMetallic (Graphic3d_Vec3 (0.985f, 0.985f, 0.985f),
        Graphic3d_Fresnel::CreateSchlick (Graphic3d_Vec3 (1.000000f, 0.765557f, 0.336057f)), 0.045f);

      // Color resulting from ambient
      myAmbientColor .SetValues (0.300f, 0.230f, 0.095f, Quantity_TOC_RGB);
      // Color resulting from dispersed
      myDiffuseColor .SetValues (0.752f, 0.580f, 0.100f, Quantity_TOC_RGB);
      // Color resulting from specular
      mySpecularColor.SetValues (1.000f, 0.710f, 0.290f, Quantity_TOC_RGB);
      break;
    case Graphic3d_NOM_PEWTER:
      myMaterialType = Graphic3d_MATERIAL_PHYSIC;

      myShininess    = 0.50f;
      myAmbientCoef  = 1.00f;
      myDiffuseCoef  = 1.00f;
      mySpecularCoef = 1.00f;

      myBSDF = Graphic3d_BSDF::CreateMetallic (Graphic3d_Vec3 (0.985f, 0.985f, 0.985f),
        Graphic3d_Fresnel::CreateConductor (1.8800f, 3.4900f), 0.045f);

      // Color resulting from ambient
      myAmbientColor .SetValues (0.106f, 0.059f, 0.114f, Quantity_TOC_RGB);
      // Color resulting from dispersed
      myDiffuseColor .SetValues (0.427f, 0.471f, 0.541f, Quantity_TOC_RGB);
      // Color resulting from specular
      mySpecularColor.SetValues (0.333f, 0.333f, 0.522f, Quantity_TOC_RGB);
      break;
    case Graphic3d_NOM_PLASTER:
      myShininess    = 0.01f;
      myAmbientCoef  = 0.26f;
      myDiffuseCoef  = 0.75f;
      mySpecularCoef = 0.05f;

      // Color resulting from ambient
      myAmbientColor .SetValues (0.192f, 0.192f, 0.192f, Quantity_TOC_RGB);
      // Color resulting from dispersed
      myDiffuseColor .SetValues (0.508f, 0.508f, 0.508f, Quantity_TOC_RGB);
      // Color resulting from specular
      mySpecularColor.SetValues (0.508f, 0.508f, 0.508f, Quantity_TOC_RGB);

      myBSDF.Kd = Graphic3d_Vec3 (0.482353f, 0.482353f, 0.482353f);

      break;
    case Graphic3d_NOM_SILVER:
      myMaterialType = Graphic3d_MATERIAL_PHYSIC;

      myShininess    = 0.75f;
      myAmbientCoef  = 1.00f;
      myDiffuseCoef  = 1.00f;
      mySpecularCoef = 1.00f;

      myBSDF = Graphic3d_BSDF::CreateMetallic (Graphic3d_Vec3 (0.985f, 0.985f, 0.985f),
        Graphic3d_Fresnel::CreateSchlick (Graphic3d_Vec3 (0.971519f, 0.959915f, 0.915324f)), 0.045f);

      // Color resulting from ambient
      myAmbientColor .SetValues (0.275f, 0.275f, 0.250f, Quantity_TOC_RGB);
      // Color resulting from dispersed
      myDiffuseColor .SetValues (0.630f, 0.630f, 0.630f, Quantity_TOC_RGB);
      // Color resulting from specular
      mySpecularColor.SetValues (0.950f, 0.930f, 0.880f, Quantity_TOC_RGB);
      break;
    case Graphic3d_NOM_STEEL:
      myMaterialType = Graphic3d_MATERIAL_PHYSIC;

      myShininess    = 0.90f;
      myAmbientCoef  = 1.00f;
      myDiffuseCoef  = 1.00f;
      mySpecularCoef = 1.00f;

      myBSDF = Graphic3d_BSDF::CreateMetallic (Graphic3d_Vec3 (0.985f, 0.985f, 0.985f),
        Graphic3d_Fresnel::CreateConductor (Graphic3d_Vec3 (2.90f, 2.80f, 2.53f), Graphic3d_Vec3 (3.08f, 2.90f, 2.74f)), 0.045f);

      // Color resulting from ambient
      myAmbientColor .SetValues (0.150f, 0.150f, 0.180f, Quantity_TOC_RGB);
      // Color resulting from dispersed
      myDiffuseColor .SetValues (0.500f, 0.510f, 0.520f, Quantity_TOC_RGB);
      // Color resulting from specular
      mySpecularColor.SetValues (0.560f, 0.570f, 0.580f, Quantity_TOC_RGB);
      break;
    case Graphic3d_NOM_STONE:
      myMaterialType = Graphic3d_MATERIAL_PHYSIC;

      myShininess    = 0.17f;
      myAmbientCoef  = 0.19f;
      myDiffuseCoef  = 0.75f;
      mySpecularCoef = 0.08f;

      // Color resulting from ambient
      myAmbientColor .SetValues (1.0,  0.8, 0.62, Quantity_TOC_RGB);
      // Color resulting from dispersed
      myDiffuseColor .SetValues (1.0,  0.8, 0.62, Quantity_TOC_RGB);
      // Color resulting from specular
      mySpecularColor.SetValues (0.98, 1.0, 0.60, Quantity_TOC_RGB);

      myBSDF.Kd = Graphic3d_Vec3 (0.243137f, 0.243137f, 0.243137f);
      myBSDF.Ks = Graphic3d_Vec3 (0.00392157f, 0.00392157f, 0.00392157f);

      break;
    // Ascending Compatibility of physical materials. Takes the same definition as in the next constructor. New materials
    case Graphic3d_NOM_CHROME:
      myMaterialType = Graphic3d_MATERIAL_PHYSIC;

      myShininess    = 0.90f;
      myAmbientCoef  = 1.00f;
      myDiffuseCoef  = 1.00f;
      mySpecularCoef = 1.00f;

      myBSDF = Graphic3d_BSDF::CreateMetallic (Graphic3d_Vec3 (0.985f, 0.985f, 0.985f),
        Graphic3d_Fresnel::CreateSchlick (Graphic3d_Vec3 (0.549585f, 0.556114f, 0.554256f)), 0.045f);

      // Color resulting from ambient
      myAmbientColor .SetValues (0.200f, 0.200f, 0.225f, Quantity_TOC_RGB);
      // Color resulting from dispersed
      myDiffuseColor .SetValues (0.550f, 0.550f, 0.550f, Quantity_TOC_RGB);
      // Color resulting from specular
      mySpecularColor.SetValues (0.975f, 0.975f, 0.975f, Quantity_TOC_RGB);
      break;
    case Graphic3d_NOM_ALUMINIUM:
      myMaterialType = Graphic3d_MATERIAL_PHYSIC;

      myShininess    = 0.75f;
      myAmbientCoef  = 1.00f;
      myDiffuseCoef  = 1.00f;
      mySpecularCoef = 1.00f;

      myBSDF = Graphic3d_BSDF::CreateMetallic (Graphic3d_Vec3 (0.985f, 0.985f, 0.985f),
        Graphic3d_Fresnel::CreateSchlick (Graphic3d_Vec3 (0.913183f, 0.921494f, 0.924524f)), 0.045f);

      // Color resulting from ambient
      myAmbientColor .SetValues (0.300f, 0.300f, 0.300f, Quantity_TOC_RGB);
      // Color resulting from dispersed
      myDiffuseColor .SetValues (0.600f, 0.600f, 0.600f, Quantity_TOC_RGB);
      // Color resulting from specular
      mySpecularColor.SetValues (0.910f, 0.920f, 0.920f, Quantity_TOC_RGB);
      break;
    case Graphic3d_NOM_NEON_PHC:
      myMaterialType = Graphic3d_MATERIAL_PHYSIC;

      myShininess    = 0.05f;
      myAmbientCoef  = 1.00f;
      myDiffuseCoef  = 1.00f;
      mySpecularCoef = 0.62f;
      myEmissiveCoef = 0.90f;
      myAmbientActivity  = Standard_False;
      myDiffuseActivity  = Standard_False;
      myEmissiveActivity = Standard_True;

      // Color resulting from ambient
      myAmbientColor .SetValues (1.0, 1.0, 1.0,  Quantity_TOC_RGB);
      // Color resulting from dispersed
      myDiffuseColor .SetValues (1.0, 1.0, 1.0,  Quantity_TOC_RGB);
      // Color resulting from specular
      mySpecularColor.SetValues (1.0, 1.0, 1.0,  Quantity_TOC_RGB);
      // Color resulting from specular
      myEmissiveColor.SetValues (0.0, 1.0, 0.46, Quantity_TOC_RGB);

      myBSDF.Kd = Graphic3d_Vec3 (0.0f);
      myBSDF.Kr = Graphic3d_Vec3 (0.5f);
      myBSDF.Le = Graphic3d_Vec3 (0.0f, 1.0f, 0.46f);
      myBSDF.Fresnel = Graphic3d_Fresnel::CreateDielectric (1.5f);
      break;
    case Graphic3d_NOM_OBSIDIAN:
      myMaterialType = Graphic3d_MATERIAL_PHYSIC;

      myShininess    = 0.3f;
      myAmbientCoef  = 1.0f;
      myDiffuseCoef  = 1.0f;
      mySpecularCoef = 1.0f;

      // Color resulting from ambient
      myAmbientColor .SetValues (0.054f, 0.050f, 0.066f, Quantity_TOC_RGB);
      // Color resulting from dispersed
      myDiffuseColor .SetValues (0.183f, 0.170f, 0.225f, Quantity_TOC_RGB);
      // Color resulting from specular
      mySpecularColor.SetValues (0.333f, 0.329f, 0.346f, Quantity_TOC_RGB);

      myBSDF.Kd = Graphic3d_Vec3 (0.023f, 0.f, 0.023f);
      myBSDF.Ks = Graphic3d_Vec3 (0.0156863f, 0.0156863f, 0.0156863f);
      myBSDF.Roughness = 0.1f;
      break;
    case Graphic3d_NOM_JADE:
      myMaterialType = Graphic3d_MATERIAL_PHYSIC;

      myShininess    = 0.10f;
      myAmbientCoef  = 1.00f;
      myDiffuseCoef  = 1.00f;
      mySpecularCoef = 1.00f;

      // Color resulting from ambient
      myAmbientColor .SetValues (0.135f, 0.223f, 0.158f, Quantity_TOC_RGB);
      // Color resulting from dispersed
      myDiffuseColor .SetValues (0.540f, 0.890f, 0.630f, Quantity_TOC_RGB);
      // Color resulting from specular
      mySpecularColor.SetValues (0.316f, 0.316f, 0.316f, Quantity_TOC_RGB);

      myBSDF.Fresnel = Graphic3d_Fresnel::CreateDielectric (1.5f);
      myBSDF.Kd = Graphic3d_Vec3 (0.208658f, 0.415686f, 0.218401f);
      myBSDF.Ks = Graphic3d_Vec3 (0.611765f, 0.611765f, 0.611765f);
      myBSDF.Roughness = 0.06f;
      break;
    case Graphic3d_NOM_CHARCOAL:
      myMaterialType = Graphic3d_MATERIAL_PHYSIC;

      myShininess    = 0.01f;
      myAmbientCoef  = 1.00f;
      myDiffuseCoef  = 1.00f;
      mySpecularCoef = 1.00f;

      // Color resulting from ambient
      myAmbientColor .SetValues (0.050f, 0.050f, 0.050f, Quantity_TOC_RGB);
      // Color resulting from dispersed
      myDiffuseColor .SetValues (0.150f, 0.150f, 0.150f, Quantity_TOC_RGB);
      // Color resulting from specular
      mySpecularColor.SetValues (0.000f, 0.000f, 0.000f, Quantity_TOC_RGB);

      myBSDF.Kd = Graphic3d_Vec3 (0.02f, 0.02f, 0.02f);
      myBSDF.Ks = Graphic3d_Vec3 (0.1f, 0.1f, 0.1f);
      myBSDF.Roughness = 0.3f;
      break;
    case Graphic3d_NOM_WATER:
      myMaterialType = Graphic3d_MATERIAL_PHYSIC;

      myShininess    = 0.90f;
      myAmbientCoef  = 1.00f;
      myDiffuseCoef  = 1.00f;
      mySpecularCoef = 1.00f;
      myRefractionIndex  = 1.33f;
      myBSDF             = Graphic3d_BSDF::CreateGlass (Graphic3d_Vec3 (1.f),
                                                        Graphic3d_Vec3 (0.7f, 0.75f, 0.85f),
                                                        0.05f,
                                                        myRefractionIndex);
      myTransparencyCoef = 0.80f;

      // Color resulting from ambient
      myAmbientColor.SetValues (0.450f, 0.450f, 0.475f, Quantity_TOC_RGB);
      // Color resulting from dispersed
      myDiffuseColor.SetValues (0.050f, 0.050f, 0.075f, Quantity_TOC_RGB);
      // Color resulting from specular
      mySpecularColor.SetValues (0.380f, 0.380f, 0.380f, Quantity_TOC_RGB);
      break;
    case Graphic3d_NOM_GLASS:
      myMaterialType = Graphic3d_MATERIAL_PHYSIC;

      myShininess    = 0.50f;
      myAmbientCoef  = 1.00f;
      myDiffuseCoef  = 1.00f;
      mySpecularCoef = 1.00f;
      myRefractionIndex  = 1.62f;
      myBSDF             = Graphic3d_BSDF::CreateGlass (Graphic3d_Vec3 (1.f),
                                                        Graphic3d_Vec3 (0.75f, 0.95f, 0.9f),
                                                        0.05f,
                                                        myRefractionIndex);
      myTransparencyCoef = 0.80f;

      // Color resulting from ambient
      myAmbientColor.SetValues (0.550f, 0.575f, 0.575f, Quantity_TOC_RGB);
      // Color resulting from dispersed
      myDiffuseColor.SetValues (0.050f, 0.075f, 0.075f, Quantity_TOC_RGB);
      // Color resulting from specular
      mySpecularColor.SetValues (0.920f, 0.920f, 0.920f, Quantity_TOC_RGB);
      break;
    case Graphic3d_NOM_DIAMOND:
      myMaterialType = Graphic3d_MATERIAL_PHYSIC;

      myShininess    = 0.90f;
      myAmbientCoef  = 1.00f;
      myDiffuseCoef  = 1.00f;
      mySpecularCoef = 1.00f;
      myRefractionIndex  = 2.42f;
      myBSDF             = Graphic3d_BSDF::CreateGlass (Graphic3d_Vec3 (1.f),
                                                        Graphic3d_Vec3 (0.95f, 0.95f, 0.95f),
                                                        0.05f,
                                                        myRefractionIndex);
      myTransparencyCoef = 0.80f;

      // Color resulting from ambient
      myAmbientColor.SetValues (0.550f, 0.550f, 0.550f, Quantity_TOC_RGB);
      // Color resulting from dispersed
      myDiffuseColor.SetValues (0.100f, 0.100f, 0.100f, Quantity_TOC_RGB);
      // Color resulting from specular
      mySpecularColor.SetValues (0.970f, 0.970f, 0.970f, Quantity_TOC_RGB);
      break;

    case Graphic3d_NOM_TRANSPARENT:
      myMaterialType = Graphic3d_MATERIAL_PHYSIC;

      myShininess    = 0.90f;
      myAmbientCoef  = 1.00f;
      myDiffuseCoef  = 1.00f;
      mySpecularCoef = 1.00f;
      myRefractionIndex  = 1.0f;

      myBSDF.Kd = Graphic3d_Vec3 (0.1f);
      myBSDF.Kt = Graphic3d_Vec3 (0.9f);
      myBSDF.Fresnel = Graphic3d_Fresnel::CreateConstant (0.0f);
      myTransparencyCoef = 0.80f;

      // Color resulting from ambient
      myAmbientColor.SetValues (0.550f, 0.550f, 0.550f, Quantity_TOC_RGB);
      // Color resulting from dispersed
      myDiffuseColor.SetValues (0.100f, 0.100f, 0.100f, Quantity_TOC_RGB);
      // Color resulting from specular
      mySpecularColor.SetValues (0.970f, 0.970f, 0.970f, Quantity_TOC_RGB);
      break;

    case Graphic3d_NOM_UserDefined:
      myStringName = "UserDefined";
      break;
    case Graphic3d_NOM_DEFAULT:
    default:
      myStringName = "Default";
      break;
  }
}

// =======================================================================
// function : IncreaseShine
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::IncreaseShine (const Standard_Real theDelta)
{
  Standard_ShortReal anOldShine = myShininess;
  myShininess = Standard_ShortReal(myShininess + myShininess * theDelta / 100.0);
  if (myShininess > 1.0 || myShininess < 0.0)
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
    SetMaterialName ("UserDefined");
  }
}

// =======================================================================
// function : SetAmbient
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::SetAmbient (const Standard_Real theValue)
{
  if (theValue < 0.0
   || theValue > 1.0)
  {
    Graphic3d_MaterialDefinitionError::Raise ("Bad value for SetAmbient < 0. or > 1.0");
  }

  myAmbientCoef = Standard_ShortReal (theValue);
  if (myAmbientActivity
   && myMaterialType == Graphic3d_MATERIAL_PHYSIC)
  {
    SetMaterialName ("UserDefined");
  }
}

// =======================================================================
// function : SetColor
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::SetColor (const Quantity_Color& theColor)
{
  const Standard_ShortReal anAmbientCoeff = 0.25f;
  myAmbientColor.SetValues (theColor.Red()   * anAmbientCoeff,
                            theColor.Green() * anAmbientCoeff,
                            theColor.Blue()  * anAmbientCoeff, Quantity_TOC_RGB);
  myDiffuseColor = theColor;
  if (myAmbientActivity && myMaterialType == Graphic3d_MATERIAL_PHYSIC)
  {
    SetMaterialName ("UserDefined");
  }
}

// =======================================================================
// function : SetAmbientColor
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::SetAmbientColor (const Quantity_Color& theColor)
{
  myAmbientColor = theColor;
  if (myAmbientActivity
   && myMaterialType == Graphic3d_MATERIAL_PHYSIC)
  {
    SetMaterialName ("UserDefined");
  }
}

// =======================================================================
// function : SetDiffuseColor
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::SetDiffuseColor (const Quantity_Color& theColor)
{
  myDiffuseColor = theColor;
  if (myDiffuseActivity
   && myMaterialType == Graphic3d_MATERIAL_PHYSIC)
  {
    SetMaterialName ("UserDefined");
  }
}

// =======================================================================
// function : SetSpecularColor
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::SetSpecularColor (const Quantity_Color& theColor)
{
  mySpecularColor = theColor;
  if (mySpecularActivity
   && myMaterialType == Graphic3d_MATERIAL_PHYSIC)
  {
    SetMaterialName ("UserDefined");
  }
}

// =======================================================================
// function : SetEmissiveColor
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::SetEmissiveColor (const Quantity_Color& theColor)
{
  myEmissiveColor = theColor;
  if (myEmissiveActivity
   && myMaterialType == Graphic3d_MATERIAL_PHYSIC)
  {
    SetMaterialName ("UserDefined");
  }
}

// =======================================================================
// function : SetDiffuse
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::SetDiffuse (const Standard_Real theValue)
{
  if (theValue < 0.0
   || theValue > 1.0)
  {
    Graphic3d_MaterialDefinitionError::Raise ("Bad value for SetDiffuse < 0. or > 1.0");
  }

  myDiffuseCoef  = Standard_ShortReal (theValue);
  if (myDiffuseActivity
   && myMaterialType == Graphic3d_MATERIAL_PHYSIC)
  {
    SetMaterialName ("UserDefined");
  }
}

// =======================================================================
// function : SetEmissive
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::SetEmissive (const Standard_Real theValue)
{
  if (theValue < 0.0
   || theValue > 1.0)
  {
    Graphic3d_MaterialDefinitionError::Raise ("Bad value for SetEmissive < 0. or > 1.0");
  }

  myEmissiveCoef = Standard_ShortReal (theValue);
  if (myDiffuseActivity
   && myMaterialType == Graphic3d_MATERIAL_PHYSIC)
  {
    SetMaterialName ("UserDefined");
  }
}

// =======================================================================
// function : SetReflectionModeOn
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::SetReflectionModeOn (const Graphic3d_TypeOfReflection theType)
{
  switch (theType)
  {
    case Graphic3d_TOR_AMBIENT:
      myAmbientActivity  = Standard_True;
      break;
    case Graphic3d_TOR_DIFFUSE:
      myDiffuseActivity  = Standard_True;
      break;
    case Graphic3d_TOR_SPECULAR:
      mySpecularActivity = Standard_True;
      break;
    case Graphic3d_TOR_EMISSION:
      myEmissiveActivity = Standard_True;
      break;
  }
  if (myMaterialType == Graphic3d_MATERIAL_PHYSIC)
  {
    SetMaterialName ("UserDefined");
  }
}

// =======================================================================
// function : SetReflectionModeOff
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::SetReflectionModeOff (const Graphic3d_TypeOfReflection theType)
{
  switch (theType)
  {
    case Graphic3d_TOR_AMBIENT:
      myAmbientActivity  = Standard_False;
      break;
    case Graphic3d_TOR_DIFFUSE:
      myDiffuseActivity  = Standard_False;
      break;
    case Graphic3d_TOR_SPECULAR:
      mySpecularActivity = Standard_False;
      break;
    case Graphic3d_TOR_EMISSION:
      myEmissiveActivity = Standard_False;
      break;
  }
  if (myMaterialType == Graphic3d_MATERIAL_PHYSIC)
  {
    SetMaterialName ("UserDefined");
  }
}

// =======================================================================
// function : SetSpecular
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::SetSpecular (const Standard_Real theValue)
{
  if (theValue < 0.0
   || theValue > 1.0)
  {
    Graphic3d_MaterialDefinitionError::Raise ("Bad value for SetSpecular < 0. or > 1.0");
  }

  mySpecularCoef  = Standard_ShortReal (theValue);
  if (mySpecularActivity
   && myMaterialType == Graphic3d_MATERIAL_PHYSIC)
  {
    SetMaterialName ("UserDefined");
  }
}

// =======================================================================
// function : SetTransparency
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::SetTransparency (const Standard_Real theValue)
{
  if (theValue < 0.0
   || theValue > 1.0)
  {
    Graphic3d_MaterialDefinitionError::Raise ("Bad value for SetTransparency < 0. or > 1.0");
  }

  myTransparencyCoef  = Standard_ShortReal (theValue);
}

// =======================================================================
// function : SetRefractionIndex
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::SetRefractionIndex (const Standard_Real theValue)
{
  if (theValue < 1.0)
  {
    Graphic3d_MaterialDefinitionError::Raise ("Bad value for refraction index < 1.0");
  }

  myRefractionIndex = static_cast<Standard_ShortReal> (theValue);
}

// =======================================================================
// function : SetBSDF
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::SetBSDF (const Graphic3d_BSDF& theBSDF)
{
  myBSDF = theBSDF;
}

// =======================================================================
// function : Color
// purpose  :
// =======================================================================
const Quantity_Color& Graphic3d_MaterialAspect::Color() const
{
  // It is generally accepted to consider diffuse color as
  // "general" color of a material when light shines on it
  return myDiffuseColor;
}

// =======================================================================
// function : AmbientColor
// purpose  :
// =======================================================================
const Quantity_Color& Graphic3d_MaterialAspect::AmbientColor() const
{
  return myAmbientColor;
}

// =======================================================================
// function : DiffuseColor
// purpose  :
// =======================================================================
const Quantity_Color& Graphic3d_MaterialAspect::DiffuseColor() const
{
  return myDiffuseColor;
}

// =======================================================================
// function : SpecularColor
// purpose  :
// =======================================================================
const Quantity_Color& Graphic3d_MaterialAspect::SpecularColor() const
{
  return mySpecularColor;
}

// =======================================================================
// function : EmissiveColor
// purpose  :
// =======================================================================
const Quantity_Color& Graphic3d_MaterialAspect::EmissiveColor() const
{
  return myEmissiveColor;
}

// =======================================================================
// function : MaterialType
// purpose  :
// =======================================================================
Standard_Boolean Graphic3d_MaterialAspect::MaterialType (const Graphic3d_TypeOfMaterial theType) const
{
  return myMaterialType == theType;
}

// =======================================================================
// function : ReflectionMode
// purpose  :
// =======================================================================
Standard_Boolean Graphic3d_MaterialAspect::ReflectionMode (const Graphic3d_TypeOfReflection theType) const
{
  switch (theType)
  {
    case Graphic3d_TOR_AMBIENT:  return myAmbientActivity;
    case Graphic3d_TOR_DIFFUSE:  return myDiffuseActivity;
    case Graphic3d_TOR_SPECULAR: return mySpecularActivity;
    case Graphic3d_TOR_EMISSION: return myEmissiveActivity;
  }
  return Standard_False;
}

// =======================================================================
// function : Ambient
// purpose  :
// =======================================================================
Standard_Real Graphic3d_MaterialAspect::Ambient() const
{
  return Standard_Real (myAmbientCoef);
}

// =======================================================================
// function : Diffuse
// purpose  :
// =======================================================================
Standard_Real Graphic3d_MaterialAspect::Diffuse() const
{
  return Standard_Real (myDiffuseCoef);
}

// =======================================================================
// function : Emissive
// purpose  :
// =======================================================================
Standard_Real Graphic3d_MaterialAspect::Emissive() const
{
  return Standard_Real (myEmissiveCoef);
}

// =======================================================================
// function : Specular
// purpose  :
// =======================================================================
Standard_Real Graphic3d_MaterialAspect::Specular() const
{
  return Standard_Real (mySpecularCoef);
}

// =======================================================================
// function : Transparency
// purpose  :
// =======================================================================
Standard_Real Graphic3d_MaterialAspect::Transparency() const
{
  return Standard_Real (myTransparencyCoef);
}

// =======================================================================
// function : RefractionIndex
// purpose  :
// =======================================================================
Standard_Real Graphic3d_MaterialAspect::RefractionIndex() const
{
  return myRefractionIndex;
}

// =======================================================================
// function : BSDF
// purpose  :
// =======================================================================
const Graphic3d_BSDF& Graphic3d_MaterialAspect::BSDF() const
{
  return myBSDF;
}

// =======================================================================
// function : Shininess
// purpose  :
// =======================================================================
Standard_Real Graphic3d_MaterialAspect::Shininess() const
{
  return Standard_Real (myShininess);
}

// =======================================================================
// function : SetShininess
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::SetShininess (const Standard_Real theValue)
{
  if (theValue < 0.0
   || theValue > 1.0)
  {
    Graphic3d_MaterialDefinitionError::Raise ("Bad value for Shininess < 0. or > 1.0");
  }

  myShininess = Standard_ShortReal (theValue);
  SetMaterialName ("UserDefined");
}

// =======================================================================
// function : SetEnvReflexion
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::SetEnvReflexion (const Standard_ShortReal theValue)
{
  myEnvReflexion = theValue;
}

// =======================================================================
// function : EnvReflexion
// purpose  :
// =======================================================================
Standard_ShortReal Graphic3d_MaterialAspect::EnvReflexion() const
{
  return myEnvReflexion;
}

// =======================================================================
// function : Name
// purpose  :
// =======================================================================
Graphic3d_NameOfMaterial Graphic3d_MaterialAspect::Name() const
{
  return myMaterialName;
}

// =======================================================================
// function : Reset
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::Reset()
{
  Init (myRequestedMaterialName);
}

// =======================================================================
// function : IsEqual
// purpose  :
// =======================================================================
Standard_Boolean Graphic3d_MaterialAspect::IsEqual (const Graphic3d_MaterialAspect& theOther) const
{
  return myAmbientCoef      == theOther.myAmbientCoef
      && myDiffuseCoef      == theOther.myDiffuseCoef
      && mySpecularCoef     == theOther.mySpecularCoef
      && myEmissiveCoef     == theOther.myEmissiveCoef
      && myTransparencyCoef == theOther.myTransparencyCoef
      && myRefractionIndex  == theOther.myRefractionIndex
      && myBSDF             == theOther.myBSDF
      && myShininess        == theOther.myShininess
      && myEnvReflexion     == theOther.myEnvReflexion
      && myAmbientColor     == theOther.myAmbientColor
      && myDiffuseColor     == theOther.myDiffuseColor
      && mySpecularColor    == theOther.mySpecularColor
      && myEmissiveColor    == theOther.myEmissiveColor
      && myAmbientActivity  == theOther.myAmbientActivity
      && myDiffuseActivity  == theOther.myDiffuseActivity
      && mySpecularActivity == theOther.mySpecularActivity
      && myEmissiveActivity == theOther.myEmissiveActivity;
}

// =======================================================================
// function : IsDifferent
// purpose  :
// =======================================================================
Standard_Boolean Graphic3d_MaterialAspect::IsDifferent (const Graphic3d_MaterialAspect& theOther) const
{
  return !IsEqual (theOther);
}

typedef struct _Material
{
  const char*              name;
  Graphic3d_TypeOfMaterial type;
} Material;

namespace
{
  static const Material theMaterials[] =
  {
    {"Brass",            Graphic3d_MATERIAL_PHYSIC},
    {"Bronze",           Graphic3d_MATERIAL_PHYSIC},
    {"Copper",           Graphic3d_MATERIAL_PHYSIC},
    {"Gold",             Graphic3d_MATERIAL_PHYSIC},
    {"Pewter",           Graphic3d_MATERIAL_PHYSIC},
    {"Plastered",        Graphic3d_MATERIAL_ASPECT},
    {"Plastified",       Graphic3d_MATERIAL_ASPECT},
    {"Silver",           Graphic3d_MATERIAL_PHYSIC},
    {"Steel",            Graphic3d_MATERIAL_PHYSIC},
    {"Stone",            Graphic3d_MATERIAL_PHYSIC},
    {"Shiny_plastified", Graphic3d_MATERIAL_ASPECT},
    {"Satined",          Graphic3d_MATERIAL_ASPECT},
    {"Metalized",        Graphic3d_MATERIAL_ASPECT},
    {"Ionized",          Graphic3d_MATERIAL_ASPECT},
    {"Chrome",           Graphic3d_MATERIAL_PHYSIC},
    {"Aluminium",        Graphic3d_MATERIAL_PHYSIC},
    {"Obsidian",         Graphic3d_MATERIAL_PHYSIC},
    {"Neon",             Graphic3d_MATERIAL_PHYSIC},
    {"Jade",             Graphic3d_MATERIAL_PHYSIC},
    {"Charcoal",         Graphic3d_MATERIAL_PHYSIC},
    {"Water",            Graphic3d_MATERIAL_PHYSIC},
    {"Glass",            Graphic3d_MATERIAL_PHYSIC},
    {"Diamond",          Graphic3d_MATERIAL_PHYSIC},
    {"Transparent",      Graphic3d_MATERIAL_PHYSIC}

  };
}

// =======================================================================
// function : NumberOfMaterials
// purpose  :
// =======================================================================
Standard_Integer Graphic3d_MaterialAspect::NumberOfMaterials()
{
  Standard_STATIC_ASSERT(sizeof(theMaterials)/sizeof(Material) == Graphic3d_NOM_DEFAULT);
  return Graphic3d_NOM_DEFAULT;
}

// =======================================================================
// function : MaterialName
// purpose  :
// =======================================================================
Standard_CString Graphic3d_MaterialAspect::MaterialName (const Standard_Integer theRank)
{
  if (theRank < 1 || theRank > NumberOfMaterials())
  {
    Standard_OutOfRange::Raise ("BAD index of material");
  }
  return theMaterials[theRank - 1].name;
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
  for (Standard_Integer aMatIter = 1; aMatIter <= aNbMaterials; ++aMatIter)
  {
    if (aName == Graphic3d_MaterialAspect::MaterialName (aMatIter))
    {
      return Graphic3d_NameOfMaterial(aMatIter - 1);
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
    Standard_OutOfRange::Raise ("BAD index of material");
  }
  return theMaterials[theRank - 1].type;
}

// =======================================================================
// function : MaterialName
// purpose  :
// =======================================================================
Standard_CString Graphic3d_MaterialAspect::MaterialName() const
{
  return myStringName.ToCString();
}

// =======================================================================
// function : SetMaterialName
// purpose  :
// =======================================================================
void Graphic3d_MaterialAspect::SetMaterialName (const Standard_CString theNewName)
{
  // if a component of a "standard" material change, the
  // result is no more standard (a blue gold is not a gold)
  myMaterialName = Graphic3d_NOM_UserDefined;
  myStringName   = theNewName;
}
