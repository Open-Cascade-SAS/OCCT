// Created on: 1997-01-17
// Created by: Robert COUBLANC
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

#include <AIS_InteractiveContext.hxx>

#include <AIS_DataMapIteratorOfDataMapOfIOStatus.hxx>
#include <AIS_ConnectedInteractive.hxx>
#include <AIS_GlobalStatus.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_MultipleConnectedInteractive.hxx>
#include <Precision.hxx>
#include <Prs3d_DatumAspect.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PlaneAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <TColStd_MapIteratorOfMapOfTransient.hxx>
#include <TopLoc_Location.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

#include <AIS_Shape.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_InteractiveContext, Standard_Transient)

namespace
{
typedef NCollection_DataMap<Handle(SelectMgr_SelectableObject), Handle(SelectMgr_IndexedMapOfOwner)>
  AIS_MapOfObjectOwners;
typedef NCollection_DataMap<Handle(SelectMgr_SelectableObject),
                            Handle(SelectMgr_IndexedMapOfOwner)>::Iterator
  AIS_MapIteratorOfMapOfObjectOwners;

//! Initialize default highlighting attributes.
static void initDefaultHilightAttributes(const Handle(Prs3d_Drawer)& theDrawer,
                                         const Quantity_Color&       theColor)
{
  theDrawer->SetMethod(Aspect_TOHM_COLOR);
  theDrawer->SetDisplayMode(0);
  theDrawer->SetColor(theColor);

  theDrawer->SetupOwnShadingAspect();
  theDrawer->SetupOwnPointAspect();
  theDrawer->SetLineAspect(new Prs3d_LineAspect(Quantity_NOC_BLACK, Aspect_TOL_SOLID, 1.0));
  *theDrawer->LineAspect()->Aspect() = *theDrawer->Link()->LineAspect()->Aspect();
  theDrawer->SetWireAspect(new Prs3d_LineAspect(Quantity_NOC_BLACK, Aspect_TOL_SOLID, 1.0));
  *theDrawer->WireAspect()->Aspect() = *theDrawer->Link()->WireAspect()->Aspect();
  theDrawer->SetPlaneAspect(new Prs3d_PlaneAspect());
  *theDrawer->PlaneAspect()->EdgesAspect() = *theDrawer->Link()->PlaneAspect()->EdgesAspect();
  theDrawer->SetFreeBoundaryAspect(new Prs3d_LineAspect(Quantity_NOC_BLACK, Aspect_TOL_SOLID, 1.0));
  *theDrawer->FreeBoundaryAspect()->Aspect() = *theDrawer->Link()->FreeBoundaryAspect()->Aspect();
  theDrawer->SetUnFreeBoundaryAspect(
    new Prs3d_LineAspect(Quantity_NOC_BLACK, Aspect_TOL_SOLID, 1.0));
  *theDrawer->UnFreeBoundaryAspect()->Aspect() =
    *theDrawer->Link()->UnFreeBoundaryAspect()->Aspect();
  theDrawer->SetDatumAspect(new Prs3d_DatumAspect());

  theDrawer->ShadingAspect()->SetColor(theColor);
  theDrawer->WireAspect()->SetColor(theColor);
  theDrawer->LineAspect()->SetColor(theColor);
  theDrawer->PlaneAspect()->ArrowAspect()->SetColor(theColor);
  theDrawer->PlaneAspect()->IsoAspect()->SetColor(theColor);
  theDrawer->PlaneAspect()->EdgesAspect()->SetColor(theColor);
  theDrawer->FreeBoundaryAspect()->SetColor(theColor);
  theDrawer->UnFreeBoundaryAspect()->SetColor(theColor);
  theDrawer->PointAspect()->SetColor(theColor);
  for (Standard_Integer aPartIter = 0; aPartIter < Prs3d_DatumParts_None; ++aPartIter)
  {
    if (Handle(Prs3d_LineAspect) aLineAsp =
          theDrawer->DatumAspect()->LineAspect((Prs3d_DatumParts)aPartIter))
    {
      aLineAsp->SetColor(theColor);
    }
  }

  theDrawer->WireAspect()->SetWidth(2.0);
  theDrawer->LineAspect()->SetWidth(2.0);
  theDrawer->PlaneAspect()->EdgesAspect()->SetWidth(2.0);
  theDrawer->FreeBoundaryAspect()->SetWidth(2.0);
  theDrawer->UnFreeBoundaryAspect()->SetWidth(2.0);
  theDrawer->PointAspect()->SetTypeOfMarker(Aspect_TOM_O_POINT);
  theDrawer->PointAspect()->SetScale(2.0);

  // the triangulation should be computed using main presentation attributes,
  // and should not be overridden by highlighting
  theDrawer->SetAutoTriangulation(Standard_False);
}
} // namespace

//=================================================================================================

AIS_InteractiveContext::AIS_InteractiveContext(const Handle(V3d_Viewer)& MainViewer)
    : myMainPM(new PrsMgr_PresentationManager(MainViewer->StructureManager())),
      myMainVwr(MainViewer),
      myToHilightSelected(Standard_True),
      mySelection(new AIS_Selection()),
      myFilters(new SelectMgr_AndOrFilter(SelectMgr_FilterType_OR)),
      myDefaultDrawer(new Prs3d_Drawer()),
      myCurDetected(0),
      myCurHighlighted(0),
      myPickingStrategy(SelectMgr_PickingStrategy_FirstAcceptable),
      myAutoHilight(Standard_True),
      myIsAutoActivateSelMode(Standard_True)
{
  mgrSelector = new SelectMgr_SelectionManager(new StdSelect_ViewerSelector3d());

  myStyles[Prs3d_TypeOfHighlight_None]          = myDefaultDrawer;
  myStyles[Prs3d_TypeOfHighlight_Selected]      = new Prs3d_Drawer();
  myStyles[Prs3d_TypeOfHighlight_Dynamic]       = new Prs3d_Drawer();
  myStyles[Prs3d_TypeOfHighlight_LocalSelected] = new Prs3d_Drawer();
  myStyles[Prs3d_TypeOfHighlight_LocalDynamic]  = new Prs3d_Drawer();
  myStyles[Prs3d_TypeOfHighlight_SubIntensity]  = new Prs3d_Drawer();

  myDefaultDrawer->SetupOwnDefaults();
  myDefaultDrawer->SetZLayer(Graphic3d_ZLayerId_Default);
  myDefaultDrawer->SetDisplayMode(0);
  {
    const Handle(Prs3d_Drawer)& aStyle = myStyles[Prs3d_TypeOfHighlight_Dynamic];
    aStyle->Link(myDefaultDrawer);
    initDefaultHilightAttributes(aStyle, Quantity_NOC_CYAN1);
    aStyle->SetZLayer(Graphic3d_ZLayerId_Top);
  }
  {
    const Handle(Prs3d_Drawer)& aStyle = myStyles[Prs3d_TypeOfHighlight_LocalDynamic];
    aStyle->Link(myDefaultDrawer);
    initDefaultHilightAttributes(aStyle, Quantity_NOC_CYAN1);
    aStyle->SetZLayer(Graphic3d_ZLayerId_Topmost);
  }
  {
    const Handle(Prs3d_Drawer)& aStyle = myStyles[Prs3d_TypeOfHighlight_Selected];
    aStyle->Link(myDefaultDrawer);
    initDefaultHilightAttributes(aStyle, Quantity_NOC_GRAY80);
    aStyle->SetZLayer(Graphic3d_ZLayerId_UNKNOWN);
  }
  {
    const Handle(Prs3d_Drawer)& aStyle = myStyles[Prs3d_TypeOfHighlight_LocalSelected];
    aStyle->Link(myDefaultDrawer);
    initDefaultHilightAttributes(aStyle, Quantity_NOC_GRAY80);
    aStyle->SetZLayer(Graphic3d_ZLayerId_UNKNOWN);
  }
  {
    const Handle(Prs3d_Drawer)& aStyle = myStyles[Prs3d_TypeOfHighlight_SubIntensity];
    aStyle->SetZLayer(Graphic3d_ZLayerId_UNKNOWN);
    aStyle->SetMethod(Aspect_TOHM_COLOR);
    aStyle->SetColor(Quantity_NOC_GRAY40);
  }

  InitAttributes();
}

//=================================================================================================

AIS_InteractiveContext::~AIS_InteractiveContext()
{
  // clear the current selection
  mySelection->Clear();
  mgrSelector.Nullify();

  Handle(AIS_InteractiveContext) aNullContext;
  for (AIS_DataMapIteratorOfDataMapOfIOStatus anObjIter(myObjects); anObjIter.More();
       anObjIter.Next())
  {
    const Handle(AIS_InteractiveObject)& anObj = anObjIter.Key();
    anObj->SetContext(aNullContext);
    for (SelectMgr_SequenceOfSelection::Iterator aSelIter(anObj->Selections()); aSelIter.More();
         aSelIter.Next())
    {
      aSelIter.Value()->UpdateBVHStatus(SelectMgr_TBU_Renew);
    }
  }
}

//=================================================================================================

Handle(V3d_View) AIS_InteractiveContext::LastActiveView() const
{
  if (myLastActiveView == NULL || myMainVwr.IsNull())
  {
    return Handle(V3d_View)();
  }

  // as a precaution - check that myLastActiveView pointer is a valid active View
  for (V3d_ListOfViewIterator aViewIter = myMainVwr->ActiveViewIterator(); aViewIter.More();
       aViewIter.Next())
  {
    if (aViewIter.Value() == myLastActiveView)
    {
      return aViewIter.Value();
    }
  }
  return Handle(V3d_View)();
}

//=================================================================================================

void AIS_InteractiveContext::UpdateCurrentViewer()
{
  if (!myMainVwr.IsNull())
    myMainVwr->Update();
}

//=================================================================================================

void AIS_InteractiveContext::DisplayedObjects(AIS_ListOfInteractive& theListOfIO) const
{
  for (AIS_DataMapIteratorOfDataMapOfIOStatus anObjIter(myObjects); anObjIter.More();
       anObjIter.Next())
  {
    if (anObjIter.Key()->DisplayStatus() == PrsMgr_DisplayStatus_Displayed)
    {
      theListOfIO.Append(anObjIter.Key());
    }
  }
}

//=================================================================================================

void AIS_InteractiveContext::DisplayedObjects(const AIS_KindOfInteractive theKind,
                                              const Standard_Integer      theSign,
                                              AIS_ListOfInteractive&      theListOfIO) const
{
  ObjectsByDisplayStatus(theKind, theSign, PrsMgr_DisplayStatus_Displayed, theListOfIO);
}

//=================================================================================================

void AIS_InteractiveContext::ErasedObjects(AIS_ListOfInteractive& theListOfIO) const
{
  ObjectsByDisplayStatus(PrsMgr_DisplayStatus_Erased, theListOfIO);
}

//=================================================================================================

void AIS_InteractiveContext::ErasedObjects(const AIS_KindOfInteractive theKind,
                                           const Standard_Integer      theSign,
                                           AIS_ListOfInteractive&      theListOfIO) const
{
  ObjectsByDisplayStatus(theKind, theSign, PrsMgr_DisplayStatus_Erased, theListOfIO);
}

//=================================================================================================

void AIS_InteractiveContext::ObjectsByDisplayStatus(const PrsMgr_DisplayStatus theStatus,
                                                    AIS_ListOfInteractive&     theListOfIO) const
{
  for (AIS_DataMapIteratorOfDataMapOfIOStatus anObjIter(myObjects); anObjIter.More();
       anObjIter.Next())
  {
    if (anObjIter.Key()->DisplayStatus() == theStatus)
    {
      theListOfIO.Append(anObjIter.Key());
    }
  }
}

//=================================================================================================

void AIS_InteractiveContext::ObjectsByDisplayStatus(const AIS_KindOfInteractive theKind,
                                                    const Standard_Integer      theSign,
                                                    const PrsMgr_DisplayStatus  theStatus,
                                                    AIS_ListOfInteractive&      theListOfIO) const
{
  for (AIS_DataMapIteratorOfDataMapOfIOStatus anObjIter(myObjects); anObjIter.More();
       anObjIter.Next())
  {
    if (theStatus != PrsMgr_DisplayStatus_None && anObjIter.Key()->DisplayStatus() != theStatus)
    {
      continue;
    }
    else if (anObjIter.Key()->Type() != theKind)
    {
      continue;
    }

    if (theSign == -1 || anObjIter.Key()->Signature() == theSign)
    {
      theListOfIO.Append(anObjIter.Key());
    }
  }
}

//=================================================================================================

void AIS_InteractiveContext::ObjectsInside(AIS_ListOfInteractive&      theListOfIO,
                                           const AIS_KindOfInteractive theKind,
                                           const Standard_Integer      theSign) const
{
  if (theKind == AIS_KindOfInteractive_None && theSign == -1)
  {
    for (AIS_DataMapIteratorOfDataMapOfIOStatus anObjIter(myObjects); anObjIter.More();
         anObjIter.Next())
    {
      theListOfIO.Append(anObjIter.Key());
    }
    return;
  }

  for (AIS_DataMapIteratorOfDataMapOfIOStatus anObjIter(myObjects); anObjIter.More();
       anObjIter.Next())
  {
    if (anObjIter.Key()->Type() != theKind)
    {
      continue;
    }

    if (theSign == -1 || anObjIter.Key()->Signature() == theSign)
    {
      theListOfIO.Append(anObjIter.Key());
    }
  }
}

//=================================================================================================

void AIS_InteractiveContext::ObjectsForView(AIS_ListOfInteractive&     theListOfIO,
                                            const Handle(V3d_View)&    theView,
                                            const Standard_Boolean     theIsVisibleInView,
                                            const PrsMgr_DisplayStatus theStatus) const
{
  Handle(Graphic3d_CView) aViewImpl = theView->View();
  const Standard_Integer  aViewId   = aViewImpl->Identification();
  for (AIS_DataMapIteratorOfDataMapOfIOStatus anObjIter(myObjects); anObjIter.More();
       anObjIter.Next())
  {
    if (theStatus != PrsMgr_DisplayStatus_None && anObjIter.Key()->DisplayStatus() != theStatus)
    {
      theListOfIO.Append(anObjIter.Key());
      continue;
    }

    Handle(Graphic3d_ViewAffinity) anAffinity = anObjIter.Key()->ViewAffinity();
    const Standard_Boolean         isVisible  = anAffinity->IsVisible(aViewId);
    if (isVisible == theIsVisibleInView)
    {
      theListOfIO.Append(anObjIter.Key());
    }
  }
}

//=================================================================================================

void AIS_InteractiveContext::Display(const Handle(AIS_InteractiveObject)& theIObj,
                                     const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  Standard_Integer aDispMode = 0, aHiMod = -1, aSelMode = -1;
  GetDefModes(theIObj, aDispMode, aHiMod, aSelMode);
  Display(theIObj, aDispMode, myIsAutoActivateSelMode ? aSelMode : -1, theToUpdateViewer);
}

//=================================================================================================

void AIS_InteractiveContext::SetViewAffinity(const Handle(AIS_InteractiveObject)& theIObj,
                                             const Handle(V3d_View)&              theView,
                                             const Standard_Boolean               theIsVisible)
{
  if (theIObj.IsNull() || !myObjects.IsBound(theIObj))
  {
    return;
  }

  Handle(Graphic3d_ViewAffinity) anAffinity = theIObj->ViewAffinity();
  Handle(Graphic3d_CView)        aViewImpl  = theView->View();
  anAffinity->SetVisible(aViewImpl->Identification(), theIsVisible == Standard_True);
}

//=================================================================================================

