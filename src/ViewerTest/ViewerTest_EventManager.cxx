// Created on: 1998-08-27
// Created by: Robert COUBLANC
// Copyright (c) 1998-1999 Matra Datavision
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

#include <ViewerTest_EventManager.hxx>

#include <AIS_AnimationCamera.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <Aspect_Grid.hxx>
#include <Draw.hxx>
#include <Message.hxx>
#include <ViewerTest_ContinuousRedrawer.hxx>
#include <ViewerTest_V3dView.hxx>
#include <ViewerTest.hxx>

#if defined(_WIN32)
  //
#elif defined(HAVE_XLIB)
  #include <Xw_Window.hxx>
  #include <X11/Xlib.h>
  #include <X11/Xutil.h>
#elif defined(__EMSCRIPTEN__)
  #include <Wasm_Window.hxx>
  #include <emscripten.h>
  #include <emscripten/html5.h>

//=======================================================================
//function : onWasmRedrawView
//purpose  :
//=======================================================================
void ViewerTest_EventManager::onWasmRedrawView (void* )
{
  Handle(ViewerTest_EventManager) aViewCtrl = ViewerTest::CurrentEventManager();
  if (!aViewCtrl.IsNull())
  {
    aViewCtrl->myNbUpdateRequests = 0;

    const Handle(V3d_View)& aView = ViewerTest::CurrentView();
    const Handle(AIS_InteractiveContext)& aCtx = ViewerTest::GetAISContext();
    if (!aView.IsNull() && !aCtx.IsNull())
    {
      aViewCtrl->ProcessExpose();
    }
  }
}
#endif

Standard_IMPORT Standard_Boolean Draw_Interprete (const char* theCommand);

IMPLEMENT_STANDARD_RTTIEXT(ViewerTest_EventManager,Standard_Transient)

//=======================================================================
//function : GlobalViewAnimation
//purpose  :
//=======================================================================
const Handle(AIS_AnimationCamera)& ViewerTest_EventManager::GlobalViewAnimation()
{
  static Handle(AIS_AnimationCamera) THE_CAMERA_ANIM = new AIS_AnimationCamera ("ViewerTest_EventManager_ViewAnimation", Handle(V3d_View)());
  THE_CAMERA_ANIM->SetOwnDuration (0.5);
  return THE_CAMERA_ANIM;
}

//=======================================================================
//function : ViewerTest_EventManager
//purpose  :
//=======================================================================
ViewerTest_EventManager::ViewerTest_EventManager (const Handle(V3d_View)&               theView,
                                                  const Handle(AIS_InteractiveContext)& theCtx)
