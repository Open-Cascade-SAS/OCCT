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
  OpenGl_PO_ClipPlanes = 0x01, //!< handle clipping planes
  OpenGl_PO_Point      = 0x02, //!< point marker
  OpenGl_PO_VertColor  = 0x04, //!< per-vertex color
  OpenGl_PO_TextureRGB = 0x08, //!< handle RGB   texturing
  OpenGl_PO_TextureA   = 0x10, //!< handle Alpha texturing
  OpenGl_PO_TextureEnv = 0x20, //!< handle environment map
  OpenGl_PO_NB         = 0x40  //!< overall number of combinations
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

  DEFINE_STANDARD_RTTI (OpenGl_SetOfShaderPrograms)

};

DEFINE_STANDARD_HANDLE(OpenGl_SetOfShaderPrograms, Standard_Transient)

typedef NCollection_DataMap<TCollection_AsciiString, Handle(OpenGl_SetOfShaderPrograms)> OpenGl_MapOfShaderPrograms;

#endif // _OpenGl_SetOfShaderPrograms_HeaderFile
