// Created on: 2011-09-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#include <OpenGl_GlCore15.hxx>
#include <OpenGl_ArbFBO.hxx>

#include <InterfaceGraphic.hxx>

#include <OpenGl_AspectLine.hxx>
#include <OpenGl_AspectFace.hxx>
#include <OpenGl_AspectMarker.hxx>
#include <OpenGl_AspectText.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_Element.hxx>
#include <OpenGl_FrameBuffer.hxx>
#include <OpenGl_Structure.hxx>
#include <OpenGl_Sampler.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <OpenGl_Texture.hxx>
#include <OpenGl_Utils.hxx>
#include <OpenGl_View.hxx>
#include <OpenGl_Workspace.hxx>

#include <Graphic3d_TextureParams.hxx>

#if defined(_WIN32) && defined(HAVE_VIDEOCAPTURE)
  #include <OpenGl_AVIWriter.hxx>
#endif

IMPLEMENT_STANDARD_HANDLE(OpenGl_Workspace,OpenGl_Window)
IMPLEMENT_STANDARD_RTTIEXT(OpenGl_Workspace,OpenGl_Window)

namespace
{
  static const TEL_COLOUR  THE_WHITE_COLOR = { { 1.0f, 1.0f, 1.0f, 1.0f } };
  static const OpenGl_Vec4 THE_BLACK_COLOR      (0.0f, 0.0f, 0.0f, 1.0f);

  static const OpenGl_AspectLine myDefaultAspectLine;
  static const OpenGl_AspectFace myDefaultAspectFace;
  static const OpenGl_AspectMarker myDefaultAspectMarker;
  static const OpenGl_AspectText myDefaultAspectText;

  static const OpenGl_TextParam myDefaultTextParam =
  {
    16, Graphic3d_HTA_LEFT, Graphic3d_VTA_BOTTOM
  };

  static const OpenGl_Matrix myDefaultMatrix =
  {
    {{ 1.0F, 0.0F, 0.0F, 0.0F },
     { 0.0F, 1.0F, 0.0F, 0.0F },
     { 0.0F, 0.0F, 1.0F, 0.0F },
     { 0.0F, 0.0F, 0.0F, 1.0F }}
  };

};

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
void OpenGl_Material::Init (const OPENGL_SURF_PROP& theProp)
{
  // ambient component
  if (theProp.color_mask & OPENGL_AMBIENT_MASK)
  {
    const float* aSrcAmb = theProp.isphysic ? theProp.ambcol.rgb : theProp.matcol.rgb;
    Ambient = OpenGl_Vec4 (aSrcAmb[0] * theProp.amb,
                           aSrcAmb[1] * theProp.amb,
                           aSrcAmb[2] * theProp.amb,
                           1.0f);
  }
  else
  {
    Ambient = THE_BLACK_COLOR;
  }

  // diffusion component
  if (theProp.color_mask & OPENGL_DIFFUSE_MASK)
  {
    const float* aSrcDif = theProp.isphysic ? theProp.difcol.rgb : theProp.matcol.rgb;
    Diffuse = OpenGl_Vec4 (aSrcDif[0] * theProp.diff,
                           aSrcDif[1] * theProp.diff,
                           aSrcDif[2] * theProp.diff,
                           1.0f);
  }
  else
  {
    Diffuse = THE_BLACK_COLOR;
  }

  // specular component
  if (theProp.color_mask & OPENGL_SPECULAR_MASK)
  {
    const float* aSrcSpe = theProp.isphysic ? theProp.speccol.rgb : THE_WHITE_COLOR.rgb;
    Specular = OpenGl_Vec4 (aSrcSpe[0] * theProp.spec,
                            aSrcSpe[1] * theProp.spec,
                            aSrcSpe[2] * theProp.spec,
                            1.0f);
  }
  else
  {
    Specular = THE_BLACK_COLOR;
  }

  // emission component
  if (theProp.color_mask & OPENGL_EMISSIVE_MASK)
  {
    const float* aSrcEms = theProp.isphysic ? theProp.emscol.rgb : theProp.matcol.rgb;
    Emission = OpenGl_Vec4 (aSrcEms[0] * theProp.emsv,
                            aSrcEms[1] * theProp.emsv,
                            aSrcEms[2] * theProp.emsv,
                            1.0f);
  }
  else
  {
    Emission = THE_BLACK_COLOR;
  }

  ChangeShine()        = theProp.shine;
  ChangeTransparency() = theProp.trans;
}

// =======================================================================
// function : OpenGl_Workspace
// purpose  :
// =======================================================================
OpenGl_Workspace::OpenGl_Workspace (const Handle(OpenGl_GraphicDriver)& theDriver,
                                    const CALL_DEF_WINDOW&        theCWindow,
                                    Aspect_RenderingContext       theGContext,
                                    const Handle(OpenGl_Caps)&    theCaps,
                                    const Handle(OpenGl_Context)& theShareCtx)
