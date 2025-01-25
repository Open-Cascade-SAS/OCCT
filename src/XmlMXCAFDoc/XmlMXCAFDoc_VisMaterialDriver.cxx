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

#include <XmlMXCAFDoc_VisMaterialDriver.hxx>

#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <XCAFDoc_VisMaterial.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Document.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMXCAFDoc_VisMaterialDriver, XmlMDF_ADriver)

IMPLEMENT_DOMSTRING(IsDoubleSided, "isdoublesided")
IMPLEMENT_DOMSTRING(AlphaMode, "alpha_mode")
IMPLEMENT_DOMSTRING(AlphaCutOff, "alpha_cutoff")
//
IMPLEMENT_DOMSTRING(BaseColor, "base_color")
IMPLEMENT_DOMSTRING(RefractionIndex, "ior")
IMPLEMENT_DOMSTRING(EmissiveFactor, "emissive_factor")
IMPLEMENT_DOMSTRING(Metallic, "metallic")
IMPLEMENT_DOMSTRING(Roughness, "roughness")
IMPLEMENT_DOMSTRING(BaseColorTexture, "base_color_texture")
IMPLEMENT_DOMSTRING(MetallicRoughnessTexture, "metallic_roughness_texture")
IMPLEMENT_DOMSTRING(EmissiveTexture, "emissive_texture")
IMPLEMENT_DOMSTRING(OcclusionTexture, "occlusion_texture")
IMPLEMENT_DOMSTRING(NormalTexture, "normal_texture")
//
IMPLEMENT_DOMSTRING(AmbientColor, "ambient_color")
IMPLEMENT_DOMSTRING(DiffuseColor, "diffuse_color")
IMPLEMENT_DOMSTRING(SpecularColor, "specular_color")
IMPLEMENT_DOMSTRING(EmissiveColor, "emissive_color")
IMPLEMENT_DOMSTRING(Shininess, "shininess")
IMPLEMENT_DOMSTRING(Transparency, "transparency")
IMPLEMENT_DOMSTRING(DiffuseTexture, "diffuse_texture")
IMPLEMENT_DOMSTRING(FilePath, "file_path")
IMPLEMENT_DOMSTRING(TextureId, "texture_id")
IMPLEMENT_DOMSTRING(Offset, "offset")
IMPLEMENT_DOMSTRING(Length, "length")

//! Encode alpha mode into character.
static const char* alphaModeToString(Graphic3d_AlphaMode theMode)
{
  switch (theMode)
  {
    case Graphic3d_AlphaMode_Opaque:
      return "Opaque";
    case Graphic3d_AlphaMode_Mask:
      return "Mask";
    case Graphic3d_AlphaMode_Blend:
      return "Blend";
    case Graphic3d_AlphaMode_MaskBlend:
      return "MaskBlend";
    case Graphic3d_AlphaMode_BlendAuto:
      return "Auto";
  }
  return "Auto";
}

//! Decode alpha mode from string.
static Graphic3d_AlphaMode alphaModeFromString(const char* theMode)
{
  if (strcasecmp(theMode, "Opaque") == 0)
  {
    return Graphic3d_AlphaMode_Opaque;
  }
  else if (strcasecmp(theMode, "Mask") == 0)
  {
    return Graphic3d_AlphaMode_Mask;
  }
  else if (strcasecmp(theMode, "Blend") == 0)
  {
    return Graphic3d_AlphaMode_Blend;
  }
  else if (strcasecmp(theMode, "MaskBlend") == 0)
  {
    return Graphic3d_AlphaMode_MaskBlend;
  }
  else if (strcasecmp(theMode, "Auto") == 0)
  {
    return Graphic3d_AlphaMode_BlendAuto;
  }
  return Graphic3d_AlphaMode_BlendAuto;
}

//! Encode short real value.
static void writeReal(XmlObjMgt_Persistent&      theTarget,
                      const XmlObjMgt_DOMString& theName,
                      const Standard_ShortReal   theValue)
{
  theTarget.Element().setAttribute(theName, TCollection_AsciiString(theValue).ToCString());
}

//! Encode short real value.
static bool readReal(const XmlObjMgt_Element&   theElement,
                     const XmlObjMgt_DOMString& theName,
                     Standard_ShortReal&        theValue)
{
  Standard_Real aValue = 0.0;
  if (XmlObjMgt::GetReal(theElement.getAttribute(theName), aValue))
  {
    theValue = (Standard_ShortReal)aValue;
    return true;
  }
  return false;
}

