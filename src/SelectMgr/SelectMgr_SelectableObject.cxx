// Created on: 1995-02-20
// Created by: Mister rmi
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

#include <SelectMgr_SelectableObject.hxx>

#include <Aspect_TypeOfMarker.hxx>
#include <Bnd_Box.hxx>
#include <gp_Pnt.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PlaneAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <PrsMgr_PresentableObjectPointer.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <Select3D_SensitiveEntity.hxx>
#include <SelectBasics_EntityOwner.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_IndexedMapOfOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <SelectMgr_SelectionManager.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Type.hxx>
#include <TopLoc_Location.hxx>

IMPLEMENT_STANDARD_RTTIEXT(SelectMgr_SelectableObject,PrsMgr_PresentableObject)

static Standard_Integer Search (const SelectMgr_SequenceOfSelection& seq,
                                const Handle (SelectMgr_Selection)& theSel)
{
  Standard_Integer ifound=0;
  for (Standard_Integer i=1;i<=seq.Length()&& ifound==0;i++)
    {if(theSel == seq.Value(i)) ifound=i;}
  return ifound;
} 

//==================================================
// Function: SelectMgr_SelectableObject
// Purpose :
//==================================================

SelectMgr_SelectableObject::SelectMgr_SelectableObject (const PrsMgr_TypeOfPresentation3d aTypeOfPresentation3d)
: PrsMgr_PresentableObject (aTypeOfPresentation3d),
  myAssemblyOwner          (NULL),
  myAutoHilight            (Standard_True),
  myGlobalSelMode          (0)
{
  //
}

//==================================================
// Function: Destructor
// Purpose : Clears all selections of the object
//==================================================
SelectMgr_SelectableObject::~SelectMgr_SelectableObject()
{
  for (Standard_Integer aSelIdx = 1; aSelIdx <= myselections.Length(); ++aSelIdx)
  {
    myselections.Value (aSelIdx)->Clear();
  }
}

//==================================================
// Function: HasSelection
// Purpose :
//==================================================
Standard_Boolean SelectMgr_SelectableObject::HasSelection (const Standard_Integer theMode) const
{
  for (Standard_Integer aSelIdx = 1; aSelIdx <= myselections.Length(); ++aSelIdx)
  {
    if (((myselections.Value (aSelIdx))->Mode()) == theMode)
      return Standard_True;
  }
  return Standard_False;
}

//==================================================
// Function: RecomputePrimitives
// Purpose : IMPORTANT: Do not use this method to update
//           selection primitives except implementing custom
//           selection manager! This method does not take
//           into account necessary BVH updates, but may
//           invalidate the pointers it refers to.
//           TO UPDATE SELECTION properly from outside classes,
//           use method UpdateSelection.
//==================================================
void SelectMgr_SelectableObject::RecomputePrimitives()
{
  for (Standard_Integer aSelIdx = 1; aSelIdx <= myselections.Length(); aSelIdx++)
    {
      RecomputePrimitives (myselections.ChangeValue (aSelIdx)->Mode());
    }
}

//==================================================
// Function: RecomputePrimitives
// Purpose : IMPORTANT: Do not use this method to update
//           selection primitives except implementing custom
//           selection manager! This method does not take
//           into account necessary BVH updates, but may
//           invalidate the pointers it refers to.
//           TO UPDATE SELECTION properly from outside classes,
//           use method UpdateSelection.
//==================================================
void SelectMgr_SelectableObject::RecomputePrimitives (const Standard_Integer theMode)
{
  Handle(PrsMgr_PresentableObject) aPrsParent (Parent());
  Handle(SelectMgr_SelectableObject) aSelParent = Handle(SelectMgr_SelectableObject)::DownCast (aPrsParent);

  for (Standard_Integer aSelIdx =1; aSelIdx <= myselections.Length(); aSelIdx++ )
  {
    if (myselections.Value (aSelIdx)->Mode() == theMode)
    {
      myselections (aSelIdx)->Clear();
      ComputeSelection (myselections (aSelIdx), theMode);
      myselections (aSelIdx)->UpdateStatus (SelectMgr_TOU_Partial);
      myselections (aSelIdx)->UpdateBVHStatus (SelectMgr_TBU_Renew);
      if (theMode == 0 && ! aSelParent.IsNull() && ! aSelParent->GetAssemblyOwner().IsNull())
      {
        SetAssemblyOwner (aSelParent->GetAssemblyOwner(), theMode);
      }
      return;
    }
  }

  Handle(SelectMgr_Selection) aNewSel = new SelectMgr_Selection (theMode);
  ComputeSelection (aNewSel, theMode);

  if (theMode == 0 && ! aSelParent.IsNull() && ! aSelParent->GetAssemblyOwner().IsNull())
  {
    SetAssemblyOwner (aSelParent->GetAssemblyOwner(), theMode);
  }

  aNewSel->UpdateStatus (SelectMgr_TOU_Partial);
  aNewSel->UpdateBVHStatus (SelectMgr_TBU_Add);

  myselections.Append (aNewSel);
}

