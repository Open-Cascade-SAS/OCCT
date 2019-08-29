// Author: Kirill Gavrilov
// Copyright (c) 2016-2019 OPEN CASCADE SAS
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

#ifndef _RWGltf_MaterialCommon_HeaderFile
#define _RWGltf_MaterialCommon_HeaderFile

#include <Image_Texture.hxx>
#include <Quantity_ColorRGBA.hxx>

//! glTF 1.0 format common (obsolete) material definition.
class RWGltf_MaterialCommon : public Standard_Transient
{
public:

  Handle(Image_Texture)   AmbientTexture;  //!< image defining ambient color
  Handle(Image_Texture)   DiffuseTexture;  //!< image defining diffuse color
  Handle(Image_Texture)   SpecularTexture; //!< image defining specular color
  TCollection_AsciiString Id;              //!< material identifier
  TCollection_AsciiString Name;            //!< material name
  Quantity_Color          AmbientColor;
  Quantity_Color          DiffuseColor;
  Quantity_Color          SpecularColor;
  Quantity_Color          EmissiveColor;
  Standard_ShortReal      Shininess;
  Standard_ShortReal      Transparency;

  RWGltf_MaterialCommon()
  : AmbientColor (0.1, 0.1, 0.1, Quantity_TOC_sRGB),
    DiffuseColor (0.8, 0.8, 0.8, Quantity_TOC_sRGB),
    SpecularColor(0.2, 0.2, 0.2, Quantity_TOC_sRGB),
    EmissiveColor(Quantity_NOC_BLACK),
    Shininess (1.0f),
    Transparency (0.0f) {}

};

#endif // _RWGltf_MaterialCommon_HeaderFile
