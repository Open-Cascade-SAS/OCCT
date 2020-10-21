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

#include <XCAFDoc_VisMaterial.hxx>

#include <Graphic3d_Aspects.hxx>
#include <Graphic3d_MaterialAspect.hxx>
#include <Standard_GUID.hxx>
#include <TDF_Label.hxx>
#include <XCAFPrs_Texture.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDoc_VisMaterial, TDF_Attribute)

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& XCAFDoc_VisMaterial::GetID()
{
  static Standard_GUID THE_VIS_MAT_ID ("EBB00255-03A0-4845-BD3B-A70EEDEEFA78");
  return THE_VIS_MAT_ID;
}

//=======================================================================
//function : Constructor
//purpose  :
//=======================================================================
XCAFDoc_VisMaterial::XCAFDoc_VisMaterial()
: myAlphaMode (Graphic3d_AlphaMode_BlendAuto),
  myAlphaCutOff (0.5f),
  myIsDoubleSided (Standard_True)
{
  //
}

//=======================================================================
//function : SetMetalRougnessMaterial
//purpose  :
//=======================================================================
void XCAFDoc_VisMaterial::SetPbrMaterial (const XCAFDoc_VisMaterialPBR& theMaterial)
{
  Backup();
  myPbrMat = theMaterial;
}

//=======================================================================
//function : SetCommonMaterial
//purpose  :
//=======================================================================
void XCAFDoc_VisMaterial::SetCommonMaterial (const XCAFDoc_VisMaterialCommon& theMaterial)
{
  Backup();
  myCommonMat = theMaterial;
}

//=======================================================================
//function : SetAlphaMode
//purpose  :
//=======================================================================
void XCAFDoc_VisMaterial::SetAlphaMode (Graphic3d_AlphaMode theMode,
                                        Standard_ShortReal  theCutOff)
{
  Backup();
  myAlphaMode   = theMode;
  myAlphaCutOff = theCutOff;
}

//=======================================================================
//function : SetDoubleSided
//purpose  :
//=======================================================================
void XCAFDoc_VisMaterial::SetDoubleSided (Standard_Boolean theIsDoubleSided)
{
  Backup();
  myIsDoubleSided = theIsDoubleSided;
}

//=======================================================================
//function : Restore
//purpose  :
//=======================================================================
void XCAFDoc_VisMaterial::Restore (const Handle(TDF_Attribute)& theWith)
{
  XCAFDoc_VisMaterial* anOther = dynamic_cast<XCAFDoc_VisMaterial* >(theWith.get());
  myPbrMat        = anOther->myPbrMat;
  myCommonMat     = anOther->myCommonMat;
  myAlphaMode     = anOther->myAlphaMode;
  myAlphaCutOff   = anOther->myAlphaCutOff;
  myIsDoubleSided = anOther->myIsDoubleSided;
}

//=======================================================================
//function : NewEmpty
//purpose  :
//=======================================================================
Handle(TDF_Attribute) XCAFDoc_VisMaterial::NewEmpty() const
{
  return new XCAFDoc_VisMaterial();
}

//=======================================================================
//function : Paste
//purpose  :
//=======================================================================
void XCAFDoc_VisMaterial::Paste (const Handle(TDF_Attribute)& theInto,
                                 const Handle(TDF_RelocationTable)& ) const
{
  XCAFDoc_VisMaterial* anOther = dynamic_cast<XCAFDoc_VisMaterial* >(theInto.get());
  anOther->Backup();
  anOther->myPbrMat        = myPbrMat;
  anOther->myCommonMat     = myCommonMat;
  anOther->myAlphaMode     = myAlphaMode;
  anOther->myAlphaCutOff   = myAlphaCutOff;
  anOther->myIsDoubleSided = myIsDoubleSided;
}

// =======================================================================
// function : BaseColor
// purpose  :
// =======================================================================
Quantity_ColorRGBA XCAFDoc_VisMaterial::BaseColor() const
{
  if (myPbrMat.IsDefined)
  {
    return myPbrMat.BaseColor;
  }
  else if (myCommonMat.IsDefined)
  {
    return Quantity_ColorRGBA (myCommonMat.DiffuseColor, 1.0f - myCommonMat.Transparency);
  }
  return Quantity_ColorRGBA (Quantity_NOC_WHITE);
}

