/***********************************************************************

FONCTION :
----------
Classe V3d_View :

HISTORIQUE DES MODIFICATIONS   :
--------------------------------
00-09-92 : GG  ; Creation.
02-10-96 : FMN ; Suppression appel Redraw sans MustBeResized()
05-06-97 : FMN ; Correction FitAll()
30-06-97 : GG ; Correction + Optimisation de Panning(...)
On fait la translation + le zoom en une seule
operation au lieu de 2 precedemment qui etait buggee.
09-07-97 : FMN ; Correction FitAll() sur le Ratio
16-07-97 : FMN ; Correction FitAll() sur le calcul de la Box
22-07-97 : FMN ; Ajout mode RetainMode pour le Transient
15-12-97 : FMN ; Ajout texture mapping
17-12-97 : FMN ; CTS19129 Correction FitAll() multiple
18-12-97 : FMN ; Ajout mode Ajout
24-12-97 : FMN ; Remplacement de math par MathGra
24-12-97 : CQO ; BUC50037 Xw_Window -> Aspect_Window
31-12-97 : CAL ; Remplacement de MathGra par Array2OfReal
07-01-98 : CAL ; Ajout de la methode DoMapping.
07-01-98 : CAL ; Retrait de tous les "this->" inutiles
21-01-98 : CAL ; Remplacement des Window->Position () par Window->Size ()
27-01-98 : FMN ; PERF: OPTIMISATION LOADER (LOPTIM)
12-02-98 : GG  ; Reactivation du Redraw dans MustBeResized()
23-02-98 : FMN ; Remplacement PI par Standard_PI
25-02-98 : FMN ; PERF.27: Optimisation of view creation from existing view
11-03-98 : STT ; S3558
19-03-98 : FMN ; Probleme dans FitAll car la methode WNT_Window::Size(Real,Real)
ne marche pas.
08-04-98 : STT ; suppr. S3558
10-04-98 : CAL ; Ajout des methodes RefToPix et PixToRef
13-06-98 : FMN ; Probleme dans FitAll car la methode WNT_Window::Size(Real,Real)
ne marche pas. Contournement en appelant WNT_Window::Size(Int,Int).
16-08-98 : CAL ; S3892. Ajout grilles 3d.
09-09-98 : CAL ; S3892. Generalisation de TrsPoint.
24-09-98 : CAL ; Ajout d'un parametre a V3d_View::SetPlotter.
06-10-98 : CAL ; Ajout d'un TIMER si CSF_GraphicTimer est definie.
16-10-98 : CAL ; Retrait d'un TIMER si CSF_GraphicTimer est definie.
06-11-98 : CAL ; PRO ?????. Probleme dans ZFitAll si un point dans la vue.
13-06-98 : FMN ; PRO14896: Correction sur la gestion de la perspective (cf Programming Guinde)
29-OCT-98 : DCB : Adding ScreenCopy () method.
22-12-98 : FMN ; Rename CSF_WALKTHROW en CSF_WALKTHROUGH
10-11-99 : GG ; PRO19603 Add Redraw( area ) method
IMP130100 : GG
-> Don't increase too much the ZSize.
-> Initialize correctly the Z clipping and D cueing
planes.
IMP100701 : SZV ; Add ToPixMap() method

REMARQUES :
-----------
About  FitAll() multiple. This probleme is caused by missing
precision of transformation matrices. If it is supposed that
projection is made in the plane (U,V), there is a difference
after several Zoom - compared to the exact value (cf ZoomX).
Don't forget that the matrices work in float and not in double.
To solve the problem (for lack of a better solution) I make 2 passes.

************************************************************************/

#define GER61351  //GG_15/12/99 Add SetBackgroundColor()
//              and BackgroundColor() methods


#define IMP240100 //GG
//      -> Remove PixToRef() method ,use
//        instead the equivalent Convert() method.
//      -> Rename RefToPix() to Convert() method.
//            -> Remove the grid computation in Convert()
//        method. Use instead the NEW ConvertToGrid() methods.
//        Reason is the Convert() method is call by
//        StdSelect_ViewSelector3d_Pick() from
//        AIS_InteractiveContext() and it's not possible
//        to select an object vertex when the grid is active!
//      -> Remove grid echo marker definition
//        (reported in the viewer)
//      -> Add SetProjModel() methods.

#define G003    //EUG 04-10-99
//      -> computed mode management
//         Add SetComputedMode(..) method
//      -> animation mode management
//         Add SetAnimationMode()
//      -> backfacing management
//         Add SetBackFacingModel() method

#define G004    //VKH 15-11-99
//      -> Add Dump() methods
//      -> GG 07/03/00 Use the new MMSize()
//         method from Aspect_Window class.

#define IMP210200       //GG Add Transparency() method

#define IMP250200 //GG With SetDepth() method, when the requested
//      depth is < 0.,
//      move the view ref point and the eye,instead
//      only the eye.

#define IMP020300 //GG Don't use ZFitAll in during Rotation
//      for perf improvment

#define IMP210600 //GG Avoid to have infinite loop when call Rotation() method
//      without call before StartRotation().
//      This problem occurs when CTRL MB3 is maintain press betwwen 2 views.

#define IMP250900 //GG Enable rotation around screen Z axis when
//      rotation begin far the center of the screen.
//      Thanks to Patrick REGINSTER (SAMTECH)
//      GG 21/12/00 Due to a regression on the previous specifications
//      this new functionnality is right now deactivated
//      by default (see StartRotation(...,zRotationThreshold)
//      method.

#define BUC60952  //GG Enable to rotate around the view axis
//      and the required view point

#define RIC120302 //GG Add a NEW SetWindow method which enable
//      to connect a graphic widget and context to OGL.

#define IMP260302 //GG To avoid conflicting in Window destructor
//      nullify this handle in Remove method

#define OCC280          //SAV fix for FitAll problem in the perspective view.

#define OCC1188         //SAV Added methods to set background image

/*----------------------------------------------------------------------*/
/*
* Includes
*/

#include <Standard_TypeMismatch.hxx>
#include <Visual3d_ViewManager.hxx>
#include <Visual3d_Light.hxx>
#include <Visual3d_ClipPlane.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_MapIteratorOfMapOfStructure.hxx>
#include <Graphic3d_MapOfStructure.hxx>
#include <Graphic3d_TextureEnv.hxx>
#include <Image_PixMap.hxx>
#include <V3d.hxx>
#include <V3d_View.ixx>
#include <Viewer_BadValue.hxx>
#include <Standard_ShortReal.hxx>
#include <gp_Dir.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <Visual3d_TransientManager.hxx>
#include <Precision.hxx>

// S3892
#include <Graphic3d_AspectMarker3d.hxx>

// S3603
#include <Aspect_GenericColorMap.hxx>
#include <Aspect_TypeMap.hxx>
#include <Aspect_WidthMap.hxx>
#include <Aspect_MarkMap.hxx>
#include <Aspect_FontMap.hxx>
#include <PlotMgt_ImageDriver.hxx>
#include <TColStd_HSequenceOfInteger.hxx>
#ifdef WNT
# include <WNT_WDriver.hxx>
#else
# include <Xw_Driver.hxx>
#endif

#ifdef G003
# define V3d_FLAG_ANIMATION     0x00000001
# define V3d_FLAG_DEGENERATION  0x00000002
# define V3d_FLAG_COMPUTATION   0x00000004
#endif  // G003

// Tumble
#include <OSD_Timer.hxx>
static OSD_Timer FullTimer;

// Perspective
#include <OSD_Environment.hxx>

/*----------------------------------------------------------------------*/
/*
* Constant
*/

#define Zmargin 1.
#define DEUXPI (2.*Standard_PI)

// in case of NO_TRACE_ECHO and NO_TRACE_POINTS, in V3d_View_4.cxx and in
// V3d_View.cxx, change MyGridEchoStructure and MyGridEchoGroup in cdl
#define NO_TRACE_ECHO
#define NO_TRACE_POINTS

/*----------------------------------------------------------------------*/
/*
* Local data definitions
*/


static Standard_Real MyXwindowCenter ;
static Standard_Real MyYwindowCenter ;
static Standard_Real MyWindowWidth ;
static Standard_Real MyWindowHeight ;

#define LOPTIM
#ifndef LOPTIM
static Graphic3d_Vector MyXscreenAxis ;
static Graphic3d_Vector MyYscreenAxis ;
static Graphic3d_Vector MyZscreenAxis ;
static Graphic3d_Vector MyViewReferencePlane ;
static Graphic3d_Vector MyViewReferenceUp ;
static Graphic3d_Vector MyViewAxis ;
static Graphic3d_Vertex MyViewReferencePoint ;
static Graphic3d_Vertex MyGravityReferencePoint ;
static Graphic3d_Vertex MyProjReferencePoint ;
#else
static Graphic3d_Vector& _MyXscreenAxis() {
  static Graphic3d_Vector MyXscreenAxis;
  return MyXscreenAxis;
}
#define MyXscreenAxis _MyXscreenAxis()

static Graphic3d_Vector& _MyYscreenAxis() {
  static Graphic3d_Vector MyYscreenAxis;
  return MyYscreenAxis;
}
#define MyYscreenAxis _MyYscreenAxis()

static Graphic3d_Vector& _MyZscreenAxis() {
  static Graphic3d_Vector MyZscreenAxis;
  return MyZscreenAxis;
}
#define MyZscreenAxis _MyZscreenAxis()

static Graphic3d_Vector& _MyViewReferencePlane() {
  static Graphic3d_Vector MyViewReferencePlane;
  return MyViewReferencePlane;
}
#define MyViewReferencePlane _MyViewReferencePlane()

static Graphic3d_Vector& _MyViewReferenceUp() {
  static Graphic3d_Vector MyViewReferenceUp;
  return MyViewReferenceUp;
}
#define MyViewReferenceUp _MyViewReferenceUp()

static Graphic3d_Vector& _MyViewAxis() {
  static Graphic3d_Vector MyViewAxis;
  return MyViewAxis;
}
#define MyViewAxis _MyViewAxis()

static Graphic3d_Vertex& _MyViewReferencePoint() {
  static Graphic3d_Vertex MyViewReferencePoint;
  return MyViewReferencePoint;
}
#define MyViewReferencePoint _MyViewReferencePoint()

static Graphic3d_Vertex& _MyGravityReferencePoint() {
  static Graphic3d_Vertex MyGravityReferencePoint;
  return MyGravityReferencePoint;
}
#define MyGravityReferencePoint _MyGravityReferencePoint()

static Graphic3d_Vertex& _MyProjReferencePoint() {
  static Graphic3d_Vertex MyProjReferencePoint;
  return MyProjReferencePoint;
}
#define MyProjReferencePoint _MyProjReferencePoint()
#endif // LOPTIM

/*----------------------------------------------------------------------*/
//-Constructors

V3d_View::V3d_View(const Handle(V3d_Viewer)& VM, const V3d_TypeOfView Type ) :
MyType ( Type ),
MyViewer(VM.operator->()),
MyActiveLights(),
MyActivePlanes(),
MyViewContext (),
myActiveLightsIterator(),
myActivePlanesIterator(),
SwitchSetFront(Standard_False),
MyTrsf (1, 4, 1, 4),                                    // S3892
MyProjModel(V3d_TPM_SCREEN)
#if defined(TRACE_POINTS)
,MyGridEchoStructure (new Graphic3d_Structure (VM->Viewer ())),  // S3892
MyGridEchoGroup (new Graphic3d_Group (MyGridEchoStructure))            // S3892
#endif
{

  myImmediateUpdate = Standard_False;
  MyView = new Visual3d_View(MyViewer->Viewer());

  // { Begin to retrieve the definition from ViewContext.
  // Step MyViewContext = MyView->Context() ;
  // to permit MyView->SetContext to compare
  // the old and the new context.
  // No problem for MyViewMapping, MyViewOrientation
  // as MyView->SetViewMapping and MyView->SetViewOrientation
  // don't try to optimize the modifications introduced to
  // viewmapping and vieworientation.

  // Aliasing
  if ((MyView->Context ()).AliasingIsOn ())
    MyViewContext.SetAliasingOn ();
  else
    MyViewContext.SetAliasingOff ();

  // DepthCueing
  MyViewContext.SetDepthCueingBackPlane
    ((MyView->Context ()).DepthCueingBackPlane ());
  MyViewContext.SetDepthCueingFrontPlane
    ((MyView->Context ()).DepthCueingFrontPlane ());

  if ((MyView->Context ()).DepthCueingIsOn ())
    MyViewContext.SetDepthCueingOn ();
  else
    MyViewContext.SetDepthCueingOff ();

  // ZClipping
  MyViewContext.SetZClippingBackPlane
    ((MyView->Context ()).ZClippingBackPlane ());
  MyViewContext.SetZClippingFrontPlane
    ((MyView->Context ()).ZClippingFrontPlane ());

  if ((MyView->Context ()).FrontZClippingIsOn ())
    MyViewContext.SetFrontZClippingOn ();
  else
    MyViewContext.SetFrontZClippingOff ();

  if ((MyView->Context ()).BackZClippingIsOn ())
    MyViewContext.SetBackZClippingOn ();
  else
    MyViewContext.SetBackZClippingOff ();

  // Visualisation and Shading Model
  MyViewContext.SetModel ((MyView->Context ()).Model ());
  MyViewContext.SetVisualization ((MyView->Context ()).Visualization ());

  // Texture Mapping
  MyViewContext.SetSurfaceDetail (MyView->Context ().SurfaceDetail ());
  MyViewContext.SetTextureEnv (MyView->Context ().TextureEnv ());
  // } End of retrieval of the definition of ViewContext.

  MyViewMapping = MyView->ViewMapping() ;
  MyViewOrientation = MyView->ViewOrientation() ;
  MyBackground = VM->GetBackgroundColor() ;
  MyGradientBackground = VM->GetGradientBackground() ;

  SetAxis(0.,0.,0.,1.,1.,1.) ;
  SetVisualization(VM->DefaultVisualization()) ;
  SetShadingModel(VM->DefaultShadingModel()) ;
  SetSurfaceDetail(VM->DefaultSurfaceDetail()) ;
  SetTwist(0.) ;
  SetAt(0.,0.,0.) ;
  SetProj(VM->DefaultViewProj()) ;
  SetSize(VM->DefaultViewSize()) ;
  Standard_Real zsize = VM->DefaultViewSize();
  SetZSize(2.*zsize+zsize*Zmargin) ;
  SetZClippingDepth(0.);
  SetZClippingWidth(zsize);
  SetZCueingDepth(0.);
  SetZCueingWidth(zsize);
  SetDepth(VM->DefaultViewSize()/2.) ;
  SetCenter(0.,0.) ;
  SetViewMappingDefault();
  VM->AddView(this) ;
  Init();
  myImmediateUpdate = Standard_True;

  // S3892
#ifndef IMP240100
#if defined(TRACE_POINTS)
  Handle(Graphic3d_AspectMarker3d) MarkerAttrib = new Graphic3d_AspectMarker3d ();
  MarkerAttrib->SetColor (Quantity_Color (Quantity_NOC_GRAY90));
  MarkerAttrib->SetScale (3.0);
  MarkerAttrib->SetType (Aspect_TOM_STAR);
  MyGridEchoStructure->SetPrimitivesAspect (MarkerAttrib);
#endif
#endif  //IMP240100

#ifdef G003
  MyAnimationFlags = 0;
#endif  // G003

#ifdef IMP210200
  MyTransparencyFlag = Standard_False;
#endif

}

/*----------------------------------------------------------------------*/

V3d_View::V3d_View(const Handle(V3d_Viewer)& VM,const Handle(V3d_View)& V, const V3d_TypeOfView Type ) :
MyType ( Type ),
MyViewer(VM.operator->()),
MyActiveLights(),
MyActivePlanes(),
MyViewContext (),
myActiveLightsIterator(),
myActivePlanesIterator(),
SwitchSetFront(Standard_False),
MyTrsf (1, 4, 1, 4),                                    // S3892
MyProjModel(V3d_TPM_SCREEN)
#if defined(TRACE_POINTS)
,MyGridEchoStructure (new Graphic3d_Structure (VM->Viewer ())),  // S3892
MyGridEchoGroup (new Graphic3d_Group (MyGridEchoStructure))      // S3892
#endif
{

  Handle(Visual3d_View) FromView = V->View() ;

  myImmediateUpdate = Standard_False;
  MyView = new Visual3d_View(MyViewer->Viewer());

  for (V->InitActiveLights();V->MoreActiveLights();V->NextActiveLights()){
    MyActiveLights.Append(V->ActiveLight());}
  for (V->InitActivePlanes();V->MoreActivePlanes();V->NextActivePlanes()){
    MyActivePlanes.Append(V->ActivePlane());}

  MyViewContext = FromView->Context() ;

  MyViewMapping = FromView->ViewMapping() ;
  MyViewOrientation = FromView->ViewOrientation() ;
  MyBackground = FromView->Background() ;
  MyGradientBackground = FromView->GradientBackground();

  MyView->SetContext(MyViewContext) ;

  SetAxis(0.,0.,0.,1.,1.,1.) ;
  VM->AddView(this) ;
  Init();
  myImmediateUpdate = Standard_True;

  // S3892
#ifndef IMP240100
#if defined(TRACE_ECHO)
  Handle(Graphic3d_AspectMarker3d) MarkerAttrib = new Graphic3d_AspectMarker3d ();
  MarkerAttrib->SetColor (Quantity_Color (Quantity_NOC_GRAY90));
  MarkerAttrib->SetScale (3.0);
  MarkerAttrib->SetType (Aspect_TOM_STAR);
  MyGridEchoStructure->SetPrimitivesAspect (MarkerAttrib);
#endif
#endif  //IMP240100

#ifdef G003
  MyAnimationFlags = 0;
#endif

}

/*----------------------------------------------------------------------*/

//-Methods, in order

void V3d_View::SetMagnify(const Handle(Aspect_Window)& TheWindow,
                          const Handle(V3d_View)& aPreviousView,
                          const Standard_Integer x1,
                          const Standard_Integer y1,
                          const Standard_Integer x2,
                          const Standard_Integer y2)
{

  if( !MyView->IsDefined() ) {
    Standard_Real a,b,c,d;
    aPreviousView->Convert(x1,y1,a,b);
    aPreviousView->Convert(x2,y2,c,d);
    MyView->SetWindow(TheWindow) ;
    FitAll(TheWindow,a,b,c,d);
    MyView->SetContext(MyViewContext) ;
    MyView->SetViewOrientation(MyViewOrientation) ;
    MyView->SetBackground(MyBackground) ;
    MyViewer->SetViewOn(this) ;
    MyWindow = TheWindow;
    MyView->Redraw() ;
    SetViewMappingDefault();
  }

}

/*----------------------------------------------------------------------*/

void V3d_View::SetWindow(const Handle(Aspect_Window)& TheWindow)
{

  Standard_MultiplyDefined_Raise_if( MyView->IsDefined(),
    "V3d_View::SetWindow, window of view already defined");

  MyView->SetWindow(TheWindow) ;
  // SetWindow carries out SetRatio and modifies
  // ViewMapping and ViewMappingDefault of MyView.
  MyViewMapping = MyView->ViewMapping() ;
  MyView->SetContext(MyViewContext) ;
  MyView->SetViewMapping(MyViewMapping) ;
  MyView->SetViewOrientation(MyViewOrientation) ;
  MyView->SetBackground(MyBackground) ;
  MyViewer->SetViewOn(this) ;
#ifdef TRACE_POINTS
  MyGridEchoStructure->SetInfiniteState (Standard_True);        // S3892
  MyGridEchoStructure->Display ();                              // S3892
#endif
  MyView->Redraw() ;
  MyWindow = TheWindow;

}

// RIC120302
/*----------------------------------------------------------------------*/

