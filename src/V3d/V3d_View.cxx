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
06-10-98 : CAL ; Ajout d'un TIMER si CSF_GraphicTimer est definie.
16-10-98 : CAL ; Retrait d'un TIMER si CSF_GraphicTimer est definie.
06-11-98 : CAL ; PRO ?????. Probleme dans ZFitAll si un point dans la vue.
29-OCT-98 : DCB : Adding ScreenCopy () method.

REMARQUES :
-----------
About  FitAll() multiple. This probleme is caused by missing
precision of transformation matrices. If it is supposed that
projection is made in the plane (U,V), there is a difference
after several Zoom - compared to the exact value (cf ZoomX).
Don't forget that the matrices work in float and not in double.
To solve the problem (for lack of a better solution) I make 2 passes.

************************************************************************/

/*----------------------------------------------------------------------*/
/*
* Includes
*/

#include <Standard_TypeMismatch.hxx>
#include <Standard_ShortReal.hxx>
#include <Standard_Assert.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_DivideByZero.hxx>

#include <NCollection_Array1.hxx>

#include <Visual3d_ViewManager.hxx>
#include <Visual3d_Light.hxx>
#include <Visual3d_Layer.hxx>

#include <V3d.hxx>
#include <V3d_View.ixx>
#include <V3d_BadValue.hxx>
#include <V3d_StereoDumpOptions.hxx>

#include <Image_AlienPixMap.hxx>

#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TColStd_HSequenceOfInteger.hxx>

#include <Bnd_Box.hxx>

#include <Precision.hxx>

#include <Graphic3d_Structure.hxx>
#include <Graphic3d_MapIteratorOfMapOfStructure.hxx>
#include <Graphic3d_MapOfStructure.hxx>
#include <Graphic3d_TextureEnv.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_GraphicDriver.hxx>

#define V3d_FLAG_COMPUTATION   0x00000004

// Perspective
#include <OSD_Environment.hxx>

/*----------------------------------------------------------------------*/
/*
* Constant
*/

#define DEUXPI (2. * M_PI)

namespace
{
  static const Standard_Integer THE_NB_BOUND_POINTS = 8;
}

//=============================================================================
//function : Constructor
//purpose  :
//=============================================================================
V3d_View::V3d_View(const Handle(V3d_Viewer)& VM, const V3d_TypeOfView Type ) :
  MyViewer(VM.operator->()),
  MyActiveLights(),
  MyViewContext (),
  myActiveLightsIterator(),
  SwitchSetFront(Standard_False),
  MyTrsf (1, 4, 1, 4)
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
  SetViewMappingDefault();
  SetViewOrientationDefault();
  VM->AddView (this);
  Init();
  myImmediateUpdate = Standard_True;

  aCamera->SetProjectionType ((Type == V3d_ORTHOGRAPHIC)
    ? Graphic3d_Camera::Projection_Orthographic
    : Graphic3d_Camera::Projection_Perspective);
}

//=============================================================================
//function : Constructor
//purpose  :
//=============================================================================
V3d_View::V3d_View(const Handle(V3d_Viewer)& theVM,const Handle(V3d_View)& theView) :
  MyViewer(theVM.operator->()),
  MyActiveLights(),
  MyViewContext (),
  myActiveLightsIterator(),
  SwitchSetFront(Standard_False),
  MyTrsf (1, 4, 1, 4)
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
  View()->SetAutoZFitMode (theView->View()->AutoZFitMode(), theView->View()->AutoZFitScaleFactor());

  MyBackground = aFromView->Background() ;
  MyGradientBackground = aFromView->GradientBackground();

  MyView->SetContext (MyViewContext) ;

  SetAxis (0.0, 0.0, 0.0, 1.0, 1.0, 1.0);

  SetViewMappingDefault();
  SetViewOrientationDefault();
  theVM->AddView (this);

  Init();

  myImmediateUpdate = Standard_True;
}

//=============================================================================
//function : SetMagnify
//purpose  :
//=============================================================================
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

//=============================================================================
//function : SetWindow
//purpose  :
//=============================================================================
void V3d_View::SetWindow(const Handle(Aspect_Window)& TheWindow)
{
  MyView->SetWindow(TheWindow) ;
  // method V3d_View::SetWindow() should assign the field MyWindow before calling Redraw()
  MyWindow = TheWindow;
  // SetWindow carries out SetRatio and modifies
  MyView->SetContext(MyViewContext) ;
  MyView->SetBackground(MyBackground) ;
  MyView->SetGradientBackground (MyGradientBackground, Standard_False);
  MyViewer->SetViewOn(this) ;
  MyView->Redraw() ;
}

//=============================================================================
//function : SetWindow
//purpose  :
//=============================================================================
void V3d_View::SetWindow(const Handle(Aspect_Window)&      aWindow,
                         const Aspect_RenderingContext     aContext,
                         const Aspect_GraphicCallbackProc& aDisplayCB,
                         const Standard_Address            aClientData)
{
  // method V3d_View::SetWindow() should assign the field MyWindow before calling Redraw()
  MyWindow = aWindow;
  MyView->SetWindow(aWindow, aContext, aDisplayCB, aClientData) ;
  MyView->SetContext(MyViewContext) ;
  MyView->SetBackground(MyBackground) ;
  MyView->SetGradientBackground (MyGradientBackground, Standard_False);
  MyViewer->SetViewOn(this) ;
  MyView->Redraw() ;
}

//=============================================================================
//function : Remove
//purpose  :
//=============================================================================
void V3d_View::Remove() const
{
  MyViewer->DelView (this);
  MyView->Remove();
  Handle(Aspect_Window)& aWin = const_cast<Handle(Aspect_Window)&> (MyWindow);
  aWin.Nullify();
}

//=============================================================================
//function : Update
//purpose  :
//=============================================================================
void V3d_View::Update() const
{
  if( MyView->IsDefined() )  MyView->Update (Aspect_TOU_ASAP) ;
}

//=============================================================================
//function : Redraw
//purpose  :
//=============================================================================
void V3d_View::Redraw() const
{
  if( MyView->IsDefined() ) MyView->Redraw() ;
}

//=============================================================================
//function : RedrawImmediate
//purpose  :
//=============================================================================
void V3d_View::RedrawImmediate() const
{
  if (MyView->IsDefined())
  {
    MyView->RedrawImmediate();
  }
}

//=============================================================================
//function : Invalidate
//purpose  :
//=============================================================================
void V3d_View::Invalidate() const
{
  if (MyView->IsDefined())
  {
    MyView->Invalidate();
  }
}

//=============================================================================
//function : AutoZFit
//purpose  :
//=============================================================================
void V3d_View::AutoZFit()
{
  View()->AutoZFit();
}

//=============================================================================
//function : ZFitAll
//purpose  :
//=============================================================================
void V3d_View::ZFitAll (const Standard_Real theScaleFactor)
{
  View()->ZFitAll (theScaleFactor);
}

//=============================================================================
//function : Redraw
//purpose  :
//=============================================================================
void V3d_View::Redraw(const Standard_Integer xc,const Standard_Integer yc,
                      const Standard_Integer width,const Standard_Integer height) const
{
  if( MyView->IsDefined() ) MyView->Redraw(xc,yc,width,height) ;
}

//=============================================================================
//function : IsEmpty
//purpose  :
//=============================================================================
Standard_Boolean V3d_View::IsEmpty() const
{
  Standard_Boolean TheStatus = Standard_True ;
  if( MyView->IsDefined() ) {
    Standard_Integer Nstruct = MyView->NumberOfDisplayedStructures() ;
    if( Nstruct > 0 ) TheStatus = Standard_False ;
  }
  return (TheStatus) ;
}

//=============================================================================
//function : UpdateLights
//purpose  :
//=============================================================================
void V3d_View::UpdateLights() const
{
  MyView->SetContext(MyViewContext);
  Update();
}

//=============================================================================
//function : DoMapping
//purpose  :
//=============================================================================
void V3d_View::DoMapping()
{
  if( MyView->IsDefined() ) {
    (MyView->Window())->DoMapping() ;
  }
}

//=============================================================================
//function : MustBeResized
//purpose  :
//=============================================================================
void V3d_View::MustBeResized()
{
  if ( !MyLayerMgr.IsNull() )
    MyLayerMgr->Resized();

  if( MyView->IsDefined() ) {
    MyView->Resized() ;
    MyView->Redraw();
  }
}

//=============================================================================
//function : SetBackgroundColor
//purpose  :
//=============================================================================
void V3d_View::SetBackgroundColor(const Quantity_TypeOfColor Type, const Standard_Real v1, const Standard_Real v2, const Standard_Real v3)
{
  Standard_Real V1 = Max( Min( v1, 1.0 ), 0.0 );
  Standard_Real V2 = Max( Min( v2, 1.0 ), 0.0 );
  Standard_Real V3 = Max( Min( v3, 1.0 ), 0.0 );

  Quantity_Color C( V1, V2, V3, Type );
  SetBackgroundColor( C );
}