//=======================================================================
//function : ConvertToCommonMaterial
//purpose  :
//=======================================================================
XCAFDoc_VisMaterialCommon XCAFDoc_VisMaterial::ConvertToCommonMaterial()
{
  if (myCommonMat.IsDefined)
  {
    return myCommonMat;
  }
  else if (!myPbrMat.IsDefined)
  {
    return XCAFDoc_VisMaterialCommon();
  }

  // convert metal-roughness into common
  XCAFDoc_VisMaterialCommon aComMat;
  aComMat.IsDefined = true;
  aComMat.DiffuseTexture = myPbrMat.BaseColorTexture;
  aComMat.DiffuseColor  = myPbrMat.BaseColor.GetRGB();
  aComMat.SpecularColor = Quantity_Color (Graphic3d_Vec3 (myPbrMat.Metallic));
  aComMat.Transparency = 1.0f - myPbrMat.BaseColor.Alpha();
  aComMat.Shininess    = 1.0f - myPbrMat.Roughness;
  return aComMat;
}

//=======================================================================
//function : ConvertToPbrMaterial
//purpose  :
//=======================================================================
XCAFDoc_VisMaterialPBR XCAFDoc_VisMaterial::ConvertToPbrMaterial()
{
  if (myPbrMat.IsDefined)
  {
    return myPbrMat;
  }
  else if (!myCommonMat.IsDefined)
  {
    return XCAFDoc_VisMaterialPBR();
  }

  XCAFDoc_VisMaterialPBR aPbrMat;
  aPbrMat.IsDefined = true;
  aPbrMat.BaseColorTexture = myCommonMat.DiffuseTexture;
  aPbrMat.BaseColor.SetRGB (myCommonMat.DiffuseColor);
  aPbrMat.BaseColor.SetAlpha (1.0f - myCommonMat.Transparency);
  aPbrMat.Metallic  = Graphic3d_PBRMaterial::MetallicFromSpecular (myCommonMat.SpecularColor);
  aPbrMat.Roughness = Graphic3d_PBRMaterial::RoughnessFromSpecular (myCommonMat.SpecularColor, myCommonMat.Shininess);
  return aPbrMat;
}

//=======================================================================
//function : FillMaterialAspect
//purpose  :
//=======================================================================
void XCAFDoc_VisMaterial::FillMaterialAspect (Graphic3d_MaterialAspect& theAspect) const
{
  if (myCommonMat.IsDefined)
  {
    theAspect = Graphic3d_MaterialAspect (Graphic3d_NameOfMaterial_UserDefined);
    theAspect.SetAmbientColor (myCommonMat.AmbientColor);
    theAspect.SetDiffuseColor (myCommonMat.DiffuseColor);
    theAspect.SetSpecularColor(myCommonMat.SpecularColor);
    theAspect.SetEmissiveColor(myCommonMat.EmissiveColor);
    theAspect.SetTransparency (myCommonMat.Transparency);
    theAspect.SetShininess    (myCommonMat.Shininess);

    // convert common into metal-roughness
    if (!myPbrMat.IsDefined)
    {
      Graphic3d_PBRMaterial aPbr;
      aPbr.SetColor (myCommonMat.DiffuseColor);
      aPbr.SetMetallic (Graphic3d_PBRMaterial::MetallicFromSpecular (myCommonMat.SpecularColor));
      aPbr.SetRoughness (Graphic3d_PBRMaterial::RoughnessFromSpecular (myCommonMat.SpecularColor, myCommonMat.Shininess));
      theAspect.SetPBRMaterial (aPbr);
      theAspect.SetBSDF (Graphic3d_BSDF::CreateMetallicRoughness (aPbr));
    }
  }

  if (myPbrMat.IsDefined)
  {
    if (!myCommonMat.IsDefined)
    {
      // convert metal-roughness into common
      theAspect = Graphic3d_MaterialAspect (Graphic3d_NameOfMaterial_UserDefined);
      theAspect.SetDiffuseColor (myPbrMat.BaseColor.GetRGB());
      theAspect.SetAlpha        (myPbrMat.BaseColor.Alpha());
      theAspect.SetSpecularColor(Quantity_Color (Graphic3d_Vec3 (myPbrMat.Metallic)));
      theAspect.SetShininess    (1.0f - myPbrMat.Roughness);
    }

    Graphic3d_PBRMaterial aPbr;
    aPbr.SetColor    (myPbrMat.BaseColor);
    aPbr.SetMetallic (myPbrMat.Metallic);
    aPbr.SetRoughness(myPbrMat.Roughness);
    aPbr.SetEmission (myPbrMat.EmissiveFactor);
    aPbr.SetIOR      (myPbrMat.RefractionIndex);
    theAspect.SetRefractionIndex (myPbrMat.RefractionIndex);
    theAspect.SetPBRMaterial (aPbr);
    theAspect.SetBSDF (Graphic3d_BSDF::CreateMetallicRoughness (aPbr));
  }
}

