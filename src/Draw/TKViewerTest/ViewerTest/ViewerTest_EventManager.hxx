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

#ifndef _ViewerTest_EventManager_HeaderFile
#define _ViewerTest_EventManager_HeaderFile

#include <AIS_ViewController.hxx>
#include <TCollection_AsciiString.hxx>

class AIS_InteractiveContext;
class Aspect_Window;
class V3d_View;

//! used to manage mouse event (move,select)
//! By default the events are transmitted to interactive context.
class ViewerTest_EventManager : public Standard_Transient, public AIS_ViewController
{
  DEFINE_STANDARD_RTTIEXT(ViewerTest_EventManager, Standard_Transient)
public:
  //! Return TRUE if View should be closed on escape.
  static bool& ToCloseViewOnEscape()
  {
    static bool Draw_ToCloseViewOnEsc = false;
    return Draw_ToCloseViewOnEsc;
  }

  //! Return TRUE if Draw Harness should exit on closing View.
  static bool& ToExitOnCloseView()
  {
    static bool Draw_ToExitOnCloseView = false;
    return Draw_ToExitOnCloseView;
  }

  //! Use global camera animation object shared across all Views in ViewerTest.
  Standard_EXPORT static const occ::handle<AIS_AnimationCamera>& GlobalViewAnimation();

public:
  //! Main constructor.
  Standard_EXPORT ViewerTest_EventManager(const occ::handle<V3d_View>&               aView,
                                          const occ::handle<AIS_InteractiveContext>& aCtx);

  //! Destructor.
  Standard_EXPORT virtual ~ViewerTest_EventManager();

  //! Setup or adjust window callbacks.
  Standard_EXPORT static void SetupWindowCallbacks(const occ::handle<Aspect_Window>& theWin);

  //! Return interactive context.
  const occ::handle<AIS_InteractiveContext>& Context() const { return myCtx; }

  //! Returns TRUE if picking point mode has been enabled (for VPick command).
  bool ToPickPoint() const { return myToPickPnt; }

  //! Start picking point for VPick command.
  void StartPickPoint(const char* theArgX, const char* theArgY, const char* theArgZ)
  {
    myToPickPnt        = true;
    myPickPntArgVec[0] = theArgX;
    myPickPntArgVec[1] = theArgY;
    myPickPntArgVec[2] = theArgZ;
  }

  //! Update mouse scroll event.
  Standard_EXPORT virtual bool UpdateMouseScroll(const Aspect_ScrollDelta& theDelta) override;

  //! Handle mouse button click event.
  Standard_EXPORT virtual bool UpdateMouseClick(const NCollection_Vec2<int>& thePoint,
                                                Aspect_VKeyMouse             theButton,
                                                Aspect_VKeyFlags             theModifiers,
                                                bool theIsDoubleClick) override;

  //! Handle mouse button press/release event.
  Standard_EXPORT virtual bool UpdateMouseButtons(const NCollection_Vec2<int>& thePoint,
                                                  Aspect_VKeyMouse             theButtons,
                                                  Aspect_VKeyFlags             theModifiers,
                                                  bool theIsEmulated) override;

  //! Release key.
  Standard_EXPORT virtual void KeyDown(Aspect_VKey theKey,
                                       double      theTime,
                                       double      thePressure = 1.0) override;

  //! Release key.
  Standard_EXPORT virtual void KeyUp(Aspect_VKey theKey, double theTime) override;

  //! Redraw the View on an Expose Event
  Standard_EXPORT virtual void ProcessExpose() override;

  //! Handle redraw.
  Standard_EXPORT virtual void handleViewRedraw(const occ::handle<AIS_InteractiveContext>& theCtx,
                                                const occ::handle<V3d_View>& theView) override;

  //! Resize View.
  Standard_EXPORT virtual void ProcessConfigure(bool theIsResized = true) override;

  //! Handle window input event immediately (flush input buffer).
  Standard_EXPORT virtual void ProcessInput() override;

  //! Handle KeyPress event.
  Standard_EXPORT void ProcessKeyPress(Aspect_VKey theKey);

  //! Callback called on Selection of another (sub)view.
  //! This method is expected to be called from rendering thread.
  Standard_EXPORT virtual void OnSubviewChanged(const occ::handle<AIS_InteractiveContext>& theCtx,
                                                const occ::handle<V3d_View>& theOldView,
                                                const occ::handle<V3d_View>& theNewView) override;

protected:
  //! Register hot-keys for specified Action.
  void addActionHotKeys(Aspect_VKey  theAction,
                        unsigned int theHotKey1 = 0,
                        unsigned int theHotKey2 = 0,
                        unsigned int theHotKey3 = 0,
                        unsigned int theHotKey4 = 0,
                        unsigned int theHotKey5 = 0)
  {
    if (theHotKey1 != 0)
    {
      myNavKeyMap.Bind(theHotKey1, theAction);
    }
    if (theHotKey2 != 0)
    {
      myNavKeyMap.Bind(theHotKey2, theAction);
    }
    if (theHotKey3 != 0)
    {
      myNavKeyMap.Bind(theHotKey3, theAction);
    }
    if (theHotKey4 != 0)
    {
      myNavKeyMap.Bind(theHotKey4, theAction);
    }
    if (theHotKey5 != 0)
    {
      myNavKeyMap.Bind(theHotKey5, theAction);
    }
  }

  //! Handle modifier key changes.
  Standard_EXPORT bool navigationKeyModifierSwitch(unsigned int theModifOld,
                                                   unsigned int theModifNew,
                                                   double       theTimeStamp);

private:
#if defined(__EMSCRIPTEN__)
  //! Callback flushing events and redrawing the WebGL canvas.
  static void onWasmRedrawView(void*);
#endif

private:
  occ::handle<AIS_InteractiveContext> myCtx;
  occ::handle<V3d_View>               myView;
  // clang-format off
  NCollection_DataMap<unsigned int, Aspect_VKey> myNavKeyMap; //!< map of Hot-Key (key+modifiers) to Action
  // clang-format on

  TCollection_AsciiString myPickPntArgVec[3];
  bool                    myToPickPnt;
  bool                    myIsTmpContRedraw;

  unsigned int myNbUpdateRequests; //!< counter for unhandled update requests
};

#endif // _ViewerTest_EventManager_HeaderFile
