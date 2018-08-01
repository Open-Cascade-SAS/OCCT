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
  OpenGl_PO_Point           = 0x0001, //!< point marker
  OpenGl_PO_VertColor       = 0x0002, //!< per-vertex color
  OpenGl_PO_TextureRGB      = 0x0004, //!< handle RGB   texturing
  OpenGl_PO_TextureA        = 0x0008, //!< handle Alpha texturing
  OpenGl_PO_TextureEnv      = 0x0010, //!< handle environment map
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
  OpenGl_PO_NeedsGeomShader = OpenGl_PO_MeshEdges,
};

//! Alias to programs array of predefined length
class OpenGl_SetOfShaderPrograms : public Standard_Transient
{
  DEFINE_STANDARD_RTTI_INLINE(OpenGl_SetOfShaderPrograms, Standard_Transient)
public:

  //! Empty constructor
  OpenGl_SetOfShaderPrograms() {}

  //! Access program by index
  Handle(OpenGl_ShaderProgram)& ChangeValue (Graphic3d_TypeOfShadingModel theShadingModel,
                                             Standard_Integer theProgramBits)
  {
    return myPrograms[theShadingModel][theProgramBits];
  }

protected:
  Handle(OpenGl_ShaderProgram) myPrograms[Graphic3d_TypeOfShadingModel_NB][OpenGl_PO_NB]; //!< programs array
};

DEFINE_STANDARD_HANDLE(OpenGl_SetOfShaderPrograms, Standard_Transient)

typedef NCollection_DataMap<TCollection_AsciiString, Handle(OpenGl_SetOfShaderPrograms)> OpenGl_MapOfShaderPrograms;

#endif // _OpenGl_SetOfShaderPrograms_HeaderFile