: OpenGl_Window (theDriver, theCWindow, theGContext, theCaps, theShareCtx),
  NamedStatus (0),
  HighlightColor (&THE_WHITE_COLOR),
  //
  myHasFboBlit (Standard_True),
  //
  myViewId               (-1),
  myAntiAliasingMode     (3),
  myTransientDrawToFront (Standard_True),
  myBackBufferRestored   (Standard_False),
  myIsImmediateDrawn     (Standard_False),
  myUseZBuffer (Standard_False),
  myUseDepthTest (Standard_True),
  myUseGLLight (Standard_True),
  myIsCullingEnabled (Standard_False),
  myFrameCounter (0),
  //
  AspectLine_set (&myDefaultAspectLine),
  AspectLine_applied (NULL),
  AspectFace_set (&myDefaultAspectFace),
  AspectFace_applied (NULL),
  AspectMarker_set (&myDefaultAspectMarker),
  AspectMarker_applied (NULL),
  AspectText_set (&myDefaultAspectText),
  AspectText_applied (NULL),
  TextParam_set (&myDefaultTextParam),
  TextParam_applied (NULL),
  ViewMatrix_applied (&myDefaultMatrix),
  StructureMatrix_applied (&myDefaultMatrix),
  myCullingMode (TelCullUndefined),
  myModelViewMatrix (myDefaultMatrix),
  PolygonOffset_applied (THE_DEFAULT_POFFSET)
{
  myGlContext->core11fwd->glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
  myMainSceneFbos[0]      = new OpenGl_FrameBuffer();
  myMainSceneFbos[1]      = new OpenGl_FrameBuffer();
  myImmediateSceneFbos[0] = new OpenGl_FrameBuffer();
  myImmediateSceneFbos[1] = new OpenGl_FrameBuffer();

  if (!myGlContext->GetResource ("OpenGl_LineAttributes", myLineAttribs))
  {
    // share and register for release once the resource is no longer used
    myLineAttribs = new OpenGl_LineAttributes();
    myGlContext->ShareResource ("OpenGl_LineAttributes", myLineAttribs);
    myLineAttribs->Init (myGlContext);
  }

  // General initialization of the context

#if !defined(GL_ES_VERSION_2_0)
  if (myGlContext->core11 != NULL)
  {
    // Eviter d'avoir les faces mal orientees en noir.
    // Pourrait etre utiliser pour detecter les problemes d'orientation
    glLightModeli ((GLenum )GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    // Optimisation pour le Fog et l'antialiasing
    glHint (GL_FOG_HINT,            GL_FASTEST);
    glHint (GL_POINT_SMOOTH_HINT,   GL_FASTEST);
  }

  glHint (GL_LINE_SMOOTH_HINT,    GL_FASTEST);
  glHint (GL_POLYGON_SMOOTH_HINT, GL_FASTEST);
#endif

  // AA mode
  const char* anAaEnv = ::getenv ("CALL_OPENGL_ANTIALIASING_MODE");
  if (anAaEnv != NULL)
  {
    int v;
    if (sscanf (anAaEnv, "%d", &v) > 0) myAntiAliasingMode = v;
  }
}

// =======================================================================
// function : SetImmediateModeDrawToFront
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Workspace::SetImmediateModeDrawToFront (const Standard_Boolean theDrawToFrontBuffer)
{
  const Standard_Boolean aPrevMode = myTransientDrawToFront;
  myTransientDrawToFront = theDrawToFrontBuffer;
  return aPrevMode;
}

inline void nullifyGlResource (Handle(OpenGl_Resource)&      theResource,
                               const Handle(OpenGl_Context)& theCtx)
{
  if (!theResource.IsNull())
  {
    theResource->Release (theCtx.operator->());
    theResource.Nullify();
  }
}

// =======================================================================
// function : ~OpenGl_Workspace
// purpose  :
// =======================================================================
OpenGl_Workspace::~OpenGl_Workspace()
{
  if (!myLineAttribs.IsNull())
  {
    myLineAttribs.Nullify();
    myGlContext->ReleaseResource ("OpenGl_LineAttributes", Standard_True);
  }

  nullifyGlResource (myMainSceneFbos[0],      myGlContext);
  nullifyGlResource (myMainSceneFbos[1],      myGlContext);
  nullifyGlResource (myImmediateSceneFbos[0], myGlContext);
  nullifyGlResource (myImmediateSceneFbos[1], myGlContext);

  myFullScreenQuad.Release (myGlContext.operator->());
}

// =======================================================================
// function : Activate
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Workspace::Activate()
{
  if (!OpenGl_Window::Activate())
    return Standard_False;

  ViewMatrix_applied      = &myDefaultMatrix;
  StructureMatrix_applied = &myDefaultMatrix;

  ResetAppliedAspect();

  return Standard_True;
}

//=======================================================================
//function : ResetAppliedAspect
//purpose  : Sets default values of GL parameters in accordance with default aspects
//=======================================================================
void OpenGl_Workspace::ResetAppliedAspect()
{
  myGlContext->BindDefaultVao();

  NamedStatus           = !myTextureBound.IsNull() ? OPENGL_NS_TEXTURE : 0;
  HighlightColor        = &THE_WHITE_COLOR;
  AspectLine_set        = &myDefaultAspectLine;
  AspectLine_applied    = NULL;
  AspectFace_set        = &myDefaultAspectFace;
  AspectFace_applied    = NULL;
  AspectMarker_set      = &myDefaultAspectMarker;
  AspectMarker_applied  = NULL;
  AspectText_set        = &myDefaultAspectText;
  AspectText_applied    = NULL;
  TextParam_set         = &myDefaultTextParam;
  TextParam_applied     = NULL;
  PolygonOffset_applied = THE_DEFAULT_POFFSET;
  myCullingMode         = TelCullUndefined;

  AspectLine(Standard_True);
  AspectFace(Standard_True);
  AspectMarker(Standard_True);
  AspectText(Standard_True);
}

// =======================================================================
// function : DisableTexture
// purpose  :
// =======================================================================
Handle(OpenGl_Texture) OpenGl_Workspace::DisableTexture()
{
  if (myTextureBound.IsNull())
  {
    return myTextureBound;
  }

  const Handle(OpenGl_Sampler)& aSampler = myGlContext->TextureSampler();
  if (!aSampler.IsNull())
  {
    aSampler->Unbind (*myGlContext);
  }

#if !defined(GL_ES_VERSION_2_0)
  // reset texture matrix because some code may expect it is identity
  if (myGlContext->core11 != NULL)
  {
    GLint aMatrixMode = GL_TEXTURE;
    glGetIntegerv (GL_MATRIX_MODE, &aMatrixMode);
    glMatrixMode (GL_TEXTURE);
    glLoadIdentity();
    glMatrixMode (aMatrixMode);
  }
#endif

  myTextureBound->Unbind (myGlContext);
  switch (myTextureBound->GetTarget())
  {
  #if !defined(GL_ES_VERSION_2_0)
    case GL_TEXTURE_1D:
    {
      if (myGlContext->core11 != NULL)
      {
        if (myTextureBound->GetParams()->GenMode() != GL_NONE)
        {
          glDisable (GL_TEXTURE_GEN_S);
        }
        glDisable (GL_TEXTURE_1D);
      }
      break;
    }
  #endif
    case GL_TEXTURE_2D:
    {
    #if !defined(GL_ES_VERSION_2_0)
      if (myGlContext->core11 != NULL)
      {
        if (myTextureBound->GetParams()->GenMode() != GL_NONE)
        {
          glDisable (GL_TEXTURE_GEN_S);
          glDisable (GL_TEXTURE_GEN_T);
          if (myTextureBound->GetParams()->GenMode() == Graphic3d_TOTM_SPRITE)
          {
            glDisable (GL_POINT_SPRITE);
          }
        }
        glDisable (GL_TEXTURE_2D);
      }
    #endif
      break;
    }
    default: break;
  }

  Handle(OpenGl_Texture) aPrevTexture = myTextureBound;
  myTextureBound.Nullify();
  return aPrevTexture;
}

// =======================================================================
// function : setTextureParams
// purpose  :
// =======================================================================
void OpenGl_Workspace::setTextureParams (Handle(OpenGl_Texture)&                theTexture,
                                         const Handle(Graphic3d_TextureParams)& theParams)
{
  const Handle(Graphic3d_TextureParams)& aParams = theParams.IsNull() ? theTexture->GetParams() : theParams;
  if (aParams.IsNull())
  {
    return;
  }

#if !defined(GL_ES_VERSION_2_0)
  GLint aMatrixMode = GL_TEXTURE;
  if (myGlContext->core11 != NULL)
  {
    glGetIntegerv (GL_MATRIX_MODE, &aMatrixMode);

    // setup texture matrix
    glMatrixMode (GL_TEXTURE);
    OpenGl_Mat4 aTextureMat;
    const Graphic3d_Vec2& aScale = aParams->Scale();
    const Graphic3d_Vec2& aTrans = aParams->Translation();
    OpenGl_Utils::Scale     (aTextureMat,  aScale.x(),  aScale.y(), 1.0f);
    OpenGl_Utils::Translate (aTextureMat, -aTrans.x(), -aTrans.y(), 0.0f);
    OpenGl_Utils::Rotate    (aTextureMat, -aParams->Rotation(), 0.0f, 0.0f, 1.0f);
    glLoadMatrixf (aTextureMat);

    GLint anEnvMode = GL_MODULATE; // lighting mode
    if (!aParams->IsModulate())
    {
      anEnvMode = GL_DECAL;
      if (theTexture->GetFormat() == GL_ALPHA
       || theTexture->GetFormat() == GL_LUMINANCE)
      {
        anEnvMode = GL_REPLACE;
      }
    }

    // setup generation of texture coordinates
    switch (aParams->GenMode())
    {
      case Graphic3d_TOTM_OBJECT:
      {
        glTexGeni  (GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
        glTexGenfv (GL_S, GL_OBJECT_PLANE,     aParams->GenPlaneS().GetData());
        if (theTexture->GetTarget() != GL_TEXTURE_1D)
        {
          glTexGeni  (GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
          glTexGenfv (GL_T, GL_OBJECT_PLANE,     aParams->GenPlaneT().GetData());
        }
        break;
      }
      case Graphic3d_TOTM_SPHERE:
      {
        glTexGeni (GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
        if (theTexture->GetTarget() != GL_TEXTURE_1D)
        {
          glTexGeni (GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
        }
        break;
      }
      case Graphic3d_TOTM_EYE:
      {
        myGlContext->WorldViewState.Push();

        myGlContext->WorldViewState.SetIdentity();
        myGlContext->ApplyWorldViewMatrix();

        glTexGeni  (GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        glTexGenfv (GL_S, GL_EYE_PLANE,        aParams->GenPlaneS().GetData());

        if (theTexture->GetTarget() != GL_TEXTURE_1D)
        {
          glTexGeni  (GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
          glTexGenfv (GL_T, GL_EYE_PLANE,        aParams->GenPlaneT().GetData());
        }

        myGlContext->WorldViewState.Pop();

        break;
      }
      case Graphic3d_TOTM_SPRITE:
      {
        if (GetGlContext()->core20fwd != NULL)
        {
          glEnable  (GL_POINT_SPRITE);
          glTexEnvi (GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
          anEnvMode = GL_REPLACE;
          GetGlContext()->core15->glPointParameteri (GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);
        }
        break;
      }
      case Graphic3d_TOTM_MANUAL:
      default: break;
    }

    // setup lighting
    glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, anEnvMode);
  }
#endif

  // get active sampler object to override default texture parameters
  const Handle(OpenGl_Sampler)& aSampler = myGlContext->TextureSampler();

  // setup texture filtering and wrapping
  //if (theTexture->GetParams() != theParams)
  const GLenum aFilter   = (aParams->Filter() == Graphic3d_TOTF_NEAREST) ? GL_NEAREST : GL_LINEAR;
  const GLenum aWrapMode = aParams->IsRepeat() ? GL_REPEAT : myGlContext->TextureWrapClamp();
  switch (theTexture->GetTarget())
  {
  #if !defined(GL_ES_VERSION_2_0)
    case GL_TEXTURE_1D:
    {
      if (aSampler.IsNull() || !aSampler->IsValid())
      {
        glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, aFilter);
        glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, aFilter);
        glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_WRAP_S,     aWrapMode);
      }
      else
      {
        aSampler->SetParameter (*myGlContext, GL_TEXTURE_MAG_FILTER, aFilter);
        aSampler->SetParameter (*myGlContext, GL_TEXTURE_MIN_FILTER, aFilter);
        aSampler->SetParameter (*myGlContext, GL_TEXTURE_WRAP_S,     aWrapMode);
      }

      break;
    }
  #endif
    case GL_TEXTURE_2D:
    {
      GLenum aFilterMin = aFilter;
      if (theTexture->HasMipmaps())
      {
        aFilterMin = GL_NEAREST_MIPMAP_NEAREST;
        if (aParams->Filter() == Graphic3d_TOTF_BILINEAR)
        {
          aFilterMin = GL_LINEAR_MIPMAP_NEAREST;
        }
        else if (aParams->Filter() == Graphic3d_TOTF_TRILINEAR)
        {
          aFilterMin = GL_LINEAR_MIPMAP_LINEAR;
        }

        if (myGlContext->extAnis)
        {
          // setup degree of anisotropy filter
          const GLint aMaxDegree = myGlContext->MaxDegreeOfAnisotropy();
          GLint aDegree;
          switch (aParams->AnisoFilter())
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

          if (aSampler.IsNull() || !aSampler->IsValid())
          {
            glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aDegree);
          }
          else
          {
            aSampler->SetParameter (*myGlContext, GL_TEXTURE_MAX_ANISOTROPY_EXT, aDegree);
          }
        }
      }

      if (aSampler.IsNull() || !aSampler->IsValid())
      {
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, aFilterMin);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, aFilter);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     aWrapMode);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     aWrapMode);
      }
      else
      {
        aSampler->SetParameter (*myGlContext, GL_TEXTURE_MIN_FILTER, aFilterMin);
        aSampler->SetParameter (*myGlContext, GL_TEXTURE_MAG_FILTER, aFilter);
        aSampler->SetParameter (*myGlContext, GL_TEXTURE_WRAP_S,     aWrapMode);
        aSampler->SetParameter (*myGlContext, GL_TEXTURE_WRAP_T,     aWrapMode);
      }

      break;
    }
    default: break;
  }

  switch (theTexture->GetTarget())
  {
  #if !defined(GL_ES_VERSION_2_0)
    case GL_TEXTURE_1D:
    {
      if (myGlContext->core11 != NULL)
      {
        if (aParams->GenMode() != Graphic3d_TOTM_MANUAL)
        {
          glEnable (GL_TEXTURE_GEN_S);
        }
        glEnable (GL_TEXTURE_1D);
      }
      break;
    }
  #endif
    case GL_TEXTURE_2D:
    {
    #if !defined(GL_ES_VERSION_2_0)
      if (myGlContext->core11 != NULL)
      {
        if (aParams->GenMode() != Graphic3d_TOTM_MANUAL)
        {
          glEnable (GL_TEXTURE_GEN_S);
          glEnable (GL_TEXTURE_GEN_T);
        }
        glEnable (GL_TEXTURE_2D);
      }
    #endif
      break;
    }
    default: break;
  }

