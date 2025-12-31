// Copyright (c) 1998-1999 Matra Datavision
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

#include <PrsMgr_PresentationManager.hxx>

#include <TopLoc_Datum3D.hxx>
#include <Prs3d_PresentationShadow.hxx>
#include <PrsMgr_PresentableObject.hxx>
#include <PrsMgr_Presentation.hxx>
#include <NCollection_Sequence.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
class PrsMgr_Presentation;

IMPLEMENT_STANDARD_RTTIEXT(PrsMgr_PresentationManager, Standard_Transient)

//=================================================================================================

PrsMgr_PresentationManager::PrsMgr_PresentationManager(
  const occ::handle<Graphic3d_StructureManager>& theStructureManager)
    : myStructureManager(theStructureManager),
      myImmediateModeOn(0)
{
  //
}

//=================================================================================================

void PrsMgr_PresentationManager::Display(const occ::handle<PrsMgr_PresentableObject>& thePrsObj,
                                         const int                                    theMode)
{
  if (thePrsObj->HasOwnPresentations())
  {
    occ::handle<PrsMgr_Presentation> aPrs = Presentation(thePrsObj, theMode, true);
    if (aPrs->MustBeUpdated())
    {
      Update(thePrsObj, theMode);
    }

    if (myImmediateModeOn > 0)
    {
      AddToImmediateList(aPrs);
    }
    else
    {
      aPrs->Display();
    }
  }
  else
  {
    thePrsObj->Compute(this, occ::handle<Prs3d_Presentation>(), theMode);
  }

  if (thePrsObj->ToPropagateVisualState())
  {
    for (NCollection_List<occ::handle<PrsMgr_PresentableObject>>::Iterator anIter(
           thePrsObj->Children());
         anIter.More();
         anIter.Next())
    {
      const occ::handle<PrsMgr_PresentableObject>& aChild = anIter.Value();
      if (aChild->DisplayStatus() != PrsMgr_DisplayStatus_Erased)
      {
        Display(anIter.Value(), theMode);
      }
    }
  }
}

//=================================================================================================

void PrsMgr_PresentationManager::Erase(const occ::handle<PrsMgr_PresentableObject>& thePrsObj,
                                       const int                                    theMode)
{
  if (thePrsObj->ToPropagateVisualState())
  {
    for (NCollection_List<occ::handle<PrsMgr_PresentableObject>>::Iterator anIter(
           thePrsObj->Children());
         anIter.More();
         anIter.Next())
    {
      Erase(anIter.Value(), theMode);
    }
  }

  NCollection_Sequence<occ::handle<PrsMgr_Presentation>>& aPrsList = thePrsObj->Presentations();
  for (NCollection_Sequence<occ::handle<PrsMgr_Presentation>>::Iterator aPrsIter(aPrsList);
       aPrsIter.More();)
  {
    const occ::handle<PrsMgr_Presentation>& aPrs = aPrsIter.Value();
    if (aPrs.IsNull())
    {
      aPrsIter.Next();
      continue;
    }

    const occ::handle<PrsMgr_PresentationManager>& aPrsMgr = aPrs->PresentationManager();
    if ((theMode == aPrs->Mode() || theMode == -1) && (this == aPrsMgr))
    {
      aPrs->Erase();

      aPrsList.Remove(aPrsIter);

      if (theMode != -1)
      {
        return;
      }
    }
    else
    {
      aPrsIter.Next();
    }
  }
}

//=================================================================================================

void PrsMgr_PresentationManager::Clear(const occ::handle<PrsMgr_PresentableObject>& thePrsObj,
                                       const int                                    theMode)
{
  if (thePrsObj->ToPropagateVisualState())
  {
    for (NCollection_List<occ::handle<PrsMgr_PresentableObject>>::Iterator anIter(
           thePrsObj->Children());
         anIter.More();
         anIter.Next())
    {
      Clear(anIter.Value(), theMode);
    }
  }

  const occ::handle<PrsMgr_Presentation> aPrs = Presentation(thePrsObj, theMode);
  if (!aPrs.IsNull())
  {
    aPrs->Clear();
  }
}

//=================================================================================================