: myCtx  (theCtx),
  myView (theView),
  myToPickPnt (Standard_False),
  myIsTmpContRedraw (Standard_False),
  myNbUpdateRequests (0)
{
  myViewAnimation = GlobalViewAnimation();

  addActionHotKeys (Aspect_VKey_NavForward,        (Standard_UInteger )Aspect_VKey_W,
                                                   (Standard_UInteger )Aspect_VKey_W | (Standard_UInteger )Aspect_VKeyFlags_SHIFT);
  addActionHotKeys (Aspect_VKey_NavBackward ,      (Standard_UInteger )Aspect_VKey_S,
                                                   (Standard_UInteger )Aspect_VKey_S | (Standard_UInteger )Aspect_VKeyFlags_SHIFT);
  addActionHotKeys (Aspect_VKey_NavSlideLeft,      (Standard_UInteger )Aspect_VKey_A,
                                                   (Standard_UInteger )Aspect_VKey_A | (Standard_UInteger )Aspect_VKeyFlags_SHIFT);
  addActionHotKeys (Aspect_VKey_NavSlideRight,     (Standard_UInteger )Aspect_VKey_D,
                                                   (Standard_UInteger )Aspect_VKey_D | (Standard_UInteger )Aspect_VKeyFlags_SHIFT);
  addActionHotKeys (Aspect_VKey_NavRollCCW,        (Standard_UInteger )Aspect_VKey_Q,
                                                   (Standard_UInteger )Aspect_VKey_Q | (Standard_UInteger )Aspect_VKeyFlags_SHIFT);
  addActionHotKeys (Aspect_VKey_NavRollCW,         (Standard_UInteger )Aspect_VKey_E,
                                                   (Standard_UInteger )Aspect_VKey_E | (Standard_UInteger )Aspect_VKeyFlags_SHIFT);

  addActionHotKeys (Aspect_VKey_NavSpeedIncrease,  (Standard_UInteger )Aspect_VKey_Plus,
                                                   (Standard_UInteger )Aspect_VKey_Plus | (Standard_UInteger )Aspect_VKeyFlags_SHIFT,
                                                   (Standard_UInteger )Aspect_VKey_Equal,
                                                   (Standard_UInteger )Aspect_VKey_NumpadAdd,
                                                   (Standard_UInteger )Aspect_VKey_NumpadAdd | (Standard_UInteger )Aspect_VKeyFlags_SHIFT);
  addActionHotKeys (Aspect_VKey_NavSpeedDecrease,  (Standard_UInteger )Aspect_VKey_Minus,
                                                   (Standard_UInteger )Aspect_VKey_Minus | (Standard_UInteger )Aspect_VKeyFlags_SHIFT,
                                                   (Standard_UInteger )Aspect_VKey_NumpadSubtract,
                                                   (Standard_UInteger )Aspect_VKey_NumpadSubtract | (Standard_UInteger )Aspect_VKeyFlags_SHIFT);

  addActionHotKeys (Aspect_VKey_NavLookUp,         (Standard_UInteger )Aspect_VKey_Up);
  addActionHotKeys (Aspect_VKey_NavLookDown,       (Standard_UInteger )Aspect_VKey_Down);
  addActionHotKeys (Aspect_VKey_NavLookLeft,       (Standard_UInteger )Aspect_VKey_Left);
  addActionHotKeys (Aspect_VKey_NavLookRight,      (Standard_UInteger )Aspect_VKey_Right);
  addActionHotKeys (Aspect_VKey_NavSlideLeft,      (Standard_UInteger )Aspect_VKey_Left  | (Standard_UInteger)Aspect_VKeyFlags_SHIFT);
  addActionHotKeys (Aspect_VKey_NavSlideRight,     (Standard_UInteger )Aspect_VKey_Right | (Standard_UInteger)Aspect_VKeyFlags_SHIFT);
  addActionHotKeys (Aspect_VKey_NavSlideUp,        (Standard_UInteger )Aspect_VKey_Up    | (Standard_UInteger)Aspect_VKeyFlags_SHIFT);
  addActionHotKeys (Aspect_VKey_NavSlideDown,      (Standard_UInteger )Aspect_VKey_Down  | (Standard_UInteger)Aspect_VKeyFlags_SHIFT);

  // window could be actually not yet set to the View
  //SetupWindowCallbacks (theView->Window());
}

//=======================================================================
//function : ~ViewerTest_EventManager
//purpose  :
//=======================================================================
ViewerTest_EventManager::~ViewerTest_EventManager()
{
  if (!myViewAnimation.IsNull()
    && myViewAnimation->View() == myView)
  {
    myViewAnimation->Stop();
    myViewAnimation->SetView (Handle(V3d_View)());
  }
}

// =======================================================================
// function : UpdateMouseClick
// purpose  :
// =======================================================================
bool ViewerTest_EventManager::UpdateMouseClick (const Graphic3d_Vec2i& thePoint,
                                                Aspect_VKeyMouse theButton,
                                                Aspect_VKeyFlags theModifiers,
                                                bool theIsDoubleClick)
{
  if (theIsDoubleClick && !myView.IsNull() && !myCtx.IsNull())
  {
    FitAllAuto (myCtx, myView);
    return true;
  }
  return AIS_ViewController::UpdateMouseClick (thePoint, theButton, theModifiers, theIsDoubleClick);
}

//=======================================================================
//function : UpdateMouseButtons
//purpose  :
//=======================================================================
bool ViewerTest_EventManager::UpdateMouseScroll (const Aspect_ScrollDelta& theDelta)
{
  if (!myView.IsNull()
    && (myView->IsSubview()
    || !myView->Subviews().IsEmpty()))
  {
    Handle(V3d_View) aParent = !myView->IsSubview() ? myView : myView->ParentView();
    Handle(V3d_View) aPickedView = aParent->PickSubview (theDelta.Point);
    if (!aPickedView.IsNull()
      && aPickedView != myView)
    {
      // switch input focus to another subview
      OnSubviewChanged (myCtx, myView, aPickedView);
      return true;
    }
  }

  return AIS_ViewController::UpdateMouseScroll (theDelta);
}