#if !defined(GL_ES_VERSION_2_0)
  if (myGlContext->core11 != NULL)
  {
    glMatrixMode (aMatrixMode); // turn back active matrix
  }
#endif
  theTexture->SetParams (aParams);
}

// =======================================================================
// function : EnableTexture
// purpose  :
// =======================================================================
Handle(OpenGl_Texture) OpenGl_Workspace::EnableTexture (const Handle(OpenGl_Texture)&          theTexture,
                                                        const Handle(Graphic3d_TextureParams)& theParams)
{
  if (theTexture.IsNull() || !theTexture->IsValid())
  {
    return DisableTexture();
  }

  if (myTextureBound == theTexture
   && (theParams.IsNull() || theParams == theTexture->GetParams()))
  {
    // already bound
    return myTextureBound;
  }

  Handle(OpenGl_Texture) aPrevTexture = DisableTexture();
  myTextureBound = theTexture;
  myTextureBound->Bind (myGlContext);
  setTextureParams (myTextureBound, theParams);

  // If custom sampler object is available it will be
  // used for overriding default texture parameters
  const Handle(OpenGl_Sampler)& aSampler = myGlContext->TextureSampler();

  if (!aSampler.IsNull() && aSampler->IsValid())
  {
    aSampler->Bind (*myGlContext);
  }

  return aPrevTexture;
}