//=============================================================================
//function : SetBackgroundColor
//purpose  :
//=============================================================================
void V3d_View::SetBackgroundColor(const Quantity_Color &Color)
{
  MyBackground.SetColor( Color );
  if ( MyView->IsDefined() )
    MyView->SetBackground( MyBackground );
  //szv: Why?
  if ( !MyLayerMgr.IsNull() )
    MyLayerMgr->Resized();
}

//=============================================================================
//function : SetBackgroundColor
//purpose  :
//=============================================================================
void V3d_View::SetBackgroundColor(const Quantity_NameOfColor Name)
{
  Quantity_Color C( Name );
  SetBackgroundColor( C );
}

//=============================================================================
//function : SetBgGradientColors
//purpose  :
//=============================================================================
void V3d_View::SetBgGradientColors( const Quantity_Color& Color1,
                                    const Quantity_Color& Color2,
                                    const Aspect_GradientFillMethod FillStyle,
                                    const Standard_Boolean status)
{
  MyGradientBackground.SetColors(Color1, Color2, FillStyle);
  if ( MyView->IsDefined() )
    MyView->SetGradientBackground( MyGradientBackground, status );
}

//=============================================================================
//function : SetBgGradientColors
//purpose  :
//=============================================================================
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

//=============================================================================
//function : SetBgGradientStyle
//purpose  :
//=============================================================================
void V3d_View::SetBgGradientStyle( const Aspect_GradientFillMethod FillStyle,
                                   const Standard_Boolean update)
{
  Quantity_Color Color1, Color2;
  MyGradientBackground.Colors( Color1, Color2 );
  MyGradientBackground.SetColors( Color1, Color2, FillStyle );
  if( MyView->IsDefined() )
    MyView->SetBgGradientStyle( FillStyle, update ) ;
}

//=============================================================================
//function : SetBackgroundImage
//purpose  :
//=============================================================================
void V3d_View::SetBackgroundImage( const Standard_CString FileName,
                                   const Aspect_FillMethod FillStyle,
                                   const Standard_Boolean update )
{
  if( MyView->IsDefined() )
    MyView->SetBackgroundImage( FileName, FillStyle, update ) ;
}

//=============================================================================
//function : SetBgImageStyle
//purpose  :
//=============================================================================
void V3d_View::SetBgImageStyle( const Aspect_FillMethod FillStyle,
                                const Standard_Boolean update )
{
  if( MyView->IsDefined() )
    MyView->SetBgImageStyle( FillStyle, update ) ;
}

//=============================================================================
//function : SetAxis
//purpose  :
//=============================================================================
void V3d_View::SetAxis(const Standard_Real X, const Standard_Real Y, const Standard_Real Z, const Standard_Real Vx, const Standard_Real Vy, const Standard_Real Vz)
{
  Standard_Real D,Nx = Vx,Ny = Vy,Nz = Vz ;

  D = Sqrt( Vx*Vx + Vy*Vy + Vz*Vz ) ;
  V3d_BadValue_Raise_if ( D <= 0. , "V3d_View::SetAxis, bad axis");
  Nx /= D ; Ny /= D ; Nz /= D ;
  MyDefaultViewPoint.SetCoord(X,Y,Z) ;
  MyDefaultViewAxis.SetCoord(Nx,Ny,Nz) ;
}

//=============================================================================
//function : SetShadingModel
//purpose  :
//=============================================================================
void V3d_View::SetShadingModel(const V3d_TypeOfShadingModel Model)
{
  MyViewContext.SetModel((Visual3d_TypeOfModel) Model) ;
  MyView->SetContext(MyViewContext) ;
}

//=============================================================================
//function : SetSurfaceDetail
//purpose  :
//=============================================================================
void V3d_View::SetSurfaceDetail(const V3d_TypeOfSurfaceDetail Model)
{
  MyViewContext.SetSurfaceDetail((Visual3d_TypeOfSurfaceDetail) Model) ;
  MyView->SetContext(MyViewContext) ;
}

//=============================================================================
//function : SetTextureEnv
//purpose  :
//=============================================================================
void V3d_View::SetTextureEnv(const Handle(Graphic3d_TextureEnv)& ATexture)
{
  MyViewContext.SetTextureEnv(ATexture) ;
  MyView->SetContext(MyViewContext) ;
}

//=============================================================================
//function : SetVisualization
//purpose  :
//=============================================================================
void V3d_View::SetVisualization(const V3d_TypeOfVisualization Mode)
{
  MyViewContext.SetVisualization((Visual3d_TypeOfVisualization) Mode);
  MyView->SetContext(MyViewContext) ;
}

//=============================================================================
//function : SetFront
//purpose  :
//=============================================================================
void V3d_View::SetFront()
{
  gp_Ax3 a = MyViewer->PrivilegedPlane();
  Standard_Real xo, yo, zo, vx, vy, vz, xu, yu, zu;

  a.Direction().Coord(vx,vy,vz);
  a.YDirection().Coord(xu,yu,zu);
  a.Location().Coord(xo,yo,zo);

  myCamera->SetCenter (gp_Pnt (xo, yo, zo));
  if(SwitchSetFront)
    myCamera->SetDirection (gp_Dir (vx, vy, vz));
  else
    myCamera->SetDirection (gp_Dir (vx, vy, vz).Reversed());
  myCamera->SetUp (gp_Dir (xu, yu, zu));

  View()->AutoZFit();

  SwitchSetFront = !SwitchSetFront;

  ImmediateUpdate();
}

//=============================================================================
//function : Rotate
//purpose  :
//=============================================================================
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

  View()->AutoZFit();

  ImmediateUpdate();
}

//=============================================================================
//function : Rotate
//purpose  :
//=============================================================================
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

  View()->AutoZFit();

  ImmediateUpdate();
}

//=============================================================================
//function : Rotate
//purpose  :
//=============================================================================
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

//=============================================================================
//function : Rotate
//purpose  :
//=============================================================================
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

  View()->AutoZFit();

  ImmediateUpdate();
}

//=============================================================================
//function : Rotate
//purpose  :
//=============================================================================
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

  myCamera->SetUp (myCamStartOpUp);
  myCamera->SetEye (myCamStartOpEye);
  myCamera->SetCenter (myCamStartOpCenter);

  gp_Trsf aRotation;
  gp_Pnt aRCenter (aPnt.X(), aPnt.Y(), aPnt.Z());
  gp_Dir aRAxis (anAxis.X(), anAxis.Y(), anAxis.Z());
  aRotation.SetRotation (gp_Ax1 (aRCenter, aRAxis), Angle);
  myCamera->Transform (aRotation);

  View()->AutoZFit();

  ImmediateUpdate();
}

//=============================================================================
//function : Turn
//purpose  :
//=============================================================================
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

  View()->AutoZFit();

  ImmediateUpdate();
}

//=============================================================================
//function : Turn
//purpose  :
//=============================================================================
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

//=============================================================================
//function : Turn
//purpose  :
//=============================================================================
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

  myCamera->SetUp (myCamStartOpUp);
  myCamera->SetEye (myCamStartOpEye);
  myCamera->SetCenter (myCamStartOpCenter);

  const Graphic3d_Vector& anAxis = MyDefaultViewAxis;

  gp_Trsf aRotation;
  gp_Pnt aRCenter = myCamera->Eye();
  gp_Dir aRAxis (anAxis.X(), anAxis.Y(), anAxis.Z());
  aRotation.SetRotation (gp_Ax1 (aRCenter, aRAxis), Angle);
  myCamera->Transform (aRotation);

  View()->AutoZFit();

  ImmediateUpdate();
}

//=============================================================================
//function : SetTwist
//purpose  :
//=============================================================================
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

  myCamera->SetUp (gp_Dir (myYscreenAxisX, myYscreenAxisY, myYscreenAxisZ));
  myCamera->Transform (aTrsf);

  View()->AutoZFit();

  ImmediateUpdate();
}

//=============================================================================
//function : SetEye
//purpose  :
//=============================================================================
void V3d_View::SetEye(const Standard_Real X,const Standard_Real Y,const Standard_Real Z)
{
  Standard_Real aTwistBefore = Twist();

  Standard_Boolean wasUpdateEnabled = SetImmediateUpdate (Standard_False);

  myCamera->SetEye (gp_Pnt (X, Y, Z));
  SetTwist (aTwistBefore);

  View()->AutoZFit();

  SetImmediateUpdate (wasUpdateEnabled);

  ImmediateUpdate();
}

//=============================================================================
//function : SetDepth
//purpose  :
//=============================================================================
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

  View()->AutoZFit();

  ImmediateUpdate();
}

