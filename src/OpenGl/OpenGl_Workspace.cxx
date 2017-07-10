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

  // reset state for safety
  myGlContext->BindProgram (Handle(OpenGl_ShaderProgram)());
  if (myGlContext->core20fwd != NULL)
  {
    myGlContext->core20fwd->glUseProgram (OpenGl_ShaderProgram::NO_PROGRAM);
  }
  myGlContext->ShaderManager()->PushState (Handle(OpenGl_ShaderProgram)());
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
  myPolygonOffsetApplied= Graphic3d_PolygonOffset();

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
      if ((float )myAspectFaceSet->Aspect()->FrontMaterial().Transparency() != 0.0f)
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
    if (myPolygonOffsetApplied.Mode   != myAspectFaceSet->Aspect()->PolygonOffset().Mode
     || myPolygonOffsetApplied.Factor != myAspectFaceSet->Aspect()->PolygonOffset().Factor
     || myPolygonOffsetApplied.Units  != myAspectFaceSet->Aspect()->PolygonOffset().Units)
    {
      SetPolygonOffset (myAspectFaceSet->Aspect()->PolygonOffset());
    }
  }

  // Case of hidden line
  if (myAspectFaceSet->Aspect()->InteriorStyle() == Aspect_IS_HIDDENLINE)
  {
    // copy all values including line edge aspect
    *myAspectFaceHl.Aspect().operator->() = *myAspectFaceSet->Aspect();
    myAspectFaceHl.SetAspectEdge (myAspectFaceSet->AspectEdge());
    myAspectFaceHl.Aspect()->SetInteriorColor (myView->BackgroundColor().GetRGB());
    myAspectFaceHl.SetNoLighting (true);
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
    case Image_Format_Gray:
      aFormat = GL_DEPTH_COMPONENT;
      aType   = GL_UNSIGNED_BYTE;
      break;
    case Image_Format_GrayF:
      aFormat = GL_DEPTH_COMPONENT;
      aType   = GL_FLOAT;
      break;
    case Image_Format_RGB:
      aFormat = GL_RGB;
      aType   = GL_UNSIGNED_BYTE;
      break;
    case Image_Format_BGR:
      aFormat = GL_BGR;
      aType   = GL_UNSIGNED_BYTE;
      break;
    case Image_Format_BGRA:
    case Image_Format_BGR32:
      aFormat = GL_BGRA;
      aType   = GL_UNSIGNED_BYTE;
      break;
    case Image_Format_BGRF:
      aFormat = GL_BGR;
      aType   = GL_FLOAT;
      break;
    case Image_Format_BGRAF:
      aFormat = GL_BGRA;
      aType   = GL_FLOAT;
      break;
  #else
    case Image_Format_Gray:
    case Image_Format_GrayF:
    case Image_Format_BGRF:
    case Image_Format_BGRAF:
      return Standard_False;
    case Image_Format_BGRA:
    case Image_Format_BGR32:
      aFormat = GL_RGBA;
      aType   = GL_UNSIGNED_BYTE;
      toSwapRgbaBgra = true;
      break;
    case Image_Format_BGR:
    case Image_Format_RGB:
      aFormat = GL_RGBA;
      aType   = GL_UNSIGNED_BYTE;
      toConvRgba2Rgb = true;
      break;
  #endif
    case Image_Format_RGBA:
    case Image_Format_RGB32:
      aFormat = GL_RGBA;
      aType   = GL_UNSIGNED_BYTE;
      break;
    case Image_Format_RGBF:
      aFormat = GL_RGB;
      aType   = GL_FLOAT;
      break;
    case Image_Format_RGBAF:
      aFormat = GL_RGBA;
      aType   = GL_FLOAT;
      break;
    case Image_Format_Alpha:
    case Image_Format_AlphaF:
      return Standard_False; // GL_ALPHA is no more supported in core context
    case Image_Format_UNKNOWN:
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
      if (theImage.Format() == Image_Format_BGR)
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
