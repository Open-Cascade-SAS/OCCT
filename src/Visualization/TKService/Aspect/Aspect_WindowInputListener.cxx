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

#include <Aspect_WindowInputListener.hxx>

#include <WNT_HIDSpaceMouse.hxx>

//=================================================================================================

Aspect_WindowInputListener::Aspect_WindowInputListener()
    : myMousePressed(Aspect_VKeyMouse_NONE),
      myMouseModifiers(Aspect_VKeyFlags_NONE),
      //
      my3dMouseNoRotate(false, false, false),
      my3dMouseToReverse(true, false, false),
      my3dMouseAccelTrans(2.0f),
      my3dMouseAccelRotate(4.0f),
      my3dMouseIsQuadric(true)
{
  memset(my3dMouseButtonState, 0, sizeof(my3dMouseButtonState));
  myEventTimer.Start();
}

//=================================================================================================

Aspect_WindowInputListener::~Aspect_WindowInputListener()
{
  //
}

//=================================================================================================

void Aspect_WindowInputListener::KeyDown(Aspect_VKey theKey, double theTime, double thePressure)
{
  myKeys.KeyDown(theKey, theTime, thePressure);
}

//=================================================================================================

void Aspect_WindowInputListener::KeyUp(Aspect_VKey theKey, double theTime)
{
  myKeys.KeyUp(theKey, theTime);
}

//=================================================================================================

void Aspect_WindowInputListener::KeyFromAxis(Aspect_VKey theNegative,
                                             Aspect_VKey thePositive,
                                             double      theTime,
                                             double      thePressure)
{
  myKeys.KeyFromAxis(theNegative, thePositive, theTime, thePressure);
}

//=================================================================================================

void Aspect_WindowInputListener::AddTouchPoint(size_t          theId,
                                               const NCollection_Vec2<double>& thePnt,
                                               bool       theClearBefore)
{
  if (theClearBefore)
  {
    RemoveTouchPoint((size_t)-1);
  }

  myTouchPoints.Add(theId, Aspect_Touch(thePnt, false));
}

//=================================================================================================

bool Aspect_WindowInputListener::RemoveTouchPoint(size_t    theId,
                                                  bool theClearSelectPnts)
{
  (void)theClearSelectPnts;
  if (theId == (size_t)-1)
  {
    myTouchPoints.Clear(false);
  }
  else
  {
    const int anOldExtent = myTouchPoints.Extent();
    myTouchPoints.RemoveKey(theId);
    if (myTouchPoints.Extent() == anOldExtent)
    {
      return false;
    }
  }

  if (myTouchPoints.Extent() == 1)
  {
    // avoid incorrect transition from pinch to one finger
    Aspect_Touch& aFirstTouch = myTouchPoints.ChangeFromIndex(1);
    aFirstTouch.To            = aFirstTouch.From;
  }
  return true;
}

//=================================================================================================

void Aspect_WindowInputListener::UpdateTouchPoint(size_t          theId,
                                                  const NCollection_Vec2<double>& thePnt)
{
  if (Aspect_Touch* aTouch = myTouchPoints.ChangeSeek(theId))
  {
    aTouch->To = thePnt;
  }
  else
  {
    AddTouchPoint(theId, thePnt);
  }
}

//=================================================================================================

bool Aspect_WindowInputListener::update3dMouseTranslation(const WNT_HIDSpaceMouse& theEvent)
{
  if (!theEvent.IsTranslation())
  {
    return false;
  }

  bool                  isIdle     = true;
  const double          aTimeStamp = EventTime();
  const NCollection_Vec3<double> aTrans =
    theEvent.Translation(isIdle, my3dMouseIsQuadric) * my3dMouseAccelTrans;
  myKeys.KeyFromAxis(Aspect_VKey_NavSlideLeft, Aspect_VKey_NavSlideRight, aTimeStamp, aTrans.x());
  myKeys.KeyFromAxis(Aspect_VKey_NavForward, Aspect_VKey_NavBackward, aTimeStamp, aTrans.y());
  myKeys.KeyFromAxis(Aspect_VKey_NavSlideUp, Aspect_VKey_NavSlideDown, aTimeStamp, aTrans.z());
  return true;
}

//=================================================================================================

bool Aspect_WindowInputListener::update3dMouseRotation(const WNT_HIDSpaceMouse& theEvent)
{
  if (!theEvent.IsRotation())
  {
    return false;
  }

  bool                  isIdle = true, toUpdate = false;
  const double          aTimeStamp = EventTime();
  const NCollection_Vec3<double> aRot3 =
    theEvent.Rotation(isIdle, my3dMouseIsQuadric) * my3dMouseAccelRotate;
  if (!my3dMouseNoRotate.x())
  {
    KeyFromAxis(Aspect_VKey_NavLookUp,
                Aspect_VKey_NavLookDown,
                aTimeStamp,
                !my3dMouseToReverse.x() ? aRot3.x() : -aRot3.x());
    toUpdate = true;
  }
  if (!my3dMouseNoRotate.y())
  {
    KeyFromAxis(Aspect_VKey_NavRollCW,
                Aspect_VKey_NavRollCCW,
                aTimeStamp,
                !my3dMouseToReverse.y() ? aRot3.y() : -aRot3.y());
    toUpdate = true;
  }
  if (!my3dMouseNoRotate.z())
  {
    KeyFromAxis(Aspect_VKey_NavLookLeft,
                Aspect_VKey_NavLookRight,
                aTimeStamp,
                !my3dMouseToReverse.z() ? aRot3.z() : -aRot3.z());
    toUpdate = true;
  }
  return toUpdate;
}

//=================================================================================================

bool Aspect_WindowInputListener::update3dMouseKeys(const WNT_HIDSpaceMouse& theEvent)
{
  bool         toUpdate   = false;
  const double aTimeStamp = EventTime();
  if (theEvent.IsKeyState())
  {
    const uint32_t aKeyState = theEvent.KeyState();
    for (unsigned short aKeyBit = 0; aKeyBit < 32; ++aKeyBit)
    {
      const bool isPressed  = (aKeyState & (1 << aKeyBit)) != 0;
      const bool isReleased = my3dMouseButtonState[aKeyBit] && !isPressed;
      // const bool isRepeated = my3dMouseButtonState[aKeyBit] &&  isPressed;
      my3dMouseButtonState[aKeyBit] = isPressed;
      if (!isReleased && !isPressed)
      {
        continue;
      }

      const Aspect_VKey aVKey = theEvent.HidToSpaceKey(aKeyBit);
      if (aVKey != Aspect_VKey_UNKNOWN)
      {
        toUpdate = true;
        if (isPressed)
        {
          KeyDown(aVKey, aTimeStamp);
        }
        else
        {
          KeyUp(aVKey, aTimeStamp);
        }
      }
    }
  }
  return toUpdate;
}
