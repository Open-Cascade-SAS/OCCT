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

#include <BinMXCAFDoc_VisMaterialDriver.hxx>

#include <BinObjMgt_Persistent.hxx>
#include <Message_Messenger.hxx>
#include <XCAFDoc_VisMaterial.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMXCAFDoc_VisMaterialDriver, BinMDF_ADriver)

//! Encode alpha mode into character.
static Standard_Byte alphaModeToChar (Graphic3d_AlphaMode theMode)
{
  switch (theMode)
  {
    case Graphic3d_AlphaMode_Opaque:    return 'O';
    case Graphic3d_AlphaMode_Mask:      return 'M';
    case Graphic3d_AlphaMode_Blend:     return 'B';
    case Graphic3d_AlphaMode_MaskBlend: return 'b';
    case Graphic3d_AlphaMode_BlendAuto: return 'A';
  }
  return 'A';
}

//! Decode alpha mode from character.
static Graphic3d_AlphaMode alphaModeFromChar (Standard_Byte theMode)
{
  switch (theMode)
  {
    case 'O': return Graphic3d_AlphaMode_Opaque;
    case 'M': return Graphic3d_AlphaMode_Mask;
    case 'B': return Graphic3d_AlphaMode_Blend;
    case 'b': return Graphic3d_AlphaMode_MaskBlend;
    case 'A': return Graphic3d_AlphaMode_BlendAuto;
  }
  return Graphic3d_AlphaMode_BlendAuto;
}

//! Encode face culling mode into character.
static Standard_Byte faceCullToChar (Graphic3d_TypeOfBackfacingModel theMode)
{
  switch (theMode)
  {
    case Graphic3d_TypeOfBackfacingModel_Auto:        return '0';
    case Graphic3d_TypeOfBackfacingModel_BackCulled:  return 'B';
    case Graphic3d_TypeOfBackfacingModel_FrontCulled: return 'F';
    case Graphic3d_TypeOfBackfacingModel_DoubleSided: return '1';
  }
  return '0';
}

//! Decode face culling mode from character.
static Graphic3d_TypeOfBackfacingModel faceCullFromChar (Standard_Byte theMode)
{
  switch (theMode)
  {
    case '0': return Graphic3d_TypeOfBackfacingModel_Auto;
    case 'B': return Graphic3d_TypeOfBackfacingModel_BackCulled;
    case 'F': return Graphic3d_TypeOfBackfacingModel_FrontCulled;
    case '1': return Graphic3d_TypeOfBackfacingModel_DoubleSided;
  }
  return Graphic3d_TypeOfBackfacingModel_Auto;
}

//! Encode vec3.
static void writeVec3 (BinObjMgt_Persistent& theTarget,
                       const Graphic3d_Vec3& theVec3)
{
  theTarget.PutShortReal (theVec3[0]);
  theTarget.PutShortReal (theVec3[1]);
  theTarget.PutShortReal (theVec3[2]);
}

//! Encode vec4.
static void writeVec4 (BinObjMgt_Persistent& theTarget,
                       const Graphic3d_Vec4& theVec4)
{
  theTarget.PutShortReal (theVec4[0]);
  theTarget.PutShortReal (theVec4[1]);
  theTarget.PutShortReal (theVec4[2]);
  theTarget.PutShortReal (theVec4[3]);
}

//! Decode vec3.
static void readVec3 (const BinObjMgt_Persistent& theSource,
                      Graphic3d_Vec3& theVec3)
{
  theSource.GetShortReal (theVec3[0]);
  theSource.GetShortReal (theVec3[1]);
  theSource.GetShortReal (theVec3[2]);
}

//! Decode vec3.
static void readColor (const BinObjMgt_Persistent& theSource,
                       Quantity_Color& theColor)
{
  Graphic3d_Vec3 aVec3;
  readVec3 (theSource, aVec3);
  theColor = Quantity_Color (aVec3);
}

//! Decode vec4.
static void readColor (const BinObjMgt_Persistent& theSource,
                       Quantity_ColorRGBA& theColor)
{
  Graphic3d_Vec4 aVec4;
  theSource.GetShortReal (aVec4[0]);
  theSource.GetShortReal (aVec4[1]);
  theSource.GetShortReal (aVec4[2]);
  theSource.GetShortReal (aVec4[3]);
  theColor = Quantity_ColorRGBA (aVec4);
}

