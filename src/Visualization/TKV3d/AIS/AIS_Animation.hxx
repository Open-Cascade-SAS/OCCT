// Created by: Anastasia BORISOVA
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

#ifndef _AIS_Animation_HeaderFile
#define _AIS_Animation_HeaderFile

#include <AIS_AnimationTimer.hxx>
#include <NCollection_Sequence.hxx>
#include <TCollection_AsciiString.hxx>

//! Structure defining current animation progress.
struct AIS_AnimationProgress
{
  Standard_Real Pts;            //!< global presentation timestamp
  Standard_Real LocalPts;       //!< presentation within current animation
                                // clang-format off
  Standard_Real LocalNormalized; //!< normalized position within current animation within 0..1 range
                                // clang-format on

  AIS_AnimationProgress()
      : Pts(-1.0),
        LocalPts(-1.0),
        LocalNormalized(-1.0)
  {
  }
};

DEFINE_STANDARD_HANDLE(AIS_Animation, Standard_Transient)

//! Class represents a basic animation class.
//! AIS_Animation can be used as:
//!
//! - Animation Implementor
//!   Sub-classes should override method AIS_Animation::update() to perform specific animation.
//!   AIS package provides limited number of such animation atoms - classes AIS_AnimationObject and
//!   AIS_AnimationCamera, which could be enough for defining a simple animation. In general case,
//!   application is expected defining own AIS_Animation sub-classes implementing
//!   application-specific animation logic (e.g. another interpolation or another kind of
//!   transformations - like color transition and others). The basic conception of
//!   AIS_Animation::update() is defining an exact scene state for the current presentation
//!   timestamp, providing a smooth and continuous animation well defined at any time step and in
//!   any direction. So that a time difference between two sequential drawn Viewer frames can vary
//!   from frame to frame without visual artifacts, increasing rendering framerate would not lead to
//!   animation being executed too fast and low framerate (on slow hardware) would not lead to
//!   animation played longer than defined duration. Hence, implementation should avoid usage of
//!   incremental step logic or should apply it very carefully.
//!
//! - Animation Container
//!   AIS_Animation (no sub-classing) can be used to aggregate a sequence of Animation items
//!   (children). Each children should be defined with its own duration and start time (presentation
//!   timestamp). It is possible defining collection of nested AIS_Animation items, so that within
//!   each container level children define start playback time relative to its holder.
//!
//! - Animation playback Controller
//!   It is suggested that application would define a single AIS_Animation instance (optional
//!   sub-classing) for controlling animation playback as whole. Such controller should be filled in
//!   by other AIS_Animation as children objects, and will be managed by application by calling
//!   StartTimer(), UpdateTimer() and IsStopped() methods.
//!
//! Note, that AIS_Animation::StartTimer() defines a timer calculating an elapsed time, not a
//! multimedia timer executing Viewer updates at specific intervals! Application should avoid using
//! implicit and immediate Viewer updates to ensure that AIS_Animation::UpdateTimer() is called
//! before each redrawing of a Viewer content. Redrawing logic should be also managed at application
//! level for managing a smooth animation (by defining a multimedia timer provided by used GUI
//! framework executing updates at desired framerate, or as continuous redraws in loop).
class AIS_Animation : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(AIS_Animation, Standard_Transient)
public:
  //! Creates empty animation.
  Standard_EXPORT AIS_Animation(const TCollection_AsciiString& theAnimationName);

  //! Destruct object, clear arguments
  Standard_EXPORT virtual ~AIS_Animation();

  //! Animation name.
  const TCollection_AsciiString& Name() const { return myName; }

public:
  //! @return start time of the animation in the timeline
  Standard_Real StartPts() const { return myPtsStart; }

  //! Sets time limits for animation in the animation timeline
  void SetStartPts(const Standard_Real thePtsStart) { myPtsStart = thePtsStart; }

  //! @return duration of the animation in the timeline
  Standard_Real Duration() const { return Max(myOwnDuration, myChildrenDuration); }

  //! Update total duration considering all animations on timeline.
  Standard_EXPORT void UpdateTotalDuration();

  //! Return true if duration is defined.
  Standard_Boolean HasOwnDuration() const { return myOwnDuration > 0.0; }

  //! @return own duration of the animation in the timeline
  Standard_Real OwnDuration() const { return myOwnDuration; }

  //! Defines duration of the animation.
  void SetOwnDuration(const Standard_Real theDuration) { myOwnDuration = theDuration; }

  //! Add single animation to the timeline.
  //! @param theAnimation input animation
  Standard_EXPORT void Add(const Handle(AIS_Animation)& theAnimation);

  //! Clear animation timeline - remove all animations from it.
  Standard_EXPORT void Clear();

  //! Return the child animation with the given name.
  Standard_EXPORT Handle(AIS_Animation) Find(const TCollection_AsciiString& theAnimationName) const;

  //! Remove the child animation.
  Standard_EXPORT Standard_Boolean Remove(const Handle(AIS_Animation)& theAnimation);

  //! Replace the child animation.
  Standard_EXPORT Standard_Boolean Replace(const Handle(AIS_Animation)& theAnimationOld,
                                           const Handle(AIS_Animation)& theAnimationNew);

  //! Clears own children and then copy child animations from another object.
  //! Copy also Start Time and Duration values.
  Standard_EXPORT void CopyFrom(const Handle(AIS_Animation)& theOther);

  //! Return sequence of child animations.
  const NCollection_Sequence<Handle(AIS_Animation)>& Children() const { return myAnimations; }

