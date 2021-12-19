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

IMPLEMENT_STANDARD_RTTIEXT(AIS_MultipleConnectedInteractive,AIS_InteractiveObject)

//=======================================================================
//function : AIS_MultipleConnectedInteractive
//purpose  :
//=======================================================================

AIS_MultipleConnectedInteractive::AIS_MultipleConnectedInteractive()
: AIS_InteractiveObject (PrsMgr_TOP_AllView)
{
  myHasOwnPresentations = Standard_False;
}

//=======================================================================
//function : connect
//purpose  :
//=======================================================================
Handle(AIS_InteractiveObject) AIS_MultipleConnectedInteractive::connect (const Handle(AIS_InteractiveObject)& theAnotherObj,
                                                                         const Handle(TopLoc_Datum3D)& theTrsf,
                                                                         const Handle(Graphic3d_TransformPers)& theTrsfPers)
{
  if (myAssemblyOwner.IsNull())
    myAssemblyOwner = new SelectMgr_EntityOwner (this);

  Handle(AIS_InteractiveObject) anObjectToAdd;

  Handle(AIS_MultipleConnectedInteractive) aMultiConnected = Handle(AIS_MultipleConnectedInteractive)::DownCast (theAnotherObj);
  if (!aMultiConnected.IsNull())
  { 
    Handle(AIS_MultipleConnectedInteractive) aNewMultiConnected = new AIS_MultipleConnectedInteractive();
    aNewMultiConnected->myAssemblyOwner = myAssemblyOwner;
    aNewMultiConnected->SetLocalTransformation (aMultiConnected->LocalTransformationGeom());

    // Perform deep copy of instance tree
    for (PrsMgr_ListOfPresentableObjectsIter anIter (aMultiConnected->Children()); anIter.More(); anIter.Next())
    {
      Handle(AIS_InteractiveObject) anInteractive = Handle(AIS_InteractiveObject)::DownCast (anIter.Value());
      if (anInteractive.IsNull())
      {
        continue;
      }

      aNewMultiConnected->Connect (anInteractive);     
    }

    anObjectToAdd = aNewMultiConnected;
  }
  else
  {
    Handle(AIS_ConnectedInteractive) aNewConnected = new AIS_ConnectedInteractive();
    aNewConnected->Connect (theAnotherObj, theAnotherObj->LocalTransformationGeom());

    anObjectToAdd = aNewConnected;
  }

  anObjectToAdd->SetLocalTransformation (theTrsf);
  if (!theTrsfPers.IsNull())
  {
    anObjectToAdd->SetTransformPersistence (theTrsfPers);
  }
  AddChild (anObjectToAdd);
  return anObjectToAdd;
}

//=======================================================================
//function : HasConnection
//purpose  : 
//=======================================================================
Standard_Boolean AIS_MultipleConnectedInteractive::HasConnection() const 
{
  return (Children().Size() != 0);
}

//=======================================================================
//function : Disconnect
//purpose  : 
//=======================================================================

void AIS_MultipleConnectedInteractive::Disconnect(const Handle(AIS_InteractiveObject)& anotherIObj)
{
  RemoveChild (anotherIObj);
}

//=======================================================================
//function : DisconnectAll
//purpose  : 
//=======================================================================

void AIS_MultipleConnectedInteractive::DisconnectAll()
{
  Standard_Integer aNbItemsToRemove = Children().Size();
  for (Standard_Integer anIter = 0; anIter < aNbItemsToRemove; ++anIter)
  {
    RemoveChild (Children().First());
  }
}

//=======================================================================
//function : Compute
//purpose  :
//=======================================================================
void AIS_MultipleConnectedInteractive::Compute (const Handle(PrsMgr_PresentationManager)& ,
                                                const Handle(Prs3d_Presentation)& ,
                                                const Standard_Integer )
{
  Handle(AIS_InteractiveContext) aCtx = GetContext();
  for (PrsMgr_ListOfPresentableObjectsIter anIter (Children()); anIter.More(); anIter.Next())
  {
    Handle(AIS_InteractiveObject) aChild = Handle(AIS_InteractiveObject)::DownCast (anIter.Value());
    if (!aChild.IsNull())
    {
      aChild->SetContext (aCtx);
    }
  }
}

//=======================================================================
//function : AcceptShapeDecomposition
//purpose  : 
//=======================================================================
Standard_Boolean AIS_MultipleConnectedInteractive::AcceptShapeDecomposition() const 
{
  for (PrsMgr_ListOfPresentableObjectsIter anIter (Children()); anIter.More(); anIter.Next())
  {
    Handle(AIS_InteractiveObject) aChild = Handle(AIS_InteractiveObject)::DownCast (anIter.Value());
    if (aChild.IsNull())
    {
      continue;
    }

    if (aChild->AcceptShapeDecomposition())
    {
      return Standard_True;
    }
  }
  return Standard_False;
}

//=======================================================================
//function : ComputeSelection
//purpose  : 
//=======================================================================
void AIS_MultipleConnectedInteractive::ComputeSelection (const Handle(SelectMgr_Selection)& /*theSelection*/,
                                                         const Standard_Integer             theMode)
{
  if (theMode == 0)
  {
    return;
  }

  for (PrsMgr_ListOfPresentableObjectsIter anIter (Children()); anIter.More(); anIter.Next())
  {
    Handle(AIS_InteractiveObject) aChild = Handle(AIS_InteractiveObject)::DownCast (anIter.Value());
    if (aChild.IsNull())
    {
      continue;
    }

    if (!aChild->HasSelection (theMode))
    {
      aChild->RecomputePrimitives (theMode);
    }

    Handle(SelectMgr_Selection) aSelection = new SelectMgr_Selection (theMode);
    aChild->ComputeSelection (aSelection, theMode);
  }
}

//=======================================================================
//function : SetContext
//purpose  :
//=======================================================================
void AIS_MultipleConnectedInteractive::SetContext (const Handle(AIS_InteractiveContext)& theCtx)
{
  AIS_InteractiveObject::SetContext (theCtx);
  for (PrsMgr_ListOfPresentableObjectsIter anIter (Children()); anIter.More(); anIter.Next())
  {
    Handle(AIS_InteractiveObject) aChild = Handle(AIS_InteractiveObject)::DownCast (anIter.Value());
    if (!aChild.IsNull())
    {
      aChild->SetContext (theCtx);
    }
  }
}