// =======================================================================
// function : bindDefaultFbo
// purpose  :
// =======================================================================
void OpenGl_Workspace::bindDefaultFbo (OpenGl_FrameBuffer* theCustomFbo)
{
  OpenGl_FrameBuffer* anFbo = (theCustomFbo != NULL && theCustomFbo->IsValid())
                            ?  theCustomFbo
                            : (!myGlContext->DefaultFrameBuffer().IsNull()
                             && myGlContext->DefaultFrameBuffer()->IsValid()
                              ? myGlContext->DefaultFrameBuffer().operator->()
                              : NULL);
  if (anFbo != NULL)
  {
    anFbo->BindBuffer (myGlContext);
  }
  else
  {
  #if !defined(GL_ES_VERSION_2_0)
    myGlContext->SetReadDrawBuffer (GL_BACK);
  #else
    if (myGlContext->arbFBO != NULL)
    {
      myGlContext->arbFBO->glBindFramebuffer (GL_FRAMEBUFFER, OpenGl_FrameBuffer::NO_FRAMEBUFFER);
    }
  #endif
  }
  myGlContext->core11fwd->glViewport (0, 0, myWidth, myHeight);
}

// =======================================================================
// function : blitBuffers
// purpose  :
// =======================================================================
bool OpenGl_Workspace::blitBuffers (OpenGl_FrameBuffer* theReadFbo,
                                    OpenGl_FrameBuffer* theDrawFbo)
{
  if (theReadFbo == NULL)
  {
    return false;
  }

  // clear destination before blitting
  if (theDrawFbo != NULL
  &&  theDrawFbo->IsValid())
  {
    theDrawFbo->BindBuffer (myGlContext);
  }
  else
  {
    myGlContext->arbFBO->glBindFramebuffer (GL_FRAMEBUFFER, OpenGl_FrameBuffer::NO_FRAMEBUFFER);
  }
#if !defined(GL_ES_VERSION_2_0)
  myGlContext->core20fwd->glClearDepth  (1.0);
#else
  myGlContext->core20fwd->glClearDepthf (1.0f);
#endif
  myGlContext->core20fwd->glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

/*#if !defined(GL_ES_VERSION_2_0)
  if (myGlContext->arbFBOBlit != NULL)
  {
    theReadFbo->BindReadBuffer (myGlContext);
    if (theDrawFbo != NULL
     && theDrawFbo->IsValid())
    {
      theDrawFbo->BindDrawBuffer (myGlContext);
    }
    else
    {
      myGlContext->arbFBO->glBindFramebuffer (GL_DRAW_FRAMEBUFFER, OpenGl_FrameBuffer::NO_FRAMEBUFFER);
    }
    // we don't copy stencil buffer here... does it matter for performance?
    myGlContext->arbFBOBlit->glBlitFramebuffer (0, 0, theReadFbo->GetVPSizeX(), theReadFbo->GetVPSizeY(),
                                                0, 0, theReadFbo->GetVPSizeX(), theReadFbo->GetVPSizeY(),
                                                GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    if (theDrawFbo != NULL
      && theDrawFbo->IsValid())
    {
      theDrawFbo->BindBuffer (myGlContext);
    }
    else
    {
      myGlContext->arbFBO->glBindFramebuffer (GL_FRAMEBUFFER, OpenGl_FrameBuffer::NO_FRAMEBUFFER);
    }
  }
  else
#endif*/
  {
    myGlContext->core20fwd->glDepthFunc (GL_ALWAYS);
    myGlContext->core20fwd->glDepthMask (GL_TRUE);
    myGlContext->core20fwd->glEnable (GL_DEPTH_TEST);

    DisableTexture();
    if (!myFullScreenQuad.IsValid())
    {
      OpenGl_Vec4 aQuad[4] =
      {
        OpenGl_Vec4( 1.0f, -1.0f, 1.0f, 0.0f),
        OpenGl_Vec4( 1.0f,  1.0f, 1.0f, 1.0f),
        OpenGl_Vec4(-1.0f, -1.0f, 0.0f, 0.0f),
        OpenGl_Vec4(-1.0f,  1.0f, 0.0f, 1.0f)
      };
      myFullScreenQuad.Init (myGlContext, 4, 4, aQuad[0].GetData());
    }

    const Handle(OpenGl_ShaderManager)& aManager = myGlContext->ShaderManager();
    if (myFullScreenQuad.IsValid()
     && aManager->BindFboBlitProgram())
    {
      theReadFbo->ColorTexture()       ->Bind   (myGlContext, GL_TEXTURE0 + 0);
      theReadFbo->DepthStencilTexture()->Bind   (myGlContext, GL_TEXTURE0 + 1);
      myFullScreenQuad.BindVertexAttrib (myGlContext, Graphic3d_TOA_POS);

      myGlContext->core20fwd->glDrawArrays (GL_TRIANGLE_STRIP, 0, 4);

      myFullScreenQuad.UnbindVertexAttrib (myGlContext, Graphic3d_TOA_POS);
      theReadFbo->DepthStencilTexture()->Unbind (myGlContext, GL_TEXTURE0 + 1);
      theReadFbo->ColorTexture()       ->Unbind (myGlContext, GL_TEXTURE0 + 0);
    }
    else
    {
      TCollection_ExtendedString aMsg = TCollection_ExtendedString()
        + "Error! FBO blitting has failed";
      myGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
                                GL_DEBUG_TYPE_ERROR_ARB,
                                0,
                                GL_DEBUG_SEVERITY_HIGH_ARB,
                                aMsg);
      myHasFboBlit = Standard_False;
      theReadFbo->Release (myGlContext.operator->());
      return true;
    }
  }
  return true;
}

