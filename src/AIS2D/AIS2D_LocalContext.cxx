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

#include <AIS2D_LocalContext.ixx>
#include <AIS2D_LocalStatus.hxx>
#include <AIS2D_ProjShape.hxx>
#include <AIS2D_ListOfIO.hxx>
#include <AIS2D_DataMapIteratorOfDataMapOfLocStat.hxx>
#include <Prs2d_AspectLine.hxx>
#include <Prs2d_Drawer.hxx>
#include <Prs2d_Point.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <AIS2D_ListIteratorOfListOfIO.hxx>
#include <TColStd_Array1OfTransient.hxx>
#include <TColStd_HArray1OfTransient.hxx>
#include <V2d_Viewer.hxx>
#include <Graphic2d_TransientManager.hxx>
#include <Graphic2d_Line.hxx>
#include <Graphic2d_View.hxx>
#include <Graphic2d_DisplayList.hxx>
#include <AIS2D_HSequenceOfIO.hxx>

static Standard_Integer GetHiMod(const Handle(AIS2D_InteractiveObject)& anIO)
{
  return anIO->HasHighlightMode() ? anIO->HighlightMode() 
                                  : anIO->DefaultHighlightMode();
}


AIS2D_LocalContext::AIS2D_LocalContext()
{
}

AIS2D_LocalContext::AIS2D_LocalContext(
			       const Handle(AIS2D_InteractiveContext)& aCtx,
				   const Standard_Integer /*Index*/,
				   const Standard_Boolean LoadDisplayed,
				   const Standard_Boolean AcceptStdModes,
				   const Standard_Boolean AcceptEraseOfTemp ):
  myICTX( aCtx ),
  myLastPicked( new AIS2D_InteractiveObject()),
  myLastinMain( new AIS2D_InteractiveObject()),
  myLastPickElInd( 0 ),
  myLastSelElInd( 0 ),
  myLoadDisplayed( LoadDisplayed ),
  myAcceptStdMode( AcceptStdModes ),
  myAcceptErase( AcceptEraseOfTemp ),
  myAutoHighlight( Standard_True ),
  myLastIndex( 0 ),
  myLastGood( 0 ),
  myCurDetected( 0 ),
  myResetDetect( Standard_True )
  
{
  if ( myLoadDisplayed ) LoadContextObjects();
}


Standard_Boolean AIS2D_LocalContext::Display( 
	   const Handle( AIS2D_InteractiveObject)& anIObj,
	   const Standard_Integer WhichMode,
	   const Standard_Boolean /*AllowDecomp*/,
	   const Standard_Integer ActivationMode ) {

  if ( myActiveObjects.IsBound(anIObj) ) {
    const Handle( AIS2D_LocalStatus )& LStatus = myActiveObjects( anIObj );
    
    if ( LStatus->DisplayMode() == -1) {
       if ( !anIObj->IsDisplayed() )
	       anIObj->Display();
        if ( LStatus->IsTemporary() )
           LStatus->SetDisplayMode( WhichMode );
	} else if ( LStatus->DisplayMode() != WhichMode && LStatus -> IsTemporary() ) {
          anIObj->Remove();
          LStatus->SetDisplayMode( WhichMode );
          if ( !anIObj->IsDisplayed() )
	         anIObj->Display();
	}
    
    if ( ActivationMode !=-1 ) {
        if ( !LStatus->IsActivated( ActivationMode ) ) {
        	LStatus->ClearSelectionModes();
	        //mySelMgr->Load( anIObj, myMainVS );
	     LStatus->AddSelectionMode( ActivationMode );
	    //mySelMgr->Activate( anIObj, ActivationMode, myMainVS );
		}
	  }
	} else {
      Handle(AIS2D_LocalStatus) LStatus = new AIS2D_LocalStatus();
    
      /*if ( anIObj->AcceptDecomposition() && AllowDecomp )
        LStatus->SetDecomposition(Standard_True);
      else 
        LStatus->SetDecomposition(Standard_False);
      */
      if( myICTX->DisplayStatus(anIObj) == AIS2D_DS_None ||
          myICTX->DisplayStatus(anIObj) == AIS2D_DS_Temporary )
        LStatus->SetTemporary(Standard_True);
      else
        LStatus->SetTemporary(Standard_False); 
	  AIS2D_TypeOfDetection HiMod ;  
      if( !myICTX->IsDisplayed( anIObj, WhichMode) ) {
          LStatus->SetDisplayMode(WhichMode);
          if ( ActivationMode != -1 )
	       LStatus->AddSelectionMode(ActivationMode);
          HiMod = anIObj->HasHighlightMode() ? 
			  anIObj->HighlightMode() : anIObj->DefaultHighlightMode();
          LStatus->SetHighlightMode( HiMod );
          if( !anIObj->IsDisplayed()) {
              DrawObject(anIObj,WhichMode);
              anIObj->Display();         
          }
          /*if(ActivationMode!=-1) {
	        mySelMgr->Load(anIObj,myMainVS);
	        mySelMgr->Activate(anIObj,ActivationMode,myMainVS);
		  } */
	  } else {
        HiMod = anIObj->HasHighlightMode()? anIObj->HighlightMode(): anIObj->DefaultHighlightMode();
        LStatus->SetHighlightMode( HiMod );
	  }
      myActiveObjects.Bind(anIObj,LStatus);
  }  
  
  return Standard_True;

}

