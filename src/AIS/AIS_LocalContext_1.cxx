// Created on: 1996-10-30
// Created by: Robert COUBLANC
// Copyright (c) 1996-1999 Matra Datavision
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

// Modified by rob Thu Apr 02 1998 
//              - use of optimisation in SelectMgr_ViewerSelector
//              -> Best management in detected entities...

#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_LocalContext.hxx>
#include <AIS_LocalStatus.hxx>
#include <AIS_Selection.hxx>
#include <AIS_Shape.hxx>
#include <Aspect_Grid.hxx>
#include <Geom_Transformation.hxx>
#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Graphic3d_Group.hxx>
#include <NCollection_Map.hxx>
#include <OSD_Environment.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Select3D_SensitiveTriangulation.hxx>
#include <SelectBasics_SensitiveEntity.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Filter.hxx>
#include <SelectMgr_OrFilter.hxx>
#include <SelectMgr_SelectableObject.hxx>
#include <SelectMgr_Selection.hxx>
#include <SelectMgr_SelectionManager.hxx>
#include <SelectMgr_SequenceOfOwner.hxx>
#include <Standard_Transient.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <StdSelect_ViewerSelector3d.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_MapIteratorOfMapOfTransient.hxx>
#include <TColStd_MapOfTransient.hxx>
#include <TopoDS_Shape.hxx>
#include <V3d_View.hxx>

static Standard_Integer GetHiMod(const Handle(AIS_InteractiveObject)& IO)
{
  return IO->HasHilightMode() ? IO->HilightMode():0;
}

//==================================================
// Function: MoveTo
// Purpose :
//==================================================
AIS_StatusOfDetection AIS_LocalContext::MoveTo (const Standard_Integer  theXpix,
                                                const Standard_Integer  theYpix,
                                                const Handle(V3d_View)& theView,
                                                const Standard_Boolean  theToRedrawImmediate)
{
  // check that ViewerSelector gives
  if (theView->Viewer() != myCTX->CurrentViewer())
  {
    return AIS_SOD_Error;
  }

  myAISCurDetected = 0;
  myAISDetectedSeq.Clear();

  myCurDetected = 0;
  myDetectedSeq.Clear();
  myFilters->SetDisabledObjects (theView->View()->HiddenObjects());
  myMainVS->Pick (theXpix, theYpix, theView);

  const Standard_Integer aDetectedNb = myMainVS->NbPicked();
  for (Standard_Integer aDetIter = 1; aDetIter <= aDetectedNb; ++aDetIter)
  {
    Handle(SelectMgr_EntityOwner) anOwner = myMainVS->Picked (aDetIter);
    if (anOwner.IsNull()
     || !myFilters->IsOk (anOwner))
    {
      continue;
    }

    myDetectedSeq.Append (aDetIter); // normally they are already arranged in correct order...
    Handle(AIS_InteractiveObject) anObj = Handle(AIS_InteractiveObject)::DownCast (anOwner->Selectable());
    if (!anObj.IsNull())
    {
      myAISDetectedSeq.Append (anObj);
    }
  }

  // result of courses..
  if (aDetectedNb == 0 || myDetectedSeq.IsEmpty())
  {
    if (mylastindex != 0 && mylastindex <= myMapOfOwner->Extent())
    {
      myMainPM->ClearImmediateDraw();
      Unhilight (myMapOfOwner->FindKey (mylastindex), theView);
      if (theToRedrawImmediate)
      {
        theView->RedrawImmediate();
      }
    }

    mylastindex = 0;
    return aDetectedNb == 0
         ? AIS_SOD_Nothing
         : AIS_SOD_AllBad;
  }

  // all owners detected by the selector are passed to the
  // filters and correct ones are preserved...
  myCurDetected = 1;
  Handle(SelectMgr_EntityOwner) anOwner = myMainVS->Picked (myDetectedSeq (myCurDetected));
  manageDetected (anOwner, theView, theToRedrawImmediate);
  if (myDetectedSeq.Length() == 1)
  {
    return aDetectedNb == 1
         ? AIS_SOD_OnlyOneDetected
         : AIS_SOD_OnlyOneGood;
  }
  else
  {
    return AIS_SOD_SeveralGood;
  }
}

//=======================================================================
//function : Select
//purpose  : 
//=======================================================================
AIS_StatusOfPick AIS_LocalContext::Select (const Standard_Boolean toUpdateViewer)
{
  if (myAutoHilight)
  {
    UnhilightPicked (Standard_False);
  }

  AIS_Selection::SetCurrentSelection (mySelName.ToCString());

  Standard_Integer aDetIndex = DetectedIndex();
  if (aDetIndex <= 0)
  {
    ClearSelected (toUpdateViewer);
    return (AIS_Selection::Extent() == 0) ? AIS_SOP_NothingSelected : AIS_SOP_Removed;
  }

  const Handle(SelectMgr_EntityOwner)& anOwner = myMapOfOwner->FindKey (aDetIndex);

  ClearSelected (Standard_False);

  if (!anOwner->IsSelected()) // anOwner is not selected
  {
    anOwner->SetSelected (Standard_True);
    AIS_Selection::Select (anOwner);
  }

  if (myAutoHilight)
  {
    const Handle(V3d_Viewer)& aViewer = myCTX->CurrentViewer();
    for (aViewer->InitActiveViews(); aViewer->MoreActiveViews(); aViewer->NextActiveViews())
    {
      Unhilight (anOwner, aViewer->ActiveView());
    }

    // advanced selection highlighting mechanism
    if (!anOwner->IsAutoHilight() && anOwner->HasSelectable())
    {
      Handle(AIS_InteractiveObject) anIO = Handle(AIS_InteractiveObject)::DownCast(anOwner->Selectable());
      UpdateSelected (anIO, Standard_False);
    }

    if (toUpdateViewer)
    {
      myCTX->CurrentViewer()->Update();
    }
  }

  return (AIS_Selection::Extent() == 1) ? AIS_SOP_OneSelected : AIS_SOP_SeveralSelected;
}

