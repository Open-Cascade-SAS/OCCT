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

#include <Prs3d_Drawer.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Select3D_SensitiveEntity.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_Shared.hxx>
#include <SelectMgr_Selection.hxx>
#include <SelectMgr_SelectionManager.hxx>
#include <Standard_NotImplemented.hxx>
#include <TopLoc_Location.hxx>
class SelectMgr_EntityOwner;

IMPLEMENT_STANDARD_RTTIEXT(SelectMgr_SelectableObject, PrsMgr_PresentableObject)

namespace
{
static const occ::handle<SelectMgr_Selection>   THE_NULL_SELECTION;
static const occ::handle<SelectMgr_EntityOwner> THE_NULL_ENTITYOWNER;
} // namespace

//=================================================================================================

SelectMgr_SelectableObject::SelectMgr_SelectableObject(
  const PrsMgr_TypeOfPresentation3d aTypeOfPresentation3d)
    : PrsMgr_PresentableObject(aTypeOfPresentation3d),
      myGlobalSelMode(0),
      myAutoHilight(true)
{
  //
}

//==================================================
// Function: Destructor
// Purpose : Clears all selections of the object
//==================================================
SelectMgr_SelectableObject::~SelectMgr_SelectableObject()
{
  for (NCollection_Sequence<occ::handle<SelectMgr_Selection>>::Iterator aSelIter(myselections);
       aSelIter.More();
       aSelIter.Next())
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
  for (NCollection_Sequence<occ::handle<SelectMgr_Selection>>::Iterator aSelIter(myselections);
       aSelIter.More();
       aSelIter.Next())
  {
    RecomputePrimitives(aSelIter.Value()->Mode());
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
void SelectMgr_SelectableObject::RecomputePrimitives(const int theMode)
{
  SelectMgr_SelectableObject* aSelParent = dynamic_cast<SelectMgr_SelectableObject*>(Parent());
  for (NCollection_Sequence<occ::handle<SelectMgr_Selection>>::Iterator aSelIter(myselections);
       aSelIter.More();
       aSelIter.Next())
  {
    const occ::handle<SelectMgr_Selection>& aSel = aSelIter.Value();
    if (aSel->Mode() == theMode)
    {
      aSel->Clear();
      ComputeSelection(aSel, theMode);
      aSel->UpdateStatus(SelectMgr_TOU_Partial);
      aSel->UpdateBVHStatus(SelectMgr_TBU_Renew);
      if (theMode == 0 && aSelParent != nullptr)
      {
        if (const occ::handle<SelectMgr_EntityOwner>& anAsmOwner = aSelParent->GetAssemblyOwner())
        {
          SetAssemblyOwner(anAsmOwner, theMode);
        }
      }
      return;
    }
  }

  occ::handle<SelectMgr_Selection> aNewSel = new SelectMgr_Selection(theMode);
  ComputeSelection(aNewSel, theMode);

  if (theMode == 0 && aSelParent != nullptr)
  {
    if (const occ::handle<SelectMgr_EntityOwner>& anAsmOwner = aSelParent->GetAssemblyOwner())
    {
      SetAssemblyOwner(anAsmOwner, theMode);
    }
  }

  aNewSel->UpdateStatus(SelectMgr_TOU_Partial);
  aNewSel->UpdateBVHStatus(SelectMgr_TBU_Add);

  myselections.Append(aNewSel);
}

//=================================================================================================

void SelectMgr_SelectableObject::ClearSelections(const bool theToUpdate)
{
  for (NCollection_Sequence<occ::handle<SelectMgr_Selection>>::Iterator aSelIter(myselections);
       aSelIter.More();
       aSelIter.Next())
  {
    const occ::handle<SelectMgr_Selection>& aSel = aSelIter.Value();
    aSel->Clear();
    aSel->UpdateBVHStatus(SelectMgr_TBU_Remove);
    if (theToUpdate)
    {
      aSel->UpdateStatus(SelectMgr_TOU_Full);
    }
  }
}

//=================================================================================================

const occ::handle<SelectMgr_Selection>& SelectMgr_SelectableObject::Selection(
  const int theMode) const
{
  if (theMode == -1)
  {
    return THE_NULL_SELECTION;
  }

  for (NCollection_Sequence<occ::handle<SelectMgr_Selection>>::Iterator aSelIter(myselections);
       aSelIter.More();
       aSelIter.Next())
  {
    const occ::handle<SelectMgr_Selection>& aSel = aSelIter.Value();
    if (aSel->Mode() == theMode)
    {
      return aSel;
    }
  }
  return THE_NULL_SELECTION;
}

//=================================================================================================

void SelectMgr_SelectableObject::AddSelection(const occ::handle<SelectMgr_Selection>& theSel,
                                              const int                               theMode)
{
  if (theSel->IsEmpty())
  {
    ComputeSelection(theSel, theMode);
    theSel->UpdateStatus(SelectMgr_TOU_Partial);
    theSel->UpdateBVHStatus(SelectMgr_TBU_Add);
  }

  bool isReplaced = false;
  for (NCollection_Sequence<occ::handle<SelectMgr_Selection>>::Iterator aSelIter(myselections);
       aSelIter.More();
       aSelIter.Next())
  {
    if (aSelIter.Value()->Mode() == theMode)
    {
      isReplaced = true;
      myselections.Remove(aSelIter);
      break;
    }
  }

  myselections.Append(theSel);
  if (isReplaced)
  {
    myselections.Last()->UpdateBVHStatus(SelectMgr_TBU_Renew);
  }

  if (theMode == 0)
  {
    SelectMgr_SelectableObject* aSelParent = dynamic_cast<SelectMgr_SelectableObject*>(Parent());
    if (aSelParent != nullptr)
    {
      if (const occ::handle<SelectMgr_EntityOwner>& anAsmOwner = aSelParent->GetAssemblyOwner())
      {
        SetAssemblyOwner(anAsmOwner, theMode);
      }
    }
  }
}

//=================================================================================================

void SelectMgr_SelectableObject::ResetTransformation()
{
  for (NCollection_Sequence<occ::handle<SelectMgr_Selection>>::Iterator aSelIter(myselections);
       aSelIter.More();
       aSelIter.Next())
  {
    const occ::handle<SelectMgr_Selection>& aSel = aSelIter.Value();
    aSel->UpdateStatus(SelectMgr_TOU_Partial);
    aSel->UpdateBVHStatus(SelectMgr_TBU_None);
  }

  PrsMgr_PresentableObject::ResetTransformation();
}

//=================================================================================================

void SelectMgr_SelectableObject::UpdateTransformation()
{
  for (NCollection_Sequence<occ::handle<SelectMgr_Selection>>::Iterator aSelIter(myselections);
       aSelIter.More();
       aSelIter.Next())
  {
    aSelIter.Value()->UpdateStatus(SelectMgr_TOU_Partial);
  }

  PrsMgr_PresentableObject::UpdateTransformation();
  if (!mySelectionPrs.IsNull())
  {
    mySelectionPrs->SetTransformation(TransformationGeom());
  }
  if (!myHilightPrs.IsNull())
  {
    myHilightPrs->SetTransformation(TransformationGeom());
  }
}

//=================================================================================================

void SelectMgr_SelectableObject::UpdateTransformations(
  const occ::handle<SelectMgr_Selection>& theSel)
{
  const TopLoc_Location aSelfLocation(Transformation());
  for (NCollection_Vector<occ::handle<SelectMgr_SensitiveEntity>>::Iterator aSelEntIter(
         theSel->Entities());
       aSelEntIter.More();
       aSelEntIter.Next())
  {
    if (const occ::handle<Select3D_SensitiveEntity>& aSensEntity =
          aSelEntIter.Value()->BaseSensitive())
    {
      if (const occ::handle<SelectMgr_EntityOwner>& aEOwner = aSensEntity->OwnerId())
      {
        aEOwner->SetLocation(aSelfLocation);
      }
    }
  }
}

//=================================================================================================

void SelectMgr_SelectableObject::HilightSelected(
  const occ::handle<PrsMgr_PresentationManager>&,
  const NCollection_Sequence<occ::handle<SelectMgr_EntityOwner>>&)
{
  throw Standard_NotImplemented("SelectMgr_SelectableObject::HilightSelected");
}

//=================================================================================================

void SelectMgr_SelectableObject::ClearSelected()
{
  if (!mySelectionPrs.IsNull())
  {
    mySelectionPrs->Clear();
  }
}

//=================================================================================================

void SelectMgr_SelectableObject::ClearDynamicHighlight(
  const occ::handle<PrsMgr_PresentationManager>& theMgr)
{
  theMgr->ClearImmediateDraw();
}

//=================================================================================================

void SelectMgr_SelectableObject::HilightOwnerWithColor(
  const occ::handle<PrsMgr_PresentationManager>&,
  const occ::handle<Prs3d_Drawer>&,
  const occ::handle<SelectMgr_EntityOwner>&)
{
  throw Standard_NotImplemented("SelectMgr_SelectableObject::HilightOwnerWithColor");
}

//=================================================================================================

occ::handle<Prs3d_Presentation> SelectMgr_SelectableObject::GetHilightPresentation(
  const occ::handle<PrsMgr_PresentationManager>& theMgr)
{
  if (myHilightPrs.IsNull() && !theMgr.IsNull())
  {
    myHilightPrs = new Prs3d_Presentation(theMgr->StructureManager());
    myHilightPrs->SetTransformPersistence(TransformPersistence());
    myHilightPrs->SetClipPlanes(myClipPlanes);
    myHilightPrs->SetTransformation(TransformationGeom());
  }

  return myHilightPrs;
}

//=================================================================================================

occ::handle<Prs3d_Presentation> SelectMgr_SelectableObject::GetSelectPresentation(
  const occ::handle<PrsMgr_PresentationManager>& theMgr)
{
  if (mySelectionPrs.IsNull() && !theMgr.IsNull())
  {
    mySelectionPrs = new Prs3d_Presentation(theMgr->StructureManager());
    mySelectionPrs->SetTransformPersistence(TransformPersistence());
    mySelectionPrs->SetClipPlanes(myClipPlanes);
    mySelectionPrs->SetTransformation(TransformationGeom());
  }

  return mySelectionPrs;
}

//=================================================================================================

void SelectMgr_SelectableObject::ErasePresentations(bool theToRemove)
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

//=================================================================================================

void SelectMgr_SelectableObject::SetZLayer(const Graphic3d_ZLayerId theLayerId)
{
  // update own presentations
  PrsMgr_PresentableObject::SetZLayer(theLayerId);

  // update selection presentations
  if (!mySelectionPrs.IsNull())
    mySelectionPrs->SetZLayer(theLayerId);

  if (!myHilightPrs.IsNull())
    myHilightPrs->SetZLayer(theLayerId);

  // update all entity owner presentations
  for (NCollection_Sequence<occ::handle<SelectMgr_Selection>>::Iterator aSelIter(myselections);
       aSelIter.More();
       aSelIter.Next())
  {
    const occ::handle<SelectMgr_Selection>& aSel = aSelIter.Value();
    for (NCollection_Vector<occ::handle<SelectMgr_SensitiveEntity>>::Iterator aSelEntIter(
           aSel->Entities());
         aSelEntIter.More();
         aSelEntIter.Next())
    {
      if (const occ::handle<Select3D_SensitiveEntity>& aEntity =
            aSelEntIter.Value()->BaseSensitive())
      {
        if (const occ::handle<SelectMgr_EntityOwner>& aOwner = aEntity->OwnerId())
        {
          aOwner->SetZLayer(theLayerId);
        }
      }
    }
  }
}

//=================================================================================================

void SelectMgr_SelectableObject::UpdateClipping()
{
  PrsMgr_PresentableObject::UpdateClipping();
  if (!mySelectionPrs.IsNull())
  {
    mySelectionPrs->SetClipPlanes(myClipPlanes);
  }
  if (!myHilightPrs.IsNull())
  {
    myHilightPrs->SetClipPlanes(myClipPlanes);
  }
}

//=======================================================================
// function : updateSelection
// purpose  : Sets update status FULL to selections of the object. Must be
//           used as the only method of UpdateSelection from outer classes
//           to prevent BVH structures from being outdated.
//=======================================================================
void SelectMgr_SelectableObject::updateSelection(const int theMode)
{
  if (theMode == -1)
  {
    for (NCollection_Sequence<occ::handle<SelectMgr_Selection>>::Iterator aSelIter(myselections);
         aSelIter.More();
         aSelIter.Next())
    {
      const occ::handle<SelectMgr_Selection>& aSel = aSelIter.Value();
      aSel->UpdateStatus(SelectMgr_TOU_Full);
    }
    return;
  }

  for (NCollection_Sequence<occ::handle<SelectMgr_Selection>>::Iterator aSelIter(myselections);
       aSelIter.More();
       aSelIter.Next())
  {
    const occ::handle<SelectMgr_Selection>& aSel = aSelIter.Value();
    if (aSel->Mode() == theMode)
    {
      aSel->UpdateStatus(SelectMgr_TOU_Full);
      return;
    }
  }
}

//=======================================================================
// function : SetAssemblyOwner
// purpose  : Sets common entity owner for assembly sensitive object entities
//=======================================================================
void SelectMgr_SelectableObject::SetAssemblyOwner(
  const occ::handle<SelectMgr_EntityOwner>& theOwner,
  const int                                 theMode)
{
  if (theMode == -1)
  {
    for (NCollection_Sequence<occ::handle<SelectMgr_Selection>>::Iterator aSelIter(myselections);
         aSelIter.More();
         aSelIter.Next())
    {
      const occ::handle<SelectMgr_Selection>& aSel = aSelIter.Value();
      for (NCollection_Vector<occ::handle<SelectMgr_SensitiveEntity>>::Iterator aSelEntIter(
             aSel->Entities());
           aSelEntIter.More();
           aSelEntIter.Next())
      {
        aSelEntIter.Value()->BaseSensitive()->Set(theOwner);
      }
    }
    return;
  }

  for (NCollection_Sequence<occ::handle<SelectMgr_Selection>>::Iterator aSelIter(myselections);
       aSelIter.More();
       aSelIter.Next())
  {
    const occ::handle<SelectMgr_Selection>& aSel = aSelIter.Value();
    if (aSel->Mode() == theMode)
    {
      for (NCollection_Vector<occ::handle<SelectMgr_SensitiveEntity>>::Iterator aSelEntIter(
             aSel->Entities());
           aSelEntIter.More();
           aSelEntIter.Next())
      {
        aSelEntIter.Value()->BaseSensitive()->Set(theOwner);
      }
      return;
    }
  }
}

//=================================================================================================

Bnd_Box SelectMgr_SelectableObject::BndBoxOfSelected(
  const occ::handle<NCollection_Shared<NCollection_IndexedMap<occ::handle<SelectMgr_EntityOwner>>>>&
    theOwners)
{
  if (theOwners->IsEmpty())
    return Bnd_Box();

  Bnd_Box aBnd;
  for (NCollection_Sequence<occ::handle<SelectMgr_Selection>>::Iterator aSelIter(myselections);
       aSelIter.More();
       aSelIter.Next())
  {
    const occ::handle<SelectMgr_Selection>& aSel = aSelIter.Value();
    if (aSel->GetSelectionState() != SelectMgr_SOS_Activated)
      continue;

    for (NCollection_Vector<occ::handle<SelectMgr_SensitiveEntity>>::Iterator aSelEntIter(
           aSel->Entities());
         aSelEntIter.More();
         aSelEntIter.Next())
    {
      const occ::handle<SelectMgr_EntityOwner>& anOwner =
        aSelEntIter.Value()->BaseSensitive()->OwnerId();
      if (theOwners->Contains(anOwner))
      {
        Select3D_BndBox3d aBox = aSelEntIter.Value()->BaseSensitive()->BoundingBox();
        aBnd.Update(aBox.CornerMin().x(),
                    aBox.CornerMin().y(),
                    aBox.CornerMin().z(),
                    aBox.CornerMax().x(),
                    aBox.CornerMax().y(),
                    aBox.CornerMax().z());
      }
    }
  }

  return aBnd;
}

//=======================================================================
// function : GlobalSelOwner
// purpose  : Returns entity owner corresponding to selection of the object as a whole
//=======================================================================
occ::handle<SelectMgr_EntityOwner> SelectMgr_SelectableObject::GlobalSelOwner() const
{
  const occ::handle<SelectMgr_Selection>& aGlobalSel = Selection(myGlobalSelMode);
  if (!aGlobalSel.IsNull() && !aGlobalSel->IsEmpty())
  {
    return aGlobalSel->Entities().First()->BaseSensitive()->OwnerId();
  }
  return THE_NULL_ENTITYOWNER;
}

//=================================================================================================

const occ::handle<SelectMgr_EntityOwner>& SelectMgr_SelectableObject::GetAssemblyOwner() const
{
  return THE_NULL_ENTITYOWNER;
}

//=================================================================================================

void SelectMgr_SelectableObject::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, PrsMgr_PresentableObject)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, mySelectionPrs.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myHilightPrs.get())

  for (NCollection_Sequence<occ::handle<SelectMgr_Selection>>::Iterator anIterator(myselections);
       anIterator.More();
       anIterator.Next())
  {
    const occ::handle<SelectMgr_Selection>& aSelection = anIterator.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, aSelection.get())
  }

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myGlobalSelMode)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myAutoHilight)
}
