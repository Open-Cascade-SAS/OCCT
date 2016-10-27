// Created by: Kirill Gavrilov
// Copyright (c) 2016 OPEN CASCADE SAS
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

#include <AIS_AnimationTimer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_AnimationTimer, Standard_Transient)

//=============================================================================
//function : Pause
//purpose  :
//=============================================================================
void AIS_AnimationTimer::Pause()
{
  myTimer.Stop();
  myTimerFrom += myTimer.ElapsedTime() * myTimerSpeed;
  myTimer.Reset();
}

//=============================================================================
//function : Stop
//purpose  :
//=============================================================================
void AIS_AnimationTimer::Stop()
{
  myTimer.Stop();
  myTimer.Reset();
  myTimerFrom = 0.0;
}

//=============================================================================
//function : SetPlaybackSpeed
//purpose  :
//=============================================================================
void AIS_AnimationTimer::SetPlaybackSpeed (const Standard_Real theSpeed)
{
  if (!myTimer.IsStarted())
  {
    myTimerSpeed = theSpeed;
    return;
  }

  myTimer.Stop();
  myTimerFrom += myTimer.ElapsedTime() * myTimerSpeed;
  myTimer.Reset();
  myTimerSpeed = theSpeed;
  myTimer.Start();
}

//=============================================================================
//function : SetPlaybackSpeed
//purpose  :
//=============================================================================
void AIS_AnimationTimer::Seek (const Standard_Real theTime)
{
  const Standard_Boolean isStarted = myTimer.IsStarted();
  myTimer.Stop();
  myTimer.Reset();
  myTimerFrom = theTime;
  if (isStarted)
  {
    myTimer.Start();
  }
}
