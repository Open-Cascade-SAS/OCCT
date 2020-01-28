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

#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PlaneAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <PrsMgr_PresentationManager.hxx>
#include <Select3D_SensitiveEntity.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_IndexedMapOfOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <SelectMgr_SelectionManager.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NotImplemented.hxx>
#include <TopLoc_Location.hxx>

IMPLEMENT_STANDARD_RTTIEXT(SelectMgr_SelectableObject,PrsMgr_PresentableObject)

namespace
{
  static const Handle(SelectMgr_Selection)   THE_NULL_SELECTION;
  static const Handle(SelectMgr_EntityOwner) THE_NULL_ENTITYOWNER;
}

//==================================================
// Function: SelectMgr_SelectableObject
// Purpose :
//==================================================

SelectMgr_SelectableObject::SelectMgr_SelectableObject (const PrsMgr_TypeOfPresentation3d aTypeOfPresentation3d)
: PrsMgr_PresentableObject (aTypeOfPresentation3d),
  myGlobalSelMode          (0),
  myAutoHilight            (Standard_True)
{
  //
}

//==================================================
// Function: Destructor
// Purpose : Clears all selections of the object
//==================================================
SelectMgr_SelectableObject::~SelectMgr_SelectableObject()
{
  for (SelectMgr_SequenceOfSelection::Iterator aSelIter (myselections); aSelIter.More(); aSelIter.Next())
  {
    aSelIter.Value()->Clear();
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
void SelectMgr_SelectableObject::RecomputePrimitives()
{
  for (SelectMgr_SequenceOfSelection::Iterator aSelIter (myselections); aSelIter.More(); aSelIter.Next())
  {
    RecomputePrimitives (aSelIter.Value()->Mode());
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
  SelectMgr_SelectableObject* aSelParent = dynamic_cast<SelectMgr_SelectableObject* > (Parent());
  for (SelectMgr_SequenceOfSelection::Iterator aSelIter (myselections); aSelIter.More(); aSelIter.Next())
  {
    const Handle(SelectMgr_Selection)& aSel = aSelIter.Value();
    if (aSel->Mode() == theMode)
    {
      aSel->Clear();
      ComputeSelection (aSel, theMode);
      aSel->UpdateStatus (SelectMgr_TOU_Partial);
      aSel->UpdateBVHStatus (SelectMgr_TBU_Renew);
      if (theMode == 0 && aSelParent != NULL)
      {
        if (const Handle(SelectMgr_EntityOwner)& anAsmOwner = aSelParent->GetAssemblyOwner())
        {
          SetAssemblyOwner (anAsmOwner, theMode);
        }
      }
      return;
    }
  }

  Handle(SelectMgr_Selection) aNewSel = new SelectMgr_Selection (theMode);
  ComputeSelection (aNewSel, theMode);

  if (theMode == 0 && aSelParent != NULL)
  {
    if (const Handle(SelectMgr_EntityOwner)& anAsmOwner = aSelParent->GetAssemblyOwner())
    {
      SetAssemblyOwner (anAsmOwner, theMode);
    }
  }

  aNewSel->UpdateStatus (SelectMgr_TOU_Partial);
  aNewSel->UpdateBVHStatus (SelectMgr_TBU_Add);

  myselections.Append (aNewSel);
}

//==================================================
// Function: ClearSelections
// Purpose :
//==================================================
void SelectMgr_SelectableObject::ClearSelections (const Standard_Boolean theToUpdate)
{
  for (SelectMgr_SequenceOfSelection::Iterator aSelIter (myselections); aSelIter.More(); aSelIter.Next())
  {
    const Handle(SelectMgr_Selection)& aSel = aSelIter.Value();
    aSel->Clear();
    aSel->UpdateBVHStatus (SelectMgr_TBU_Remove);
    if (theToUpdate)
    {
      aSel->UpdateStatus (SelectMgr_TOU_Full);
    }
  }
}


//==================================================
// Function: Selection
// Purpose :
//==================================================

const Handle(SelectMgr_Selection)& SelectMgr_SelectableObject::Selection (const Standard_Integer theMode) const
{
  if (theMode == -1)
  {
    return THE_NULL_SELECTION;
  }

  for (SelectMgr_SequenceOfSelection::Iterator aSelIter (myselections); aSelIter.More(); aSelIter.Next())
  {
    const Handle(SelectMgr_Selection)& aSel = aSelIter.Value();
    if (aSel->Mode() == theMode)
    {
      return aSel;
    }
  }
  return THE_NULL_SELECTION;
}

//==================================================
// Function: AddSelection
// Purpose :
//==================================================

void SelectMgr_SelectableObject::AddSelection (const Handle(SelectMgr_Selection)& theSel,
                                               const Standard_Integer theMode)
{
  if(theSel->IsEmpty())
  {
    ComputeSelection(theSel, theMode);
    theSel->UpdateStatus(SelectMgr_TOU_Partial);
    theSel->UpdateBVHStatus (SelectMgr_TBU_Add);
  }

  Standard_Boolean isReplaced = Standard_False;
  for (SelectMgr_SequenceOfSelection::Iterator aSelIter (myselections); aSelIter.More(); aSelIter.Next())
  {
    if (aSelIter.Value()->Mode() == theMode)
    {
      isReplaced = Standard_True;
      myselections.Remove (aSelIter);
      break;
    }
  }

  myselections.Append (theSel);
  if (isReplaced)
  {
    myselections.Last()->UpdateBVHStatus (SelectMgr_TBU_Renew);
  }

  if (theMode == 0)
  {
    SelectMgr_SelectableObject* aSelParent = dynamic_cast<SelectMgr_SelectableObject* > (Parent());
    if (aSelParent != NULL)
    {
      if (const Handle(SelectMgr_EntityOwner)& anAsmOwner = aSelParent->GetAssemblyOwner())
      {
        SetAssemblyOwner (anAsmOwner, theMode);
      }
    }
  }
}

//=======================================================================
//function : ReSetTransformation
//purpose  :
//=======================================================================
void SelectMgr_SelectableObject::ResetTransformation() 
{
  for (SelectMgr_SequenceOfSelection::Iterator aSelIter (myselections); aSelIter.More(); aSelIter.Next())
  {
    const Handle(SelectMgr_Selection)& aSel = aSelIter.Value();
    aSel->UpdateStatus (SelectMgr_TOU_Partial);
    aSel->UpdateBVHStatus (SelectMgr_TBU_None);
  }

  PrsMgr_PresentableObject::ResetTransformation();
}

//=======================================================================
//function : UpdateTransformation
//purpose  :
//=======================================================================
void SelectMgr_SelectableObject::UpdateTransformation()
{
  for (SelectMgr_SequenceOfSelection::Iterator aSelIter (myselections); aSelIter.More(); aSelIter.Next())
  {
    aSelIter.Value()->UpdateStatus (SelectMgr_TOU_Partial);
  }

  PrsMgr_PresentableObject::UpdateTransformation();
  if (!mySelectionPrs.IsNull())
  {
    mySelectionPrs->SetTransformation (TransformationGeom());
  }
  if (!myHilightPrs.IsNull())
  {
    myHilightPrs->SetTransformation (TransformationGeom());
  }
}

//=======================================================================
//function : UpdateTransformation
//purpose  :
//=======================================================================
void SelectMgr_SelectableObject::UpdateTransformations (const Handle(SelectMgr_Selection)& theSel)
{
  const TopLoc_Location aSelfLocation (Transformation());
  for (NCollection_Vector<Handle(SelectMgr_SensitiveEntity)>::Iterator aSelEntIter (theSel->Entities()); aSelEntIter.More(); aSelEntIter.Next())
  {
    if (const Handle(Select3D_SensitiveEntity)& aSensEntity = aSelEntIter.Value()->BaseSensitive())
    {
      if (const Handle(SelectMgr_EntityOwner)& aEOwner = aSensEntity->OwnerId())
      {
        aEOwner->SetLocation (aSelfLocation);
      }
    }
  }
}

//=======================================================================
//function : HilightSelected
//purpose  :
//=======================================================================
void SelectMgr_SelectableObject::HilightSelected (const Handle(PrsMgr_PresentationManager)&,
                                                  const SelectMgr_SequenceOfOwner&)
{
  throw Standard_NotImplemented("SelectMgr_SelectableObject::HilightSelected");
}

//=======================================================================
//function : ClearSelected
//purpose  :
//=======================================================================
void SelectMgr_SelectableObject::ClearSelected()
{
  if(!mySelectionPrs.IsNull())
  {
    mySelectionPrs->Clear();
  }
}

//=======================================================================
//function : ClearDynamicHighlight
//purpose  :
//=======================================================================
void SelectMgr_SelectableObject::ClearDynamicHighlight (const Handle(PrsMgr_PresentationManager)& theMgr)
{
  theMgr->ClearImmediateDraw();
}

//=======================================================================
//function : HilightOwnerWithColor
//purpose  :
//=======================================================================
void SelectMgr_SelectableObject::HilightOwnerWithColor (const Handle(PrsMgr_PresentationManager)&,
                                                        const Handle(Prs3d_Drawer)&,
                                                        const Handle(SelectMgr_EntityOwner)&)
{
  throw Standard_NotImplemented("SelectMgr_SelectableObject::HilightOwnerWithColor");
}

//=======================================================================
//function : GetHilightPresentation
//purpose  :
//=======================================================================
Handle(Prs3d_Presentation) SelectMgr_SelectableObject::GetHilightPresentation (const Handle(PrsMgr_PresentationManager)& theMgr)
{
  if (myHilightPrs.IsNull() && !theMgr.IsNull())
  {
    myHilightPrs = new Prs3d_Presentation (theMgr->StructureManager());
    myHilightPrs->SetTransformPersistence (TransformPersistence());
    myHilightPrs->SetClipPlanes (myClipPlanes);
    myHilightPrs->SetTransformation (TransformationGeom());
  }

  return myHilightPrs;
}

//=======================================================================
//function : GetSelectPresentation
//purpose  :
//=======================================================================
Handle(Prs3d_Presentation) SelectMgr_SelectableObject::GetSelectPresentation (const Handle(PrsMgr_PresentationManager)& theMgr)
{
  if (mySelectionPrs.IsNull() && !theMgr.IsNull())
  {
    mySelectionPrs = new Prs3d_Presentation (theMgr->StructureManager());
    mySelectionPrs->SetTransformPersistence (TransformPersistence());
    mySelectionPrs->SetClipPlanes (myClipPlanes);
    mySelectionPrs->SetTransformation (TransformationGeom());
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
  for (SelectMgr_SequenceOfSelection::Iterator aSelIter (myselections); aSelIter.More(); aSelIter.Next())
  {
    const Handle(SelectMgr_Selection)& aSel = aSelIter.Value();
    for (NCollection_Vector<Handle(SelectMgr_SensitiveEntity)>::Iterator aSelEntIter (aSel->Entities()); aSelEntIter.More(); aSelEntIter.Next())
    {
      if (const Handle(Select3D_SensitiveEntity)& aEntity = aSelEntIter.Value()->BaseSensitive())
      {
        if (const Handle(SelectMgr_EntityOwner)& aOwner = aEntity->OwnerId())
        {
          aOwner->SetZLayer (theLayerId);
        }
      }
    }
  }
}

//=======================================================================
//function : UpdateClipping
//purpose  :
//=======================================================================
void SelectMgr_SelectableObject::UpdateClipping()
{
  PrsMgr_PresentableObject::UpdateClipping();
  if (!mySelectionPrs.IsNull())
  {
    mySelectionPrs->SetClipPlanes (myClipPlanes);
  }
  if (!myHilightPrs.IsNull())
  {
    myHilightPrs->SetClipPlanes (myClipPlanes);
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
    for (SelectMgr_SequenceOfSelection::Iterator aSelIter (myselections); aSelIter.More(); aSelIter.Next())
    {
      const Handle(SelectMgr_Selection)& aSel = aSelIter.Value();
      aSel->UpdateStatus (SelectMgr_TOU_Full);
    }
    return;
  }

  for (SelectMgr_SequenceOfSelection::Iterator aSelIter (myselections); aSelIter.More(); aSelIter.Next())
  {
    const Handle(SelectMgr_Selection)& aSel = aSelIter.Value();
    if (aSel->Mode() == theMode)
    {
      aSel->UpdateStatus (SelectMgr_TOU_Full);
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
    for (SelectMgr_SequenceOfSelection::Iterator aSelIter (myselections); aSelIter.More(); aSelIter.Next())
    {
      const Handle(SelectMgr_Selection)& aSel = aSelIter.Value();
      for (NCollection_Vector<Handle(SelectMgr_SensitiveEntity)>::Iterator aSelEntIter (aSel->Entities()); aSelEntIter.More(); aSelEntIter.Next())
      {
        aSelEntIter.Value()->BaseSensitive()->Set (theOwner);
      }
    }
    return;
  }

  for (SelectMgr_SequenceOfSelection::Iterator aSelIter (myselections); aSelIter.More(); aSelIter.Next())
  {
    const Handle(SelectMgr_Selection)& aSel = aSelIter.Value();
    if (aSel->Mode() == theMode)
    {
      for (NCollection_Vector<Handle(SelectMgr_SensitiveEntity)>::Iterator aSelEntIter (aSel->Entities()); aSelEntIter.More(); aSelEntIter.Next())
      {
        aSelEntIter.Value()->BaseSensitive()->Set (theOwner);
      }
      return;
    }
  }
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
  for (SelectMgr_SequenceOfSelection::Iterator aSelIter (myselections); aSelIter.More(); aSelIter.Next())
  {
    const Handle(SelectMgr_Selection)& aSel = aSelIter.Value();
    if (aSel->GetSelectionState() != SelectMgr_SOS_Activated)
      continue;

    for (NCollection_Vector<Handle(SelectMgr_SensitiveEntity)>::Iterator aSelEntIter (aSel->Entities()); aSelEntIter.More(); aSelEntIter.Next())
    {
      const Handle(SelectMgr_EntityOwner)& anOwner = aSelEntIter.Value()->BaseSensitive()->OwnerId();
      if (theOwners->Contains (anOwner))
      {
        Select3D_BndBox3d aBox = aSelEntIter.Value()->BaseSensitive()->BoundingBox();
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
  const Handle(SelectMgr_Selection)& aGlobalSel = Selection (myGlobalSelMode);
  if (!aGlobalSel.IsNull()
   && !aGlobalSel->IsEmpty())
  {
    return aGlobalSel->Entities().First()->BaseSensitive()->OwnerId();
  }
  return THE_NULL_ENTITYOWNER;
}

//=======================================================================
//function : GetAssemblyOwner
//purpose  :
//=======================================================================
const Handle(SelectMgr_EntityOwner)& SelectMgr_SelectableObject::GetAssemblyOwner() const
{
  return THE_NULL_ENTITYOWNER;
}

// =======================================================================
// function : DumpJson
// purpose  :
// =======================================================================
void SelectMgr_SelectableObject::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)

  OCCT_DUMP_BASE_CLASS (theOStream, theDepth, PrsMgr_PresentableObject)

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, mySelectionPrs.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myHilightPrs.get())

  for (SelectMgr_SequenceOfSelection::Iterator anIterator (myselections); anIterator.More(); anIterator.Next())
  {
    const Handle(SelectMgr_Selection)& aSelection = anIterator.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, aSelection.get())
  }

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myGlobalSelMode)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myAutoHilight)
}
