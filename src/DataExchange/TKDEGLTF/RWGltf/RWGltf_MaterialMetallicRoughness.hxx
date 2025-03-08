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

#ifndef _RWGltf_MaterialMetallicRoughness_HeaderFile
#define _RWGltf_MaterialMetallicRoughness_HeaderFile

#include <Graphic3d_Vec.hxx>
#include <Quantity_ColorRGBA.hxx>
#include <RWGltf_GltfAlphaMode.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>

class Image_Texture;

//! glTF 2.0 format PBR material definition.
class RWGltf_MaterialMetallicRoughness : public Standard_Transient
{
public:
  Handle(Image_Texture) BaseColorTexture; //!< RGB texture for the base color
                                          // clang-format off
  Handle(Image_Texture)   MetallicRoughnessTexture; //!< RG texture packing the metallic and roughness properties together
  Handle(Image_Texture)   EmissiveTexture;          //!< RGB emissive map controls the color and intensity of the light being emitted by the material
  Handle(Image_Texture)   OcclusionTexture;         //!< R occlusion map indicating areas of indirect lighting
  Handle(Image_Texture)   NormalTexture;            //!< normal map
  TCollection_AsciiString Id;                       //!< material identifier
  TCollection_AsciiString Name;                     //!< material name
  Quantity_ColorRGBA      BaseColor;                //!< base color (or scale factor to the texture); [1.0, 1.0, 1.0, 1.0] by default
  Graphic3d_Vec3          EmissiveFactor;           //!< emissive color; [0.0, 0.0, 0.0] by default
  Standard_ShortReal      Metallic;                 //!< metalness  (or scale factor to the texture) within range [0.0, 1.0]; 1.0 by default
  Standard_ShortReal      Roughness;                //!< roughness  (or scale factor to the texture) within range [0.0, 1.0]; 1.0 by default
  Standard_ShortReal      AlphaCutOff;              //!< alpha cutoff value; 0.5 by default
  RWGltf_GltfAlphaMode    AlphaMode;                //!< alpha mode; RWGltf_GltfAlphaMode_Opaque by default
  Standard_Boolean        IsDoubleSided;            //!< specifies whether the material is double sided; FALSE by default
                                          // clang-format on

  RWGltf_MaterialMetallicRoughness()
      : BaseColor(1.0f, 1.0f, 1.0f, 1.0f),
        EmissiveFactor(0.0f, 0.0f, 0.0f),
        Metallic(1.0f),
        Roughness(1.0f),
        AlphaCutOff(0.5f),
        AlphaMode(RWGltf_GltfAlphaMode_Opaque),
        IsDoubleSided(Standard_False)
  {
  }
};

#endif // _RWGltf_MaterialMetallicRoughness_HeaderFile
