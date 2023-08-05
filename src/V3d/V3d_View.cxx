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

#include <Aspect_CircularGrid.hxx>
#include <Aspect_GradientBackground.hxx>
#include <Aspect_Grid.hxx>
#include <Aspect_NeutralWindow.hxx>
#include <Aspect_RectangularGrid.hxx>
#include <Aspect_Window.hxx>
#include <Bnd_Box.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_MapIteratorOfMapOfStructure.hxx>
#include <Graphic3d_MapOfStructure.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_TextureEnv.hxx>
#include <Image_AlienPixMap.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Assert.hxx>
#include <Standard_DivideByZero.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_ShortReal.hxx>
#include <Standard_Type.hxx>
#include <Standard_TypeMismatch.hxx>
#include <TColStd_Array2OfReal.hxx>
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
: myIsInvalidatedImmediate (Standard_True),
  MyViewer (theViewer.operator->()),
  SwitchSetFront (Standard_False),
  myZRotation (Standard_False),
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
: myIsInvalidatedImmediate (Standard_True),
  MyViewer (theViewer.operator->()),
  SwitchSetFront(Standard_False),
  myZRotation (Standard_False),
  MyTrsf (1, 4, 1, 4)
{
  myView = theViewer->Driver()->CreateView (theViewer->StructureManager());

  myView->CopySettings (theView->View());
  myDefaultViewPoint = theView->myDefaultViewPoint;
  myDefaultViewAxis  = theView->myDefaultViewAxis;

  myDefaultCamera = new Graphic3d_Camera (theView->DefaultCamera());

  myImmediateUpdate = Standard_False;
  SetAutoZFitMode (theView->AutoZFitMode(), theView->AutoZFitScaleFactor());
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
  if (myParentView != nullptr)
  {
    myParentView->RemoveSubview (this);
    myParentView = nullptr;
  }
  {
    NCollection_Sequence<Handle(V3d_View)> aSubviews = mySubviews;
    mySubviews.Clear();
    for (const Handle(V3d_View)& aViewIter : aSubviews)
    {
      //aViewIter->Remove();
      aViewIter->myParentView = nullptr;
      aViewIter->MyWindow.Nullify();
      aViewIter->myView->Remove();
      if (aViewIter->MyViewer != nullptr)
      {
        aViewIter->MyViewer->SetViewOff (aViewIter);
      }
    }
  }

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
    myView->SetWindow (Handle(Graphic3d_CView)(), theWindow, nullptr);
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
  if (myParentView != nullptr)
  {
    throw Standard_ProgramError ("V3d_View::SetWindow() called twice");
  }

  // method V3d_View::SetWindow() should assign the field MyWindow before calling Redraw()
  MyWindow = theWindow;
  myView->SetWindow (Handle(Graphic3d_CView)(), theWindow, theContext);
  MyViewer->SetViewOn (this);
  SetRatio();
  if (myImmediateUpdate)
  {
    Redraw();
  }
}

//=============================================================================
//function : SetWindow
//purpose  :
//=============================================================================
void V3d_View::SetWindow (const Handle(V3d_View)& theParentView,
                          const Graphic3d_Vec2d& theSize,
                          Aspect_TypeOfTriedronPosition theCorner,
                          const Graphic3d_Vec2d& theOffset,
                          const Graphic3d_Vec2i& theMargins)
{
  if (myView->IsRemoved())
  {
    return;
  }

  Handle(V3d_View) aParentView = !theParentView->IsSubview()
                               ? theParentView
                               : theParentView->ParentView();
  if (aParentView != myParentView)
  {
    if (myParentView != nullptr)
    {
      throw Standard_ProgramError ("V3d_View::SetWindow() called twice");
    }

    myParentView = aParentView.get();
    aParentView->AddSubview (this);
  }

  Handle(Aspect_NeutralWindow) aWindow = new Aspect_NeutralWindow();
  aWindow->SetVirtual (true);
  aWindow->SetSize (4, 4);
  myView->SetSubviewCorner (theCorner);
  myView->SetSubviewSize (theSize);
  myView->SetSubviewOffset (theOffset);
  myView->SetSubviewMargins (theMargins);

  MyWindow = aWindow;
  myView->SetWindow (aParentView->View(), aWindow, 0);
  MyViewer->SetViewOn (this);
  SetRatio();
}

//=============================================================================
//function : Remove
//purpose  :
//=============================================================================
void V3d_View::Remove()
{
  if (!MyGrid.IsNull())
  {
    MyGrid->Erase();
  }
  if (!myTrihedron.IsNull())
  {
    myTrihedron->Erase();
  }

  if (myParentView != nullptr)
  {
    myParentView->RemoveSubview (this);
    myParentView = nullptr;
  }
  {
    NCollection_Sequence<Handle(V3d_View)> aSubviews = mySubviews;
    mySubviews.Clear();
    for (const Handle(V3d_View)& aViewIter : aSubviews)
    {
      aViewIter->Remove();
    }
  }

  if (MyViewer != nullptr)
  {
    MyViewer->DelView (this);
    MyViewer = nullptr;
  }
  myView->Remove();
  MyWindow.Nullify();
}

// =======================================================================
// function : AddSubview
// purpose  :
// =======================================================================
void V3d_View::AddSubview (const Handle(V3d_View)& theView)
{
  mySubviews.Append (theView);
}

// =======================================================================
// function : RemoveSubview
// purpose  :
// =======================================================================
bool V3d_View::RemoveSubview (const V3d_View* theView)
{
  for (NCollection_Sequence<Handle(V3d_View)>::Iterator aViewIter (mySubviews); aViewIter.More(); aViewIter.Next())
  {
    if (aViewIter.Value() == theView)
    {
      mySubviews.Remove (aViewIter);
      return true;
    }
  }
  return false;
}

// =============================================================================
// function : PickSubview
// purpose  :
// =============================================================================
Handle(V3d_View) V3d_View::PickSubview (const Graphic3d_Vec2i& thePnt) const
{
  if (thePnt.x() < 0
   || thePnt.x() >= MyWindow->Dimensions().x()
   || thePnt.y() < 0
   || thePnt.y() >= MyWindow->Dimensions().y())
  {
    return Handle(V3d_View)();
  }

  // iterate in opposite direction - from front to bottom views
  for (Standard_Integer aSubviewIter = mySubviews.Upper(); aSubviewIter >= mySubviews.Lower(); --aSubviewIter)
  {
    const Handle(V3d_View)& aSubview = mySubviews.Value (aSubviewIter);
    if (aSubview->View()->IsActive()
     && thePnt.x() >= aSubview->View()->SubviewTopLeft().x()
     && thePnt.x() < (aSubview->View()->SubviewTopLeft().x() + aSubview->Window()->Dimensions().x())
     && thePnt.y() >= aSubview->View()->SubviewTopLeft().y()
     && thePnt.y() < (aSubview->View()->SubviewTopLeft().y() + aSubview->Window()->Dimensions().y()))
    {
      return aSubview;
    }
  }

  return this;
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

  myIsInvalidatedImmediate = Standard_False;
  myView->Update();
  myView->Compute();
  AutoZFit();
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

  myIsInvalidatedImmediate = Standard_False;
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

  myIsInvalidatedImmediate = Standard_False;
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
  Handle(Graphic3d_LightSet) aLights = new Graphic3d_LightSet();
  for (V3d_ListOfLight::Iterator anActiveLightIter (myActiveLights); anActiveLightIter.More(); anActiveLightIter.Next())
  {
    aLights->Add (anActiveLightIter.Value());
  }
  myView->SetLights (aLights);
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
  if (myImmediateUpdate)
  {
    Redraw();
  }
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
  Handle(Graphic3d_Texture2D) aTextureMap = new Graphic3d_Texture2D (theFileName);
  aTextureMap->DisableModulate();
  SetBackgroundImage (aTextureMap, theFillStyle, theToUpdate);
}

