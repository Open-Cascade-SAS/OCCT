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

#include <OpenGl_Workspace.hxx>

#include <InterfaceGraphic.hxx>

#include <OpenGl_ArbFBO.hxx>
#include <OpenGl_AspectLine.hxx>
#include <OpenGl_AspectFace.hxx>
#include <OpenGl_AspectMarker.hxx>
#include <OpenGl_AspectText.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_Element.hxx>
#include <OpenGl_FrameBuffer.hxx>
#include <OpenGl_GlCore15.hxx>
#include <OpenGl_SceneGeometry.hxx>
#include <OpenGl_Structure.hxx>
#include <OpenGl_Sampler.hxx>
#include <OpenGl_Texture.hxx>
#include <OpenGl_View.hxx>
#include <OpenGl_Window.hxx>

#include <Graphic3d_TextureParams.hxx>
#include <Graphic3d_TransformUtils.hxx>

#ifdef HAVE_GL2PS
  #include <gl2ps.h>
  /* OCC22216 NOTE: linker dependency can be switched off by undefining macro.
     Pragma comment for gl2ps.lib is defined only here. */
  #ifdef _MSC_VER
  #pragma comment( lib, "gl2ps.lib" )
  #endif
#endif

namespace
{
  static const TEL_COLOUR  THE_WHITE_COLOR = { { 1.0f, 1.0f, 1.0f, 1.0f } };
  static const OpenGl_Vec4 THE_BLACK_COLOR      (0.0f, 0.0f, 0.0f, 1.0f);

  static const OpenGl_AspectLine myDefaultAspectLine;
  static const OpenGl_AspectFace myDefaultAspectFace;
  static const OpenGl_AspectMarker myDefaultAspectMarker;

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
OpenGl_Workspace::OpenGl_Workspace (OpenGl_View* theView, const Handle(OpenGl_Window)& theWindow)
: NamedStatus (0),
  HighlightColor (&THE_WHITE_COLOR),
  myView (theView),
  myWindow (theWindow),
  myGlContext (!theWindow.IsNull() ? theWindow->GetGlContext() : NULL),
  myUseZBuffer    (Standard_True),
  myUseDepthWrite (Standard_True),
  myUseGLLight (Standard_True),
  //
  AspectLine_set (&myDefaultAspectLine),
  AspectLine_applied (NULL),
  AspectFace_set (&myDefaultAspectFace),
  AspectFace_applied (NULL),
  AspectMarker_set (&myDefaultAspectMarker),
  AspectMarker_applied (NULL),
  ViewMatrix_applied (&myDefaultMatrix),
  StructureMatrix_applied (&myDefaultMatrix),
  myCullingMode (TelCullUndefined),
  myModelViewMatrix (myDefaultMatrix),
  PolygonOffset_applied (THE_DEFAULT_POFFSET)
{
  if (!myGlContext.IsNull() && myGlContext->MakeCurrent())
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
  }

  myDefaultCappingAlgoFilter         = new OpenGl_CappingAlgoFilter();
  myNoneCulling.ChangeCullingMode()  = TelCullNone;
  myNoneCulling.ChangeEdge()         = 0;
  myFrontCulling.ChangeCullingMode() = TelCullBack;
  myFrontCulling.ChangeEdge()        = 0;
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
}

