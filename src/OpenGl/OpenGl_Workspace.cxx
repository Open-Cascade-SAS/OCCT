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

#include <InterfaceGraphic.hxx>

#include <OpenGl_AspectLine.hxx>
#include <OpenGl_AspectFace.hxx>
#include <OpenGl_AspectMarker.hxx>
#include <OpenGl_AspectText.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_Element.hxx>
#include <OpenGl_FrameBuffer.hxx>
#include <OpenGl_Structure.hxx>
#include <OpenGl_Texture.hxx>
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
  myComputeInitStatus (OpenGl_RT_NONE),
  myIsRaytraceDataValid (Standard_False),
  myViewModificationStatus (0),
  myLayersModificationStatus (0),
  //
  myRaytraceFilter       (new OpenGl_RaytraceFilter()),
  myToRedrawGL           (Standard_True),
  myAntiAliasingMode     (3),
  myTransientDrawToFront (Standard_True),
  myBackBufferRestored   (Standard_False),
  myIsImmediateDrawn     (Standard_False),
  myUseTransparency (Standard_False),
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

  if (!myGlContext->GetResource ("OpenGl_LineAttributes", myLineAttribs))
  {
    // share and register for release once the resource is no longer used
    myLineAttribs = new OpenGl_LineAttributes();
    myGlContext->ShareResource ("OpenGl_LineAttributes", myLineAttribs);
    myLineAttribs->Init (myGlContext);
  }

  // General initialization of the context

