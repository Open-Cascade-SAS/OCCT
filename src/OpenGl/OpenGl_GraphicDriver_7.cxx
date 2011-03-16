
// File   OpenGl_GraphicDriver_7.cxx
// Created  Mardi 28 janvier 1997
// Author CAL
// Modified     GG 10/11/99 PRO19603 Change the Redraw method (add redraw area capabillity)
//              EUG 07/10/99 G003 Add DegenerateStructure() and
//                                    SetBackFacingModel() methods.

//-Copyright  MatraDatavision 1997

//-Version

//-Design Declaration des variables specifiques aux Drivers

//-Warning  Un driver encapsule les Pex et OpenGl drivers

//-References

//-Language C++ 2.0

//-Declarations

// for the class
#include <OpenGl_GraphicDriver.jxx>

#include <Aspect_DriverDefinitionError.hxx>
#include <InterfaceGraphic_RawBufferData.hxx>

#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_tsm_ws.hxx>
#include <OpenGl_tgl_tox.hxx>
#include <OpenGl_txgl.hxx>
#include <OpenGl_FrameBuffer.hxx>

//-Aliases

//-Global data definitions

//-Methods, in order

void OpenGl_GraphicDriver::ActivateView (const Graphic3d_CView& ACView) {

  Graphic3d_CView MyCView = ACView;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_activateview");
    PrintCView (MyCView, 1);
  }
  call_togl_activateview (&MyCView);

}

void OpenGl_GraphicDriver::AntiAliasing (const Graphic3d_CView& ACView, const Standard_Boolean AFlag) {

  Graphic3d_CView MyCView = ACView;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_antialiasing");
    PrintCView (MyCView, 1);
    PrintBoolean ("AFlag", AFlag);
  }
  call_togl_antialiasing (&MyCView, (AFlag ? 1 : 0));

}

void OpenGl_GraphicDriver::Background (const Graphic3d_CView& ACView) {

  Graphic3d_CView MyCView = ACView;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_background");
    PrintCView (MyCView, 1);
  }
  call_togl_background (&MyCView);

}

void OpenGl_GraphicDriver::GradientBackground(const Graphic3d_CView& ACView,
                                              const Quantity_Color& AColor1,
                                              const Quantity_Color& AColor2,
                                              const Aspect_GradientFillMethod AType){

  Graphic3d_CView MyCView = ACView;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_gradient_background");
    PrintCView (MyCView, 1);
  }
  Standard_Real R1,G1,B1,R2,G2,B2;
  AColor1.Values( R1, G1, B1, Quantity_TOC_RGB );
  AColor2.Values( R2, G2, B2, Quantity_TOC_RGB );
  TEL_COLOUR tcolor1, tcolor2;
  tcolor1.rgb[0] = R1;
  tcolor1.rgb[1] = G1;
  tcolor1.rgb[2] = B1;
  tcolor1.rgb[3] = 0;
  tcolor2.rgb[0] = R2;
  tcolor2.rgb[1] = G2;
  tcolor2.rgb[2] = B2;
  tcolor2.rgb[3] = 0;

  call_togl_gradient_background (MyCView.WsId, AType, &tcolor1, &tcolor2);

}


void OpenGl_GraphicDriver::Blink (const Graphic3d_CStructure& ACStructure, const Standard_Boolean Create) {

  Graphic3d_CStructure MyCStructure = ACStructure;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_blink");
    PrintCStructure (MyCStructure, 1);
    PrintBoolean ("Create", Create);
  }
  call_togl_blink (&MyCStructure, (Create ? 1 : 0));

}

void OpenGl_GraphicDriver::BoundaryBox (const Graphic3d_CStructure& ACStructure, const Standard_Boolean Create) {

  Graphic3d_CStructure MyCStructure = ACStructure;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_boundarybox");
    PrintCStructure (MyCStructure, 1);
    PrintBoolean ("Create", Create);
  }
  call_togl_boundarybox (&MyCStructure, (Create ? 1 : 0));

}

void OpenGl_GraphicDriver::HighlightColor (const Graphic3d_CStructure& ACStructure, const Standard_ShortReal R, const Standard_ShortReal G, const Standard_ShortReal B, const Standard_Boolean Create) {

  Graphic3d_CStructure MyCStructure = ACStructure;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_highlightcolor");
    PrintCStructure (MyCStructure, 1);
    PrintShortReal ("R", R);
    PrintShortReal ("G", G);
    PrintShortReal ("B", B);
  }
  CALL_DEF_COLOR acolor;
  acolor.r  = R;
  acolor.g  = G;
  acolor.b  = B;
  call_togl_highlightcolor (&MyCStructure, &acolor, (Create ? 1 : 0));

}

