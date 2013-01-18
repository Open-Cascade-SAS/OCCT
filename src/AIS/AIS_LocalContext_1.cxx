// Created on: 1996-10-30
// Created by: Robert COUBLANC
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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

#define BUC60863	//GG_270301 Clear hilight soon after selecting or
//                      unselecting something in Local Context mode.

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

#define IMP120402	// GG : Add protection in manual detection methods 
//			after deselecting any item using ShiftSelect action. 
//			Thanks to Ivan FONTAINE of SAMTECH company

#define IMP051001	//GG manage Z detection 

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
#include <Visual3d_TransientManager.hxx>
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
#ifdef IMP120701
#include <AIS_Shape.hxx>
#endif


static Standard_Integer GetHiMod(const Handle(AIS_InteractiveObject)& IO)
{
  return IO->HasHilightMode() ? IO->HilightMode():0;
}

//==================================================
// Function: 
// Purpose :
//==================================================
AIS_StatusOfDetection AIS_LocalContext::MoveTo(const Standard_Integer Xpix,
					       const Standard_Integer Ypix,
					       const Handle(V3d_View)& aview)
{
  // check that ViewerSelector gives 
  if(aview->Viewer()== myCTX->CurrentViewer()) {
#ifdef IMP160701
    //Nullify class members storing information about detected AIS objects.
    myAISCurDetected = 0;
    myAISDetectedSeq.Clear();
#endif
    myCurDetected = 0;
    myDetectedSeq.Clear();
    myMainVS->Pick(Xpix,Ypix,aview);
    Standard_Boolean had_nothing = myMainVS->NbPicked()==0;
    Standard_Integer NbDetected =  myMainVS->NbPicked();
    Handle(SelectMgr_EntityOwner) EO;

    for(Standard_Integer i_detect = 1;i_detect<=NbDetected;i_detect++){
      EO = myMainVS->Picked(i_detect);
      if(!EO.IsNull()){
	if(myFilters->IsOk(EO)) {
	  myDetectedSeq.Append(i_detect); // normallly they are already arranged in correct order...
#ifdef IMP160701
        Handle(AIS_InteractiveObject) anObj = Handle(AIS_InteractiveObject)::DownCast(EO->Selectable());
        if(!Handle(AIS_Shape)::DownCast(anObj).IsNull())
          myAISDetectedSeq.Append(anObj);
#endif
      }
    }
    }
    
    //result of  courses..
    if(had_nothing || myDetectedSeq.IsEmpty()){
      if(mylastindex !=0 && mylastindex <= myMapOfOwner.Extent()){

#ifdef BUC60863
	Unhilight(myMapOfOwner(mylastindex),aview);
#else
	if(!IsSelected(myMapOfOwner(mylastindex)))
	  	Unhilight(myMapOfOwner(mylastindex),aview);
#endif
      }
      mylastindex=0;
      return (had_nothing ? AIS_SOD_Nothing : AIS_SOD_AllBad);
    }
    
    // all owners detected by the selector are passed to the 
    // filters and correct ones are preserved...
    myCurDetected = 1;
    EO = myMainVS->Picked(myDetectedSeq(myCurDetected));
    
    static Standard_Boolean Normal_State(Standard_True);
    static Standard_Boolean firsttime(Standard_True);
    if(firsttime){
      OSD_Environment toto("HITRI");
      if(!toto.Value().IsEmpty())
	Normal_State = Standard_False;
      firsttime = Standard_False;
    }
    
    if(Normal_State)
      ManageDetected(EO,aview);
    else
      HilightTriangle(1,aview);
    
    if(myDetectedSeq.Length() == 1){
      if(NbDetected==1)
	return AIS_SOD_OnlyOneDetected;
      else
	return AIS_SOD_OnlyOneGood;
    }
    else 
      return AIS_SOD_SeveralGood;
    
  
  }
  return AIS_SOD_Error;
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
    
  if(myAutoHilight) {
#ifdef BUC60863
    const Handle(V3d_Viewer)& aViewer = myCTX->CurrentViewer();
    for(aViewer->InitActiveViews(); aViewer->MoreActiveViews(); aViewer->NextActiveViews())
      Unhilight(EO, aViewer->ActiveView());

    // san - advanced selection highlighting mechanism
    if (!EO->IsAutoHilight() && EO->HasSelectable()){
      Handle(AIS_InteractiveObject) anIO = 
        Handle(AIS_InteractiveObject)::DownCast(EO->Selectable());
      UpdateSelected(anIO, Standard_False);
    }

    if(updateviewer)
        myCTX->CurrentViewer()->Update();
#else
    HilightPicked(updateviewer);
#endif
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
#ifndef BUC60863
    if(myAutoHilight)
      UnhilightPicked(Standard_False);
#endif
    
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
    
    if(myAutoHilight) {
#ifdef BUC60863
      const Handle(V3d_Viewer)& aViewer = myCTX->CurrentViewer();
      for(aViewer->InitActiveViews(); aViewer->MoreActiveViews(); aViewer->NextActiveViews())
        Unhilight(EO, aViewer->ActiveView());

      // san - advanced selection highlighting mechanism
      if (!EO->IsAutoHilight() && EO->HasSelectable()){
        Handle(AIS_InteractiveObject) anIO = 
          Handle(AIS_InteractiveObject)::DownCast(EO->Selectable());
        UpdateSelected(anIO, Standard_False);
      }

      if(updateviewer)
        myCTX->CurrentViewer()->Update();
#else
      HilightPicked(updateviewer);
#endif
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
// Function: 
// Purpose :
//==================================================
void AIS_LocalContext::Hilight(const Handle(SelectMgr_EntityOwner)& Ownr,
			       const Handle(V3d_View)& aview)
{
#ifdef BUC60863
  if( aview.IsNull() ) return;
  aview->TransientManagerClearDraw();
#else
  if(aview->TransientManagerBeginDraw())
    Visual3d_TransientManager::EndDraw();
#endif  
  myMainPM->BeginDraw();
  Handle(SelectMgr_SelectableObject) SO = Ownr->Selectable();
  Standard_Integer HM = GetHiMod(*((Handle(AIS_InteractiveObject)*)&SO));
  Ownr->HilightWithColor(myMainPM,myCTX->HilightColor(),HM);
#ifdef IMP051001
  myMainPM->EndDraw(aview,myCTX->ZDetection());
#else
  myMainPM->EndDraw(aview);
#endif

}

//==================================================
// Function: 
// Purpose :
//==================================================
void AIS_LocalContext::Unhilight(const Handle(SelectMgr_EntityOwner)& Ownr,
	    const Handle(V3d_View)& aview)
{

  Handle(SelectMgr_SelectableObject) SO = Ownr->Selectable();
  Standard_Integer HM = GetHiMod(*((Handle(AIS_InteractiveObject)*)&SO));
#ifdef BUC60863
  if( aview.IsNull() ) return;
  if( IsSelected(Ownr) ) {
    if ( Ownr->IsAutoHilight() )
      Ownr->HilightWithColor(myMainPM,myCTX->SelectionColor(),HM);
  }    
  else 
  {
    myMainPM->BeginDraw();
    Ownr->Unhilight(myMainPM,HM);
    myMainPM->EndDraw(aview);
  }
  aview->TransientManagerClearDraw();
#else
  if(aview->TransientManagerBeginDraw())
    Visual3d_TransientManager::EndDraw();
  myMainPM->BeginDraw();
  Ownr->Unhilight(myMainPM,HM);
  myMainPM->EndDraw(aview);
#endif
  
}

//=======================================================================
//function : HilightPicked
//purpose  : 
//=======================================================================
void AIS_LocalContext::HilightPicked(const Standard_Boolean updateviewer)
{
  Standard_Boolean updMain(Standard_False),updColl(Standard_False);

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
	  if(myCTX->IsInCollector(IO)){
	    PM = myCTX->CollectorPrsMgr();
	    updColl = Standard_True;}
	  else
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

  if(updateviewer){
#ifdef BUC60863
     myCTX->CurrentViewer()->Update();
#else
    if(updMain) myCTX->CurrentViewer()->Update();
#endif
    if(updColl) myCTX->Collector()->Update();
  }
}

//==================================================
// Function: 
// Purpose :
//==================================================
void AIS_LocalContext::
UnhilightPicked(const Standard_Boolean updateviewer)
{
  Standard_Boolean updMain(Standard_False),updColl(Standard_False);

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
	  if(myCTX->IsInCollector(IO)){
	    PM = myCTX->CollectorPrsMgr();
	    updColl = Standard_True;}
	  else
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
    if(updColl) myCTX->Collector()->Update();
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
// Function: 
// Purpose :
//==================================================
void AIS_LocalContext::ManageDetected(const Handle(SelectMgr_EntityOwner)& aPickOwner, 
				      const Handle(V3d_View)& aview)
{
#ifdef BUC60818
  // Warning : aPickOwner may be null !
  if (aPickOwner.IsNull()) return;
#else
  if(!myAutoHilight) return;
#endif
//  const Handle(SelectMgr_SelectableObject)& SO = aPickOwner->Selectable();
  Standard_Boolean okStatus = myFilters->IsOk(aPickOwner);
  // OK...
  if(okStatus){
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

    
    //szv:
    Standard_Boolean wasContained = myMapOfOwner.Contains(aPickOwner);
    Standard_Integer theNewIndex = 0;
    if (wasContained)
      theNewIndex = myMapOfOwner.FindIndex(aPickOwner);
    else
      theNewIndex = myMapOfOwner.Add(aPickOwner);

    // For the advanced mesh selection mode the owner indices comparison
    // is not effective because in that case only one owner manage the
    // selection in current selection mode. It is necessary to check the current detected
    // entity and hilight it only if the detected entity is not the same as 
    // previous detected (IsForcedHilight call)
    if (theNewIndex != mylastindex || aPickOwner->IsForcedHilight()) {

      if (mylastindex && mylastindex <= myMapOfOwner.Extent()) {

        const Handle(SelectMgr_EntityOwner)& LastOwnr = myMapOfOwner(mylastindex);
#ifdef BUC60863
        Unhilight(LastOwnr,aview);
#else
        if(!IsSelected(LastOwnr))
          Unhilight(LastOwnr,aview);
#endif
      }

      if (myAutoHilight) {
        // wasContained should not be checked because with this verification different
        // behaviour of application may occer depending whether mouse is moved above 
        // owner first or second time
        //if (wasContained) {
#ifdef BUC60569
          if (aPickOwner->State() <= 0 || myCTX->ToHilightSelected())
#else
          if(!IsSelected (aPickOwner) || myCTX->ToHilightSelected())
#endif
            Hilight(aPickOwner,aview);
        /*}
        else Hilight(aPickOwner,aview);*/
      }

      mylastindex = theNewIndex;
    }
  }

  if (mylastindex) mylastgood = mylastindex;
  
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

Standard_Integer AIS_LocalContext::HilightNextDetected(const Handle(V3d_View)& V)
{
  // go to the next owner

  if(myDetectedSeq.IsEmpty()) return Standard_False;
  Standard_Integer L = myDetectedSeq.Length();
  myCurDetected++;

  if(myCurDetected>L)
    myCurDetected = 1;
  Handle(SelectMgr_EntityOwner) EO = myMainVS->Picked(myCurDetected);
#ifdef IMP120402
  if( EO.IsNull() ) return 0;
#endif

  static Standard_Boolean Normal_State(Standard_True);
  static Standard_Boolean firsttime(Standard_True);
  if(firsttime){
    OSD_Environment toto("HITRI");
    if(!toto.Value().IsEmpty())
      Normal_State = Standard_False;
    firsttime = Standard_False;
  }


  if(Normal_State)
    ManageDetected(EO,V);
  else
    HilightTriangle(myCurDetected,V);
  return myCurDetected;
}

//=======================================================================
//function : HilightPreviousDetected
//purpose  : 
//=======================================================================
Standard_Integer AIS_LocalContext::HilightPreviousDetected(const Handle(V3d_View)& V)
{
  if(myDetectedSeq.IsEmpty()) return Standard_False;

  myCurDetected--;

  if(myCurDetected<1)
    myCurDetected = 1;
  Handle(SelectMgr_EntityOwner) EO = myMainVS->Picked(myCurDetected);
#ifdef IMP120402
  if( EO.IsNull() ) return 0;
#endif

  static Standard_Boolean Normal_State(Standard_True);
  static Standard_Boolean firsttime(Standard_True);
  if(firsttime){
    OSD_Environment toto("HITRI");
    if(!toto.Value().IsEmpty())
      Normal_State = Standard_False;
    firsttime = Standard_False;
  }



  if(Normal_State)
    ManageDetected(EO,V);
  else
    HilightTriangle(myCurDetected,V);
  return myCurDetected;
}

//=======================================================================
//function : UnhilightLastDetected
//purpose  : 
//=======================================================================
Standard_Boolean AIS_LocalContext::UnhilightLastDetected(const Handle(V3d_View)& aview)
{
  if(!IsValidIndex(mylastindex)) return Standard_False;
  myMainPM->BeginDraw();
  const Handle(SelectMgr_EntityOwner)& Ownr = myMapOfOwner(mylastindex);
  Standard_Integer HM(0);
  if(Ownr->HasSelectable()){
    Handle(SelectMgr_SelectableObject) SO = Ownr->Selectable();
    HM = GetHiMod(*((Handle(AIS_InteractiveObject)*)&SO));
  }
  myMapOfOwner(mylastindex)->Unhilight(myMainPM,HM);
  myMainPM->EndDraw(aview);
  mylastindex =0;
  return Standard_True;
}



//=======================================================================
//function : HilightTriangle
//purpose  : 
//=======================================================================

void AIS_LocalContext::HilightTriangle(const Standard_Integer Rank,
				       const Handle(V3d_View)& view)
{
  static Standard_Integer PrevRank(0);
  if(Rank==PrevRank) return;
  Handle(SelectBasics_SensitiveEntity) SE = myMainVS->Primitive(Rank);
  if(SE->IsKind(STANDARD_TYPE(Select3D_SensitiveTriangulation)))
  {
    Handle(Select3D_SensitiveTriangulation) Tr = *((Handle(Select3D_SensitiveTriangulation)*)&SE);
    gp_Pnt p1,p2,p3 ; Tr->DetectedTriangle(p1,p2,p3);

    Handle(Graphic3d_ArrayOfTriangles) aTris = new Graphic3d_ArrayOfTriangles(3);
	aTris->AddVertex(p1);
	aTris->AddVertex(p2);
	aTris->AddVertex(p3);

    static Handle(Prs3d_Presentation) TriPrs = 
      new Prs3d_Presentation(myMainPM->StructureManager());
    TriPrs->Clear();
#ifdef IMP300101
    Handle(Prs3d_ShadingAspect) asp = myCTX->DefaultDrawer()->ShadingAspect();
    asp->SetColor(myCTX->HilightColor());
    TriPrs->SetShadingAspect(asp);
#endif
    Prs3d_Root::CurrentGroup(TriPrs)->AddPrimitiveArray(aTris);

#ifndef IMP300101
    if(view->TransientManagerBeginDraw())
      Visual3d_TransientManager::EndDraw();
#endif
    if(view->TransientManagerBeginDraw()) {
      Visual3d_TransientManager::DrawStructure(TriPrs);
      Visual3d_TransientManager::EndDraw();
    }
  }
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
    //now iterate over all shapes loaded into the context (but inside the collector)
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