//=======================================================================
//function : UpdateMouseButtons
//purpose  :
//=======================================================================
bool ViewerTest_EventManager::UpdateMouseButtons (const Graphic3d_Vec2i& thePoint,
                                                  Aspect_VKeyMouse theButtons,
                                                  Aspect_VKeyFlags theModifiers,
                                                  bool theIsEmulated)
{
  if (!myView.IsNull()
    && myMousePressed == Aspect_VKeyMouse_NONE
    && theButtons != Aspect_VKeyMouse_NONE
    && (myView->IsSubview()
    || !myView->Subviews().IsEmpty()))
  {
    Handle(V3d_View) aParent = !myView->IsSubview() ? myView : myView->ParentView();
    Handle(V3d_View) aPickedView = aParent->PickSubview (thePoint);
    if (!aPickedView.IsNull()
      && aPickedView != myView)
    {
      // switch input focus to another subview
      OnSubviewChanged (myCtx, myView, aPickedView);
    }
  }

  SetAllowRotation (!ViewerTest_V3dView::IsCurrentViewIn2DMode());

  if (theButtons == Aspect_VKeyMouse_LeftButton)
  {
    if (myToPickPnt && (theModifiers & Aspect_VKeyFlags_CTRL) != 0)
    {
      Graphic3d_Vec3d anXYZ;
      myView->Convert (thePoint.x(), thePoint.y(), anXYZ.x(), anXYZ.y(), anXYZ.z());
      Draw::Set (myPickPntArgVec[0].ToCString(), anXYZ.x());
      Draw::Set (myPickPntArgVec[1].ToCString(), anXYZ.y());
      Draw::Set (myPickPntArgVec[2].ToCString(), anXYZ.z());
      myToPickPnt = false;
    }
  }

  return AIS_ViewController::UpdateMouseButtons (thePoint, theButtons, theModifiers, theIsEmulated);
}

//==============================================================================
//function : ProcessExpose
//purpose  :
//==============================================================================
void ViewerTest_EventManager::ProcessExpose()
{
  if (!myView.IsNull())
  {
    FlushViewEvents (myCtx, myView, true);
  }
}

//==============================================================================
//function : handleViewRedraw
//purpose  :
//==============================================================================
void ViewerTest_EventManager::handleViewRedraw (const Handle(AIS_InteractiveContext)& theCtx,
                                                const Handle(V3d_View)& theView)
{
  AIS_ViewController::handleViewRedraw (theCtx, theView);

  // On non-Windows platforms Aspect_Window::InvalidateContent() from rendering thread does not work as expected
  // as in Tcl event loop the new message might go to sleep with new event remaining in queue.
  // As a workaround - use dedicated background thread to ping Tcl event loop.
  if (myToAskNextFrame)
  {
    ViewerTest_ContinuousRedrawer& aRedrawer = ViewerTest_ContinuousRedrawer::Instance();
    if (!myIsTmpContRedraw
     && (!aRedrawer.IsStarted() || aRedrawer.IsPaused()))
    {
      myIsTmpContRedraw = true;
    #if !defined(_WIN32) && !defined(__EMSCRIPTEN__) && !defined(__APPLE__)
      aRedrawer.Start (theView, 60.0);
    #endif
    }

    // ask more frames
    if (++myNbUpdateRequests == 1)
    {
    #if defined(__EMSCRIPTEN__)
      emscripten_async_call (onWasmRedrawView, this, -1);
    #endif
    }
  }
  else if (myIsTmpContRedraw)
  {
    myIsTmpContRedraw = false;
  #if !defined(_WIN32) && !defined(__APPLE__)
    ViewerTest_ContinuousRedrawer& aRedrawer = ViewerTest_ContinuousRedrawer::Instance();
    aRedrawer.Pause();
  #endif
  }
}

//==============================================================================
//function : ProcessConfigure
//purpose  :
//==============================================================================
void ViewerTest_EventManager::ProcessConfigure (bool theIsResized)
{
  if (myView.IsNull())
  {
    return;
  }

  if (!theIsResized
    // track window moves to reverse stereo pair
    && myView->RenderingParams().StereoMode != Graphic3d_StereoMode_RowInterlaced
    && myView->RenderingParams().StereoMode != Graphic3d_StereoMode_ColumnInterlaced
    && myView->RenderingParams().StereoMode != Graphic3d_StereoMode_ChessBoard)
  {
    return;
  }

  Handle(V3d_View) aParent = !myView->IsSubview()
                            ? myView
                            : myView->ParentView();
  aParent->Window()->DoResize();
  aParent->MustBeResized();
  aParent->Invalidate();
  for (const Handle(V3d_View)& aChildIter : aParent->Subviews())
  {
    aChildIter->Window()->DoResize();
    aChildIter->MustBeResized();
    aChildIter->Invalidate();
  }

  FlushViewEvents (myCtx, myView, true);
}