void PrsMgr_PresentationManager::SetVisibility(
  const occ::handle<PrsMgr_PresentableObject>& thePrsObj,
  const int                                    theMode,
  const bool                                   theValue)
{
  if (thePrsObj->ToPropagateVisualState())
  {
    for (NCollection_List<occ::handle<PrsMgr_PresentableObject>>::Iterator anIter(
           thePrsObj->Children());
         anIter.More();
         anIter.Next())
    {
      const occ::handle<PrsMgr_PresentableObject>& aChild = anIter.Value();
      if (!theValue || aChild->DisplayStatus() != PrsMgr_DisplayStatus_Erased)
      {
        SetVisibility(anIter.Value(), theMode, theValue);
      }
    }
  }
  if (!thePrsObj->HasOwnPresentations())
  {
    return;
  }

  occ::handle<PrsMgr_Presentation> aPrs = Presentation(thePrsObj, theMode);
  if (!aPrs.IsNull())
  {
    aPrs->SetVisible(theValue);
  }
}

//=================================================================================================

void PrsMgr_PresentationManager::Unhighlight(const occ::handle<PrsMgr_PresentableObject>& thePrsObj)
{
  if (thePrsObj->ToPropagateVisualState())
  {
    for (NCollection_List<occ::handle<PrsMgr_PresentableObject>>::Iterator anIter(
           thePrsObj->Children());
         anIter.More();
         anIter.Next())
    {
      Unhighlight(anIter.Value());
    }
  }

  const NCollection_Sequence<occ::handle<PrsMgr_Presentation>>& aPrsList =
    thePrsObj->Presentations();
  for (NCollection_Sequence<occ::handle<PrsMgr_Presentation>>::Iterator aPrsIter(aPrsList);
       aPrsIter.More();
       aPrsIter.Next())
  {
    const occ::handle<PrsMgr_Presentation>&        aPrs    = aPrsIter.Value();
    const occ::handle<PrsMgr_PresentationManager>& aPrsMgr = aPrs->PresentationManager();
    if (this == aPrsMgr && aPrs->IsHighlighted())
    {
      aPrs->Unhighlight();
    }
  }
}

//=================================================================================================

void PrsMgr_PresentationManager::SetDisplayPriority(
  const occ::handle<PrsMgr_PresentableObject>& thePrsObj,
  const int                                    theMode,
  const Graphic3d_DisplayPriority              theNewPrior) const
{
  if (thePrsObj->ToPropagateVisualState())
  {
    for (NCollection_List<occ::handle<PrsMgr_PresentableObject>>::Iterator anIter(
           thePrsObj->Children());
         anIter.More();
         anIter.Next())
    {
      SetDisplayPriority(anIter.Value(), theMode, theNewPrior);
    }
  }

  const occ::handle<PrsMgr_Presentation> aPrs = Presentation(thePrsObj, theMode);
  if (!aPrs.IsNull())
  {
    aPrs->SetDisplayPriority(theNewPrior);
  }
}

//=================================================================================================

Graphic3d_DisplayPriority PrsMgr_PresentationManager::DisplayPriority(
  const occ::handle<PrsMgr_PresentableObject>& thePrsObj,
  const int                                    theMode) const
{
  if (thePrsObj->ToPropagateVisualState())
  {
    for (NCollection_List<occ::handle<PrsMgr_PresentableObject>>::Iterator anIter(
           thePrsObj->Children());
         anIter.More();
         anIter.Next())
    {
      Graphic3d_DisplayPriority aPriority = DisplayPriority(anIter.Value(), theMode);
      if (aPriority != Graphic3d_DisplayPriority_INVALID)
      {
        return aPriority;
      }
    }
  }

  const occ::handle<PrsMgr_Presentation> aPrs = Presentation(thePrsObj, theMode);
  return !aPrs.IsNull() ? aPrs->DisplayPriority() : Graphic3d_DisplayPriority_INVALID;
}

//=================================================================================================