//=======================================================================
//function : Select
//purpose  : 
//=======================================================================
AIS_StatusOfPick AIS_LocalContext::Select (const Standard_Integer  theXPMin,
                                           const Standard_Integer  theYPMin,
                                           const Standard_Integer  theXPMax,
                                           const Standard_Integer  theYPMax,
                                           const Handle(V3d_View)& theView,
                                           const Standard_Boolean  toUpdateViewer)
{
  if (theView->Viewer() == myCTX->CurrentViewer())
  {
    myMainVS->Pick (theXPMin, theYPMin, theXPMax, theYPMax, theView);
    if (myAutoHilight)
    {
      UnhilightPicked (Standard_False);
    }

    AIS_Selection::SetCurrentSelection (mySelName.ToCString());
    Standard_Integer aSelNum = AIS_Selection::Extent();

    myMainVS->Init();
    if (!myMainVS->More())
    {
      ClearSelected (toUpdateViewer);
      mylastindex = 0;
      return aSelNum == 0 ? AIS_SOP_NothingSelected : AIS_SOP_Removed;
    }

    ClearSelected (Standard_False);

    for (myMainVS->Init(); myMainVS->More(); myMainVS->Next())
    {
      const Handle(SelectMgr_EntityOwner)& anOwner = myMainVS->Picked();
      if (myFilters->IsOk (anOwner))
      {
        // it can be helpful to classify this owner immediately...
        if (!anOwner->IsSelected())
        {
          anOwner->SetSelected (Standard_True);
          AIS_Selection::Select (anOwner);
        }
      }
    }

    if (myAutoHilight)
    {
      HilightPicked (toUpdateViewer);
    }
  }

  Standard_Integer aSelNum = AIS_Selection::Extent();

  return (aSelNum == 1) ? AIS_SOP_OneSelected
                        : (aSelNum > 1) ? AIS_SOP_SeveralSelected
                                        : AIS_SOP_Error;
}

//==================================================
// Function: Select
// Purpose : Selection by polyline
//==================================================
AIS_StatusOfPick AIS_LocalContext::Select (const TColgp_Array1OfPnt2d& thePolyline,
                                           const Handle(V3d_View)& theView,
                                           const Standard_Boolean toUpdateViewer)
{
  if (theView->Viewer() == myCTX->CurrentViewer())
  {
    myMainVS->Pick (thePolyline, theView);

    AIS_Selection::SetCurrentSelection (mySelName.ToCString());

    Standard_Integer aLastSelNum = AIS_Selection::Extent();
    myMainVS->Init();
    if (!myMainVS->More())
    {
      // Nothing is selected clear selection.
      ClearSelected (toUpdateViewer);
      mylastindex = 0;

      // Return state to know if something was unselected
      return aLastSelNum == 0 ? AIS_SOP_NothingSelected : AIS_SOP_Removed;
    }

    if (myAutoHilight)
    {
      UnhilightPicked (Standard_False);
    }

    // Clear previous selection without update to process this selection
    ClearSelected (Standard_False);

    for (myMainVS->Init(); myMainVS->More(); myMainVS->Next())
    {
      const Handle(SelectMgr_EntityOwner)& anOwner = myMainVS->Picked();
      if (myFilters->IsOk (anOwner))
      {
        // it can be helpful to classify this owner immediately...
        if (!anOwner->IsSelected())
        {
          AIS_Selection::AddSelect (anOwner);
          anOwner->SetSelected (Standard_True);
        }
      }
    }

    if (myAutoHilight)
    {
      HilightPicked (toUpdateViewer);
    }
  }

  Standard_Integer aSelNum = AIS_Selection::Extent();
  return (aSelNum == 1) ? AIS_SOP_OneSelected
                        : (aSelNum > 1) ? AIS_SOP_SeveralSelected
                                        : AIS_SOP_Error;
}

//=======================================================================
//function : ShiftSelect
//purpose  : 
//=======================================================================
AIS_StatusOfPick AIS_LocalContext::ShiftSelect (const Standard_Boolean toUpdateViewer)
{
  Standard_Integer aDetIndex = DetectedIndex();
  AIS_Selection::SetCurrentSelection (mySelName.ToCString());

  if(aDetIndex > 0)
  {
    AIS_Selection::SetCurrentSelection (mySelName.ToCString());
    Standard_Integer aSelNum = AIS_Selection::Extent();
    const Handle(SelectMgr_EntityOwner)& anOwner = myMapOfOwner->FindKey (aDetIndex);
    Standard_Boolean toSelect = anOwner->IsSelected() ? Standard_False : Standard_True;
    AIS_Selection::Select (anOwner);
    anOwner->SetSelected (toSelect);

    if(myAutoHilight)
    {
      myMainPM->ClearImmediateDraw();
      const Handle(V3d_Viewer)& aViewer = myCTX->CurrentViewer();
      for (aViewer->InitActiveViews(); aViewer->MoreActiveViews(); aViewer->NextActiveViews())
      {
        Unhilight (anOwner, aViewer->ActiveView());
      }

      // advanced selection highlighting mechanism
      if (!anOwner->IsAutoHilight() && anOwner->HasSelectable())
      {
        Handle(AIS_InteractiveObject) anIO = Handle(AIS_InteractiveObject)::DownCast (anOwner->Selectable());
        UpdateSelected (anIO, Standard_False);
      }

      if (toUpdateViewer)
      {
        myCTX->CurrentViewer()->Update();
      }
    } 

    Standard_Integer NS = AIS_Selection::Extent();
    if( NS == 1 ) return AIS_SOP_OneSelected;
    else if( NS > 1 ) return AIS_SOP_SeveralSelected;
    return aSelNum == 0 ? AIS_SOP_NothingSelected : AIS_SOP_Removed;
  }
  return AIS_SOP_Error;
}

//=======================================================================
//function : ShiftSelect
//purpose  : 
//=======================================================================
AIS_StatusOfPick AIS_LocalContext::ShiftSelect (const Standard_Integer  theXPMin,
                                                const Standard_Integer  theYPMin,
                                                const Standard_Integer  theXPMax,
                                                const Standard_Integer  theYPMax,
                                                const Handle(V3d_View)& theView,
                                                const Standard_Boolean  toUpdateViewer)
{
  myMainPM->ClearImmediateDraw();

  if (theView->Viewer() == myCTX->CurrentViewer())
  {
    myMainVS->Pick (theXPMin, theYPMin, theXPMax, theYPMax, theView);

    AIS_Selection::SetCurrentSelection (mySelName.ToCString());
    Standard_Integer aLastSelNum = AIS_Selection::Extent();

    myMainVS->Init();
    if (!myMainVS->More())
    {
      // Nothing is selected clear selection, but don't clear the selection
      // as it is shift selection and previous selection matters.
      // Return state to know if something was unselected
      return aLastSelNum == 0 ? AIS_SOP_NothingSelected : AIS_SOP_Removed;
    }

    if (myAutoHilight)
    {
      UnhilightPicked (Standard_False);
    }

    for (myMainVS->Init(); myMainVS->More(); myMainVS->Next())
    {
      const Handle(SelectMgr_EntityOwner)& anOwner = myMainVS->Picked();
      if(myFilters->IsOk (anOwner))
      {
        Standard_Boolean toSelect = anOwner->IsSelected() ? Standard_False : Standard_True;
        AIS_Selection::Select (anOwner);
        anOwner->SetSelected (toSelect);
      }
    }

    if (myAutoHilight)
    {
      HilightPicked (toUpdateViewer);
    }
  }

  Standard_Integer aSelNum = AIS_Selection::Extent();

  return (aSelNum == 1) ? AIS_SOP_OneSelected
                        : (aSelNum > 1) ? AIS_SOP_SeveralSelected
                                        : AIS_SOP_Error;
}

