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

#ifndef _Media_Timer_HeaderFile
#define _Media_Timer_HeaderFile

#include <OSD_Timer.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>

//! Auxiliary class defining the animation timer.
class Media_Timer : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Media_Timer, Standard_Transient)
public:

  //! Empty constructor.
  Media_Timer() : myTimerFrom (0.0), myTimerSpeed (1.0) {}

  //! Return elapsed time in seconds.
  Standard_Real ElapsedTime() const
  {
    return myTimerFrom + myTimer.ElapsedTime() * myTimerSpeed;
  }

  //! Return playback speed coefficient (1.0 means normal speed).
  Standard_Real PlaybackSpeed() const { return myTimerSpeed; }

  //! Setup playback speed coefficient.
  Standard_EXPORT void SetPlaybackSpeed (const Standard_Real theSpeed);

  //! Return true if timer has been started.
  Standard_Boolean IsStarted() const
  {
    return myTimer.IsStarted();
  }

  //! Start the timer.
  void Start()
  {
    myTimer.Start();
  }

  //! Pause the timer.
  Standard_EXPORT void Pause();

  //! Stop the timer.
  Standard_EXPORT void Stop();

  //! Seek the timer to specified position.
  Standard_EXPORT void Seek (const Standard_Real theTime);

protected:

  OSD_Timer     myTimer;
  Standard_Real myTimerFrom;
  Standard_Real myTimerSpeed;

};

DEFINE_STANDARD_HANDLE(Media_Timer, Standard_Transient)

#endif // _Media_Timer_HeaderFile