//=============================================================================
//function : SetProj
//purpose  :
//=============================================================================
void V3d_View::SetProj( const Standard_Real Vx,const Standard_Real Vy, const Standard_Real Vz )
{
  V3d_BadValue_Raise_if( Sqrt(Vx*Vx + Vy*Vy + Vz*Vz) <= 0.,
    "V3d_View::SetProj, null projection vector");

  Standard_Real aTwistBefore = Twist();

  Standard_Boolean wasUpdateEnabled = SetImmediateUpdate (Standard_False);

  myCamera->SetDirection (gp_Dir (Vx, Vy, Vz).Reversed());

  SetTwist(aTwistBefore);

  View()->AutoZFit();

  SetImmediateUpdate (wasUpdateEnabled);

  ImmediateUpdate();
}

//=============================================================================
//function : SetProj
//purpose  :
//=============================================================================
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

  // retain camera panning from origin when switching projection
  gp_Pnt anOriginVCS = myCamera->ConvertWorld2View (gp::Origin());
  Standard_Real aPanX = anOriginVCS.X();
  Standard_Real aPanY = anOriginVCS.Y();

  myCamera->SetCenter (gp_Pnt (0, 0, 0));
  myCamera->SetDirection (gp_Dir (aBck.X(), aBck.Y(), aBck.Z()).Reversed());
  myCamera->SetUp (gp_Dir (Xpn, Ypn, Zpn));
  myCamera->OrthogonalizeUp();

  Panning (aPanX, aPanY);

  View()->AutoZFit();

  ImmediateUpdate();
}

//=============================================================================
//function : SetAt
//purpose  :
//=============================================================================
void V3d_View::SetAt(const Standard_Real X,const Standard_Real Y,const Standard_Real Z)
{
  Standard_Real aTwistBefore = Twist();

  Standard_Boolean wasUpdateEnabled = SetImmediateUpdate (Standard_False);

  myCamera->SetCenter (gp_Pnt (X, Y, Z));

  SetTwist (aTwistBefore);

  View()->AutoZFit();

  SetImmediateUpdate (wasUpdateEnabled);

  ImmediateUpdate();
}

//=============================================================================
//function : SetUp
//purpose  :
//=============================================================================
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

  myCamera->SetUp (gp_Dir (myYscreenAxisX, myYscreenAxisY, myYscreenAxisZ));

  View()->AutoZFit();

  ImmediateUpdate();
}

//=============================================================================
//function : SetUp
//purpose  :
//=============================================================================
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

  myCamera->SetUp (gp_Dir (myYscreenAxisX, myYscreenAxisY, myYscreenAxisZ));

  View()->AutoZFit();

  ImmediateUpdate();
}

//=============================================================================
//function : SetViewOrientationDefault
//purpose  :
//=============================================================================
void V3d_View::SetViewOrientationDefault()
{
  MyView->SetViewOrientationDefault() ;

  ImmediateUpdate();
}

//=============================================================================
//function : ResetViewOrientation
//purpose  :
//=============================================================================
void V3d_View::ResetViewOrientation()
{
  MyView->ViewOrientationReset() ;

  ImmediateUpdate();
}

//=============================================================================
//function : Reset
//purpose  :
//=============================================================================
void V3d_View::Reset( const Standard_Boolean update )
{
  Handle(Graphic3d_Camera) aDefaultCamera = MyView->DefaultCamera();

  if (!aDefaultCamera.IsNull())
  {
    myCamera->CopyMappingData (aDefaultCamera);
    myCamera->CopyOrientationData (aDefaultCamera);

    View()->AutoZFit();
  }

  SwitchSetFront = Standard_False;

  if( myImmediateUpdate || update ) Update();
}

//=======================================================================
//function : SetCenter
//purpose  :
//=======================================================================
void V3d_View::SetCenter (const Standard_Integer theXp,
                          const Standard_Integer theYp)
{
  Standard_Real aXv, aYv;
  Convert (theXp, theYp, aXv, aYv);
  Translate (myCamera, aXv, aYv);

  ImmediateUpdate();
}

//=============================================================================
//function : SetSize
//purpose  :
//=============================================================================
void V3d_View::SetSize (const Standard_Real theSize)
{
  V3d_BadValue_Raise_if (theSize <= 0.0, "V3d_View::SetSize, Window Size is NULL");

  myCamera->SetScale (myCamera->Aspect() >= 1.0 ? theSize / myCamera->Aspect() : theSize);

  View()->AutoZFit();

  ImmediateUpdate();
}

//=============================================================================
//function : SetZSize
//purpose  :
//=============================================================================
void V3d_View::SetZSize(const Standard_Real Size)
{
  Standard_Real Zmax = Size/2.;

  Standard_Real aDistance = myCamera->Distance();

  if( Size <= 0. ) {
    Zmax = aDistance;
  }

  Standard_Real Front = MyViewContext.ZClippingFrontPlane();
  Standard_Real Back  = MyViewContext.ZClippingBackPlane();

  // ShortReal precision factor used to add meaningful tolerance to
  // ZNear, ZFar values in order to avoid equality after type conversion
  // to ShortReal matrices type.
  const Standard_Real aPrecision = 1.0 / Pow (10.0, ShortRealDigits() - 1);

  Standard_Real aZFar  =  Zmax  + aDistance * 2.0;
  Standard_Real aZNear = -Zmax  + aDistance;
  aZNear              -= Abs (aZNear) * aPrecision;
  aZFar               += Abs (aZFar)  * aPrecision;

  if (!myCamera->IsOrthographic())
  {
    if (aZFar < aPrecision)
    {
      // Invalid case when both values are negative
      aZNear = aPrecision;
      aZFar  = aPrecision * 2.0;
    }
    else if (aZNear < Abs (aZFar) * aPrecision)
    {
      // Z is less than 0.0, try to fix it using any appropriate z-scale
      aZNear = Abs (aZFar) * aPrecision;
    }
  }

  // If range is too small
  if (aZFar < (aZNear + Abs (aZFar) * aPrecision))
  {
    aZFar = aZNear + Abs (aZFar) * aPrecision;
  }

  myCamera->SetZRange (aZNear, aZFar);

  if (MyViewContext.FrontZClippingIsOn()  ||
      MyViewContext.BackZClippingIsOn())
  {
    MyViewContext.SetZClippingFrontPlane (Front);
    MyViewContext.SetZClippingBackPlane (Back);
    MyView->SetContext (MyViewContext);
  }
}

//=============================================================================
//function : SetZoom
//purpose  :
//=============================================================================
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

  myCamera->SetEye (myCamStartOpEye);
  myCamera->SetCenter (myCamStartOpCenter);
  myCamera->SetScale (myCamera->Scale() / Coef);
  View()->AutoZFit();

  ImmediateUpdate();
}

//=============================================================================
//function : SetScale
//purpose  :
//=============================================================================
void V3d_View::SetScale( const Standard_Real Coef )
{
  V3d_BadValue_Raise_if( Coef <= 0. ,"V3d_View::SetScale, bad coefficient");

  Handle(Graphic3d_Camera) aDefaultCamera = MyView->DefaultCamera();

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

  View()->AutoZFit();

  ImmediateUpdate();
}

//=============================================================================
//function : SetAxialScale
//purpose  :
//=============================================================================
void V3d_View::SetAxialScale( const Standard_Real Sx, const Standard_Real Sy, const Standard_Real Sz )
{
  V3d_BadValue_Raise_if( Sx <= 0. || Sy <= 0. || Sz <= 0.,"V3d_View::SetAxialScale, bad coefficient");

  myCamera->SetAxialScale (gp_XYZ (Sx, Sy, Sz));
  View()->AutoZFit();
}

//=============================================================================
//function : FitAll
//purpose  :
//=============================================================================
void V3d_View::FitAll (const Quantity_Coefficient theMargin, const Standard_Boolean theToUpdate)
{
  FitAll (MyView->MinMaxValues(), theMargin, theToUpdate);
}

//=============================================================================
//function : FitAll
//purpose  :
//=============================================================================
void V3d_View::FitAll (const Bnd_Box& theBox, const Quantity_Coefficient theMargin, const Standard_Boolean theToUpdate)
{
  Standard_ASSERT_RAISE(theMargin >= 0.0 && theMargin < 1.0, "Invalid margin coefficient");

  if (MyView->NumberOfDisplayedStructures() == 0)
  {
    return;
  }

  if (!FitMinMax (myCamera, theBox, theMargin, 10.0 * Precision::Confusion()))
  {
    return;
  }

  View()->AutoZFit();

  if (myImmediateUpdate || theToUpdate)
  {
    Update();
  }
}

//=============================================================================
//function : DepthFitAll
//purpose  :
//=============================================================================
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

  Bnd_Box aBox = MyView->MinMaxValues();
  if (aBox.IsVoid())
  {
    ImmediateUpdate();
    return ;
  }
    aBox.Get (Xmin,Ymin,Zmin,Xmax,Ymax,Zmax);
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