void AIS_InteractiveContext::Display(const Handle(AIS_InteractiveObject)& theIObj,
                                     const Standard_Integer               theDispMode,
                                     const Standard_Integer               theSelectionMode,
                                     const Standard_Boolean               theToUpdateViewer,
                                     const PrsMgr_DisplayStatus           theDispStatus)
{
  if (theIObj.IsNull())
  {
    return;
  }

  if (theDispStatus == PrsMgr_DisplayStatus_Erased)
  {
    Erase(theIObj, theToUpdateViewer);
    Load(theIObj, theSelectionMode);
    if (Handle(AIS_GlobalStatus)* aStatusPtr = myObjects.ChangeSeek(theIObj))
    {
      (*aStatusPtr)->SetDisplayMode(theDispMode);
    }
    return;
  }

  setContextToObject(theIObj);
  if (!myObjects.IsBound(theIObj))
  {
    setObjectStatus(theIObj, PrsMgr_DisplayStatus_Displayed, theDispMode, theSelectionMode);
    theIObj->ViewAffinity()->SetVisible(true); // reset view affinity mask
    myMainVwr->StructureManager()->RegisterObject(theIObj, theIObj->ViewAffinity());
    myMainPM->Display(theIObj, theDispMode);
    if (theSelectionMode != -1)
    {
      const Handle(SelectMgr_SelectableObject)& anObj = theIObj; // to avoid ambiguity
      if (!mgrSelector->Contains(anObj))
      {
        mgrSelector->Load(theIObj);
      }
      mgrSelector->Activate(theIObj, theSelectionMode);
    }
  }
  else
  {
    Handle(AIS_GlobalStatus) aStatus = myObjects(theIObj);

    // Mark the presentation modes hidden of interactive object different from aDispMode.
    // Then make sure aDispMode is displayed and maybe highlighted.
    // Finally, activate selection mode <SelMode> if not yet activated.
    const Standard_Integer anOldMode = aStatus->DisplayMode();
    if (anOldMode != theDispMode)
    {
      if (myMainPM->IsHighlighted(theIObj, anOldMode))
      {
        unhighlightGlobal(theIObj);
      }
      myMainPM->SetVisibility(theIObj, anOldMode, Standard_False);
    }

    aStatus->SetDisplayMode(theDispMode);

    theIObj->SetDisplayStatus(PrsMgr_DisplayStatus_Displayed);
    myMainPM->Display(theIObj, theDispMode);
    if (aStatus->IsHilighted())
    {
      highlightGlobal(theIObj, aStatus->HilightStyle(), theDispMode);
    }
    if (theSelectionMode != -1)
    {
      const Handle(SelectMgr_SelectableObject)& anObj = theIObj; // to avoid ambiguity
      if (!mgrSelector->Contains(anObj))
      {
        mgrSelector->Load(theIObj);
      }
      if (!mgrSelector->IsActivated(theIObj, theSelectionMode))
      {
        aStatus->AddSelectionMode(theSelectionMode);
        mgrSelector->Activate(theIObj, theSelectionMode);
      }
    }
  }

  if (theToUpdateViewer)
  {
    myMainVwr->Update();
  }
}

//=================================================================================================

void AIS_InteractiveContext::Load(const Handle(AIS_InteractiveObject)& theIObj,
                                  const Standard_Integer               theSelMode)
{
  if (theIObj.IsNull())
  {
    return;
  }

  setContextToObject(theIObj);
  if (!myObjects.IsBound(theIObj))
  {
    Standard_Integer aDispMode, aHiMod, aSelModeDef;
    GetDefModes(theIObj, aDispMode, aHiMod, aSelModeDef);
    setObjectStatus(theIObj,
                    PrsMgr_DisplayStatus_Erased,
                    aDispMode,
                    theSelMode != -1 ? theSelMode : aSelModeDef);
    theIObj->ViewAffinity()->SetVisible(true); // reset view affinity mask
    myMainVwr->StructureManager()->RegisterObject(theIObj, theIObj->ViewAffinity());
  }

  // Register theIObj in the selection manager to prepare further activation of selection
  const Handle(SelectMgr_SelectableObject)& anObj = theIObj; // to avoid ambiguity
  if (!mgrSelector->Contains(anObj))
  {
    mgrSelector->Load(theIObj);
  }
}

//=================================================================================================

void AIS_InteractiveContext::Erase(const Handle(AIS_InteractiveObject)& theIObj,
                                   const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  if (!theIObj->IsAutoHilight())
  {
    theIObj->ClearSelected();
  }

  EraseGlobal(theIObj, Standard_False);
  if (theToUpdateViewer)
  {
    myMainVwr->Update();
  }
}

//=================================================================================================

void AIS_InteractiveContext::EraseAll(const Standard_Boolean theToUpdateViewer)
{
  for (AIS_DataMapIteratorOfDataMapOfIOStatus anObjIter(myObjects); anObjIter.More();
       anObjIter.Next())
  {
    if (anObjIter.Key()->DisplayStatus() == PrsMgr_DisplayStatus_Displayed)
    {
      Erase(anObjIter.Key(), Standard_False);
    }
  }

  if (theToUpdateViewer)
  {
    myMainVwr->Update();
  }
}

//=================================================================================================

void AIS_InteractiveContext::DisplayAll(const Standard_Boolean theToUpdateViewer)
{
  for (AIS_DataMapIteratorOfDataMapOfIOStatus anObjIter(myObjects); anObjIter.More();
       anObjIter.Next())
  {
    const PrsMgr_DisplayStatus aStatus = anObjIter.Key()->DisplayStatus();
    if (aStatus == PrsMgr_DisplayStatus_Erased)
    {
      Display(anObjIter.Key(), Standard_False);
    }
  }

  if (theToUpdateViewer)
  {
    myMainVwr->Update();
  }
}

//=================================================================================================

void AIS_InteractiveContext::DisplaySelected(const Standard_Boolean theToUpdateViewer)
{
  for (AIS_NListOfEntityOwner::Iterator aSelIter(mySelection->Objects()); aSelIter.More();
       aSelIter.Next())
  {
    Handle(AIS_InteractiveObject) anObj =
      Handle(AIS_InteractiveObject)::DownCast(aSelIter.Value()->Selectable());
    Display(anObj, Standard_False);
  }

  if (theToUpdateViewer && !mySelection->Objects().IsEmpty())
  {
    myMainVwr->Update();
  }
}

//=================================================================================================

void AIS_InteractiveContext::EraseSelected(const Standard_Boolean theToUpdateViewer)
{
  Standard_Boolean isFound = Standard_False;
  for (AIS_NListOfEntityOwner::Iterator aSelIter(mySelection->Objects()); aSelIter.More();
       aSelIter.Init(mySelection->Objects()))
  {
    Handle(AIS_InteractiveObject) anObj =
      Handle(AIS_InteractiveObject)::DownCast(aSelIter.Value()->Selectable());
    Erase(anObj, Standard_False);
    isFound = Standard_True;
  }

  if (isFound && theToUpdateViewer)
  {
    myMainVwr->Update();
  }
}

//=================================================================================================

PrsMgr_DisplayStatus AIS_InteractiveContext::DisplayStatus(
  const Handle(AIS_InteractiveObject)& theIObj) const
{
  if (theIObj.IsNull())
  {
    return PrsMgr_DisplayStatus_None;
  }
  const Handle(AIS_GlobalStatus)* aStatus = myObjects.Seek(theIObj);
  return aStatus != NULL ? theIObj->DisplayStatus() : PrsMgr_DisplayStatus_None;
}

//=================================================================================================

void AIS_InteractiveContext::Remove(const Handle(AIS_InteractiveObject)& theIObj,
                                    const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  if (theIObj->HasInteractiveContext())
  {
    if (theIObj->myCTXPtr != this)
    {
      throw Standard_ProgramError(
        "AIS_InteractiveContext - object has been displayed in another context!");
    }
    theIObj->SetContext(Handle(AIS_InteractiveContext)());
  }
  ClearGlobal(theIObj, theToUpdateViewer);
}

//=================================================================================================

void AIS_InteractiveContext::RemoveAll(const Standard_Boolean theToUpdateViewer)
{
  ClearDetected();

  AIS_ListOfInteractive aList;
  ObjectsInside(aList);
  for (AIS_ListOfInteractive::Iterator aListIterator(aList); aListIterator.More();
       aListIterator.Next())
  {
    Remove(aListIterator.Value(), Standard_False);
  }

  if (theToUpdateViewer)
  {
    myMainVwr->Update();
  }
}

//=================================================================================================

void AIS_InteractiveContext::HilightWithColor(const Handle(AIS_InteractiveObject)& theObj,
                                              const Handle(Prs3d_Drawer)&          theStyle,
                                              const Standard_Boolean               theIsToUpdate)
{
  if (theObj.IsNull())
  {
    return;
  }

  setContextToObject(theObj);
  if (!myObjects.IsBound(theObj))
  {
    return;
  }

  const Handle(AIS_GlobalStatus)& aStatus = myObjects(theObj);
  aStatus->SetHilightStatus(Standard_True);

  if (theObj->DisplayStatus() == PrsMgr_DisplayStatus_Displayed)
  {
    highlightGlobal(theObj, theStyle, aStatus->DisplayMode());
    aStatus->SetHilightStyle(theStyle);
  }

  if (theIsToUpdate)
  {
    myMainVwr->Update();
  }
}

//=================================================================================================

void AIS_InteractiveContext::Unhilight(const Handle(AIS_InteractiveObject)& theObj,
                                       const Standard_Boolean               theToUpdateViewer)
{
  Handle(AIS_GlobalStatus)* aStatus = !theObj.IsNull() ? myObjects.ChangeSeek(theObj) : NULL;
  if (aStatus == NULL)
  {
    return;
  }

  (*aStatus)->SetHilightStatus(Standard_False);
  (*aStatus)->SetHilightStyle(Handle(Prs3d_Drawer)());
  if (theObj->DisplayStatus() == PrsMgr_DisplayStatus_Displayed)
  {
    unhighlightGlobal(theObj);
  }

  if (theToUpdateViewer)
  {
    myMainVwr->Update();
  }
}

//=======================================================================
// function : IsHilighted
// purpose  : Returns true if the objects global status is set to highlighted.
//=======================================================================
Standard_Boolean AIS_InteractiveContext::IsHilighted(
  const Handle(AIS_InteractiveObject)& theObj) const
{
  const Handle(AIS_GlobalStatus)* aStatus = myObjects.Seek(theObj);
  return aStatus != NULL && (*aStatus)->IsHilighted();
}

//=======================================================================
// function : IsHilighted
// purpose  : Returns true if the owner is highlighted with selection style.
//=======================================================================
Standard_Boolean AIS_InteractiveContext::IsHilighted(
  const Handle(SelectMgr_EntityOwner)& theOwner) const
{
  if (theOwner.IsNull() || !theOwner->HasSelectable())
    return Standard_False;

  const Handle(AIS_InteractiveObject) anObj =
    Handle(AIS_InteractiveObject)::DownCast(theOwner->Selectable());

  if (anObj->GlobalSelOwner() == theOwner)
  {
    if (!myObjects.IsBound(anObj))
      return Standard_False;

    return myObjects(anObj)->IsHilighted();
  }

  const Handle(Prs3d_Drawer)& aStyle  = getSelStyle(anObj, theOwner);
  const Standard_Integer      aHiMode = getHilightMode(anObj, aStyle, -1);
  return theOwner->IsHilighted(myMainPM, aHiMode);
}

//=================================================================================================

Standard_Boolean AIS_InteractiveContext::HighlightStyle(const Handle(AIS_InteractiveObject)& theObj,
                                                        Handle(Prs3d_Drawer)& theStyle) const
{
  const Handle(AIS_GlobalStatus)* aStatus = myObjects.Seek(theObj);
  if (aStatus != NULL && (*aStatus)->IsHilighted())
  {
    theStyle = (*aStatus)->HilightStyle();
    return Standard_True;
  }

  theStyle.Nullify();
  return Standard_False;
}

//=================================================================================================

Standard_Boolean AIS_InteractiveContext::HighlightStyle(
  const Handle(SelectMgr_EntityOwner)& theOwner,
  Handle(Prs3d_Drawer)&                theStyle) const
{
  if (theOwner.IsNull() || !theOwner->HasSelectable())
    return Standard_False;

  if (IsHilighted(theOwner))
  {
    const Handle(AIS_InteractiveObject) anObj =
      Handle(AIS_InteractiveObject)::DownCast(theOwner->Selectable());
    if (anObj->GlobalSelOwner() == theOwner)
    {
      theStyle = myObjects(anObj)->HilightStyle();
    }
    else
    {
      // since part selection style is not stored in global status,
      // check if the object has own selection style. If not, it can
      // only be highlighted with default selection style (because
      // sub-intensity does not modify any selection states)
      theStyle = getSelStyle(anObj, theOwner);
    }
    return Standard_True;
  }
  else
  {
    theStyle.Nullify();
    return Standard_False;
  }
}

//=================================================================================================

Standard_Boolean AIS_InteractiveContext::IsDisplayed(
  const Handle(AIS_InteractiveObject)& theObj) const
{
  if (theObj.IsNull())
    return Standard_False;

  const Handle(AIS_GlobalStatus)* aStatus = myObjects.Seek(theObj);
  return aStatus != NULL && theObj->DisplayStatus() == PrsMgr_DisplayStatus_Displayed;
}

//=================================================================================================

Standard_Boolean AIS_InteractiveContext::IsDisplayed(const Handle(AIS_InteractiveObject)& theIObj,
                                                     const Standard_Integer theMode) const
{
  if (theIObj.IsNull())
  {
    return Standard_False;
  }

  const Handle(AIS_GlobalStatus)* aStatus = myObjects.Seek(theIObj);
  return aStatus != NULL && theIObj->DisplayStatus() == PrsMgr_DisplayStatus_Displayed
         && (*aStatus)->DisplayMode() == theMode;
}

//=================================================================================================

Graphic3d_DisplayPriority AIS_InteractiveContext::DisplayPriority(
  const Handle(AIS_InteractiveObject)& theIObj) const
{
  if (theIObj.IsNull())
  {
    return Graphic3d_DisplayPriority_INVALID;
  }

  const Handle(AIS_GlobalStatus)* aStatus = myObjects.Seek(theIObj);
  if (aStatus != NULL
      && (theIObj->DisplayStatus() == PrsMgr_DisplayStatus_Displayed
          || theIObj->DisplayStatus() == PrsMgr_DisplayStatus_Erased))
  {
    Standard_Integer aDispMode = theIObj->HasDisplayMode()
                                   ? theIObj->DisplayMode()
                                   : (theIObj->AcceptDisplayMode(myDefaultDrawer->DisplayMode())
                                        ? myDefaultDrawer->DisplayMode()
                                        : 0);
    return myMainPM->DisplayPriority(theIObj, aDispMode);
  }
  return Graphic3d_DisplayPriority_INVALID;
}

//=================================================================================================

void AIS_InteractiveContext::SetDisplayPriority(const Handle(AIS_InteractiveObject)& theIObj,
                                                const Graphic3d_DisplayPriority      thePriority)
{
  if (theIObj.IsNull())
  {
    return;
  }

  setContextToObject(theIObj);
  const Handle(AIS_GlobalStatus)* aStatus = myObjects.Seek(theIObj);
  if (aStatus != NULL
      && (theIObj->DisplayStatus() == PrsMgr_DisplayStatus_Displayed
          || theIObj->DisplayStatus() == PrsMgr_DisplayStatus_Erased))
  {
    Standard_Integer aDisplayMode = theIObj->HasDisplayMode()
                                      ? theIObj->DisplayMode()
                                      : (theIObj->AcceptDisplayMode(myDefaultDrawer->DisplayMode())
                                           ? myDefaultDrawer->DisplayMode()
                                           : 0);
    myMainPM->SetDisplayPriority(theIObj, aDisplayMode, thePriority);
  }
}

//=================================================================================================

void AIS_InteractiveContext::Redisplay(const Handle(AIS_InteractiveObject)& theIObj,
                                       const Standard_Boolean               theToUpdateViewer,
                                       const Standard_Boolean               theAllModes)
{
  RecomputePrsOnly(theIObj, theToUpdateViewer, theAllModes);
  RecomputeSelectionOnly(theIObj);
}

//=================================================================================================

void AIS_InteractiveContext::Redisplay(const AIS_KindOfInteractive theKOI,
                                       const Standard_Integer /*theSign*/,
                                       const Standard_Boolean theToUpdateViewer)
{
  Standard_Boolean isRedisplayed = Standard_False;
  for (AIS_DataMapIteratorOfDataMapOfIOStatus anObjIter(myObjects); anObjIter.More();
       anObjIter.Next())
  {
    const Handle(AIS_InteractiveObject)& anObj = anObjIter.Key();
    if (anObj->Type() != theKOI)
    {
      continue;
    }

    Redisplay(anObj, Standard_False);
    isRedisplayed =
      anObjIter.Key()->DisplayStatus() == PrsMgr_DisplayStatus_Displayed || isRedisplayed;
  }

  if (theToUpdateViewer && isRedisplayed)
  {
    myMainVwr->Update();
  }
}

