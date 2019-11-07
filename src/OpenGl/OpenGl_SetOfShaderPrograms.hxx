// Created on: 2014-10-08
// Created by: Kirill Gavrilov
// Copyright (c) 2014 OPEN CASCADE SAS
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

#ifndef _OpenGl_SetOfShaderPrograms_HeaderFile
#define _OpenGl_SetOfShaderPrograms_HeaderFile

#include <Graphic3d_TypeOfShadingModel.hxx>
#include <NCollection_DataMap.hxx>
#include <OpenGl_ShaderProgram.hxx>

//! Standard GLSL program combination bits.
enum OpenGl_ProgramOptions
{
  OpenGl_PO_VertColor       = 0x0001, //!< per-vertex color
  OpenGl_PO_TextureRGB      = 0x0002, //!< handle RGB texturing
  OpenGl_PO_TextureEnv      = 0x0004, //!< handle environment map (obsolete, to be removed)
  OpenGl_PO_TextureNormal   = OpenGl_PO_TextureRGB|OpenGl_PO_TextureEnv, //!< extended texture set (with normal map)
  OpenGl_PO_PointSimple     = 0x0008, //!< point marker without sprite
  OpenGl_PO_PointSprite     = 0x0010, //!< point sprite with RGB image
  OpenGl_PO_PointSpriteA    = OpenGl_PO_PointSimple|OpenGl_PO_PointSprite, //!< point sprite with Alpha image
  OpenGl_PO_StippleLine     = 0x0020, //!< stipple line
  OpenGl_PO_ClipPlanes1     = 0x0040, //!< handle 1 clipping plane
  OpenGl_PO_ClipPlanes2     = 0x0080, //!< handle 2 clipping planes
  OpenGl_PO_ClipPlanesN     = OpenGl_PO_ClipPlanes1|OpenGl_PO_ClipPlanes2, //!< handle N clipping planes
  OpenGl_PO_ClipChains      = 0x0100, //!< handle chains of clipping planes
  OpenGl_PO_MeshEdges       = 0x0200, //!< draw mesh edges (wireframe)
  OpenGl_PO_AlphaTest       = 0x0400, //!< discard fragment by alpha test (defined by cutoff value)
  OpenGl_PO_WriteOit        = 0x0800, //!< write coverage buffer for Blended Order-Independent Transparency
  //
  OpenGl_PO_NB              = 0x1000, //!< overall number of combinations
  OpenGl_PO_IsPoint         = OpenGl_PO_PointSimple|OpenGl_PO_PointSprite|OpenGl_PO_PointSpriteA,
  OpenGl_PO_HasTextures     = OpenGl_PO_TextureRGB|OpenGl_PO_TextureEnv,
  OpenGl_PO_NeedsGeomShader = OpenGl_PO_MeshEdges,
};

//! Alias to programs array of predefined length
class OpenGl_SetOfPrograms : public Standard_Transient
{
  DEFINE_STANDARD_RTTI_INLINE(OpenGl_SetOfPrograms, Standard_Transient)
public:

  //! Empty constructor
  OpenGl_SetOfPrograms() {}

  //! Access program by index
  Handle(OpenGl_ShaderProgram)& ChangeValue (Standard_Integer theProgramBits) { return myPrograms[theProgramBits]; }

protected:
  Handle(OpenGl_ShaderProgram) myPrograms[OpenGl_PO_NB]; //!< programs array
};

//! Alias to 2D programs array of predefined length
class OpenGl_SetOfShaderPrograms : public Standard_Transient
{
  DEFINE_STANDARD_RTTI_INLINE(OpenGl_SetOfShaderPrograms, Standard_Transient)
public:

  //! Empty constructor
  OpenGl_SetOfShaderPrograms() {}

  //! Constructor
  OpenGl_SetOfShaderPrograms (const Handle(OpenGl_SetOfPrograms)& thePrograms)
  {
    for (Standard_Integer aSetIter = 0; aSetIter < Graphic3d_TypeOfShadingModel_NB - 1; ++aSetIter)
    {
      myPrograms[aSetIter] = thePrograms;
    }
  }

  //! Access program by index
  Handle(OpenGl_ShaderProgram)& ChangeValue (Graphic3d_TypeOfShadingModel theShadingModel,
                                             Standard_Integer theProgramBits)
  {
    Handle(OpenGl_SetOfPrograms)& aSet = myPrograms[theShadingModel - 1];
    if (aSet.IsNull())
    {
      aSet = new OpenGl_SetOfPrograms();
    }
    return aSet->ChangeValue (theProgramBits);
  }

protected:
  Handle(OpenGl_SetOfPrograms) myPrograms[Graphic3d_TypeOfShadingModel_NB - 1]; //!< programs array, excluding Graphic3d_TOSM_UNLIT
};

typedef NCollection_DataMap<TCollection_AsciiString, Handle(OpenGl_SetOfShaderPrograms)> OpenGl_MapOfShaderPrograms;

#endif // _OpenGl_SetOfShaderPrograms_HeaderFile