//==================================================
// Function: Select
// Purpose : Selection by polyline
//==================================================
AIS_StatusOfPick AIS_LocalContext::ShiftSelect (const TColgp_Array1OfPnt2d& thePolyline,
                                                const Handle(V3d_View)&     theView,
                                                const Standard_Boolean      toUpdateViewer)
{
  if (theView->Viewer() == myCTX->CurrentViewer())
  {
    myMainVS->Pick (thePolyline, theView);

    AIS_Selection::SetCurrentSelection (mySelName.ToCString());

    Standard_Integer aLastSelNum = AIS_Selection::Extent();
    myMainVS->Init();
    if(!myMainVS->More())
    {
      // Nothing is selected clear selection, but don't clear the selection
      // as it is shift selection and previous selection matters.
      // Return state to know if something was unselected
      return aLastSelNum == 0 ? AIS_SOP_NothingSelected : AIS_SOP_Removed;
    }

    if (myAutoHilight)
    {
      UnhilightPicked (Standard_False);
    }

    for (myMainVS->Init(); myMainVS->More(); myMainVS->Next())
    {
      const Handle(SelectMgr_EntityOwner)& anOwner = myMainVS->Picked();
      if (myFilters->IsOk (anOwner))
      {
        Standard_Boolean toSelect = anOwner->IsSelected() ? Standard_False : Standard_True;
        AIS_Selection::Select (anOwner);
        anOwner->SetSelected (toSelect);
      }
    }
    if (myAutoHilight)
    {
      HilightPicked (toUpdateViewer);
    }
  }

  Standard_Integer aSelNum = AIS_Selection::Extent();

  return (aSelNum == 1) ? AIS_SOP_OneSelected
                        : (aSelNum > 1) ? AIS_SOP_SeveralSelected
                                        : AIS_SOP_Error;
}

//==================================================
// Function: Hilight
// Purpose :
//==================================================
void AIS_LocalContext::Hilight (const Handle(SelectMgr_EntityOwner)& theOwner,
                                const Handle(V3d_View)&              theView)
{
  if (theView.IsNull())
  {
    return;
  }

  const Standard_Integer aHilightMode = GetHiMod (Handle(AIS_InteractiveObject)::DownCast (theOwner->Selectable()));
  myMainPM->BeginImmediateDraw();
  theOwner->HilightWithColor (myMainPM, myCTX->HilightColor(), aHilightMode);
  myMainPM->EndImmediateDraw (theView->Viewer());
}

//==================================================
// Function: Unhilight
// Purpose :
//==================================================
void AIS_LocalContext::Unhilight (const Handle(SelectMgr_EntityOwner)& theOwner,
                                  const Handle(V3d_View)&              theView)
{
  if (theView.IsNull())
  {
    return;
  }

  const Standard_Integer aHilightMode = GetHiMod (Handle(AIS_InteractiveObject)::DownCast (theOwner->Selectable()));
  if (IsSelected (theOwner))
  {
    if (theOwner->IsAutoHilight())
    {
      theOwner->HilightWithColor (myMainPM, myCTX->SelectionColor(), aHilightMode);
    }
  }
  else
  {
    theOwner->Unhilight (myMainPM, aHilightMode);
  }
}

//=======================================================================
//function : HilightPicked
//purpose  : 
//=======================================================================
void AIS_LocalContext::HilightPicked(const Standard_Boolean updateviewer)
{
  Handle(AIS_Selection) Sel = AIS_Selection::Selection(mySelName.ToCString());
  if( Sel.IsNull() ) return;

  typedef NCollection_DataMap <Handle(SelectMgr_SelectableObject), NCollection_Handle<SelectMgr_SequenceOfOwner> > SelectMgr_DataMapOfObjectOwners;
  SelectMgr_DataMapOfObjectOwners aMap;

  Handle (PrsMgr_PresentationManager3d) PM = myMainPM;
  
  // to avoid problems when there is a loop searching for selected objects...
  const AIS_NListTransient& Obj = Sel->Objects();
  AIS_NListTransient::Iterator anIter( Obj );
  for(; anIter.More(); anIter.Next())
  {
    const Handle(Standard_Transient)& Tr = anIter.Value();
    if(!Tr.IsNull()){
      const Handle(SelectMgr_EntityOwner)& Ownr =
        *((const Handle(SelectMgr_EntityOwner)*) &Tr);
      Handle(AIS_InteractiveObject) IO;
      if(Ownr->HasSelectable()){
	Handle(StdSelect_BRepOwner) BROwnr = Handle(StdSelect_BRepOwner)::DownCast(Ownr);
	if(BROwnr.IsNull() || !BROwnr->ComesFromDecomposition()){
	  Handle(SelectMgr_SelectableObject) SO  = Ownr->Selectable();
	  IO = Handle(AIS_InteractiveObject)::DownCast (SO);
	}
      }
      Handle(SelectMgr_SelectableObject) SO = Ownr->Selectable();
      Standard_Integer HM = GetHiMod(Handle(AIS_InteractiveObject)::DownCast (SO));
      if ( Ownr->IsAutoHilight() )
        Ownr->HilightWithColor(PM,myCTX->SelectionColor(),HM);
      else if ( aMap.IsBound (SO) )
        aMap(SO)->Append ( Ownr );        
      else {
        NCollection_Handle<SelectMgr_SequenceOfOwner> aSeq = new SelectMgr_SequenceOfOwner;
        aSeq->Append ( Ownr );
        aMap.Bind ( SO, aSeq );
      }      
    }
  }

  for ( SelectMgr_DataMapOfObjectOwners::Iterator aMapIter(aMap); 
        aMapIter.More(); aMapIter.Next() )
  {
    aMapIter.Key()->HilightSelected (myMainPM, *aMapIter.Value());
  }

  if (updateviewer)
  {
    myCTX->CurrentViewer()->Update();
  }
}