//=============================================================================
//function : SetBackgroundImage
//purpose  :
//=============================================================================
void V3d_View::SetBackgroundImage (const Handle(Graphic3d_Texture2D)& theTexture,
                                   const Aspect_FillMethod theFillStyle,
                                   const Standard_Boolean theToUpdate)
{
  myView->SetBackgroundImage (theTexture);
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
//function : SetBackgroundCubeMap
//purpose  :
//=============================================================================
void V3d_View::SetBackgroundCubeMap (const Handle(Graphic3d_CubeMap)& theCubeMap,
                                     Standard_Boolean                 theToUpdatePBREnv,
                                     Standard_Boolean                 theToUpdate)
{
  myView->SetBackgroundImage (theCubeMap, theToUpdatePBREnv);
  if (myImmediateUpdate || theToUpdate)
  {
    Redraw();
  }
}

//=============================================================================
//function : SetBackgroundSkydome
//purpose  :
//=============================================================================
void V3d_View::SetBackgroundSkydome (const Aspect_SkydomeBackground& theAspect,
                                     Standard_Boolean theToUpdatePBREnv)
{
  myView->SetBackgroundSkydome (theAspect, theToUpdatePBREnv);
}

//=============================================================================
//function : IsImageBasedLighting
//purpose  :
//=============================================================================
Standard_Boolean V3d_View::IsImageBasedLighting() const
{
  return !myView->IBLCubeMap().IsNull();
}

//=============================================================================
//function : SetImageBasedLighting
//purpose  :
//=============================================================================
void V3d_View::SetImageBasedLighting (Standard_Boolean theToEnableIBL,
                                      Standard_Boolean theToUpdate)
{
  myView->SetImageBasedLighting (theToEnableIBL);
  if (myImmediateUpdate || theToUpdate)
  {
    Redraw();
  }
}

//=============================================================================
//function : SetAxis
//purpose  :
//=============================================================================
void V3d_View::SetAxis (const Standard_Real theX,  const Standard_Real theY,  const Standard_Real theZ,
                        const Standard_Real theVx, const Standard_Real theVy, const Standard_Real theVz)
{
  myDefaultViewPoint.SetCoord (theX, theY, theZ);
  myDefaultViewAxis.SetCoord (theVx, theVy, theVz);
}

//=============================================================================
//function : SetShadingModel
//purpose  :
//=============================================================================
void V3d_View::SetShadingModel (const Graphic3d_TypeOfShadingModel theShadingModel)
{
  myView->SetShadingModel (theShadingModel);
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
    myCamStartOpDir    = aCamera->Direction();
    myCamStartOpEye    = aCamera->Eye();
    myCamStartOpCenter = aCamera->Center();
  }

  aCamera->SetUp (myCamStartOpUp);
  aCamera->SetEyeAndCenter (myCamStartOpEye, myCamStartOpCenter);
  aCamera->SetDirectionFromEye (myCamStartOpDir);

  // rotate camera around 3 initial axes
  gp_Dir aBackDir = -myCamStartOpDir;
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
    myCamStartOpDir    = aCamera->Direction();
    myCamStartOpEye    = aCamera->Eye();
    myCamStartOpCenter = aCamera->Center();
  }

  const Graphic3d_Vertex& aVref = myGravityReferencePoint;

  aCamera->SetUp (myCamStartOpUp);
  aCamera->SetEyeAndCenter (myCamStartOpEye, myCamStartOpCenter);
  aCamera->SetDirectionFromEye (myCamStartOpDir);

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
void V3d_View::Rotate (const V3d_TypeOfAxe theAxe, const Standard_Real theAngle,
                       const Standard_Real theX, const Standard_Real theY, const Standard_Real theZ, const Standard_Boolean theStart)
{
  Standard_Real anAngle = theAngle;

  if (anAngle > 0.0) while (anAngle > DEUXPI) anAngle -= DEUXPI;
  else if (anAngle < 0.0) while (anAngle < -DEUXPI) anAngle += DEUXPI;

  Handle(Graphic3d_Camera) aCamera = Camera();

  if (theStart)
  {
    myGravityReferencePoint.SetCoord (theX, theY, theZ);
    myCamStartOpUp     = aCamera->Up();
    myCamStartOpDir    = aCamera->Direction();
    myCamStartOpEye    = aCamera->Eye();
    myCamStartOpCenter = aCamera->Center();
    switch (theAxe)
    {
      case V3d_X: myViewAxis = gp::DX(); break;
      case V3d_Y: myViewAxis = gp::DY(); break;
      case V3d_Z: myViewAxis = gp::DZ(); break;
    }
  }

  const Graphic3d_Vertex& aVref = myGravityReferencePoint;

  aCamera->SetUp (myCamStartOpUp);
  aCamera->SetEyeAndCenter (myCamStartOpEye, myCamStartOpCenter);
  aCamera->SetDirectionFromEye (myCamStartOpDir);

  // rotate camera around passed axis
  gp_Trsf aRotation;
  gp_Pnt aRCenter (aVref.X(), aVref.Y(), aVref.Z());
  gp_Dir aRAxis ((theAxe == V3d_X) ? 1.0 : 0.0,
                 (theAxe == V3d_Y) ? 1.0 : 0.0,
                 (theAxe == V3d_Z) ? 1.0 : 0.0);

  aRotation.SetRotation (gp_Ax1 (aRCenter, aRAxis), anAngle);

  aCamera->Transform (aRotation);

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

  if (Start)
  {
    myCamStartOpUp     = aCamera->Up();
    myCamStartOpDir    = aCamera->Direction();
    myCamStartOpEye    = aCamera->Eye();
    myCamStartOpCenter = aCamera->Center();
  }

  aCamera->SetUp (myCamStartOpUp);
  aCamera->SetEyeAndCenter (myCamStartOpEye, myCamStartOpCenter);
  aCamera->SetDirectionFromEye (myCamStartOpDir);

  gp_Trsf aRotation;
  gp_Pnt aRCenter (myDefaultViewPoint);
  gp_Dir aRAxis (myDefaultViewAxis);
  aRotation.SetRotation (gp_Ax1 (aRCenter, aRAxis), Angle);

  aCamera->Transform (aRotation);

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

  if (Start)
  {
    myCamStartOpUp     = aCamera->Up();
    myCamStartOpDir    = aCamera->Direction();
    myCamStartOpEye    = aCamera->Eye();
    myCamStartOpCenter = aCamera->Center();
  }

  aCamera->SetUp (myCamStartOpUp);
  aCamera->SetEyeAndCenter (myCamStartOpEye, myCamStartOpCenter);
  aCamera->SetDirectionFromEye (myCamStartOpDir);

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

  if (Start)
  {
    myCamStartOpUp     = aCamera->Up();
    myCamStartOpDir    = aCamera->Direction();
    myCamStartOpEye    = aCamera->Eye();
    myCamStartOpCenter = aCamera->Center();
  }

  aCamera->SetUp (myCamStartOpUp);
  aCamera->SetEyeAndCenter (myCamStartOpEye, myCamStartOpCenter);
  aCamera->SetDirectionFromEye (myCamStartOpDir);

  gp_Trsf aRotation;
  gp_Pnt aRCenter = aCamera->Eye();
  gp_Dir aRAxis (myDefaultViewAxis);
  aRotation.SetRotation (gp_Ax1 (aRCenter, aRAxis), Angle);

  aCamera->Transform (aRotation);

  ImmediateUpdate();
}

