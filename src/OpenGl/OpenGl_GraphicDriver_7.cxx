// Created on: 2011-10-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <OpenGl_GraphicDriver.hxx>

#include <OpenGl_FrameBuffer.hxx>

#include <OpenGl_Structure.hxx>
#include <OpenGl_CView.hxx>
#include <OpenGl_Display.hxx>
#include <OpenGl_Text.hxx>

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

void OpenGl_GraphicDriver::BoundaryBox (const Graphic3d_CStructure& theCStructure,
                                        const Standard_Boolean      toCreate)
{
  OpenGl_Structure* aStructure = (OpenGl_Structure* )theCStructure.ptrStructure;
  if (aStructure == NULL)
    return;

  if (toCreate)
    aStructure->SetHighlightBox (GetSharedContext(), theCStructure.BoundBox);
  else
    aStructure->ClearHighlightBox (GetSharedContext());
}

void OpenGl_GraphicDriver::HighlightColor (const Graphic3d_CStructure& theCStructure,
                                           const Standard_ShortReal R,
                                           const Standard_ShortReal G,
                                           const Standard_ShortReal B,
                                           const Standard_Boolean   toCreate)
{
  OpenGl_Structure* aStructure = (OpenGl_Structure* )theCStructure.ptrStructure;
  if (aStructure == NULL)
    return;

  if (toCreate)
    aStructure->SetHighlightColor (GetSharedContext(), R, G, B);
  else
    aStructure->ClearHighlightColor (GetSharedContext());
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

Standard_Boolean OpenGl_GraphicDriver::UnProjectRaster (const Graphic3d_CView& ACView, const Standard_Integer /*Axm*/, const Standard_Integer Aym, const Standard_Integer /*AXM*/, const Standard_Integer AYM, const Standard_Integer AU, const Standard_Integer AV, Standard_ShortReal& Ax, Standard_ShortReal& Ay, Standard_ShortReal& Az)
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

Standard_Boolean OpenGl_GraphicDriver::UnProjectRasterWithRay (const Graphic3d_CView& ACView, const Standard_Integer /*Axm*/, const Standard_Integer Aym, const Standard_Integer /*AXM*/, const Standard_Integer AYM, const Standard_Integer AU, const Standard_Integer AV, Standard_ShortReal& Ax, Standard_ShortReal& Ay, Standard_ShortReal& Az, Standard_ShortReal& Dx, Standard_ShortReal& Dy, Standard_ShortReal& Dz)
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

void OpenGl_GraphicDriver::Redraw (const Graphic3d_CView& ACView, 
                                   const Aspect_CLayer2d& ACUnderLayer, 
                                   const Aspect_CLayer2d& ACOverLayer, 
                                   const Standard_Integer /*x*/, 
                                   const Standard_Integer /*y*/, 
                                   const Standard_Integer /*width*/, 
                                   const Standard_Integer /*height*/)
{
  if (!myCaps->vboDisable && ACView.IsRaytracing)
  {
    if (ACView.WasRedrawnGL)
    {
      myDeviceLostFlag = Standard_True;
    }

    myCaps->keepArrayData = Standard_True;
  }

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

inline bool getDataFormat (const Image_PixMap& theData,
                           GLenum&             thePixelFormat,
                           GLenum&             theDataType)
{
  thePixelFormat = GL_RGB;
  theDataType    = GL_UNSIGNED_BYTE;
  switch (theData.Format())
  {
    case Image_PixMap::ImgGray:
      thePixelFormat = GL_DEPTH_COMPONENT;
      theDataType    = GL_UNSIGNED_BYTE;
      return true;
    case Image_PixMap::ImgRGB:
      thePixelFormat = GL_RGB;
      theDataType    = GL_UNSIGNED_BYTE;
      return true;
    case Image_PixMap::ImgBGR:
      thePixelFormat = GL_BGR;
      theDataType    = GL_UNSIGNED_BYTE;
      return true;
    case Image_PixMap::ImgRGBA:
    case Image_PixMap::ImgRGB32:
      thePixelFormat = GL_RGBA;
      theDataType    = GL_UNSIGNED_BYTE;
      return true;
    case Image_PixMap::ImgBGRA:
    case Image_PixMap::ImgBGR32:
      thePixelFormat = GL_BGRA;
      theDataType    = GL_UNSIGNED_BYTE;
      return true;
    case Image_PixMap::ImgGrayF:
      thePixelFormat = GL_DEPTH_COMPONENT;
      theDataType    = GL_FLOAT;
      return true;
    case Image_PixMap::ImgRGBF:
      thePixelFormat = GL_RGB;
      theDataType    = GL_FLOAT;
      return true;
    case Image_PixMap::ImgBGRF:
      thePixelFormat = GL_BGR;
      theDataType    = GL_FLOAT;
      return true;
    case Image_PixMap::ImgRGBAF:
      thePixelFormat = GL_RGBA;
      theDataType    = GL_FLOAT;
      return true;
    case Image_PixMap::ImgBGRAF:
      thePixelFormat = GL_BGRA;
      theDataType    = GL_FLOAT;
      return true;
    default:
      return false;
  }
}

Standard_Boolean OpenGl_GraphicDriver::BufferDump (const Graphic3d_CView&      theCView,
                                                   Image_PixMap&               theImage,
                                                   const Graphic3d_BufferType& theBufferType)
{
  const OpenGl_CView* aCView = (const OpenGl_CView* )theCView.ptrView;
  return (aCView != NULL) && aCView->WS->BufferDump ((OpenGl_FrameBuffer* )theCView.ptrFBO, theImage, theBufferType);
}

Standard_Boolean OpenGl_Workspace::BufferDump (OpenGl_FrameBuffer*         theFBOPtr,
                                               Image_PixMap&               theImage,
                                               const Graphic3d_BufferType& theBufferType)
{
  GLenum aFormat, aType;
  if (theImage.IsEmpty()
   || !getDataFormat (theImage, aFormat, aType)
   || ((theBufferType == Graphic3d_BT_Depth) && (aFormat != GL_DEPTH_COMPONENT))
   || !Activate())
  {
    return Standard_False;
  }

  // bind FBO if used
  GLint aReadBufferPrev = GL_BACK;
  if (theFBOPtr != NULL && theFBOPtr->IsValid())
  {
    theFBOPtr->BindBuffer (GetGlContext());
  }
  else
  {
    glGetIntegerv (GL_READ_BUFFER, &aReadBufferPrev);
    GLint aDrawBufferPrev = GL_BACK;
    glGetIntegerv (GL_DRAW_BUFFER, &aDrawBufferPrev);
    glReadBuffer (aDrawBufferPrev);
  }

  // setup alignment
  const GLint anExtraBytes = (GLint )theImage.RowExtraBytes();
  const GLint anAligment   = Min (GLint(theImage.MaxRowAligmentBytes()), 8); // limit to 8 bytes for OpenGL
  glPixelStorei (GL_PACK_ALIGNMENT, anAligment);

  const GLint aPixelsWidth = GLint(theImage.SizeRowBytes() / theImage.SizePixelBytes());
  glPixelStorei (GL_PACK_ROW_LENGTH, (anExtraBytes >= anAligment) ? aPixelsWidth : 0);

  if (theImage.IsTopDown())
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
  glPixelStorei (GL_PACK_ROW_LENGTH, 0);

  if (theFBOPtr != NULL && theFBOPtr->IsValid())
  {
    theFBOPtr->UnbindBuffer (GetGlContext());
  }
  else
  {
    glReadBuffer (aReadBufferPrev);
  }
  return Standard_True;
}

void OpenGl_GraphicDriver::RemoveView (const Graphic3d_CView& theCView)
{
  Handle(OpenGl_Context)   aCtx = GetSharedContext();
  Handle(OpenGl_View)      aView;
  Handle(OpenGl_Workspace) aWindow;
  if (myMapOfWS.Find (theCView.WsId, aWindow))
  {
    myMapOfWS.UnBind (theCView.WsId);
  }
  if (!aWindow.IsNull())
  {
    if (aWindow->GetGlContext()->MakeCurrent())
    {
      aCtx = aWindow->GetGlContext();
    }
    else
    {
      // try to hijack another context if any
      const Handle(OpenGl_Context)& anOtherCtx = GetSharedContext();
      if (!anOtherCtx.IsNull()
       && anOtherCtx != aWindow->GetGlContext())
      {
        aCtx = anOtherCtx;
        aCtx->MakeCurrent();
      }
    }
  }
  if (myMapOfView.Find (theCView.ViewId, aView))
  {
    aView->ReleaseGlResources (aCtx);
    myMapOfView.UnBind (theCView.ViewId);
  }

  if (myMapOfWS.IsEmpty())
  {
    // The last view removed but some objects still present.
    // Release GL resources now without object destruction.
    for (NCollection_DataMap<Standard_Integer, OpenGl_Structure*>::Iterator aStructIt (myMapOfStructure);
         aStructIt.More (); aStructIt.Next())
    {
      OpenGl_Structure* aStruct = aStructIt.ChangeValue();
      aStruct->ReleaseGlResources (aCtx);
    }
    myTempText->Release (aCtx);
    myGlDisplay->ReleaseAttributes (aCtx.operator->());
    myDeviceLostFlag = !myMapOfStructure.IsEmpty();
  }

  OpenGl_CView* aCView = (OpenGl_CView* )theCView.ptrView;
  delete aCView;
  ((Graphic3d_CView *)&theCView)->ptrView = NULL;

  aCtx.Nullify();
  aView.Nullify();
  aWindow.Nullify();
}

void OpenGl_GraphicDriver::SetLight (const Graphic3d_CView& ACView)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
    aCView->View->SetLights(ACView.Context);
}

void OpenGl_GraphicDriver::SetClipPlanes (const Graphic3d_CView& theCView)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)theCView.ptrView;
  if (aCView)
  {
    aCView->View->SetClipPlanes (theCView.Context.ClipPlanes);
  }
}

