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


#include <AIS_ConnectedInteractive.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_MultipleConnectedInteractive.hxx>
#include <Geom_Transformation.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <NCollection_DataMap.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Projector.hxx>
#include <PrsMgr_ModedPresentation.hxx>
#include <PrsMgr_Presentation.hxx>
#include <Select3D_SensitiveEntity.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Type.hxx>
#include <TopLoc_Location.hxx>

namespace
{
  //! SelectMgr_AssemblyEntityOwner replaces original owners in sensitive entities
  //! copied from reference objects to AIS_MultipleConnectedInteractive in order to
  //! redirect all selection queries to multiply connected (assembly).
  class SelectMgr_AssemblyEntityOwner : public SelectMgr_EntityOwner
  {

  public:

    // Copies another SelectMgr_EntityOwner.
    SelectMgr_AssemblyEntityOwner (const Handle(SelectMgr_EntityOwner) theOwner,
                                   SelectMgr_SelectableObject* theAssembly);

    void SetAssembly (SelectMgr_SelectableObject* theAssembly)
    {
      myAssembly = theAssembly;
    }

    //! Selectable() method modified to return myAssembly.
    virtual Handle(SelectMgr_SelectableObject) Selectable() const;

    Standard_Boolean IsHilighted (const Handle(PrsMgr_PresentationManager)& PM,const Standard_Integer aMode) const;

    void Hilight (const Handle(PrsMgr_PresentationManager)& PM,const Standard_Integer aMode);

    void HilightWithColor (const Handle(PrsMgr_PresentationManager3d)& PM,
                           const Quantity_NameOfColor aColor,
                           const Standard_Integer aMode);

    void Unhilight (const Handle(PrsMgr_PresentationManager)& PM, const Standard_Integer aMode);

  private:

    SelectMgr_SelectableObject* myAssembly;
  };

}

//=======================================================================
//function : SelectMgr_AssemblyEntityOwner
//purpose  : 
//=======================================================================
SelectMgr_AssemblyEntityOwner::SelectMgr_AssemblyEntityOwner (const Handle(SelectMgr_EntityOwner) theOwner,
                                                              SelectMgr_SelectableObject* theAssembly)
:
  SelectMgr_EntityOwner (theOwner),
  myAssembly (theAssembly)
{
}

//=======================================================================
//function : Selectable
//purpose  : 
//=======================================================================
Handle(SelectMgr_SelectableObject) SelectMgr_AssemblyEntityOwner::Selectable() const
{  
  return myAssembly;
}

//=======================================================================
//function : IsHilighted
//purpose  : 
//=======================================================================
Standard_Boolean SelectMgr_AssemblyEntityOwner::IsHilighted (const Handle(PrsMgr_PresentationManager)& PM,
                                                             const Standard_Integer aMode) const 
{
  if (HasSelectable())
  {
   return PM->IsHighlighted (myAssembly, aMode);
  }

  return Standard_False;
}

//=======================================================================
//function : Hilight
//purpose  : 
//=======================================================================
void SelectMgr_AssemblyEntityOwner::Hilight (const Handle(PrsMgr_PresentationManager)& PM,
                                             const Standard_Integer aMode)
{
  if (HasSelectable())
  {
   PM->Highlight (myAssembly, aMode);
  }
}

//=======================================================================
//function : HilightWithColor
//purpose  : 
//=======================================================================
void SelectMgr_AssemblyEntityOwner::HilightWithColor (const Handle(PrsMgr_PresentationManager3d)& PM,
                                                      const Quantity_NameOfColor aColor,
                                                      const Standard_Integer aMode)
{
  if (HasSelectable())
  {
    if (IsAutoHilight())
    {
      PM->Color (myAssembly, aColor, aMode);
    }
    else
    {
      myAssembly->HilightOwnerWithColor (PM, aColor, this);
    }
  }
}

//=======================================================================
//function : Unhilight
//purpose  : 
//=======================================================================
void SelectMgr_AssemblyEntityOwner::Unhilight (const Handle(PrsMgr_PresentationManager)& PM,
                                               const Standard_Integer aMode)
{
  if (HasSelectable())
  {
    PM->Unhighlight (myAssembly, aMode);
  }
}


//=======================================================================
//function : AIS_MultipleConnectedInteractive
//purpose  : 
//=======================================================================

AIS_MultipleConnectedInteractive::AIS_MultipleConnectedInteractive()
  : AIS_InteractiveObject (PrsMgr_TOP_AllView)
{
  myHasOwnPresentations = Standard_False;
  myAssemblyOwner = NULL;
  SetHilightMode (0);
}