//! Encode vec3.
static void writeVec3(XmlObjMgt_Persistent&      theTarget,
                      const XmlObjMgt_DOMString& theName,
                      const Graphic3d_Vec3&      theVec3)
{
  TCollection_AsciiString aString =
    TCollection_AsciiString() + theVec3[0] + " " + theVec3[1] + " " + theVec3[2];
  theTarget.Element().setAttribute(theName, aString.ToCString());
}

//! Decode vec3.
static bool readVec3(const XmlObjMgt_Element&   theElement,
                     const XmlObjMgt_DOMString& theName,
                     Graphic3d_Vec3&            theVec3)
{
  Graphic3d_Vec3 aVec3;
  LDOMString     aString = theElement.getAttribute(theName);
  const char*    aPos    = aString.GetString();
  char*          aNext   = NULL;
  aVec3[0]               = (float)Strtod(aPos, &aNext);
  aPos                   = aNext;
  aVec3[1]               = (float)Strtod(aPos, &aNext);
  aPos                   = aNext;
  aVec3[2]               = (float)Strtod(aPos, &aNext);
  if (aPos != aNext)
  {
    theVec3 = aVec3;
    return true;
  }
  return false;
}

//! Decode vec3.
static bool readColor(const XmlObjMgt_Element&   theElement,
                      const XmlObjMgt_DOMString& theName,
                      Quantity_Color&            theColor)
{
  Graphic3d_Vec3 aVec3;
  if (readVec3(theElement, theName, aVec3))
  {
    theColor = Quantity_Color(aVec3);
    return true;
  }
  return false;
}

//! Encode vec4.
static void writeVec4(XmlObjMgt_Persistent&      theTarget,
                      const XmlObjMgt_DOMString& theName,
                      const Graphic3d_Vec4&      theVec4)
{
  TCollection_AsciiString aString =
    TCollection_AsciiString() + theVec4[0] + " " + theVec4[1] + " " + theVec4[2] + " " + theVec4[3];
  theTarget.Element().setAttribute(theName, aString.ToCString());
}

//! Decode vec34
static bool readVec4(const XmlObjMgt_Element&   theElement,
                     const XmlObjMgt_DOMString& theName,
                     Graphic3d_Vec4&            theVec4)
{
  Graphic3d_Vec4 aVec4;
  LDOMString     aString = theElement.getAttribute(theName);
  const char*    aPos    = aString.GetString();
  char*          aNext   = NULL;
  aVec4[0]               = (float)Strtod(aPos, &aNext);
  aPos                   = aNext;
  aVec4[1]               = (float)Strtod(aPos, &aNext);
  aPos                   = aNext;
  aVec4[2]               = (float)Strtod(aPos, &aNext);
  aPos                   = aNext;
  aVec4[3]               = (float)Strtod(aPos, &aNext);
  if (aPos != aNext)
  {
    theVec4 = aVec4;
    return true;
  }
  return false;
}

//! Decode vec4.
static bool readColor(const XmlObjMgt_Element&   theElement,
                      const XmlObjMgt_DOMString& theName,
                      Quantity_ColorRGBA&        theColor)
{
  Graphic3d_Vec4 aVec4;
  if (readVec4(theElement, theName, aVec4))
  {
    theColor = Quantity_ColorRGBA(aVec4);
    return true;
  }
  return false;
}

//! Encode texture path.
static void writeTexture(XmlObjMgt_Persistent&        theTarget,
                         const XmlObjMgt_DOMString&   theName,
                         const Handle(Image_Texture)& theImage)
{
  if (theImage.IsNull())
  {
    return;
  }
  XmlObjMgt_Document aDoc(theTarget.Element().getOwnerDocument());
  XmlObjMgt_Element  aCurTarget = aDoc.createElement(theName);
  theTarget.Element().appendChild(aCurTarget);
  if (theImage->DataBuffer().IsNull())
  {
    aCurTarget.setAttribute(::FilePath(), theImage->FilePath().ToCString());
    if (theImage->FileOffset() == -1 || theImage->FileLength() == -1)
    {
      return;
    }
    aCurTarget.setAttribute(::Offset(), static_cast<int>(theImage->FileOffset()));
    aCurTarget.setAttribute(::Length(), static_cast<int>(theImage->FileLength()));
  }
  else
  {
    Message::SendWarning()
      << "Warning: XmlMXCAFDoc_VisMaterialDriver : Can't write a texture to buffer.";
  }
}