//=================================================================================================

void AIS_InteractiveContext::RecomputePrsOnly(const Handle(AIS_InteractiveObject)& theIObj,
                                              const Standard_Boolean theToUpdateViewer,
                                              const Standard_Boolean theAllModes)
{
  if (theIObj.IsNull())
  {
    return;
  }

  theIObj->SetToUpdate();
  theIObj->UpdatePresentations(theAllModes);
  if (!theToUpdateViewer)
  {
    return;
  }

  const Handle(AIS_GlobalStatus)* aStatus = myObjects.Seek(theIObj);
  if (aStatus != NULL && theIObj->DisplayStatus() == PrsMgr_DisplayStatus_Displayed)
  {
    myMainVwr->Update();
  }
}

//=================================================================================================

void AIS_InteractiveContext::RecomputeSelectionOnly(const Handle(AIS_InteractiveObject)& theIO)
{
  if (theIO.IsNull())
  {
    return;
  }

  TColStd_ListOfInteger aModes;
  ActivatedModes(theIO, aModes);

  for (TColStd_ListIteratorOfListOfInteger aModesIter(aModes); aModesIter.More(); aModesIter.Next())
  {
    mgrSelector->Deactivate(theIO, aModesIter.Value());
  }

  mgrSelector->RecomputeSelection(theIO);

  const Handle(AIS_GlobalStatus)* aStatus = myObjects.Seek(theIO);
  if (aStatus == NULL || theIO->DisplayStatus() != PrsMgr_DisplayStatus_Displayed)
  {
    return;
  }

  for (TColStd_ListIteratorOfListOfInteger aModesIter(aModes); aModesIter.More(); aModesIter.Next())
  {
    mgrSelector->Activate(theIO, aModesIter.Value());
  }
}

//=================================================================================================

void AIS_InteractiveContext::Update(const Handle(AIS_InteractiveObject)& theIObj,
                                    const Standard_Boolean               theUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  theIObj->UpdatePresentations();
  mgrSelector->Update(theIObj);

  if (theUpdateViewer)
  {
    const Handle(AIS_GlobalStatus)* aStatus = myObjects.Seek(theIObj);
    if (aStatus != NULL && theIObj->DisplayStatus() == PrsMgr_DisplayStatus_Displayed)
    {
      myMainVwr->Update();
    }
  }
}

//=================================================================================================

void AIS_InteractiveContext::SetLocation(const Handle(AIS_InteractiveObject)& theIObj,
                                         const TopLoc_Location&               theLoc)
{
  if (theIObj.IsNull())
  {
    return;
  }

  if (theIObj->HasTransformation() && theLoc.IsIdentity())
  {
    theIObj->ResetTransformation();
    mgrSelector->Update(theIObj, Standard_False);
    return;
  }
  else if (theLoc.IsIdentity())
  {
    return;
  }

  // first reset the previous location to properly clean everything...
  if (theIObj->HasTransformation())
  {
    theIObj->ResetTransformation();
  }

  theIObj->SetLocalTransformation(theLoc.Transformation());

  mgrSelector->Update(theIObj, Standard_False);

  // if the object or its part is highlighted dynamically, it is necessary to apply location
  // transformation to its highlight structure immediately
  if (!myLastPicked.IsNull() && myLastPicked->IsSameSelectable(theIObj))
  {
    const Standard_Integer aHiMod = theIObj->HasHilightMode() ? theIObj->HilightMode() : 0;
    myLastPicked->UpdateHighlightTrsf(myMainVwr, myMainPM, aHiMod);
  }
}

//=================================================================================================

void AIS_InteractiveContext::ResetLocation(const Handle(AIS_InteractiveObject)& theIObj)
{
  if (theIObj.IsNull())
  {
    return;
  }

  theIObj->ResetTransformation();
  mgrSelector->Update(theIObj, Standard_False);
}

//=================================================================================================

Standard_Boolean AIS_InteractiveContext::HasLocation(
  const Handle(AIS_InteractiveObject)& theIObj) const
{
  return !theIObj.IsNull() && theIObj->HasTransformation();
}

//=================================================================================================

TopLoc_Location AIS_InteractiveContext::Location(const Handle(AIS_InteractiveObject)& theIObj) const
{
  return theIObj->Transformation();
}

//=================================================================================================

void AIS_InteractiveContext::SetDisplayMode(const Standard_Integer theMode,
                                            const Standard_Boolean theToUpdateViewer)
{
  if (theMode == myDefaultDrawer->DisplayMode())
  {
    return;
  }

  for (AIS_DataMapIteratorOfDataMapOfIOStatus anObjIter(myObjects); anObjIter.More();
       anObjIter.Next())
  {
    Handle(AIS_InteractiveObject) anObj     = anObjIter.Key();
    Standard_Boolean              toProcess = anObj->IsKind(STANDARD_TYPE(AIS_Shape))
                                 || anObj->IsKind(STANDARD_TYPE(AIS_ConnectedInteractive))
                                 || anObj->IsKind(STANDARD_TYPE(AIS_MultipleConnectedInteractive));

    if (!toProcess || anObj->HasDisplayMode() || !anObj->AcceptDisplayMode(theMode))
    {
      continue;
    }

    const Handle(AIS_GlobalStatus)& aStatus = anObjIter.Value();
    aStatus->SetDisplayMode(theMode);

    if (anObj->DisplayStatus() == PrsMgr_DisplayStatus_Displayed)
    {
      myMainPM->Display(anObj, theMode);
      if (!myLastPicked.IsNull() && myLastPicked->IsSameSelectable(anObj))
      {
        myMainPM->BeginImmediateDraw();
        unhighlightGlobal(anObj);
        myMainPM->EndImmediateDraw(myMainVwr);
      }
      if (aStatus->IsSubIntensityOn())
      {
        highlightWithSubintensity(anObj, theMode);
      }
      myMainPM->SetVisibility(anObj, myDefaultDrawer->DisplayMode(), Standard_False);
    }
  }

  myDefaultDrawer->SetDisplayMode(theMode);
  if (theToUpdateViewer)
  {
    myMainVwr->Update();
  }
}

//=================================================================================================

void AIS_InteractiveContext::SetDisplayMode(const Handle(AIS_InteractiveObject)& theIObj,
                                            const Standard_Integer               theMode,
                                            const Standard_Boolean               theToUpdateViewer)
{
  setContextToObject(theIObj);
  if (!myObjects.IsBound(theIObj))
  {
    theIObj->SetDisplayMode(theMode);
    return;
  }
  else if (!theIObj->AcceptDisplayMode(theMode))
  {
    return;
  }

  Handle(AIS_GlobalStatus) aStatus = myObjects(theIObj);
  if (theIObj->DisplayStatus() != PrsMgr_DisplayStatus_Displayed)
  {
    aStatus->SetDisplayMode(theMode);
    theIObj->SetDisplayMode(theMode);
    return;
  }

  // erase presentations for all display modes different from <aMode>
  const Standard_Integer anOldMode = aStatus->DisplayMode();
  if (anOldMode != theMode)
  {
    if (myMainPM->IsHighlighted(theIObj, anOldMode))
    {
      unhighlightGlobal(theIObj);
    }
    myMainPM->SetVisibility(theIObj, anOldMode, Standard_False);
  }

  aStatus->SetDisplayMode(theMode);

  myMainPM->Display(theIObj, theMode);
  if (aStatus->IsHilighted())
  {
    highlightGlobal(theIObj, getSelStyle(theIObj, theIObj->GlobalSelOwner()), theMode);
  }
  if (aStatus->IsSubIntensityOn())
  {
    highlightWithSubintensity(theIObj, theMode);
  }

  if (theToUpdateViewer)
  {
    myMainVwr->Update();
  }
  theIObj->SetDisplayMode(theMode);
}

//=================================================================================================

void AIS_InteractiveContext::UnsetDisplayMode(const Handle(AIS_InteractiveObject)& theIObj,
                                              const Standard_Boolean theToUpdateViewer)
{
  if (theIObj.IsNull() || !theIObj->HasDisplayMode())
  {
    return;
  }

  if (!myObjects.IsBound(theIObj))
  {
    theIObj->UnsetDisplayMode();
    return;
  }

  const Standard_Integer anOldMode = theIObj->DisplayMode();
  if (myDefaultDrawer->DisplayMode() == anOldMode)
  {
    return;
  }

  const Handle(AIS_GlobalStatus)& aStatus = myObjects(theIObj);
  aStatus->SetDisplayMode(myDefaultDrawer->DisplayMode());

  if (theIObj->DisplayStatus() == PrsMgr_DisplayStatus_Displayed)
  {
    if (myMainPM->IsHighlighted(theIObj, anOldMode))
    {
      unhighlightGlobal(theIObj);
    }
    myMainPM->SetVisibility(theIObj, anOldMode, Standard_False);
    myMainPM->Display(theIObj, myDefaultDrawer->DisplayMode());
    if (aStatus->IsHilighted())
    {
      highlightSelected(theIObj->GlobalSelOwner());
    }
    if (aStatus->IsSubIntensityOn())
    {
      highlightWithSubintensity(theIObj, myDefaultDrawer->DisplayMode());
    }

    if (theToUpdateViewer)
    {
      myMainVwr->Update();
    }
  }

  theIObj->UnsetDisplayMode();
}

//=================================================================================================

void AIS_InteractiveContext::SetCurrentFacingModel(const Handle(AIS_InteractiveObject)& theIObj,
                                                   const Aspect_TypeOfFacingModel       theModel)
{
  if (!theIObj.IsNull())
  {
    theIObj->SetCurrentFacingModel(theModel);
  }
}

//=================================================================================================

void AIS_InteractiveContext::SetColor(const Handle(AIS_InteractiveObject)& theIObj,
                                      const Quantity_Color&                theColor,
                                      const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  setContextToObject(theIObj);
  theIObj->SetColor(theColor);
  theIObj->UpdatePresentations();
  if (theToUpdateViewer)
  {
    UpdateCurrentViewer();
  }
}

//=================================================================================================

void AIS_InteractiveContext::IsoOnTriangulation(const Standard_Boolean               theIsEnabled,
                                                const Handle(AIS_InteractiveObject)& theObject)
{
  if (theObject.IsNull())
  {
    return;
  }

  theObject->SetIsoOnTriangulation(theIsEnabled);
}

//=================================================================================================

void AIS_InteractiveContext::SetDeviationCoefficient(const Handle(AIS_InteractiveObject)& theIObj,
                                                     const Standard_Real    theCoefficient,
                                                     const Standard_Boolean theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  // to be modified after the related methods of AIS_Shape are passed to InteractiveObject
  setContextToObject(theIObj);
  if (theIObj->Type() != AIS_KindOfInteractive_Object
      && theIObj->Type() != AIS_KindOfInteractive_Shape)
  {
    return;
  }
  else if (theIObj->Signature() != 0)
  {
    return;
  }

  Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast(theIObj);
  aShape->SetOwnDeviationCoefficient(theCoefficient);
  aShape->UpdatePresentations();
  if (theToUpdateViewer)
  {
    UpdateCurrentViewer();
  }
}

//=================================================================================================

void AIS_InteractiveContext::SetDeviationAngle(const Handle(AIS_InteractiveObject)& theIObj,
                                               const Standard_Real                  theAngle,
                                               const Standard_Boolean theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  // To be modified after the related methods of AIS_Shape are passed to InteractiveObject
  setContextToObject(theIObj);
  if (theIObj->Type() != AIS_KindOfInteractive_Shape)
  {
    return;
  }
  else if (theIObj->Signature() != 0)
  {
    return;
  }

  Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast(theIObj);
  aShape->SetOwnDeviationAngle(theAngle);
  aShape->UpdatePresentations();
  if (theToUpdateViewer)
  {
    UpdateCurrentViewer();
  }
}

//=================================================================================================

void AIS_InteractiveContext::SetAngleAndDeviation(const Handle(AIS_InteractiveObject)& theIObj,
                                                  const Standard_Real                  theAngle,
                                                  const Standard_Boolean theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  // To be modified after the related methods of AIS_Shape are passed to InteractiveObject
  setContextToObject(theIObj);
  if (theIObj->Type() != AIS_KindOfInteractive_Shape)
  {
    return;
  }
  if (theIObj->Signature() != 0)
  {
    return;
  }

  Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast(theIObj);
  aShape->SetAngleAndDeviation(theAngle);
  aShape->UpdatePresentations();
  if (theToUpdateViewer)
  {
    UpdateCurrentViewer();
  }
}

//=================================================================================================

void AIS_InteractiveContext::UnsetColor(const Handle(AIS_InteractiveObject)& theIObj,
                                        const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  theIObj->UnsetColor();
  theIObj->UpdatePresentations();
  if (theToUpdateViewer)
  {
    UpdateCurrentViewer();
  }
}

//=================================================================================================

Standard_Boolean AIS_InteractiveContext::HasColor(
  const Handle(AIS_InteractiveObject)& theIObj) const
{
  return theIObj->HasColor();
}

//=================================================================================================

void AIS_InteractiveContext::Color(const Handle(AIS_InteractiveObject)& theIObj,
                                   Quantity_Color&                      theColor) const
{
  theIObj->Color(theColor);
}

//=================================================================================================

Standard_Real AIS_InteractiveContext::Width(const Handle(AIS_InteractiveObject)& theIObj) const
{
  return theIObj->Width();
}

//=================================================================================================

void AIS_InteractiveContext::SetWidth(const Handle(AIS_InteractiveObject)& theIObj,
                                      const Standard_Real                  theWidth,
                                      const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  setContextToObject(theIObj);
  theIObj->SetWidth(theWidth);
  theIObj->UpdatePresentations();
  if (!myLastPicked.IsNull() && myLastPicked->IsSameSelectable(theIObj))
  {
    if (myLastPicked->IsAutoHilight())
    {
      const Standard_Integer aHiMode = theIObj->HasHilightMode() ? theIObj->HilightMode() : 0;
      myLastPicked->HilightWithColor(myMainPM,
                                     myLastPicked->IsSelected() ? getSelStyle(theIObj, myLastPicked)
                                                                : getHiStyle(theIObj, myLastPicked),
                                     aHiMode);
    }
    else
    {
      theIObj->HilightOwnerWithColor(myMainPM,
                                     myLastPicked->IsSelected() ? getSelStyle(theIObj, myLastPicked)
                                                                : getHiStyle(theIObj, myLastPicked),
                                     myLastPicked);
    }
  }
  if (theToUpdateViewer)
  {
    UpdateCurrentViewer();
  }
}

//=================================================================================================

void AIS_InteractiveContext::UnsetWidth(const Handle(AIS_InteractiveObject)& theIObj,
                                        const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  theIObj->UnsetWidth();
  theIObj->UpdatePresentations();
  if (theToUpdateViewer)
  {
    UpdateCurrentViewer();
  }
}

//=================================================================================================

void AIS_InteractiveContext::SetMaterial(const Handle(AIS_InteractiveObject)& theIObj,
                                         const Graphic3d_MaterialAspect&      theMaterial,
                                         const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  setContextToObject(theIObj);
  theIObj->SetMaterial(theMaterial);
  theIObj->UpdatePresentations();
  if (theToUpdateViewer)
  {
    UpdateCurrentViewer();
  }
}

//=================================================================================================

void AIS_InteractiveContext::UnsetMaterial(const Handle(AIS_InteractiveObject)& theIObj,
                                           const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }
  theIObj->UnsetMaterial();
  theIObj->UpdatePresentations();
  if (theToUpdateViewer)
  {
    UpdateCurrentViewer();
  }
}

//=================================================================================================

void AIS_InteractiveContext::SetTransparency(const Handle(AIS_InteractiveObject)& theIObj,
                                             const Standard_Real                  theValue,
                                             const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  setContextToObject(theIObj);
  if (!theIObj->IsTransparent() && theValue <= 0.005)
  {
    return;
  }

  if (theValue <= 0.005)
  {
    UnsetTransparency(theIObj, theToUpdateViewer);
    return;
  }

  theIObj->SetTransparency(theValue);
  theIObj->UpdatePresentations();
  if (theToUpdateViewer)
  {
    UpdateCurrentViewer();
  }
}