//==============================================================================
//function : OnSubviewChanged
//purpose  :
//==============================================================================
void ViewerTest_EventManager::OnSubviewChanged (const Handle(AIS_InteractiveContext)& ,
                                                const Handle(V3d_View)& ,
                                                const Handle(V3d_View)& theNewView)
{
  ViewerTest::ActivateView (theNewView, false);
}

//==============================================================================
//function : ProcessInput
//purpose  :
//==============================================================================
void ViewerTest_EventManager::ProcessInput()
{
  if (myView.IsNull())
  {
    return;
  }

#if defined(__EMSCRIPTEN__)
  // Queue onWasmRedrawView() callback to redraw canvas after all user input is flushed by browser.
  // Redrawing viewer on every single message would be a pointless waste of resources,
  // as user will see only the last drawn frame due to WebGL implementation details.
  // -1 in emscripten_async_call() redirects to requestAnimationFrame();
  // requestPostAnimationFrame() is a better under development alternative.
  if (++myNbUpdateRequests == 1)
  {
    emscripten_async_call (onWasmRedrawView, this, -1);
  }
#else
  // handle synchronously
  ProcessExpose();
#endif
}

// =======================================================================
// function : navigationKeyModifierSwitch
// purpose  :
// =======================================================================
bool ViewerTest_EventManager::navigationKeyModifierSwitch (unsigned int theModifOld,
                                                           unsigned int theModifNew,
                                                           double       theTimeStamp)
{
  bool hasActions = false;
  for (unsigned int aKeyIter = 0; aKeyIter < Aspect_VKey_ModifiersLower; ++aKeyIter)
  {
    if (!myKeys.IsKeyDown (aKeyIter))
    {
      continue;
    }

    Aspect_VKey anActionOld = Aspect_VKey_UNKNOWN, anActionNew = Aspect_VKey_UNKNOWN;
    myNavKeyMap.Find (aKeyIter | theModifOld, anActionOld);
    myNavKeyMap.Find (aKeyIter | theModifNew, anActionNew);
    if (anActionOld == anActionNew)
    {
      continue;
    }

    if (anActionOld != Aspect_VKey_UNKNOWN)
    {
      myKeys.KeyUp (anActionOld, theTimeStamp);
    }
    if (anActionNew != Aspect_VKey_UNKNOWN)
    {
      hasActions = true;
      myKeys.KeyDown (anActionNew, theTimeStamp);
    }
  }
  return hasActions;
}

//=======================================================================
//function : KeyDown
//purpose  :
//=======================================================================
void ViewerTest_EventManager::KeyDown (Aspect_VKey theKey,
                                       double theTime,
                                       double thePressure)
{
  const unsigned int aModifOld = myKeys.Modifiers();
  AIS_ViewController::KeyDown (theKey, theTime, thePressure);

  const unsigned int aModifNew = myKeys.Modifiers();
  if (aModifNew != aModifOld
   && navigationKeyModifierSwitch (aModifOld, aModifNew, theTime))
  {
    // modifier key just pressed
  }

  Aspect_VKey anAction = Aspect_VKey_UNKNOWN;
  if (myNavKeyMap.Find (theKey | myKeys.Modifiers(), anAction)
  &&  anAction != Aspect_VKey_UNKNOWN)
  {
    AIS_ViewController::KeyDown (anAction, theTime, thePressure);
  }
}

//=======================================================================
//function : KeyUp
//purpose  :
//=======================================================================
void ViewerTest_EventManager::KeyUp (Aspect_VKey theKey,
                                     double theTime)
{
  const unsigned int aModifOld = myKeys.Modifiers();
  AIS_ViewController::KeyUp (theKey, theTime);

  Aspect_VKey anAction = Aspect_VKey_UNKNOWN;
  if (myNavKeyMap.Find (theKey | myKeys.Modifiers(), anAction)
  &&  anAction != Aspect_VKey_UNKNOWN)
  {
    AIS_ViewController::KeyUp (anAction, theTime);
    ProcessKeyPress (anAction);
  }

  const unsigned int aModifNew = myKeys.Modifiers();
  if (aModifNew != aModifOld
   && navigationKeyModifierSwitch (aModifOld, aModifNew, theTime))
  {
    // modifier key released
  }

  ProcessKeyPress (theKey | aModifNew);
}

