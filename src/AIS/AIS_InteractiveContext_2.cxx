// Created on: 1997-01-29
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


#include <AIS_DataMapIteratorOfDataMapOfIOStatus.hxx>
#include <AIS_GlobalStatus.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_Selection.hxx>
#include <Graphic3d_MapIteratorOfMapOfStructure.hxx>
#include <Graphic3d_MapOfStructure.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_StructureManager.hxx>
#include <Prs3d_BasicAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Quantity_Color.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Filter.hxx>
#include <SelectMgr_OrFilter.hxx>
#include <SelectMgr_SelectionManager.hxx>
#include <Standard_Transient.hxx>
#include <StdSelect_ViewerSelector3d.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Shape.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

//=======================================================================
//function : SetSelectionModeActive
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetSelectionModeActive (const Handle(AIS_InteractiveObject)& theObj,
                                                     const Standard_Integer theMode,
                                                     const Standard_Boolean theIsActive,
                                                     const AIS_SelectionModesConcurrency theActiveFilter,
                                                     const Standard_Boolean theIsForce)
{
  if (theObj.IsNull())
  {
    return;
  }

  const Handle(AIS_GlobalStatus)* aStat = myObjects.Seek (theObj);
  if (aStat == NULL)
  {
    return;
  }

  if (!theIsActive
   || (theMode == -1
    && theActiveFilter == AIS_SelectionModesConcurrency_Single))
  {
    if ((*aStat)->GraphicStatus() == AIS_DS_Displayed
     || theIsForce)
    {
      if (theMode == -1)
      {
        for (TColStd_ListIteratorOfListOfInteger aModeIter ((*aStat)->SelectionModes()); aModeIter.More(); aModeIter.Next())
        {
          mgrSelector->Deactivate (theObj, aModeIter.Value());
        }
      }
      else
      {
        mgrSelector->Deactivate (theObj, theMode);
      }
    }

    if (theMode == -1)
    {
      (*aStat)->ClearSelectionModes();
    }
    else
    {
      (*aStat)->RemoveSelectionMode (theMode);
    }
    return;
  }
  else if (theMode == -1)
  {
    return;
  }

  if ((*aStat)->SelectionModes().Size() == 1
   && (*aStat)->SelectionModes().First() == theMode)
  {
    return;
  }

  if ((*aStat)->GraphicStatus() == AIS_DS_Displayed
    || theIsForce)
  {
    switch (theActiveFilter)
    {
      case AIS_SelectionModesConcurrency_Single:
      {
        for (TColStd_ListIteratorOfListOfInteger aModeIter ((*aStat)->SelectionModes()); aModeIter.More(); aModeIter.Next())
        {
          mgrSelector->Deactivate (theObj, aModeIter.Value());
        }
        (*aStat)->ClearSelectionModes();
        break;
      }
      case AIS_SelectionModesConcurrency_GlobalOrLocal:
      {
        const Standard_Integer aGlobSelMode = theObj->GlobalSelectionMode();
        TColStd_ListOfInteger aRemovedModes;
        for (TColStd_ListIteratorOfListOfInteger aModeIter ((*aStat)->SelectionModes()); aModeIter.More(); aModeIter.Next())
        {
          if ((theMode == aGlobSelMode && aModeIter.Value() != aGlobSelMode)
           || (theMode != aGlobSelMode && aModeIter.Value() == aGlobSelMode))
          {
            mgrSelector->Deactivate (theObj, aModeIter.Value());
            aRemovedModes.Append (aModeIter.Value());
          }
        }
        if (aRemovedModes.Size() == (*aStat)->SelectionModes().Size())
        {
          (*aStat)->ClearSelectionModes();
        }
        else
        {
          for (TColStd_ListIteratorOfListOfInteger aModeIter (aRemovedModes); aModeIter.More(); aModeIter.Next())
          {
            (*aStat)->RemoveSelectionMode (aModeIter.Value());
          }
        }
        break;
      }
      case AIS_SelectionModesConcurrency_Multiple:
      {
        break;
      }
    }
    mgrSelector->Activate (theObj, theMode);
  }
  (*aStat)->AddSelectionMode (theMode);
}