//=================================================================================================

void AIS_InteractiveContext::UnsetTransparency(const Handle(AIS_InteractiveObject)& theIObj,
                                               const Standard_Boolean theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  theIObj->UnsetTransparency();
  theIObj->UpdatePresentations();
  if (theToUpdateViewer)
  {
    UpdateCurrentViewer();
  }
}

//=================================================================================================

void AIS_InteractiveContext::SetSelectedAspect(const Handle(Prs3d_BasicAspect)& theAspect,
                                               const Standard_Boolean           theToUpdateViewer)
{
  Standard_DISABLE_DEPRECATION_WARNINGS Standard_Boolean isFound = Standard_False;
  for (AIS_NListOfEntityOwner::Iterator aSelIter(mySelection->Objects()); aSelIter.More();
       aSelIter.Next())
  {
    isFound = Standard_True;
    Handle(AIS_InteractiveObject) anObj =
      Handle(AIS_InteractiveObject)::DownCast(aSelIter.Value()->Selectable());
    anObj->SetAspect(theAspect);
  }
  Standard_ENABLE_DEPRECATION_WARNINGS

    if (isFound && theToUpdateViewer)
  {
    myMainVwr->Update();
  }
}

//=================================================================================================

void AIS_InteractiveContext::SetLocalAttributes(const Handle(AIS_InteractiveObject)& theIObj,
                                                const Handle(Prs3d_Drawer)&          theDrawer,
                                                const Standard_Boolean theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  setContextToObject(theIObj);
  theIObj->SetAttributes(theDrawer);
  Update(theIObj, theToUpdateViewer);
}

//=================================================================================================

void AIS_InteractiveContext::UnsetLocalAttributes(const Handle(AIS_InteractiveObject)& theIObj,
                                                  const Standard_Boolean theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  setContextToObject(theIObj);
  theIObj->UnsetAttributes();
  Update(theIObj, theToUpdateViewer);
}

//=================================================================================================

void AIS_InteractiveContext::Status(const Handle(AIS_InteractiveObject)& theIObj,
                                    TCollection_ExtendedString&          theStatus) const
{
  theStatus = "";
  if (theIObj.IsNull() || !myObjects.IsBound(theIObj))
  {
    return;
  }

  theStatus += "\t ____________________________________________";
  theStatus += "\t| Known at Neutral Point:\n\tDisplayStatus:";
  const Handle(AIS_GlobalStatus)& aStatus = myObjects(theIObj);
  switch (theIObj->DisplayStatus())
  {
    case PrsMgr_DisplayStatus_Displayed: {
      theStatus += "\t| -->Displayed\n";
      break;
    }
    case PrsMgr_DisplayStatus_Erased: {
      theStatus += "\t| -->Erased\n";
      break;
    }
    default:
      break;
  }

  theStatus += "\t| Active Display Modes in the MainViewer :\n";
  theStatus += "\t|\t Mode ";
  theStatus += TCollection_AsciiString(aStatus->DisplayMode());
  theStatus += "\n";

  if (IsSelected(theIObj))
    theStatus += "\t| Selected\n";

  theStatus += "\t| Active Selection Modes in the MainViewer :\n";
  for (TColStd_ListIteratorOfListOfInteger aSelModeIter(aStatus->SelectionModes());
       aSelModeIter.More();
       aSelModeIter.Next())
  {
    theStatus += "\t\t Mode ";
    theStatus += TCollection_AsciiString(aSelModeIter.Value());
    theStatus += "\n";
  }
  theStatus += "\t ____________________________________________";
}

//=================================================================================================

void AIS_InteractiveContext::GetDefModes(const Handle(AIS_InteractiveObject)& theIObj,
                                         Standard_Integer&                    theDispMode,
                                         Standard_Integer&                    theHiMode,
                                         Standard_Integer&                    theSelMode) const
{
  if (theIObj.IsNull())
  {
    return;
  }

  theDispMode =
    theIObj->HasDisplayMode()
      ? theIObj->DisplayMode()
      : (theIObj->AcceptDisplayMode(myDefaultDrawer->DisplayMode()) ? myDefaultDrawer->DisplayMode()
                                                                    : 0);
  theHiMode  = theIObj->HasHilightMode() ? theIObj->HilightMode() : theDispMode;
  theSelMode = theIObj->GlobalSelectionMode();
}

//=================================================================================================

void AIS_InteractiveContext::EraseGlobal(const Handle(AIS_InteractiveObject)& theIObj,
                                         const Standard_Boolean               theToUpdateviewer)
{
  Handle(AIS_GlobalStatus) aStatus;
  if (theIObj.IsNull() || !myObjects.Find(theIObj, aStatus)
      || theIObj->DisplayStatus() == PrsMgr_DisplayStatus_Erased)
  {
    return;
  }

  const Standard_Integer aDispMode = theIObj->HasHilightMode() ? theIObj->HilightMode() : 0;
  unselectOwners(theIObj);
  myMainPM->SetVisibility(theIObj, aStatus->DisplayMode(), Standard_False);

  if (!myLastPicked.IsNull() && myLastPicked->IsSameSelectable(theIObj))
  {
    clearDynamicHighlight();
  }

  // make sure highlighting presentations are properly erased
  theIObj->ErasePresentations(false);

  if (IsSelected(theIObj) && aStatus->DisplayMode() != aDispMode)
  {
    myMainPM->SetVisibility(theIObj, aDispMode, Standard_False);
  }

  for (TColStd_ListIteratorOfListOfInteger aSelModeIter(aStatus->SelectionModes());
       aSelModeIter.More();
       aSelModeIter.Next())
  {
    mgrSelector->Deactivate(theIObj, aSelModeIter.Value());
  }
  aStatus->ClearSelectionModes();
  theIObj->SetDisplayStatus(PrsMgr_DisplayStatus_Erased);

  if (theToUpdateviewer)
  {
    myMainVwr->Update();
  }
}

//=================================================================================================

void AIS_InteractiveContext::unselectOwners(const Handle(AIS_InteractiveObject)& theObject)
{
  SelectMgr_SequenceOfOwner aSeq;
  for (AIS_NListOfEntityOwner::Iterator aSelIter(mySelection->Objects()); aSelIter.More();
       aSelIter.Next())
  {
    if (aSelIter.Value()->IsSameSelectable(theObject))
    {
      aSeq.Append(aSelIter.Value());
    }
  }
  for (SelectMgr_SequenceOfOwner::Iterator aDelIter(aSeq); aDelIter.More(); aDelIter.Next())
  {
    AddOrRemoveSelected(aDelIter.Value(), Standard_False);
  }
}

//=================================================================================================

void AIS_InteractiveContext::ClearGlobal(const Handle(AIS_InteractiveObject)& theIObj,
                                         const Standard_Boolean               theToUpdateviewer)
{
  Handle(AIS_GlobalStatus) aStatus;
  if (theIObj.IsNull() || !myObjects.Find(theIObj, aStatus))
  {
    // for cases when reference shape of connected interactives was not displayed
    // but its selection primitives were calculated
    const Handle(SelectMgr_SelectableObject)& anObj = theIObj; // to avoid ambiguity
    mgrSelector->Remove(anObj);
    return;
  }

  unselectOwners(theIObj);

  myMainPM->Erase(theIObj, -1);
  theIObj->ErasePresentations(true); // make sure highlighting presentations are properly erased

  // Object removes from Detected sequence
  for (Standard_Integer aDetIter = myDetectedSeq.Lower(); aDetIter <= myDetectedSeq.Upper();)
  {
    Handle(SelectMgr_EntityOwner) aPicked = MainSelector()->Picked(myDetectedSeq(aDetIter));
    Handle(AIS_InteractiveObject) anObj;
    if (!aPicked.IsNull())
    {
      anObj = Handle(AIS_InteractiveObject)::DownCast(aPicked->Selectable());
    }

    if (!anObj.IsNull() && anObj == theIObj)
    {
      myDetectedSeq.Remove(aDetIter);
      if (myCurDetected == aDetIter)
      {
        myCurDetected = Min(myDetectedSeq.Upper(), aDetIter);
      }
      if (myCurHighlighted == aDetIter)
      {
        myCurHighlighted = 0;
      }
    }
    else
    {
      aDetIter++;
    }
  }

  // remove IO from the selection manager to avoid memory leaks
  const Handle(SelectMgr_SelectableObject)& anObj = theIObj; // to avoid ambiguity
  mgrSelector->Remove(anObj);

  setObjectStatus(theIObj, PrsMgr_DisplayStatus_None, -1, -1);
  theIObj->ViewAffinity()->SetVisible(true); // reset view affinity mask
  myMainVwr->StructureManager()->UnregisterObject(theIObj);

  if (!myLastPicked.IsNull())
  {
    if (myLastPicked->IsSameSelectable(theIObj))
    {
      clearDynamicHighlight();
      myLastPicked.Nullify();
    }
  }

  if (theToUpdateviewer && theIObj->DisplayStatus() == PrsMgr_DisplayStatus_Displayed)
  {
    myMainVwr->Update();
  }
}

//=================================================================================================

void AIS_InteractiveContext::ClearGlobalPrs(const Handle(AIS_InteractiveObject)& theIObj,
                                            const Standard_Integer               theMode,
                                            const Standard_Boolean               theToUpdateViewer)
{
  const Handle(AIS_GlobalStatus)* aStatus = !theIObj.IsNull() ? myObjects.Seek(theIObj) : NULL;
  if (aStatus == NULL)
  {
    return;
  }

  if ((*aStatus)->DisplayMode() == theMode)
  {
    const Standard_Integer aDispMode = theIObj->HasHilightMode() ? theIObj->HilightMode() : 0;
    if (aDispMode == theMode && myMainPM->IsHighlighted(theIObj, theMode))
    {
      unhighlightGlobal(theIObj);
    }

    myMainPM->Erase(theIObj, theMode);
  }

  if (theIObj->DisplayStatus() == PrsMgr_DisplayStatus_Displayed && theToUpdateViewer)
  {
    myMainVwr->Update();
  }
}

//=================================================================================================

Standard_Boolean AIS_InteractiveContext::ClearDetected(Standard_Boolean theToRedrawImmediate)
{
  myCurDetected    = 0;
  myCurHighlighted = 0;
  myDetectedSeq.Clear();
  Standard_Boolean toUpdate = Standard_False;
  if (!myLastPicked.IsNull() && myLastPicked->HasSelectable())
  {
    toUpdate = Standard_True;
    clearDynamicHighlight();
  }
  myLastPicked.Nullify();
  MainSelector()->ClearPicked();
  if (toUpdate && theToRedrawImmediate)
  {
    myMainVwr->RedrawImmediate();
  }
  return toUpdate;
}

//=================================================================================================

void AIS_InteractiveContext::SetIsoNumber(const Standard_Integer theNb, const AIS_TypeOfIso theType)
{
  switch (theType)
  {
    case AIS_TOI_IsoU:
      myDefaultDrawer->UIsoAspect()->SetNumber(theNb);
      break;
    case AIS_TOI_IsoV:
      myDefaultDrawer->VIsoAspect()->SetNumber(theNb);
      break;
    case AIS_TOI_Both:
      myDefaultDrawer->UIsoAspect()->SetNumber(theNb);
      myDefaultDrawer->VIsoAspect()->SetNumber(theNb);
      break;
  }
}

//=================================================================================================

Standard_Integer AIS_InteractiveContext::IsoNumber(const AIS_TypeOfIso theType)
{
  switch (theType)
  {
    case AIS_TOI_IsoU:
      return myDefaultDrawer->UIsoAspect()->Number();
    case AIS_TOI_IsoV:
      return myDefaultDrawer->VIsoAspect()->Number();
    case AIS_TOI_Both:
      return myDefaultDrawer->UIsoAspect()->Number() == myDefaultDrawer->VIsoAspect()->Number()
               ? myDefaultDrawer->UIsoAspect()->Number()
               : -1;
  }
  return 0;
}

//=================================================================================================

void AIS_InteractiveContext::SetPixelTolerance(const Standard_Integer thePrecision)
{
  MainSelector()->SetPixelTolerance(thePrecision);
}

//=================================================================================================

Standard_Integer AIS_InteractiveContext::PixelTolerance() const
{
  return MainSelector()->PixelTolerance();
}

//=======================================================================
// function : SetSelectionSensitivity
// purpose  : Allows to manage sensitivity of a particular selection of interactive object theObject
//=======================================================================
void AIS_InteractiveContext::SetSelectionSensitivity(const Handle(AIS_InteractiveObject)& theObject,
                                                     const Standard_Integer               theMode,
                                                     const Standard_Integer theNewSensitivity)
{
  mgrSelector->SetSelectionSensitivity(theObject, theMode, theNewSensitivity);
}

//=================================================================================================

void AIS_InteractiveContext::InitAttributes()
{
  Graphic3d_MaterialAspect aMat(Graphic3d_NameOfMaterial_Brass);
  myDefaultDrawer->ShadingAspect()->SetMaterial(aMat);

  //  myDefaultDrawer->ShadingAspect()->SetColor(Quantity_NOC_GRAY70);
  Handle(Prs3d_LineAspect) aLineAspect = myDefaultDrawer->HiddenLineAspect();
  aLineAspect->SetColor(Quantity_NOC_GRAY20);
  aLineAspect->SetWidth(1.0);
  aLineAspect->SetTypeOfLine(Aspect_TOL_DASH);

  // tolerance to 2 pixels...
  SetPixelTolerance(2);

  // Customizing the drawer for trihedrons and planes...
  Handle(Prs3d_DatumAspect) aTrihAspect = myDefaultDrawer->DatumAspect();
  const Standard_Real       aLength     = 100.0;
  aTrihAspect->SetAxisLength(aLength, aLength, aLength);
  const Quantity_Color aColor = Quantity_NOC_LIGHTSTEELBLUE4;
  aTrihAspect->LineAspect(Prs3d_DatumParts_XAxis)->SetColor(aColor);
  aTrihAspect->LineAspect(Prs3d_DatumParts_YAxis)->SetColor(aColor);
  aTrihAspect->LineAspect(Prs3d_DatumParts_ZAxis)->SetColor(aColor);

  Handle(Prs3d_PlaneAspect) aPlaneAspect = myDefaultDrawer->PlaneAspect();
  const Standard_Real       aPlaneLength = 200.0;
  aPlaneAspect->SetPlaneLength(aPlaneLength, aPlaneLength);
  aPlaneAspect->EdgesAspect()->SetColor(Quantity_NOC_SKYBLUE);
}

//=================================================================================================

Standard_Real AIS_InteractiveContext::TrihedronSize() const
{
  return myDefaultDrawer->DatumAspect()->AxisLength(Prs3d_DatumParts_XAxis);
}

//=================================================================================================

void AIS_InteractiveContext::SetTrihedronSize(const Standard_Real theVal,
                                              const Standard_Boolean /*updateviewer*/)
{
  myDefaultDrawer->DatumAspect()->SetAxisLength(theVal, theVal, theVal);
  Redisplay(AIS_KindOfInteractive_Datum, 3, Standard_False);
  Redisplay(AIS_KindOfInteractive_Datum, 4, Standard_True);
}

//=================================================================================================

void AIS_InteractiveContext::SetPlaneSize(const Standard_Real    theValX,
                                          const Standard_Real    theValY,
                                          const Standard_Boolean theToUpdateViewer)
{
  myDefaultDrawer->PlaneAspect()->SetPlaneLength(theValX, theValY);
  Redisplay(AIS_KindOfInteractive_Datum, 7, theToUpdateViewer);
}

//=================================================================================================

void AIS_InteractiveContext::SetPlaneSize(const Standard_Real    theVal,
                                          const Standard_Boolean theToUpdateViewer)
{
  SetPlaneSize(theVal, theVal, theToUpdateViewer);
}

//=================================================================================================

Standard_Boolean AIS_InteractiveContext::PlaneSize(Standard_Real& theX, Standard_Real& theY) const
{
  theX = myDefaultDrawer->PlaneAspect()->PlaneXLength();
  theY = myDefaultDrawer->PlaneAspect()->PlaneYLength();
  return (Abs(theX - theY) <= Precision::Confusion());
}

//=================================================================================================