void V3d_View::SetWindow(const Handle(Aspect_Window)&      aWindow,
                         const Aspect_RenderingContext     aContext,
                         const Aspect_GraphicCallbackProc& aDisplayCB,
                         const Standard_Address            aClientData)
{

  Standard_MultiplyDefined_Raise_if( MyView->IsDefined(),
    "V3d_View::SetWindow, "
    "window of view already defined");
  MyView->SetWindow(aWindow, aContext, aDisplayCB, aClientData) ;
  MyViewMapping = MyView->ViewMapping() ;
  MyView->SetContext(MyViewContext) ;
  MyView->SetViewMapping(MyViewMapping) ;
  MyView->SetViewOrientation(MyViewOrientation) ;
  MyView->SetBackground(MyBackground) ;
  MyViewer->SetViewOn(this) ;
  MyView->Redraw() ;
  MyWindow = aWindow;

}
// RIC120302

/*----------------------------------------------------------------------*/

void V3d_View::Remove() const  {

  MyViewer->DelView(this) ;
  MyView->Remove() ;
#ifdef IMP260303
  MyWindow.Nullify();
#endif

}

/*----------------------------------------------------------------------*/

void V3d_View::Update()  const {
  if( MyView->IsDefined() )  MyView->Update() ;
}

/*----------------------------------------------------------------------*/

void V3d_View::Redraw() const {
  if( MyView->IsDefined() ) MyView->Redraw() ;
}
/*----------------------------------------------------------------------*/

void V3d_View::Redraw(const Standard_Integer xc,const Standard_Integer yc,
                      const Standard_Integer width,const Standard_Integer height) const
{
  if( MyView->IsDefined() ) MyView->Redraw(xc,yc,width,height) ;
}

/*----------------------------------------------------------------------*/

Standard_Boolean V3d_View::IsEmpty() const {

  Standard_Boolean TheStatus = Standard_True ;
  if( MyView->IsDefined() ) {
    Standard_Integer Nstruct = MyView->NumberOfDisplayedStructures() ;
    if( Nstruct > 0 ) TheStatus = Standard_False ;
  }
  return (TheStatus) ;

}

/*----------------------------------------------------------------------*/

void V3d_View::UpdateLights() const {
  MyView->SetContext(MyViewContext);
  Update();
}

/*----------------------------------------------------------------------*/

void V3d_View::DoMapping() {
  if( MyView->IsDefined() ) {
    (MyView->Window())->DoMapping() ;
  }
}

/*----------------------------------------------------------------------*/

void V3d_View::MustBeResized() {

  if ( !MyLayerMgr.IsNull() )
    MyLayerMgr->Resized();

  if( MyView->IsDefined() ) {
    MyView->Resized() ;
    MyViewMapping = MyView->ViewMapping();
    MyView->Redraw();
  }

}

/*----------------------------------------------------------------------*/

void V3d_View::SetBackgroundColor(const Quantity_TypeOfColor Type, const Standard_Real v1, const Standard_Real v2, const Standard_Real v3)
{
  Standard_Real V1 = Max( Min( v1, 1.0 ), 0.0 );
  Standard_Real V2 = Max( Min( v2, 1.0 ), 0.0 );
  Standard_Real V3 = Max( Min( v3, 1.0 ), 0.0 );

  Quantity_Color C( V1, V2, V3, Type );
#ifdef GER61351
  SetBackgroundColor( C );
#else
  MyBackground.SetColor( C );
  if ( MyView->IsDefined() )
    MyView->SetBackground( MyBackground );
  if ( !MyLayerMgr.IsNull() )
    MyLayerMgr->Resized();
#endif
}

#ifdef GER61351
/*----------------------------------------------------------------------*/

void V3d_View::SetBackgroundColor(const Quantity_Color &Color)
{
  MyBackground.SetColor( Color );
  if ( MyView->IsDefined() )
    MyView->SetBackground( MyBackground );
  //szv: Why?
  if ( !MyLayerMgr.IsNull() )
    MyLayerMgr->Resized();
}
#endif

/*----------------------------------------------------------------------*/

void V3d_View::SetBackgroundColor(const Quantity_NameOfColor Name)
{
  Quantity_Color C( Name );
#ifdef GER61351
  SetBackgroundColor( C );
#else
  MyBackground.SetColor( C );
  if ( MyView->IsDefined() )
    MyView->SetBackground( MyBackground );
  if ( !MyColorScale.IsNull() )
    MyColorScale->Resized();
#endif
}

/*----------------------------------------------------------------------*/

void V3d_View::SetBgGradientColors( const Quantity_Color& Color1,
                                    const Quantity_Color& Color2,
                                    const Aspect_GradientFillMethod FillStyle,
                                    const Standard_Boolean status)
{

  MyGradientBackground.SetColors(Color1, Color2, FillStyle);
  if ( MyView->IsDefined() )
    MyView->SetGradientBackground( MyGradientBackground, status );

}

/*----------------------------------------------------------------------*/

void V3d_View::SetBgGradientColors( const Quantity_NameOfColor Color1,
                                    const Quantity_NameOfColor Color2,
                                    const Aspect_GradientFillMethod FillStyle,
                                    const Standard_Boolean status )
{
  Quantity_Color C1( Color1 );
  Quantity_Color C2( Color2 );
  MyGradientBackground.SetColors( C1, C2, FillStyle );
  if ( MyView->IsDefined() )
    MyView->SetGradientBackground( MyGradientBackground, status );
}

/*----------------------------------------------------------------------*/

void V3d_View::SetBgGradientStyle( const Aspect_GradientFillMethod FillStyle,
                                   const Standard_Boolean update) const
{
  if( MyView->IsDefined() )
    MyView->SetBgGradientStyle( FillStyle , update ) ;
}

/*----------------------------------------------------------------------*/

void V3d_View::SetBackgroundImage( const Standard_CString FileName,
                                   const Aspect_FillMethod FillStyle,
                                   const Standard_Boolean update )
{
#ifdef OCC1188
  if( MyView->IsDefined() )
    MyView->SetBackgroundImage( FileName, FillStyle, update ) ;
#endif
}

/*----------------------------------------------------------------------*/

void V3d_View::SetBgImageStyle( const Aspect_FillMethod FillStyle,
                                const Standard_Boolean update )
{
#ifdef OCC1188
  if( MyView->IsDefined() )
    MyView->SetBgImageStyle( FillStyle, update ) ;
#endif
}

/*----------------------------------------------------------------------*/

void V3d_View::SetAxis(const Standard_Real X, const Standard_Real Y, const Standard_Real Z, const Standard_Real Vx, const Standard_Real Vy, const Standard_Real Vz) {

  Standard_Real D,Nx = Vx,Ny = Vy,Nz = Vz ;

  D = Sqrt( Vx*Vx + Vy*Vy + Vz*Vz ) ;
  Viewer_BadValue_Raise_if ( D <= 0. , "V3d_View::SetAxis, bad axis");
  Nx /= D ; Ny /= D ; Nz /= D ;
  MyDefaultViewPoint.SetCoord(X,Y,Z) ;
  MyDefaultViewAxis.SetCoord(Nx,Ny,Nz) ;
  MyDefaultViewAxis.Normalize() ;

}

/*----------------------------------------------------------------------*/

void V3d_View::SetShadingModel(const V3d_TypeOfShadingModel Model) {

  MyViewContext.SetModel((Visual3d_TypeOfModel) Model) ;
  MyView->SetContext(MyViewContext) ;

}

/*----------------------------------------------------------------------*/

void V3d_View::SetSurfaceDetail(const V3d_TypeOfSurfaceDetail Model) {

  MyViewContext.SetSurfaceDetail((Visual3d_TypeOfSurfaceDetail) Model) ;
  MyView->SetContext(MyViewContext) ;

}

/*----------------------------------------------------------------------*/

void V3d_View::SetTextureEnv(const Handle(Graphic3d_TextureEnv)& ATexture) {

  MyViewContext.SetTextureEnv(ATexture) ;
  MyView->SetContext(MyViewContext) ;

}

/*----------------------------------------------------------------------*/

void V3d_View::SetVisualization(const V3d_TypeOfVisualization Mode) {

  MyViewContext.SetVisualization((Visual3d_TypeOfVisualization) Mode);
  MyView->SetContext(MyViewContext) ;

}

/*----------------------------------------------------------------------*/

void V3d_View::SetFront() {

  gp_Ax3 a = MyViewer->PrivilegedPlane();
  Standard_Real xo,yo,zo,vx,vy,vz,xu,yu,zu;

  a.Direction().Coord(vx,vy,vz);
  a.YDirection().Coord(xu,yu,zu);
  a.Location().Coord(xo,yo,zo);

  if(SwitchSetFront)
    MyViewOrientation.SetViewReferencePlane(Graphic3d_Vector(-vx,-vy,-vz));
  else
    MyViewOrientation.SetViewReferencePlane(Graphic3d_Vector(vx,vy,vz));

  SwitchSetFront = !SwitchSetFront;

  MyViewOrientation.SetViewReferenceUp(Graphic3d_Vector(xu,yu,zu));
  MyViewOrientation.SetViewReferencePoint(Graphic3d_Vertex(xo,yo,zo));

  MyView->SetViewOrientation(MyViewOrientation) ;

  ImmediateUpdate();

}

/*----------------------------------------------------------------------*/

void V3d_View::Rotate(const Standard_Real ax, const Standard_Real ay, const Standard_Real az, const Standard_Boolean Start) {

  Standard_Real Ax = ax ;
  Standard_Real Ay = ay ;
  Standard_Real Az = az ;
  Graphic3d_Vector Vpn,Vup ;
  TColStd_Array2OfReal Matrix(0,3,0,3) ;
  TColStd_Array2OfReal Rmatrix(0,3,0,3) ;

  if( Ax > 0. ) while ( Ax > DEUXPI ) Ax -= DEUXPI ;
  else if( Ax < 0. ) while ( Ax < -DEUXPI ) Ax += DEUXPI ;
  if( Ay > 0. ) while ( Ay > DEUXPI ) Ay -= DEUXPI ;
  else if( Ay < 0. ) while ( Ay < -DEUXPI ) Ay += DEUXPI ;
  if( Az > 0. ) while ( Az > DEUXPI ) Az -= DEUXPI ;
  else if( Az < 0. ) while ( Az < -DEUXPI ) Az += DEUXPI ;

  if( Start ) {
    MyViewReferencePoint = MyViewOrientation.ViewReferencePoint() ;
    MyViewReferencePlane = MyViewOrientation.ViewReferencePlane() ;
    MyViewReferenceUp = MyViewOrientation.ViewReferenceUp() ;
    if (!ScreenAxis(MyViewReferencePlane,MyViewReferenceUp,
      MyXscreenAxis,MyYscreenAxis,MyZscreenAxis))
      Viewer_BadValue::Raise ("V3d_View::Rotate, alignment of Eye,At,Up");
  }

  InitMatrix(Matrix) ;

  if( Ax != 0. ) RotAxis(MyViewReferencePoint,MyYscreenAxis,-Ax,Matrix);
  if( Ay != 0. ) {
    RotAxis(MyViewReferencePoint,MyXscreenAxis,Ay,Rmatrix) ;
    Multiply(Matrix, Rmatrix, Matrix);
  }
  if( Az != 0. ) {
    RotAxis(MyViewReferencePoint,MyZscreenAxis,Az,Rmatrix) ;
    Multiply(Matrix, Rmatrix, Matrix);
  }
  Vpn = TrsPoint(MyViewReferencePlane,Matrix) ;
  MyViewOrientation.SetViewReferencePlane(Vpn) ;
  Vup = TrsPoint(MyViewReferenceUp,Matrix) ;
  MyViewOrientation.SetViewReferenceUp(Vup) ;
  MyView->SetViewOrientation(MyViewOrientation) ;
#ifdef IMP020300
  SetZSize(0.) ;
#endif
  ImmediateUpdate();

}

/*----------------------------------------------------------------------*/

void V3d_View::Rotate(const Standard_Real ax, const Standard_Real ay, const Standard_Real az,
                      const Standard_Real X, const Standard_Real Y, const Standard_Real Z, const Standard_Boolean Start)
{

  Standard_Real Ax = ax ;
  Standard_Real Ay = ay ;
  Standard_Real Az = az ;
  Graphic3d_Vector Vpn,Vup ;
  Graphic3d_Vertex Vrp ;
  TColStd_Array2OfReal Matrix(0,3,0,3) ;
  TColStd_Array2OfReal Rmatrix(0,3,0,3) ;

  if( Ax > 0. ) while ( Ax > DEUXPI ) Ax -= DEUXPI ;
  else if( Ax < 0. ) while ( Ax < -DEUXPI ) Ax += DEUXPI ;
  if( Ay > 0. ) while ( Ay > DEUXPI ) Ay -= DEUXPI ;
  else if( Ay < 0. ) while ( Ay < -DEUXPI ) Ay += DEUXPI ;
  if( Az > 0. ) while ( Az > DEUXPI ) Az -= DEUXPI ;
  else if( Az < 0. ) while ( Az < -DEUXPI ) Az += DEUXPI ;

  if( Start ) {
    MyGravityReferencePoint.SetCoord(X,Y,Z) ;
    MyViewReferencePoint = MyViewOrientation.ViewReferencePoint() ;
    MyViewReferencePlane = MyViewOrientation.ViewReferencePlane() ;
    MyViewReferenceUp = MyViewOrientation.ViewReferenceUp() ;
    if (!ScreenAxis(MyViewReferencePlane,MyViewReferenceUp,
      MyXscreenAxis,MyYscreenAxis,MyZscreenAxis))
      Viewer_BadValue::Raise ("V3d_View::Rotate, alignment of Eye,At,Up");
  }

  InitMatrix(Matrix) ;

  if( Ax != 0. ) RotAxis(MyGravityReferencePoint,MyYscreenAxis,-Ax,Matrix);
  if( Ay != 0. ) {
    RotAxis(MyGravityReferencePoint,MyXscreenAxis,Ay,Rmatrix) ;
    Multiply(Matrix, Rmatrix, Matrix);
  }
  if( Az != 0. ) {
    RotAxis(MyGravityReferencePoint,MyZscreenAxis,Az,Rmatrix) ;
    Multiply(Matrix, Rmatrix, Matrix);
  }
  Vrp = TrsPoint(MyViewReferencePoint,Matrix) ;
  MyViewOrientation.SetViewReferencePoint(Vrp) ;
  Vpn = TrsPoint(MyViewReferencePlane,Matrix) ;
  MyViewOrientation.SetViewReferencePlane(Vpn) ;
  Vup = TrsPoint(MyViewReferenceUp,Matrix) ;
  MyViewOrientation.SetViewReferenceUp(Vup) ;
  MyView->SetViewOrientation(MyViewOrientation) ;
#ifdef IMP020300
  SetZSize(0.) ;
#endif
  ImmediateUpdate();

}

/*----------------------------------------------------------------------*/

void V3d_View::Rotate(const V3d_TypeOfAxe Axe, const Standard_Real angle, const Standard_Boolean Start) {

  switch (Axe) {
  case V3d_X :
    Rotate(angle,0.,0.,Start);
    break ;
  case V3d_Y :
    Rotate(0.,angle,0.,Start);
    break ;
  case V3d_Z :
    Rotate(0.,0.,angle,Start);
    break ;
  }

}

/*----------------------------------------------------------------------*/

void V3d_View::Rotate(const V3d_TypeOfAxe Axe, const Standard_Real angle,
                      const Standard_Real X, const Standard_Real Y, const Standard_Real Z, const Standard_Boolean Start)
{

  Standard_Real Angle = angle ;
  Graphic3d_Vector Vpn,Vup ;
  Graphic3d_Vertex Vrp ;
  TColStd_Array2OfReal Matrix(0,3,0,3) ;

  if( Angle > 0. ) while ( Angle > DEUXPI ) Angle -= DEUXPI ;
  else if( Angle < 0. ) while ( Angle < -DEUXPI ) Angle += DEUXPI ;

  if( Start ) {
    MyGravityReferencePoint.SetCoord(X,Y,Z) ;
    MyViewReferencePoint = MyViewOrientation.ViewReferencePoint() ;
    MyViewReferencePlane = MyViewOrientation.ViewReferencePlane() ;
    MyViewReferenceUp = MyViewOrientation.ViewReferenceUp() ;
    switch (Axe) {
    case V3d_X :
      MyViewAxis.SetCoord(1.,0.,0.) ;
      break ;
    case V3d_Y :
      MyViewAxis.SetCoord(0.,1.,0.) ;
      break ;
    case V3d_Z :
      MyViewAxis.SetCoord(0.,0.,1.) ;
      break ;
    }
  }

  RotAxis(MyGravityReferencePoint,MyViewAxis,Angle,Matrix) ;
  Vrp = TrsPoint(MyViewReferencePoint,Matrix) ;
  MyViewOrientation.SetViewReferencePoint(Vrp) ;
  Vpn = TrsPoint(MyViewReferencePlane,Matrix) ;
  MyViewOrientation.SetViewReferencePlane(Vpn) ;
  Vup = TrsPoint(MyViewReferenceUp,Matrix) ;
  MyViewOrientation.SetViewReferenceUp(Vup) ;
  MyView->SetViewOrientation(MyViewOrientation) ;
#ifdef IMP020300
  SetZSize(0.) ;
#endif
  ImmediateUpdate();

}

/*----------------------------------------------------------------------*/

void V3d_View::Rotate(const Standard_Real angle, const Standard_Boolean Start) {

  Standard_Real Angle = angle ;
  Graphic3d_Vector Vpn,Vup ;
  TColStd_Array2OfReal Matrix(0,3,0,3) ;

  if( Angle > 0. ) while ( Angle > DEUXPI ) Angle -= DEUXPI ;
  else if( Angle < 0. ) while ( Angle < -DEUXPI ) Angle += DEUXPI ;

  if( Start ) {
    MyViewReferencePoint = MyViewOrientation.ViewReferencePoint() ;
    MyViewReferencePlane = MyViewOrientation.ViewReferencePlane() ;
    MyViewReferenceUp = MyViewOrientation.ViewReferenceUp() ;
  }

  RotAxis(MyDefaultViewPoint,MyDefaultViewAxis,Angle,Matrix) ;
#ifdef BUC60952
  Graphic3d_Vertex Vrp ;
  Vrp = TrsPoint(MyViewReferencePoint,Matrix) ;
  MyViewOrientation.SetViewReferencePoint(Vrp) ;
#endif
  Vpn = TrsPoint(MyViewReferencePlane,Matrix) ;
  MyViewOrientation.SetViewReferencePlane(Vpn) ;
  Vup = TrsPoint(MyViewReferenceUp,Matrix) ;
  MyViewOrientation.SetViewReferenceUp(Vup) ;
  MyView->SetViewOrientation(MyViewOrientation) ;
#ifdef IMP020300
  SetZSize(0.) ;
#endif
  ImmediateUpdate();

}

/*----------------------------------------------------------------------*/

