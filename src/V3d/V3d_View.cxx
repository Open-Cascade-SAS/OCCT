// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

//GER61351  //GG_15/12/99 Add SetBackgroundColor() and BackgroundColor() methods


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
#include <Image_AlienPixMap.hxx>
#include <V3d.hxx>
#include <V3d_View.ixx>
#include <V3d_BadValue.hxx>
#include <V3d_StereoDumpOptions.hxx>
#include <Standard_ShortReal.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <Visual3d_TransientManager.hxx>
#include <Precision.hxx>

#include <Graphic3d_Structure.hxx>
#include <Graphic3d_MapIteratorOfMapOfStructure.hxx>
#include <Graphic3d_MapOfStructure.hxx>
#include <Graphic3d_TextureEnv.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_GraphicDriver.hxx>

// S3603
#include <Aspect_GenericColorMap.hxx>
#include <Aspect_TypeMap.hxx>
#include <Aspect_WidthMap.hxx>
#include <Aspect_MarkMap.hxx>
#include <Aspect_FontMap.hxx>
#include <TColStd_HSequenceOfInteger.hxx>

#define V3d_FLAG_COMPUTATION   0x00000004

// Perspective
#include <OSD_Environment.hxx>

/*----------------------------------------------------------------------*/
/*
* Constant
*/

#define DEUXPI (2. * M_PI)

/*----------------------------------------------------------------------*/
//-Constructors

V3d_View::V3d_View(const Handle(V3d_Viewer)& VM, const V3d_TypeOfView Type ) :
  MyProjModel(V3d_TPM_SCREEN),
  MyViewer(VM.operator->()),
  MyActiveLights(),
  MyViewContext (),
  myActiveLightsIterator(),
  SwitchSetFront(Standard_False),  
  MyTrsf (1, 4, 1, 4),
  myAutoZFitMode (Standard_True)
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

  // Visualization and Shading Model
  MyViewContext.SetModel ((MyView->Context ()).Model ());
  MyViewContext.SetVisualization ((MyView->Context ()).Visualization ());

  // Texture Mapping
  MyViewContext.SetSurfaceDetail (MyView->Context ().SurfaceDetail ());
  MyViewContext.SetTextureEnv (MyView->Context ().TextureEnv ());
  // } End of retrieval of the definition of ViewContext.

  MyBackground = VM->GetBackgroundColor() ;
  MyGradientBackground = VM->GetGradientBackground() ;

  // camera init
  Handle(Graphic3d_Camera) aCamera = new Graphic3d_Camera();
  aCamera->SetFOVy (45.0);
  aCamera->SetIOD (Graphic3d_Camera::IODType_Relative, 0.05);
  aCamera->SetZFocus (Graphic3d_Camera::FocusType_Relative, 1.0);
  SetCamera (aCamera);

  SetAxis (0.,0.,0.,1.,1.,1.);
  SetVisualization (VM->DefaultVisualization());
  SetShadingModel (VM->DefaultShadingModel());
  SetSurfaceDetail (VM->DefaultSurfaceDetail());
  SetTwist (0.);
  SetAt (0.,0.,0.);
  SetProj (VM->DefaultViewProj());
  SetSize (VM->DefaultViewSize());
  Standard_Real zsize = VM->DefaultViewSize();
  SetZSize (2.*zsize);
  SetZClippingDepth (0.);
  SetZClippingWidth (zsize);
  SetZCueingDepth (0.);
  SetZCueingWidth (zsize);
  SetDepth (VM->DefaultViewSize()/2.0);
  SetCenter (0.,0.);
  SetViewMappingDefault();
  VM->AddView (this);
  Init();
  myImmediateUpdate = Standard_True;

  aCamera->SetProjectionType ((Type == V3d_ORTHOGRAPHIC)
    ? Graphic3d_Camera::Projection_Orthographic
    : Graphic3d_Camera::Projection_Perspective);

  MyTransparencyFlag = Standard_False;
}

/*----------------------------------------------------------------------*/

V3d_View::V3d_View(const Handle(V3d_Viewer)& theVM,const Handle(V3d_View)& theView) :
MyProjModel(V3d_TPM_SCREEN),
MyViewer(theVM.operator->()),
MyActiveLights(),
MyViewContext (),
myActiveLightsIterator(),
SwitchSetFront(Standard_False),
MyTrsf (1, 4, 1, 4), 
myAutoZFitMode (Standard_True)
{
  Handle(Visual3d_View) aFromView = theView->View();

  myImmediateUpdate = Standard_False;
  MyView = new Visual3d_View (MyViewer->Viewer());

  for (theView->InitActiveLights(); theView->MoreActiveLights(); theView->NextActiveLights())
  {
    MyActiveLights.Append (theView->ActiveLight());
  }

  MyViewContext = aFromView->Context() ;

  SetCamera (new Graphic3d_Camera (theView->Camera()));

  MyBackground = aFromView->Background() ;
  MyGradientBackground = aFromView->GradientBackground();

  MyView->SetContext (MyViewContext) ;

  SetAxis (0.0, 0.0, 0.0, 1.0, 1.0, 1.0);

  theVM->AddView (this);

  Init();

  myImmediateUpdate = Standard_True;
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
  // AGV: Method V3d_View::SetWindow() should assign the field MyWindow before
  // calling Redraw(). Otherwise it is impossible to call certain methods of
  // V3d_View like Convert() inside the context of Redraw(),
  // particularly in class NIS_View.
  MyWindow = TheWindow;
  // SetWindow carries out SetRatio and modifies
  MyView->SetContext(MyViewContext) ;
  MyView->SetBackground(MyBackground) ;
  MyViewer->SetViewOn(this) ;
  MyView->Redraw() ;
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
  // AGV: Method V3d_View::SetWindow() should assign the field MyWindow before
  // calling Redraw(). Otherwise it is impossible to call certain methods of
  // V3d_View like Convert() inside the context of Redraw(),
  // particularly in class NIS_View.
  MyWindow = aWindow;
  MyView->SetWindow(aWindow, aContext, aDisplayCB, aClientData) ;
  MyView->SetContext(MyViewContext) ;
  MyView->SetBackground(MyBackground) ;
  MyViewer->SetViewOn(this) ;
  MyView->Redraw() ;
}
// RIC120302

/*----------------------------------------------------------------------*/

void V3d_View::Remove() const
{
  MyViewer->DelView (this);
  MyView->Remove();
  Handle(Aspect_Window)& aWin = const_cast<Handle(Aspect_Window)&> (MyWindow);
  aWin.Nullify();
}

/*----------------------------------------------------------------------*/

void V3d_View::Update() const
{
  if( MyView->IsDefined() )  MyView->Update() ;
}

/*----------------------------------------------------------------------*/

void V3d_View::Redraw() const
{
  if( MyView->IsDefined() ) MyView->Redraw() ;
}

/*----------------------------------------------------------------------*/

void V3d_View::Redraw(const Standard_Integer xc,const Standard_Integer yc,
                      const Standard_Integer width,const Standard_Integer height) const
{
  if( MyView->IsDefined() ) MyView->Redraw(xc,yc,width,height) ;
}

/*----------------------------------------------------------------------*/

Standard_Boolean V3d_View::IsEmpty() const
{
  Standard_Boolean TheStatus = Standard_True ;
  if( MyView->IsDefined() ) {
    Standard_Integer Nstruct = MyView->NumberOfDisplayedStructures() ;
    if( Nstruct > 0 ) TheStatus = Standard_False ;
  }
  return (TheStatus) ;
}

/*----------------------------------------------------------------------*/

void V3d_View::UpdateLights() const
{
  MyView->SetContext(MyViewContext);
  Update();
}

/*----------------------------------------------------------------------*/

void V3d_View::DoMapping()
{
  if( MyView->IsDefined() ) {
    (MyView->Window())->DoMapping() ;
  }
}

/*----------------------------------------------------------------------*/