//==============================================================================
//function : ProcessKeyPress
//purpose  :
//==============================================================================
void ViewerTest_EventManager::ProcessKeyPress (Aspect_VKey theKey)
{
  if (myCtx.IsNull()
   || myView.IsNull())
  {
    return;
  }

  switch (theKey)
  {
    case Aspect_VKey_Backspace: // AXO
    {
      if (!ViewerTest_V3dView::IsCurrentViewIn2DMode())
      {
        myView->SetProj(V3d_XposYnegZpos);
      }
      break;
    }
    case Aspect_VKey_F:
    {
      if (myCtx->NbSelected() > 0)
      {
        myCtx->FitSelected (myView);
      }
      else
      {
        myView->FitAll();
      }
      break;
    }
    case Aspect_VKey_H: // HLR
    {
      std::cout << "HLR\n";
      myView->SetComputedMode (!myView->ComputedMode());
      myView->Redraw();
      break;
    }
    case Aspect_VKey_P: // Type of HLR
    {
      myCtx->DefaultDrawer()->SetTypeOfHLR (myCtx->DefaultDrawer()->TypeOfHLR() == Prs3d_TOH_Algo
                                          ? Prs3d_TOH_PolyAlgo
                                          : Prs3d_TOH_Algo);
      if (myCtx->NbSelected() == 0)
      {
        AIS_ListOfInteractive aListOfShapes;
        myCtx->DisplayedObjects (aListOfShapes);
        for (AIS_ListIteratorOfListOfInteractive anIter (aListOfShapes); anIter.More(); anIter.Next())
        {
          if (Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast (anIter.Value()))
          {
            aShape->SetTypeOfHLR (aShape->TypeOfHLR() == Prs3d_TOH_PolyAlgo
                                ? Prs3d_TOH_Algo
                                : Prs3d_TOH_PolyAlgo);
            myCtx->Redisplay (aShape, Standard_False);
          }
        }
      }
      else
      {
        for (myCtx->InitSelected(); myCtx->MoreSelected(); myCtx->NextSelected())
        {
          if (Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast (myCtx->SelectedInteractive()))
          {
            aShape->SetTypeOfHLR (aShape->TypeOfHLR() == Prs3d_TOH_PolyAlgo
                                ? Prs3d_TOH_Algo
                                : Prs3d_TOH_PolyAlgo);
            myCtx->Redisplay (aShape, Standard_False);
          }
        }
      }
      myCtx->UpdateCurrentViewer();
      break;
    }
    case (Standard_UInteger)Aspect_VKey_S | (Standard_UInteger)Aspect_VKeyFlags_CTRL:
    case (Standard_UInteger)Aspect_VKey_W | (Standard_UInteger)Aspect_VKeyFlags_CTRL:
    {
      Standard_Integer aDispMode = AIS_Shaded;
      if (theKey == ((Standard_UInteger)Aspect_VKey_S | (Standard_UInteger)Aspect_VKeyFlags_CTRL))
      {
        aDispMode = AIS_Shaded;
        std::cout << "setup Shaded display mode\n";
      }
      else
      {
        aDispMode = AIS_WireFrame;
        std::cout << "setup WireFrame display mode\n";
      }

      if (myCtx->NbSelected() == 0)
      {
        myCtx->SetDisplayMode (aDispMode, true);
      }
      else
      {
        for (myCtx->InitSelected(); myCtx->MoreSelected(); myCtx->NextSelected())
        {
          myCtx->SetDisplayMode (myCtx->SelectedInteractive(), aDispMode, false);
        }
        myCtx->UpdateCurrentViewer();
      }
      break;
    }
    case Aspect_VKey_U: // Unset display mode
    {
      std::cout << "reset display mode to defaults\n";
      if (myCtx->NbSelected() == 0)
      {
        myCtx->SetDisplayMode (AIS_WireFrame, true);
      }
      else
      {
        for (myCtx->InitSelected(); myCtx->MoreSelected(); myCtx->NextSelected())
        {
          myCtx->UnsetDisplayMode (myCtx->SelectedInteractive(), false);
        }
        myCtx->UpdateCurrentViewer();
      }
      break;
    }
    case Aspect_VKey_T:
    {
      if (!ViewerTest_V3dView::IsCurrentViewIn2DMode())
      {
        myView->SetProj (V3d_TypeOfOrientation_Zup_Top);
      }
      break;
    }
    case Aspect_VKey_B:
    {
      if (!ViewerTest_V3dView::IsCurrentViewIn2DMode())
      {
        myView->SetProj (V3d_TypeOfOrientation_Zup_Bottom);
      }
      break;
    }
    case Aspect_VKey_L:
    {
      if (!ViewerTest_V3dView::IsCurrentViewIn2DMode())
      {
        myView->SetProj (V3d_TypeOfOrientation_Zup_Left);
      }
      break;
    }
    case Aspect_VKey_R:
    {
      if (!ViewerTest_V3dView::IsCurrentViewIn2DMode())
      {
        myView->SetProj (V3d_TypeOfOrientation_Zup_Right);
      }
      break;
    }
    case Aspect_VKey_Comma:
    {
      myCtx->HilightNextDetected (myView);
      break;
    }
    case Aspect_VKey_Period:
    {
      myCtx->HilightPreviousDetected (myView);
      break;
    }
    case Aspect_VKey_Slash:
    case Aspect_VKey_NumpadDivide:
    {
      Handle(Graphic3d_Camera) aCamera = myView->Camera();
      if (aCamera->IsStereo())
      {
        aCamera->SetIOD (aCamera->GetIODType(), aCamera->IOD() - 0.01);
        myView->Redraw();
      }
      break;
    }
    case Aspect_VKey_NumpadMultiply:
    {
      Handle(Graphic3d_Camera) aCamera = myView->Camera();
      if (aCamera->IsStereo())
      {
        aCamera->SetIOD (aCamera->GetIODType(), aCamera->IOD() + 0.01);
        myView->Redraw();
      }
      break;
    }
    case Aspect_VKey_Delete:
    {
      if (!myCtx.IsNull()
        && myCtx->NbSelected() > 0)
      {
        Draw_Interprete ("verase");
      }
      break;
    }
    case Aspect_VKey_Escape:
    {
      if (!myCtx.IsNull()
        && ViewerTest_EventManager::ToCloseViewOnEscape())
      {
        Draw_Interprete (ViewerTest_EventManager::ToExitOnCloseView() ? "exit" : "vclose");
      }
      break;
    }
    case Aspect_VKey_NavSpeedDecrease:
    case Aspect_VKey_NavSpeedIncrease:
    {
      // handle slide speed
      float aNewSpeed = theKey == Aspect_VKey_NavSpeedDecrease
                      ? myWalkSpeedRelative * 0.5f
                      : myWalkSpeedRelative * 2.0f;
      if (aNewSpeed >= 0.00001f
       && aNewSpeed <= 10.0f)
      {
        if (Abs (aNewSpeed - 0.1f) < 0.001f)
        {
          aNewSpeed = 0.1f;
        }
        myWalkSpeedRelative = aNewSpeed;
      }
      break;
    }
  }

  if (theKey >= Aspect_VKey_0
   && theKey <= Aspect_VKey_7)
  {
    const Standard_Integer aSelMode = theKey - Aspect_VKey_0;
    bool toEnable = true;
    if (!myCtx.IsNull())
    {
      AIS_ListOfInteractive aPrsList;
      myCtx->DisplayedObjects (aPrsList);
      for (AIS_ListOfInteractive::Iterator aPrsIter (aPrsList); aPrsIter.More() && toEnable; aPrsIter.Next())
      {
        TColStd_ListOfInteger aModes;
        myCtx->ActivatedModes (aPrsIter.Value(), aModes);
        for (TColStd_ListOfInteger::Iterator aModeIter (aModes); aModeIter.More() && toEnable; aModeIter.Next())
        {
          if (aModeIter.Value() == aSelMode)
          {
            toEnable = false;
          }
        }
      }
    }
    TCollection_AsciiString aCmd = TCollection_AsciiString ("vselmode ") + aSelMode + (toEnable ? " 1" : " 0");
    Draw_Interprete (aCmd.ToCString());
  }
}

