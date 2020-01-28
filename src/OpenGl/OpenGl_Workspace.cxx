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
#include <OpenGl_Aspects.hxx>
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

namespace
{
  static const OpenGl_Vec4 THE_WHITE_COLOR (1.0f, 1.0f, 1.0f, 1.0f);
  static const OpenGl_Vec4 THE_BLACK_COLOR (0.0f, 0.0f, 0.0f, 1.0f);

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
void OpenGl_Material::Init (const OpenGl_Context& theCtx,
                            const Graphic3d_MaterialAspect& theMat,
                            const Quantity_Color& theInteriorColor)
{
  Common.ChangeShine()        = 128.0f * theMat.Shininess();
  Common.ChangeTransparency() = theMat.Alpha();

  Pbr.ChangeMetallic()  = theMat.PBRMaterial().Metallic();
  Pbr.ChangeRoughness() = theMat.PBRMaterial().NormalizedRoughness();
  Pbr.EmissionIOR = Graphic3d_Vec4 (theMat.PBRMaterial().Emission(), theMat.PBRMaterial().IOR());

  const OpenGl_Vec3& aSrcAmb = theMat.AmbientColor();
  const OpenGl_Vec3& aSrcDif = theMat.DiffuseColor();
  const OpenGl_Vec3& aSrcSpe = theMat.SpecularColor();
  const OpenGl_Vec3& aSrcEms = theMat.EmissiveColor();
  Common.Specular.SetValues (aSrcSpe, 1.0f); // interior color is ignored for Specular
  switch (theMat.MaterialType())
  {
    case Graphic3d_MATERIAL_ASPECT:
    {
      Common.Ambient .SetValues (aSrcAmb * theInteriorColor, 1.0f);
      Common.Diffuse .SetValues (aSrcDif * theInteriorColor, 1.0f);
      Common.Emission.SetValues (aSrcEms * theInteriorColor, 1.0f);
      Pbr  .BaseColor.SetValues (theInteriorColor, theMat.Alpha());
      break;
    }
    case Graphic3d_MATERIAL_PHYSIC:
    {
      Common.Ambient .SetValues (aSrcAmb, 1.0f);
      Common.Diffuse .SetValues (aSrcDif, 1.0f);
      Common.Emission.SetValues (aSrcEms, 1.0f);
      Pbr.BaseColor = theMat.PBRMaterial().Color();
      break;
    }
  }

  Common.Ambient  = theCtx.Vec4FromQuantityColor (Common.Ambient);
  Common.Diffuse  = theCtx.Vec4FromQuantityColor (Common.Diffuse);
  Common.Specular = theCtx.Vec4FromQuantityColor (Common.Specular);
  Common.Emission = theCtx.Vec4FromQuantityColor (Common.Emission);
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
  myNbSkippedTranspElems (0),
  myRenderFilter (OpenGl_RenderFilter_Empty),
  //
  myAspectsSet (&myDefaultAspects),
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
    if (myGlContext->Vendor() == "microsoft corporation"
    && !myGlContext->IsGlGreaterEqual (1, 2))
    {
      // this software implementation causes too slow rendering into GL_FRONT on modern Windows
      theView->SetImmediateModeDrawToFront (false);
    }
  #endif
  }

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
  myAspectsSet = &myDefaultAspects;
  myAspectsApplied.Nullify();
  myGlContext->SetPolygonOffset (Graphic3d_PolygonOffset());

  ApplyAspects();
  myGlContext->SetLineStipple(myDefaultAspects.Aspect()->LinePattern());
  myGlContext->SetLineWidth  (myDefaultAspects.Aspect()->LineWidth());
  if (myGlContext->core15fwd != NULL)
  {
    myGlContext->core15fwd->glActiveTexture (GL_TEXTURE0);
  }
}

// =======================================================================
// function : SetDefaultPolygonOffset
// purpose  :
// =======================================================================
Graphic3d_PolygonOffset OpenGl_Workspace::SetDefaultPolygonOffset (const Graphic3d_PolygonOffset& theOffset)
{
  Graphic3d_PolygonOffset aPrev = myDefaultAspects.Aspect()->PolygonOffset();
  myDefaultAspects.Aspect()->SetPolygonOffset (theOffset);
  if (myAspectsApplied == myDefaultAspects.Aspect()
   || myAspectsApplied.IsNull()
   || (myAspectsApplied->PolygonOffset().Mode & Aspect_POM_None) == Aspect_POM_None)
  {
    myGlContext->SetPolygonOffset (theOffset);
  }
  return aPrev;
}

// =======================================================================
// function : SetAspects
// purpose  :
// =======================================================================
const OpenGl_Aspects* OpenGl_Workspace::SetAspects (const OpenGl_Aspects* theAspect)
{
  const OpenGl_Aspects* aPrevAspects = myAspectsSet;
  myAspectsSet = theAspect;
  return aPrevAspects;
}

