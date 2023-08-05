// Copyright (c) 2018 OPEN CASCADE SAS
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

#include <Graphic3d_FrameStatsData.hxx>

// =======================================================================
// function : Graphic3d_FrameStatsData
// purpose  :
// =======================================================================
Graphic3d_FrameStatsData::Graphic3d_FrameStatsData()
: myFps (-1.0),
  myFpsCpu (-1.0),
  myFpsImmediate (-1.0),
  myFpsCpuImmediate (-1.0)
{
  myCounters .resize (Graphic3d_FrameStatsCounter_NB, 0);
  myTimers   .resize (Graphic3d_FrameStatsTimer_NB, 0.0);
  myTimersMin.resize (Graphic3d_FrameStatsTimer_NB, RealLast());
  myTimersMax.resize (Graphic3d_FrameStatsTimer_NB, 0.0);
  Reset();
}

// =======================================================================
// function : Graphic3d_FrameStatsData
// purpose  :
// =======================================================================
Graphic3d_FrameStatsData::Graphic3d_FrameStatsData(const Graphic3d_FrameStatsData& theOther) :
  myCounters(theOther.myCounters),
  myTimers(theOther.myTimers),
  myTimersMin(theOther.myTimersMin),
  myTimersMax(theOther.myTimersMax),
  myFps(theOther.myFps),
  myFpsCpu(theOther.myFpsCpu),
  myFpsImmediate(theOther.myFpsImmediate),
  myFpsCpuImmediate(theOther.myFpsCpuImmediate)
{}

// =======================================================================
// function : Graphic3d_FrameStatsData
// purpose  :
// =======================================================================
Graphic3d_FrameStatsData::Graphic3d_FrameStatsData(Graphic3d_FrameStatsData&& theOther) noexcept :
  myCounters(std::move(theOther.myCounters)),
  myTimers(std::move(theOther.myTimers)),
  myTimersMin(std::move(theOther.myTimersMin)),
  myTimersMax(std::move(theOther.myTimersMax)),
  myFps(std::move(theOther.myFps)),
  myFpsCpu(std::move(theOther.myFpsCpu)),
  myFpsImmediate(std::move(theOther.myFpsImmediate)),
  myFpsCpuImmediate(std::move(theOther.myFpsCpuImmediate))
{}

// =======================================================================
// function : operator=
// purpose  :
// =======================================================================
Graphic3d_FrameStatsData& Graphic3d_FrameStatsData::operator= (const Graphic3d_FrameStatsData& theOther)
{
  if (&theOther == this)
  {
    return *this;
  }
  myFps         = theOther.myFps;
  myFpsCpu      = theOther.myFpsCpu;
  myFpsImmediate    = theOther.myFpsImmediate;
  myFpsCpuImmediate = theOther.myFpsCpuImmediate;
  myCounters    = theOther.myCounters;
  myTimers      = theOther.myTimers;
  myTimersMin   = theOther.myTimersMin;
  myTimersMax   = theOther.myTimersMax;
  return *this;
}

// =======================================================================
// function : operator=
// purpose  :
// =======================================================================
Graphic3d_FrameStatsData& Graphic3d_FrameStatsData::operator=(Graphic3d_FrameStatsData&& theOther) noexcept
{
  if (&theOther == this)
  {
    return *this;
  }
  myFps = std::move(theOther.myFps);
  myFpsCpu = std::move(theOther.myFpsCpu);
  myFpsImmediate = std::move(theOther.myFpsImmediate);
  myFpsCpuImmediate = std::move(theOther.myFpsCpuImmediate);
  myCounters = std::move(theOther.myCounters);
  myTimers = std::move(theOther.myTimers);
  myTimersMin = std::move(theOther.myTimersMin);
  myTimersMax = std::move(theOther.myTimersMax);
  return *this;
}

// =======================================================================
// function : Reset
// purpose  :
// =======================================================================
void Graphic3d_FrameStatsData::Reset()
{
  myFps    = -1.0;
  myFpsCpu = -1.0;
  myFpsImmediate = -1.0;
  myFpsCpuImmediate = -1.0;
  myCounters .assign (myCounters.size(),  0);
  myTimers   .assign (myTimers.size(),    0.0);
  myTimersMin.assign (myTimersMin.size(), RealLast());
  myTimersMax.assign (myTimersMax.size(), 0.0);
}

// =======================================================================
// function : FillMax
// purpose  :
// =======================================================================
void Graphic3d_FrameStatsData::FillMax (const Graphic3d_FrameStatsData& theOther)
{
  myFps    = Max (myFps,    theOther.myFps);
  myFpsCpu = Max (myFpsCpu, theOther.myFpsCpu);
  myFpsImmediate    = Max (myFpsImmediate,    theOther.myFpsImmediate);
  myFpsCpuImmediate = Max (myFpsCpuImmediate, theOther.myFpsCpuImmediate);
  for (size_t aCounterIter = 0; aCounterIter < myCounters.size(); ++aCounterIter)
  {
    myCounters[aCounterIter] = myCounters[aCounterIter] > theOther.myCounters[aCounterIter] ? myCounters[aCounterIter] : theOther.myCounters[aCounterIter];
  }
  for (size_t aTimerIter = 0; aTimerIter < myTimers.size(); ++aTimerIter)
  {
    myTimersMax[aTimerIter] = Max (myTimersMax[aTimerIter], theOther.myTimersMax[aTimerIter]);
    myTimersMin[aTimerIter] = Min (myTimersMin[aTimerIter], theOther.myTimersMin[aTimerIter]);
    myTimers   [aTimerIter] = myTimersMax[aTimerIter];
  }
}

// =======================================================================
// function : Graphic3d_FrameStatsDataTmp
// purpose  :
// =======================================================================
Graphic3d_FrameStatsDataTmp::Graphic3d_FrameStatsDataTmp()
{
  myOsdTimers .resize (Graphic3d_FrameStatsTimer_NB, OSD_Timer (true));
  myTimersPrev.resize (Graphic3d_FrameStatsTimer_NB, 0.0);
}

// =======================================================================
// function : FlushTimers
// purpose  :
// =======================================================================
void Graphic3d_FrameStatsDataTmp::FlushTimers (Standard_Size theNbFrames, bool theIsFinal)
{
  for (size_t aTimerIter = 0; aTimerIter < myTimers.size(); ++aTimerIter)
  {
    const Standard_Real aFrameTime = myTimers[aTimerIter] - myTimersPrev[aTimerIter];
    myTimersMax [aTimerIter] = Max (myTimersMax[aTimerIter], aFrameTime);
    myTimersMin [aTimerIter] = Min (myTimersMin[aTimerIter], aFrameTime);
    myTimersPrev[aTimerIter] = myTimers[aTimerIter];
  }

  if (theIsFinal)
  {
    const Standard_Real aNbFrames = (Standard_Real )theNbFrames;
    for (size_t aTimerIter = 0; aTimerIter < myTimers.size(); ++aTimerIter)
    {
      myTimers[aTimerIter] /= aNbFrames;
    }
  }
}

// =======================================================================
// function : Reset
// purpose  :
// =======================================================================
void Graphic3d_FrameStatsDataTmp::Reset()
{
  Graphic3d_FrameStatsData::Reset();
  myTimersPrev.assign (myTimersPrev.size(), 0.0);
  for (size_t aTimerIter = 0; aTimerIter < myOsdTimers.size(); ++aTimerIter)
  {
    myOsdTimers[aTimerIter].Reset();
  }
}
