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
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <AIS_MapIteratorOfMapOfInteractive.hxx>
#include <AIS_MultipleConnectedInteractive.hxx>
#include <AIS_Shape.hxx>
#include <AIS_Trihedron.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <HLRBRep.hxx>
#include <OSD_Environment.hxx>
#include <Precision.hxx>
#include <Prs3d_BasicAspect.hxx>
#include <Prs3d_DatumAspect.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PlaneAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <PrsMgr_PresentableObject.hxx>
#include <Quantity_Color.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Filter.hxx>
#include <SelectMgr_OrFilter.hxx>
#include <SelectMgr_SelectionManager.hxx>
#include <Standard_Atomic.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <StdSelect_ViewerSelector3d.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_MapIteratorOfMapOfTransient.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Shape.hxx>
#include <UnitsAPI.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_InteractiveContext, Standard_Transient)

namespace
{
  typedef NCollection_DataMap<Handle(SelectMgr_SelectableObject), Handle(SelectMgr_IndexedMapOfOwner)> AIS_MapOfObjectOwners;
  typedef NCollection_DataMap<Handle(SelectMgr_SelectableObject), Handle(SelectMgr_IndexedMapOfOwner)>::Iterator AIS_MapIteratorOfMapOfObjectOwners;

  //! Initialize default highlighting attributes.
  static void initDefaultHilightAttributes (const Handle(Prs3d_Drawer)& theDrawer,
                                            const Quantity_Color& theColor)
  {
    theDrawer->SetMethod (Aspect_TOHM_COLOR);
    theDrawer->SetDisplayMode (0);
    theDrawer->SetColor (theColor);

    theDrawer->SetupOwnShadingAspect();
    theDrawer->SetupOwnPointAspect();
    theDrawer->SetLineAspect (new Prs3d_LineAspect (Quantity_NOC_BLACK, Aspect_TOL_SOLID, 1.0));
    *theDrawer->LineAspect()->Aspect() = *theDrawer->Link()->LineAspect()->Aspect();
    theDrawer->SetWireAspect (new Prs3d_LineAspect (Quantity_NOC_BLACK, Aspect_TOL_SOLID, 1.0));
    *theDrawer->WireAspect()->Aspect() = *theDrawer->Link()->WireAspect()->Aspect();
    theDrawer->SetPlaneAspect (new Prs3d_PlaneAspect());
    *theDrawer->PlaneAspect()->EdgesAspect() = *theDrawer->Link()->PlaneAspect()->EdgesAspect();
    theDrawer->SetFreeBoundaryAspect   (new Prs3d_LineAspect (Quantity_NOC_BLACK, Aspect_TOL_SOLID, 1.0));
    *theDrawer->FreeBoundaryAspect()->Aspect() = *theDrawer->Link()->FreeBoundaryAspect()->Aspect();
    theDrawer->SetUnFreeBoundaryAspect (new Prs3d_LineAspect (Quantity_NOC_BLACK, Aspect_TOL_SOLID, 1.0));
    *theDrawer->UnFreeBoundaryAspect()->Aspect() = *theDrawer->Link()->UnFreeBoundaryAspect()->Aspect();
    theDrawer->SetDatumAspect (new Prs3d_DatumAspect());

    theDrawer->ShadingAspect()->SetColor (theColor);
    theDrawer->WireAspect()->SetColor (theColor);
    theDrawer->LineAspect()->SetColor (theColor);
    theDrawer->PlaneAspect()->ArrowAspect()->SetColor (theColor);
    theDrawer->PlaneAspect()->IsoAspect()->SetColor (theColor);
    theDrawer->PlaneAspect()->EdgesAspect()->SetColor (theColor);
    theDrawer->FreeBoundaryAspect()->SetColor (theColor);
    theDrawer->UnFreeBoundaryAspect()->SetColor (theColor);
    theDrawer->PointAspect()->SetColor (theColor);
    for (Standard_Integer aPartIter = 0; aPartIter < Prs3d_DP_None; ++aPartIter)
    {
      if (Handle(Prs3d_LineAspect) aLineAsp = theDrawer->DatumAspect()->LineAspect ((Prs3d_DatumParts )aPartIter))
      {
        aLineAsp->SetColor (theColor);
      }
    }

    theDrawer->WireAspect()->SetWidth (2.0);
    theDrawer->LineAspect()->SetWidth (2.0);
    theDrawer->PlaneAspect()->EdgesAspect()->SetWidth (2.0);
    theDrawer->FreeBoundaryAspect()  ->SetWidth (2.0);
    theDrawer->UnFreeBoundaryAspect()->SetWidth (2.0);
    theDrawer->PointAspect()->SetTypeOfMarker (Aspect_TOM_O_POINT);
    theDrawer->PointAspect()->SetScale (2.0);

    // the triangulation should be computed using main presentation attributes,
    // and should not be overridden by highlighting
    theDrawer->SetAutoTriangulation (Standard_False);
  }
}

//=======================================================================
//function : AIS_InteractiveContext
//purpose  : 
//=======================================================================

AIS_InteractiveContext::AIS_InteractiveContext(const Handle(V3d_Viewer)& MainViewer):
myMainPM(new PrsMgr_PresentationManager3d(MainViewer->StructureManager())),
myMainVwr(MainViewer),
myMainSel(new StdSelect_ViewerSelector3d()),
myToHilightSelected(Standard_True),
mySelection(new AIS_Selection()),
myFilters (new SelectMgr_AndOrFilter(SelectMgr_FilterType_OR)),
myDefaultDrawer(new Prs3d_Drawer()),
myCurDetected(0),
myCurHighlighted(0),
myPickingStrategy (SelectMgr_PickingStrategy_FirstAcceptable),
myAutoHilight(Standard_True),
myIsAutoActivateSelMode(Standard_True)
{
  mgrSelector = new SelectMgr_SelectionManager (myMainSel);

  myStyles[Prs3d_TypeOfHighlight_None]          = myDefaultDrawer;
  myStyles[Prs3d_TypeOfHighlight_Selected]      = new Prs3d_Drawer();
  myStyles[Prs3d_TypeOfHighlight_Dynamic]       = new Prs3d_Drawer();
  myStyles[Prs3d_TypeOfHighlight_LocalSelected] = new Prs3d_Drawer();
  myStyles[Prs3d_TypeOfHighlight_LocalDynamic]  = new Prs3d_Drawer();
  myStyles[Prs3d_TypeOfHighlight_SubIntensity]  = new Prs3d_Drawer();

  myDefaultDrawer->SetZLayer(Graphic3d_ZLayerId_Default);
  myDefaultDrawer->SetDisplayMode(0);
  {
    const Handle(Prs3d_Drawer)& aStyle = myStyles[Prs3d_TypeOfHighlight_Dynamic];
    aStyle->Link (myDefaultDrawer);
    initDefaultHilightAttributes (aStyle, Quantity_NOC_CYAN1);
    aStyle->SetZLayer(Graphic3d_ZLayerId_Top);
  }
  {
    const Handle(Prs3d_Drawer)& aStyle = myStyles[Prs3d_TypeOfHighlight_LocalDynamic];
    aStyle->Link (myDefaultDrawer);
    initDefaultHilightAttributes (aStyle, Quantity_NOC_CYAN1);
    aStyle->SetZLayer(Graphic3d_ZLayerId_Topmost);
  }
  {
    const Handle(Prs3d_Drawer)& aStyle = myStyles[Prs3d_TypeOfHighlight_Selected];
    aStyle->Link (myDefaultDrawer);
    initDefaultHilightAttributes (aStyle, Quantity_NOC_GRAY80);
    aStyle->SetZLayer(Graphic3d_ZLayerId_UNKNOWN);
  }
  {
    const Handle(Prs3d_Drawer)& aStyle = myStyles[Prs3d_TypeOfHighlight_LocalSelected];
    aStyle->Link (myDefaultDrawer);
    initDefaultHilightAttributes (aStyle, Quantity_NOC_GRAY80);
    aStyle->SetZLayer(Graphic3d_ZLayerId_UNKNOWN);
  }
  {
    const Handle(Prs3d_Drawer)& aStyle = myStyles[Prs3d_TypeOfHighlight_SubIntensity];
    aStyle->SetZLayer(Graphic3d_ZLayerId_UNKNOWN);
    aStyle->SetMethod(Aspect_TOHM_COLOR);
    aStyle->SetColor (Quantity_NOC_GRAY40);
  }

  InitAttributes();
}

//=======================================================================
//function : ~AIS_InteractiveContext
//purpose  :
//=======================================================================
AIS_InteractiveContext::~AIS_InteractiveContext()
{
  // clear the current selection
  mySelection->Clear();
  mgrSelector.Nullify();

  Handle(AIS_InteractiveContext) aNullContext;
  for (AIS_DataMapIteratorOfDataMapOfIOStatus anObjIter (myObjects); anObjIter.More(); anObjIter.Next())
  {
    const Handle(AIS_InteractiveObject)& anObj = anObjIter.Key();
    anObj->SetContext (aNullContext);
    for (SelectMgr_SequenceOfSelection::Iterator aSelIter (anObj->Selections()); aSelIter.More(); aSelIter.Next())
    {
      aSelIter.Value()->UpdateBVHStatus (SelectMgr_TBU_Renew);
    }
  }
}

//=======================================================================
//function : LastActiveView
//purpose  :
//=======================================================================
Handle(V3d_View) AIS_InteractiveContext::LastActiveView() const
{
  if (myLastActiveView == NULL
   || myMainVwr.IsNull())
  {
    return Handle(V3d_View)();
  }

  // as a precaution - check that myLastActiveView pointer is a valid active View
  for (V3d_ListOfViewIterator aViewIter = myMainVwr->ActiveViewIterator(); aViewIter.More(); aViewIter.Next())
  {
    if (aViewIter.Value() == myLastActiveView)
    {
      return aViewIter.Value();
    }
  }
  return Handle(V3d_View)();
}