public:
  //! Start animation with internally defined timer instance.
  //! Calls ::Start() internally.
  //!
  //! Note, that this method initializes a timer calculating an elapsed time (presentation
  //! timestamps within AIS_Animation::UpdateTimer()), not a multimedia timer executing Viewer
  //! updates at specific intervals! Viewer redrawing should be managed at application level, so
  //! that AIS_Animation::UpdateTimer() is called once right before each redrawing of a Viewer
  //! content.
  //!
  //! @param theStartPts    starting timer position (presentation timestamp)
  //! @param thePlaySpeed   playback speed (1.0 means normal speed)
  //! @param theToUpdate    flag to update defined animations to specified start position
  //! @param theToStopTimer flag to pause timer at the starting position
  Standard_EXPORT virtual void StartTimer(const Standard_Real    theStartPts,
                                          const Standard_Real    thePlaySpeed,
                                          const Standard_Boolean theToUpdate,
                                          const Standard_Boolean theToStopTimer = Standard_False);

  //! Update single frame of animation, update timer state
  //! @return current time of timeline progress.
  Standard_EXPORT virtual Standard_Real UpdateTimer();

  //! Return elapsed time.
  Standard_Real ElapsedTime() const { return !myTimer.IsNull() ? myTimer->ElapsedTime() : 0.0; }

  //! Return playback timer.
  const Handle(Media_Timer)& Timer() const { return myTimer; }

  //! Set playback timer.
  void SetTimer(const Handle(Media_Timer)& theTimer) { myTimer = theTimer; }

public:
  //! Start animation. This method changes status of the animation to Started.
  //! This status defines whether animation is to be performed in the timeline or not.
  //! @param theToUpdate call Update() method
  Standard_EXPORT virtual void Start(const Standard_Boolean theToUpdate);

  //! Pause the process timeline.
  Standard_EXPORT virtual void Pause();

  //! Stop animation. This method changed status of the animation to Stopped.
  //! This status shows that animation will not be performed in the timeline or it is finished.
  Standard_EXPORT virtual void Stop();

  //! Check if animation is to be performed in the animation timeline.
  //! @return True if it is stopped of finished.
  bool IsStopped() { return myState != AnimationState_Started; }

  //! Update single frame of animation, update timer state
  //! @param[in] thePts  the time moment within [0; Duration()]
  //! @return True if timeline is in progress
  Standard_EXPORT virtual Standard_Boolean Update(const Standard_Real thePts);

protected:
  //! Process one step of the animation according to the input time progress, including all
  //! children. Calls also ::update() to update own animation.
  Standard_EXPORT virtual void updateWithChildren(const AIS_AnimationProgress& thePosition);

  //! Update the own animation to specified position - should be overridden by sub-class.
  virtual void update(const AIS_AnimationProgress& theProgress) { (void)theProgress; }

protected:
  //! Defines animation state.
  enum AnimationState
  {
    AnimationState_Started, //!< animation is in progress
    AnimationState_Stopped, //!< animation is finished, force stopped or not started
    AnimationState_Paused   //!< animation is paused and can be started from the pause moment
  };

protected:
  Handle(Media_Timer) myTimer;

  TCollection_AsciiString                     myName;       //!< animation name
  NCollection_Sequence<Handle(AIS_Animation)> myAnimations; //!< sequence of child animations

  AnimationState myState;            //!< animation state - started, stopped of paused
  Standard_Real  myPtsStart;         //!< time of start in the timeline
  Standard_Real  myOwnDuration;      //!< duration of animation excluding children
  Standard_Real  myChildrenDuration; //!< duration of animation including children
};

#endif // _AIS_Animation_HeaderFile
