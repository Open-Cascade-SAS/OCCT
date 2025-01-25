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

#include <Media_Timer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Media_Timer, Standard_Transient)

//=================================================================================================

void Media_Timer::Pause()
{
  myTimer.Stop();
  myTimerFrom += myTimer.ElapsedTime() * myTimerSpeed;
  myTimer.Reset();
}

//=================================================================================================

void Media_Timer::Stop()
{
  myTimer.Stop();
  myTimer.Reset();
  myTimerFrom = 0.0;
}

//=================================================================================================

void Media_Timer::SetPlaybackSpeed(const Standard_Real theSpeed)
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

//=================================================================================================

void Media_Timer::Seek(const Standard_Real theTime)
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