//=======================================================================
//function : UpdateCurrentViewer
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::UpdateCurrentViewer()
{
  if (!myMainVwr.IsNull())
    myMainVwr->Update();
}

//=======================================================================
//function : DisplayedObjects
//purpose  :
//=======================================================================
void AIS_InteractiveContext::DisplayedObjects (AIS_ListOfInteractive& theListOfIO) const
{
  for (AIS_DataMapIteratorOfDataMapOfIOStatus anObjIter (myObjects); anObjIter.More(); anObjIter.Next())
  {
    if (anObjIter.Value()->GraphicStatus() == AIS_DS_Displayed)
    {
      theListOfIO.Append (anObjIter.Key());
    }
  }
}

//=======================================================================
//function : DisplayedObjects
//purpose  :
//=======================================================================
void AIS_InteractiveContext::DisplayedObjects (const AIS_KindOfInteractive theKind,
                                               const Standard_Integer      theSign,
                                               AIS_ListOfInteractive&      theListOfIO) const
{
  ObjectsByDisplayStatus (theKind, theSign, AIS_DS_Displayed, theListOfIO);
}

//=======================================================================
//function : ErasedObjects
//purpose  :
//=======================================================================
void AIS_InteractiveContext::ErasedObjects (AIS_ListOfInteractive& theListOfIO) const
{
  ObjectsByDisplayStatus (AIS_DS_Erased, theListOfIO);
}

//=======================================================================
//function : ErasedObjects
//purpose  :
//=======================================================================
void AIS_InteractiveContext::ErasedObjects (const AIS_KindOfInteractive theKind,
                                            const Standard_Integer      theSign,
                                            AIS_ListOfInteractive&      theListOfIO) const
{
  ObjectsByDisplayStatus (theKind, theSign, AIS_DS_Erased, theListOfIO);
}

//=======================================================================
//function : ObjectsByDisplayStatus
//purpose  :
//=======================================================================
void AIS_InteractiveContext::ObjectsByDisplayStatus (const AIS_DisplayStatus theStatus,
                                                     AIS_ListOfInteractive&  theListOfIO) const
{
  for (AIS_DataMapIteratorOfDataMapOfIOStatus anObjIter (myObjects); anObjIter.More(); anObjIter.Next())
  {
    if (anObjIter.Value()->GraphicStatus() == theStatus)
    {
      theListOfIO.Append (anObjIter.Key());
    }
  }
}

//=======================================================================
//function : ObjectsByDisplayStatus
//purpose  :
//=======================================================================
void AIS_InteractiveContext::ObjectsByDisplayStatus (const AIS_KindOfInteractive theKind,
                                                     const Standard_Integer      theSign,
                                                     const AIS_DisplayStatus     theStatus,
                                                     AIS_ListOfInteractive&      theListOfIO) const
{
  for (AIS_DataMapIteratorOfDataMapOfIOStatus anObjIter (myObjects); anObjIter.More(); anObjIter.Next())
  {
    if (theStatus != AIS_DS_None
     && anObjIter.Value()->GraphicStatus() != theStatus)
    {
      continue;
    }
    else if (anObjIter.Key()->Type() != theKind)
    {
      continue;
    }

    if (theSign == -1
     || anObjIter.Key()->Signature() == theSign)
    {
      theListOfIO.Append (anObjIter.Key());
    }
  }
}

//=======================================================================
//function : ObjectsInside
//purpose  :
//=======================================================================
void AIS_InteractiveContext::ObjectsInside (AIS_ListOfInteractive&      theListOfIO,
                                            const AIS_KindOfInteractive theKind,
                                            const Standard_Integer      theSign) const
{
  if (theKind == AIS_KOI_None
   && theSign == -1)
  {
    for (AIS_DataMapIteratorOfDataMapOfIOStatus anObjIter (myObjects); anObjIter.More(); anObjIter.Next())
    {
      theListOfIO.Append (anObjIter.Key());
    }
    return;
  }

  for (AIS_DataMapIteratorOfDataMapOfIOStatus anObjIter (myObjects); anObjIter.More(); anObjIter.Next())
  {
    if (anObjIter.Key()->Type() != theKind)
    {
      continue;
    }

    if (theSign == -1
     || anObjIter.Key()->Signature() == theSign)
    {
      theListOfIO.Append (anObjIter.Key());
    }
  }
}

//=======================================================================
//function : ObjectsForView
//purpose  :
//=======================================================================
void AIS_InteractiveContext::ObjectsForView (AIS_ListOfInteractive&  theListOfIO,
                                             const Handle(V3d_View)& theView,
                                             const Standard_Boolean  theIsVisibleInView,
                                             const AIS_DisplayStatus theStatus) const
{
  Handle(Graphic3d_CView) aViewImpl = theView->View();
  const Standard_Integer  aViewId   = aViewImpl->Identification();
  for (AIS_DataMapIteratorOfDataMapOfIOStatus anObjIter (myObjects); anObjIter.More(); anObjIter.Next())
  {
    if (theStatus != AIS_DS_None
     && anObjIter.Value()->GraphicStatus() != theStatus)
    {
      theListOfIO.Append (anObjIter.Key());
      continue;
    }

    Handle(Graphic3d_ViewAffinity) anAffinity = myMainVwr->StructureManager()->ObjectAffinity (anObjIter.Key());
    const Standard_Boolean isVisible = anAffinity->IsVisible (aViewId);
    if (isVisible == theIsVisibleInView)
    {
      theListOfIO.Append (anObjIter.Key());
    }
  }
}

//=======================================================================
//function : Display
//purpose  :
//=======================================================================
void AIS_InteractiveContext::Display (const Handle(AIS_InteractiveObject)& theIObj,
                                      const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  Standard_Integer aDispMode = 0, aHiMod = -1, aSelMode = -1;
  GetDefModes (theIObj, aDispMode, aHiMod, aSelMode);
  Display (theIObj, aDispMode, myIsAutoActivateSelMode ? aSelMode : -1, theToUpdateViewer);
}

//=======================================================================
//function : SetViewAffinity
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetViewAffinity (const Handle(AIS_InteractiveObject)& theIObj,
                                              const Handle(V3d_View)&              theView,
                                              const Standard_Boolean               theIsVisible)
{
  if (theIObj.IsNull()
  || !myObjects.IsBound (theIObj))
  {
    return;
  }

  Handle(Graphic3d_ViewAffinity) anAffinity = myMainVwr->StructureManager()->ObjectAffinity (theIObj);
  Handle(Graphic3d_CView) aViewImpl = theView->View();
  anAffinity->SetVisible (aViewImpl->Identification(), theIsVisible == Standard_True);
  if (theIsVisible)
  {
    theView->View()->ChangeHiddenObjects()->Remove (theIObj.get());
  }
  else
  {
    theView->View()->ChangeHiddenObjects()->Add (theIObj.get());
  }
}

//=======================================================================
//function : Display
//purpose  :
//=======================================================================
void AIS_InteractiveContext::Display (const Handle(AIS_InteractiveObject)& theIObj,
                                      const Standard_Integer               theDispMode,
                                      const Standard_Integer               theSelectionMode,
                                      const Standard_Boolean               theToUpdateViewer,
                                      const AIS_DisplayStatus              theDispStatus)
{
  if (theIObj.IsNull())
  {
    return;
  }

  if (theDispStatus == AIS_DS_Erased)
  {
    Erase  (theIObj, theToUpdateViewer);
    Load (theIObj, theSelectionMode);
    if (Handle(AIS_GlobalStatus)* aStatusPtr = myObjects.ChangeSeek (theIObj))
    {
      (*aStatusPtr)->SetDisplayMode (theDispMode);
    }
    return;
  }

  setContextToObject (theIObj);
  if (!myObjects.IsBound (theIObj))
  {
    setObjectStatus (theIObj, AIS_DS_Displayed, theDispMode, theSelectionMode);
    myMainVwr->StructureManager()->RegisterObject (theIObj);
    myMainPM->Display(theIObj, theDispMode);
    if (theSelectionMode != -1)
    {
      const Handle(SelectMgr_SelectableObject)& anObj = theIObj; // to avoid ambiguity
      if (!mgrSelector->Contains (anObj))
      {
        mgrSelector->Load (theIObj);
      }
      mgrSelector->Activate (theIObj, theSelectionMode);
    }
  }
  else
  {
    Handle(AIS_GlobalStatus) aStatus = myObjects (theIObj);

    // Mark the presentation modes hidden of interactive object different from aDispMode.
    // Then make sure aDispMode is displayed and maybe highlighted.
    // Finally, activate selection mode <SelMode> if not yet activated.
    const Standard_Integer anOldMode = aStatus->DisplayMode();
    if (anOldMode != theDispMode)
    {
      if(myMainPM->IsHighlighted (theIObj, anOldMode))
      {
        unhighlightGlobal (theIObj);
      }
      myMainPM->SetVisibility (theIObj, anOldMode, Standard_False);
    }

    aStatus->SetDisplayMode (theDispMode);

    myMainPM->Display (theIObj, theDispMode);
    aStatus->SetGraphicStatus (AIS_DS_Displayed);
    if (aStatus->IsHilighted())
    {
      highlightGlobal (theIObj, aStatus->HilightStyle(), theDispMode);
    }
    if (theSelectionMode != -1)
    {
      const Handle(SelectMgr_SelectableObject)& anObj = theIObj; // to avoid ambiguity
      if (!mgrSelector->Contains (anObj))
      {
        mgrSelector->Load (theIObj);
      }
      if (!mgrSelector->IsActivated (theIObj, theSelectionMode))
      {
        if (!aStatus->IsSModeIn (theSelectionMode))
          aStatus->AddSelectionMode (theSelectionMode);
        mgrSelector->Activate (theIObj, theSelectionMode);
      }
    }
  }

  if (theToUpdateViewer)
  {
    myMainVwr->Update();
  }
}