// =======================================================================
// function : drawStereoPair
// purpose  :
// =======================================================================
void OpenGl_Workspace::drawStereoPair()
{
  OpenGl_FrameBuffer* aPair[2] =
  {
    myImmediateSceneFbos[0]->IsValid() ? myImmediateSceneFbos[0].operator->() : NULL,
    myImmediateSceneFbos[1]->IsValid() ? myImmediateSceneFbos[1].operator->() : NULL
  };
  if (aPair[0] == NULL
   || aPair[1] == NULL)
  {
    aPair[0] = myMainSceneFbos[0]->IsValid() ? myMainSceneFbos[0].operator->() : NULL;
    aPair[1] = myMainSceneFbos[1]->IsValid() ? myMainSceneFbos[1].operator->() : NULL;
  }

  if (aPair[0] == NULL
   || aPair[1] == NULL)
  {
    return;
  }

  myGlContext->core20fwd->glDepthFunc (GL_ALWAYS);
  myGlContext->core20fwd->glDepthMask (GL_TRUE);
  myGlContext->core20fwd->glEnable (GL_DEPTH_TEST);

  DisableTexture();
  if (!myFullScreenQuad.IsValid())
  {
    OpenGl_Vec4 aQuad[4] =
    {
      OpenGl_Vec4( 1.0f, -1.0f, 1.0f, 0.0f),
      OpenGl_Vec4( 1.0f,  1.0f, 1.0f, 1.0f),
      OpenGl_Vec4(-1.0f, -1.0f, 0.0f, 0.0f),
      OpenGl_Vec4(-1.0f,  1.0f, 0.0f, 1.0f)
    };
    myFullScreenQuad.Init (myGlContext, 4, 4, aQuad[0].GetData());
  }

  const Handle(OpenGl_ShaderManager)& aManager = myGlContext->ShaderManager();
  if (myFullScreenQuad.IsValid()
   && aManager->BindAnaglyphProgram())
  {
    aPair[0]->ColorTexture()->Bind (myGlContext, GL_TEXTURE0 + 0);
    aPair[1]->ColorTexture()->Bind (myGlContext, GL_TEXTURE0 + 1);
    myFullScreenQuad.BindVertexAttrib (myGlContext, 0);

    myGlContext->core20fwd->glDrawArrays (GL_TRIANGLE_STRIP, 0, 4);

    myFullScreenQuad.UnbindVertexAttrib (myGlContext, 0);
    aPair[1]->ColorTexture()->Unbind (myGlContext, GL_TEXTURE0 + 1);
    aPair[0]->ColorTexture()->Unbind (myGlContext, GL_TEXTURE0 + 0);
  }
  else
  {
    TCollection_ExtendedString aMsg = TCollection_ExtendedString()
      + "Error! Anaglyph has failed";
    myGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
                              GL_DEBUG_TYPE_ERROR_ARB,
                              0,
                              GL_DEBUG_SEVERITY_HIGH_ARB,
                              aMsg);
  }
}

