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

#include <OpenGl_ArbSamplerObject.hxx>
#include <OpenGl_Texture.hxx>
#include <Standard_Assert.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_Sampler,OpenGl_Resource)

// =======================================================================
// function : OpenGl_Sampler
// purpose  :
// =======================================================================
OpenGl_Sampler::OpenGl_Sampler (const Handle(Graphic3d_TextureParams)& theParams)
: myParams (theParams),
  mySamplerRevision (0),
  mySamplerID (NO_SAMPLER),
  myIsImmutable (false)
{
  if (myParams.IsNull())
  {
    myParams = new Graphic3d_TextureParams();
  }
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
void OpenGl_Sampler::Release (OpenGl_Context* theCtx)
{
  myIsImmutable = false;
  mySamplerRevision = myParams->SamplerRevision() - 1;
  if (!isValidSampler())
  {
    return;
  }

  // application can not handle this case by exception - this is bug in code
  Standard_ASSERT_RETURN (theCtx != NULL,
    "OpenGl_Sampler destroyed without GL context! Possible GPU memory leakage...",);

  if (theCtx->IsValid())
  {
    theCtx->arbSamplerObject->glDeleteSamplers (1, &mySamplerID);
  }

  mySamplerID = NO_SAMPLER;
}

// =======================================================================
// function : Create
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Sampler::Create (const Handle(OpenGl_Context)& theCtx)
{
  if (isValidSampler())
  {
    return Standard_True;
  }
  else if (theCtx->arbSamplerObject == NULL)
  {
    return Standard_False;
  }

  theCtx->arbSamplerObject->glGenSamplers (1, &mySamplerID);
  return Standard_True;
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Sampler::Init (const Handle(OpenGl_Context)& theCtx,
                                       const OpenGl_Texture& theTexture)
{
  if (isValidSampler())
  {
    if (!ToUpdateParameters())
    {
      return Standard_True;
    }
    else if (!myIsImmutable)
    {
      applySamplerParams (theCtx, myParams, this, theTexture.GetTarget(), theTexture.HasMipmaps());
      return Standard_True;
    }
    Release (theCtx.get());
  }

  if (!Create (theCtx))
  {
    return Standard_False;
  }

  applySamplerParams (theCtx, myParams, this, theTexture.GetTarget(), theTexture.HasMipmaps());
  return Standard_True;
}

// =======================================================================
// function : Bind
// purpose  : Binds sampler object to the given texture unit
// =======================================================================
void OpenGl_Sampler::Bind (const Handle(OpenGl_Context)& theCtx,
                           const Graphic3d_TextureUnit   theUnit)
{
  if (isValidSampler())
  {
    theCtx->arbSamplerObject->glBindSampler (theUnit, mySamplerID);
  }
}

// =======================================================================
// function : Unbind
// purpose  : Unbinds sampler object from the given texture unit
// =======================================================================
void OpenGl_Sampler::Unbind (const Handle(OpenGl_Context)& theCtx,
                             const Graphic3d_TextureUnit   theUnit)
{
  if (isValidSampler())
  {
    theCtx->arbSamplerObject->glBindSampler (theUnit, NO_SAMPLER);
  }
}

// =======================================================================
// function : setParameter
// purpose  :
// =======================================================================
void OpenGl_Sampler::setParameter (const Handle(OpenGl_Context)& theCtx,
                                   OpenGl_Sampler* theSampler,
                                   GLenum theTarget,
                                   GLenum theParam,
                                   GLint  theValue)
{
  if (theSampler != NULL && theSampler->isValidSampler())
  {
    theCtx->arbSamplerObject->glSamplerParameteri (theSampler->mySamplerID, theParam, theValue);
  }
  else
  {
    theCtx->core11fwd->glTexParameteri (theTarget, theParam, theValue);
  }
}

// =======================================================================
// function : SetParameters
// purpose  :
// =======================================================================
void OpenGl_Sampler::SetParameters (const Handle(Graphic3d_TextureParams)& theParams)
{
  if (myParams != theParams)
  {
    myParams = theParams;
    mySamplerRevision = myParams->SamplerRevision() - 1;
  }
}

// =======================================================================
// function : applySamplerParams
// purpose  :
// =======================================================================
void OpenGl_Sampler::applySamplerParams (const Handle(OpenGl_Context)& theCtx,
                                         const Handle(Graphic3d_TextureParams)& theParams,
                                         OpenGl_Sampler* theSampler,
                                         const GLenum theTarget,
                                         const bool theHasMipMaps)
{
  if (theSampler != NULL && theSampler->Parameters() == theParams)
  {
    theSampler->mySamplerRevision = theParams->SamplerRevision();
  }

  // setup texture filtering
  const GLenum aFilter = (theParams->Filter() == Graphic3d_TOTF_NEAREST) ? GL_NEAREST : GL_LINEAR;
  GLenum aFilterMin = aFilter;
  if (theHasMipMaps)
  {
    aFilterMin = GL_NEAREST_MIPMAP_NEAREST;
    if (theParams->Filter() == Graphic3d_TOTF_BILINEAR)
    {
      aFilterMin = GL_LINEAR_MIPMAP_NEAREST;
    }
    else if (theParams->Filter() == Graphic3d_TOTF_TRILINEAR)
    {
      aFilterMin = GL_LINEAR_MIPMAP_LINEAR;
    }
  }

  setParameter (theCtx, theSampler, theTarget, GL_TEXTURE_MIN_FILTER, aFilterMin);
  setParameter (theCtx, theSampler, theTarget, GL_TEXTURE_MAG_FILTER, aFilter);

  // setup texture wrapping
  const GLenum aWrapMode = theParams->IsRepeat() ? GL_REPEAT : theCtx->TextureWrapClamp();
  setParameter (theCtx, theSampler, theTarget, GL_TEXTURE_WRAP_S, aWrapMode);
#if !defined(GL_ES_VERSION_2_0)
  if (theTarget == GL_TEXTURE_1D)
  {
    return;
  }
#endif
  setParameter (theCtx, theSampler, theTarget, GL_TEXTURE_WRAP_T, aWrapMode);
  if (theTarget == GL_TEXTURE_3D)
  {
    setParameter (theCtx, theSampler, theTarget, GL_TEXTURE_WRAP_R, aWrapMode);
    return;
  }

  if (theCtx->extAnis)
  {
    // setup degree of anisotropy filter
    const GLint aMaxDegree = theCtx->MaxDegreeOfAnisotropy();
    GLint aDegree;
    switch (theParams->AnisoFilter())
    {
      case Graphic3d_LOTA_QUALITY:
      {
        aDegree = aMaxDegree;
        break;
      }
      case Graphic3d_LOTA_MIDDLE:
      {
        aDegree = (aMaxDegree <= 4) ? 2 : (aMaxDegree / 2);
        break;
      }
      case Graphic3d_LOTA_FAST:
      {
        aDegree = 2;
        break;
      }
      case Graphic3d_LOTA_OFF:
      default:
      {
        aDegree = 1;
        break;
      }
    }

    setParameter (theCtx, theSampler, theTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, aDegree);
  }
}

// =======================================================================
// function : applyGlobalTextureParams
// purpose  :
// =======================================================================
void OpenGl_Sampler::applyGlobalTextureParams (const Handle(OpenGl_Context)& theCtx,
                                               const OpenGl_Texture& theTexture,
                                               const Handle(Graphic3d_TextureParams)& theParams)
{
#if defined(GL_ES_VERSION_2_0)
  (void )theCtx;
  (void )theTexture;
  (void )theParams;
#else
  if (theCtx->core11 == NULL)
  {
    return;
  }

  GLint anEnvMode = GL_MODULATE; // lighting mode
  if (!theParams->IsModulate())
  {
    anEnvMode = GL_DECAL;
    if (theTexture.GetFormat() == GL_ALPHA
     || theTexture.GetFormat() == GL_LUMINANCE)
    {
      anEnvMode = GL_REPLACE;
    }
  }

  // setup generation of texture coordinates
  switch (theParams->GenMode())
  {
    case Graphic3d_TOTM_OBJECT:
    {
      theCtx->core11->glTexGeni  (GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
      theCtx->core11->glTexGenfv (GL_S, GL_OBJECT_PLANE,     theParams->GenPlaneS().GetData());
      if (theTexture.GetTarget() != GL_TEXTURE_1D)
      {
        theCtx->core11->glTexGeni  (GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
        theCtx->core11->glTexGenfv (GL_T, GL_OBJECT_PLANE,     theParams->GenPlaneT().GetData());
      }
      break;
    }
    case Graphic3d_TOTM_SPHERE:
    {
      theCtx->core11->glTexGeni (GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
      if (theTexture.GetTarget() != GL_TEXTURE_1D)
      {
        theCtx->core11->glTexGeni (GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
      }
      break;
    }
    case Graphic3d_TOTM_EYE:
    {
      theCtx->WorldViewState.Push();
      theCtx->WorldViewState.SetIdentity();
      theCtx->ApplyWorldViewMatrix();

      theCtx->core11->glTexGeni  (GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
      theCtx->core11->glTexGenfv (GL_S, GL_EYE_PLANE,        theParams->GenPlaneS().GetData());
      if (theTexture.GetTarget() != GL_TEXTURE_1D)
      {
        theCtx->core11->glTexGeni  (GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        theCtx->core11->glTexGenfv (GL_T, GL_EYE_PLANE,        theParams->GenPlaneT().GetData());
      }

      theCtx->WorldViewState.Pop();
      break;
    }
    case Graphic3d_TOTM_SPRITE:
    {
      if (theCtx->core20fwd != NULL)
      {
        theCtx->core11fwd->glEnable (GL_POINT_SPRITE);
        glTexEnvi (GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
        anEnvMode = GL_REPLACE;
      }
      break;
    }
    case Graphic3d_TOTM_MANUAL:
    default: break;
  }

  // setup lighting
  glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, anEnvMode);

  switch (theTexture.GetTarget())
  {
    case GL_TEXTURE_1D:
    {
      if (theParams->GenMode() != Graphic3d_TOTM_MANUAL)
      {
        theCtx->core11fwd->glEnable (GL_TEXTURE_GEN_S);
      }
      theCtx->core11fwd->glEnable (GL_TEXTURE_1D);
      break;
    }
    case GL_TEXTURE_2D:
    {
      if (theParams->GenMode() != Graphic3d_TOTM_MANUAL)
      {
        theCtx->core11fwd->glEnable (GL_TEXTURE_GEN_S);
        theCtx->core11fwd->glEnable (GL_TEXTURE_GEN_T);
      }
      theCtx->core11fwd->glEnable (GL_TEXTURE_2D);
      break;
    }
    default: break;
  }
#endif
}

// =======================================================================
// function : resetGlobalTextureParams
// purpose  :
// =======================================================================
void OpenGl_Sampler::resetGlobalTextureParams (const Handle(OpenGl_Context)& theCtx,
                                               const OpenGl_Texture& theTexture,
                                               const Handle(Graphic3d_TextureParams)& theParams)
{
#if defined(GL_ES_VERSION_2_0)
  (void )theCtx;
  (void )theTexture;
  (void )theParams;
#else
  if (theCtx->core11 == NULL)
  {
    return;
  }

  // reset texture matrix because some code may expect it is identity
  GLint aMatrixMode = GL_TEXTURE;
  theCtx->core11fwd->glGetIntegerv (GL_MATRIX_MODE, &aMatrixMode);
  theCtx->core11->glMatrixMode (GL_TEXTURE);
  theCtx->core11->glLoadIdentity();
  theCtx->core11->glMatrixMode (aMatrixMode);

  switch (theTexture.GetTarget())
  {
    case GL_TEXTURE_1D:
    {
      if (theParams->GenMode() != GL_NONE)
      {
        theCtx->core11fwd->glDisable (GL_TEXTURE_GEN_S);
      }
      theCtx->core11fwd->glDisable (GL_TEXTURE_1D);
      break;
    }
    case GL_TEXTURE_2D:
    {
      if (theParams->GenMode() != GL_NONE)
      {
        theCtx->core11fwd->glDisable (GL_TEXTURE_GEN_S);
        theCtx->core11fwd->glDisable (GL_TEXTURE_GEN_T);
        if (theParams->GenMode() == Graphic3d_TOTM_SPRITE)
        {
          theCtx->core11fwd->glDisable (GL_POINT_SPRITE);
        }
      }
      theCtx->core11fwd->glDisable (GL_TEXTURE_2D);
      break;
    }
    default: break;
  }
#endif
}
