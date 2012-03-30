// Created on: 2011-10-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2012 OPEN CASCADE SAS
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


#include <OpenGl_GraphicDriver.hxx>

#include <OpenGl_FrameBuffer.hxx>

#include <OpenGl_Structure.hxx>
#include <OpenGl_CView.hxx>
#include <OpenGl_Display.hxx>

/*----------------------------------------------------------------------*/

void OpenGl_GraphicDriver::ActivateView (const Graphic3d_CView& ACView)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
    aCView->WS->SetActiveView(aCView->View);
}

void OpenGl_GraphicDriver::AntiAliasing (const Graphic3d_CView& ACView, const Standard_Boolean AFlag)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
    aCView->View->SetAntiAliasing(AFlag);
}

void OpenGl_GraphicDriver::Background (const Graphic3d_CView& ACView)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
  {
    aCView->WS->SetBackgroundColor(ACView.DefWindow.Background.r,ACView.DefWindow.Background.g,ACView.DefWindow.Background.b);
  }
}

void OpenGl_GraphicDriver::GradientBackground (const Graphic3d_CView& ACView,
                                              const Quantity_Color& AColor1,
                                              const Quantity_Color& AColor2,
                                              const Aspect_GradientFillMethod AType)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
  {
    aCView->View->SetBackgroundGradient(AColor1,AColor2,AType);
  }
}

void OpenGl_GraphicDriver::Blink (const Graphic3d_CStructure &, const Standard_Boolean)
{
  // Do nothing
}

void OpenGl_GraphicDriver::BoundaryBox (const Graphic3d_CStructure& ACStructure, const Standard_Boolean Create)
{
  OpenGl_Structure *astructure = (OpenGl_Structure *)ACStructure.ptrStructure;
  if (!astructure)
    return;

  if ( Create )
    astructure->SetHighlightBox(ACStructure.BoundBox);
  else
    astructure->ClearHighlightBox();
}

void OpenGl_GraphicDriver::HighlightColor (const Graphic3d_CStructure& ACStructure, const Standard_ShortReal R, const Standard_ShortReal G, const Standard_ShortReal B, const Standard_Boolean Create)
{
  OpenGl_Structure *astructure = (OpenGl_Structure *)ACStructure.ptrStructure;
  if (!astructure)
    return;

  if ( Create )
    astructure->SetHighlightColor(R,G,B);
  else
    astructure->ClearHighlightColor();
}

void OpenGl_GraphicDriver::NameSetStructure (const Graphic3d_CStructure& ACStructure)
{
  OpenGl_Structure *astructure = (OpenGl_Structure *)ACStructure.ptrStructure;
  if (astructure)
  {
    Standard_Integer aStatus = 0;
    if (ACStructure.highlight) aStatus |= OPENGL_NS_HIGHLIGHT;
    if (!ACStructure.visible) aStatus |= OPENGL_NS_HIDE;
    astructure->SetNamedStatus( aStatus );
  }
}

void OpenGl_GraphicDriver::ClipLimit (const Graphic3d_CView& ACView, const Standard_Boolean AWait)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
  {
    aCView->View->SetClipLimit(ACView);
    if (!AWait)
    {
      aCView->WS->Resize(ACView.DefWindow);
    }
  }
}

void OpenGl_GraphicDriver::DeactivateView (const Graphic3d_CView& ACView)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
  {
    const Handle(OpenGl_View) aDummyView;
    aCView->WS->SetActiveView(aDummyView);
  }
}

void OpenGl_GraphicDriver::DepthCueing (const Graphic3d_CView& ACView, const Standard_Boolean AFlag)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
    aCView->View->SetFog(ACView, AFlag);
}

Standard_Boolean OpenGl_GraphicDriver::ProjectRaster (const Graphic3d_CView& ACView, const Standard_ShortReal AX, const Standard_ShortReal AY, const Standard_ShortReal AZ, Standard_Integer& AU, Standard_Integer& AV)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (!aCView)
    return Standard_False;

  Standard_Integer aWidth = aCView->WS->Width();
  Standard_Integer aHeight = aCView->WS->Height();
  Standard_ShortReal xr, yr;
  if (aCView->View->ProjectObjectToRaster(aWidth, aHeight, AX, AY, AZ, xr, yr))
  {
    AU = (Standard_Integer) xr;
    AV = aHeight - (Standard_Integer) yr;
    return Standard_True;
  }

  return Standard_False;
}