// =======================================================================
// function : Redraw
// purpose  :
// =======================================================================
void OpenGl_Workspace::Redraw (const Graphic3d_CView& theCView,
                               const Aspect_CLayer2d& theCUnderLayer,
                               const Aspect_CLayer2d& theCOverLayer)
{
  if (!Activate())
  {
    return;
  }

  ++myFrameCounter;
  myIsCullingEnabled = theCView.IsCullingEnabled;

  // release pending GL resources
  myGlContext->ReleaseDelayed();

  // fetch OpenGl context state
  myGlContext->FetchState();

  OpenGl_FrameBuffer* aFrameBuffer = (OpenGl_FrameBuffer* )theCView.ptrFBO;
  bool toSwap = myGlContext->IsRender()
            && !myGlContext->caps->buffersNoSwap
            &&  aFrameBuffer == NULL;

  Standard_Integer aSizeX = aFrameBuffer != NULL ? aFrameBuffer->GetVPSizeX() : myWidth;
  Standard_Integer aSizeY = aFrameBuffer != NULL ? aFrameBuffer->GetVPSizeY() : myHeight;

  if ( aFrameBuffer == NULL
   && !myGlContext->DefaultFrameBuffer().IsNull()
   &&  myGlContext->DefaultFrameBuffer()->IsValid())
  {
    aFrameBuffer = myGlContext->DefaultFrameBuffer().operator->();
  }

  if (myHasFboBlit
   && myTransientDrawToFront)
  {
    if (myMainSceneFbos[0]->GetVPSizeX() != aSizeX
     || myMainSceneFbos[0]->GetVPSizeY() != aSizeY)
    {
      // prepare FBOs containing main scene
      // for further blitting and rendering immediate presentations on top
      if (myGlContext->core20fwd != NULL)
      {
        myMainSceneFbos[0]->Init (myGlContext, aSizeX, aSizeY);
      }
    }
  }
  else
  {
    myMainSceneFbos     [0]->Release (myGlContext.operator->());
    myMainSceneFbos     [1]->Release (myGlContext.operator->());
    myImmediateSceneFbos[0]->Release (myGlContext.operator->());
    myImmediateSceneFbos[1]->Release (myGlContext.operator->());
    myMainSceneFbos     [0]->ChangeViewport (0, 0);
    myMainSceneFbos     [1]->ChangeViewport (0, 0);
    myImmediateSceneFbos[0]->ChangeViewport (0, 0);
    myImmediateSceneFbos[1]->ChangeViewport (0, 0);
  }

  // draw entire frame using normal OpenGL pipeline
  const Handle(Graphic3d_Camera)& aCamera      = myView->Camera();
  Graphic3d_Camera::Projection    aProjectType = aCamera->ProjectionType();
  if (aProjectType == Graphic3d_Camera::Projection_Stereo)
  {
    if (aFrameBuffer != NULL
    || !myGlContext->IsRender())
    {
      // implicitly switch to mono camera for image dump
      aProjectType = Graphic3d_Camera::Projection_Perspective;
    }
    else if (myMainSceneFbos[0]->IsValid())
    {
      myMainSceneFbos[1]->InitLazy (myGlContext, aSizeX, aSizeY);
      if (!myMainSceneFbos[1]->IsValid())
      {
        // no enough memory?
        aProjectType = Graphic3d_Camera::Projection_Perspective;
      }
      else if (!myGlContext->HasStereoBuffers())
      {
        myImmediateSceneFbos[0]->InitLazy (myGlContext, aSizeX, aSizeY);
        myImmediateSceneFbos[1]->InitLazy (myGlContext, aSizeX, aSizeY);
        if (!myImmediateSceneFbos[0]->IsValid()
         || !myImmediateSceneFbos[1]->IsValid())
        {
          aProjectType = Graphic3d_Camera::Projection_Perspective;
        }
      }
    }
  }

  if (aProjectType == Graphic3d_Camera::Projection_Stereo)
  {
    OpenGl_FrameBuffer* aMainFbos[2] =
    {
      myMainSceneFbos[0]->IsValid() ? myMainSceneFbos[0].operator->() : NULL,
      myMainSceneFbos[1]->IsValid() ? myMainSceneFbos[1].operator->() : NULL
    };
    OpenGl_FrameBuffer* anImmFbos[2] =
    {
      myImmediateSceneFbos[0]->IsValid() ? myImmediateSceneFbos[0].operator->() : NULL,
      myImmediateSceneFbos[1]->IsValid() ? myImmediateSceneFbos[1].operator->() : NULL
    };

  #if !defined(GL_ES_VERSION_2_0)
    myGlContext->SetReadDrawBuffer (GL_BACK_LEFT);
  #endif
    redraw1 (theCView, theCUnderLayer, theCOverLayer,
             aMainFbos[0], Graphic3d_Camera::Projection_MonoLeftEye);
    myBackBufferRestored = Standard_True;
    myIsImmediateDrawn   = Standard_False;
  #if !defined(GL_ES_VERSION_2_0)
    myGlContext->SetReadDrawBuffer (GL_BACK_LEFT);
  #endif
    if (!redrawImmediate (theCView, theCOverLayer, theCUnderLayer, aMainFbos[0], aProjectType, anImmFbos[0]))
    {
      toSwap = false;
    }

  #if !defined(GL_ES_VERSION_2_0)
    myGlContext->SetReadDrawBuffer (GL_BACK_RIGHT);
  #endif
    redraw1 (theCView, theCUnderLayer, theCOverLayer,
             aMainFbos[1], Graphic3d_Camera::Projection_MonoRightEye);
    myBackBufferRestored = Standard_True;
    myIsImmediateDrawn   = Standard_False;
    if (!redrawImmediate (theCView, theCOverLayer, theCUnderLayer, aMainFbos[1], aProjectType, anImmFbos[1]))
    {
      toSwap = false;
    }

    if (anImmFbos[0] != NULL)
    {
      bindDefaultFbo (aFrameBuffer);
      drawStereoPair();
    }
  }
  else
  {
    OpenGl_FrameBuffer* aMainFbo = myMainSceneFbos[0]->IsValid() ? myMainSceneFbos[0].operator->() : NULL;
  #if !defined(GL_ES_VERSION_2_0)
    if (aMainFbo     == NULL
     && aFrameBuffer == NULL)
    {
      myGlContext->SetReadDrawBuffer (GL_BACK);
    }
  #endif
    redraw1 (theCView, theCUnderLayer, theCOverLayer,
             aMainFbo != NULL ? aMainFbo : aFrameBuffer, aProjectType);
    myBackBufferRestored = Standard_True;
    myIsImmediateDrawn   = Standard_False;
    if (!redrawImmediate (theCView, theCOverLayer, theCUnderLayer, aMainFbo, aProjectType, aFrameBuffer))
    {
      toSwap = false;
    }
  }

#if defined(_WIN32) && defined(HAVE_VIDEOCAPTURE)
  if (OpenGl_AVIWriter_AllowWriting (theCView.DefWindow.XWindow))
  {
    GLint params[4];
    glGetIntegerv (GL_VIEWPORT, params);
    int nWidth  = params[2] & ~0x7;
    int nHeight = params[3] & ~0x7;

    const int nBitsPerPixel = 24;
    GLubyte* aDumpData = new GLubyte[nWidth * nHeight * nBitsPerPixel / 8];

    glPixelStorei (GL_PACK_ALIGNMENT, 1);
    glReadPixels (0, 0, nWidth, nHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, aDumpData);
    OpenGl_AVIWriter_AVIWriter (aDumpData, nWidth, nHeight, nBitsPerPixel);
    delete[] aDumpData;
  }
#endif

  // bind default FBO
  bindDefaultFbo();

  // Swap the buffers
  if (toSwap)
  {
    GetGlContext()->SwapBuffers();
    if (!myMainSceneFbos[0]->IsValid())
    {
      myBackBufferRestored = Standard_False;
    }
  }
  else
  {
    myGlContext->core11fwd->glFlush();
  }

  // reset render mode state
  myGlContext->FetchState();
}