#if defined(__EMSCRIPTEN__)
//! Handle browser window resize event.
static EM_BOOL onResizeCallback (int theEventType, const EmscriptenUiEvent* theEvent, void* )
{
  Handle(ViewerTest_EventManager) aViewCtrl = ViewerTest::CurrentEventManager();
  if (!aViewCtrl.IsNull()
   && !ViewerTest::CurrentView().IsNull())
  {
    Handle(Wasm_Window) aWindow = Handle(Wasm_Window)::DownCast (ViewerTest::CurrentView()->Window());
    return aWindow->ProcessUiEvent (*aViewCtrl, theEventType, theEvent) ? EM_TRUE : EM_FALSE;
  }
  return EM_FALSE;
}

//! Update canvas bounding rectangle.
EM_JS(void, occJSUpdateBoundingClientRect, (), {
  Module._myCanvasRect = Module.canvas.getBoundingClientRect();
});

//! Get canvas bounding top.
EM_JS(int, occJSGetBoundingClientTop, (), {
  return Math.round(Module._myCanvasRect.top);
});

//! Get canvas bounding left.
EM_JS(int, occJSGetBoundingClientLeft, (), {
  return Math.round(Module._myCanvasRect.left);
});

//! Handle mouse input event.
static EM_BOOL onWasmMouseCallback (int theEventType, const EmscriptenMouseEvent* theEvent, void* )
{
  Handle(ViewerTest_EventManager) aViewCtrl = ViewerTest::CurrentEventManager();
  if (!aViewCtrl.IsNull()
   && !ViewerTest::CurrentView().IsNull())
  {
    Handle(Wasm_Window) aWindow = Handle(Wasm_Window)::DownCast (ViewerTest::CurrentView()->Window());
    if (theEventType == EMSCRIPTEN_EVENT_MOUSEMOVE
     || theEventType == EMSCRIPTEN_EVENT_MOUSEUP)
    {
      // these events are bound to EMSCRIPTEN_EVENT_TARGET_WINDOW, and coordinates should be converted
      occJSUpdateBoundingClientRect();
      EmscriptenMouseEvent anEvent = *theEvent;
      anEvent.targetX -= occJSGetBoundingClientLeft();
      anEvent.targetY -= occJSGetBoundingClientTop();
      aWindow->ProcessMouseEvent (*aViewCtrl, theEventType, &anEvent);
      return EM_FALSE;
    }

    return aWindow->ProcessMouseEvent (*aViewCtrl, theEventType, theEvent) ? EM_TRUE : EM_FALSE;
  }
  return EM_FALSE;
}