Standard_Boolean AIS2D_LocalContext::Erase( const Handle(AIS2D_InteractiveObject)& anIObj) {

  if ( !myActiveObjects.IsBound( anIObj ) ) return Standard_False;

  const Handle(AIS2D_LocalStatus)& LStatus = myActiveObjects( anIObj );
  
  if ( LStatus->IsSubIntensityOn() ) {
    LStatus->SubIntensityOff();
    anIObj->Unhighlight();
  }

  Standard_Boolean stat( Standard_False );

  if ( LStatus->DisplayMode() != -1 ) {
   // if ( IsSelected( anIObj ) )
   //   AddOrRemoveSelected( anIObj );
    if ( anIObj->IsHighlighted() )
      anIObj->Unhighlight();
    anIObj->Remove();
    LStatus->SetDisplayMode(-1);
    stat = Standard_True;
  }
  if ( LStatus->IsTemporary() ) 
    if ( anIObj->IsDisplayed() )
      anIObj->Remove();

   
/*  TColStd_ListIteratorOfListOfInteger It( * LStatus->SelectionModes() );
  for ( ; It.More(); It.Next() )
    mySelMgr->Deactivate( anIObj, It.Value(), myMainVS );
 */
   return stat;
}

Standard_Boolean AIS2D_LocalContext::Load( 
		const Handle(AIS2D_InteractiveObject)& anIObj,
        const Standard_Boolean /*AllowDecomp*/,
        const Standard_Integer ActivationMode ) {

  if ( myActiveObjects.IsBound( anIObj ) ) return Standard_False;
  Handle(AIS2D_LocalStatus) LStatus = new AIS2D_LocalStatus();
  
  /*if ( anIObj->AcceptDecomposition() && AllowDecomp )
    LStatus->SetDecomposition( Standard_True );
  else 
    LStatus->SetDecomposition( Standard_False );
  */
  if ( !myICTX->IsDisplayed( anIObj ) )
    LStatus->SetTemporary( Standard_True );
  else
    LStatus->SetTemporary( Standard_False );
  LStatus->SetDisplayMode( -1 );
  
  //storing information....
  if ( ActivationMode !=-1 )
    LStatus->AddSelectionMode( ActivationMode );
  AIS2D_TypeOfDetection HiMod = anIObj->HasHighlightMode() ? anIObj->HighlightMode() : anIObj->DefaultHighlightMode();
  LStatus->SetHighlightMode( HiMod );
  
  /*mySelMgr->Load( anIObj, myMainVS );
  if ( ActivationMode != -1 ) {
    mySelMgr->Activate( anIObj, ActivationMode, myMainVS );
  } */
  myActiveObjects.Bind( anIObj, LStatus );
  
  return Standard_True;

}

Standard_Boolean AIS2D_LocalContext::Remove(const Handle(AIS2D_InteractiveObject)& /*aSelObj*/ ) {
  
  return Standard_False;
}

void AIS2D_LocalContext::LoadContextObjects() {

  AIS2D_ListIteratorOfListOfIO It;
  if ( myLoadDisplayed ) {
    AIS2D_ListOfIO theLOI;
    myICTX->DisplayedObjects( theLOI, Standard_True );
    Handle(AIS2D_LocalStatus) LStatus;
    for ( It.Initialize(theLOI); It.More(); It.Next() ) {
      LStatus = new AIS2D_LocalStatus();
//      LStatus->SetDecomposition((It.Value()->AcceptDecomposition() && myAcceptStdMode));
      LStatus->SetTemporary(Standard_False);
      LStatus->SetHighlightMode( It.Value()->HasHighlightMode() ? It.Value()->HighlightMode(): It.Value()->DefaultHighlightMode() );
      
      myActiveObjects.Bind(It.Value(),LStatus);
    }
  }
}

void AIS2D_LocalContext::UnloadContextObjects()
{}


void AIS2D_LocalContext::Terminate() {

  ClearDetected();
  Clear();
  myLastIndex = 0;
  
  if ( ! myICTX->mySeqOfSelIO->IsEmpty() ) 
      for ( int i = 1; i <= myICTX->mySeqOfSelIO->Length(); ++i )
          myICTX->mySeqOfSelIO->Value(i)->SetState(0);

 /*       
  AIS2D_Selection::Select();
  AIS2D_Selection::Remove(mySelName.ToCString());*/
  Handle(V2d_Viewer) Vwr = myICTX->CurrentViewer();
  Handle(V2d_View) curV;
  for ( Vwr->InitActiveViews(); Vwr->MoreActiveViews(); Vwr->NextActiveViews() ) {
    curV = Vwr->ActiveView(); 
  }

}

void AIS2D_LocalContext::Clear(const AIS2D_ClearMode aType) {
  switch (aType){
  case AIS2D_CM_All:
    {
      ClearObjects();
      break;
    }
  case AIS2D_CM_Interactive:
    ClearObjects();
    break;
  case AIS2D_CM_StandardModes:
    {
     while(!myListOfStdMode.IsEmpty())
       //DeactivateStdMode(AIS2D_Shape::SelectionType(myListOfStdMode.Last()));
      break;
    }
  case AIS2D_CM_TemporaryShapePrs:
    ClearDetected();
  }
  //UpdateSort();
}

