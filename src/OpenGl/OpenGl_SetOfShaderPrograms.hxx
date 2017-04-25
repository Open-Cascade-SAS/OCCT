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

#include <NCollection_DataMap.hxx>
#include <OpenGl_ShaderProgram.hxx>

//! Standard GLSL program combination bits.
enum OpenGl_ProgramOptions
{
  OpenGl_PO_Point       = 0x001, //!< point marker
  OpenGl_PO_VertColor   = 0x002, //!< per-vertex color
  OpenGl_PO_TextureRGB  = 0x004, //!< handle RGB   texturing
  OpenGl_PO_TextureA    = 0x008, //!< handle Alpha texturing
  OpenGl_PO_TextureEnv  = 0x010, //!< handle environment map
  OpenGl_PO_StippleLine = 0x020, //!< stipple line
  OpenGl_PO_ClipPlanes1 = 0x040, //!< handle 1 clipping plane
  OpenGl_PO_ClipPlanes2 = 0x080, //!< handle 2 clipping planes
  OpenGl_PO_ClipPlanesN = 0x100, //!< handle N clipping planes
  OpenGl_PO_WriteOit    = 0x200, //!< write coverage buffer for Blended Order-Independent Transparency
  OpenGl_PO_NB          = 0x400  //!< overall number of combinations
};

//! Alias to programs array of predefined length
class OpenGl_SetOfShaderPrograms : public Standard_Transient
{

public:

  //! Empty constructor
  OpenGl_SetOfShaderPrograms() {}

  //! Access program by index
  Handle(OpenGl_ShaderProgram)& ChangeValue (const Standard_Integer theIndex) { return myPrograms[theIndex]; }

protected:

  Handle(OpenGl_ShaderProgram) myPrograms[OpenGl_PO_NB]; //!< programs array

public:

  DEFINE_STANDARD_RTTI_INLINE(OpenGl_SetOfShaderPrograms,Standard_Transient)

};

DEFINE_STANDARD_HANDLE(OpenGl_SetOfShaderPrograms, Standard_Transient)

typedef NCollection_DataMap<TCollection_AsciiString, Handle(OpenGl_SetOfShaderPrograms)> OpenGl_MapOfShaderPrograms;

#endif // _OpenGl_SetOfShaderPrograms_HeaderFile