bool PrsMgr_PresentationManager::IsDisplayed(const occ::handle<PrsMgr_PresentableObject>& thePrsObj,
                                             const int theMode) const
{
  if (thePrsObj->ToPropagateVisualState())
  {
    for (NCollection_List<occ::handle<PrsMgr_PresentableObject>>::Iterator anIter(
           thePrsObj->Children());
         anIter.More();
         anIter.Next())
    {
      if (IsDisplayed(anIter.Value(), theMode))
      {
        return true;
      }
    }
  }

  const occ::handle<PrsMgr_Presentation> aPrs = Presentation(thePrsObj, theMode);
  return !aPrs.IsNull() && aPrs->IsDisplayed();
}

//=================================================================================================

bool PrsMgr_PresentationManager::IsHighlighted(
  const occ::handle<PrsMgr_PresentableObject>& thePrsObj,
  const int                                    theMode) const
{
  if (thePrsObj->ToPropagateVisualState())
  {
    for (NCollection_List<occ::handle<PrsMgr_PresentableObject>>::Iterator anIter(
           thePrsObj->Children());
         anIter.More();
         anIter.Next())
    {
      if (IsHighlighted(anIter.Value(), theMode))
      {
        return true;
      }
    }
  }

  const occ::handle<PrsMgr_Presentation> aPrs = Presentation(thePrsObj, theMode);
  return !aPrs.IsNull() && aPrs->IsHighlighted();
}

//=================================================================================================

void PrsMgr_PresentationManager::Update(const occ::handle<PrsMgr_PresentableObject>& thePrsObj,
                                        const int                                    theMode) const
{
  for (NCollection_List<occ::handle<PrsMgr_PresentableObject>>::Iterator anIter(
         thePrsObj->Children());
       anIter.More();
       anIter.Next())
  {
    Update(anIter.Value(), theMode);
  }

  occ::handle<PrsMgr_Presentation> aPrs = Presentation(thePrsObj, theMode);
  if (!aPrs.IsNull())
  {
    aPrs->Clear();
    thePrsObj->Fill(this, aPrs, theMode);
    aPrs->SetUpdateStatus(false);
  }
}

//=================================================================================================

void PrsMgr_PresentationManager::BeginImmediateDraw()
{
  if (++myImmediateModeOn > 1)
  {
    return;
  }

  ClearImmediateDraw();
}

//=================================================================================================

void PrsMgr_PresentationManager::ClearImmediateDraw()
{
  for (NCollection_List<occ::handle<Prs3d_Presentation>>::Iterator anIter(myImmediateList);
       anIter.More();
       anIter.Next())
  {
    anIter.Value()->Erase();
  }

  for (NCollection_List<occ::handle<Prs3d_Presentation>>::Iterator anIter(
         myViewDependentImmediateList);
       anIter.More();
       anIter.Next())
  {
    anIter.Value()->Erase();
  }

  myImmediateList.Clear();
  myViewDependentImmediateList.Clear();
}

// =======================================================================
// function : displayImmediate
// purpose  : Handles the structures from myImmediateList and its visibility
//            in all views of the viewer given by setting proper affinity
// =======================================================================
void PrsMgr_PresentationManager::displayImmediate(const occ::handle<V3d_Viewer>& theViewer)
{
  for (NCollection_List<occ::handle<V3d_View>>::Iterator anActiveViewIter(
         theViewer->ActiveViewIterator());
       anActiveViewIter.More();
       anActiveViewIter.Next())
  {
    const occ::handle<Graphic3d_CView>& aView = anActiveViewIter.Value()->View();
    for (NCollection_List<occ::handle<Prs3d_Presentation>>::Iterator anIter(myImmediateList);
         anIter.More();
         anIter.Next())
    {
      const occ::handle<Prs3d_Presentation>& aPrs = anIter.Value();
      if (aPrs.IsNull())
        continue;

      occ::handle<Graphic3d_Structure>      aViewDepPrs;
      occ::handle<Prs3d_PresentationShadow> aShadowPrs =
        occ::down_cast<Prs3d_PresentationShadow>(aPrs);
      if (!aShadowPrs.IsNull() && aView->IsComputed(aShadowPrs->ParentId(), aViewDepPrs))
      {
        const Graphic3d_ZLayerId aZLayer = aShadowPrs->GetZLayer();
        aShadowPrs.Nullify();

        aShadowPrs = new Prs3d_PresentationShadow(myStructureManager, aViewDepPrs);
        aShadowPrs->SetZLayer(aZLayer);
        aShadowPrs->SetClipPlanes(aViewDepPrs->ClipPlanes());
        aShadowPrs->CStructure()->IsForHighlight = 1;
        aShadowPrs->Highlight(aPrs->HighlightStyle());
        myViewDependentImmediateList.Append(aShadowPrs);
      }
      // handles custom highlight presentations which were defined in overridden
      // HilightOwnerWithColor method of a custom AIS objects and maintain its
      // visibility in different views on their own
      else if (aShadowPrs.IsNull())
      {
        aPrs->Display();
        continue;
      }

      if (!aShadowPrs->IsDisplayed())
      {
        aShadowPrs->CStructure()->ViewAffinity = new Graphic3d_ViewAffinity();
        aShadowPrs->CStructure()->ViewAffinity->SetVisible(false);
        aShadowPrs->Display();
      }

      int  aViewId         = aView->Identification();
      bool isParentVisible = aShadowPrs->ParentAffinity().IsNull()
                               ? true
                               : aShadowPrs->ParentAffinity()->IsVisible(aViewId);
      aShadowPrs->CStructure()->ViewAffinity->SetVisible(aViewId, isParentVisible);
    }
  }
}

