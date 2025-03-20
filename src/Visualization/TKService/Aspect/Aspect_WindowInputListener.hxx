// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef _Aspect_WindowInputListener_HeaderFile
#define _Aspect_WindowInputListener_HeaderFile

#include <Aspect_VKeySet.hxx>
#include <Aspect_TouchMap.hxx>
#include <Graphic3d_Vec.hxx>

struct Aspect_ScrollDelta;
class WNT_HIDSpaceMouse;

//! Defines a listener for window input events.
class Aspect_WindowInputListener
{
public:
  /// DEFINE_STANDARD_ALLOC
public:
  //! Destructor.
  Standard_EXPORT virtual ~Aspect_WindowInputListener();

  //! Return event time (e.g. current time).
  double EventTime() const { return myEventTimer.ElapsedTime(); }

  //! Handle expose event (window content has been invalidation and should be redrawn).
  virtual void ProcessExpose() = 0;

  //! Handle window resize event.
  virtual void ProcessConfigure(bool theIsResized) = 0;

  //! Handle window input event immediately (flush input buffer or ignore).
  virtual void ProcessInput() = 0;

  //! Handle focus event.
  virtual void ProcessFocus(bool theIsActivated) = 0;

  //! Handle window close event.
  virtual void ProcessClose() = 0;

public: //! @name keyboard input
  //! Return keyboard state.
  const Aspect_VKeySet& Keys() const { return myKeys; }

  //! Return keyboard state.
  Aspect_VKeySet& ChangeKeys() { return myKeys; }

  //! Press key.
  //! Default implementation updates internal cache.
  //! @param theKey key pressed
  //! @param theTime event timestamp
  Standard_EXPORT virtual void KeyDown(Aspect_VKey theKey,
                                       double      theTime,
                                       double      thePressure = 1.0) = 0;

  //! Release key.
  //! Default implementation updates internal cache.
  //! @param theKey key pressed
  //! @param theTime event timestamp
  Standard_EXPORT virtual void KeyUp(Aspect_VKey theKey, double theTime) = 0;

  //! Simulate key up/down events from axis value.
  //! Default implementation updates internal cache.
  Standard_EXPORT virtual void KeyFromAxis(Aspect_VKey theNegative,
                                           Aspect_VKey thePositive,
                                           double      theTime,
                                           double      thePressure) = 0;

public: //! @name mouse input
  //! Update mouse scroll event.
  //! This method is expected to be called from UI thread.
  //! @param theDelta mouse cursor position and delta
  //! @return TRUE if new event has been created or FALSE if existing one has been updated
  virtual bool UpdateMouseScroll(const Aspect_ScrollDelta& theDelta) = 0;

  //! Handle mouse button press/release event.
  //! This method is expected to be called from UI thread.
  //! @param thePoint      mouse cursor position
  //! @param theButtons    pressed buttons
  //! @param theModifiers  key modifiers
  //! @param theIsEmulated if TRUE then mouse event comes NOT from real mouse
  //!                      but emulated from non-precise input like touch on screen
  //! @return TRUE if window content should be redrawn
  virtual bool UpdateMouseButtons(const Graphic3d_Vec2i& thePoint,
                                  Aspect_VKeyMouse       theButtons,
                                  Aspect_VKeyFlags       theModifiers,
                                  bool                   theIsEmulated) = 0;

  //! Handle mouse cursor movement event.
  //! This method is expected to be called from UI thread.
  //! Default implementation does nothing.
  //! @param thePoint      mouse cursor position
  //! @param theButtons    pressed buttons
  //! @param theModifiers  key modifiers
  //! @param theIsEmulated if TRUE then mouse event comes NOT from real mouse
  //!                      but emulated from non-precise input like touch on screen
  //! @return TRUE if window content should be redrawn
  virtual bool UpdateMousePosition(const Graphic3d_Vec2i& thePoint,
                                   Aspect_VKeyMouse       theButtons,
                                   Aspect_VKeyFlags       theModifiers,
                                   bool                   theIsEmulated) = 0;

  //! Handle mouse button press event.
  //! This method is expected to be called from UI thread.
  //! Default implementation redirects to UpdateMousePosition().
  //! @param thePoint      mouse cursor position
  //! @param theButton     pressed button
  //! @param theModifiers  key modifiers
  //! @param theIsEmulated if TRUE then mouse event comes NOT from real mouse
  //!                      but emulated from non-precise input like touch on screen
  //! @return TRUE if window content should be redrawn
  bool PressMouseButton(const Graphic3d_Vec2i& thePoint,
                        Aspect_VKeyMouse       theButton,
                        Aspect_VKeyFlags       theModifiers,
                        bool                   theIsEmulated)
  {
    return UpdateMouseButtons(thePoint, myMousePressed | theButton, theModifiers, theIsEmulated);
  }

  //! Handle mouse button release event.
  //! This method is expected to be called from UI thread.
  //! Default implementation redirects to UpdateMousePosition().
  //! @param thePoint      mouse cursor position
  //! @param theButton     released button
  //! @param theModifiers  key modifiers
  //! @param theIsEmulated if TRUE then mouse event comes NOT from real mouse
  //!                      but emulated from non-precise input like touch on screen
  //! @return TRUE if window content should be redrawn
  bool ReleaseMouseButton(const Graphic3d_Vec2i& thePoint,
                          Aspect_VKeyMouse       theButton,
                          Aspect_VKeyFlags       theModifiers,
                          bool                   theIsEmulated)
  {
    Aspect_VKeyMouse aButtons = myMousePressed & (~theButton);
    return UpdateMouseButtons(thePoint, aButtons, theModifiers, theIsEmulated);
  }

