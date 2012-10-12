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
  return Standard_False;
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

  GLint anAlignBack = 1;
  glGetIntegerv (GL_PACK_ALIGNMENT, &anAlignBack);
  GLint anExtraBytes = (GLint )theImage.RowExtraBytes();
  GLint anAligment   = Min (GLint(theImage.MaxRowAligmentBytes()), 8); // limit to 8 bytes for OpenGL
  glPixelStorei (GL_PACK_ALIGNMENT, anAligment);

  if (anExtraBytes >= anAligment)
  {
    // copy row by row
    for (Standard_Size aRow = 0; aRow < theImage.SizeY(); ++aRow)
    {
      glReadPixels (0, GLint(aRow), GLsizei (theImage.SizeX()), 1, aFormat, aType, theImage.ChangeRow (aRow));
    }
  }
  else
  {
    // read pixels
    glReadPixels (0, 0, GLsizei (theImage.SizeX()), GLsizei (theImage.SizeY()), aFormat, aType, theImage.ChangeData());
    theImage.SetTopDown (false); // image bottom-up in OpenGL
  }

  glPixelStorei (GL_PACK_ALIGNMENT, anAlignBack);

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
  Handle(OpenGl_Context) aShareCtx = GetSharedContext();
  if (myMapOfView.IsBound (theCView.ViewId))
    myMapOfView.UnBind (theCView.ViewId);

  if (myMapOfWS.IsBound (theCView.WsId))
    myMapOfWS.UnBind (theCView.WsId);

  if (myMapOfWS.IsEmpty() && !myMapOfStructure.IsEmpty())
  {
    // The last view removed but some objects still present.
    // Release GL resources now without object destruction.
    for (NCollection_DataMap<Standard_Integer, OpenGl_Structure*>::Iterator aStructIt (myMapOfStructure);
         aStructIt.More (); aStructIt.Next())
    {
      OpenGl_Structure* aStruct = aStructIt.ChangeValue();
      aStruct->ReleaseGlResources (aShareCtx);
    }
  }

  OpenGl_CView *aCView = (OpenGl_CView *)theCView.ptrView;
  delete aCView;
  ((Graphic3d_CView *)&theCView)->ptrView = NULL;
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

Standard_Boolean OpenGl_GraphicDriver::View (Graphic3d_CView& theCView)
{
  if (openglDisplay.IsNull())
    return Standard_False;

  if (myMapOfView.IsBound (theCView.ViewId))
    myMapOfView.UnBind (theCView.ViewId);

  if (myMapOfWS.IsBound (theCView.WsId))
    myMapOfWS.UnBind (theCView.WsId);

  Handle(OpenGl_Workspace) aWS = Handle(OpenGl_Workspace)::DownCast(openglDisplay->GetWindow (theCView.DefWindow.XWindow));
  if (aWS.IsNull())
  {
    Handle(OpenGl_Context) aShareCtx = GetSharedContext();
    aWS = new OpenGl_Workspace (openglDisplay, theCView.DefWindow, theCView.GContext, aShareCtx);
    openglDisplay->SetWindow (theCView.DefWindow.XWindow, aWS);
  }

  myMapOfWS.Bind (theCView.WsId, aWS);

  Handle(OpenGl_View) aView = new OpenGl_View (theCView.Context);
  myMapOfView.Bind (theCView.ViewId, aView);

  OpenGl_CView* aCView = new OpenGl_CView();
  aCView->View = aView;
  aCView->WS = aWS;
  theCView.ptrView = aCView;

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
