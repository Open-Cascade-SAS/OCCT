// Created on: 2014-10-08
// Created by: Denis BOGOLEPOV
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

#include <OpenGl_Sampler.hxx>
#include <OpenGl_GlCore33.hxx>
#include <Standard_Assert.hxx>

IMPLEMENT_STANDARD_HANDLE (OpenGl_Sampler, OpenGl_Resource)
IMPLEMENT_STANDARD_RTTIEXT(OpenGl_Sampler, OpenGl_Resource)

// =======================================================================
// function : OpenGl_Sampler
// purpose  :
// =======================================================================
OpenGl_Sampler::OpenGl_Sampler()
: mySamplerID (NO_SAMPLER)
{
  //
}

// =======================================================================
// function : ~OpenGl_Sampler
// purpose  :
// =======================================================================
OpenGl_Sampler::~OpenGl_Sampler()
{
  Release (NULL);
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_Sampler::Release (OpenGl_Context* theContext)
{
  if (isValidSampler())
  {
    // application can not handle this case by exception - this is bug in code
    Standard_ASSERT_RETURN (theContext != NULL,
      "OpenGl_Sampler destroyed without GL context! Possible GPU memory leakage...",);

    if (theContext->IsValid())
    {
    #if !defined(GL_ES_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
      theContext->core33->glDeleteSamplers (1, &mySamplerID);
    #endif
    }

    mySamplerID = NO_SAMPLER;
  }
}

// =======================================================================
// function : Init
// purpose  : Initializes sampler object
// =======================================================================
Standard_Boolean OpenGl_Sampler::Init (OpenGl_Context& theContext)
{
  if (theContext.core33 == NULL)
  {
    return Standard_False;
  }

  if (isValidSampler())
  {
    Release (&theContext);
  }

#if !defined(GL_ES_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
  theContext.core33->glGenSamplers (1, &mySamplerID);
  return Standard_True;
#else
  return Standard_False;
#endif
}

// =======================================================================
// function : Bind
// purpose  : Binds sampler object to the given texture unit
// =======================================================================
void OpenGl_Sampler::Bind (OpenGl_Context& theContext,
                           const GLuint    theUnit)
{
  if (isValidSampler())
  {
  #if !defined(GL_ES_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
    theContext.core33->glBindSampler (theUnit, mySamplerID);
  #endif
  }
}

// =======================================================================
// function : Unbind
// purpose  : Unbinds sampler object from the given texture unit
// =======================================================================
void OpenGl_Sampler::Unbind (OpenGl_Context& theContext,
                             const GLuint    theUnit)
{
  if (isValidSampler())
  {
  #if !defined(GL_ES_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
    theContext.core33->glBindSampler (theUnit, NO_SAMPLER);
  #endif
  }
}

// =======================================================================
// function : SetParameter
// purpose  : Sets sampler parameters
// =======================================================================
void OpenGl_Sampler::SetParameter (OpenGl_Context& theContext,
                                   const GLenum    theParam,
                                   const GLint     theValue)
{
  if (isValidSampler())
  {
  #if !defined(GL_ES_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
    theContext.core33->glSamplerParameteri (mySamplerID, theParam, theValue);
  #endif
  }
}