//==================================================
// Function: 
// Purpose :
//==================================================
void AIS_LocalContext::UnhilightPicked (const Standard_Boolean updateviewer)
{
  myMainPM->ClearImmediateDraw();

  Handle(AIS_Selection) Sel = AIS_Selection::Selection(mySelName.ToCString());
  if( Sel.IsNull() ) return;
  Handle (PrsMgr_PresentationManager3d) PM = myMainPM;
  NCollection_Map<Handle(SelectMgr_SelectableObject)> anObjMap;
  
  const AIS_NListTransient& Obj = Sel->Objects();
  AIS_NListTransient::Iterator anIter( Obj );
  for(; anIter.More(); anIter.Next()){
    const Handle(Standard_Transient)& Tr = anIter.Value();
    if(!Tr.IsNull()){
      const Handle(SelectMgr_EntityOwner)& Ownr =
        *((const Handle(SelectMgr_EntityOwner)*) &Tr);
      Standard_Integer HM(0);
      if(Ownr->HasSelectable()){
	Handle(SelectMgr_SelectableObject) SO  = Ownr->Selectable();
	Handle(AIS_InteractiveObject) IO = Handle(AIS_InteractiveObject)::DownCast (SO);
        anObjMap.Add (IO);

        HM = GetHiMod(IO);
	Handle(StdSelect_BRepOwner) BROwnr = Handle(StdSelect_BRepOwner)::DownCast(Ownr);
	if(BROwnr.IsNull() || !BROwnr->ComesFromDecomposition()){
	}
      }
      Ownr->Unhilight(PM,HM);
    }
  }
  
  for (NCollection_Map<Handle(SelectMgr_SelectableObject)>::Iterator anIter1 ( anObjMap ); 
        anIter1.More(); anIter1.Next() )
  {
    if ( !anIter1.Key()->IsAutoHilight() )
      anIter1.Key()->ClearSelected();
  }

  if(updateviewer)
    myCTX->CurrentViewer()->Update();
}

//=======================================================================
//function : IsSelected
//purpose  : 
//=======================================================================
Standard_Boolean AIS_LocalContext::IsSelected(const Handle(AIS_InteractiveObject)& anIObj) const 
{
  return (!FindSelectedOwnerFromIO(anIObj).IsNull());
}

//=======================================================================
//function : IsSelected
//purpose  : 
//=======================================================================

Standard_Boolean AIS_LocalContext::IsSelected (const Handle(SelectMgr_EntityOwner)& theOwner) const 
{
  return !theOwner.IsNull() && theOwner->IsSelected();
}

//==================================================
// Function: 
// Purpose :
//==================================================
void AIS_LocalContext::
InitSelected()
{
  AIS_Selection::SetCurrentSelection(mySelName.ToCString());
  AIS_Selection::CurrentSelection()->Init();
}

//==================================================
// Function: 
// Purpose :
//==================================================
Standard_Boolean AIS_LocalContext::
MoreSelected() const 
{
  return AIS_Selection::CurrentSelection()->More();
}

//==================================================
// Function: 
// Purpose :
//==================================================
void AIS_LocalContext::
NextSelected()
{
  AIS_Selection::CurrentSelection()->Next();
}

//==================================================
// Function: 
// Purpose :
//==================================================
Standard_Boolean AIS_LocalContext::
HasShape() const 
{
  Handle(Standard_Transient) Tr = AIS_Selection::CurrentSelection()->Value();
  if( Tr.IsNull() ) return Standard_False;
  Handle(SelectMgr_EntityOwner) EO = Handle(SelectMgr_EntityOwner)::DownCast (Tr);
  Handle(StdSelect_BRepOwner) BRO = Handle(StdSelect_BRepOwner)::DownCast(EO);
  if(BRO.IsNull()) return Standard_False;
  Standard_Boolean hasshape = BRO->HasShape();
  Standard_Boolean comes = BRO->ComesFromDecomposition();
  return (hasshape&&comes);
}

//================================================================
// Function : HasSelectedShape
// Purpose  : Checks if there is a selected shape regardless of its decomposition status
//================================================================
Standard_Boolean AIS_LocalContext::HasSelectedShape() const
{
  if (AIS_Selection::CurrentSelection()->Extent() == 0)
    return Standard_False;

  Handle(Standard_Transient) aCurSelection = AIS_Selection::CurrentSelection()->Value();
  if (aCurSelection.IsNull())
    return Standard_False;

  Handle(SelectMgr_EntityOwner) anOwner = Handle(SelectMgr_EntityOwner)::DownCast (aCurSelection);
  Handle(StdSelect_BRepOwner) aBrepOwner = Handle(StdSelect_BRepOwner)::DownCast (anOwner);
  if (aBrepOwner.IsNull())
  {
    return Standard_False;
  }
  return aBrepOwner->HasShape();
}

//==================================================
// Function: 
// Purpose :
//==================================================
TopoDS_Shape AIS_LocalContext::SelectedShape() const 
{
  Handle(Standard_Transient) aTr = AIS_Selection::CurrentSelection()->Value();
  Handle(SelectMgr_EntityOwner) anEO = Handle(SelectMgr_EntityOwner)::DownCast (aTr);
  Handle(StdSelect_BRepOwner) aBRO = Handle(StdSelect_BRepOwner)::DownCast(anEO);
  if( aBRO.IsNull() ) 
  {
    return TopoDS_Shape();
  }

  return aBRO->Shape().Located (aBRO->Location() * aBRO->Shape().Location());
}

//==================================================
// Function: 
// Purpose :
//==================================================
Handle(AIS_InteractiveObject) AIS_LocalContext::
SelectedInteractive() const 
{
  Handle(AIS_InteractiveObject) IO;
  Handle(Standard_Transient) Tr = AIS_Selection::CurrentSelection()->Value();
  if( !Tr.IsNull() ) {
    Handle(SelectMgr_EntityOwner) EO = Handle(SelectMgr_EntityOwner)::DownCast (Tr);
    Handle(SelectMgr_SelectableObject) SO;
    if(EO->HasSelectable()){
      SO = EO->Selectable();
      IO = Handle(AIS_InteractiveObject)::DownCast (SO);
    }
  }
  return IO;
}
//==================================================
// Function: 
// Purpose :
//==================================================
Handle(SelectMgr_EntityOwner) AIS_LocalContext::
SelectedOwner() const 
{
  Handle(SelectMgr_EntityOwner) EO;
  Handle(Standard_Transient) Tr = AIS_Selection::CurrentSelection()->Value();
  if( !Tr.IsNull() )
  	EO = Handle(SelectMgr_EntityOwner)::DownCast (Tr);
  return EO;
}

//==================================================
// Function: 
// Purpose :
//==================================================
Standard_Boolean AIS_LocalContext::
HasApplicative() const 
{
  Handle(AIS_InteractiveObject) IO = SelectedInteractive();
  if( IO.IsNull() ) return Standard_False;
  return IO->HasOwner();
}