//=============================================================================
//function : FitAll
//purpose  :
//=============================================================================
void V3d_View::FitAll(const Standard_Real theMinXv,
                      const Standard_Real theMinYv,
                      const Standard_Real theMaxXv,
                      const Standard_Real theMaxYv)
{
  FitAll (MyWindow, theMinXv, theMinYv, theMaxXv, theMaxYv);
}

//=============================================================================
//function : WindowFitAll
//purpose  :
//=============================================================================
void V3d_View::WindowFitAll(const Standard_Integer Xmin,
                            const Standard_Integer Ymin,
                            const Standard_Integer Xmax,
                            const Standard_Integer Ymax)
{
  WindowFit(Xmin,Ymin,Xmax,Ymax);
}

//=======================================================================
//function : WindowFit
//purpose  :
//=======================================================================
void V3d_View::WindowFit (const Standard_Integer theMinXp,
                          const Standard_Integer theMinYp,
                          const Standard_Integer theMaxXp,
                          const Standard_Integer theMaxYp)
{
  Standard_Boolean wasUpdateEnabled = SetImmediateUpdate (Standard_False);

  if (!myCamera->IsOrthographic())
  {
    // normalize view coordinates
    Standard_Integer aWinWidth, aWinHeight;
    MyWindow->Size (aWinWidth, aWinHeight);

    // z coordinate of camera center
    Standard_Real aDepth = myCamera->Project (myCamera->Center()).Z();

    // camera projection coordinate are in NDC which are normalized [-1, 1]
    Standard_Real aUMin = (2.0 / aWinWidth) * theMinXp  - 1.0;
    Standard_Real aUMax = (2.0 / aWinWidth) * theMaxXp  - 1.0;
    Standard_Real aVMin = (2.0 / aWinHeight) * theMinYp - 1.0;
    Standard_Real aVMax = (2.0 / aWinHeight) * theMaxYp - 1.0;

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

    Translate (myCamera, aPanVec.X(), -aPanVec.Y());
    Scale (myCamera, aUSize, aVSize);
    View()->AutoZFit();
  }
  else
  {
    Standard_Real aX1, aY1, aX2, aY2;
    Convert (theMinXp, theMinYp, aX1, aY1);
    Convert (theMaxXp, theMaxYp, aX2, aY2);
    FitAll (aX1, aY1, aX2, aY2);
  }

  SetImmediateUpdate (wasUpdateEnabled);

  ImmediateUpdate();
}

//=======================================================================
//function : SetViewMappingDefault
//purpose  :
//=======================================================================
void V3d_View::SetViewMappingDefault()
{
  MyView->SetViewMappingDefault();

  ImmediateUpdate();
}

//=======================================================================
//function : ResetViewMapping
//purpose  :
//=======================================================================
void V3d_View::ResetViewMapping()
{
  MyView->ViewMappingReset();

  Update();
}

//=======================================================================
//function : ConvertToGrid
//purpose  :
//=======================================================================
void V3d_View::ConvertToGrid(const Standard_Integer Xp,
                             const Standard_Integer Yp,
                             Standard_Real& Xg,
                             Standard_Real& Yg,
                             Standard_Real& Zg) const
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

//=======================================================================
//function : ConvertToGrid
//purpose  :
//=======================================================================
void V3d_View::ConvertToGrid(const Standard_Real X,
                             const Standard_Real Y,
                             const Standard_Real Z,
                             Standard_Real& Xg,
                             Standard_Real& Yg,
                             Standard_Real& Zg) const
{
  if( MyViewer->Grid()->IsActive() ) {
    Graphic3d_Vertex aVrp (X,Y,Z) ;
    Graphic3d_Vertex aNewVrp = Compute (aVrp) ;
    aNewVrp.Coord(Xg,Yg,Zg) ;
  } else {
    Xg = X; Yg = Y; Zg = Z;
  }
}

//=======================================================================
//function : Convert
//purpose  :
//=======================================================================
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

//=======================================================================
//function : Convert
//purpose  :
//=======================================================================
void V3d_View::Convert(const Standard_Integer Xp,
                       const Standard_Integer Yp,
                       Standard_Real& Xv,
                       Standard_Real& Yv) const
{
  Standard_Integer aDxw, aDyw;

  V3d_UnMapped_Raise_if (!MyView->IsDefined(), "view has no window");

  MyWindow->Size (aDxw, aDyw);

  gp_Pnt aPoint (Xp * 2.0 / aDxw - 1.0, (aDyw - Yp) * 2.0 / aDyw - 1.0, 0.0);
  aPoint = myCamera->ConvertProj2View (aPoint);

  Xv = aPoint.X();
  Yv = aPoint.Y();
}

//=======================================================================
//function : Convert
//purpose  :
//=======================================================================
Standard_Integer V3d_View::Convert(const Standard_Real Vv) const
{
  V3d_UnMapped_Raise_if (!MyView->IsDefined(), "view has no window");

  Standard_Integer aDxw, aDyw;
  MyWindow->Size (aDxw, aDyw);

  gp_Pnt aViewDims = myCamera->ViewDimensions();
  Standard_Integer aValue = RealToInt (aDxw * Vv / (aViewDims.X()));

  return aValue;
}

//=======================================================================
//function : Convert
//purpose  :
//=======================================================================
void V3d_View::Convert(const Standard_Real Xv,
                       const Standard_Real Yv,
                       Standard_Integer& Xp,
                       Standard_Integer& Yp) const
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

//=======================================================================
//function : Convert
//purpose  :
//=======================================================================
void V3d_View::Convert(const Standard_Integer Xp,
                       const Standard_Integer Yp,
                       Standard_Real& X,
                       Standard_Real& Y,
                       Standard_Real& Z) const
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
}

//=======================================================================
//function : ConvertWithProj
//purpose  :
//=======================================================================
void V3d_View::ConvertWithProj(const Standard_Integer Xp,
                               const Standard_Integer Yp,
                               Standard_Real& X,
                               Standard_Real& Y,
                               Standard_Real& Z,
                               Standard_Real& Dx,
                               Standard_Real& Dy,
                               Standard_Real& Dz) const
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

  Graphic3d_Vec3d aNormDir;
  aNormDir.x() = X - aResult.X();
  aNormDir.y() = Y - aResult.Y();
  aNormDir.z() = Z - aResult.Z();
  aNormDir.Normalize();

  Dx = aNormDir.x();
  Dy = aNormDir.y();
  Dz = aNormDir.z();
}

//=======================================================================
//function : Convert
//purpose  :
//=======================================================================
void V3d_View::Convert(const Standard_Real X,
                       const Standard_Real Y,
                       const Standard_Real Z,
                       Standard_Integer& Xp,
                       Standard_Integer& Yp) const
{
  V3d_UnMapped_Raise_if( !MyView->IsDefined(), "view has no window");
  Standard_Integer aHeight, aWidth;
  MyWindow->Size (aWidth, aHeight);

  gp_Pnt aPoint = myCamera->Project (gp_Pnt (X, Y, Z));

  Xp = RealToInt ((aPoint.X() + 1) * 0.5 * aWidth);
  Yp = RealToInt (aHeight - 1 - (aPoint.Y() + 1) * 0.5 * aHeight);
}

//=======================================================================
//function : Project
//purpose  :
//=======================================================================
void V3d_View::Project(const Standard_Real X,
                       const Standard_Real Y,
                       const Standard_Real Z,
                       Standard_Real &Xp,
                       Standard_Real &Yp) const
{
  Standard_Real Zp;
  MyView->Projects (X, Y, Z, Xp, Yp, Zp);
}

//=======================================================================
//function : BackgroundColor
//purpose  :
//=======================================================================
void V3d_View::BackgroundColor(const Quantity_TypeOfColor Type,
                               Standard_Real& V1,
                               Standard_Real& V2,
                               Standard_Real& V3) const
{
  Quantity_Color C = BackgroundColor() ;
  C.Values(V1,V2,V3,Type) ;
}

//=======================================================================
//function : BackgroundColor
//purpose  :
//=======================================================================
Quantity_Color V3d_View::BackgroundColor() const
{
  return MyBackground.Color() ;
}

//=======================================================================
//function : GradientBackgroundColors
//purpose  :
//=======================================================================
void V3d_View::GradientBackgroundColors(Quantity_Color& Color1,Quantity_Color& Color2) const
{
  MyGradientBackground.Colors(Color1, Color2);
}

//=======================================================================
//function : GradientBackground
//purpose  :
//=======================================================================
Aspect_GradientBackground V3d_View::GradientBackground() const
{
   return MyGradientBackground;
}

//=======================================================================
//function : Scale
//purpose  :
//=======================================================================
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