Standard_Boolean AIS2D_LocalContext::IsSelected( const Handle(AIS2D_InteractiveObject)& anIObj) const {
  
   if ( ! myICTX->mySeqOfSelIO->IsEmpty() ) 
       for ( int i = 1; i <= myICTX->mySeqOfSelIO->Length(); ++i )
           if ( myICTX->mySeqOfSelIO->Value(i) == anIObj )
               return Standard_True;
    return Standard_False;
}

Handle(AIS2D_InteractiveObject) AIS2D_LocalContext::SelectedIO() const {
 return myLastPicked;
}


AIS2D_StatusOfDetection AIS2D_LocalContext::MoveTo(
					    const Standard_Integer XPix,
					    const Standard_Integer YPix,
					    const Handle(V2d_View)& aView ) {

  if ( aView->Viewer() == myICTX->CurrentViewer() ) {
    Standard_Integer Dprecision = myICTX->DetectPrecision();
    Standard_Boolean UpdVwr = Standard_False;
    Handle(V2d_Viewer) theViewer = aView->Viewer();
 
    AIS2D_StatusOfDetection theStat( AIS2D_SOD_Nothing );

    Handle(Graphic2d_Primitive) thePrim;                                           
    Handle(Graphic2d_TransientManager) theDrawer = 
    Handle(Graphic2d_TransientManager)::DownCast( theViewer->View()->Drawer() );
 
    Handle(Graphic2d_DisplayList) thePickList = aView->Pick( XPix, YPix, Dprecision );
 
    if ( thePickList->Length() > 0 ) {
   
     if ( thePickList->Length() == 1 ) {
      Handle(AIS2D_InteractiveObject) theIO = Handle(AIS2D_InteractiveObject)::DownCast(thePickList->Value(1));
      if ( theIO == myLastPicked ) 
       switch ( theIO->HighlightMode() ) {
       default:
       case AIS2D_TOD_OBJECT: 
          return AIS2D_SOD_OnlyOneDetected;
          break;
       case AIS2D_TOD_PRIMITIVE:
          if ( myLastPicked->Primitive(myLastPicked->PickedIndex()) == myLastPickPrim )
            return AIS2D_SOD_OnlyOneDetected;
          break;
       case AIS2D_TOD_ELEMENT:
          if ( ( myLastPicked->Primitive(myLastPicked->PickedIndex()) == myLastPickPrim ) && 
             ( myLastPicked->Primitive(myLastPicked->PickedIndex())->PickedIndex() == myLastPickElInd ) )
            return AIS2D_SOD_OnlyOneDetected;
          break;
       case AIS2D_TOD_VERTEX:
          if ( ( myLastPicked->Primitive(myLastPicked->PickedIndex()) == myLastPickPrim ) && 
             ( myLastPicked->Primitive(myLastPicked->PickedIndex())->PickedIndex() == myLastPickElInd ) )
            return AIS2D_SOD_OnlyOneDetected;
          break; 
             
       }
   }
   if ( !myLastPicked.IsNull() )
    if ( !myLastPicked->State() ) {
      myLastPicked->Unhighlight();
	    UpdVwr = Standard_True;
    } // end if the last picked object isn't selected one
 
   if ( myResetDetect ) {

     theDrawer->SetOverride(Standard_True);
     theDrawer->SetOverrideColor( theViewer->InitializeColor(myICTX->HighlightColor()) );
     theDrawer->BeginDraw( aView->Driver() );

     for ( int i = 1 ; i <= thePickList->Length(); i++ ) {

       myLastPicked = Handle(AIS2D_InteractiveObject)::DownCast( thePickList->Value(i) );
       myLastinMain = myLastPicked;
   
     
       if ( myLastPicked->HasHighlightMode() ) {

        myLastPicked->Highlight(theViewer->InitializeColor(myICTX->HighlightColor()));
        switch( myLastPicked->HighlightMode() ) {
	    default:
        case AIS2D_TOD_OBJECT:
         if ( myLastPicked->State() != 1 ) {
           theDrawer->Draw( myLastPicked );  
         } else {
           theStat = AIS2D_SOD_Selected;
         }
         break;
        case AIS2D_TOD_PRIMITIVE:
         myLastPickPrim = myLastPicked->Primitive(myLastPicked->PickedIndex());
         if ( !myLastPickPrim->IsHighlighted() ) {
            theDrawer->Draw(myLastPickPrim);
         } else {
            theStat = AIS2D_SOD_Selected;
         }
	     break;
        case AIS2D_TOD_ELEMENT: {
         Standard_Integer pInd = myLastPicked->PickedIndex();
	     myLastPickPrim = myLastPicked->Primitive(pInd);
         if ( myLastPickPrim != myLastSelPrim ||
            ( myLastPickPrim == myLastSelPrim && 
              myLastPickPrim->PickedIndex() != myLastSelElInd ) ||
            ( !myLastPickPrim->PickedIndex() && 
              !myLastPickPrim->IsHighlighted() ) ) {
            myLastPickElInd = myLastPickPrim->PickedIndex();
            theDrawer->DrawElement(myLastPickPrim,myLastPickElInd);    
         } else {
            theStat = AIS2D_SOD_Selected;
         }
         break;
        }
        case AIS2D_TOD_VERTEX: {
	     myLastPickPrim = myLastPicked->Primitive(myLastPicked->PickedIndex());
         if ( myLastPickPrim != myLastSelPrim ||
            ( myLastPickPrim == myLastSelPrim && 
              myLastPickPrim->PickedIndex() != myLastSelElInd ) ||
            ( !myLastPickPrim->PickedIndex() && 
              !myLastPickPrim->IsHighlighted() ) ) {
            myLastPickElInd = myLastPickPrim->PickedIndex();
            theDrawer->DrawVertex(myLastPickPrim,-myLastPickElInd);    
         } else {
             theStat = AIS2D_SOD_Selected;
         }
	     break;
        }
        } //end switch
       } // end if lastPicked has highlight mode
   
     } // end for

          theDrawer->EndDraw();
          theDrawer->SetOverride(Standard_False);
	      myResetDetect= Standard_False;

   } // end if myResetDetect is true

 } else {

    theStat = AIS2D_SOD_Nothing;
    theDrawer->RestoreArea( aView->Driver() );
    myResetDetect= Standard_True;
    if ( !myLastPicked.IsNull() ) {
     if ( myLastPicked->HighlightMode() == AIS2D_TOD_ELEMENT || 
          myLastPicked->HighlightMode() == AIS2D_TOD_VERTEX ) {
       myLastPicked->Highlight(theViewer->InitializeColor( myICTX->SelectionColor() ));

       myLastPicked->Unhighlight();
       UpdVwr = Standard_True;
     } else if ( !myLastPicked->State() ) {
       myLastPicked->Unhighlight();
       UpdVwr = Standard_True;
     }
    }
    
    myLastinMain.Nullify();
    myLastPickPrim.Nullify();
    myLastPickElInd = 0;

 }  // end if PickList isn't empty
 
 //if ( !myLastPicked.IsNull() )  myLastPicked->Unhighlight();
    
 if ( UpdVwr ) theViewer->Update();

 myLastPicked.Nullify();
 //myLastMoveView = aView;
 return theStat;
 
 } else 
   return AIS2D_SOD_Error;

}

