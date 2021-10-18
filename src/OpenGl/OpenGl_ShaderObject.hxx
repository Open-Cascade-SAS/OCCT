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

#ifndef OpenGl_ShaderObject_HeaderFile
#define OpenGl_ShaderObject_HeaderFile

#include <Graphic3d_ShaderObject.hxx>
#include <OpenGl_GlCore20.hxx>
#include <OpenGl_Resource.hxx>
#include <Quantity_Date.hxx>

//! Wrapper for OpenGL shader object.
class OpenGl_ShaderObject : public OpenGl_Resource
{
  DEFINE_STANDARD_RTTIEXT(OpenGl_ShaderObject, OpenGl_Resource)
  friend class OpenGl_ShaderProgram;
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

  //! Wrapper for compiling shader object with verbose printing on error.
  //! @param theCtx bound OpenGL context
  //! @param theId  GLSL program id to define file name
  //! @param theSource source code to load
  //! @param theIsVerbose flag to print log on error
  //! @param theToPrintSource flag to print source code on error
  Standard_EXPORT Standard_Boolean LoadAndCompile (const Handle(OpenGl_Context)& theCtx,
                                                   const TCollection_AsciiString& theId,
                                                   const TCollection_AsciiString& theSource,
                                                   bool theIsVerbose = true,
                                                   bool theToPrintSource = true);

  //! Print source code of this shader object to messenger.
  Standard_EXPORT void DumpSourceCode (const Handle(OpenGl_Context)& theCtx,
                                       const TCollection_AsciiString& theId,
                                       const TCollection_AsciiString& theSource) const;

  //! Fetches information log of the last compile operation.
  Standard_EXPORT Standard_Boolean FetchInfoLog (const Handle(OpenGl_Context)& theCtx,
                                                 TCollection_AsciiString&      theLog);

  //! Creates new empty shader object of specified type.
  Standard_EXPORT Standard_Boolean Create (const Handle(OpenGl_Context)& theCtx);

  //! Destroys shader object.
  Standard_EXPORT virtual void Release (OpenGl_Context* theCtx) Standard_OVERRIDE;

  //! Returns estimated GPU memory usage - not implemented.
  virtual Standard_Size EstimatedDataSize() const Standard_OVERRIDE { return 0; }

  //! Returns type of shader object.
  GLenum Type() const { return myType; }

public:

  //! Update the shader object from external file in the following way:
  //! 1) If external file does not exist, then it will be created (current source code will be dumped, no recompilation) and FALSE will be returned.
  //! 2) If external file exists and it has the same timestamp as   myDumpDate, nothing will be done      and FALSE will be returned.
  //! 3) If external file exists and it has    newer timestamp than myDumpDate, shader  will be recompiled and TRUE will be returned.
  //! @param theCtx OpenGL context bound to this working thread
  //! @param theId  GLSL program id to define file name
  //! @param theFolder folder to store files
  //! @param theToBeautify flag improving formatting (add extra newlines)
  //! @param theToReset when TRUE, existing dumps will be overridden
  Standard_EXPORT Standard_Boolean updateDebugDump (const Handle(OpenGl_Context)& theCtx,
                                                    const TCollection_AsciiString& theId,
                                                    const TCollection_AsciiString& theFolder,
                                                    Standard_Boolean theToBeautify,
                                                    Standard_Boolean theToReset);

protected:

  Quantity_Date myDumpDate; //!< The recent date of the shader dump
  GLenum        myType;     //!< Type of OpenGL shader object
  GLuint        myShaderID; //!< Handle of OpenGL shader object

};

DEFINE_STANDARD_HANDLE(OpenGl_ShaderObject, OpenGl_Resource)

#endif // _OpenGl_ShaderObject_Header