// ============================================================================
// function : Activate
// purpose  :
// ============================================================================
void AIS_InteractiveContext::Activate (const Standard_Integer theMode,
                                       const Standard_Boolean theIsForce)
{
  AIS_ListOfInteractive aDisplayedObjects;
  DisplayedObjects (aDisplayedObjects);

  for (AIS_ListIteratorOfListOfInteractive anIter (aDisplayedObjects); anIter.More(); anIter.Next())
  {
    Load (anIter.Value(), -1);
    Activate (anIter.Value(), theMode, theIsForce);
  }

}

// ============================================================================
// function : Deactivate
// purpose  :
// ============================================================================
void AIS_InteractiveContext::Deactivate (const Standard_Integer theMode)
{
  AIS_ListOfInteractive aDisplayedObjects;
  DisplayedObjects (aDisplayedObjects);

  for (AIS_ListIteratorOfListOfInteractive anIter (aDisplayedObjects); anIter.More(); anIter.Next())
  {
    Deactivate (anIter.Value(), theMode);
  }
}

// ============================================================================
// function : Deactivate
// purpose  :
// ============================================================================
void AIS_InteractiveContext::Deactivate()
{
  AIS_ListOfInteractive aDisplayedObjects;
  DisplayedObjects (aDisplayedObjects);

  for (AIS_ListIteratorOfListOfInteractive anIter (aDisplayedObjects); anIter.More(); anIter.Next())
  {
    Deactivate (anIter.Value());
  }
}

//=======================================================================
//function : ActivatedModes
//purpose  :
//=======================================================================
void AIS_InteractiveContext::ActivatedModes (const Handle(AIS_InteractiveObject)& theObj,
                                             TColStd_ListOfInteger& theList) const
{
  const Handle(AIS_GlobalStatus)* aStatus = myObjects.Seek (theObj);
  if (aStatus != NULL)
  {
    for (TColStd_ListIteratorOfListOfInteger aModeIter ((*aStatus)->SelectionModes()); aModeIter.More(); aModeIter.Next())
    {
      theList.Append (aModeIter.Value());
    }
  }
}

//=======================================================================
//function : SubIntensityOn
//purpose  : 
//=======================================================================
void AIS_InteractiveContext::
SubIntensityOn(const Handle(AIS_InteractiveObject)& anIObj,
               const Standard_Boolean updateviewer)
{
  turnOnSubintensity (anIObj);
  if (updateviewer)
    myMainVwr->Update();
}
//=======================================================================
//function : SubIntensityOff
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::SubIntensityOff (const Handle(AIS_InteractiveObject)& theObj,
                                              const Standard_Boolean theToUpdateViewer)
{
  const Handle(AIS_GlobalStatus)* aStatus = myObjects.Seek (theObj);
  if (aStatus == NULL
   || !(*aStatus)->IsSubIntensityOn())
  {
    return;
  }

  (*aStatus)->SubIntensityOff();
  Standard_Boolean toUpdateMain = Standard_False;
  if ((*aStatus)->GraphicStatus() == AIS_DS_Displayed)
  {
    myMainPM->Unhighlight (theObj);
    toUpdateMain = Standard_True;
  }
    
  if (IsSelected (theObj))
  {
    highlightSelected (theObj->GlobalSelOwner());
  }

  if (theToUpdateViewer && toUpdateMain)
  {
    myMainVwr->Update();
  }
}

//=======================================================================
//function : AddFilter
//purpose  : 
//=======================================================================
void AIS_InteractiveContext::AddFilter(const Handle(SelectMgr_Filter)& aFilter)
{
  myFilters->Add(aFilter);
}

//=======================================================================
//function : RemoveFilter
//purpose  : 
//=======================================================================
void AIS_InteractiveContext::RemoveFilter(const Handle(SelectMgr_Filter)& aFilter)
{
  myFilters->Remove(aFilter);
}

//=======================================================================
//function : RemoveFilters
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::RemoveFilters()
{
  myFilters->Clear();
}

//=======================================================================
//function : Filters
//purpose  : 
//=======================================================================
const SelectMgr_ListOfFilter& AIS_InteractiveContext::Filters() const 
{
  return myFilters->StoredFilters();
}

