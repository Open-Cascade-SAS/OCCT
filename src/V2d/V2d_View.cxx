/***********************************************************************
 
     FUNCTION :
     ----------
        Classe V2d_View :
 
     HISTORY OF MODIFICATIONS   :
     --------------------------------
      00-09-92 : GG  ; Creation.
      13-11-97 : EUG ; ???????? New method Plot(...) 
      06-01-98 : FMN ; BUC60065 
      20-02-98 : STT ; S3558 
      08-04-98 : STT ; suppr. S3558
      24-03-98 : GG ; ?????? UseMFT()
      26-03-98 : GG ; BUC60049 Add conversion method 
                      View space -> Window pixel space
      04-05-98 : GG ; PS driver PS is also usable under WNT
      21-07-98 : DCB/SYL: Remove useless calls to SetXXXMap() methods
      in PlotScreen ().
      OCT-98   : DCB/SYL : ScreenCopy() method.
      12-06-00 : TCL : new method Pick( Xmin, Ymin, Xmax, Ymax ) 
      03-10-00 : TCL : new methods SetBackground(...)
      14-11-01 : SAV : Added PickByCircle().
      24-11-01 : SAV : SetBackground(...) overloaded.

     NOTES :
     -----------

************************************************************************/

#define TRACE 0

#define PRO16111        //GG_021198
//                      Correction of method Fitall() that was wrong if 
//                      the window height was superior to the width

#define BUC60611	//VTN_291198
//			Re change Fitall() computation method

#define IMP130300	//GG OPTIMIZATION The V2d_View::UpdateNew() method MUST use 
//			Graphic2d_View::TinyUpdate() and no more Update()

#define JAP60249        //GG_131098
//                      Plots the view accordingly with the requested view center

#define IMP250500       //GG Update() the view after creation
//                      the driver of the view must be known before
//                      any minmax computation.

#define OCC540          //SAV Add Color( Quantity_Color& ) to retrieve background via RGB

/*----------------------------------------------------------------------*/
/*
 * Includes
 */

#include <V2d_View.ixx>
#include <Aspect_Window.hxx>
#include <Aspect_Units.hxx>
#include <Aspect_TypeOfResize.hxx>
#include <Graphic2d_Array1OfVertex.hxx>
#include <Graphic2d_Polyline.hxx>
#include <Graphic2d_Vertex.hxx>
#include <Graphic2d_Marker.hxx>
#include <Graphic2d_Text.hxx>
#include <TCollection_ExtendedString.hxx>
#include <V2d_DefaultMap.hxx>
#include <Graphic2d_CircleMarker.hxx>
#include <Aspect_GenericColorMap.hxx>
#include <Aspect_ColorMap.hxx>
#include <Aspect_ColorMapEntry.hxx>
#include <Quantity_Color.hxx>
#include <Aspect_Background.hxx>
#include <Quantity_Color.hxx>
#include <PS_Driver.hxx>
#include <Standard_ShortReal.hxx>
// S3603 - ScreenCopy ()
#include <Aspect_TypeMap.hxx>
#include <Aspect_WidthMap.hxx>
#include <Aspect_FontMap.hxx>
#include <Aspect_MarkMap.hxx>
#include <PlotMgt_ImageDriver.hxx>
#include <TColStd_HSequenceOfInteger.hxx>

//OCC186
#include <Standard_ErrorHandler.hxx>
#include <Standard_NumericError.hxx>
//OCC186

V2d_View::V2d_View(const Handle(Aspect_WindowDriver)& aWindowDriver,
                   const Handle(V2d_Viewer)& aViewer,
                   const Standard_Real aXCenter,
                   const Standard_Real aYCenter,
                   const Standard_Real aSize)

