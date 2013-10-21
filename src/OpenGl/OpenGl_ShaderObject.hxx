// Created on: 2013-09-19
// Created by: Denis BOGOLEPOV
// Copyright (c) 2013 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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
  Standard_EXPORT virtual void Release (const OpenGl_Context* theCtx);

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