void V3d_View::Turn(const Standard_Real ax, const Standard_Real ay, const Standard_Real az, const Standard_Boolean Start)
{

  Standard_Real Ax = ax ;
  Standard_Real Ay = ay ;
  Standard_Real Az = az ;
  Graphic3d_Vertex Vrp,Eye ;
  Graphic3d_Vector Vpn,Vup ;
  Standard_Real Xrp,Yrp,Zrp,Xpn,Ypn,Zpn,Xat,Yat,Zat,Xeye,Yeye,Zeye ;
  TColStd_Array2OfReal Matrix(0,3,0,3) ;
  TColStd_Array2OfReal Rmatrix(0,3,0,3) ;

  if( Ax > 0. ) while ( Ax > DEUXPI ) Ax -= DEUXPI ;
  else if( Ax < 0. ) while ( Ax < -DEUXPI ) Ax += DEUXPI ;
  if( Ay > 0. ) while ( Ay > DEUXPI ) Ay -= DEUXPI ;
  else if( Ay < 0. ) while ( Ay < -DEUXPI ) Ay += DEUXPI ;
  if( Az > 0. ) while ( Az > DEUXPI ) Az -= DEUXPI ;
  else if( Az < 0. ) while ( Az < -DEUXPI ) Az += DEUXPI ;

  if( Start ) {
    MyProjReferencePoint = MyViewMapping.ProjectionReferencePoint();
    MyViewReferencePoint = MyViewOrientation.ViewReferencePoint() ;
    MyViewReferencePlane = MyViewOrientation.ViewReferencePlane() ;
    MyViewReferenceUp = MyViewOrientation.ViewReferenceUp() ;
    if (!ScreenAxis(MyViewReferencePlane,MyViewReferenceUp,
      MyXscreenAxis,MyYscreenAxis,MyZscreenAxis))
      Viewer_BadValue::Raise ("V3d_View::Turn, alignment of Eye,At,Up");
  }

  InitMatrix(Matrix) ;
  MyProjReferencePoint.Coord(Xrp,Yrp,Zrp) ;
  MyViewReferencePoint.Coord(Xat,Yat,Zat) ;
  MyViewReferencePlane.Coord(Xpn,Ypn,Zpn) ;
  Xeye = Zrp*Xpn + Xat ; Yeye = Zrp*Ypn + Yat ; Zeye = Zrp*Zpn + Zat ;
  Eye.SetCoord(Xeye,Yeye,Zeye) ;
  if( Ax != 0. ) RotAxis(Eye,MyYscreenAxis,-Ax,Matrix) ;
  if( Ay != 0. ) {
    RotAxis(Eye,MyXscreenAxis,Ay,Rmatrix) ;
    Multiply(Matrix, Rmatrix, Matrix);
  }
  if( Az != 0. ) {
    RotAxis(Eye,MyZscreenAxis,Az,Rmatrix) ;
    Multiply(Matrix, Rmatrix, Matrix);
  }
  Vrp = TrsPoint(MyViewReferencePoint,Matrix) ;
  MyViewOrientation.SetViewReferencePoint(Vrp) ;
  Vpn = TrsPoint(MyViewReferencePlane,Matrix) ;
  MyViewOrientation.SetViewReferenceUp(Vpn) ;
  Vup = TrsPoint(MyViewReferenceUp,Matrix) ;
  MyViewOrientation.SetViewReferenceUp(Vup) ;
  MyView->SetViewOrientation(MyViewOrientation) ;
#ifdef IMP020300
  SetZSize(0.) ;
#else
  // Check ZClipping planes
  Standard_Real Zmax ;
  Vrp.Coord(Xat,Yat,Zat) ;
  Zmax = Sqrt( Xat*Xat + Yat*Yat + Zat*Zat) ;
  if( Zmax > MyViewMapping.FrontPlaneDistance() &&
    MyProjModel == V3d_TPM_SCREEN ) {
      SetZSize(2.*Zmax+Zmax*Zmargin) ;
    }
#endif
    ImmediateUpdate();

}

/*----------------------------------------------------------------------*/

void V3d_View::Turn(const V3d_TypeOfAxe Axe, const Standard_Real angle, const Standard_Boolean Start)
{

  switch (Axe) {
  case V3d_X :
    Turn(angle,0.,0.,Start);
    break ;
  case V3d_Y :
    Turn(0.,angle,0.,Start);
    break ;
  case V3d_Z :
    Turn(0.,0.,angle,Start);
    break ;
  }
}

void V3d_View::Turn(const Standard_Real angle, const Standard_Boolean Start) {
  Standard_Real Angle = angle ;
  Graphic3d_Vertex Vrp,Eye ;
  Graphic3d_Vector Vpn,Vup ;
  Standard_Real Xrp,Yrp,Zrp,Xpn,Ypn,Zpn,Xat,Yat,Zat,Xeye,Yeye,Zeye ;
  TColStd_Array2OfReal Matrix(0,3,0,3) ;

  if( Angle > 0. ) while ( Angle > DEUXPI ) Angle -= DEUXPI ;
  else if( Angle < 0. ) while ( Angle < -DEUXPI ) Angle += DEUXPI ;

  if( Start ) {
    MyProjReferencePoint = MyViewMapping.ProjectionReferencePoint();
    MyViewReferencePoint = MyViewOrientation.ViewReferencePoint() ;
    MyViewReferencePlane = MyViewOrientation.ViewReferencePlane() ;
    MyViewReferenceUp = MyViewOrientation.ViewReferenceUp() ;
  }
  MyProjReferencePoint.Coord(Xrp,Yrp,Zrp) ;
  MyViewReferencePoint.Coord(Xat,Yat,Zat) ;
  MyViewReferencePlane.Coord(Xpn,Ypn,Zpn) ;
  Xeye = Zrp*Xpn + Xat ; Yeye = Zrp*Ypn + Yat ; Zeye = Zrp*Zpn + Zat ;
  Eye.SetCoord(Xeye,Yeye,Zeye) ;
  RotAxis(Eye,MyDefaultViewAxis,Angle,Matrix) ;
  Vrp = TrsPoint(MyViewReferencePoint,Matrix) ;
  MyViewOrientation.SetViewReferencePoint(Vrp) ;
  Vpn = TrsPoint(MyViewReferencePlane,Matrix) ;
  MyViewOrientation.SetViewReferencePlane(Vpn) ;
  Vup = TrsPoint(MyViewReferenceUp,Matrix) ;
  MyViewOrientation.SetViewReferenceUp(Vup) ;
  MyView->SetViewOrientation(MyViewOrientation) ;
#ifdef IMP020300
  SetZSize(0.) ;
#else
  // Check ZClipping planes
  Standard_Real Zmax ;
  Vrp.Coord(Xat,Yat,Zat) ;
  Zmax = Sqrt( Xat*Xat + Yat*Yat + Zat*Zat ) ;
  if( Zmax > MyViewMapping.FrontPlaneDistance() &&
    MyProjModel == V3d_TPM_SCREEN ) {
      SetZSize(2.*Zmax+Zmax*Zmargin) ;
    }
#endif
    ImmediateUpdate();

}

void V3d_View::SetTwist(const Standard_Real angle) {

  Standard_Real Angle = angle ;
  TColStd_Array2OfReal Matrix(0,3,0,3) ;
  Standard_Boolean TheStatus ;

  if( Angle > 0. ) while ( Angle > DEUXPI ) Angle -= DEUXPI ;
  else if( Angle < 0. ) while ( Angle < -DEUXPI ) Angle += DEUXPI ;

  MyViewReferencePlane = MyViewOrientation.ViewReferencePlane() ;
  MyViewReferenceUp.SetCoord(0.,0.,1.) ;
  TheStatus = ScreenAxis(MyViewReferencePlane,MyViewReferenceUp,
    MyXscreenAxis,MyYscreenAxis,MyZscreenAxis) ;
  if( !TheStatus ) {
    MyViewReferenceUp.SetCoord(0.,1.,0.) ;
    TheStatus = ScreenAxis(MyViewReferencePlane,MyViewReferenceUp,
      MyXscreenAxis,MyYscreenAxis,MyZscreenAxis) ;
  }
  if( !TheStatus ) {
    MyViewReferenceUp.SetCoord(1.,0.,0.) ;
    TheStatus = ScreenAxis(MyViewReferencePlane,MyViewReferenceUp,
      MyXscreenAxis,MyYscreenAxis,MyZscreenAxis) ;
  }

  Viewer_BadValue_Raise_if( !TheStatus,"V3d_ViewSetTwist, alignment of Eye,At,Up,");

  MyViewReferencePoint = MyViewOrientation.ViewReferencePoint() ;
  RotAxis(MyViewReferencePoint,MyZscreenAxis,Angle,Matrix) ;
  MyViewReferenceUp = TrsPoint(MyYscreenAxis,Matrix) ;
  MyViewOrientation.SetViewReferenceUp(MyViewReferenceUp) ;
  MyView->SetViewOrientation(MyViewOrientation) ;
  ImmediateUpdate();

}

#ifdef IMP240100
void V3d_View::SetProjModel( const V3d_TypeOfProjectionModel aModel )
{

  MyProjModel = aModel;

}

V3d_TypeOfProjectionModel V3d_View::ProjModel() const {

  return MyProjModel;

}
#endif

void V3d_View::SetEye(const Standard_Real X,const Standard_Real Y,const Standard_Real Z) {

  Standard_Real Angle,Xat,Yat,Zat,Xrp,Yrp,Zrp,Xpn,Ypn,Zpn ;
  Angle = Twist() ;
  MyProjReferencePoint = MyViewMapping.ProjectionReferencePoint() ;
  MyProjReferencePoint.Coord(Xrp,Yrp,Zrp) ;
  MyViewReferencePoint = MyViewOrientation.ViewReferencePoint() ;
  MyViewReferencePoint.Coord(Xat,Yat,Zat) ;
  MyViewReferenceUp = MyViewOrientation.ViewReferenceUp() ;
  Xpn = X - Xat ; Ypn = Y - Yat ; Zpn = Z - Zat ;
  Zrp = Sqrt(Xpn*Xpn + Ypn*Ypn + Zpn*Zpn) ;
  Viewer_BadValue_Raise_if( Zrp <= 0. , "V3d_View::SetEye:: Eye,At are Confused");

  Xpn /= Zrp ; Ypn /= Zrp ; Zpn /= Zrp ;
  MyViewReferencePlane.SetCoord(Xpn,Ypn,Zpn) ;
  MyViewOrientation.SetViewReferencePlane(MyViewReferencePlane) ;
  MyProjReferencePoint.SetCoord(Xrp,Yrp,Zrp) ;
  MyViewMapping.SetProjectionReferencePoint(MyProjReferencePoint);

  Standard_Boolean update = myImmediateUpdate;
  myImmediateUpdate = Standard_False;
  if( MyProjModel == V3d_TPM_WALKTHROUGH ) {
    //FMN desactivation temp SetTwist(Angle) ;
    // Set infos.
    MyView->SetViewOrientation(MyViewOrientation) ;
    MyView->SetViewMapping(MyViewMapping) ;
  } else {
    SetTwist(Angle) ;
  }
#ifdef IMP020300
  SetZSize(0.) ;
#else
  // Check ZClipping planes
  Standard_Real Zmax = Sqrt( X*X + Y*Y + Z*Z ) ;
  if( Zmax > MyViewMapping.FrontPlaneDistance() &&
    MyProjModel == V3d_TPM_SCREEN ) {
      SetZSize(2.*Zmax+Zmax*Zmargin) ;
    } else {
      if( MyType == V3d_PERSPECTIVE ) SetFocale(focale) ;
      MyView->SetViewMapping(MyViewMapping);
    }
#endif
    myImmediateUpdate = update;
    ImmediateUpdate();

}

void V3d_View::SetDepth(const Standard_Real Depth) {

  Standard_Real Xrp,Yrp,Zrp ;
#ifdef IMP250200
  Viewer_BadValue_Raise_if( Depth == 0. ,"V3d_View::SetDepth, bad depth");
#else
  Viewer_BadValue_Raise_if( Depth <= 0. ,"V3d_View::SetDepth, bad depth");
#endif

#ifdef DEB
  Standard_Real twist =
#endif
    Twist();
  MyViewReferencePoint = MyViewOrientation.ViewReferencePoint() ;
  MyViewReferencePlane = MyViewOrientation.ViewReferencePlane() ;
  MyProjReferencePoint = MyViewMapping.ProjectionReferencePoint() ;
  MyProjReferencePoint.Coord(Xrp,Yrp,Zrp) ;
#ifdef IMP250200
  if( Depth > 0. )
#endif
  {
    Zrp = Depth;
    MyProjReferencePoint.SetCoord(Xrp,Yrp,Zrp) ;
    MyViewMapping.SetProjectionReferencePoint(MyProjReferencePoint);
  }

  // Check ZClipping planes
  Standard_Real Xat,Yat,Zat,Xpn,Ypn,Zpn,Xeye,Yeye,Zeye ;
  MyViewReferencePoint.Coord(Xat,Yat,Zat) ;
  MyViewReferencePlane.Coord(Xpn,Ypn,Zpn) ;
#ifdef IMP250200
  if( Depth < 0. ) {  // Move the view ref point instead of the eye.
    Xeye = Xpn*Zrp + Xat ; Yeye = Ypn*Zrp + Yat ; Zeye = Zpn*Zrp + Zat ;
    Zrp = Abs(Depth) ;
    Xat = Xeye - Xpn*Zrp ; Yat = Yeye - Ypn*Zrp ; Zat = Zeye - Zpn*Zrp;
    MyViewReferencePoint.SetCoord(Xat,Yat,Zat) ;
    MyProjReferencePoint.SetCoord(Xrp,Yrp,Zrp) ;
    MyViewOrientation.SetViewReferencePoint(MyViewReferencePoint) ;
    MyView->SetViewOrientation(MyViewOrientation);
  }
#endif
#ifdef IMP020300
  MyView->SetViewMapping(MyViewMapping) ;
  SetZSize(0.) ;
#else
  Xeye = Xpn*Zrp + Xat ; Yeye = Ypn*Zrp + Yat ; Zeye = Zpn*Zrp + Zat ;
  Standard_Real Zmax = Sqrt( Xeye*Xeye + Yeye*Yeye + Zeye*Zeye );

  if( Zmax > MyViewMapping.FrontPlaneDistance() &&
    MyProjModel == V3d_TPM_SCREEN ) {
      SetZSize(2.*Zmax+Zmax*Zmargin) ;
    } else {
      if( MyType == V3d_PERSPECTIVE ) SetFocale(focale) ;
#ifdef IMP250200
      if( Depth > 0. )
#endif
        MyView->SetViewMapping(MyViewMapping) ;
    }
#endif

    ImmediateUpdate();

}


void V3d_View::SetProj( const Standard_Real Vx,const Standard_Real Vy, const Standard_Real Vz )
{

  Standard_Real Angle ;

  Viewer_BadValue_Raise_if( Sqrt(Vx*Vx + Vy*Vy + Vz*Vz) <= 0.,
    "V3d_View::SetProj, null projection vector");

  Angle = Twist() ;
  MyViewReferencePlane.SetCoord(Vx,Vy,Vz) ;
  MyViewReferencePlane.Normalize() ;
  MyViewOrientation.SetViewReferencePlane(MyViewReferencePlane) ;
  Standard_Boolean update = myImmediateUpdate;
  myImmediateUpdate = Standard_False;
  if( MyProjModel == V3d_TPM_SCREEN ) SetTwist(Angle) ;
#ifdef IMP020300
  SetZSize(0.) ;
#endif
  myImmediateUpdate = update;
  ImmediateUpdate();

}

void V3d_View::SetProj( const V3d_TypeOfOrientation Orientation ) {

  MyViewReferencePlane = V3d::GetProjAxis(Orientation) ;
  MyViewOrientation.SetViewReferencePlane(MyViewReferencePlane) ;
  // MSV 14.03.2007: reset ViewReferencePoint to debug LH3D14955
  MyViewOrientation.SetViewReferencePoint(Graphic3d_Vertex(0,0,0));
  Standard_Real Xpn=0;
  Standard_Real Ypn=0;
  Standard_Real Zpn=0;

  switch (Orientation) {
  case V3d_Zpos :
    Ypn = 1. ;
    break ;
  case V3d_Zneg :
    Ypn = -1. ;
    break ;
  default:
    Zpn = 1.;
  }
  SetUp(Xpn,Ypn,Zpn);
#ifdef IMP020300
  SetZSize(0.) ;
#endif
  ImmediateUpdate();

}

void V3d_View::SetAt(const Standard_Real X,const Standard_Real Y,const Standard_Real Z) {

  Standard_Real Angle,Xrp,Yrp,Zrp,Xpn,Ypn,Zpn,Xat,Yat,Zat ;
  Standard_Real Xeye,Yeye,Zeye ;

  Angle = Twist() ;
  MyProjReferencePoint = MyViewMapping.ProjectionReferencePoint() ;
  MyProjReferencePoint.Coord(Xrp,Yrp,Zrp) ;
  MyViewReferencePlane = MyViewOrientation.ViewReferencePlane() ;
  MyViewReferencePlane.Coord(Xpn,Ypn,Zpn) ;
  MyViewReferencePoint = MyViewOrientation.ViewReferencePoint() ;
  MyViewReferencePoint.Coord(Xat,Yat,Zat) ;
  Xeye = Zrp*Xpn + Xat ; Yeye = Zrp*Ypn + Yat ; Zeye = Zrp*Zpn + Zat ;
  Xpn = Xeye - X ; Ypn = Yeye - Y ; Zpn = Zeye - Z ;
  Zrp = Sqrt(Xpn*Xpn + Ypn*Ypn + Zpn*Zpn) ;
  Viewer_BadValue_Raise_if( Zrp <= 0.,
    "V3d_View::SetAt, Eye,At are Confused");

  Xpn /= Zrp ; Ypn /= Zrp ; Zpn /= Zrp ;
  MyViewReferencePoint.SetCoord(X,Y,Z) ;
  MyViewOrientation.SetViewReferencePoint(MyViewReferencePoint) ;
  MyViewReferencePlane.SetCoord(Xpn,Ypn,Zpn) ;
  MyViewOrientation.SetViewReferencePlane(MyViewReferencePlane) ;
  MyProjReferencePoint.SetCoord(Xrp,Yrp,Zrp) ;
  MyViewMapping.SetProjectionReferencePoint(MyProjReferencePoint);
  Standard_Boolean update = myImmediateUpdate;
  myImmediateUpdate = Standard_False;
  if( MyProjModel == V3d_TPM_WALKTHROUGH ) {
    //FMN desactivation temp SetTwist(Angle) ;
    // Set infos.
    MyView->SetViewOrientation(MyViewOrientation) ;
    MyView->SetViewMapping(MyViewMapping) ;
  } else {
    SetTwist(Angle) ;
  }
#ifdef IMP020300
  MyView->SetViewMapping(MyViewMapping);
  SetZSize(0.) ;
#else
  ImmediateUpdate();
  // Check ZClipping planes
  Standard_Real Zmax = Sqrt( X*X + Y*Y + Z*Z ) ;
  if( Zmax > MyViewMapping.FrontPlaneDistance() &&
    MyProjModel == V3d_TPM_SCREEN ) {
      SetZSize(2.*Zmax+Zmax*Zmargin) ;
    } else {
      if( MyType == V3d_PERSPECTIVE ) SetFocale(focale) ;
      MyView->SetViewMapping(MyViewMapping);
    }
#endif
    myImmediateUpdate = update;
    ImmediateUpdate();

}

void V3d_View::SetUp(const Standard_Real Vx,const Standard_Real Vy,const Standard_Real Vz) {

  Standard_Boolean TheStatus ;
  Viewer_BadValue_Raise_if( Sqrt(Vx*Vx + Vy*Vy + Vz*Vz) <= 0. ,
    "V3d_View::SetUp, nullUp vector");

  MyViewReferencePlane = MyViewOrientation.ViewReferencePlane() ;
  MyViewReferenceUp.SetCoord(Vx,Vy,Vz) ;
  MyViewReferenceUp.Normalize() ;
  TheStatus = ScreenAxis(MyViewReferencePlane,MyViewReferenceUp,
    MyXscreenAxis,MyYscreenAxis,MyZscreenAxis) ;
  if( !TheStatus ) {
    MyViewReferenceUp.SetCoord(0.,0.,1.) ;
    TheStatus = ScreenAxis(MyViewReferencePlane,MyViewReferenceUp,
      MyXscreenAxis,MyYscreenAxis,MyZscreenAxis) ;
  }
  if( !TheStatus ) {
    MyViewReferenceUp.SetCoord(0.,1.,0.) ;
    TheStatus = ScreenAxis(MyViewReferencePlane,MyViewReferenceUp,
      MyXscreenAxis,MyYscreenAxis,MyZscreenAxis) ;
  }
  if( !TheStatus ) {
    MyViewReferenceUp.SetCoord(1.,0.,0.) ;
    TheStatus = ScreenAxis(MyViewReferencePlane,MyViewReferenceUp,
      MyXscreenAxis,MyYscreenAxis,MyZscreenAxis) ;
  }
  Viewer_BadValue_Raise_if( !TheStatus,"V3d_View::Setup, alignment of Eye,At,Up");

  MyViewReferenceUp = MyYscreenAxis ;
  MyViewOrientation.SetViewReferenceUp(MyViewReferenceUp) ;
  MyView->SetViewOrientation(MyViewOrientation) ;
  ImmediateUpdate();

}