AIS2D_StatusOfPick AIS2D_LocalContext::Select( const Standard_Boolean UpdateVwr ) {
  
  if ( myAutoHighlight )
    UnhighlightPicked( Standard_False );
  
  Standard_Integer DI = DetectedIndex();
//  AIS2D_Selection::SetCurrentSelection( mySelName.ToCString() );
//  Standard_Integer NbSel = AIS2D_Selection::Extent();
   Standard_Integer NbSel = myICTX->mySeqOfSelIO->Length();  
  if ( DI <= 0 ) {
    ClearSelected( UpdateVwr );
    return NbSel == 0 ? AIS2D_SOP_NothingSelected : AIS2D_SOP_Removed;
  }
 
  if ( myAutoHighlight )
    HighlightPicked( UpdateVwr );
  //return ( AIS2D_Selection::Extent() == 1) ? AIS2D_SOP_OneSelected : AIS2D_SOP_SeveralSelected;
  return ( myICTX->mySeqOfSelIO->Length() == 1) ? AIS2D_SOP_OneSelected : AIS2D_SOP_SeveralSelected;
}

AIS2D_StatusOfPick AIS2D_LocalContext::ShiftSelect( const Standard_Boolean /*UpdateVwr*/ ) {
  
/*  Standard_Integer I = DetectedIndex();
  if ( I > 0 ) {
    UnhighlightPicked(Standard_False);
    
    AIS2D_Selection::SetCurrentSelection(mySelName.ToCString());
    const Handle(SelectMgr_EntityOwner)& EntOwr  = myMapOfOwner(I);
    Standard_Integer mod = EntOwr->State() == 0 ? 1 : 0;
    EntOwr->State(mod);
    
    AIS2D_Selection::Select(EntOwr);
    
    if ( myAutoHighlight )
      HighlightPicked(UpdateVwr);  
  }*/
  return AIS2D_SOP_Error;
}

void AIS2D_LocalContext::SetDisplayPriority(
					const Handle(AIS2D_InteractiveObject)& anIObj,
					const Standard_Integer /*Prior*/ ) {
  
  if ( !myActiveObjects.IsBound(anIObj) ) return;
  const Handle(AIS2D_LocalStatus)& LStatus = myActiveObjects(anIObj);
  if ( LStatus->DisplayMode() == -1) return;
  //myMainPM->SetDisplayPriority(anIObj, LStatus->DisplayMode(),Prior);
  //if ( LStatus->IsSubIntensityOn())
  //  myMainPM->SetDisplayPriority(anIObj, LStatus->HighlightMode(), Prior);
  
}

Standard_Integer AIS2D_LocalContext::DisplayedObjects(TColStd_MapOfTransient& theMap) const {

  Standard_Integer NbDisp(0);
  for ( AIS2D_DataMapIteratorOfDataMapOfLocStat theIt(myActiveObjects); theIt.More(); theIt.Next() ) {
    const Handle(AIS2D_InteractiveObject)& SObj = theIt.Key();
    if ( !theMap.Contains(SObj) )
      if ( theIt.Value()->DisplayMode() != -1 ) {
         theMap.Add(SObj);
	     NbDisp++;
      }
  }
  return NbDisp;
}