//=================================================================================================

void PrsMgr_PresentationManager::EndImmediateDraw(const occ::handle<V3d_Viewer>& theViewer)
{
  if (--myImmediateModeOn > 0)
  {
    return;
  }

  displayImmediate(theViewer);
}

// =======================================================================
// function : RedrawImmediate
// purpose  : Clears all immediate structures and redisplays with proper
//            affinity
//=======================================================================
void PrsMgr_PresentationManager::RedrawImmediate(const occ::handle<V3d_Viewer>& theViewer)
{
  if (myImmediateList.IsEmpty())
    return;

  // Clear previously displayed structures
  for (NCollection_List<occ::handle<Prs3d_Presentation>>::Iterator anIter(myImmediateList);
       anIter.More();
       anIter.Next())
  {
    anIter.Value()->Erase();
  }
  for (NCollection_List<occ::handle<Prs3d_Presentation>>::Iterator anIter(
         myViewDependentImmediateList);
       anIter.More();
       anIter.Next())
  {
    anIter.Value()->Erase();
  }
  myViewDependentImmediateList.Clear();

  displayImmediate(theViewer);
}

//=================================================================================================

void PrsMgr_PresentationManager::AddToImmediateList(const occ::handle<Prs3d_Presentation>& thePrs)
{
  if (myImmediateModeOn < 1)
  {
    return;
  }

  for (NCollection_List<occ::handle<Prs3d_Presentation>>::Iterator anIter(myImmediateList);
       anIter.More();
       anIter.Next())
  {
    if (anIter.Value() == thePrs)
    {
      return;
    }
  }

  myImmediateList.Append(thePrs);
}

//=================================================================================================

bool PrsMgr_PresentationManager::HasPresentation(
  const occ::handle<PrsMgr_PresentableObject>& thePrsObj,
  const int                                    theMode) const
{
  if (!thePrsObj->HasOwnPresentations())
    return false;

  const NCollection_Sequence<occ::handle<PrsMgr_Presentation>>& aPrsList =
    thePrsObj->Presentations();
  for (NCollection_Sequence<occ::handle<PrsMgr_Presentation>>::Iterator aPrsIter(aPrsList);
       aPrsIter.More();
       aPrsIter.Next())
  {
    const occ::handle<PrsMgr_Presentation>&        aPrs    = aPrsIter.Value();
    const occ::handle<PrsMgr_PresentationManager>& aPrsMgr = aPrs->PresentationManager();
    if (theMode == aPrs->Mode() && this == aPrsMgr)
    {
      return true;
    }
  }
  return false;
}

//=================================================================================================