void V3d_View::SetUp( const V3d_TypeOfOrientation Orientation ) {

  Standard_Boolean TheStatus ;

  MyViewReferenceUp = V3d::GetProjAxis(Orientation) ;
  MyViewReferencePlane = MyViewOrientation.ViewReferencePlane() ;
  TheStatus = ScreenAxis(MyViewReferencePlane,MyViewReferenceUp,
    MyXscreenAxis,MyYscreenAxis,MyZscreenAxis) ;
  if( !TheStatus ) {
    MyViewReferenceUp.SetCoord(0.,0.,1.) ;
    TheStatus = ScreenAxis(MyViewReferencePlane,MyViewReferenceUp,
      MyXscreenAxis,MyYscreenAxis,MyZscreenAxis) ;
  }
  if( !TheStatus ) {
    MyViewReferenceUp.SetCoord(0.,1.,0.) ;
    TheStatus = ScreenAxis(MyViewReferencePlane,MyViewReferenceUp,
      MyXscreenAxis,MyYscreenAxis,MyZscreenAxis) ;
  }
  if( !TheStatus ) {
    MyViewReferenceUp.SetCoord(1.,0.,0.) ;
    TheStatus = ScreenAxis(MyViewReferencePlane,MyViewReferenceUp,
      MyXscreenAxis,MyYscreenAxis,MyZscreenAxis) ;
  }
  Viewer_BadValue_Raise_if( !TheStatus, "V3d_View::SetUp, alignment of Eye,At,Up");

  MyViewReferenceUp = MyYscreenAxis ;
  MyViewOrientation.SetViewReferenceUp(MyViewReferenceUp) ;
  MyView->SetViewOrientation(MyViewOrientation) ;
  ImmediateUpdate();

}

void V3d_View::SetViewOrientation(const Visual3d_ViewOrientation& VO)
{
  MyViewOrientation = VO;
  MyView->SetViewOrientation(MyViewOrientation) ;
  ImmediateUpdate();
}

void V3d_View::SetViewOrientationDefault() {

  MyView->SetViewOrientation(MyViewOrientation) ;
  MyView->SetViewOrientationDefault() ;
  ImmediateUpdate();
}

void V3d_View::ResetViewOrientation() {

  MyView->ViewOrientationReset() ;
  MyViewOrientation = MyView->ViewOrientation() ;
  ImmediateUpdate();
}

void V3d_View::Reset( const Standard_Boolean update ) {

  MyView->ViewOrientationReset() ;
  MyViewOrientation = MyView->ViewOrientation() ;
  MyView->ViewMappingReset();
  MyViewMapping = MyView->ViewMapping() ;

  ZFitAll (Zmargin);
  SwitchSetFront = Standard_False;
#ifdef IMP020300
  if( !myImmediateUpdate && update ) Update();
#else
  ImmediateUpdate();
#endif

}

void V3d_View::Panning(const Standard_Real Dx, const Standard_Real Dy, const Quantity_Factor aZoomFactor, const Standard_Boolean Start)
{

  Standard_Real Umin,Vmin,Umax,Vmax,Xrp,Yrp,Zrp,Dxv,Dyv ;
  Viewer_BadValue_Raise_if( aZoomFactor <= 0.,"V3d_View::Panning, bad zoom factor");

  if( Start ) {
    MyProjReferencePoint = MyViewMapping.ProjectionReferencePoint() ;
    MyViewMapping.WindowLimit(Umin,Vmin,Umax,Vmax) ;
    MyXwindowCenter = (Umin + Umax)/2. ;
    MyYwindowCenter = (Vmin + Vmax)/2. ;
    MyWindowWidth = Abs(Umax - Umin) ;
    MyWindowHeight = Abs(Vmax - Vmin) ;
    Viewer_BadValue_Raise_if( MyWindowWidth <= 0. || MyWindowHeight <= 0. ,
      "V3d_View::Panning, Window Size is NULL");
  }
  MyProjReferencePoint.Coord(Xrp,Yrp,Zrp) ;
  Xrp = MyXwindowCenter - Dx ;
  Yrp = MyYwindowCenter - Dy ;
  Dxv = MyWindowWidth/aZoomFactor ; Dyv = MyWindowHeight/aZoomFactor ;
  Umin = Xrp - Dxv/2. ; Umax = Xrp + Dxv/2. ;
  Vmin = Yrp - Dyv/2. ; Vmax = Yrp + Dyv/2. ;
  MyViewMapping.SetWindowLimit(Umin,Vmin,Umax,Vmax) ;
  if( MyType != V3d_PERSPECTIVE ) {
    MyProjReferencePoint.SetCoord(Xrp,Yrp,Zrp) ;
    MyViewMapping.SetProjectionReferencePoint(MyProjReferencePoint) ;
  }
  MyView->SetViewMapping(MyViewMapping) ;
  ImmediateUpdate();

}
void V3d_View::SetCenter(const Standard_Integer X, const Standard_Integer Y) {
  Standard_Real x,y;
  Convert(X,Y,x,y);
  SetCenter(x,y);
}

void V3d_View::SetCenter(const Standard_Real Xc, const Standard_Real Yc){

  Standard_Real Umin,Vmin,Umax,Vmax,Xrp,Yrp,Zrp ;

  MyProjReferencePoint = MyViewMapping.ProjectionReferencePoint() ;
  MyProjReferencePoint.Coord(Xrp,Yrp,Zrp) ;
  MyXwindowCenter = Xrp = Xc ; MyYwindowCenter = Yrp = Yc ;
  MyViewMapping.WindowLimit(Umin,Vmin,Umax,Vmax) ;
  MyWindowWidth = Abs(Umax - Umin) ; MyWindowHeight = Abs(Vmax - Vmin) ;
  Viewer_BadValue_Raise_if( MyWindowWidth <= 0. || MyWindowHeight <= 0. ,
    "V3d_View::SetCenter, Window Size is NULL");

  Umin = Xc - MyWindowWidth/2. ; Vmin = Yc - MyWindowHeight/2. ;
  Umax = Xc + MyWindowWidth/2. ; Vmax = Yc + MyWindowHeight/2. ;
  MyViewMapping.SetWindowLimit(Umin,Vmin,Umax,Vmax) ;
  if( MyType != V3d_PERSPECTIVE ) {
    MyProjReferencePoint.SetCoord(Xrp,Yrp,Zrp) ;
    MyViewMapping.SetProjectionReferencePoint(MyProjReferencePoint) ;
  }
  MyView->SetViewMapping(MyViewMapping) ;
  ImmediateUpdate();

}

void V3d_View::SetSize(const Standard_Real Size)
{

  Standard_Real Umin,Vmin,Umax,Vmax,Rap ;

  Viewer_BadValue_Raise_if(  Size  <= 0.,
    "V3d_View::SetSize, Window Size is NULL");


  MyViewMapping.WindowLimit(Umin,Vmin,Umax,Vmax) ;
  MyWindowWidth = Abs(Umax - Umin) ;
  MyWindowHeight = Abs(Vmax - Vmin) ;
  MyXwindowCenter = (Umin + Umax)/2. ;
  MyYwindowCenter = (Vmin + Vmax)/2. ;
  Rap = MyWindowWidth/MyWindowHeight ;
  if( MyWindowWidth >= MyWindowHeight ) {
    MyWindowWidth = Size ; MyWindowHeight = Size/Rap ;
  } else {
    MyWindowHeight = Size ; MyWindowWidth = Size*Rap ;
  }
  Umin = MyXwindowCenter - MyWindowWidth/2. ;
  Vmin = MyYwindowCenter - MyWindowHeight/2. ;
  Umax = MyXwindowCenter + MyWindowWidth/2. ;
  Vmax = MyYwindowCenter + MyWindowHeight/2. ;
  MyViewMapping.SetWindowLimit(Umin,Vmin,Umax,Vmax) ;
  MyView->SetViewMapping(MyViewMapping) ;
  ImmediateUpdate();

}

void V3d_View::SetZSize(const Standard_Real Size)
{

  Standard_Real Zmax = Size/2.;

#ifdef IMP020300
  if( Size <= 0. ) {
    Standard_Real Xat,Yat,Zat,Xpn,Ypn,Zpn,Xrp,Yrp,Zrp,Xeye,Yeye,Zeye;
    MyViewReferencePoint.Coord(Xat,Yat,Zat) ;
    MyProjReferencePoint.Coord(Xrp,Yrp,Zrp) ;
    MyViewReferencePlane.Coord(Xpn,Ypn,Zpn) ;
    Xeye = Zrp*Xpn + Xat ; Yeye = Zrp*Ypn + Yat ; Zeye = Zrp*Zpn + Zat;
    Zmax = Sqrt( Xeye*Xeye + Yeye*Yeye + Zeye*Zeye );
    if( Zmax <= MyViewMapping.FrontPlaneDistance() ) return;
  }
#else
  Viewer_BadValue_Raise_if(  Size  <= 0.,
    "V3d_View::SetZSize, Window ZSize is NULL");
#endif

  Standard_Real Front = MyViewContext.ZClippingFrontPlane() ;
  Standard_Real Back  = MyViewContext.ZClippingBackPlane() ;
  Standard_Real focale= Focale();

  MyViewMapping.SetFrontPlaneDistance(Zmax) ;
  MyViewMapping.SetBackPlaneDistance(-Zmax) ;

  // OCC18942
  if( MyProjModel != V3d_TPM_WALKTHROUGH ) {
    MyViewMapping.SetViewPlaneDistance(MyType == V3d_PERSPECTIVE ? 0. : Zmax) ;
  }

  MyView->SetViewMapping(MyViewMapping) ;
  if( MyViewContext.FrontZClippingIsOn()  ||
    MyViewContext.BackZClippingIsOn() ) {
      MyViewContext.SetZClippingFrontPlane(Front) ;
      MyViewContext.SetZClippingBackPlane(Back) ;
      MyView->SetContext(MyViewContext) ;
    }

}

void V3d_View::SetZoom(const Standard_Real Coef,const Standard_Boolean Start)
{

  Standard_Real Umin,Vmin,Umax,Vmax,Dxv,Dyv ;
  Viewer_BadValue_Raise_if( Coef <= 0.,"V3d_View::SetZoom, bad coefficient");

  if( Start ) {
    MyViewMapping.WindowLimit(Umin,Vmin,Umax,Vmax) ;
    MyXwindowCenter = (Umin + Umax)/2. ;
    MyYwindowCenter = (Vmin + Vmax)/2. ;
    MyWindowWidth = Abs(Umax - Umin) ;
    MyWindowHeight = Abs(Vmax - Vmin) ;
  }

  // ensure that zoom will not be too small or too big
  Standard_Real coef = Coef;
  if ( MyWindowWidth < coef * Precision::Confusion() )
    coef = MyWindowWidth / Precision::Confusion();
  else if ( MyWindowWidth > coef * 1e12 )
    coef = MyWindowWidth / 1e12;
  if ( MyWindowHeight < coef * Precision::Confusion() )
    coef = MyWindowHeight / Precision::Confusion();
  else if ( MyWindowHeight > coef * 1e12 )
    coef = MyWindowHeight / 1e12;

  Dxv = MyWindowWidth/coef;
  Dyv = MyWindowHeight/coef;
  Umin = MyXwindowCenter - Dxv/2. ; Umax = MyXwindowCenter + Dxv/2. ;
  Vmin = MyYwindowCenter - Dyv/2. ; Vmax = MyYwindowCenter + Dyv/2. ;
  MyViewMapping.SetWindowLimit(Umin,Vmin,Umax,Vmax) ;
  MyView->SetViewMapping(MyViewMapping) ;
  ImmediateUpdate();
}

void V3d_View::SetScale( const Standard_Real Coef ) {
  Standard_Real Umin,Vmin,Umax,Vmax,Xrp,Yrp,Dxv,Dyv ;
  Visual3d_ViewMapping VMD = MyView->ViewMappingDefault() ;

  Viewer_BadValue_Raise_if( Coef <= 0. ,"V3d_View::SetScale, bad coefficient");

  VMD.WindowLimit(Umin,Vmin,Umax,Vmax) ;
  Dxv = Abs(Umax - Umin) ; Dyv = Abs(Vmax - Vmin) ;
  Dxv /= Coef ; Dyv /= Coef ;
  MyViewMapping.WindowLimit(Umin,Vmin,Umax,Vmax) ;
  Xrp = (Umin + Umax)/2. ; Yrp = (Vmin + Vmax)/2. ;
  Umin = Xrp - Dxv/2. ; Umax = Xrp + Dxv/2. ;
  Vmin = Yrp - Dyv/2. ; Vmax = Yrp + Dyv/2. ;
  MyViewMapping.SetWindowLimit(Umin,Vmin,Umax,Vmax) ;
  MyView->SetViewMapping(MyViewMapping) ;
  ImmediateUpdate();

}

void V3d_View::SetAxialScale( const Standard_Real Sx, const Standard_Real Sy, const Standard_Real Sz ) {

  Standard_Real Xmin,Ymin,Zmin,Xmax,Ymax,Zmax,U,V,W ;
  Standard_Real Umin,Vmin,Wmin,Umax,Vmax,Wmax ;
  Viewer_BadValue_Raise_if( Sx <= 0. || Sy <= 0. || Sz <= 0.,"V3d_View::SetAxialScale, bad coefficient");

  MyViewOrientation.SetAxialScale( Sx, Sy, Sz );
  Aspect_TypeOfUpdate updateMode = MyView->ViewManager()->UpdateMode();
  MyView->ViewManager()->SetUpdateMode(Aspect_TOU_ASAP);
  MyView->SetViewOrientation(MyViewOrientation);
  MyView->ViewManager()->SetUpdateMode(updateMode);

  MyView->MinMaxValues(Xmin,Ymin,Zmin,Xmax,Ymax,Zmax) ;

  Standard_Real LIM = ShortRealLast() -1.;
  if     (Abs(Xmin) > LIM || Abs(Ymin) > LIM || Abs(Zmin) > LIM
    ||  Abs(Xmax) > LIM || Abs(Ymax) > LIM || Abs(Zmax) > LIM ) {
      return;
    }

    MyView->Projects(Xmin,Ymin,Zmin,Umin,Vmin,Wmin) ;
    MyView->Projects(Xmax,Ymax,Zmax,Umax,Vmax,Wmax) ;
    Umax = Max(Umin,Umax) ; Vmax = Max(Vmin,Vmax) ;
    MyView->Projects(Xmin,Ymin,Zmax,U,V,W) ;
    Umax = Max(Umax,U) ; Vmax = Max(Vmax,V) ;
    Wmin = Min(W,Wmin) ; Wmax = Max(W,Wmax) ;
    MyView->Projects(Xmax,Ymin,Zmax,U,V,W) ;
    Umax = Max(Umax,U) ; Vmax = Max(Vmax,V) ;
    Wmin = Min(W,Wmin) ; Wmax = Max(W,Wmax) ;
    MyView->Projects(Xmax,Ymin,Zmin,U,V,W) ;
    Umax = Max(Umax,U) ; Vmax = Max(Vmax,V) ;
    Wmin = Min(W,Wmin) ; Wmax = Max(W,Wmax) ;
    MyView->Projects(Xmax,Ymax,Zmin,U,V,W) ;
    Umax = Max(Umax,U) ; Vmax = Max(Vmax,V) ;
    Wmin = Min(W,Wmin) ; Wmax = Max(W,Wmax) ;
    MyView->Projects(Xmin,Ymax,Zmax,U,V,W) ;
    Umax = Max(Umax,U) ; Vmax = Max(Vmax,V) ;
    Wmin = Min(W,Wmin) ; Wmax = Max(W,Wmax) ;
    MyView->Projects(Xmin,Ymax,Zmin,U,V,W) ;
    Umax = Max(Umax,U) ; Vmax = Max(Vmax,V) ;
    Umax = Max(Umax,Vmax) ;
    Wmin = Min(W,Wmin) ; Wmax = Max(W,Wmax) ;
    Wmax = Max(Abs(Wmin),Abs(Wmax)) ;
    Wmax = Max(Abs(Umax),Abs(Wmax)) ;

    if( Wmax > 0. ) {
      SetZSize(2.*Wmax + Wmax) ;
    }

}