Standard_Boolean AIS2D_LocalContext::IsDisplayed(const Handle(AIS2D_InteractiveObject)& anIObj) const {

  if ( !myActiveObjects.IsBound(anIObj) ) 
      return Standard_False;
  return ( myActiveObjects(anIObj)->DisplayMode() != -1 );
}

Standard_Boolean AIS2D_LocalContext::IsDisplayed(const Handle(AIS2D_InteractiveObject)& anIObj,
						  const Standard_Integer aMode) const {

  if (!myActiveObjects.IsBound(anIObj)) return Standard_False;
  return (myActiveObjects(anIObj)->DisplayMode()==aMode);
}

void AIS2D_LocalContext::Unhighlight(const Handle(AIS2D_InteractiveObject)& /*anIObj*/)
{
}

Standard_Boolean AIS2D_LocalContext::IsHighlight(const Handle(AIS2D_InteractiveObject)& anIObj) const {

  if ( !myActiveObjects.IsBound(anIObj) ) return Standard_False;
  return myActiveObjects( anIObj )->IsSubIntensityOn();
}

Standard_Boolean AIS2D_LocalContext::IsHighlight(const Handle(AIS2D_InteractiveObject)& anIObj,
					       Standard_Boolean& WithColor,
					       Quantity_NameOfColor& HiCol) const 
{
  if(!myActiveObjects.IsBound(anIObj)) return Standard_False;
  if( myActiveObjects(anIObj)->IsSubIntensityOn()){
    HiCol = myActiveObjects(anIObj)->HighlightColor();
    if(HiCol==Quantity_NOC_WHITE)
      WithColor = Standard_True;
    else
      WithColor = Standard_False;
    return Standard_True;
  }
  return Standard_False;
}

void AIS2D_LocalContext::SubIntensityOn(const Handle(AIS2D_InteractiveObject)& anIObj) {
  
  if ( !myActiveObjects.IsBound(anIObj) ) return;
  
  const Handle(AIS2D_LocalStatus)& LStatus = myActiveObjects(anIObj);
  if ( LStatus->IsTemporary() ) {
  	  Quantity_NameOfColor aCol = myICTX->SubIntensityColor();
      myICTX->HighlightWithColor(anIObj, aCol);
  }
  LStatus->SubIntensityOn();
}

void AIS2D_LocalContext::SubIntensityOff(const Handle(AIS2D_InteractiveObject)& anIObj) {
  
  if ( !myActiveObjects.IsBound(anIObj) ) return;
  
  const Handle(AIS2D_LocalStatus)& LStatus = myActiveObjects(anIObj);

  if ( LStatus->IsTemporary()) 
    myICTX->Unhighlight(anIObj);
  LStatus->SubIntensityOff();
}

Standard_Boolean AIS2D_LocalContext::IsIn( const Handle(AIS2D_InteractiveObject)& anIObj) const {
  return myActiveObjects.IsBound(anIObj);
}

Standard_Boolean AIS2D_LocalContext::UnhighLastDetect(const Handle(V2d_View)& /*aView*/) {  
	
//  if ( !IsValidIndex(myLastIndex) ) return Standard_False;
/*  myMainPM->BeginDraw();
  const Handle(SelectMgr_EntityOwner)& Ownr = myMapOfOwner(myLastIndex);
  Standard_Integer HM(0);
  if ( Ownr->HasSelectable() ) {
    Handle(SelectMgr_SelectableObject) SO = Ownr->Selectable();
    HM = GetHiMod(* ( (Handle(AIS2D_InteractiveObject ) * ) & SO ) );
  }
  myMapOfOwner( myLastIndex )->Unhilight( myMainPM, HM );
  myMainPM->EndDraw( aView );
 */
  myLastIndex =0;
  return Standard_True;
}