void OpenGl_GraphicDriver::NameSetStructure (const Graphic3d_CStructure& ACStructure) {

  Graphic3d_CStructure MyCStructure = ACStructure;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_namesetstructure");
    PrintCStructure (MyCStructure, 1);
  }
  call_togl_namesetstructure (&MyCStructure);

}

void OpenGl_GraphicDriver::ClipLimit (const Graphic3d_CView& ACView, const Standard_Boolean AWait) {

  Graphic3d_CView MyCView = ACView;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_cliplimit");
    PrintCView (MyCView, 1);
    PrintBoolean ("AWait", AWait);
  }
  call_togl_cliplimit (&MyCView, (AWait ? 1 : 0));

}

void OpenGl_GraphicDriver::DeactivateView (const Graphic3d_CView& ACView) {
  Graphic3d_CView MyCView = ACView;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_deactivateview");
    PrintCView (MyCView, 1);
  }
  call_togl_deactivateview (&MyCView);

}

void OpenGl_GraphicDriver::DepthCueing (const Graphic3d_CView& ACView, const Standard_Boolean AFlag) {

  Graphic3d_CView MyCView = ACView;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_depthcueing");
    PrintCView (MyCView, 1);
    PrintBoolean ("AFlag", AFlag);
  }
  call_togl_depthcueing (&MyCView, (AFlag ? 1 : 0));

}

Standard_Boolean OpenGl_GraphicDriver::ProjectRaster (const Graphic3d_CView& ACView, const Standard_ShortReal AX, const Standard_ShortReal AY, const Standard_ShortReal AZ, Standard_Integer& AU, Standard_Integer& AV) {

  Graphic3d_CView MyCView = ACView;

  Standard_Integer Result;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_project_raster");
    PrintCView (MyCView, 1);
  }
  //if we want project something before to dump it into pixmap
  if ( ACView.DefBitmap.bitmap ) {
    Result = call_togl_adopt_to_rect( ACView.ViewId,
      ACView.DefBitmap.width,
      ACView.DefBitmap.height );
    if (MyTraceLevel) {
      PrintIResult ("call_togl_adopt_to_rect", Result);
    }
    if (Result)
      return Standard_False;
  }

  Result = call_togl_project_raster (ACView.ViewId, AX, AY, AZ, &AU, &AV);
  if (MyTraceLevel) {
    PrintIResult ("call_togl_project_raster", Result);
  }

  if ( ACView.DefBitmap.bitmap ) {
    Result = call_togl_adopt_to_rect( ACView.ViewId,
      ACView.DefWindow.dx,
      ACView.DefWindow.dy );
    if (MyTraceLevel) {
      PrintIResult ("call_togl_adopt_to_rect", Result);
    }
    if (Result)
      return Standard_False;
  }

  return (Result == 0 ? Standard_True : Standard_False);

}

Standard_Boolean OpenGl_GraphicDriver::UnProjectRaster (const Graphic3d_CView& ACView, const Standard_Integer Axm, const Standard_Integer Aym, const Standard_Integer AXM, const Standard_Integer AYM, const Standard_Integer AU, const Standard_Integer AV, Standard_ShortReal& Ax, Standard_ShortReal& Ay, Standard_ShortReal& Az) {

  Graphic3d_CView MyCView = ACView;

  Standard_Integer Result;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_unproject_raster");
    PrintCView (MyCView, 1);
  }
  Result = call_togl_unproject_raster
    (ACView.ViewId, Axm, Aym, AXM, AYM, AU, AV, &Ax, &Ay, &Az);
  if (MyTraceLevel) {
    PrintIResult ("call_togl_unproject_raster", Result);
  }
  return (Result == 0 ? Standard_True : Standard_False);

}

Standard_Boolean OpenGl_GraphicDriver::UnProjectRasterWithRay (const Graphic3d_CView& ACView, const Standard_Integer Axm, const Standard_Integer Aym, const Standard_Integer AXM, const Standard_Integer AYM, const Standard_Integer AU, const Standard_Integer AV, Standard_ShortReal& Ax, Standard_ShortReal& Ay, Standard_ShortReal& Az, Standard_ShortReal& Dx, Standard_ShortReal& Dy, Standard_ShortReal& Dz) {

  Graphic3d_CView MyCView = ACView;

  Standard_Integer Result;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_unproject_raster_with_ray");
    PrintCView (MyCView, 1);
  }
  Result = call_togl_unproject_raster_with_ray
    (ACView.ViewId, Axm, Aym, AXM, AYM, AU, AV, &Ax, &Ay, &Az, &Dx, &Dy, &Dz);
  if (MyTraceLevel) {
    PrintIResult ("call_togl_unproject_raster_with_ray", Result);
  }
  return (Result == 0 ? Standard_True : Standard_False);

}