Standard_Boolean OpenGl_GraphicDriver::UnProjectRaster (const Graphic3d_CView& ACView, const Standard_Integer Axm, const Standard_Integer Aym, const Standard_Integer AXM, const Standard_Integer AYM, const Standard_Integer AU, const Standard_Integer AV, Standard_ShortReal& Ax, Standard_ShortReal& Ay, Standard_ShortReal& Az)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (!aCView)
    return Standard_False;

  const Standard_Integer aWidth = aCView->WS->Width();
  const Standard_Integer aHeight = aCView->WS->Height();

  /*
  Patched by P.Dolbey: the window pixel height decreased by one 
  in order for yr to remain within valid coordinate range [0; Ym -1]
  where Ym means window pixel height.
  */
  return aCView->View->ProjectRasterToObject( aWidth, aHeight, AU, (AYM-1)-Aym-AV, Ax, Ay, Az );
}

Standard_Boolean OpenGl_GraphicDriver::UnProjectRasterWithRay (const Graphic3d_CView& ACView, const Standard_Integer Axm, const Standard_Integer Aym, const Standard_Integer AXM, const Standard_Integer AYM, const Standard_Integer AU, const Standard_Integer AV, Standard_ShortReal& Ax, Standard_ShortReal& Ay, Standard_ShortReal& Az, Standard_ShortReal& Dx, Standard_ShortReal& Dy, Standard_ShortReal& Dz)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (!aCView)
    return Standard_False;

  const Standard_Integer aWidth = aCView->WS->Width();
  const Standard_Integer aHeight = aCView->WS->Height();

  return aCView->View->ProjectRasterToObjectWithRay( aWidth, aHeight, AU, AYM-Aym-AV, Ax, Ay, Az, Dx, Dy, Dz );
}

void OpenGl_GraphicDriver::RatioWindow (const Graphic3d_CView& theCView)
{
  const OpenGl_CView* aCView = (const OpenGl_CView* )theCView.ptrView;
  if (aCView != NULL)
    aCView->WS->Resize (theCView.DefWindow);
}

void OpenGl_GraphicDriver::Redraw (const Graphic3d_CView& ACView, const Aspect_CLayer2d& ACUnderLayer, const Aspect_CLayer2d& ACOverLayer, const Standard_Integer x, const Standard_Integer y, const Standard_Integer width, const Standard_Integer height)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
  {
    /*if( width <= 0 || height <= 0  )
      aCView->WS->Redraw(ACView, ACUnderLayer, ACOverLayer);
    else
      aCView->WS->RedrawArea(ACView, ACUnderLayer, ACOverLayer, x, y, width, height);*/
    // Always do full redraw
    aCView->WS->Redraw(ACView, ACUnderLayer, ACOverLayer);
  }
}

Graphic3d_PtrFrameBuffer OpenGl_GraphicDriver::FBOCreate (const Graphic3d_CView& ACView, const Standard_Integer theWidth, const Standard_Integer theHeight)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
    return aCView->WS->FBOCreate(theWidth, theHeight);
  return (Graphic3d_PtrFrameBuffer)NULL;
}

Graphic3d_PtrFrameBuffer OpenGl_Workspace::FBOCreate (const Standard_Integer theWidth, const Standard_Integer theHeight)
{
  // activate OpenGL context
  if (!Activate())
    return NULL;

  // create the FBO
  OpenGl_FrameBuffer* aFrameBuffer = new OpenGl_FrameBuffer();
  if (!aFrameBuffer->Init (GetGlContext(), theWidth, theHeight))
  {
    delete aFrameBuffer;
    return NULL;
  }
  return (Graphic3d_PtrFrameBuffer )aFrameBuffer;
}

void OpenGl_GraphicDriver::FBORelease (const Graphic3d_CView& ACView, Graphic3d_PtrFrameBuffer& theFBOPtr)
{
  if (theFBOPtr == NULL)
    return;
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
  {
    aCView->WS->FBORelease(theFBOPtr);
    theFBOPtr = NULL;
  }
}

void OpenGl_Workspace::FBORelease (Graphic3d_PtrFrameBuffer theFBOPtr)
{
  // activate OpenGL context
  if (!Activate())
    return;

  // release the object
  OpenGl_FrameBuffer* aFrameBuffer = (OpenGl_FrameBuffer*)theFBOPtr;
  aFrameBuffer->Release (GetGlContext());
  delete aFrameBuffer;
}

void OpenGl_GraphicDriver::FBOGetDimensions (const Graphic3d_CView& ,
                                             const Graphic3d_PtrFrameBuffer theFBOPtr,
                                             Standard_Integer& theWidth,    Standard_Integer& theHeight,
                                             Standard_Integer& theWidthMax, Standard_Integer& theHeightMax)
{
  if (theFBOPtr == NULL)
  {
    return;
  }
  const OpenGl_FrameBuffer* aFrameBuffer = (const OpenGl_FrameBuffer* )theFBOPtr;
  theWidth  = aFrameBuffer->GetVPSizeX(); // current viewport size
  theHeight = aFrameBuffer->GetVPSizeY();
  theWidthMax  = aFrameBuffer->GetSizeX(); // texture size
  theHeightMax = aFrameBuffer->GetSizeY();
}

