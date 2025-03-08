// Author: Kirill Gavrilov
// Copyright (c) 2015-2019 OPEN CASCADE SAS
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

#ifndef _RWObj_Material_HeaderFile
#define _RWObj_Material_HeaderFile

#include <Quantity_Color.hxx>
#include <TCollection_AsciiString.hxx>

//! Material definition for OBJ file format.
struct RWObj_Material
{
  TCollection_AsciiString Name;        //!< material name (identifier) as defined in MTL file
                                       // clang-format off
  TCollection_AsciiString DiffuseTexture;  //!< path to the texture image file defining diffuse color
  TCollection_AsciiString SpecularTexture; //!< path to the texture image file defining specular color
                                       // clang-format on
  TCollection_AsciiString BumpTexture; //!< path to the texture image file defining normal map
  Quantity_Color          AmbientColor;
  Quantity_Color          DiffuseColor;
  Quantity_Color          SpecularColor;
  Standard_ShortReal      Shininess;
  Standard_ShortReal      Transparency;

  RWObj_Material()
      : AmbientColor(0.1, 0.1, 0.1, Quantity_TOC_sRGB),
        DiffuseColor(0.8, 0.8, 0.8, Quantity_TOC_sRGB),
        SpecularColor(0.2, 0.2, 0.2, Quantity_TOC_sRGB),
        Shininess(1.0f),
        Transparency(0.0f)
  {
  }
};

#endif // _RWObj_Material_HeaderFile