//==================================================
// Function: 
// Purpose :
//==================================================
const Handle(Standard_Transient)& AIS_LocalContext::
SelectedApplicative() const 
{
  return SelectedInteractive()->GetOwner();
}



//=======================================================================
//function : UpdateSelection
//purpose  : should disappear...
//=======================================================================
void AIS_LocalContext::UpdateSelected(const Standard_Boolean updateviewer)
{
  UnhilightPicked(Standard_False);
  HilightPicked(updateviewer);
}

//================================================================
// Function : UpdateSelected
// Purpose  : Part of advanced selection mechanism.
//            Highlightes or clears selection presentation for the given IO
//================================================================
void AIS_LocalContext::UpdateSelected(const Handle(AIS_InteractiveObject)& anobj,
                                      const Standard_Boolean updateviewer)
{
  if (anobj.IsNull() || anobj->IsAutoHilight())
    return;

  AIS_Selection::SetCurrentSelection(mySelName.ToCString());
  Handle(AIS_Selection) Sel = AIS_Selection::CurrentSelection();

  SelectMgr_SequenceOfOwner aSeq;
  for ( Sel->Init(); Sel->More(); Sel->Next() ){
    Handle(SelectMgr_EntityOwner) aOwner = Handle(SelectMgr_EntityOwner)::DownCast(Sel->Value());

    if ( !aOwner.IsNull() && aOwner->HasSelectable() && aOwner->Selectable() == anobj )
      aSeq.Append( aOwner );
  }

  if ( aSeq.Length() )
    anobj->HilightSelected( myMainPM, aSeq );
  else
    anobj->ClearSelected();

  if(updateviewer){
     myCTX->CurrentViewer()->Update();
  }
}

//==================================================
// Function: ClearSelected
// Purpose :
//==================================================
void AIS_LocalContext::ClearSelected (const Standard_Boolean updateviewer)
{
  UnhilightPicked(updateviewer);
  AIS_Selection::SetCurrentSelection(mySelName.ToCString());

  Handle(AIS_Selection) Sel = AIS_Selection::CurrentSelection();
  const AIS_NListTransient& Obj = Sel->Objects();
  AIS_NListTransient::Iterator anIter( Obj );
  for(; anIter.More(); anIter.Next()){
    const Handle(Standard_Transient)& Tr = anIter.Value();
    if(!Tr.IsNull())
    {
      (*((const Handle(SelectMgr_EntityOwner)*)&Tr))->SetSelected (Standard_False);
    }
  }
  AIS_Selection::Select();
  mylastindex = 0;
}

//==================================================
// Function: ClearOutdatedSelection
// Purpose :
//==================================================
void AIS_LocalContext::ClearOutdatedSelection (const Handle(AIS_InteractiveObject)& theIO,
                                               const Standard_Boolean toClearDeactivated)
{
  // 1. Collect selectable entities
  SelectMgr_IndexedMapOfOwner aValidOwners;

  const TColStd_ListOfInteger& aModes = SelectionModes (theIO);

  TColStd_ListIteratorOfListOfInteger aModeIter (aModes);
  for (; aModeIter.More(); aModeIter.Next())
  {
    int aMode = aModeIter.Value();
    if (!theIO->HasSelection(aMode))
    {
      continue;
    }

    if (toClearDeactivated && !mySM->IsActivated(theIO, aMode, myMainVS))
    {
      continue;
    }

    Handle(SelectMgr_Selection) aSelection = theIO->Selection(aMode);
    for (aSelection->Init(); aSelection->More(); aSelection->Next())
    {
      Handle(SelectBasics_SensitiveEntity) anEntity = aSelection->Sensitive()->BaseSensitive();
      if (anEntity.IsNull())
      {
        continue;
      }

      Handle(SelectMgr_EntityOwner) anOwner =
        Handle(SelectMgr_EntityOwner)::DownCast (anEntity->OwnerId());

      if (anOwner.IsNull())
      {
        continue;
      }

      aValidOwners.Add(anOwner);
    }
  }

  // 2. Refresh context's detection and selection and keep only active owners
  // Keep last detected object for lastindex initialization.
  Handle(SelectMgr_EntityOwner) aLastPicked = myMainVS->OnePicked();

  // Remove entity owners from detected sequences
  for (Standard_Integer anIdx = 1; anIdx <= myDetectedSeq.Length(); ++anIdx)
  {
    Handle(SelectMgr_EntityOwner) anOwner = myMainVS->Picked (myDetectedSeq (anIdx));
    if (anOwner.IsNull() || anOwner->Selectable() != theIO || aValidOwners.Contains (anOwner))
    {
      continue;
    }

    myDetectedSeq.Remove (anIdx--);

    if (anIdx < myCurDetected)
    {
      myCurDetected--;
    }
  }
  myCurDetected = Max (myCurDetected, 1);

  Standard_Boolean isAISRemainsDetected = Standard_False;

  // 3. AIS_Selection : remove entity owners from AIS_Selection
  const Handle(V3d_Viewer)& aViewer = myCTX->CurrentViewer();
  Handle(AIS_Selection) aSelection = AIS_Selection::Selection (mySelName.ToCString());
  AIS_NListTransient::Iterator anIter (aSelection->Objects());
  AIS_NListTransient aRemoveEntites;
  for (; anIter.More(); anIter.Next())
  {
    Handle(SelectMgr_EntityOwner) anOwner = Handle(SelectMgr_EntityOwner)::DownCast (anIter.Value());
    if (anOwner.IsNull() || anOwner->Selectable() != theIO)
    {
      continue;
    }

    if (aValidOwners.Contains (anOwner))
    {
      isAISRemainsDetected = Standard_True;
    }
    else
    {
      aRemoveEntites.Append (anOwner);
      anOwner->SetSelected (Standard_False);
      for (aViewer->InitActiveViews(); aViewer->MoreActiveViews(); aViewer->NextActiveViews())
      {
        Unhilight (anOwner, aViewer->ActiveView());
      }
    }
  }
  AIS_NListTransient::Iterator anIterRemove (aRemoveEntites);
  for (; anIterRemove.More(); anIterRemove.Next())
  {
    aSelection->Select (anIterRemove.Value());
  }

  // 4. AIS_LocalContext - myMapOfOwner : remove entity owners from myMapOfOwner
  SelectMgr_IndexedMapOfOwner anOwnersToKeep;
  for (Standard_Integer anIdx = 1; anIdx <= myMapOfOwner->Extent(); anIdx++)
  {
    Handle(SelectMgr_EntityOwner) anOwner = myMapOfOwner->FindKey (anIdx);
    if (anOwner.IsNull())
    {
      continue;
    }

    if (anOwner->Selectable() != theIO || aValidOwners.Contains (anOwner))
    {
      anOwnersToKeep.Add (anOwner);
    }
    else
    {
      for (aViewer->InitActiveViews(); aViewer->MoreActiveViews(); aViewer->NextActiveViews())
      {
        Unhilight (anOwner, aViewer->ActiveView());
      }
    }
  }
  myMapOfOwner->Clear();
  myMapOfOwner->Assign (anOwnersToKeep);
  mylastindex = myMapOfOwner->FindIndex (aLastPicked);
  if (!IsValidIndex (mylastindex))
  {
    myMainPM->ClearImmediateDraw();
  }

  if (!isAISRemainsDetected)
  {
    // Remove the interactive object from detected sequences
    for (Standard_Integer anIdx = 1; anIdx <= myAISDetectedSeq.Length(); ++anIdx)
    {
      Handle(AIS_InteractiveObject) aDetectedIO = myAISDetectedSeq.Value (anIdx);
      if (aDetectedIO.IsNull() || aDetectedIO != theIO)
      {
        continue;
      }

      myAISDetectedSeq.Remove (anIdx--);

      if (anIdx < myAISCurDetected)
      {
        myAISCurDetected--;
      }
    }
    myAISCurDetected = Max (myAISCurDetected, 1);
  }
}