: myWindowDriver(aWindowDriver) ,
  myViewer(aViewer.operator->()),
  myFitallRatio (0.01) ,
  myHitPoint(new Graphic2d_GraphicObject(aViewer->View())),
  myHitBuf(new Graphic2d_Buffer(aViewer->View(),0.,0.))
{

  myViewMapping = new Graphic2d_ViewMapping;
  myViewMapping->SetViewMapping(aXCenter,aYCenter,aSize);
  myViewMapping->SetViewMappingDefault();
  myWindowDriver->WorkSpace(myWidth,myHeight);
  this->MapToCenter();
  myEnablePrevious = Standard_True;
  this->StoreCurrent();
  myDeflection = 0.0001 METER; // 10th of  millimeter at the screen.

  aWindowDriver->SetColorMap(aViewer->ColorMap());
  aWindowDriver->SetWidthMap(aViewer->WidthMap());
  aWindowDriver->SetTypeMap (aViewer->TypeMap());
  aWindowDriver->SetFontMap (aViewer->FontMap(),aViewer->UseMFT());
  aWindowDriver->SetMarkMap (aViewer->MarkMap());
  aViewer->AddView(this);
#ifdef IMP250500
  Update();
#endif
}
void V2d_View::SetDefaultPosition(const Standard_Real aXCenter,
                   const Standard_Real aYCenter,
                   const Standard_Real aSize) {

  myViewMapping->SetViewMapping(aXCenter,aYCenter,aSize);
  myViewMapping->SetViewMappingDefault();
  this->MapToCenter();
  this->StoreCurrent();
}
void V2d_View::Fitall() {

  this->StoreCurrent();
  this->MapToCenter();

  Quantity_Ratio rat = myWindowDriver->Window()->Ratio();

  Quantity_Length XVMin,YVMin,XVMax,YVMax;
  myViewer->View()->MinMax(XVMin,XVMax,YVMin,YVMax);
  if(XVMin < ShortRealFirst() + 1. || YVMin < ShortRealFirst() + 1.) {
    XVMin = ShortRealLast(); YVMin = ShortRealLast();    // Minmax are crossed, as
    XVMax = ShortRealFirst();YVMax = ShortRealFirst();   // CAL does not do it!!!!!!!!!!!!
  }

  Quantity_Length XMMin,YMMin,XMMax,YMMax;
  myViewer->View()->MarkerMinMax(XMMin,XMMax,YMMin,YMMax);
  if(XMMin < ShortRealFirst() + 1. || YMMin < ShortRealFirst() + 1.) {
    XMMin = ShortRealLast(); YMMin = ShortRealLast();    // Minmax are crossed, because 
    XMMax = ShortRealFirst();YMMax = ShortRealFirst();   // CAL does not do it !!!!!!!!!!!!
  }

  Quantity_Length XMin = Min(XVMin,XMMin),YMin= Min(YVMin,YMMin),
                  XMax = Max(XVMax,XMMax),YMax= Max(YVMax,YMMax);

  Quantity_Length Xold,Yold,Sizeold;
  myViewMapping->ViewMapping(Xold,Yold,Sizeold);

#ifdef PRO16111
  Quantity_Length Xnew = Xold,Ynew = Yold,Sizenew = Sizeold;
  Quantity_Length Dxnew = Sizeold, Dynew = Sizeold;
  if( XMax > XMin ) {
    Dxnew = XMax - XMin;
    Xnew =  (XMax+XMin)/2.;
  }
  if( YMax > YMin ) {
    Dynew = YMax - YMin;
    Ynew =  (YMax+YMin)/2.;
  }
  if( rat >= 1. ) {
#ifdef BUC60611
    if( Dxnew >= Dynew*rat ) Sizenew = Dxnew/2./rat;
#else
    if( Dxnew >= Dynew ) Sizenew = Dxnew/2./rat;
#endif
    else Sizenew = Dynew/2.;
  } else {
#ifdef BUC60611
    if( Dynew >= Dxnew/rat ) Sizenew = Dynew/2.*rat;
    else Sizenew = Dxnew/2;
#else
    if( Dxnew >= Dynew ) Sizenew = Dxnew/2.;
    else Sizenew = Dynew/2.*rat;
#endif
  }
  Sizenew += Sizenew * myFitallRatio;
#else 
  Quantity_Length Xnew,Ynew,Sizenew;
  if ( XMin < XMax ) {
    Xnew =  (XMax+XMin)/2.;
    Sizenew = (XMax-XMin)/(1. - myFitallRatio)/2./rat;
  }
  else {
    Xnew = Xold;
    Sizenew = Sizeold;
  }

  if ( YMin <  YMax ) {
    Ynew =  (YMax+YMin)/2.;
    Sizenew = Max ( Sizenew , (YMax-YMin)/(1. - myFitallRatio)/2.);
    }
  else {
    Ynew = Yold;
    Sizenew = Max ( Sizenew , Sizeold);
    }
#endif

  myViewMapping->SetViewMapping( Xnew ,Ynew , Sizenew);
  ImmediateUpdate();
}

void V2d_View::WindowFit(const Standard_Integer aX1,
                   const Standard_Integer aY1,
                   const Standard_Integer aX2,
                   const Standard_Integer aY2) {
 Fit(aX1,aY1,aX2,aY2);
}

void V2d_View::Fit(const Standard_Integer aX1,
                   const Standard_Integer aY1,
                   const Standard_Integer aX2,
                   const Standard_Integer aY2) {

  if( aX1 != aX2 && aY1 != aY2 ) {
    this->MapToCenter();
    Standard_Real x1,y1,x2,y2,xx1,yy1,xx2,yy2;
    myWindowDriver->Convert(aX1,aY1,xx1,yy1);
    myWindowDriver->Convert(aX2,aY2,xx2,yy2);
    myViewer->View()->Convert(myViewMapping,xx1,yy1,myXPosition,myYPosition,myScale,x1,y1);
    myViewer->View()->Convert(myViewMapping,xx2,yy2,myXPosition,myYPosition,myScale,x2,y2);
    Fit(x1,y1,x2,y2,Standard_False);
  }
}
void V2d_View::Fit(const Quantity_Length aX1,
                   const Quantity_Length aY1,
                   const Quantity_Length aX2,
                   const Quantity_Length aY2,
                   const Standard_Boolean UseMinimum) {

  Viewer_BadValue_Raise_if( aX1 == aX2 || aY1 == aY2 ,"coordinates are confused");
  this->StoreCurrent();
  Quantity_Ratio rat = myWindowDriver->Window()->Ratio();
  Quantity_Length XCenter, YCenter, Size;
  XCenter = (aX1+aX2)/2.; YCenter = (aY1+aY2)/2.;
  if (UseMinimum)
    Size = Abs(aX2-aX1)/rat < Abs(aY2-aY1) ? Abs(aX2-aX1)/rat : Abs(aY2-aY1);
  else
    Size = Abs(aX2-aX1)/rat > Abs(aY2-aY1) ? Abs(aX2-aX1)/rat : Abs(aY2-aY1);

  myViewMapping->SetViewMapping(XCenter,YCenter,Size/2.);
//OCC186
  try {
    ImmediateUpdate();
  }
  catch (Standard_NumericError) {
    cout << "Exception caught during view Fit. " << endl;
  }
//OCC186
}

