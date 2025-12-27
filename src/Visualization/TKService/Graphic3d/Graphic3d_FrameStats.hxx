// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef _Graphic3d_FrameStats_HeaderFile
#define _Graphic3d_FrameStats_HeaderFile

#include <Graphic3d_FrameStatsData.hxx>
#include <Graphic3d_RenderingParams.hxx>
#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_IndexedDataMap.hxx>

class Graphic3d_CView;

//! Class storing the frame statistics.
class Graphic3d_FrameStats : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_FrameStats, Standard_Transient)
public:
  //! Default constructor.
  Standard_EXPORT Graphic3d_FrameStats();

  //! Destructor.
  Standard_EXPORT virtual ~Graphic3d_FrameStats();

  //! Returns interval in seconds for updating meters across several frames; 1 second by default.
  double UpdateInterval() const { return myUpdateInterval; }

  //! Sets interval in seconds for updating values.
  void SetUpdateInterval(double theInterval) { myUpdateInterval = theInterval; }

  //! Prefer longer lines over more greater of lines.
  bool IsLongLineFormat() const { return myIsLongLineFormat; }

  //! Set if format should prefer longer lines over greater number of lines.
  void SetLongLineFormat(bool theValue) { myIsLongLineFormat = theValue; }

  //! Frame redraw started.
  Standard_EXPORT virtual void FrameStart(const occ::handle<Graphic3d_CView>& theView,
                                          bool                           theIsImmediateOnly);

  //! Frame redraw finished.
  Standard_EXPORT virtual void FrameEnd(const occ::handle<Graphic3d_CView>& theView,
                                        bool                           theIsImmediateOnly);

public:
  //! Returns formatted string.
  Standard_EXPORT virtual TCollection_AsciiString FormatStats(
    Graphic3d_RenderingParams::PerfCounters theFlags) const;

  //! Fill in the dictionary with formatted statistic info.
  Standard_EXPORT virtual void FormatStats(NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString>&   theDict,
                                           Graphic3d_RenderingParams::PerfCounters theFlags) const;

  //! Returns duration of the last frame in seconds.
  double FrameDuration() const { return myFrameDuration; }

  //! Returns FPS (frames per seconds, elapsed time).
  //! This number indicates an actual frame rate averaged for several frames within UpdateInterval()
  //! duration, basing on a real elapsed time between updates.
  double FrameRate() const { return LastDataFrame().FrameRate(); }

  //! Returns CPU FPS (frames per seconds, CPU time).
  //! This number indicates a PREDICTED frame rate,
  //! basing on CPU elapsed time between updates and NOT real elapsed time (which might include
  //! periods of CPU inactivity). Number is expected to be greater then actual frame rate returned
  //! by FrameRate(). Values significantly greater actual frame rate indicate that rendering is
  //! limited by GPU performance (CPU is stalled in-between), while values around actual frame rate
  //! indicate rendering being limited by CPU performance (GPU is stalled in-between).
  double FrameRateCpu() const { return LastDataFrame().FrameRateCpu(); }

  //! Returns value of specified counter, cached between stats updates.
  //! Should NOT be called between ::FrameStart() and ::FrameEnd() calls.
  size_t CounterValue(Graphic3d_FrameStatsCounter theCounter) const
  {
    return LastDataFrame()[theCounter];
  }

  //! Returns value of specified timer for modification, should be called between ::FrameStart() and
  //! ::FrameEnd() calls. Should NOT be called between ::FrameStart() and ::FrameEnd() calls.
  double TimerValue(Graphic3d_FrameStatsTimer theTimer) const
  {
    return LastDataFrame()[theTimer];
  }

  //! Returns TRUE if some Layers have been culled.
  bool HasCulledLayers() const
  {
    return LastDataFrame()[Graphic3d_FrameStatsCounter_NbLayersNotCulled]
           != LastDataFrame()[Graphic3d_FrameStatsCounter_NbLayers];
  }

  //! Returns TRUE if some structures have been culled.
  bool HasCulledStructs() const
  {
    return LastDataFrame()[Graphic3d_FrameStatsCounter_NbStructsNotCulled]
           != LastDataFrame()[Graphic3d_FrameStatsCounter_NbStructs];
  }

  //! Returns last data frame, cached between stats updates.
  //! Should NOT be called between ::FrameStart() and ::FrameEnd() calls.
  const Graphic3d_FrameStatsData& LastDataFrame() const
  {
    return myCounters.Value(myLastFrameIndex);
  }

  //! Returns last data frame index.
  int LastDataFrameIndex() const { return myLastFrameIndex; }

  //! Returns data frames.
  const NCollection_Array1<Graphic3d_FrameStatsData>& DataFrames() const { return myCounters; }

  //! Returns data frames.
  NCollection_Array1<Graphic3d_FrameStatsData>& ChangeDataFrames() { return myCounters; }

public:
  //! Returns value of specified counter for modification, should be called between ::FrameStart()
  //! and ::FrameEnd() calls.
  size_t& ChangeCounter(Graphic3d_FrameStatsCounter theCounter)
  {
    return ActiveDataFrame()[theCounter];
  }

  //! Returns value of specified timer for modification, should be called between ::FrameStart() and
  //! ::FrameEnd() calls.
  double& ChangeTimer(Graphic3d_FrameStatsTimer theTimer)
  {
    return ActiveDataFrame()[theTimer];
  }

  //! Returns currently filling data frame for modification, should be called between ::FrameStart()
  //! and ::FrameEnd() calls.
  Graphic3d_FrameStatsDataTmp& ActiveDataFrame() { return myCountersTmp; }

protected:
  //! Method to collect statistics from the View; called by FrameEnd().
  virtual void updateStatistics(const occ::handle<Graphic3d_CView>& theView,
                                bool                           theIsImmediateOnly) = 0;

protected:
  OSD_Timer     myFpsTimer;       //!< timer for FPS measurements
  double myFrameStartTime; //!< time at the beginning of frame redraw
  double myFrameDuration;  //!< frame duration
  double myUpdateInterval; //!< interval to update meters
  // clang-format off
  size_t    myFpsFrameCount;           //!< FPS counter (within short measurement time slice)
  NCollection_Array1<Graphic3d_FrameStatsData> myCounters; //!< data frames history
  Graphic3d_FrameStatsDataTmp myCountersTmp;  //!< data frame values filled to be filled between FrameStart() and FrameEnd() calls
  Graphic3d_FrameStatsData    myCountersMax;  //!< data frame values with absolute maximum values in the history
  int myLastFrameIndex;          //!< last data frame index
  bool myIsLongLineFormat;        //!< prefer longer lines over greater number of lines
  // clang-format on
};

#endif // _Graphic3d_FrameStats_HeaderFile