//=======================================================================
//function : DisplayActiveSensitive
//purpose  : 
//=======================================================================
void AIS_InteractiveContext::DisplayActiveSensitive(const Handle(V3d_View)& aviou)
{
  myMainSel->DisplaySensitive(aviou);
}
//=======================================================================
//function : DisplayActiveSensitive
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::DisplayActiveSensitive(const Handle(AIS_InteractiveObject)& theObj,
                                                    const Handle(V3d_View)& theView)
{
  const Handle(AIS_GlobalStatus)* aStatus = myObjects.Seek (theObj);
  if (aStatus == NULL)
  {
    return;
  }

  for (TColStd_ListIteratorOfListOfInteger aModeIter ((*aStatus)->SelectionModes()); aModeIter.More(); aModeIter.Next())
  {
    const Handle(SelectMgr_Selection)& aSel = theObj->Selection (aModeIter.Value());
    myMainSel->DisplaySensitive (aSel, theObj->Transformation(), theView, Standard_False);
  }
}

//=======================================================================
//function : ClearActiveSensitive
//purpose  : 
//=======================================================================
void AIS_InteractiveContext::ClearActiveSensitive (const Handle(V3d_View)& theView)
{
  myMainSel->ClearSensitive (theView);
}

//=======================================================================
//function : PurgeDisplay
//purpose  : 
//=======================================================================

Standard_Integer AIS_InteractiveContext::PurgeDisplay()
{
  Standard_Integer NbStr = PurgeViewer(myMainVwr);
  myMainVwr->Update();
  return NbStr;
}


//=======================================================================
//function : PurgeViewer
//purpose  : 
//=======================================================================
Standard_Integer AIS_InteractiveContext::PurgeViewer(const Handle(V3d_Viewer)& Vwr)
{
  Handle(Graphic3d_StructureManager) GSM = Vwr->StructureManager();
  Standard_Integer NbCleared(0);
  Graphic3d_MapOfStructure SOS;
  GSM->DisplayedStructures(SOS);

  Handle(Graphic3d_Structure) G;
  for(Graphic3d_MapIteratorOfMapOfStructure It(SOS); It.More();It.Next()){
    G = It.Key();
    Standard_Address Add = G->Owner();
    if(Add==NULL){
      G->Erase();
      G->Clear();// it means that it is not referenced as a presentation of InterfactiveObject...
      NbCleared++;
    }
    Handle(AIS_InteractiveObject) IO = (AIS_InteractiveObject*)Add;
    if(!myObjects.IsBound(IO)){
      G->Erase();
      NbCleared++;
    }
  }
  return NbCleared;
}

//=======================================================================
//function : IsImmediateModeOn
//purpose  :
//=======================================================================

Standard_Boolean AIS_InteractiveContext::IsImmediateModeOn()  const 
{
  return myMainPM->IsImmediateModeOn();
}

//=======================================================================
//function : BeginImmediateDraw
//purpose  :
//=======================================================================

Standard_Boolean AIS_InteractiveContext::BeginImmediateDraw()
{
  if (myMainPM->IsImmediateModeOn())
  {
    myMainPM->BeginImmediateDraw();
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : ImmediateAdd
//purpose  :
//=======================================================================

Standard_Boolean AIS_InteractiveContext::ImmediateAdd (const Handle(AIS_InteractiveObject)& theObj,
                                                       const Standard_Integer               theMode)
{
  if (!myMainPM->IsImmediateModeOn())
  {
    return Standard_False;
  }

  myMainPM->AddToImmediateList (myMainPM->Presentation (theObj, theMode));
  return Standard_True;
}

//=======================================================================
//function : EndImmediateDraw
//purpose  :
//=======================================================================

Standard_Boolean AIS_InteractiveContext::EndImmediateDraw (const Handle(V3d_View)& theView)
{
  if (!myMainPM->IsImmediateModeOn())
  {
    return Standard_False;
  }

  myMainPM->EndImmediateDraw (theView->Viewer());
  return Standard_True;
}

//=======================================================================
//function : EndImmediateDraw
//purpose  :
//=======================================================================

Standard_Boolean AIS_InteractiveContext::EndImmediateDraw()
{
  if (!myMainPM->IsImmediateModeOn())
  {
    return Standard_False;
  }

  myMainPM->EndImmediateDraw (myMainVwr);
  return Standard_True;
}