void OpenGl_GraphicDriver::FBOChangeViewport (const Graphic3d_CView& ,
                                              Graphic3d_PtrFrameBuffer& theFBOPtr,
                                              const Standard_Integer theWidth, const Standard_Integer theHeight)
{
  if (theFBOPtr == NULL)
  {
    return;
  }
  OpenGl_FrameBuffer* aFrameBuffer = (OpenGl_FrameBuffer* )theFBOPtr;
  aFrameBuffer->ChangeViewport (theWidth, theHeight);
}

// OpenGL 1.2 stuff
#ifndef GL_BGR
  #define GL_BGR  0x80E0
#endif
#ifndef GL_BGRA
  #define GL_BGRA 0x80E1
#endif

static inline GLenum TFormatToGLEnum (const TRawBufferDataFormat theTFormat)
{
  switch (theTFormat)
  {
    case TRGB:   return GL_RGB;
    case TBGR:   return GL_BGR;
    case TRGBA:  return GL_RGBA;
    case TBGRA:  return GL_BGRA;
    case TDepthComponent: return GL_DEPTH_COMPONENT;
    case TRed:   return GL_RED;
    case TGreen: return GL_GREEN;
    case TBlue:  return GL_BLUE;
    case TAlpha: return GL_ALPHA;
    default:     return 0;
  }
}

static inline GLenum TTypeToGLEnum (const TRawBufferDataType theTType)
{
  switch (theTType)
  {
    case TUByte: return GL_UNSIGNED_BYTE;
    case TFloat: return GL_FLOAT;
    default:     return 0;
  }
}

Standard_Boolean OpenGl_GraphicDriver::BufferDump (const Graphic3d_CView& ACView, Image_CRawBufferData& theBuffer)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
    return aCView->WS->BufferDump((OpenGl_FrameBuffer *)ACView.ptrFBO,theBuffer);
  return Standard_False;
}

Standard_Boolean OpenGl_Workspace::BufferDump (OpenGl_FrameBuffer *theFBOPtr, Image_CRawBufferData& theBuffer)
{
  GLenum aFormat = TFormatToGLEnum (theBuffer.format);
  GLenum aType = TTypeToGLEnum (theBuffer.type);

  // safe checks
  if (aFormat == 0 || aType == 0 ||
      theBuffer.widthPx == 0 || theBuffer.heightPx == 0 ||
      theBuffer.dataPtr == NULL)
  {
    return Standard_False;
  }

  // activate OpenGL context
  if (!Activate())
    return Standard_False;

  // bind FBO if used
  OpenGl_FrameBuffer* aFrameBuffer = theFBOPtr;
  GLint aReadBufferPrev = GL_BACK;
  if (aFrameBuffer != NULL && aFrameBuffer->IsValid())
  {
    aFrameBuffer->BindBuffer (GetGlContext());
  }
  else
  {
    glGetIntegerv (GL_READ_BUFFER, &aReadBufferPrev);
    GLint aDrawBufferPrev = GL_BACK;
    glGetIntegerv (GL_DRAW_BUFFER, &aDrawBufferPrev);
    glReadBuffer (aDrawBufferPrev);
  }

  GLint anAlignBack = 1;
  glGetIntegerv (GL_PACK_ALIGNMENT, &anAlignBack);
  if (theBuffer.rowAligmentBytes == 0)
  {
    theBuffer.rowAligmentBytes = 1;
  }
  glPixelStorei (GL_PACK_ALIGNMENT, theBuffer.rowAligmentBytes);

  // read pixels
  glReadPixels (0, 0, theBuffer.widthPx, theBuffer.heightPx, aFormat, aType, (GLvoid* )theBuffer.dataPtr);
  glPixelStorei (GL_PACK_ALIGNMENT, anAlignBack);

  if (aFrameBuffer != NULL && aFrameBuffer->IsValid())
  {
    aFrameBuffer->UnbindBuffer (GetGlContext());
  }
  else
  {
    glReadBuffer (aReadBufferPrev);
  }
  return Standard_True;
}

void OpenGl_GraphicDriver::RemoveView (const Graphic3d_CView& ACView)
{
  if (GetMapOfViews().IsBound (ACView.ViewId))
    GetMapOfViews().UnBind (ACView.ViewId);

  if (GetMapOfWorkspaces().IsBound (ACView.WsId))
    GetMapOfWorkspaces().UnBind (ACView.WsId);

  OpenGl_CView *aCView = (OpenGl_CView *)ACView.ptrView;
  delete aCView;
  ((Graphic3d_CView *)&ACView)->ptrView = NULL;
}