//==================================================
// Function: ClearSelections
// Purpose :
//==================================================
void SelectMgr_SelectableObject::ClearSelections(const Standard_Boolean update)
{
  for (Standard_Integer i =1; i<= myselections.Length(); i++ ) {
    myselections.Value(i)->Clear();
    myselections.Value (i)->UpdateBVHStatus (SelectMgr_TBU_Remove);
    if(update)
    {
      myselections.Value(i)->UpdateStatus(SelectMgr_TOU_Full);
    }
  }
}


//==================================================
// Function: Selection
// Purpose :
//==================================================

const Handle(SelectMgr_Selection)& SelectMgr_SelectableObject
::Selection(const Standard_Integer aMode) const
{
  Standard_Boolean Found = Standard_False;
  Standard_Integer Rank=0;
  for (Standard_Integer i=1;i<=myselections.Length() && !Found;i++)
    {
      if((myselections.Value(i))->Mode()==aMode){ Found = Standard_True;
                                                  Rank=i;}}
  return myselections.Value(Rank);
}


//==================================================
// Function: AddSelection
// Purpose :
//==================================================

void SelectMgr_SelectableObject
::AddSelection(const Handle(SelectMgr_Selection)& aSel,
               const Standard_Integer aMode)
{
  Standard_Boolean isReplaced = Standard_False;
  if(aSel->IsEmpty())
  {
    ComputeSelection(aSel, aMode);
    aSel->UpdateStatus(SelectMgr_TOU_Partial);
    aSel->UpdateBVHStatus (SelectMgr_TBU_Add);
  }
  if (HasSelection(aMode))
  {
    const Handle(SelectMgr_Selection)& temp= Selection(aMode);
    Standard_Integer I = Search(myselections,temp);
    if(I!=0)
    {
      myselections.Remove(I);
      isReplaced = Standard_True;
    }
  }

  myselections.Append(aSel);
  if (isReplaced)
  {
    myselections.Last()->UpdateBVHStatus (SelectMgr_TBU_Renew);
  }

  if (aMode == 0)
  {
    Handle(PrsMgr_PresentableObject) aPrsParent (Parent());
    Handle(SelectMgr_SelectableObject) aSelParent = Handle(SelectMgr_SelectableObject)::DownCast (aPrsParent);
    if (! aSelParent.IsNull() && ! aSelParent->GetAssemblyOwner().IsNull())
    {
      SetAssemblyOwner (aSelParent->GetAssemblyOwner(), aMode);
    }
  }
}



//=======================================================================
//function : ReSetTransformation
//purpose  : 
//=======================================================================
void SelectMgr_SelectableObject::ResetTransformation() 
{
  for (Init(); More(); Next())
  {
    const Handle(SelectMgr_Selection) & aSel = CurrentSelection();
    for (aSel->Init(); aSel->More(); aSel->Next())
    {
      aSel->UpdateStatus(SelectMgr_TOU_Partial);
      aSel->UpdateBVHStatus (SelectMgr_TBU_None);
    }
  }

  PrsMgr_PresentableObject::ResetTransformation();
}

//=======================================================================
//function : UpdateTransformation
//purpose  : 
//=======================================================================
void SelectMgr_SelectableObject::UpdateTransformation()
{
  for (Init(); More(); Next())
  {
    CurrentSelection()->UpdateStatus (SelectMgr_TOU_Partial);
  }

  PrsMgr_PresentableObject::UpdateTransformation();
}

//=======================================================================
//function : UpdateTransformation
//purpose  : 
//=======================================================================
void SelectMgr_SelectableObject::UpdateTransformations(const Handle(SelectMgr_Selection)& Sel)
{
  TopLoc_Location aSelfLocation (Transformation());
  Handle(Select3D_SensitiveEntity) SE;
  if(aSelfLocation.IsIdentity()) return;
  for(Sel->Init();Sel->More();Sel->Next()){
    SE =  Handle(Select3D_SensitiveEntity)::DownCast (Sel->Sensitive()->BaseSensitive());
    if(!SE.IsNull()){
      const Handle(SelectBasics_EntityOwner)& aEOwner = SE->OwnerId();
      Handle(SelectMgr_EntityOwner) aMgrEO =
                              Handle(SelectMgr_EntityOwner)::DownCast (aEOwner);
      if (!aMgrEO.IsNull())
        aMgrEO->SetLocation (aSelfLocation);
    }
  }
}