//! Encode texture path.
static void writeTexture (BinObjMgt_Persistent& theTarget,
                          const Handle(Image_Texture)& theImage)
{
  if (theImage.IsNull())
  {
    theTarget.PutAsciiString("");
    return;
  }
  if (theImage->DataBuffer().IsNull())
  {
    theTarget.PutAsciiString(theImage->FilePath());
    theTarget.PutBoolean(false);
    if (theImage->FileOffset() == -1 || theImage->FileLength() == -1)
    {
      theTarget.PutBoolean(true);
      return;
    }
    theTarget.PutBoolean(false);
    theTarget.PutInteger(static_cast<int>(theImage->FileOffset()));
    theTarget.PutInteger(static_cast<int>(theImage->FileLength()));
    return;
  }
  theTarget.PutAsciiString(theImage->TextureId());
  theTarget.PutBoolean(true);
  theTarget.PutInteger(static_cast<int>(theImage->DataBuffer()->Size()));
  theTarget.PutByteArray((Standard_Byte*)theImage->DataBuffer()->Data(),
                         static_cast<int>(theImage->DataBuffer()->Size()));
}

//! Decode texture path.
static void readTexture (const BinObjMgt_Persistent& theSource,
                         Handle(Image_Texture)& theTexture)
{
  TCollection_AsciiString aStr;
  theSource.GetAsciiString(aStr);
  if (aStr.IsEmpty())
  {
    return;
  }
  Standard_Boolean anUseBuffer;
  if (!theSource.GetBoolean(anUseBuffer).IsOK())
  {
    theTexture = new Image_Texture(aStr);
    return;
  }
  Standard_Integer anOffset = -1, aLength = -1;
  if (!anUseBuffer)
  {
    Standard_Boolean isOnlyFilePath;
    theSource.GetBoolean(isOnlyFilePath);
    if (isOnlyFilePath)
    {
      theTexture = new Image_Texture(aStr);
      return;
    }
    theSource.GetInteger(anOffset);
    theSource.GetInteger(aLength);
    theTexture = new Image_Texture(aStr, anOffset, aLength);
    return;
  }
  theSource.GetInteger(aLength);
  Handle(NCollection_Buffer) aBuff =
    new NCollection_Buffer(NCollection_BaseAllocator::CommonBaseAllocator(), aLength);
  theSource.GetByteArray(aBuff->ChangeData(), aLength);
  theTexture = new Image_Texture(aBuff, aStr);
}

//=======================================================================
//function : Constructor
//purpose  :
//=======================================================================
BinMXCAFDoc_VisMaterialDriver::BinMXCAFDoc_VisMaterialDriver (const Handle(Message_Messenger)& theMsgDriver)
: BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_VisMaterial)->Name())
{
}

//=======================================================================
//function : NewEmpty
//purpose  :
//=======================================================================
Handle(TDF_Attribute) BinMXCAFDoc_VisMaterialDriver::NewEmpty() const
{
  return new XCAFDoc_VisMaterial();
}

