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

#include <OpenGl_GlCore11.hxx>
#include <OpenGl_Workspace.hxx>
#include <OpenGl_Context.hxx>

// =======================================================================
// function : OpenGl_ClippingIterator
// purpose  :
// =======================================================================
OpenGl_ClippingIterator::OpenGl_ClippingIterator (const OpenGl_Clipping& theClipping)
: myDisabled  (&theClipping.myDisabledPlanes),
  myCurrIndex (1)
{
  if (!theClipping.myPlanesGlobal.IsNull())
  {
    myIter1.Init (*theClipping.myPlanesGlobal);
  }
  if (!theClipping.myPlanesLocal.IsNull())
  {
    myIter2.Init (*theClipping.myPlanesLocal);
  }
}

// =======================================================================
// function : OpenGl_ClippingState
// purpose  :
// =======================================================================
OpenGl_Clipping::OpenGl_Clipping ()
: myNbClipping (0),
  myNbCapping  (0),
  myNbDisabled (0)
{}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
void OpenGl_Clipping::Init (const Standard_Integer )
{
  myPlanesGlobal.Nullify();
  myPlanesLocal.Nullify();

  myNbClipping = 0;
  myNbCapping  = 0;
  myNbDisabled = 0;
}

// =======================================================================
// function : Reset
// purpose  :
// =======================================================================
void OpenGl_Clipping::Reset (const Handle(OpenGl_Context)& theGlCtx,
                             const Handle(Graphic3d_SequenceOfHClipPlane)& thePlanes)
{
  const Standard_Integer aStartIndex = myPlanesGlobal.IsNull() ? 1 : myPlanesGlobal->Size() + 1;
  remove (theGlCtx, myPlanesLocal,  aStartIndex);
  remove (theGlCtx, myPlanesGlobal, 1);

  myPlanesGlobal = thePlanes;
  myPlanesLocal.Nullify();

  add (theGlCtx, thePlanes, 1);
  myNbDisabled = 0;

  // Method ::add() implicitly extends myDisabledPlanes (NCollection_Vector::SetValue()),
  // however we do not reset myDisabledPlanes and mySkipFilter beforehand to avoid redundant memory re-allocations.
  // So once extended, they will never reduce their size to lower values.
  // This should not be a problem since overall number of clipping planes is expected to be quite small.
}

// =======================================================================
// function : SetLocalPlanes
// purpose  :
// =======================================================================
void OpenGl_Clipping::SetLocalPlanes (const Handle(OpenGl_Context)& theGlCtx,
                                      const Handle(Graphic3d_SequenceOfHClipPlane)& thePlanes)
{
  const Standard_Integer aStartIndex = myPlanesGlobal.IsNull() ? 1 : myPlanesGlobal->Size() + 1;
  remove (theGlCtx, myPlanesLocal, aStartIndex);

  myPlanesLocal = thePlanes;

  add (theGlCtx, thePlanes, aStartIndex);
}

// =======================================================================
// function : add
// purpose  :
// =======================================================================
void OpenGl_Clipping::add (const Handle(OpenGl_Context)& ,
                           const Handle(Graphic3d_SequenceOfHClipPlane)& thePlanes,
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

    if (aPlane->IsCapping())
    {
      ++myNbCapping;
    }
    else
    {
      ++myNbClipping;
    }
  }
}

// =======================================================================
// function : remove
// purpose  :
// =======================================================================
void OpenGl_Clipping::remove (const Handle(OpenGl_Context)& ,
                              const Handle(Graphic3d_SequenceOfHClipPlane)& thePlanes,
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

    if (aPlane->IsCapping())
    {
      --myNbCapping;
    }
    else
    {
      --myNbClipping;
    }
  }
}

// =======================================================================
// function : SetEnabled
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Clipping::SetEnabled (const Handle(OpenGl_Context)&  ,
                                              const OpenGl_ClippingIterator& thePlane,
                                              const Standard_Boolean         theIsEnabled)
{
  const Standard_Integer aPlaneIndex = thePlane.PlaneIndex();
  Standard_Boolean& isDisabled = myDisabledPlanes.ChangeValue (aPlaneIndex);
  if (isDisabled == !theIsEnabled)
  {
    return Standard_False;
  }

  isDisabled = !theIsEnabled;
  if (thePlane.Value()->IsCapping())
  {
    myNbCapping += (theIsEnabled ? 1 : -1);
  }
  else
  {
    myNbClipping += (theIsEnabled ? 1 : -1);
  }
  myNbDisabled -= (theIsEnabled ? 1 : -1);
  return Standard_True;
}

// =======================================================================
// function : RestoreDisabled
// purpose  :
// =======================================================================
void OpenGl_Clipping::RestoreDisabled (const Handle(OpenGl_Context)& )
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
    if (aPlane->IsCapping())
    {
      ++myNbCapping;
    }
    else
    {
      ++myNbClipping;
    }
  }
}

// =======================================================================
// function : DisableGlobal
// purpose  :
// =======================================================================
void OpenGl_Clipping::DisableGlobal (const Handle(OpenGl_Context)& theGlCtx)
{
  for (OpenGl_ClippingIterator aPlaneIter (*this); aPlaneIter.More(); aPlaneIter.Next())
  {
    if (!aPlaneIter.IsGlobal())
    {
      // local planes always follow global ones in iterator
      return;
    }

    SetEnabled (theGlCtx, aPlaneIter, Standard_False);
  }
}

// =======================================================================
// function : DisableAllExcept
// purpose  :
// =======================================================================
void OpenGl_Clipping::DisableAllExcept (const Handle(OpenGl_Context)&  theGlCtx,
                                        const OpenGl_ClippingIterator& thePlane)
{
  for (OpenGl_ClippingIterator aPlaneIter (*this); aPlaneIter.More(); aPlaneIter.Next())
  {
    if (aPlaneIter.IsDisabled())
    {
      mySkipFilter.SetValue (aPlaneIter.PlaneIndex(), Standard_True);
      continue;
    }

    const Standard_Boolean isOn = (aPlaneIter.PlaneIndex() == thePlane.PlaneIndex());
    SetEnabled (theGlCtx, aPlaneIter, isOn);
    mySkipFilter.SetValue (aPlaneIter.PlaneIndex(), Standard_False);
  }
}

// =======================================================================
// function : EnableAllExcept
// purpose  :
// =======================================================================
void OpenGl_Clipping::EnableAllExcept (const Handle(OpenGl_Context)&  theGlCtx,
                                       const OpenGl_ClippingIterator& thePlane)
{
  for (OpenGl_ClippingIterator aPlaneIter (*this); aPlaneIter.More(); aPlaneIter.Next())
  {
    if (mySkipFilter.Value (aPlaneIter.PlaneIndex()))
    {
      continue;
    }

    const Standard_Boolean isOn = (aPlaneIter.PlaneIndex() != thePlane.PlaneIndex());
    SetEnabled (theGlCtx, aPlaneIter, isOn);
  }
}
