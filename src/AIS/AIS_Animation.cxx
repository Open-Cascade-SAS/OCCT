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

#include <AIS_Animation.hxx>

#include <Standard_Assert.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_Animation, Standard_Transient)

//=============================================================================
//function : Constructor
//purpose  :
//=============================================================================
AIS_Animation::AIS_Animation (const TCollection_AsciiString& theAnimationName)
: myName (theAnimationName),
  myState (AnimationState_Stopped),
  myPtsStart (0.0),
  myOwnDuration (0.0),
  myChildrenDuration (0.0)
{
  //
}

//=============================================================================
//function : ~AIS_Animation
//purpose  :
//=============================================================================
AIS_Animation::~AIS_Animation()
{
  Clear();
}

//=============================================================================
//function : Clear
//purpose  :
//=============================================================================
void AIS_Animation::Clear()
{
  myAnimations.Clear();
  myOwnDuration = 0.0;
}

//=============================================================================
//function : Add
//purpose  :
//=============================================================================
void AIS_Animation::Add (const Handle(AIS_Animation)& theAnimation)
{
  if (theAnimation.IsNull())
  {
    throw Standard_ProgramError("AIS_Animation::Add() - attempt to add a NULL animation!");
  }

  for (NCollection_Sequence<Handle(AIS_Animation)>::Iterator anIter (myAnimations); anIter.More(); anIter.Next())
  {
    if (anIter.Value() == theAnimation)
    {
      UpdateTotalDuration();
      return;
    }
  }

  myAnimations.Append (theAnimation);
  UpdateTotalDuration();
}

//=============================================================================
//function : Find
//purpose  :
//=============================================================================
Handle(AIS_Animation) AIS_Animation::Find (const TCollection_AsciiString& theAnimationName) const
{
  for (NCollection_Sequence<Handle(AIS_Animation)>::Iterator anIter (myAnimations); anIter.More(); anIter.Next())
  {
    if (anIter.Value()->Name() == theAnimationName)
    {
      return anIter.Value();
    }
  }
  return Handle(AIS_Animation)();
}

//=============================================================================
//function : Remove
//purpose  :
//=============================================================================
Standard_Boolean AIS_Animation::Remove (const Handle(AIS_Animation)& theAnimation)
{
  for (NCollection_Sequence<Handle(AIS_Animation)>::Iterator anIter (myAnimations); anIter.More(); anIter.Next())
  {
    if (anIter.Value() == theAnimation)
    {
      myAnimations.Remove (anIter);
      UpdateTotalDuration();
      return Standard_True;
    }
  }
  return Standard_False;
}

//=============================================================================
//function : Replace
//purpose  :
//=============================================================================
Standard_Boolean AIS_Animation::Replace (const Handle(AIS_Animation)& theAnimationOld,
                                         const Handle(AIS_Animation)& theAnimationNew)
{
  for (NCollection_Sequence<Handle(AIS_Animation)>::Iterator anIter (myAnimations); anIter.More(); anIter.Next())
  {
    if (anIter.Value() == theAnimationOld)
    {
      anIter.ChangeValue() = theAnimationNew;
      UpdateTotalDuration();
      return Standard_True;
    }
  }
  return Standard_False;
}

//=============================================================================
//function : CopyFrom
//purpose  :
//=============================================================================
void AIS_Animation::CopyFrom (const Handle(AIS_Animation)& theOther)
{
  myAnimations.Clear();
  for (NCollection_Sequence<Handle(AIS_Animation)>::Iterator anIter (theOther->myAnimations); anIter.More(); anIter.Next())
  {
    myAnimations.Append (anIter.Value());
  }
  UpdateTotalDuration();
  myPtsStart    = theOther->myPtsStart;
  myOwnDuration = theOther->myOwnDuration;
}

//=============================================================================
//function : UpdateTotalDuration
//purpose  :
//=============================================================================
void AIS_Animation::UpdateTotalDuration()
{
  myChildrenDuration = 0.0;
  for (NCollection_Sequence<Handle(AIS_Animation)>::Iterator anIter (myAnimations); anIter.More(); anIter.Next())
  {
    myChildrenDuration = Max (myChildrenDuration, anIter.Value()->StartPts() + anIter.Value()->Duration());
  }
}