void V2d_View::SetFitallRatio (const Standard_Real aRatio) {

  Viewer_BadValue_Raise_if( aRatio < 0 || aRatio >=1 , 
                            "ratio must be  more than 0. and less than 1.");
  myFitallRatio = aRatio;

}
void V2d_View::Zoom (const Standard_Real Zoom) {
  Viewer_BadValue_Raise_if(Zoom <= 0., "Zoom coefficient must be greater than 0.");
  this->StoreCurrent();
  Quantity_Length XCenter, YCenter, Size;

  myViewMapping->ViewMapping(XCenter, YCenter, Size);
  myViewMapping->SetViewMapping(XCenter,YCenter,Size/Zoom);

//OCC186
  try {
    ImmediateUpdate();
  }
  catch (Standard_NumericError) {
    cout << "Exception caught during zooming! Zooming factor is too big." << endl;
  }
//OCC186
}

void V2d_View::Zoom (const Standard_Integer aX1,
                     const Standard_Integer aY1,
                     const Standard_Integer aX2,
                     const Standard_Integer aY2,
                     const Quantity_Ratio aCoefficient) {
  Standard_Real D = Abs(aX1-aX2) >= Abs(aY1-aY2) ? aX1-aX2 : aY2-aY1;
  this->Zoom ( D > 0 ? ( 1. /(1. + aCoefficient * Abs(D))) 
               : (1. + aCoefficient * Abs(D)));
}

void V2d_View::Zoom (const Standard_Integer aX,
                     const Standard_Integer aY,
                     const Quantity_Ratio aCoefficient) {

  Standard_Integer Width, Height;
  myWindowDriver->Window()->Size(Width, Height);
  this->Zoom(Width/2,Height/2,aX,aY,aCoefficient);
}
void V2d_View::Magnify(const Handle(V2d_View)& anOriginView,
                       const Standard_Integer X1,
                       const Standard_Integer Y1,
                       const Standard_Integer X2,
                       const Standard_Integer Y2) {
  Standard_Real x1,y1,x2,y2;
  anOriginView->Convert(X1,Y1,x1,y1);
  anOriginView->Convert(X2,Y2,x2,y2);
  myViewMapping->SetViewMapping((x1+x2)/2.,(y1+y2)/2.,Max(x2-x1,y2-y1)/2.);
  myViewMapping->SetViewMappingDefault();
//OCC186
  try {
    ImmediateUpdate();
  }
  catch (Standard_NumericError) {
    cout << "Exception caught during view Magnify. " << endl;
  }
//OCC186
}

void V2d_View::Translate (const Quantity_Length dx, const Quantity_Length dy) {

  this->StoreCurrent();

  Quantity_Length XCenter, YCenter;
  myViewMapping->Center(XCenter, YCenter);
  myViewMapping->SetCenter(XCenter+dx,YCenter+dy);

}

void V2d_View::Place (const Standard_Integer x, const Standard_Integer y,
                      const Quantity_Length aZoomFactor) {
  Quantity_Length xx,yy;
  this->Convert(x,y,xx,yy);
  this->ScreenPlace(xx,yy,aZoomFactor);
}
void V2d_View::ScreenPlace (const Quantity_Length x, const Quantity_Length y,
                     const Quantity_Factor aZoomFactor)  {

  this->StoreCurrent();
  myViewMapping->SetCenter(x,y);
  Zoom(aZoomFactor/Zoom());
}
void V2d_View::Pan (const Standard_Integer dx, const Standard_Integer dy) {

  Standard_Real x,y;
  x = myWindowDriver->Convert(dx);
  y = myWindowDriver->Convert(dy);
  Standard_Real xx,yy;
  xx =   myViewer->View()->Convert(myViewMapping,x,myScale);
  yy =   myViewer->View()->Convert(myViewMapping,y,myScale);
  this->Translate(-xx,-yy);
  ImmediateUpdate();
}

Quantity_Length V2d_View::Convert(const Standard_Integer V) const {

  return myViewer->View()->Convert(myViewMapping,myWindowDriver->Convert(V),myScale);
}

void V2d_View::Convert (const Standard_Integer X, const Standard_Integer Y,
                        Quantity_Length& ViewX, Quantity_Length& ViewY) 
  const {

  Standard_Real xx,yy;
  myWindowDriver->Convert(X,Y,xx,yy);
  myViewer->View()->Convert(myViewMapping,xx,yy,myXPosition,myYPosition,myScale,
           ViewX,ViewY);
}