// =======================================================================
// function : ApplyAspects
// purpose  :
// =======================================================================
const OpenGl_Aspects* OpenGl_Workspace::ApplyAspects (bool theToBindTextures)
{
  if (myView->BackfacingModel() == Graphic3d_TOBM_AUTOMATIC)
  {
    bool toSuppressBackFaces = myToAllowFaceCulling
                            && myAspectsSet->Aspect()->ToSuppressBackFaces();
    if (toSuppressBackFaces)
    {
      if (myAspectsSet->Aspect()->InteriorStyle() == Aspect_IS_HATCH
       || myAspectsSet->Aspect()->AlphaMode() == Graphic3d_AlphaMode_Blend
       || myAspectsSet->Aspect()->AlphaMode() == Graphic3d_AlphaMode_Mask
       || (myAspectsSet->Aspect()->AlphaMode() == Graphic3d_AlphaMode_BlendAuto
        && myAspectsSet->Aspect()->FrontMaterial().Transparency() != 0.0f))
      {
        // disable culling in case of translucent shading aspect
        toSuppressBackFaces = false;
      }
    }
    myGlContext->SetCullBackFaces (toSuppressBackFaces);
  }

  if (myAspectsSet->Aspect() == myAspectsApplied
   && myHighlightStyle == myAspectFaceAppliedWithHL)
  {
    return myAspectsSet;
  }
  myAspectFaceAppliedWithHL = myHighlightStyle;

  // Aspect_POM_None means: do not change current settings
  if ((myAspectsSet->Aspect()->PolygonOffset().Mode & Aspect_POM_None) != Aspect_POM_None)
  {
    myGlContext->SetPolygonOffset (myAspectsSet->Aspect()->PolygonOffset());
  }

  const Aspect_InteriorStyle anIntstyle = myAspectsSet->Aspect()->InteriorStyle();
  if (myAspectsApplied.IsNull()
   || myAspectsApplied->InteriorStyle() != anIntstyle)
  {
  #if !defined(GL_ES_VERSION_2_0)
    myGlContext->SetPolygonMode (anIntstyle == Aspect_IS_POINT ? GL_POINT : GL_FILL);
    myGlContext->SetPolygonHatchEnabled (anIntstyle == Aspect_IS_HATCH);
  #endif
  }

#if !defined(GL_ES_VERSION_2_0)
  if (anIntstyle == Aspect_IS_HATCH)
  {
    myGlContext->SetPolygonHatchStyle (myAspectsSet->Aspect()->HatchStyle());
  }
#endif

  // Case of hidden line
  if (anIntstyle == Aspect_IS_HIDDENLINE)
  {
    // copy all values including line edge aspect
    *myAspectFaceHl.Aspect() = *myAspectsSet->Aspect();
    myAspectFaceHl.Aspect()->SetShadingModel (Graphic3d_TOSM_UNLIT);
    myAspectFaceHl.Aspect()->SetInteriorColor (myView->BackgroundColor().GetRGB());
    myAspectFaceHl.Aspect()->SetDistinguish (false);
    myAspectFaceHl.SetNoLighting();
    myAspectsSet = &myAspectFaceHl;
  }
  else
  {
    myGlContext->SetShadingMaterial (myAspectsSet, myHighlightStyle);
  }

  if (theToBindTextures)
  {
    const Handle(OpenGl_TextureSet)& aTextureSet = TextureSet();
    myGlContext->BindTextures (aTextureSet, Handle(OpenGl_ShaderProgram)());
  }

  if ((myView->myShadingModel == Graphic3d_TOSM_PBR
    || myView->myShadingModel == Graphic3d_TOSM_PBR_FACET)
   && !myView->myPBREnvironment.IsNull()
   &&  myView->myPBREnvironment->IsNeededToBeBound())
  {
    myView->myPBREnvironment->Bind (myGlContext);
  }

  myAspectsApplied = myAspectsSet->Aspect();
  return myAspectsSet;
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
  aCtx->BindTextures (Handle(OpenGl_TextureSet)(), Handle(OpenGl_ShaderProgram)());
  Handle(OpenGl_FrameBuffer) aFrameBuffer = new OpenGl_FrameBuffer();
  if (!aFrameBuffer->Init (aCtx, theWidth, theHeight, GL_SRGB8_ALPHA8, GL_DEPTH24_STENCIL8, 0))
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
bool OpenGl_Workspace::ShouldRender (const OpenGl_Element* theElement)
{
  // render only non-raytracable elements when RayTracing is enabled
  if ((myRenderFilter & OpenGl_RenderFilter_NonRaytraceableOnly) != 0)
  {
    if (OpenGl_Raytrace::IsRaytracedElement (theElement))
    {
      return false;
    }
  }
  else if ((myRenderFilter & OpenGl_RenderFilter_FillModeOnly) != 0)
  {
    if (!theElement->IsFillDrawMode())
    {
      return false;
    }
  }

  // handle opaque/transparency render passes
  if ((myRenderFilter & OpenGl_RenderFilter_OpaqueOnly) != 0)
  {
    if (!theElement->IsFillDrawMode())
    {
      return true;
    }

    if (OpenGl_Context::CheckIsTransparent (myAspectsSet, myHighlightStyle))
    {
      ++myNbSkippedTranspElems;
      return false;
    }
  }
  else if ((myRenderFilter & OpenGl_RenderFilter_TransparentOnly) != 0)
  {
    if (!theElement->IsFillDrawMode())
    {
      if (dynamic_cast<const OpenGl_Aspects*> (theElement) == NULL)
      {
        return false;
      }
    }
    else if (!OpenGl_Context::CheckIsTransparent (myAspectsSet, myHighlightStyle))
    {
      return false;
    }
  }
  return true;
}

// =======================================================================
// function : DumpJson
// purpose  :
// =======================================================================
void OpenGl_Workspace::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myUseZBuffer)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myUseDepthWrite)

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myNoneCulling)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myFrontCulling)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myNbSkippedTranspElems)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myRenderFilter)

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myDefaultAspects)

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myAspectsSet)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myAspectsApplied.get())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myToAllowFaceCulling)

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myAspectFaceHl)
}