void OpenGl_GraphicDriver::SetLight (const Graphic3d_CView& ACView)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
    aCView->View->SetLights(ACView.Context);
}

void OpenGl_GraphicDriver::SetPlane (const Graphic3d_CView& ACView)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
    aCView->View->SetClippingPlanes(ACView.Context);
}

void OpenGl_GraphicDriver::SetVisualisation (const Graphic3d_CView& ACView)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
  {
    aCView->View->SetVisualisation(ACView.Context);
    aCView->WS->UseZBuffer() = ( ACView.Context.Visualization == 0? (ACView.Context.ZBufferActivity == 1) : (ACView.Context.ZBufferActivity != 0) );
  }
}

void OpenGl_GraphicDriver::TransformStructure (const Graphic3d_CStructure& ACStructure)
{
  OpenGl_Structure *astructure = (OpenGl_Structure *)ACStructure.ptrStructure;
  if (astructure)
    astructure->SetTransformation(&(ACStructure.Transformation[0][0]));
}

void OpenGl_GraphicDriver::DegenerateStructure (const Graphic3d_CStructure& ACStructure)
{
  OpenGl_Structure *astructure = (OpenGl_Structure *)ACStructure.ptrStructure;
  if (astructure)
    astructure->SetDegenerateModel( ACStructure.ContextFillArea.DegenerationMode, ACStructure.ContextFillArea.SkipRatio );
}

void OpenGl_GraphicDriver::Transparency (const Graphic3d_CView& ACView, const Standard_Boolean AFlag)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
    aCView->WS->UseTransparency(AFlag);
}

void OpenGl_GraphicDriver::Update (const Graphic3d_CView& ACView, const Aspect_CLayer2d& ACUnderLayer, const Aspect_CLayer2d& ACOverLayer)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
    aCView->WS->Update(ACView,ACUnderLayer,ACOverLayer);
}

Standard_Boolean OpenGl_GraphicDriver::View (Graphic3d_CView& ACView)
{
  if (openglDisplay.IsNull())
    return Standard_False;

  if (GetMapOfViews().IsBound (ACView.ViewId))
    GetMapOfViews().UnBind (ACView.ViewId);

  if (GetMapOfWorkspaces().IsBound (ACView.WsId))
    GetMapOfWorkspaces().UnBind (ACView.WsId);

  Handle(OpenGl_Workspace) aWS = Handle(OpenGl_Workspace)::DownCast(openglDisplay->GetWindow( ACView.DefWindow.XWindow ));
  if ( aWS.IsNull() )
  {
    aWS = new OpenGl_Workspace( openglDisplay, ACView.DefWindow, ACView.GContext );
    openglDisplay->SetWindow( ACView.DefWindow.XWindow, aWS );
  }

  GetMapOfWorkspaces().Bind (ACView.WsId, aWS);

  Handle(OpenGl_View) aView = new OpenGl_View( ACView.Context );
  GetMapOfViews().Bind (ACView.ViewId, aView);

  OpenGl_CView *aCView = new OpenGl_CView;
  aCView->View = aView;
  aCView->WS = aWS;
  ACView.ptrView = aCView;

  return Standard_True;
}

void OpenGl_GraphicDriver::ViewMapping (const Graphic3d_CView& ACView, const Standard_Boolean AWait)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
  {
    aCView->View->SetMapping(ACView);
    if (!AWait)
    {
      aCView->WS->Resize(ACView.DefWindow);
    }
  }
}

void OpenGl_GraphicDriver::ViewOrientation (const Graphic3d_CView& ACView, const Standard_Boolean AWait)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
  {
    aCView->View->SetOrientation(ACView);
    if (!AWait)
    {
      aCView->WS->Resize(ACView.DefWindow);
    }
  }
}

void OpenGl_GraphicDriver::SetBackFacingModel (const Graphic3d_CView& ACView)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
    aCView->View->SetBackfacing(ACView.Backfacing);
}

//=======================================================================
//function : AddZLayer
//purpose  : 
//=======================================================================

void OpenGl_GraphicDriver::AddZLayer (const Graphic3d_CView& theCView,
                                      const Standard_Integer theLayerId)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)theCView.ptrView;
  if (aCView)
    aCView->View->AddZLayer (theLayerId);
}

//=======================================================================
//function : RemoveZLayer
//purpose  :
//=======================================================================

void OpenGl_GraphicDriver::RemoveZLayer (const Graphic3d_CView& theCView,
                                         const Standard_Integer theLayerId)
{
  const OpenGl_CView* aCView = (const OpenGl_CView *)theCView.ptrView;
  if (aCView)
    aCView->View->RemoveZLayer (theLayerId);
}
