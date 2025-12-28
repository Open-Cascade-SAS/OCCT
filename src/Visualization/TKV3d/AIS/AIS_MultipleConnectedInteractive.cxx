// Created on: 1997-04-22
// Created by: Guest Design
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <AIS_MultipleConnectedInteractive.hxx>

#include <AIS_ConnectedInteractive.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <SelectMgr_EntityOwner.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_MultipleConnectedInteractive, AIS_InteractiveObject)

//=================================================================================================

AIS_MultipleConnectedInteractive::AIS_MultipleConnectedInteractive()
    : AIS_InteractiveObject(PrsMgr_TOP_AllView)
{
  myHasOwnPresentations = false;
}

//=================================================================================================

occ::handle<AIS_InteractiveObject> AIS_MultipleConnectedInteractive::connect(
  const occ::handle<AIS_InteractiveObject>&   theAnotherObj,
  const occ::handle<TopLoc_Datum3D>&          theTrsf,
  const occ::handle<Graphic3d_TransformPers>& theTrsfPers)
{
  if (myAssemblyOwner.IsNull())
    myAssemblyOwner = new SelectMgr_EntityOwner(this);

  occ::handle<AIS_InteractiveObject> anObjectToAdd;

  occ::handle<AIS_MultipleConnectedInteractive> aMultiConnected =
    occ::down_cast<AIS_MultipleConnectedInteractive>(theAnotherObj);
  if (!aMultiConnected.IsNull())
  {
    occ::handle<AIS_MultipleConnectedInteractive> aNewMultiConnected =
      new AIS_MultipleConnectedInteractive();
    aNewMultiConnected->myAssemblyOwner = myAssemblyOwner;
    aNewMultiConnected->SetLocalTransformation(aMultiConnected->LocalTransformationGeom());

    // Perform deep copy of instance tree
    for (NCollection_List<occ::handle<PrsMgr_PresentableObject>>::Iterator anIter(
           aMultiConnected->Children());
         anIter.More();
         anIter.Next())
    {
      occ::handle<AIS_InteractiveObject> anInteractive =
        occ::down_cast<AIS_InteractiveObject>(anIter.Value());
      if (anInteractive.IsNull())
      {
        continue;
      }

      aNewMultiConnected->Connect(anInteractive);
    }

    anObjectToAdd = aNewMultiConnected;
  }
  else
  {
    occ::handle<AIS_ConnectedInteractive> aNewConnected = new AIS_ConnectedInteractive();
    aNewConnected->Connect(theAnotherObj, theAnotherObj->LocalTransformationGeom());

    anObjectToAdd = aNewConnected;
  }

  anObjectToAdd->SetLocalTransformation(theTrsf);
  if (!theTrsfPers.IsNull())
  {
    anObjectToAdd->SetTransformPersistence(theTrsfPers);
  }
  AddChild(anObjectToAdd);
  return anObjectToAdd;
}

//=================================================================================================

bool AIS_MultipleConnectedInteractive::HasConnection() const
{
  return (Children().Size() != 0);
}

//=================================================================================================

void AIS_MultipleConnectedInteractive::Disconnect(
  const occ::handle<AIS_InteractiveObject>& anotherIObj)
{
  RemoveChild(anotherIObj);
}

//=================================================================================================

void AIS_MultipleConnectedInteractive::DisconnectAll()
{
  int aNbItemsToRemove = Children().Size();
  for (int anIter = 0; anIter < aNbItemsToRemove; ++anIter)
  {
    RemoveChild(Children().First());
  }
}

//=================================================================================================

void AIS_MultipleConnectedInteractive::Compute(const occ::handle<PrsMgr_PresentationManager>&,
                                               const occ::handle<Prs3d_Presentation>&,
                                               const int)
{
  occ::handle<AIS_InteractiveContext> aCtx = GetContext();
  for (NCollection_List<occ::handle<PrsMgr_PresentableObject>>::Iterator anIter(Children());
       anIter.More();
       anIter.Next())
  {
    occ::handle<AIS_InteractiveObject> aChild =
      occ::down_cast<AIS_InteractiveObject>(anIter.Value());
    if (!aChild.IsNull())
    {
      aChild->SetContext(aCtx);
    }
  }
}

//=================================================================================================

bool AIS_MultipleConnectedInteractive::AcceptShapeDecomposition() const
{
  for (NCollection_List<occ::handle<PrsMgr_PresentableObject>>::Iterator anIter(Children());
       anIter.More();
       anIter.Next())
  {
    occ::handle<AIS_InteractiveObject> aChild =
      occ::down_cast<AIS_InteractiveObject>(anIter.Value());
    if (aChild.IsNull())
    {
      continue;
    }

    if (aChild->AcceptShapeDecomposition())
    {
      return true;
    }
  }
  return false;
}

//=================================================================================================

void AIS_MultipleConnectedInteractive::ComputeSelection(
  const occ::handle<SelectMgr_Selection>& /*theSelection*/,
  const int theMode)
{
  if (theMode == 0)
  {
    return;
  }

  for (NCollection_List<occ::handle<PrsMgr_PresentableObject>>::Iterator anIter(Children());
       anIter.More();
       anIter.Next())
  {
    occ::handle<AIS_InteractiveObject> aChild =
      occ::down_cast<AIS_InteractiveObject>(anIter.Value());
    if (aChild.IsNull())
    {
      continue;
    }

    if (!aChild->HasSelection(theMode))
    {
      aChild->RecomputePrimitives(theMode);
    }

    occ::handle<SelectMgr_Selection> aSelection = new SelectMgr_Selection(theMode);
    aChild->ComputeSelection(aSelection, theMode);
  }
}

//=================================================================================================

void AIS_MultipleConnectedInteractive::SetContext(const occ::handle<AIS_InteractiveContext>& theCtx)
{
  AIS_InteractiveObject::SetContext(theCtx);
  for (NCollection_List<occ::handle<PrsMgr_PresentableObject>>::Iterator anIter(Children());
       anIter.More();
       anIter.Next())
  {
    occ::handle<AIS_InteractiveObject> aChild =
      occ::down_cast<AIS_InteractiveObject>(anIter.Value());
    if (!aChild.IsNull())
    {
      aChild->SetContext(theCtx);
    }
  }
}