//=======================================================================
//function : Type
//purpose  : 
//=======================================================================
AIS_KindOfInteractive AIS_MultipleConnectedInteractive::Type() const
{
  return AIS_KOI_Object;
}

//=======================================================================
//function : Signature
//purpose  : 
//=======================================================================
Standard_Integer AIS_MultipleConnectedInteractive::Signature() const
{
  return 1;
}

//=======================================================================
//function : Connect
//purpose  :
//=======================================================================
Handle(AIS_InteractiveObject) AIS_MultipleConnectedInteractive::Connect (const Handle(AIS_InteractiveObject)& theAnotherObj,
                                                                         const gp_Trsf&                       theTransformation,
                                                                         const Graphic3d_TransModeFlags&      theTrsfPersFlag,
                                                                         const gp_Pnt&                        theTrsfPersPoint)
{
  if (myAssemblyOwner.IsNull())
    myAssemblyOwner = new SelectMgr_EntityOwner (this);

  Handle(AIS_InteractiveObject) anObjectToAdd;

  Handle(AIS_MultipleConnectedInteractive) aMultiConnected = Handle(AIS_MultipleConnectedInteractive)::DownCast (theAnotherObj);
  if (!aMultiConnected.IsNull())
  { 
    Handle(AIS_MultipleConnectedInteractive) aNewMultiConnected = new AIS_MultipleConnectedInteractive();
    aNewMultiConnected->myAssemblyOwner = myAssemblyOwner;
    aNewMultiConnected->SetLocalTransformation (aMultiConnected->LocalTransformation());

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
    aNewConnected->Connect (theAnotherObj);
    aNewConnected->SetLocalTransformation (theAnotherObj->LocalTransformation());

    anObjectToAdd = aNewConnected;
  }

  anObjectToAdd->SetLocalTransformation (theTransformation);
  if (theTrsfPersFlag != Graphic3d_TMF_None)
  {
    anObjectToAdd->SetTransformPersistence (theTrsfPersFlag, theTrsfPersPoint);
  }
  AddChild (anObjectToAdd);
  return anObjectToAdd;
}

//=======================================================================
//function : Connect
//purpose  :
//=======================================================================
Handle(AIS_InteractiveObject) AIS_MultipleConnectedInteractive::Connect (const Handle(AIS_InteractiveObject)& theAnotherObj)
{
  return Connect (theAnotherObj, theAnotherObj->LocalTransformation(),
                  theAnotherObj->GetTransformPersistenceMode(),
                  theAnotherObj->GetTransformPersistencePoint());
}

//=======================================================================
//function : Connect
//purpose  :
//=======================================================================
Handle(AIS_InteractiveObject) AIS_MultipleConnectedInteractive::Connect (const Handle(AIS_InteractiveObject)& theAnotherObj,
                                                                         const gp_Trsf&                       theTransformation)
{
  return Connect (theAnotherObj, theTransformation,
                  theAnotherObj->GetTransformPersistenceMode(),
                  theAnotherObj->GetTransformPersistencePoint());
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
void AIS_MultipleConnectedInteractive::Compute (const Handle(PrsMgr_PresentationManager3d)& /*thePrsMgr*/,
                                                const Handle(Prs3d_Presentation)&           /*thePrs*/,
                                                const Standard_Integer                      /*theMode*/)
{
  for (PrsMgr_ListOfPresentableObjectsIter anIter (Children()); anIter.More(); anIter.Next())
  {
    Handle(AIS_InteractiveObject) aChild = Handle(AIS_InteractiveObject)::DownCast (anIter.Value());
    if (aChild.IsNull())
    {
      continue;
    }

    if (!aChild->HasInteractiveContext())
    {
      aChild->SetContext (GetContext());
    }
  }
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void AIS_MultipleConnectedInteractive::Compute(const Handle(Prs3d_Projector)& aProjector,
                                               const Handle(Prs3d_Presentation)& aPresentation)
{
  PrsMgr_PresentableObject::Compute( aProjector , aPresentation ) ;
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void AIS_MultipleConnectedInteractive::Compute(const Handle(Prs3d_Projector)& aProjector,
                                               const Handle(Geom_Transformation)& aTransformation,
                                               const Handle(Prs3d_Presentation)& aPresentation)
{
  PrsMgr_PresentableObject::Compute( aProjector , aTransformation , aPresentation ) ;
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
  if (theMode != 0)
  {
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
}

//=======================================================================
//function : GlobalSelOwner
//purpose  :
//=======================================================================
Handle(SelectMgr_EntityOwner) AIS_MultipleConnectedInteractive::GlobalSelOwner() const
{
  return myAssemblyOwner;
}
