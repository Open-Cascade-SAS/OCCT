// Created on: 2013-09-19
// Created by: Denis BOGOLEPOV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#ifndef _OpenGl_ShaderObject_Header
#define _OpenGl_ShaderObject_Header

#include <Handle_OpenGl_ShaderObject.hxx>
#include <Graphic3d_ShaderObject.hxx>
#include <OpenGl_GlCore20.hxx>
#include <OpenGl_Resource.hxx>

//! Wrapper for OpenGL shader object.
class OpenGl_ShaderObject : public OpenGl_Resource
{

public:

  //! Non-valid shader name.
  static const GLuint NO_SHADER = 0;

public:

  //! Creates uninitialized shader object.
  Standard_EXPORT OpenGl_ShaderObject (GLenum theType);

  //! Releases resources of shader object.
  Standard_EXPORT virtual ~OpenGl_ShaderObject();

  //! Loads shader source code.
  Standard_EXPORT Standard_Boolean LoadSource (const Handle(OpenGl_Context)&  theCtx,
                                               const TCollection_AsciiString& theSource);

  //! Compiles the shader object.
  Standard_EXPORT Standard_Boolean Compile (const Handle(OpenGl_Context)& theCtx);

  //! Initializes (loads and compiles) shader object with the specified description.
  Standard_EXPORT Standard_Boolean Initialize (const Handle(OpenGl_Context)&         theCtx,
                                               const Handle(Graphic3d_ShaderObject)& theShader);

  //! Fetches information log of the last compile operation.
  Standard_EXPORT Standard_Boolean FetchInfoLog (const Handle(OpenGl_Context)& theCtx,
                                                 TCollection_AsciiString&      theLog);

  //! Creates new empty shader object of specified type.
  Standard_EXPORT Standard_Boolean Create (const Handle(OpenGl_Context)& theCtx);

  //! Destroys shader object.
  Standard_EXPORT virtual void Release (OpenGl_Context* theCtx);

  //! Returns type of shader object.
  GLenum Type() const { return myType; }
  
protected:

  GLenum myType;     //!< Type of OpenGL shader object
  GLuint myShaderID; //!< Handle of OpenGL shader object

public:

  DEFINE_STANDARD_RTTI (OpenGl_ShaderObject)
  friend class OpenGl_ShaderProgram;

};

#endif // _OpenGl_ShaderObject_Header
