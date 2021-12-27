// Copyright (c) 2023 OPEN CASCADE SAS
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

#include <AIS_BaseAnimationObject.hxx>

#include <V3d_View.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_BaseAnimationObject, AIS_Animation)

//=============================================================================
//function : Constructor
//purpose  :
//=============================================================================
AIS_BaseAnimationObject::AIS_BaseAnimationObject (const TCollection_AsciiString& theAnimationName,
                                                  const Handle(AIS_InteractiveContext)& theContext,
                                                  const Handle(AIS_InteractiveObject)&  theObject)
: AIS_Animation (theAnimationName),
  myContext  (theContext),
  myObject   (theObject)
{
  //
}

//=============================================================================
//function : updateTrsf
//purpose  :
//=============================================================================
void AIS_BaseAnimationObject::updateTrsf (const gp_Trsf& theTrsf)
{
  if (!myContext.IsNull())
  {
    myContext->SetLocation (myObject, theTrsf);
    invalidateViewer();
  }
  else
  {
    myObject->SetLocalTransformation (theTrsf);
  }
}

//=============================================================================
//function : invalidateViewer
//purpose  :
//=============================================================================
void AIS_BaseAnimationObject::invalidateViewer()
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
