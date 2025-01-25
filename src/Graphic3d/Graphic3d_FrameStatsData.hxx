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

#ifndef _Graphic3d_FrameStatsData_HeaderFile
#define _Graphic3d_FrameStatsData_HeaderFile

#include <NCollection_Array1.hxx>
#include <Graphic3d_FrameStatsCounter.hxx>
#include <Graphic3d_FrameStatsTimer.hxx>
#include <OSD_Timer.hxx>

#include <vector>

//! Data frame definition.
class Graphic3d_FrameStatsData
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns FPS (frames per seconds, elapsed time).
  //! This number indicates an actual frame rate averaged for several frames within UpdateInterval()
  //! duration, basing on a real elapsed time between updates.
  Standard_Real FrameRate() const { return myFps; }

  //! Returns CPU FPS (frames per seconds, CPU time).
  //! This number indicates a PREDICTED frame rate,
  //! basing on CPU elapsed time between updates and NOT real elapsed time (which might include
  //! periods of CPU inactivity). Number is expected to be greater then actual frame rate returned
  //! by FrameRate(). Values significantly greater actual frame rate indicate that rendering is
  //! limited by GPU performance (CPU is stalled in-between), while values around actual frame rate
  //! indicate rendering being limited by CPU performance (GPU is stalled in-between).
  Standard_Real FrameRateCpu() const { return myFpsCpu; }

  //! Returns FPS for immediate redraws.
  Standard_Real ImmediateFrameRate() const { return myFpsImmediate; }

  //! Returns CPU FPS for immediate redraws.
  Standard_Real ImmediateFrameRateCpu() const { return myFpsCpuImmediate; }

  //! Get counter value.
  Standard_Size CounterValue(Graphic3d_FrameStatsCounter theIndex) const
  {
    return myCounters[theIndex];
  }

  //! Get counter value.
  Standard_Size operator[](Graphic3d_FrameStatsCounter theIndex) const
  {
    return CounterValue(theIndex);
  }

  //! Get timer value.
  Standard_Real TimerValue(Graphic3d_FrameStatsTimer theIndex) const { return myTimers[theIndex]; }

  //! Get timer value.
  Standard_Real operator[](Graphic3d_FrameStatsTimer theIndex) const
  {
    return TimerValue(theIndex);
  }

  //! Empty constructor.
  Standard_EXPORT Graphic3d_FrameStatsData();

  //! Copy constructor.
  Standard_EXPORT Graphic3d_FrameStatsData(const Graphic3d_FrameStatsData& theOther);

  //! Move constructor.
  Standard_EXPORT Graphic3d_FrameStatsData(Graphic3d_FrameStatsData&& theOther) noexcept;

  //! Assignment operator.
  Standard_EXPORT Graphic3d_FrameStatsData& operator=(const Graphic3d_FrameStatsData& theOther);

  //! Assignment with move operator.
  Standard_EXPORT Graphic3d_FrameStatsData& operator=(Graphic3d_FrameStatsData&& theOther) noexcept;

  //! Reset data.
  Standard_EXPORT void Reset();

  //! Fill with maximum values.
  Standard_EXPORT void FillMax(const Graphic3d_FrameStatsData& theOther);

protected:
  std::vector<Standard_Size> myCounters;     //!< counters
  std::vector<Standard_Real> myTimers;       //!< timers
  std::vector<Standard_Real> myTimersMin;    //!< minimal values of timers
  std::vector<Standard_Real> myTimersMax;    //!< maximum values of timers
  Standard_Real              myFps;          //!< FPS     meter (frames per seconds, elapsed time)
  Standard_Real              myFpsCpu;       //!< CPU FPS meter (frames per seconds, CPU time)
  Standard_Real              myFpsImmediate; //!< FPS     meter for immediate redraws
  Standard_Real              myFpsCpuImmediate; //!< CPU FPS meter for immediate redraws
};

//! Temporary data frame definition.
class Graphic3d_FrameStatsDataTmp : public Graphic3d_FrameStatsData
{
public:
  //! Empty constructor.
  Standard_EXPORT Graphic3d_FrameStatsDataTmp();

  //! Compute average data considering the amount of rendered frames.
  Standard_EXPORT void FlushTimers(Standard_Size theNbFrames, bool theIsFinal);

  //! Reset data.
  Standard_EXPORT void Reset();

  //! Assignment operator (skip copying irrelevant properties).
  void operator=(const Graphic3d_FrameStatsData& theOther)
  {
    Graphic3d_FrameStatsData::operator=(theOther);
  }

  //! Returns FPS (frames per seconds, elapsed time).
  Standard_Real& ChangeFrameRate() { return myFps; }

  //! Returns CPU FPS (frames per seconds, CPU time).
  Standard_Real& ChangeFrameRateCpu() { return myFpsCpu; }

  //! Returns FPS for immediate redraws.
  Standard_Real& ChangeImmediateFrameRate() { return myFpsImmediate; }

  //! Returns CPU FPS for immediate redraws.
  Standard_Real& ChangeImmediateFrameRateCpu() { return myFpsCpuImmediate; }

  //! Return a timer object for time measurements.
  OSD_Timer& ChangeTimer(Graphic3d_FrameStatsTimer theTimer) { return myOsdTimers[theTimer]; }

  //! Get counter value.
  Standard_Size& ChangeCounterValue(Graphic3d_FrameStatsCounter theIndex)
  {
    return myCounters[theIndex];
  }

  //! Modify counter value.
  Standard_Size& operator[](Graphic3d_FrameStatsCounter theIndex)
  {
    return ChangeCounterValue(theIndex);
  }

  //! Modify timer value.
  Standard_Real& ChangeTimerValue(Graphic3d_FrameStatsTimer theIndex) { return myTimers[theIndex]; }

  //! Modify timer value.
  Standard_Real& operator[](Graphic3d_FrameStatsTimer theIndex)
  {
    return ChangeTimerValue(theIndex);
  }

protected:
  std::vector<OSD_Timer>     myOsdTimers;  //!< precise timers for time measurements
  std::vector<Standard_Real> myTimersPrev; //!< previous timers values
};

#endif // _Graphic3d_FrameStatsData_HeaderFile
