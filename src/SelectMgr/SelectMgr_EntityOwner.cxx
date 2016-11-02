// Created on: 1995-05-23
// Created by: Robert COUBLANC
// Copyright (c) 1995-1999 Matra Datavision
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

#include <SelectMgr_EntityOwner.hxx>

#include <PrsMgr_PresentationManager.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_Type.hxx>
#include <TopLoc_Location.hxx>

IMPLEMENT_STANDARD_RTTIEXT(SelectMgr_EntityOwner,SelectBasics_EntityOwner)

//==================================================
// Function: SelectMgr_EntityOwner
// Purpose :
//==================================================
SelectMgr_EntityOwner::SelectMgr_EntityOwner (const Standard_Integer thePriority)
: SelectBasics_EntityOwner (thePriority),
  mySelectable (NULL),
  myIsSelected (Standard_False),
  myFromDecomposition (Standard_False)
{
  //
}

//==================================================
// Function: SelectMgr_EntityOwner
// Purpose :
//==================================================
SelectMgr_EntityOwner::SelectMgr_EntityOwner (const Handle(SelectMgr_SelectableObject)& theSelObj,
                                              const Standard_Integer thePriority)
: SelectBasics_EntityOwner (thePriority),
  mySelectable (theSelObj.operator->()),
  myIsSelected (Standard_False),
  myFromDecomposition (Standard_False)
{
  //
}

//==================================================
// Function: SelectMgr_EntityOwner
// Purpose :
//==================================================
SelectMgr_EntityOwner::SelectMgr_EntityOwner (const Handle(SelectMgr_EntityOwner)& theOwner,
                                              const Standard_Integer thePriority)
: SelectBasics_EntityOwner (thePriority),
  mySelectable (theOwner->mySelectable),
  myIsSelected (Standard_False),
  myFromDecomposition (Standard_False)
{
  //
}

//=======================================================================
//function : SetSelectable
//purpose  :
//=======================================================================
void SelectMgr_EntityOwner::SetSelectable (const Handle(SelectMgr_SelectableObject)& theSelObj)
{
  mySelectable = theSelObj.operator->();
}

//=======================================================================
//function : Selectable
//purpose  :
//=======================================================================
Handle(SelectMgr_SelectableObject) SelectMgr_EntityOwner::Selectable() const
{  
  return mySelectable;
}

//=======================================================================
//function : IsHilighted
//purpose  :
//=======================================================================
Standard_Boolean SelectMgr_EntityOwner::IsHilighted (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                                                     const Standard_Integer theMode) const
{
  return mySelectable != NULL
      && thePrsMgr->IsHighlighted (mySelectable, theMode);
}

void SelectMgr_EntityOwner::HilightWithColor (const Handle(PrsMgr_PresentationManager3d)& thePM,
                                              const Handle(Prs3d_Drawer)& theStyle,
                                              const Standard_Integer theMode)
{
  if (!HasSelectable())
  {
    return;
  }

  if (IsAutoHilight())
  {
    const Graphic3d_ZLayerId aHiLayer = theStyle->ZLayer() != Graphic3d_ZLayerId_UNKNOWN ? theStyle->ZLayer() : mySelectable->ZLayer();
    thePM->Color (mySelectable, theStyle, theMode, NULL, aHiLayer);
  }
  else
  {
    mySelectable->HilightOwnerWithColor (thePM, theStyle, this);
  }
}

void SelectMgr_EntityOwner::Unhilight (const Handle(PrsMgr_PresentationManager)& thePrsMgr, const Standard_Integer )
{
  if (HasSelectable())
  {
    thePrsMgr->Unhighlight (mySelectable);
  }
}

void SelectMgr_EntityOwner::Clear(const Handle(PrsMgr_PresentationManager)&,
				  const Standard_Integer)
{
// nothing done on the selectable here...
}

//=======================================================================
//function : about Transformation
//purpose  : 
//=======================================================================

Standard_Boolean SelectMgr_EntityOwner::HasLocation() const
{
  return (HasSelectable() && mySelectable->HasTransformation());
}

void SelectMgr_EntityOwner::SetLocation(const TopLoc_Location&)
{
}

TopLoc_Location SelectMgr_EntityOwner::Location() const
{
  return !HasLocation() ? TopLoc_Location() : TopLoc_Location(mySelectable->Transformation());
}

void SelectMgr_EntityOwner::ResetLocation()
{
}

Standard_Boolean SelectMgr_EntityOwner::IsAutoHilight () const
{
  return mySelectable == NULL
      || mySelectable->IsAutoHilight();
}

Standard_Boolean SelectMgr_EntityOwner::IsForcedHilight () const
{
  return Standard_False;
}

//=======================================================================
//function : SetZLayer
//purpose  :
//=======================================================================
void SelectMgr_EntityOwner::SetZLayer (const Standard_Integer )
{
  //
}

//=======================================================================
//function : UpdateHighlightTrsf
//purpose  :
//=======================================================================
void SelectMgr_EntityOwner::UpdateHighlightTrsf (const Handle(V3d_Viewer)& theViewer,
                                                 const Handle(PrsMgr_PresentationManager3d)& theManager,
                                                 const Standard_Integer theDispMode)
{
  if (mySelectable == NULL)
    return;

  theManager->UpdateHighlightTrsf (theViewer, mySelectable, theDispMode);
}