void V2d_View::Convert (const Quantity_Length ViewX, const Quantity_Length ViewY, Standard_Integer& X, Standard_Integer &Y)
  const {

  Standard_Real XCenter,YCenter,Size;
  myViewMapping->ViewMapping(XCenter,YCenter,Size);
  Standard_Real x = myXPosition + ((ViewX-XCenter)/Size) *myScale;
  Standard_Real y = myYPosition + ((ViewY-YCenter)/Size) *myScale;
  myWindowDriver->Convert(x,y,X,Y);
}

Quantity_Length V2d_View::Convert(const Quantity_Length DriverSize) const {
  Quantity_Length X,Y,S;
  myViewMapping->ViewMapping(X,Y,S);
  return DriverSize / myScale * S;
}

void V2d_View::Reset () {

  this->StoreCurrent();
  this->MapToCenter();
  myViewMapping->ViewMappingReset();
  ImmediateUpdate();
}

Handle(Graphic2d_View) V2d_View::View () const {return myViewer->View();}

Handle(V2d_Viewer) V2d_View::Viewer () const {return myViewer;}

Handle(Aspect_WindowDriver) V2d_View::Driver() const {return myWindowDriver;}

void V2d_View::Update() const { 

//Augustin myWindowDriver->Window()->Clear();
//  EraseHit();
  Quantity_Length XCenter, YCenter, Size;
  myViewer->View()->SetTypeOfDeflection(Aspect_TOD_ABSOLUTE);
  myViewMapping->ViewMapping(XCenter, YCenter, Size);
  myViewer->View()->SetDeflection(myDeflection*Size/myScale);
  myViewer->View()->Update(myWindowDriver,myViewMapping,myXPosition,myYPosition,myScale);}

void V2d_View::UpdateNew() const {

#ifdef IMP130300
  myViewer->View()->TinyUpdate(myWindowDriver,myViewMapping,myXPosition,myYPosition,myScale,Standard_True);
#else
  myViewer->View()->Update(myWindowDriver,myViewMapping,myXPosition,myYPosition,myScale,Standard_False);
#endif
}
void V2d_View::Restore() const { 

  if(myWindowDriver->Window()->BackingStore()) 
    myWindowDriver->Window()->Restore();
  else 
    this->Update();
 }   

void V2d_View::RestoreArea (const Standard_Integer Xc,
                            const Standard_Integer Yc,
                            const Standard_Integer Width,
                            const Standard_Integer Height) const {

  if(myWindowDriver->Window()->BackingStore()) 
    myWindowDriver->Window()->RestoreArea(Xc,Yc,Width,Height);
  else 
    this->Update();
 }   
        
void V2d_View::Dump() const { 
 //myView->Update(new V2d_DumpDriver);
}

void V2d_View::Dump( const Standard_CString aFileName) const { 
 myWindowDriver->Window()->Dump(aFileName); 
}

Handle(Graphic2d_DisplayList) V2d_View::Pick (
				const Standard_Integer X,
                                const Standard_Integer Y,
                                const Standard_Integer aPrecision) {

  Standard_Real x,y,xx,yy,prec;
  myWindowDriver->Convert(X,Y,xx,yy);
  prec =  myViewer->View()->Convert(myViewMapping,
                          myWindowDriver->Convert(aPrecision),
                          myScale);
  myViewer->View()->Convert(myViewMapping,xx,yy,myXPosition,myYPosition,myScale,x,y);
  return myViewer->View()->Pick(myViewMapping,x,y,prec,myXPosition,myYPosition,myScale);
}

//SAV
Handle(Graphic2d_DisplayList) V2d_View::PickByCircle( const Standard_Integer X,
						      const Standard_Integer Y,
						      const Standard_Integer Radius )
{
  Standard_Real x, y, xx, yy, radius;
  myWindowDriver->Convert(X,Y,xx,yy);
  radius =  myViewer->View()->Convert( myViewMapping,
				       myWindowDriver->Convert( Radius ),
				       myScale );
  myViewer->View()->Convert( myViewMapping,
			     xx,yy, myXPosition, myYPosition, myScale, x, y );
  return myViewer->View()->PickByCircle( myViewMapping, x, y, radius, 
					 myXPosition, myYPosition, myScale );
}

Handle(Graphic2d_DisplayList) V2d_View::Pick (
				const Standard_Integer Xmin,
                const Standard_Integer Ymin,
                const Standard_Integer Xmax,
                const Standard_Integer Ymax,
                const Graphic2d_PickMode aPickMode ) {

  Standard_Real x1, y1, xx1, yy1, x2, y2, xx2, yy2;
  myWindowDriver->Convert( Xmin, Ymin, xx1, yy1 );
  myWindowDriver->Convert( Xmax, Ymax, xx2, yy2 );

  myViewer->View()->Convert( myViewMapping, xx1, yy1, myXPosition, myYPosition, myScale, x1, y1 );
  myViewer->View()->Convert( myViewMapping, xx2, yy2, myXPosition, myYPosition, myScale, x2, y2 );

  return myViewer->View()->Pick( myViewMapping, x1, y1, x2, y2, myXPosition, myYPosition, myScale, aPickMode );
}
  