//=======================================================================
//function : Load
//purpose  :
//=======================================================================
void AIS_InteractiveContext::Load (const Handle(AIS_InteractiveObject)& theIObj,
                                   const Standard_Integer               theSelMode)
{
  if (theIObj.IsNull())
  {
    return;
  }

  setContextToObject (theIObj);
  if (!myObjects.IsBound (theIObj))
  {
    Standard_Integer aDispMode, aHiMod, aSelModeDef;
    GetDefModes (theIObj, aDispMode, aHiMod, aSelModeDef);
    setObjectStatus (theIObj, AIS_DS_Erased, aDispMode, theSelMode != -1 ? theSelMode : aSelModeDef);
    myMainVwr->StructureManager()->RegisterObject (theIObj);
  }

  // Register theIObj in the selection manager to prepare further activation of selection
  const Handle(SelectMgr_SelectableObject)& anObj = theIObj; // to avoid ambiguity
  if (!mgrSelector->Contains (anObj))
  {
    mgrSelector->Load (theIObj);
  }
}

//=======================================================================
//function : Erase
//purpose  :
//=======================================================================
void AIS_InteractiveContext::Erase (const Handle(AIS_InteractiveObject)& theIObj,
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

  EraseGlobal (theIObj, Standard_False);
  if (theToUpdateViewer)
  {
    myMainVwr->Update();
  }
}

//=======================================================================
//function : EraseAll
//purpose  :
//=======================================================================
void AIS_InteractiveContext::EraseAll (const Standard_Boolean theToUpdateViewer)
{
  for (AIS_DataMapIteratorOfDataMapOfIOStatus anObjIter (myObjects); anObjIter.More(); anObjIter.Next())
  {
    if (anObjIter.Value()->GraphicStatus() == AIS_DS_Displayed)
    {
      Erase (anObjIter.Key(), Standard_False);
    }
  }

  if (theToUpdateViewer)
  {
    myMainVwr->Update();
  }
}

//=======================================================================
//function : DisplayAll
//purpose  :
//=======================================================================
void AIS_InteractiveContext::DisplayAll (const Standard_Boolean theToUpdateViewer)
{
  for (AIS_DataMapIteratorOfDataMapOfIOStatus anObjIter (myObjects); anObjIter.More(); anObjIter.Next())
  {
    const AIS_DisplayStatus aStatus = anObjIter.Value()->GraphicStatus();
    if (aStatus == AIS_DS_Erased)
    {
      Display (anObjIter.Key(), Standard_False);
    }
  }

  if (theToUpdateViewer)
  {
    myMainVwr->Update();
  }
}

//=======================================================================
//function : DisplaySelected
//purpose  :
//=======================================================================
void AIS_InteractiveContext::DisplaySelected (const Standard_Boolean theToUpdateViewer)
{
  for (AIS_NListOfEntityOwner::Iterator aSelIter (mySelection->Objects()); aSelIter.More(); aSelIter.Next())
  {
    Handle(AIS_InteractiveObject) anObj = Handle(AIS_InteractiveObject)::DownCast (aSelIter.Value()->Selectable());
    Display (anObj, Standard_False);
  }

  if (theToUpdateViewer && !mySelection->Objects().IsEmpty())
  {
    myMainVwr->Update();
  }
}

//=======================================================================
//function : EraseSelected
//purpose  :
//=======================================================================
void AIS_InteractiveContext::EraseSelected (const Standard_Boolean theToUpdateViewer)
{
  Standard_Boolean isFound = Standard_False;
  for (AIS_NListOfEntityOwner::Iterator aSelIter (mySelection->Objects()); aSelIter.More(); aSelIter.Init (mySelection->Objects()))
  {
    Handle(AIS_InteractiveObject) anObj = Handle(AIS_InteractiveObject)::DownCast (aSelIter.Value()->Selectable());
    Erase (anObj, Standard_False);
    isFound = Standard_True;
  }

  if (isFound && theToUpdateViewer)
  {
    myMainVwr->Update();
  }
}

//=======================================================================
//function : DisplayStatus
//purpose  :
//=======================================================================
AIS_DisplayStatus AIS_InteractiveContext::DisplayStatus (const Handle(AIS_InteractiveObject)& theIObj) const
{
  if (theIObj.IsNull())
  {
    return AIS_DS_None;
  }
  const Handle(AIS_GlobalStatus)* aStatus = myObjects.Seek (theIObj);
  return aStatus != NULL ? (*aStatus)->GraphicStatus() : AIS_DS_None;
}

//=======================================================================
//function : Remove
//purpose  :
//=======================================================================
void AIS_InteractiveContext::Remove (const Handle(AIS_InteractiveObject)& theIObj,
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
      throw Standard_ProgramError("AIS_InteractiveContext - object has been displayed in another context!");
    }
    theIObj->SetContext (Handle(AIS_InteractiveContext)());
  }
  ClearGlobal (theIObj, theToUpdateViewer);
}

//=======================================================================
//function : RemoveAll
//purpose  :
//=======================================================================
void AIS_InteractiveContext::RemoveAll (const Standard_Boolean theToUpdateViewer)
{
  AIS_ListOfInteractive aList;
  ObjectsInside (aList);
  for (AIS_ListIteratorOfListOfInteractive aListIterator (aList); aListIterator.More(); aListIterator.Next())
  {
    Remove (aListIterator.Value(), Standard_False);
  }

  if (theToUpdateViewer)
  {
    myMainVwr->Update();
  }
}

//=======================================================================
//function : HilightWithColor
//purpose  : 
//=======================================================================
void AIS_InteractiveContext::HilightWithColor(const Handle(AIS_InteractiveObject)& theObj,
                                              const Handle(Prs3d_Drawer)& theStyle,
                                              const Standard_Boolean theIsToUpdate)
{
  if (theObj.IsNull())
    return;

  setContextToObject (theObj);
  if (!myObjects.IsBound (theObj))
    return;

  const Handle(AIS_GlobalStatus)& aStatus = myObjects (theObj);
  aStatus->SetHilightStatus (Standard_True);

  if (aStatus->GraphicStatus() == AIS_DS_Displayed)
  {
    highlightGlobal (theObj, theStyle, aStatus->DisplayMode());
    aStatus->SetHilightStyle (theStyle);
  }

  if (theIsToUpdate)
    myMainVwr->Update();
}

//=======================================================================
//function : Unhilight
//purpose  : 
//=======================================================================
void AIS_InteractiveContext::Unhilight(const Handle(AIS_InteractiveObject)& anIObj, const Standard_Boolean updateviewer)
{
  if(anIObj.IsNull()) return;
  if(!myObjects.IsBound(anIObj)) return;

  const Handle(AIS_GlobalStatus)& aStatus = myObjects(anIObj);
  aStatus->SetHilightStatus (Standard_False);
  aStatus->SetHilightStyle (Handle(Prs3d_Drawer)());

  if (aStatus->GraphicStatus() == AIS_DS_Displayed)
  {
    unhighlightGlobal (anIObj);
  }

  if(updateviewer) myMainVwr->Update();
}

//=======================================================================
//function : IsHilighted
//purpose  : Returns true if the objects global status is set to highlighted.
//=======================================================================
Standard_Boolean AIS_InteractiveContext::IsHilighted (const Handle(AIS_InteractiveObject)& theObj) const
{
  const Handle(AIS_GlobalStatus)* aStatus = myObjects.Seek (theObj);
  return aStatus != NULL
      && (*aStatus)->IsHilighted();
}

//=======================================================================
//function : IsHilighted
//purpose  : Returns true if the owner is highlighted with selection style.
//=======================================================================
Standard_Boolean AIS_InteractiveContext::IsHilighted (const Handle(SelectMgr_EntityOwner)& theOwner) const
{
  if (theOwner.IsNull() || !theOwner->HasSelectable())
    return Standard_False;

  const Handle(AIS_InteractiveObject) anObj =
    Handle(AIS_InteractiveObject)::DownCast (theOwner->Selectable());

  if (anObj->GlobalSelOwner() == theOwner)
  {
    if (!myObjects.IsBound (anObj))
      return Standard_False;

    return myObjects (anObj)->IsHilighted();
  }

  const Handle(Prs3d_Drawer)& aStyle = getSelStyle (anObj, theOwner);
  const Standard_Integer aHiMode = getHilightMode (anObj, aStyle, -1);
  return theOwner->IsHilighted (myMainPM, aHiMode);
}

//=======================================================================
//function : HighlightStyle
//purpose  :
//=======================================================================
Standard_Boolean AIS_InteractiveContext::HighlightStyle (const Handle(AIS_InteractiveObject)& theObj,
                                                         Handle(Prs3d_Drawer)& theStyle) const
{
  const Handle(AIS_GlobalStatus)* aStatus = myObjects.Seek (theObj);
  if (aStatus != NULL
   && (*aStatus)->IsHilighted())
  {
    theStyle = (*aStatus)->HilightStyle();
    return Standard_True;
  }

  theStyle.Nullify();
  return Standard_False;
}

