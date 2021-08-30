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

#ifndef _XCAFDoc_VisMaterialPBR_HeaderFile
#define _XCAFDoc_VisMaterialPBR_HeaderFile

#include <Graphic3d_AlphaMode.hxx>
#include <Graphic3d_Vec.hxx>
#include <Image_Texture.hxx>
#include <Quantity_ColorRGBA.hxx>
#include <Standard_Dump.hxx>

//! Metallic-roughness PBR material definition.
struct XCAFDoc_VisMaterialPBR
{
  Handle(Image_Texture)   BaseColorTexture;         //!< RGB texture for the base color
  Handle(Image_Texture)   MetallicRoughnessTexture; //!< RG texture packing the metallic and roughness properties together
  Handle(Image_Texture)   EmissiveTexture;          //!< RGB emissive map controls the color and intensity of the light being emitted by the material
  Handle(Image_Texture)   OcclusionTexture;         //!< R occlusion map indicating areas of indirect lighting
  Handle(Image_Texture)   NormalTexture;            //!< normal map
  Quantity_ColorRGBA      BaseColor;                //!< base color (or scale factor to the texture); [1.0, 1.0, 1.0, 1.0] by default
  Graphic3d_Vec3          EmissiveFactor;           //!< emissive color; [0.0, 0.0, 0.0] by default
  Standard_ShortReal      Metallic;                 //!< metalness  (or scale factor to the texture) within range [0.0, 1.0]; 1.0 by default
  Standard_ShortReal      Roughness;                //!< roughness  (or scale factor to the texture) within range [0.0, 1.0]; 1.0 by default
  Standard_ShortReal      RefractionIndex;          //!< IOR (index of refraction) within range [1.0, 3.0]; 1.5 by default
  Standard_Boolean        IsDefined;                //!< defined flag; TRUE by default

  //! Empty constructor.
  XCAFDoc_VisMaterialPBR()
  : BaseColor (1.0f, 1.0f, 1.0f, 1.0f),
    EmissiveFactor (0.0f, 0.0f, 0.0f),
    Metallic  (1.0f),
    Roughness (1.0f),
    RefractionIndex (1.5f),
    IsDefined (Standard_True) {}

  //! Compare two materials.
  Standard_Boolean IsEqual (const XCAFDoc_VisMaterialPBR& theOther) const
  {
    if (&theOther == this)
    {
      return true;
    }
    else if (theOther.IsDefined != IsDefined)
    {
      return false;
    }
    else if (!IsDefined)
    {
      return true;
    }

    return theOther.BaseColorTexture == BaseColorTexture
        && theOther.MetallicRoughnessTexture == MetallicRoughnessTexture
        && theOther.EmissiveTexture == EmissiveTexture
        && theOther.OcclusionTexture == OcclusionTexture
        && theOther.NormalTexture == NormalTexture
        && theOther.BaseColor == BaseColor
        && theOther.EmissiveFactor == EmissiveFactor
        && theOther.Metallic == Metallic
        && theOther.Roughness == Roughness
        && theOther.RefractionIndex == RefractionIndex;
  }
  
  //! Dumps the content of me into the stream
  void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const
  {
    OCCT_DUMP_CLASS_BEGIN (theOStream, XCAFDoc_VisMaterialPBR)

    OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, BaseColorTexture.get())
    OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, MetallicRoughnessTexture.get())
    OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, EmissiveTexture.get())
    OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, OcclusionTexture.get())
    OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, NormalTexture.get())

    OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &BaseColor)
    OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &EmissiveFactor)

    OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, Metallic)
    OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, Roughness)
    OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, RefractionIndex)
    OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, IsDefined)
  }
};

#endif // _XCAFDoc_VisMaterialPBR_HeaderFile