void OpenGl_GraphicDriver::SetClipPlanes (const Graphic3d_CStructure& theCStructure)
{
  OpenGl_Structure* aStructure = (OpenGl_Structure *)theCStructure.ptrStructure;
  if (aStructure)
  {
    aStructure->SetClipPlanes (theCStructure.ClipPlanes);
  }
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

Standard_Boolean OpenGl_GraphicDriver::View (Graphic3d_CView& theCView)
{
  if (myGlDisplay.IsNull()
   || myMapOfView.IsBound (theCView.ViewId)
   || myMapOfWS  .IsBound (theCView.WsId))
  {
    return Standard_False;
  }

  Handle(OpenGl_Context)   aShareCtx = GetSharedContext();
  Handle(OpenGl_Workspace) aWS       = new OpenGl_Workspace (myGlDisplay, theCView.DefWindow, theCView.GContext, myCaps, aShareCtx);
  Handle(OpenGl_View)      aView     = new OpenGl_View (theCView.Context, &myStateCounter);
  myMapOfWS  .Bind (theCView.WsId,   aWS);
  myMapOfView.Bind (theCView.ViewId, aView);

  OpenGl_CView* aCView = new OpenGl_CView();
  aCView->View = aView;
  aCView->WS   = aWS;
  theCView.ptrView = aCView;

  return Standard_True;
}

void OpenGl_GraphicDriver::ViewMapping (const Graphic3d_CView& ACView, const Standard_Boolean AWait)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
  {
    aCView->View->SetMapping (myGlDisplay, ACView);
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
