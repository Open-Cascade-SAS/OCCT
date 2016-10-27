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

#include <AIS_AnimationObject.hxx>

#include <AIS_InteractiveContext.hxx>
#include <TopLoc_Location.hxx>
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_AnimationObject, AIS_Animation)

//=============================================================================
//function : Constructor
//purpose  :
//=============================================================================
AIS_AnimationObject::AIS_AnimationObject (const TCollection_AsciiString& theAnimationName,
                                          const Handle(AIS_InteractiveContext)& theContext,
                                          const Handle(AIS_InteractiveObject)&  theObject,
                                          const gp_Trsf& theTrsfStart,
                                          const gp_Trsf& theTrsfEnd)
: AIS_Animation (theAnimationName),
  myContext  (theContext),
  myObject   (theObject),
  myTrsfLerp (theTrsfStart, theTrsfEnd)
{
  //
}

//=============================================================================
//function : update
//purpose  :
//=============================================================================
void AIS_AnimationObject::update (const AIS_AnimationProgress& theProgress)
{
  if (myObject.IsNull())
  {
    return;
  }

  gp_Trsf aTrsf;
  myTrsfLerp.Interpolate (theProgress.LocalNormalized, aTrsf);
  if (!myContext.IsNull())
  {
    myContext->SetLocation (myObject, aTrsf);
    invalidateViewer();
  }
  else
  {
    myObject->SetLocalTransformation (aTrsf);
  }
}

//=============================================================================
//function : invalidateViewer
//purpose  :
//=============================================================================
void AIS_AnimationObject::invalidateViewer()
{
  if (myContext.IsNull())
  {
    return;
  }

  const Standard_Boolean isImmediate = myContext->CurrentViewer()->ZLayerSettings (myObject->ZLayer()).IsImmediate();
  if (!isImmediate)
  {
    myContext->CurrentViewer()->Invalidate();
    return;
  }

  // Invalidate immediate view only if it is going out of z-fit range.
  // This might be sub-optimal performing this for each animated objects in case of many animated objects.
  for (V3d_ListOfView::Iterator aDefViewIter = myContext->CurrentViewer()->DefinedViewIterator();
       aDefViewIter.More(); aDefViewIter.Next())
  {
    const Handle(V3d_View)& aView = aDefViewIter.Value();
    const Bnd_Box aMinMaxBox  = aView->View()->MinMaxValues (Standard_False);
    const Bnd_Box aGraphicBox = aView->View()->MinMaxValues (Standard_True);
    Standard_Real aZNear = 0.0;
    Standard_Real aZFar  = 0.0;
    if (aView->Camera()->ZFitAll (aDefViewIter.Value()->AutoZFitScaleFactor(), aMinMaxBox, aGraphicBox, aZNear, aZFar))
    {
      if (aZNear < aView->Camera()->ZNear()
       || aZFar  > aView->Camera()->ZFar())
      {
        aDefViewIter.Value()->Invalidate();
      }
    }
  }
}