//=======================================================================
//function : HighlightStyle
//purpose  :
//=======================================================================
Standard_Boolean AIS_InteractiveContext::HighlightStyle (const Handle(SelectMgr_EntityOwner)& theOwner,
                                                         Handle(Prs3d_Drawer)& theStyle) const
{
  if (theOwner.IsNull() || !theOwner->HasSelectable())
    return Standard_False;

  if (IsHilighted (theOwner))
  {
    const Handle(AIS_InteractiveObject) anObj =
      Handle(AIS_InteractiveObject)::DownCast (theOwner->Selectable());
    if (anObj->GlobalSelOwner() == theOwner)
    {
      theStyle = myObjects (anObj)->HilightStyle();
    }
    else
    {
      // since part selection style is not stored in global status,
      // check if the object has own selection style. If not, it can
      // only be highlighted with default selection style (because
      // sub-intensity does not modify any selection states)
      theStyle = getSelStyle (anObj, theOwner);
    }
    return Standard_True;
  }
  else
  {
    theStyle.Nullify();
    return Standard_False;
  }
}

//=======================================================================
//function : IsDisplayed
//purpose  : 
//=======================================================================

Standard_Boolean AIS_InteractiveContext::IsDisplayed(const Handle(AIS_InteractiveObject)& theObj) const 
{
  if(theObj.IsNull()) return Standard_False;

  const Handle(AIS_GlobalStatus)* aStatus = myObjects.Seek (theObj);
  return aStatus != NULL
    && (*aStatus)->GraphicStatus() == AIS_DS_Displayed; 
}

//=======================================================================
//function : IsDisplayed
//purpose  :
//=======================================================================
Standard_Boolean AIS_InteractiveContext::IsDisplayed (const Handle(AIS_InteractiveObject)& theIObj,
                                                      const Standard_Integer               theMode) const
{
  if (theIObj.IsNull())
  {
    return Standard_False;
  }

  const Handle(AIS_GlobalStatus)* aStatus = myObjects.Seek (theIObj);
  return aStatus != NULL
      && (*aStatus)->GraphicStatus() == AIS_DS_Displayed
      && (*aStatus)->DisplayMode() == theMode;
}

//=======================================================================
//function : DisplayPriority
//purpose  :
//=======================================================================
Standard_Integer AIS_InteractiveContext::DisplayPriority (const Handle(AIS_InteractiveObject)& theIObj) const
{
  if (theIObj.IsNull())
  {
    return -1;
  }

  const Handle(AIS_GlobalStatus)* aStatus = myObjects.Seek (theIObj);
  if (aStatus != NULL
   && ((*aStatus)->GraphicStatus() == AIS_DS_Displayed
    || (*aStatus)->GraphicStatus() == AIS_DS_Erased))
  {
    Standard_Integer aDispMode = theIObj->HasDisplayMode()
                               ? theIObj->DisplayMode()
                               : (theIObj->AcceptDisplayMode (myDefaultDrawer->DisplayMode())
                                ? myDefaultDrawer->DisplayMode()
                                : 0);
    return myMainPM->DisplayPriority (theIObj, aDispMode);
  }
  return 0;
}

//=======================================================================
//function : SetDisplayPriority
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetDisplayPriority (const Handle(AIS_InteractiveObject)& theIObj,
                                                 const Standard_Integer               thePriority)
{
  if (theIObj.IsNull())
  {
    return;
  }

  setContextToObject (theIObj);
  const Handle(AIS_GlobalStatus)* aStatus = myObjects.Seek (theIObj);
  if (aStatus != NULL
   && ((*aStatus)->GraphicStatus() == AIS_DS_Displayed
    || (*aStatus)->GraphicStatus() == AIS_DS_Erased))
  {
    Standard_Integer aDisplayMode = theIObj->HasDisplayMode()
                                  ? theIObj->DisplayMode()
                                  : (theIObj->AcceptDisplayMode (myDefaultDrawer->DisplayMode())
                                    ? myDefaultDrawer->DisplayMode()
                                    : 0);
    myMainPM->SetDisplayPriority (theIObj, aDisplayMode, thePriority);
  }
}

//=======================================================================
//function : Redisplay
//purpose  :
//=======================================================================
void AIS_InteractiveContext::Redisplay (const Handle(AIS_InteractiveObject)& theIObj,
                                        const Standard_Boolean               theToUpdateViewer,
                                        const Standard_Boolean               theAllModes)
{
  RecomputePrsOnly (theIObj, theToUpdateViewer, theAllModes);
  RecomputeSelectionOnly (theIObj);
}

//=======================================================================
//function : Redisplay
//purpose  :
//=======================================================================
void AIS_InteractiveContext::Redisplay (const AIS_KindOfInteractive theKOI,
                                        const Standard_Integer    /*theSign*/,
                                        const Standard_Boolean      theToUpdateViewer)
{
  Standard_Boolean isRedisplayed = Standard_False;
  for (AIS_DataMapIteratorOfDataMapOfIOStatus anObjIter (myObjects); anObjIter.More(); anObjIter.Next())
  {
    Handle(AIS_InteractiveObject) anObj = anObjIter.Key();
    if (anObj->Type() != theKOI)
    {
      continue;
    }

    Redisplay (anObj, Standard_False);
    isRedisplayed = anObjIter.Value()->GraphicStatus() == AIS_DS_Displayed
                 || isRedisplayed;
  }

  if (theToUpdateViewer
   && isRedisplayed)
  {
    myMainVwr->Update();
  }
}

//=======================================================================
//function : RecomputePrsOnly
//purpose  :
//=======================================================================
void AIS_InteractiveContext::RecomputePrsOnly (const Handle(AIS_InteractiveObject)& theIObj,
                                               const Standard_Boolean               theToUpdateViewer,
                                               const Standard_Boolean               theAllModes)
{
  if (theIObj.IsNull())
  {
    return;
  }

  theIObj->SetToUpdate();
  theIObj->UpdatePresentations (theAllModes);
  if (!theToUpdateViewer)
  {
    return;
  }

  const Handle(AIS_GlobalStatus)* aStatus = myObjects.Seek (theIObj);
  if (aStatus != NULL
   && (*aStatus)->GraphicStatus() == AIS_DS_Displayed)
  {
    myMainVwr->Update();
  }
}
//=======================================================================
//function : RecomputeSelectionOnly
//purpose  : 
//=======================================================================
void AIS_InteractiveContext::RecomputeSelectionOnly (const Handle(AIS_InteractiveObject)& theIO)
{
  if (theIO.IsNull())
  {
    return;
  }

  mgrSelector->RecomputeSelection (theIO);

  const Handle(AIS_GlobalStatus)* aStatus = myObjects.Seek (theIO);
  if (aStatus == NULL
  || (*aStatus)->GraphicStatus() != AIS_DS_Displayed)
  {
    return;
  }

  TColStd_ListOfInteger aModes;
  ActivatedModes (theIO, aModes);
  TColStd_ListIteratorOfListOfInteger aModesIter (aModes);
  for (; aModesIter.More(); aModesIter.Next())
  {
    mgrSelector->Activate (theIO, aModesIter.Value());
  }
}

//=======================================================================
//function : Update
//purpose  :
//=======================================================================
void AIS_InteractiveContext::Update (const Handle(AIS_InteractiveObject)& theIObj,
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
    const Handle(AIS_GlobalStatus)* aStatus = myObjects.Seek (theIObj);
    if (aStatus != NULL
     && (*aStatus)->GraphicStatus() == AIS_DS_Displayed)
    {
      myMainVwr->Update();
    }
  }
}

