// Created on: 2013-09-05
// Created by: Anton POLETAEV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#include <OpenGl_Clipping.hxx>

#include <OpenGl_ClippingIterator.hxx>

// =======================================================================
// function : OpenGl_Clipping
// purpose  :
// =======================================================================
OpenGl_Clipping::OpenGl_Clipping()
: myCappedSubPlane (0),
  myNbClipping (0),
  myNbCapping  (0),
  myNbChains   (0),
  myNbDisabled (0)
{}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
void OpenGl_Clipping::Init()
{
  myPlanesGlobal.Nullify();
  myPlanesLocal.Nullify();

  myNbClipping = 0;
  myNbCapping  = 0;
  myNbChains   = 0;
  myNbDisabled = 0;
  myCappedSubPlane = 0;
  myCappedChain.Nullify();
}

// =======================================================================
// function : Reset
// purpose  :
// =======================================================================
void OpenGl_Clipping::Reset (const Handle(Graphic3d_SequenceOfHClipPlane)& thePlanes)
{
  const Standard_Integer aStartIndex = myPlanesGlobal.IsNull() ? 1 : myPlanesGlobal->Size() + 1;
  remove (myPlanesLocal,  aStartIndex);
  remove (myPlanesGlobal, 1);

  myPlanesGlobal = thePlanes;
  myPlanesLocal.Nullify();

  add (thePlanes, 1);
  myNbDisabled = 0;
  myCappedSubPlane = 0;
  myCappedChain.Nullify();

  // Method ::add() implicitly extends myDisabledPlanes (NCollection_Vector::SetValue()),
  // however we do not reset myDisabledPlanes and mySkipFilter beforehand to avoid redundant memory re-allocations.
  // So once extended, they will never reduce their size to lower values.
  // This should not be a problem since overall number of clipping planes is expected to be quite small.
}

// =======================================================================
// function : SetLocalPlanes
// purpose  :
// =======================================================================
void OpenGl_Clipping::SetLocalPlanes (const Handle(Graphic3d_SequenceOfHClipPlane)& thePlanes)
{
  const Standard_Integer aStartIndex = myPlanesGlobal.IsNull() ? 1 : myPlanesGlobal->Size() + 1;
  remove (myPlanesLocal, aStartIndex);

  myPlanesLocal = thePlanes;

  add (thePlanes, aStartIndex);
}

// =======================================================================
// function : add
// purpose  :
// =======================================================================
void OpenGl_Clipping::add (const Handle(Graphic3d_SequenceOfHClipPlane)& thePlanes,
                           const Standard_Integer theStartIndex)
{
  if (thePlanes.IsNull())
  {
    return;
  }

  Standard_Integer aPlaneId = theStartIndex;
  for (Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt (*thePlanes); aPlaneIt.More(); aPlaneIt.Next(), ++aPlaneId)
  {
    const Handle(Graphic3d_ClipPlane)& aPlane = aPlaneIt.Value();
    myDisabledPlanes.SetValue (aPlaneId, Standard_False); // automatically resizes the vector
    if (!aPlane->IsOn())
    {
      continue;
    }

    const Standard_Integer aNbSubPlanes = aPlane->NbChainNextPlanes();
    myNbChains += 1;
    if (aPlane->IsCapping())
    {
      myNbCapping += aNbSubPlanes;
    }
    else
    {
      myNbClipping += aNbSubPlanes;
    }
  }
}

// =======================================================================
// function : remove
// purpose  :
// =======================================================================
void OpenGl_Clipping::remove (const Handle(Graphic3d_SequenceOfHClipPlane)& thePlanes,
                              const Standard_Integer theStartIndex)
{
  if (thePlanes.IsNull())
  {
    return;
  }

  Standard_Integer aPlaneIndex = theStartIndex;
  for (Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt (*thePlanes); aPlaneIt.More(); aPlaneIt.Next(), ++aPlaneIndex)
  {
    const Handle(Graphic3d_ClipPlane)& aPlane = aPlaneIt.Value();
    if (!aPlane->IsOn()
     || myDisabledPlanes.Value (aPlaneIndex))
    {
      continue;
    }

    const Standard_Integer aNbSubPlanes = aPlane->NbChainNextPlanes();
    myNbChains -= 1;
    if (aPlane->IsCapping())
    {
      myNbCapping -= aNbSubPlanes;
    }
    else
    {
      myNbClipping -= aNbSubPlanes;
    }
  }
}