void V3d_View::FitAll(const Standard_Real Coef, const Standard_Boolean FitZ,
                      const Standard_Boolean update)
{
  Standard_Real Umin,Umax,Vmin,Vmax,Xrp,Yrp,Zrp,U,V,W,U1,V1,W1 ;
  Standard_Real Xmin,Ymin,Zmin,Xmax,Ymax,Zmax ;
  Standard_Real DxvOld,DyvOld,DxvNew,DyvNew,RapOld,RapNew ;
  Standard_Integer Xpixel,Ypixel;
  // CAL 6/11/98
  Standard_Integer Nstruct = MyView->NumberOfDisplayedStructures() ;
  Standard_Integer nbPasse = 2;

  if( (Nstruct <= 0) || (Coef < 0.) || (Coef > 1.) ) {
#ifndef IMP020300
    ImmediateUpdate();
#endif
    return ;
  }
  MyProjReferencePoint = MyViewMapping.ProjectionReferencePoint() ;
  MyProjReferencePoint.Coord(Xrp,Yrp,Zrp) ;
  if( MyView->IsDefined() ) {
    MyWindow->Size(Xpixel,Ypixel);
    DxvOld = Xpixel; DyvOld = Ypixel;
  } else {
    MyViewMapping.WindowLimit(Umin,Vmin,Umax,Vmax) ;
    DxvOld = Abs(Umax - Umin) ; DyvOld = Abs(Vmax - Vmin) ;
  }
  if( (DxvOld == 0.) || (DyvOld == 0.) ) return ;
  RapOld = DxvOld/DyvOld ;

  MyView->MinMaxValues(Xmin,Ymin,Zmin,Xmax,Ymax,Zmax) ;

  //Object with null bounding box for anyone axis has been
  //obtained unit bounding interval
  if( Xmin == Xmax ) {
    Xmin--; Xmax++;
  }
  if( Ymin == Ymax ) {
    Ymin--; Ymax++;
  }
  if( Zmin == Zmax ) {
    Zmin--; Zmax++;
  }

  Standard_Real LIM = ShortRealLast() -1.;
  if     (Abs(Xmin) > LIM || Abs(Ymin) > LIM || Abs(Zmin) > LIM
    ||  Abs(Xmax) > LIM || Abs(Ymax) > LIM || Abs(Zmax) > LIM ) {
#ifndef IMP020300
      ImmediateUpdate();
#endif
      return ;
    }

    while (nbPasse != 0)
    {
      MyView->Projects(Xmin,Ymin,Zmin,U,V,W) ;
      MyView->Projects(Xmax,Ymax,Zmax,U1,V1,W1) ;
      Umin = Min(U,U1) ; Umax = Max(U,U1) ;
      Vmin = Min(V,V1) ; Vmax = Max(V,V1) ;
      MyView->Projects(Xmin,Ymin,Zmax,U,V,W) ;
      Umin = Min(U,Umin) ; Umax = Max(U,Umax) ;
      Vmin = Min(V,Vmin) ; Vmax = Max(V,Vmax) ;
      MyView->Projects(Xmax,Ymin,Zmax,U,V,W) ;
      Umin = Min(U,Umin) ; Umax = Max(U,Umax) ;
      Vmin = Min(V,Vmin) ; Vmax = Max(V,Vmax) ;
      MyView->Projects(Xmax,Ymin,Zmin,U,V,W) ;
      Umin = Min(U,Umin) ; Umax = Max(U,Umax) ;
      Vmin = Min(V,Vmin) ; Vmax = Max(V,Vmax) ;
      MyView->Projects(Xmax,Ymax,Zmin,U,V,W) ;
      Umin = Min(U,Umin) ; Umax = Max(U,Umax) ;
      Vmin = Min(V,Vmin) ; Vmax = Max(V,Vmax) ;
      MyView->Projects(Xmin,Ymax,Zmax,U,V,W) ;
      Umin = Min(U,Umin) ; Umax = Max(U,Umax) ;
      Vmin = Min(V,Vmin) ; Vmax = Max(V,Vmax) ;
      MyView->Projects(Xmin,Ymax,Zmin,U,V,W) ;
      Umin = Min(U,Umin) ; Umax = Max(U,Umax) ;
      Vmin = Min(V,Vmin) ; Vmax = Max(V,Vmax) ;
      if( (Umax > Umin) && (Vmax > Vmin) ) {
        DxvNew = Abs(Umax - Umin) ; DyvNew = Abs(Vmax - Vmin) ;

        RapNew = DxvNew/DyvNew ;
        if( RapNew >= RapOld ) {
          DxvNew += Coef*DxvNew ;
          DyvNew = DxvNew/RapOld ;
        } else {
          DyvNew += Coef*DyvNew ;
          DxvNew = DyvNew*RapOld ;
        }

        Xrp = (Umin + Umax)/2. ; Yrp = (Vmin + Vmax)/2. ;
        Umin = Xrp - DxvNew/2. ; Umax = Xrp + DxvNew/2. ;
        Vmin = Yrp - DyvNew/2. ; Vmax = Yrp + DyvNew/2. ;
        MyViewMapping.SetWindowLimit(Umin,Vmin,Umax,Vmax) ;

#ifdef OCC280
        if( MyType != V3d_PERSPECTIVE ) {
          MyProjReferencePoint.SetCoord(Xrp,Yrp,Zrp) ;
          MyViewMapping.SetProjectionReferencePoint(MyProjReferencePoint);
        }
#else
        MyProjReferencePoint.SetCoord(Xrp,Yrp,Zrp) ;
        MyViewMapping.SetProjectionReferencePoint(MyProjReferencePoint);
#endif
        MyView->SetViewMapping(MyViewMapping) ;
      }

      nbPasse--;
    } // while

    if(FitZ) {
      ZFitAll(Zmargin);
#ifdef IMP020300
    } else {
      ImmediateUpdate();
#endif
    }
#ifdef IMP020300
    if( !myImmediateUpdate && update ) Update();
#endif
}

void V3d_View::ZFitAll(const Standard_Real Coef) {

  Standard_Real Xmin,Ymin,Zmin,Xmax,Ymax,Zmax,U,V,W ;
  Standard_Real Umin,Vmin,Wmin,Umax,Vmax,Wmax ;
  // CAL 6/11/98
  Standard_Integer Nstruct = MyView->NumberOfDisplayedStructures() ;

  if( MyType == V3d_PERSPECTIVE ) {
    ImmediateUpdate();
    return ;
  }

  if( (Nstruct <= 0) || (Coef < 0.) ) {
    ImmediateUpdate();
    return ;
  }

  MyView->MinMaxValues(Xmin,Ymin,Zmin,Xmax,Ymax,Zmax) ;

  Standard_Real LIM = ShortRealLast() -1.;
  if   (Abs(Xmin) > LIM || Abs(Ymin) > LIM || Abs(Zmin) > LIM
    ||  Abs(Xmax) > LIM || Abs(Ymax) > LIM || Abs(Zmax) > LIM ) {
      ImmediateUpdate();
      return ;
    }

  //Object with null bounding box for anyone axis has been
  //obtained unit bounding interval
  if( Xmin == Xmax ) {
    Xmin--; Xmax++;
  }
  if( Ymin == Ymax ) {
    Ymin--; Ymax++;
  }
  if( Zmin == Zmax ) {
    Zmin--; Zmax++;
  }

    // CAL 6/11/98
    // Case when view contains only a point
    if (Xmin == Xmax && Ymin == Ymax && Zmin == Zmax) {
      ImmediateUpdate();
      return ;
    }
    MyView->Projects(Xmin,Ymin,Zmin,Umin,Vmin,Wmin) ;
    MyView->Projects(Xmax,Ymax,Zmax,Umax,Vmax,Wmax) ;
    MyView->Projects(Xmin,Ymin,Zmax,U,V,W) ;
    Wmin = Min(W,Wmin) ; Wmax = Max(W,Wmax) ;
    MyView->Projects(Xmax,Ymin,Zmax,U,V,W) ;
    Wmin = Min(W,Wmin) ; Wmax = Max(W,Wmax) ;
    MyView->Projects(Xmax,Ymin,Zmin,U,V,W) ;
    Wmin = Min(W,Wmin) ; Wmax = Max(W,Wmax) ;
    MyView->Projects(Xmax,Ymax,Zmin,U,V,W) ;
    Wmin = Min(W,Wmin) ; Wmax = Max(W,Wmax) ;
    MyView->Projects(Xmin,Ymax,Zmax,U,V,W) ;
    Wmin = Min(W,Wmin) ; Wmax = Max(W,Wmax) ;
    MyView->Projects(Xmin,Ymax,Zmin,U,V,W) ;
    Wmin = Min(W,Wmin) ; Wmax = Max(W,Wmax) ;
    Wmax = Max(Abs(Wmin),Abs(Wmax)) ;
    if( Wmax > 0. ) {
      SetZSize(2.*Wmax + Coef * Wmax) ;
    }
    ImmediateUpdate();
}

void V3d_View::DepthFitAll(const Quantity_Coefficient Aspect,
                           const Quantity_Coefficient Margin)
{
  Standard_Real Xmin,Ymin,Zmin,Xmax,Ymax,Zmax,U,V,W,U1,V1,W1 ;
  Standard_Real Umin,Vmin,Wmin,Umax,Vmax,Wmax ;
  Standard_Real Xrp,Yrp,Zrp,Dx,Dy,Dz,Size;

  Standard_Integer Nstruct = MyView->NumberOfDisplayedStructures() ;

  if( (Nstruct <= 0) || (Aspect < 0.) || (Margin < 0.) || (Margin > 1.)) {
    ImmediateUpdate();
    return ;
  }

  MyProjReferencePoint = MyViewMapping.ProjectionReferencePoint() ;
  MyProjReferencePoint.Coord(Xrp,Yrp,Zrp) ;

  MyView->MinMaxValues(Xmin,Ymin,Zmin,Xmax,Ymax,Zmax) ;

  Standard_Real LIM = ShortRealLast() -1.;
  if     (Abs(Xmin) > LIM || Abs(Ymin) > LIM || Abs(Zmin) > LIM
    ||  Abs(Xmax) > LIM || Abs(Ymax) > LIM || Abs(Zmax) > LIM ) {
      ImmediateUpdate();
      return ;
    }

  //Object with null bounding box for anyone axis has been
  //obtained unit bounding interval
  if( Xmin == Xmax ) {
    Xmin--; Xmax++;
  }
  if( Ymin == Ymax ) {
    Ymin--; Ymax++;
  }
  if( Zmin == Zmax ) {
    Zmin--; Zmax++;
  }

    if (Xmin == Xmax && Ymin == Ymax && Zmin == Zmax) {
      ImmediateUpdate();
      return ;
    }
    MyView->Projects(Xmin,Ymin,Zmin,U,V,W) ;
    MyView->Projects(Xmax,Ymax,Zmax,U1,V1,W1) ;
    Umin = Min(U,U1) ; Umax = Max(U,U1) ;
    Vmin = Min(V,V1) ; Vmax = Max(V,V1) ;
    Wmin = Min(W,W1) ; Wmax = Max(W,W1) ;
    MyView->Projects(Xmin,Ymin,Zmax,U,V,W) ;
    Umin = Min(U,Umin) ; Umax = Max(U,Umax) ;
    Vmin = Min(V,Vmin) ; Vmax = Max(V,Vmax) ;
    Wmin = Min(W,Wmin) ; Wmax = Max(W,Wmax) ;
    MyView->Projects(Xmax,Ymin,Zmax,U,V,W) ;
    Umin = Min(U,Umin) ; Umax = Max(U,Umax) ;
    Vmin = Min(V,Vmin) ; Vmax = Max(V,Vmax) ;
    Wmin = Min(W,Wmin) ; Wmax = Max(W,Wmax) ;
    MyView->Projects(Xmax,Ymin,Zmin,U,V,W) ;
    Umin = Min(U,Umin) ; Umax = Max(U,Umax) ;
    Vmin = Min(V,Vmin) ; Vmax = Max(V,Vmax) ;
    Wmin = Min(W,Wmin) ; Wmax = Max(W,Wmax) ;
    MyView->Projects(Xmax,Ymax,Zmin,U,V,W) ;
    Umin = Min(U,Umin) ; Umax = Max(U,Umax) ;
    Vmin = Min(V,Vmin) ; Vmax = Max(V,Vmax) ;
    Wmin = Min(W,Wmin) ; Wmax = Max(W,Wmax) ;
    MyView->Projects(Xmin,Ymax,Zmax,U,V,W) ;
    Umin = Min(U,Umin) ; Umax = Max(U,Umax) ;
    Vmin = Min(V,Vmin) ; Vmax = Max(V,Vmax) ;
    Wmin = Min(W,Wmin) ; Wmax = Max(W,Wmax) ;
    MyView->Projects(Xmin,Ymax,Zmin,U,V,W) ;
    Umin = Min(U,Umin) ; Umax = Max(U,Umax) ;
    Vmin = Min(V,Vmin) ; Vmax = Max(V,Vmax) ;
    Wmin = Min(W,Wmin) ; Wmax = Max(W,Wmax) ;

    // Adjust Z size
    Wmax = Max(Abs(Wmin),Abs(Wmax)) ;
    Dz = 2.*Wmax + Margin * Wmax;

    // Compute depth value
    Dx = Abs(Umax - Umin) ; Dy = Abs(Vmax - Vmin) ; // Dz = Abs(Wmax - Wmin);
    Dx += Margin * Dx; Dy += Margin * Dy;
    Size = Sqrt(Dx*Dx + Dy*Dy + Dz*Dz);
    if( Size > 0. ) {
      SetZSize(Size) ;
      SetDepth( Aspect * Size / 2.);
    }

    ImmediateUpdate();
}

void V3d_View::FitAll(const Standard_Real Xmin, const Standard_Real Ymin, const Standard_Real Xmax, const Standard_Real Ymax) {
  FitAll(MyWindow,Xmin,Ymin,Xmax,Ymax);
#ifndef IMP020300
  ImmediateUpdate();
#endif
}

void V3d_View::WindowFitAll(const Standard_Integer Xmin, const Standard_Integer Ymin, const Standard_Integer Xmax, const Standard_Integer Ymax) {
  WindowFit(Xmin,Ymin,Xmax,Ymax);

}
void V3d_View::WindowFit(const Standard_Integer Xmin, const Standard_Integer Ymin, const Standard_Integer Xmax, const Standard_Integer Ymax) {
  Standard_Real x1,y1,x2,y2;
  Convert(Xmin,Ymin,x1,y1);
  Convert(Xmax,Ymax,x2,y2);
  FitAll(x1,y1,x2,y2);
}

void V3d_View::SetViewMapping(const Visual3d_ViewMapping& VM)
{
  MyViewMapping = VM;
  // ajust view type according to mapping projection
  // NOTE: Might be dangerous, potentially conflicts with the C++ view class
  // (orthographic or perspective)! Use with care!
  if ( VM.Projection() == Visual3d_TOP_PERSPECTIVE )
    MyType = V3d_PERSPECTIVE;
  else MyType = V3d_ORTHOGRAPHIC;

  MyView->SetViewMapping(MyViewMapping) ;
  ImmediateUpdate();
}

void V3d_View::SetViewMappingDefault() {
  MyView->SetViewMapping(MyViewMapping) ;
  MyView->SetViewMappingDefault();
  ImmediateUpdate();
}

void V3d_View::ResetViewMapping() {

  MyView->ViewMappingReset();
  MyViewMapping = MyView->ViewMapping() ;
#ifdef IMP020300
  ZFitAll (Zmargin);
  Update();
#else
  ImmediateUpdate();
#endif
}

Standard_Real V3d_View::Convert(const Standard_Integer Vp) const {
  Standard_Real Umin,Umax,Vmin,Vmax,Dxv,Vv ;
  Standard_Integer Dxw,Dyw ;

  V3d_UnMapped_Raise_if( !MyView->IsDefined(), "view has no window");

  MyWindow->Size(Dxw,Dyw);
  MyViewMapping.WindowLimit(Umin,Vmin,Umax,Vmax) ;
  Dxv = Umax - Umin ;
  Vv = Dxv*(Standard_Real)Vp/(Standard_Real)Dxw ;

  return Vv ;
}

void V3d_View::Convert(const Standard_Integer Xp, const Standard_Integer Yp, Standard_Real& Xv, Standard_Real& Yv) const {
  Standard_Real Umin,Umax,Vmin,Vmax,Dxv,Dyv ;
  Standard_Integer Dxw,Dyw ;

  V3d_UnMapped_Raise_if( !MyView->IsDefined(), "view has no window");

  MyWindow->Size(Dxw,Dyw);
  MyViewMapping.WindowLimit(Umin,Vmin,Umax,Vmax) ;
  Dxv = Umax - Umin ; Dyv = Vmax - Vmin ;
  Xv = Umin + Dxv*(Standard_Real)Xp/(Standard_Real)Dxw ;
  Yv = Vmin + Dyv*(Standard_Real)(Dyw-Yp)/(Standard_Real)Dyw ;
}

Standard_Integer V3d_View::Convert(const Standard_Real Vv) const
{
  V3d_UnMapped_Raise_if( !MyView->IsDefined(), "view has no window");

  Standard_Integer Dxw, Dyw;
  MyWindow->Size(Dxw,Dyw);

  Standard_Real Umin,Umax,Vmin,Vmax;
  MyViewMapping.WindowLimit(Umin,Vmin,Umax,Vmax) ;
  Standard_Real Dxv = Umax - Umin;
  return RealToInt ( Dxw * Vv / Dxv );
}

void V3d_View::Convert(const Standard_Real Xv, const Standard_Real Yv, Standard_Integer& Xp, Standard_Integer& Yp) const
{
  V3d_UnMapped_Raise_if( !MyView->IsDefined(), "view has no window");

  Standard_Integer Dxw, Dyw;
  MyWindow->Size(Dxw,Dyw);

  Standard_Real Umin,Umax,Vmin,Vmax;
  MyViewMapping.WindowLimit(Umin,Vmin,Umax,Vmax) ;
  Standard_Real Dxv = Umax - Umin;
  Standard_Real Dyv = Vmax - Vmin;

  // CAL 15/12/93 warning:  double  assigned to  int
  Xp = RealToInt (Dxw*(Xv - Umin)/Dxv);
  // CAL 15/12/93 warning:  double  assigned to  int
  Yp = Dyw - RealToInt (Dyw*(Yv - Vmin)/Dyv);
}

void V3d_View::Convert(const Standard_Integer Xp, const Standard_Integer Yp, Standard_Real& X, Standard_Real& Y, Standard_Real& Z) const {

  Graphic3d_Vertex Vrp ;
  Vrp = (MyViewer->Viewer())->ConvertCoord(MyWindow,Xp,Yp) ;
  Vrp.Coord(X,Y,Z) ;
#ifndef IMP240100
  if( MyViewer->Grid()->IsActive() ) {
    Graphic3d_Vertex NewVrp = Compute(Vrp) ;
    NewVrp.Coord(X,Y,Z) ;
  }
#endif
}

void V3d_View::ConvertWithProj(const Standard_Integer Xp, const Standard_Integer Yp, Standard_Real& X, Standard_Real& Y, Standard_Real& Z, Standard_Real& Dx, Standard_Real& Dy, Standard_Real& Dz) const
{
  Graphic3d_Vertex Vrp ;
  Graphic3d_Vector Proj ;
  (MyViewer->Viewer())->ConvertCoordWithProj(MyWindow,Xp,Yp, Vrp, Proj) ;
  Vrp.Coord(X,Y,Z) ;
  Proj.Coord(Dx,Dy,Dz) ;
#ifndef IMP240100
  if( MyViewer->Grid()->IsActive() ) {
    Graphic3d_Vertex NewVrp = Compute(Vrp) ;
    NewVrp.Coord(X,Y,Z) ;
  }
#endif
}

#ifdef IMP240100
void V3d_View::ConvertToGrid(const Standard_Integer Xp, const Standard_Integer Yp, Standard_Real& Xg, Standard_Real& Yg, Standard_Real& Zg) const {

  Graphic3d_Vertex Vrp ;
  Vrp = (MyViewer->Viewer())->ConvertCoord(MyWindow,Xp,Yp) ;
  if( MyViewer->Grid()->IsActive() ) {
    Graphic3d_Vertex NewVrp = Compute(Vrp) ;
    NewVrp.Coord(Xg,Yg,Zg) ;
  } else
    Vrp.Coord(Xg,Yg,Zg) ;
}

void V3d_View::ConvertToGrid(const Standard_Real X, const Standard_Real Y, const Standard_Real Z, Standard_Real& Xg, Standard_Real& Yg, Standard_Real& Zg) const {

  if( MyViewer->Grid()->IsActive() ) {
    Graphic3d_Vertex Vrp(X,Y,Z) ;
    Graphic3d_Vertex NewVrp = Compute(Vrp) ;
    NewVrp.Coord(Xg,Yg,Zg) ;
  } else {
    Xg = X; Yg = Y; Zg = Z;
  }
}
#endif

#ifndef IMP240100
void V3d_View::PixToRef(const Standard_Integer Xp, const Standard_Integer Yp, Standard_Real& X, Standard_Real& Y, Standard_Real& Z) const {

  Graphic3d_Vertex Vrp ;
  Vrp = (MyViewer->Viewer())->ConvertCoord(MyWindow,Xp,Yp) ;
  Vrp.Coord(X,Y,Z) ;
  if( MyViewer->Grid()->IsActive() ) {
    Graphic3d_Vertex NewVrp = Compute(Vrp) ;
    NewVrp.Coord(X,Y,Z) ;
  }
}
#endif

