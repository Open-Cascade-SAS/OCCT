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
#include <NCollection_AlignedAllocator.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_Workspace,Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(OpenGl_RaytraceFilter,OpenGl_RenderFilter)

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
  static const OpenGl_Vec4 THE_WHITE_COLOR (1.0f, 1.0f, 1.0f, 1.0f);
  static const OpenGl_Vec4 THE_BLACK_COLOR (0.0f, 0.0f, 0.0f, 1.0f);

  static const OpenGl_AspectLine myDefaultAspectLine;
  static const OpenGl_AspectFace myDefaultAspectFace;
  static const OpenGl_AspectMarker myDefaultAspectMarker;
  static const OpenGl_AspectText myDefaultAspectText;

  static const OpenGl_Matrix myDefaultMatrix =
  {
    {{ 1.0F, 0.0F, 0.0F, 0.0F },
     { 0.0F, 1.0F, 0.0F, 0.0F },
     { 0.0F, 0.0F, 1.0F, 0.0F },
     { 0.0F, 0.0F, 0.0F, 1.0F }}
  };

}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
void OpenGl_Material::Init (const Graphic3d_MaterialAspect& theMat,
                            const Quantity_Color&           theInteriorColor)
{
  const bool isPhysic = theMat.MaterialType (Graphic3d_MATERIAL_PHYSIC) == Standard_True;

  // ambient component
  if (theMat.ReflectionMode (Graphic3d_TOR_AMBIENT))
  {
    const OpenGl_Vec3& aSrcAmb = isPhysic ? theMat.AmbientColor() : theInteriorColor;
    Ambient = OpenGl_Vec4 (aSrcAmb * (float )theMat.Ambient(), 1.0f);
  }
  else
  {
    Ambient = THE_BLACK_COLOR;
  }

  // diffusion component
  if (theMat.ReflectionMode (Graphic3d_TOR_DIFFUSE))
  {
    const OpenGl_Vec3& aSrcDif = isPhysic ? theMat.DiffuseColor() : theInteriorColor;
    Diffuse = OpenGl_Vec4 (aSrcDif * (float )theMat.Diffuse(), 1.0f);
  }
  else
  {
    Diffuse = THE_BLACK_COLOR;
  }

  // specular component
  if (theMat.ReflectionMode (Graphic3d_TOR_SPECULAR))
  {
    const OpenGl_Vec3& aSrcSpe = isPhysic ? (const OpenGl_Vec3& )theMat.SpecularColor() : THE_WHITE_COLOR.rgb();
    Specular = OpenGl_Vec4 (aSrcSpe * (float )theMat.Specular(), 1.0f);
  }
  else
  {
    Specular = THE_BLACK_COLOR;
  }

  // emission component
  if (theMat.ReflectionMode (Graphic3d_TOR_EMISSION))
  {
    const OpenGl_Vec3& aSrcEms = isPhysic ? theMat.EmissiveColor() : theInteriorColor;
    Emission = OpenGl_Vec4 (aSrcEms * (float )theMat.Emissive(), 1.0f);
  }
  else
  {
    Emission = THE_BLACK_COLOR;
  }

  ChangeShine()        = 128.0f * float(theMat.Shininess());
  ChangeTransparency() = 1.0f - (float )theMat.Transparency();
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
  //
  myAspectLineSet (&myDefaultAspectLine),
  myAspectFaceSet (&myDefaultAspectFace),
  myAspectMarkerSet (&myDefaultAspectMarker),
  myAspectTextSet (&myDefaultAspectText),
  myAspectFaceAppliedWithHL (false),
  //
  ViewMatrix_applied (&myDefaultMatrix),
  StructureMatrix_applied (&myDefaultMatrix),
  myToAllowFaceCulling (false),
  myToHighlight (false),
  myModelViewMatrix (myDefaultMatrix)
{
  if (!myGlContext.IsNull() && myGlContext->MakeCurrent())
  {
    myGlContext->core11fwd->glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

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

  myDefaultCappingAlgoFilter = new OpenGl_CappingAlgoFilter();

  myNoneCulling .Aspect()->SetSuppressBackFaces (false);
  myNoneCulling .Aspect()->SetDrawEdges (false);
  myFrontCulling.Aspect()->SetSuppressBackFaces (true);
  myFrontCulling.Aspect()->SetDrawEdges (false);
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
  myToAllowFaceCulling  = false;
  myAspectLineSet       = &myDefaultAspectLine;
  myAspectFaceSet       = &myDefaultAspectFace;
  myAspectFaceApplied.Nullify();
  myAspectMarkerSet     = &myDefaultAspectMarker;
  myAspectMarkerApplied.Nullify();
  myAspectTextSet       = &myDefaultAspectText;
  myPolygonOffsetApplied= Graphic3d_PolygonOffset();

  ApplyAspectLine();
  ApplyAspectFace();
  ApplyAspectMarker();
  ApplyAspectText();

  myGlContext->SetTypeOfLine (myDefaultAspectLine.Aspect()->Type());
  myGlContext->SetLineWidth  (myDefaultAspectLine.Aspect()->Width());
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
  if (myGlContext->core11 != NULL)
  {
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
// function : updateMaterial
// purpose  :
// =======================================================================
void OpenGl_Workspace::updateMaterial (const int theFlag)
{
  // Case of hidden line
  if (myAspectFaceSet->Aspect()->InteriorStyle() == Aspect_IS_HIDDENLINE)
  {
    // copy all values including line edge aspect
    *myAspectFaceHl.Aspect().operator->() = *myAspectFaceSet->Aspect();
    myAspectFaceHl.SetAspectEdge (myAspectFaceSet->AspectEdge());
    myAspectFaceHl.Aspect()->SetInteriorColor (myView->BackgroundColor().GetRGB());
    myAspectFaceHl.SetNoLighting (true);
    myAspectFaceSet = &myAspectFaceHl;
    return;
  }

  const Graphic3d_MaterialAspect* aSrcMat      = &myAspectFaceSet->Aspect()->FrontMaterial();
  const Quantity_Color*           aSrcIntColor = &myAspectFaceSet->Aspect()->InteriorColor();
  GLenum aFace = GL_FRONT_AND_BACK;
  if (theFlag == TEL_BACK_MATERIAL)
  {
    aFace        = GL_BACK;
    aSrcMat      = &myAspectFaceSet->Aspect()->BackMaterial();
    aSrcIntColor = &myAspectFaceSet->Aspect()->BackInteriorColor();
  }
  else if (myAspectFaceSet->Aspect()->Distinguish()
        && !(NamedStatus & OPENGL_NS_RESMAT))
  {
    aFace = GL_FRONT;
  }

  myMatTmp.Init (*aSrcMat, *aSrcIntColor);
  if (myToHighlight)
  {
    myMatTmp.SetColor (*HighlightColor);
  }

  // handling transparency
  if (NamedStatus & OPENGL_NS_2NDPASSDO)
  {
    // second pass
    myMatTmp.Diffuse.a() = aSrcMat->EnvReflexion();
  }
  else
  {
    if (aSrcMat->EnvReflexion() != 0.0f)
    {
      // if the material reflects the environment scene, the second pass is needed
      NamedStatus |= OPENGL_NS_2NDPASSNEED;
    }

    const float aTransp = (float )aSrcMat->Transparency();
    if (aTransp != 0.0f)
    {
      // render transparent
      myMatTmp.Diffuse.a() = 1.0f - aTransp;
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
      glBlendFunc (GL_ONE, GL_ZERO);
      glDisable   (GL_BLEND);
      if (myUseDepthWrite)
      {
        glDepthMask (GL_TRUE);
      }
    }
  }

  // do not update material properties in case of zero reflection mode,
  // because GL lighting will be disabled by OpenGl_PrimitiveArray::DrawArray() anyway.
  if (myAspectFaceSet->IsNoLighting())
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
const OpenGl_AspectLine* OpenGl_Workspace::SetAspectLine (const OpenGl_AspectLine* theAspect)
{
  const OpenGl_AspectLine* aPrevAspectLine = myAspectLineSet;
  myAspectLineSet = theAspect;
  return aPrevAspectLine;
}

// =======================================================================
// function : SetAspectFace
// purpose  :
// =======================================================================
const OpenGl_AspectFace * OpenGl_Workspace::SetAspectFace (const OpenGl_AspectFace* theAspect)
{
  const OpenGl_AspectFace* aPrevAspectFace = myAspectFaceSet;
  myAspectFaceSet = theAspect;
  return aPrevAspectFace;
}

// =======================================================================
// function : SetAspectMarker
// purpose  :
// =======================================================================
const OpenGl_AspectMarker* OpenGl_Workspace::SetAspectMarker (const OpenGl_AspectMarker* theAspect)
{
  const OpenGl_AspectMarker* aPrevAspectMarker = myAspectMarkerSet;
  myAspectMarkerSet = theAspect;
  return aPrevAspectMarker;
}

// =======================================================================
// function : SetAspectText
// purpose  :
// =======================================================================
const OpenGl_AspectText * OpenGl_Workspace::SetAspectText (const OpenGl_AspectText* theAspect)
{
  const OpenGl_AspectText* aPrevAspectText = myAspectTextSet;
  myAspectTextSet = theAspect;
  return aPrevAspectText;
}

// =======================================================================
// function : ApplyAspectFace
// purpose  :
// =======================================================================
const OpenGl_AspectFace* OpenGl_Workspace::ApplyAspectFace()
{
  if (myView->BackfacingModel() == Graphic3d_TOBM_AUTOMATIC)
  {
    // manage back face culling mode, disable culling when clipping is enabled
    bool toSuppressBackFaces = myToAllowFaceCulling
                            && myAspectFaceSet->Aspect()->ToSuppressBackFaces();
    if (toSuppressBackFaces)
    {
      if (myGlContext->Clipping().IsClippingOrCappingOn()
       || myAspectFaceSet->Aspect()->InteriorStyle() == Aspect_IS_HATCH)
      {
        toSuppressBackFaces = false;
      }
    }
    if (toSuppressBackFaces)
    {
      if (!(NamedStatus & OPENGL_NS_2NDPASSDO)
       && (float )myAspectFaceSet->Aspect()->FrontMaterial().Transparency() != 0.0f)
      {
        // disable culling in case of translucent shading aspect
        toSuppressBackFaces = false;
      }
    }
    myGlContext->SetCullBackFaces (toSuppressBackFaces);
  }

  if (myAspectFaceSet->Aspect() == myAspectFaceApplied
   && myToHighlight == myAspectFaceAppliedWithHL)
  {
    return myAspectFaceSet;
  }
  myAspectFaceAppliedWithHL = myToHighlight;

#if !defined(GL_ES_VERSION_2_0)
  const Aspect_InteriorStyle anIntstyle = myAspectFaceSet->Aspect()->InteriorStyle();
  if (myAspectFaceApplied.IsNull()
   || myAspectFaceApplied->InteriorStyle() != anIntstyle)
  {
    switch (anIntstyle)
    {
      case Aspect_IS_EMPTY:
      case Aspect_IS_HOLLOW:
      {
        myGlContext->SetPolygonMode (GL_LINE);
        break;
      }
      case Aspect_IS_HATCH:
      {
        myGlContext->SetPolygonMode (GL_FILL);
        myGlContext->SetPolygonHatchEnabled (true);
        break;
      }
      case Aspect_IS_SOLID:
      case Aspect_IS_HIDDENLINE:
      {
        myGlContext->SetPolygonMode (GL_FILL);
        myGlContext->SetPolygonHatchEnabled (false);
        break;
      }
      case Aspect_IS_POINT:
      {
        myGlContext->SetPolygonMode (GL_POINT);
        break;
      }
    }
  }

  if (anIntstyle == Aspect_IS_HATCH)
  {
    myGlContext->SetPolygonHatchStyle (myAspectFaceSet->Aspect()->HatchStyle());
  }
#endif

  // Aspect_POM_None means: do not change current settings
  if ((myAspectFaceSet->Aspect()->PolygonOffset().Mode & Aspect_POM_None) != Aspect_POM_None)
  {
    if (myPolygonOffsetApplied.Mode   != myAspectFaceSet->Aspect()->PolygonOffset().Mode
     || myPolygonOffsetApplied.Factor != myAspectFaceSet->Aspect()->PolygonOffset().Factor
     || myPolygonOffsetApplied.Units  != myAspectFaceSet->Aspect()->PolygonOffset().Units)
    {
      SetPolygonOffset (myAspectFaceSet->Aspect()->PolygonOffset());
    }
  }

  updateMaterial (TEL_FRONT_MATERIAL);
  if (myAspectFaceSet->Aspect()->Distinguish())
  {
    updateMaterial (TEL_BACK_MATERIAL);
  }

  if (myAspectFaceSet->Aspect()->ToMapTexture())
  {
    EnableTexture (myAspectFaceSet->TextureRes (myGlContext),
                   myAspectFaceSet->TextureParams());
  }
  else
  {
    if (!myEnvironmentTexture.IsNull())
    {
      EnableTexture (myEnvironmentTexture,
                     myEnvironmentTexture->GetParams());
    }
    else
    {
      DisableTexture();
    }
  }

  myAspectFaceApplied = myAspectFaceSet->Aspect();
  return myAspectFaceSet;
}

//=======================================================================
//function : SetPolygonOffset
//purpose  :
//=======================================================================
void OpenGl_Workspace::SetPolygonOffset (const Graphic3d_PolygonOffset& theParams)
{
  myPolygonOffsetApplied = theParams;

  if ((theParams.Mode & Aspect_POM_Fill) == Aspect_POM_Fill)
  {
    glEnable (GL_POLYGON_OFFSET_FILL);
  }
  else
  {
    glDisable (GL_POLYGON_OFFSET_FILL);
  }

#if !defined(GL_ES_VERSION_2_0)
  if ((theParams.Mode & Aspect_POM_Line) == Aspect_POM_Line)
  {
    glEnable (GL_POLYGON_OFFSET_LINE);
  }
  else
  {
    glDisable (GL_POLYGON_OFFSET_LINE);
  }

  if ((theParams.Mode & Aspect_POM_Point) == Aspect_POM_Point)
  {
    glEnable (GL_POLYGON_OFFSET_POINT);
  }
  else
  {
    glDisable (GL_POLYGON_OFFSET_POINT);
  }
#endif
  glPolygonOffset (theParams.Factor, theParams.Units);
}

// =======================================================================
// function : ApplyAspectMarker
// purpose  :
// =======================================================================
const OpenGl_AspectMarker* OpenGl_Workspace::ApplyAspectMarker()
{
  if (myAspectMarkerSet->Aspect() != myAspectMarkerApplied)
  {
    if (myAspectMarkerApplied.IsNull()
    || (myAspectMarkerSet->Aspect()->Scale() != myAspectMarkerApplied->Scale()))
    {
    #if !defined(GL_ES_VERSION_2_0)
      glPointSize (myAspectMarkerSet->Aspect()->Scale());
    #ifdef HAVE_GL2PS
      gl2psPointSize (myAspectMarkerSet->Aspect()->Scale());
    #endif
    #endif
    }
    myAspectMarkerApplied = myAspectMarkerSet->Aspect();
  }
  return myAspectMarkerSet;
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
Handle(OpenGl_FrameBuffer) OpenGl_Workspace::FBOCreate (const Standard_Integer theWidth,
                                                        const Standard_Integer theHeight)
{
  // activate OpenGL context
  if (!Activate())
    return Handle(OpenGl_FrameBuffer)();

  DisableTexture();

  // create the FBO
  const Handle(OpenGl_Context)& aCtx = GetGlContext();
  Handle(OpenGl_FrameBuffer) aFrameBuffer = new OpenGl_FrameBuffer();
  if (!aFrameBuffer->Init (aCtx, theWidth, theHeight, GL_RGBA8, GL_DEPTH24_STENCIL8, 0))
  {
    aFrameBuffer->Release (aCtx.operator->());
    return Handle(OpenGl_FrameBuffer)();
  }
  return aFrameBuffer;
}

// =======================================================================
// function : FBORelease
// purpose  :
// =======================================================================
void OpenGl_Workspace::FBORelease (Handle(OpenGl_FrameBuffer)& theFbo)
{
  // activate OpenGL context
  if (!Activate()
   || theFbo.IsNull())
  {
    return;
  }

  theFbo->Release (GetGlContext().operator->());
  theFbo.Nullify();
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

template<typename T>
inline void convertRowFromRgba (T* theRgbRow,
                                const Image_ColorRGBA* theRgbaRow,
                                const Standard_Size theWidth)
{
  for (Standard_Size aCol = 0; aCol < theWidth; ++aCol)
  {
    const Image_ColorRGBA& anRgba = theRgbaRow[aCol];
    T& anRgb = theRgbRow[aCol];
    anRgb.r() = anRgba.r();
    anRgb.g() = anRgba.g();
    anRgb.b() = anRgba.b();
  }
}

// =======================================================================
// function : BufferDump
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Workspace::BufferDump (const Handle(OpenGl_FrameBuffer)& theFbo,
                                               Image_PixMap&                     theImage,
                                               const Graphic3d_BufferType&       theBufferType)
{
  if (theImage.IsEmpty()
  || !Activate())
  {
    return Standard_False;
  }

  GLenum aFormat = 0;
  GLenum aType   = 0;
  bool toSwapRgbaBgra = false;
  bool toConvRgba2Rgb = false;
  switch (theImage.Format())
  {
  #if !defined(GL_ES_VERSION_2_0)
    case Image_PixMap::ImgGray:
      aFormat = GL_DEPTH_COMPONENT;
      aType   = GL_UNSIGNED_BYTE;
      break;
    case Image_PixMap::ImgGrayF:
      aFormat = GL_DEPTH_COMPONENT;
      aType   = GL_FLOAT;
      break;
    case Image_PixMap::ImgRGB:
      aFormat = GL_RGB;
      aType   = GL_UNSIGNED_BYTE;
      break;
    case Image_PixMap::ImgBGR:
      aFormat = GL_BGR;
      aType   = GL_UNSIGNED_BYTE;
      break;
    case Image_PixMap::ImgBGRA:
    case Image_PixMap::ImgBGR32:
      aFormat = GL_BGRA;
      aType   = GL_UNSIGNED_BYTE;
      break;
    case Image_PixMap::ImgBGRF:
      aFormat = GL_BGR;
      aType   = GL_FLOAT;
      break;
    case Image_PixMap::ImgBGRAF:
      aFormat = GL_BGRA;
      aType   = GL_FLOAT;
      break;
  #else
    case Image_PixMap::ImgGray:
    case Image_PixMap::ImgGrayF:
    case Image_PixMap::ImgBGRF:
    case Image_PixMap::ImgBGRAF:
      return Standard_False;
    case Image_PixMap::ImgBGRA:
    case Image_PixMap::ImgBGR32:
      aFormat = GL_RGBA;
      aType   = GL_UNSIGNED_BYTE;
      toSwapRgbaBgra = true;
      break;
    case Image_PixMap::ImgBGR:
    case Image_PixMap::ImgRGB:
      aFormat = GL_RGBA;
      aType   = GL_UNSIGNED_BYTE;
      toConvRgba2Rgb = true;
      break;
  #endif
    case Image_PixMap::ImgRGBA:
    case Image_PixMap::ImgRGB32:
      aFormat = GL_RGBA;
      aType   = GL_UNSIGNED_BYTE;
      break;
    case Image_PixMap::ImgRGBF:
      aFormat = GL_RGB;
      aType   = GL_FLOAT;
      break;
    case Image_PixMap::ImgRGBAF:
      aFormat = GL_RGBA;
      aType   = GL_FLOAT;
      break;
    case Image_PixMap::ImgAlpha:
    case Image_PixMap::ImgAlphaF:
      return Standard_False; // GL_ALPHA is no more supported in core context
    case Image_PixMap::ImgUNKNOWN:
      return Standard_False;
  }

  if (aFormat == 0)
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
#else
  (void )theBufferType;
#endif

  // bind FBO if used
  if (!theFbo.IsNull() && theFbo->IsValid())
  {
    theFbo->BindBuffer (GetGlContext());
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
  if (toConvRgba2Rgb)
  {
    Handle(NCollection_BaseAllocator) anAlloc = new NCollection_AlignedAllocator (16);
    const Standard_Size aRowSize = theImage.SizeX() * 4;
    NCollection_Buffer aRowBuffer (anAlloc);
    if (!aRowBuffer.Allocate (aRowSize))
    {
      return Standard_False;
    }

    for (Standard_Size aRow = 0; aRow < theImage.SizeY(); ++aRow)
    {
      // Image_PixMap rows indexation always starts from the upper corner
      // while order in memory depends on the flag and processed by ChangeRow() method
      glReadPixels (0, GLint(theImage.SizeY() - aRow - 1), GLsizei (theImage.SizeX()), 1, aFormat, aType, aRowBuffer.ChangeData());
      const Image_ColorRGBA* aRowDataRgba = (const Image_ColorRGBA* )aRowBuffer.Data();
      if (theImage.Format() == Image_PixMap::ImgBGR)
      {
        convertRowFromRgba ((Image_ColorBGR* )theImage.ChangeRow (aRow), aRowDataRgba, theImage.SizeX());
      }
      else
      {
        convertRowFromRgba ((Image_ColorRGB* )theImage.ChangeRow (aRow), aRowDataRgba, theImage.SizeX());
      }
    }
  }
  else if (!isBatchCopy)
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
  const bool hasErrors = myGlContext->ResetErrors (true);

  glPixelStorei (GL_PACK_ALIGNMENT,  1);
#if !defined(GL_ES_VERSION_2_0)
  glPixelStorei (GL_PACK_ROW_LENGTH, 0);
#endif

  if (!theFbo.IsNull() && theFbo->IsValid())
  {
    theFbo->UnbindBuffer (GetGlContext());
  }
  else
  {
  #if !defined(GL_ES_VERSION_2_0)
    glReadBuffer (aReadBufferPrev);
  #endif
  }

  if (toSwapRgbaBgra)
  {
    Image_PixMap::SwapRgbaBgra (theImage);
  }

  return !hasErrors;
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
