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
#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <Aspect_Grid.hxx>
#include <Draw.hxx>
#include <ViewerTest_ContinuousRedrawer.hxx>
#include <ViewerTest_V3dView.hxx>

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
  myIsTmpContRedraw (Standard_False)
{
  myViewAnimation = GlobalViewAnimation();

  addActionHotKeys (Aspect_VKey_NavForward,        Aspect_VKey_W, Aspect_VKey_W | Aspect_VKeyFlags_SHIFT);
  addActionHotKeys (Aspect_VKey_NavBackward ,      Aspect_VKey_S, Aspect_VKey_S | Aspect_VKeyFlags_SHIFT);
  addActionHotKeys (Aspect_VKey_NavSlideLeft,      Aspect_VKey_A, Aspect_VKey_A | Aspect_VKeyFlags_SHIFT);
  addActionHotKeys (Aspect_VKey_NavSlideRight,     Aspect_VKey_D, Aspect_VKey_D | Aspect_VKeyFlags_SHIFT);
  addActionHotKeys (Aspect_VKey_NavRollCCW,        Aspect_VKey_Q, Aspect_VKey_Q | Aspect_VKeyFlags_SHIFT);
  addActionHotKeys (Aspect_VKey_NavRollCW,         Aspect_VKey_E, Aspect_VKey_E | Aspect_VKeyFlags_SHIFT);

  addActionHotKeys (Aspect_VKey_NavSpeedIncrease,  Aspect_VKey_Plus,  Aspect_VKey_Plus  | Aspect_VKeyFlags_SHIFT,
                                                   Aspect_VKey_Equal,
                                                   Aspect_VKey_NumpadAdd, Aspect_VKey_NumpadAdd | Aspect_VKeyFlags_SHIFT);
  addActionHotKeys (Aspect_VKey_NavSpeedDecrease,  Aspect_VKey_Minus, Aspect_VKey_Minus | Aspect_VKeyFlags_SHIFT,
                                                   Aspect_VKey_NumpadSubtract, Aspect_VKey_NumpadSubtract | Aspect_VKeyFlags_SHIFT);

  addActionHotKeys (Aspect_VKey_NavLookUp,         Aspect_VKey_Up);
  addActionHotKeys (Aspect_VKey_NavLookDown,       Aspect_VKey_Down);
  addActionHotKeys (Aspect_VKey_NavLookLeft,       Aspect_VKey_Left);
  addActionHotKeys (Aspect_VKey_NavLookRight,      Aspect_VKey_Right);
  addActionHotKeys (Aspect_VKey_NavSlideLeft,      Aspect_VKey_Left  | Aspect_VKeyFlags_SHIFT);
  addActionHotKeys (Aspect_VKey_NavSlideRight,     Aspect_VKey_Right | Aspect_VKeyFlags_SHIFT);
  addActionHotKeys (Aspect_VKey_NavSlideUp,        Aspect_VKey_Up    | Aspect_VKeyFlags_SHIFT);
  addActionHotKeys (Aspect_VKey_NavSlideDown,      Aspect_VKey_Down  | Aspect_VKeyFlags_SHIFT);
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

//=======================================================================
//function : UpdateMouseButtons
//purpose  :
//=======================================================================
bool ViewerTest_EventManager::UpdateMouseButtons (const Graphic3d_Vec2i& thePoint,
                                                  Aspect_VKeyMouse theButtons,
                                                  Aspect_VKeyFlags theModifiers,
                                                  bool theIsEmulated)
{
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
    myView->Invalidate();
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
    #ifndef _WIN32
      aRedrawer.Start (theView->Window(), 60.0);
    #endif
    }
  }
  else if (myIsTmpContRedraw)
  {
    myIsTmpContRedraw = false;
  #ifndef _WIN32
    ViewerTest_ContinuousRedrawer& aRedrawer = ViewerTest_ContinuousRedrawer::Instance();
    aRedrawer.Pause();
  #endif
  }
}

//==============================================================================
//function : ProcessConfigure
//purpose  :
//==============================================================================
void ViewerTest_EventManager::ProcessConfigure()
{
  if (!myView.IsNull())
  {
    myView->MustBeResized();
    FlushViewEvents (myCtx, myView, true);
  }
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
    case Aspect_VKey_S | Aspect_VKeyFlags_CTRL:
    case Aspect_VKey_W | Aspect_VKeyFlags_CTRL:
    {
      Standard_Integer aDispMode = AIS_Shaded;
      if (theKey == (Aspect_VKey_S | Aspect_VKeyFlags_CTRL))
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