void V2d_View::Erase () { 
  myViewer->View()->Erase();
}

void V2d_View::HasBeenMoved() {

  myWindowDriver->ResizeSpace();
}

void V2d_View::MustBeResized(const V2d_TypeOfWindowResizingEffect anEffect) {

  Standard_Real OldWidth = myWidth, OldHeight = myHeight;
  Aspect_TypeOfResize TOR = myWindowDriver->ResizeSpace();
  myWindowDriver->WorkSpace(myWidth,myHeight);
  
  switch (anEffect) {

  case V2d_TOWRE_ENLARGE_SPACE: {
    switch (TOR) {
      case Aspect_TOR_UNKNOWN : break;
      case Aspect_TOR_NO_BORDER : break;
      case Aspect_TOR_TOP_BORDER :  break;
      case Aspect_TOR_RIGHT_BORDER :  break;
      case Aspect_TOR_BOTTOM_BORDER :  
        myYPosition = myHeight - OldHeight + myYPosition;
        break;
      case Aspect_TOR_LEFT_BORDER : 
        myXPosition = myWidth -  OldWidth  + myXPosition;
        break;
      case Aspect_TOR_TOP_AND_RIGHT_BORDER : break;
      case Aspect_TOR_RIGHT_AND_BOTTOM_BORDER : 
        myYPosition = myHeight - OldHeight + myYPosition;
        break;
      case Aspect_TOR_BOTTOM_AND_LEFT_BORDER : 
        myXPosition = myWidth - OldWidth + myXPosition;
        myYPosition = myHeight - OldHeight + myYPosition;
        break;
      case Aspect_TOR_LEFT_AND_TOP_BORDER: 
        myXPosition = myWidth - OldWidth + myXPosition;
        break;
      }
    break;
  }
  case V2d_TOWRE_ENLARGE_OBJECTS:{
    
    this->MapToCenter();
    break;
  }
  }
  this->StoreCurrent();
  ImmediateUpdate();
}

void V2d_View::SetDefaultHighlightColor(const Standard_Integer aColorIndex) {

  myViewer->View()->SetDefaultOverrideColor(aColorIndex);

}

#define SCREENCOPY_FILENAME "screencopy2d.gif"
#define NO_HILIGHT          0xFFFFFF
void V2d_View::ScreenCopy (const Handle(PlotMgt_PlotterDriver)& aPlotterDriver,
                           const Standard_Boolean fWhiteBackground,
                           const Quantity_Factor aPlotScale)
{
  int i ;
  TCollection_AsciiString aFileToDump;
  Quantity_Factor    aScale;
  Quantity_Length    thePixel;
  Quantity_Parameter theWWidth, theWHeight;
  Quantity_Parameter thePWidth, thePHeight;
  Quantity_Length    aViewX, aViewY, aViewSize;
  aPlotterDriver -> WorkSpace (thePWidth, thePHeight);
  myWindowDriver -> WorkSpace (theWWidth, theWHeight);
  myViewMapping  -> ViewMapping (aViewX, aViewY, aViewSize);
  thePixel = myWindowDriver -> Convert (1);

  if (theWWidth * theWHeight != 0.) {
    if (aPlotScale == 0.) {
      aScale = Min (thePWidth / theWWidth, thePHeight / theWHeight);
    } else {
      aScale = aPlotScale;
      aScale *= (aViewSize / theWWidth); 
    }
    Quantity_Length aPlotX, aPlotY;
    aPlotX = theWWidth  /2. * aScale;
    aPlotY = theWHeight /2. * aScale;

    // Set default maps (color, type, etc.) for plotter driver
    aPlotterDriver -> SetColorMap ( new Aspect_GenericColorMap () );
    aPlotterDriver -> SetTypeMap  ( new Aspect_TypeMap         () );
    aPlotterDriver -> SetWidthMap ( new Aspect_WidthMap        () );
    aPlotterDriver -> SetFontMap  ( new Aspect_FontMap         () );
    aPlotterDriver -> SetMarkMap  ( new Aspect_MarkMap         () );
    myViewer -> View() -> SetTypeOfDeflection (Aspect_TOD_ABSOLUTE);
    myViewer -> View() -> SetDeflection       (myDeflection);
  
    // Set backgroung to white, unihiglight if any
    Handle(TColStd_HSequenceOfInteger) theColors;
    Handle(Graphic2d_DisplayList)      theDisplayList;
    Aspect_Background theBack = myWindowDriver->Window()->Background();
    if (fWhiteBackground)
      myWindowDriver -> Window() -> SetBackground (Quantity_NOC_WHITE);
    theDisplayList = myViewer -> View() -> DisplayList();
    theColors      = new TColStd_HSequenceOfInteger ();
    for ( i = 1; i <= theDisplayList -> Length(); i++) {
      if (theDisplayList -> Value(i) -> IsHighlighted()) {
        theColors -> Append (theDisplayList -> Value(i) -> OverrideColor());
        theDisplayList -> Value(i) -> Display();
      } else {
        theColors -> Append (NO_HILIGHT);
      }
    }
    Update ();

    // Dump the view
    if (aPlotterDriver->IsKind(STANDARD_TYPE(PlotMgt_ImageDriver))) {
      aFileToDump  = aPlotterDriver->PlotFileName();
    } else {
      aFileToDump  = aPlotterDriver->SpoolDirectory();
      aFileToDump += SCREENCOPY_FILENAME;
    }
    myWindowDriver -> Window() -> Dump (aFileToDump.ToCString());

    // Return background and highlight if any
    for (i = 1; i <= theDisplayList -> Length(); i++)
      if (theColors -> Value(i) != NO_HILIGHT)
        theDisplayList -> Value(i) -> Highlight(theColors -> Value(i));
    if (fWhiteBackground)
      myWindowDriver -> Window() -> SetBackground (theBack);
    Update ();

    // Draw imagefile by plotter driver
    aPlotterDriver -> SetPixelSize (thePixel);
    aPlotterDriver -> BeginDraw ();
    aPlotterDriver -> DrawImageFile (
      aFileToDump.ToCString(), (float)aPlotX, (float)aPlotY, aScale);
    aPlotterDriver -> EndDraw   ();
  }
}
#undef NO_HILIGHT
#undef SCREENCOPY_FILENAME