void AIS2D_LocalContext::DrawObject( const Handle(AIS2D_InteractiveObject)& anIObj, const Standard_Integer /*aDispMode*/ ) {
    
 if ( anIObj->IsKind(STANDARD_TYPE(AIS2D_ProjShape) ) ) {

       Handle(AIS2D_ProjShape) thePS = Handle(AIS2D_ProjShape)::DownCast(anIObj);
       Handle(Graphic2d_Line) theLines = thePS->GetPrimitives();
       Handle(Prs2d_AspectLine) theAspLine;

       if ( !anIObj->HasAspect(theLines) ) {
         Handle(Prs2d_AspectRoot) theAspRoot = anIObj->Attributes()->FindAspect(Prs2d_AN_LINE);
         theAspLine = Handle(Prs2d_AspectLine)::DownCast(theAspRoot);
         anIObj->SetAspect( theAspLine, theLines );
                 
       } else {
         theAspLine = Handle(Prs2d_AspectLine)::DownCast( anIObj->GetAspect(theLines) );
       }      
     
       Quantity_Color     aColor;
       Aspect_TypeOfLine  aType;
       Aspect_WidthOfLine aWidth;
  
       theAspLine->ValuesOfLine( aColor, aType, aWidth );
  
       Standard_Integer colorInd = theAspLine->ColorIndex();
       Standard_Integer typeInd = theAspLine->TypeIndex();
       Standard_Integer widthInd = theAspLine->WidthIndex();
     
       if ( !colorInd ) {
         colorInd = myICTX->InitializeColor( aColor );
         theLines->SetColorIndex( colorInd );
       }

       if ( !typeInd ) {
         typeInd = myICTX->InitializeStyle( aType );
         theLines->SetTypeIndex( typeInd );
       }

       if ( !widthInd ) {
	     widthInd = myICTX->InitializeWidth( aWidth );
         theLines->SetWidthIndex( widthInd );
       }

       if ( thePS->IsHLMode() ) {
         Handle(Graphic2d_Line) theHLines = thePS->GetHLPrimitives();
         theHLines->SetColorIndex( colorInd );
         theHLines->SetWidthIndex( widthInd );
         Standard_Integer theDashInd = myICTX->InitializeStyle( Aspect_TOL_DASH );
         theHLines->SetTypeIndex( theDashInd );          
       }

    }  else {

      Handle(Graphic2d_Primitive) thePrim;

      for ( Standard_Integer i = 1; i <= anIObj->Length(); i++ ) {
       thePrim = anIObj->Primitive( i );
       if ( thePrim->IsKind(STANDARD_TYPE(Graphic2d_Line) ) ) {

         Handle(Graphic2d_Line) theLine = Handle(Graphic2d_Line)::DownCast(thePrim);
         Handle(Prs2d_AspectLine) theAspLine;

         if ( !anIObj->HasAspect(theLine) ) {
           Handle(Prs2d_AspectRoot) theAspRoot = anIObj->Attributes()->FindAspect(Prs2d_AN_LINE);
           theAspLine = Handle(Prs2d_AspectLine)::DownCast(theAspRoot);
           anIObj->SetAspect( theAspLine, theLine );
                 
         } else {
           theAspLine = Handle(Prs2d_AspectLine)::DownCast( anIObj->GetAspect(theLine) );
         }      
     
         Quantity_Color     aColor;
         Aspect_TypeOfLine  aType;
         Aspect_WidthOfLine aWidth;
  
         Quantity_Color aIntColor;
         Graphic2d_TypeOfPolygonFilling aTypeFill;
         Standard_Integer aTile;
         Standard_Boolean aDrawEdge;
  
         theAspLine->ValuesOfLine( aColor, aType, aWidth );
         theAspLine->ValuesOfPoly( aIntColor, aTypeFill, aTile, aDrawEdge );
     
         Standard_Integer colorInd = theAspLine->ColorIndex();
         Standard_Integer typeInd = theAspLine->TypeIndex();
         Standard_Integer widthInd = theAspLine->WidthIndex();
         Standard_Integer colorIntInd = theAspLine->InterColorIndex();

         if ( !colorInd ) {
           colorInd = myICTX->InitializeColor( aColor );
           theLine->SetColorIndex( colorInd );
         }

         if ( !typeInd ) {
           typeInd = myICTX->InitializeStyle( aType );
           theLine->SetTypeIndex( typeInd );
         }

         if ( !widthInd ) {
	       widthInd = myICTX->InitializeWidth( aWidth );
           theLine->SetWidthIndex( widthInd );
         }

         if ( !colorIntInd ) {
           colorIntInd = myICTX->InitializeColor( aIntColor );
           theLine->SetInteriorColorIndex( colorIntInd );
         }
   
         theLine->SetTypeOfPolygonFilling( aTypeFill );
         theLine->SetInteriorPattern( aTile );
         theLine->SetDrawEdge( aDrawEdge );

         if ( theLine->IsKind(STANDARD_TYPE(Prs2d_Point) ) ) {

           Handle(Prs2d_Point) thePnt = Handle(Prs2d_Point)::DownCast(theLine);
           thePnt->SetIndex( myICTX->InitializeMarker( thePnt->Marker() ) );
         }    

    } // end if the kind of primitive is Graphic2d_Line

  } // end for exploration of the interactive object 
 
 }  // end if IO is AIS2D_ProjShape
    
}

void AIS2D_LocalContext::ClearObjects() {

  AIS2D_DataMapIteratorOfDataMapOfLocStat It( myActiveObjects );
  for ( ; It.More(); It.Next() ) {
    Handle(AIS2D_InteractiveObject) SObj = Handle(AIS2D_InteractiveObject)::DownCast( It.Key() );
      
      const Handle(AIS2D_LocalStatus) LStatus = It.Value();
//      TColStd_ListIteratorOfListOfInteger ItL;
      
      AIS2D_DisplayStatus theDS = myICTX->DisplayStatus( SObj );
      
      if ( theDS != AIS2D_DS_Displayed){
    	//if ( myMainPM->IsDisplayed( SObj, LStatus->DisplayMode() ) ) {
       if ( myICTX->IsDisplayed(SObj) ) {
	     if ( LStatus->IsSubIntensityOn() && SObj->IsHighlighted() )
	       SObj->Unhighlight();
	     SObj->Remove();
		}
	
	    if ( LStatus->IsTemporary() )
	     SObj->Remove();
      } else {
	    if ( LStatus->IsSubIntensityOn() ) {
	      myICTX->SubIntensityOff(Handle(AIS2D_InteractiveObject)::DownCast(SObj) ); }
          Standard_Integer DiMo = SObj->HasDisplayMode() ? SObj->DisplayMode() : myICTX->DisplayMode();
          if ( LStatus->DisplayMode() !=-1 && LStatus->DisplayMode() != DiMo)
	         SObj->Remove();
	  }
      
  }
  ClearSelected();
  myActiveObjects.Clear();
  
}