//=======================================================================
//function : SetLocation
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetLocation (const Handle(AIS_InteractiveObject)& theIObj,
                                          const TopLoc_Location&               theLoc)
{
  if (theIObj.IsNull())
  {
    return;
  }

  if (theIObj->HasTransformation()
   && theLoc.IsIdentity())
  {
    theIObj->ResetTransformation();
    mgrSelector->Update (theIObj, Standard_False);
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

  theIObj->SetLocalTransformation (theLoc.Transformation());

  mgrSelector->Update (theIObj, Standard_False);

  // if the object or its part is highlighted dynamically, it is necessary to apply location transformation
  // to its highlight structure immediately
  if (!myLastPicked.IsNull() && myLastPicked->IsSameSelectable (theIObj))
  {
    const Standard_Integer aHiMod = theIObj->HasHilightMode() ? theIObj->HilightMode() : 0;
    myLastPicked->UpdateHighlightTrsf (myMainVwr,
                                       myMainPM,
                                       aHiMod);
  }
}

//=======================================================================
//function : ResetLocation
//purpose  :
//=======================================================================
void AIS_InteractiveContext::ResetLocation (const Handle(AIS_InteractiveObject)& theIObj)
{
  if (theIObj.IsNull())
  {
    return;
  }

  theIObj->ResetTransformation();
  mgrSelector->Update (theIObj, Standard_False);
}

//=======================================================================
//function : HasLocation
//purpose  :
//=======================================================================
Standard_Boolean AIS_InteractiveContext::HasLocation (const Handle(AIS_InteractiveObject)& theIObj) const
{
  return !theIObj.IsNull()
       && theIObj->HasTransformation();
}

//=======================================================================
//function : Location
//purpose  :
//=======================================================================
TopLoc_Location AIS_InteractiveContext::Location (const Handle(AIS_InteractiveObject)& theIObj) const
{
  return theIObj->Transformation();
}

//=======================================================================
//function : SetDeviationCoefficient
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetDeviationCoefficient (const Standard_Real theCoefficient)
{
  myDefaultDrawer->SetDeviationCoefficient (theCoefficient);
}

//=======================================================================
//function : SetDeviationAngle
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetDeviationAngle (const Standard_Real theAngle)
{
  myDefaultDrawer->SetDeviationAngle (theAngle);
}

//=======================================================================
//function : DeviationAngle
//purpose  : Gets  deviationAngle
//=======================================================================
Standard_Real AIS_InteractiveContext::DeviationAngle() const
{
  return myDefaultDrawer->DeviationAngle();
}

//=======================================================================
//function : DeviationCoefficient
//purpose  :
//=======================================================================
Standard_Real AIS_InteractiveContext::DeviationCoefficient() const
{
  return myDefaultDrawer->DeviationCoefficient();
}

//=======================================================================
//function : SetDisplayMode
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetDisplayMode(const Standard_Integer theMode,
                                            const Standard_Boolean theToUpdateViewer)
{
  if (theMode == myDefaultDrawer->DisplayMode())
  {
    return;
  }

  for (AIS_DataMapIteratorOfDataMapOfIOStatus anObjIter (myObjects); anObjIter.More(); anObjIter.Next())
  {
    Handle(AIS_InteractiveObject) anObj = anObjIter.Key();
    Standard_Boolean toProcess = anObj->IsKind (STANDARD_TYPE(AIS_Shape))
                              || anObj->IsKind (STANDARD_TYPE(AIS_ConnectedInteractive))
                              || anObj->IsKind (STANDARD_TYPE(AIS_MultipleConnectedInteractive));
    
    if (!toProcess
     ||  anObj->HasDisplayMode()
     || !anObj->AcceptDisplayMode (theMode))
    {
      continue;
    }

    Handle(AIS_GlobalStatus) aStatus = anObjIter.Value();
    aStatus->SetDisplayMode (theMode);

    if (aStatus->GraphicStatus() == AIS_DS_Displayed)
    {
      myMainPM->Display (anObj, theMode);
      if (!myLastPicked.IsNull() && myLastPicked->IsSameSelectable (anObj))
      {
        myMainPM->BeginImmediateDraw();
        unhighlightGlobal (anObj);
        myMainPM->EndImmediateDraw (myMainVwr);
      }
      if (aStatus->IsSubIntensityOn())
      {
        highlightWithSubintensity (anObj, theMode);
      }
      myMainPM->SetVisibility (anObj, myDefaultDrawer->DisplayMode(), Standard_False);
    }
  }

  myDefaultDrawer->SetDisplayMode (theMode);
  if (theToUpdateViewer)
  {
    myMainVwr->Update();
  }
}

//=======================================================================
//function : SetDisplayMode
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetDisplayMode (const Handle(AIS_InteractiveObject)& theIObj,
                                             const Standard_Integer               theMode,
                                             const Standard_Boolean               theToUpdateViewer)
{
  setContextToObject (theIObj);
  if (!myObjects.IsBound (theIObj))
  {
    theIObj->SetDisplayMode (theMode);
    return;
  }
  else if (!theIObj->AcceptDisplayMode (theMode))
  {
    return;
  }

  Handle(AIS_GlobalStatus) aStatus = myObjects (theIObj);
  if (aStatus->GraphicStatus() != AIS_DS_Displayed)
  {
    aStatus->SetDisplayMode (theMode);
    theIObj->SetDisplayMode (theMode);
    return;
  }

  // erase presentations for all display modes different from <aMode>
  const Standard_Integer anOldMode = aStatus->DisplayMode();
  if (anOldMode != theMode)
  {
    if (myMainPM->IsHighlighted (theIObj, anOldMode))
    {
      unhighlightGlobal (theIObj);
    }
    myMainPM->SetVisibility (theIObj, anOldMode, Standard_False);
  }

  aStatus->SetDisplayMode (theMode);

  myMainPM->Display (theIObj, theMode);
  if (aStatus->IsHilighted())
  {
    highlightGlobal (theIObj, getSelStyle (theIObj, theIObj->GlobalSelOwner()), theMode);
  }
  if (aStatus->IsSubIntensityOn())
  {
    highlightWithSubintensity (theIObj, theMode);
  }

  if (theToUpdateViewer)
  {
    myMainVwr->Update();
  }
  theIObj->SetDisplayMode (theMode);
}

//=======================================================================
//function : UnsetDisplayMode
//purpose  :
//=======================================================================
void AIS_InteractiveContext::UnsetDisplayMode (const Handle(AIS_InteractiveObject)& theIObj,
                                               const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull()
  || !theIObj->HasDisplayMode())
  {
    return;
  }

  if (!myObjects.IsBound (theIObj))
  {
    theIObj->UnsetDisplayMode();
    return;
  }

  const Standard_Integer anOldMode = theIObj->DisplayMode();
  if (myDefaultDrawer->DisplayMode() == anOldMode)
  {
    return;
  }

  const Handle(AIS_GlobalStatus)& aStatus = myObjects (theIObj);
  aStatus->SetDisplayMode (myDefaultDrawer->DisplayMode());

  if (aStatus->GraphicStatus() == AIS_DS_Displayed)
  {
    if (myMainPM->IsHighlighted (theIObj, anOldMode))
    {
      unhighlightGlobal (theIObj);
    }
    myMainPM->SetVisibility (theIObj, anOldMode, Standard_False);
    myMainPM->Display (theIObj, myDefaultDrawer->DisplayMode());
    if (aStatus->IsHilighted())
    {
      highlightSelected (theIObj->GlobalSelOwner());
    }
    if (aStatus->IsSubIntensityOn())
    {
      highlightWithSubintensity (theIObj, myDefaultDrawer->DisplayMode());
    }

    if (theToUpdateViewer)
    {
      myMainVwr->Update();
    }
  }

  theIObj->UnsetDisplayMode();
}

//=======================================================================
//function : SetCurrentFacingModel
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetCurrentFacingModel (const Handle(AIS_InteractiveObject)& theIObj,
                                                    const Aspect_TypeOfFacingModel       theModel)
{
  if (!theIObj.IsNull())
  {
    theIObj->SetCurrentFacingModel (theModel);
  }
}

//=======================================================================
//function : SetColor
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetColor (const Handle(AIS_InteractiveObject)& theIObj,
                                       const Quantity_Color&                theColor,
                                       const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  setContextToObject (theIObj);
  theIObj->SetColor (theColor);
  theIObj->UpdatePresentations();
  if (theToUpdateViewer)
  {
    UpdateCurrentViewer();
  }
}

//=======================================================================
//function : SetIsoOnTriangulation
//purpose  :
//=======================================================================
void AIS_InteractiveContext::IsoOnTriangulation (const Standard_Boolean theIsEnabled,
                                                 const Handle(AIS_InteractiveObject)& theObject)
{
  if (theObject.IsNull())
  {
    return;
  }

  theObject->SetIsoOnTriangulation (theIsEnabled);
}

//=======================================================================
//function : SetDeviationCoefficient
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetDeviationCoefficient (const Handle(AIS_InteractiveObject)& theIObj,
                                                      const Standard_Real                  theCoefficient,
                                                      const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  // to be modified after the related methods of AIS_Shape are passed to InteractiveObject
  setContextToObject (theIObj);
  if (theIObj->Type() != AIS_KOI_Object
   && theIObj->Type() != AIS_KOI_Shape)
  {
    return;
  }
  else if (theIObj->Signature() != 0)
  {
    return;
  }

  Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast (theIObj);
  aShape->SetOwnDeviationCoefficient (theCoefficient);
  aShape->UpdatePresentations();
  if (theToUpdateViewer)
  {
    UpdateCurrentViewer();
  }
}

//=======================================================================
//function : SetDeviationAngle
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetDeviationAngle (const Handle(AIS_InteractiveObject)& theIObj,
                                                const Standard_Real                  theAngle,
                                                const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  // To be modified after the related methods of AIS_Shape are passed to InteractiveObject
  setContextToObject (theIObj);
  if (theIObj->Type() != AIS_KOI_Shape)
  {
    return;
  }
  else if (theIObj->Signature() != 0)
  {
    return;
  }

  Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast (theIObj);
  aShape->SetOwnDeviationAngle (theAngle);
  aShape->UpdatePresentations();
  if (theToUpdateViewer)
  {
    UpdateCurrentViewer();
  }
}

//=======================================================================
//function : SetAngleAndDeviation
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetAngleAndDeviation (const Handle(AIS_InteractiveObject)& theIObj,
                                                   const Standard_Real                  theAngle,
                                                   const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  // To be modified after the related methods of AIS_Shape are passed to InteractiveObject
  setContextToObject (theIObj);
  if (theIObj->Type() != AIS_KOI_Shape)
  {
    return;
  }
  if (theIObj->Signature() != 0)
  {
    return;
  }

  Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast (theIObj);
  aShape->SetAngleAndDeviation (theAngle);
  aShape->UpdatePresentations();
  if (theToUpdateViewer)
  {
    UpdateCurrentViewer();
  }
}