  //! Return currently pressed mouse buttons.
  Aspect_VKeyMouse PressedMouseButtons() const { return myMousePressed; }

  //! Return active key modifiers passed with last mouse event.
  Aspect_VKeyFlags LastMouseFlags() const { return myMouseModifiers; }

  //! Return last mouse position.
  const Graphic3d_Vec2i& LastMousePosition() const { return myMousePositionLast; }

public: //! @name multi-touch input
  //! Return TRUE if touches map is not empty.
  bool HasTouchPoints() const { return !myTouchPoints.IsEmpty(); }

  //! Return map of active touches.
  const Aspect_TouchMap& TouchPoints() const { return myTouchPoints; }

  //! Add touch point with the given ID.
  //! This method is expected to be called from UI thread.
  //! @param theId touch unique identifier
  //! @param thePnt touch coordinates
  //! @param theClearBefore if TRUE previously registered touches will be removed
  Standard_EXPORT virtual void AddTouchPoint(Standard_Size          theId,
                                             const Graphic3d_Vec2d& thePnt,
                                             Standard_Boolean       theClearBefore = false);

  //! Remove touch point with the given ID.
  //! This method is expected to be called from UI thread.
  //! @param theId touch unique identifier
  //! @param theClearSelectPnts if TRUE will initiate clearing of selection points
  //! @return TRUE if point has been removed
  Standard_EXPORT virtual bool RemoveTouchPoint(Standard_Size    theId,
                                                Standard_Boolean theClearSelectPnts = false);

  //! Update touch point with the given ID.
  //! If point with specified ID was not registered before, it will be added.
  //! This method is expected to be called from UI thread.
  //! @param theId touch unique identifier
  //! @param thePnt touch coordinates
  Standard_EXPORT virtual void UpdateTouchPoint(Standard_Size theId, const Graphic3d_Vec2d& thePnt);

public: //! @name 3d mouse input
  //! Return acceleration ratio for translation event; 2.0 by default.
  float Get3dMouseTranslationScale() const { return my3dMouseAccelTrans; }

  //! Set acceleration ratio for translation event.
  void Set3dMouseTranslationScale(float theScale) { my3dMouseAccelTrans = theScale; }

  //! Return acceleration ratio for rotation event; 4.0 by default.
  float Get3dMouseRotationScale() const { return my3dMouseAccelRotate; }

  //! Set acceleration ratio for rotation event.
  void Set3dMouseRotationScale(float theScale) { my3dMouseAccelRotate = theScale; }

  //! Return quadric acceleration flag; TRUE by default.
  bool To3dMousePreciseInput() const { return my3dMouseIsQuadric; }

  //! Set quadric acceleration flag.
  void Set3dMousePreciseInput(bool theIsQuadric) { my3dMouseIsQuadric = theIsQuadric; }

  //! Return 3d mouse rotation axes (tilt/roll/spin) ignore flag; (FALSE, FALSE, FALSE) by default.
  const NCollection_Vec3<bool>& Get3dMouseIsNoRotate() const { return my3dMouseNoRotate; }

  //! Return 3d mouse rotation axes (tilt/roll/spin) ignore flag; (FALSE, FALSE, FALSE) by default.
  NCollection_Vec3<bool>& Change3dMouseIsNoRotate() { return my3dMouseNoRotate; }

  //! Return 3d mouse rotation axes (tilt/roll/spin) reverse flag; (TRUE, FALSE, FALSE) by default.
  const NCollection_Vec3<bool>& Get3dMouseToReverse() const { return my3dMouseToReverse; }

  //! Return 3d mouse rotation axes (tilt/roll/spin) reverse flag; (TRUE, FALSE, FALSE) by default.
  NCollection_Vec3<bool>& Change3dMouseToReverse() { return my3dMouseToReverse; }

  //! Process 3d mouse input event (redirects to translation, rotation and keys).
  virtual bool Update3dMouse(const WNT_HIDSpaceMouse& theEvent) = 0;

  //! Process 3d mouse input translation event.
  Standard_EXPORT virtual bool update3dMouseTranslation(const WNT_HIDSpaceMouse& theEvent);

  //! Process 3d mouse input rotation event.
  Standard_EXPORT virtual bool update3dMouseRotation(const WNT_HIDSpaceMouse& theEvent);

  //! Process 3d mouse input keys event.
  Standard_EXPORT virtual bool update3dMouseKeys(const WNT_HIDSpaceMouse& theEvent);

protected:
  //! Empty constructor.
  Standard_EXPORT Aspect_WindowInputListener();

protected:
  OSD_Timer myEventTimer; //!< timer for timestamping events

protected:               //! @name keyboard input variables
  Aspect_VKeySet myKeys; //!< keyboard state

protected:                              //! @name mouse input variables
  Graphic3d_Vec2i  myMousePositionLast; //!< last mouse position
  Aspect_VKeyMouse myMousePressed;      //!< active mouse buttons
  Aspect_VKeyFlags myMouseModifiers;    //!< active key modifiers passed with last mouse event

protected:
  Aspect_TouchMap myTouchPoints; //!< map of active touches

protected:                                         //! @name 3d mouse input variables
  bool                   my3dMouseButtonState[32]; //!< cached button state
  NCollection_Vec3<bool> my3dMouseNoRotate;        //!< ignore  3d mouse rotation axes
  NCollection_Vec3<bool> my3dMouseToReverse;       //!< reverse 3d mouse rotation axes
  float                  my3dMouseAccelTrans;      //!< acceleration ratio for translation event
  float                  my3dMouseAccelRotate;     //!< acceleration ratio for rotation event
  bool                   my3dMouseIsQuadric;       //!< quadric acceleration
};

#endif // _Aspect_WindowInputListener_HeaderFile