void AIS2D_LocalContext::ClearDetected() {

 /*for ( Standard_Integer i = 1; i <= myMapOfOwner.Extent(); i++ ) {
    if ( !myMapOfOwner(i).IsNull() ) {
      if ( myMapOfOwner(i)->IsHilighted(myMainPM) )
        myMapOfOwner(i)->Unhilight(myMainPM);
      else if ( myMapOfOwner(i)->IsHilighted( myICTX->CollectorPrsMgr() ) )
	          myMapOfOwner(i)->Unhilight( myICTX->CollectorPrsMgr() );
      	   else {
	          const Handle(SelectMgr_SelectableObject)& SO = 
	                   myMapOfOwner.FindKey(i)->Selectable();
	          if ( myActiveObjects.IsBound(SO) ) {
	            const Handle(AIS2D_LocalStatus)& LStatus = myActiveObjects(SO);
                if ( LStatus->IsTemporary() && 
					 LStatus->DisplayMode() == -1 && 
	                 LStatus->SelectionModes()->IsEmpty() ) 
	   
				   myMapOfOwner(i)->Clear(myMainPM);
				
			  }
		   }
	}
 } */
}

void AIS2D_LocalContext::ClearSelected(const Standard_Boolean UpdateVwr) {
  
  UnhighlightPicked(UpdateVwr);
 // AIS2D_Selection::SetCurrentSelection(mySelName.ToCString());

 // Handle(AIS2D_Selection) Sel = AIS2D_Selection::CurrentSelection();
  /*Handle(Standard_Transient) Tr;
  Handle(SelectMgr_EntityOwner) EO;
  const TColStd_Array1OfTransient& Obj = Sel->Objects()->Array1();
  for ( Standard_Integer i = Obj.Lower(); i <= Sel->NbStored(); i++ ) {
    Tr = Obj(i);
    if ( !Tr.IsNull() )
      (*((Handle(SelectMgr_EntityOwner)*)&Tr))->State(0);
  } */
  //AIS2D_Selection::Select();
  myLastIndex = 0;
}

void AIS2D_LocalContext::HighlightPicked(const Standard_Boolean /*UpdateVwr*/) {
  
  /*Standard_Boolean updMain(Standard_False),updColl(Standard_False);
  Quantity_NameOfColor SelCol = myICTX->SelectionColor();
  
  Handle(AIS2D_Selection) Sel = AIS2D_Selection::Selection(mySelName.ToCString());
  Handle(Standard_Transient) Tr;
  Handle(SelectMgr_EntityOwner) Ownr;
  Handle(PrsMgr_PresentationManager2d) PM = myMainPM;
  
  const TColStd_Array1OfTransient& Obj = Sel->Objects()->Array1();
  for ( Standard_Integer i = Obj.Lower(); i <= Sel->NbStored(); i++ ) {
    Tr = Obj(i);
    if ( !Tr.IsNull() ) {
      Ownr = * ( (Handle(SelectMgr_EntityOwner) * ) &Tr );
      Handle(AIS2D_InteractiveObject) IO;
      if ( Ownr->HasSelectable() ) 
 	    if ( !Ownr->ComesFromDecomposition() ) {
    	  Handle(SelectMgr_SelectableObject) SO  = Ownr->Selectable();
	      Handle(AIS2D_InteractiveObject) IO = 
			   * ( (Handle(AIS2D_InteractiveObject) * ) &SO );
	      if ( myICTX->IsInCollector(IO) ) {
	        PM = myICTX->CollectorPrsMgr();
	        updColl = Standard_True;
		  } else
	        updMain = Standard_True;
		} else
	       updMain = Standard_True;
	  else
    	updMain = Standard_True;
	  // endif HasSelectable
      Handle(SelectMgr_SelectableObject) SO = Ownr->Selectable();
      Standard_Integer HM = GetHiMod( * ( (Handle(AIS2D_InteractiveObject ) * ) &SO ) );
 //     Ownr->HilightWithColor( PM, myICTX->SelectionColor(), HM );
	  if ( Ownr->HasSelectable() ) {
	      Quantity_NameOfColor aCol = myICTX->SelectionColor();
		  Standard_Integer indCol = myICTX->CurrentViewer()->InitializeColor( aCol );
          PM->SetHighlightColor( indCol );
		  PM->ColorHighlight( SO, indCol, HM );
	  }
		  
	} //endif 
  }	//end for
  if (UpdateVwr) {
    if ( updMain ) myICTX->CurrentViewer()->Update();
    if ( updColl ) myICTX->Collector()->Update();
  } */
}