//=======================================================================
//function : SetSelected
//purpose  : 
//=======================================================================
void AIS_LocalContext::SetSelected(const Handle(AIS_InteractiveObject)& anIObj,
				   const Standard_Boolean updateviewer)
{
  if(!IsValidForSelection(anIObj)) return;
  UnhilightPicked(Standard_False);
  
  //1st case, owner already <anIObj> as owner  
  // and not separated is found...

  Handle(AIS_Selection) sel = AIS_Selection::Selection(mySelName.ToCString());
  //Standard_Boolean found(Standard_False);
  Handle(Standard_Transient) Tr;
  Handle(SelectMgr_EntityOwner) EO = FindSelectedOwnerFromIO(anIObj);
  if(EO.IsNull()){
    //check if in selection number 0 there is an owner that can be triturated...
    if(anIObj->HasSelection(0)){
      const Handle(SelectMgr_Selection)& SIOBJ = anIObj->Selection(0);
      SIOBJ->Init();
      if(SIOBJ->More()){
        Handle(SelectBasics_EntityOwner) BO = SIOBJ->Sensitive()->BaseSensitive()->OwnerId();
	EO = Handle(SelectMgr_EntityOwner)::DownCast (BO);
      }
    }
    if(EO.IsNull()) 
      EO = new SelectMgr_EntityOwner((const Handle(SelectMgr_SelectableObject)&)anIObj);
  }
  
  ClearSelected(Standard_False);

  AIS_Selection::Select(EO);
  EO->SetSelected (Standard_True);

  HilightPicked(updateviewer);
}

//=======================================================================
//function : AddOrRemoveSelected
//purpose  : 
//=======================================================================

void AIS_LocalContext::AddOrRemoveSelected(const Handle(AIS_InteractiveObject)& anIObj,
					   const Standard_Boolean updateviewer)
{
  if(!IsValidForSelection(anIObj)) return;
  UnhilightPicked(Standard_False);
  // first check if it is selected...
  Handle(SelectMgr_EntityOwner) EO;

  EO = FindSelectedOwnerFromIO(anIObj);

  if (EO.IsNull())
  {
    if(anIObj->HasSelection(0))
    {
      const Handle(SelectMgr_Selection)& SIOBJ = anIObj->Selection(0);
      SIOBJ->Init();
      if(SIOBJ->More()){
        Handle(SelectBasics_EntityOwner) BO = SIOBJ->Sensitive()->BaseSensitive()->OwnerId();
	EO = Handle(SelectMgr_EntityOwner)::DownCast (BO);
      }
    }
    if(EO.IsNull())
    {
      EO = new SelectMgr_EntityOwner((const Handle(SelectMgr_SelectableObject)&)anIObj);
    }
  }
  
//  cout<<"AIS_LocalContext::AddOrRemoveSelected : Selection = "<<mySelName<<endl;
  const Handle(AIS_Selection)& S = AIS_Selection::Selection(mySelName.ToCString());
  
  if (!S.IsNull())
  {
    AIS_SelectStatus aStatus = S->Select(EO);
    EO->SetSelected (aStatus == AIS_SS_Added);
  }

  HilightPicked(updateviewer);
}

//=======================================================================
//function : AddOrRemoveSelected
//purpose  : To check...
//=======================================================================
void AIS_LocalContext::AddOrRemoveSelected(const TopoDS_Shape& Sh,
					   const Standard_Boolean updateviewer)
{     
  UnhilightPicked (Standard_False);
  Handle(SelectMgr_EntityOwner) EO = FindSelectedOwnerFromShape(Sh);
  if (!EO.IsNull())
  {
    AIS_Selection::Selection(mySelName.ToCString())->Select(EO);
    EO->SetSelected (Standard_True);
  }
  HilightPicked (updateviewer);
}

void AIS_LocalContext::AddOrRemoveSelected (const Handle(SelectMgr_EntityOwner)& theOwner,
					   const Standard_Boolean toUpdateViewer)
{
  if(myAutoHilight)
  {
    UnhilightPicked (Standard_False);
  }

  Standard_Boolean toSelect = theOwner->IsSelected() ? Standard_False : Standard_True;

  AIS_Selection::Selection (mySelName.ToCString())->Select (theOwner);
  theOwner->SetSelected (toSelect);

  if(myAutoHilight)
  {
    HilightPicked (toUpdateViewer);
  }
}