// =======================================================================
// function : redraw1
// purpose  :
// =======================================================================
void OpenGl_Workspace::redraw1 (const Graphic3d_CView&               theCView,
                                const Aspect_CLayer2d&               theCUnderLayer,
                                const Aspect_CLayer2d&               theCOverLayer,
                                OpenGl_FrameBuffer*                  theReadDrawFbo,
                                const Graphic3d_Camera::Projection   theProjection)
{
  if (myView.IsNull())
  {
    return;
  }

  if (theReadDrawFbo != NULL)
  {
    theReadDrawFbo->BindBuffer    (myGlContext);
    theReadDrawFbo->SetupViewport (myGlContext);
  }
  else
  {
    myGlContext->core11fwd->glViewport (0, 0, myWidth, myHeight);
  }

  // request reset of material
  NamedStatus |= OPENGL_NS_RESMAT;

  GLbitfield toClear = GL_COLOR_BUFFER_BIT;
  if (myUseZBuffer)
  {
    glDepthFunc (GL_LEQUAL);
    glDepthMask (GL_TRUE);
    if (myUseDepthTest)
    {
      glEnable (GL_DEPTH_TEST);
    }
    else
    {
      glDisable (GL_DEPTH_TEST);
    }

  #if !defined(GL_ES_VERSION_2_0)
    glClearDepth (1.0);
  #else
    glClearDepthf (1.0f);
  #endif
    toClear |= GL_DEPTH_BUFFER_BIT;
  }
  else
  {
    glDisable (GL_DEPTH_TEST);
  }

  if (NamedStatus & OPENGL_NS_WHITEBACK)
  {
    // set background to white
    glClearColor (1.0f, 1.0f, 1.0f, 1.0f);
    toClear |= GL_DEPTH_BUFFER_BIT;
  }
  else
  {
    glClearColor (myBgColor.rgb[0], myBgColor.rgb[1], myBgColor.rgb[2], 0.0f);
  }

  glClear (toClear);

  Handle(OpenGl_Workspace) aWS (this);
  myView->Render (myPrintContext, aWS, theReadDrawFbo, theProjection, theCView, theCUnderLayer, theCOverLayer, Standard_False);
}

// =======================================================================
// function : copyBackToFront
// purpose  :
// =======================================================================
void OpenGl_Workspace::copyBackToFront()
{
#if !defined(GL_ES_VERSION_2_0)

  OpenGl_Mat4 aProjectMat;
  OpenGl_Utils::Ortho2D<Standard_ShortReal> (aProjectMat,
    0.f, static_cast<GLfloat> (myWidth), 0.f, static_cast<GLfloat> (myHeight));

  myGlContext->WorldViewState.Push();
  myGlContext->ProjectionState.Push();

  myGlContext->WorldViewState.SetIdentity();
  myGlContext->ProjectionState.SetCurrent (aProjectMat);

  myGlContext->ApplyProjectionMatrix();
  myGlContext->ApplyWorldViewMatrix();

  DisableFeatures();

  switch (myGlContext->DrawBuffer())
  {
    case GL_BACK_LEFT:
    {
      myGlContext->SetReadBuffer (GL_BACK_LEFT);
      myGlContext->SetDrawBuffer (GL_FRONT_LEFT);
      break;
    }
    case GL_BACK_RIGHT:
    {
      myGlContext->SetReadBuffer (GL_BACK_RIGHT);
      myGlContext->SetDrawBuffer (GL_FRONT_RIGHT);
      break;
    }
    default:
    {
      myGlContext->SetReadBuffer (GL_BACK);
      myGlContext->SetDrawBuffer (GL_FRONT);
      break;
    }
  }

  glRasterPos2i (0, 0);
  glCopyPixels  (0, 0, myWidth + 1, myHeight + 1, GL_COLOR);
  //glCopyPixels  (0, 0, myWidth + 1, myHeight + 1, GL_DEPTH);

  EnableFeatures();

  myGlContext->WorldViewState.Pop();
  myGlContext->ProjectionState.Pop();
  myGlContext->ApplyProjectionMatrix();

  // read/write from front buffer now
  myGlContext->SetReadBuffer (myGlContext->DrawBuffer());
#endif
  myIsImmediateDrawn = Standard_False;
}

// =======================================================================
// function : DisplayCallback
// purpose  :
// =======================================================================
void OpenGl_Workspace::DisplayCallback (const Graphic3d_CView& theCView,
                                        Standard_Integer       theReason)
{
  if (theCView.GDisplayCB == NULL)
  {
    return;
  }

  Aspect_GraphicCallbackStruct aCallData;
  aCallData.reason    = theReason;
  aCallData.glContext = myGlContext;
  aCallData.wsID      = theCView.WsId;
  aCallData.viewID    = theCView.ViewId;
  aCallData.IsCoreProfile = (myGlContext->core11 == NULL);
  theCView.GDisplayCB (theCView.DefWindow.XWindow, theCView.GClientData, &aCallData);
}