#if !defined(GL_ES_VERSION_2_0)
  // Eviter d'avoir les faces mal orientees en noir.
  // Pourrait etre utiliser pour detecter les problemes d'orientation
  glLightModeli ((GLenum )GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

  // Optimisation pour le Fog et l'antialiasing
  glHint (GL_FOG_HINT,            GL_FASTEST);
  glHint (GL_POINT_SMOOTH_HINT,   GL_FASTEST);
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

  ReleaseRaytraceResources();
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

// =======================================================================
// function : UseTransparency
// purpose  : call_togl_transparency
// =======================================================================
void OpenGl_Workspace::UseTransparency (const Standard_Boolean theFlag)
{
  myUseTransparency = theFlag;
}

//=======================================================================
//function : ResetAppliedAspect
//purpose  : Sets default values of GL parameters in accordance with default aspects
//=======================================================================
void OpenGl_Workspace::ResetAppliedAspect()
{
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

#if !defined(GL_ES_VERSION_2_0)
  // reset texture matrix because some code may expect it is identity
  GLint aMatrixMode = GL_TEXTURE;
  glGetIntegerv (GL_MATRIX_MODE, &aMatrixMode);
  glMatrixMode (GL_TEXTURE);
  glLoadIdentity();
  glMatrixMode (aMatrixMode);
#endif

  myTextureBound->Unbind (myGlContext);
  switch (myTextureBound->GetTarget())
  {
  #if !defined(GL_ES_VERSION_2_0)
    case GL_TEXTURE_1D:
    {

      if (myTextureBound->GetParams()->GenMode() != GL_NONE)
      {
        glDisable (GL_TEXTURE_GEN_S);
      }
      glDisable (GL_TEXTURE_1D);
      break;
    }
  #endif
    case GL_TEXTURE_2D:
    {
    #if !defined(GL_ES_VERSION_2_0)
      if (myTextureBound->GetParams()->GenMode() != GL_NONE)
      {
        glDisable (GL_TEXTURE_GEN_S);
        glDisable (GL_TEXTURE_GEN_T);
        if (myTextureBound->GetParams()->GenMode() == Graphic3d_TOTM_SPRITE)
        {
          glDisable (GL_POINT_SPRITE);
        }
      }
    #endif
      glDisable (GL_TEXTURE_2D);
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
  glGetIntegerv (GL_MATRIX_MODE, &aMatrixMode);

  // setup texture matrix
  glMatrixMode (GL_TEXTURE);
  glLoadIdentity();
  const Graphic3d_Vec2& aScale = aParams->Scale();
  const Graphic3d_Vec2& aTrans = aParams->Translation();
  glScalef     ( aScale.x(),  aScale.y(), 1.0f);
  glTranslatef (-aTrans.x(), -aTrans.y(), 0.0f);
  glRotatef (-aParams->Rotation(), 0.0f, 0.0f, 1.0f);

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
      glMatrixMode (GL_MODELVIEW);
      glPushMatrix();
      glLoadIdentity();

      glTexGeni  (GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
      glTexGenfv (GL_S, GL_EYE_PLANE,        aParams->GenPlaneS().GetData());

      if (theTexture->GetTarget() != GL_TEXTURE_1D)
      {
        glTexGeni  (GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        glTexGenfv (GL_T, GL_EYE_PLANE,        aParams->GenPlaneT().GetData());
      }
      glPopMatrix();
      break;
    }
    case Graphic3d_TOTM_SPRITE:
    {
      if (GetGlContext()->core20 != NULL)
      {
        glEnable  (GL_POINT_SPRITE);
        glTexEnvi (GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
        glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        GetGlContext()->core15->glPointParameteri (GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);
      }
      break;
    }
    case Graphic3d_TOTM_MANUAL:
    default: break;
  }

  // setup lighting
  if (aParams->GenMode() != Graphic3d_TOTM_SPRITE)
  {
    glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, aParams->IsModulate() ? GL_MODULATE : GL_DECAL);
  }
#endif

  // setup texture filtering and wrapping
  //if (theTexture->GetParams() != theParams)
  const GLenum aFilter   = (aParams->Filter() == Graphic3d_TOTF_NEAREST) ? GL_NEAREST : GL_LINEAR;
  const GLenum aWrapMode = aParams->IsRepeat() ? GL_REPEAT : myGlContext->TextureWrapClamp();
  switch (theTexture->GetTarget())
  {
  #if !defined(GL_ES_VERSION_2_0)
    case GL_TEXTURE_1D:
    {
      glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, aFilter);
      glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, aFilter);
      glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_WRAP_S,     aWrapMode);
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
          switch (aParams->AnisoFilter())
          {
            case Graphic3d_LOTA_QUALITY:
            {
              glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aMaxDegree);
              break;
            }
            case Graphic3d_LOTA_MIDDLE:
            {

              glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, (aMaxDegree <= 4) ? 2 : (aMaxDegree / 2));
              break;
            }
            case Graphic3d_LOTA_FAST:
            {
              glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 2);
              break;
            }
            case Graphic3d_LOTA_OFF:
            default:
            {
              glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1);
              break;
            }
          }
        }
      }
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, aFilterMin);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, aFilter);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     aWrapMode);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     aWrapMode);
      break;
    }
    default: break;
  }

  switch (theTexture->GetTarget())
  {
  #if !defined(GL_ES_VERSION_2_0)
    case GL_TEXTURE_1D:
    {
      if (aParams->GenMode() != Graphic3d_TOTM_MANUAL)
      {
        glEnable (GL_TEXTURE_GEN_S);
      }
      glEnable (GL_TEXTURE_1D);
      break;
    }
  #endif
    case GL_TEXTURE_2D:
    {
    #if !defined(GL_ES_VERSION_2_0)
      if (aParams->GenMode() != Graphic3d_TOTM_MANUAL)
      {
        glEnable (GL_TEXTURE_GEN_S);
        glEnable (GL_TEXTURE_GEN_T);
      }
    #endif
      glEnable (GL_TEXTURE_2D);
      break;
    }
    default: break;
  }