//==================================================
// Function: manageDetected
// Purpose :
//==================================================
void AIS_LocalContext::manageDetected (const Handle(SelectMgr_EntityOwner)& thePickOwner,
                                       const Handle(V3d_View)&              theView,
                                       const Standard_Boolean               theToRedrawImmediate)
{
  if (thePickOwner.IsNull())
  {
    myMainPM->ClearImmediateDraw();
    if (theToRedrawImmediate)
    {
      theView->RedrawImmediate();
    }
    return;
  }

  if (!myFilters->IsOk (thePickOwner))
  {
    if (mylastindex != 0)
    {
      mylastgood = mylastindex;
    }
    if (theToRedrawImmediate)
    {
      theView->RedrawImmediate();
    }
    return;
  }

  //=======================================================================================================
  // 2 cases : a- object is in the map of picks:
  //             1. this is the same index as the last detected: -> Do nothing
  //             2. otherwise :
  //                  - if lastindex = 0 (no object was detected at the last step)
  //                    the object presentation is highlighted and lastindex = index(objet)
  //                  - othrwise :
  //                           the presentation of the object corresponding to lastindex is "unhighlighted"
  //                           it is removed if the object is not visualized but only active
  //                           then the presentation of the detected object is highlighted and lastindex = index(objet)
  //         b- the object is not in the map of picked objects
  //                  - if lastindex != 0 (object detected at the last step) it is unhighlighted ...
  //            if the object was decomposed, presentation is created for the detected shape and the couple
  //             (Proprietaire,Prs)is added in the map.
  //           otherwise the couple(proprietaire, NullPrs) is placed in the map and the interactive object
  //           itself is highlighted.
  //
  //=======================================================================================================

  const Standard_Integer aNewIndex = myMapOfOwner->Contains  (thePickOwner)
                                   ? myMapOfOwner->FindIndex (thePickOwner)
                                   : myMapOfOwner->Add       (thePickOwner);

  // For the advanced mesh selection mode the owner indices comparison
  // is not effective because in that case only one owner manage the
  // selection in current selection mode. It is necessary to check the current detected
  // entity and hilight it only if the detected entity is not the same as
  // previous detected (IsForcedHilight call)
  if (aNewIndex != mylastindex
   || thePickOwner->IsForcedHilight())
  {
    myMainPM->ClearImmediateDraw();
    if (mylastindex != 0
     && mylastindex <= myMapOfOwner->Extent())
    {
      const Handle(SelectMgr_EntityOwner)& aLastOwner = myMapOfOwner->FindKey (mylastindex);
      Unhilight (aLastOwner, theView);
    }

    if (myAutoHilight)
    {
      if (!thePickOwner->IsSelected() || myCTX->ToHilightSelected())
      {
        Hilight (thePickOwner, theView);
      }
      if (theToRedrawImmediate)
      {
        theView->RedrawImmediate();
      }
    }

    mylastindex = aNewIndex;
  }

  if (mylastindex != 0)
  {
    mylastgood = mylastindex;
  }
}

//=======================================================================
//function : HasDetectedShape
//purpose  : 
//=======================================================================

Standard_Boolean AIS_LocalContext::HasDetectedShape() const 
{
  if(mylastindex==0) return Standard_False;
  return IsShape(mylastindex);
}

//=======================================================================
//function : DetectedShape
//purpose  : 
//=======================================================================

const TopoDS_Shape&
AIS_LocalContext::DetectedShape() const
{
  static TopoDS_Shape bidsh;
  if(mylastindex != 0)
  {
    Handle(StdSelect_BRepOwner) BROwnr = Handle(StdSelect_BRepOwner)::DownCast(myMapOfOwner->FindKey (mylastindex));
    if(BROwnr.IsNull()) return bidsh;
    return BROwnr->Shape();
  }
  return bidsh;
}					    

//=======================================================================
//function : DetectedInteractive
//purpose  : 
//=======================================================================

Handle(AIS_InteractiveObject) 
AIS_LocalContext::DetectedInteractive() const 
{
  Handle(AIS_InteractiveObject) Iobj;
  if(IsValidIndex(mylastindex)){
    Handle(SelectMgr_SelectableObject) SO = myMapOfOwner->FindKey(mylastindex)->Selectable();
    Iobj = Handle(AIS_InteractiveObject)::DownCast (SO);
  }
  return Iobj;
}
//=======================================================================
//function : DetectedInteractive
//purpose  : 
//=======================================================================
Handle(SelectMgr_EntityOwner) AIS_LocalContext::DetectedOwner() const 
{
  Handle(SelectMgr_EntityOwner) bid;
  if(!IsValidIndex(mylastindex)) return bid;
  return myMapOfOwner->FindKey(mylastindex);
}


//=======================================================================
//function : ComesFromDecomposition
//purpose  : 
//=======================================================================

Standard_Boolean AIS_LocalContext::ComesFromDecomposition(const Standard_Integer PickedIndex) const 
{
  const Handle(SelectMgr_EntityOwner)& OWN = myMapOfOwner->FindKey(PickedIndex);
  Handle(SelectMgr_SelectableObject) aSel  = OWN->Selectable();
  if (myActiveObjects.IsBound (aSel)) { // debug of jmi
    const Handle(AIS_LocalStatus)& Stat      = myActiveObjects(aSel);    
    return Stat->Decomposed();
  }
  return Standard_False;
}

//=======================================================================
//function : DisplaySensitive
//purpose  : 
//=======================================================================

void AIS_LocalContext::DisplaySensitive(const Handle(V3d_View)& aviou)
{
    myMainVS->DisplaySensitive(aviou);
}

//=======================================================================
//function : ClearSensitive
//purpose  : 
//=======================================================================

void AIS_LocalContext::ClearSensitive(const Handle(V3d_View)& aviou)
{
    myMainVS->ClearSensitive(aviou);
}


//=======================================================================
//function : IsShape
//purpose  : 
//=======================================================================
Standard_Boolean AIS_LocalContext::IsShape(const Standard_Integer Index) const
{
  Handle(SelectMgr_EntityOwner) aEO (myMapOfOwner->FindKey(Index));
  if (aEO.IsNull() || ! aEO->IsKind(STANDARD_TYPE(StdSelect_BRepOwner)))
    return Standard_False;
  return 
    ComesFromDecomposition(Index);
}

Standard_Boolean AIS_LocalContext::IsValidForSelection(const Handle(AIS_InteractiveObject)& anIObj) const 
{

  // Shape was not transfered from AIS_Shape to EntityOwner
  Handle(AIS_Shape) shape = Handle(AIS_Shape)::DownCast(anIObj);
  if( !shape.IsNull() ) 
    return myFilters->IsOk(new StdSelect_BRepOwner(shape->Shape(),shape));
  return myFilters->IsOk(new SelectMgr_EntityOwner((const Handle(SelectMgr_SelectableObject)&)anIObj));
}


//=======================================================================
//function : HilightNextDetected
//purpose  :
//=======================================================================
Standard_Integer AIS_LocalContext::HilightNextDetected (const Handle(V3d_View)& theView,
                                                        const Standard_Boolean  theToRedrawImmediate)
{
  // go to the next owner
  if (myDetectedSeq.IsEmpty())
  {
    return 0;
  }

  const Standard_Integer aLen = myDetectedSeq.Length();
  if (++myCurDetected > aLen)
  {
    myCurDetected = 1;
  }
  Handle(SelectMgr_EntityOwner) anOwner = myMainVS->Picked (myDetectedSeq (myCurDetected));
  if (anOwner.IsNull())
  {
    return 0;
  }
  manageDetected (anOwner, theView, theToRedrawImmediate);
  return myCurDetected;
}