//! Decode texture path.
static void readTexture(const XmlObjMgt_Element&   theElement,
                        const XmlObjMgt_DOMString& theName,
                        Handle(Image_Texture)&     theImage)
{
  TCollection_AsciiString aStr(theElement.getAttribute(theName).GetString());
  if (!aStr.IsEmpty())
  {
    theImage = new Image_Texture(aStr);
    return;
  }
  LDOM_Element anElement = theElement.GetChildByTagName(theName);
  if (anElement.isNull())
  {
    return;
  }
  TCollection_AsciiString aFilePath(anElement.getAttribute(::FilePath()).GetString());
  TCollection_AsciiString anId(anElement.getAttribute(::TextureId()).GetString());
  Standard_Integer        anOffset = -1, aLength = -1;
  if (!aFilePath.IsEmpty())
  {
    anElement.getAttribute(::Offset()).GetInteger(anOffset);
    anElement.getAttribute(::Length()).GetInteger(aLength);
    if (anOffset == -1 || aLength == -1)
    {
      theImage = new Image_Texture(aFilePath);
      return;
    }
    theImage = new Image_Texture(aFilePath, anOffset, aLength);
  }
  else if (!anId.IsEmpty())
  {
    Message::SendWarning()
      << "Warning: XmlMXCAFDoc_VisMaterialDriver : Can't write a texture to buffer.";
  }
}

//=================================================================================================

XmlMXCAFDoc_VisMaterialDriver::XmlMXCAFDoc_VisMaterialDriver(
  const Handle(Message_Messenger)& theMsgDriver)
    : XmlMDF_ADriver(theMsgDriver, "xcaf", "VisMaterial")
{
  //
}

//=================================================================================================

Handle(TDF_Attribute) XmlMXCAFDoc_VisMaterialDriver::NewEmpty() const
{
  return new XCAFDoc_VisMaterial();
}

//=======================================================================
// function : Paste
// purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMXCAFDoc_VisMaterialDriver::Paste(const XmlObjMgt_Persistent&  theSource,
                                                      const Handle(TDF_Attribute)& theTarget,
                                                      XmlObjMgt_RRelocationTable&) const
{
  Handle(XCAFDoc_VisMaterial) aMat = Handle(XCAFDoc_VisMaterial)::DownCast(theTarget);

  const XmlObjMgt_DOMString aDoubleSidedStr = theSource.Element().getAttribute(::IsDoubleSided());
  Standard_Integer          aDoubleSidedInt = 1;
  aDoubleSidedStr.GetInteger(aDoubleSidedInt);
  Standard_ShortReal anAlphaCutOff = 0.5f;
  readReal(theSource, ::AlphaCutOff(), anAlphaCutOff);
  switch (aDoubleSidedInt)
  {
    case 1:
      aMat->SetFaceCulling(Graphic3d_TypeOfBackfacingModel_DoubleSided);
      break;
    case 2:
      aMat->SetFaceCulling(Graphic3d_TypeOfBackfacingModel_BackCulled);
      break;
    case 3:
      aMat->SetFaceCulling(Graphic3d_TypeOfBackfacingModel_FrontCulled);
      break;
    case 0:
    default:
      aMat->SetFaceCulling(Graphic3d_TypeOfBackfacingModel_Auto);
      break;
  }
  aMat->SetAlphaMode(
    alphaModeFromString(theSource.Element().getAttribute(::AlphaMode()).GetString()),
    anAlphaCutOff);

  Quantity_ColorRGBA aBaseColor;
  if (readColor(theSource, ::BaseColor(), aBaseColor))
  {
    XCAFDoc_VisMaterialPBR aPbrMat;
    aPbrMat.IsDefined = true;
    aPbrMat.BaseColor = aBaseColor;
    readVec3(theSource, ::EmissiveFactor(), aPbrMat.EmissiveFactor);
    readReal(theSource, ::Metallic(), aPbrMat.Metallic);
    readReal(theSource, ::Roughness(), aPbrMat.Roughness);
    readReal(theSource, ::RefractionIndex(), aPbrMat.RefractionIndex);
    readTexture(theSource, ::BaseColorTexture(), aPbrMat.BaseColorTexture);
    readTexture(theSource, ::MetallicRoughnessTexture(), aPbrMat.MetallicRoughnessTexture);
    readTexture(theSource, ::EmissiveTexture(), aPbrMat.EmissiveTexture);
    readTexture(theSource, ::OcclusionTexture(), aPbrMat.OcclusionTexture);
    readTexture(theSource, ::NormalTexture(), aPbrMat.NormalTexture);
    aMat->SetPbrMaterial(aPbrMat);
  }

  Quantity_Color aDiffColor;
  if (readColor(theSource, ::DiffuseColor(), aDiffColor))
  {
    XCAFDoc_VisMaterialCommon aComMat;
    aComMat.IsDefined    = true;
    aComMat.DiffuseColor = aDiffColor;
    readColor(theSource, ::AmbientColor(), aComMat.AmbientColor);
    readColor(theSource, ::SpecularColor(), aComMat.SpecularColor);
    readColor(theSource, ::EmissiveColor(), aComMat.EmissiveColor);
    readReal(theSource, ::Shininess(), aComMat.Shininess);
    readReal(theSource, ::Transparency(), aComMat.Transparency);
    readTexture(theSource, ::DiffuseTexture(), aComMat.DiffuseTexture);
    aMat->SetCommonMaterial(aComMat);
  }
  return Standard_True;
}