void AIS_InteractiveContext::SetZLayer(const Handle(AIS_InteractiveObject)& theIObj,
                                       const Graphic3d_ZLayerId             theLayerId)
{
  if (theIObj.IsNull())
    return;

  theIObj->SetZLayer(theLayerId);
}

//=================================================================================================

Graphic3d_ZLayerId AIS_InteractiveContext::GetZLayer(
  const Handle(AIS_InteractiveObject)& theIObj) const
{
  return !theIObj.IsNull() ? theIObj->ZLayer() : Graphic3d_ZLayerId_UNKNOWN;
}

//=======================================================================
// function : RebuildSelectionStructs
// purpose  : Rebuilds 1st level of BVH selection forcibly
//=======================================================================
void AIS_InteractiveContext::RebuildSelectionStructs()
{
  MainSelector()->RebuildObjectsTree(Standard_True);
}

//=======================================================================
// function : Disconnect
// purpose  : Disconnects selectable object from an assembly and updates selection structures
//=======================================================================
void AIS_InteractiveContext::Disconnect(const Handle(AIS_InteractiveObject)& theAssembly,
                                        const Handle(AIS_InteractiveObject)& theObjToDisconnect)
{
  if (theAssembly->IsInstance(STANDARD_TYPE(AIS_MultipleConnectedInteractive)))
  {
    Handle(AIS_MultipleConnectedInteractive) theObj(
      Handle(AIS_MultipleConnectedInteractive)::DownCast(theAssembly));
    theObj->Disconnect(theObjToDisconnect);
    if (!myObjects.IsBound(theObjToDisconnect))
    {
      // connected presentation might contain displayed presentations
      myMainPM->Erase(theObjToDisconnect, -1);
      theObjToDisconnect->ErasePresentations(true);
    }

    const Handle(SelectMgr_SelectableObject)& anObj = theObjToDisconnect; // to avoid ambiguity
    mgrSelector->Remove(anObj);
  }
  else if (theAssembly->IsInstance(STANDARD_TYPE(AIS_ConnectedInteractive))
           && theObjToDisconnect.IsNull())
  {
    Handle(AIS_ConnectedInteractive) theObj(
      Handle(AIS_ConnectedInteractive)::DownCast(theAssembly));
    theObj->Disconnect();
    const Handle(SelectMgr_SelectableObject)& anObj = theObj; // to avoid ambiguity
    mgrSelector->Remove(anObj);
  }
  else
    return;
}

//=======================================================================
// function : FitSelected
// purpose  : Fits the view corresponding to the bounds of selected objects
//=======================================================================
void AIS_InteractiveContext::FitSelected(const Handle(V3d_View)& theView)
{
  FitSelected(theView, 0.01, Standard_True);
}

//=================================================================================================

Bnd_Box AIS_InteractiveContext::BoundingBoxOfSelection(const Handle(V3d_View)& theView) const
{
  Bnd_Box                aBndSelected;
  AIS_MapOfObjectOwners  anObjectOwnerMap;
  const Standard_Integer aViewId = !theView.IsNull() ? theView->View()->Identification() : -1;
  for (AIS_NListOfEntityOwner::Iterator aSelIter(mySelection->Objects()); aSelIter.More();
       aSelIter.Next())
  {
    const Handle(SelectMgr_EntityOwner)& anOwner = aSelIter.Value();
    Handle(AIS_InteractiveObject)        anObj =
      Handle(AIS_InteractiveObject)::DownCast(anOwner->Selectable());
    if (anObj->IsInfinite())
    {
      continue;
    }

    Handle(Graphic3d_ViewAffinity) anAffinity = anObj->ViewAffinity();
    const Standard_Boolean         isVisible  = aViewId == -1 || anAffinity->IsVisible(aViewId);
    if (!isVisible)
    {
      continue;
    }

    if (anOwner == anObj->GlobalSelOwner())
    {
      Bnd_Box aTmpBnd;
      anObj->BoundingBox(aTmpBnd);
      aBndSelected.Add(aTmpBnd);
    }
    else
    {
      Handle(SelectMgr_IndexedMapOfOwner) anOwnerMap;
      if (!anObjectOwnerMap.Find(anOwner->Selectable(), anOwnerMap))
      {
        anOwnerMap = new SelectMgr_IndexedMapOfOwner();
        anObjectOwnerMap.Bind(anOwner->Selectable(), anOwnerMap);
      }

      anOwnerMap->Add(anOwner);
    }
  }

  for (AIS_MapIteratorOfMapOfObjectOwners anIter(anObjectOwnerMap); anIter.More(); anIter.Next())
  {
    const Handle(SelectMgr_SelectableObject)& anObject = anIter.Key();
    Bnd_Box aTmpBox = anObject->BndBoxOfSelected(anIter.ChangeValue());
    aBndSelected.Add(aTmpBox);
  }

  return aBndSelected;
}

//=======================================================================
// function : FitSelected
// purpose  : Fits the view corresponding to the bounds of selected objects
//=======================================================================
void AIS_InteractiveContext::FitSelected(const Handle(V3d_View)& theView,
                                         const Standard_Real     theMargin,
                                         const Standard_Boolean  theToUpdate)
{
  Bnd_Box aBndSelected = BoundingBoxOfSelection(theView);
  if (!aBndSelected.IsVoid())
  {
    theView->FitAll(aBndSelected, theMargin, theToUpdate);
  }
}

//=================================================================================================

void AIS_InteractiveContext::SetTransformPersistence(
  const Handle(AIS_InteractiveObject)&   theObject,
  const Handle(Graphic3d_TransformPers)& theTrsfPers)
{
  theObject->SetTransformPersistence(theTrsfPers);
  if (!myObjects.IsBound(theObject))
  {
    return;
  }

  mgrSelector->UpdateSelection(theObject);

  const Graphic3d_ZLayerId  aLayerId   = theObject->ZLayer();
  const Handle(V3d_Viewer)& aCurViewer = CurrentViewer();
  for (V3d_ListOfViewIterator anActiveViewIter(aCurViewer->ActiveViewIterator());
       anActiveViewIter.More();
       anActiveViewIter.Next())
  {
    anActiveViewIter.Value()->View()->InvalidateBVHData(aLayerId);
    anActiveViewIter.Value()->View()->InvalidateZLayerBoundingBox(aLayerId);
  }
}

//=================================================================================================

gp_Pnt AIS_InteractiveContext::GravityPoint(const Handle(V3d_View)& theView) const
{
  return theView->GravityPoint();
}

//=================================================================================================

void AIS_InteractiveContext::setContextToObject(const Handle(AIS_InteractiveObject)& theObj)
{
  if (theObj->HasInteractiveContext())
  {
    if (theObj->myCTXPtr != this)
    {
      throw Standard_ProgramError(
        "AIS_InteractiveContext - object has been already displayed in another context!");
    }
  }
  else
  {
    theObj->SetContext(this);
  }

  for (PrsMgr_ListOfPresentableObjectsIter aPrsIter(theObj->Children()); aPrsIter.More();
       aPrsIter.Next())
  {
    if (Handle(AIS_InteractiveObject) aChild =
          Handle(AIS_InteractiveObject)::DownCast(aPrsIter.Value()))
    {
      setContextToObject(aChild);
    }
  }
}

//=================================================================================================

void AIS_InteractiveContext::setObjectStatus(const Handle(AIS_InteractiveObject)& theIObj,
                                             const PrsMgr_DisplayStatus           theStatus,
                                             const Standard_Integer               theDispMode,
                                             const Standard_Integer               theSelectionMode)
{
  theIObj->SetDisplayStatus(theStatus);
  if (theStatus != PrsMgr_DisplayStatus_None)
  {
    Handle(AIS_GlobalStatus) aStatus = new AIS_GlobalStatus();
    aStatus->SetDisplayMode(theDispMode);
    if (theSelectionMode != -1)
    {
      aStatus->AddSelectionMode(theSelectionMode);
    }
    myObjects.Bind(theIObj, aStatus);
  }
  else
  {
    myObjects.UnBind(theIObj);
  }

  for (PrsMgr_ListOfPresentableObjectsIter aPrsIter(theIObj->Children()); aPrsIter.More();
       aPrsIter.Next())
  {
    Handle(AIS_InteractiveObject) aChild(Handle(AIS_InteractiveObject)::DownCast(aPrsIter.Value()));
    if (aChild.IsNull())
    {
      continue;
    }

    setObjectStatus(aChild, theStatus, theDispMode, theSelectionMode);
  }
}

//=================================================================================================

void AIS_InteractiveContext::highlightWithColor(const Handle(SelectMgr_EntityOwner)& theOwner,
                                                const Handle(V3d_Viewer)&            theViewer)
{
  const Handle(AIS_InteractiveObject) anObj =
    Handle(AIS_InteractiveObject)::DownCast(theOwner->Selectable());
  if (anObj.IsNull())
  {
    return;
  }

  const Handle(Prs3d_Drawer)& aStyle  = getHiStyle(anObj, theOwner);
  const Standard_Integer      aHiMode = getHilightMode(anObj, aStyle, -1);

  myMainPM->BeginImmediateDraw();
  theOwner->HilightWithColor(myMainPM, aStyle, aHiMode);
  myMainPM->EndImmediateDraw(theViewer.IsNull() ? myMainVwr : theViewer);
}

//=================================================================================================

void AIS_InteractiveContext::highlightSelected(const Handle(SelectMgr_EntityOwner)& theOwner)
{
  AIS_NListOfEntityOwner              anOwners;
  const Handle(AIS_InteractiveObject) anObj =
    Handle(AIS_InteractiveObject)::DownCast(theOwner->Selectable());
  if (anObj.IsNull())
  {
    return;
  }

  if (!theOwner->IsAutoHilight())
  {
    SelectMgr_SequenceOfOwner aSeq;
    for (AIS_NListOfEntityOwner::Iterator aSelIter(mySelection->Objects()); aSelIter.More();
         aSelIter.Next())
    {
      if (aSelIter.Value()->IsSameSelectable(anObj))
      {
        anOwners.Append(aSelIter.Value());
      }
    }
  }
  else
  {
    anOwners.Append(theOwner);
  }
  highlightOwners(anOwners, Handle(Prs3d_Drawer)());
}

//=================================================================================================

void AIS_InteractiveContext::highlightGlobal(const Handle(AIS_InteractiveObject)& theObj,
                                             const Handle(Prs3d_Drawer)&          theStyle,
                                             const Standard_Integer               theDispMode)
{
  if (theObj.IsNull())
  {
    return;
  }

  const Standard_Integer               aHiMode    = getHilightMode(theObj, theStyle, theDispMode);
  const Handle(SelectMgr_EntityOwner)& aGlobOwner = theObj->GlobalSelOwner();

  if (aGlobOwner.IsNull())
  {
    myMainPM->Color(theObj, theStyle, aHiMode);
    return;
  }

  AIS_NListOfEntityOwner anOwners;
  if (!aGlobOwner->IsAutoHilight())
  {
    SelectMgr_SequenceOfOwner aSeq;
    for (AIS_NListOfEntityOwner::Iterator aSelIter(mySelection->Objects()); aSelIter.More();
         aSelIter.Next())
    {
      if (aSelIter.Value()->IsSameSelectable(theObj))
      {
        anOwners.Append(aSelIter.Value());
      }
    }
  }
  else
  {
    anOwners.Append(aGlobOwner);
  }
  highlightOwners(anOwners, theStyle);
}

//=================================================================================================

void AIS_InteractiveContext::unhighlightSelected(const Standard_Boolean theIsToHilightSubIntensity)
{
  unhighlightOwners(mySelection->Objects(), theIsToHilightSubIntensity);
}

//=================================================================================================

void AIS_InteractiveContext::unhighlightOwners(const AIS_NListOfEntityOwner& theOwners,
                                               const Standard_Boolean theIsToHilightSubIntensity)
{
  NCollection_IndexedMap<Handle(AIS_InteractiveObject)> anObjToClear;
  for (AIS_NListOfEntityOwner::Iterator aSelIter(theOwners); aSelIter.More(); aSelIter.Next())
  {
    const Handle(SelectMgr_EntityOwner) anOwner = aSelIter.Value();
    const Handle(AIS_InteractiveObject) anInteractive =
      Handle(AIS_InteractiveObject)::DownCast(anOwner->Selectable());
    Handle(AIS_GlobalStatus)* aStatusPtr = myObjects.ChangeSeek(anInteractive);
    if (!aStatusPtr)
    {
      continue;
    }

    if (anOwner->IsAutoHilight())
    {
      anOwner->Unhilight(myMainPM);
      if (theIsToHilightSubIntensity)
      {
        if ((*aStatusPtr)->IsSubIntensityOn())
        {
          const Standard_Integer aHiMode = getHilightMode(anInteractive,
                                                          (*aStatusPtr)->HilightStyle(),
                                                          (*aStatusPtr)->DisplayMode());
          highlightWithSubintensity(anOwner, aHiMode);
        }
      }
    }
    else
    {
      anObjToClear.Add(anInteractive);
    }
    if (anOwner == anInteractive->GlobalSelOwner())
    {
      (*aStatusPtr)->SetHilightStatus(Standard_False);
    }
    (*aStatusPtr)->SetHilightStyle(Handle(Prs3d_Drawer)());
  }
  for (NCollection_IndexedMap<Handle(AIS_InteractiveObject)>::Iterator anIter(anObjToClear);
       anIter.More();
       anIter.Next())
  {
    const Handle(AIS_InteractiveObject)& anObj = anIter.Value();
    myMainPM->Unhighlight(anObj);
    anObj->ClearSelected();
  }
}

//=================================================================================================

void AIS_InteractiveContext::unhighlightGlobal(const Handle(AIS_InteractiveObject)& theObj)
{
  if (theObj.IsNull())
  {
    return;
  }

  const Handle(SelectMgr_EntityOwner)& aGlobOwner = theObj->GlobalSelOwner();
  if (aGlobOwner.IsNull())
  {
    myMainPM->Unhighlight(theObj);
    return;
  }

  AIS_NListOfEntityOwner anOwners;
  anOwners.Append(aGlobOwner);
  unhighlightOwners(anOwners);
}

//=================================================================================================

void AIS_InteractiveContext::turnOnSubintensity(const Handle(AIS_InteractiveObject)& theObject,
                                                const Standard_Integer               theDispMode,
                                                const Standard_Boolean theIsDisplayedOnly) const
{
  // the only differ with selection highlight is color, so sync transparency values
  const Handle(Prs3d_Drawer)& aSubStyle = myStyles[Prs3d_TypeOfHighlight_SubIntensity];
  aSubStyle->SetTransparency(myStyles[Prs3d_TypeOfHighlight_Selected]->Transparency());

  if (theObject.IsNull())
  {
    for (AIS_DataMapIteratorOfDataMapOfIOStatus anObjsIter(myObjects); anObjsIter.More();
         anObjsIter.Next())
    {
      const Handle(AIS_GlobalStatus)& aStatus = anObjsIter.Value();
      if (theObject->DisplayStatus() != PrsMgr_DisplayStatus_Displayed && theIsDisplayedOnly)
      {
        continue;
      }

      aStatus->SetSubIntensity(true);
      myMainPM->Color(anObjsIter.Key(),
                      aSubStyle,
                      theDispMode != -1 ? theDispMode : aStatus->DisplayMode());
    }
  }
  else
  {
    Handle(AIS_GlobalStatus) aStatus;
    if (!myObjects.Find(theObject, aStatus))
    {
      return;
    }

    if (theObject->DisplayStatus() != PrsMgr_DisplayStatus_Displayed && theIsDisplayedOnly)
    {
      return;
    }

    aStatus->SetSubIntensity(true);
    myMainPM->Color(theObject, aSubStyle, theDispMode != -1 ? theDispMode : aStatus->DisplayMode());
  }
}

//=================================================================================================

void AIS_InteractiveContext::highlightWithSubintensity(
  const Handle(AIS_InteractiveObject)& theObject,
  const Standard_Integer               theMode) const
{
  // the only differ with selection highlight is color, so
  // sync transparency values
  myStyles[Prs3d_TypeOfHighlight_SubIntensity]->SetTransparency(
    myStyles[Prs3d_TypeOfHighlight_Selected]->Transparency());

  myMainPM->Color(theObject, myStyles[Prs3d_TypeOfHighlight_SubIntensity], theMode);
}

//=================================================================================================