//=============================================================================
//function : SetTwist
//purpose  :
//=============================================================================
void V3d_View::SetTwist(const Standard_Real angle)
{
  Standard_Real Angle = angle ;

  if( Angle > 0. ) while ( Angle > DEUXPI ) Angle -= DEUXPI ;
  else if( Angle < 0. ) while ( Angle < -DEUXPI ) Angle += DEUXPI ;

  Handle(Graphic3d_Camera) aCamera = Camera();

  const gp_Dir aReferencePlane (aCamera->Direction().Reversed());
  if (!screenAxis (aReferencePlane, gp::DZ(), myXscreenAxis, myYscreenAxis, myZscreenAxis)
   && !screenAxis (aReferencePlane, gp::DY(), myXscreenAxis, myYscreenAxis, myZscreenAxis)
   && !screenAxis (aReferencePlane, gp::DX(), myXscreenAxis, myYscreenAxis, myZscreenAxis))
  {
    throw V3d_BadValue ("V3d_ViewSetTwist, alignment of Eye,At,Up,");
  }
  
  gp_Pnt aRCenter = aCamera->Center();
  gp_Dir aZAxis (aCamera->Direction().Reversed());

  gp_Trsf aTrsf;
  aTrsf.SetRotation (gp_Ax1 (aRCenter, aZAxis), Angle);

  aCamera->SetUp (gp_Dir (myYscreenAxis));
  aCamera->Transform (aTrsf);

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

  SetImmediateUpdate (wasUpdateEnabled);

  ImmediateUpdate();
}

//=============================================================================
//function : SetProj
//purpose  :
//=============================================================================
void V3d_View::SetProj (const V3d_TypeOfOrientation theOrientation,
                        const Standard_Boolean theIsYup)
{
  Graphic3d_Vec3d anUp = theIsYup ? Graphic3d_Vec3d (0.0, 1.0, 0.0) : Graphic3d_Vec3d (0.0, 0.0, 1.0);
  if (theIsYup)
  {
    if (theOrientation == V3d_Ypos
     || theOrientation == V3d_Yneg)
    {
      anUp.SetValues (0.0, 0.0, -1.0);
    }
  }
  else
  {
    if (theOrientation == V3d_Zpos)
    {
      anUp.SetValues (0.0, 1.0, 0.0);
    }
    else if (theOrientation == V3d_Zneg)
    {
      anUp.SetValues (0.0, -1.0, 0.0);
    }
  }

  const gp_Dir aBck = V3d::GetProjAxis (theOrientation);

  // retain camera panning from origin when switching projection
  const Handle(Graphic3d_Camera)& aCamera = Camera();
  const gp_Pnt anOriginVCS = aCamera->ConvertWorld2View (gp::Origin());

  const Standard_Real aNewDist = aCamera->Eye().Distance (gp_Pnt (0, 0, 0));
  aCamera->SetEyeAndCenter (gp_XYZ (0, 0, 0) + aBck.XYZ() * aNewDist,
                            gp_XYZ (0, 0, 0));
  aCamera->SetDirectionFromEye (-aBck);
  aCamera->SetUp (gp_Dir (anUp.x(), anUp.y(), anUp.z()));
  aCamera->OrthogonalizeUp();

  Panning (anOriginVCS.X(), anOriginVCS.Y());

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

  SetImmediateUpdate (wasUpdateEnabled);

  ImmediateUpdate();
}

//=============================================================================
//function : SetUp
//purpose  :
//=============================================================================
void V3d_View::SetUp (const Standard_Real theVx, const Standard_Real theVy, const Standard_Real theVz)
{
  Handle(Graphic3d_Camera) aCamera = Camera();

  const gp_Dir aReferencePlane (aCamera->Direction().Reversed());
  const gp_Dir anUp (theVx, theVy, theVz);
  if (!screenAxis (aReferencePlane, anUp,     myXscreenAxis, myYscreenAxis, myZscreenAxis)
   && !screenAxis (aReferencePlane, gp::DZ(), myXscreenAxis, myYscreenAxis, myZscreenAxis)
   && !screenAxis (aReferencePlane, gp::DY(), myXscreenAxis, myYscreenAxis, myZscreenAxis)
   && !screenAxis (aReferencePlane, gp::DX(), myXscreenAxis, myYscreenAxis, myZscreenAxis))
  {
    throw V3d_BadValue ("V3d_View::Setup, alignment of Eye,At,Up");
  }

  aCamera->SetUp (gp_Dir (myYscreenAxis));

  ImmediateUpdate();
}

//=============================================================================
//function : SetUp
//purpose  :
//=============================================================================
void V3d_View::SetUp (const V3d_TypeOfOrientation theOrientation)
{
  Handle(Graphic3d_Camera) aCamera = Camera();

  const gp_Dir aReferencePlane (aCamera->Direction().Reversed());
  const gp_Dir anUp = V3d::GetProjAxis (theOrientation);
  if (!screenAxis (aReferencePlane, anUp,     myXscreenAxis, myYscreenAxis, myZscreenAxis)
   && !screenAxis (aReferencePlane, gp::DZ(), myXscreenAxis, myYscreenAxis, myZscreenAxis)
   && !screenAxis (aReferencePlane, gp::DY(), myXscreenAxis, myYscreenAxis, myZscreenAxis)
   && !screenAxis (aReferencePlane, gp::DX(), myXscreenAxis, myYscreenAxis, myZscreenAxis))
  {
    throw V3d_BadValue ("V3d_View::SetUp, alignment of Eye,At,Up");
  }

  aCamera->SetUp (gp_Dir (myYscreenAxis));

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
  ImmediateUpdate();
}