void OpenGl_GraphicDriver::RatioWindow (const Graphic3d_CView& ACView) {

  Graphic3d_CView MyCView = ACView;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_ratio_window");
    PrintCView (MyCView, 1);
  }
  call_togl_ratio_window (&MyCView);

}

void OpenGl_GraphicDriver::Redraw (const Graphic3d_CView& ACView, const Aspect_CLayer2d& ACUnderLayer, const Aspect_CLayer2d& ACOverLayer, const Standard_Integer x, const Standard_Integer y, const Standard_Integer width, const Standard_Integer height) {

  Graphic3d_CView MyCView = ACView;
  Aspect_CLayer2d MyCUnderLayer = ACUnderLayer;
  Aspect_CLayer2d MyCOverLayer = ACOverLayer;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_redraw");
    PrintCView (MyCView, 1);
  }
  //PRO19603
  if( width <= 0 || height <= 0  )
    call_togl_redraw (&MyCView, &MyCUnderLayer, &MyCOverLayer);
  else {
    call_togl_redraw_area (&MyCView, &MyCUnderLayer, &MyCOverLayer,
      x, y, width, height);
  }
  //PRO19603
}

Graphic3d_PtrFrameBuffer OpenGl_GraphicDriver::FBOCreate (const Graphic3d_CView& theCView,
                                                          const Standard_Integer theWidth,
                                                          const Standard_Integer theHeight) {
  // activate OpenGL context
  CMN_KEY_DATA aData;
  if ((TsmGetWSAttri (theCView.WsId, WSWindow, &aData) != TSuccess) ||
      (TxglWinset (call_thedisplay, (WINDOW) aData.ldata) != TSuccess))
  {
    return NULL;
  }
  // create the FBO
  OpenGl_FrameBuffer* aFrameBuffer = new OpenGl_FrameBuffer();
  if (!aFrameBuffer->Init (theWidth, theHeight))
  {
    delete aFrameBuffer;
    return NULL;
  }
  return (Graphic3d_PtrFrameBuffer )aFrameBuffer;
}

void OpenGl_GraphicDriver::FBORelease (const Graphic3d_CView& theCView,
                                       Graphic3d_PtrFrameBuffer& theFBOPtr)
{
  if (theFBOPtr == NULL)
  {
    return;
  }
  // activate OpenGL context
  CMN_KEY_DATA aData;
  if ((TsmGetWSAttri (theCView.WsId, WSWindow, &aData) != TSuccess) ||
      (TxglWinset (call_thedisplay, (WINDOW) aData.ldata) != TSuccess))
  {
    return;
  }
  // release the object
  OpenGl_FrameBuffer* aFrameBuffer = (OpenGl_FrameBuffer* )theFBOPtr;
  delete aFrameBuffer;
  theFBOPtr = NULL;
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

Standard_Boolean OpenGl_GraphicDriver::BufferDump (const Graphic3d_CView& theCView,
                                                   Image_CRawBufferData& theBuffer)
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
  CMN_KEY_DATA aData;
  if ((TsmGetWSAttri (theCView.WsId, WSWindow, &aData) != TSuccess) ||
      (TxglWinset (call_thedisplay, (WINDOW) aData.ldata) != TSuccess))
  {
    return Standard_False;
  }

  // bind FBO if used
  OpenGl_FrameBuffer* aFrameBuffer = (OpenGl_FrameBuffer* )theCView.ptrFBO;
  GLint aReadBufferPrev = GL_BACK;
  if (aFrameBuffer != NULL && aFrameBuffer->IsValid())
  {
    aFrameBuffer->BindBuffer();
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
    aFrameBuffer->UnbindBuffer();
  }
  else
  {
    glReadBuffer (aReadBufferPrev);
  }
  return Standard_True;
}

void OpenGl_GraphicDriver::RemoveView (const Graphic3d_CView& ACView) {

  Graphic3d_CView MyCView = ACView;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_removeview");
    PrintCView (MyCView, 1);
  }
  call_togl_removeview (&MyCView);

}

void OpenGl_GraphicDriver::SetLight (const Graphic3d_CView& ACView) {

  Graphic3d_CView MyCView = ACView;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_setlight");
    PrintCView (MyCView, 1);
  }
  call_togl_setlight (&MyCView);

}