//=======================================================================
//function : HilightSelected
//purpose  : 
//=======================================================================
void SelectMgr_SelectableObject::HilightSelected 
  ( const Handle(PrsMgr_PresentationManager3d)&,
    const SelectMgr_SequenceOfOwner&)
{
  throw Standard_NotImplemented("SelectMgr_SelectableObject::HilightSelected");
}

//=======================================================================
//function : ClearSelected
//purpose  : 
//=======================================================================
void SelectMgr_SelectableObject::ClearSelected ()
{
  if( !mySelectionPrs.IsNull() )
    mySelectionPrs->Clear();
}

//=======================================================================
//function : ClearDynamicHighlight
//purpose  :
//=======================================================================
void SelectMgr_SelectableObject::ClearDynamicHighlight (const Handle(PrsMgr_PresentationManager3d)& theMgr)
{
  theMgr->ClearImmediateDraw();
}

//=======================================================================
//function : HilightOwnerWithColor
//purpose  : 
//=======================================================================
void SelectMgr_SelectableObject::HilightOwnerWithColor (const Handle(PrsMgr_PresentationManager3d)&,
                                                        const Handle(Prs3d_Drawer)&,
                                                        const Handle(SelectMgr_EntityOwner)&)
{
  throw Standard_NotImplemented("SelectMgr_SelectableObject::HilightOwnerWithColor");
}

//=======================================================================
//function : MaxFaceNodes
//purpose  : 
//=======================================================================
Standard_Boolean SelectMgr_SelectableObject::IsAutoHilight () const
{
  return myAutoHilight;
}

//=======================================================================
//function : MaxFaceNodes
//purpose  : 
//=======================================================================
void SelectMgr_SelectableObject::SetAutoHilight ( const Standard_Boolean newAutoHilight )
{
  myAutoHilight = newAutoHilight;
}

//=======================================================================
//function : GetHilightPresentation
//purpose  : 
//=======================================================================
Handle(Prs3d_Presentation) SelectMgr_SelectableObject::GetHilightPresentation (const Handle(PrsMgr_PresentationManager3d)& theMgr)
{
  if (myHilightPrs.IsNull() && !theMgr.IsNull())
  {
    myHilightPrs = new Prs3d_Presentation (theMgr->StructureManager());
    myHilightPrs->SetTransformPersistence (TransformPersistence());
  }

  return myHilightPrs;
}


//=======================================================================
//function : GetSelectPresentation
//purpose  : 
//=======================================================================
Handle(Prs3d_Presentation) SelectMgr_SelectableObject::GetSelectPresentation (const Handle(PrsMgr_PresentationManager3d)& theMgr)
{
  if (mySelectionPrs.IsNull() && !theMgr.IsNull())
  {
    mySelectionPrs = new Prs3d_Presentation (theMgr->StructureManager());
    mySelectionPrs->SetTransformPersistence (TransformPersistence());
  }

  return mySelectionPrs;
}

//=======================================================================
//function : ErasePresentations
//purpose  :
//=======================================================================
void SelectMgr_SelectableObject::ErasePresentations (Standard_Boolean theToRemove)
{
  if (!mySelectionPrs.IsNull())
  {
    mySelectionPrs->Erase();
    if (theToRemove)
    {
      mySelectionPrs->Clear();
      mySelectionPrs.Nullify();
    }
  }
  if (!myHilightPrs.IsNull())
  {
    myHilightPrs->Erase();
    if (theToRemove)
    {
      myHilightPrs->Clear();
      myHilightPrs.Nullify();
    }
  }
}

//=======================================================================
//function : SetZLayer
//purpose  :
//=======================================================================
void SelectMgr_SelectableObject::SetZLayer (const Graphic3d_ZLayerId theLayerId)
{
  // update own presentations
  PrsMgr_PresentableObject::SetZLayer (theLayerId);

  // update selection presentations
  if (!mySelectionPrs.IsNull())
    mySelectionPrs->SetZLayer (theLayerId);

  if (!myHilightPrs.IsNull())
    myHilightPrs->SetZLayer (theLayerId);

  // update all entity owner presentations
  for (Init (); More () ;Next ())
  {
    const Handle(SelectMgr_Selection)& aSel = CurrentSelection();
    for (aSel->Init (); aSel->More (); aSel->Next ())
    {
      Handle(Select3D_SensitiveEntity) aEntity = 
        Handle(Select3D_SensitiveEntity)::DownCast (aSel->Sensitive()->BaseSensitive());
      if (!aEntity.IsNull())
      {
        Handle(SelectMgr_EntityOwner) aOwner = 
          Handle(SelectMgr_EntityOwner)::DownCast (aEntity->OwnerId());
        if (!aOwner.IsNull())
          aOwner->SetZLayer (theLayerId);
      }
    }
  }
}