//=============================================================================
//function : StartTimer
//purpose  :
//=============================================================================
void AIS_Animation::StartTimer (const Standard_Real    theStartPts,
                                const Standard_Real    thePlaySpeed,
                                const Standard_Boolean theToUpdate,
                                const Standard_Boolean theToStopTimer)
{
  if (myTimer.IsNull())
  {
    myTimer = new AIS_AnimationTimer();
  }
  myTimer->Stop();
  myTimer->Seek (theStartPts);
  myTimer->SetPlaybackSpeed (thePlaySpeed);
  Start (theToUpdate);
  if (theToStopTimer)
  {
    myTimer->Stop();
    myTimer->Seek (theStartPts);
  }
}

//=============================================================================
//function : UpdateTimer
//purpose  :
//=============================================================================
Standard_Real AIS_Animation::UpdateTimer()
{
  if (myTimer.IsNull())
  {
    throw Standard_ProgramError("AIS_Animation::UpdateTimer() - timer was not created!");
  }

  const Standard_Real anElapsedTime = myTimer->ElapsedTime();
  Update (anElapsedTime);
  return anElapsedTime;
}

//=============================================================================
//function : Start
//purpose  :
//=============================================================================
void AIS_Animation::Start (const Standard_Boolean theToUpdate)
{
  UpdateTotalDuration();
  myState = AnimationState_Started;
  for (NCollection_Sequence<Handle(AIS_Animation)>::Iterator anIter (myAnimations); anIter.More(); anIter.Next())
  {
    anIter.ChangeValue()->Start (Standard_False);
  }

  if (theToUpdate)
  {
    const Standard_Real anElapsedTime = !myTimer.IsNull()
                                       ? myTimer->ElapsedTime()
                                       : 0.0;
    Update (anElapsedTime);
  }

  if (!myTimer.IsNull())
  {
    myTimer->Start();
  }
}

//=============================================================================
//function : Pause
//purpose  :
//=============================================================================
void AIS_Animation::Pause()
{
  myState = AnimationState_Paused;
  if (!myTimer.IsNull())
  {
    myTimer->Pause();
  }

  for (NCollection_Sequence<Handle(AIS_Animation)>::Iterator anIter (myAnimations); anIter.More(); anIter.Next())
  {
    anIter.ChangeValue()->Stop();
  }
}

//=============================================================================
//function : Stop
//purpose  :
//=============================================================================
void AIS_Animation::Stop()
{
  myState = AnimationState_Stopped;
  if (!myTimer.IsNull())
  {
    myTimer->Stop();
  }

  for (NCollection_Sequence<Handle(AIS_Animation)>::Iterator anIter (myAnimations); anIter.More(); anIter.Next())
  {
    anIter.ChangeValue()->Stop();
  }
}

//=============================================================================
//function : Update
//purpose  :
//=============================================================================
Standard_Boolean AIS_Animation::Update (const Standard_Real thePts)
{
  AIS_AnimationProgress aPosition;
  aPosition.Pts             = thePts;
  aPosition.LocalPts        = thePts - myPtsStart;
  aPosition.LocalNormalized = HasOwnDuration()
                            ? (aPosition.LocalPts / myOwnDuration)
                            : 0.0;
  aPosition.LocalNormalized = Max (0.0, aPosition.LocalNormalized);
  aPosition.LocalNormalized = Min (1.0, aPosition.LocalNormalized);
  updateWithChildren (aPosition);
  return thePts < myPtsStart + Duration();
}

//=============================================================================
//function : updateWithChildren
//purpose  :
//=============================================================================
void AIS_Animation::updateWithChildren (const AIS_AnimationProgress& thePosition)
{
  if (thePosition.LocalPts < 0.0
   || IsStopped())
  {
    return;
  }

  for (NCollection_Sequence<Handle(AIS_Animation)>::Iterator anIter (myAnimations); anIter.More(); anIter.Next())
  {
    const Handle(AIS_Animation)& anAnim = anIter.Value();
    AIS_AnimationProgress aPosition = thePosition;
    aPosition.LocalPts        = aPosition.LocalPts - anAnim->StartPts();
    aPosition.LocalNormalized = anAnim->HasOwnDuration()
                              ? (aPosition.LocalPts / anAnim->OwnDuration())
                              : 0.0;
    aPosition.LocalNormalized = Max (0.0, aPosition.LocalNormalized);
    aPosition.LocalNormalized = Min (1.0, aPosition.LocalNormalized);
    anAnim->updateWithChildren (aPosition);
  }

  if (thePosition.LocalPts >= Duration())
  {
    Stop();
  }

  update (thePosition);
}