void OpenGl_GraphicDriver::SetPlane (const Graphic3d_CView& ACView) {

  Graphic3d_CView MyCView = ACView;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_setplane");
    PrintCView (MyCView, 1);
  }
  call_togl_setplane (&MyCView);

}

void OpenGl_GraphicDriver::SetVisualisation (const Graphic3d_CView& ACView) {

  Graphic3d_CView MyCView = ACView;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_setvisualisation");
    PrintCView (MyCView, 1);
  }
  call_togl_setvisualisation (&MyCView);

}

void OpenGl_GraphicDriver::TransformStructure (const Graphic3d_CStructure& ACStructure) {

  Graphic3d_CStructure MyCStructure = ACStructure;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_transformstructure");
    PrintCStructure (MyCStructure, 1);
  }
  call_togl_transformstructure (&MyCStructure);

}

void OpenGl_GraphicDriver :: DegenerateStructure (
  const Graphic3d_CStructure& ACStructure
  ) {
    Graphic3d_CStructure MyCStructure = ACStructure;

    if ( MyTraceLevel ) {

      PrintFunction ( "call_togl_degeneratestructure" );
      PrintCStructure ( MyCStructure, 1 );

    }  // end if

    call_togl_degeneratestructure ( &MyCStructure );
  }  // end OpenGl_GraphicDriver :: DegenerateStructure

  void OpenGl_GraphicDriver::Transparency (const Graphic3d_CView& ACView, const Standard_Boolean AFlag) {

    Graphic3d_CView MyCView = ACView;

    if (MyTraceLevel) {
      PrintFunction ("call_togl_transparency");
      PrintCView (MyCView, 1);
      PrintBoolean ("AFlag", AFlag);
    }
    call_togl_transparency (MyCView.WsId, MyCView.ViewId, (AFlag ? 1 : 0));

  }

  void OpenGl_GraphicDriver::Update (const Graphic3d_CView& ACView, const Aspect_CLayer2d& ACUnderLayer, const Aspect_CLayer2d& ACOverLayer) {

    Graphic3d_CView MyCView = ACView;
    Aspect_CLayer2d MyCUnderLayer = ACUnderLayer;
    Aspect_CLayer2d MyCOverLayer = ACOverLayer;

    if (MyTraceLevel) {
      PrintFunction ("call_togl_update");
      PrintCView (MyCView, 1);
    }
    call_togl_update (&MyCView, &MyCUnderLayer, &MyCOverLayer);

  }

  Standard_Boolean OpenGl_GraphicDriver::View (Graphic3d_CView& ACView) {

    Graphic3d_CView MyCView = ACView;
    Standard_Integer Result;

    if (MyTraceLevel) {
      PrintFunction ("call_togl_view");
      PrintCView (MyCView, 1);
    }
    Result = call_togl_view (&MyCView);
    if (MyTraceLevel) {
      PrintIResult ("call_togl_view", Result);
    }
    return (Result == 1 ? Standard_True : Standard_False);

  }

  void OpenGl_GraphicDriver::ViewMapping (const Graphic3d_CView& ACView, const Standard_Boolean AWait) {

    Graphic3d_CView MyCView = ACView;
    Standard_Integer Result;

    if (MyTraceLevel) {
      PrintFunction ("call_togl_viewmapping");
      PrintCView (MyCView, 1);
      PrintBoolean ("AWait", AWait);
    }
    Result = call_togl_viewmapping (&MyCView, (AWait ? 1 : 0));
    if (MyTraceLevel) {
      PrintIResult ("call_togl_viewmapping", Result);
    }

  }

  void OpenGl_GraphicDriver::ViewOrientation (const Graphic3d_CView& ACView, const Standard_Boolean AWait) {

    Graphic3d_CView MyCView = ACView;
    Standard_Integer Result;

    if (MyTraceLevel) {
      PrintFunction ("call_togl_vieworientation");
      PrintCView (MyCView, 1);
      PrintBoolean ("AWait", AWait);
    }
    Result = call_togl_vieworientation (&MyCView, (AWait ? 1 : 0));
    if (MyTraceLevel) {
      PrintIResult ("call_togl_viewmapping", Result);
    }

  }

  void OpenGl_GraphicDriver :: SetBackFacingModel ( const Graphic3d_CView& aView )
  {
    Graphic3d_CView myView = aView;

    if ( MyTraceLevel ) {

      PrintFunction ( "call_togl_backfacing" );
      PrintCView ( myView, 1 );

    }  // end if

    call_togl_backfacing ( &myView );
  }  // end Graphic3d_GraphicDriver :: SetBackFacingModel