void V2d_View::Plot(const Handle(PlotMgt_PlotterDriver)& aPlotterDriver,
                    const Quantity_Length aXCenter,
                    const Quantity_Length aYCenter,
                    const Quantity_Factor aPlotScale) const
{
  Quantity_Length                 PaperWidth,PaperHeight;
  
  aPlotterDriver->SetColorMap(myWindowDriver->ColorMap());
  aPlotterDriver->SetTypeMap(myWindowDriver->TypeMap());
  aPlotterDriver->SetWidthMap(myWindowDriver->WidthMap());
  aPlotterDriver->SetFontMap(myWindowDriver->FontMap(),myWindowDriver->UseMFT());
  aPlotterDriver->SetMarkMap(myWindowDriver->MarkMap());
  myViewer->View()->SetTypeOfDeflection(Aspect_TOD_ABSOLUTE);
  myViewer->View()->SetDeflection(myDeflection);
  
  aPlotterDriver->WorkSpace (PaperWidth,PaperHeight);
  // size of the map to force set the required size 
  Quantity_Length aViewX , aViewY , aViewSize;
  myViewMapping -> ViewMapping ( aViewX, aViewY, aViewSize );
  Quantity_Factor aScale = aViewSize * aPlotScale;

  Quantity_Length aPlotX , aPlotY;
#ifdef JAP60249
//printf(" V2d_View::Plot(%f,%f,%f,%f,%f,%f,%f)\n",PaperWidth,PaperHeight,aXCenter,aYCenter,aPlotScale,aViewSize,aScale);
  Handle(Graphic2d_ViewMapping) myPlotMapping = new Graphic2d_ViewMapping;
  myPlotMapping->SetViewMapping(aXCenter,aYCenter,aViewSize);
  aPlotX = /*aPlotScale * */PaperWidth/2.;
  aPlotY = /*aPlotScale * */PaperHeight/2.;
  myViewer->View()->Update(aPlotterDriver,myPlotMapping,aPlotX,aPlotY,aScale);
#else
  // to place the bottom view to the left of the list and not in the middle  
  if (myWidth < myHeight ){
    aPlotX = aViewSize*aPlotScale;
    aPlotY = aPlotX*myHeight/myWidth;
    if (PaperWidth > PaperHeight)
      aPlotX *= 2.;
  } else {
    aPlotY  = aViewSize*aPlotScale;
    aPlotX  = aPlotY*myWidth/myHeight;  
    if (PaperWidth < PaperHeight)
      aPlotY *= 2.;
  }

  myViewer->View()->Update(aPlotterDriver,myViewMapping,aPlotX,aPlotY,aScale);
#endif

#if TRACE > 0
  // info size paper 
  cout << "V2d_View::Plot(Driver,centre X,Y,scale):CenterX" << aViewX << " Y " << aViewY << " Size=" << aViewSize 
    << " ViewPosX=" << myXPosition << " ViewPosY=" << myYPosition 
      << " VueX=" << myWidth << " VueY=" << myHeight 
        << " PlotX=" << PaperWidth << " PlotY=" << PaperHeight
          << " PlotScale=" << aPlotScale 
            << " PosX=" << aPlotX << " Posy=" << aPlotY << " Scale " << aScale 
              << endl ;
#endif
}

void V2d_View :: Plot (
                  const Handle( PlotMgt_PlotterDriver )& aDriver,
                  const Quantity_Factor          aScale
                 ) const {
#if TRACE > 0
 cout << "V2d_View::Plot(driver,scale) scale=" << aScale << endl  ;
#endif
 Plot( aDriver, (Quantity_Length) 0., (Quantity_Length) 0., aScale);
}  

