// Copyright (c) 2016-2019 OPEN CASCADE SAS
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

#ifndef _Aspect_VKeySet_HeaderFile
#define _Aspect_VKeySet_HeaderFile

#include <Aspect_VKey.hxx>

#include <NCollection_Array1.hxx>
#include <OSD_Timer.hxx>
#include <Standard_Mutex.hxx>
#include <Standard_Transient.hxx>

//! Structure defining key state.
class Aspect_VKeySet : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Aspect_VKeySet, Standard_Transient)
public:

  //! Main constructor.
  Standard_EXPORT Aspect_VKeySet();

  //! Return active modifiers.
  Aspect_VKeyFlags Modifiers() const
  {
    Standard_Mutex::Sentry aLock (myLock);
    return myModifiers;
  }

  //! Return timestamp of press event.
  double DownTime (Aspect_VKey theKey) const
  {
    Standard_Mutex::Sentry aLock (myLock);
    return myKeys[theKey].TimeDown;
  }

  //! Return timestamp of release event.
  double TimeUp (Aspect_VKey theKey) const
  {
    Standard_Mutex::Sentry aLock (myLock);
    return myKeys[theKey].TimeUp;
  }

  //! Return TRUE if key is in Free state.
  bool IsFreeKey (Aspect_VKey theKey) const
  {
    Standard_Mutex::Sentry aLock (myLock);
    return myKeys[theKey].KStatus == KeyStatus_Free;
  }

  //! Return TRUE if key is in Pressed state.
  bool IsKeyDown (Aspect_VKey theKey) const
  {
    Standard_Mutex::Sentry aLock (myLock);
    return myKeys[theKey].KStatus == KeyStatus_Pressed;
  }

  //! Return mutex for thread-safe updates.
  //! All operations in class implicitly locks this mutex,
  //! so this method could be used only for batch processing of keys.
  Standard_Mutex& Mutex() { return myLock; }

public:

  //! Reset the key state into unpressed state.
  Standard_EXPORT void Reset();

  //! Press key.
  //! @param theKey key pressed
  //! @param theTime event timestamp
  Standard_EXPORT void KeyDown (Aspect_VKey theKey,
                                double theTime,
                                double thePressure = 1.0);

  //! Release key.
  //! @param theKey key pressed
  //! @param theTime event timestamp
  Standard_EXPORT void KeyUp (Aspect_VKey theKey,
                              double theTime);

  //! Simulate key up/down events from axis value.
  Standard_EXPORT void KeyFromAxis (Aspect_VKey theNegative,
                                    Aspect_VKey thePositive,
                                    double theTime,
                                    double thePressure);

  //! Return duration of the button in pressed state.
  //! @param theKey      key to check
  //! @param theTime     current time (for computing duration from key down time)
  //! @param theDuration key press duration
  //! @return TRUE if key was in pressed state
  bool HoldDuration (Aspect_VKey theKey,
                     double theTime,
                     double& theDuration)
  {
    double aPressure = -1.0;
    return HoldDuration (theKey, theTime, theDuration, aPressure);
  }

  //! Return duration of the button in pressed state.
  //! @param theKey      key to check
  //! @param theTime     current time (for computing duration from key down time)
  //! @param theDuration key press duration
  //! @param thePressure key pressure
  //! @return TRUE if key was in pressed state
  Standard_EXPORT bool HoldDuration (Aspect_VKey theKey,
                                     double theTime,
                                     double& theDuration,
                                     double& thePressure);

private:

  //! Key state.
  enum KeyStatus
  {
    KeyStatus_Free,     //!< free status
    KeyStatus_Pressed,  //!< key is in pressed state
    KeyStatus_Released, //!< key has been just released (transient state before KeyStatus_Free)
  };

  //! Structure defining key state.
  struct KeyState
  {
    KeyState() : TimeDown (0.0), TimeUp (0.0), Pressure (1.0), KStatus (KeyStatus_Free) {}
    void Reset()
    {
      KStatus = KeyStatus_Free;
      TimeDown = 0.0;
      TimeUp   = 0.0;
      Pressure = 1.0;
    }

    double    TimeDown; //!< time of key press   event
    double    TimeUp;   //!< time of key release event
    double    Pressure; //!< key pressure
    KeyStatus KStatus;  //!< key status
  };

private:

  NCollection_Array1<KeyState> myKeys;      //!< keys state
  mutable Standard_Mutex       myLock;      //!< mutex for thread-safe updates
  Aspect_VKeyFlags             myModifiers; //!< active modifiers

};

#endif // _Aspect_VKeySet_HeaderFile
