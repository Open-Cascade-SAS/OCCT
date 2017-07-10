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

#include <V3d_View.hxx>

#include <Aspect_GradientBackground.hxx>
#include <Aspect_Grid.hxx>
#include <Aspect_Window.hxx>
#include <Bnd_Box.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_MapIteratorOfMapOfStructure.hxx>
#include <Graphic3d_MapOfStructure.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_TextureEnv.hxx>
#include <Graphic3d_Vector.hxx>
#include <Image_AlienPixMap.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Assert.hxx>
#include <Standard_DivideByZero.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_MultiplyDefined.hxx>
#include <Standard_ShortReal.hxx>
#include <Standard_Type.hxx>
#include <Standard_TypeMismatch.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TColStd_HSequenceOfInteger.hxx>
#include <V3d.hxx>
#include <V3d_BadValue.hxx>
#include <V3d_Light.hxx>
#include <V3d_StereoDumpOptions.hxx>
#include <V3d_UnMapped.hxx>
#include <V3d_Viewer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(V3d_View,Standard_Transient)

#define DEUXPI (2. * M_PI)

namespace
{
  static const Standard_Integer THE_NB_BOUND_POINTS = 8;
}

//=============================================================================
//function : Constructor
//purpose  :
//=============================================================================
V3d_View::V3d_View (const Handle(V3d_Viewer)& theViewer, const V3d_TypeOfView theType)
: MyViewer (theViewer.operator->()),
  SwitchSetFront (Standard_False),
  myZRotation (Standard_False),
  myTrihedron (new V3d_Trihedron()),
  MyTrsf (1, 4, 1, 4)
{
  myView = theViewer->Driver()->CreateView (theViewer->StructureManager());

  myView->SetBackground         (theViewer->GetBackgroundColor());
  myView->SetGradientBackground (theViewer->GetGradientBackground());

  ChangeRenderingParams() = theViewer->DefaultRenderingParams();

  // camera init
  Handle(Graphic3d_Camera) aCamera = new Graphic3d_Camera();
  aCamera->SetFOVy (45.0);
  aCamera->SetIOD (Graphic3d_Camera::IODType_Relative, 0.05);
  aCamera->SetZFocus (Graphic3d_Camera::FocusType_Relative, 1.0);
  aCamera->SetProjectionType ((theType == V3d_ORTHOGRAPHIC)
    ? Graphic3d_Camera::Projection_Orthographic
    : Graphic3d_Camera::Projection_Perspective);

  myDefaultCamera = new Graphic3d_Camera();

  myImmediateUpdate = Standard_False;
  SetAutoZFitMode (Standard_True, 1.0);
  SetBackFacingModel (V3d_TOBM_AUTOMATIC);
  SetCamera (aCamera);
  SetAxis (0.,0.,0.,1.,1.,1.);
  SetVisualization (theViewer->DefaultVisualization());
  SetShadingModel (theViewer->DefaultShadingModel());
  SetTwist (0.);
  SetAt (0.,0.,0.);
  SetProj (theViewer->DefaultViewProj());
  SetSize (theViewer->DefaultViewSize());
  Standard_Real zsize = theViewer->DefaultViewSize();
  SetZSize (2.*zsize);
  SetDepth (theViewer->DefaultViewSize() / 2.0);
  SetViewMappingDefault();
  SetViewOrientationDefault();
  theViewer->AddView (this);
  Init();
  myImmediateUpdate = Standard_True;
}

//=============================================================================
//function : Constructor
//purpose  :
//=============================================================================
V3d_View::V3d_View (const Handle(V3d_Viewer)& theViewer, const Handle(V3d_View)& theView)
: MyViewer (theViewer.operator->()),
  SwitchSetFront(Standard_False),
  myZRotation (Standard_False),
  MyTrsf (1, 4, 1, 4)
{
  myView = theViewer->Driver()->CreateView (theViewer->StructureManager());

  myView->CopySettings (theView->View());

  myDefaultCamera = new Graphic3d_Camera();

  myImmediateUpdate = Standard_False;
  SetAutoZFitMode (theView->AutoZFitMode(), theView->AutoZFitScaleFactor());
  SetAxis (0.0, 0.0, 0.0, 1.0, 1.0, 1.0);
  SetViewMappingDefault();
  SetViewOrientationDefault();
  theViewer->AddView (this);
  Init();
  myImmediateUpdate = Standard_True;
}

//=============================================================================
//function : Destructor
//purpose  :
//=============================================================================
V3d_View::~V3d_View()
{
  if (!myView->IsRemoved())
  {
    myView->Remove();
  }
}

//=============================================================================
//function : SetMagnify
//purpose  :
//=============================================================================
void V3d_View::SetMagnify (const Handle(Aspect_Window)& theWindow,
                           const Handle(V3d_View)& thePreviousView,
                           const Standard_Integer theX1,
                           const Standard_Integer theY1,
                           const Standard_Integer theX2,
                           const Standard_Integer theY2)
{
  if (!myView->IsRemoved() && !myView->IsDefined())
  {
    Standard_Real aU1, aV1, aU2, aV2;
    thePreviousView->Convert (theX1, theY1, aU1, aV1);
    thePreviousView->Convert (theX2, theY2, aU2, aV2);
    myView->SetWindow (theWindow);
    FitAll (aU1, aV1, aU2, aV2);
    MyViewer->SetViewOn (this);
    MyWindow = theWindow;
    SetRatio();
    Redraw();
    SetViewMappingDefault();
  }
}

//=============================================================================
//function : SetWindow
//purpose  :
//=============================================================================
void V3d_View::SetWindow (const Handle(Aspect_Window)&  theWindow,
                          const Aspect_RenderingContext theContext)
{
  if (myView->IsRemoved())
  {
    return;
  }

  // method V3d_View::SetWindow() should assign the field MyWindow before calling Redraw()
  MyWindow = theWindow;
  myView->SetWindow (theWindow, theContext);
  MyViewer->SetViewOn (this);
  SetRatio();
  Redraw();
}

//=============================================================================
//function : Remove
//purpose  :
//=============================================================================
void V3d_View::Remove() const
{
  if (!MyGrid.IsNull())
  {
    MyGrid->Erase();
  }
  if (!myTrihedron.IsNull())
  {
    myTrihedron->Erase();
  }

  MyViewer->DelView (this);
  myView->Remove();
  Handle(Aspect_Window)& aWin = const_cast<Handle(Aspect_Window)&> (MyWindow);
  aWin.Nullify();
}

//=============================================================================
//function : Update
//purpose  :
//=============================================================================
void V3d_View::Update() const
{
  if (!myView->IsDefined()
   || !myView->IsActive())
  {
    return;
  }

  myView->Update();
  myView->Compute();
  myView->Redraw();
}

//=============================================================================
//function : Redraw
//purpose  :
//=============================================================================
void V3d_View::Redraw() const
{
  if (!myView->IsDefined()
   || !myView->IsActive())
  {
    return;
  }

  Handle(Graphic3d_StructureManager) aStructureMgr  = MyViewer->StructureManager();
  for (Standard_Integer aRetryIter = 0; aRetryIter < 2; ++aRetryIter)
  {
    if (aStructureMgr->IsDeviceLost())
    {
      aStructureMgr->RecomputeStructures();
    }

    AutoZFit();

    myView->Redraw();

    if (!aStructureMgr->IsDeviceLost())
    {
      return;
    }
  }
}

//=============================================================================
//function : RedrawImmediate
//purpose  :
//=============================================================================
void V3d_View::RedrawImmediate() const
{
  if (!myView->IsDefined()
   || !myView->IsActive())
  {
    return;
  }

  myView->RedrawImmediate();
}

//=============================================================================
//function : Invalidate
//purpose  :
//=============================================================================
void V3d_View::Invalidate() const
{
  if (!myView->IsDefined())
  {
    return;
  }

  myView->Invalidate();
}

//=============================================================================
//function : IsInvalidated
//purpose  :
//=============================================================================
Standard_Boolean V3d_View::IsInvalidated() const
{
  return !myView->IsDefined()
       || myView->IsInvalidated();
}

// ========================================================================
// function : SetAutoZFitMode
// purpose  :
// ========================================================================
void V3d_View::SetAutoZFitMode (const Standard_Boolean theIsOn,
                                const Standard_Real    theScaleFactor)
{
  Standard_ASSERT_RAISE (theScaleFactor > 0.0, "Zero or negative scale factor is not allowed.");
  myAutoZFitScaleFactor = theScaleFactor;
  myAutoZFitIsOn = theIsOn;
}