void V2d_View::PostScriptOutput(const Standard_CString aFile,
                                const Quantity_Length aWidth,
                                const Quantity_Length aHeight,
                                const Quantity_Length aXCenter,
                                const Quantity_Length aYCenter,
                                const Quantity_Factor aScale,
                                const Aspect_TypeOfColorSpace aTypeOfColorSpace) 
  const {
  Handle(PS_Driver) aPSDriver = new PS_Driver(aFile,aWidth,aHeight,aTypeOfColorSpace);
//  if(aTypeOfColorSpace != PS_TOCS_BlackAndWhite) 
//     aPSDriver->SetColorMap(myWindowDriver->ColorMap());
  Plot(aPSDriver,aXCenter,aYCenter,aScale);
}

void V2d_View::PlotScreen(const Handle(PlotMgt_PlotterDriver)& aPlotterDriver) const {
 
  // recuperate size paper
  Quantity_Length PaperWidth,PaperHeight;
  aPlotterDriver->WorkSpace(PaperWidth,PaperHeight);

  // Determination size to fit all in the paper
  Quantity_Length aViewX , aViewY , aViewSize;
  Quantity_Length aViewSizeX , aViewSizeY;
  Quantity_Length aPlotX , aPlotY;
  Standard_Real aPlotScale;
  myViewMapping -> ViewMapping ( aViewX, aViewY, aViewSize );
  if (myWidth < myHeight ){
    aViewSizeX = aViewSize;
    aViewSizeY = aViewSizeX*myHeight/myWidth;
  }else {
    aViewSizeY = aViewSize;
    aViewSizeX = aViewSizeY*myWidth/myHeight;  
  }
  aPlotX = PaperWidth /2./aViewSizeX;
  aPlotY = PaperHeight/2./aViewSizeY;
  aPlotScale = (aPlotX < aPlotY  ? aPlotX : aPlotY );

  Quantity_Length aXCenter, aYCenter;
  this->Center(aXCenter, aYCenter);

#if TRACE > 0
  cout << "V2d_View::PlotScreen(Driver):Centre X=" << aViewX << " Y=" << aViewY << " Size=" << aViewSize   
      << " SizeX=" << aViewSizeX << " Y=" << aViewSizeY 
        << " PlotX=" << PaperWidth << " Y=" << PaperHeight        
          << " PlotScaleX=" << aPlotX << " Y=" << aPlotY 
            << " => PlotScale=" << aPlotScale 
              << endl ;  
#endif

  this->Plot(aPlotterDriver,aXCenter,aYCenter,aPlotScale);

}

void V2d_View::ScreenPostScriptOutput(const Standard_CString aFile,
                                const Quantity_Length aWidth,
                                const Quantity_Length aHeight,
                                const Aspect_TypeOfColorSpace aTypeOfColorSpace)
 const {
  Handle(PS_Driver) aPSDriver = new PS_Driver(aFile,aWidth,aHeight,aTypeOfColorSpace);
//  if(aTypeOfColorSpace != PS_TOCS_BlackAndWhite) 
//     aPSDriver->SetColorMap(myWindowDriver->ColorMap());
  PlotScreen(aPSDriver);
}


void V2d_View::StoreCurrent () {

  if(myEnablePrevious) {

    myViewMapping->ViewMapping(myPreviousX,myPreviousY,myPreviousSize);
    myPreviousXPosition = myXPosition;
    myPreviousYPosition = myYPosition;
    myPreviousScale = myScale;
  }

}
void V2d_View::Previous () {

  if(myEnablePrevious) {
    Standard_Real x,y,s,a,b,c;
    x = myPreviousX;
    y = myPreviousY;
    s = myPreviousSize;
    a = myPreviousXPosition;
    b = myPreviousYPosition;
    c = myPreviousScale;
    this->StoreCurrent();
    myViewMapping->SetViewMapping(x,y,s);
    myXPosition = a;
    myYPosition = b;
    myScale = c;
  }
  ImmediateUpdate();
}
void V2d_View::SetDeflection (const Quantity_Length aDeflection) {

  myDeflection = aDeflection;

}

Quantity_Length V2d_View::Deflection() const { 

  return myDeflection; 
}

void V2d_View::EnableStorePrevious () {myEnablePrevious = Standard_True;}

void V2d_View::DisableStorePrevious () {

this->StoreCurrent();
myEnablePrevious = Standard_False;}


void V2d_View::MapToCenter () {

   myXPosition =  myWidth  /2.;
   myYPosition =  myHeight /2.;
   myScale     = ( (myWidth<myHeight) ? myWidth : myHeight ) /2.; 

}
Quantity_Factor V2d_View::Zoom () const {
  return 1./myViewMapping->Zoom();
}

void V2d_View::Center (Quantity_Length& aX, Quantity_Length& aY) const {
  myViewMapping->Center(aX,aY);
}

Quantity_Length V2d_View::Size() const {
  Quantity_Length XCenter, YCenter, Size;
  myViewMapping->ViewMapping(XCenter, YCenter, Size);
  return Size;
}

Quantity_NameOfColor V2d_View::Color () const {
  return myWindowDriver->Window()->Background().Color().Name();
}

void V2d_View::Color( Quantity_Color& color ) const
{
#ifdef OCC540
  color.Assign( myWindowDriver->Window()->Background().Color() );
#endif
}