//=======================================================================
//function : FillAspect
//purpose  :
//=======================================================================
void XCAFDoc_VisMaterial::FillAspect (const Handle(Graphic3d_Aspects)& theAspect) const
{
  if (IsEmpty())
  {
    return;
  }

  Graphic3d_MaterialAspect aMaterial;
  FillMaterialAspect (aMaterial);
  theAspect->SetFrontMaterial (aMaterial);
  theAspect->SetAlphaMode (myAlphaMode , myAlphaCutOff);
  theAspect->SetSuppressBackFaces (!myIsDoubleSided);

  const Handle(Image_Texture)& aColorTexture = !myPbrMat.BaseColorTexture.IsNull() ? myPbrMat.BaseColorTexture : myCommonMat.DiffuseTexture;
  Standard_Integer aNbTexUnits = 0;
  if (!aColorTexture.IsNull())             { ++aNbTexUnits; }
  if (!myPbrMat.EmissiveTexture.IsNull())  { ++aNbTexUnits; }
  if (!myPbrMat.NormalTexture.IsNull())    { ++aNbTexUnits; }
  if (!myPbrMat.OcclusionTexture.IsNull()) { ++aNbTexUnits; }
  if (!myPbrMat.MetallicRoughnessTexture.IsNull()) { ++aNbTexUnits; }
  if (aNbTexUnits == 0)
  {
    return;
  }

  Standard_Integer aTexIter = 0;
  Handle(Graphic3d_TextureSet) aTextureSet = new Graphic3d_TextureSet (aNbTexUnits);
  if (!aColorTexture.IsNull())
  {
    aTextureSet->SetValue (aTexIter++, new XCAFPrs_Texture (*aColorTexture, Graphic3d_TextureUnit_BaseColor));
  }
  if (!myPbrMat.EmissiveTexture.IsNull())
  {
    aTextureSet->SetValue (aTexIter++, new XCAFPrs_Texture (*myPbrMat.EmissiveTexture, Graphic3d_TextureUnit_Emissive));
  }
  if (!myPbrMat.OcclusionTexture.IsNull())
  {
    aTextureSet->SetValue (aTexIter++, new XCAFPrs_Texture (*myPbrMat.OcclusionTexture, Graphic3d_TextureUnit_Occlusion));
  }
  if (!myPbrMat.NormalTexture.IsNull())
  {
    aTextureSet->SetValue (aTexIter++, new XCAFPrs_Texture (*myPbrMat.NormalTexture, Graphic3d_TextureUnit_Normal));
  }
  if (!myPbrMat.MetallicRoughnessTexture.IsNull())
  {
    aTextureSet->SetValue (aTexIter++, new XCAFPrs_Texture (*myPbrMat.MetallicRoughnessTexture, Graphic3d_TextureUnit_MetallicRoughness));
  }

  theAspect->SetTextureSet (aTextureSet);
  theAspect->SetTextureMapOn (true);
}

//=======================================================================
//function : DumpJson
//purpose  : 
//=======================================================================
void XCAFDoc_VisMaterial::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)

  OCCT_DUMP_BASE_CLASS (theOStream, theDepth, TDF_Attribute)

  OCCT_DUMP_FIELD_VALUE_STRING (theOStream, myRawName.get())

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myPbrMat)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myCommonMat)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myAlphaMode)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myAlphaCutOff)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myIsDoubleSided)
}
