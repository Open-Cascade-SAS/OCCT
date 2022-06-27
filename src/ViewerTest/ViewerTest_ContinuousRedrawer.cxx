// Copyright (c) 2019-2020 OPEN CASCADE SAS
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

#include <ViewerTest_ContinuousRedrawer.hxx>

#include <Aspect_DisplayConnection.hxx>
#include <OSD.hxx>
#include <OSD_Timer.hxx>
#include <V3d_View.hxx>

// =======================================================================
// function : Instance
// purpose  :
// =======================================================================
ViewerTest_ContinuousRedrawer& ViewerTest_ContinuousRedrawer::Instance()
{
  static ViewerTest_ContinuousRedrawer aRedrawer;
  return aRedrawer;
}

// =======================================================================
// function : ViewerTest_ContinuousRedrawer
// purpose  :
// =======================================================================
ViewerTest_ContinuousRedrawer::ViewerTest_ContinuousRedrawer()
: myThread (doThreadWrapper),
  myWakeEvent (false),
  myTargetFps (0.0),
  myToStop (false),
  myToPause (false)
{
  //
}

// =======================================================================
// function : ~ViewerTest_ContinuousRedrawer
// purpose  :
// =======================================================================
ViewerTest_ContinuousRedrawer::~ViewerTest_ContinuousRedrawer()
{
  Stop();
}

// =======================================================================
// function : Start
// purpose  :
// =======================================================================
void ViewerTest_ContinuousRedrawer::Start (const Handle(V3d_View)& theView,
                                           Standard_Real theTargetFps)
{
  if (myView != theView
   || myTargetFps != theTargetFps)
  {
    Stop();
    myView = theView;
    myTargetFps = theTargetFps;
  }

  if (myThread.GetId() == 0)
  {
    myToStop = false;
    myToPause = false;
    myThread.Run (this);
  }
  else
  {
    {
      Standard_Mutex::Sentry aLock (myMutex);
      myToStop = false;
      myToPause = false;
    }
    myWakeEvent.Set();
  }
}

// =======================================================================
// function : Stop
// purpose  :
// =======================================================================
void ViewerTest_ContinuousRedrawer::Stop (const Handle(V3d_View)& theView)
{
  if (!theView.IsNull()
    && myView != theView)
  {
    return;
  }

  {
    Standard_Mutex::Sentry aLock (myMutex);
    myToStop = true;
    myToPause = false;
  }
  myWakeEvent.Set();
  myThread.Wait();
  myToStop = false;
  myView.Nullify();
}

// =======================================================================
// function : Pause
// purpose  :
// =======================================================================
void ViewerTest_ContinuousRedrawer::Pause()
{
  if (!myToPause)
  {
    Standard_Mutex::Sentry aLock (myMutex);
    myToPause = true;
  }
}

// =======================================================================
// function : doThreadLoop
// purpose  :
// =======================================================================
void ViewerTest_ContinuousRedrawer::doThreadLoop()
{
  Handle(Aspect_DisplayConnection) aDisp = new Aspect_DisplayConnection();
  OSD_Timer aTimer;
  aTimer.Start();
  Standard_Real aTimeOld = 0.0;
  const Standard_Real aTargetDur = myTargetFps > 0.0 ? 1.0 / myTargetFps : -1.0;
  for (;;)
  {
    bool toPause = false;
    {
      Standard_Mutex::Sentry aLock (myMutex);
      if (myToStop)
      {
        return;
      }
      toPause = myToPause;
    }
    if (toPause)
    {
      myWakeEvent.Wait();
      myWakeEvent.Reset();
    }

    if (myTargetFps > 0.0)
    {
      const Standard_Real aTimeNew  = aTimer.ElapsedTime();
      const Standard_Real aDuration = aTimeNew - aTimeOld;
      if (aDuration >= aTargetDur)
      {
        myView->Invalidate();
        myView->Window()->InvalidateContent (aDisp);
        aTimeOld = aTimeNew;
      }
    }
    else
    {
      myView->Invalidate();
      myView->Window()->InvalidateContent (aDisp);
    }

    OSD::MilliSecSleep (1);
  }
}
