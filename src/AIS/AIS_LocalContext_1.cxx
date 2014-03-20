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

#define BUC60569 	//GG_051199 Enable to select the local context 
//			in any case and especially in multi selection mode.
//			Note that right now when an hilighted owner is selected
//			this owner is unhilighted,this permits to see the selection!
//		 	Principle : an owner can have 3 state :
//			1 : The owner is selected and no more highlightable
//			0 : The owner is NOT selected
//			-1: The owner is selected but stay highlightable (NEW)

// IMP230600	//GG Add protection on selection methodes
//			when nothing is selected

#define BUC60726        //GG_040900 When nothing is detected, 
//			Clear the last temporary stuff in any case

#define BUC60765	//GG_121000 Avoid to raise when the same selection 
//			is attached to several local context.

#define BUC60771	//GG_261000	Avoid to crash after closing a view
//			containing a selected entity and creating a new one.

#define BUC60774	//GG_261000	Returns right select status on
//			bounding-box selection type.

#define BUC60818	//GG_300101	Enable detection even if
//			SetAutomaticHilight(FALSE) has been used.

#define IMP300101       //GG Enable to use polygon highlighting

#define BUC60876	//GG_050401 Clear selection always even
//			if the current highlight mode is not 0.

#define BUC60953        //SAV_060701 For Select optimization. Selection by rectangle case.
// for single selection no optimization done.

#define IMP120701	//SZV made a shape valid for selection
//			when required.

#define IMP160701       //SZV Add InitDetected(),MoreDetected(),NextDetected(),
//                       DetectedCurrentShape(),DetectedCurrentObject()
//                       methods

#define OCC138          //VTN Avoding infinit loop in AddOrRemoveSelected method.

#define OCC189          //SAV: 18/03/02 AIS_Selection::Objects() returns ListOfTransient
// instead of array.

#define USE_MAP         //san : 18/04/03 USE_MAP - additional datamap is used to speed up access 
//to certain owners in AIS_Selection::myresult list  

#define OCC9026		//AEL Performance optimization of the FindSelectedOwnerFromShape() method.

#include <AIS_LocalContext.jxx>
#include <StdSelect_BRepOwner.hxx>
#include <TColStd_MapOfTransient.hxx>
#include <TColStd_MapIteratorOfMapOfTransient.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <AIS_LocalStatus.hxx>
#include <StdPrs_WFShape.hxx>
#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Graphic3d_Group.hxx>
#include <Select3D_SensitiveTriangulation.hxx>
#include <SelectBasics_SensitiveEntity.hxx>
#include <TCollection_AsciiString.hxx>
#ifdef OCC9026
#include <SelectMgr_DataMapIteratorOfDataMapOfIntegerSensitive.hxx>
#endif
#include <SelectMgr_Selection.hxx>
#include <OSD_Environment.hxx>
#include <SelectMgr_DataMapOfObjectOwners.hxx>