void AIS2D_LocalContext::UnhighlightPicked( const Standard_Boolean /*UpdateVwr*/ ) {

  /*Standard_Boolean updMain(Standard_False), updColl(Standard_False);
  Quantity_NameOfColor SelCol = myICTX->SelectionColor();
  
  Handle(AIS2D_Selection) Sel = AIS2D_Selection::Selection(mySelName.ToCString());
  Handle(Standard_Transient) Tr;
  Handle(SelectMgr_EntityOwner) Ownr;
  Handle(PrsMgr_PresentationManager2d) PM = myMainPM;
  
  const TColStd_Array1OfTransient& Obj = Sel->Objects()->Array1();
  for ( Standard_Integer i = Obj.Lower(); i <= Sel->NbStored(); i++ ) {
    Tr = Obj(i);
    if ( !Tr.IsNull() ) {
      Ownr = * ( (Handle(SelectMgr_EntityOwner) * ) &Tr );
      Standard_Integer HM(0);
      if ( Ownr->HasSelectable() ) 
		 if ( !Ownr->ComesFromDecomposition() ) {
	       Handle(SelectMgr_SelectableObject) SO  = Ownr->Selectable();
           Handle(AIS2D_InteractiveObject) IO = 
			   * ( (Handle(AIS2D_InteractiveObject) * ) &SO );
	       HM = GetHiMod(IO);
	       if ( myICTX->IsInCollector(IO) ) {
	         PM = myICTX->CollectorPrsMgr();
	         updColl = Standard_True;
		   } else
	         updMain = Standard_True;
		 } else
	       updMain = Standard_True;
	 // endif HasSelectable
      Ownr->Unhilight( PM, HM );
    }  //end if
  }	//end for
  
  if ( UpdateVwr ) {
    if ( updMain ) myICTX->CurrentViewer()->Update();
    if ( updColl ) myICTX->Collector()->Update();
  } */
}

Standard_Boolean AIS2D_LocalContext::IsValidForSelection(const Handle(AIS2D_InteractiveObject)& /*anIObj*/) const {
  
  // return myFilter->IsOk(new SelectMgr_EntityOwner(anIObj));
    return Standard_False;
}

void AIS2D_LocalContext::UpdateSelected(const Standard_Boolean UpdateVwr) {
  
  UnhighlightPicked(Standard_False);
  HighlightPicked(UpdateVwr);
}

void AIS2D_LocalContext::SetSelected( const Handle(AIS2D_InteractiveObject)& /*anIObj*/,
				   const Standard_Boolean /*UpdateVwr*/ ) {

  /*if ( !IsValidForSelection(anIObj) ) return;
  UnhighlightPicked(Standard_False);
  
  Handle(AIS2D_Selection) sel = AIS2D_Selection::Selection(mySelName.ToCString());
  Standard_Boolean found(Standard_False);
  Handle(Standard_Transient) Tr;
  Handle(SelectMgr_EntityOwner) EntOwr = FindSelectedOwnerFromIO(anIObj);
  if ( EntOwr.IsNull() ) {
     if ( anIObj->HasSelection(0) ) {
      const Handle(SelectMgr_Selection)& SIObj = anIObj->Selection(0);
      SIObj->Init();
      if ( SIObj->More() ) {
    	Handle(SelectBasics_EntityOwner) BO = SIObj->Sensitive()->OwnerId();
    	EntOwr = *( (Handle(SelectMgr_EntityOwner) * ) & BO );
      }
    }
    if ( EntOwr.IsNull() ) 
      EntOwr = new SelectMgr_EntityOwner(anIObj);
  }
  
  ClearSelected(Standard_False);
  EntOwr->State(1);
  AIS2D_Selection::Select(EntOwr);
  HighlightPicked(UpdateVwr);
  */
}

void AIS2D_LocalContext::AddOrRemoveSelected(
					const Handle(AIS2D_InteractiveObject)& /*anIObj*/,
					const Standard_Boolean /*UpdateVwr*/) {

  /*if ( !IsValidForSelection(anIObj) ) return;
  UnhighlightPicked(Standard_False);
  Handle(SelectMgr_EntityOwner) EntOwr;

  EntOwr = FindSelectedOwnerFromIO(anIObj);
  if ( !EntOwr.IsNull() )
    EntOwr->State(0);
  else {
    if ( anIObj->HasSelection(0) ) {
      const Handle(SelectMgr_Selection)& SIObj = anIObj->Selection(0);
      SIObj->Init();
      if ( SIObj->More() ) {
	    Handle(SelectBasics_EntityOwner) BO = SIObj->Sensitive()->OwnerId();
		EntOwr = * ( ( Handle(SelectMgr_EntityOwner)* )& BO );      	
	  }
    }
    if ( EntOwr.IsNull() ) 
      EntOwr = new SelectMgr_EntityOwner(anIObj);
    EntOwr->State(1);
  }
  
  const Handle(AIS2D_Selection)& aSel = AIS2D_Selection::Selection(mySelName.ToCString() );
  if ( !aSel.IsNull() )
    aSel->Select(EntOwr);
  HighlightPicked(UpdateVwr);
  */
}

Standard_Integer AIS2D_LocalContext::DetectedIndex() {
  return myLastIndex;
}

const Handle(AIS2D_LocalStatus)& AIS2D_LocalContext::Status(const Handle(AIS2D_InteractiveObject)& anObject) const {
  return myActiveObjects(anObject);
}