//=======================================================================
//function : UnsetColor
//purpose  :
//=======================================================================
void AIS_InteractiveContext::UnsetColor (const Handle(AIS_InteractiveObject)& theIObj,
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

//=======================================================================
//function : HasColor
//purpose  :
//=======================================================================
Standard_Boolean AIS_InteractiveContext::HasColor (const Handle(AIS_InteractiveObject)& theIObj) const
{
  return theIObj->HasColor();
}

//=======================================================================
//function : Color
//purpose  :
//=======================================================================
void AIS_InteractiveContext::Color (const Handle(AIS_InteractiveObject)& theIObj,
                                    Quantity_Color&                      theColor) const
{
  theIObj->Color (theColor);
}

//=======================================================================
//function : Width
//purpose  :
//=======================================================================
Standard_Real AIS_InteractiveContext::Width (const Handle(AIS_InteractiveObject)& theIObj) const
{
  return theIObj->Width();
}

//=======================================================================
//function : SetWidth
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetWidth (const Handle(AIS_InteractiveObject)& theIObj,
                                       const Standard_Real                  theWidth,
                                       const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  setContextToObject (theIObj);
  theIObj->SetWidth (theWidth);
  theIObj->UpdatePresentations();
  if (!myLastPicked.IsNull() && myLastPicked->IsSameSelectable (theIObj))
  {
    if (myLastPicked->IsAutoHilight())
    {
      const Standard_Integer aHiMode = theIObj->HasHilightMode() ? theIObj->HilightMode() : 0;
      myLastPicked->HilightWithColor (myMainPM,
                                      myLastPicked->IsSelected() ? getSelStyle (theIObj, myLastPicked) : getHiStyle (theIObj, myLastPicked),
                                      aHiMode);
    }
    else
    {
      theIObj->HilightOwnerWithColor (myMainPM,
                                      myLastPicked->IsSelected() ? getSelStyle (theIObj, myLastPicked) : getHiStyle (theIObj, myLastPicked),
                                      myLastPicked);
    }
  }
  if (theToUpdateViewer)
  {
    UpdateCurrentViewer();
  }
}

//=======================================================================
//function : UnsetWidth
//purpose  :
//=======================================================================
void AIS_InteractiveContext::UnsetWidth (const Handle(AIS_InteractiveObject)& theIObj,
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

//=======================================================================
//function : SetMaterial
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetMaterial (const Handle(AIS_InteractiveObject)& theIObj,
                                          const Graphic3d_MaterialAspect&      theMaterial,
                                          const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  setContextToObject (theIObj);
  theIObj->SetMaterial (theMaterial);
  theIObj->UpdatePresentations();
  if (theToUpdateViewer)
  {
    UpdateCurrentViewer();
  }
}

//=======================================================================
//function : UnsetMaterial
//purpose  :
//=======================================================================
void AIS_InteractiveContext::UnsetMaterial (const Handle(AIS_InteractiveObject)& theIObj,
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

//=======================================================================
//function : SetTransparency
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetTransparency (const Handle(AIS_InteractiveObject)& theIObj,
                                              const Standard_Real                  theValue,
                                              const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  setContextToObject (theIObj);
  if (!theIObj->IsTransparent()
    && theValue <= 0.005)
  {
    return;
  }

  if (theValue <= 0.005)
  {
    UnsetTransparency (theIObj, theToUpdateViewer);
    return;
  }

  theIObj->SetTransparency (theValue);
  theIObj->UpdatePresentations();
  if (theToUpdateViewer)
  {
    UpdateCurrentViewer();
  }
}

//=======================================================================
//function : UnsetTransparency
//purpose  :
//=======================================================================
void AIS_InteractiveContext::UnsetTransparency (const Handle(AIS_InteractiveObject)& theIObj,
                                                const Standard_Boolean               theToUpdateViewer)
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

//=======================================================================
//function : SetSelectedAspect
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetSelectedAspect (const Handle(Prs3d_BasicAspect)& theAspect,
                                                const Standard_Boolean           theToUpdateViewer)
{
  Standard_DISABLE_DEPRECATION_WARNINGS
  Standard_Boolean isFound = Standard_False;
  for (AIS_NListOfEntityOwner::Iterator aSelIter (mySelection->Objects()); aSelIter.More(); aSelIter.Next())
  {
    isFound = Standard_True;
    Handle(AIS_InteractiveObject) anObj = Handle(AIS_InteractiveObject)::DownCast (aSelIter.Value()->Selectable());
    anObj->SetAspect (theAspect);
  }
  Standard_ENABLE_DEPRECATION_WARNINGS

  if (isFound && theToUpdateViewer)
  {
    myMainVwr->Update();
  }
}

//=======================================================================
//function : SetLocalAttributes
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetLocalAttributes (const Handle(AIS_InteractiveObject)& theIObj,
                                                 const Handle(Prs3d_Drawer)&          theDrawer,
                                                 const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  setContextToObject (theIObj);
  theIObj->SetAttributes (theDrawer);
  Update (theIObj, theToUpdateViewer);
}

//=======================================================================
//function : UnsetLocalAttributes
//purpose  :
//=======================================================================
void AIS_InteractiveContext::UnsetLocalAttributes (const Handle(AIS_InteractiveObject)& theIObj,
                                                   const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  setContextToObject (theIObj);
  theIObj->UnsetAttributes();
  Update (theIObj, theToUpdateViewer);
}

//=======================================================================
//function : Status
//purpose  :
//=======================================================================
void AIS_InteractiveContext::Status (const Handle(AIS_InteractiveObject)& theIObj,
                                     TCollection_ExtendedString&          theStatus) const
{
  theStatus = "";
  if (theIObj.IsNull()
  || !myObjects.IsBound (theIObj))
  {
    return;
  }

  theStatus += "\t ____________________________________________";
  theStatus += "\t| Known at Neutral Point:\n\tDisplayStatus:";
  const Handle(AIS_GlobalStatus)& aStatus = myObjects (theIObj);
  switch (aStatus->GraphicStatus())
  {
    case AIS_DS_Displayed:
    {
      theStatus += "\t| -->Displayed\n";
      break;
    }
    case AIS_DS_Erased:
    {
      theStatus += "\t| -->Erased\n";
      break;
    }
    default:
      break;
  }

  theStatus += "\t| Active Display Modes in the MainViewer :\n";
  theStatus += "\t|\t Mode ";
  theStatus += TCollection_AsciiString (aStatus->DisplayMode());
  theStatus += "\n";

  if (IsSelected(theIObj)) theStatus +="\t| Selected\n";

  theStatus += "\t| Active Selection Modes in the MainViewer :\n";
  for (TColStd_ListIteratorOfListOfInteger aSelModeIter (aStatus->SelectionModes()); aSelModeIter.More(); aSelModeIter.Next())
  {
    theStatus += "\t\t Mode ";
    theStatus += TCollection_AsciiString (aSelModeIter.Value());
    theStatus += "\n";
  }
  theStatus += "\t ____________________________________________";
}

//=======================================================================
//function : GetDefModes
//purpose  :
//=======================================================================
void AIS_InteractiveContext::GetDefModes (const Handle(AIS_InteractiveObject)& theIObj,
                                          Standard_Integer&                    theDispMode,
                                          Standard_Integer&                    theHiMode,
                                          Standard_Integer&                    theSelMode) const
{
  if (theIObj.IsNull())
  {
    return;
  }

  theDispMode = theIObj->HasDisplayMode()
              ? theIObj->DisplayMode()
              : (theIObj->AcceptDisplayMode (myDefaultDrawer->DisplayMode())
               ? myDefaultDrawer->DisplayMode()
               : 0);
  theHiMode  = theIObj->HasHilightMode()   ? theIObj->HilightMode()   : theDispMode;
  theSelMode = theIObj->GlobalSelectionMode();
}

//=======================================================================
//function : EraseGlobal
//purpose  :
//=======================================================================
void AIS_InteractiveContext::EraseGlobal (const Handle(AIS_InteractiveObject)& theIObj,
                                          const Standard_Boolean               theToUpdateviewer)
{
  Handle(AIS_GlobalStatus) aStatus;
  if (theIObj.IsNull()
  || !myObjects.Find (theIObj, aStatus)
  ||  aStatus->GraphicStatus() == AIS_DS_Erased)
  {
    return;
  }

  const Standard_Integer aDispMode = theIObj->HasHilightMode() ? theIObj->HilightMode() : 0;
  unselectOwners (theIObj);
  myMainPM->SetVisibility (theIObj, aStatus->DisplayMode(), Standard_False);

  if (!myLastPicked.IsNull()
    && myLastPicked->IsSameSelectable (theIObj))
  {
    clearDynamicHighlight();
  }

  // make sure highlighting presentations are properly erased
  theIObj->ErasePresentations (false);

  if (IsSelected (theIObj)
   && aStatus->DisplayMode() != aDispMode)
  {
    myMainPM->SetVisibility (theIObj, aDispMode, Standard_False);
  }

  for (TColStd_ListIteratorOfListOfInteger aSelModeIter (aStatus->SelectionModes()); aSelModeIter.More(); aSelModeIter.Next())
  {
    mgrSelector->Deactivate (theIObj, aSelModeIter.Value());
  }
  aStatus->ClearSelectionModes();
  aStatus->SetGraphicStatus (AIS_DS_Erased);

  if (theToUpdateviewer)
  {
    myMainVwr->Update();
  }
}

//=======================================================================
//function : unselectOwners
//purpose  :
//=======================================================================
void AIS_InteractiveContext::unselectOwners (const Handle(AIS_InteractiveObject)& theObject)
{
  SelectMgr_SequenceOfOwner aSeq;
  for (AIS_NListOfEntityOwner::Iterator aSelIter (mySelection->Objects()); aSelIter.More(); aSelIter.Next())
  {
    if (aSelIter.Value()->IsSameSelectable (theObject))
    {
      aSeq.Append (aSelIter.Value());
    }
  }
  for (SelectMgr_SequenceOfOwner::Iterator aDelIter (aSeq); aDelIter.More(); aDelIter.Next())
  {
    AddOrRemoveSelected (aDelIter.Value(), Standard_False);
  }
}

//=======================================================================
//function : ClearGlobal
//purpose  :
//=======================================================================
void AIS_InteractiveContext::ClearGlobal (const Handle(AIS_InteractiveObject)& theIObj,
                                          const Standard_Boolean               theToUpdateviewer)
{
  Handle(AIS_GlobalStatus) aStatus;
  if (theIObj.IsNull()
  || !myObjects.Find (theIObj, aStatus))
  {
    // for cases when reference shape of connected interactives was not displayed
    // but its selection primitives were calculated
    const Handle(SelectMgr_SelectableObject)& anObj = theIObj; // to avoid ambiguity
    mgrSelector->Remove (anObj);
    return;
  }

  unselectOwners (theIObj);

  myMainPM->Erase (theIObj, -1);
  theIObj->ErasePresentations (true); // make sure highlighting presentations are properly erased

  // Object removes from Detected sequence
  Standard_DISABLE_DEPRECATION_WARNINGS
  for (Standard_Integer aDetIter = myDetectedSeq.Lower(); aDetIter <= myDetectedSeq.Upper();)
  {
    Handle(SelectMgr_EntityOwner) aPicked = myMainSel->Picked (myDetectedSeq (aDetIter));
    Handle(AIS_InteractiveObject) anObj;
    if (!aPicked.IsNull())
    {
      anObj = Handle(AIS_InteractiveObject)::DownCast (aPicked->Selectable());
    }

    if (!anObj.IsNull()
      && anObj == theIObj)
    {
      myDetectedSeq.Remove (aDetIter);
      if (myCurDetected == aDetIter)
      {
        myCurDetected = Min (myDetectedSeq.Upper(), aDetIter);
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
  Standard_ENABLE_DEPRECATION_WARNINGS

  // remove IO from the selection manager to avoid memory leaks
  const Handle(SelectMgr_SelectableObject)& anObj = theIObj; // to avoid ambiguity
  mgrSelector->Remove (anObj);

  setObjectStatus (theIObj, AIS_DS_None, -1, -1);
  myMainVwr->StructureManager()->UnregisterObject (theIObj);

  for (V3d_ListOfViewIterator aDefViewIter (myMainVwr->DefinedViewIterator()); aDefViewIter.More(); aDefViewIter.Next())
  {
    aDefViewIter.Value()->View()->ChangeHiddenObjects()->Remove (theIObj.get());
  }

  if (!myLastPicked.IsNull())
  {
    if (myLastPicked->IsSameSelectable (theIObj))
    {
      clearDynamicHighlight();
      myLastPicked.Nullify();
    }
  }

  if (theToUpdateviewer && aStatus->GraphicStatus() == AIS_DS_Displayed)
  {
    myMainVwr->Update();
  }
}

//=======================================================================
//function : ClearGlobalPrs
//purpose  :
//=======================================================================
void AIS_InteractiveContext::ClearGlobalPrs (const Handle(AIS_InteractiveObject)& theIObj,
                                             const Standard_Integer               theMode,
                                             const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull()
  || !myObjects.IsBound (theIObj))
  {
    return;
  }

  const Handle(AIS_GlobalStatus)& aStatus = myObjects (theIObj);
  if (aStatus->DisplayMode() == theMode)
  {
    const Standard_Integer aDispMode = theIObj->HasHilightMode() ? theIObj->HilightMode() : 0;
    if (aDispMode == theMode
     && myMainPM->IsHighlighted (theIObj, theMode))
    {
      unhighlightGlobal (theIObj);
    }

    myMainPM->Erase (theIObj, theMode);
  }

  if (aStatus->GraphicStatus() == AIS_DS_Displayed
   && theToUpdateViewer)
  {
    myMainVwr->Update();
  }
}

//=======================================================================
//function : ClearDetected
//purpose  :
//=======================================================================
Standard_Boolean AIS_InteractiveContext::ClearDetected (Standard_Boolean theToRedrawImmediate)
{
  myCurDetected = 0;
  myCurHighlighted = 0;
  myDetectedSeq.Clear();
  Standard_Boolean toUpdate = Standard_False;
  if (!myLastPicked.IsNull() && myLastPicked->HasSelectable())
  {
    toUpdate = Standard_True;
    clearDynamicHighlight();
  }
  myLastPicked.Nullify();
  myMainSel->ClearPicked();
  if (toUpdate && theToRedrawImmediate)
  {
    myMainVwr->RedrawImmediate();
  }
  return toUpdate;
}

//=======================================================================
//function : DrawHiddenLine
//purpose  :
//=======================================================================
Standard_Boolean AIS_InteractiveContext::DrawHiddenLine() const
{
  return myDefaultDrawer->DrawHiddenLine();
}

//=======================================================================
//function : EnableDrawHiddenLine
//purpose  :
//=======================================================================
void AIS_InteractiveContext::EnableDrawHiddenLine() const
{
  myDefaultDrawer->EnableDrawHiddenLine();
}

//=======================================================================
//function : DisableDrawHiddenLine 
//purpose  :
//=======================================================================
void AIS_InteractiveContext::DisableDrawHiddenLine() const
{
  myDefaultDrawer->DisableDrawHiddenLine();
}

//=======================================================================
//function : HiddenLineAspect
//purpose  :
//=======================================================================
Handle (Prs3d_LineAspect) AIS_InteractiveContext::HiddenLineAspect() const
{
  return myDefaultDrawer->HiddenLineAspect();
}

//=======================================================================
//function : SetHiddenLineAspect
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetHiddenLineAspect (const Handle(Prs3d_LineAspect)& theAspect) const
{
  myDefaultDrawer->SetHiddenLineAspect (theAspect);
}

//=======================================================================
//function : SetIsoNumber
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetIsoNumber (const Standard_Integer theNb,
                                           const AIS_TypeOfIso    theType)
{
  switch (theType)
  {
    case AIS_TOI_IsoU:
      myDefaultDrawer->UIsoAspect()->SetNumber (theNb);
      break;
    case AIS_TOI_IsoV:
      myDefaultDrawer->VIsoAspect()->SetNumber (theNb);
      break;
    case AIS_TOI_Both:
      myDefaultDrawer->UIsoAspect()->SetNumber (theNb);
      myDefaultDrawer->VIsoAspect()->SetNumber (theNb);
      break;
  }
}

//=======================================================================
//function : IsoNumber
//purpose  :
//=======================================================================
Standard_Integer AIS_InteractiveContext::IsoNumber (const AIS_TypeOfIso theType)
{
  switch (theType)
  {
    case AIS_TOI_IsoU: return myDefaultDrawer->UIsoAspect()->Number();
    case AIS_TOI_IsoV: return myDefaultDrawer->VIsoAspect()->Number();
    case AIS_TOI_Both: return myDefaultDrawer->UIsoAspect()->Number() == myDefaultDrawer->VIsoAspect()->Number()
                            ? myDefaultDrawer->UIsoAspect()->Number()
                            : -1;
  }
  return 0;
}

//=======================================================================
//function : IsoOnPlane
//purpose  :
//=======================================================================
void AIS_InteractiveContext::IsoOnPlane (const Standard_Boolean theToSwitchOn)
{
  myDefaultDrawer->SetIsoOnPlane (theToSwitchOn);
}

//=======================================================================
//function : IsoOnPlane
//purpose  :
//=======================================================================
Standard_Boolean AIS_InteractiveContext::IsoOnPlane() const
{
  return myDefaultDrawer->IsoOnPlane();
}

//=======================================================================
//function : IsoOnTriangulation
//purpose  :
//=======================================================================
void AIS_InteractiveContext::IsoOnTriangulation (const Standard_Boolean theToSwitchOn)
{
  myDefaultDrawer->SetIsoOnTriangulation (theToSwitchOn);
}

//=======================================================================
//function : IsoOnTriangulation
//purpose  :
//=======================================================================
Standard_Boolean AIS_InteractiveContext::IsoOnTriangulation() const
{
  return myDefaultDrawer->IsoOnTriangulation();
}

//=======================================================================
//function : SetPixelTolerance
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetPixelTolerance (const Standard_Integer thePrecision)
{
  myMainSel->SetPixelTolerance (thePrecision);
}

//=======================================================================
//function : PixelTolerance
//purpose  :
//=======================================================================
Standard_Integer AIS_InteractiveContext::PixelTolerance() const
{
  return myMainSel->PixelTolerance();
}

//=======================================================================
//function : SetSelectionSensitivity
//purpose  : Allows to manage sensitivity of a particular selection of interactive object theObject
//=======================================================================
void AIS_InteractiveContext::SetSelectionSensitivity (const Handle(AIS_InteractiveObject)& theObject,
                                                      const Standard_Integer theMode,
                                                      const Standard_Integer theNewSensitivity)
{
  mgrSelector->SetSelectionSensitivity (theObject, theMode, theNewSensitivity);
}

//=======================================================================
//function : InitAttributes
//purpose  :
//=======================================================================
void AIS_InteractiveContext::InitAttributes()
{
  Graphic3d_MaterialAspect aMat (Graphic3d_NameOfMaterial_Brass);
  myDefaultDrawer->ShadingAspect()->SetMaterial (aMat);

//  myDefaultDrawer->ShadingAspect()->SetColor(Quantity_NOC_GRAY70);
  Handle(Prs3d_LineAspect) aLineAspect = myDefaultDrawer->HiddenLineAspect();
  aLineAspect->SetColor      (Quantity_NOC_GRAY20);
  aLineAspect->SetWidth      (1.0);
  aLineAspect->SetTypeOfLine (Aspect_TOL_DASH);

  // tolerance to 2 pixels...
  SetPixelTolerance (2);

  // Customizing the drawer for trihedrons and planes...
  Handle(Prs3d_DatumAspect) aTrihAspect = myDefaultDrawer->DatumAspect();
  const Standard_Real aLength = 100.0;
  aTrihAspect->SetAxisLength (aLength, aLength, aLength);
  const Quantity_Color aColor = Quantity_NOC_LIGHTSTEELBLUE4;
  aTrihAspect->LineAspect(Prs3d_DP_XAxis)->SetColor (aColor);
  aTrihAspect->LineAspect(Prs3d_DP_YAxis)->SetColor (aColor);
  aTrihAspect->LineAspect(Prs3d_DP_ZAxis)->SetColor (aColor);

  Handle(Prs3d_PlaneAspect) aPlaneAspect = myDefaultDrawer->PlaneAspect();
  const Standard_Real aPlaneLength = 200.0;
  aPlaneAspect->SetPlaneLength (aPlaneLength, aPlaneLength);
  aPlaneAspect->EdgesAspect()->SetColor (Quantity_NOC_SKYBLUE);
}

//=======================================================================
//function : TrihedronSize
//purpose  :
//=======================================================================
Standard_Real AIS_InteractiveContext::TrihedronSize() const
{
  return myDefaultDrawer->DatumAspect()->AxisLength(Prs3d_DP_XAxis);
}

//=======================================================================
//function : SetTrihedronSize
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetTrihedronSize (const Standard_Real    theVal,
                                               const Standard_Boolean /*updateviewer*/)
{
  myDefaultDrawer->DatumAspect()->SetAxisLength (theVal, theVal, theVal);
  Redisplay (AIS_KOI_Datum, 3, Standard_False);
  Redisplay (AIS_KOI_Datum, 4, Standard_True);
}

//=======================================================================
//function : SetPlaneSize
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetPlaneSize(const Standard_Real    theValX,
                                          const Standard_Real    theValY,
                                          const Standard_Boolean theToUpdateViewer)
{
  myDefaultDrawer->PlaneAspect()->SetPlaneLength (theValX, theValY);
  Redisplay (AIS_KOI_Datum, 7, theToUpdateViewer);
}

//=======================================================================
//function : SetPlaneSize
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetPlaneSize (const Standard_Real    theVal,
                                           const Standard_Boolean theToUpdateViewer)
{
  SetPlaneSize (theVal, theVal, theToUpdateViewer);
}

//=======================================================================
//function : PlaneSize
//purpose  :
//=======================================================================
Standard_Boolean AIS_InteractiveContext::PlaneSize (Standard_Real& theX,
                                                    Standard_Real& theY) const
{
  theX = myDefaultDrawer->PlaneAspect()->PlaneXLength();
  theY = myDefaultDrawer->PlaneAspect()->PlaneYLength();
  return (Abs (theX - theY) <= Precision::Confusion());
}

//=======================================================================
//function : SetZLayer
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetZLayer (const Handle(AIS_InteractiveObject)& theIObj,
                                        const Graphic3d_ZLayerId theLayerId)
{
  if (theIObj.IsNull())
    return;

  theIObj->SetZLayer (theLayerId);
}

//=======================================================================
//function : GetZLayer
//purpose  :
//=======================================================================
Graphic3d_ZLayerId AIS_InteractiveContext::GetZLayer (const Handle(AIS_InteractiveObject)& theIObj) const
{
  return !theIObj.IsNull()
       ?  theIObj->ZLayer()
       :  Graphic3d_ZLayerId_UNKNOWN;
}

//=======================================================================
//function : RebuildSelectionStructs
//purpose  : Rebuilds 1st level of BVH selection forcibly
//=======================================================================
void AIS_InteractiveContext::RebuildSelectionStructs()
{
  myMainSel->RebuildObjectsTree (Standard_True);
}

//=======================================================================
//function : Disconnect
//purpose  : Disconnects selectable object from an assembly and updates selection structures
//=======================================================================
void AIS_InteractiveContext::Disconnect (const Handle(AIS_InteractiveObject)& theAssembly,
                                         const Handle(AIS_InteractiveObject)& theObjToDisconnect)
{
  if (theAssembly->IsInstance ("AIS_MultipleConnectedInteractive"))
  {
    Handle(AIS_MultipleConnectedInteractive) theObj (Handle(AIS_MultipleConnectedInteractive)::DownCast (theAssembly));
    theObj->Disconnect (theObjToDisconnect);
    if (!myObjects.IsBound (theObjToDisconnect))
    {
      // connected presentation might contain displayed presentations
      myMainPM->Erase (theObjToDisconnect, -1);
      theObjToDisconnect->ErasePresentations (true);
    }

    const Handle(SelectMgr_SelectableObject)& anObj = theObjToDisconnect; // to avoid ambiguity
    mgrSelector->Remove (anObj);
  }
  else if (theAssembly->IsInstance ("AIS_ConnectedInteractive") && theObjToDisconnect.IsNull())
  {
    Handle(AIS_ConnectedInteractive) theObj (Handle(AIS_ConnectedInteractive)::DownCast (theAssembly));
    theObj->Disconnect();
    const Handle(SelectMgr_SelectableObject)& anObj = theObj; // to avoid ambiguity
    mgrSelector->Remove (anObj);
  }
  else
    return;
}

//=======================================================================
//function : FitSelected
//purpose  : Fits the view corresponding to the bounds of selected objects
//=======================================================================
void AIS_InteractiveContext::FitSelected (const Handle(V3d_View)& theView)
{
  FitSelected (theView, 0.01, Standard_True);
}

//=======================================================================
//function : BoundingBoxOfSelection
//purpose  :
//=======================================================================
Bnd_Box AIS_InteractiveContext::BoundingBoxOfSelection() const
{
  Bnd_Box aBndSelected;
  AIS_MapOfObjectOwners anObjectOwnerMap;
  for (AIS_NListOfEntityOwner::Iterator aSelIter (mySelection->Objects()); aSelIter.More(); aSelIter.Next())
  {
    const Handle(SelectMgr_EntityOwner)& anOwner = aSelIter.Value();
    Handle(AIS_InteractiveObject) anObj = Handle(AIS_InteractiveObject)::DownCast(anOwner->Selectable());
    if (anObj->IsInfinite())
    {
      continue;
    }

    if (anOwner == anObj->GlobalSelOwner())
    {
      Bnd_Box aTmpBnd;
      anObj->BoundingBox (aTmpBnd);
      aBndSelected.Add (aTmpBnd);
    }
    else
    {
      Handle(SelectMgr_IndexedMapOfOwner) anOwnerMap;
      if (!anObjectOwnerMap.Find (anOwner->Selectable(), anOwnerMap))
      {
        anOwnerMap = new SelectMgr_IndexedMapOfOwner();
        anObjectOwnerMap.Bind (anOwner->Selectable(), anOwnerMap);
      }

      anOwnerMap->Add (anOwner);
    }
  }

  for (AIS_MapIteratorOfMapOfObjectOwners anIter (anObjectOwnerMap); anIter.More(); anIter.Next())
  {
    const Handle(SelectMgr_SelectableObject) anObject = anIter.Key();
    Bnd_Box aTmpBox = anObject->BndBoxOfSelected (anIter.ChangeValue());
    aBndSelected.Add (aTmpBox);
  }

  return aBndSelected;
}

//=======================================================================
//function : FitSelected
//purpose  : Fits the view corresponding to the bounds of selected objects
//=======================================================================
void AIS_InteractiveContext::FitSelected (const Handle(V3d_View)& theView,
                                          const Standard_Real theMargin,
                                          const Standard_Boolean theToUpdate)
{
  Bnd_Box aBndSelected = BoundingBoxOfSelection();
  if (!aBndSelected.IsVoid())
  {
    theView->FitAll (aBndSelected, theMargin, theToUpdate);
  }
}

//=======================================================================
//function : SetTransformPersistence
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetTransformPersistence (const Handle(AIS_InteractiveObject)& theObject,
                                                      const Handle(Graphic3d_TransformPers)& theTrsfPers)
{
  theObject->SetTransformPersistence (theTrsfPers);
  if (!myObjects.IsBound (theObject))
  {
    return;
  }

  mgrSelector->UpdateSelection (theObject);

  const Standard_Integer    aLayerId   = myObjects.Find (theObject)->GetLayerIndex();
  const Handle(V3d_Viewer)& aCurViewer = CurrentViewer();
  for (V3d_ListOfViewIterator anActiveViewIter (aCurViewer->ActiveViewIterator()); anActiveViewIter.More(); anActiveViewIter.Next())
  {
    anActiveViewIter.Value()->View()->InvalidateBVHData (aLayerId);
    anActiveViewIter.Value()->View()->InvalidateZLayerBoundingBox (aLayerId);
  }
}

//=======================================================================
//function : GravityPoint
//purpose  :
//=======================================================================
gp_Pnt AIS_InteractiveContext::GravityPoint (const Handle(V3d_View)& theView) const
{
  return theView->GravityPoint();
}
//=======================================================================
//function : setObjectStatus
//purpose  :
//=======================================================================
void AIS_InteractiveContext::setObjectStatus (const Handle(AIS_InteractiveObject)& theIObj,
                                              const AIS_DisplayStatus theStatus,
                                              const Standard_Integer theDispMode,
                                              const Standard_Integer theSelectionMode)
{
  if (theStatus != AIS_DS_None)
  {
    Handle(AIS_GlobalStatus) aStatus = new AIS_GlobalStatus (AIS_DS_Displayed, theDispMode, theSelectionMode);
    myObjects.Bind (theIObj, aStatus);
  }
  else
    myObjects.UnBind (theIObj);

  for (PrsMgr_ListOfPresentableObjectsIter aPrsIter (theIObj->Children()); aPrsIter.More(); aPrsIter.Next())
  {
    Handle(AIS_InteractiveObject) aChild (Handle(AIS_InteractiveObject)::DownCast (aPrsIter.Value()));
    if (aChild.IsNull())
      continue;

    setObjectStatus (aChild, theStatus, theDispMode, theSelectionMode);
  }
}