#if !defined(GL_ES_VERSION_2_0)
  glMatrixMode (aMatrixMode); // turn back active matrix
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

  return aPrevTexture;
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
  Handle(OpenGl_Context) aGlCtx = GetGlContext();
  aGlCtx->ReleaseDelayed();

  // fetch OpenGl context state
  aGlCtx->FetchState();

  Tint toSwap = (aGlCtx->IsRender() && !aGlCtx->caps->buffersNoSwap) ? 1 : 0; // swap buffers
  GLint aViewPortBack[4];
  OpenGl_FrameBuffer* aFrameBuffer = (OpenGl_FrameBuffer* )theCView.ptrFBO;
  if (aFrameBuffer != NULL)
  {
    glGetIntegerv (GL_VIEWPORT, aViewPortBack);
    aFrameBuffer->SetupViewport (aGlCtx);
    toSwap = 0; // no need to swap buffers
  }

  myToRedrawGL = Standard_True;
  if (theCView.RenderParams.Method == Graphic3d_RM_RAYTRACING
   && myComputeInitStatus != OpenGl_RT_FAIL)
  {
    if (UpdateRaytraceGeometry (OpenGl_GUM_CHECK) && myIsRaytraceDataValid)
    {
      myToRedrawGL = Standard_False;

      // Only non-raytracable structures should be rendered in OpenGL mode.
      Handle(OpenGl_RenderFilter) aRenderFilter = GetRenderFilter();
      myRaytraceFilter->SetPrevRenderFilter (aRenderFilter);
      SetRenderFilter (myRaytraceFilter);

      Standard_Integer aSizeX = aFrameBuffer != NULL ? aFrameBuffer->GetVPSizeX() : myWidth;
      Standard_Integer aSizeY = aFrameBuffer != NULL ? aFrameBuffer->GetVPSizeY() : myHeight;

      if (myOpenGlFBO.IsNull())
      {
        myOpenGlFBO = new OpenGl_FrameBuffer();
      }
      if (myOpenGlFBO->GetVPSizeX() != aSizeX
       || myOpenGlFBO->GetVPSizeY() != aSizeY)
      {
        myOpenGlFBO->Init (aGlCtx, aSizeX, aSizeY);
      }

      // OverLayer and UnderLayer shouldn't be drawn by OpenGL.
      // They will be drawn during ray-tracing.
      Aspect_CLayer2d anEmptyCLayer;
      anEmptyCLayer.ptrLayer = NULL;

      myOpenGlFBO->BindBuffer (aGlCtx);
      redraw1 (theCView, anEmptyCLayer, anEmptyCLayer, 0);
      myOpenGlFBO->UnbindBuffer (aGlCtx);

      const Standard_Boolean isImmediate = !myView->ImmediateStructures().IsEmpty();
      Raytrace (theCView, aSizeX, aSizeY, isImmediate ? 0 : toSwap,
                theCOverLayer, theCUnderLayer, aFrameBuffer);

      if (isImmediate)
      {
        RedrawImmediate (theCView, theCUnderLayer, theCOverLayer, Standard_True);
      }

      SetRenderFilter (aRenderFilter);

      theCView.WasRedrawnGL = Standard_False;
    }
  }

  if (myToRedrawGL)
  {
    // draw entire frame using normal OpenGL pipeline
    if (aFrameBuffer != NULL)
    {
      aFrameBuffer->BindBuffer (aGlCtx);
    }

    const Standard_Boolean isImmediate = !myView->ImmediateStructures().IsEmpty();
    redraw1 (theCView, theCUnderLayer, theCOverLayer, isImmediate ? 0 : toSwap);
    if (isImmediate)
    {
      RedrawImmediate (theCView, theCUnderLayer, theCOverLayer, Standard_True);
    }

    theCView.WasRedrawnGL = Standard_True;
  }

  if (aFrameBuffer != NULL)
  {
    aFrameBuffer->UnbindBuffer (aGlCtx);
    // move back original viewport
    glViewport (aViewPortBack[0], aViewPortBack[1], aViewPortBack[2], aViewPortBack[3]);
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

  // reset render mode state
  aGlCtx->FetchState();
}