//=======================================================================
//function : Paste
//purpose  :
//=======================================================================
Standard_Boolean BinMXCAFDoc_VisMaterialDriver::Paste (const BinObjMgt_Persistent&  theSource,
                                                       const Handle(TDF_Attribute)& theTarget,
                                                       BinObjMgt_RRelocationTable& /*theRelocTable*/) const
{
  Handle(XCAFDoc_VisMaterial) aMat = Handle(XCAFDoc_VisMaterial)::DownCast(theTarget);
  Standard_Byte aVerMaj = 0, aVerMin = 0;
  theSource.GetByte (aVerMaj);
  theSource.GetByte (aVerMin);
  if (aVerMaj < 1 || aVerMaj > MaterialVersionMajor)
  {
    myMessageDriver->Send (TCollection_AsciiString ("Skipping XCAFDoc_VisMaterial of unknown version ")
                         + Standard_Integer(aVerMaj) + "." + Standard_Integer(aVerMin)
                         + " (supported version: " + Standard_Integer(MaterialVersionMajor) + "." + Standard_Integer(MaterialVersionMinor) + ")");
    return false;
  }

  Standard_Byte isDoubleSided = 0, anAlphaMode = 0;
  Standard_ShortReal anAlphaCutOff = 0.5f;
  theSource.GetByte (isDoubleSided);
  theSource.GetByte (anAlphaMode);
  theSource.GetShortReal (anAlphaCutOff);
  aMat->SetFaceCulling (faceCullFromChar (isDoubleSided));
  aMat->SetAlphaMode (alphaModeFromChar (anAlphaMode), anAlphaCutOff);

  XCAFDoc_VisMaterialPBR aPbrMat;
  theSource.GetBoolean (aPbrMat.IsDefined);
  if (aPbrMat.IsDefined)
  {
    readColor (theSource, aPbrMat.BaseColor);
    readVec3  (theSource, aPbrMat.EmissiveFactor);
    theSource.GetShortReal (aPbrMat.Metallic);
    theSource.GetShortReal (aPbrMat.Roughness);
    readTexture (theSource, aPbrMat.BaseColorTexture);
    readTexture (theSource, aPbrMat.MetallicRoughnessTexture);
    readTexture (theSource, aPbrMat.EmissiveTexture);
    readTexture (theSource, aPbrMat.OcclusionTexture);
    readTexture (theSource, aPbrMat.NormalTexture);
    aMat->SetPbrMaterial (aPbrMat);
  }

  bool hasComMat = false;
  theSource.GetBoolean (hasComMat);
  if (hasComMat)
  {
    XCAFDoc_VisMaterialCommon aComMat;
    aComMat.IsDefined = true;
    readColor (theSource, aComMat.AmbientColor);
    readColor (theSource, aComMat.DiffuseColor);
    readColor (theSource, aComMat.SpecularColor);
    readColor (theSource, aComMat.EmissiveColor);
    theSource.GetShortReal (aComMat.Shininess);
    theSource.GetShortReal (aComMat.Transparency);
    readTexture (theSource, aComMat.DiffuseTexture);
    aMat->SetCommonMaterial (aComMat);
  }

  if (aVerMaj > MaterialVersionMajor_1
    || (aVerMaj == MaterialVersionMajor_1
     && aVerMin >= MaterialVersionMinor_1))
  {
    if (aPbrMat.IsDefined)
    {
      theSource.GetShortReal (aPbrMat.RefractionIndex);
    }
  }

  if (aPbrMat.IsDefined)
  {
    aMat->SetPbrMaterial (aPbrMat);
  }

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  :
//=======================================================================
void BinMXCAFDoc_VisMaterialDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                           BinObjMgt_Persistent& theTarget,
                                           BinObjMgt_SRelocationTable& ) const
{
  Handle(XCAFDoc_VisMaterial) aMat = Handle(XCAFDoc_VisMaterial)::DownCast(theSource);
  theTarget.PutByte (MaterialVersionMajor);
  theTarget.PutByte (MaterialVersionMinor);

  theTarget.PutByte (faceCullToChar (aMat->FaceCulling()));
  theTarget.PutByte (alphaModeToChar (aMat->AlphaMode()));
  theTarget.PutShortReal (aMat->AlphaCutOff());

  theTarget.PutBoolean (aMat->HasPbrMaterial());
  if (aMat->HasPbrMaterial())
  {
    const XCAFDoc_VisMaterialPBR& aPbrMat = aMat->PbrMaterial();
    writeVec4 (theTarget, aPbrMat.BaseColor);
    writeVec3 (theTarget, aPbrMat.EmissiveFactor);
    theTarget.PutShortReal (aPbrMat.Metallic);
    theTarget.PutShortReal (aPbrMat.Roughness);
    writeTexture (theTarget, aPbrMat.BaseColorTexture);
    writeTexture (theTarget, aPbrMat.MetallicRoughnessTexture);
    writeTexture (theTarget, aPbrMat.EmissiveTexture);
    writeTexture (theTarget, aPbrMat.OcclusionTexture);
    writeTexture (theTarget, aPbrMat.NormalTexture);
  }

  theTarget.PutBoolean (aMat->HasCommonMaterial());
  if (aMat->HasCommonMaterial())
  {
    const XCAFDoc_VisMaterialCommon& aComMat = aMat->CommonMaterial();
    writeVec3 (theTarget, aComMat.AmbientColor);
    writeVec3 (theTarget, aComMat.DiffuseColor);
    writeVec3 (theTarget, aComMat.SpecularColor);
    writeVec3 (theTarget, aComMat.EmissiveColor);
    theTarget.PutShortReal (aComMat.Shininess);
    theTarget.PutShortReal (aComMat.Transparency);
    writeTexture (theTarget, aComMat.DiffuseTexture);
  }

  if (aMat->HasPbrMaterial())
  {
    theTarget.PutShortReal (aMat->PbrMaterial().RefractionIndex);
  }
}