void AIS_InteractiveContext::highlightWithSubintensity(
  const Handle(SelectMgr_EntityOwner)& theOwner,
  const Standard_Integer               theMode) const
{
  // the only differ with selection highlight is color, so
  // sync transparency values
  myStyles[Prs3d_TypeOfHighlight_SubIntensity]->SetTransparency(
    myStyles[Prs3d_TypeOfHighlight_Selected]->Transparency());

  theOwner->HilightWithColor(myMainPM, myStyles[Prs3d_TypeOfHighlight_SubIntensity], theMode);
}

//=================================================================================================

Standard_Boolean AIS_InteractiveContext::isSlowHiStyle(
  const Handle(SelectMgr_EntityOwner)& theOwner,
  const Handle(V3d_Viewer)&            theViewer) const
{
  if (const Handle(AIS_InteractiveObject) anObj =
        Handle(AIS_InteractiveObject)::DownCast(theOwner->Selectable()))
  {
    const Handle(Prs3d_Drawer)& aHiStyle = getHiStyle(anObj, myLastPicked);
    return aHiStyle->ZLayer() == Graphic3d_ZLayerId_UNKNOWN
           || !theViewer->ZLayerSettings(aHiStyle->ZLayer()).IsImmediate();
  }
  return Standard_False;
}

//=================================================================================================

AIS_StatusOfDetection AIS_InteractiveContext::MoveTo(const Standard_Integer  theXPix,
                                                     const Standard_Integer  theYPix,
                                                     const Handle(V3d_View)& theView,
                                                     const Standard_Boolean  theToRedrawOnUpdate)
{
  if (theView->Viewer() != myMainVwr)
  {
    throw Standard_ProgramError("AIS_InteractiveContext::MoveTo() - invalid argument");
  }
  MainSelector()->Pick(theXPix, theYPix, theView);
  return moveTo(theView, theToRedrawOnUpdate);
}

//=================================================================================================

AIS_StatusOfDetection AIS_InteractiveContext::MoveTo(const gp_Ax1&           theAxis,
                                                     const Handle(V3d_View)& theView,
                                                     const Standard_Boolean  theToRedrawOnUpdate)
{
  if (theView->Viewer() != myMainVwr)
  {
    throw Standard_ProgramError("AIS_InteractiveContext::MoveTo() - invalid argument");
  }
  MainSelector()->Pick(theAxis, theView);
  return moveTo(theView, theToRedrawOnUpdate);
}

//=================================================================================================

AIS_StatusOfDetection AIS_InteractiveContext::moveTo(const Handle(V3d_View)& theView,
                                                     const Standard_Boolean  theToRedrawOnUpdate)
{
  myCurDetected    = 0;
  myCurHighlighted = 0;
  myDetectedSeq.Clear();
  myLastActiveView = theView.get();

  // preliminaries
  AIS_StatusOfDetection aStatus        = AIS_SOD_Nothing;
  Standard_Boolean      toUpdateViewer = Standard_False;

  // filling of myAISDetectedSeq sequence storing information about detected AIS objects
  // (the objects must be AIS_Shapes)
  const Standard_Integer aDetectedNb    = MainSelector()->NbPicked();
  Standard_Integer       aNewDetected   = 0;
  Standard_Boolean       toIgnoreDetTop = Standard_False;
  for (Standard_Integer aDetIter = 1; aDetIter <= aDetectedNb; ++aDetIter)
  {
    Handle(SelectMgr_EntityOwner) anOwner = MainSelector()->Picked(aDetIter);
    if (anOwner.IsNull() || !myFilters->IsOk(anOwner))
    {
      if (myPickingStrategy == SelectMgr_PickingStrategy_OnlyTopmost)
      {
        toIgnoreDetTop = Standard_True;
      }
      continue;
    }

    if (aNewDetected < 1 && !toIgnoreDetTop)
    {
      aNewDetected = aDetIter;
    }

    myDetectedSeq.Append(aDetIter);
  }

  if (aNewDetected >= 1)
  {
    myCurHighlighted = myDetectedSeq.Lower();

    // Does nothing if previously detected object is equal to the current one.
    // However in advanced selection modes the owners comparison
    // is not effective because in that case only one owner manage the
    // selection in current selection mode. It is necessary to check the current detected
    // entity and hilight it only if the detected entity is not the same as
    // previous detected (IsForcedHilight call)
    Handle(SelectMgr_EntityOwner) aNewPickedOwner = MainSelector()->Picked(aNewDetected);
    if (aNewPickedOwner == myLastPicked && !aNewPickedOwner->IsForcedHilight())
    {
      return myLastPicked->IsSelected() ? AIS_SOD_Selected : AIS_SOD_OnlyOneDetected;
    }

    // Previously detected object is unhilighted if it is not selected or hilighted
    // with selection color if it is selected. Such highlighting with selection color
    // is needed only if myToHilightSelected flag is true. In this case previously detected
    // object has been already highlighted with myHilightColor during previous MoveTo()
    // method call. As result it is necessary to rehighligt it with mySelectionColor.
    if (!myLastPicked.IsNull() && myLastPicked->HasSelectable())
    {
      if (isSlowHiStyle(myLastPicked, theView->Viewer()))
      {
        theView->Viewer()->Invalidate();
      }

      clearDynamicHighlight();
      toUpdateViewer = Standard_True;
    }

    // initialize myLastPicked field with currently detected object
    myLastPicked = aNewPickedOwner;

    // highlight detected object if it is not selected or myToHilightSelected flag is true
    if (myLastPicked->HasSelectable())
    {
      if (myAutoHilight && (!myLastPicked->IsSelected() || myToHilightSelected))
      {
        if (isSlowHiStyle(myLastPicked, theView->Viewer()))
        {
          theView->Viewer()->Invalidate();
        }

        highlightWithColor(myLastPicked, theView->Viewer());
        toUpdateViewer = Standard_True;
      }

      aStatus = myLastPicked->IsSelected() ? AIS_SOD_Selected : AIS_SOD_OnlyOneDetected;
    }
  }
  else
  {
    // previously detected object is unhilighted if it is not selected or hilighted
    // with selection color if it is selected
    aStatus = AIS_SOD_Nothing;
    if (myAutoHilight && !myLastPicked.IsNull() && myLastPicked->HasSelectable())
    {
      if (isSlowHiStyle(myLastPicked, theView->Viewer()))
      {
        theView->Viewer()->Invalidate();
      }

      clearDynamicHighlight();
      toUpdateViewer = Standard_True;
    }

    myLastPicked.Nullify();
  }

  if (toUpdateViewer && theToRedrawOnUpdate)
  {
    if (theView->ComputedMode())
    {
      theView->Viewer()->Update();
    }
    else
    {
      if (theView->IsInvalidated())
      {
        theView->Viewer()->Redraw();
      }
      else
      {
        theView->Viewer()->RedrawImmediate();
      }
    }
  }

  return aStatus;
}

//=================================================================================================

AIS_StatusOfPick AIS_InteractiveContext::AddSelect(const Handle(SelectMgr_EntityOwner)& theObject)
{
  mySelection->AddSelect(theObject);

  Standard_Integer aSelNum = NbSelected();
  return (aSelNum == 0)   ? AIS_SOP_NothingSelected
         : (aSelNum == 1) ? AIS_SOP_OneSelected
                          : AIS_SOP_SeveralSelected;
}

//=================================================================================================

AIS_StatusOfPick AIS_InteractiveContext::SelectRectangle(const Graphic3d_Vec2i&    thePntMin,
                                                         const Graphic3d_Vec2i&    thePntMax,
                                                         const Handle(V3d_View)&   theView,
                                                         const AIS_SelectionScheme theSelScheme)
{
  if (theView->Viewer() != myMainVwr)
  {
    throw Standard_ProgramError("AIS_InteractiveContext::SelectRectangle() - invalid argument");
  }

  myLastActiveView = theView.get();
  MainSelector()->Pick(thePntMin.x(), thePntMin.y(), thePntMax.x(), thePntMax.y(), theView);

  AIS_NArray1OfEntityOwner aPickedOwners;
  if (MainSelector()->NbPicked() > 0)
  {
    aPickedOwners.Resize(1, MainSelector()->NbPicked(), false);
    for (Standard_Integer aPickIter = 1; aPickIter <= MainSelector()->NbPicked(); ++aPickIter)
    {
      aPickedOwners.SetValue(aPickIter, MainSelector()->Picked(aPickIter));
    }
  }

  return Select(aPickedOwners, theSelScheme);
}

//=================================================================================================

AIS_StatusOfPick AIS_InteractiveContext::SelectPolygon(const TColgp_Array1OfPnt2d& thePolyline,
                                                       const Handle(V3d_View)&     theView,
                                                       const AIS_SelectionScheme   theSelScheme)
{
  if (theView->Viewer() != myMainVwr)
  {
    throw Standard_ProgramError("AIS_InteractiveContext::SelectPolygon() - invalid argument");
  }

  myLastActiveView = theView.get();
  MainSelector()->Pick(thePolyline, theView);

  AIS_NArray1OfEntityOwner aPickedOwners;
  if (MainSelector()->NbPicked() > 0)
  {
    aPickedOwners.Resize(1, MainSelector()->NbPicked(), false);
    for (Standard_Integer aPickIter = 1; aPickIter <= MainSelector()->NbPicked(); ++aPickIter)
    {
      aPickedOwners.SetValue(aPickIter, MainSelector()->Picked(aPickIter));
    }
  }

  return Select(aPickedOwners, theSelScheme);
}

//=================================================================================================

AIS_StatusOfPick AIS_InteractiveContext::SelectPoint(const Graphic3d_Vec2i&    thePnt,
                                                     const Handle(V3d_View)&   theView,
                                                     const AIS_SelectionScheme theSelScheme)
{
  if (theView->Viewer() != myMainVwr)
  {
    throw Standard_ProgramError("AIS_InteractiveContext::SelectPoint() - invalid argument");
  }

  myLastActiveView = theView.get();
  MainSelector()->Pick(thePnt.x(), thePnt.y(), theView);

  AIS_NArray1OfEntityOwner aPickedOwners;
  if (MainSelector()->NbPicked() > 0)
  {
    aPickedOwners.Resize(1, MainSelector()->NbPicked(), false);
    for (Standard_Integer aPickIter = 1; aPickIter <= MainSelector()->NbPicked(); ++aPickIter)
    {
      aPickedOwners.SetValue(aPickIter, MainSelector()->Picked(aPickIter));
    }
  }

  return Select(aPickedOwners, theSelScheme);
}

//=================================================================================================

AIS_StatusOfPick AIS_InteractiveContext::SelectDetected(const AIS_SelectionScheme theSelScheme)
{
  if (theSelScheme == AIS_SelectionScheme_Replace && !myLastPicked.IsNull())
  {
    Graphic3d_Vec2i aMousePos(-1, -1);
    gp_Pnt2d        aMouseRealPos = MainSelector()->GetManager().GetMousePosition();
    if (!Precision::IsInfinite(aMouseRealPos.X()) && !Precision::IsInfinite(aMouseRealPos.Y()))
    {
      aMousePos.SetValues((Standard_Integer)aMouseRealPos.X(), (Standard_Integer)aMouseRealPos.Y());
    }
    if (myLastPicked->HandleMouseClick(aMousePos,
                                       Aspect_VKeyMouse_LeftButton,
                                       Aspect_VKeyFlags_NONE,
                                       false))
    {
      return AIS_SOP_NothingSelected;
    }
  }

  AIS_NArray1OfEntityOwner aPickedOwners(1, 1);
  aPickedOwners.SetValue(1, myLastPicked);
  return Select(aPickedOwners, theSelScheme);
}

//=================================================================================================

AIS_StatusOfPick AIS_InteractiveContext::Select(const Standard_Integer  theXPMin,
                                                const Standard_Integer  theYPMin,
                                                const Standard_Integer  theXPMax,
                                                const Standard_Integer  theYPMax,
                                                const Handle(V3d_View)& theView,
                                                const Standard_Boolean  theToUpdateViewer)
{
  AIS_StatusOfPick aStatus = SelectRectangle(Graphic3d_Vec2i(theXPMin, theYPMin),
                                             Graphic3d_Vec2i(theXPMax, theYPMax),
                                             theView);
  if (theToUpdateViewer)
  {
    UpdateCurrentViewer();
  }
  return aStatus;
}

//=======================================================================
// function : Select
// purpose  : Selection by polyline
//=======================================================================
AIS_StatusOfPick AIS_InteractiveContext::Select(const TColgp_Array1OfPnt2d& thePolyline,
                                                const Handle(V3d_View)&     theView,
                                                const Standard_Boolean      theToUpdateViewer)
{
  AIS_StatusOfPick aStatus = SelectPolygon(thePolyline, theView);
  if (theToUpdateViewer)
  {
    UpdateCurrentViewer();
  }
  return aStatus;
}

//=================================================================================================

AIS_StatusOfPick AIS_InteractiveContext::Select(const Standard_Boolean theToUpdateViewer)
{
  AIS_StatusOfPick aStatus = SelectDetected();
  if (theToUpdateViewer)
  {
    UpdateCurrentViewer();
  }
  return aStatus;
}

//=================================================================================================

AIS_StatusOfPick AIS_InteractiveContext::ShiftSelect(const Standard_Boolean theToUpdateViewer)
{
  AIS_StatusOfPick aStatus = SelectDetected(AIS_SelectionScheme_XOR);
  if (theToUpdateViewer)
  {
    UpdateCurrentViewer();
  }
  return aStatus;
}

//=================================================================================================

AIS_StatusOfPick AIS_InteractiveContext::ShiftSelect(const Standard_Integer  theXPMin,
                                                     const Standard_Integer  theYPMin,
                                                     const Standard_Integer  theXPMax,
                                                     const Standard_Integer  theYPMax,
                                                     const Handle(V3d_View)& theView,
                                                     const Standard_Boolean  theToUpdateViewer)
{
  AIS_StatusOfPick aStatus = SelectRectangle(Graphic3d_Vec2i(theXPMin, theYPMin),
                                             Graphic3d_Vec2i(theXPMax, theYPMax),
                                             theView,
                                             AIS_SelectionScheme_XOR);
  if (theToUpdateViewer)
  {
    UpdateCurrentViewer();
  }
  return aStatus;
}

//=================================================================================================

AIS_StatusOfPick AIS_InteractiveContext::ShiftSelect(const TColgp_Array1OfPnt2d& thePolyline,
                                                     const Handle(V3d_View)&     theView,
                                                     const Standard_Boolean      theToUpdateViewer)
{
  AIS_StatusOfPick aStatus = SelectPolygon(thePolyline, theView, AIS_SelectionScheme_XOR);
  if (theToUpdateViewer)
  {
    UpdateCurrentViewer();
  }
  return aStatus;
}

//=================================================================================================

AIS_StatusOfPick AIS_InteractiveContext::Select(const AIS_NArray1OfEntityOwner& theOwners,
                                                const AIS_SelectionScheme       theSelScheme)
{
  NCollection_IndexedMap<Handle(SelectMgr_EntityOwner)> aSelOwnerMap(
    myAutoHilight ? mySelection->Objects().Size() : 0);
  if (myAutoHilight)
  {
    clearDynamicHighlight();

    // collect currently selected owners
    for (AIS_NListOfEntityOwner::Iterator anOwnerIter(mySelection->Objects()); anOwnerIter.More();
         anOwnerIter.Next())
    {
      aSelOwnerMap.Add(anOwnerIter.Value());
    }
  }

  mySelection->SelectOwners(theOwners,
                            theSelScheme,
                            MainSelector()->GetManager().IsOverlapAllowed(),
                            myFilters);

  if (myAutoHilight)
  {
    // collect lists of owners to unhighlight (unselected) and to highlight (selected)
    AIS_NListOfEntityOwner anOwnersToUnhighlight, anOwnersToHighlight;
    for (AIS_NListOfEntityOwner::Iterator anOwnerIter(mySelection->Objects()); anOwnerIter.More();
         anOwnerIter.Next())
    {
      // add newly selected owners
      const Handle(SelectMgr_EntityOwner)& anOwner = anOwnerIter.Value();
      if (!aSelOwnerMap.RemoveKey(anOwner))
      {
        // newly selected owner
        anOwnersToHighlight.Append(anOwner);
      }
      else
      {
        // already selected owner
        if (!anOwner->IsAutoHilight() && theSelScheme != AIS_SelectionScheme_XOR
            && theSelScheme != AIS_SelectionScheme_Add)
        {
          // hack to perform AIS_InteractiveObject::ClearSelected() before highlighting
          anOwnersToUnhighlight.Append(anOwner);
          anOwnersToHighlight.Append(anOwner);
        }
        else if (anOwner->IsForcedHilight() || !anOwner->IsAutoHilight())
        {
          anOwnersToHighlight.Append(anOwner);
        }
      }
    }

    for (NCollection_IndexedMap<Handle(SelectMgr_EntityOwner)>::Iterator anOwnerIter(aSelOwnerMap);
         anOwnerIter.More();
         anOwnerIter.Next())
    {
      // owners removed from selection
      const Handle(SelectMgr_EntityOwner)& anOwner = anOwnerIter.Value();
      anOwnersToUnhighlight.Append(anOwner);
    }

    unhighlightOwners(anOwnersToUnhighlight);
    highlightOwners(anOwnersToHighlight, Handle(Prs3d_Drawer)());
  }

  Standard_Integer aSelNum = NbSelected();
  return (aSelNum == 0)   ? AIS_SOP_NothingSelected
         : (aSelNum == 1) ? AIS_SOP_OneSelected
                          : AIS_SOP_SeveralSelected;
}

