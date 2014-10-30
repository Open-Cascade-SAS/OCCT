// Created on: 2011-10-20
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

#include <OpenGl_GraphicDriver.hxx>

#include <OpenGl_FrameBuffer.hxx>

#include <OpenGl_Structure.hxx>
#include <OpenGl_CView.hxx>
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
  if (ACView.RenderParams.Method == Graphic3d_RM_RAYTRACING
  && !myCaps->vboDisable
  && !myCaps->keepArrayData)
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

void OpenGl_GraphicDriver::RedrawImmediate (const Graphic3d_CView& theCView,
                                            const Aspect_CLayer2d& theCUnderLayer,
                                            const Aspect_CLayer2d& theCOverLayer)
{
  const OpenGl_CView* aCView = (const OpenGl_CView* )theCView.ptrView;
  if (aCView != NULL)
  {
    aCView->WS->RedrawImmediate (theCView, theCUnderLayer, theCOverLayer);
  }
}

void OpenGl_GraphicDriver::Invalidate (const Graphic3d_CView& theCView)
{
  const OpenGl_CView* aCView = (const OpenGl_CView* )theCView.ptrView;
  if (aCView != NULL)
  {
    aCView->WS->Invalidate (theCView);
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

#if !defined(GL_ES_VERSION_2_0)
  const GLint anExtraBytes = (GLint )theImage.RowExtraBytes();
  const GLint aPixelsWidth = GLint(theImage.SizeRowBytes() / theImage.SizePixelBytes());
  glPixelStorei (GL_PACK_ROW_LENGTH, (anExtraBytes >= anAligment) ? aPixelsWidth : 0);
#endif

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
    myTempText->Release (aCtx.operator->());
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

//=======================================================================
//function : SetCamera
//purpose  :
//=======================================================================
void OpenGl_GraphicDriver::SetCamera (const Graphic3d_CView& theCView)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)theCView.ptrView;
  if (aCView)
  {
    aCView->View->SetCamera (theCView.Context.Camera);
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

void OpenGl_GraphicDriver::Transparency (const Graphic3d_CView& ACView, const Standard_Boolean AFlag)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
    aCView->WS->UseTransparency(AFlag);
}

// =======================================================================
// function : InvalidateBVHData
// purpose  :
// =======================================================================
void OpenGl_GraphicDriver::InvalidateBVHData (Graphic3d_CView& theCView, const Standard_Integer theLayerId)
{
  OpenGl_CView *aCView = (OpenGl_CView *)theCView.ptrView;
  if(aCView)
  {
    aCView->View->InvalidateBVHData (theLayerId);
  }
}

Standard_Boolean OpenGl_GraphicDriver::View (Graphic3d_CView& theCView)
{
  Handle(OpenGl_Context) aShareCtx = GetSharedContext();
  if (myMapOfView.IsBound (theCView.ViewId))
  {
    OpenGl_CView* aCView = (OpenGl_CView* )theCView.ptrView;
    if (!myMapOfWS.IsBound (theCView.WsId)
     || aCView == NULL)
    {
      return Standard_False;
    }

    Handle(OpenGl_Workspace) aWS = new OpenGl_Workspace (this, theCView.DefWindow, theCView.GContext, myCaps, aShareCtx);
    aCView->WS = aWS;
    aWS->SetActiveView (aCView->View);

    myMapOfWS.UnBind (theCView.WsId);
    myMapOfWS.Bind   (theCView.WsId, aWS);
    return Standard_True;
  }

  Handle(OpenGl_Workspace) aWS       = new OpenGl_Workspace (this, theCView.DefWindow, theCView.GContext, myCaps, aShareCtx);
  Handle(OpenGl_View)      aView     = new OpenGl_View (theCView.Context, &myStateCounter);
  myMapOfWS  .Bind (theCView.WsId,   aWS);
  myMapOfView.Bind (theCView.ViewId, aView);

  OpenGl_CView* aCView = new OpenGl_CView();
  aCView->View = aView;
  aCView->WS   = aWS;
  theCView.ptrView = aCView;
  aWS->SetActiveView (aCView->View);

  return Standard_True;
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

//=======================================================================
//function : SetZLayerSettings
//purpose  :
//=======================================================================
Standard_EXPORT void OpenGl_GraphicDriver::SetZLayerSettings (const Graphic3d_CView& theCView,
                                                              const Standard_Integer theLayerId,
                                                              const Graphic3d_ZLayerSettings& theSettings)
{
  const OpenGl_CView* aCView = (const OpenGl_CView* )theCView.ptrView;
  if (aCView)
    aCView->View->SetZLayerSettings (theLayerId, theSettings);
}