//=======================================================================
// function : Paste
// purpose  : transient -> persistent (store)
//=======================================================================
void XmlMXCAFDoc_VisMaterialDriver::Paste(const Handle(TDF_Attribute)& theSource,
                                          XmlObjMgt_Persistent&        theTarget,
                                          XmlObjMgt_SRelocationTable&) const
{
  Handle(XCAFDoc_VisMaterial) aMat            = Handle(XCAFDoc_VisMaterial)::DownCast(theSource);
  Standard_Integer            aDoubleSidedInt = 0;
  switch (aMat->FaceCulling())
  {
    case Graphic3d_TypeOfBackfacingModel_DoubleSided:
      aDoubleSidedInt = 1;
      break;
    case Graphic3d_TypeOfBackfacingModel_BackCulled:
      aDoubleSidedInt = 2;
      break;
    case Graphic3d_TypeOfBackfacingModel_FrontCulled:
      aDoubleSidedInt = 3;
      break;
    case Graphic3d_TypeOfBackfacingModel_Auto:
      aDoubleSidedInt = 0;
      break;
  }
  theTarget.Element().setAttribute(::IsDoubleSided(), aDoubleSidedInt);
  theTarget.Element().setAttribute(::AlphaMode(), alphaModeToString(aMat->AlphaMode()));
  writeReal(theTarget, ::AlphaCutOff(), aMat->AlphaCutOff());
  if (aMat->HasPbrMaterial())
  {
    const XCAFDoc_VisMaterialPBR& aPbrMat = aMat->PbrMaterial();
    writeVec4(theTarget, ::BaseColor(), aPbrMat.BaseColor);
    writeVec3(theTarget, ::EmissiveFactor(), aPbrMat.EmissiveFactor);
    writeReal(theTarget, ::Metallic(), aPbrMat.Metallic);
    writeReal(theTarget, ::Roughness(), aPbrMat.Roughness);
    writeReal(theTarget, ::RefractionIndex(), aPbrMat.RefractionIndex);
    writeTexture(theTarget, ::BaseColorTexture(), aPbrMat.BaseColorTexture);
    writeTexture(theTarget, ::MetallicRoughnessTexture(), aPbrMat.MetallicRoughnessTexture);
    writeTexture(theTarget, ::EmissiveTexture(), aPbrMat.EmissiveTexture);
    writeTexture(theTarget, ::OcclusionTexture(), aPbrMat.OcclusionTexture);
    writeTexture(theTarget, ::NormalTexture(), aPbrMat.NormalTexture);
  }

  if (aMat->HasCommonMaterial())
  {
    const XCAFDoc_VisMaterialCommon& aComMat = aMat->CommonMaterial();
    writeVec3(theTarget, ::AmbientColor(), aComMat.AmbientColor);
    writeVec3(theTarget, ::DiffuseColor(), aComMat.DiffuseColor);
    writeVec3(theTarget, ::SpecularColor(), aComMat.SpecularColor);
    writeVec3(theTarget, ::EmissiveColor(), aComMat.EmissiveColor);
    writeReal(theTarget, ::Shininess(), aComMat.Shininess);
    writeReal(theTarget, ::Transparency(), aComMat.Transparency);
    writeTexture(theTarget, ::DiffuseTexture(), aComMat.DiffuseTexture);
  }
}