// ========================================================================
// function : AutoZFitMode
// purpose  :
// ========================================================================
Standard_Boolean V3d_View::AutoZFitMode() const
{
  return myAutoZFitIsOn;
}

// ========================================================================
// function : AutoZFitScaleFactor
// purpose  :
// ========================================================================
Standard_Real V3d_View::AutoZFitScaleFactor() const
{
  return myAutoZFitScaleFactor;
}

//=============================================================================
//function : AutoZFit
//purpose  :
//=============================================================================
void V3d_View::AutoZFit() const
{
  if (!AutoZFitMode())
  {
    return;
  }

  ZFitAll (myAutoZFitScaleFactor);
}

//=============================================================================
//function : ZFitAll
//purpose  :
//=============================================================================
void V3d_View::ZFitAll (const Standard_Real theScaleFactor) const
{
  Bnd_Box aMinMaxBox   = myView->MinMaxValues (Standard_False); // applicative min max boundaries
  Bnd_Box aGraphicBox  = myView->MinMaxValues (Standard_True);  // real graphical boundaries (not accounting infinite flag).

  myView->Camera()->ZFitAll (theScaleFactor, aMinMaxBox, aGraphicBox);
}

//=============================================================================
//function : IsEmpty
//purpose  :
//=============================================================================
Standard_Boolean V3d_View::IsEmpty() const
{
  Standard_Boolean TheStatus = Standard_True ;
  if( myView->IsDefined() ) {
    Standard_Integer Nstruct = myView->NumberOfDisplayedStructures() ;
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
  Graphic3d_ListOfCLight aLights;
  for (V3d_ListOfLight::Iterator anActiveLightIter (myActiveLights); anActiveLightIter.More(); anActiveLightIter.Next())
  {
    aLights.Append (anActiveLightIter.Value()->Light());
  }
  myView->SetLights (aLights);
  Update();
}

//=============================================================================
//function : DoMapping
//purpose  :
//=============================================================================
void V3d_View::DoMapping()
{
  if (!myView->IsDefined())
  {
    return;
  }

  myView->Window()->DoMapping();
}

//=============================================================================
//function : MustBeResized
//purpose  :
//=============================================================================
void V3d_View::MustBeResized()
{
  if (!myView->IsDefined())
  {
    return;
  }

  myView->Resized();

  SetRatio();

  Redraw();
}

//=============================================================================
//function : SetBackgroundColor
//purpose  :
//=============================================================================
void V3d_View::SetBackgroundColor (const Quantity_TypeOfColor theType,
                                   const Standard_Real theV1,
                                   const Standard_Real theV2,
                                   const Standard_Real theV3)
{
  Standard_Real aV1 = Max (Min (theV1, 1.0), 0.0);
  Standard_Real aV2 = Max (Min (theV2, 1.0), 0.0);
  Standard_Real aV3 = Max (Min (theV3, 1.0), 0.0);

  SetBackgroundColor (Quantity_Color (aV1, aV2, aV3, theType));
}

//=============================================================================
//function : SetBackgroundColor
//purpose  :
//=============================================================================
void V3d_View::SetBackgroundColor (const Quantity_Color& theColor)
{
  myView->SetBackground (Aspect_Background (theColor));

  if (myImmediateUpdate)
  {
    Redraw();
  }
}

//=============================================================================
//function : SetBgGradientColors
//purpose  :
//=============================================================================
void V3d_View::SetBgGradientColors (const Quantity_Color& theColor1,
                                    const Quantity_Color& theColor2,
                                    const Aspect_GradientFillMethod theFillStyle,
                                    const Standard_Boolean theToUpdate)
{
  Aspect_GradientBackground aGradientBg (theColor1, theColor2, theFillStyle);

  myView->SetGradientBackground (aGradientBg);

  if (myImmediateUpdate || theToUpdate)
  {
    Redraw();
  }
}

//=============================================================================
//function : SetBgGradientStyle
//purpose  :
//=============================================================================
void V3d_View::SetBgGradientStyle (const Aspect_GradientFillMethod theFillStyle, const Standard_Boolean theToUpdate)
{
  Quantity_Color aColor1;
  Quantity_Color aColor2;
  GradientBackground().Colors (aColor1, aColor2);

  SetBgGradientColors (aColor1, aColor2, theFillStyle, theToUpdate);
}

//=============================================================================
//function : SetBackgroundImage
//purpose  :
//=============================================================================
void V3d_View::SetBackgroundImage (const Standard_CString theFileName,
                                   const Aspect_FillMethod theFillStyle,
                                   const Standard_Boolean theToUpdate)
{
  myView->SetBackgroundImage (theFileName);
  myView->SetBackgroundImageStyle (theFillStyle);

  if (myImmediateUpdate || theToUpdate)
  {
    Redraw();
  }
}

//=============================================================================
//function : SetBgImageStyle
//purpose  :
//=============================================================================
void V3d_View::SetBgImageStyle (const Aspect_FillMethod theFillStyle, const Standard_Boolean theToUpdate)
{
  myView->SetBackgroundImageStyle (theFillStyle);

  if (myImmediateUpdate || theToUpdate)
  {
    Redraw();
  }
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
void V3d_View::SetShadingModel (const V3d_TypeOfShadingModel theShadingModel)
{
  myView->SetShadingModel (static_cast<Graphic3d_TypeOfShadingModel> (theShadingModel));
}

//=============================================================================
//function : SetTextureEnv
//purpose  :
//=============================================================================
void V3d_View::SetTextureEnv (const Handle(Graphic3d_TextureEnv)& theTexture)
{
  myView->SetTextureEnv (theTexture);

  if (myImmediateUpdate)
  {
    Redraw();
  }
}

//=============================================================================
//function : SetVisualization
//purpose  :
//=============================================================================
void V3d_View::SetVisualization (const V3d_TypeOfVisualization theType)
{
  myView->SetVisualizationType (static_cast <Graphic3d_TypeOfVisualization> (theType));

  if (myImmediateUpdate)
  {
    Redraw();
  }
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

  Handle(Graphic3d_Camera) aCamera = Camera();

  aCamera->SetCenter (gp_Pnt (xo, yo, zo));

  if(SwitchSetFront)
  {
    aCamera->SetDirection (gp_Dir (vx, vy, vz));
  }
  else
  {
    aCamera->SetDirection (gp_Dir (vx, vy, vz).Reversed());
  }

  aCamera->SetUp (gp_Dir (xu, yu, zu));

  AutoZFit();

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

  Handle(Graphic3d_Camera) aCamera = Camera();

  if (Start)
  {
    myCamStartOpUp     = aCamera->Up();
    myCamStartOpEye    = aCamera->Eye();
    myCamStartOpCenter = aCamera->Center();
  }

  aCamera->SetUp     (myCamStartOpUp);
  aCamera->SetEye    (myCamStartOpEye);
  aCamera->SetCenter (myCamStartOpCenter);

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

  aCamera->Transform (aTrsf);

  AutoZFit();

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

  Handle(Graphic3d_Camera) aCamera = Camera();

  if (Start)
  {
    myGravityReferencePoint.SetCoord (X, Y, Z);
    myCamStartOpUp     = aCamera->Up();
    myCamStartOpEye    = aCamera->Eye();
    myCamStartOpCenter = aCamera->Center();
  }

  const Graphic3d_Vertex& aVref = myGravityReferencePoint;

  aCamera->SetUp     (myCamStartOpUp);
  aCamera->SetEye    (myCamStartOpEye);
  aCamera->SetCenter (myCamStartOpCenter);

  // rotate camera around 3 initial axes
  gp_Pnt aRCenter (aVref.X(), aVref.Y(), aVref.Z());

  gp_Dir aZAxis (aCamera->Direction().Reversed());
  gp_Dir aYAxis (aCamera->Up());
  gp_Dir aXAxis (aYAxis.Crossed (aZAxis)); 

  gp_Trsf aRot[3], aTrsf;
  aRot[0].SetRotation (gp_Ax1 (aRCenter, aYAxis), -Ax);
  aRot[1].SetRotation (gp_Ax1 (aRCenter, aXAxis), Ay);
  aRot[2].SetRotation (gp_Ax1 (aRCenter, aZAxis), Az);
  aTrsf.Multiply (aRot[0]);
  aTrsf.Multiply (aRot[1]);
  aTrsf.Multiply (aRot[2]);

  aCamera->Transform (aTrsf);

  AutoZFit();

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

  Handle(Graphic3d_Camera) aCamera = Camera();

  if (Start)
  {
    myGravityReferencePoint.SetCoord (X, Y, Z);
    myCamStartOpUp     = aCamera->Up();
    myCamStartOpEye    = aCamera->Eye();
    myCamStartOpCenter = aCamera->Center();

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

    myCamStartOpUp     = aCamera->Up();
    myCamStartOpEye    = aCamera->Eye();
    myCamStartOpCenter = aCamera->Center();
  }

  const Graphic3d_Vertex& aVref = myGravityReferencePoint;

  aCamera->SetUp     (myCamStartOpUp);
  aCamera->SetEye    (myCamStartOpEye);
  aCamera->SetCenter (myCamStartOpCenter);

  // rotate camera around passed axis
  gp_Trsf aRotation;
  gp_Pnt aRCenter (aVref.X(), aVref.Y(), aVref.Z());
  gp_Dir aRAxis ((Axe == V3d_X) ? 1.0 : 0.0,
                  (Axe == V3d_Y) ? 1.0 : 0.0,
                  (Axe == V3d_Z) ? 1.0 : 0.0);

  aRotation.SetRotation (gp_Ax1 (aRCenter, aRAxis), Angle);

  aCamera->Transform (aRotation);

  AutoZFit();

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

  Handle(Graphic3d_Camera) aCamera = Camera();

  if( Start ) {
    myCamStartOpUp     = aCamera->Up();
    myCamStartOpEye    = aCamera->Eye();
    myCamStartOpCenter = aCamera->Center();
  }

  const Graphic3d_Vertex& aPnt = MyDefaultViewPoint;
  const Graphic3d_Vector& anAxis = MyDefaultViewAxis;

  aCamera->SetUp     (myCamStartOpUp);
  aCamera->SetEye    (myCamStartOpEye);
  aCamera->SetCenter (myCamStartOpCenter);

  gp_Trsf aRotation;
  gp_Pnt aRCenter (aPnt.X(), aPnt.Y(), aPnt.Z());
  gp_Dir aRAxis (anAxis.X(), anAxis.Y(), anAxis.Z());
  aRotation.SetRotation (gp_Ax1 (aRCenter, aRAxis), Angle);

  aCamera->Transform (aRotation);

  AutoZFit();

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

  Handle(Graphic3d_Camera) aCamera = Camera();

  if( Start ) {
    myCamStartOpUp     = aCamera->Up();
    myCamStartOpEye    = aCamera->Eye();
    myCamStartOpCenter = aCamera->Center();
  }

  aCamera->SetUp     (myCamStartOpUp);
  aCamera->SetEye    (myCamStartOpEye);
  aCamera->SetCenter (myCamStartOpCenter);

  // rotate camera around 3 initial axes
  gp_Pnt aRCenter = aCamera->Eye();
  gp_Dir aZAxis (aCamera->Direction().Reversed());
  gp_Dir aYAxis (aCamera->Up());
  gp_Dir aXAxis (aYAxis.Crossed (aZAxis)); 

  gp_Trsf aRot[3], aTrsf;
  aRot[0].SetRotation (gp_Ax1 (aRCenter, aYAxis), -Ax);
  aRot[1].SetRotation (gp_Ax1 (aRCenter, aXAxis), Ay);
  aRot[2].SetRotation (gp_Ax1 (aRCenter, aZAxis), Az);
  aTrsf.Multiply (aRot[0]);
  aTrsf.Multiply (aRot[1]);
  aTrsf.Multiply (aRot[2]);

  aCamera->Transform (aTrsf);

  AutoZFit();

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

  Handle(Graphic3d_Camera) aCamera = Camera();

  if( Start ) {
    myCamStartOpUp     = aCamera->Up();
    myCamStartOpEye    = aCamera->Eye();
    myCamStartOpCenter = aCamera->Center();
  }

  aCamera->SetUp     (myCamStartOpUp);
  aCamera->SetEye    (myCamStartOpEye);
  aCamera->SetCenter (myCamStartOpCenter);

  const Graphic3d_Vector& anAxis = MyDefaultViewAxis;

  gp_Trsf aRotation;
  gp_Pnt aRCenter = aCamera->Eye();
  gp_Dir aRAxis (anAxis.X(), anAxis.Y(), anAxis.Z());
  aRotation.SetRotation (gp_Ax1 (aRCenter, aRAxis), Angle);

  aCamera->Transform (aRotation);

  AutoZFit();

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

  Handle(Graphic3d_Camera) aCamera = Camera();

  gp_Dir aReferencePlane (aCamera->Direction().Reversed());
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
  
  gp_Pnt aRCenter = aCamera->Center();
  gp_Dir aZAxis (aCamera->Direction().Reversed());

  gp_Trsf aTrsf;
  aTrsf.SetRotation (gp_Ax1 (aRCenter, aZAxis), Angle);

  Standard_Real myYscreenAxisX, myYscreenAxisY, myYscreenAxisZ;
  myYscreenAxis.Coord (myYscreenAxisX, myYscreenAxisY, myYscreenAxisZ);

  aCamera->SetUp (gp_Dir (myYscreenAxisX, myYscreenAxisY, myYscreenAxisZ));
  aCamera->Transform (aTrsf);

  AutoZFit();

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

  Handle(Graphic3d_Camera) aCamera = Camera();

  aCamera->SetEye (gp_Pnt (X, Y, Z));

  SetTwist (aTwistBefore);

  AutoZFit();

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

  Handle(Graphic3d_Camera) aCamera = Camera();

  if( Depth > 0. )
  {
    // Move eye using center (target) as anchor.
    aCamera->SetDistance (Depth);
  }
  else
  {
    // Move the view ref point instead of the eye.
    gp_Vec aDir (aCamera->Direction());
    gp_Pnt aCameraEye = aCamera->Eye();
    gp_Pnt aCameraCenter = aCameraEye.Translated (aDir.Multiplied (Abs (Depth)));

    aCamera->SetCenter (aCameraCenter);
  }

  AutoZFit();

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

  Camera()->SetDirection (gp_Dir (Vx, Vy, Vz).Reversed());

  SetTwist(aTwistBefore);

  AutoZFit();

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
  Handle(Graphic3d_Camera) aCamera = Camera();

  gp_Pnt anOriginVCS  = aCamera->ConvertWorld2View (gp::Origin());
  Standard_Real aPanX = anOriginVCS.X();
  Standard_Real aPanY = anOriginVCS.Y();

  aCamera->SetCenter (gp_Pnt (0, 0, 0));
  aCamera->SetDirection (gp_Dir (aBck.X(), aBck.Y(), aBck.Z()).Reversed());
  aCamera->SetUp (gp_Dir (Xpn, Ypn, Zpn));
  aCamera->OrthogonalizeUp();

  Panning (aPanX, aPanY);

  AutoZFit();

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

  Camera()->SetCenter (gp_Pnt (X, Y, Z));

  SetTwist (aTwistBefore);

  AutoZFit();

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

  Handle(Graphic3d_Camera) aCamera = Camera();

  gp_Dir aReferencePlane (aCamera->Direction().Reversed());
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

  aCamera->SetUp (gp_Dir (myYscreenAxisX, myYscreenAxisY, myYscreenAxisZ));

  AutoZFit();

  ImmediateUpdate();
}

//=============================================================================
//function : SetUp
//purpose  :
//=============================================================================
void V3d_View::SetUp( const V3d_TypeOfOrientation Orientation )
{
  Standard_Boolean TheStatus ;

  Handle(Graphic3d_Camera) aCamera = Camera();

  gp_Dir aReferencePlane (aCamera->Direction().Reversed());
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

  aCamera->SetUp (gp_Dir (myYscreenAxisX, myYscreenAxisY, myYscreenAxisZ));

  AutoZFit();

  ImmediateUpdate();
}

//=============================================================================
//function : SetViewOrientationDefault
//purpose  :
//=============================================================================
void V3d_View::SetViewOrientationDefault()
{
  myDefaultCamera->CopyOrientationData (Camera());
}

//=======================================================================
//function : SetViewMappingDefault
//purpose  :
//=======================================================================
void V3d_View::SetViewMappingDefault()
{
  myDefaultCamera->CopyMappingData (Camera());
}

//=============================================================================
//function : ResetViewOrientation
//purpose  :
//=============================================================================
void V3d_View::ResetViewOrientation()
{
  Camera()->CopyOrientationData (myDefaultCamera);

  AutoZFit();

  ImmediateUpdate();
}

//=======================================================================
//function : ResetViewMapping
//purpose  :
//=======================================================================
void V3d_View::ResetViewMapping()
{
  Camera()->CopyMappingData (myDefaultCamera);

  AutoZFit();

  ImmediateUpdate();
}

//=============================================================================
//function : Reset
//purpose  :
//=============================================================================
void V3d_View::Reset (const Standard_Boolean theToUpdate)
{
  Camera()->Copy (myDefaultCamera);

  AutoZFit();

  SwitchSetFront = Standard_False;

  if (myImmediateUpdate || theToUpdate)
  {
    Update();
  }
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
  Translate (Camera(), aXv, aYv);

  ImmediateUpdate();
}

//=============================================================================
//function : SetSize
//purpose  :
//=============================================================================
void V3d_View::SetSize (const Standard_Real theSize)
{
  V3d_BadValue_Raise_if (theSize <= 0.0, "V3d_View::SetSize, Window Size is NULL");

  Handle(Graphic3d_Camera) aCamera = Camera();

  aCamera->SetScale (aCamera->Aspect() >= 1.0 ? theSize / aCamera->Aspect() : theSize);

  AutoZFit();

  ImmediateUpdate();
}

//=============================================================================
//function : SetZSize
//purpose  :
//=============================================================================
void V3d_View::SetZSize (const Standard_Real theSize)
{
  Handle(Graphic3d_Camera) aCamera = Camera();

  Standard_Real Zmax = theSize / 2.;

  Standard_Real aDistance = aCamera->Distance();

  if (theSize <= 0.)
  {
    Zmax = aDistance;
  }

  // ShortReal precision factor used to add meaningful tolerance to
  // ZNear, ZFar values in order to avoid equality after type conversion
  // to ShortReal matrices type.
  const Standard_Real aPrecision = 1.0 / Pow (10.0, ShortRealDigits() - 1);

  Standard_Real aZFar  =  Zmax  + aDistance * 2.0;
  Standard_Real aZNear = -Zmax  + aDistance;
  aZNear              -= Abs (aZNear) * aPrecision;
  aZFar               += Abs (aZFar)  * aPrecision;

  if (!aCamera->IsOrthographic())
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

  aCamera->SetZRange (aZNear, aZFar);

  if (myImmediateUpdate)
  {
    Redraw();
  }
}

//=============================================================================
//function : SetZoom
//purpose  :
//=============================================================================
void V3d_View::SetZoom (const Standard_Real theCoef,const Standard_Boolean theToStart)
{
  V3d_BadValue_Raise_if (theCoef <= 0., "V3d_View::SetZoom, bad coefficient");

  Handle(Graphic3d_Camera) aCamera = Camera();

  if (theToStart)
  {
    myCamStartOpEye    = aCamera->Eye();
    myCamStartOpCenter = aCamera->Center();
  }

  Standard_Real aViewWidth  = aCamera->ViewDimensions().X();
  Standard_Real aViewHeight = aCamera->ViewDimensions().Y();

  // ensure that zoom will not be too small or too big
  Standard_Real aCoef = theCoef;
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

  aCamera->SetEye (myCamStartOpEye);
  aCamera->SetCenter (myCamStartOpCenter);
  aCamera->SetScale (aCamera->Scale() / aCoef);

  AutoZFit();

  ImmediateUpdate();
}

//=============================================================================
//function : SetScale
//purpose  :
//=============================================================================
void V3d_View::SetScale( const Standard_Real Coef )
{
  V3d_BadValue_Raise_if( Coef <= 0. ,"V3d_View::SetScale, bad coefficient");

  Handle(Graphic3d_Camera) aCamera = Camera();

  Standard_Real aDefaultScale = myDefaultCamera->Scale();
  aCamera->SetAspect (myDefaultCamera->Aspect());
  aCamera->SetScale (aDefaultScale / Coef);

  AutoZFit();

  ImmediateUpdate();
}

//=============================================================================
//function : SetAxialScale
//purpose  :
//=============================================================================
void V3d_View::SetAxialScale( const Standard_Real Sx, const Standard_Real Sy, const Standard_Real Sz )
{
  V3d_BadValue_Raise_if( Sx <= 0. || Sy <= 0. || Sz <= 0.,"V3d_View::SetAxialScale, bad coefficient");

  Camera()->SetAxialScale (gp_XYZ (Sx, Sy, Sz));

  AutoZFit();
}

//=============================================================================
//function : SetRatio
//purpose  :
//=============================================================================
void V3d_View::SetRatio()
{
  if (MyWindow.IsNull())
  {
    return;
  }

  Standard_Integer aWidth  = 0;
  Standard_Integer aHeight = 0;
  MyWindow->Size (aWidth, aHeight);
  if (aWidth > 0 && aHeight > 0)
  {
    Standard_Real aRatio = static_cast<Standard_Real> (aWidth) /
                           static_cast<Standard_Real> (aHeight);

    Camera()       ->SetAspect (aRatio);
    myDefaultCamera->SetAspect (aRatio);
  }
}

//=============================================================================
//function : FitAll
//purpose  :
//=============================================================================
void V3d_View::FitAll (const Standard_Real theMargin, const Standard_Boolean theToUpdate)
{
  FitAll (myView->MinMaxValues(), theMargin, theToUpdate);
}

//=============================================================================
//function : FitAll
//purpose  :
//=============================================================================
void V3d_View::FitAll (const Bnd_Box& theBox, const Standard_Real theMargin, const Standard_Boolean theToUpdate)
{
  Standard_ASSERT_RAISE(theMargin >= 0.0 && theMargin < 1.0, "Invalid margin coefficient");

  if (myView->NumberOfDisplayedStructures() == 0)
  {
    return;
  }

  if (!FitMinMax (Camera(), theBox, theMargin, 10.0 * Precision::Confusion()))
  {
    return;
  }

  AutoZFit();

  if (myImmediateUpdate || theToUpdate)
  {
    Update();
  }
}

//=============================================================================
//function : DepthFitAll
//purpose  :
//=============================================================================
void V3d_View::DepthFitAll(const Standard_Real Aspect,
                           const Standard_Real Margin)
{
  Standard_Real Xmin,Ymin,Zmin,Xmax,Ymax,Zmax,U,V,W,U1,V1,W1 ;
  Standard_Real Umin,Vmin,Wmin,Umax,Vmax,Wmax ;
  Standard_Real Dx,Dy,Dz,Size;

  Standard_Integer Nstruct = myView->NumberOfDisplayedStructures() ;

  if((Nstruct <= 0) || (Aspect < 0.) || (Margin < 0.) || (Margin > 1.)) {
    ImmediateUpdate();
    return ;
  }

  Bnd_Box aBox = myView->MinMaxValues();
  if (aBox.IsVoid())
  {
    ImmediateUpdate();
    return ;
  }
    aBox.Get (Xmin,Ymin,Zmin,Xmax,Ymax,Zmax);
    Project (Xmin,Ymin,Zmin,U,V,W) ;
    Project (Xmax,Ymax,Zmax,U1,V1,W1) ;
    Umin = Min(U,U1) ; Umax = Max(U,U1) ;
    Vmin = Min(V,V1) ; Vmax = Max(V,V1) ;
    Wmin = Min(W,W1) ; Wmax = Max(W,W1) ;
    Project (Xmin,Ymin,Zmax,U,V,W) ;
    Umin = Min(U,Umin) ; Umax = Max(U,Umax) ;
    Vmin = Min(V,Vmin) ; Vmax = Max(V,Vmax) ;
    Wmin = Min(W,Wmin) ; Wmax = Max(W,Wmax) ;
    Project (Xmax,Ymin,Zmax,U,V,W) ;
    Umin = Min(U,Umin) ; Umax = Max(U,Umax) ;
    Vmin = Min(V,Vmin) ; Vmax = Max(V,Vmax) ;
    Wmin = Min(W,Wmin) ; Wmax = Max(W,Wmax) ;
    Project (Xmax,Ymin,Zmin,U,V,W) ;
    Umin = Min(U,Umin) ; Umax = Max(U,Umax) ;
    Vmin = Min(V,Vmin) ; Vmax = Max(V,Vmax) ;
    Wmin = Min(W,Wmin) ; Wmax = Max(W,Wmax) ;
    Project (Xmax,Ymax,Zmin,U,V,W) ;
    Umin = Min(U,Umin) ; Umax = Max(U,Umax) ;
    Vmin = Min(V,Vmin) ; Vmax = Max(V,Vmax) ;
    Wmin = Min(W,Wmin) ; Wmax = Max(W,Wmax) ;
    Project (Xmin,Ymax,Zmax,U,V,W) ;
    Umin = Min(U,Umin) ; Umax = Max(U,Umax) ;
    Vmin = Min(V,Vmin) ; Vmax = Max(V,Vmax) ;
    Wmin = Min(W,Wmin) ; Wmax = Max(W,Wmax) ;
    Project (Xmin,Ymax,Zmin,U,V,W) ;
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

  Handle(Graphic3d_Camera) aCamera = Camera();

  if (!aCamera->IsOrthographic())
  {
    // normalize view coordinates
    Standard_Integer aWinWidth, aWinHeight;
    MyWindow->Size (aWinWidth, aWinHeight);

    // z coordinate of camera center
    Standard_Real aDepth = aCamera->Project (aCamera->Center()).Z();

    // camera projection coordinate are in NDC which are normalized [-1, 1]
    Standard_Real aUMin = (2.0 / aWinWidth) * theMinXp  - 1.0;
    Standard_Real aUMax = (2.0 / aWinWidth) * theMaxXp  - 1.0;
    Standard_Real aVMin = (2.0 / aWinHeight) * theMinYp - 1.0;
    Standard_Real aVMax = (2.0 / aWinHeight) * theMaxYp - 1.0;

    // compute camera panning
    gp_Pnt aScreenCenter (0.0, 0.0, aDepth);
    gp_Pnt aFitCenter ((aUMin + aUMax) * 0.5, (aVMin + aVMax) * 0.5, aDepth);
    gp_Pnt aPanTo = aCamera->ConvertProj2View (aFitCenter);
    gp_Pnt aPanFrom = aCamera->ConvertProj2View (aScreenCenter);
    gp_Vec aPanVec (aPanFrom, aPanTo);

    // compute section size
    gp_Pnt aFitTopRight (aUMax, aVMax, aDepth);
    gp_Pnt aFitBotLeft (aUMin, aVMin, aDepth);
    gp_Pnt aViewBotLeft = aCamera->ConvertProj2View (aFitBotLeft);
    gp_Pnt aViewTopRight = aCamera->ConvertProj2View (aFitTopRight);

    Standard_Real aUSize = aViewTopRight.X() - aViewBotLeft.X();
    Standard_Real aVSize = aViewTopRight.Y() - aViewBotLeft.Y();

    Translate (aCamera, aPanVec.X(), -aPanVec.Y());
    Scale (aCamera, aUSize, aVSize);
    AutoZFit();
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

  V3d_UnMapped_Raise_if (!myView->IsDefined(), "view has no window");

  MyWindow->Size (aDxw, aDyw);
  Standard_Real aValue;

  gp_Pnt aViewDims = Camera()->ViewDimensions();
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

  V3d_UnMapped_Raise_if (!myView->IsDefined(), "view has no window");

  MyWindow->Size (aDxw, aDyw);

  gp_Pnt aPoint (Xp * 2.0 / aDxw - 1.0, (aDyw - Yp) * 2.0 / aDyw - 1.0, 0.0);
  aPoint = Camera()->ConvertProj2View (aPoint);

  Xv = aPoint.X();
  Yv = aPoint.Y();
}

//=======================================================================
//function : Convert
//purpose  :
//=======================================================================
Standard_Integer V3d_View::Convert(const Standard_Real Vv) const
{
  V3d_UnMapped_Raise_if (!myView->IsDefined(), "view has no window");

  Standard_Integer aDxw, aDyw;
  MyWindow->Size (aDxw, aDyw);

  gp_Pnt aViewDims = Camera()->ViewDimensions();
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
  V3d_UnMapped_Raise_if (!myView->IsDefined(), "view has no window");

  Standard_Integer aDxw, aDyw;
  MyWindow->Size (aDxw, aDyw);

  gp_Pnt aPoint (Xv, Yv, 0.0);
  aPoint = Camera()->ConvertView2Proj (aPoint);
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
  V3d_UnMapped_Raise_if (!myView->IsDefined(), "view has no window");
  Standard_Integer aHeight, aWidth;
  MyWindow->Size (aWidth, aHeight);

  Standard_Real anX = 2.0 * Xp / aWidth - 1.0;
  Standard_Real anY = 2.0 * (aHeight - 1 - Yp) / aHeight - 1.0;
  Standard_Real  aZ = 2.0 * 0.0 - 1.0;

  gp_Pnt aResult = Camera()->UnProject (gp_Pnt (anX, anY, aZ));

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
  V3d_UnMapped_Raise_if (!myView->IsDefined(), "view has no window");
  Standard_Integer aHeight, aWidth;
  MyWindow->Size (aWidth, aHeight);

  Standard_Real anX = 2.0 * Xp / aWidth - 1.0;
  Standard_Real anY = 2.0 * (aHeight - 1 - Yp) / aHeight - 1.0;
  Standard_Real  aZ = 2.0 * 0.0 - 1.0;

  Handle(Graphic3d_Camera) aCamera = Camera();

  gp_Pnt aResult = aCamera->UnProject (gp_Pnt (anX, anY, aZ));

  X = aResult.X();
  Y = aResult.Y();
  Z = aResult.Z();

  Graphic3d_Vertex aVrp;
  aVrp.SetCoord (X, Y, Z);

  aResult = aCamera->UnProject (gp_Pnt (anX, anY, aZ - 10.0));

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
  V3d_UnMapped_Raise_if (!myView->IsDefined(), "view has no window");
  Standard_Integer aHeight, aWidth;
  MyWindow->Size (aWidth, aHeight);

  gp_Pnt aPoint = Camera()->Project (gp_Pnt (X, Y, Z));

  Xp = RealToInt ((aPoint.X() + 1) * 0.5 * aWidth);
  Yp = RealToInt (aHeight - 1 - (aPoint.Y() + 1) * 0.5 * aHeight);
}

//=======================================================================
//function : Project
//purpose  :
//=======================================================================
void V3d_View::Project (const Standard_Real theX,
                        const Standard_Real theY,
                        const Standard_Real theZ,
                        Standard_Real& theXp,
                        Standard_Real& theYp) const
{
  Standard_Real aZp;
  Project (theX, theY, theZ, theXp, theYp, aZp);
}

//=======================================================================
//function : Project
//purpose  :
//=======================================================================
void V3d_View::Project (const Standard_Real theX,
                        const Standard_Real theY,
                        const Standard_Real theZ,
                        Standard_Real& theXp,
                        Standard_Real& theYp,
                        Standard_Real& theZp) const
{
  Handle(Graphic3d_Camera) aCamera = Camera();

  gp_XYZ aViewSpaceDimensions = aCamera->ViewDimensions();
  Standard_Real aXSize = aViewSpaceDimensions.X();
  Standard_Real aYSize = aViewSpaceDimensions.Y();
  Standard_Real aZSize = aViewSpaceDimensions.Z();

  gp_Pnt aPoint = aCamera->Project (gp_Pnt (theX, theY, theZ));

  // NDC [-1, 1] --> PROJ [ -size / 2, +size / 2 ]
  theXp = aPoint.X() * aXSize * 0.5;
  theYp = aPoint.Y() * aYSize * 0.5;
  theZp = aPoint.Z() * aZSize * 0.5;
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
  return myView->Background().Color() ;
}

//=======================================================================
//function : GradientBackgroundColors
//purpose  :
//=======================================================================
void V3d_View::GradientBackgroundColors (Quantity_Color& theColor1, Quantity_Color& theColor2) const
{
  myView->GradientBackground().Colors (theColor1, theColor2);
}

//=======================================================================
//function : GradientBackground
//purpose  :
//=======================================================================
Aspect_GradientBackground V3d_View::GradientBackground() const
{
  return myView->GradientBackground();
}

//=======================================================================
//function : Scale
//purpose  :
//=======================================================================
Standard_Real V3d_View::Scale() const
{
  return myDefaultCamera->Scale() / Camera()->Scale();
}

//=======================================================================
//function : AxialScale
//purpose  :
//=======================================================================
void V3d_View::AxialScale(Standard_Real& Sx, Standard_Real& Sy, Standard_Real& Sz) const
{
  gp_Pnt anAxialScale = Camera()->AxialScale();
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
  gp_Pnt aViewDims = Camera()->ViewDimensions();

  Width = aViewDims.X();
  Height = aViewDims.Y();
}

//=======================================================================
//function : ZSize
//purpose  :
//=======================================================================
Standard_Real V3d_View::ZSize() const
{
  gp_Pnt aViewDims = Camera()->ViewDimensions();

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
  Standard_Integer Nstruct = myView->NumberOfDisplayedStructures() ;

  if( Nstruct ) {
    Bnd_Box aBox = myView->MinMaxValues();
    aBox.Get (Xmin,Ymin,Zmin,Xmax,Ymax,Zmax);
    Project (Xmin,Ymin,Zmin,Umin,Vmin,Wmin) ;
    Project (Xmax,Ymax,Zmax,Umax,Vmax,Wmax) ;
    Project (Xmin,Ymin,Zmax,U,V,W) ;
    Umin = Min(U,Umin) ; Umax = Max(U,Umax) ;
    Vmin = Min(V,Vmin) ; Vmax = Max(V,Vmax) ;
    Wmin = Min(W,Wmin) ; Wmax = Max(W,Wmax) ;
    Project (Xmax,Ymin,Zmax,U,V,W) ;
    Umin = Min(U,Umin) ; Umax = Max(U,Umax) ;
    Vmin = Min(V,Vmin) ; Vmax = Max(V,Vmax) ;
    Wmin = Min(W,Wmin) ; Wmax = Max(W,Wmax) ;
    Project (Xmax,Ymin,Zmin,U,V,W) ;
    Umin = Min(U,Umin) ; Umax = Max(U,Umax) ;
    Vmin = Min(V,Vmin) ; Vmax = Max(V,Vmax) ;
    Wmin = Min(W,Wmin) ; Wmax = Max(W,Wmax) ;
    Project (Xmax,Ymax,Zmin,U,V,W) ;
    Umin = Min(U,Umin) ; Umax = Max(U,Umax) ;
    Vmin = Min(V,Vmin) ; Vmax = Max(V,Vmax) ;
    Wmin = Min(W,Wmin) ; Wmax = Max(W,Wmax) ;
    Project (Xmin,Ymax,Zmax,U,V,W) ;
    Umin = Min(U,Umin) ; Umax = Max(U,Umax) ;
    Vmin = Min(V,Vmin) ; Vmax = Max(V,Vmax) ;
    Wmin = Min(W,Wmin) ; Wmax = Max(W,Wmax) ;
    Project (Xmin,Ymax,Zmin,U,V,W) ;
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
  Standard_Integer Nstruct = myView->NumberOfDisplayedStructures() ;

  if( Nstruct ) {
    Bnd_Box aBox = myView->MinMaxValues();
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
  myView->DisplayedStructures (aSetOfStructures);

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
      || aStruct->IsInfinite()
      || (hasSelection && !aStruct->IsHighlighted()))
    {
      continue;
    }

    const Graphic3d_BndBox3d& aBox = aStruct->CStructure()->BoundingBox();
    if (!aBox.IsValid())
    {
      continue;
    }

    // skip transformation-persistent objects
    if (!aStruct->TransformPersistence().IsNull())
    {
      continue;
    }

    // use camera projection to find gravity point
    Xmin = aBox.CornerMin().x();
    Ymin = aBox.CornerMin().y();
    Zmin = aBox.CornerMin().z();
    Xmax = aBox.CornerMax().x();
    Ymax = aBox.CornerMax().y();
    Zmax = aBox.CornerMax().z();
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
      const gp_Pnt  aProjected = Camera()->Project (aBndPnt);
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
    // fallback - just use bounding box of entire scene
    Bnd_Box aBox = myView->MinMaxValues();
    if (!aBox.IsVoid())
    {
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
  gp_Pnt aCameraEye = Camera()->Eye();
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
  return Camera()->Distance();
}

//=============================================================================
//function : Proj
//purpose  :
//=============================================================================
void V3d_View::Proj(Standard_Real& Dx, Standard_Real& Dy, Standard_Real& Dz) const
{
  gp_Dir aCameraDir = Camera()->Direction().Reversed();
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
  gp_Pnt aCameraCenter = Camera()->Center();
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
  gp_Dir aCameraUp = Camera()->Up();
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

  gp_Dir aReferencePlane (Camera()->Direction().Reversed());
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
  return static_cast<V3d_TypeOfShadingModel> (myView->ShadingModel());
}

//=============================================================================
//function : TextureEnv
//purpose  :
//=============================================================================
Handle(Graphic3d_TextureEnv) V3d_View::TextureEnv() const
{
  return myView->TextureEnv();
}

//=============================================================================
//function : Visualization
//purpose  :
//=============================================================================
V3d_TypeOfVisualization V3d_View::Visualization() const
{
  return static_cast<V3d_TypeOfVisualization> (myView->VisualizationType());
}

//=============================================================================
//function : Viewer
//purpose  :
//=============================================================================
Handle(V3d_Viewer) V3d_View::Viewer() const
{
  return MyViewer;
}

//=============================================================================
//function : IfWindow
//purpose  :
//=============================================================================
Standard_Boolean V3d_View::IfWindow() const
{
  return myView->IsDefined();
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
  return Camera()->IsOrthographic() ? V3d_ORTHOGRAPHIC : V3d_PERSPECTIVE;
}

//=============================================================================
//function : SetFocale
//purpose  :
//=============================================================================
void V3d_View::SetFocale( const Standard_Real focale )
{
  Handle(Graphic3d_Camera) aCamera = Camera();

  if (aCamera->IsOrthographic())
  {
    return;
  }

  Standard_Real aFOVyRad = ATan (focale / (aCamera->Distance() * 2.0));

  aCamera->SetFOVy (aFOVyRad * (360 / M_PI));

  ImmediateUpdate();
}

//=============================================================================
//function : Focale
//purpose  :
//=============================================================================
Standard_Real V3d_View::Focale() const
{
  Handle(Graphic3d_Camera) aCamera = Camera();

  if (aCamera->IsOrthographic())
  {
    return 0.0;
  }

  return aCamera->Distance() * 2.0 * Tan (aCamera->FOVy() * M_PI / 360.0);
}

//=============================================================================
//function : View
//purpose  :
//=============================================================================
Handle(Graphic3d_CView) V3d_View::View() const
{
  return myView;
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
gp_XYZ V3d_View::TrsPoint (const Graphic3d_Vertex& thePnt, const TColStd_Array2OfReal& theMat)
{
  // CAL. S3892
  const Standard_Integer lr = theMat.LowerRow();
  const Standard_Integer ur = theMat.UpperRow();
  const Standard_Integer lc = theMat.LowerCol();
  const Standard_Integer uc = theMat.UpperCol();
  if ((ur - lr + 1 != 4) || (uc - lc + 1 != 4))
  {
    return gp_XYZ (thePnt.X(), thePnt.Y(), thePnt.Z());
  }

  Standard_Real X, Y, Z;
  thePnt.Coord (X,Y,Z);
  const Standard_Real XX = (theMat(lr,lc+3)   + X*theMat(lr,lc)   + Y*theMat(lr,lc+1)   + Z*theMat(lr,lc+2)) / theMat(lr+3,lc+3);
  const Standard_Real YY = (theMat(lr+1,lc+3) + X*theMat(lr+1,lc) + Y*theMat(lr+1,lc+1) + Z*theMat(lr+1,lc+2))/theMat(lr+3,lc+3);
  const Standard_Real ZZ = (theMat(lr+2,lc+3) + X*theMat(lr+2,lc) + Y*theMat(lr+2,lc+1) + Z*theMat(lr+2,lc+2))/theMat(lr+3,lc+3);
  return gp_XYZ (XX, YY, ZZ);
}

//=======================================================================
//function : Pan
//purpose  :
//=======================================================================
void V3d_View::Pan (const Standard_Integer theDXp,
                    const Standard_Integer theDYp,
                    const Standard_Real    theZoomFactor,
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
                        const Standard_Real theZoomFactor,
                        const Standard_Boolean theToStart)
{
  Standard_ASSERT_RAISE (theZoomFactor > 0.0, "Bad zoom factor");

  Handle(Graphic3d_Camera) aCamera = Camera();

  if (theToStart)
  {
    myCamStartOpEye    = aCamera->Eye();
    myCamStartOpCenter = aCamera->Center();
  }

  Standard_Boolean wasUpdateEnabled = SetImmediateUpdate (Standard_False);

  gp_Pnt aViewDims = aCamera->ViewDimensions();

  aCamera->SetEye (myCamStartOpEye);
  aCamera->SetCenter (myCamStartOpCenter);
  Translate (aCamera, -theDXv, -theDYv);
  Scale (aCamera, aViewDims.X() / theZoomFactor, aViewDims.Y() / theZoomFactor);

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

  Handle(Graphic3d_Camera) aCamera = Camera();

  Standard_Real aViewWidth  = aCamera->ViewDimensions().X();
  Standard_Real aViewHeight = aCamera->ViewDimensions().Y();

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

  Standard_Real aDxv = aZoomAtPointXv / aCoef;
  Standard_Real aDyv = aZoomAtPointYv / aCoef;

  aCamera->SetScale (aCamera->Scale() / aCoef);
  Translate (aCamera, aZoomAtPointXv - aDxv, aZoomAtPointYv - aDyv);

  AutoZFit();

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
void V3d_View::FitAll(const Standard_Real theXmin,
                      const Standard_Real theYmin,
                      const Standard_Real theXmax,
                      const Standard_Real theYmax)
{
  Handle(Graphic3d_Camera) aCamera = Camera();
  Standard_Real anAspect = aCamera->Aspect();

  Standard_Real aFitSizeU  = Abs (theXmax - theXmin);
  Standard_Real aFitSizeV  = Abs (theYmax - theYmin);
  Standard_Real aFitAspect = aFitSizeU / aFitSizeV;
  if (aFitAspect >= anAspect)
  {
    aFitSizeV = aFitSizeU / anAspect;
  }
  else
  {
    aFitSizeU = aFitSizeV * anAspect;
  }

  Translate (aCamera, (theXmin + theXmax) * 0.5, (theYmin + theYmax) * 0.5);
  Scale (aCamera, aFitSizeU, aFitSizeV);

  AutoZFit();

  ImmediateUpdate();
}

//=============================================================================
//function : StartRotation
//purpose  :
//=============================================================================
void V3d_View::StartRotation(const Standard_Integer X,
                             const Standard_Integer Y,
                             const Standard_Real zRotationThreshold)
{
  sx = X; sy = Y;
  Standard_Real x,y;
  Size(x,y);
  rx = Standard_Real(Convert(x));
  ry = Standard_Real(Convert(y));
  Gravity(gx,gy,gz);
  Rotate(0.,0.,0.,gx,gy,gz,Standard_True);
  myZRotation = Standard_False;
  if( zRotationThreshold > 0. ) {
    Standard_Real dx = Abs(sx - rx/2.);
    Standard_Real dy = Abs(sy - ry/2.);
    //  if( dx > rx/3. || dy > ry/3. ) myZRotation = Standard_True;
    Standard_Real dd = zRotationThreshold * (rx + ry)/2.;
    if( dx > dd || dy > dd ) myZRotation = Standard_True;
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
  if( myZRotation ) {
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
void V3d_View::SetComputedMode (const Standard_Boolean theMode)
{
  if (theMode)
  {
    if (myComputedMode)
    {
      myView->SetComputedMode (Standard_True);
    }
  }
  else
  {
    myView->SetComputedMode (Standard_False);
  }
}

//=============================================================================
//function : ComputedMode
//purpose  :
//=============================================================================
Standard_Boolean V3d_View::ComputedMode() const
{
  return myView->ComputedMode();
}

//=============================================================================
//function : SetBackFacingModel
//purpose  :
//=============================================================================
void V3d_View::SetBackFacingModel (const V3d_TypeOfBackfacingModel theModel)
{
  myView->SetBackfacingModel (static_cast<Graphic3d_TypeOfBackfacingModel> (theModel));
  Redraw();
}

//=============================================================================
//function : BackFacingModel
//purpose  :
//=============================================================================
V3d_TypeOfBackfacingModel V3d_View::BackFacingModel() const
{
  return static_cast<V3d_TypeOfBackfacingModel> (myView->BackfacingModel());
}

//=============================================================================
//function : Init
//purpose  :
//=============================================================================
void V3d_View::Init()
{
  myComputedMode = MyViewer->ComputedMode();
  if (!myComputedMode || !MyViewer->DefaultComputedMode())
  {
    SetComputedMode (Standard_False);
  }
}

//=============================================================================
//function : Export
//purpose  :
//=============================================================================
Standard_Boolean V3d_View::Export (const Standard_CString theFileName,
                                   const Graphic3d_ExportFormat theFormat,
                                   const Graphic3d_SortType theSortType)
{
  return myView->Export (theFileName, theFormat, theSortType);
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
                                     const V3d_ImageDumpOptions& theParams)
{
  Graphic3d_Vec2i aTargetSize (theParams.Width, theParams.Height);
  if (aTargetSize.x() != 0
   && aTargetSize.y() != 0)
  {
    // allocate image buffer for dumping
    if (theImage.IsEmpty()
     || theImage.SizeX() != Standard_Size(aTargetSize.x())
     || theImage.SizeY() != Standard_Size(aTargetSize.y()))
    {
      Image_Format aFormat = Image_Format_UNKNOWN;
      switch (theParams.BufferType)
      {
        case Graphic3d_BT_RGB:                 aFormat = Image_Format_RGB;   break;
        case Graphic3d_BT_RGBA:                aFormat = Image_Format_RGBA;  break;
        case Graphic3d_BT_Depth:               aFormat = Image_Format_GrayF; break;
        case Graphic3d_BT_RGB_RayTraceHdrLeft: aFormat = Image_Format_RGBF;  break;
      }

      if (!theImage.InitZero (aFormat, Standard_Size(aTargetSize.x()), Standard_Size(aTargetSize.y())))
      {
        Message::DefaultMessenger()->Send (TCollection_AsciiString ("Fail to allocate an image ") + aTargetSize.x() + "x" + aTargetSize.y()
                                                                 + " for view dump", Message_Fail);
        return Standard_False;
      }
    }
  }
  if (theImage.IsEmpty())
  {
    Message::DefaultMessenger()->Send (TCollection_AsciiString ("V3d_View::ToPixMap() has been called without image dimensions"), Message_Fail);
    return Standard_False;
  }
  aTargetSize.x() = (Standard_Integer )theImage.SizeX();
  aTargetSize.y() = (Standard_Integer )theImage.SizeY();

  Handle(Standard_Transient) aFBOPtr;
  Handle(Standard_Transient) aPrevFBOPtr = myView->FBO();
  Graphic3d_Vec2i aFBOVPSize = aTargetSize;

  bool isTiling = false;
  if (theParams.TileSize > 0)
  {
    if (aFBOVPSize.x() > theParams.TileSize
     || aFBOVPSize.y() > theParams.TileSize)
    {
      aFBOVPSize.x() = Min (aFBOVPSize.x(), theParams.TileSize);
      aFBOVPSize.y() = Min (aFBOVPSize.y(), theParams.TileSize);
      isTiling = true;
    }
  }

  Graphic3d_Vec2i aPrevFBOVPSize;
  if (!aPrevFBOPtr.IsNull())
  {
    Graphic3d_Vec2i aPrevFBOSizeMax;
    myView->FBOGetDimensions (aPrevFBOPtr,
                              aPrevFBOVPSize.x(),  aPrevFBOVPSize.y(),
                              aPrevFBOSizeMax.x(), aPrevFBOSizeMax.y());
    if (aFBOVPSize.x() <= aPrevFBOSizeMax.x()
     && aFBOVPSize.y() <= aPrevFBOSizeMax.y())
    {
      aFBOPtr = aPrevFBOPtr;
    }
  }

  if (aFBOPtr.IsNull())
  {
    Standard_Integer aMaxTexSize = MyViewer->Driver()->InquireLimit (Graphic3d_TypeOfLimit_MaxTextureSize);
    if (theParams.TileSize > aMaxTexSize)
    {
      Message::DefaultMessenger()->Send (TCollection_AsciiString ("Image dump can not be performed - specified tile size (")
                                                                 + theParams.TileSize + ") exceeds hardware limits (" + aMaxTexSize + ")", Message_Fail);
      return Standard_False;
    }

    if (aFBOVPSize.x() > aMaxTexSize
     || aFBOVPSize.y() > aMaxTexSize)
    {
      aFBOVPSize.x() = Min (aFBOVPSize.x(), aMaxTexSize);
      aFBOVPSize.y() = Min (aFBOVPSize.y(), aMaxTexSize);
      isTiling = true;
    }

    // Try to create hardware accelerated buffer
    aFBOPtr = myView->FBOCreate (aFBOVPSize.x(), aFBOVPSize.y());
  }
  myView->SetFBO (aFBOPtr);

  if (aFBOPtr.IsNull())
  {
    // try to use on-screen buffer
    Graphic3d_Vec2i aWinSize;
    MyWindow->Size (aWinSize.x(), aWinSize.y());
    if (aFBOVPSize.x() != aWinSize.x()
     || aFBOVPSize.y() != aWinSize.y())
    {
      isTiling = true;
    }
    aFBOVPSize = aWinSize;

    Message::DefaultMessenger()->Send (TCollection_AsciiString ("Warning, on screen buffer is used for image dump - content might be invalid"), Message_Warning);
  }

  // backup camera parameters
  Handle(Graphic3d_Camera) aStoreMapping = new Graphic3d_Camera();
  Handle(Graphic3d_Camera) aCamera = Camera();
  aStoreMapping->Copy (aCamera);
  if (aCamera->IsStereo())
  {
    switch (theParams.StereoOptions)
    {
      case V3d_SDO_MONO:
      {
        aCamera->SetProjectionType (Graphic3d_Camera::Projection_Perspective);
        break;
      }
      case V3d_SDO_LEFT_EYE:
      {
        aCamera->SetProjectionType (Graphic3d_Camera::Projection_MonoLeftEye);
        break;
      }
      case V3d_SDO_RIGHT_EYE:
      {
        aCamera->SetProjectionType (Graphic3d_Camera::Projection_MonoRightEye);
        break;
      }
      case V3d_SDO_BLENDED:
      {
        break; // dump as is
      }
    }
  }
  if (theParams.ToAdjustAspect)
  {
    aCamera->SetAspect (Standard_Real(aTargetSize.x()) / Standard_Real(aTargetSize.y()));
  }
  AutoZFit();

  // render immediate structures into back buffer rather than front
  const Standard_Boolean aPrevImmediateMode = myView->SetImmediateModeDrawToFront (Standard_False);

  Standard_Boolean isSuccess = Standard_True;
  if (!isTiling)
  {
    if (!aFBOPtr.IsNull())
    {
      myView->FBOChangeViewport (aFBOPtr, aTargetSize.x(), aTargetSize.y());
    }
    Redraw();
    isSuccess = isSuccess && myView->BufferDump (theImage, theParams.BufferType);
  }
  else
  {
    Image_PixMap aTilePixMap;
    aTilePixMap.SetTopDown (theImage.IsTopDown());

    Graphic3d_Vec2i anOffset (0, 0);
    for (; anOffset.y() < aTargetSize.y(); anOffset.y() += aFBOVPSize.y())
    {
      anOffset.x() = 0;
      for (; anOffset.x() < aTargetSize.x(); anOffset.x() += aFBOVPSize.x())
      {
        Graphic3d_CameraTile aTile;
        aTile.Offset    = anOffset;
        aTile.TotalSize = aTargetSize;
        aTile.TileSize  = aFBOVPSize;
        if (!aFBOPtr.IsNull())
        {
          // crop corners in case of FBO
          // (no API to resize viewport of on-screen buffer - keep uncropped in this case)
          aTile = aTile.Cropped();
        }
        if (aTile.TileSize.x() < 1
         || aTile.TileSize.y() < 1)
        {
          continue;
        }

        const Standard_Integer aLeft   = aTile.Offset.x();
        Standard_Integer       aBottom = aTile.Offset.y();
        if (theImage.IsTopDown())
        {
          const Standard_Integer aTop = aTile.Offset.y() + aTile.TileSize.y();
          aBottom = aTargetSize.y() - aTop;
        }
        aTilePixMap.InitWrapper (theImage.Format(), theImage.ChangeData()
                               + theImage.SizeRowBytes() * aBottom + theImage.SizePixelBytes() * aLeft,
                                 aTile.TileSize.x(), aTile.TileSize.y(),
                                 theImage.SizeRowBytes());

        aCamera->SetTile (aTile);
        if (!aFBOPtr.IsNull())
        {
          myView->FBOChangeViewport (aFBOPtr, aTile.TileSize.x(), aTile.TileSize.y());
        }
        Redraw();
        isSuccess = isSuccess && myView->BufferDump (aTilePixMap, theParams.BufferType);
        if (!isSuccess)
        {
          break;
        }
      }
      if (!isSuccess)
      {
        break;
      }
    }
  }

  // restore state
  myView->SetImmediateModeDrawToFront (aPrevImmediateMode);
  aCamera->Copy (aStoreMapping);
  if (aFBOPtr != aPrevFBOPtr)
  {
    myView->FBORelease (aFBOPtr);
  }
  else if (!aPrevFBOPtr.IsNull())
  {
    myView->FBOChangeViewport (aPrevFBOPtr, aPrevFBOVPSize.x(), aPrevFBOVPSize.y());
  }
  myView->SetFBO (aPrevFBOPtr);
  return isSuccess;
}

//=============================================================================
//function : ImmediateUpdate
//purpose  :
//=============================================================================
void V3d_View::ImmediateUpdate() const
{
  if (myImmediateUpdate)
  {
    Update();
  }
}

//=============================================================================
//function : SetImmediateUpdate
//purpose  :
//=============================================================================
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
  myView->SetCamera (theCamera);

  ImmediateUpdate();
}

// =======================================================================
// function : GetCamera
// purpose  :
// =======================================================================
const Handle(Graphic3d_Camera)& V3d_View::Camera() const
{
  return myView->Camera();
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
  Standard_Real anAssymX = Tan (( aCamSide).Angle (aFrustumPlane (1).Axis().Direction()))
                         - Tan ((-aCamSide).Angle (aFrustumPlane (2).Axis().Direction()));
  Standard_Real anAssymY = Tan (( aCamUp)  .Angle (aFrustumPlane (3).Axis().Direction()))
                         - Tan ((-aCamUp)  .Angle (aFrustumPlane (4).Axis().Direction()));

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
  aFitDistance.ChangeValue (1) *= Sqrt(1 + Pow (Tan (  aCamSide .Angle (aFrustumPlane (1).Axis().Direction())), 2.0));
  aFitDistance.ChangeValue (2) *= Sqrt(1 + Pow (Tan ((-aCamSide).Angle (aFrustumPlane (2).Axis().Direction())), 2.0));
  aFitDistance.ChangeValue (3) *= Sqrt(1 + Pow (Tan (  aCamUp   .Angle (aFrustumPlane (3).Axis().Direction())), 2.0));
  aFitDistance.ChangeValue (4) *= Sqrt(1 + Pow (Tan ((-aCamUp)  .Angle (aFrustumPlane (4).Axis().Direction())), 2.0));
  aFitDistance.ChangeValue (5) *= Sqrt(1 + Pow (Tan (  aCamDir  .Angle (aFrustumPlane (5).Axis().Direction())), 2.0));
  aFitDistance.ChangeValue (6) *= Sqrt(1 + Pow (Tan ((-aCamDir) .Angle (aFrustumPlane (6).Axis().Direction())), 2.0));

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

  Scale (theCamera, aViewSizeXv, aViewSizeYv);

  const Standard_Real aZoomCoef = myView->ConsiderZoomPersistenceObjects();

  Scale (theCamera, theCamera->ViewDimensions().X() * (aZoomCoef + theMargin), theCamera->ViewDimensions().Y() * (aZoomCoef + theMargin));

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
  return myView->IsCullingEnabled();
}

// =======================================================================
// function : SetFrustumCulling
// purpose  :
// =======================================================================
void V3d_View::SetFrustumCulling (const Standard_Boolean theToClip)
{
  myView->SetCullingEnabled (theToClip);
}

// =======================================================================
// function : DiagnosticInformation
// purpose  :
// =======================================================================
void V3d_View::DiagnosticInformation (TColStd_IndexedDataMapOfStringString& theDict,
                                      Graphic3d_DiagnosticInfo theFlags) const
{
  myView->DiagnosticInformation (theDict, theFlags);
}

//=============================================================================
//function : RenderingParams
//purpose  :
//=============================================================================
const Graphic3d_RenderingParams& V3d_View::RenderingParams() const
{
  return myView->RenderingParams();
}

//=============================================================================
//function : ChangeRenderingParams
//purpose  :
//=============================================================================
Graphic3d_RenderingParams& V3d_View::ChangeRenderingParams()
{
  return myView->ChangeRenderingParams();
}
