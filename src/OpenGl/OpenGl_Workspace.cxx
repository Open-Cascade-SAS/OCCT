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
#include <OpenGl_ShaderManager.hxx>
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
  const bool isPhysic = theMat.MaterialType (Graphic3d_MATERIAL_PHYSIC);
  ChangeShine()        = 128.0f * theMat.Shininess();
  ChangeTransparency() = theMat.Alpha();

  // ambient component
  if (theMat.ReflectionMode (Graphic3d_TOR_AMBIENT))
  {
    const OpenGl_Vec3& aSrcAmb = isPhysic ? theMat.AmbientColor() : theInteriorColor;
    Ambient = OpenGl_Vec4 (aSrcAmb * theMat.Ambient(), 1.0f);
  }
  else
  {
    Ambient = THE_BLACK_COLOR;
  }

  // diffusion component
  if (theMat.ReflectionMode (Graphic3d_TOR_DIFFUSE))
  {
    const OpenGl_Vec3& aSrcDif = isPhysic ? theMat.DiffuseColor() : theInteriorColor;
    Diffuse = OpenGl_Vec4 (aSrcDif * theMat.Diffuse(), 1.0f);
  }
  else
  {
    Diffuse = THE_BLACK_COLOR;
  }

  // specular component
  if (theMat.ReflectionMode (Graphic3d_TOR_SPECULAR))
  {
    const OpenGl_Vec3& aSrcSpe = isPhysic ? (const OpenGl_Vec3& )theMat.SpecularColor() : THE_WHITE_COLOR.rgb();
    Specular = OpenGl_Vec4 (aSrcSpe * theMat.Specular(), 1.0f);
  }
  else
  {
    Specular = THE_BLACK_COLOR;
  }

  // emission component
  if (theMat.ReflectionMode (Graphic3d_TOR_EMISSION))
  {
    const OpenGl_Vec3& aSrcEms = isPhysic ? theMat.EmissiveColor() : theInteriorColor;
    Emission = OpenGl_Vec4 (aSrcEms * theMat.Emissive(), 1.0f);
  }
  else
  {
    Emission = THE_BLACK_COLOR;
  }
}