occ::handle<PrsMgr_Presentation> PrsMgr_PresentationManager::Presentation(
  const occ::handle<PrsMgr_PresentableObject>& thePrsObj,
  const int                                    theMode,
  const bool                                   theToCreate,
  const occ::handle<PrsMgr_PresentableObject>& theSelObj) const
{
  const NCollection_Sequence<occ::handle<PrsMgr_Presentation>>& aPrsList =
    thePrsObj->Presentations();
  for (NCollection_Sequence<occ::handle<PrsMgr_Presentation>>::Iterator aPrsIter(aPrsList);
       aPrsIter.More();
       aPrsIter.Next())
  {
    const occ::handle<PrsMgr_Presentation>&        aPrs    = aPrsIter.Value();
    const occ::handle<PrsMgr_PresentationManager>& aPrsMgr = aPrs->PresentationManager();
    if (theMode == aPrs->Mode() && this == aPrsMgr)
    {
      return aPrs;
    }
  }

  if (!theToCreate)
  {
    return occ::handle<PrsMgr_Presentation>();
  }

  occ::handle<PrsMgr_Presentation> aPrs = new PrsMgr_Presentation(this, thePrsObj, theMode);
  aPrs->SetZLayer(thePrsObj->ZLayer());
  aPrs->CStructure()->ViewAffinity =
    !theSelObj.IsNull() ? theSelObj->ViewAffinity() : thePrsObj->ViewAffinity();
  thePrsObj->Presentations().Append(aPrs);
  thePrsObj->Fill(this, aPrs, theMode);

  // set layer index accordingly to object's presentations
  aPrs->SetUpdateStatus(false);
  return aPrs;
}

//=================================================================================================

bool PrsMgr_PresentationManager::RemovePresentation(
  const occ::handle<PrsMgr_PresentableObject>& thePrsObj,
  const int                                    theMode)
{
  NCollection_Sequence<occ::handle<PrsMgr_Presentation>>& aPrsList = thePrsObj->Presentations();
  for (NCollection_Sequence<occ::handle<PrsMgr_Presentation>>::Iterator aPrsIter(aPrsList);
       aPrsIter.More();
       aPrsIter.Next())
  {
    const occ::handle<PrsMgr_Presentation>&        aPrs    = aPrsIter.Value();
    const occ::handle<PrsMgr_PresentationManager>& aPrsMgr = aPrs->PresentationManager();
    if (theMode == aPrs->Mode() && this == aPrsMgr)
    {
      aPrsList.Remove(aPrsIter);
      return true;
    }
  }
  return false;
}

//=================================================================================================

void PrsMgr_PresentationManager::SetZLayer(const occ::handle<PrsMgr_PresentableObject>& thePrsObj,
                                           const Graphic3d_ZLayerId                     theLayerId)
{
  if (thePrsObj->ToPropagateVisualState())
  {
    for (NCollection_List<occ::handle<PrsMgr_PresentableObject>>::Iterator anIter(
           thePrsObj->Children());
         anIter.More();
         anIter.Next())
    {
      SetZLayer(anIter.Value(), theLayerId);
    }
  }

  if (!thePrsObj->HasOwnPresentations())
  {
    return;
  }

  thePrsObj->SetZLayer(theLayerId);
}

//=================================================================================================

Graphic3d_ZLayerId PrsMgr_PresentationManager::GetZLayer(
  const occ::handle<PrsMgr_PresentableObject>& thePrsObj) const
{
  return thePrsObj->ZLayer();
}

//=================================================================================================

void PrsMgr_PresentationManager::Connect(
  const occ::handle<PrsMgr_PresentableObject>& thePrsObject,
  const occ::handle<PrsMgr_PresentableObject>& theOtherObject,
  const int                                    theMode,
  const int                                    theOtherMode)
{
  occ::handle<PrsMgr_Presentation> aPrs      = Presentation(thePrsObject, theMode, true);
  occ::handle<PrsMgr_Presentation> aPrsOther = Presentation(theOtherObject, theOtherMode, true);
  aPrs->Connect(aPrsOther.get(), Graphic3d_TOC_DESCENDANT);
}

//=================================================================================================

void PrsMgr_PresentationManager::Transform(const occ::handle<PrsMgr_PresentableObject>& thePrsObj,
                                           const occ::handle<TopLoc_Datum3D>& theTransformation,
                                           const int                          theMode)
{
  Presentation(thePrsObj, theMode)->SetTransformation(theTransformation);
}

//=================================================================================================

