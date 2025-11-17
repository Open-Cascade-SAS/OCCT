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

#include "Aspect_VKeySet.hxx"

IMPLEMENT_STANDARD_RTTIEXT(Aspect_VKeySet, Standard_Transient)

//=================================================================================================

Aspect_VKeySet::Aspect_VKeySet()
    : myKeys(0, Aspect_VKey_MAX),
      myModifiers(Aspect_VKeyFlags_NONE)
{
  //
}

//=================================================================================================

void Aspect_VKeySet::Reset()
{
  std::lock_guard<std::shared_mutex> aLock(myLock);
  myModifiers = 0;
  for (NCollection_Array1<KeyState>::Iterator aKeyIter(myKeys); aKeyIter.More(); aKeyIter.Next())
  {
    aKeyIter.ChangeValue().Reset();
  }
}

//=================================================================================================

void Aspect_VKeySet::KeyDown_Unlocked(Aspect_VKey theKey, double theTime, double thePressure)
{
  if (myKeys[theKey].KStatus != KeyStatus_Pressed)
  {
    myKeys[theKey].KStatus  = KeyStatus_Pressed;
    myKeys[theKey].TimeDown = theTime;
  }
  myKeys[theKey].Pressure = thePressure;

  const unsigned int aModif = Aspect_VKey2Modifier(theKey);
  myModifiers               = myModifiers | aModif;
}

//=================================================================================================

void Aspect_VKeySet::KeyDown(Aspect_VKey theKey, double theTime, double thePressure)
{
  std::lock_guard<std::shared_mutex> aLock(myLock);
  KeyDown_Unlocked(theKey, theTime, thePressure);
}

//=================================================================================================

void Aspect_VKeySet::KeyUp_Unlocked(Aspect_VKey theKey, double theTime)
{
  if (myKeys[theKey].KStatus == KeyStatus_Pressed)
  {
    myKeys[theKey].KStatus = KeyStatus_Released;
    myKeys[theKey].TimeUp  = theTime;
  }

  const unsigned int aModif = Aspect_VKey2Modifier(theKey);
  if (aModif != 0)
  {
    myModifiers = myModifiers & ~aModif;
  }
}

//=================================================================================================

void Aspect_VKeySet::KeyUp(Aspect_VKey theKey, double theTime)
{
  std::lock_guard<std::shared_mutex> aLock(myLock);
  KeyUp_Unlocked(theKey, theTime);
}

//=================================================================================================

void Aspect_VKeySet::KeyFromAxis(Aspect_VKey theNegative,
                                 Aspect_VKey thePositive,
                                 double      theTime,
                                 double      thePressure)
{
  std::lock_guard<std::shared_mutex> aLock(myLock);
  if (thePressure != 0)
  {
    const Aspect_VKey aKeyDown = thePressure > 0 ? thePositive : theNegative;
    const Aspect_VKey aKeyUp   = thePressure < 0 ? thePositive : theNegative;

    KeyDown_Unlocked(aKeyDown, theTime, std::abs(thePressure));
    if (myKeys[aKeyUp].KStatus == KeyStatus_Pressed)
    {
      KeyUp_Unlocked(aKeyUp, theTime);
    }
  }
  else
  {
    if (myKeys[theNegative].KStatus == KeyStatus_Pressed)
    {
      KeyUp_Unlocked(theNegative, theTime);
    }
    if (myKeys[thePositive].KStatus == KeyStatus_Pressed)
    {
      KeyUp_Unlocked(thePositive, theTime);
    }
  }
}

//=================================================================================================

bool Aspect_VKeySet::HoldDuration(Aspect_VKey theKey,
                                  double      theTime,
                                  double&     theDuration,
                                  double&     thePressure)
{
  std::lock_guard<std::shared_mutex> aLock(myLock);
  switch (myKeys[theKey].KStatus)
  {
    case KeyStatus_Free: {
      theDuration = 0.0;
      return false;
    }
    case KeyStatus_Released: {
      myKeys[theKey].KStatus = KeyStatus_Free;
      theDuration            = myKeys[theKey].TimeUp - myKeys[theKey].TimeDown;
      thePressure            = myKeys[theKey].Pressure;
      return true;
    }
    case KeyStatus_Pressed: {
      theDuration = theTime - myKeys[theKey].TimeDown;
      thePressure = myKeys[theKey].Pressure;
      return true;
    }
  }
  return false;
}