// =======================================================================
// function : RedrawImmediate
// purpose  :
// =======================================================================
void OpenGl_Workspace::RedrawImmediate (const Graphic3d_CView& theCView,
                                        const Aspect_CLayer2d& theCUnderLayer,
                                        const Aspect_CLayer2d& theCOverLayer)
{
  const Handle(Graphic3d_Camera)& aCamera      = myView->Camera();
  Graphic3d_Camera::Projection    aProjectType = aCamera->ProjectionType();
  OpenGl_FrameBuffer* aFrameBuffer = (OpenGl_FrameBuffer* )theCView.ptrFBO;
  if ( aFrameBuffer == NULL
   && !myGlContext->DefaultFrameBuffer().IsNull()
   &&  myGlContext->DefaultFrameBuffer()->IsValid())
  {
    aFrameBuffer = myGlContext->DefaultFrameBuffer().operator->();
  }

  if (aProjectType == Graphic3d_Camera::Projection_Stereo)
  {
    if (aFrameBuffer != NULL)
    {
      // implicitly switch to mono camera for image dump
      aProjectType = Graphic3d_Camera::Projection_Perspective;
    }
    else if (myMainSceneFbos[0]->IsValid()
         && !myMainSceneFbos[1]->IsValid())
    {
      aProjectType = Graphic3d_Camera::Projection_Perspective;
    }
  }

  if (!myTransientDrawToFront
   || !myBackBufferRestored
   || (myGlContext->caps->buffersNoSwap && !myMainSceneFbos[0]->IsValid()))
  {
    Redraw (theCView, theCUnderLayer, theCOverLayer);
    return;
  }
  else if (!Activate())
  {
    return;
  }

  bool toSwap = false;
  if (aProjectType == Graphic3d_Camera::Projection_Stereo)
  {
    OpenGl_FrameBuffer* aMainFbos[2] =
    {
      myMainSceneFbos[0]->IsValid() ? myMainSceneFbos[0].operator->() : NULL,
      myMainSceneFbos[1]->IsValid() ? myMainSceneFbos[1].operator->() : NULL
    };
    OpenGl_FrameBuffer* anImmFbos[2] =
    {
      myImmediateSceneFbos[0]->IsValid() ? myImmediateSceneFbos[0].operator->() : NULL,
      myImmediateSceneFbos[1]->IsValid() ? myImmediateSceneFbos[1].operator->() : NULL
    };

    if (myGlContext->arbFBO != NULL)
    {
      myGlContext->arbFBO->glBindFramebuffer (GL_FRAMEBUFFER, OpenGl_FrameBuffer::NO_FRAMEBUFFER);
    }
  #if !defined(GL_ES_VERSION_2_0)
    if (anImmFbos[0] == NULL)
    {
      myGlContext->SetReadDrawBuffer (GL_BACK_LEFT);
    }
  #endif
    toSwap = redrawImmediate (theCView, theCUnderLayer, theCOverLayer,
                              aMainFbos[0],
                              Graphic3d_Camera::Projection_MonoLeftEye,
                              anImmFbos[0],
                              Standard_True) || toSwap;

    if (myGlContext->arbFBO != NULL)
    {
      myGlContext->arbFBO->glBindFramebuffer (GL_FRAMEBUFFER, OpenGl_FrameBuffer::NO_FRAMEBUFFER);
    }
  #if !defined(GL_ES_VERSION_2_0)
    if (anImmFbos[1] == NULL)
    {
      myGlContext->SetReadDrawBuffer (GL_BACK_RIGHT);
    }
  #endif
    toSwap = redrawImmediate (theCView, theCUnderLayer, theCOverLayer,
                              aMainFbos[1],
                              Graphic3d_Camera::Projection_MonoRightEye,
                              anImmFbos[1],
                              Standard_True) || toSwap;
    if (anImmFbos[0] != NULL)
    {
      bindDefaultFbo (aFrameBuffer);
      drawStereoPair();
    }
  }
  else
  {
    OpenGl_FrameBuffer* aMainFbo = myMainSceneFbos[0]->IsValid() ? myMainSceneFbos[0].operator->() : NULL;
  #if !defined(GL_ES_VERSION_2_0)
    if (aMainFbo == NULL)
    {
      myGlContext->SetReadDrawBuffer (GL_BACK);
    }
  #endif
    toSwap = redrawImmediate (theCView, theCUnderLayer, theCOverLayer,
                              aMainFbo,
                              aProjectType,
                              aFrameBuffer,
                              Standard_True) || toSwap;
  }

  // bind default FBO
  bindDefaultFbo();

  if (toSwap
  && !myGlContext->caps->buffersNoSwap)
  {
    myGlContext->SwapBuffers();
  }
  else
  {
    myGlContext->core11fwd->glFlush();
  }
}

// =======================================================================
// function : redrawImmediate
// purpose  :
// =======================================================================
bool OpenGl_Workspace::redrawImmediate (const Graphic3d_CView& theCView,
                                        const Aspect_CLayer2d& theCUnderLayer,
                                        const Aspect_CLayer2d& theCOverLayer,
                                        OpenGl_FrameBuffer*    theReadFbo,
                                        const Graphic3d_Camera::Projection theProjection,
                                        OpenGl_FrameBuffer*    theDrawFbo,
                                        const Standard_Boolean theIsPartialUpdate)
{
  GLboolean toCopyBackToFront = GL_FALSE;
  if (!myTransientDrawToFront)
  {
    myBackBufferRestored = Standard_False;
  }
  else if (theReadFbo != NULL
        && theReadFbo->IsValid()
        && myGlContext->IsRender())
  {
    if (!blitBuffers (theReadFbo, theDrawFbo))
    {
      return true;
    }
  }
  else if (theDrawFbo == NULL)
  {
  #if !defined(GL_ES_VERSION_2_0)
    myGlContext->core11fwd->glGetBooleanv (GL_DOUBLEBUFFER, &toCopyBackToFront);
  #endif
    if (toCopyBackToFront)
    {
      if (!myView->HasImmediateStructures()
       && !theIsPartialUpdate)
      {
        // prefer Swap Buffers within Redraw in compatibility mode (without FBO)
        return true;
      }
      copyBackToFront();
    }
    else
    {
      myBackBufferRestored = Standard_False;
    }
  }
  else
  {
    myBackBufferRestored = Standard_False;
  }
  myIsImmediateDrawn = Standard_True;

  Handle(OpenGl_Workspace) aWS (this);

  if (myUseZBuffer)
  {
    glDepthFunc (GL_LEQUAL);
    glDepthMask (GL_TRUE);
    if (myUseDepthTest)
    {
      glEnable (GL_DEPTH_TEST);
    }
    else
    {
      glDisable (GL_DEPTH_TEST);
    }

  #if !defined(GL_ES_VERSION_2_0)
    glClearDepth (1.0);
  #else
    glClearDepthf (1.0f);
  #endif
  }
  else
  {
    glDisable (GL_DEPTH_TEST);
  }

  myView->Render (myPrintContext, aWS, theDrawFbo, theProjection,
                  theCView, theCUnderLayer, theCOverLayer, Standard_True);
  if (!myView->ImmediateStructures().IsEmpty())
  {
    glDisable (GL_DEPTH_TEST);
  }
  for (OpenGl_SequenceOfStructure::Iterator anIter (myView->ImmediateStructures());
       anIter.More(); anIter.Next())
  {
    const OpenGl_Structure* aStructure = anIter.Value();
    if (!aStructure->IsVisible())
    {
      continue;
    }

    aStructure->Render (aWS);
  }

  return !toCopyBackToFront;
}

IMPLEMENT_STANDARD_HANDLE (OpenGl_RaytraceFilter, OpenGl_RenderFilter)
IMPLEMENT_STANDARD_RTTIEXT(OpenGl_RaytraceFilter, OpenGl_RenderFilter)

// =======================================================================
// function : CanRender
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_RaytraceFilter::CanRender (const OpenGl_Element* theElement)
{
  Standard_Boolean aPrevFilterResult = Standard_True;
  if (!myPrevRenderFilter.IsNull())
  {
    aPrevFilterResult = myPrevRenderFilter->CanRender (theElement);
  }
  return aPrevFilterResult &&
    !OpenGl_Raytrace::IsRaytracedElement (theElement);
}