//! Handle mouse wheel event.
static EM_BOOL onWasmWheelCallback (int theEventType, const EmscriptenWheelEvent* theEvent, void* )
{
  Handle(ViewerTest_EventManager) aViewCtrl = ViewerTest::CurrentEventManager();
  if (!aViewCtrl.IsNull()
   && !ViewerTest::CurrentView().IsNull())
  {
    Handle(Wasm_Window) aWindow = Handle(Wasm_Window)::DownCast (ViewerTest::CurrentView()->Window());
    return aWindow->ProcessWheelEvent (*aViewCtrl, theEventType, theEvent) ? EM_TRUE : EM_FALSE;
  }
  return EM_FALSE;
}

//! Handle touch input event.
static EM_BOOL onWasmTouchCallback (int theEventType, const EmscriptenTouchEvent* theEvent, void* )
{
  Handle(ViewerTest_EventManager) aViewCtrl = ViewerTest::CurrentEventManager();
  if (!aViewCtrl.IsNull()
   && !ViewerTest::CurrentView().IsNull())
  {
    Handle(Wasm_Window) aWindow = Handle(Wasm_Window)::DownCast (ViewerTest::CurrentView()->Window());
    return aWindow->ProcessTouchEvent (*aViewCtrl, theEventType, theEvent) ? EM_TRUE : EM_FALSE;
  }
  return EM_FALSE;
}

//! Handle keyboard input event.
static EM_BOOL onWasmKeyCallback (int theEventType, const EmscriptenKeyboardEvent* theEvent, void* )
{
  Handle(ViewerTest_EventManager) aViewCtrl = ViewerTest::CurrentEventManager();
  if (!aViewCtrl.IsNull()
   && !ViewerTest::CurrentView().IsNull())
  {
    Handle(Wasm_Window) aWindow = Handle(Wasm_Window)::DownCast (ViewerTest::CurrentView()->Window());
    aWindow->ProcessKeyEvent (*aViewCtrl, theEventType, theEvent);
    return EM_TRUE;
  }
  return EM_FALSE;
}

//! Handle focus change event.
static EM_BOOL onWasmFocusCallback (int theEventType, const EmscriptenFocusEvent* theEvent, void*)
{
  Handle(ViewerTest_EventManager) aViewCtrl = ViewerTest::CurrentEventManager();
  if (!aViewCtrl.IsNull()
   && !ViewerTest::CurrentView().IsNull())
  {
    Handle(Wasm_Window) aWindow = Handle(Wasm_Window)::DownCast (ViewerTest::CurrentView()->Window());
    return aWindow->ProcessFocusEvent (*aViewCtrl, theEventType, theEvent) ? EM_TRUE : EM_FALSE;
  }
  return EM_FALSE;
}
#endif