void V3d_View::MustBeResized()
{
  if ( !MyLayerMgr.IsNull() )
    MyLayerMgr->Resized();

  if( MyView->IsDefined() ) {
    MyView->Resized() ;
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
  SetBackgroundColor( C );
}

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

/*----------------------------------------------------------------------*/

void V3d_View::SetBackgroundColor(const Quantity_NameOfColor Name)
{
  Quantity_Color C( Name );
  SetBackgroundColor( C );
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
                                   const Standard_Boolean update)
{
  Quantity_Color Color1, Color2;
  MyGradientBackground.Colors( Color1, Color2 );
  MyGradientBackground.SetColors( Color1, Color2, FillStyle );
  if( MyView->IsDefined() )
    MyView->SetBgGradientStyle( FillStyle, update ) ;
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

void V3d_View::SetAxis(const Standard_Real X, const Standard_Real Y, const Standard_Real Z, const Standard_Real Vx, const Standard_Real Vy, const Standard_Real Vz)
{
  Standard_Real D,Nx = Vx,Ny = Vy,Nz = Vz ;

  D = Sqrt( Vx*Vx + Vy*Vy + Vz*Vz ) ;
  V3d_BadValue_Raise_if ( D <= 0. , "V3d_View::SetAxis, bad axis");
  Nx /= D ; Ny /= D ; Nz /= D ;
  MyDefaultViewPoint.SetCoord(X,Y,Z) ;
  MyDefaultViewAxis.SetCoord(Nx,Ny,Nz) ;
}

/*----------------------------------------------------------------------*/

void V3d_View::SetShadingModel(const V3d_TypeOfShadingModel Model)
{
  MyViewContext.SetModel((Visual3d_TypeOfModel) Model) ;
  MyView->SetContext(MyViewContext) ;
}

/*----------------------------------------------------------------------*/

void V3d_View::SetSurfaceDetail(const V3d_TypeOfSurfaceDetail Model)
{
  MyViewContext.SetSurfaceDetail((Visual3d_TypeOfSurfaceDetail) Model) ;
  MyView->SetContext(MyViewContext) ;
}

/*----------------------------------------------------------------------*/

void V3d_View::SetTextureEnv(const Handle(Graphic3d_TextureEnv)& ATexture)
{
  MyViewContext.SetTextureEnv(ATexture) ;
  MyView->SetContext(MyViewContext) ;
}

/*----------------------------------------------------------------------*/

void V3d_View::SetVisualization(const V3d_TypeOfVisualization Mode)
{
  MyViewContext.SetVisualization((Visual3d_TypeOfVisualization) Mode);
  MyView->SetContext(MyViewContext) ;
}

/*----------------------------------------------------------------------*/

void V3d_View::SetFront()
{
  gp_Ax3 a = MyViewer->PrivilegedPlane();
  Standard_Real xo, yo, zo, vx, vy, vz, xu, yu, zu;

  a.Direction().Coord(vx,vy,vz);
  a.YDirection().Coord(xu,yu,zu);
  a.Location().Coord(xo,yo,zo);

  myCamera->BeginUpdate();
  myCamera->SetCenter (gp_Pnt (xo, yo, zo));
  if(SwitchSetFront)
    myCamera->SetDirection (gp_Dir (vx, vy, vz));
  else
    myCamera->SetDirection (gp_Dir (vx, vy, vz).Reversed());
  myCamera->SetUp (gp_Dir (xu, yu, zu));

  AutoZFit();
  myCamera->EndUpdate();

  SwitchSetFront = !SwitchSetFront;

  ImmediateUpdate();
}

/*----------------------------------------------------------------------*/

void V3d_View::Rotate (const Standard_Real ax, 
                       const Standard_Real ay, 
                       const Standard_Real az, 
                       const Standard_Boolean Start)
{
  Standard_Real Ax = ax;
  Standard_Real Ay = ay;
  Standard_Real Az = az;

  if( Ax > 0. ) while ( Ax > DEUXPI ) Ax -= DEUXPI;
  else if( Ax < 0. ) while ( Ax < -DEUXPI ) Ax += DEUXPI;
  if( Ay > 0. ) while ( Ay > DEUXPI ) Ay -= DEUXPI;
  else if( Ay < 0. ) while ( Ay < -DEUXPI ) Ay += DEUXPI;
  if( Az > 0. ) while ( Az > DEUXPI ) Az -= DEUXPI;
  else if( Az < 0. ) while ( Az < -DEUXPI ) Az += DEUXPI;

  if (Start)
  {
    myCamStartOpUp = myCamera->Up();
    myCamStartOpEye = myCamera->Eye();
    myCamStartOpCenter = myCamera->Center();
  }

  myCamera->BeginUpdate();
  myCamera->SetUp (myCamStartOpUp);
  myCamera->SetEye (myCamStartOpEye);
  myCamera->SetCenter (myCamStartOpCenter);

  // rotate camera around 3 initial axes
  gp_Dir aBackDir (gp_Vec (myCamStartOpCenter, myCamStartOpEye));
  gp_Dir aXAxis (myCamStartOpUp.Crossed (aBackDir));
  gp_Dir aYAxis (aBackDir.Crossed (aXAxis));
  gp_Dir aZAxis (aXAxis.Crossed (aYAxis));

  gp_Trsf aRot[3], aTrsf;
  aRot[0].SetRotation (gp_Ax1 (myCamStartOpCenter, aYAxis), -Ax);
  aRot[1].SetRotation (gp_Ax1 (myCamStartOpCenter, aXAxis), Ay);
  aRot[2].SetRotation (gp_Ax1 (myCamStartOpCenter, aZAxis), Az);
  aTrsf.Multiply (aRot[0]);
  aTrsf.Multiply (aRot[1]);
  aTrsf.Multiply (aRot[2]);

  myCamera->Transform (aTrsf);

  myCamera->EndUpdate();

  AutoZFit();

  ImmediateUpdate();
}

/*----------------------------------------------------------------------*/

void V3d_View::Rotate(const Standard_Real ax, const Standard_Real ay, const Standard_Real az,
                      const Standard_Real X, const Standard_Real Y, const Standard_Real Z, const Standard_Boolean Start)
{

  Standard_Real Ax = ax ;
  Standard_Real Ay = ay ;
  Standard_Real Az = az ;

  if( Ax > 0. ) while ( Ax > DEUXPI ) Ax -= DEUXPI ;
  else if( Ax < 0. ) while ( Ax < -DEUXPI ) Ax += DEUXPI ;
  if( Ay > 0. ) while ( Ay > DEUXPI ) Ay -= DEUXPI ;
  else if( Ay < 0. ) while ( Ay < -DEUXPI ) Ay += DEUXPI ;
  if( Az > 0. ) while ( Az > DEUXPI ) Az -= DEUXPI ;
  else if( Az < 0. ) while ( Az < -DEUXPI ) Az += DEUXPI ;

  if (Start)
  {
    myGravityReferencePoint.SetCoord (X, Y, Z);
    myCamStartOpUp = myCamera->Up();
    myCamStartOpEye = myCamera->Eye();
    myCamStartOpCenter = myCamera->Center();
  }

  const Graphic3d_Vertex& aVref = myGravityReferencePoint;

  myCamera->BeginUpdate();
  myCamera->SetUp (myCamStartOpUp);
  myCamera->SetEye (myCamStartOpEye);
  myCamera->SetCenter (myCamStartOpCenter);

  // rotate camera around 3 initial axes
  gp_Pnt aRCenter (aVref.X(), aVref.Y(), aVref.Z());

  gp_Dir aZAxis (myCamera->Direction().Reversed());
  gp_Dir aYAxis (myCamera->Up());
  gp_Dir aXAxis (aYAxis.Crossed (aZAxis)); 

  gp_Trsf aRot[3], aTrsf;
  aRot[0].SetRotation (gp_Ax1 (aRCenter, aYAxis), -Ax);
  aRot[1].SetRotation (gp_Ax1 (aRCenter, aXAxis), Ay);
  aRot[2].SetRotation (gp_Ax1 (aRCenter, aZAxis), Az);
  aTrsf.Multiply (aRot[0]);
  aTrsf.Multiply (aRot[1]);
  aTrsf.Multiply (aRot[2]);

  myCamera->Transform (aTrsf);
  myCamera->EndUpdate();

  AutoZFit();

  ImmediateUpdate();
}

/*----------------------------------------------------------------------*/

void V3d_View::Rotate(const V3d_TypeOfAxe Axe, const Standard_Real angle, const Standard_Boolean Start)
{
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

  if( Angle > 0. ) while ( Angle > DEUXPI ) Angle -= DEUXPI ;
  else if( Angle < 0. ) while ( Angle < -DEUXPI ) Angle += DEUXPI ;

  if (Start)
  {
    myGravityReferencePoint.SetCoord (X, Y, Z);
    myCamStartOpUp = myCamera->Up();
    myCamStartOpEye = myCamera->Eye();
    myCamStartOpCenter = myCamera->Center();

    switch (Axe) {
    case V3d_X :
      myViewAxis.SetCoord(1.,0.,0.) ;
      break ;
    case V3d_Y :
      myViewAxis.SetCoord(0.,1.,0.) ;
      break ;
    case V3d_Z :
      myViewAxis.SetCoord(0.,0.,1.) ;
      break ;
    }

    myCamStartOpUp = myCamera->Up();
    myCamStartOpEye = myCamera->Eye();
    myCamStartOpCenter = myCamera->Center();
  }

  const Graphic3d_Vertex& aVref = myGravityReferencePoint;

  myCamera->BeginUpdate();
  myCamera->SetUp (myCamStartOpUp);
  myCamera->SetEye (myCamStartOpEye);
  myCamera->SetCenter (myCamStartOpCenter);

  // rotate camera around passed axis
  gp_Trsf aRotation;
  gp_Pnt aRCenter (aVref.X(), aVref.Y(), aVref.Z());
  gp_Dir aRAxis ((Axe == V3d_X) ? 1.0 : 0.0,
                  (Axe == V3d_Y) ? 1.0 : 0.0,
                  (Axe == V3d_Z) ? 1.0 : 0.0);

  aRotation.SetRotation (gp_Ax1 (aRCenter, aRAxis), Angle);
  myCamera->Transform (aRotation);

  myCamera->EndUpdate();

  AutoZFit();

  ImmediateUpdate();
}

/*----------------------------------------------------------------------*/

void V3d_View::Rotate(const Standard_Real angle, const Standard_Boolean Start)
{
  Standard_Real Angle = angle;

  if( Angle > 0. ) while ( Angle > DEUXPI ) Angle -= DEUXPI ;
  else if( Angle < 0. ) while ( Angle < -DEUXPI ) Angle += DEUXPI ;

  if( Start ) {
    myCamStartOpUp = myCamera->Up();
    myCamStartOpEye = myCamera->Eye();
    myCamStartOpCenter = myCamera->Center();
  }

  const Graphic3d_Vertex& aPnt = MyDefaultViewPoint;
  const Graphic3d_Vector& anAxis = MyDefaultViewAxis;

  myCamera->BeginUpdate();
  myCamera->SetUp (myCamStartOpUp);
  myCamera->SetEye (myCamStartOpEye);
  myCamera->SetCenter (myCamStartOpCenter);

  gp_Trsf aRotation;
  gp_Pnt aRCenter (aPnt.X(), aPnt.Y(), aPnt.Z());
  gp_Dir aRAxis (anAxis.X(), anAxis.Y(), anAxis.Z());
  aRotation.SetRotation (gp_Ax1 (aRCenter, aRAxis), Angle);
  myCamera->Transform (aRotation);

  myCamera->EndUpdate();

  AutoZFit();

  ImmediateUpdate();
}

/*----------------------------------------------------------------------*/

void V3d_View::Turn(const Standard_Real ax, const Standard_Real ay, const Standard_Real az, const Standard_Boolean Start)
{
  Standard_Real Ax = ax;
  Standard_Real Ay = ay;
  Standard_Real Az = az;

  if( Ax > 0. ) while ( Ax > DEUXPI ) Ax -= DEUXPI ;
  else if( Ax < 0. ) while ( Ax < -DEUXPI ) Ax += DEUXPI ;
  if( Ay > 0. ) while ( Ay > DEUXPI ) Ay -= DEUXPI ;
  else if( Ay < 0. ) while ( Ay < -DEUXPI ) Ay += DEUXPI ;
  if( Az > 0. ) while ( Az > DEUXPI ) Az -= DEUXPI ;
  else if( Az < 0. ) while ( Az < -DEUXPI ) Az += DEUXPI ;

  if( Start ) {
    myCamStartOpUp = myCamera->Up();
    myCamStartOpEye = myCamera->Eye();
    myCamStartOpCenter = myCamera->Center();
  }

  myCamera->BeginUpdate();
  myCamera->SetUp (myCamStartOpUp);
  myCamera->SetEye (myCamStartOpEye);
  myCamera->SetCenter (myCamStartOpCenter);

  // rotate camera around 3 initial axes
  gp_Pnt aRCenter = myCamera->Eye();
  gp_Dir aZAxis (myCamera->Direction().Reversed());
  gp_Dir aYAxis (myCamera->Up());
  gp_Dir aXAxis (aYAxis.Crossed (aZAxis)); 

  gp_Trsf aRot[3], aTrsf;
  aRot[0].SetRotation (gp_Ax1 (aRCenter, aYAxis), -Ax);
  aRot[1].SetRotation (gp_Ax1 (aRCenter, aXAxis), Ay);
  aRot[2].SetRotation (gp_Ax1 (aRCenter, aZAxis), Az);
  aTrsf.Multiply (aRot[0]);
  aTrsf.Multiply (aRot[1]);
  aTrsf.Multiply (aRot[2]);

  myCamera->Transform (aTrsf);
  myCamera->EndUpdate();

  AutoZFit();

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

void V3d_View::Turn(const Standard_Real angle, const Standard_Boolean Start)
{
  Standard_Real Angle = angle ;

  if( Angle > 0. ) while ( Angle > DEUXPI ) Angle -= DEUXPI ;
  else if( Angle < 0. ) while ( Angle < -DEUXPI ) Angle += DEUXPI ;

  if( Start ) {
    myCamStartOpUp = myCamera->Up();
    myCamStartOpEye = myCamera->Eye();
    myCamStartOpCenter = myCamera->Center();
  }

  myCamera->BeginUpdate();
  myCamera->SetUp (myCamStartOpUp);
  myCamera->SetEye (myCamStartOpEye);
  myCamera->SetCenter (myCamStartOpCenter);

  const Graphic3d_Vector& anAxis = MyDefaultViewAxis;

  gp_Trsf aRotation;
  gp_Pnt aRCenter = myCamera->Eye();
  gp_Dir aRAxis (anAxis.X(), anAxis.Y(), anAxis.Z());
  aRotation.SetRotation (gp_Ax1 (aRCenter, aRAxis), Angle);
  myCamera->Transform (aRotation);

  myCamera->EndUpdate();

  AutoZFit();

  ImmediateUpdate();
}

void V3d_View::SetTwist(const Standard_Real angle)
{
  Standard_Real Angle = angle ;
  Standard_Boolean TheStatus;

  if( Angle > 0. ) while ( Angle > DEUXPI ) Angle -= DEUXPI ;
  else if( Angle < 0. ) while ( Angle < -DEUXPI ) Angle += DEUXPI ;

  gp_Dir aReferencePlane (myCamera->Direction().Reversed());
  gp_Dir anUp;

  anUp = gp_Dir (0.0, 0.0, 1.0);

  TheStatus = ScreenAxis(aReferencePlane, anUp,
    myXscreenAxis,myYscreenAxis,myZscreenAxis) ;
  if( !TheStatus ) {
    anUp = gp_Dir (0.0, 1.0, 0.0);
    TheStatus = ScreenAxis(aReferencePlane, anUp,
      myXscreenAxis,myYscreenAxis,myZscreenAxis) ;
  }
  if( !TheStatus ) {
    anUp = gp_Dir (1.0, 0.0, 0.0);
    TheStatus = ScreenAxis(aReferencePlane, anUp,
      myXscreenAxis,myYscreenAxis,myZscreenAxis) ;
  }

  V3d_BadValue_Raise_if( !TheStatus,"V3d_ViewSetTwist, alignment of Eye,At,Up,");
  
  gp_Pnt aRCenter = myCamera->Center();
  gp_Dir aZAxis (myCamera->Direction().Reversed());

  gp_Trsf aTrsf;
  aTrsf.SetRotation (gp_Ax1 (aRCenter, aZAxis), Angle);

  Standard_Real myYscreenAxisX, myYscreenAxisY, myYscreenAxisZ;
  myYscreenAxis.Coord (myYscreenAxisX, myYscreenAxisY, myYscreenAxisZ);
    
  myCamera->BeginUpdate();
  myCamera->SetUp (gp_Dir (myYscreenAxisX, myYscreenAxisY, myYscreenAxisZ));
  myCamera->Transform (aTrsf);
  myCamera->EndUpdate();

  AutoZFit();

  ImmediateUpdate();
}

void V3d_View::SetAutoZFitMode (Standard_Boolean theMode)
{
  myAutoZFitMode = theMode;
}

Standard_Boolean V3d_View::AutoZFitMode () const
{
  return myAutoZFitMode;
}

void V3d_View::SetEye(const Standard_Real X,const Standard_Real Y,const Standard_Real Z)
{
  Standard_Real Angle;
  Angle = Twist();

  myCamera->SetEye (gp_Pnt (X, Y, Z));

  Standard_Boolean update = myImmediateUpdate;
  myImmediateUpdate = Standard_False;

  SetTwist(Angle);

  AutoZFit();

  myImmediateUpdate = update;
  ImmediateUpdate();
}

void V3d_View::SetDepth(const Standard_Real Depth)
{
  V3d_BadValue_Raise_if (Depth == 0. ,"V3d_View::SetDepth, bad depth");

  if( Depth > 0. )
  {
    // Move eye using center (target) as anchor.
    myCamera->SetDistance (Depth);
  }
  else
  { 
    // Move the view ref point instead of the eye.
    gp_Vec aDir (myCamera->Direction());
    gp_Pnt aCameraEye = myCamera->Eye();
    gp_Pnt aCameraCenter = aCameraEye.Translated (aDir.Multiplied (Abs (Depth)));      
      
    myCamera->SetCenter (aCameraCenter);
  }

  AutoZFit();

  ImmediateUpdate();
}


void V3d_View::SetProj( const Standard_Real Vx,const Standard_Real Vy, const Standard_Real Vz )
{
  Standard_Real Angle ;

  V3d_BadValue_Raise_if( Sqrt(Vx*Vx + Vy*Vy + Vz*Vz) <= 0.,
    "V3d_View::SetProj, null projection vector");

  Angle = Twist();

  myCamera->SetDirection (gp_Dir (Vx, Vy, Vz).Reversed());

  Standard_Boolean update = myImmediateUpdate;
  myImmediateUpdate = Standard_False;

  if( MyProjModel == V3d_TPM_SCREEN ) SetTwist(Angle) ;

  AutoZFit();

  myImmediateUpdate = update;
  ImmediateUpdate();
}

void V3d_View::SetProj( const V3d_TypeOfOrientation Orientation )
{
  Standard_Real Xpn=0;
  Standard_Real Ypn=0;
  Standard_Real Zpn=0;

  switch (Orientation) {
  case V3d_Zpos :
    Ypn = 1.;
    break;
  case V3d_Zneg :
    Ypn = -1.;
    break;
  default:
    Zpn = 1.;
  }

  const Graphic3d_Vector& aBck = V3d::GetProjAxis (Orientation);
  myCamera->BeginUpdate();
  myCamera->SetCenter (gp_Pnt (0, 0, 0));
  myCamera->SetDirection (gp_Dir (aBck.X(), aBck.Y(), aBck.Z()).Reversed());
  myCamera->SetUp (gp_Dir (Xpn, Ypn, Zpn));
  myCamera->EndUpdate();

  AutoZFit();

  ImmediateUpdate();
}

void V3d_View::SetAt(const Standard_Real X,const Standard_Real Y,const Standard_Real Z)
{
  Standard_Real Angle;

  Angle = Twist();

  myCamera->SetCenter (gp_Pnt (X, Y, Z));
  Standard_Boolean update = myImmediateUpdate;
  myImmediateUpdate = Standard_False;

  SetTwist(Angle) ;

  AutoZFit();

  myImmediateUpdate = update;
  ImmediateUpdate();
}

void V3d_View::SetUp(const Standard_Real Vx,const Standard_Real Vy,const Standard_Real Vz)
{
  Standard_Boolean TheStatus ;
  V3d_BadValue_Raise_if( Sqrt(Vx*Vx + Vy*Vy + Vz*Vz) <= 0. ,
    "V3d_View::SetUp, nullUp vector");

  gp_Dir aReferencePlane (myCamera->Direction().Reversed());
  gp_Dir anUp (Vx, Vy, Vz);

  TheStatus = ScreenAxis(aReferencePlane,anUp,
    myXscreenAxis,myYscreenAxis,myZscreenAxis) ;
  if( !TheStatus ) {
    anUp = gp_Dir (0.0, 0.0, 1.0);
    TheStatus = ScreenAxis(aReferencePlane,anUp,
      myXscreenAxis,myYscreenAxis,myZscreenAxis) ;
  }
  if( !TheStatus ) {
    anUp = gp_Dir (0.0, 1.0, 0.0);
    TheStatus = ScreenAxis(aReferencePlane,anUp,
      myXscreenAxis,myYscreenAxis,myZscreenAxis) ;
  }
  if( !TheStatus ) {
    anUp = gp_Dir (1.0, 0.0, 0.0);
    TheStatus = ScreenAxis(aReferencePlane,anUp,
      myXscreenAxis,myYscreenAxis,myZscreenAxis) ;
  }
  V3d_BadValue_Raise_if( !TheStatus,"V3d_View::Setup, alignment of Eye,At,Up");

  Standard_Real myYscreenAxisX, myYscreenAxisY, myYscreenAxisZ;
  myYscreenAxis.Coord (myYscreenAxisX, myYscreenAxisY, myYscreenAxisZ);

  myCamera->BeginUpdate();
  myCamera->SetUp (gp_Dir (myYscreenAxisX, myYscreenAxisY, myYscreenAxisZ));
  AutoZFit();
  myCamera->EndUpdate();

  ImmediateUpdate();
}

void V3d_View::SetUp( const V3d_TypeOfOrientation Orientation )
{
  Standard_Boolean TheStatus ;

  gp_Dir aReferencePlane (myCamera->Direction().Reversed());
  gp_Dir anUp;

  const Graphic3d_Vector& aViewReferenceUp = V3d::GetProjAxis(Orientation) ;
  anUp = gp_Dir (aViewReferenceUp.X(), aViewReferenceUp.Y(), aViewReferenceUp.Z());

  TheStatus = ScreenAxis(aReferencePlane,anUp,
    myXscreenAxis,myYscreenAxis,myZscreenAxis) ;
  if( !TheStatus ) {
    anUp = gp_Dir (0.,0.,1.);
    TheStatus = ScreenAxis(aReferencePlane,anUp,
      myXscreenAxis,myYscreenAxis,myZscreenAxis) ;
  }
  if( !TheStatus ) {
    anUp = gp_Dir (0.,1.,0.);
    TheStatus = ScreenAxis(aReferencePlane,anUp,
      myXscreenAxis,myYscreenAxis,myZscreenAxis) ;
  }
  if( !TheStatus ) {
    anUp = gp_Dir (1.,0.,0.);
    TheStatus = ScreenAxis(aReferencePlane,anUp,
      myXscreenAxis,myYscreenAxis,myZscreenAxis) ;
  }
  V3d_BadValue_Raise_if( !TheStatus, "V3d_View::SetUp, alignment of Eye,At,Up");

  Standard_Real myYscreenAxisX, myYscreenAxisY, myYscreenAxisZ;
  myYscreenAxis.Coord (myYscreenAxisX, myYscreenAxisY, myYscreenAxisZ);

  myCamera->BeginUpdate();
  myCamera->SetUp (gp_Dir (myYscreenAxisX, myYscreenAxisY, myYscreenAxisZ));
  AutoZFit();
  myCamera->EndUpdate();

  ImmediateUpdate();
}

void V3d_View::SetViewOrientationDefault()
{
  MyView->SetViewOrientationDefault() ;

  ImmediateUpdate();
}

void V3d_View::ResetViewOrientation()
{
  MyView->ViewOrientationReset() ;

  ImmediateUpdate();
}

void V3d_View::Reset( const Standard_Boolean update )
{
  Handle(Graphic3d_Camera) aDefaultCamera = MyView->DefaultCamera();

  if (!aDefaultCamera.IsNull())
  {
    myCamera->BeginUpdate();
    myCamera->CopyMappingData (aDefaultCamera);
    myCamera->CopyOrientationData (aDefaultCamera);
    myCamera->EndUpdate();
  }

  AutoZFit();
  SwitchSetFront = Standard_False;

  if( !myImmediateUpdate && update ) Update();
}

void V3d_View::Panning(const Standard_Real Dx, const Standard_Real Dy, const Quantity_Factor aZoomFactor, const Standard_Boolean Start)
{
  V3d_BadValue_Raise_if( aZoomFactor <= 0.,"V3d_View::Panning, bad zoom factor");

  if( Start ) {
    myCamStartOpEye = myCamera->Eye();
    myCamStartOpCenter = myCamera->Center();
    myCamProjectionShift = myCamera->ProjectionShift();
  }

  myCamera->BeginUpdate();
  myCamera->SetEye (myCamStartOpEye);
  myCamera->SetCenter (myCamStartOpCenter);
  myCamera->SetProjectionShift (myCamProjectionShift);
  PanCamera (-Dx, -Dy);
  gp_Pnt aViewDims = myCamera->ViewDimensions();
  ZoomCamera (aViewDims.X() / aZoomFactor, aViewDims.Y() / aZoomFactor);
  myCamera->EndUpdate();

  ImmediateUpdate();
}

void V3d_View::SetCenter(const Standard_Integer X, const Standard_Integer Y)
{
  Standard_Real x,y;
  Convert(X,Y,x,y);
  SetCenter(x,y);
}

void V3d_View::SetCenter(const Standard_Real Xc, const Standard_Real Yc)
{
  myCamera->SetProjectionShift (gp_Pnt (-Xc, -Yc, 0.0)); 

  ImmediateUpdate();
}

void V3d_View::SetSize(const Standard_Real Size)
{
  V3d_BadValue_Raise_if(  Size  <= 0.,
    "V3d_View::SetSize, Window Size is NULL");

  myCamera->BeginUpdate();
  myCamera->SetScale (Size);
  AutoZFit();
  myCamera->EndUpdate();

  ImmediateUpdate();
}

void V3d_View::SetZSize(const Standard_Real Size)
{
  Standard_Real Zmax = Size/2.;

  Standard_Real aDistance = myCamera->Distance();

  if( Size <= 0. ) {
    Zmax = aDistance;
  }

  Standard_Real Front = MyViewContext.ZClippingFrontPlane();
  Standard_Real Back  = MyViewContext.ZClippingBackPlane();

  myCamera->SetZFar (Zmax + aDistance * 2.0);
  myCamera->SetZNear (-Zmax + aDistance);

  if (MyViewContext.FrontZClippingIsOn()  ||
      MyViewContext.BackZClippingIsOn())
  {
    MyViewContext.SetZClippingFrontPlane (Front);
    MyViewContext.SetZClippingBackPlane (Back);
    MyView->SetContext (MyViewContext);
  }
}

void V3d_View::SetZoom(const Standard_Real Coef,const Standard_Boolean Start)
{
  V3d_BadValue_Raise_if( Coef <= 0.,"V3d_View::SetZoom, bad coefficient");

  if (Start)
  {
    myCamStartOpEye    = myCamera->Eye();
    myCamStartOpCenter = myCamera->Center();
  }

  Standard_Real aViewWidth  = myCamera->ViewDimensions().X();
  Standard_Real aViewHeight = myCamera->ViewDimensions().Y();

  // ensure that zoom will not be too small or too big
  Standard_Real coef = Coef;
  if (aViewWidth < coef * Precision::Confusion())
  {
    coef = aViewWidth / Precision::Confusion();
  }
  else if (aViewWidth > coef * 1e12)
  {
    coef = aViewWidth / 1e12;
  }
  if (aViewHeight < coef * Precision::Confusion())
  {
    coef = aViewHeight / Precision::Confusion();
  }
  else if (aViewHeight > coef * 1e12)
  {
    coef = aViewHeight / 1e12;
  }

  myCamera->BeginUpdate();
  myCamera->SetEye (myCamStartOpEye);
  myCamera->SetCenter (myCamStartOpCenter);
  myCamera->SetScale (myCamera->Scale() / Coef);
  AutoZFit();
  myCamera->EndUpdate();

  ImmediateUpdate();
}

void V3d_View::SetScale( const Standard_Real Coef )
{
  V3d_BadValue_Raise_if( Coef <= 0. ,"V3d_View::SetScale, bad coefficient");

  Handle(Graphic3d_Camera) aDefaultCamera = MyView->DefaultCamera();

  myCamera->BeginUpdate();
  
  // Strange behavior for the sake of compatibility.
  if (!aDefaultCamera.IsNull())
  {
    myCamera->SetAspect (aDefaultCamera->Aspect());
    Standard_Real aDefaultScale = aDefaultCamera->Scale();
    myCamera->SetScale (aDefaultScale / Coef);
  } 
  else
  {
    myCamera->SetScale (myCamera->Scale() / Coef);
  }

  AutoZFit();
  myCamera->EndUpdate();

  ImmediateUpdate();
}

void V3d_View::SetAxialScale( const Standard_Real Sx, const Standard_Real Sy, const Standard_Real Sz )
{
  V3d_BadValue_Raise_if( Sx <= 0. || Sy <= 0. || Sz <= 0.,"V3d_View::SetAxialScale, bad coefficient");

  myCamera->BeginUpdate();
  myCamera->SetAxialScale (gp_Pnt (Sx, Sy, Sz));
  AutoZFit();
  myCamera->EndUpdate();
}

void V3d_View::FitAll(const Standard_Real Coef, const Standard_Boolean update)
{
  Standard_Real Xmin, Ymin, Zmin, Xmax, Ymax, Zmax;

  // retrieve min / max values for current displayed objects
  MyView->MinMaxValues (Xmin, Ymin, Zmin,
                        Xmax, Ymax, Zmax);

  Standard_Real LIM = ShortRealLast() - 1.0;
  if  (Abs(Xmin) > LIM || Abs(Ymin) > LIM || Abs(Zmin) > LIM
    || Abs(Xmax) > LIM || Abs(Ymax) > LIM || Abs(Zmax) > LIM)
  {
    ImmediateUpdate();
    return;
  }

  myCamera->BeginUpdate();
  FitCamera (Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);
  myCamera->SetScale (myCamera->Scale() * (1.0 + Coef));
  AutoZFit();
  myCamera->EndUpdate();

  if (myImmediateUpdate || update)
  {
    Update();
  }
}

//===============================================================================================
//function : AutoZFit
//purpose  :
//===============================================================================================
void V3d_View::AutoZFit()
{
  if (myAutoZFitMode)
  {
    ZFitAll();
  }
}

void V3d_View::ZFitAll (const Standard_Real theCoeff)
{
  V3d_BadValue_Raise_if (theCoeff <= 0.0, "V3d_View::ZFitAll, bad margin coefficient");

  Standard_Real aMinMax[6];

  MyView->MinMaxValues (aMinMax[0], aMinMax[1], aMinMax[2], aMinMax[3], aMinMax[4], aMinMax[5]);

  gp_Pnt aBMin = gp_Pnt (aMinMax[0], aMinMax[1], aMinMax[2]);
  gp_Pnt aBMax = gp_Pnt (aMinMax[3], aMinMax[4], aMinMax[5]);

  // check bounding box for validness
  Standard_Real aLim = (ShortRealLast() - 1.0);
  if (Abs (aBMin.X()) > aLim || Abs (aBMin.Y()) > aLim || Abs (aBMin.Z()) > aLim ||
      Abs (aBMax.X()) > aLim || Abs (aBMax.Y()) > aLim || Abs (aBMax.Z()) > aLim)
  {
    SetZSize (0.0);
    ImmediateUpdate();
    return;
  }

  // adapt bound points
  gp_Pnt aPnts[8] = 
    { gp_Pnt (aBMin.X(), aBMin.Y(), aBMin.Z()),
      gp_Pnt (aBMin.X(), aBMin.Y(), aBMax.Z()),
      gp_Pnt (aBMin.X(), aBMax.Y(), aBMin.Z()),
      gp_Pnt (aBMin.X(), aBMax.Y(), aBMax.Z()),
      gp_Pnt (aBMax.X(), aBMin.Y(), aBMin.Z()),
      gp_Pnt (aBMax.X(), aBMin.Y(), aBMax.Z()),
      gp_Pnt (aBMax.X(), aBMax.Y(), aBMin.Z()),
      gp_Pnt (aBMax.X(), aBMax.Y(), aBMax.Z()) };

  // camera Eye plane
  gp_Dir aDir = myCamera->Direction();
  gp_Pnt anEye = myCamera->Eye();
  gp_Pln aCamPln (anEye, aDir);

  Standard_Real aMinDist = RealLast() - 1;
  Standard_Real aMaxDist = RealFirst() + 1;

  gp_Pnt anAxialScale = myCamera->AxialScale();

  // get minimum and maximum distances to the eye plane
  for (Standard_Integer aPntIt = 0; aPntIt < 8; ++aPntIt)
  {
    gp_Pnt aPnt = aPnts[aPntIt];

    aPnt = gp_Pnt (aPnt.X() * anAxialScale.X(),
                   aPnt.Y() * anAxialScale.Y(),
                   aPnt.Z() * anAxialScale.Z());

    Standard_Real aDistance = aCamPln.Distance (aPnt);

    // check if the camera is intruded into the scene
    if (aDir.IsOpposite (gp_Vec (anEye, aPnt), M_PI * 0.5))
    {
      aDistance *= -1;
    }

    aMinDist = Min (aDistance, aMinDist);
    aMaxDist = Max (aDistance, aMaxDist);
  }

  // compute depth of bounding box center
  Standard_Real aMidDepth  = (aMinDist + aMaxDist) * 0.5;
  Standard_Real aHalfDepth = (aMaxDist - aMinDist) * 0.5;

  // compute enlarged or shrank near and far z ranges.
  Standard_Real aZNear = aMidDepth - aHalfDepth * theCoeff;
  Standard_Real aZFar  = aMidDepth + aHalfDepth * theCoeff;

  myCamera->BeginUpdate();

  if (myCamera->IsOrthographic())
  {
    myCamera->SetZFar  (myCamera->Distance() * 3.0);
    myCamera->SetZNear (0.0);

    if (aZNear < 0.0)
    {
      myCamera->SetDistance (myCamera->Distance() - (aZNear + myCamera->ZNear()) + 10.0);
    }
  }
  else
  {
    myCamera->SetZFar  (aZFar);
    myCamera->SetZNear (aZNear); 
  }

  myCamera->EndUpdate();

  ImmediateUpdate();
}


// Better to use ZFitAll instead. 
void V3d_View::DepthFitAll(const Quantity_Coefficient Aspect,
                           const Quantity_Coefficient Margin)
{
  Standard_Real Xmin,Ymin,Zmin,Xmax,Ymax,Zmax,U,V,W,U1,V1,W1 ;
  Standard_Real Umin,Vmin,Wmin,Umax,Vmax,Wmax ;
  Standard_Real Dx,Dy,Dz,Size;

  Standard_Integer Nstruct = MyView->NumberOfDisplayedStructures() ;

  if((Nstruct <= 0) || (Aspect < 0.) || (Margin < 0.) || (Margin > 1.)) {
    ImmediateUpdate();
    return ;
  }

  MyView->MinMaxValues(Xmin,Ymin,Zmin,Xmax,Ymax,Zmax) ;

  Standard_Real LIM = ShortRealLast() -1.;
  if     (Abs(Xmin) > LIM || Abs(Ymin) > LIM || Abs(Zmin) > LIM
    ||  Abs(Xmax) > LIM || Abs(Ymax) > LIM || Abs(Zmax) > LIM ) {
      ImmediateUpdate();
      return ;
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

void V3d_View::FitAll(const Standard_Real Xmin, const Standard_Real Ymin, const Standard_Real Xmax, const Standard_Real Ymax)
{
  FitAll(MyWindow,Xmin,Ymin,Xmax,Ymax);

  ImmediateUpdate();
}

void V3d_View::WindowFitAll(const Standard_Integer Xmin, const Standard_Integer Ymin, const Standard_Integer Xmax, const Standard_Integer Ymax)
{
  WindowFit(Xmin,Ymin,Xmax,Ymax);
}

void V3d_View::WindowFit(const Standard_Integer Xmin, const Standard_Integer Ymin, const Standard_Integer Xmax, const Standard_Integer Ymax)
{
  if (!myCamera->IsOrthographic())
  {
    // normalize view coordiantes
    Standard_Integer aWinWidth, aWinHeight;
    MyWindow->Size (aWinWidth, aWinHeight);

    // z coordinate of camera center
    Standard_Real aDepth = myCamera->Project (myCamera->Center()).Z();

    // camera projection coordinate are in NDC which are normalized [-1, 1]
    Standard_Real aUMin = (2.0 / aWinWidth) * Xmin - 1.0;
    Standard_Real aUMax = (2.0 / aWinWidth) * Xmax - 1.0;
    Standard_Real aVMin = (2.0 / aWinHeight) * Ymin - 1.0;
    Standard_Real aVMax = (2.0 / aWinHeight) * Ymax - 1.0;

    // compute camera panning
    gp_Pnt aScreenCenter (0.0, 0.0, aDepth);
    gp_Pnt aFitCenter ((aUMin + aUMax) * 0.5, (aVMin + aVMax) * 0.5, aDepth);
    gp_Pnt aPanTo = myCamera->ConvertProj2View (aFitCenter);
    gp_Pnt aPanFrom = myCamera->ConvertProj2View (aScreenCenter);
    gp_Vec aPanVec (aPanFrom, aPanTo);

    // compute section size
    gp_Pnt aFitTopRight (aUMax, aVMax, aDepth);
    gp_Pnt aFitBotLeft (aUMin, aVMin, aDepth);
    gp_Pnt aViewBotLeft = myCamera->ConvertProj2View (aFitBotLeft);
    gp_Pnt aViewTopRight = myCamera->ConvertProj2View (aFitTopRight);

    Standard_Real aUSize = aViewTopRight.X() - aViewBotLeft.X();
    Standard_Real aVSize = aViewTopRight.Y() - aViewBotLeft.Y();

    myCamera->BeginUpdate();
    PanCamera (aPanVec.X(), -aPanVec.Y());
    ZoomCamera (aUSize, aVSize);
    AutoZFit();
    myCamera->EndUpdate();
  }
  else
  {
    Standard_Real x1,y1,x2,y2;
    Convert(Xmin,Ymin,x1,y1);
    Convert(Xmax,Ymax,x2,y2);

    FitAll(x1,y1,x2,y2);
  }
}

void V3d_View::SetViewMappingDefault()
{
  MyView->SetViewMappingDefault();

  ImmediateUpdate();
}

void V3d_View::ResetViewMapping()
{
  MyView->ViewMappingReset();

  Update();
}

void V3d_View::ConvertToGrid(const Standard_Integer Xp, const Standard_Integer Yp, Standard_Real& Xg, Standard_Real& Yg, Standard_Real& Zg) const
{
  Graphic3d_Vertex aVrp;
  Standard_Real anX, anY, aZ;
  Convert (Xp, Yp, anX, anY, aZ);
  aVrp.SetCoord (anX, anY, aZ);

  if( MyViewer->Grid()->IsActive() ) {
    Graphic3d_Vertex aNewVrp = Compute (aVrp) ;
    aNewVrp.Coord (Xg,Yg,Zg) ;
  } else
    aVrp.Coord (Xg,Yg,Zg) ;
}

void V3d_View::ConvertToGrid(const Standard_Real X, const Standard_Real Y, const Standard_Real Z, Standard_Real& Xg, Standard_Real& Yg, Standard_Real& Zg) const
{
  if( MyViewer->Grid()->IsActive() ) {
    Graphic3d_Vertex aVrp (X,Y,Z) ;
    Graphic3d_Vertex aNewVrp = Compute (aVrp) ;
    aNewVrp.Coord(Xg,Yg,Zg) ;
  } else {
    Xg = X; Yg = Y; Zg = Z;
  }
}


Standard_Real V3d_View::Convert(const Standard_Integer Vp) const
{
  Standard_Integer aDxw, aDyw ;

  V3d_UnMapped_Raise_if (!MyView->IsDefined(), "view has no window");

  MyWindow->Size (aDxw, aDyw);
  Standard_Real aValue;
  
  gp_Pnt aViewDims = myCamera->ViewDimensions();
  aValue = aViewDims.X() * (Standard_Real)Vp / (Standard_Real)aDxw;

  return aValue;
}

void V3d_View::Convert(const Standard_Integer Xp, const Standard_Integer Yp, Standard_Real& Xv, Standard_Real& Yv) const
{
  Standard_Integer aDxw, aDyw;

  V3d_UnMapped_Raise_if (!MyView->IsDefined(), "view has no window");

  MyWindow->Size (aDxw, aDyw);

  gp_Pnt aPoint (Xp * 2.0 / aDxw - 1.0, (aDyw - Yp) * 2.0 / aDyw - 1.0, 0.0);
  aPoint = myCamera->ConvertProj2View (aPoint);

  Xv = aPoint.X();
  Yv = aPoint.Y();
}

Standard_Integer V3d_View::Convert(const Standard_Real Vv) const
{
  V3d_UnMapped_Raise_if (!MyView->IsDefined(), "view has no window");

  Standard_Integer aDxw, aDyw;
  MyWindow->Size (aDxw, aDyw);

  gp_Pnt aViewDims = myCamera->ViewDimensions();
  Standard_Integer aValue = RealToInt (aDxw * Vv / (aViewDims.X()));

  return aValue;
}

void V3d_View::Convert(const Standard_Real Xv, const Standard_Real Yv, Standard_Integer& Xp, Standard_Integer& Yp) const
{
  V3d_UnMapped_Raise_if (!MyView->IsDefined(), "view has no window");

  Standard_Integer aDxw, aDyw;
  MyWindow->Size (aDxw, aDyw);

  gp_Pnt aPoint (Xv, Yv, 0.0);
  aPoint = myCamera->ConvertView2Proj (aPoint);
  aPoint = gp_Pnt ((aPoint.X() + 1.0) * aDxw / 2.0, aDyw - (aPoint.Y() + 1.0) * aDyw / 2.0, 0.0);

  Xp = RealToInt (aPoint.X());
  Yp = RealToInt (aPoint.Y());
}

void V3d_View::Convert(const Standard_Integer Xp, const Standard_Integer Yp, Standard_Real& X, Standard_Real& Y, Standard_Real& Z) const
{
  V3d_UnMapped_Raise_if (!MyView->IsDefined(), "view has no window");
  Standard_Integer aHeight, aWidth;
  MyWindow->Size (aWidth, aHeight);

  Standard_Real anX = 2.0 * Xp / aWidth - 1.0;
  Standard_Real anY = 2.0 * (aHeight - 1 - Yp) / aHeight - 1.0;
  Standard_Real  aZ = 2.0 * 0.0 - 1.0;

  gp_Pnt aResult = myCamera->UnProject (gp_Pnt (anX, anY, aZ));

  X = aResult.X();
  Y = aResult.Y();
  Z = aResult.Z();

  Graphic3d_Vertex aVrp;
  aVrp.SetCoord (X, Y, Z);

  if( MyViewer->Grid()->IsActive() ) {
    Graphic3d_Vertex aNewVrp = Compute (aVrp) ;
    aNewVrp.Coord (X, Y, Z) ;
  }
}

void V3d_View::ConvertWithProj(const Standard_Integer Xp, const Standard_Integer Yp, Standard_Real& X, Standard_Real& Y, Standard_Real& Z, Standard_Real& Dx, Standard_Real& Dy, Standard_Real& Dz) const
{
  V3d_UnMapped_Raise_if( !MyView->IsDefined(), "view has no window");
  Standard_Integer aHeight, aWidth;
  MyWindow->Size (aWidth, aHeight);

  Standard_Real anX = 2.0 * Xp / aWidth - 1.0;
  Standard_Real anY = 2.0 * (aHeight - 1 - Yp) / aHeight - 1.0;
  Standard_Real  aZ = 2.0 * 0.0 - 1.0;

  gp_Pnt aResult = myCamera->UnProject (gp_Pnt (anX, anY, aZ));

  X = aResult.X();
  Y = aResult.Y();
  Z = aResult.Z();

  Graphic3d_Vertex aVrp;
  aVrp.SetCoord (X, Y, Z);

  aResult = myCamera->UnProject (gp_Pnt (anX, anY, aZ - 10.0));

  Dx = X - aResult.X();
  Dy = Y - aResult.Y();
  Dz = Z - aResult.Z();

  if( MyViewer->Grid()->IsActive() ) {
    Graphic3d_Vertex aNewVrp = Compute (aVrp) ;
    aNewVrp.Coord (X, Y, Z) ;
  }
}

void V3d_View::Convert(const Standard_Real X, const Standard_Real Y, const Standard_Real Z, Standard_Integer& Xp, Standard_Integer& Yp) const
{
  V3d_UnMapped_Raise_if( !MyView->IsDefined(), "view has no window");
  Standard_Integer aHeight, aWidth;
  MyWindow->Size (aWidth, aHeight);

  gp_Pnt aPoint = myCamera->Project (gp_Pnt (X, Y, Z));

  Xp = RealToInt ((aPoint.X() + 1) * 0.5 * aWidth);
  Yp = RealToInt ((aPoint.Y() + 1) * 0.5 * aHeight);
}

void V3d_View::Project(const Standard_Real X, const Standard_Real Y, const Standard_Real Z, Standard_Real &Xp, Standard_Real &Yp) const
{
  Standard_Real Zp;
  MyView->Projects (X, Y, Z, Xp, Yp, Zp);
}

void V3d_View::BackgroundColor(const Quantity_TypeOfColor Type,Standard_Real& V1, Standard_Real& V2, Standard_Real& V3) const
{
  Quantity_Color C = BackgroundColor() ;
  C.Values(V1,V2,V3,Type) ;
}

Quantity_Color V3d_View::BackgroundColor() const
{
  return MyBackground.Color() ;
}

void V3d_View::GradientBackgroundColors(Quantity_Color& Color1,Quantity_Color& Color2) const
{
  MyGradientBackground.Colors(Color1, Color2);
}

Aspect_GradientBackground V3d_View::GradientBackground() const
{
   return MyGradientBackground;
}

Standard_Real V3d_View::Scale() const
{
  Handle(Graphic3d_Camera) aDefaultCamera = MyView->DefaultCamera();

  Standard_Real aCameraScale;

  // Strange behavior for the sake of compatibility.
  if (!aDefaultCamera.IsNull())
  {
    Standard_Real aDefaultScale = aDefaultCamera->Scale();
    aCameraScale = aDefaultScale / myCamera->Scale();
  } 
  else
  {
    aCameraScale = myCamera->Scale();
  }

  return aCameraScale;
}

void V3d_View::AxialScale(Standard_Real& Sx, Standard_Real& Sy, Standard_Real& Sz) const
{
  gp_Pnt anAxialScale = myCamera->AxialScale();
  Sx = anAxialScale.X();
  Sy = anAxialScale.Y();
  Sz = anAxialScale.Z();
}

void V3d_View::Center(Standard_Real& Xc, Standard_Real& Yc) const
{
  gp_Pnt aCamProjShift = myCamera->ProjectionShift();

  Xc = -aCamProjShift.X();
  Yc = -aCamProjShift.Y();
}

void V3d_View::Size(Standard_Real& Width, Standard_Real& Height) const
{
  gp_Pnt aViewDims = myCamera->ViewDimensions();

  Width = aViewDims.X();
  Height = aViewDims.Y();
}

Standard_Real V3d_View::ZSize() const
{
  gp_Pnt aViewDims = myCamera->ViewDimensions();

  return aViewDims.Z();
}

Standard_Integer V3d_View::MinMax(Standard_Real& Umin, Standard_Real& Vmin, Standard_Real& Umax, Standard_Real& Vmax) const
{
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

Standard_Integer V3d_View::MinMax(Standard_Real& Xmin, Standard_Real& Ymin, Standard_Real& Zmin, Standard_Real& Xmax, Standard_Real& Ymax, Standard_Real& Zmax) const
{
  // CAL 6/11/98
  // Standard_Integer Nstruct = (MyView->DisplayedStructures())->Extent() ;
  Standard_Integer Nstruct = MyView->NumberOfDisplayedStructures() ;

  if( Nstruct ) {
    MyView->MinMaxValues(Xmin,Ymin,Zmin,Xmax,Ymax,Zmax) ;
  }
  return Nstruct ;
}

Standard_Integer V3d_View::Gravity(Standard_Real& X, Standard_Real& Y, Standard_Real& Z) const
{
  Standard_Real Xmin,Ymin,Zmin,Xmax,Ymax,Zmax;
  Standard_Integer Nstruct,Npoint ;
  Graphic3d_MapOfStructure MySetOfStructures;

  MyView->DisplayedStructures (MySetOfStructures);
  Nstruct = MySetOfStructures.Extent() ;

  Graphic3d_MapIteratorOfMapOfStructure MyIterator(MySetOfStructures) ;

  Npoint = 0 ; X = Y = Z = 0. ;
  for (; MyIterator.More(); MyIterator.Next())
  {
    const Handle(Graphic3d_Structure)& aStruct = MyIterator.Key();
    if (!aStruct->IsEmpty())
    {
      aStruct->MinMaxValues (Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);

      // use camera projection to find gravity point
      gp_Pnt aPnts[8] = { 
        gp_Pnt (Xmin, Ymin, Zmin), gp_Pnt (Xmin, Ymin, Zmax),
        gp_Pnt (Xmin, Ymax, Zmin), gp_Pnt (Xmin, Ymax, Zmax),
        gp_Pnt (Xmax, Ymin, Zmin), gp_Pnt (Xmax, Ymin, Zmax),
        gp_Pnt (Xmax, Ymax, Zmin), gp_Pnt (Xmax, Ymax, Zmax) };

      for (Standard_Integer aPntIt = 0; aPntIt < 8; ++aPntIt)
      {
        const gp_Pnt& aBndPnt = aPnts[aPntIt];

        gp_Pnt aProjected = myCamera->Project (aBndPnt);
        const Standard_Real& U = aProjected.X();
        const Standard_Real& V = aProjected.Y();
        if (Abs(U) <= 1.0 && Abs(V) <= 1.0)
        {
          Npoint++;
          X += aBndPnt.X();
          Y += aBndPnt.Y();
          Z += aBndPnt.Z();
        }
      }
    }
  }
  if( Npoint > 0 ) {
    X /= Npoint ; Y /= Npoint ; Z /= Npoint ;
  }

  return Nstruct ;
}

void V3d_View::Eye(Standard_Real& X, Standard_Real& Y, Standard_Real& Z) const
{
  gp_Pnt aCameraEye = myCamera->Eye();
  X = aCameraEye.X();
  Y = aCameraEye.Y();
  Z = aCameraEye.Z();
}

void V3d_View::FocalReferencePoint(Standard_Real& X, Standard_Real& Y,Standard_Real& Z) const
{
  Eye (X,Y,Z);
}

void V3d_View::ProjReferenceAxe(const Standard_Integer Xpix, const Standard_Integer Ypix, Standard_Real& XP, Standard_Real& YP, Standard_Real& ZP, Standard_Real& VX, Standard_Real& VY, Standard_Real& VZ) const
{
  Standard_Real Xo,Yo,Zo;

  Convert (Xpix, Ypix, XP, YP, ZP);
  if ( Type() == V3d_PERSPECTIVE ) 
  {
    FocalReferencePoint (Xo,Yo,Zo);
    VX = Xo - XP;
    VY = Yo - YP;
    VZ = Zo - ZP;
  }
  else 
  {
    Proj (VX,VY,VZ);
  }
}

Standard_Real V3d_View::Depth() const
{
  return myCamera->Distance();
}

void V3d_View::Proj(Standard_Real& Dx, Standard_Real& Dy, Standard_Real& Dz) const
{
  gp_Dir aCameraDir = myCamera->Direction().Reversed();
  Dx = aCameraDir.X();
  Dy = aCameraDir.Y();
  Dz = aCameraDir.Z();
}

void V3d_View::At(Standard_Real& X, Standard_Real& Y, Standard_Real& Z) const
{
  gp_Pnt aCameraCenter = myCamera->Center();
  X = aCameraCenter.X();
  Y = aCameraCenter.Y();
  Z = aCameraCenter.Z();
}

void V3d_View::Up(Standard_Real& Vx, Standard_Real& Vy, Standard_Real& Vz) const
{
  gp_Dir aCameraUp = myCamera->Up();
  Vx = aCameraUp.X();
  Vy = aCameraUp.Y();
  Vz = aCameraUp.Z();
}

Standard_Real V3d_View::Twist() const
{
  Standard_Real Xup,Yup,Zup,Xpn,Ypn,Zpn,X0,Y0,Z0 ;
  Standard_Real pvx,pvy,pvz,pvn,sca,angle ;
  Graphic3d_Vector Xaxis,Yaxis,Zaxis ;
  Standard_Boolean TheStatus ;

  gp_Dir aReferencePlane (myCamera->Direction().Reversed());
  gp_Dir anUp;

  Proj(Xpn,Ypn,Zpn);
  anUp = gp_Dir (0.,0.,1.) ;
  TheStatus = ScreenAxis (aReferencePlane, anUp,Xaxis,Yaxis,Zaxis) ;
  if( !TheStatus ) {
    anUp = gp_Dir (0.,1.,0.) ;
    TheStatus = ScreenAxis (aReferencePlane, anUp,Xaxis,Yaxis,Zaxis) ;
  }
  if( !TheStatus ) {
    anUp = gp_Dir (1.,0.,0.) ;
    TheStatus = ScreenAxis (aReferencePlane, anUp,Xaxis,Yaxis,Zaxis) ;
  }
  Yaxis.Coord(X0,Y0,Z0) ;

  Up(Xup,Yup,Zup) ;
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
  if( sca < 0. ) angle = M_PI - angle ;
  if( angle > 0. && angle < M_PI ) {
    sca = pvx*Xpn + pvy*Ypn + pvz*Zpn ;
    if( sca < 0. ) angle = DEUXPI - angle ;
  }
  return angle ;
}

V3d_TypeOfShadingModel V3d_View::ShadingModel() const
{
  V3d_TypeOfShadingModel SM = (V3d_TypeOfShadingModel)MyViewContext.Model() ;
  return SM ;
}

V3d_TypeOfSurfaceDetail V3d_View::SurfaceDetail() const
{
  V3d_TypeOfSurfaceDetail SM = (V3d_TypeOfSurfaceDetail)MyViewContext.SurfaceDetail() ;
  return SM ;
}

Handle_Graphic3d_TextureEnv V3d_View::TextureEnv() const
{
  Handle(Graphic3d_TextureEnv) SM = MyViewContext.TextureEnv() ;
  return SM ;
}

V3d_TypeOfVisualization V3d_View::Visualization() const
{
  V3d_TypeOfVisualization V =
    (V3d_TypeOfVisualization)MyViewContext.Visualization() ;
  return V ;
}

Standard_Boolean V3d_View::Antialiasing() const
{
  Standard_Boolean A = MyViewContext.AliasingIsOn() ;
  return A ;
}

Handle(V3d_Viewer) V3d_View::Viewer() const
{
  return MyViewer ;
}

Standard_Boolean V3d_View::IfWindow() const
{
  Standard_Boolean TheStatus = MyView->IsDefined() ;
  return TheStatus ;
}

Handle(Aspect_Window) V3d_View::Window() const
{
  return MyWindow;
}

V3d_TypeOfView V3d_View::Type() const
{
  return myCamera->IsOrthographic() ? V3d_ORTHOGRAPHIC : V3d_PERSPECTIVE;
}

void V3d_View::SetFocale( const Standard_Real focale )
{
  if (myCamera->IsOrthographic())
  {
    return;
  }

  Standard_Real aFOVyRad = ATan (focale / (myCamera->Distance() * 2.0));

  myCamera->SetFOVy (aFOVyRad * (360 / M_PI));

  ImmediateUpdate();
}

Standard_Real V3d_View::Focale() const
{
  if (myCamera->IsOrthographic())
  {
    return 0.0;
  }

  return myCamera->Distance() * 2.0 * Tan(myCamera->FOVy() * M_PI / 360.0);
}

void V3d_View::SetViewingVolume(const Standard_Real Left, const Standard_Real Right,
                                const Standard_Real Bottom, const Standard_Real Top,
                                const Standard_Real ZNear, const Standard_Real ZFar)
{
  V3d_BadValue_Raise_if (ZNear >= ZFar, "V3d_View::SetVolume, bad distances");


  myCamera->BeginUpdate();

  myCamera->SetZFar (ZFar);
  myCamera->SetZNear (ZNear);

  myCamera->SetScale (Top - Bottom);
  myCamera->SetAspect ((Right - Left) / (Top - Bottom));
  gp_Pnt aCameraProjShift (-(Left + Right) / 2.0, -(Bottom + Top) / 2.0, 0.0);
  myCamera->SetProjectionShift (aCameraProjShift);
  if (myCamera->IsOrthographic())
  {
    myCamera->SetDistance (Abs (ZNear));
  }
  AutoZFit();
  myCamera->EndUpdate();

  ImmediateUpdate();
}

Handle(Visual3d_View) V3d_View::View() const
{
  return MyView ;
}

Standard_Boolean V3d_View::ScreenAxis( const gp_Dir &Vpn, const gp_Dir &Vup, Graphic3d_Vector &Xaxe, Graphic3d_Vector &Yaxe, Graphic3d_Vector &Zaxe)
{
  Standard_Real Xpn, Ypn, Zpn, Xup, Yup, Zup;
  Standard_Real dx1, dy1, dz1, xx, yy, zz;

  Xpn = Vpn.X(); Ypn = Vpn.Y(); Zpn = Vpn.Z();
  Xup = Vup.X(); Yup = Vup.Y(); Zup = Vup.Z();
  xx = Yup*Zpn - Zup*Ypn;
  yy = Zup*Xpn - Xup*Zpn;
  zz = Xup*Ypn - Yup*Xpn;
  Xaxe.SetCoord (xx, yy, zz);
  if (Xaxe.LengthZero()) return Standard_False;
  Xaxe.Normalize(); 
  Xaxe.Coord(dx1, dy1, dz1);
  xx = Ypn*dz1 - Zpn*dy1;
  yy = Zpn*dx1 - Xpn*dz1;
  zz = Xpn*dy1 - Ypn*dx1;
  Yaxe.SetCoord (xx, yy, zz) ;
  if (Yaxe.LengthZero()) return Standard_False;
  Yaxe.Normalize(); 

  Zaxe.SetCoord (Xpn, Ypn, Zpn);
  Zaxe.Normalize();
  return Standard_True;
}

Graphic3d_Vertex V3d_View::TrsPoint( const Graphic3d_Vertex &P, const TColStd_Array2OfReal &Matrix )
{
  Graphic3d_Vertex PP ;
  Standard_Real X,Y,Z,XX,YY,ZZ ;

  // CAL. S3892
  Standard_Integer lr, ur, lc, uc;
  lr    = Matrix.LowerRow ();
  ur    = Matrix.UpperRow ();
  lc    = Matrix.LowerCol ();
  uc    = Matrix.UpperCol ();
  if ((ur - lr + 1 != 4) || (uc - lc + 1 != 4) ) {
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

void V3d_View::Pan(const Standard_Integer Dx, const Standard_Integer Dy,const Quantity_Factor aZoomFactor)
{
  Panning (Convert(Dx), Convert(Dy), aZoomFactor, Standard_True);
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
    V3d_BadValue_Raise_if( dzoom <= 0.,"V3d_View::ZoomAtPoint, bad coefficient");

    Standard_Real aViewWidth  = myCamera->ViewDimensions().X();
    Standard_Real aViewHeight = myCamera->ViewDimensions().Y();

    // Ensure that zoom will not be too small or too big.
    Standard_Real coef = dzoom;
    if (aViewWidth < coef * Precision::Confusion())
    {
      coef = aViewWidth / Precision::Confusion();
    }
    else if (aViewWidth > coef * 1e12)
    {
      coef = aViewWidth / 1e12;
    }
    if (aViewHeight < coef * Precision::Confusion())
    {
      coef = aViewHeight / Precision::Confusion();
    }
    else if (aViewHeight > coef * 1e12)
    {
      coef = aViewHeight / 1e12;
    }

    V3d_Coordinate Dxv = (XS - X0) / coef;
    V3d_Coordinate Dyv = (YS - Y0) / coef;

    myCamera->SetScale (myCamera->Scale() / coef);
    PanCamera (-Dxv, -Dyv);

    AutoZFit();

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
  // normalize view coordinates
  Standard_Integer aWinWidth, aWinHeight;
  aWindow->Size (aWinWidth, aWinHeight);

  // compute camera panning
  gp_Vec aPanVec ((Xmin + Xmax) * 0.5, (Ymin + Ymax) * 0.5, 0.0);

  // compute section size
  gp_Pnt aViewBotLeft (Xmin, Ymin, 0.0);
  gp_Pnt aViewTopRight (Xmax, Ymax, 0.0);

  Standard_Real aUSize = Abs (Xmax - Xmin);
  Standard_Real aVSize = Abs (Ymax - Ymin);

  myCamera->BeginUpdate();
  myCamera->SetProjectionShift (gp_Pnt (0.0, 0.0, 0.0));
  PanCamera (aPanVec.X(), aPanVec.Y());
  ZoomCamera (aUSize, aVSize);
  myCamera->EndUpdate();

  AutoZFit();

  Update();
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
    dx = (Standard_Real(X) - sx) * M_PI / rx;
    dy = (sy - Standard_Real(Y)) * M_PI / ry;
  }
  Rotate(dx, dy, dz, gx, gy, gz, Standard_False);
#else
  Standard_Real dx = (Standard_Real(X - sx)) * M_PI;
  Standard_Real dy = (Standard_Real(sy - Y)) * M_PI;
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
}

Standard_Boolean V3d_View :: ComputedMode () const
{
  return MyView -> ComputedMode ();
}

void V3d_View :: SetBackFacingModel (const V3d_TypeOfBackfacingModel aModel)
{
  MyView -> SetBackFacingModel ( Visual3d_TypeOfBackfacingModel(aModel) );
  Redraw();
}

V3d_TypeOfBackfacingModel V3d_View :: BackFacingModel () const
{
  return V3d_TypeOfBackfacingModel(MyView -> BackFacingModel ());
}

Standard_Boolean V3d_View::TransientManagerBeginDraw(const Standard_Boolean DoubleBuffer,const Standard_Boolean RetainMode) const
{
  return Visual3d_TransientManager::BeginDraw(MyView,DoubleBuffer,RetainMode);
}

void V3d_View::TransientManagerClearDraw() const
{
  Visual3d_TransientManager::ClearDraw(MyView);
}

Standard_Boolean V3d_View::TransientManagerBeginAddDraw() const
{
  return Visual3d_TransientManager::BeginAddDraw(MyView);
}

void V3d_View::Init()
{
  myComputedMode = MyViewer->ComputedMode();
  if( !myComputedMode || !MyViewer->DefaultComputedMode() ) {
    SetComputedMode(Standard_False);
  }
}

void V3d_View::SetPlotter(const Handle(Graphic3d_Plotter)& aPlotter)
{
  MyPlotter = aPlotter;
}

void V3d_View::Plot()
{
  V3d_BadValue_Raise_if( !MyPlotter.IsNull(), "view has no plotter");
  MyView->Plot(MyPlotter);
}

#include <Aspect.hxx>
#include <Visual3d_Layer.hxx>

////////////////////////////////////////////////////////////////
Standard_Boolean V3d_View::Dump (const Standard_CString      theFile,
                                 const Graphic3d_BufferType& theBufferType)
{
  Standard_Integer aWinWidth, aWinHeight;
  MyWindow->Size (aWinWidth, aWinHeight);
  Image_AlienPixMap anImage;

  return ToPixMap (anImage, aWinWidth, aWinHeight, theBufferType) && anImage.Save (theFile);
}

////////////////////////////////////////////////////////////////
Standard_Boolean V3d_View::ToPixMap (Image_PixMap&               theImage,
                                     const Standard_Integer      theWidth,
                                     const Standard_Integer      theHeight,
                                     const Graphic3d_BufferType& theBufferType,
                                     const Standard_Boolean      theIsForceCentred,
                                     const V3d_StereoDumpOptions theStereoOptions)
{
  Graphic3d_CView* cView = (Graphic3d_CView* )MyView->CView();

  // always prefer hardware accelerated offscreen buffer
  Graphic3d_PtrFrameBuffer aFBOPtr = NULL;
  Graphic3d_PtrFrameBuffer aPrevFBOPtr = (Graphic3d_PtrFrameBuffer )cView->ptrFBO;
  Standard_Integer aFBOVPSizeX (theWidth), aFBOVPSizeY (theHeight), aFBOSizeXMax (0), aFBOSizeYMax (0);
  Standard_Integer aPrevFBOVPSizeX (0), aPrevFBOVPSizeY (0), aPrevFBOSizeXMax (0), aPrevFBOSizeYMax (0);
  if (aPrevFBOPtr != NULL)
  {
    MyView->FBOGetDimensions (aPrevFBOPtr,
                              aPrevFBOVPSizeX, aPrevFBOVPSizeY,
                              aPrevFBOSizeXMax, aPrevFBOSizeYMax);
    if (aFBOVPSizeX <= aPrevFBOSizeXMax && aFBOVPSizeY <= aPrevFBOSizeYMax)
    {
      MyView->FBOChangeViewport (aPrevFBOPtr, aFBOVPSizeX, aFBOVPSizeY);
      aFBOPtr = aPrevFBOPtr;
    }
  }

  if (aFBOPtr == NULL)
  {
    // Try to create hardware accelerated buffer
    aFBOPtr = MyView->FBOCreate (aFBOVPSizeX, aFBOVPSizeY);
    if (aFBOPtr != NULL)
    {
      MyView->FBOGetDimensions (aFBOPtr,
                                aFBOVPSizeX,  aFBOVPSizeY,
                                aFBOSizeXMax, aFBOSizeYMax);
      // reduce viewport in case of hardware limits
      if (aFBOVPSizeX > aFBOSizeXMax) aFBOVPSizeX = aFBOSizeXMax;
      if (aFBOVPSizeY > aFBOSizeYMax) aFBOVPSizeY = aFBOSizeYMax;
      MyView->FBOChangeViewport (aFBOPtr, aFBOVPSizeX, aFBOVPSizeY);
    }
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
    if (aFBOVPSizeX != aWinWidth || aFBOVPSizeY != aWinHeight)
    {
      return Standard_False;
    }
  }

  Handle(Graphic3d_Camera) aStoreMapping = new Graphic3d_Camera();

  aStoreMapping->Copy (myCamera);
  Standard_Real Umin, Vmin, Umax, Vmax;

  if (myCamera->IsStereo())
  {
    switch (theStereoOptions)
    {
      case V3d_SDO_MONO :
        myCamera->SetProjectionType (Graphic3d_Camera::Projection_Perspective);
        break;

      case V3d_SDO_LEFT_EYE :
        myCamera->SetProjectionType (Graphic3d_Camera::Projection_MonoLeftEye);
        break;

      case V3d_SDO_RIGHT_EYE :
        myCamera->SetProjectionType (Graphic3d_Camera::Projection_MonoRightEye);
        break;
    }
  }

  if (theIsForceCentred)
  {
    Standard_Real PUmin, PVmin, PUmax, PVmax;
    myCamera->WindowLimit (PUmin, PVmin, PUmax, PVmax);

    // calculate expansion
    Umin = PUmin; Vmin = PVmin; Umax = PUmax; Vmax = PVmax;
    Standard_Real oldWidth = (PUmax - PUmin), oldHeight = (PVmax - PVmin);
    Standard_Real newWidth = (oldHeight * aFBOVPSizeX) / aFBOVPSizeY;
    if (newWidth < oldWidth)
    {
      Standard_Real newHeight = (oldWidth * aFBOVPSizeY) / aFBOVPSizeX;
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

    FitAll (Umin, Vmin, Umax, Vmax);
  }

  //workaround for rendering list of Over and Under Layers
  if (!MyLayerMgr.IsNull())
  {
    MyLayerMgr->Compute();
  }

  // render immediate structures into back buffer rather than front
  Handle(Graphic3d_GraphicDriver) aDriver = Handle(Graphic3d_GraphicDriver)::DownCast (MyView->GraphicDriver());
  const Standard_Boolean aPrevImmediateMode = aDriver.IsNull() ? Standard_True : aDriver->SetImmediateModeDrawToFront (*cView, Standard_False);
  Redraw();

  if (!aDriver.IsNull())
  {
    aDriver->SetImmediateModeDrawToFront (*cView, aPrevImmediateMode);
  }

  myCamera->Copy (aStoreMapping);

  Standard_Boolean isSuccess = Standard_True;

  // allocate image buffer for dumping
  if (theImage.IsEmpty()
   || (Standard_Size )aFBOVPSizeX != theImage.SizeX()
   || (Standard_Size )aFBOVPSizeY != theImage.SizeY())
  {
    bool isBigEndian = Image_PixMap::IsBigEndianHost();
    Image_PixMap::ImgFormat aFormat = Image_PixMap::ImgUNKNOWN;
    switch (theBufferType)
    {
      case Graphic3d_BT_RGB:   aFormat = isBigEndian ? Image_PixMap::ImgRGB  : Image_PixMap::ImgBGR;  break;
      case Graphic3d_BT_RGBA:  aFormat = isBigEndian ? Image_PixMap::ImgRGBA : Image_PixMap::ImgBGRA; break;
      case Graphic3d_BT_Depth: aFormat = Image_PixMap::ImgGrayF; break;
    }

    isSuccess = isSuccess && theImage.InitZero (aFormat, aFBOVPSizeX, aFBOVPSizeY);
  }
  isSuccess = isSuccess && MyView->BufferDump (theImage, theBufferType);

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
  return isSuccess;
}

void V3d_View::ImmediateUpdate() const
{
  if (myImmediateUpdate) Update();
}

Standard_Boolean V3d_View::SetImmediateUpdate (const Standard_Boolean theImmediateUpdate)
{
  Standard_Boolean aPreviousMode = myImmediateUpdate;
  myImmediateUpdate = theImmediateUpdate;
  return aPreviousMode;
}

// =======================================================================
// function : SetCamera
// purpose  :
// =======================================================================
void V3d_View::SetCamera (const Handle(Graphic3d_Camera)& theCamera)
{
  myCamera = theCamera;

  MyView->SetCamera (theCamera);
}

// =======================================================================
// function : GetCamera
// purpose  :
// =======================================================================
Handle(Graphic3d_Camera) V3d_View::Camera() const
{
  return myCamera;
}

// =======================================================================
// function : FitCamera
// purpose  :
// =======================================================================
void V3d_View::FitCamera (const Standard_Real theXmin,
                          const Standard_Real theYmin,
                          const Standard_Real theZmin,
                          const Standard_Real theXmax,
                          const Standard_Real theYmax,
                          const Standard_Real theZmax)
{
  if (myCamera.IsNull())
    return;

  // check bounding box for validness
  Standard_Real aLim = (ShortRealLast() - 1.0);
  if (Abs (theXmin) > aLim || Abs (theYmin) > aLim || Abs (theZmin) > aLim ||
      Abs (theXmax) > aLim || Abs (theYmax) > aLim || Abs (theZmax) > aLim)
    return;

  // place camera center at the geometrical center
  // of the passed bounding box
  gp_Pnt aCenter ((theXmin + theXmax) * 0.5,
                  (theYmin + theYmax) * 0.5,
                  (theZmin + theZmax) * 0.5);

  // adapt bound points
  gp_Pnt aPnts[8] = 
    { gp_Pnt (theXmin, theYmin, theZmin),
      gp_Pnt (theXmin, theYmin, theZmax),
      gp_Pnt (theXmin, theYmax, theZmin),
      gp_Pnt (theXmin, theYmax, theZmax),
      gp_Pnt (theXmax, theYmin, theZmin),
      gp_Pnt (theXmax, theYmin, theZmax),
      gp_Pnt (theXmax, theYmax, theZmin),
      gp_Pnt (theXmax, theYmax, theZmax) };

  Standard_Real aViewMinX = (RealLast() - 1);
  Standard_Real aViewMinY = (RealLast() - 1);
  Standard_Real aViewMinZ = (RealLast() - 1);
  Standard_Real aViewMaxX = (RealFirst() + 1);
  Standard_Real aViewMaxY = (RealFirst() + 1);
  Standard_Real aViewMaxZ = (RealFirst() + 1);

  // find out minimum and maximum values of bounding box
  // converted to view space. the limits point out a rectangular
  // section parallel to the screen that camera should zoom in.
  for (Standard_Integer aPntIt = 0; aPntIt < 8; ++aPntIt)
  {
    gp_Pnt aView = myCamera->ConvertWorld2View (aPnts[aPntIt]);
    aViewMinX = Min (aViewMinX, aView.X());
    aViewMinY = Min (aViewMinY, aView.Y());
    aViewMaxX = Max (aViewMaxX, aView.X());
    aViewMaxY = Max (aViewMaxY, aView.Y());
    aViewMinZ = Min (aViewMinZ, aView.Z());
    aViewMaxZ = Max (aViewMaxZ, aView.Z());
  }

  // evaluate section size for x1 zoom.
  Standard_Real aSectU  = (aViewMaxX - aViewMinX);
  Standard_Real aSectV  = (aViewMaxY - aViewMinY);

  // zoom camera to front plane of bounding box. the camera
  // is set up at the center of bbox, so the depth is half
  // space of it in view coordinate space.
  Standard_Real aSectDepth = (aViewMaxZ - aViewMinZ) * 0.5;

  // re-compute Eye position
  gp_Vec aBck = gp_Vec (myCamera->Center(), myCamera->Eye());
  gp_Pnt aEye = aCenter.Translated (aBck);

  // start camera updates
  myCamera->BeginUpdate();

  if (myCamera->IsOrthographic())
  {
    Standard_Real anX = (aViewMaxX + aViewMinX) * 0.5; 
    Standard_Real anY = (aViewMaxY + aViewMinY) * 0.5;
    myCamera->SetProjectionShift (gp_Pnt (-anX, -anY, 0.0));
  }
  else
  {
    myCamera->SetProjectionShift (gp_Pnt (0.0, 0.0, 0.0));
    myCamera->SetCenter (aCenter);
    myCamera->SetEye (aEye);
  }

  // zoom camera to fit in the bounding box.
  ZoomCamera (aSectU, aSectV, aSectDepth);

  // re-evaluate camera
  myCamera->EndUpdate();
}

// =======================================================================
// function : ZoomCamera
// purpose  :
// =======================================================================
void V3d_View::ZoomCamera (const Standard_Real theUSize,
                           const Standard_Real theVSize,
                           const Standard_Real theZDepth)
{
  if (myCamera.IsNull())
    return;

  // compute maximum section size along both directions.
  Standard_Real anAspect = myCamera->Aspect();
  Standard_Real aSize = Max (theUSize / anAspect, theVSize);

  myCamera->BeginUpdate();
  myCamera->SetScale (aSize);

  if (!myCamera->IsOrthographic())
  {
    myCamera->SetDistance (myCamera->Distance() + theZDepth);
  }

  AutoZFit();

  myCamera->EndUpdate();
}

// =======================================================================
// function : PanCamera
// purpose  : panning is fun
// =======================================================================
void V3d_View::PanCamera (const Standard_Real theU,
                          const Standard_Real theV)
{
  if (myCamera.IsNull())
    return;

  if (myCamera->IsOrthographic())
  {
    // Projection based panning for compatibility.
    myCamera->SetProjectionShift (myCamera->ProjectionShift().
      Translated (gp_Vec (-theU, -theV, 0.0)));
  } 
  else
  {
    gp_Vec anUp = myCamera->Up();
    gp_Vec aSide  = myCamera->Direction().Crossed (anUp);

    gp_Vec aPanU = aSide.Scaled (theU);
    gp_Vec aPanV = anUp.Scaled (theV);
    gp_Pnt aPannedEye (myCamera->Eye());
    gp_Pnt aPannedCenter (myCamera->Center());

    aPannedEye.Translate (aPanU);
    aPannedEye.Translate (aPanV);
    aPannedCenter.Translate (aPanU);
    aPannedCenter.Translate (aPanV);

    myCamera->BeginUpdate();
    myCamera->SetEye (aPannedEye);
    myCamera->SetCenter (aPannedCenter);
    myCamera->EndUpdate();
  }

  AutoZFit();
}