// =======================================================================
// function : OpenGl_Workspace
// purpose  :
// =======================================================================
OpenGl_Workspace::OpenGl_Workspace (OpenGl_View* theView, const Handle(OpenGl_Window)& theWindow)
: myView (theView),
  myWindow (theWindow),
  myGlContext (!theWindow.IsNull() ? theWindow->GetGlContext() : NULL),
  myUseZBuffer    (Standard_True),
  myUseDepthWrite (Standard_True),
  //
  myAspectLineSet (&myDefaultAspectLine),
  myAspectFaceSet (&myDefaultAspectFace),
  myAspectMarkerSet (&myDefaultAspectMarker),
  myAspectTextSet (&myDefaultAspectText),
  //
  ViewMatrix_applied (&myDefaultMatrix),
  StructureMatrix_applied (&myDefaultMatrix),
  myToAllowFaceCulling (false),
  myModelViewMatrix (myDefaultMatrix)
{
  if (!myGlContext.IsNull() && myGlContext->MakeCurrent())
  {
    myGlContext->core11fwd->glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

    // General initialization of the context
  #if !defined(GL_ES_VERSION_2_0)
    if (myGlContext->core11 != NULL)
    {
      // enable two-side lighting by default
      glLightModeli ((GLenum )GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
      glHint (GL_POINT_SMOOTH_HINT, GL_FASTEST);
      if (myGlContext->caps->ffpEnable)
      {
        glHint (GL_FOG_HINT, GL_FASTEST);
      }
    }

    glHint (GL_LINE_SMOOTH_HINT,    GL_FASTEST);
    glHint (GL_POLYGON_SMOOTH_HINT, GL_FASTEST);
  #endif
  }

  myDefaultCappingAlgoFilter = new OpenGl_CappingAlgoFilter();

  myFontFaceAspect.Aspect()->SetAlphaMode (Graphic3d_AlphaMode_Mask, 0.285f);
  myFontFaceAspect.Aspect()->SetShadingModel (Graphic3d_TOSM_UNLIT);

  myNoneCulling .Aspect()->SetSuppressBackFaces (false);
  myNoneCulling .Aspect()->SetDrawEdges (false);
  myNoneCulling .Aspect()->SetAlphaMode (Graphic3d_AlphaMode_Opaque);

  myFrontCulling.Aspect()->SetSuppressBackFaces (true);
  myFrontCulling.Aspect()->SetDrawEdges (false);
  myFrontCulling.Aspect()->SetAlphaMode (Graphic3d_AlphaMode_Opaque);
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

  // reset state for safety
  myGlContext->BindProgram (Handle(OpenGl_ShaderProgram)());
  if (myGlContext->core20fwd != NULL)
  {
    myGlContext->core20fwd->glUseProgram (OpenGl_ShaderProgram::NO_PROGRAM);
  }
  if (myGlContext->caps->ffpEnable)
  {
    myGlContext->ShaderManager()->PushState (Handle(OpenGl_ShaderProgram)());
  }
  return Standard_True;
}

//=======================================================================
//function : ResetAppliedAspect
//purpose  : Sets default values of GL parameters in accordance with default aspects
//=======================================================================
void OpenGl_Workspace::ResetAppliedAspect()
{
  myGlContext->BindDefaultVao();

  myHighlightStyle.Nullify();
  myToAllowFaceCulling  = false;
  myAspectLineSet       = &myDefaultAspectLine;
  myAspectFaceSet       = &myDefaultAspectFace;
  myAspectFaceApplied.Nullify();
  myAspectMarkerSet     = &myDefaultAspectMarker;
  myAspectMarkerApplied.Nullify();
  myAspectTextSet       = &myDefaultAspectText;
  myGlContext->SetPolygonOffset (Graphic3d_PolygonOffset());

  ApplyAspectLine();
  ApplyAspectFace();
  ApplyAspectMarker();
  ApplyAspectText();

  myGlContext->SetTypeOfLine (myDefaultAspectLine.Aspect()->Type());
  myGlContext->SetLineWidth  (myDefaultAspectLine.Aspect()->Width());
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
      if (myAspectFaceSet->Aspect()->AlphaMode() == Graphic3d_AlphaMode_Blend
       || myAspectFaceSet->Aspect()->AlphaMode() == Graphic3d_AlphaMode_Mask
       || (myAspectFaceSet->Aspect()->AlphaMode() == Graphic3d_AlphaMode_BlendAuto
        && myAspectFaceSet->Aspect()->FrontMaterial().Transparency() != 0.0f))
      {
        // disable culling in case of translucent shading aspect
        toSuppressBackFaces = false;
      }
    }
    myGlContext->SetCullBackFaces (toSuppressBackFaces);
  }

  if (myAspectFaceSet->Aspect() == myAspectFaceApplied
   && myHighlightStyle == myAspectFaceAppliedWithHL)
  {
    return myAspectFaceSet;
  }
  myAspectFaceAppliedWithHL = myHighlightStyle;

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
    myGlContext->SetPolygonOffset (myAspectFaceSet->Aspect()->PolygonOffset());
  }

  // Case of hidden line
  if (myAspectFaceSet->Aspect()->InteriorStyle() == Aspect_IS_HIDDENLINE)
  {
    // copy all values including line edge aspect
    *myAspectFaceHl.Aspect() = *myAspectFaceSet->Aspect();
    myAspectFaceHl.SetAspectEdge (myAspectFaceSet->AspectEdge());
    myAspectFaceHl.Aspect()->SetShadingModel (Graphic3d_TOSM_UNLIT);
    myAspectFaceHl.Aspect()->SetInteriorColor (myView->BackgroundColor().GetRGB());
    myAspectFaceHl.SetNoLighting();
    myAspectFaceSet = &myAspectFaceHl;
  }
  else
  {
    myGlContext->SetShadingMaterial (myAspectFaceSet, myHighlightStyle);
  }

  if (myAspectFaceSet->Aspect()->ToMapTexture())
  {
    myGlContext->BindTextures (myAspectFaceSet->TextureSet (myGlContext));
  }
  else
  {
    myGlContext->BindTextures (myEnvironmentTexture);
  }

  myAspectFaceApplied = myAspectFaceSet->Aspect();
  return myAspectFaceSet;
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

  // create the FBO
  const Handle(OpenGl_Context)& aCtx = GetGlContext();
  aCtx->BindTextures (Handle(OpenGl_TextureSet)());
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
// function : BufferDump
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Workspace::BufferDump (const Handle(OpenGl_FrameBuffer)& theFbo,
                                               Image_PixMap&                     theImage,
                                               const Graphic3d_BufferType&       theBufferType)
{
  return !theImage.IsEmpty()
      && Activate()
      && OpenGl_FrameBuffer::BufferDump (GetGlContext(), theFbo, theImage, theBufferType);
}

// =======================================================================
// function : ShouldRender
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_RaytraceFilter::ShouldRender (const Handle(OpenGl_Workspace)& theWorkspace,
                                                      const OpenGl_Element*           theElement)
{
  Standard_Boolean aPrevFilterResult = Standard_True;
  if (!myPrevRenderFilter.IsNull())
  {
    aPrevFilterResult = myPrevRenderFilter->ShouldRender (theWorkspace, theElement);
  }
  return aPrevFilterResult &&
    !OpenGl_Raytrace::IsRaytracedElement (theElement);
}