// =======================================================================
// function : SetEnabled
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Clipping::SetEnabled (const OpenGl_ClippingIterator& thePlane,
                                              const Standard_Boolean         theIsEnabled)
{
  const Standard_Integer aPlaneIndex = thePlane.PlaneIndex();
  Standard_Boolean& isDisabled = myDisabledPlanes.ChangeValue (aPlaneIndex);
  if (isDisabled == !theIsEnabled)
  {
    return Standard_False;
  }

  isDisabled = !theIsEnabled;
  const Standard_Integer aNbSubPlanes = thePlane.Value()->NbChainNextPlanes();
  if (thePlane.Value()->IsCapping())
  {
    myNbCapping += (theIsEnabled ? aNbSubPlanes : -aNbSubPlanes);
  }
  else
  {
    myNbClipping += (theIsEnabled ? aNbSubPlanes : -aNbSubPlanes);
  }
  myNbChains   += (theIsEnabled ? 1 : -1);
  myNbDisabled += (theIsEnabled ? -aNbSubPlanes : aNbSubPlanes);
  return Standard_True;
}

// =======================================================================
// function : RestoreDisabled
// purpose  :
// =======================================================================
void OpenGl_Clipping::RestoreDisabled()
{
  if (myNbDisabled == 0)
  {
    return;
  }

  myNbDisabled = 0;
  for (OpenGl_ClippingIterator aPlaneIter (*this); aPlaneIter.More(); aPlaneIter.Next())
  {
    Standard_Boolean& isDisabled = myDisabledPlanes.ChangeValue (aPlaneIter.PlaneIndex());
    if (!isDisabled)
    {
      continue;
    }

    isDisabled = Standard_False;
    const Handle(Graphic3d_ClipPlane)& aPlane = aPlaneIter.Value();
    const Standard_Integer aNbSubPlanes = aPlane->NbChainNextPlanes();
    myNbChains += 1;
    if (aPlane->IsCapping())
    {
      myNbCapping += aNbSubPlanes;
    }
    else
    {
      myNbClipping += aNbSubPlanes;
    }
  }
}

// =======================================================================
// function : DisableGlobal
// purpose  :
// =======================================================================
void OpenGl_Clipping::DisableGlobal()
{
  for (OpenGl_ClippingIterator aPlaneIter (*this); aPlaneIter.More(); aPlaneIter.Next())
  {
    if (!aPlaneIter.IsGlobal())
    {
      // local planes always follow global ones in iterator
      return;
    }

    SetEnabled (aPlaneIter, Standard_False);
  }
}

// =======================================================================
// function : DisableAllExcept
// purpose  :
// =======================================================================
void OpenGl_Clipping::DisableAllExcept (const Handle(Graphic3d_ClipPlane)& theChain,
                                        const Standard_Integer theSubPlaneIndex)
{
  myCappedChain = theChain;
  myCappedSubPlane = theSubPlaneIndex;
}

// =======================================================================
// function : EnableAllExcept
// purpose  :
// =======================================================================
void OpenGl_Clipping::EnableAllExcept (const Handle(Graphic3d_ClipPlane)& theChain,
                                       const Standard_Integer theSubPlaneIndex)
{
  myCappedChain = theChain;
  myCappedSubPlane = -theSubPlaneIndex;
}

// =======================================================================
// function : ResetCappingFilter
// purpose  :
// =======================================================================
void OpenGl_Clipping::ResetCappingFilter()
{
  myCappedChain.Nullify();
  myCappedSubPlane = 0;
}
