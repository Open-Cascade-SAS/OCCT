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

#include <Graphic3d_ShaderObject.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_ShaderObject.hxx>
#include <OSD_Path.hxx>
#include <Standard_Assert.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_HANDLE (OpenGl_ShaderObject, OpenGl_Resource)
IMPLEMENT_STANDARD_RTTIEXT(OpenGl_ShaderObject, OpenGl_Resource)

// =======================================================================
// function : OpenGl_ShaderObject
// purpose  : Creates uninitialized shader object
// =======================================================================
OpenGl_ShaderObject::OpenGl_ShaderObject (GLenum theType)
: myType     (theType),
  myShaderID (NO_SHADER)
{
  //
}

// =======================================================================
// function : ~OpenGl_ShaderObject
// purpose  : Releases resources of shader object
// =======================================================================
OpenGl_ShaderObject::~OpenGl_ShaderObject()
{
  Release (NULL);
}

// =======================================================================
// function : LoadSource
// purpose  : Loads shader source code
// =======================================================================
Standard_Boolean OpenGl_ShaderObject::LoadSource (const Handle(OpenGl_Context)&  theCtx,
                                                  const TCollection_AsciiString& theSource)
{
  if (myShaderID == NO_SHADER)
  {
    return Standard_False;
  }

  const GLchar* aLines = theSource.ToCString();
  theCtx->core20->glShaderSource (myShaderID, 1, &aLines, NULL);
  return Standard_True;
}

// =======================================================================
// function : Compile
// purpose  : Compiles the shader object
// =======================================================================
Standard_Boolean OpenGl_ShaderObject::Compile (const Handle(OpenGl_Context)& theCtx)
{
  if (myShaderID == NO_SHADER)
  {
    return Standard_False;
  }

  // Try to compile shader
  theCtx->core20->glCompileShader (myShaderID);

  // Check compile status
  GLint aStatus = GL_FALSE;
  theCtx->core20->glGetShaderiv (myShaderID, GL_COMPILE_STATUS, &aStatus);
  return aStatus != GL_FALSE;
}

// =======================================================================
// function : FetchInfoLog
// purpose  : Fetches information log of the last compile operation
// =======================================================================
Standard_Boolean OpenGl_ShaderObject::FetchInfoLog (const Handle(OpenGl_Context)& theCtx,
                                                    TCollection_AsciiString&      theLog)
{
  if (myShaderID == NO_SHADER)
  {
    return Standard_False;
  }

  // Load information log of the compiler
  GLint aLength = 0;
  theCtx->core20->glGetShaderiv (myShaderID, GL_INFO_LOG_LENGTH, &aLength);
  if (aLength > 0)
  {
    GLchar* aLog = (GLchar*) alloca (aLength);
    memset (aLog, 0, aLength);
    theCtx->core20->glGetShaderInfoLog (myShaderID, aLength, NULL, aLog);
    theLog = aLog;
  }

  return Standard_True;
}

// =======================================================================
// function : Create
// purpose  : Creates new empty shader object of specified type
// =======================================================================
Standard_Boolean OpenGl_ShaderObject::Create (const Handle(OpenGl_Context)& theCtx)
{
  if (myShaderID == NO_SHADER
   && theCtx->core20 != NULL)
  {
    myShaderID = theCtx->core20->glCreateShader (myType);
  }

  return myShaderID != NO_SHADER;
}

// =======================================================================
// function : Release
// purpose  : Destroys shader object
// =======================================================================
void OpenGl_ShaderObject::Release (const OpenGl_Context* theCtx)
{
  if (myShaderID == NO_SHADER)
  {
    return;
  }

  Standard_ASSERT_RETURN (theCtx != NULL,
    "OpenGl_ShaderObject destroyed without GL context! Possible GPU memory leakage...",);

  if (theCtx->core20 != NULL)
  {
    theCtx->core20->glDeleteShader (myShaderID);
  }
  myShaderID = NO_SHADER;
}
