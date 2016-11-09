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
  Standard_Real Pts;             //!< global presentation timestamp
  Standard_Real LocalPts;        //!< presentation within current animation
  Standard_Real LocalNormalized; //!< normalized position within current animation within 0..1 range

  AIS_AnimationProgress() : Pts (-1.0), LocalPts (-1.0), LocalNormalized (-1.0) {}
};

DEFINE_STANDARD_HANDLE(AIS_Animation, Standard_Transient)

//! Class represents single animation.
//! It is defined with:
//! - Start time on the timeline started from 0, in seconds
//! - Duration, in seconds
//! - virtual method Update() for handling an update
class AIS_Animation : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(AIS_Animation, Standard_Transient)
public:

  //! Creates empty animation.
  Standard_EXPORT AIS_Animation (const TCollection_AsciiString& theAnimationName);

  //! Destruct object, clear arguments
  Standard_EXPORT virtual ~AIS_Animation();

  //! Animation name.
  const TCollection_AsciiString& Name() const { return myName; }

public:

  //! @return start time of the animation in the timeline
  Standard_Real StartPts() const { return myPtsStart; }

  //! Sets time limits for animation in the animation timeline
  void SetStartPts (const Standard_Real thePtsStart) { myPtsStart = thePtsStart; }

  //! @return duration of the animation in the timeline
  Standard_Real Duration() const { return Max (myOwnDuration, myChildrenDuration); }

  //! Update total duration considering all animations on timeline.
  Standard_EXPORT void UpdateTotalDuration();

  //! Return true if duration is defined.
  Standard_Boolean HasOwnDuration() const { return myOwnDuration > 0.0; }

  //! @return own duration of the animation in the timeline
  Standard_Real OwnDuration() const { return myOwnDuration; }

  //! Defines duration of the animation.
  void SetOwnDuration (const Standard_Real theDuration) { myOwnDuration = theDuration; }

  //! Add single animation to the timeline.
  //! @param theAnimation input animation
  Standard_EXPORT void Add (const Handle(AIS_Animation)& theAnimation);

  //! Clear animation timeline - remove all animations from it.
  Standard_EXPORT void Clear();

  //! Return the child animation with the given name.
  Standard_EXPORT Handle(AIS_Animation) Find (const TCollection_AsciiString& theAnimationName) const;

  //! Remove the child animation.
  Standard_EXPORT Standard_Boolean Remove (const Handle(AIS_Animation)& theAnimation);

  //! Replace the child animation.
  Standard_EXPORT Standard_Boolean Replace (const Handle(AIS_Animation)& theAnimationOld,
                                            const Handle(AIS_Animation)& theAnimationNew);

  //! Clears own children and then copy child animations from another object.
  //! Copy also Start Time and Duration values.
  Standard_EXPORT void CopyFrom (const Handle(AIS_Animation)& theOther);

  //! Return sequence of child animations.
  const NCollection_Sequence<Handle(AIS_Animation)>& Children() const { return myAnimations; }

public:

  //! Start animation with internally defined timer instance.
  //! Calls ::Start() internally.
  //! @param theStartPts    starting timer position (presentation timestamp)
  //! @param thePlaySpeed   playback speed (1.0 means normal speed)
  //! @param theToUpdate    flag to update defined animations to specified start position
  //! @param theToStopTimer flag to pause timer at the starting position
  Standard_EXPORT virtual void StartTimer (const Standard_Real    theStartPts,
                                           const Standard_Real    thePlaySpeed,
                                           const Standard_Boolean theToUpdate,
                                           const Standard_Boolean theToStopTimer = Standard_False);

  //! Update single frame of animation, update timer state
  //! @return current time of timeline progress.
  Standard_EXPORT virtual Standard_Real UpdateTimer();

  //! Return elapsed time.
  Standard_Real ElapsedTime() const { return !myTimer.IsNull() ? myTimer->ElapsedTime() : 0.0; }

public:

  //! Start animation. This method changes status of the animation to Started.
  //! This status defines whether animation is to be performed in the timeline or not.
  //! @param theToUpdate call Update() method
  Standard_EXPORT virtual void Start (const Standard_Boolean theToUpdate);

  //! Pause the process timeline.
  Standard_EXPORT virtual void Pause();

  //! Stop animation. This method changed status of the animation to Stopped.
  //! This status shows that animation will not be performed in the timeline or it is finished.
  Standard_EXPORT virtual void Stop();

  //! Check if animation is to be performed in the animation timeline.
  //! @return True if it is stopped of finished.
  bool IsStopped() { return myState != AnimationState_Started; }

  //! Update single frame of animation, update timer state
  //! @param thePts [in] the time moment within [0; Duration()]
  //! @return True if timeline is in progress
  Standard_EXPORT virtual Standard_Boolean Update (const Standard_Real thePts);

protected:

  //! Process one step of the animation according to the input time progress, including all children.
  //! Calls also ::update() to update own animation.
  Standard_EXPORT virtual void updateWithChildren (const AIS_AnimationProgress& thePosition);

  //! Update the own animation to specified position - should be overridden by sub-class.
  virtual void update (const AIS_AnimationProgress& theProgress) { (void )theProgress; }

protected:

  //! Defines animation state.
  enum AnimationState
  {
    AnimationState_Started, //!< animation is in progress
    AnimationState_Stopped, //!< animation is finished, force stopped or not started
    AnimationState_Paused   //!< animation is paused and can be started from the pause moment
  };

protected:

  Handle(AIS_AnimationTimer) myTimer;

  TCollection_AsciiString myName;           //!< animation name
  NCollection_Sequence<Handle(AIS_Animation)>
                        myAnimations;       //!< sequence of child animations

  AnimationState        myState;            //!< animation state - started, stopped of paused
  Standard_Real         myPtsStart;         //!< time of start in the timeline
  Standard_Real         myOwnDuration;      //!< duration of animation excluding children
  Standard_Real         myChildrenDuration; //!< duration of animation including children

};

#endif // _AIS_Animation_HeaderFile