void V2d_View::Hit(const Standard_Integer X,
                   const Standard_Integer Y,
                   Quantity_Length& gx,
                   Quantity_Length& gy) const {

  Standard_Real rx,ry;
  Convert(X,Y,rx,ry);
  myViewer->Hit(rx,ry,gx,gy);
}

void V2d_View::ShowHit(const Standard_Integer X,
                   const Standard_Integer Y) {
  Standard_Real rx,ry,gx,gy;
  Convert(X,Y,rx,ry);
  myViewer->Hit(rx,ry,gx,gy);

  myHitPoint->RemovePrimitives();
  Handle(Graphic2d_Marker) 
    M = new Graphic2d_Marker(myHitPoint,
                             myViewer->HitPointMarkerIndex(),
                             gx,gy,
                             0.002 METER,0.002 METER);
  M->SetColorIndex(myViewer->HitPointColor());


  Standard_Real HitTextX,HitTextY;
  myViewer->View()->Convert(myViewMapping,0.,0.,myXPosition,myYPosition,myScale,HitTextX,HitTextY);
  Handle(Graphic2d_Text)
   T1 = new Graphic2d_Text(myHitPoint,
                           TCollection_ExtendedString(gx),
                           HitTextX,HitTextY,0.);
  T1->SetColorIndex(myViewer->CoordinatesColor());
  T1->SetOffset(0. METER,0.005 METER);
  T1->SetZoomable(Standard_False); //BUC50093
  Handle(Graphic2d_Text)
    T2 = new Graphic2d_Text(myHitPoint,
                           TCollection_ExtendedString(gy),
                            HitTextX,HitTextY,0.);
  T2->SetColorIndex(myViewer->CoordinatesColor());
  T2->SetZoomable(Standard_False); //BUC50093
//if(myHitPoint->IsDisplayed()) myHitPoint->Erase();
//myHitPoint->Display();
  if(myHitBuf->IsPosted()) myHitBuf->UnPost();
  myHitBuf->Clear();
  myHitBuf->Add(myHitPoint);
  myHitBuf->Post();
}

void V2d_View::EraseHit() {
//  if(myHitPoint->IsDisplayed()) myHitPoint->Erase();
  if(myHitBuf->IsPosted()) myHitBuf->UnPost();
}  
void V2d_View::Scroll(Standard_Integer& dxc, Standard_Integer& dyc,
                 Standard_Integer& lx, Standard_Integer& ly) {


  Standard_Boolean Case = lx > 0 ? Standard_True: Standard_False; //war
// Case = Standard_false: there are only markers at the position 0,0;
// Case = Standard_True:  works properly only if there is nothing but geometry.

  lx = Abs(lx);  // war

  Quantity_Length XCenter, YCenter, Size;
  myViewMapping->ViewMapping(XCenter, YCenter, Size);


  Convert( - dxc , - dyc ,pxmin,pymax);
  Convert( - dxc + lx, - dyc + ly ,pxmax,pymin);


  Quantity_Length XMin,YMin,XMax,YMax;
  myViewer->View()->MinMax(XMin,XMax,YMin,YMax);

  dxc = 0;
  dyc = 0;

  if(XMin <= XMax && YMin <= YMax) { // empty view

// warning !!!
    if(!Case) {
      XMin = XMin  / myScale * Size;
      XMax = XMax  / myScale * Size;
      YMin = YMin  / myScale * Size;
      YMax = YMax  / myScale * Size;
    }
// end war

    Standard_Boolean t = Standard_False;

    if (XMin < pxmin) {
      Standard_Real dx = XMin - pxmin;
      Translate(dx,0);
      Standard_Real ddx = dx  /Size * myScale; 
      dxc = myWindowDriver->Convert(ddx);
      t = Standard_True;
    }
    else
      XMin = pxmin;
    
    if (YMax > pymax) {
      Standard_Real dy = YMax - pymax;
      Translate(0,dy);
      Standard_Real ddy = dy  /Size * myScale; 
      dyc = myWindowDriver->Convert(ddy);
      t = Standard_True;
    }    
    else
      YMax = pymax;
    
    if (XMax < pxmax) XMax = pxmax;
    if (YMin > pymin) YMin = pymin;
    
    if(t) ImmediateUpdate();

    Standard_Real a = (XMax - XMin)/Size * myScale; 
    Standard_Real b = (YMax - YMin)/Size * myScale; 
    
    
    lx = myWindowDriver->Convert(a);
    ly = myWindowDriver->Convert(b);
  }
}


Standard_Integer V2d_View::DefaultHighlightColor() const {
Standard_Integer index = 
	(myViewer->View()->IsDefinedColor()) ?
  	myViewer->View()->DefaultOverrideColor() : -1;

  return index;
}

void V2d_View::SetBackground(const Quantity_NameOfColor aNameColor) {

   myWindowDriver->Window()->SetBackground( aNameColor );
}

Standard_Boolean V2d_View::SetBackground( const Standard_CString aNameFile,
					  const Aspect_FillMethod aMethod ) {

  return myWindowDriver->Window()->SetBackground(aNameFile,aMethod);

}

void V2d_View::SetBackground( const Quantity_Color& color )
{
  myWindowDriver->Window()->SetBackground( color );
}