// ==============================================================================
// function : SetupWindowCallbacks
// purpose  :
// ==============================================================================
void ViewerTest_EventManager::SetupWindowCallbacks (const Handle(Aspect_Window)& theWin)
{
#ifdef _WIN32
  (void )theWin;
#elif defined(HAVE_XLIB)
  // X11
  Window anXWin = (Window )theWin->NativeHandle();
  Display* anXDisplay = (Display* )theWin->DisplayConnection()->GetDisplayAspect();
  XSynchronize (anXDisplay, 1);

  // X11 : For keyboard on SUN
  XWMHints aWmHints;
  memset (&aWmHints, 0, sizeof(aWmHints));
  aWmHints.flags = InputHint;
  aWmHints.input = 1;
  XSetWMHints (anXDisplay, anXWin, &aWmHints);

  XSelectInput (anXDisplay, anXWin,
                ExposureMask | KeyPressMask | KeyReleaseMask
              | ButtonPressMask | ButtonReleaseMask
              | StructureNotifyMask
              | PointerMotionMask
              | Button1MotionMask | Button2MotionMask
              | Button3MotionMask | FocusChangeMask);
  Atom aDeleteWindowAtom = theWin->DisplayConnection()->GetAtom (Aspect_XA_DELETE_WINDOW);
  XSetWMProtocols (anXDisplay, anXWin, &aDeleteWindowAtom, 1);

  XSynchronize (anXDisplay, 0);
#elif defined(__EMSCRIPTEN__)
  Handle(Wasm_Window) aWindow = Handle(Wasm_Window)::DownCast (theWin);
  if (aWindow->CanvasId().IsEmpty()
   || aWindow->CanvasId() == "#")
  {
    Message::SendFail ("Error: unable registering callbacks to Module.canvas");
    return;
  }

  const char* aTargetId = aWindow->CanvasId().ToCString();
  const EM_BOOL toUseCapture = EM_TRUE;
  void* anOpaque = NULL; //this; // unused

  // make sure to clear previously set listeners (e.g. created by another ViewerTest_EventManager instance)
  emscripten_html5_remove_all_event_listeners();

  // resize event implemented only for a window by browsers,
  // so that if web application changes canvas size by other means it should use another way to tell OCCT about resize
  emscripten_set_resize_callback     (EMSCRIPTEN_EVENT_TARGET_WINDOW, anOpaque, toUseCapture, onResizeCallback);

  // bind these events to window to track mouse movements outside of canvas
  //emscripten_set_mouseup_callback    (aTargetId, anOpaque, toUseCapture, onWasmMouseCallback);
  //emscripten_set_mousemove_callback  (aTargetId, anOpaque, toUseCapture, onWasmMouseCallback);
  //emscripten_set_mouseleave_callback (aTargetId, anOpaque, toUseCapture, onWasmMouseCallback);
  emscripten_set_mouseup_callback    (EMSCRIPTEN_EVENT_TARGET_WINDOW, anOpaque, toUseCapture, onWasmMouseCallback);
  emscripten_set_mousemove_callback  (EMSCRIPTEN_EVENT_TARGET_WINDOW, anOpaque, toUseCapture, onWasmMouseCallback);

  emscripten_set_mousedown_callback  (aTargetId, anOpaque, toUseCapture, onWasmMouseCallback);
  emscripten_set_dblclick_callback   (aTargetId, anOpaque, toUseCapture, onWasmMouseCallback);
  emscripten_set_click_callback      (aTargetId, anOpaque, toUseCapture, onWasmMouseCallback);
  emscripten_set_mouseenter_callback (aTargetId, anOpaque, toUseCapture, onWasmMouseCallback);
  emscripten_set_wheel_callback      (aTargetId, anOpaque, toUseCapture, onWasmWheelCallback);

  emscripten_set_touchstart_callback (aTargetId, anOpaque, toUseCapture, onWasmTouchCallback);
  emscripten_set_touchend_callback   (aTargetId, anOpaque, toUseCapture, onWasmTouchCallback);
  emscripten_set_touchmove_callback  (aTargetId, anOpaque, toUseCapture, onWasmTouchCallback);
  emscripten_set_touchcancel_callback(aTargetId, anOpaque, toUseCapture, onWasmTouchCallback);

  // keyboard input requires a focusable element or EMSCRIPTEN_EVENT_TARGET_WINDOW
  emscripten_set_keydown_callback    (aTargetId, anOpaque, toUseCapture, onWasmKeyCallback);
  emscripten_set_keyup_callback      (aTargetId, anOpaque, toUseCapture, onWasmKeyCallback);
  //emscripten_set_focus_callback    (aTargetId, anOpaque, toUseCapture, onWasmFocusCallback);
  //emscripten_set_focusin_callback  (aTargetId, anOpaque, toUseCapture, onWasmFocusCallback);
  emscripten_set_focusout_callback   (aTargetId, anOpaque, toUseCapture, onWasmFocusCallback);
#else
  (void )theWin;
#endif
}