#ifdef IMP240100
void V3d_View::Convert(const Standard_Real X, const Standard_Real Y, const Standard_Real Z, Standard_Integer& Xp, Standard_Integer& Yp) const {
#else
void V3d_View::RefToPix(const Standard_Real X, const Standard_Real Y, const Standard_Real Z, Standard_Integer& Xp, Standard_Integer& Yp) const {
#endif

  Graphic3d_Vertex Vrp(X,Y,Z) ;
  (MyViewer->Viewer())->ConvertCoord(MyWindow,Vrp,Xp,Yp) ;
}

void V3d_View::Project(const Standard_Real X, const Standard_Real Y, const Standard_Real Z, Standard_Real &Xp, Standard_Real &Yp) const
{
  if ( MyType != V3d_PERSPECTIVE )
  {
    // use old implementation
    Standard_Real Zp;
    MyView->Projects( X, Y, Z, Xp, Yp, Zp );
  }
  else
  {
    // NKV - Using raster projection
    Standard_Integer Xpx, Ypx;
    Convert(X, Y, Z, Xpx, Ypx);
    Convert(Xpx, Ypx, Xp, Yp);
  }
}

void V3d_View::BackgroundColor(const Quantity_TypeOfColor Type,Standard_Real& V1, Standard_Real& V2, Standard_Real& V3)const  {

#ifdef GER61351
  Quantity_Color C = BackgroundColor() ;
#else
  Quantity_Color C ;
  C = MyBackground.Color() ;
#endif
  C.Values(V1,V2,V3,Type) ;
}

#ifdef GER61351
Quantity_Color V3d_View::BackgroundColor() const  {

  return MyBackground.Color() ;
}
#endif

void V3d_View::GradientBackgroundColors(Quantity_Color& Color1,Quantity_Color& Color2) const{
  MyGradientBackground.Colors(Color1, Color2);
}

Aspect_GradientBackground V3d_View::GradientBackground() const {
   return MyGradientBackground;
}

Standard_Real V3d_View::Scale()const  {
  Standard_Real Umin,Vmin,Umax,Vmax,Dxv ;
  Visual3d_ViewMapping VMD = MyView->ViewMappingDefault() ;
  Standard_Real S = 0. ;

  VMD.WindowLimit(Umin,Vmin,Umax,Vmax) ;
  Dxv = Umax - Umin ;
  MyViewMapping.WindowLimit(Umin,Vmin,Umax,Vmax) ;
  if( Umax > Umin && Vmax > Vmin ) S = Dxv/(Umax - Umin) ;
  return S ;
}

void V3d_View::AxialScale(Standard_Real& Sx, Standard_Real& Sy, Standard_Real& Sz)const  {
  MyViewOrientation.AxialScale( Sx, Sy, Sz );
}

void V3d_View::Center(Standard_Real& Xc, Standard_Real& Yc) const {

  Standard_Real Umin,Vmin,Umax,Vmax ;

  MyViewMapping.WindowLimit(Umin,Vmin,Umax,Vmax) ;
  Xc = (Umin + Umax)/2. ;
  Yc = (Vmin + Vmax)/2. ;
}

void V3d_View::Size(Standard_Real& Width, Standard_Real& Height) const {

  Standard_Real Umin,Vmin,Umax,Vmax ;

  MyViewMapping.WindowLimit(Umin,Vmin,Umax,Vmax) ;
  Width = Umax - Umin ;
  Height = Vmax - Vmin ;
}

Standard_Real V3d_View::ZSize() const {

  Standard_Real Wmin,Wmax,Depth ;

  Wmax = MyViewMapping.FrontPlaneDistance() ;
  Wmin = MyViewMapping.BackPlaneDistance() ;
  Depth = 2. * Max(Wmin,Wmax) ;
  return (Depth) ;
}

Standard_Integer V3d_View::MinMax(Standard_Real& Umin, Standard_Real& Vmin, Standard_Real& Umax, Standard_Real& Vmax) const {

  Standard_Real Wmin,Wmax,U,V,W ;
  Standard_Real Xmin,Ymin,Zmin,Xmax,Ymax,Zmax ;
  // CAL 6/11/98
  Standard_Integer Nstruct = MyView->NumberOfDisplayedStructures() ;

  if( Nstruct ) {
    MyView->MinMaxValues(Xmin,Ymin,Zmin,Xmax,Ymax,Zmax) ;
    MyView->Projects(Xmin,Ymin,Zmin,Umin,Vmin,Wmin) ;
    MyView->Projects(Xmax,Ymax,Zmax,Umax,Vmax,Wmax) ;
    MyView->Projects(Xmin,Ymin,Zmax,U,V,W) ;
    Umin = Min(U,Umin) ; Umax = Max(U,Umax) ;
    Vmin = Min(V,Vmin) ; Vmax = Max(V,Vmax) ;
    Wmin = Min(W,Wmin) ; Wmax = Max(W,Wmax) ;
    MyView->Projects(Xmax,Ymin,Zmax,U,V,W) ;
    Umin = Min(U,Umin) ; Umax = Max(U,Umax) ;
    Vmin = Min(V,Vmin) ; Vmax = Max(V,Vmax) ;
    Wmin = Min(W,Wmin) ; Wmax = Max(W,Wmax) ;
    MyView->Projects(Xmax,Ymin,Zmin,U,V,W) ;
    Umin = Min(U,Umin) ; Umax = Max(U,Umax) ;
    Vmin = Min(V,Vmin) ; Vmax = Max(V,Vmax) ;
    Wmin = Min(W,Wmin) ; Wmax = Max(W,Wmax) ;
    MyView->Projects(Xmax,Ymax,Zmin,U,V,W) ;
    Umin = Min(U,Umin) ; Umax = Max(U,Umax) ;
    Vmin = Min(V,Vmin) ; Vmax = Max(V,Vmax) ;
    Wmin = Min(W,Wmin) ; Wmax = Max(W,Wmax) ;
    MyView->Projects(Xmin,Ymax,Zmax,U,V,W) ;
    Umin = Min(U,Umin) ; Umax = Max(U,Umax) ;
    Vmin = Min(V,Vmin) ; Vmax = Max(V,Vmax) ;
    Wmin = Min(W,Wmin) ; Wmax = Max(W,Wmax) ;
    MyView->Projects(Xmin,Ymax,Zmin,U,V,W) ;
    Umin = Min(U,Umin) ; Umax = Max(U,Umax) ;
    Vmin = Min(V,Vmin) ; Vmax = Max(V,Vmax) ;
    Wmin = Min(W,Wmin) ; Wmax = Max(W,Wmax) ;
  }
  return Nstruct ;
}

Standard_Integer V3d_View::MinMax(Standard_Real& Xmin, Standard_Real& Ymin, Standard_Real& Zmin, Standard_Real& Xmax, Standard_Real& Ymax, Standard_Real& Zmax) const {
  // CAL 6/11/98
  // Standard_Integer Nstruct = (MyView->DisplayedStructures())->Extent() ;
  Standard_Integer Nstruct = MyView->NumberOfDisplayedStructures() ;

  if( Nstruct ) {
    MyView->MinMaxValues(Xmin,Ymin,Zmin,Xmax,Ymax,Zmax) ;
  }
  return Nstruct ;
}

Standard_Integer V3d_View::Gravity(Standard_Real& X, Standard_Real& Y, Standard_Real& Z) const {

  Standard_Real Xmin,Ymin,Zmin,Xmax,Ymax,Zmax,U,V,W ;
  Standard_Real Umin,Vmin,Umax,Vmax ;
  Standard_Integer Nstruct,Npoint ;
  Graphic3d_MapOfStructure MySetOfStructures;

  MyView->DisplayedStructures (MySetOfStructures);
  Nstruct = MySetOfStructures.Extent() ;

  Graphic3d_MapIteratorOfMapOfStructure MyIterator(MySetOfStructures) ;

  MyViewMapping.WindowLimit(Umin,Vmin,Umax,Vmax) ;
  Npoint = 0 ; X = Y = Z = 0. ;
  for( ; MyIterator.More(); MyIterator.Next()) {
    if (!(MyIterator.Key())->IsEmpty()) {
      (MyIterator.Key())->MinMaxValues(Xmin,Ymin,Zmin,
        Xmax,Ymax,Zmax) ;

      Standard_Real LIM = ShortRealLast() -1.;
      if (!    (Abs(Xmin) > LIM || Abs(Ymin) > LIM || Abs(Zmin) > LIM
        ||  Abs(Xmax) > LIM || Abs(Ymax) > LIM || Abs(Zmax) > LIM )) {

          MyView->Projects(Xmin,Ymin,Zmin,U,V,W) ;
          if( U >= Umin && U <= Umax && V >= Vmin && V <= Vmax ) {
            Npoint++ ; X += Xmin ; Y += Ymin ; Z += Zmin ;
          }
          MyView->Projects(Xmax,Ymin,Zmin,U,V,W) ;
          if( U >= Umin && U <= Umax && V >= Vmin && V <= Vmax ) {
            Npoint++ ; X += Xmax ; Y += Ymin ; Z += Zmin ;
          }
          MyView->Projects(Xmin,Ymax,Zmin,U,V,W) ;
          if( U >= Umin && U <= Umax && V >= Vmin && V <= Vmax ) {
            Npoint++ ; X += Xmin ; Y += Ymax ; Z += Zmin ;
          }
          MyView->Projects(Xmax,Ymax,Zmin,U,V,W) ;
          if( U >= Umin && U <= Umax && V >= Vmin && V <= Vmax ) {
            Npoint++ ; X += Xmax ; Y += Ymax ; Z += Zmin ;
          }
          MyView->Projects(Xmin,Ymin,Zmax,U,V,W) ;
          if( U >= Umin && U <= Umax && V >= Vmin && V <= Vmax ) {
            Npoint++ ; X += Xmin ; Y += Ymin ; Z += Zmax ;
          }
          MyView->Projects(Xmax,Ymin,Zmax,U,V,W) ;
          if( U >= Umin && U <= Umax && V >= Vmin && V <= Vmax ) {
            Npoint++ ; X += Xmax ; Y += Ymin ; Z += Zmax ;
          }
          MyView->Projects(Xmin,Ymax,Zmax,U,V,W) ;
          if( U >= Umin && U <= Umax && V >= Vmin && V <= Vmax ) {
            Npoint++ ; X += Xmin ; Y += Ymax ; Z += Zmax ;
          }
          MyView->Projects(Xmax,Ymax,Zmax,U,V,W) ;
          if( U >= Umin && U <= Umax && V >= Vmin && V <= Vmax ) {
            Npoint++ ; X += Xmax ; Y += Ymax ; Z += Zmax ;
          }
        }
    }
  }
  if( Npoint > 0 ) {
    X /= Npoint ; Y /= Npoint ; Z /= Npoint ;
  }

  return Nstruct ;
}

void V3d_View::Eye(Standard_Real& X, Standard_Real& Y, Standard_Real& Z) const {
  Graphic3d_Vertex Prp ;
  Graphic3d_Vector Vpn ;
  Standard_Real Xrp,Yrp,Zrp,Xpn,Ypn,Zpn,Xat,Yat,Zat ;

  MyViewReferencePoint = MyViewOrientation.ViewReferencePoint() ;
  MyViewReferencePoint.Coord(Xat,Yat,Zat) ;
  Prp = MyViewMapping.ProjectionReferencePoint() ;
  Prp.Coord(Xrp,Yrp,Zrp) ;
  Vpn = MyViewOrientation.ViewReferencePlane() ;
  Vpn.Coord(Xpn,Ypn,Zpn) ;
  X = Zrp*Xpn + Xat; Y = Zrp*Ypn + Yat; Z = Zrp*Zpn + Zat;
}

void V3d_View::FocalReferencePoint(Standard_Real& X, Standard_Real& Y,Standard_Real& Z) const {

  Graphic3d_Vertex PRP,VRPoint;
  Graphic3d_Vector VRPlane;
  Standard_Real FPD,Xprp,Yprp,Zprp;
  Standard_Real Xvrp,Yvrp,Zvrp,DXvrp,DYvrp,DZvrp;

  if ( MyType == V3d_PERSPECTIVE ) {
    PRP = MyViewMapping.ProjectionReferencePoint() ;
    FPD = MyViewMapping.FrontPlaneDistance();
    PRP.Coord(Xprp,Yprp,Zprp);
    VRPoint = MyViewOrientation.ViewReferencePoint();
    VRPoint.Coord(Xvrp,Yvrp,Zvrp);
    VRPlane = MyViewOrientation.ViewReferencePlane();
    VRPlane.Coord(DXvrp,DYvrp,DZvrp);
    X = (FPD + Zprp) * DXvrp + Xvrp;
    Y = (FPD + Zprp) * DYvrp + Yvrp;
    Z = (FPD + Zprp) * DZvrp + Zvrp;
  }
  else Eye(X,Y,Z);
}

void V3d_View::ProjReferenceAxe(const Standard_Integer Xpix, const Standard_Integer Ypix, Standard_Real& XP, Standard_Real& YP, Standard_Real& ZP, Standard_Real& VX, Standard_Real& VY, Standard_Real& VZ) const {

  Standard_Real Xo,Yo,Zo;

  Convert(Xpix,Ypix,XP,YP,ZP);
  if ( MyType == V3d_PERSPECTIVE ) {
    FocalReferencePoint(Xo,Yo,Zo);
    VX = Xo - XP;
    VY = Yo - YP;
    VZ = Zo - ZP;
  }
  else {
    Proj(VX,VY,VZ);
  }
}


Standard_Real V3d_View::Depth() const {
  Graphic3d_Vertex Prp ;
  Standard_Real Xrp,Yrp,Zrp ;

  Prp = MyViewMapping.ProjectionReferencePoint() ;
  Prp.Coord(Xrp,Yrp,Zrp) ;
  return Zrp ;
}

void V3d_View::Proj(Standard_Real& Dx, Standard_Real& Dy, Standard_Real& Dz) const {
  Graphic3d_Vector Vpn ;

  Vpn = MyViewOrientation.ViewReferencePlane() ;
  Vpn.Coord(Dx,Dy,Dz) ;
}

void V3d_View::At(Standard_Real& X, Standard_Real& Y, Standard_Real& Z) const {
  Graphic3d_Vertex Vrp ;

  Vrp = MyViewOrientation.ViewReferencePoint() ;
  Vrp.Coord(X,Y,Z) ;
}

void V3d_View::Up(Standard_Real& Vx, Standard_Real& Vy, Standard_Real& Vz) const {
  Graphic3d_Vector Vup ;

  Vup = MyViewOrientation.ViewReferenceUp() ;
  Vup.Coord(Vx,Vy,Vz) ;
}

Standard_Real V3d_View::Twist()const  {
  Standard_Real Xup,Yup,Zup,Xpn,Ypn,Zpn,X0,Y0,Z0 ;
  Standard_Real pvx,pvy,pvz,pvn,sca,angle ;
  Graphic3d_Vector Vpn,Vup,Xaxis,Yaxis,Zaxis ;
  Standard_Boolean TheStatus ;

  Vpn = MyViewOrientation.ViewReferencePlane() ;
  Vpn.Coord(Xpn,Ypn,Zpn) ;
  Vup.SetCoord(0.,0.,1.) ;
  TheStatus = ScreenAxis(Vpn,Vup,Xaxis,Yaxis,Zaxis) ;
  if( !TheStatus ) {
    Vup.SetCoord(0.,1.,0.) ;
    TheStatus = ScreenAxis(Vpn,Vup,Xaxis,Yaxis,Zaxis) ;
  }
  if( !TheStatus ) {
    Vup.SetCoord(1.,0.,0.) ;
    TheStatus = ScreenAxis(Vpn,Vup,Xaxis,Yaxis,Zaxis) ;
  }
  Yaxis.Coord(X0,Y0,Z0) ;
  Vup = MyViewOrientation.ViewReferenceUp() ;
  Vup.Coord(Xup,Yup,Zup) ;
  /* Compute Cross Vector From Up & Origin */
  pvx = Y0*Zup - Z0*Yup ;
  pvy = Z0*Xup - X0*Zup ;
  pvz = X0*Yup - Y0*Xup ;
  pvn = pvx*pvx + pvy*pvy + pvz*pvz ;
  sca = X0*Xup + Y0*Yup + Z0*Zup ;
  /* Compute Angle */
  angle = Sqrt(pvn) ;
  if( angle > 1. ) angle = 1. ;
  else if( angle < -1. ) angle = -1. ;
  angle = asin(angle) ;
  if( sca < 0. ) angle = Standard_PI - angle ;
  if( angle > 0. && angle < Standard_PI ) {
    sca = pvx*Xpn + pvy*Ypn + pvz*Zpn ;
    if( sca < 0. ) angle = DEUXPI - angle ;
  }
  return angle ;
}

V3d_TypeOfShadingModel V3d_View::ShadingModel()const  {
  V3d_TypeOfShadingModel SM = (V3d_TypeOfShadingModel)MyViewContext.Model() ;
  return SM ;
}

V3d_TypeOfSurfaceDetail V3d_View::SurfaceDetail()const  {
  V3d_TypeOfSurfaceDetail SM = (V3d_TypeOfSurfaceDetail)MyViewContext.SurfaceDetail() ;
  return SM ;
}


Handle_Graphic3d_TextureEnv V3d_View::TextureEnv() const {
  Handle(Graphic3d_TextureEnv) SM = MyViewContext.TextureEnv() ;
  return SM ;
}

V3d_TypeOfVisualization V3d_View::Visualization()const  {
  V3d_TypeOfVisualization V =
    (V3d_TypeOfVisualization)MyViewContext.Visualization() ;
  return V ;
}

Standard_Boolean V3d_View::Antialiasing()const  {
  Standard_Boolean A = MyViewContext.AliasingIsOn() ;
  return A ;
}

Handle(V3d_Viewer) V3d_View::Viewer() const {
  return MyViewer ;
}

Standard_Boolean V3d_View::IfWindow() const {
  Standard_Boolean TheStatus = MyView->IsDefined() ;
  return TheStatus ;
}

Handle(Aspect_Window) V3d_View::Window() const {
  return MyWindow;
}

V3d_TypeOfView V3d_View::Type() const {

  return (MyType) ;
}

void V3d_View::SetFocale( const Standard_Real focale ) {

  Standard_TypeMismatch_Raise_if (MyType != V3d_PERSPECTIVE,
    "the view is not a perspective view");
  Standard_Real Xrp,Yrp,Zrp,ViewPlane,FrontPlane ;
  Graphic3d_Vertex Prp ;
  Prp = MyViewMapping.ProjectionReferencePoint() ;
  Prp.Coord(Xrp,Yrp,Zrp) ;
  if( MyProjModel == V3d_TPM_WALKTHROUGH ) {
    ViewPlane = Zrp - focale ;
  } else {
    FrontPlane = MyViewMapping.FrontPlaneDistance() ;
    ViewPlane = FrontPlane + Zrp - focale ;
  }
  MyViewMapping.SetViewPlaneDistance(ViewPlane) ;
  MyView->SetViewMapping(MyViewMapping) ;

  ImmediateUpdate();
}

Standard_Real V3d_View::Focale( ) const {
  Standard_Real Xrp,Yrp,Zrp,ViewPlane,FrontPlane ;
  Standard_Real focale = 0.0 ;
  Graphic3d_Vertex Prp ;

  if( MyType == V3d_PERSPECTIVE ) {
    Prp = MyViewMapping.ProjectionReferencePoint() ;
    Prp.Coord(Xrp,Yrp,Zrp) ;
    ViewPlane = MyViewMapping.ViewPlaneDistance() ;
    if( MyProjModel == V3d_TPM_WALKTHROUGH ) {
      focale = Zrp - ViewPlane ;
    } else {
      FrontPlane = MyViewMapping.FrontPlaneDistance() ;
      focale = FrontPlane + Zrp - ViewPlane ;
    }
  }
  return (focale) ;
}

void V3d_View::SetViewingVolume(const Standard_Real Left, const Standard_Real Right,
                                const Standard_Real Bottom, const Standard_Real Top,
                                const Standard_Real ZNear, const Standard_Real ZFar)
{
  Standard_Real Umin,Vmin,Umax,Vmax,Xrp,Yrp,Zrp;

  Viewer_BadValue_Raise_if ( ZNear <= 0. || ZFar <= 0. || ZNear >= ZFar, "V3d_View::SetVolume, bad distances");

  MyProjReferencePoint = MyViewMapping.ProjectionReferencePoint() ;
  MyProjReferencePoint.Coord(Xrp,Yrp,Zrp) ;
  Zrp = ZNear;

  Standard_Real size = (ZFar - ZNear) / 2.;

  MyViewMapping.SetFrontPlaneDistance(size);
  MyViewMapping.SetBackPlaneDistance(-size);

  // keep view plane at front plane distance
  MyViewMapping.SetViewPlaneDistance(MyType == V3d_PERSPECTIVE ? 0. : size);

  // set window limits
  Umin = Left; Umax = Right;
  Vmin = Bottom; Vmax = Top;
  MyViewMapping.SetWindowLimit(Left, Bottom, Right, Top);

  // Update window center
  if ( MyType == V3d_PERSPECTIVE ) {
    Xrp = Yrp = 0.0;
  }
  else {
    Xrp = (Umin + Umax)/2. ; Yrp = (Vmin + Vmax)/2. ;
  }
  MyProjReferencePoint.SetCoord(Xrp,Yrp,Zrp) ;
  MyViewMapping.SetProjectionReferencePoint(MyProjReferencePoint);

  MyView->SetViewMapping(MyViewMapping) ;
  //  SetZSize(0.);
  ImmediateUpdate();
}

Handle(Visual3d_View) V3d_View::View() const
{
  return MyView ;
}

Visual3d_ViewMapping V3d_View::ViewMapping() const
{
  return MyViewMapping;
}

Visual3d_ViewOrientation V3d_View::ViewOrientation() const
{
  return MyViewOrientation;
}

Standard_Boolean V3d_View::ScreenAxis( const Graphic3d_Vector &Vpn, const Graphic3d_Vector &Vup, Graphic3d_Vector &Xaxe, Graphic3d_Vector &Yaxe, Graphic3d_Vector &Zaxe) {
  Standard_Real Xpn,Ypn,Zpn,Xup,Yup,Zup ;
  Standard_Real dx1,dy1,dz1,dx2,dy2,dz2,xx,yy,zz ;

  Vpn.Coord(Xpn,Ypn,Zpn) ;
  Vup.Coord(Xup,Yup,Zup) ;
  xx = Yup*Zpn - Zup*Ypn ;
  yy = Zup*Xpn - Xup*Zpn ;
  zz = Xup*Ypn - Yup*Xpn ;
  Xaxe.SetCoord(xx,yy,zz) ;
  if( Xaxe.LengthZero() ) return Standard_False;
  Xaxe.Normalize() ; Xaxe.Coord(dx1,dy1,dz1) ;
  xx = Ypn*dz1 - Zpn*dy1 ;
  yy = Zpn*dx1 - Xpn*dz1 ;
  zz = Xpn*dy1 - Ypn*dx1 ;
  Yaxe.SetCoord(xx,yy,zz) ;
  if( Yaxe.LengthZero() ) return Standard_False;
  Yaxe.Normalize() ; Yaxe.Coord(dx2,dy2,dz2) ;
  xx = dy1*dz2 - dz1*dy2 ;
  yy = dz1*dx2 - dx1*dz2 ;
  zz = dx1*dy2 - dy1*dx2 ;
  Zaxe.SetCoord(xx,yy,zz) ;
  if( Zaxe.LengthZero() ) return Standard_False;
  Zaxe.Normalize() ;
  return Standard_True ;
}

void V3d_View::InitMatrix( TColStd_Array2OfReal& Matrix ) {
  Standard_Integer LR = Matrix.LowerRow() ;
  Standard_Integer UR = Matrix.UpperRow() ;
  Standard_Integer LC = Matrix.LowerCol() ;
  Standard_Integer UC = Matrix.UpperCol() ;
  Standard_Integer I,J ;

  for( I=LR ; I<=UR ; I++ ) {
    for( J=LC ; J<=UC ; J++ ) Matrix(I,J) = 0. ;
  }
  for( I=LR,J=LC ; I<=UR ; I++,J++ ) Matrix(I,J) = 1. ;
}

Standard_Boolean V3d_View::Multiply (const TColStd_Array2OfReal& Left, const TColStd_Array2OfReal& Right, TColStd_Array2OfReal& Matrix) {

  Standard_Integer llr = Left.LowerRow ();
  Standard_Integer llc = Left.LowerCol ();
  Standard_Integer luc = Left.UpperCol ();

  Standard_Integer rlr = Right.LowerRow ();
  Standard_Integer rur = Right.UpperRow ();
  Standard_Integer rlc = Right.LowerCol ();

  Standard_Integer mlr = Matrix.LowerRow ();
  Standard_Integer mur = Matrix.UpperRow ();
  Standard_Integer mlc = Matrix.LowerCol ();
  Standard_Integer muc = Matrix.UpperCol ();

  TColStd_Array2OfReal Result (mlr, mur, mlc, muc);

  InitMatrix (Result);

  // Left et Right incompatibles
  if (luc - llc + 1 != rur - rlr + 1)
    return Standard_False;

  Standard_Integer i, j, k;

  Standard_Real Som;
  Standard_Integer I1 = llr;

  for (i=mlr; i<=mur; i++) {
    Standard_Integer J2 = rlc;
    for (j=mlc; j<=muc; j++) {
      Som = 0.0;
      Standard_Integer J1 = llc;
      Standard_Integer I2 = rlr;
      for (k=llc; k<=luc; k++) {
        Som = Som + Left (I1, J1) * Right (I2, J2);
        J1++;
        I2++;
      }
      Result (i, j) = Som;
      J2++;
    }
    I1++;
  }

  for (i=mlr; i<=mur; i++)
    for (j=mlc; j<=muc; j++)
      Matrix (i, j) = Result (i, j);

  return Standard_True;
}

/*----------------------------------------------------------------------*/

void V3d_View::RotAxis( const Graphic3d_Vertex &Vrp, const Graphic3d_Vector &Axe, const Standard_Real angle, TColStd_Array2OfReal& Matrix ) {
  Standard_Real Xrp,Yrp,Zrp,Xaxe,Yaxe,Zaxe ;
  Standard_Real sina,cosa,cos1m,terms1,terms2,terms3 ;
  Standard_Real termc12,termc13,termc23,vcal ;
  TColStd_Array2OfReal Tmatrix(0,3,0,3) ;
  TColStd_Array2OfReal Rmatrix(0,3,0,3) ;

  InitMatrix(Matrix) ;
  InitMatrix(Tmatrix) ;

  Vrp.Coord(Xrp,Yrp,Zrp) ;
  Axe.Coord(Xaxe,Yaxe,Zaxe) ;
  /* translation of x,y,z */
  Tmatrix(0,3) = Xrp ; Tmatrix(1,3) = Yrp ; Tmatrix(2,3) = Zrp ;
  /* rotation around an axis */
  cosa = cos(angle); sina = sin(angle); cos1m = 1. - cosa ;
  termc12 = Xaxe * Yaxe * cos1m ;
  termc13 = Xaxe * Zaxe * cos1m ;
  termc23 = Yaxe * Zaxe * cos1m ;
  terms1 =  Xaxe * sina ;
  terms2 =  Yaxe * sina ;
  terms3 =  Zaxe * sina ;

  vcal =  Xaxe * Xaxe ;
  Rmatrix(0,0) = vcal + ( 1. - vcal ) * cosa ;
  Rmatrix(1,0) = termc12 + terms3 ;
  Rmatrix(2,0) = termc13 - terms2 ;
  Rmatrix(3,0) = 0. ;
  Rmatrix(0,1) = termc12 - terms3 ;
  vcal =  Yaxe * Yaxe ;
  Rmatrix(1,1) = vcal + ( 1. - vcal ) * cosa ;
  Rmatrix(2,1) = termc23 + terms1 ;
  Rmatrix(3,1) = 0. ;
  Rmatrix(0,2) = termc13 + terms2 ;
  Rmatrix(1,2) = termc23 - terms1 ;
  vcal =  Zaxe * Zaxe ;
  Rmatrix(2,2) = vcal + ( 1. - vcal ) * cosa ;
  Rmatrix(3,2) = 0. ;
  Rmatrix(0,3) = 0. ;
  Rmatrix(1,3) = 0. ;
  Rmatrix(2,3) = 0. ;
  Rmatrix(3,3) = 1. ;
  /* produced by two matrices */
  Multiply(Tmatrix, Rmatrix, Matrix);
  /* translation invert */
  Tmatrix(0,3) = -Xrp;
  Tmatrix(1,3) = -Yrp;
  Tmatrix(2,3) = -Zrp;
  /* product final */
  Multiply(Matrix, Tmatrix, Matrix);
}

/*----------------------------------------------------------------------*/

Graphic3d_Vertex V3d_View::TrsPoint( const Graphic3d_Vertex &P, const TColStd_Array2OfReal &Matrix ) {
  Graphic3d_Vertex PP ;
  Standard_Real X,Y,Z,XX,YY,ZZ ;

  // CAL. S3892
  Standard_Integer lr, ur, lc, uc;
  lr    = Matrix.LowerRow ();
  ur    = Matrix.UpperRow ();
  lc    = Matrix.LowerCol ();
  uc    = Matrix.UpperCol ();
  if ( (ur - lr + 1 != 4) || (uc - lc + 1 != 4) ) {
    P.Coord(X,Y,Z) ;
    PP.SetCoord(X,Y,Z) ;
    return PP ;
  }
  P.Coord(X,Y,Z) ;
  XX = (Matrix(lr,lc+3) + X*Matrix(lr,lc) + Y*Matrix(lr,lc+1)+
    Z*Matrix(lr,lc+2))/Matrix(lr+3,lc+3) ;

  YY = (Matrix(lr+1,lc+3) + X*Matrix(lr+1,lc) + Y*Matrix(lr+1,lc+1) +
    Z*Matrix(lr+1,lc+2))/Matrix(lr+3,lc+3) ;

  ZZ = (Matrix(lr+2,lc+3) + X*Matrix(lr+2,lc) + Y*Matrix(lr+2,lc+1) +
    Z*Matrix(lr+2,lc+2))/Matrix(lr+3,lc+3) ;
  PP.SetCoord(XX,YY,ZZ) ;
  return PP ;
}

Graphic3d_Vector V3d_View::TrsPoint( const Graphic3d_Vector& V, const TColStd_Array2OfReal& Matrix ) {
  Graphic3d_Vector VV ;
  Standard_Real X,Y,Z,XX,YY,ZZ ;

  // CAL. S3892
  Standard_Integer lr, ur, lc, uc;
  lr    = Matrix.LowerRow ();
  ur    = Matrix.UpperRow ();
  lc    = Matrix.LowerCol ();
  uc    = Matrix.UpperCol ();
  if ( (ur - lr + 1 != 4) || (uc - lc + 1 != 4) ) {
    V.Coord(X,Y,Z) ;
    VV.SetCoord(X,Y,Z) ;
    return VV ;
  }
  V.Coord(X,Y,Z) ;
  XX = X*Matrix(lr,lc)+Y*Matrix(lr,lc+1)+Z*Matrix(lr,lc+2) ;
  YY = X*Matrix(lr+1,lc)+Y*Matrix(lr+1,lc+1)+Z*Matrix(lr+1,lc+2) ;
  ZZ = X*Matrix(lr+2,lc)+Y*Matrix(lr+2,lc+1)+Z*Matrix(lr+2,lc+2) ;
  VV.SetCoord(XX,YY,ZZ) ; VV.Normalize() ;
  return VV ;
}


void V3d_View::Pan(const Standard_Integer Dx, const Standard_Integer Dy,const Quantity_Factor aZoomFactor) {

  Panning(Convert(Dx),Convert(Dy),aZoomFactor,Standard_True);

}

void V3d_View::Zoom (const Standard_Integer X1,
                     const Standard_Integer Y1,
                     const Standard_Integer X2,
                     const Standard_Integer Y2)
{

  Standard_Real dx = Standard_Real (X2-X1);
  Standard_Real dy = Standard_Real (Y2-Y1);
  if ( dx != 0. || dy != 0. ) {
    Standard_Real dzoom = Sqrt(dx*dx + dy*dy) / 100. + 1;
    dzoom = (dx > 0) ?  dzoom : 1./dzoom;
    SetZoom(dzoom, Standard_True);
  }
}

void V3d_View::Zoom (const Standard_Integer X1,
                     const Standard_Integer Y1)
{
  Standard_Real x,y;
  Center(x,y);
  Standard_Integer ix,iy;
  Convert(x,y,ix,iy);
  Zoom(ix,iy,X1,Y1);
}

// Defines the point (pixel) of zooming (for the method ZoomAtPoint()).
void V3d_View::StartZoomAtPoint(const Standard_Integer xpix, const Standard_Integer ypix)
{
    MyZoomAtPointX = xpix;
    MyZoomAtPointY = ypix;
}

// Zooms the model at a pixel defined by the method StartZoomAtPoint().
void V3d_View::ZoomAtPoint(const Standard_Integer mouseStartX,
                           const Standard_Integer mouseStartY,
                           const Standard_Integer mouseEndX,
                           const Standard_Integer mouseEndY)
{
    Standard_Boolean update;
    V3d_Coordinate X0, Y0, XS, YS;

    // Forbid any update.
    update = SetImmediateUpdate(Standard_False);

    // Get center point
    Center(X0, Y0);

    // Pan the point to the center of window.
    Convert(MyZoomAtPointX, MyZoomAtPointY, XS, YS);
    Panning(X0-XS, Y0-YS);

    // Zoom
    Standard_Real d = Standard_Real ((mouseEndX + mouseEndY) - (mouseStartX + mouseStartY));

    Standard_Real dzoom = fabs(d) / 100.0 + 1.0;
    dzoom = (d > 0) ?  dzoom : 1.0 / dzoom;
    Viewer_BadValue_Raise_if( dzoom <= 0.,"V3d_View::ZoomAtPoint, bad coefficient");

    Standard_Real Umin,Vmin,Umax,Vmax;
    MyViewMapping.WindowLimit(Umin,Vmin,Umax,Vmax);

    MyXwindowCenter = (Umin + Umax) / 2.0;
    MyYwindowCenter = (Vmin + Vmax) / 2.0;
    MyWindowWidth = Abs(Umax - Umin);
    MyWindowHeight = Abs(Vmax - Vmin);

    // Ensure that zoom will not be too small or too big.
    Standard_Real coef = dzoom;
    if (MyWindowWidth < coef * Precision::Confusion())
        coef = MyWindowWidth / Precision::Confusion();
    else if (MyWindowWidth > coef * 1e12)
        coef = MyWindowWidth / 1e12;
    if (MyWindowHeight < coef * Precision::Confusion())
        coef = MyWindowHeight / Precision::Confusion();
    else if (MyWindowHeight > coef * 1e12)
        coef = MyWindowHeight / 1e12;

    Standard_Real Dxv = MyWindowWidth / coef;
    Standard_Real Dyv = MyWindowHeight / coef;

    Umin = MyXwindowCenter - Dxv/2.0; Umax = MyXwindowCenter + Dxv/2.0;
    Vmin = MyYwindowCenter - Dyv/2.0; Vmax = MyYwindowCenter + Dyv/2.0;

    // Pan the point backwards.
    Dxv = (XS - X0) / coef;
    Dyv = (YS - Y0) / coef;

    MyXwindowCenter = (Umin + Umax) / 2.0;
    MyYwindowCenter = (Vmin + Vmax) / 2.0;
    MyWindowWidth = Abs(Umax - Umin);
    MyWindowHeight = Abs(Vmax - Vmin);

    Standard_Real Xrp,Yrp,Zrp;
    MyProjReferencePoint = MyViewMapping.ProjectionReferencePoint();
    MyProjReferencePoint.Coord(Xrp,Yrp,Zrp);

    Xrp = MyXwindowCenter - Dxv;
    Yrp = MyYwindowCenter - Dyv;

    Umin = Xrp - MyWindowWidth / 2.0; Umax = Xrp + MyWindowWidth / 2.0;
    Vmin = Yrp - MyWindowHeight / 2.0; Vmax = Yrp + MyWindowHeight / 2.0;

    // Set new reference plane coordintes of the window.
    MyViewMapping.SetWindowLimit(Umin,Vmin,Umax,Vmax);
    if (MyType != V3d_PERSPECTIVE)
    {
        MyProjReferencePoint.SetCoord(Xrp,Yrp,Zrp);
        MyViewMapping.SetProjectionReferencePoint(MyProjReferencePoint);
    }
    MyView->SetViewMapping(MyViewMapping);

    // Update the view.
    SetImmediateUpdate(update);
    ImmediateUpdate();
}

void V3d_View::AxialScale (const Standard_Integer Dx,
                           const Standard_Integer Dy,
                           const V3d_TypeOfAxe Axis)
{
  if( Dx != 0. || Dy != 0. ) {
    Standard_Real Sx, Sy, Sz;
    AxialScale( Sx, Sy, Sz );
    Standard_Real dscale = Sqrt(Dx*Dx + Dy*Dy) / 100. + 1;
    dscale = (Dx > 0) ?  dscale : 1./dscale;
    if( Axis == V3d_X ) Sx = dscale;
    if( Axis == V3d_Y ) Sy = dscale;
    if( Axis == V3d_Z ) Sz = dscale;
    SetAxialScale( Sx, Sy, Sz );
  }
}

void V3d_View::FitAll(const Handle(Aspect_Window)& aWindow,
                      const Standard_Real Xmin,
                      const Standard_Real Ymin,
                      const Standard_Real Xmax,
                      const Standard_Real Ymax)
{

  Standard_Real Umin,Umax,Vmin,Vmax,Xrp,Yrp,Zrp ;
  Standard_Real DxvOld,DyvOld,DxvNew,DyvNew,RapOld,RapNew ;
  Standard_Integer Xpixel,Ypixel;
  //Standard_Integer Xleft,Yup,Xright,Ylow ;

  Viewer_BadValue_Raise_if( (Xmin == Xmax) || (Ymin == Ymax) ,
    "V3d_View::FitAll, Window Size is NULL");

  MyProjReferencePoint = MyViewMapping.ProjectionReferencePoint() ;
  MyProjReferencePoint.Coord(Xrp,Yrp,Zrp) ;
  aWindow->Size(Xpixel,Ypixel);
  DxvOld = Xpixel; DyvOld = Ypixel;

  if( (DxvOld == 0.) || (DyvOld == 0.) ) return ;
  RapOld = DxvOld/DyvOld ;
  DxvNew = Abs(Xmax - Xmin) ; DyvNew = Abs(Ymax - Ymin) ;
  RapNew = DxvNew/DyvNew ;
  if( RapNew >= RapOld ) {
    DyvNew = DxvNew/RapOld ;
  } else {
    DxvNew = DyvNew*RapOld ;
  }
  Xrp = (Xmin + Xmax)/2. ; Yrp = (Ymin + Ymax)/2. ;
  Umin = Xrp - DxvNew/2. ; Vmin = Yrp - DyvNew/2. ;
  Umax = Xrp + DxvNew/2. ; Vmax = Yrp + DyvNew/2. ;
  MyViewMapping.SetWindowLimit(Umin,Vmin,Umax,Vmax) ;
  if( MyType != V3d_PERSPECTIVE ) {
    MyProjReferencePoint.SetCoord(Xrp,Yrp,Zrp) ;
    MyViewMapping.SetProjectionReferencePoint(MyProjReferencePoint);
  }
  MyView->SetViewMapping(MyViewMapping) ;
#ifdef IMP020300
  Update();
#else
  ImmediateUpdate();
#endif
}

#ifdef IMP250900
static Standard_Boolean zRotation = Standard_False;
#endif
void V3d_View::StartRotation(const Standard_Integer X,
                             const Standard_Integer Y,
                             const Quantity_Ratio zRotationThreshold)
{

  sx = X; sy = Y;
  Standard_Real x,y;
  Size(x,y);
  rx = Standard_Real(Convert(x));
  ry = Standard_Real(Convert(y));
  Gravity(gx,gy,gz);
  Rotate(0.,0.,0.,gx,gy,gz,Standard_True);
#ifdef IMP250900
  zRotation = Standard_False;
  if( zRotationThreshold > 0. ) {
    Standard_Real dx = Abs(sx - rx/2.);
    Standard_Real dy = Abs(sy - ry/2.);
    //  if( dx > rx/3. || dy > ry/3. ) zRotation = Standard_True;
    Standard_Real dd = zRotationThreshold * (rx + ry)/2.;
    if( dx > dd || dy > dd ) zRotation = Standard_True;
  }
#endif

}

void V3d_View::Rotation(const Standard_Integer X,
                        const Standard_Integer Y)
{

#ifdef IMP210600
  if( rx == 0. || ry == 0. ) {
    StartRotation(X,Y);
    return;
  }
#endif
#ifdef IMP250900
  Standard_Real dx=0.,dy=0.,dz=0.;
  if( zRotation ) {
    dz = atan2(Standard_Real(X)-rx/2., ry/2.-Standard_Real(Y)) -
      atan2(sx-rx/2.,ry/2.-sy);
  } else {
    dx = (Standard_Real(X) - sx) * Standard_PI/rx;
    dy = (sy - Standard_Real(Y)) * Standard_PI/ry;
  }
  Rotate(dx, dy, dz, gx, gy, gz, Standard_False);
#else
  Standard_Real dx = (Standard_Real(X - sx)) * Standard_PI;
  Standard_Real dy = (Standard_Real(sy - Y)) * Standard_PI;
  Rotate(dx/rx, dy/ry, 0., gx, gy, gz, Standard_False);
#endif
#ifdef IMP020300
  if( !myImmediateUpdate ) Update();
#else
  myImmediateUpdate = Standard_False;
  Rotate(dx/rx, dy/ry, 0., gx, gy, gz, Standard_False);
  ZFitAll (Zmargin);    //Don't do that, perf improvment
  myImmediateUpdate = Standard_True;
  ImmediateUpdate();
#endif
}

void V3d_View::SetAnimationModeOn () {
#ifdef G003
  if ( MyAnimationFlags & V3d_FLAG_ANIMATION ) {
    if( ComputedMode() ) {  // Deactivates computed mode during animation
      MyAnimationFlags |= V3d_FLAG_COMPUTATION;
      Standard_Boolean immediatUpdate = myImmediateUpdate;
      myImmediateUpdate = Standard_False;
      SetComputedMode(Standard_False);
      myImmediateUpdate = immediatUpdate;
    }
    MyView -> SetAnimationModeOn ( MyAnimationFlags & V3d_FLAG_DEGENERATION );
  }
#else
  MyView->SetAnimationModeOn();
#endif
}

void V3d_View::SetAnimationModeOff () {
#ifdef G003
  if ( MyAnimationFlags & V3d_FLAG_ANIMATION ) {
    MyView -> SetAnimationModeOff ();
    if ( MyAnimationFlags & V3d_FLAG_COMPUTATION ) {
      // Reactivates computed mode after animation
      MyAnimationFlags &= ~V3d_FLAG_COMPUTATION;
      SetComputedMode(Standard_True);
    } else if( MyAnimationFlags & V3d_FLAG_DEGENERATION ) {
      Update();
    }
  }
#else
  MyView->SetAnimationModeOff();
#endif
}

Standard_Boolean V3d_View::AnimationModeIsOn () const {
  return MyView->AnimationModeIsOn();
}

#ifdef G003
void V3d_View :: SetAnimationMode
(
 const Standard_Boolean anAnimationFlag,
 const Standard_Boolean aDegenerationFlag
 )
{

  if ( anAnimationFlag )
    MyAnimationFlags |= V3d_FLAG_ANIMATION;
  else
    MyAnimationFlags &= ~V3d_FLAG_ANIMATION;

  if ( aDegenerationFlag )
    MyAnimationFlags |= V3d_FLAG_DEGENERATION;
  else
    MyAnimationFlags &= ~V3d_FLAG_DEGENERATION;

}  // end V3d_View :: SetAnimationMode

Standard_Boolean V3d_View::AnimationMode( Standard_Boolean& isDegenerate ) const
{

  isDegenerate = MyAnimationFlags & V3d_FLAG_DEGENERATION;
  return MyAnimationFlags & V3d_FLAG_ANIMATION;
}
#endif

void V3d_View::SetDegenerateModeOn() {
#ifdef G003
  SetComputedMode(Standard_False);
#else
  MyView->SetDegenerateModeOn();
  ImmediateUpdate();
#endif
}

void V3d_View::SetDegenerateModeOff()
{
#ifdef G003
  SetComputedMode(Standard_True);
#else
  if(myComputedMode) {
    MyView->SetDegenerateModeOff();
    ImmediateUpdate();
  }
#endif
}

Standard_Boolean V3d_View::DegenerateModeIsOn() const
{
#ifdef G003
  return !ComputedMode();
#else
  return MyView->DegenerateModeIsOn();
#endif
}

#ifdef G003
void V3d_View :: SetComputedMode ( const Standard_Boolean aMode )
{

  if( aMode ) {
    if( myComputedMode ) {
      MyView -> SetComputedMode ( Standard_True );
      Update ();
    }
  } else {
    MyView -> SetComputedMode ( Standard_False );
    Update ();
  }

}  // end V3d_View :: SetComputedMode

Standard_Boolean V3d_View :: ComputedMode () const
{

  return MyView -> ComputedMode ();

}  // end V3d_View :: ComputedMode

void V3d_View :: SetBackFacingModel (
                                     const V3d_TypeOfBackfacingModel aModel)
{

  MyView -> SetBackFacingModel ( Visual3d_TypeOfBackfacingModel(aModel) );
  Redraw();
}  // end V3d_View :: SetBackFacingModel

V3d_TypeOfBackfacingModel V3d_View :: BackFacingModel () const {

  return V3d_TypeOfBackfacingModel(MyView -> BackFacingModel ());

}  // end V3d_View :: BackFacingModel
#endif

Standard_Boolean V3d_View::TransientManagerBeginDraw(const Standard_Boolean DoubleBuffer,const Standard_Boolean RetainMode) const {
  return Visual3d_TransientManager::BeginDraw(MyView,DoubleBuffer,RetainMode);
}

void V3d_View::TransientManagerClearDraw() const {
  Visual3d_TransientManager::ClearDraw(MyView);
}

Standard_Boolean V3d_View::TransientManagerBeginAddDraw() const {
  return Visual3d_TransientManager::BeginAddDraw(MyView);
}

void V3d_View::Init() {
  myComputedMode = MyViewer->ComputedMode();
#ifdef G003
  if( !myComputedMode || !MyViewer->DefaultComputedMode() ) {
    SetComputedMode(Standard_False);
  }
#else
  if(!myComputedMode)
    MyView->SetDegenerateModeOn();
  else
    if(!MyViewer->DefaultComputedMode()) MyView->SetDegenerateModeOn();
#endif

#ifdef IMP240100
  OSD_Environment env_walkthrow("CSF_WALKTHROUGH");
  env_walkthrow.SetName( "CSF_WALKTHROUGH" );
  if ( env_walkthrow.Value () != "" ) MyProjModel = V3d_TPM_WALKTHROUGH;
  else MyProjModel = V3d_TPM_SCREEN;
#endif

}

void V3d_View::SetPlotter(const Handle(Graphic3d_Plotter)& aPlotter) {
  MyPlotter = aPlotter;
}

void V3d_View::Plot()
{

  Viewer_BadValue_Raise_if( !MyPlotter.IsNull(), "view has no plotter");

  MyView->Plot(MyPlotter);
}

Standard_Real V3d_View::Tumble (const Standard_Integer NbImages, const Standard_Boolean AnimationMode)
{

  FullTimer.Reset ();
  FullTimer.Start ();

  if (AnimationMode) MyView->SetAnimationModeOn();
  Standard_Integer i;
  Standard_Real delta = 0.01;
  Standard_Real xangle, yangle;
  xangle = yangle = delta*int(NbImages/3);

  Rotate (0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Standard_True);
  for (i=1; i<=int(NbImages/3); i++)
    Rotate (delta*i, 0.0, 0.0, 0.0, 0.0, 0.0, Standard_False);
  for (i=1; i<=int(NbImages/3); i++)
    Rotate (xangle, delta*i, 0.0, 0.0, 0.0, 0.0, Standard_False);
  for (i=1; i<=int(NbImages/3); i++)
    Rotate (xangle, yangle, delta*i, 0.0, 0.0, 0.0, Standard_False);
  if (AnimationMode) MyView->SetAnimationModeOff();

  FullTimer.Stop ();
  cout << "For " << NbImages << " Images : " << endl;
  FullTimer.Show (cout);
  cout << flush;

  Standard_Real Seconds, CPUtime;
  Standard_Integer Minutes, Hours;

  FullTimer.Show (Seconds, Minutes, Hours, CPUtime);
  cout << "Result " << (AnimationMode ? "with " : "without ")
    << "display list : " << NbImages/CPUtime << " images/Second."
    << endl;

  return NbImages/CPUtime;

}

#define SCREENCOPY_FILENAME "screencopy3d.gif"
void V3d_View::ScreenCopy (const Handle(PlotMgt_PlotterDriver)& aPlotterDriver,
                           const Standard_Boolean fWhiteBackground,
                           const Quantity_Factor aPlotScale)
{
  TCollection_AsciiString aFileToDump;
  Handle(Aspect_WindowDriver) aWindowDriver =
#ifdef WNT
    new WNT_WDriver (Handle(WNT_Window)::DownCast(MyWindow));
#else
    new Xw_Driver (Handle(Xw_Window)::DownCast(MyWindow));
#endif // WNT
  Quantity_Factor    aScale;
  Quantity_Length    thePixel;
  Quantity_Parameter theWWidth, theWHeight;
  Quantity_Parameter thePWidth, thePHeight;
  aPlotterDriver -> WorkSpace (thePWidth, thePHeight);
  aWindowDriver  -> WorkSpace (theWWidth, theWHeight);
  thePixel = aWindowDriver -> Convert (1);

  if (theWWidth * theWHeight != 0.) {
    if (aPlotScale == 0.) {
      aScale = Min (thePWidth / theWWidth, thePHeight / theWHeight);
    } else {
      // To be changed !!!!!!!!!!!!!!!
      aScale = Min (thePWidth / theWWidth, thePHeight / theWHeight);
    }

    // Set default maps (color, type, etc) for plotter driver
    aPlotterDriver -> SetColorMap ( new Aspect_GenericColorMap () );
    aPlotterDriver -> SetTypeMap  ( new Aspect_TypeMap         () );
    aPlotterDriver -> SetWidthMap ( new Aspect_WidthMap        () );
    aPlotterDriver -> SetFontMap  ( new Aspect_FontMap         () );
    aPlotterDriver -> SetMarkMap  ( new Aspect_MarkMap         () );

    // Set backgroung to white, unihiglight if any
    Quantity_Parameter theRed, theGreen, theBlue;
    Handle(TColStd_HSequenceOfInteger) theFlags;
    Graphic3d_MapOfStructure  theStructures;
    BackgroundColor (Quantity_TOC_RGB, theRed, theGreen, theBlue);
    if (fWhiteBackground)
      SetBackgroundColor (Quantity_NOC_WHITE);
    MyView -> DisplayedStructures (theStructures);
    theFlags      = new TColStd_HSequenceOfInteger ();
    Graphic3d_MapIteratorOfMapOfStructure Iterator (theStructures);
    while (Iterator.More ()) {
      Handle(Graphic3d_Structure) aStructure = Iterator.Key();
      if (aStructure -> IsHighlighted()) {
        theFlags -> Append (1);
        aStructure -> UnHighlight ();
      } else {
        theFlags -> Append (0);
      }
      Iterator.Next ();
    }
    Redraw ();

    // Dump the view
    if (aPlotterDriver->IsKind(STANDARD_TYPE(PlotMgt_ImageDriver))) {
      aFileToDump  = aPlotterDriver->PlotFileName();
    } else {
      aFileToDump  = aPlotterDriver->SpoolDirectory();
      aFileToDump += SCREENCOPY_FILENAME;
    }
    MyWindow -> Dump (aFileToDump.ToCString());

    Standard_Integer theCurStruct = 1;
    Iterator = Graphic3d_MapIteratorOfMapOfStructure (theStructures);
    while (Iterator.More ()) {
      if (theFlags -> Value(theCurStruct))
        Iterator.Key() -> Highlight (Aspect_TOHM_COLOR);
      Iterator.Next ();
      theCurStruct++;
    }
    if (fWhiteBackground)
      SetBackgroundColor (Quantity_TOC_RGB, theRed, theGreen, theBlue);
    Redraw ();

    // Draw imagefile by plotter driver
    aPlotterDriver -> SetPixelSize (thePixel);
    aPlotterDriver -> BeginDraw ();
    aPlotterDriver -> DrawImageFile (
      aFileToDump.ToCString(), (float)(thePWidth / 2.), (float)(thePHeight / 2.), aScale);
    aPlotterDriver -> EndDraw ();
  }
}
#undef SCREENCOPY_FILENAME

#include <Aspect.hxx>
#include <Visual3d_Layer.hxx>

////////////////////////////////////////////////////////////////
Standard_Boolean V3d_View::Dump (const Standard_CString theFile,
                                 const Image_TypeOfImage theBufferType)
{
  Standard_Integer aWinWidth, aWinHeight;
  MyWindow->Size (aWinWidth, aWinHeight);

  Handle(Aspect_PixMap) aPixMap = ToPixMap (aWinWidth, aWinHeight, theBufferType);
  return !aPixMap.IsNull() && aPixMap->Dump (theFile);
}

////////////////////////////////////////////////////////////////
Standard_Boolean V3d_View::Dump (const Standard_CString theFile,
                                 const Aspect_FormatOfSheetPaper theFormat,
                                 const Image_TypeOfImage theBufferType)
{
  Standard_Boolean isDone = Standard_False;
  // convert Aspect_FormatOfSheetPaper size to pixel ...
  Quantity_Length anSPWidth, anSPHeight;
  Aspect::ValuesOfFOSP (theFormat, anSPWidth, anSPHeight);

  // adjusting to the ratio width/height ...
  Quantity_Length aWinWidth, aWinHeight;
  MyWindow->MMSize (aWinWidth, aWinHeight);
  Standard_Integer aPixelWidth, aPixelHeight;
  MyWindow->Size (aPixelWidth, aPixelHeight);

  Quantity_Factor aScale = Min (anSPWidth / aWinWidth, anSPHeight / aWinHeight);
  aPixelWidth  = Standard_Integer (aPixelWidth  * aScale);
  aPixelHeight = Standard_Integer (aPixelHeight * aScale);
  {
    Handle(Aspect_PixMap) aBitmap = ToPixMap (aPixelWidth, aPixelHeight, theBufferType);
    Standard_Real aGammaValue = 1.0;
    OSD_Environment anEnvGamma ("CSF_GAMMA_CORRECTION");
    TCollection_AsciiString strGamma (anEnvGamma.Value());
    if (!strGamma.IsEmpty()) aGammaValue = strGamma.RealValue();
    isDone = !aBitmap.IsNull() && aBitmap->Dump (theFile, aGammaValue);
  }
  return isDone;
}

////////////////////////////////////////////////////////////////
Handle(Image_PixMap) V3d_View::ToPixMap (const Standard_Integer  theWidth,
                                         const Standard_Integer  theHeight,
                                         const Image_TypeOfImage theBufferType,
                                         const Standard_Boolean  theIsForceCentred)
{
  // always prefer hardware accelerated offscreen buffer
  Graphic3d_CView* cView = (Graphic3d_CView* )MyView->CView();
  Graphic3d_PtrFrameBuffer aFBOPtr = NULL;
  Graphic3d_PtrFrameBuffer aPrevFBOPtr = (Graphic3d_PtrFrameBuffer )cView->ptrFBO;
  Standard_Integer aPrevFBOVPSizeX (0), aPrevFBOVPSizeY (0), aPrevFBOSizeXMax (0), aPrevFBOSizeYMax (0);
  if (aPrevFBOPtr != NULL)
  {
    MyView->FBOGetDimensions (aPrevFBOPtr,
                              aPrevFBOVPSizeX, aPrevFBOVPSizeY,
                              aPrevFBOSizeXMax, aPrevFBOSizeYMax);
    if (theWidth <= aPrevFBOSizeXMax && theHeight <= aPrevFBOSizeYMax)
    {
      MyView->FBOChangeViewport (aPrevFBOPtr, theWidth, theHeight);
      aFBOPtr = aPrevFBOPtr;
    }
  }

  if (aFBOPtr == NULL)
  {
    // Try to create hardware accelerated buffer
    aFBOPtr = MyView->FBOCreate (theWidth, theHeight);
  }
  cView->ptrFBO = aFBOPtr;

  // If hardware accelerated buffer - try to use onscreen buffer
  // Results may be bad!
  if (aFBOPtr == NULL)
  {
    // retrieve window sizes
    Standard_Integer aWinWidth, aWinHeight;
    MyWindow->Size (aWinWidth, aWinHeight);

    // technically we can reduce existing viewport...
    // but currently allow only dumping the window itself
    if (theWidth != aWinWidth || theHeight != aWinHeight)
    {
      return Handle(Image_PixMap)();
    }
  }

  //szv: save mapping
  Visual3d_ViewMapping prevMapping = MyView->ViewMapping();
  Standard_Real Umin, Vmin, Umax, Vmax;

  if (theIsForceCentred)
  {
    //szv: get mapping frame
    Standard_Real PUmin, PVmin, PUmax, PVmax;
    prevMapping.WindowLimit (PUmin, PVmin, PUmax, PVmax);

    //szv: calculate expansion
    Umin = PUmin; Vmin = PVmin; Umax = PUmax; Vmax = PVmax;
    Standard_Real oldWidth = (PUmax - PUmin), oldHeight = (PVmax - PVmin);
    Standard_Real newWidth = (oldHeight * theWidth) / theHeight;
    if (newWidth < oldWidth)
    {
      Standard_Real newHeight = (oldWidth * theHeight) / theWidth;
      // Expand height
      Standard_Real delta = 0.5 * (newHeight - oldHeight);
      Vmin = PVmin - delta;
      Vmax = PVmax + delta;
    }
    else
    {
      // Expand width
      Standard_Real delta = 0.5 * (newWidth - oldWidth);
      Umin = PUmin - delta;
      Umax = PUmax + delta;
    }

    //szv: apply expanded mapping
    MyViewMapping.SetWindowLimit (Umin, Vmin, Umax, Vmax);
    MyView->SetViewMapping (MyViewMapping);
  }

  //workround for rendering list of Over and Under Layers
  if (!MyLayerMgr.IsNull())
  {
    MyLayerMgr->Compute();
  }
  Redraw();

  //szv: restore mapping
  MyViewMapping = prevMapping;
  MyView->SetViewMapping (prevMapping);

  // allocate image buffer for dumping
  Image_CRawBufferData aRawBuffer;
  Handle(Image_PixMap) anImageBitmap = new Image_PixMap (theWidth, theHeight, theBufferType);
  anImageBitmap->AccessBuffer (aRawBuffer);
  if (!MyView->BufferDump (aRawBuffer))
  {
    // dump is failed!
    anImageBitmap = Handle(Image_PixMap)();
  }

  // FBO now useless, free resources
  if (aFBOPtr != aPrevFBOPtr)
  {
    MyView->FBORelease (aFBOPtr);
  }
  else if (aPrevFBOPtr != NULL)
  {
    MyView->FBOChangeViewport (aPrevFBOPtr, aPrevFBOVPSizeX, aPrevFBOVPSizeY);
  }
  cView->ptrFBO = aPrevFBOPtr;
  return anImageBitmap;
}