//=======================================================================
//function : AxialScale
//purpose  :
//=======================================================================
void V3d_View::AxialScale(Standard_Real& Sx, Standard_Real& Sy, Standard_Real& Sz) const
{
  gp_Pnt anAxialScale = myCamera->AxialScale();
  Sx = anAxialScale.X();
  Sy = anAxialScale.Y();
  Sz = anAxialScale.Z();
}

//=======================================================================
//function : Size
//purpose  :
//=======================================================================
void V3d_View::Size(Standard_Real& Width, Standard_Real& Height) const
{
  gp_Pnt aViewDims = myCamera->ViewDimensions();

  Width = aViewDims.X();
  Height = aViewDims.Y();
}

//=======================================================================
//function : ZSize
//purpose  :
//=======================================================================
Standard_Real V3d_View::ZSize() const
{
  gp_Pnt aViewDims = myCamera->ViewDimensions();

  return aViewDims.Z();
}

//=======================================================================
//function : MinMax
//purpose  :
//=======================================================================
Standard_Integer V3d_View::MinMax(Standard_Real& Umin,
                                  Standard_Real& Vmin,
                                  Standard_Real& Umax,
                                  Standard_Real& Vmax) const
{
  Standard_Real Wmin,Wmax,U,V,W ;
  Standard_Real Xmin,Ymin,Zmin,Xmax,Ymax,Zmax ;
  // CAL 6/11/98
  Standard_Integer Nstruct = MyView->NumberOfDisplayedStructures() ;

  if( Nstruct ) {
    Bnd_Box aBox = MyView->MinMaxValues();
    aBox.Get (Xmin,Ymin,Zmin,Xmax,Ymax,Zmax);
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

//=======================================================================
//function : MinMax
//purpose  :
//=======================================================================
Standard_Integer V3d_View::MinMax(Standard_Real& Xmin,
                                  Standard_Real& Ymin,
                                  Standard_Real& Zmin,
                                  Standard_Real& Xmax,
                                  Standard_Real& Ymax,
                                  Standard_Real& Zmax) const
{
  // CAL 6/11/98
  // Standard_Integer Nstruct = (MyView->DisplayedStructures())->Extent() ;
  Standard_Integer Nstruct = MyView->NumberOfDisplayedStructures() ;

  if( Nstruct ) {
    Bnd_Box aBox = MyView->MinMaxValues();
    aBox.Get (Xmin,Ymin,Zmin,Xmax,Ymax,Zmax);
  }
  return Nstruct ;
}

//=======================================================================
//function : Gravity
//purpose  :
//=======================================================================
void V3d_View::Gravity (Standard_Real& theX,
                        Standard_Real& theY,
                        Standard_Real& theZ) const
{
  Graphic3d_MapOfStructure aSetOfStructures;
  MyView->DisplayedStructures (aSetOfStructures);

  Standard_Boolean hasSelection = Standard_False;
  for (Graphic3d_MapIteratorOfMapOfStructure aStructIter (aSetOfStructures);
       aStructIter.More(); aStructIter.Next())
  {
    if (aStructIter.Key()->IsHighlighted()
     && aStructIter.Key()->IsVisible())
    {
      hasSelection = Standard_True;
      break;
    }
  }

  Standard_Real Xmin, Ymin, Zmin, Xmax, Ymax, Zmax;
  Standard_Integer aNbPoints = 0;
  gp_XYZ aResult (0.0, 0.0, 0.0);
  for (Graphic3d_MapIteratorOfMapOfStructure aStructIter (aSetOfStructures);
       aStructIter.More(); aStructIter.Next())
  {
    const Handle(Graphic3d_Structure)& aStruct = aStructIter.Key();
    if (!aStruct->IsVisible()
    || (hasSelection && !aStruct->IsHighlighted())
    ||  aStruct->IsEmpty())
    {
      continue;
    }

    Bnd_Box aBox = aStruct->MinMaxValues();
    if (aBox.IsVoid() || aStruct->IsInfinite())
    {
      continue;
    }

    // use camera projection to find gravity point
    aBox.Get (Xmin, Ymin, Zmin,
              Xmax, Ymax, Zmax);
    gp_Pnt aPnts[THE_NB_BOUND_POINTS] =
    {
      gp_Pnt (Xmin, Ymin, Zmin), gp_Pnt (Xmin, Ymin, Zmax),
      gp_Pnt (Xmin, Ymax, Zmin), gp_Pnt (Xmin, Ymax, Zmax),
      gp_Pnt (Xmax, Ymin, Zmin), gp_Pnt (Xmax, Ymin, Zmax),
      gp_Pnt (Xmax, Ymax, Zmin), gp_Pnt (Xmax, Ymax, Zmax)
    };

    for (Standard_Integer aPntIt = 0; aPntIt < THE_NB_BOUND_POINTS; ++aPntIt)
    {
      const gp_Pnt& aBndPnt    = aPnts[aPntIt];
      const gp_Pnt  aProjected = myCamera->Project (aBndPnt);
      if (Abs (aProjected.X()) <= 1.0
       && Abs (aProjected.Y()) <= 1.0)
      {
        aResult += aBndPnt.XYZ();
        ++aNbPoints;
      }
    }
  }

  if (aNbPoints == 0)
  {
    for (Graphic3d_MapIteratorOfMapOfStructure aStructIter (aSetOfStructures);
         aStructIter.More(); aStructIter.Next())
    {
      const Handle(Graphic3d_Structure)& aStruct = aStructIter.Key();
      if (aStruct->IsEmpty())
      {
        continue;
      }

      Bnd_Box aBox = aStruct->MinMaxValues();
      if (aBox.IsVoid() || aStruct->IsInfinite())
      {
        continue;
      }

      aBox.Get (Xmin, Ymin, Zmin,
                Xmax, Ymax, Zmax);
      gp_Pnt aPnts[THE_NB_BOUND_POINTS] =
      {
        gp_Pnt (Xmin, Ymin, Zmin), gp_Pnt (Xmin, Ymin, Zmax),
        gp_Pnt (Xmin, Ymax, Zmin), gp_Pnt (Xmin, Ymax, Zmax),
        gp_Pnt (Xmax, Ymin, Zmin), gp_Pnt (Xmax, Ymin, Zmax),
        gp_Pnt (Xmax, Ymax, Zmin), gp_Pnt (Xmax, Ymax, Zmax)
      };

      for (Standard_Integer aPntIt = 0; aPntIt < THE_NB_BOUND_POINTS; ++aPntIt)
      {
        const gp_Pnt& aBndPnt = aPnts[aPntIt];
        aResult += aBndPnt.XYZ();
        ++aNbPoints;
      }
    }
  }

  if (aNbPoints > 0)
  {
    aResult /= aNbPoints;
  }
  theX = aResult.X();
  theY = aResult.Y();
  theZ = aResult.Z();
}

//=======================================================================
//function : Eye
//purpose  :
//=======================================================================
void V3d_View::Eye(Standard_Real& X, Standard_Real& Y, Standard_Real& Z) const
{
  gp_Pnt aCameraEye = myCamera->Eye();
  X = aCameraEye.X();
  Y = aCameraEye.Y();
  Z = aCameraEye.Z();
}

//=============================================================================
//function : FocalReferencePoint
//purpose  :
//=============================================================================
void V3d_View::FocalReferencePoint(Standard_Real& X, Standard_Real& Y,Standard_Real& Z) const
{
  Eye (X,Y,Z);
}

//=============================================================================
//function : ProjReferenceAxe
//purpose  :
//=============================================================================
void V3d_View::ProjReferenceAxe(const Standard_Integer Xpix,
                                const Standard_Integer Ypix,
                                Standard_Real& XP,
                                Standard_Real& YP,
                                Standard_Real& ZP,
                                Standard_Real& VX,
                                Standard_Real& VY,
                                Standard_Real& VZ) const
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

//=============================================================================
//function : Depth
//purpose  :
//=============================================================================
Standard_Real V3d_View::Depth() const
{
  return myCamera->Distance();
}

//=============================================================================
//function : Proj
//purpose  :
//=============================================================================
void V3d_View::Proj(Standard_Real& Dx, Standard_Real& Dy, Standard_Real& Dz) const
{
  gp_Dir aCameraDir = myCamera->Direction().Reversed();
  Dx = aCameraDir.X();
  Dy = aCameraDir.Y();
  Dz = aCameraDir.Z();
}

//=============================================================================
//function : At
//purpose  :
//=============================================================================
void V3d_View::At(Standard_Real& X, Standard_Real& Y, Standard_Real& Z) const
{
  gp_Pnt aCameraCenter = myCamera->Center();
  X = aCameraCenter.X();
  Y = aCameraCenter.Y();
  Z = aCameraCenter.Z();
}

//=============================================================================
//function : Up
//purpose  :
//=============================================================================
void V3d_View::Up(Standard_Real& Vx, Standard_Real& Vy, Standard_Real& Vz) const
{
  gp_Dir aCameraUp = myCamera->Up();
  Vx = aCameraUp.X();
  Vy = aCameraUp.Y();
  Vz = aCameraUp.Z();
}

//=============================================================================
//function : Twist
//purpose  :
//=============================================================================
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

//=============================================================================
//function : ShadingModel
//purpose  :
//=============================================================================
V3d_TypeOfShadingModel V3d_View::ShadingModel() const
{
  V3d_TypeOfShadingModel SM = (V3d_TypeOfShadingModel)MyViewContext.Model() ;
  return SM ;
}

//=============================================================================
//function : SurfaceDetail
//purpose  :
//=============================================================================
V3d_TypeOfSurfaceDetail V3d_View::SurfaceDetail() const
{
  V3d_TypeOfSurfaceDetail SM = (V3d_TypeOfSurfaceDetail)MyViewContext.SurfaceDetail() ;
  return SM ;
}

//=============================================================================
//function : TextureEnv
//purpose  :
//=============================================================================
Handle(Graphic3d_TextureEnv) V3d_View::TextureEnv() const
{
  Handle(Graphic3d_TextureEnv) SM = MyViewContext.TextureEnv() ;
  return SM ;
}

//=============================================================================
//function : Visualization
//purpose  :
//=============================================================================
V3d_TypeOfVisualization V3d_View::Visualization() const
{
  V3d_TypeOfVisualization V =
    (V3d_TypeOfVisualization)MyViewContext.Visualization() ;
  return V ;
}

//=============================================================================
//function : Antialiasing
//purpose  :
//=============================================================================
Standard_Boolean V3d_View::Antialiasing() const
{
  Standard_Boolean A = MyViewContext.AliasingIsOn() ;
  return A ;
}

//=============================================================================
//function : Viewer
//purpose  :
//=============================================================================
Handle(V3d_Viewer) V3d_View::Viewer() const
{
  return MyViewer ;
}

//=============================================================================
//function : IfWindow
//purpose  :
//=============================================================================
Standard_Boolean V3d_View::IfWindow() const
{
  Standard_Boolean TheStatus = MyView->IsDefined() ;
  return TheStatus ;
}

//=============================================================================
//function : Window
//purpose  :
//=============================================================================
Handle(Aspect_Window) V3d_View::Window() const
{
  return MyWindow;
}

//=============================================================================
//function : Type
//purpose  :
//=============================================================================
V3d_TypeOfView V3d_View::Type() const
{
  return myCamera->IsOrthographic() ? V3d_ORTHOGRAPHIC : V3d_PERSPECTIVE;
}

//=============================================================================
//function : SetFocale
//purpose  :
//=============================================================================
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

//=============================================================================
//function : Focale
//purpose  :
//=============================================================================
Standard_Real V3d_View::Focale() const
{
  if (myCamera->IsOrthographic())
  {
    return 0.0;
  }

  return myCamera->Distance() * 2.0 * Tan(myCamera->FOVy() * M_PI / 360.0);
}

//=============================================================================
//function : View
//purpose  :
//=============================================================================
Handle(Visual3d_View) V3d_View::View() const
{
  return MyView ;
}

//=============================================================================
//function : ScreenAxis
//purpose  :
//=============================================================================
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

//=============================================================================
//function : TrsPoint
//purpose  :
//=============================================================================
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

//=======================================================================
//function : Pan
//purpose  :
//=======================================================================
void V3d_View::Pan (const Standard_Integer theDXp,
                    const Standard_Integer theDYp,
                    const Quantity_Factor  theZoomFactor,
                    const Standard_Boolean theToStart)
{
  Panning (Convert (theDXp), Convert (theDYp), theZoomFactor, theToStart);
}

//=======================================================================
//function : Panning
//purpose  :
//=======================================================================
void V3d_View::Panning (const Standard_Real theDXv,
                        const Standard_Real theDYv,
                        const Quantity_Factor theZoomFactor,
                        const Standard_Boolean theToStart)
{
  Standard_ASSERT_RAISE (theZoomFactor > 0.0, "Bad zoom factor");

  if (theToStart)
  {
    myCamStartOpEye = myCamera->Eye();
    myCamStartOpCenter = myCamera->Center();
  }

  Standard_Boolean wasUpdateEnabled = SetImmediateUpdate (Standard_False);

  gp_Pnt aViewDims = myCamera->ViewDimensions();

  myCamera->SetEye (myCamStartOpEye);
  myCamera->SetCenter (myCamStartOpCenter);
  Translate (myCamera, -theDXv, -theDYv);
  Scale (myCamera, aViewDims.X() / theZoomFactor, aViewDims.Y() / theZoomFactor);

  SetImmediateUpdate (wasUpdateEnabled);

  ImmediateUpdate();
}

//=======================================================================
//function : Zoom
//purpose  :
//=======================================================================
void V3d_View::Zoom (const Standard_Integer theXp1,
                     const Standard_Integer theYp1,
                     const Standard_Integer theXp2,
                     const Standard_Integer theYp2)
{
  Standard_Integer aDx = theXp2 - theXp1;
  Standard_Integer aDy = theYp2 - theYp1;
  if (aDx != 0 || aDy != 0)
  {
    Standard_Real aCoeff = Sqrt( (Standard_Real)(aDx * aDx + aDy * aDy) ) / 100.0 + 1.0;
    aCoeff = (aDx > 0) ? aCoeff : 1.0 / aCoeff;
    SetZoom (aCoeff, Standard_True);
  }
}

//=======================================================================
//function : StartZoomAtPoint
//purpose  :
//=======================================================================
void V3d_View::StartZoomAtPoint (const Standard_Integer theXp,
                                 const Standard_Integer theYp)
{
  MyZoomAtPointX = theXp;
  MyZoomAtPointY = theYp;
}

//=======================================================================
//function : ZoomAtPoint
//purpose  :
//=======================================================================
void V3d_View::ZoomAtPoint (const Standard_Integer theMouseStartX,
                            const Standard_Integer theMouseStartY,
                            const Standard_Integer theMouseEndX,
                            const Standard_Integer theMouseEndY)
{
  Standard_Boolean wasUpdateEnabled = SetImmediateUpdate (Standard_False);

  // zoom
  Standard_Real aDxy = Standard_Real ((theMouseEndX + theMouseEndY) - (theMouseStartX + theMouseStartY));
  Standard_Real aDZoom = Abs (aDxy) / 100.0 + 1.0;
  aDZoom = (aDxy > 0.0) ?  aDZoom : 1.0 / aDZoom;

  V3d_BadValue_Raise_if (aDZoom <= 0.0, "V3d_View::ZoomAtPoint, bad coefficient");

  Standard_Real aViewWidth  = myCamera->ViewDimensions().X();
  Standard_Real aViewHeight = myCamera->ViewDimensions().Y();

  // ensure that zoom will not be too small or too big.
  Standard_Real aCoef = aDZoom;
  if (aViewWidth < aCoef * Precision::Confusion())
  {
    aCoef = aViewWidth / Precision::Confusion();
  }
  else if (aViewWidth > aCoef * 1e12)
  {
    aCoef = aViewWidth / 1e12;
  }
  if (aViewHeight < aCoef * Precision::Confusion())
  {
    aCoef = aViewHeight / Precision::Confusion();
  }
  else if (aViewHeight > aCoef * 1e12)
  {
    aCoef = aViewHeight / 1e12;
  }

  Standard_Real aZoomAtPointXv = 0.0;
  Standard_Real aZoomAtPointYv = 0.0;
  Convert (MyZoomAtPointX, MyZoomAtPointY, aZoomAtPointXv, aZoomAtPointYv);

  V3d_Coordinate aDxv = aZoomAtPointXv / aCoef;
  V3d_Coordinate aDyv = aZoomAtPointYv / aCoef;

  myCamera->SetScale (myCamera->Scale() / aCoef);
  Translate (myCamera, aZoomAtPointXv - aDxv, aZoomAtPointYv - aDyv);

  View()->AutoZFit();

  SetImmediateUpdate (wasUpdateEnabled);

  ImmediateUpdate();
}

//=============================================================================
//function : AxialScale
//purpose  :
//=============================================================================
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

//=============================================================================
//function : FitAll
//purpose  :
//=============================================================================
void V3d_View::FitAll(const Handle(Aspect_Window)& aWindow,
                      const Standard_Real Xmin,
                      const Standard_Real Ymin,
                      const Standard_Real Xmax,
                      const Standard_Real Ymax)
{
  Standard_Integer aWinWidth, aWinHeight;
  aWindow->Size (aWinWidth, aWinHeight);

  Standard_Real aWinAspect = (Standard_Real)aWinWidth / aWinHeight;
  Standard_Real aFitSizeU  = Abs (Xmax - Xmin);
  Standard_Real aFitSizeV  = Abs (Ymax - Ymin);
  Standard_Real aFitAspect = aFitSizeU / aFitSizeV;
  if (aFitAspect >= aWinAspect)
  {
    aFitSizeV = aFitSizeU / aWinAspect;
  }
  else
  {
    aFitSizeU = aFitSizeV * aWinAspect;
  }

  myCamera->SetAspect (aWinAspect);
  Translate (myCamera, (Xmin + Xmax) * 0.5, (Ymin + Ymax) * 0.5);
  Scale (myCamera, aFitSizeU, aFitSizeV);
  View()->AutoZFit();

  ImmediateUpdate();
}

//=============================================================================
//function : StartRotation
//purpose  :
//=============================================================================
static Standard_Boolean zRotation = Standard_False;
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
  zRotation = Standard_False;
  if( zRotationThreshold > 0. ) {
    Standard_Real dx = Abs(sx - rx/2.);
    Standard_Real dy = Abs(sy - ry/2.);
    //  if( dx > rx/3. || dy > ry/3. ) zRotation = Standard_True;
    Standard_Real dd = zRotationThreshold * (rx + ry)/2.;
    if( dx > dd || dy > dd ) zRotation = Standard_True;
  }

}

//=============================================================================
//function : Rotation
//purpose  :
//=============================================================================
void V3d_View::Rotation(const Standard_Integer X,
                        const Standard_Integer Y)
{
  if( rx == 0. || ry == 0. ) {
    StartRotation(X,Y);
    return;
  }
  Standard_Real dx=0.,dy=0.,dz=0.;
  if( zRotation ) {
    dz = atan2(Standard_Real(X)-rx/2., ry/2.-Standard_Real(Y)) -
      atan2(sx-rx/2.,ry/2.-sy);
  } else {
    dx = (Standard_Real(X) - sx) * M_PI / rx;
    dy = (sy - Standard_Real(Y)) * M_PI / ry;
  }

  Rotate(dx, dy, dz, gx, gy, gz, Standard_False);
}

//=============================================================================
//function : SetComputedMode
//purpose  :
//=============================================================================
void V3d_View::SetComputedMode (const Standard_Boolean aMode)
{
  if (aMode)
  {
    if (myComputedMode)
    {
      MyView->SetComputedMode (Standard_True);
      Update();
    }
  } 
  else 
  {
    MyView->SetComputedMode (Standard_False);
    Update();
  }
}

//=============================================================================
//function : ComputedMode
//purpose  :
//=============================================================================
Standard_Boolean V3d_View::ComputedMode() const
{
  return MyView->ComputedMode();
}

//=============================================================================
//function : SetBackFacingModel
//purpose  :
//=============================================================================
void V3d_View::SetBackFacingModel (const V3d_TypeOfBackfacingModel aModel)
{
  MyView->SetBackFacingModel (Visual3d_TypeOfBackfacingModel(aModel));
  Redraw();
}

//=============================================================================
//function : BackFacingModel
//purpose  :
//=============================================================================
V3d_TypeOfBackfacingModel V3d_View::BackFacingModel() const
{
  return V3d_TypeOfBackfacingModel(MyView -> BackFacingModel ());
}

void V3d_View::Init()
{
  myComputedMode = MyViewer->ComputedMode();
  if( !myComputedMode || !MyViewer->DefaultComputedMode() ) {
    SetComputedMode(Standard_False);
  }
}

//=============================================================================
//function : Dump
//purpose  :
//=============================================================================
Standard_Boolean V3d_View::Dump (const Standard_CString      theFile,
                                 const Graphic3d_BufferType& theBufferType)
{
  Standard_Integer aWinWidth, aWinHeight;
  MyWindow->Size (aWinWidth, aWinHeight);
  Image_AlienPixMap anImage;

  return ToPixMap (anImage, aWinWidth, aWinHeight, theBufferType) && anImage.Save (theFile);
}

//=============================================================================
//function : ToPixMap
//purpose  :
//=============================================================================
Standard_Boolean V3d_View::ToPixMap (Image_PixMap&               theImage,
                                     const Standard_Integer      theWidth,
                                     const Standard_Integer      theHeight,
                                     const Graphic3d_BufferType& theBufferType,
                                     const Standard_Boolean      theToKeepAspect,
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

  if (myCamera->IsStereo())
  {
    switch (theStereoOptions)
    {
      case V3d_SDO_MONO:
      {
        myCamera->SetProjectionType (Graphic3d_Camera::Projection_Perspective);
        break;
      }
      case V3d_SDO_LEFT_EYE:
      {
        myCamera->SetProjectionType (Graphic3d_Camera::Projection_MonoLeftEye);
        break;
      }
      case V3d_SDO_RIGHT_EYE:
      {
        myCamera->SetProjectionType (Graphic3d_Camera::Projection_MonoRightEye);
        break;
      }
      case V3d_SDO_BLENDED:
      {
        break; // dump as is
      }
    }
  }

  // render immediate structures into back buffer rather than front
  Handle(Graphic3d_GraphicDriver) aDriver = Handle(Graphic3d_GraphicDriver)::DownCast (MyView->GraphicDriver());
  const Standard_Boolean aPrevImmediateMode = aDriver.IsNull() ? Standard_True : aDriver->SetImmediateModeDrawToFront (*cView, Standard_False);

  const Standard_Boolean toAutoUpdate = myImmediateUpdate;
  myImmediateUpdate = Standard_False;
  View()->AutoZFit();
  myImmediateUpdate = toAutoUpdate;

  if (theToKeepAspect)
  {
    myCamera->SetAspect ((Standard_Real) aFBOVPSizeX / aFBOVPSizeY);
  }

  //workaround for rendering list of Over and Under Layers
  if (!MyLayerMgr.IsNull())
  {
    MyLayerMgr->Compute();
  }

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
  Standard_ASSERT_RAISE (!theCamera.IsNull(), "Void camera is not allowed");

  myCamera = theCamera;

  MyView->SetCamera (theCamera);
}

// =======================================================================
// function : GetCamera
// purpose  :
// =======================================================================
const Handle(Graphic3d_Camera)& V3d_View::Camera() const
{
  return myCamera;
}

// =======================================================================
// function : FitMinMax
// purpose  : Internal
// =======================================================================
Standard_Boolean V3d_View::FitMinMax (const Handle(Graphic3d_Camera)& theCamera,
                                      const Bnd_Box& theBox,
                                      const Standard_Real theMargin,
                                      const Standard_Real theResolution,
                                      const Standard_Boolean theToEnlargeIfLine) const
{
  // Check bounding box for validness
  if (theBox.IsVoid())
  {
    return Standard_False; // bounding box is out of bounds...
  }

  // Apply "axial scaling" to the bounding points.
  // It is not the best approach to make this scaling as a part of fit all operation,
  // but the axial scale is integrated into camera orientation matrix and the other
  // option is to perform frustum plane adjustment algorithm in view camera space,
  // which will lead to a number of additional world-view space conversions and
  // loosing precision as well.
  gp_Pnt aBndMin = theBox.CornerMin().XYZ().Multiplied (theCamera->AxialScale());
  gp_Pnt aBndMax = theBox.CornerMax().XYZ().Multiplied (theCamera->AxialScale());

  if (aBndMax.IsEqual (aBndMin, RealEpsilon()))
  {
    return Standard_False; // nothing to fit all
  }

  // Prepare camera frustum planes.
  NCollection_Array1<gp_Pln> aFrustumPlane (1, 6);
  theCamera->Frustum (aFrustumPlane.ChangeValue (1),
                      aFrustumPlane.ChangeValue (2),
                      aFrustumPlane.ChangeValue (3),
                      aFrustumPlane.ChangeValue (4),
                      aFrustumPlane.ChangeValue (5),
                      aFrustumPlane.ChangeValue (6));

  // Prepare camera up, side, direction vectors.
  gp_Dir aCamUp  = theCamera->OrthogonalizedUp();
  gp_Dir aCamDir = theCamera->Direction();
  gp_Dir aCamSide = aCamDir ^ aCamUp;

  // Prepare scene bounding box parameters.
  gp_Pnt aBndCenter = (aBndMin.XYZ() + aBndMax.XYZ()) / 2.0;

  NCollection_Array1<gp_Pnt> aBndCorner (1, 8);
  aBndCorner.ChangeValue (1) = gp_Pnt (aBndMin.X(), aBndMin.Y(), aBndMin.Z());
  aBndCorner.ChangeValue (2) = gp_Pnt (aBndMin.X(), aBndMin.Y(), aBndMax.Z());
  aBndCorner.ChangeValue (3) = gp_Pnt (aBndMin.X(), aBndMax.Y(), aBndMin.Z());
  aBndCorner.ChangeValue (4) = gp_Pnt (aBndMin.X(), aBndMax.Y(), aBndMax.Z());
  aBndCorner.ChangeValue (5) = gp_Pnt (aBndMax.X(), aBndMin.Y(), aBndMin.Z());
  aBndCorner.ChangeValue (6) = gp_Pnt (aBndMax.X(), aBndMin.Y(), aBndMax.Z());
  aBndCorner.ChangeValue (7) = gp_Pnt (aBndMax.X(), aBndMax.Y(), aBndMin.Z());
  aBndCorner.ChangeValue (8) = gp_Pnt (aBndMax.X(), aBndMax.Y(), aBndMax.Z());

  // Perspective-correct camera projection vector, matching the bounding box is determined geometrically.
  // Knowing the initial shape of a frustum it is possible to match it to a bounding box.
  // Then, knowing the relation of camera projection vector to the frustum shape it is possible to
  // set up perspective-correct camera projection matching the bounding box.
  // These steps support non-asymmetric transformations of view-projection space provided by camera.
  // The zooming can be done by calculating view plane size matching the bounding box at center of
  // the bounding box. The only limitation here is that the scale of camera should define size of
  // its view plane passing through the camera center, and the center of camera should be on the
  // same line with the center of bounding box.

  // The following method is applied:
  // 1) Determine normalized asymmetry of camera projection vector by frustum planes.
  // 2) Determine new location of frustum planes, "matching" the bounding box.
  // 3) Determine new camera projection vector using the normalized asymmetry.
  // 4) Determine new zooming in view space.

  // 1. Determine normalized projection asymmetry (if any).
  Standard_Real anAssymX = Tan ( aCamSide.Angle (aFrustumPlane (1).Axis().Direction()))
                         - Tan (-aCamSide.Angle (aFrustumPlane (2).Axis().Direction()));
  Standard_Real anAssymY = Tan ( aCamUp.Angle   (aFrustumPlane (3).Axis().Direction()))
                         - Tan (-aCamUp.Angle   (aFrustumPlane (4).Axis().Direction()));

  // 2. Determine how far should be the frustum planes placed from center
  //    of bounding box, in order to match the bounding box closely.
  NCollection_Array1<Standard_Real> aFitDistance (1, 6);
  aFitDistance.ChangeValue (1) = 0.0;
  aFitDistance.ChangeValue (2) = 0.0;
  aFitDistance.ChangeValue (3) = 0.0;
  aFitDistance.ChangeValue (4) = 0.0;
  aFitDistance.ChangeValue (5) = 0.0;
  aFitDistance.ChangeValue (6) = 0.0;

  for (Standard_Integer anI = aFrustumPlane.Lower(); anI <= aFrustumPlane.Upper(); ++anI)
  {
    // Measure distances from center of bounding box to its corners towards the frustum plane.
    const gp_Dir& aPlaneN = aFrustumPlane.ChangeValue (anI).Axis().Direction();

    Standard_Real& aFitDist = aFitDistance.ChangeValue (anI);

    for (Standard_Integer aJ = aBndCorner.Lower(); aJ <= aBndCorner.Upper(); ++aJ)
    {
      aFitDist = Max (aFitDist, gp_Vec (aBndCenter, aBndCorner (aJ)).Dot (aPlaneN));
    }
  }
  // The center of camera is placed on the same line with center of bounding box.
  // The view plane section crosses the bounding box at its center.
  // To compute view plane size, evaluate coefficients converting "point -> plane distance"
  // into view section size between the point and the frustum plane.
  //       proj
  //       /|\   right half of frame     //
  //        |                           //
  //  point o<--  distance * coeff  -->//---- (view plane section)
  //         \                        //
  //      (distance)                 // 
  //                ~               //
  //                 (distance)    //
  //                           \/\//
  //                            \//
  //                            //
  //                      (frustum plane)
  aFitDistance.ChangeValue (1) *= Sqrt(1 + Pow (Tan ( aCamSide.Angle (aFrustumPlane (1).Axis().Direction())), 2.0));
  aFitDistance.ChangeValue (2) *= Sqrt(1 + Pow (Tan (-aCamSide.Angle (aFrustumPlane (2).Axis().Direction())), 2.0));
  aFitDistance.ChangeValue (3) *= Sqrt(1 + Pow (Tan ( aCamUp.Angle   (aFrustumPlane (3).Axis().Direction())), 2.0));
  aFitDistance.ChangeValue (4) *= Sqrt(1 + Pow (Tan (-aCamUp.Angle   (aFrustumPlane (4).Axis().Direction())), 2.0));
  aFitDistance.ChangeValue (5) *= Sqrt(1 + Pow (Tan ( aCamDir.Angle  (aFrustumPlane (5).Axis().Direction())), 2.0));
  aFitDistance.ChangeValue (6) *= Sqrt(1 + Pow (Tan (-aCamDir.Angle  (aFrustumPlane (6).Axis().Direction())), 2.0));

  Standard_Real aViewSizeXv = aFitDistance (1) + aFitDistance (2);
  Standard_Real aViewSizeYv = aFitDistance (3) + aFitDistance (4);
  Standard_Real aViewSizeZv = aFitDistance (5) + aFitDistance (6);

  // 3. Place center of camera on the same line with center of bounding
  //    box applying corresponding projection asymmetry (if any).
  Standard_Real anAssymXv = anAssymX * aViewSizeXv * 0.5;
  Standard_Real anAssymYv = anAssymY * aViewSizeYv * 0.5;
  Standard_Real anOffsetXv = (aFitDistance (2) - aFitDistance (1)) * 0.5 + anAssymXv;
  Standard_Real anOffsetYv = (aFitDistance (4) - aFitDistance (3)) * 0.5 + anAssymYv;
  gp_Vec aTranslateSide = gp_Vec (aCamSide) * anOffsetXv;
  gp_Vec aTranslateUp   = gp_Vec (aCamUp)   * anOffsetYv;
  gp_Pnt aCamNewCenter  = aBndCenter.Translated (aTranslateSide).Translated (aTranslateUp);

  gp_Trsf aCenterTrsf;
  aCenterTrsf.SetTranslation (theCamera->Center(), aCamNewCenter);
  theCamera->Transform (aCenterTrsf);
  theCamera->SetDistance (aFitDistance (6) + aFitDistance (5));

  // Bounding box collapses to a point or thin line going in depth of the screen
  if (aViewSizeXv < theResolution && aViewSizeYv < theResolution)
  {
    if (aViewSizeXv < theResolution || !theToEnlargeIfLine)
    {
      return Standard_True; // This is just one point or line and zooming has no effect.
    }

    // Looking along line and "theToEnlargeIfLine" is requested.
    // Fit view to see whole scene on rotation.
    aViewSizeXv = aViewSizeZv;
    aViewSizeYv = aViewSizeZv;
  }

  Scale (theCamera, aViewSizeXv * (1.0 + theMargin), aViewSizeYv * (1.0 + theMargin));

  return Standard_True;
}

// =======================================================================
// function : Scale
// purpose  : Internal
// =======================================================================
void V3d_View::Scale (const Handle(Graphic3d_Camera)& theCamera,
                      const Standard_Real theSizeXv,
                      const Standard_Real theSizeYv) const
{
  Standard_Real anAspect = theCamera->Aspect();
  if (anAspect > 1.0)
  {
    theCamera->SetScale (Max (theSizeXv / anAspect, theSizeYv));
  }
  else
  {
    theCamera->SetScale (Max (theSizeXv, theSizeYv * anAspect));
  }
}

// =======================================================================
// function : Translate
// purpose  : Internal
// =======================================================================
void V3d_View::Translate (const Handle(Graphic3d_Camera)& theCamera,
                          const Standard_Real theDXv,
                          const Standard_Real theDYv) const
{
  const gp_Pnt& aCenter = theCamera->Center();
  const gp_Dir& aDir = theCamera->Direction();
  const gp_Dir& anUp = theCamera->Up();
  gp_Ax3 aCameraCS (aCenter, aDir.Reversed(), aDir ^ anUp);

  gp_Vec aCameraPanXv = gp_Vec (aCameraCS.XDirection()) * theDXv;
  gp_Vec aCameraPanYv = gp_Vec (aCameraCS.YDirection()) * theDYv;
  gp_Vec aCameraPan = aCameraPanXv + aCameraPanYv;
  gp_Trsf aPanTrsf;
  aPanTrsf.SetTranslation (aCameraPan);

  theCamera->Transform (aPanTrsf);
}

// =======================================================================
// function : IsCullingEnabled
// purpose  :
// =======================================================================
Standard_Boolean V3d_View::IsCullingEnabled() const
{
  Graphic3d_CView* aView = (Graphic3d_CView* )MyView->CView();
  return aView->IsCullingEnabled;
}

// =======================================================================
// function : SetFrustumCulling
// purpose  :
// =======================================================================
void V3d_View::SetFrustumCulling (const Standard_Boolean theToClip)
{
  Graphic3d_CView* aView = (Graphic3d_CView* )MyView->CView();
  aView->IsCullingEnabled = theToClip;
}