// =======================================================================
// function : Activate
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Workspace::Activate()
{
  if (myWindow.IsNull() || !myWindow->Activate())
  {
    return Standard_False;
  }

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
  PolygonOffset_applied = THE_DEFAULT_POFFSET;
  myCullingMode         = TelCullUndefined;

  AspectLine(Standard_True);
  AspectFace(Standard_True);
  AspectMarker(Standard_True);
  AspectText(Standard_True);

  myGlContext->SetTypeOfLine (myDefaultAspectLine.Type());
  myGlContext->SetLineWidth  (myDefaultAspectLine.Width());
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
    Graphic3d_TransformUtils::Scale     (aTextureMat,  aScale.x(),  aScale.y(), 1.0f);
    Graphic3d_TransformUtils::Translate (aTextureMat, -aTrans.x(), -aTrans.y(), 0.0f);
    Graphic3d_TransformUtils::Rotate    (aTextureMat, -aParams->Rotation(), 0.0f, 0.0f, 1.0f);
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
        if (myGlContext->core20fwd != NULL)
        {
          glEnable  (GL_POINT_SPRITE);
          glTexEnvi (GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
          anEnvMode = GL_REPLACE;
          myGlContext->core15->glPointParameteri (GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);
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
// function : TelUpdatePolygonOffsets
// purpose  :
// =======================================================================
static void TelUpdatePolygonOffsets (const TEL_POFFSET_PARAM& theOffsetData)
{
  if ((theOffsetData.mode & Aspect_POM_Fill) == Aspect_POM_Fill)
  {
    glEnable (GL_POLYGON_OFFSET_FILL);
  }
  else
  {
    glDisable (GL_POLYGON_OFFSET_FILL);
  }

#if !defined(GL_ES_VERSION_2_0)
  if ((theOffsetData.mode & Aspect_POM_Line) == Aspect_POM_Line)
  {
    glEnable (GL_POLYGON_OFFSET_LINE);
  }
  else
  {
    glDisable (GL_POLYGON_OFFSET_LINE);
  }

  if ((theOffsetData.mode & Aspect_POM_Point) == Aspect_POM_Point)
  {
    glEnable (GL_POLYGON_OFFSET_POINT);
  }
  else
  {
    glDisable (GL_POLYGON_OFFSET_POINT);
  }
#endif

  glPolygonOffset (theOffsetData.factor, theOffsetData.units);
}

// =======================================================================
// function : updateMaterial
// purpose  :
// =======================================================================
void OpenGl_Workspace::updateMaterial (const int theFlag)
{
  // Case of hidden line
  if (AspectFace_set->InteriorStyle() == Aspect_IS_HIDDENLINE)
  {
    myAspectFaceHl = *AspectFace_set; // copy all values including line edge aspect
    myAspectFaceHl.ChangeIntFront().matcol     = myView->BackgroundColor();
    myAspectFaceHl.ChangeIntFront().color_mask = 0;
    myAspectFaceHl.ChangeIntFront().color_mask = 0;

    AspectFace_set = &myAspectFaceHl;
    return;
  }

  const OPENGL_SURF_PROP* aProps = &AspectFace_set->IntFront();
  GLenum aFace = GL_FRONT_AND_BACK;
  if (theFlag == TEL_BACK_MATERIAL)
  {
    aFace  = GL_BACK;
    aProps = &AspectFace_set->IntBack();
  }
  else if (AspectFace_set->DistinguishingMode() == TOn
        && !(NamedStatus & OPENGL_NS_RESMAT))
  {
    aFace = GL_FRONT;
  }

  myMatTmp.Init (*aProps);

  // handling transparency
  if (NamedStatus & OPENGL_NS_2NDPASSDO)
  {
    // second pass
    myMatTmp.Diffuse.a() = aProps->env_reflexion;
  }
  else
  {
    if (aProps->env_reflexion != 0.0f)
    {
      // if the material reflects the environment scene, the second pass is needed
      NamedStatus |= OPENGL_NS_2NDPASSNEED;
    }

    if (aProps->trans != 1.0f)
    {
      // render transparent
      myMatTmp.Diffuse.a() = aProps->trans;
      glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glEnable    (GL_BLEND);
      if (myUseDepthWrite)
      {
        glDepthMask (GL_FALSE);
      }
    }
    else
    {
      // render opaque
      if ((NamedStatus & OPENGL_NS_ANTIALIASING) == 0)
      {
        glBlendFunc (GL_ONE, GL_ZERO);
        glDisable   (GL_BLEND);
      }
      if (myUseDepthWrite)
      {
        glDepthMask (GL_TRUE);
      }
    }
  }

  // do not update material properties in case of zero reflection mode,
  // because GL lighting will be disabled by OpenGl_PrimitiveArray::DrawArray() anyway.
  if (aProps->color_mask == 0)
  {
    return;
  }

  // reset material
  if (NamedStatus & OPENGL_NS_RESMAT)
  {
  #if !defined(GL_ES_VERSION_2_0)
    if (myGlContext->core11 != NULL)
    {
      myGlContext->core11->glMaterialfv (aFace, GL_AMBIENT,   myMatTmp.Ambient.GetData());
      myGlContext->core11->glMaterialfv (aFace, GL_DIFFUSE,   myMatTmp.Diffuse.GetData());
      myGlContext->core11->glMaterialfv (aFace, GL_SPECULAR,  myMatTmp.Specular.GetData());
      myGlContext->core11->glMaterialfv (aFace, GL_EMISSION,  myMatTmp.Emission.GetData());
      myGlContext->core11->glMaterialf  (aFace, GL_SHININESS, myMatTmp.Shine());
    }
  #endif

    if (theFlag == TEL_FRONT_MATERIAL)
    {
      myMatFront = myMatTmp;
      myMatBack  = myMatTmp;
    }
    else
    {
      myMatBack = myMatTmp;
    }

    NamedStatus &= ~OPENGL_NS_RESMAT;
    return;
  }

  // reduce updates
  OpenGl_Material& anOld = (theFlag == TEL_FRONT_MATERIAL)
                         ? myMatFront
                         : myMatBack;
#if !defined(GL_ES_VERSION_2_0)
  if (myGlContext->core11 != NULL)
  {
    if (myMatTmp.Ambient.r() != anOld.Ambient.r()
     || myMatTmp.Ambient.g() != anOld.Ambient.g()
     || myMatTmp.Ambient.b() != anOld.Ambient.b())
    {
      myGlContext->core11->glMaterialfv (aFace, GL_AMBIENT, myMatTmp.Ambient.GetData());
    }
    if (myMatTmp.Diffuse.r() != anOld.Diffuse.r()
     || myMatTmp.Diffuse.g() != anOld.Diffuse.g()
     || myMatTmp.Diffuse.b() != anOld.Diffuse.b()
     || fabs (myMatTmp.Diffuse.a() - anOld.Diffuse.a()) > 0.01f)
    {
      myGlContext->core11->glMaterialfv (aFace, GL_DIFFUSE, myMatTmp.Diffuse.GetData());
    }
    if (myMatTmp.Specular.r() != anOld.Specular.r()
     || myMatTmp.Specular.g() != anOld.Specular.g()
     || myMatTmp.Specular.b() != anOld.Specular.b())
    {
      myGlContext->core11->glMaterialfv (aFace, GL_SPECULAR, myMatTmp.Specular.GetData());
    }
    if (myMatTmp.Emission.r() != anOld.Emission.r()
     || myMatTmp.Emission.g() != anOld.Emission.g()
     || myMatTmp.Emission.b() != anOld.Emission.b())
    {
      myGlContext->core11->glMaterialfv (aFace, GL_EMISSION, myMatTmp.Emission.GetData());
    }
    if (myMatTmp.Shine() != anOld.Shine())
    {
      myGlContext->core11->glMaterialf (aFace, GL_SHININESS, myMatTmp.Shine());
    }
  }
#endif
  anOld = myMatTmp;
  if (aFace == GL_FRONT_AND_BACK)
  {
    myMatBack = myMatTmp;
  }
}

// =======================================================================
// function : SetAspectLine
// purpose  :
// =======================================================================
const OpenGl_AspectLine * OpenGl_Workspace::SetAspectLine(const OpenGl_AspectLine *AnAspect)
{
  const OpenGl_AspectLine *AspectLine_old = AspectLine_set;
  AspectLine_set = AnAspect;
  return AspectLine_old;
}

// =======================================================================
// function : SetAspectFace
// purpose  :
// =======================================================================
const OpenGl_AspectFace * OpenGl_Workspace::SetAspectFace(const OpenGl_AspectFace *AnAspect)
{
  const OpenGl_AspectFace *AspectFace_old = AspectFace_set;
  AspectFace_set = AnAspect;
  return AspectFace_old;
}

// =======================================================================
// function : SetAspectMarker
// purpose  :
// =======================================================================
const OpenGl_AspectMarker * OpenGl_Workspace::SetAspectMarker(const OpenGl_AspectMarker *AnAspect)
{
  const OpenGl_AspectMarker *AspectMarker_old = AspectMarker_set;
  AspectMarker_set = AnAspect;
  return AspectMarker_old;
}

// =======================================================================
// function : SetAspectText
// purpose  :
// =======================================================================
const OpenGl_AspectText * OpenGl_Workspace::SetAspectText(const OpenGl_AspectText *AnAspect)
{
  const OpenGl_AspectText *AspectText_old = AspectText_set;
  AspectText_set = AnAspect;
  return AspectText_old;
}

// =======================================================================
// function : AspectLine
// purpose  :
// =======================================================================
const OpenGl_AspectLine * OpenGl_Workspace::AspectLine(const Standard_Boolean theWithApply)
{
  if (theWithApply)
  {
    AspectLine_applied = AspectLine_set;
  }

  return AspectLine_set;
}

// =======================================================================
// function : AspectFace
// purpose  :
// =======================================================================
const OpenGl_AspectFace* OpenGl_Workspace::AspectFace (const Standard_Boolean theToApply)
{
  if (!theToApply)
  {
    return AspectFace_set;
  }

  if (myView->BackfacingModel() == Graphic3d_TOBM_AUTOMATIC)
  {
    // manage back face culling mode, disable culling when clipping is enabled
    TelCullMode aCullingMode = (myGlContext->Clipping().IsClippingOrCappingOn()
                             || AspectFace_set->InteriorStyle() == Aspect_IS_HATCH)
                             ? TelCullNone
                             : (TelCullMode )AspectFace_set->CullingMode();
    if (aCullingMode != TelCullNone
     && !(NamedStatus & OPENGL_NS_2NDPASSDO))
    {
      // disable culling in case of translucent shading aspect
      if (AspectFace_set->IntFront().trans != 1.0f)
      {
        aCullingMode = TelCullNone;
      }
    }
    if (myCullingMode != aCullingMode)
    {
      myCullingMode = aCullingMode;
      switch (myCullingMode)
      {
        case TelCullNone:
        case TelCullUndefined:
        {
          glDisable (GL_CULL_FACE);
          break;
        }
        case TelCullFront:
        {
          glCullFace (GL_FRONT);
          glEnable (GL_CULL_FACE);
          break;
        }
        case TelCullBack:
        {
          glCullFace (GL_BACK);
          glEnable (GL_CULL_FACE);
          break;
        }
      }
    }
  }

  if (AspectFace_set == AspectFace_applied)
  {
    return AspectFace_set;
  }

#if !defined(GL_ES_VERSION_2_0)
  const Aspect_InteriorStyle anIntstyle = AspectFace_set->InteriorStyle();
  if (AspectFace_applied == NULL || AspectFace_applied->InteriorStyle() != anIntstyle)
  {
    switch (anIntstyle)
    {
      case Aspect_IS_EMPTY:
      case Aspect_IS_HOLLOW:
      {
        glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
        break;
      }
      case Aspect_IS_HATCH:
      {
        glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
        myLineAttribs->SetTypeOfHatch (AspectFace_applied != NULL ? AspectFace_applied->Hatch() : TEL_HS_SOLID);
        break;
      }
      case Aspect_IS_SOLID:
      case Aspect_IS_HIDDENLINE:
      {
        glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
        if (myGlContext->core11 != NULL)
        {
          glDisable (GL_POLYGON_STIPPLE);
        }
        break;
      }
      case Aspect_IS_POINT:
      {
        glPolygonMode (GL_FRONT_AND_BACK, GL_POINT);
        break;
      }
    }
  }

  if (anIntstyle == Aspect_IS_HATCH)
  {
    const Tint hatchstyle = AspectFace_set->Hatch();
    if (AspectFace_applied == NULL || AspectFace_applied->Hatch() != hatchstyle)
    {
      myLineAttribs->SetTypeOfHatch (hatchstyle);
    }
  }
#endif

  // Aspect_POM_None means: do not change current settings
  if ((AspectFace_set->PolygonOffset().mode & Aspect_POM_None) != Aspect_POM_None)
  {
    if (PolygonOffset_applied.mode   != AspectFace_set->PolygonOffset().mode
     || PolygonOffset_applied.factor != AspectFace_set->PolygonOffset().factor
     || PolygonOffset_applied.units  != AspectFace_set->PolygonOffset().units)
    {
      SetPolygonOffset (AspectFace_set->PolygonOffset().mode,
                        AspectFace_set->PolygonOffset().factor,
                        AspectFace_set->PolygonOffset().units);
    }
  }

  updateMaterial (TEL_FRONT_MATERIAL);
  if (AspectFace_set->DistinguishingMode() == TOn)
  {
    updateMaterial (TEL_BACK_MATERIAL);
  }

  if ((NamedStatus & OPENGL_NS_FORBIDSETTEX) == 0)
  {
    if (AspectFace_set->DoTextureMap())
    {
      EnableTexture (AspectFace_set->TextureRes (myGlContext),
                     AspectFace_set->TextureParams());
    }
    else
    {
      DisableTexture();
    }
  }

  AspectFace_applied = AspectFace_set;
  return AspectFace_set;
}

//=======================================================================
//function : SetPolygonOffset
//purpose  :
//=======================================================================
void OpenGl_Workspace::SetPolygonOffset (int theMode,
                                         Standard_ShortReal theFactor,
                                         Standard_ShortReal theUnits)
{
  PolygonOffset_applied.mode   = theMode;
  PolygonOffset_applied.factor = theFactor;
  PolygonOffset_applied.units  = theUnits;

  TelUpdatePolygonOffsets (PolygonOffset_applied);
}

// =======================================================================
// function : AspectMarker
// purpose  :
// =======================================================================
const OpenGl_AspectMarker* OpenGl_Workspace::AspectMarker (const Standard_Boolean theToApply)
{
  if (theToApply && (AspectMarker_set != AspectMarker_applied))
  {
    if (!AspectMarker_applied || (AspectMarker_set->Scale() != AspectMarker_applied->Scale()))
    {
    #if !defined(GL_ES_VERSION_2_0)
      glPointSize (AspectMarker_set->Scale());
    #ifdef HAVE_GL2PS
      gl2psPointSize (AspectMarker_set->Scale());
    #endif
    #endif
    }
    AspectMarker_applied = AspectMarker_set;
  }
  return AspectMarker_set;
}

// =======================================================================
// function : AspectText
// purpose  :
// =======================================================================
const OpenGl_AspectText* OpenGl_Workspace::AspectText (const Standard_Boolean theWithApply)
{
  if (theWithApply)
  {
    AspectText_applied = AspectText_set;
  }

  return AspectText_set;
}

// =======================================================================
// function : Width
// purpose  :
// =======================================================================
Standard_Integer OpenGl_Workspace::Width()  const
{
  return !myView->GlWindow().IsNull() ? myView->GlWindow()->Width() : 0;
}

// =======================================================================
// function : Height
// purpose  :
// =======================================================================
Standard_Integer OpenGl_Workspace::Height() const
{
  return !myView->GlWindow().IsNull() ? myView->GlWindow()->Height() : 0;
}

// =======================================================================
// function : UseGLLight
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Workspace::UseGLLight() const
{
  return myView->IsGLLightEnabled();
}

// =======================================================================
// function : AntiAliasingMode
// purpose  :
// =======================================================================
Standard_Integer OpenGl_Workspace::AntiAliasingMode() const
{
  return myView->IsAntialiasingEnabled();
}

// =======================================================================
// function : IsCullingEnabled
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Workspace::IsCullingEnabled() const
{
  return myView->IsCullingEnabled();
}

// =======================================================================
// function : FBOCreate
// purpose  :
// =======================================================================
Graphic3d_PtrFrameBuffer OpenGl_Workspace::FBOCreate (const Standard_Integer theWidth,
                                                      const Standard_Integer theHeight)
{
  // activate OpenGL context
  if (!Activate())
    return NULL;

  // create the FBO
  const Handle(OpenGl_Context)& aCtx = GetGlContext();
  OpenGl_FrameBuffer* aFrameBuffer = new OpenGl_FrameBuffer();
  if (!aFrameBuffer->Init (aCtx, theWidth, theHeight))
  {
    aFrameBuffer->Release (aCtx.operator->());
    delete aFrameBuffer;
    return NULL;
  }
  return (Graphic3d_PtrFrameBuffer )aFrameBuffer;
}

// =======================================================================
// function : FBORelease
// purpose  :
// =======================================================================
void OpenGl_Workspace::FBORelease (Graphic3d_PtrFrameBuffer theFBOPtr)
{
  // activate OpenGL context
  if (!Activate()
   || theFBOPtr == NULL)
  {
    return;
  }

  // release the object
  OpenGl_FrameBuffer* aFrameBuffer = (OpenGl_FrameBuffer*)theFBOPtr;
  if (aFrameBuffer != NULL)
  {
    aFrameBuffer->Release (GetGlContext().operator->());
  }
  delete aFrameBuffer;
}

inline bool getDataFormat (const Image_PixMap& theData,
                           GLenum&             thePixelFormat,
                           GLenum&             theDataType)
{
  thePixelFormat = GL_RGB;
  theDataType    = GL_UNSIGNED_BYTE;
  switch (theData.Format())
  {
  #if !defined(GL_ES_VERSION_2_0)
    case Image_PixMap::ImgGray:
      thePixelFormat = GL_DEPTH_COMPONENT;
      theDataType    = GL_UNSIGNED_BYTE;
      return true;
    case Image_PixMap::ImgGrayF:
      thePixelFormat = GL_DEPTH_COMPONENT;
      theDataType    = GL_FLOAT;
      return true;
    case Image_PixMap::ImgBGR:
      thePixelFormat = GL_BGR;
      theDataType    = GL_UNSIGNED_BYTE;
      return true;
    case Image_PixMap::ImgBGRA:
    case Image_PixMap::ImgBGR32:
      thePixelFormat = GL_BGRA;
      theDataType    = GL_UNSIGNED_BYTE;
      return true;
    case Image_PixMap::ImgBGRF:
      thePixelFormat = GL_BGR;
      theDataType    = GL_FLOAT;
      return true;
    case Image_PixMap::ImgBGRAF:
      thePixelFormat = GL_BGRA;
      theDataType    = GL_FLOAT;
      return true;
  #else
    case Image_PixMap::ImgGray:
    case Image_PixMap::ImgGrayF:
    case Image_PixMap::ImgBGR:
    case Image_PixMap::ImgBGRA:
    case Image_PixMap::ImgBGR32:
    case Image_PixMap::ImgBGRF:
    case Image_PixMap::ImgBGRAF:
      return false;
  #endif
    case Image_PixMap::ImgRGB:
      thePixelFormat = GL_RGB;
      theDataType    = GL_UNSIGNED_BYTE;
      return true;
    case Image_PixMap::ImgRGBA:
    case Image_PixMap::ImgRGB32:
      thePixelFormat = GL_RGBA;
      theDataType    = GL_UNSIGNED_BYTE;
      return true;
    case Image_PixMap::ImgRGBF:
      thePixelFormat = GL_RGB;
      theDataType    = GL_FLOAT;
      return true;
    case Image_PixMap::ImgRGBAF:
      thePixelFormat = GL_RGBA;
      theDataType    = GL_FLOAT;
      return true;
    case Image_PixMap::ImgAlpha:
    case Image_PixMap::ImgAlphaF:
      return false; // GL_ALPHA is no more supported in core context
    case Image_PixMap::ImgUNKNOWN:
      return false;
  }
  return false;
}

// =======================================================================
// function : getAligned
// purpose  :
// =======================================================================
inline Standard_Size getAligned (const Standard_Size theNumber,
                                 const Standard_Size theAlignment)
{
  return theNumber + theAlignment - 1 - (theNumber - 1) % theAlignment;
}

// =======================================================================
// function : BufferDump
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Workspace::BufferDump (OpenGl_FrameBuffer*         theFBOPtr,
                                               Image_PixMap&               theImage,
                                               const Graphic3d_BufferType& theBufferType)
{
  GLenum aFormat, aType;
  if (theImage.IsEmpty()
   || !getDataFormat (theImage, aFormat, aType)
   || !Activate())
  {
    return Standard_False;
  }
#if !defined(GL_ES_VERSION_2_0)
  GLint aReadBufferPrev = GL_BACK;
  if (theBufferType == Graphic3d_BT_Depth
   && aFormat != GL_DEPTH_COMPONENT)
  {
    return Standard_False;
  }
#endif

  // bind FBO if used
  if (theFBOPtr != NULL && theFBOPtr->IsValid())
  {
    theFBOPtr->BindBuffer (GetGlContext());
  }
  else
  {
  #if !defined(GL_ES_VERSION_2_0)
    glGetIntegerv (GL_READ_BUFFER, &aReadBufferPrev);
    GLint aDrawBufferPrev = GL_BACK;
    glGetIntegerv (GL_DRAW_BUFFER, &aDrawBufferPrev);
    glReadBuffer (aDrawBufferPrev);
  #endif
  }

  // setup alignment
  const GLint anAligment   = Min (GLint(theImage.MaxRowAligmentBytes()), 8); // limit to 8 bytes for OpenGL
  glPixelStorei (GL_PACK_ALIGNMENT, anAligment);
  bool isBatchCopy = !theImage.IsTopDown();

  const GLint   anExtraBytes       = GLint(theImage.RowExtraBytes());
  GLint         aPixelsWidth       = GLint(theImage.SizeRowBytes() / theImage.SizePixelBytes());
  Standard_Size aSizeRowBytesEstim = getAligned (theImage.SizePixelBytes() * aPixelsWidth, anAligment);
  if (anExtraBytes < anAligment)
  {
    aPixelsWidth = 0;
  }
  else if (aSizeRowBytesEstim != theImage.SizeRowBytes())
  {
    aPixelsWidth = 0;
    isBatchCopy  = false;
  }
#if !defined(GL_ES_VERSION_2_0)
  glPixelStorei (GL_PACK_ROW_LENGTH, aPixelsWidth);
#else
  if (aPixelsWidth != 0)
  {
    isBatchCopy = false;
  }
#endif

  if (!isBatchCopy)
  {
    // copy row by row
    for (Standard_Size aRow = 0; aRow < theImage.SizeY(); ++aRow)
    {
      // Image_PixMap rows indexation always starts from the upper corner
      // while order in memory depends on the flag and processed by ChangeRow() method
      glReadPixels (0, GLint(theImage.SizeY() - aRow - 1), GLsizei (theImage.SizeX()), 1, aFormat, aType, theImage.ChangeRow (aRow));
    }
  }
  else
  {
    glReadPixels (0, 0, GLsizei (theImage.SizeX()), GLsizei (theImage.SizeY()), aFormat, aType, theImage.ChangeData());
  }

  glPixelStorei (GL_PACK_ALIGNMENT,  1);
#if !defined(GL_ES_VERSION_2_0)
  glPixelStorei (GL_PACK_ROW_LENGTH, 0);
#endif

  if (theFBOPtr != NULL && theFBOPtr->IsValid())
  {
    theFBOPtr->UnbindBuffer (GetGlContext());
  }
  else
  {
  #if !defined(GL_ES_VERSION_2_0)
    glReadBuffer (aReadBufferPrev);
  #endif
  }
  return Standard_True;
}

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