//=======================================================================
//function : ResetViewMapping
//purpose  :
//=======================================================================
void V3d_View::ResetViewMapping()
{
  Camera()->CopyMappingData (myDefaultCamera);
  ImmediateUpdate();
}

//=============================================================================
//function : Reset
//purpose  :
//=============================================================================
void V3d_View::Reset (const Standard_Boolean theToUpdate)
{
  Camera()->Copy (myDefaultCamera);

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
void V3d_View::ConvertToGrid(const Standard_Integer theXp,
                             const Standard_Integer theYp,
                             Standard_Real& theXg,
                             Standard_Real& theYg,
                             Standard_Real& theZg) const
{
  Graphic3d_Vec3d anXYZ;
  Convert (theXp, theYp, anXYZ.x(), anXYZ.y(), anXYZ.z());

  Graphic3d_Vertex aVrp;
  aVrp.SetCoord (anXYZ.x(), anXYZ.y(), anXYZ.z());
  if (MyViewer->IsGridActive())
  {
    Graphic3d_Vertex aNewVrp = Compute (aVrp);
    aNewVrp.Coord (theXg, theYg, theZg);
  }
  else
  {
    aVrp.Coord (theXg, theYg, theZg);
  }
}

//=======================================================================
//function : ConvertToGrid
//purpose  :
//=======================================================================
void V3d_View::ConvertToGrid(const Standard_Real theX,
                             const Standard_Real theY,
                             const Standard_Real theZ,
                             Standard_Real& theXg,
                             Standard_Real& theYg,
                             Standard_Real& theZg) const
{
  if (MyViewer->IsGridActive())
  {
    Graphic3d_Vertex aVrp (theX, theY, theZ);
    Graphic3d_Vertex aNewVrp = Compute (aVrp);
    aNewVrp.Coord (theXg, theYg, theZg);
  }
  else
  {
    theXg = theX; theYg = theY; theZg = theZ;
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
void V3d_View::Convert(const Standard_Integer theXp,
                       const Standard_Integer theYp,
                       Standard_Real& theX,
                       Standard_Real& theY,
                       Standard_Real& theZ) const
{
  V3d_UnMapped_Raise_if (!myView->IsDefined(), "view has no window");
  Standard_Integer aHeight = 0, aWidth = 0;
  MyWindow->Size (aWidth, aHeight);

  const gp_Pnt anXYZ (2.0 * theXp / aWidth - 1.0,
                      2.0 * (aHeight - 1 - theYp) / aHeight - 1.0,
                      Camera()->IsZeroToOneDepth() ? 0.0 : -1.0);
  const gp_Pnt aResult = Camera()->UnProject (anXYZ);
  theX = aResult.X();
  theY = aResult.Y();
  theZ = aResult.Z();
}

//=======================================================================
//function : ConvertWithProj
//purpose  :
//=======================================================================
void V3d_View::ConvertWithProj(const Standard_Integer theXp,
                               const Standard_Integer theYp,
                               Standard_Real& theX,
                               Standard_Real& theY,
                               Standard_Real& theZ,
                               Standard_Real& theDx,
                               Standard_Real& theDy,
                               Standard_Real& theDz) const
{
  V3d_UnMapped_Raise_if (!myView->IsDefined(), "view has no window");
  Standard_Integer aHeight = 0, aWidth = 0;
  MyWindow->Size (aWidth, aHeight);

  const Standard_Real anX = 2.0 * theXp / aWidth - 1.0;
  const Standard_Real anY = 2.0 * (aHeight - 1 - theYp) / aHeight - 1.0;
  const Standard_Real  aZ = 2.0 * 0.0 - 1.0;

  const Handle(Graphic3d_Camera)& aCamera = Camera();
  const gp_Pnt aResult1 = aCamera->UnProject (gp_Pnt (anX, anY, aZ));
  const gp_Pnt aResult2 = aCamera->UnProject (gp_Pnt (anX, anY, aZ - 10.0));

  theX = aResult1.X();
  theY = aResult1.Y();
  theZ = aResult1.Z();
  Graphic3d_Vec3d aNormDir (theX - aResult2.X(),
                            theY - aResult2.Y(),
                            theZ - aResult2.Z());
  aNormDir.Normalize();

  theDx = aNormDir.x();
  theDy = aNormDir.y();
  theDz = aNormDir.z();
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
  theZp = Camera()->IsZeroToOneDepth()
        ? aPoint.Z() * aZSize
        : aPoint.Z() * aZSize * 0.5;
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
//function : GravityPoint
//purpose  :
//=======================================================================
gp_Pnt V3d_View::GravityPoint() const
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

  return aResult;
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
  gp_Vec Xaxis, Yaxis, Zaxis;
  const gp_Dir aReferencePlane (Camera()->Direction().Reversed());
  if (!screenAxis (aReferencePlane, gp::DZ(), Xaxis, Yaxis, Zaxis)
   && !screenAxis (aReferencePlane, gp::DY(), Xaxis, Yaxis, Zaxis)
   && !screenAxis (aReferencePlane, gp::DX(), Xaxis, Yaxis, Zaxis))
  {
    //
  }

  // Compute Cross Vector From Up & Origin
  const gp_Dir aCameraUp = Camera()->Up();
  const gp_XYZ aP = Yaxis.XYZ().Crossed (aCameraUp.XYZ());

  // compute Angle
  Standard_Real anAngle = ASin (Max (Min (aP.Modulus(), 1.0), -1.0));
  if (Yaxis.Dot (aCameraUp.XYZ()) < 0.0)
  {
    anAngle = M_PI - anAngle;
  }
  if (anAngle > 0.0
   && anAngle < M_PI)
  {
    const gp_Dir aProjDir = Camera()->Direction().Reversed();
    if (aP.Dot (aProjDir.XYZ()) < 0.0)
    {
      anAngle = DEUXPI - anAngle;
    }
  }
  return anAngle;
}

//=============================================================================
//function : ShadingModel
//purpose  :
//=============================================================================
Graphic3d_TypeOfShadingModel V3d_View::ShadingModel() const
{
  return myView->ShadingModel();
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
//function : IfWindow
//purpose  :
//=============================================================================
Standard_Boolean V3d_View::IfWindow() const
{
  return myView->IsDefined();
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
//function : screenAxis
//purpose  :
//=============================================================================
Standard_Boolean V3d_View::screenAxis (const gp_Dir& theVpn, const gp_Dir& theVup,
                                       gp_Vec& theXaxe, gp_Vec& theYaxe, gp_Vec& theZaxe)
{
  theXaxe = theVup.XYZ().Crossed (theVpn.XYZ());
  if (theXaxe.Magnitude() <= gp::Resolution())
  {
    return Standard_False;
  }
  theXaxe.Normalize();

  theYaxe = theVpn.XYZ().Crossed (theXaxe.XYZ());
  if (theYaxe.Magnitude() <= gp::Resolution())
  {
    return Standard_False;
  }
  theYaxe.Normalize();

  theZaxe = theVpn.XYZ();
  theZaxe.Normalize();
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
    myCamStartOpDir    = aCamera->Direction();
    myCamStartOpEye    = aCamera->Eye();
    myCamStartOpCenter = aCamera->Center();
  }

  Standard_Boolean wasUpdateEnabled = SetImmediateUpdate (Standard_False);

  gp_Pnt aViewDims = aCamera->ViewDimensions();

  aCamera->SetEyeAndCenter (myCamStartOpEye, myCamStartOpCenter);
  aCamera->SetDirectionFromEye (myCamStartOpDir);
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
  myRotateGravity = GravityPoint();
  Rotate (0.0, 0.0, 0.0,
          myRotateGravity.X(), myRotateGravity.Y(), myRotateGravity.Z(),
          Standard_True);
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

  Rotate (dx, dy, dz,
          myRotateGravity.X(), myRotateGravity.Y(), myRotateGravity.Z(),
          Standard_False);
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
void V3d_View::SetBackFacingModel (const Graphic3d_TypeOfBackfacingModel theModel)
{
  myView->SetBackfacingModel (theModel);
  Redraw();
}

//=============================================================================
//function : BackFacingModel
//purpose  :
//=============================================================================
Graphic3d_TypeOfBackfacingModel V3d_View::BackFacingModel() const
{
  return myView->BackfacingModel();
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
        case Graphic3d_BT_Red:                 aFormat = Image_Format_Gray;  break;
      }

      if (!theImage.InitZero (aFormat, Standard_Size(aTargetSize.x()), Standard_Size(aTargetSize.y())))
      {
        Message::SendFail (TCollection_AsciiString ("Fail to allocate an image ") + aTargetSize.x() + "x" + aTargetSize.y() + " for view dump");
        return Standard_False;
      }
    }
  }
  if (theImage.IsEmpty())
  {
    Message::SendFail ("V3d_View::ToPixMap() has been called without image dimensions");
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
    Standard_Integer aMaxTexSizeX = MyViewer->Driver()->InquireLimit (Graphic3d_TypeOfLimit_MaxViewDumpSizeX);
    Standard_Integer aMaxTexSizeY = MyViewer->Driver()->InquireLimit (Graphic3d_TypeOfLimit_MaxViewDumpSizeY);
    if (theParams.TileSize > aMaxTexSizeX
     || theParams.TileSize > aMaxTexSizeY)
    {
      Message::SendFail (TCollection_AsciiString ("Image dump can not be performed - specified tile size (")
                       + theParams.TileSize + ") exceeds hardware limits (" + aMaxTexSizeX + "x" + aMaxTexSizeY + ")");
      return Standard_False;
    }

    if (aFBOVPSize.x() > aMaxTexSizeX
     || aFBOVPSize.y() > aMaxTexSizeY)
    {
      if (MyViewer->Driver()->InquireLimit (Graphic3d_TypeOfLimit_IsWorkaroundFBO))
      {
        Message::SendWarning ("Warning, workaround for Intel driver problem with empty FBO for images with big width is applied");
      }
      Message::SendInfo (TCollection_AsciiString ("Info, tiling image dump is used, image size (")
                       + aFBOVPSize.x() + "x" + aFBOVPSize.y() + ") exceeds hardware limits (" + aMaxTexSizeX + "x" + aMaxTexSizeY + ")");
      aFBOVPSize.x() = Min (aFBOVPSize.x(), aMaxTexSizeX);
      aFBOVPSize.y() = Min (aFBOVPSize.y(), aMaxTexSizeY);
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

    Message::SendWarning ("Warning, on screen buffer is used for image dump - content might be invalid");
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
        Graphic3d_CameraTile aTileUncropped;
        aTileUncropped.Offset    = anOffset;
        aTileUncropped.TotalSize = aTargetSize;
        aTileUncropped.TileSize  = aFBOVPSize;
        const Graphic3d_CameraTile aTile = aTileUncropped.Cropped();
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

        if (!aFBOPtr.IsNull())
        {
          aCamera->SetTile (aTile);
          myView->FBOChangeViewport (aFBOPtr, aTile.TileSize.x(), aTile.TileSize.y());
        }
        else
        {
          // no API to resize viewport of on-screen buffer - render uncropped
          aCamera->SetTile (aTileUncropped);
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
  if (!theCamera->FitMinMax (theBox, theResolution, theToEnlargeIfLine))
  {
    return Standard_False; // bounding box is out of bounds...
  }

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
  Invalidate();
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
  Invalidate();
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

//=======================================================================
//function : StatisticInformation
//purpose  :
//=======================================================================
void V3d_View::StatisticInformation (TColStd_IndexedDataMapOfStringString& theDict) const
{
  myView->StatisticInformation (theDict);
}

// =======================================================================
// function : StatisticInformation
// purpose  :
// =======================================================================
TCollection_AsciiString V3d_View::StatisticInformation() const
{
  return myView->StatisticInformation();
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


//=============================================================================
//function : SetLightOn
//purpose  :
//=============================================================================
void V3d_View::SetLightOn (const Handle(V3d_Light)& theLight)
{
  if (!myActiveLights.Contains (theLight))
  {
    myActiveLights.Append (theLight);
    UpdateLights();
  }
}

//=============================================================================
//function : SetLightOff
//purpose  :
//=============================================================================
void V3d_View::SetLightOff (const Handle(V3d_Light)& theLight)
{
  if (MyViewer->IsGlobalLight (theLight))
    throw Standard_TypeMismatch("V3d_View::SetLightOff, the light is global");
  myActiveLights.Remove (theLight);
  UpdateLights();
}

//=============================================================================
//function : IsActiveLight
//purpose  :
//=============================================================================
Standard_Boolean V3d_View::IsActiveLight (const Handle(V3d_Light)& theLight) const
{
  return !theLight.IsNull()
       && myActiveLights.Contains (theLight);
}

//=============================================================================
//function : SetLightOn
//purpose  :
//=============================================================================
void V3d_View::SetLightOn()
{
  for (V3d_ListOfLightIterator aDefLightIter (MyViewer->DefinedLightIterator()); aDefLightIter.More(); aDefLightIter.Next())
  {
    if (!myActiveLights.Contains (aDefLightIter.Value()))
    {
      myActiveLights.Append (aDefLightIter.Value());
    }
  }
  UpdateLights();
}

//=============================================================================
//function : SetLightOff
//purpose  :
//=============================================================================
void V3d_View::SetLightOff()
{
  for (V3d_ListOfLight::Iterator anActiveLightIter (myActiveLights); anActiveLightIter.More();)
  {
    if (!MyViewer->IsGlobalLight (anActiveLightIter.Value()))
    {
      myActiveLights.Remove (anActiveLightIter);
    }
    else
    {
      anActiveLightIter.Next();
    }
  }
  UpdateLights();
}

//=============================================================================
//function : IfMoreLights
//purpose  :
//=============================================================================
Standard_Boolean V3d_View::IfMoreLights() const
{
  return myActiveLights.Extent() < LightLimit();
}

//=======================================================================
//function : LightLimit
//purpose  :
//=======================================================================
Standard_Integer V3d_View::LightLimit() const
{
  return Viewer()->Driver()->InquireLightLimit();
}

//=======================================================================
//function : AddClipPlane
//purpose  :
//=======================================================================
void V3d_View::AddClipPlane (const Handle(Graphic3d_ClipPlane)& thePlane)
{
  Handle(Graphic3d_SequenceOfHClipPlane) aSeqOfPlanes = ClipPlanes();
  if (aSeqOfPlanes.IsNull())
  {
    aSeqOfPlanes = new Graphic3d_SequenceOfHClipPlane();
  }
  else
  {
    for (Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt (*aSeqOfPlanes); aPlaneIt.More(); aPlaneIt.Next())
    {
      const Handle(Graphic3d_ClipPlane)& aPlane = aPlaneIt.Value();
      if (aPlane == thePlane)
      {
        // plane is already defined in view
        return;
      }
    }
  }

  aSeqOfPlanes->Append (thePlane);
  SetClipPlanes (aSeqOfPlanes);
}

//=======================================================================
//function : RemoveClipPlane
//purpose  :
//=======================================================================
void V3d_View::RemoveClipPlane (const Handle(Graphic3d_ClipPlane)& thePlane)
{
  Handle(Graphic3d_SequenceOfHClipPlane) aSeqOfPlanes = ClipPlanes();
  if (aSeqOfPlanes.IsNull())
  {
    return;
  }

  for (Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt (*aSeqOfPlanes); aPlaneIt.More(); aPlaneIt.Next())
  {
    const Handle(Graphic3d_ClipPlane)& aPlane = aPlaneIt.Value();
    if (aPlane != thePlane)
    {
      continue;
    }

    aSeqOfPlanes->Remove (aPlaneIt);
    SetClipPlanes (aSeqOfPlanes);
    return;
  }
}

//=======================================================================
//function : SetClipPlanes
//purpose  :
//=======================================================================
void V3d_View::SetClipPlanes (const Handle(Graphic3d_SequenceOfHClipPlane)& thePlanes)
{
  myView->SetClipPlanes (thePlanes);
}

//=======================================================================
//function : ClipPlanes
//purpose  :
//=======================================================================
const Handle(Graphic3d_SequenceOfHClipPlane)& V3d_View::ClipPlanes() const
{
  return myView->ClipPlanes();
}

//=======================================================================
//function : PlaneLimit
//purpose  :
//=======================================================================
Standard_Integer V3d_View::PlaneLimit() const
{
  return Viewer()->Driver()->InquirePlaneLimit();
}

//=============================================================================
//function : Move
//purpose  :
//=============================================================================
void V3d_View::Move (const Standard_Real theDx,
                     const Standard_Real theDy,
                     const Standard_Real theDz,
                     const Standard_Boolean theStart)
{
  Handle(Graphic3d_Camera) aCamera = Camera();
  if (theStart)
  {
    myCamStartOpEye = aCamera->Eye();

    gp_Dir aReferencePlane (aCamera->Direction().Reversed());
    gp_Dir anUp (aCamera->Up());
    if (!screenAxis (aReferencePlane, anUp, myXscreenAxis, myYscreenAxis, myZscreenAxis))
    {
      throw V3d_BadValue ("V3d_View::Translate, alignment of Eye,At,Up");
    }
  }

  Standard_Real XX, XY, XZ, YX, YY, YZ, ZX, ZY, ZZ;
  myXscreenAxis.Coord (XX,XY,XZ);
  myYscreenAxis.Coord (YX,YY,YZ);
  myZscreenAxis.Coord (ZX,ZY,ZZ);

  aCamera->SetEye (myCamStartOpEye);

  aCamera->SetEye (aCamera->Eye().XYZ()
    + theDx * gp_Pnt (XX, XY, XZ).XYZ()
    + theDy * gp_Pnt (YX, YY, YZ).XYZ()
    + theDz * gp_Pnt (ZX, ZY, ZZ).XYZ()
    );

  ImmediateUpdate();
}

//=============================================================================
//function : Move
//purpose  :
//=============================================================================
void V3d_View::Move (const Standard_Real theLength, const Standard_Boolean theStart)
{
  Handle(Graphic3d_Camera) aCamera = Camera();
  if (theStart)
  {
    myCamStartOpEye = aCamera->Eye();
  }
  aCamera->SetEye (myCamStartOpEye);
  aCamera->SetEye (aCamera->Eye().XYZ() + theLength * myDefaultViewAxis.XYZ());

  ImmediateUpdate();
}

//=============================================================================
//function : Move
//purpose  :
//=============================================================================
void V3d_View::Move (const V3d_TypeOfAxe theAxe,
                     const Standard_Real theLength,
                     const Standard_Boolean theStart)
{
  switch (theAxe)
  {
    case V3d_X:
      Move (theLength,0.,0.,theStart);
      break;
    case V3d_Y:
      Move (0.,theLength,0.,theStart);
      break;
    case V3d_Z:
      Move (0.,0.,theLength,theStart);
      break;
  }
}

//=============================================================================
//function : Translate
//purpose  :
//=============================================================================
void V3d_View::Translate (const Standard_Real theDx,
                          const Standard_Real theDy,
                          const Standard_Real theDz,
                          const Standard_Boolean theStart)
{
  Handle(Graphic3d_Camera) aCamera = Camera();
  if (theStart)
  {
    myCamStartOpEye = aCamera->Eye();
    myCamStartOpCenter = aCamera->Center();

    gp_Dir aReferencePlane (aCamera->Direction().Reversed());
    gp_Dir anUp (aCamera->Up());
    if (!screenAxis (aReferencePlane, anUp, myXscreenAxis, myYscreenAxis, myZscreenAxis))
    {
      throw V3d_BadValue ("V3d_View::Translate, alignment of Eye,At,Up");
    }
  }

  aCamera->SetEye (myCamStartOpEye);
  aCamera->SetCenter (myCamStartOpCenter);

  aCamera->SetCenter (aCamera->Center().XYZ()
    - theDx * myXscreenAxis.XYZ()
    - theDy * myYscreenAxis.XYZ()
    - theDz * myZscreenAxis.XYZ()
    );

  aCamera->SetEye (aCamera->Eye().XYZ()
    - theDx * myXscreenAxis.XYZ()
    - theDy * myYscreenAxis.XYZ()
    - theDz * myZscreenAxis.XYZ()
    );

  ImmediateUpdate();
}

//=============================================================================
//function : Translate
//purpose  :
//=============================================================================
void V3d_View::Translate (const V3d_TypeOfAxe theAxe, const Standard_Real theLength,const Standard_Boolean theStart)
{
  switch (theAxe)
  {
    case V3d_X:
      Translate (theLength,0.,0., theStart);
      break;
    case V3d_Y:
      Translate (0.,theLength,0., theStart);
      break;
    case V3d_Z:
      Translate (0.,0.,theLength, theStart);
      break;
  }
}

//=======================================================================
//function : Place
//purpose  :
//=======================================================================
void V3d_View::Place (const Standard_Integer theXp,
                      const Standard_Integer theYp,
                      const Standard_Real theZoomFactor)
{
  Standard_Integer aWinWidth  = 0;
  Standard_Integer aWinHeight = 0;
  View()->Window()->Size (aWinWidth, aWinHeight);

  Standard_Integer aWinCXp = aWinWidth  / 2;
  Standard_Integer aWinCYp = aWinHeight / 2;
  Pan (aWinCXp - theXp, -(aWinCYp - theYp), theZoomFactor / Scale());
}

//=======================================================================
//function : Translate
//purpose  :
//=======================================================================
void V3d_View::Translate (const Standard_Real theLength, const Standard_Boolean theStart)
{
  Handle(Graphic3d_Camera) aCamera = Camera();
  if (theStart)
  {
    myCamStartOpCenter = aCamera->Center() ;
  }

  gp_Pnt aNewCenter (myCamStartOpCenter.XYZ() - myDefaultViewAxis.XYZ() * theLength);
  aCamera->SetCenter (aNewCenter);

  ImmediateUpdate();
}

//=============================================================================
//function : SetGrid
//purpose  :
//=============================================================================
void V3d_View::SetGrid (const gp_Ax3& aPlane, const Handle(Aspect_Grid)& aGrid)
{
  MyPlane	= aPlane;
  MyGrid	= aGrid;

  Standard_Real xl, yl, zl;
  Standard_Real xdx, xdy, xdz;
  Standard_Real ydx, ydy, ydz;
  Standard_Real dx, dy, dz;
  aPlane.Location ().Coord (xl, yl, zl);
  aPlane.XDirection ().Coord (xdx, xdy, xdz);
  aPlane.YDirection ().Coord (ydx, ydy, ydz);
  aPlane.Direction ().Coord (dx, dy, dz);

  Standard_Real CosAlpha = Cos (MyGrid->RotationAngle ());
  Standard_Real SinAlpha = Sin (MyGrid->RotationAngle ());

  TColStd_Array2OfReal Trsf1 (1, 4, 1, 4);
  Trsf1 (4, 4) = 1.0;
  Trsf1 (4, 1) = Trsf1 (4, 2) = Trsf1 (4, 3) = 0.0;
  // Translation
  Trsf1 (1, 4) = xl,
  Trsf1 (2, 4) = yl,
  Trsf1 (3, 4) = zl;
  // Transformation change of marker
  Trsf1 (1, 1) = xdx,
  Trsf1 (2, 1) = xdy,
  Trsf1 (3, 1) = xdz,
  Trsf1 (1, 2) = ydx,
  Trsf1 (2, 2) = ydy,
  Trsf1 (3, 2) = ydz,
  Trsf1 (1, 3) = dx,
  Trsf1 (2, 3) = dy,
  Trsf1 (3, 3) = dz;

  TColStd_Array2OfReal Trsf2 (1, 4, 1, 4);
  Trsf2 (4, 4) = 1.0;
  Trsf2 (4, 1) = Trsf2 (4, 2) = Trsf2 (4, 3) = 0.0;
  // Translation of the origin
  Trsf2 (1, 4) = -MyGrid->XOrigin (),
  Trsf2 (2, 4) = -MyGrid->YOrigin (),
  Trsf2 (3, 4) = 0.0;
  // Rotation Alpha around axis -Z
  Trsf2 (1, 1) = CosAlpha,
  Trsf2 (2, 1) = -SinAlpha,
  Trsf2 (3, 1) = 0.0,
  Trsf2 (1, 2) = SinAlpha,
  Trsf2 (2, 2) = CosAlpha,
  Trsf2 (3, 2) = 0.0,
  Trsf2 (1, 3) = 0.0,
  Trsf2 (2, 3) = 0.0,
  Trsf2 (3, 3) = 1.0;

  Standard_Real valuetrsf;
  Standard_Real valueoldtrsf;
  Standard_Real valuenewtrsf;
  Standard_Integer i, j, k;
  // Calculation of the product of matrices
  for (i=1; i<=4; i++)
      for (j=1; j<=4; j++) {
    MyTrsf (i, j) = 0.0;
    for (k=1; k<=4; k++) {
        valueoldtrsf = Trsf1 (i, k);
        valuetrsf	 = Trsf2 (k, j);
        valuenewtrsf = MyTrsf (i, j) + valueoldtrsf * valuetrsf;
        MyTrsf (i, j) = valuenewtrsf;
    }
     }
}

//=============================================================================
//function : SetGridActivity
//purpose  :
//=============================================================================
void V3d_View::SetGridActivity (const Standard_Boolean AFlag)
{
  if (AFlag) MyGrid->Activate ();
  else MyGrid->Deactivate ();
}

//=============================================================================
//function : toPolarCoords
//purpose  :
//=============================================================================
void toPolarCoords (const Standard_Real theX, const Standard_Real theY,
                          Standard_Real& theR, Standard_Real& thePhi)
{
  theR = Sqrt (theX * theX + theY * theY);
  thePhi = ATan2 (theY, theX);
}

//=============================================================================
//function : toCartesianCoords
//purpose  :
//=============================================================================
void toCartesianCoords (const Standard_Real theR, const Standard_Real thePhi,
                              Standard_Real& theX, Standard_Real& theY)
{
  theX = theR * Cos (thePhi);
  theY = theR * Sin (thePhi);
}

//=============================================================================
//function : Compute
//purpose  :
//=============================================================================
Graphic3d_Vertex V3d_View::Compute (const Graphic3d_Vertex& theVertex) const
{
  const Handle(Graphic3d_Camera)& aCamera = Camera();
  gp_Dir VPN = aCamera->Direction().Reversed(); // RefPlane
  gp_Dir GPN = MyPlane.Direction();

  Standard_Real XPp = 0.0, YPp = 0.0;
  Project (theVertex.X(), theVertex.Y(), theVertex.Z(), XPp, YPp);

  // Casw when the plane of the grid and the plane of the view
  // are perpendicular to MYEPSILON2 close radians
  #define MYEPSILON2 M_PI / 180.0 // Delta between 2 angles
  if (Abs (VPN.Angle (GPN) - M_PI / 2.) < MYEPSILON2)
  {
    return theVertex;
  }

  const gp_XYZ aPnt0 = V3d_View::TrsPoint (Graphic3d_Vertex (0.0, 0.0, 0.0), MyTrsf);

  // get grid axes in world space
  const gp_XYZ aPnt1 = V3d_View::TrsPoint (Graphic3d_Vertex (1.0, 0.0, 0.0), MyTrsf);
  gp_Vec aGridX (aPnt0, aPnt1);
  aGridX.Normalize();

  const gp_XYZ aPnt2 = V3d_View::TrsPoint (Graphic3d_Vertex (0.0, 1.0, 0.0), MyTrsf);
  gp_Vec aGridY (aPnt0, aPnt2);
  aGridY.Normalize();

  // project ray from camera onto grid plane
  const gp_Vec aProjection  = aCamera->IsOrthographic()
                            ? gp_Vec (aCamera->Direction())
                            : gp_Vec (aCamera->Eye(), gp_Pnt (theVertex.X(), theVertex.Y(), theVertex.Z())).Normalized();
  const gp_Vec aPointOrigin = gp_Vec (gp_Pnt (theVertex.X(), theVertex.Y(), theVertex.Z()), aPnt0);
  const Standard_Real aT    = aPointOrigin.Dot (MyPlane.Direction()) / aProjection.Dot (MyPlane.Direction());
  const gp_XYZ aPointOnPlane = gp_XYZ (theVertex.X(), theVertex.Y(), theVertex.Z()) + aProjection.XYZ() * aT;

  if (Handle(Aspect_RectangularGrid) aRectGrid = Handle(Aspect_RectangularGrid)::DownCast (MyGrid))
  {
    // project point on plane to grid local space
    const gp_Vec aToPoint (aPnt0, aPointOnPlane);
    const Standard_Real anXSteps = Round (aGridX.Dot (aToPoint) / aRectGrid->XStep());
    const Standard_Real anYSteps = Round (aGridY.Dot (aToPoint) / aRectGrid->YStep());

    // clamp point to grid
    const gp_Vec aResult = aGridX * anXSteps * aRectGrid->XStep()
                         + aGridY * anYSteps * aRectGrid->YStep()
                         + gp_Vec (aPnt0);
    return Graphic3d_Vertex (aResult.X(), aResult.Y(), aResult.Z());
  }
  else if (Handle(Aspect_CircularGrid) aCircleGrid = Handle(Aspect_CircularGrid)::DownCast (MyGrid))
  {
    const Standard_Real anAlpha = M_PI / Standard_Real (aCircleGrid->DivisionNumber());

    // project point on plane to grid local space
    const gp_Vec aToPoint (aPnt0, aPointOnPlane);
    Standard_Real aLocalX = aGridX.Dot (aToPoint);
    Standard_Real aLocalY = aGridY.Dot (aToPoint);
    Standard_Real anR = 0.0, aPhi = 0.0;
    toPolarCoords (aLocalX, aLocalY, anR, aPhi);

    // clamp point to grid
    const Standard_Real anRSteps  = Round (anR / aCircleGrid->RadiusStep());
    const Standard_Real aPhiSteps = Round (aPhi / anAlpha);
    toCartesianCoords (anRSteps * aCircleGrid->RadiusStep(), aPhiSteps * anAlpha, aLocalX, aLocalY);

    const gp_Vec aResult = aGridX * aLocalX + aGridY * aLocalY + gp_Vec (aPnt0);
    return Graphic3d_Vertex (aResult.X(), aResult.Y(), aResult.Z());
  }
  return Graphic3d_Vertex (0.0, 0.0, 0.0);
}

//=============================================================================
//function : ZBufferTriedronSetup
//purpose  :
//=============================================================================
void V3d_View::ZBufferTriedronSetup (const Quantity_Color&  theXColor,
                                     const Quantity_Color&  theYColor,
                                     const Quantity_Color&  theZColor,
                                     const Standard_Real    theSizeRatio,
                                     const Standard_Real    theAxisDiametr,
                                     const Standard_Integer theNbFacettes)
{
  const Handle(V3d_Trihedron)& aTrihedron = Trihedron (true);
  aTrihedron->SetArrowsColor   (theXColor, theYColor, theZColor);
  aTrihedron->SetSizeRatio     (theSizeRatio);
  aTrihedron->SetNbFacets      (theNbFacettes);
  aTrihedron->SetArrowDiameter (theAxisDiametr);
}

//=============================================================================
//function : TriedronDisplay
//purpose  :
//=============================================================================
void V3d_View::TriedronDisplay (const Aspect_TypeOfTriedronPosition thePosition,
                                const Quantity_Color& theColor,
                                const Standard_Real theScale,
                                const V3d_TypeOfVisualization theMode)
{
  const Handle(V3d_Trihedron)& aTrihedron = Trihedron (true);
  aTrihedron->SetLabelsColor (theColor);
  aTrihedron->SetScale       (theScale);
  aTrihedron->SetPosition    (thePosition);
  aTrihedron->SetWireframe   (theMode == V3d_WIREFRAME);

  aTrihedron->Display (*this);
}

//=============================================================================
//function : TriedronErase
//purpose  :
//=============================================================================
void V3d_View::TriedronErase()
{
  if (!myTrihedron.IsNull())
  {
    myTrihedron->Erase();
  }
}

//=============================================================================
//function : GetGraduatedTrihedron
//purpose  :
//=============================================================================
const Graphic3d_GraduatedTrihedron& V3d_View::GetGraduatedTrihedron() const
{
  return myView->GetGraduatedTrihedron();
}

//=============================================================================
//function : GraduatedTrihedronDisplay
//purpose  :
//=============================================================================
void V3d_View::GraduatedTrihedronDisplay (const Graphic3d_GraduatedTrihedron& theTrihedronData)
{
  myView->GraduatedTrihedronDisplay (theTrihedronData);
}

//=============================================================================
//function : GraduatedTrihedronErase
//purpose  :
//=============================================================================
void V3d_View::GraduatedTrihedronErase()
{
  myView->GraduatedTrihedronErase();
}

// =======================================================================
// function : DumpJson
// purpose  :
// =======================================================================
void V3d_View::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myOldMouseX)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myOldMouseY)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myCamStartOpUp)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myCamStartOpDir)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myCamStartOpEye)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myCamStartOpCenter)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myDefaultCamera.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myView.get())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myImmediateUpdate)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myIsInvalidatedImmediate)
  
  OCCT_DUMP_FIELD_VALUE_POINTER (theOStream, MyViewer)
  for (V3d_ListOfLight::Iterator anIterator (myActiveLights); anIterator.More(); anIterator.Next())
  {
    class Handle(Graphic3d_CLight)& anActiveLight = anIterator.ChangeValue();
    OCCT_DUMP_FIELD_VALUE_POINTER (theOStream, anActiveLight)
  }
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myDefaultViewAxis)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myDefaultViewPoint)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, MyWindow.get())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, sx)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, sy)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, rx)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, ry)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myRotateGravity)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myComputedMode)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, SwitchSetFront)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myZRotation)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, MyZoomAtPointX)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, MyZoomAtPointY)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myTrihedron.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, MyGrid.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &MyPlane)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, MyGridEchoStructure.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, MyGridEchoGroup.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myXscreenAxis)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myYscreenAxis)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myZscreenAxis)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myViewAxis)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myGravityReferencePoint)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myAutoZFitIsOn)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myAutoZFitScaleFactor)
}