//=======================================================================
//function : updateSelection
//purpose  : Sets update status FULL to selections of the object. Must be
//           used as the only method of UpdateSelection from outer classes
//           to prevent BVH structures from being outdated.
//=======================================================================
void SelectMgr_SelectableObject::updateSelection (const Standard_Integer theMode)
{
  if (theMode == -1)
  {
    for (Init(); More(); Next())
    {
      const Handle(SelectMgr_Selection)& aSel = CurrentSelection();
      aSel->UpdateStatus (SelectMgr_TOU_Full);
    }

    return;
  }

  for (Init(); More(); Next())
  {
    if (CurrentSelection()->Mode() == theMode)
    {
      CurrentSelection()->UpdateStatus (SelectMgr_TOU_Full);
      return;
    }
  }
}

//=======================================================================
//function : SetAssemblyOwner
//purpose  : Sets common entity owner for assembly sensitive object entities
//=======================================================================
void SelectMgr_SelectableObject::SetAssemblyOwner (const Handle(SelectMgr_EntityOwner)& theOwner,
                                                   const Standard_Integer theMode)
{
  if (theMode == -1)
  {
    for (Standard_Integer aModeIter = 1; aModeIter <= myselections.Length(); ++aModeIter)
    {
      Handle(SelectMgr_Selection)& aSel = myselections.ChangeValue (aModeIter);
      for (aSel->Init(); aSel->More(); aSel->Next())
      {
        aSel->Sensitive()->BaseSensitive()->Set (theOwner);
      }
    }

    return;
  }

  if (!HasSelection (theMode))
    return;

  for (Standard_Integer aModeIter = 1; aModeIter <= myselections.Length(); ++aModeIter)
  {
    if (myselections.Value (aModeIter)->Mode() == theMode)
    {
      Handle(SelectMgr_Selection)& aSel = myselections.ChangeValue (aModeIter);
      for (aSel->Init(); aSel->More(); aSel->Next())
      {
        aSel->Sensitive()->BaseSensitive()->Set (theOwner);
      }
      return;
    }
  }
}

//=======================================================================
//function : GetAssemblyOwner
//purpose  : Returns common entity owner if it is an assembly
//=======================================================================
const Handle(SelectMgr_EntityOwner)& SelectMgr_SelectableObject::GetAssemblyOwner() const
{
  return myAssemblyOwner;
}

//=======================================================================
//function : BndBoxOfSelected
//purpose  :
//=======================================================================
Bnd_Box SelectMgr_SelectableObject::BndBoxOfSelected (const Handle(SelectMgr_IndexedMapOfOwner)& theOwners)
{
  if (theOwners->IsEmpty())
    return Bnd_Box();

  Bnd_Box aBnd;
  for (Init(); More(); Next())
  {
    const Handle(SelectMgr_Selection)& aSel = CurrentSelection();
    if (aSel->GetSelectionState() != SelectMgr_SOS_Activated)
      continue;

    for (aSel->Init(); aSel->More(); aSel->Next())
    {
      const Handle(SelectMgr_EntityOwner) anOwner =
        Handle(SelectMgr_EntityOwner)::DownCast (aSel->Sensitive()->BaseSensitive()->OwnerId());
      if (theOwners->Contains (anOwner))
      {
        Select3D_BndBox3d aBox = aSel->Sensitive()->BaseSensitive()->BoundingBox();
        aBnd.Update (aBox.CornerMin().x(), aBox.CornerMin().y(), aBox.CornerMin().z(),
                     aBox.CornerMax().x(), aBox.CornerMax().y(), aBox.CornerMax().z());
      }
    }
  }

  return aBnd;
}

//=======================================================================
//function : GlobalSelOwner
//purpose  : Returns entity owner corresponding to selection of the object as a whole
//=======================================================================
Handle(SelectMgr_EntityOwner) SelectMgr_SelectableObject::GlobalSelOwner() const
{
   Handle(SelectMgr_EntityOwner) anOwner;

  if (!HasSelection (myGlobalSelMode))
    return anOwner;

  const Handle(SelectMgr_Selection)& aGlobalSel = Selection (myGlobalSelMode);
  if (aGlobalSel->IsEmpty())
    return anOwner;

  aGlobalSel->Init();
  anOwner =
    Handle(SelectMgr_EntityOwner)::DownCast (aGlobalSel->Sensitive()->BaseSensitive()->OwnerId());

  return anOwner;
}
