// Copyright (c) 2015-2021 OPEN CASCADE SAS
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

#include <RWObj_ObjMaterialMap.hxx>

#include <Message.hxx>
#include <OSD_OpenFile.hxx>

IMPLEMENT_STANDARD_RTTIEXT(RWObj_ObjMaterialMap, RWMesh_MaterialMap)

// ================================================================
// Function : RWObj_ObjMaterialMap
// Purpose  :
// ================================================================
RWObj_ObjMaterialMap::RWObj_ObjMaterialMap(const TCollection_AsciiString& theFile)
    : RWMesh_MaterialMap(theFile),
      myFile(NULL)
{
  //
}

// ================================================================
// Function : ~RWObj_ObjMaterialMap
// Purpose  :
// ================================================================
RWObj_ObjMaterialMap::~RWObj_ObjMaterialMap()
{
  if (myFile != NULL)
  {
    if (::fclose(myFile) != 0)
    {
      myIsFailed = true;
    }
  }

  if (myIsFailed)
  {
    Message::SendFail(TCollection_AsciiString("File cannot be written\n") + myFileName);
  }
}

// ================================================================
// Function : AddMaterial
// Purpose  :
// ================================================================
TCollection_AsciiString RWObj_ObjMaterialMap::AddMaterial(const XCAFPrs_Style& theStyle)
{
  if (myFile == NULL && !myIsFailed)
  {
    myFile     = OSD_OpenFile(myFileName.ToCString(), "wb");
    myIsFailed = myFile == NULL;
    if (myFile != NULL)
    {
      Fprintf(myFile, "# Exported by Open CASCADE Technology [dev.opencascade.org]\n");
    }
  }
  if (myFile == NULL)
  {
    return TCollection_AsciiString();
  }

  return RWMesh_MaterialMap::AddMaterial(theStyle);
}

// ================================================================
// Function : DefineMaterial
// Purpose  :
// ================================================================
void RWObj_ObjMaterialMap::DefineMaterial(const XCAFPrs_Style&           theStyle,
                                          const TCollection_AsciiString& theKey,
                                          const TCollection_AsciiString& theName)
{
  (void)theName;
  Fprintf(myFile, "newmtl %s\n", theKey.ToCString());

  bool                            hasMaterial = false;
  const XCAFDoc_VisMaterialCommon aDefMat     = !myDefaultStyle.Material().IsNull()
                                                  ? myDefaultStyle.Material()->ConvertToCommonMaterial()
                                                  : XCAFDoc_VisMaterialCommon();
  Quantity_Color                  anAmbQ(aDefMat.AmbientColor), aDiffQ(aDefMat.DiffuseColor),
    aSpecQ(aDefMat.SpecularColor);
  Standard_ShortReal aTransp   = 0.0f;
  Standard_ShortReal aSpecular = aDefMat.Shininess * 1000.0f;
  if (!theStyle.Material().IsNull() && !theStyle.Material()->IsEmpty())
  {
    hasMaterial                             = true;
    const XCAFDoc_VisMaterialCommon aComMat = theStyle.Material()->ConvertToCommonMaterial();
    anAmbQ                                  = aComMat.AmbientColor;
    aDiffQ                                  = aComMat.DiffuseColor;
    aSpecQ                                  = aComMat.SpecularColor;
    aTransp                                 = aComMat.Transparency;
    aSpecular                               = aComMat.Shininess * 1000.0f;
  }
  if (theStyle.IsSetColorSurf())
  {
    hasMaterial = true;
    aDiffQ      = theStyle.GetColorSurf();
    anAmbQ      = Quantity_Color((Graphic3d_Vec3)theStyle.GetColorSurf() * 0.25f);
    if (theStyle.GetColorSurfRGBA().Alpha() < 1.0f)
    {
      aTransp = 1.0f - theStyle.GetColorSurfRGBA().Alpha();
    }
  }

  if (hasMaterial)
  {
    Graphic3d_Vec3d anAmb, aDiff, aSpec;
    anAmbQ.Values(anAmb.r(), anAmb.g(), anAmb.b(), Quantity_TOC_sRGB);
    aDiffQ.Values(aDiff.r(), aDiff.g(), aDiff.b(), Quantity_TOC_sRGB);
    aSpecQ.Values(aSpec.r(), aSpec.g(), aSpec.b(), Quantity_TOC_sRGB);

    Fprintf(myFile, "Ka %f %f %f\n", anAmb.r(), anAmb.g(), anAmb.b());
    Fprintf(myFile, "Kd %f %f %f\n", aDiff.r(), aDiff.g(), aDiff.b());
    Fprintf(myFile, "Ks %f %f %f\n", aSpec.r(), aSpec.g(), aSpec.b());
    Fprintf(myFile, "Ns %f\n", aSpecular);
    if (aTransp >= 0.0001f)
    {
      Fprintf(myFile, "Tr %f\n", aTransp);
    }
  }

  if (const Handle(Image_Texture)& aBaseTexture = theStyle.BaseColorTexture())
  {
    TCollection_AsciiString aTexture;
    if (!myImageMap.Find(aBaseTexture, aTexture) && !myImageFailMap.Contains(aBaseTexture))
    {
      if (CopyTexture(aTexture, aBaseTexture, TCollection_AsciiString(myImageMap.Extent() + 1)))
      {
        myImageMap.Bind(aBaseTexture, aTexture);
      }
      else
      {
        myImageFailMap.Add(aBaseTexture);
      }
    }
    if (!aTexture.IsEmpty())
    {
      Fprintf(myFile, "map_Kd %s\n", aTexture.ToCString());
    }
  }
}