//=======================================================================
//function : HilightPreviousDetected
//purpose  :
//=======================================================================
Standard_Integer AIS_LocalContext::HilightPreviousDetected (const Handle(V3d_View)& theView,
                                                            const Standard_Boolean  theToRedrawImmediate)
{
  if (myDetectedSeq.IsEmpty())
  {
    return 0;
  }

  const Standard_Integer aLen = myDetectedSeq.Length();
  if (--myCurDetected < 1)
  {
    myCurDetected = aLen;
  }
  Handle(SelectMgr_EntityOwner) anOwner = myMainVS->Picked (myDetectedSeq (myCurDetected));
  if (anOwner.IsNull())
  {
    return 0;
  }

  manageDetected (anOwner, theView, theToRedrawImmediate);
  return myCurDetected;
}

//=======================================================================
//function : UnhilightLastDetected
//purpose  :
//=======================================================================
Standard_Boolean AIS_LocalContext::UnhilightLastDetected (const Handle(V3d_View)& theView)
{
  if (!IsValidIndex (mylastindex))
  {
    return Standard_False;
  }

  myMainPM->BeginImmediateDraw();
  const Handle(SelectMgr_EntityOwner)& anOwner = myMapOfOwner->FindKey (mylastindex);
  const Standard_Integer aHilightMode = anOwner->HasSelectable()
                                      ? GetHiMod (Handle(AIS_InteractiveObject)::DownCast (anOwner->Selectable()))
                                      : 0;

  myMapOfOwner->FindKey (mylastindex)->Unhilight (myMainPM, aHilightMode);
  myMainPM->EndImmediateDraw (theView->Viewer());
  mylastindex = 0;
  return Standard_True;
}

//=======================================================================
//function : FindSelectedOwnerFromIO
//purpose  : it is checked if one of the selected owners really presents IObj
//=======================================================================
Handle(SelectMgr_EntityOwner) AIS_LocalContext::FindSelectedOwnerFromIO
                          (const Handle(AIS_InteractiveObject)& anIObj) const 
{
  Handle(SelectMgr_EntityOwner) EO,bid;
  if (anIObj.IsNull()) return EO;
  
  Handle(AIS_Selection) Sel = AIS_Selection::Selection(mySelName.ToCString());
  if(Sel.IsNull()) {
#ifdef OCCT_DEBUG
    cout<<"\t\tAIS_LocalCOntext::FindSelectedOwnerFromShape : Selection "
        <<mySelName<<" Nulle "<<endl;
#endif
    return EO;
  }
  Standard_Boolean found(Standard_False);
  const AIS_NListTransient& Obj = Sel->Objects();
  AIS_NListTransient::Iterator anIter( Obj );
  for(; anIter.More(); anIter.Next()){
    const Handle(Standard_Transient)& Tr = anIter.Value();
    if(!Tr.IsNull()){
      EO = Handle(SelectMgr_EntityOwner)::DownCast (Tr);
      if(EO->HasSelectable()){
	Handle(StdSelect_BRepOwner) BROwnr = Handle(StdSelect_BRepOwner)::DownCast(EO);
	if(BROwnr.IsNull() || !BROwnr->ComesFromDecomposition()){
	  if (anIObj == EO->Selectable()){
	    found =Standard_True;
	    break;
	  }
	}
      }
    }
  }
  if(found)  return EO;
  return bid;
}

//=======================================================================
//function : FindSelectedOwnerFromShape
//purpose  : it is checked if one of the selected owners really presents IObj
//=======================================================================
Handle(SelectMgr_EntityOwner) AIS_LocalContext::FindSelectedOwnerFromShape(const TopoDS_Shape& sh) const 
{
  Handle(SelectMgr_EntityOwner) EO, bid;
  if (sh.IsNull()) return EO;
  
  Handle(AIS_Selection) Sel = AIS_Selection::Selection(mySelName.ToCString());
  if(Sel.IsNull()) {
#ifdef OCCT_DEBUG
    cout<<"\t\tAIS_LocalCOntext::FindSelectedOwnerFromShape : Selection "<<mySelName<<" Nulle "<<endl;
#endif
    return EO;
  }
  
  Standard_Boolean found(Standard_False);

  if (!found) {
    NCollection_List<Handle(SelectBasics_EntityOwner)> anActiveOwners;
    myMainVS->ActiveOwners (anActiveOwners);
    for (NCollection_List<Handle(SelectBasics_EntityOwner)>::Iterator anOwnersIt (anActiveOwners); anOwnersIt.More(); anOwnersIt.Next())
    {
      EO = Handle(SelectMgr_EntityOwner)::DownCast (anOwnersIt.Value());
      Handle(StdSelect_BRepOwner) BROwnr = Handle(StdSelect_BRepOwner)::DownCast(EO);
      if (!BROwnr.IsNull() && BROwnr->HasShape() && BROwnr->Shape() == sh) {
	 found = Standard_True;
	 break;
      }
    }
  }

  if(found)  return EO;
  return bid;
}

//=======================================================================
//function : AIS_LocalContext::InitDetected
//purpose  :
//=======================================================================
void AIS_LocalContext::InitDetected()
{
  myAISCurDetected = myAISDetectedSeq.Length()? 1 : 0;
}

//=======================================================================
//function : AIS_LocalContext::MoreDetected
//purpose  :
//=======================================================================
Standard_Boolean AIS_LocalContext::MoreDetected() const
{
  return (myAISCurDetected > 0 && myAISCurDetected <= myAISDetectedSeq.Length());
}

//=======================================================================
//function : AIS_LocalContext::NextDetected
//purpose  :
//=======================================================================
void AIS_LocalContext::NextDetected()
{
  myAISCurDetected++;
}

//=======================================================================
//function : DetectedCurrentShape
//purpose  :
//=======================================================================
const TopoDS_Shape& AIS_LocalContext::DetectedCurrentShape() const
{
  static TopoDS_Shape aDummyShape;

  Handle(AIS_Shape) aCurrentShape = Handle(AIS_Shape)::DownCast (DetectedCurrentObject());

  if (aCurrentShape.IsNull())
  {
    return aDummyShape;
  }

  return aCurrentShape->Shape();
}
//=======================================================================
//function : DetectedCurrentObject
//purpose  :
//=======================================================================
Handle(AIS_InteractiveObject) AIS_LocalContext::DetectedCurrentObject() const
{
  return MoreDetected() ? myAISDetectedSeq(myAISCurDetected) : NULL;
}