//=================================================================================================

void AIS_InteractiveContext::HilightSelected(const Standard_Boolean theToUpdateViewer)
{
  // In case of selection without using local context
  clearDynamicHighlight();

  highlightOwners(mySelection->Objects(), Handle(Prs3d_Drawer)());

  if (theToUpdateViewer)
  {
    UpdateCurrentViewer();
  }
}

//=================================================================================================

void AIS_InteractiveContext::highlightOwners(const AIS_NListOfEntityOwner& theOwners,
                                             const Handle(Prs3d_Drawer)&   theStyle)
{
  NCollection_DataMap<Handle(AIS_InteractiveObject), NCollection_Handle<SelectMgr_SequenceOfOwner>>
    anObjOwnerMap;
  for (AIS_NListOfEntityOwner::Iterator aSelIter(theOwners); aSelIter.More(); aSelIter.Next())
  {
    const Handle(SelectMgr_EntityOwner) anOwner = aSelIter.Value();
    const Handle(AIS_InteractiveObject) anObj =
      Handle(AIS_InteractiveObject)::DownCast(anOwner->Selectable());
    if (anObj.IsNull())
      continue;

    const Handle(Prs3d_Drawer)& anObjSelStyle =
      !theStyle.IsNull() ? theStyle : getSelStyle(anObj, anOwner);
    Handle(AIS_GlobalStatus)* aStatusPtr = myObjects.ChangeSeek(anObj);
    if (!aStatusPtr)
    {
      continue;
    }
    if (anOwner == anObj->GlobalSelOwner())
    {
      (*aStatusPtr)->SetHilightStatus(Standard_True);
      (*aStatusPtr)->SetHilightStyle(anObjSelStyle);
    }
    if (!anOwner->IsAutoHilight())
    {
      NCollection_Handle<SelectMgr_SequenceOfOwner> aSeq;
      if (anObjOwnerMap.Find(anObj, aSeq))
      {
        aSeq->Append(anOwner);
      }
      else
      {
        aSeq = new SelectMgr_SequenceOfOwner();
        aSeq->Append(anOwner);
        anObjOwnerMap.Bind(anObj, aSeq);
      }
    }
    else
    {
      const Standard_Integer aHiMode =
        getHilightMode(anObj, anObjSelStyle, (*aStatusPtr)->DisplayMode());
      anOwner->HilightWithColor(myMainPM, anObjSelStyle, aHiMode);
    }
  }

  if (!anObjOwnerMap.IsEmpty())
  {
    for (NCollection_DataMap<Handle(AIS_InteractiveObject),
                             NCollection_Handle<SelectMgr_SequenceOfOwner>>::Iterator
           anIter(anObjOwnerMap);
         anIter.More();
         anIter.Next())
    {
      anIter.Key()->HilightSelected(myMainPM, *anIter.Value());
    }
    anObjOwnerMap.Clear();
  }
}

//=================================================================================================

void AIS_InteractiveContext::UnhilightSelected(const Standard_Boolean theToUpdateViewer)
{
  unhighlightSelected();

  if (theToUpdateViewer)
  {
    UpdateCurrentViewer();
  }
}

//=================================================================================================

void AIS_InteractiveContext::ClearSelected(const Standard_Boolean theToUpdateViewer)
{
  if (NbSelected() == 0)
  {
    return;
  }

  if (myAutoHilight)
  {
    unhighlightSelected();
  }

  mySelection->Clear();
  if (myAutoHilight)
  {
    clearDynamicHighlight();
  }

  if (theToUpdateViewer)
  {
    UpdateCurrentViewer();
  }
}

//=================================================================================================

Standard_Boolean AIS_InteractiveContext::isDetected(const Handle(AIS_InteractiveObject)& theObject)
{
  for (Standard_Integer aDetIter = myDetectedSeq.Lower(); aDetIter <= myDetectedSeq.Upper();
       aDetIter++)
  {
    Handle(SelectMgr_EntityOwner) aPicked = MainSelector()->Picked(myDetectedSeq(aDetIter));
    Handle(AIS_InteractiveObject) anObj;
    if (!aPicked.IsNull())
    {
      anObj = Handle(AIS_InteractiveObject)::DownCast(aPicked->Selectable());
    }

    if (!anObj.IsNull() && anObj == theObject)
    {
      return Standard_True;
    }
  }
  return Standard_False;
}

//=======================================================================
// function : SetSelected
// purpose  : Sets the whole object as selected and highlights it with selection color
//=======================================================================
void AIS_InteractiveContext::SetSelected(const Handle(AIS_InteractiveObject)& theObject,
                                         const Standard_Boolean               theToUpdateViewer)
{
  if (theObject.IsNull())
  {
    return;
  }

  if (!myObjects.IsBound(theObject))
  {
    return;
  }

  Handle(SelectMgr_EntityOwner) anOwner = theObject->GlobalSelOwner();
  if (anOwner.IsNull())
  {
    return;
  }

  const Handle(Prs3d_Drawer)& anObjSelStyle = getSelStyle(theObject, anOwner);
  if (NbSelected() == 1 && myObjects(theObject)->IsHilighted() && myAutoHilight)
  {
    Handle(Prs3d_Drawer) aCustomStyle;
    if (HighlightStyle(theObject, aCustomStyle))
    {
      if (!aCustomStyle.IsNull() && anObjSelStyle != aCustomStyle)
      {
        HilightWithColor(theObject, anObjSelStyle, theToUpdateViewer);
      }
    }
    return;
  }

  for (AIS_NListOfEntityOwner::Iterator aSelIter(mySelection->Objects()); aSelIter.More();
       aSelIter.Next())
  {
    const Handle(SelectMgr_EntityOwner)& aSelOwner = aSelIter.Value();
    if (!myFilters->IsOk(aSelOwner))
    {
      continue;
    }

    Handle(AIS_InteractiveObject) aSelectable =
      Handle(AIS_InteractiveObject)::DownCast(aSelOwner->Selectable());
    if (myAutoHilight)
    {
      Unhilight(aSelectable, Standard_False);
    }
    if (aSelOwner == aSelectable->GlobalSelOwner())
    {
      if (Handle(AIS_GlobalStatus)* aStatusPtr = myObjects.ChangeSeek(aSelectable))
      {
        (*aStatusPtr)->SetHilightStatus(Standard_False);
      }
    }
  }

  // added to avoid untimely viewer update...
  const Handle(AIS_InteractiveObject) anObj =
    Handle(AIS_InteractiveObject)::DownCast(anOwner->Selectable());
  mySelection->ClearAndSelect(anOwner, myFilters, isDetected(anObj));

  if (myAutoHilight)
  {
    Handle(Prs3d_Drawer) aCustomStyle;
    if (HighlightStyle(theObject, aCustomStyle))
    {
      if (!aCustomStyle.IsNull() && anObjSelStyle != aCustomStyle)
      {
        HilightWithColor(theObject, anObjSelStyle, Standard_False);
      }
    }
    else
    {
      HilightWithColor(theObject, anObjSelStyle, Standard_False);
    }
  }

  if (theToUpdateViewer)
  {
    UpdateCurrentViewer();
  }
}

//=======================================================================
// function : SetSelected
// purpose  : Sets the whole object as selected and highlights it with selection color
//=======================================================================
void AIS_InteractiveContext::SetSelected(const Handle(SelectMgr_EntityOwner)& theOwner,
                                         const Standard_Boolean               theToUpdateViewer)
{
  if (theOwner.IsNull() || !theOwner->HasSelectable() || !myFilters->IsOk(theOwner))
  {
    return;
  }

  const Handle(AIS_InteractiveObject) anObject =
    Handle(AIS_InteractiveObject)::DownCast(theOwner->Selectable());
  const Handle(Prs3d_Drawer)& anObjSelStyle = getSelStyle(anObject, theOwner);
  if (NbSelected() == 1 && theOwner->IsSelected() && !theOwner->IsForcedHilight())
  {
    Handle(Prs3d_Drawer) aCustomStyle;
    if (myAutoHilight && HighlightStyle(theOwner, aCustomStyle))
    {
      if (!aCustomStyle.IsNull() && anObjSelStyle != aCustomStyle)
      {
        const Standard_Integer aHiMode = anObject->HasHilightMode() ? anObject->HilightMode() : 0;
        theOwner->HilightWithColor(myMainPM, anObjSelStyle, aHiMode);
      }
    }
    return;
  }

  if (!myObjects.IsBound(anObject))
  {
    return;
  }

  if (myAutoHilight)
  {
    unhighlightSelected();
  }

  mySelection->ClearAndSelect(theOwner, myFilters, isDetected(anObject));
  if (myAutoHilight)
  {
    Handle(Prs3d_Drawer) aCustomStyle;
    if (!HighlightStyle(theOwner, aCustomStyle)
        || (!aCustomStyle.IsNull() && aCustomStyle != anObjSelStyle))
    {
      highlightSelected(theOwner);
    }
  }

  if (theToUpdateViewer)
  {
    UpdateCurrentViewer();
  }
}

//=================================================================================================

void AIS_InteractiveContext::AddOrRemoveSelected(const Handle(AIS_InteractiveObject)& theObject,
                                                 const Standard_Boolean theToUpdateViewer)
{
  if (theObject.IsNull() || !myObjects.IsBound(theObject))
  {
    return;
  }

  const Handle(SelectMgr_EntityOwner) anOwner = theObject->GlobalSelOwner();
  if (!anOwner.IsNull() && anOwner->HasSelectable())
  {
    AddOrRemoveSelected(anOwner, theToUpdateViewer);
  }
}

//=======================================================================
// function : AddOrRemoveSelected
// purpose  : Allows to highlight or unhighlight the owner given depending on
//           its selection status
//=======================================================================
void AIS_InteractiveContext::AddOrRemoveSelected(const Handle(SelectMgr_EntityOwner)& theOwner,
                                                 const Standard_Boolean theToUpdateViewer)
{
  if (theOwner.IsNull() || !theOwner->HasSelectable())
  {
    return;
  }

  if (!myFilters->IsOk(theOwner) && !theOwner->IsSelected())
  {
    return;
  }

  AIS_SelectionScheme aSelScheme =
    theOwner->IsSelected() ? AIS_SelectionScheme_Remove : AIS_SelectionScheme_Add;
  const Handle(AIS_InteractiveObject) anObj =
    Handle(AIS_InteractiveObject)::DownCast(theOwner->Selectable());
  mySelection->Select(theOwner, myFilters, aSelScheme, isDetected(anObj));

  if (myAutoHilight)
  {
    Handle(AIS_GlobalStatus)* aStatusPtr = myObjects.ChangeSeek(anObj);
    if (!aStatusPtr)
    {
      return;
    }

    if (theOwner->IsSelected())
    {
      highlightSelected(theOwner);
    }
    else
    {
      AIS_NListOfEntityOwner anOwners;
      anOwners.Append(theOwner);
      unhighlightOwners(anOwners);

      (*aStatusPtr)->SetHilightStyle(Handle(Prs3d_Drawer)());
    }
  }

  if (theToUpdateViewer)
  {
    UpdateCurrentViewer();
  }
}

// =======================================================================
// function : SetSelectedState
// purpose  :
// =======================================================================
Standard_Boolean AIS_InteractiveContext::SetSelectedState(
  const Handle(SelectMgr_EntityOwner)& theEntity,
  const Standard_Boolean               theIsSelected)
{
  if (theEntity.IsNull())
  {
    throw Standard_ProgramError(
      "Internal error: AIS_InteractiveContext::SetSelectedState() called with NO object");
  }

  if (!theEntity->HasSelectable() || mySelection->IsSelected(theEntity) == theIsSelected)
  {
    return false;
  }

  if (theEntity->IsAutoHilight())
  {
    AddOrRemoveSelected(theEntity, false);
    return true;
  }

  if (theIsSelected)
  {
    const AIS_SelectStatus aSelStatus = mySelection->AddSelect(theEntity);
    theEntity->SetSelected(true);
    return aSelStatus == AIS_SS_Added;
  }
  else
  {
    const Handle(AIS_InteractiveObject) anObj =
      Handle(AIS_InteractiveObject)::DownCast(theEntity->Selectable());
    const AIS_SelectStatus aSelStatus =
      mySelection->Select(theEntity, myFilters, AIS_SelectionScheme_Remove, isDetected(anObj));
    theEntity->SetSelected(false);
    return aSelStatus == AIS_SS_Removed;
  }
}

//=================================================================================================

Standard_Boolean AIS_InteractiveContext::IsSelected(
  const Handle(AIS_InteractiveObject)& theObj) const
{
  if (theObj.IsNull())
  {
    return Standard_False;
  }

  const Handle(AIS_GlobalStatus)* aStatus = myObjects.Seek(theObj);
  if (aStatus == NULL)
  {
    return Standard_False;
  }

  const Standard_Integer       aGlobalSelMode   = theObj->GlobalSelectionMode();
  const TColStd_ListOfInteger& anActivatedModes = (*aStatus)->SelectionModes();
  for (TColStd_ListIteratorOfListOfInteger aModeIter(anActivatedModes); aModeIter.More();
       aModeIter.Next())
  {
    if (aModeIter.Value() == aGlobalSelMode)
    {
      if (Handle(SelectMgr_EntityOwner) aGlobOwner = theObj->GlobalSelOwner())
      {
        return aGlobOwner->IsSelected();
      }
      return Standard_False;
    }
  }
  return Standard_False;
}

//=================================================================================================

Handle(AIS_InteractiveObject) AIS_InteractiveContext::FirstSelectedObject() const
{
  return !mySelection->Objects().IsEmpty()
           ? Handle(AIS_InteractiveObject)::DownCast(mySelection->Objects().First()->Selectable())
           : Handle(AIS_InteractiveObject)();
}

//=================================================================================================

Standard_Boolean AIS_InteractiveContext::HasSelectedShape() const
{
  if (!mySelection->More())
  {
    return Standard_False;
  }

  const Handle(StdSelect_BRepOwner) anOwner =
    Handle(StdSelect_BRepOwner)::DownCast(mySelection->Value());
  return !anOwner.IsNull() && anOwner->HasShape();
}

//=================================================================================================

TopoDS_Shape AIS_InteractiveContext::SelectedShape() const
{
  if (!mySelection->More())
  {
    return TopoDS_Shape();
  }

  const Handle(StdSelect_BRepOwner) anOwner =
    Handle(StdSelect_BRepOwner)::DownCast(mySelection->Value());
  if (anOwner.IsNull() || !anOwner->HasSelectable())
  {
    return TopoDS_Shape();
  }

  return anOwner->Shape().Located(anOwner->Location() * anOwner->Shape().Location());
}

//=================================================================================================