#include <Geom_Transformation.hxx>
#include <AIS_Selection.hxx>
#include <Aspect_Grid.hxx>
#ifdef IMP120701
#include <AIS_Shape.hxx>
#endif


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

    myDetectedSeq.Append (aDetIter); // normallly they are already arranged in correct order...
    Handle(AIS_InteractiveObject) anObj = Handle(AIS_InteractiveObject)::DownCast (anOwner->Selectable());
    if (!Handle(AIS_Shape)::DownCast (anObj).IsNull())
    {
      myAISDetectedSeq.Append (anObj);
    }
  }

  // result of courses..
  if (aDetectedNb == 0 || myDetectedSeq.IsEmpty())
  {
    if (mylastindex != 0 && mylastindex <= myMapOfOwner.Extent())
    {
      Unhilight (myMapOfOwner (mylastindex), theView);
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

//==================================================
// Function: 
// Purpose :
//==================================================
AIS_StatusOfPick AIS_LocalContext::Select(const Standard_Boolean updateviewer)
{
  if(myAutoHilight)
    UnhilightPicked(Standard_False);
  
  Standard_Integer DI = DetectedIndex();
  AIS_Selection::SetCurrentSelection(mySelName.ToCString());
  Standard_Integer NbSel = AIS_Selection::Extent();

  if(DI<=0){
    ClearSelected(updateviewer);
    return NbSel== 0 ? AIS_SOP_NothingSelected : AIS_SOP_Removed;
  }

  const Handle(SelectMgr_EntityOwner)& EO = myMapOfOwner(DI);

#ifdef BUC60569
  ClearSelected(Standard_False);
  Standard_Integer state = EO->State();
  if( state < 1 ){
    EO->State(1);
    if( state == 0 ) AIS_Selection::Select(EO);
  }
#else
  if(!IsSelected(EO))
    AIS_Selection::ClearAndSelect(EO);
#endif
    
  if (myAutoHilight)
  {
    const Handle(V3d_Viewer)& aViewer = myCTX->CurrentViewer();
    for (aViewer->InitActiveViews(); aViewer->MoreActiveViews(); aViewer->NextActiveViews())
    {
      Unhilight (EO, aViewer->ActiveView());
    }

    // advanced selection highlighting mechanism
    if (!EO->IsAutoHilight() && EO->HasSelectable())
    {
      Handle(AIS_InteractiveObject) anIO = Handle(AIS_InteractiveObject)::DownCast(EO->Selectable());
      UpdateSelected (anIO, Standard_False);
    }

    if (updateviewer)
    {
      myCTX->CurrentViewer()->Update();
    }
  }
  return ( AIS_Selection::Extent() == 1)? AIS_SOP_OneSelected : AIS_SOP_SeveralSelected ;
}
//==================================================
// Function: 
// Purpose :
//==================================================
AIS_StatusOfPick AIS_LocalContext::Select(const Standard_Integer XPMin,
					  const Standard_Integer YPMin,
					  const Standard_Integer XPMax,
					  const Standard_Integer YPMax, 
					  const Handle(V3d_View)& aView,
					  const Standard_Boolean updateviewer)
{
  if(aView->Viewer()== myCTX->CurrentViewer()){
    myMainVS->Pick( XPMin,YPMin,XPMax,YPMax,aView);
    if (myAutoHilight) UnhilightPicked(Standard_False);
    
    AIS_Selection::SetCurrentSelection(mySelName.ToCString());
    Standard_Integer LastExt = AIS_Selection::Extent();
    
    myMainVS->Init();
    if(!myMainVS->More()) {
      ClearSelected(updateviewer);
      mylastindex=0;
      return LastExt == 0 ? AIS_SOP_NothingSelected:AIS_SOP_Removed;
    }

    ClearSelected(Standard_False);
   
    for(myMainVS->Init();myMainVS->More();myMainVS->Next()){
      const Handle(SelectMgr_EntityOwner)& OWNR = myMainVS->Picked();
      if(myFilters->IsOk(OWNR)){
	// it can be helpfil to classify this owner immediately...
#ifdef BUC60569
	  Standard_Integer state = OWNR->State();
	  if( state < 1 ){
#ifdef BUC60953
            if( state == 0 ) AIS_Selection::Select(OWNR);
            OWNR->State(1);
#else
            OWNR->State(1);
            if( state == 0 ) AIS_Selection::Select(OWNR);
#endif //BUC60953
	  }
#else //BUC60569
	  if(!IsSelected(OWNR)){
	    OWNR->State(1);
	    AIS_Selection::Select(OWNR);
	  }
#endif //BUC60569
      }
    }
    if (myAutoHilight) 
      HilightPicked(updateviewer);
  }
#ifdef BUC60774
  Standard_Integer NS = AIS_Selection::Extent();
  if( NS == 1 ) return AIS_SOP_OneSelected;
  else if( NS > 1 ) return AIS_SOP_SeveralSelected;
#endif
  return AIS_SOP_Error;
}



//==================================================
// Function: 
// Purpose :
//==================================================
AIS_StatusOfPick AIS_LocalContext::ShiftSelect(const Standard_Boolean updateviewer)
{
  Standard_Integer I = DetectedIndex();
  if(I>0){
    
    AIS_Selection::SetCurrentSelection(mySelName.ToCString());
#ifdef BUC60774
    Standard_Integer NbSel = AIS_Selection::Extent();
#endif
    const Handle(SelectMgr_EntityOwner)& EO  = myMapOfOwner(I);
#ifdef BUC60569
    Standard_Integer mod = EO->State()==0 ? -1 : 0;
#else
    Standard_Integer mod = EO->State()==0 ? 1 : 0;
#endif

#ifdef BUC60953
    AIS_Selection::Select(EO);
    EO->State(mod);
#else
    EO->State(mod);

    AIS_Selection::Select(EO);
#endif
    
    if(myAutoHilight)
    {
      const Handle(V3d_Viewer)& aViewer = myCTX->CurrentViewer();
      for (aViewer->InitActiveViews(); aViewer->MoreActiveViews(); aViewer->NextActiveViews())
      {
        Unhilight (EO, aViewer->ActiveView());
      }

      // advanced selection highlighting mechanism
      if (!EO->IsAutoHilight() && EO->HasSelectable())
      {
        Handle(AIS_InteractiveObject) anIO = Handle(AIS_InteractiveObject)::DownCast (EO->Selectable());
        UpdateSelected (anIO, Standard_False);
      }

      if (updateviewer)
      {
        myCTX->CurrentViewer()->Update();
      }
    } 
#ifdef BUC60774
    Standard_Integer NS = AIS_Selection::Extent();
    if( NS == 1 ) return AIS_SOP_OneSelected;
    else if( NS > 1 ) return AIS_SOP_SeveralSelected;
    return NbSel== 0 ? AIS_SOP_NothingSelected : AIS_SOP_Removed;
#endif
  }
  return AIS_SOP_Error;
}
//==================================================
// Function: the already selected objects are unselected
// Purpose : others are selected.
//==================================================
AIS_StatusOfPick AIS_LocalContext::ShiftSelect(const Standard_Integer XPMin,
				   const Standard_Integer YPMin,
				   const Standard_Integer XPMax,
				   const Standard_Integer YPMax, 
				   const Handle(V3d_View)& aView,
				   const Standard_Boolean updateviewer)
{
  myMainPM->ClearImmediateDraw();

  if(aView->Viewer()== myCTX->CurrentViewer()) {
    myMainVS->Pick( XPMin,YPMin,XPMax,YPMax,aView);
#ifdef BUC60774
    AIS_Selection::SetCurrentSelection(mySelName.ToCString());
    Standard_Integer LastExt = AIS_Selection::Extent();
#endif
    myMainVS->Init();
    if(!myMainVS->More()) 
#ifdef BUC60774
      return LastExt == 0 ? AIS_SOP_NothingSelected:AIS_SOP_Removed;
#else
      return AIS_SOP_NothingSelected; // no effet if click on empty space
#endif

    AIS_Selection::SetCurrentSelection(mySelName.ToCString());
    if (myAutoHilight) UnhilightPicked(Standard_False);
    
    for(myMainVS->Init();myMainVS->More();myMainVS->Next()){
      const Handle(SelectMgr_EntityOwner)& EO = myMainVS->Picked();
      if(myFilters->IsOk(EO)){
#ifdef BUC60569
        Standard_Integer mod = EO->State()==0 ? -1 : 0;
#else
	Standard_Integer mod = EO->State()==0 ? 1 : 0;
#endif

#ifdef BUC60953
	AIS_Selection::Select(EO);
	EO->State(mod);
#else
	EO->State(mod);
	AIS_Selection::Select(EO);
#endif
      }
    }
    if (myAutoHilight) HilightPicked(updateviewer);
    
  }
#ifdef BUC60774
  Standard_Integer NS = AIS_Selection::Extent();
  if( NS == 1 ) return AIS_SOP_OneSelected;
  else if( NS > 1 ) return AIS_SOP_SeveralSelected;
#endif
  return AIS_SOP_Error;
}

//==================================================
// Function: Select
// Purpose : Selection by polyline
//==================================================
AIS_StatusOfPick AIS_LocalContext::Select(const TColgp_Array1OfPnt2d& aPolyline,
					  const Handle(V3d_View)& aView,
					  const Standard_Boolean updateviewer)
{
  if(aView->Viewer()== myCTX->CurrentViewer()){
    myMainVS->Pick(aPolyline,aView);
    if (myAutoHilight) UnhilightPicked(Standard_False);
    
    AIS_Selection::SetCurrentSelection(mySelName.ToCString());
    Standard_Integer LastExt = AIS_Selection::Extent();
    
    myMainVS->Init();
    if(!myMainVS->More()) {
      ClearSelected(updateviewer);
      mylastindex=0;
      return LastExt == 0 ? AIS_SOP_NothingSelected:AIS_SOP_Removed;
    }

    ClearSelected(Standard_False);

    for(myMainVS->Init();myMainVS->More();myMainVS->Next()){
      const Handle(SelectMgr_EntityOwner)& OWNR = myMainVS->Picked();
      if(myFilters->IsOk(OWNR)){
	// it can be helpfil to classify this owner immediately...
#ifdef BUC60953
        Standard_Integer state = OWNR->State();
        if( state < 1 ){
          if( state == 0 ) AIS_Selection::AddSelect(OWNR);
          OWNR->State(1);
        }
#else 
	if(!IsSelected(OWNR)){
	  OWNR->State(1);
	  AIS_Selection::AddSelect(OWNR);
	}
#endif //BUC60953
      }
    }

    if (myAutoHilight) 
      HilightPicked(updateviewer);
  }
#ifdef BUC60774
  Standard_Integer NS = AIS_Selection::Extent();
  if( NS == 1 ) return AIS_SOP_OneSelected;
  else if( NS > 1 ) return AIS_SOP_SeveralSelected;
#endif
  return AIS_SOP_Error;
}

//==================================================
// Function: Select
// Purpose : Selection by polyline
//==================================================
AIS_StatusOfPick AIS_LocalContext::ShiftSelect( const TColgp_Array1OfPnt2d& aPolyline,
                                                const Handle(V3d_View)& aView,
                                                const Standard_Boolean updateviewer )
{
    if( aView->Viewer() == myCTX->CurrentViewer() ) {
        myMainVS->Pick( aPolyline, aView );
        
        AIS_Selection::SetCurrentSelection( mySelName.ToCString() );
        Standard_Integer LastExt = AIS_Selection::Extent();
        myMainVS->Init();
        if( !myMainVS->More() ) 
            return LastExt == 0 ? AIS_SOP_NothingSelected : AIS_SOP_Removed;
        
        AIS_Selection::SetCurrentSelection( mySelName.ToCString() );
        
        if ( myAutoHilight )
            UnhilightPicked( Standard_False );
        for( myMainVS->Init(); myMainVS->More(); myMainVS->Next() ) {
            const Handle(SelectMgr_EntityOwner)& EO = myMainVS->Picked();
            if( myFilters->IsOk( EO ) ) {
                Standard_Integer mod = EO->State() == 0 ? -1 : 0;
                AIS_Selection::Select(EO);
                EO->State( mod );
            }
        }
        if ( myAutoHilight )
            HilightPicked( updateviewer );
    }
    Standard_Integer NS = AIS_Selection::Extent();
    if( NS == 1 ) 
        return AIS_SOP_OneSelected;
    else if( NS > 1 ) 
        return AIS_SOP_SeveralSelected;
    return AIS_SOP_Error;
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
  myMainPM->EndImmediateDraw (theView);
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

  myMainPM->ClearImmediateDraw();
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
  Standard_Boolean updMain(Standard_False);

  Handle(AIS_Selection) Sel = AIS_Selection::Selection(mySelName.ToCString());
#ifdef BUC60765
  if( Sel.IsNull() ) return;
#endif

  Handle (PrsMgr_PresentationManager3d) PM = myMainPM;
  SelectMgr_DataMapOfObjectOwners aMap;
  
  // to avoid problems when there is a loop searching for selected objects...
#if !defined OCC189 && !defined USE_MAP
  const TColStd_Array1OfTransient& Obj = Sel->Objects()->Array1();
  for(Standard_Integer i =Obj.Lower();i<=Sel->NbStored();i++)
  {
    const Handle(Standard_Transient)& Tr = Obj(i);
#else
  const AIS_NListTransient& Obj = Sel->Objects();
  AIS_NListTransient::Iterator anIter( Obj );
  for(; anIter.More(); anIter.Next())
  {
    const Handle(Standard_Transient)& Tr = anIter.Value();
#endif
    if(!Tr.IsNull()){
      const Handle(SelectMgr_EntityOwner)& Ownr =
        *((const Handle(SelectMgr_EntityOwner)*) &Tr);
      Handle(AIS_InteractiveObject) IO;
      if(Ownr->HasSelectable()){
	Handle(StdSelect_BRepOwner) BROwnr = Handle(StdSelect_BRepOwner)::DownCast(Ownr);
	if(BROwnr.IsNull() || !BROwnr->ComesFromDecomposition()){
	  Handle(SelectMgr_SelectableObject) SO  = Ownr->Selectable();
	  IO = *((Handle(AIS_InteractiveObject)*)&SO);
	  updMain = Standard_True;
	}
	else
	  updMain = Standard_True;
      }
      else
	updMain = Standard_True;
      Handle(SelectMgr_SelectableObject) SO = Ownr->Selectable();
      Standard_Integer HM = GetHiMod(*((Handle(AIS_InteractiveObject)*)&SO));
      if ( Ownr->IsAutoHilight() )
        Ownr->HilightWithColor(PM,myCTX->SelectionColor(),HM);
      else if ( aMap.IsBound (SO) )
        aMap.ChangeFind(SO).Append ( Ownr );        
      else {
        SelectMgr_SequenceOfOwner aSeq;
        aSeq.Append ( Ownr );
        aMap.Bind ( SO, aSeq );
      }      
    }
  }

  for ( SelectMgr_DataMapIteratorOfMapOfObjectOwners aMapIter(aMap); 
        aMapIter.More(); aMapIter.Next() )
    aMapIter.Key()->HilightSelected ( myMainPM, aMapIter.Value() );

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

  Standard_Boolean updMain(Standard_False);

  Handle(AIS_Selection) Sel = AIS_Selection::Selection(mySelName.ToCString());
#ifdef BUC60765
  if( Sel.IsNull() ) return;
#endif
  Handle (PrsMgr_PresentationManager3d) PM = myMainPM;
  SelectMgr_DataMapOfObjectOwners anObjMap;
  SelectMgr_SequenceOfOwner anOwnSeq;
  
#if !defined OCC189 && !defined USE_MAP  
  const TColStd_Array1OfTransient& Obj = Sel->Objects()->Array1();
  for(Standard_Integer i =Obj.Lower();i<=Sel->NbStored();i++){
    const Handle(Standard_Transient)& Tr = Obj(i);
#else
  const AIS_NListTransient& Obj = Sel->Objects();
  AIS_NListTransient::Iterator anIter( Obj );
  for(; anIter.More(); anIter.Next()){
    const Handle(Standard_Transient)& Tr = anIter.Value();
#endif
    if(!Tr.IsNull()){
      const Handle(SelectMgr_EntityOwner)& Ownr =
        *((const Handle(SelectMgr_EntityOwner)*) &Tr);
      Standard_Integer HM(0);
      if(Ownr->HasSelectable()){
#ifdef BUC60876
	Handle(SelectMgr_SelectableObject) SO  = Ownr->Selectable();
	Handle(AIS_InteractiveObject) IO = *((Handle(AIS_InteractiveObject)*)&SO);
  anObjMap.Bind ( IO, anOwnSeq );

  HM = GetHiMod(IO);
#endif
	Handle(StdSelect_BRepOwner) BROwnr = Handle(StdSelect_BRepOwner)::DownCast(Ownr);
	if(BROwnr.IsNull() || !BROwnr->ComesFromDecomposition()){
#ifndef BUC60876
	  Handle(SelectMgr_SelectableObject) SO  = Ownr->Selectable();
	  Handle(AIS_InteractiveObject) IO = *((Handle(AIS_InteractiveObject)*)&SO);
	  HM = GetHiMod(IO);
#endif
	  updMain = Standard_True;
	}
	else
	  updMain = Standard_True;
      }
      Ownr->Unhilight(PM,HM);
    }
  }
  
  for ( SelectMgr_DataMapIteratorOfMapOfObjectOwners anIter1 ( anObjMap ); 
        anIter1.More(); anIter1.Next() )
    if ( !anIter1.Key()->IsAutoHilight() )
      anIter1.Key()->ClearSelected();

  if(updateviewer){
#ifdef BUC60774
    myCTX->CurrentViewer()->Update();
#else
    if(updMain) myCTX->CurrentViewer()->Update();
#endif
  }
  
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

Standard_Boolean AIS_LocalContext::IsSelected(const Handle(SelectMgr_EntityOwner)& Ownr) const 
{
  if (Ownr.IsNull()) return Standard_False;
#ifdef BUC60569
  Standard_Boolean state = (Ownr->State()!=0);
#else
  Standard_Boolean state = (Ownr->State()==1);
#endif
  return state;
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
  Handle(SelectMgr_EntityOwner) EO = *((Handle(SelectMgr_EntityOwner)*)&Tr);
  Handle(StdSelect_BRepOwner) BRO = Handle(StdSelect_BRepOwner)::DownCast(EO);
  if(BRO.IsNull()) return Standard_False;
  Standard_Boolean hasshape = BRO->HasShape();
  Standard_Boolean comes = BRO->ComesFromDecomposition();
  return (hasshape&&comes);
}

//==================================================
// Function: 
// Purpose :
//==================================================
const TopoDS_Shape& AIS_LocalContext::
SelectedShape() const 
{
  static TopoDS_Shape aSh;
  Handle(Standard_Transient) Tr = AIS_Selection::CurrentSelection()->Value();
  Handle(SelectMgr_EntityOwner) EO = *((Handle(SelectMgr_EntityOwner)*)&Tr);
  Handle(StdSelect_BRepOwner) BRO = Handle(StdSelect_BRepOwner)::DownCast(EO);
  if( BRO.IsNull() ) 
  {
    return aSh;
  }
  return BRO->Shape();
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
    Handle(SelectMgr_EntityOwner) EO = *((Handle(SelectMgr_EntityOwner)*)&Tr);
    Handle(SelectMgr_SelectableObject) SO;
    if(EO->HasSelectable()){
      SO = EO->Selectable();
      IO = *((Handle(AIS_InteractiveObject)*)&SO);
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
  	EO = *((Handle(SelectMgr_EntityOwner)*)&Tr);
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
void AIS_LocalContext::ClearSelected(const Standard_Boolean updateviewer)
{
  UnhilightPicked(updateviewer);
  AIS_Selection::SetCurrentSelection(mySelName.ToCString());

  Handle(AIS_Selection) Sel = AIS_Selection::CurrentSelection();
#if !defined OCC189 && !defined USE_MAP   
  const TColStd_Array1OfTransient& Obj = Sel->Objects()->Array1();
  for(Standard_Integer i =Obj.Lower();i<=Sel->NbStored();i++){
    const Handle(Standard_Transient)& Tr = Obj(i);
#else
  const AIS_NListTransient& Obj = Sel->Objects();
  AIS_NListTransient::Iterator anIter( Obj );
  for(; anIter.More(); anIter.Next()){
    const Handle(Standard_Transient)& Tr = anIter.Value();
#endif
    if(!Tr.IsNull()){
      (*((const Handle(SelectMgr_EntityOwner)*)&Tr))->State(0);
    }
  }
  AIS_Selection::Select();
  mylastindex = 0;
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
	Handle(SelectBasics_EntityOwner) BO = SIOBJ->Sensitive()->OwnerId();
	EO = *((Handle(SelectMgr_EntityOwner)*)&BO);
      }
    }
    if(EO.IsNull()) 
      EO = new SelectMgr_EntityOwner(anIObj);
  }
  
  ClearSelected(Standard_False);
#ifdef OCC138 
  AIS_Selection::Select(EO);
  EO->State(1);
#else
  EO->State(1);
  AIS_Selection::Select(EO);
#endif
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
#ifndef OCC138
  if(!EO.IsNull())
    EO->State(0);
  else{
#else
  if(EO.IsNull()) {
#endif
    if(anIObj->HasSelection(0)){
      const Handle(SelectMgr_Selection)& SIOBJ = anIObj->Selection(0);
      SIOBJ->Init();
      if(SIOBJ->More()){
	Handle(SelectBasics_EntityOwner) BO = SIOBJ->Sensitive()->OwnerId();
	EO = *((Handle(SelectMgr_EntityOwner)*)&BO);
      }
    }
    if(EO.IsNull()) 
      EO = new SelectMgr_EntityOwner(anIObj);
#ifndef OCC138
    EO->State(1);
#endif
  }
  
//  cout<<"AIS_LocalContext::AddOrRemoveSelected : Selection = "<<mySelName<<endl;
  const Handle(AIS_Selection)& S = AIS_Selection::Selection(mySelName.ToCString());
#ifdef OCC138
  if(!S.IsNull()) {
    AIS_SelectStatus aStatus = S->Select(EO);
    if(aStatus == AIS_SS_Added)
      EO->State(1);
    else
      EO->State(0);
  }
#else
  if(!S.IsNull())
    S->Select(EO);
#endif
  HilightPicked(updateviewer);
}

//=======================================================================
//function : AddOrRemoveSelected
//purpose  : To check...
//=======================================================================
void AIS_LocalContext::AddOrRemoveSelected(const TopoDS_Shape& Sh,
					   const Standard_Boolean updateviewer)
{     
  UnhilightPicked(Standard_False);
  Handle(SelectMgr_EntityOwner) EO = FindSelectedOwnerFromShape(Sh);
  if(!EO.IsNull()){
//    cout<<"AIS_LocalContext::AddOrRemoveSelected(sh) : Selection = "<<mySelName<<endl;
    
#ifdef OCC138
    AIS_Selection::Selection(mySelName.ToCString())->Select(EO);
    EO->State(1);
#else
    EO->State(1);
    AIS_Selection::Selection(mySelName.ToCString())->Select(EO);
#endif
  }
  HilightPicked(updateviewer);
}

void AIS_LocalContext::AddOrRemoveSelected(const Handle(SelectMgr_EntityOwner)& Ownr,
					   const Standard_Boolean updateviewer)
{     
  //Not Yet Implemented
  if(myAutoHilight)
  UnhilightPicked(Standard_False);
//  cout<<"AIS_LocalContext::AddOrRemoveSelected(ownr) : Selection = "<<mySelName<<endl;

  Standard_Integer mod = Ownr->State()==0 ? 1 : 0;
#ifdef OCC138
  AIS_Selection::Selection(mySelName.ToCString())->Select(Ownr);

  Ownr->State(mod);  
#else
  Ownr->State(mod);
  
  AIS_Selection::Selection(mySelName.ToCString())->Select(Ownr);
#endif
  if(myAutoHilight)
  HilightPicked(updateviewer);
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

  const Standard_Integer aNewIndex = myMapOfOwner.Contains  (thePickOwner)
                                   ? myMapOfOwner.FindIndex (thePickOwner)
                                   : myMapOfOwner.Add       (thePickOwner);

  // For the advanced mesh selection mode the owner indices comparison
  // is not effective because in that case only one owner manage the
  // selection in current selection mode. It is necessary to check the current detected
  // entity and hilight it only if the detected entity is not the same as
  // previous detected (IsForcedHilight call)
  if (aNewIndex != mylastindex
   || thePickOwner->IsForcedHilight())
  {
    if (mylastindex != 0
     && mylastindex <= myMapOfOwner.Extent())
    {
      const Handle(SelectMgr_EntityOwner)& aLastOwner = myMapOfOwner (mylastindex);
      Unhilight (aLastOwner, theView);
    }

    if (myAutoHilight)
    {
      if (thePickOwner->State() <= 0
       || myCTX->ToHilightSelected())
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

  if (mylastindex)
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
    Handle(StdSelect_BRepOwner) BROwnr = Handle(StdSelect_BRepOwner)::DownCast(myMapOfOwner(mylastindex));
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
    Handle(SelectMgr_SelectableObject) SO = myMapOfOwner.FindKey(mylastindex)->Selectable();
    Iobj = *((Handle(AIS_InteractiveObject)*) &SO);
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
  return myMapOfOwner.FindKey(mylastindex);
}


//=======================================================================
//function : ComesFromDecomposition
//purpose  : 
//=======================================================================

Standard_Boolean AIS_LocalContext::ComesFromDecomposition(const Standard_Integer PickedIndex) const 
{
  const Handle(SelectMgr_EntityOwner)& OWN = myMapOfOwner.FindKey(PickedIndex);
  Handle(SelectMgr_SelectableObject) aSel  = OWN->Selectable();
  if (myActiveObjects.IsBound (aSel)) { // debug of jmi
    const Handle(AIS_LocalStatus)& Stat      = myActiveObjects(aSel);    
    return Stat->Decomposed();
  }
  return Standard_False;
}


//=======================================================================
//function : DisplayAreas
//purpose  : 
//=======================================================================

void AIS_LocalContext::DisplayAreas(const Handle(V3d_View)& aviou)
{
    myMainVS->DisplayAreas(aviou);
}

//=======================================================================
//function : ClearAreas
//purpose  : 
//=======================================================================

void AIS_LocalContext::ClearAreas(const Handle(V3d_View)& aviou)
{
    myMainVS->ClearAreas(aviou);
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
  
  if(Handle(StdSelect_BRepOwner)::DownCast(myMapOfOwner.FindKey(Index)).IsNull())
    return Standard_False;
  return 
    ComesFromDecomposition(Index);
}

Standard_Boolean AIS_LocalContext::IsValidForSelection(const Handle(AIS_InteractiveObject)& anIObj) const 
{

#ifdef IMP120701
  // Shape was not transfered from AIS_Shape to EntityOwner
  Handle(AIS_Shape) shape = Handle(AIS_Shape)::DownCast(anIObj);
  if( !shape.IsNull() ) 
    return myFilters->IsOk(new StdSelect_BRepOwner(shape->Shape(),shape));
#endif
  return myFilters->IsOk(new SelectMgr_EntityOwner(anIObj));
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
  Handle(SelectMgr_EntityOwner) anOwner = myMainVS->Picked (myCurDetected);
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

  if (--myCurDetected < 1)
  {
    myCurDetected = 1;
  }
  Handle(SelectMgr_EntityOwner) anOwner = myMainVS->Picked (myCurDetected);
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
  const Handle(SelectMgr_EntityOwner)& anOwner = myMapOfOwner (mylastindex);
  const Standard_Integer aHilightMode = anOwner->HasSelectable()
                                      ? GetHiMod (Handle(AIS_InteractiveObject)::DownCast (anOwner->Selectable()))
                                      : 0;

  myMapOfOwner (mylastindex)->Unhilight (myMainPM, aHilightMode);
  myMainPM->EndImmediateDraw (theView);
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
#ifdef DEB
    cout<<"\t\tAIS_LocalCOntext::FindSelectedOwnerFromShape : Selection "
        <<mySelName<<" Nulle "<<endl;
#endif
    return EO;
  }
  Standard_Boolean found(Standard_False);
#if !defined OCC189 && !defined USE_MAP     
  const TColStd_Array1OfTransient& Obj = Sel->Objects()->Array1();
  for(Standard_Integer i =Obj.Lower();i<=Sel->NbStored();i++){
    const Handle(Standard_Transient)& Tr = Obj(i);
#else
  const AIS_NListTransient& Obj = Sel->Objects();
  AIS_NListTransient::Iterator anIter( Obj );
  for(; anIter.More(); anIter.Next()){
    const Handle(Standard_Transient)& Tr = anIter.Value();
#endif
    if(!Tr.IsNull()){
      EO = *((Handle(SelectMgr_EntityOwner)*)&Tr);
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
#ifdef OCC9026
  Handle(SelectMgr_EntityOwner) EO, bid;
#else
  Handle(SelectMgr_EntityOwner) EO;
#endif
  if (sh.IsNull()) return EO;
  
  Handle(AIS_Selection) Sel = AIS_Selection::Selection(mySelName.ToCString());
  if(Sel.IsNull()) {
#ifdef DEB
    cout<<"\t\tAIS_LocalCOntext::FindSelectedOwnerFromShape : Selection "<<mySelName<<" Nulle "<<endl;
#endif
    return EO;
  }
  
  Standard_Boolean found(Standard_False);

#ifdef OCC9026
  if (!found) {
    SelectMgr_DataMapIteratorOfDataMapOfIntegerSensitive aSensitiveIt (myMainVS->Primitives());
    for (; aSensitiveIt.More(); aSensitiveIt.Next()) {
      EO = Handle(SelectMgr_EntityOwner)::DownCast (aSensitiveIt.Value()->OwnerId());
      Handle(StdSelect_BRepOwner) BROwnr = Handle(StdSelect_BRepOwner)::DownCast(EO);
      if (!BROwnr.IsNull() && BROwnr->HasShape() && BROwnr->Shape() == sh) {
	 found = Standard_True;
	 break;
      }
    }
  }
#else
#if !defined OCC189 && !defined USE_MAP   
  const TColStd_Array1OfTransient& Obj = Sel->Objects()->Array1();
  for(Standard_Integer i =Obj.Lower();i<=Sel->NbStored();i++){
    const Handle(Standard_Transient)& Tr = Obj(i);
#else
  const AIS_NListTransient& Obj = Sel->Objects();
  AIS_NListTransient::Iterator anIter( Obj );
  for(; anIter.More(); anIter.Next()){
    const Handle(Standard_Transient)& Tr = anIter.Value();
#endif
    if(!Tr.IsNull()){
      
      EO = *((Handle(SelectMgr_EntityOwner)*)&Tr);
      if(EO->HasShape())
	if ( EO->Shape() == sh)
	  found =Standard_True;
          break;
    }
  }
#endif

  if(found)  return EO;
  return bid;
}

#ifdef IMP160701
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
  if (MoreDetected()) myAISCurDetected++;
}

//=======================================================================
//function : DetectedCurrentShape
//purpose  :
//=======================================================================

const TopoDS_Shape& AIS_LocalContext::DetectedCurrentShape() const
{
  static TopoDS_Shape bidsh;
  if (MoreDetected())
    return Handle(AIS_Shape)::DownCast(myAISDetectedSeq(myAISCurDetected))->Shape();
  return bidsh;
}

//=======================================================================
//function : DetectedCurrentObject
//purpose  :
//=======================================================================

Handle(AIS_InteractiveObject) AIS_LocalContext::DetectedCurrentObject() const
{
  Handle(AIS_InteractiveObject) theIObj;
  if (MoreDetected())
    theIObj = myAISDetectedSeq(myAISCurDetected);

  return theIObj;
}
#endif
