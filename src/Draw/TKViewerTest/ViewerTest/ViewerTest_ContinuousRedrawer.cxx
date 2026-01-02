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

//=================================================================================================

ViewerTest_ContinuousRedrawer& ViewerTest_ContinuousRedrawer::Instance()
{
  static ViewerTest_ContinuousRedrawer aRedrawer;
  return aRedrawer;
}

//=================================================================================================

ViewerTest_ContinuousRedrawer::ViewerTest_ContinuousRedrawer()
    : myThread(doThreadWrapper),
      myWakeEvent(false),
      myTargetFps(0.0),
      myToStop(false),
      myToPause(false)
{}

//=================================================================================================

ViewerTest_ContinuousRedrawer::~ViewerTest_ContinuousRedrawer()
{
  Stop();
}

//=================================================================================================

void ViewerTest_ContinuousRedrawer::Start(const occ::handle<V3d_View>& theView, double theTargetFps)
{
  if (myView != theView || myTargetFps != theTargetFps)
  {
    Stop();
    myView      = theView;
    myTargetFps = theTargetFps;
  }

  if (myThread.GetId() == 0)
  {
    myToStop  = false;
    myToPause = false;
    myThread.Run(this);
  }
  else
  {
    {
      std::lock_guard<std::mutex> aLock(myMutex);
      myToStop  = false;
      myToPause = false;
    }
    myWakeEvent.Set();
  }
}

//=================================================================================================

void ViewerTest_ContinuousRedrawer::Stop(const occ::handle<V3d_View>& theView)
{
  if (!theView.IsNull() && myView != theView)
  {
    return;
  }

  {
    std::lock_guard<std::mutex> aLock(myMutex);
    myToStop  = true;
    myToPause = false;
  }
  myWakeEvent.Set();
  myThread.Wait();
  myToStop = false;
  myView.Nullify();
}

//=================================================================================================

void ViewerTest_ContinuousRedrawer::Pause()
{
  if (!myToPause)
  {
    std::lock_guard<std::mutex> aLock(myMutex);
    myToPause = true;
  }
}

//=================================================================================================

void ViewerTest_ContinuousRedrawer::doThreadLoop()
{
  occ::handle<Aspect_DisplayConnection> aDisp = new Aspect_DisplayConnection();
  OSD_Timer                             aTimer;
  aTimer.Start();
  double       aTimeOld   = 0.0;
  const double aTargetDur = myTargetFps > 0.0 ? 1.0 / myTargetFps : -1.0;
  for (;;)
  {
    bool toPause = false;
    {
      std::lock_guard<std::mutex> aLock(myMutex);
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
      const double aTimeNew  = aTimer.ElapsedTime();
      const double aDuration = aTimeNew - aTimeOld;
      if (aDuration >= aTargetDur)
      {
        myView->Invalidate();
        myView->Window()->InvalidateContent(aDisp);
        aTimeOld = aTimeNew;
      }
    }
    else
    {
      myView->Invalidate();
      myView->Window()->InvalidateContent(aDisp);
    }

    OSD::MilliSecSleep(1);
  }
}