void AIS_InteractiveContext::EntityOwners(Handle(SelectMgr_IndexedMapOfOwner)& theOwners,
                                          const Handle(AIS_InteractiveObject)& theIObj,
                                          const Standard_Integer               theMode) const
{
  if (theIObj.IsNull())
  {
    return;
  }

  TColStd_ListOfInteger aModes;
  if (theMode == -1)
  {
    ActivatedModes(theIObj, aModes);
  }
  else
  {
    aModes.Append(theMode);
  }

  if (theOwners.IsNull())
  {
    theOwners = new SelectMgr_IndexedMapOfOwner();
  }

  for (TColStd_ListIteratorOfListOfInteger anItr(aModes); anItr.More(); anItr.Next())
  {
    const int                          aMode = anItr.Value();
    const Handle(SelectMgr_Selection)& aSel  = theIObj->Selection(aMode);
    if (aSel.IsNull())
    {
      continue;
    }

    for (NCollection_Vector<Handle(SelectMgr_SensitiveEntity)>::Iterator aSelEntIter(
           aSel->Entities());
         aSelEntIter.More();
         aSelEntIter.Next())
    {
      if (Handle(Select3D_SensitiveEntity) aEntity = aSelEntIter.Value()->BaseSensitive())
      {
        if (const Handle(SelectMgr_EntityOwner)& aOwner = aEntity->OwnerId())
        {
          theOwners->Add(aOwner);
        }
      }
    }
  }
}

//=================================================================================================

Standard_Boolean AIS_InteractiveContext::HasDetectedShape() const
{
  Handle(StdSelect_BRepOwner) anOwner = Handle(StdSelect_BRepOwner)::DownCast(myLastPicked);
  return !anOwner.IsNull() && anOwner->HasShape();
}

//=================================================================================================

const TopoDS_Shape& AIS_InteractiveContext::DetectedShape() const
{
  Handle(StdSelect_BRepOwner) anOwner = Handle(StdSelect_BRepOwner)::DownCast(myLastPicked);
  return anOwner->Shape();
}

//=================================================================================================

Standard_Integer AIS_InteractiveContext::HilightNextDetected(
  const Handle(V3d_View)& theView,
  const Standard_Boolean  theToRedrawImmediate)
{
  myMainPM->ClearImmediateDraw();
  if (myDetectedSeq.IsEmpty())
  {
    return 0;
  }

  if (++myCurHighlighted > myDetectedSeq.Upper())
  {
    myCurHighlighted = myDetectedSeq.Lower();
  }
  const Handle(SelectMgr_EntityOwner)& anOwner =
    MainSelector()->Picked(myDetectedSeq(myCurHighlighted));
  if (anOwner.IsNull())
  {
    return 0;
  }

  highlightWithColor(anOwner, theView->Viewer());
  myLastPicked = anOwner;

  if (theToRedrawImmediate)
  {
    myMainPM->RedrawImmediate(theView->Viewer());
    myMainVwr->RedrawImmediate();
  }

  return myCurHighlighted;
}

//=================================================================================================

Standard_Integer AIS_InteractiveContext::HilightPreviousDetected(
  const Handle(V3d_View)& theView,
  const Standard_Boolean  theToRedrawImmediate)
{
  myMainPM->ClearImmediateDraw();
  if (myDetectedSeq.IsEmpty())
  {
    return 0;
  }

  if (--myCurHighlighted < myDetectedSeq.Lower())
  {
    myCurHighlighted = myDetectedSeq.Upper();
  }
  const Handle(SelectMgr_EntityOwner)& anOwner =
    MainSelector()->Picked(myDetectedSeq(myCurHighlighted));
  if (anOwner.IsNull())
  {
    return 0;
  }

  highlightWithColor(anOwner, theView->Viewer());
  myLastPicked = anOwner;

  if (theToRedrawImmediate)
  {
    myMainPM->RedrawImmediate(theView->Viewer());
    myMainVwr->RedrawImmediate();
  }

  return myCurHighlighted;
}

//=================================================================================================

Handle(SelectMgr_EntityOwner) AIS_InteractiveContext::DetectedCurrentOwner() const
{
  return MoreDetected() ? MainSelector()->Picked(myDetectedSeq(myCurDetected))
                        : Handle(SelectMgr_EntityOwner)();
}

//=================================================================================================

const TopoDS_Shape& AIS_InteractiveContext::DetectedCurrentShape() const
{
  static const TopoDS_Shape AIS_InteractiveContext_myDummyShape;

  Standard_DISABLE_DEPRECATION_WARNINGS Handle(AIS_Shape) aCurrentShape =
    Handle(AIS_Shape)::DownCast(DetectedCurrentObject());
  Standard_ENABLE_DEPRECATION_WARNINGS return !aCurrentShape.IsNull()
    ? aCurrentShape->Shape()
    : AIS_InteractiveContext_myDummyShape;
}

//=================================================================================================

Handle(AIS_InteractiveObject) AIS_InteractiveContext::DetectedCurrentObject() const
{
  return MoreDetected() ? Handle(AIS_InteractiveObject)::DownCast(
                            MainSelector()->Picked(myDetectedSeq(myCurDetected))->Selectable())
                        : Handle(AIS_InteractiveObject)();
}

//=================================================================================================

void AIS_InteractiveContext::SetSelectionModeActive(
  const Handle(AIS_InteractiveObject)& theObj,
  const Standard_Integer               theMode,
  const Standard_Boolean               theIsActive,
  const AIS_SelectionModesConcurrency  theActiveFilter,
  const Standard_Boolean               theIsForce)
{
  if (theObj.IsNull())
  {
    return;
  }

  const Handle(AIS_GlobalStatus)* aStat = myObjects.Seek(theObj);
  if (aStat == NULL)
  {
    return;
  }

  if (!theIsActive || (theMode == -1 && theActiveFilter == AIS_SelectionModesConcurrency_Single))
  {
    if (theObj->DisplayStatus() == PrsMgr_DisplayStatus_Displayed || theIsForce)
    {
      if (theMode == -1)
      {
        for (TColStd_ListIteratorOfListOfInteger aModeIter((*aStat)->SelectionModes());
             aModeIter.More();
             aModeIter.Next())
        {
          mgrSelector->Deactivate(theObj, aModeIter.Value());
        }
      }
      else
      {
        mgrSelector->Deactivate(theObj, theMode);
      }
    }

    if (theMode == -1)
    {
      (*aStat)->ClearSelectionModes();
    }
    else
    {
      (*aStat)->RemoveSelectionMode(theMode);
    }
    return;
  }
  else if (theMode == -1)
  {
    return;
  }

  if ((*aStat)->SelectionModes().Size() == 1 && (*aStat)->SelectionModes().First() == theMode)
  {
    return;
  }

  if (theObj->DisplayStatus() == PrsMgr_DisplayStatus_Displayed || theIsForce)
  {
    switch (theActiveFilter)
    {
      case AIS_SelectionModesConcurrency_Single: {
        for (TColStd_ListIteratorOfListOfInteger aModeIter((*aStat)->SelectionModes());
             aModeIter.More();
             aModeIter.Next())
        {
          mgrSelector->Deactivate(theObj, aModeIter.Value());
        }
        (*aStat)->ClearSelectionModes();
        break;
      }
      case AIS_SelectionModesConcurrency_GlobalOrLocal: {
        const Standard_Integer aGlobSelMode = theObj->GlobalSelectionMode();
        TColStd_ListOfInteger  aRemovedModes;
        for (TColStd_ListIteratorOfListOfInteger aModeIter((*aStat)->SelectionModes());
             aModeIter.More();
             aModeIter.Next())
        {
          if ((theMode == aGlobSelMode && aModeIter.Value() != aGlobSelMode)
              || (theMode != aGlobSelMode && aModeIter.Value() == aGlobSelMode))
          {
            mgrSelector->Deactivate(theObj, aModeIter.Value());
            aRemovedModes.Append(aModeIter.Value());
          }
        }
        if (aRemovedModes.Size() == (*aStat)->SelectionModes().Size())
        {
          (*aStat)->ClearSelectionModes();
        }
        else
        {
          for (TColStd_ListIteratorOfListOfInteger aModeIter(aRemovedModes); aModeIter.More();
               aModeIter.Next())
          {
            (*aStat)->RemoveSelectionMode(aModeIter.Value());
          }
        }
        break;
      }
      case AIS_SelectionModesConcurrency_Multiple: {
        break;
      }
    }
    mgrSelector->Activate(theObj, theMode);
  }
  (*aStat)->AddSelectionMode(theMode);
}

// ============================================================================
// function : Activate
// purpose  :
// ============================================================================
void AIS_InteractiveContext::Activate(const Standard_Integer theMode,
                                      const Standard_Boolean theIsForce)
{
  AIS_ListOfInteractive aDisplayedObjects;
  DisplayedObjects(aDisplayedObjects);
  for (AIS_ListOfInteractive::Iterator anIter(aDisplayedObjects); anIter.More(); anIter.Next())
  {
    Load(anIter.Value(), -1);
    Activate(anIter.Value(), theMode, theIsForce);
  }
}

// ============================================================================
// function : Deactivate
// purpose  :
// ============================================================================
void AIS_InteractiveContext::Deactivate(const Standard_Integer theMode)
{
  AIS_ListOfInteractive aDisplayedObjects;
  DisplayedObjects(aDisplayedObjects);
  for (AIS_ListOfInteractive::Iterator anIter(aDisplayedObjects); anIter.More(); anIter.Next())
  {
    Deactivate(anIter.Value(), theMode);
  }
}

// ============================================================================
// function : Deactivate
// purpose  :
// ============================================================================
void AIS_InteractiveContext::Deactivate()
{
  AIS_ListOfInteractive aDisplayedObjects;
  DisplayedObjects(aDisplayedObjects);

  for (AIS_ListOfInteractive::Iterator anIter(aDisplayedObjects); anIter.More(); anIter.Next())
  {
    Deactivate(anIter.Value());
  }
}

//=================================================================================================

void AIS_InteractiveContext::ActivatedModes(const Handle(AIS_InteractiveObject)& theObj,
                                            TColStd_ListOfInteger&               theList) const
{
  const Handle(AIS_GlobalStatus)* aStatus = myObjects.Seek(theObj);
  if (aStatus != NULL)
  {
    for (TColStd_ListIteratorOfListOfInteger aModeIter((*aStatus)->SelectionModes());
         aModeIter.More();
         aModeIter.Next())
    {
      theList.Append(aModeIter.Value());
    }
  }
}

//=================================================================================================

void AIS_InteractiveContext::SubIntensityOn(const Handle(AIS_InteractiveObject)& theObj,
                                            const Standard_Boolean               theToUpdateViewer)
{
  turnOnSubintensity(theObj);
  if (theToUpdateViewer)
  {
    myMainVwr->Update();
  }
}

//=================================================================================================

void AIS_InteractiveContext::SubIntensityOff(const Handle(AIS_InteractiveObject)& theObj,
                                             const Standard_Boolean               theToUpdateViewer)
{
  const Handle(AIS_GlobalStatus)* aStatus = myObjects.Seek(theObj);
  if (aStatus == NULL || !(*aStatus)->IsSubIntensityOn())
  {
    return;
  }

  (*aStatus)->SetSubIntensity(false);
  Standard_Boolean toUpdateMain = Standard_False;
  if (theObj->DisplayStatus() == PrsMgr_DisplayStatus_Displayed)
  {
    myMainPM->Unhighlight(theObj);
    toUpdateMain = Standard_True;
  }

  if (IsSelected(theObj))
  {
    highlightSelected(theObj->GlobalSelOwner());
  }

  if (theToUpdateViewer && toUpdateMain)
  {
    myMainVwr->Update();
  }
}

//=================================================================================================

void AIS_InteractiveContext::DisplayActiveSensitive(const Handle(V3d_View)& theView)
{
  MainSelector()->DisplaySensitive(theView);
}

//=================================================================================================

void AIS_InteractiveContext::DisplayActiveSensitive(const Handle(AIS_InteractiveObject)& theObj,
                                                    const Handle(V3d_View)&              theView)
{
  const Handle(AIS_GlobalStatus)* aStatus = myObjects.Seek(theObj);
  if (aStatus == NULL)
  {
    return;
  }

  for (TColStd_ListIteratorOfListOfInteger aModeIter((*aStatus)->SelectionModes());
       aModeIter.More();
       aModeIter.Next())
  {
    const Handle(SelectMgr_Selection)& aSel = theObj->Selection(aModeIter.Value());
    MainSelector()->DisplaySensitive(aSel, theObj->Transformation(), theView, Standard_False);
  }
}

//=================================================================================================

void AIS_InteractiveContext::ClearActiveSensitive(const Handle(V3d_View)& theView)
{
  MainSelector()->ClearSensitive(theView);
}

//=================================================================================================

Standard_Boolean AIS_InteractiveContext::IsImmediateModeOn() const
{
  return myMainPM->IsImmediateModeOn();
}

//=================================================================================================

Standard_Boolean AIS_InteractiveContext::BeginImmediateDraw()
{
  if (myMainPM->IsImmediateModeOn())
  {
    myMainPM->BeginImmediateDraw();
    return Standard_True;
  }
  return Standard_False;
}

//=================================================================================================

Standard_Boolean AIS_InteractiveContext::ImmediateAdd(const Handle(AIS_InteractiveObject)& theObj,
                                                      const Standard_Integer               theMode)
{
  if (myMainPM->IsImmediateModeOn())
  {
    myMainPM->AddToImmediateList(myMainPM->Presentation(theObj, theMode));
    return Standard_True;
  }
  return Standard_False;
}

//=================================================================================================

Standard_Boolean AIS_InteractiveContext::EndImmediateDraw(const Handle(V3d_View)& theView)
{
  if (myMainPM->IsImmediateModeOn())
  {
    myMainPM->EndImmediateDraw(theView->Viewer());
    return Standard_True;
  }
  return Standard_False;
}

//=================================================================================================

Standard_Boolean AIS_InteractiveContext::EndImmediateDraw()
{
  if (myMainPM->IsImmediateModeOn())
  {
    myMainPM->EndImmediateDraw(myMainVwr);
    return Standard_True;
  }
  return Standard_False;
}

//=================================================================================================

void AIS_InteractiveContext::SetPolygonOffsets(const Handle(AIS_InteractiveObject)& theObj,
                                               const Standard_Integer               theMode,
                                               const Standard_ShortReal             theFactor,
                                               const Standard_ShortReal             theUnits,
                                               const Standard_Boolean theToUpdateViewer)
{
  if (theObj.IsNull())
  {
    return;
  }

  setContextToObject(theObj);
  theObj->SetPolygonOffsets(theMode, theFactor, theUnits);

  const Handle(AIS_GlobalStatus)* aStatus = theToUpdateViewer ? myObjects.Seek(theObj) : NULL;
  if (aStatus != NULL && theObj->DisplayStatus() == PrsMgr_DisplayStatus_Displayed)
  {
    myMainVwr->Update();
  }
}

//=================================================================================================

Standard_Boolean AIS_InteractiveContext::HasPolygonOffsets(
  const Handle(AIS_InteractiveObject)& theObj) const
{
  return !theObj.IsNull() && theObj->HasPolygonOffsets();
}

//=================================================================================================

void AIS_InteractiveContext::PolygonOffsets(const Handle(AIS_InteractiveObject)& theObj,
                                            Standard_Integer&                    theMode,
                                            Standard_ShortReal&                  theFactor,
                                            Standard_ShortReal&                  theUnits) const
{
  if (HasPolygonOffsets(theObj))
  {
    theObj->PolygonOffsets(theMode, theFactor, theUnits);
  }
}

//=================================================================================================

void AIS_InteractiveContext::DumpJson(Standard_OStream& theOStream, Standard_Integer) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myObjects.Size())

  OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, mgrSelector.get())
  OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, myMainPM.get())
  OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, myMainVwr.get())
  OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, myLastActiveView)

  OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, myLastPicked.get())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myToHilightSelected)

  OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, mySelection.get())
  OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, myFilters.get())
  OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, myDefaultDrawer.get())

  OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, myStyles[Prs3d_TypeOfHighlight_Selected])
  OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, myStyles[Prs3d_TypeOfHighlight_Dynamic])
  OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, myStyles[Prs3d_TypeOfHighlight_LocalSelected])
  OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, myStyles[Prs3d_TypeOfHighlight_LocalDynamic])
  OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, myStyles[Prs3d_TypeOfHighlight_SubIntensity])

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myDetectedSeq.Size())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myCurDetected)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myCurHighlighted)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myPickingStrategy)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myAutoHilight)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIsAutoActivateSelMode)
}