// =======================================================================
// function : redraw1
// purpose  :
// =======================================================================
void OpenGl_Workspace::redraw1 (const Graphic3d_CView& theCView,
                                const Aspect_CLayer2d& theCUnderLayer,
                                const Aspect_CLayer2d& theCOverLayer,
                                const int              theToSwap)
{
  if (myView.IsNull())
  {
    return;
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

  if (!ToRedrawGL())
  {
    // set background to black
    glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
    toClear |= GL_DEPTH_BUFFER_BIT; //
  }
  else
  {
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
  }
  glClear (toClear);

  Handle(OpenGl_Workspace) aWS (this);
  myView->Render (myPrintContext, aWS, theCView, theCUnderLayer, theCOverLayer);

  // swap the buffers
  if (theToSwap)
  {
    GetGlContext()->SwapBuffers();
    myBackBufferRestored = Standard_False;
    myIsImmediateDrawn   = Standard_False;
  }
  else
  {
    glFlush(); //
    myBackBufferRestored = Standard_True;//
    myIsImmediateDrawn   = Standard_False;//
  }
}

// =======================================================================
// function : copyBackToFront
// purpose  :
// =======================================================================
void OpenGl_Workspace::copyBackToFront()
{
#if !defined(GL_ES_VERSION_2_0)
  glMatrixMode (GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D (0.0, (GLdouble )myWidth, 0.0, (GLdouble )myHeight);

  glMatrixMode (GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  DisableFeatures();

  glDrawBuffer (GL_FRONT);
  glReadBuffer (GL_BACK);

  glRasterPos2i (0, 0);
  glCopyPixels  (0, 0, myWidth + 1, myHeight + 1, GL_COLOR);

  EnableFeatures();

  glMatrixMode (GL_PROJECTION);
  glPopMatrix();
  glMatrixMode (GL_MODELVIEW);
  glPopMatrix();

  glDrawBuffer (GL_BACK);

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
  aCallData.glContext = GetGlContext();
  aCallData.wsID      = theCView.WsId;
  aCallData.viewID    = theCView.ViewId;
  theCView.GDisplayCB (theCView.DefWindow.XWindow, theCView.GClientData, &aCallData);
}

// =======================================================================
// function : RedrawImmediate
// purpose  :
// =======================================================================
void OpenGl_Workspace::RedrawImmediate (const Graphic3d_CView& theCView,
                                        const Aspect_CLayer2d& theCUnderLayer,
                                        const Aspect_CLayer2d& theCOverLayer,
                                        const Standard_Boolean theToForce)
{
  if (!Activate())
  {
    return;
  }

  GLboolean isDoubleBuffer = GL_FALSE;
#if !defined(GL_ES_VERSION_2_0)
  glGetBooleanv (GL_DOUBLEBUFFER, &isDoubleBuffer);
#endif
  if (myView->ImmediateStructures().IsEmpty())
  {
    if (theToForce
     || !myIsImmediateDrawn)
    {
      myIsImmediateDrawn = Standard_False;
      return;
    }

    if (myBackBufferRestored
     && isDoubleBuffer)
    {
      copyBackToFront();
      glFlush();
    }
    else
    {
      Redraw (theCView, theCUnderLayer, theCOverLayer);
    }
    return;
  }

  if (isDoubleBuffer && myTransientDrawToFront)
  {
    if (!myBackBufferRestored)
    {
      Redraw (theCView, theCUnderLayer, theCOverLayer);
      return;
    }
    copyBackToFront();
    MakeFrontBufCurrent();
  }
  else
  {
    myBackBufferRestored = Standard_False;
  }
  myIsImmediateDrawn = Standard_True;

  NamedStatus |= OPENGL_NS_IMMEDIATE;
  ///glDisable (GL_LIGHTING);
  glDisable (GL_DEPTH_TEST);

  Handle(OpenGl_Workspace) aWS (this);
  for (OpenGl_SequenceOfStructure::Iterator anIter (myView->ImmediateStructures());
       anIter.More(); anIter.Next())
  {
    const OpenGl_Structure* aStructure = anIter.Value();
    aStructure->Render (aWS);
  }

  NamedStatus &= ~OPENGL_NS_IMMEDIATE;

  if (isDoubleBuffer && myTransientDrawToFront)
  {
    glFlush();
    MakeBackBufCurrent();
  }
}