void PrsMgr_PresentationManager::Color(const occ::handle<PrsMgr_PresentableObject>& thePrsObj,
                                       const occ::handle<Prs3d_Drawer>&             theStyle,
                                       const int                                    theMode,
                                       const occ::handle<PrsMgr_PresentableObject>& theSelObj,
                                       const int theImmediateStructLayerId)
{
  if (thePrsObj->ToPropagateVisualState())
  {
    for (NCollection_List<occ::handle<PrsMgr_PresentableObject>>::Iterator anIter(
           thePrsObj->Children());
         anIter.More();
         anIter.Next())
    {
      const occ::handle<PrsMgr_PresentableObject>& aChild = anIter.Value();
      if (aChild->DisplayStatus() != PrsMgr_DisplayStatus_Erased)
      {
        Color(aChild, theStyle, theMode, nullptr, theImmediateStructLayerId);
      }
    }
  }
  if (!thePrsObj->HasOwnPresentations())
  {
    return;
  }

  occ::handle<PrsMgr_Presentation> aPrs = Presentation(thePrsObj, theMode, true, theSelObj);
  if (aPrs->MustBeUpdated())
  {
    Update(thePrsObj, theMode);
  }

  if (myImmediateModeOn > 0)
  {
    occ::handle<Prs3d_PresentationShadow> aShadow =
      new Prs3d_PresentationShadow(myStructureManager, aPrs);
    aShadow->SetZLayer(theImmediateStructLayerId);
    aShadow->SetClipPlanes(aPrs->ClipPlanes());
    aShadow->CStructure()->IsForHighlight = 1;
    aShadow->Highlight(theStyle);
    AddToImmediateList(aShadow);
  }
  else
  {
    aPrs->Highlight(theStyle);
  }
}

namespace
{
//! Internal function that scans thePrsList for shadow presentations
//! and applies transformation theTrsf to them in case if parent ID
//! of shadow presentation is equal to theRefId
static void updatePrsTransformation(
  const NCollection_List<occ::handle<Prs3d_Presentation>>& thePrsList,
  const int                                                theRefId,
  const occ::handle<TopLoc_Datum3D>&                       theTrsf)
{
  for (NCollection_List<occ::handle<Prs3d_Presentation>>::Iterator anIter(thePrsList);
       anIter.More();
       anIter.Next())
  {
    const occ::handle<Prs3d_Presentation>& aPrs = anIter.Value();
    if (aPrs.IsNull())
      continue;

    occ::handle<Prs3d_PresentationShadow> aShadowPrs =
      occ::down_cast<Prs3d_PresentationShadow>(aPrs);
    if (aShadowPrs.IsNull() || aShadowPrs->ParentId() != theRefId)
      continue;

    aShadowPrs->CStructure()->SetTransformation(theTrsf);
  }
}
} // namespace

//=================================================================================================

void PrsMgr_PresentationManager::UpdateHighlightTrsf(
  const occ::handle<V3d_Viewer>&               theViewer,
  const occ::handle<PrsMgr_PresentableObject>& theObj,
  const int                                    theMode,
  const occ::handle<PrsMgr_PresentableObject>& theSelObj)
{
  if (theObj.IsNull())
    return;

  occ::handle<PrsMgr_Presentation> aPrs =
    Presentation(!theSelObj.IsNull() ? theSelObj : theObj, theMode, false);
  if (aPrs.IsNull())
  {
    return;
  }

  occ::handle<TopLoc_Datum3D> aTrsf     = theObj->LocalTransformationGeom();
  const int                   aParentId = aPrs->CStructure()->Identification();
  updatePrsTransformation(myImmediateList, aParentId, aTrsf);

  if (!myViewDependentImmediateList.IsEmpty())
  {
    for (NCollection_List<occ::handle<V3d_View>>::Iterator anActiveViewIter(
           theViewer->ActiveViewIterator());
         anActiveViewIter.More();
         anActiveViewIter.Next())
    {
      const occ::handle<Graphic3d_CView>& aView = anActiveViewIter.Value()->View();
      occ::handle<Graphic3d_Structure>    aViewDepParentPrs;
      if (aView->IsComputed(aParentId, aViewDepParentPrs))
      {
        updatePrsTransformation(myViewDependentImmediateList,
                                aViewDepParentPrs->CStructure()->Identification(),
                                aTrsf);
      }
    }
  }
}
