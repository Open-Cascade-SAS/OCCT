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

#define IMP140901       //GG 14/09/01 goback to avoid a regression
//			See V2d_Viewer

// SAV 14/11/01 - modification for detection/highlighting/selection elements of
//                SetOf<>. Fully used for SetOfMarkers.

#define OCC197 // jfa 01/03/2002 // for multiple selection of elements
#define OCC146 // jfa 01/03/2002 // for single selection two times
#define OCC154 // jfa 06/03/2002 // for correct erasing
#define OCC389 // vro 19/06/2002 // for Display(obj,dispMode...) setting a view for the IO

#include <Standard_Stream.hxx>

#include <AIS2D_InteractiveContext.ixx>
#include <Aspect_ColorMap.hxx>
#include <Aspect_GenericColorMap.hxx>
#include <Aspect_FontMap.hxx>
#include <Aspect_MarkMap.hxx>
#include <Aspect_TypeMap.hxx>
#include <Aspect_WidthMap.hxx>
#include <Aspect_FontStyle.hxx>
#include <Aspect_LineStyle.hxx>
#include <Aspect_TypeOfMarker.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <Aspect_WidthOfLine.hxx>
#include <Aspect_ColorMapEntry.hxx>
#include <Aspect_TypeMapEntry.hxx>
#include <Aspect_WidthMapEntry.hxx>
#include <Aspect_FontMapEntry.hxx>
#include <Aspect_MarkMapEntry.hxx>
#include <gp_GTrsf2d.hxx>
#include <gp_Trsf2d.hxx>

#include <Graphic2d_Primitive.hxx>
#include <Graphic2d_SequenceOfPrimitives.hxx>

#include <Graphic2d_View.hxx>
#include <Graphic2d_DisplayList.hxx>
#include <V2d_View.hxx>
#include <Prs2d_Point.hxx>
#include <Prs2d_Drawer.hxx>
#include <Prs2d_AspectLine.hxx>

#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_MapIteratorOfMapOfTransient.hxx>
#include <TColStd_MapOfTransient.hxx>
#include <TColStd_HSequenceOfInteger.hxx>
#include <AIS2D_DataMapIteratorOfDataMapOfLC.hxx>
#include <AIS2D_DataMapIteratorOfDataMapOfIOStatus.hxx>
#include <AIS2D_ListIteratorOfListOfIO.hxx>
#include <AIS2D_PrimitiveArchit.hxx>
#include <AIS2D_HSequenceOfPrimArchit.hxx>
#include <AIS2D_GlobalStatus.hxx>
#include <AIS2D_LocalStatus.hxx>
#include <AIS2D_LocalContext.hxx>
#include <AIS2D_ProjShape.hxx>

//SAV
#include <TColStd_MapOfInteger.hxx>
#include <TColStd_MapIteratorOfMapOfInteger.hxx>

static TColStd_ListIteratorOfListOfInteger ItL;
static Standard_Boolean RectMoveTo = Standard_False;

AIS2D_InteractiveContext::AIS2D_InteractiveContext(const Handle(V2d_Viewer)& MainViewer):
 
    myMainVwr( MainViewer ),
    myDetectPrecision( 4 ),
    myResetDetect( Standard_True ),
    myLastPicked(),
    myLastinMain(),
    myLastinColl(),
    mySeqOfDetIO( new AIS2D_HSequenceOfIO() ),
    mySeqOfSelIO( new AIS2D_HSequenceOfIO() ),
    myCurDetectMode( AIS2D_TOD_NONE ),
    myWasLastMain(),
    myPickMode( Graphic2d_PM_INCLUDE ),
    myHighlightColor( Quantity_NOC_CYAN1 ),
    mySelectionColor( Quantity_NOC_GRAY80 ),
    mySubIntensity( Quantity_NOC_GRAY40 ),
    myDisplayMode( 0 ),
    myLocalContexts( ),
    myCurLocalIndex( 0 ),
    myLastMoveView() ,
    myCurSelected( 0 ),  
     myMaxSelected( 0 ) 
{

  myMainVwr->View()->SetDefaultOverrideColor( myMainVwr->InitializeColor( mySelectionColor ) );
}

AIS2D_InteractiveContext::AIS2D_InteractiveContext(const Handle(V2d_Viewer)& MainViewer,const Handle(V2d_Viewer)& aCollector):
 
    myMainVwr( MainViewer ), 
	
    myCollectorVwr( aCollector ),
    myIsCollClosed( Standard_False ),
    myDetectPrecision( 4 ),
    myResetDetect( Standard_True ),
    myLastPicked(),
    myLastinMain(),
    myLastinColl(),
    mySeqOfDetIO( new AIS2D_HSequenceOfIO() ),
    mySeqOfSelIO( new AIS2D_HSequenceOfIO() ),
    myCurDetectMode( AIS2D_TOD_NONE ),
    myWasLastMain(),
    myPickMode( Graphic2d_PM_INCLUDE ),
    myHighlightColor( Quantity_NOC_CYAN1 ),
    mySelectionColor( Quantity_NOC_GRAY80 ),
    mySubIntensity( Quantity_NOC_GRAY40 ),
    myDisplayMode( 0 ),
    myLocalContexts( ),
    myCurLocalIndex( 0 ),
    myLastMoveView()
{  
  myMainVwr->View()->SetDefaultOverrideColor( myMainVwr->InitializeColor( mySelectionColor ) );
  myCollectorVwr->View()->SetDefaultOverrideColor( myMainVwr->InitializeColor( mySelectionColor ) );
}


 //*************************************************************   
 //          Management of the Trash Bin Viewer
 // *************************************************************

 void AIS2D_InteractiveContext::OpenCollector() {
	 myIsCollClosed = Standard_False;
 }

 
 // *************************************************************
 //        Editing general Display Services 
 // *************************************************************
  
void AIS2D_InteractiveContext::Display( const Handle(AIS2D_InteractiveObject)& anIObj, 
	                                    const Standard_Boolean UpdateVwr ) {
    
 if ( anIObj.IsNull() ) return;

 Standard_Integer DispMode, SelMode;
 AIS2D_TypeOfDetection HiMod;
 Standard_Boolean UpdateCol;
 Handle( AIS2D_GlobalStatus ) GStatus;

 GetDefModes( anIObj, DispMode, HiMod, SelMode );
  
 if ( !anIObj->HasInteractiveContext() ) 
      anIObj->SetContext( this );
   
 anIObj->SetView( myMainVwr->View() );

 if ( !HasOpenedContext() ) {

  if ( IsDisplayed( anIObj ) ) return;
  UpdateCol = Standard_False;
     
  if ( !myObjects.IsBound( anIObj ) ) {

     GStatus = new AIS2D_GlobalStatus( AIS2D_DS_Displayed, DispMode, 0);
     myObjects.Bind( anIObj, GStatus );
     DrawObject( anIObj, DispMode ) ;
     
  } else {
  
     GStatus = myObjects( anIObj );
     switch ( GStatus->GraphicStatus() ) {

     case AIS2D_DS_Erased: {

      if ( GStatus->IsHighlight() )
        UpdateCol = UpdateVwr ;
     }
     case AIS2D_DS_FullErased: {

      for ( ItL.Initialize( * GStatus -> DisplayedModes()); ItL.More(); ItL.Next()) 
   	    if ( GStatus->IsSubIntensityOn() ) {
        //          Standard_Integer indCol = myMainVwr->InitializeColor(mySubIntensity);
		//		 myMainPM->SetHighlightColor(indCol);    
		//		 myMainPM->ColorHighlight( anIObj, indCol, ItL.Value() );
        }
 
  	  for ( ItL.Initialize( * GStatus -> SelectionModes() ); ItL.More();ItL.Next() )
          //  mySelectorMgr->Activate( anIObj, ItL.Value(), myMainSelector );
	        GStatus->SetGraphicStatus( AIS2D_DS_Displayed );
	        if ( GStatus->IsHighlight() )
	             Highlight( anIObj, HiMod );
	        break;
     }
     default:
     break;
     } // end switch
  }  //endif !myObjects.IsBound  
   
  anIObj->Display();

  if ( UpdateCol && !myCollectorVwr.IsNull() ) myCollectorVwr->UpdateNew();

 } else {

   myLocalContexts( myCurLocalIndex )->Display( anIObj, DispMode, Standard_True,SelMode );

 }//endif !HasOpenedContext
   
 if ( UpdateVwr ) myMainVwr->UpdateNew();

}

void AIS2D_InteractiveContext::Display(
	                const Handle(AIS2D_InteractiveObject)& anIObj,
	                const Standard_Integer                 aDispMode,
	                const Standard_Integer                 aSelectMode,
	                const Standard_Boolean                 UpdateVwr,
	                const Standard_Boolean                 AllowDecomp) {

   if ( anIObj.IsNull() ) return;
   if ( !anIObj->HasInteractiveContext() ) anIObj->SetContext(this);

#ifdef OCC389
   anIObj->SetView(myMainVwr->View());
#endif
   
   if ( !HasOpenedContext() ) {
    
	if ( !anIObj->HasDisplayMode() )
          anIObj->SetDisplayMode(aDispMode);
    
	if ( !myObjects.IsBound(anIObj) ) {
      Handle(AIS2D_GlobalStatus) GStatus=
	     new AIS2D_GlobalStatus( AIS2D_DS_Displayed, aDispMode, aSelectMode );
      myObjects.Bind( anIObj, GStatus );
     DrawObject( anIObj, aDispMode );

     if ( aSelectMode != -1 ) {
    	//if ( !mySelectorMgr->Contains( anIObj ) )
	     // mySelectorMgr->Load( anIObj, myMainSelector,aSelectMode );
	    //mySelectorMgr->Activate( anIObj, aSelectMode, myMainSelector );
     }
     
	  anIObj->SetSelectionMode(aSelectMode);

    } else {
       
      Handle(AIS2D_GlobalStatus) GStatus = myObjects( anIObj );
      if ( !GStatus->IsDModeIn( aDispMode ) ) {
    	DrawObject( anIObj, aDispMode );
    	GStatus->AddDisplayMode( aDispMode );
		if ( GStatus->IsSubIntensityOn() ) {
#ifdef DEB
		   Standard_Integer indCol = myMainVwr->InitializeColor(mySubIntensity);
           //myMainPM->SetHighlightColor( indCol );
	       //myMainPM->ColorHighlight( anIObj, indCol, aDispMode );
#endif
		}
      }
      if ( aSelectMode != -1 ) {
	    GStatus->AddSelectionMode( aSelectMode );
	    //mySelectorMgr->Activate( anIObj, aSelectMode, myMainSelector );
      }
     
    }
   anIObj->Display();
  }	else 
     
     myLocalContexts(myCurLocalIndex)->Display( anIObj, aDispMode, AllowDecomp, aSelectMode);
      
  if (UpdateVwr) myMainVwr->Update();

}

void AIS2D_InteractiveContext::Redisplay( const Handle(AIS2D_InteractiveObject)& anIObj,
                     				      const Standard_Boolean updateVwr,
				                          const Standard_Boolean /*allModes*/ ) {
  
  if ( anIObj.IsNull() ) return;
  //anIObj->Update(allModes);
  
  if ( updateVwr )  {
    if ( HasOpenedContext() )
      myMainVwr->Update();
    else {
      if ( myObjects.IsBound( anIObj ) ) {
	   switch ( myObjects(anIObj)->GraphicStatus() ) {
	     case AIS2D_DS_Displayed:
             DrawObject( anIObj, 0 );
	         myMainVwr->Update();
	         break;
	     case AIS2D_DS_Erased: 
	       if ( !myCollectorVwr.IsNull() )
	          myCollectorVwr->Update();
	     default:
	     break;
       }
      }
    }
  }

}

void AIS2D_InteractiveContext::Load(
	          const Handle(AIS2D_InteractiveObject)& anIObj,
	          const Standard_Integer SelMode,
	          const Standard_Boolean AllowDecomp) {

  if ( anIObj.IsNull() ) return;
  if ( !anIObj->HasInteractiveContext() ) anIObj->SetContext( this );
 
  if ( !HasOpenedContext() ) {
    if ( SelMode == -1 && !AllowDecomp ) {
      Standard_Integer DispMode;
      AIS2D_TypeOfDetection HiMod;
      Standard_Integer selMode = SelMode;
      
      GetDefModes( anIObj, DispMode, HiMod, selMode );
      Handle(AIS2D_GlobalStatus) GStatus= 
	     new AIS2D_GlobalStatus( AIS2D_DS_FullErased, DispMode, selMode );
      myObjects.Bind( anIObj,GStatus );      
      return;
	}
  }
  //endif !HasOpenedContext
  myLocalContexts(myCurLocalIndex)->Load( anIObj, AllowDecomp, SelMode);
}

void AIS2D_InteractiveContext::Erase(
	              const Handle(AIS2D_InteractiveObject)& anIObj,
	              const Standard_Boolean UpdateVwr,
				  const Standard_Boolean /*PutInCollector*/) {
	   
  if ( anIObj.IsNull() ) return;
  
  if ( !HasOpenedContext() )
#ifdef OCC154
  {
    anIObj->Erase();
    if (myObjects.IsBound(anIObj)) 
      myObjects(anIObj)->SetGraphicStatus(AIS2D_DS_Erased);
  }
#else
     anIObj->Remove();
#endif
  if ( UpdateVwr ) myMainVwr->UpdateNew();
   
}

void AIS2D_InteractiveContext::EraseAll (const Standard_Boolean /*PutInCollector*/, 
                                         const Standard_Boolean UpdateVwr)
{
  if ( !HasOpenedContext() )
#ifdef OCC154
  {
    myMainVwr->View()->Erase();
    AIS2D_DataMapIteratorOfDataMapOfIOStatus anItr (myObjects);
    for (; anItr.More(); anItr.Next())
    {
      anItr.Value()->SetGraphicStatus(AIS2D_DS_Erased);
    }
  }
#else
     myMainVwr->View()->Remove();
#endif
  if ( UpdateVwr ) myMainVwr->Update();
}

void AIS2D_InteractiveContext::DisplayAll (const Standard_Boolean /*OnlyFromCollector*/,
                                           const Standard_Boolean /*updateVwr*/) {

}

void AIS2D_InteractiveContext::EraseSelected (const Standard_Boolean PutInCollector,
                                              const Standard_Boolean UpdateVwr) {
  if ( !HasOpenedContext() ) {
    Standard_Boolean found = Standard_False;
    if ( mySeqOfSelIO->IsEmpty() ) return;
    for ( int i = 1; i <= mySeqOfSelIO->Length(); ++i )
         Erase( mySeqOfSelIO->Value(i), Standard_False, PutInCollector );
         found = Standard_True;
    if ( found && UpdateVwr ) {
      myMainVwr->Update();
      if ( PutInCollector && !myCollectorVwr.IsNull() )
	    myCollectorVwr->Update();
	}
  }

}

void AIS2D_InteractiveContext::DisplaySelected( const Standard_Boolean updateVwr ) {

  if ( !HasOpenedContext() ) {
    Standard_Boolean found = Standard_False;
    if ( mySeqOfSelIO->IsEmpty() ) return;
    for ( int i = 1; i <= mySeqOfSelIO->Length(); ++i ) {
      Display( mySeqOfSelIO->Value(i), Standard_False );
      found = Standard_True;
    }
    if ( found && updateVwr ) {
      myMainVwr->Update();
      if ( !(myIsCollClosed && myCollectorVwr.IsNull()) ) 
    	myCollectorVwr->Update();
    }
  }

}

Standard_Boolean AIS2D_InteractiveContext::KeepTemporary( const Handle(AIS2D_InteractiveObject)& anIObj,
                                                          const Standard_Integer WhichContext ) { 

  if ( anIObj.IsNull() ) return Standard_False;

  if ( !HasOpenedContext() ) return Standard_False;
  if ( myObjects.IsBound(anIObj) ) return Standard_False;
  if ( WhichContext != -1 && !myLocalContexts.IsBound(WhichContext) ) return Standard_False;
  
  Standard_Integer IsItInLocal = myCurLocalIndex;
  Standard_Boolean Found( Standard_False );

  while ( IsItInLocal > 0 && !Found ) {
    if ( !myLocalContexts.IsBound( IsItInLocal ) )
      IsItInLocal--;
    else if ( myLocalContexts(IsItInLocal)->IsIn( anIObj ) )
      Found = Standard_True;
    else
      IsItInLocal--;
  }

  if ( !Found ) return Standard_False;
  
  Handle(AIS2D_LocalStatus) LS = myLocalContexts(IsItInLocal)->Status(anIObj);
  
  
  if ( LS->IsTemporary() ){
    Standard_Integer DM,SM;
    AIS2D_TypeOfDetection HM;
    GetDefModes( anIObj, DM, HM, SM );
    
    SM = LS->SelectionModes()->IsEmpty() ? SM : LS->SelectionModes()->First();
    if (LS->DisplayMode() != DM ) {
      Standard_Integer LSM =  LS->SelectionModes()->IsEmpty() ? -1 : LS->SelectionModes()->First();
      myLocalContexts(IsItInLocal)->Display(anIObj,DM,LS->Decomposed(),LSM);
    }

    Handle(AIS2D_GlobalStatus) GS = new AIS2D_GlobalStatus( AIS2D_DS_Displayed,
							                                DM, SM, Standard_False);
    myObjects.Bind( anIObj, GS );
    
    LS->SetTemporary(Standard_False);
  }				    
  return Standard_True;
}

Standard_Boolean AIS2D_InteractiveContext::IsSelected(const Handle(AIS2D_InteractiveObject)& anIObj) const {

  if ( anIObj.IsNull() ) return Standard_False;
  if ( !HasOpenedContext() ) 
    return ( anIObj->State() == 1 );
  else 
    return myLocalContexts(myCurLocalIndex)->IsSelected(anIObj);
}

Standard_Boolean AIS2D_InteractiveContext::IsCurrent(const Handle(AIS2D_InteractiveObject)& anIObj) const {
                       
  if ( anIObj.IsNull() ) return Standard_False;
  return ( anIObj->State() );
}

Standard_Integer AIS2D_InteractiveContext::OpenLocalContext( const Standard_Boolean UseDispObj,
                                                             const Standard_Boolean AllowDecomposit,
                                                             const Standard_Boolean AcceptEraseOfObj ) {
  if ( !IsCurrent( myLastPicked ) ) 
    if ( !myLastPicked.IsNull() ) {
      myLastPicked->Unhighlight();
	 }
  
  if ( !myLastMoveView.IsNull() )
    if ( myCurLocalIndex > 0 )
      myLocalContexts( myCurLocalIndex )->UnhighLastDetect( myLastMoveView  );
    
  myLastinMain.Nullify();
  myLastinColl.Nullify();
  myLastPicked.Nullify();
  myWasLastMain = Standard_True;
  
//  Standard_Integer untilNow = myCurLocalIndex;
  
  myCurLocalIndex = HighestIndex() + 1;
  
  Handle(AIS2D_LocalContext) NewLocal= new AIS2D_LocalContext(this,
	                           myCurLocalIndex,
							   UseDispObj,
							   AllowDecomposit,
							   AcceptEraseOfObj );
   
  myLocalContexts.Bind( myCurLocalIndex, NewLocal );
 
  if ( UseDispObj ) {
     if ( AllowDecomposit )
      cout<<"\t\tDecomposition Authorized for Loaded Shapes"<<endl;
     else
      cout<<"\t\tNo Decomposition Authorized for Loaded Shapes"<<endl;
  } else
    cout<<"\t\tNo Objects Were Loaded "<<endl;

  return myCurLocalIndex;

}

void AIS2D_InteractiveContext::CloseLocalContext( const Standard_Integer ind,
                                                  const Standard_Boolean updateVwr ) {

 Standard_Integer GoodIndex = ( ind == -1 ) ? myCurLocalIndex : ind;
 if ( !HasOpenedContext() ) return;
 if ( !myLocalContexts.IsBound(GoodIndex) ) return;
 
 if ( myLocalContexts.Extent() == 1 && GoodIndex == myCurLocalIndex ) {
   
   myLocalContexts(myCurLocalIndex)->Terminate();
   myLocalContexts.UnBind(myCurLocalIndex);
   myCurLocalIndex = 0;

   ResetOriginalState(Standard_False);
 
 } else {

   myLocalContexts(GoodIndex)->Terminate();
   myLocalContexts.UnBind(GoodIndex);
   
   if ( GoodIndex == myCurLocalIndex ) {
     myCurLocalIndex = HighestIndex();
   }
   
 }

 if ( updateVwr ) myMainVwr->Update();

}

Standard_Integer AIS2D_InteractiveContext::IndexOfCurrentLocal() const {
  	 return myCurLocalIndex;
}

void AIS2D_InteractiveContext::CloseAllContext( const Standard_Boolean /*updateVwr*/ ) {



}

void AIS2D_InteractiveContext::SetCurrentObject( const Handle(AIS2D_InteractiveObject)& anIObj,
                                                 const Standard_Boolean UpdateVwr ) {

  if ( anIObj.IsNull() ) return;
    
  /*if ( NbCurrents() == 1 && anIObj->State() == 1 ) {
    Quantity_NameOfColor HiCol;
    Standard_Boolean HasHiCol;
    if ( IsHighlighted( anIObj, HasHiCol, HiCol ) )
      if ( HasHiCol && HiCol != mySelectionColor ) 
	      Highlight( anIObj, UpdateVwr );
    return;
  }
  */
  if ( !HasOpenedContext() ) {
    if ( !myObjects.IsBound(anIObj) ) 
      HighlightWithColor( anIObj, mySelectionColor, Standard_False );
    
    anIObj->SetState(1);
    Quantity_NameOfColor HiCol;
    Standard_Boolean HasHiCol;
    if ( IsHighlighted( anIObj, HasHiCol, HiCol ) ) {
      if ( HasHiCol && HiCol != mySelectionColor ) 
	    Highlight( anIObj,UpdateVwr );
    } else {
      Highlight( anIObj, UpdateVwr );
    }
    //if (UpdateVwr) 
    //  UpdateCurrentViewer();
  } 

}

void AIS2D_InteractiveContext::AddOrRemoveCurObject( const Handle(AIS2D_InteractiveObject)& anIObj,
                                                     const Standard_Boolean UpdateVwr ) {


  if ( !HasOpenedContext() ) {
    if ( anIObj.IsNull() ) return;

    switch ( anIObj->HighlightMode() ) { 
    default:
    case AIS2D_TOD_OBJECT: {
      if ( IsIOSelected( anIObj ) ) {
        Unhighlight( anIObj, Standard_False );
        anIObj->SetState( 0 );
        for ( int i = 1; i <= mySeqOfSelIO->Length(); ++i )
            if ( mySeqOfSelIO->Value(i) == anIObj ) {
                mySeqOfSelIO->Remove(i);             
                break;   
             }
      } else {
        anIObj->SetState( 1 );
        Highlight( anIObj, Standard_False );
        mySeqOfSelIO->Append(anIObj);
      }
     }
     break;                 
    case AIS2D_TOD_PRIMITIVE: {
      Standard_Boolean found = Standard_False;
      Handle(AIS2D_HSequenceOfPrimArchit) thePA;
      Handle(Graphic2d_Primitive) thePrim = anIObj->Primitive(anIObj->PickedIndex());
      thePA = anIObj->GetSelectedSeqPrim();
      for ( int i = 1; i <= thePA->Length(); ++i )
          if ( thePA->Value(i)->GetPrimitive() == thePrim ) {
              //thePrim->Unhighlight();    
              Unhighlight( anIObj, Standard_False );
              thePA->Remove(i);
              found = Standard_True;
              break;
          } 
      if ( !found ) {
         Highlight( anIObj, Standard_False );
         thePrim->Highlight();
         anIObj->AddSelectPrim( thePrim, 0 );
         if ( ! IsIOSelected( anIObj ) ) {
            mySeqOfSelIO->Append(anIObj);  
            anIObj->SetState( 1 );          
         }
      }
     }
     break;
    case AIS2D_TOD_ELEMENT:{
      Standard_Boolean found = Standard_False;
      Handle(AIS2D_HSequenceOfPrimArchit) thePA;
      Handle(Graphic2d_Primitive) thePrim = anIObj->Primitive(anIObj->PickedIndex());
      Standard_Integer ind = thePrim->PickedIndex();
      thePA = anIObj->GetSelectedSeqPrim();
      for ( int i = 1; i <= thePA->Length(); ++i )
          if ( thePA->Value(i)->GetPrimitive() == thePrim &&
               thePA->Value(i)->GetIndex() == ind ) {
#ifdef OCC197
            // unhighlight the element
            // thePrim->Unhighlight(ind);
            Handle(TColStd_HSequenceOfInteger) aHiInds = thePrim->HighlightIndices();
            for (int k = 1; k <= aHiInds->Length(); k++)
            {
              if (aHiInds->Value(k) == ind)
              {
                aHiInds->Remove(k);
                break;
              }
            }
            // unhighlight entire object only if we remove last selected element of it
            if (thePA->Length() == 1)
#endif
               Unhighlight( anIObj, Standard_False );
               thePA->Remove(i);
               found = Standard_True;
              break;
          }
      if ( !found ) {
         Highlight( anIObj, Standard_False );
         thePrim->Highlight( ind );
         anIObj->AddSelectPrim( thePrim, ind );
         if ( ! IsIOSelected( anIObj ) ) {
            mySeqOfSelIO->Append(anIObj);  
            anIObj->SetState( 1 );          
         }
      }
     }
     break;
    case AIS2D_TOD_VERTEX: {
      Standard_Boolean found = Standard_False;
      Handle(AIS2D_HSequenceOfPrimArchit) thePA;
      Handle(Graphic2d_Primitive) thePrim = anIObj->Primitive(anIObj->PickedIndex());
      Standard_Integer ind = thePrim->PickedIndex();
      thePA = anIObj->GetSelectedSeqPrim();
      for ( int i = 1; i <= thePA->Length(); ++i )
          if ( thePA->Value(i)->GetPrimitive() == thePrim &&
               thePA->Value(i)->GetIndex() == ind ) {
#ifdef OCC197
            // unhighlight the vertex
            // thePrim->Unhighlight(ind);
            Handle(TColStd_HSequenceOfInteger) aHiInds = thePrim->HighlightIndices();
            for (int k = 1; k <= aHiInds->Length(); k++)
            {
              if (aHiInds->Value(k) == ind)
              {
                aHiInds->Remove(k);
                break;
              }
            }
            // unhighlight entire object only if we remove last selected vertex of it
            if (thePA->Length() == 1)
#endif
               Unhighlight( anIObj, Standard_False );
               thePA->Remove(i);
               found = Standard_True;
              break;
          } 
      if ( !found ) {
         Highlight( anIObj, Standard_False );
         thePrim->Highlight( ind );
         anIObj->AddSelectPrim( thePrim, ind );
         if ( ! IsIOSelected( anIObj ) ) {
            mySeqOfSelIO->Append(anIObj);  
            anIObj->SetState( 1 );          
         }
       }
     }
     break;
      
     } // end switch

#ifdef OCC197
    if ( UpdateVwr ) myWasLastMain ? myMainVwr->Update() : UpdateCollector();
#else
    if ( UpdateVwr ) UpdateCurrentViewer();
#endif
  } else {

//    cout<<"Nothing Done : Opened Local Context"<<endl;
  }

}

void AIS2D_InteractiveContext::UpdateCurrent() {


}

void AIS2D_InteractiveContext::SetOkCurrent() {

}

/*Handle(AIS2D_InteractiveObject) AIS2D_InteractiveContext::FirstCurrentObject() {
  Handle(AIS2D_InteractiveObject) IO ;
  InitCurrent();
  if ( MoreCurrent() )
    return Current();
  return IO;
}
*/
void AIS2D_InteractiveContext::HighlightCurrent(const Standard_Boolean UpdateVwr) {
  if ( mySeqOfSelIO->IsEmpty() ) return;
  Handle(AIS2D_InteractiveObject) IO;
  AIS2D_TypeOfDetection theTOD;
  for ( int i = 1; i <= mySeqOfSelIO->Length(); ++i ) {
      IO = mySeqOfSelIO->Value(i);
      theTOD = IO->HighlightMode();
      if ( theTOD == AIS2D_TOD_OBJECT )
           Highlight(IO,Standard_False);
      else if ( theTOD == AIS2D_TOD_PRIMITIVE || 
                theTOD == AIS2D_TOD_ELEMENT ||
                theTOD == AIS2D_TOD_VERTEX ) {
           Standard_Integer pLength = IO->PickList()->Length();
           if ( pLength ) {
               IO->Highlight(myMainVwr->InitializeColor( mySelectionColor ));
               IO->Unhighlight();
               for ( int j = 1; j <= pLength; j++ )    
                  IO->Primitive(IO->PickList()->Value(j))->Highlight();
           }                
      }        
  } // end for
  if ( UpdateVwr ) 
         UpdateCurrentViewer();

}

void AIS2D_InteractiveContext::UnhighlightCurrent(const Standard_Boolean UpdateVwr) {
  if ( mySeqOfSelIO->IsEmpty() ) return;
  for ( int i = 1; i <= mySeqOfSelIO->Length(); ++i ) {
     Unhighlight( mySeqOfSelIO->Value(i), Standard_False );
     mySeqOfSelIO->Value(i)->SetState(0);
  }
  if ( UpdateVwr )
    UpdateCurrentViewer();
}


void AIS2D_InteractiveContext::ClearCurrent(const Standard_Boolean UpdateVwr) {
  if ( mySeqOfSelIO->IsEmpty() ) return;
  for ( int i = 1; i <= mySeqOfSelIO->Length(); ++i ) 
     Unhighlight( mySeqOfSelIO->Value(i), Standard_False );
        
  if ( UpdateVwr )
    UpdateCurrentViewer();
}

void AIS2D_InteractiveContext::SetSelectedCurrent() {

}

// SAV - just appending object
void AIS2D_InteractiveContext::SetSelected( const Handle(AIS2D_InteractiveObject)& obj,
					    const Standard_Boolean update )
{
  mySeqOfSelIO->Append( obj );
  for( Standard_Integer i = 1; i <= obj->Length(); i++ )
    obj->AddSelectPrim( obj->Primitive( i ), i );
}

void AIS2D_InteractiveContext::UpdateSelected( const Standard_Boolean UpdateVwr ) {
  
  if ( !HasOpenedContext() ) {
    HighlightSelected();
  }
  myLocalContexts(myCurLocalIndex)->UpdateSelected( UpdateVwr );
}

void AIS2D_InteractiveContext::AddOrRemoveSelected( 
				  const Handle(AIS2D_InteractiveObject)& anIObj,
				  const Standard_Boolean UpdateVwr ) {

  if ( !anIObj->HasInteractiveContext() ) anIObj->SetContext(this);
  if ( ! HasOpenedContext() )
    AddOrRemoveCurObject( anIObj, UpdateVwr );
  else
    myLocalContexts(myCurLocalIndex)->AddOrRemoveSelected( anIObj, UpdateVwr);
  
}

void AIS2D_InteractiveContext::HighlightSelected( const Standard_Boolean UpdateVwr ) {
  
  if ( !HasOpenedContext() ) {
    if ( ! mySeqOfSelIO->IsEmpty() ) 
        for ( int i = 1; i <= mySeqOfSelIO->Length(); ++i )
           Highlight( mySeqOfSelIO->Value(i), Standard_False );
  }	else {
     myLocalContexts(myCurLocalIndex)->UnhighlightPicked( UpdateVwr );
  } 
  if( UpdateVwr )
    UpdateCurrentViewer();
}

void AIS2D_InteractiveContext::UnhighlightSelected( const Standard_Boolean UpdateVwr ) {
  if ( !HasOpenedContext() ) {
    if ( ! mySeqOfSelIO->IsEmpty() ) 
        for ( int i = 1; i <= mySeqOfSelIO->Length(); ++i )
           Unhighlight( mySeqOfSelIO->Value(i), Standard_False );
  }	else {
     myLocalContexts(myCurLocalIndex)->UnhighlightPicked( UpdateVwr );
  } 
  if( UpdateVwr )
    UpdateCurrentViewer();
}

void AIS2D_InteractiveContext::ClearSelected( const Standard_Boolean UpdateVwr ) {
 if ( !HasOpenedContext() )
    ClearCurrent( UpdateVwr );
 else 
    myLocalContexts(myCurLocalIndex)->ClearSelected( UpdateVwr );
}

AIS2D_DisplayStatus AIS2D_InteractiveContext::DisplayStatus( const Handle(AIS2D_InteractiveObject)& anIObj ) const {

  if ( anIObj.IsNull() ) return AIS2D_DS_None;

  if ( myObjects.IsBound(anIObj))
    return myObjects(anIObj)->GraphicStatus();

  AIS2D_DataMapIteratorOfDataMapOfLC ItM(myLocalContexts);
  for ( ; ItM.More(); ItM.Next() ) 
    if ( ItM.Value()->IsIn(anIObj) )
      return AIS2D_DS_Temporary;
  
  return AIS2D_DS_None;

}

AIS2D_PToListOfInt AIS2D_InteractiveContext::DisplayedModes( const Handle(AIS2D_InteractiveObject)& anIObj ) const {
    return myObjects(anIObj)->DisplayedModes();
}

Standard_Boolean AIS2D_InteractiveContext::IsDisplayed( const Handle(AIS2D_InteractiveObject)& anIObj ) const {

 if ( anIObj.IsNull() ) return Standard_False;

 if ( myObjects.IsBound(anIObj) ) 
    if ( myObjects(anIObj)->GraphicStatus() == AIS2D_DS_Displayed )
      return Standard_True;
  
  AIS2D_DataMapIteratorOfDataMapOfLC ItM(myLocalContexts);
  for ( ; ItM.More(); ItM.Next() )
    if ( ItM.Value()->IsDisplayed(anIObj) ) return Standard_True;
  
 return Standard_False;

}

Standard_Boolean AIS2D_InteractiveContext::IsDisplayed( const Handle(AIS2D_InteractiveObject)& /*anIObj*/,
                                                        const Standard_Integer /*aMode*/ ) const {

 return Standard_False;

}

Standard_Boolean AIS2D_InteractiveContext::IsHighlighted( const Handle(AIS2D_InteractiveObject)& anIObj ) const {

  if ( anIObj.IsNull() ) return Standard_False;

  if (!HasOpenedContext()){
#ifdef OCC146
    if (!anIObj->IsHighlighted()) return Standard_False;
#endif
    if(!myObjects.IsBound(anIObj)) 
      return Standard_False;
    return myObjects(anIObj)->IsHighlight();
  }
  AIS2D_DataMapIteratorOfDataMapOfLC ItM(myLocalContexts);
  for (;ItM.More();ItM.Next() ) {
    if (ItM.Value()->IsHighlight(anIObj) )
      return Standard_True;
  }
  return Standard_False;


}

Standard_Boolean AIS2D_InteractiveContext::IsHighlighted( const Handle(AIS2D_InteractiveObject)& anIObj,
                                                          Standard_Boolean& /*WithColor*/,
                                                          Quantity_NameOfColor& /*theHiCol*/ ) const {
  if ( anIObj.IsNull() ) return Standard_False;

  if ( !HasOpenedContext() ) {
#ifdef OCC146
    if (!anIObj->IsHighlighted()) return Standard_False;
#endif
    if ( !myObjects.IsBound(anIObj) ) 
      return Standard_False;
    return myObjects(anIObj)->IsHighlight();
  }
  AIS2D_DataMapIteratorOfDataMapOfLC ItM(myLocalContexts);
  for ( ; ItM.More(); ItM.Next() )
    if ( ItM.Value()->IsHighlight(anIObj) )
      return Standard_True;
  
  return Standard_False;

}
 
void AIS2D_InteractiveContext::SubIntensityOn(
		const Handle(AIS2D_InteractiveObject)& anIObj,
	    const Standard_Boolean UpdateVwr) {

  Standard_Integer indCol;
  if ( !HasOpenedContext() ) {
    if ( !myObjects.IsBound(anIObj) )  return;
    const Handle(AIS2D_GlobalStatus) GStatus = myObjects(anIObj);
    if ( GStatus->IsSubIntensityOn() ) return;
    GStatus->SubIntensityOn();
    Standard_Boolean UpdMain(Standard_False),UpdColl(Standard_False);
    
    for (TColStd_ListIteratorOfListOfInteger It( * GStatus->DisplayedModes() ); It.More(); It.Next() ) {
      if ( GStatus->GraphicStatus()==AIS2D_DS_Displayed) {
		  SetHighlightColor( mySubIntensity );
	      HighlightWithColor( anIObj, mySubIntensity );
	      UpdMain = Standard_True;
	  }	else if ( GStatus->GraphicStatus()==AIS2D_DS_Erased ) {
		indCol = myCollectorVwr->InitializeColor( mySubIntensity );
		myCollectorVwr->View()->SetDefaultOverrideColor( indCol );
        HighlightWithColor(anIObj, mySubIntensity );
    	UpdColl=Standard_True;
      }
    }
    if ( UpdateVwr ) {
      if ( UpdMain)	myMainVwr->Update();
      if ( UpdColl ) myCollectorVwr->Update();
    }
  } else {
    if ( myObjects.IsBound(anIObj) ) {
      const Handle(AIS2D_GlobalStatus)& GStatus = myObjects(anIObj);
      GStatus->SubIntensityOn();						 
      for ( ItL.Initialize( * GStatus->DisplayedModes() ); ItL.More(); ItL.Next() ) {
		  SetHighlightColor( mySubIntensity );
		  HighlightWithColor(anIObj, mySubIntensity );
	  }
    } else
      myLocalContexts(myCurLocalIndex)->SubIntensityOn(anIObj);
    
    if (UpdateVwr) myMainVwr->Update();
  }
}

void AIS2D_InteractiveContext::SubIntensityOff(
	    const Handle(AIS2D_InteractiveObject)& anIObj,
		const Standard_Boolean UpdateVwr) {
  
   if ( !HasOpenedContext() ) {
     if ( !myObjects.IsBound(anIObj) ) return;
     const Handle(AIS2D_GlobalStatus)& GStatus = myObjects(anIObj);
     if ( !GStatus->IsSubIntensityOn() ) return;
     GStatus->SubIntensityOff();
     Standard_Boolean UpdMain(Standard_False),UpdColl(Standard_False);
    
     for ( TColStd_ListIteratorOfListOfInteger It( * GStatus->DisplayedModes() ); It.More() ;It.Next() ) {
      if ( GStatus->GraphicStatus() != AIS2D_DS_Erased ) {
	     Unhighlight( anIObj );
	     UpdMain = Standard_True;
      } else {
	    Unhighlight( anIObj );
	    UpdColl=Standard_True;
      }
    }
    
    Standard_Integer DM,SM;
    AIS2D_TypeOfDetection HM;
    GetDefModes(anIObj,DM,HM,SM);
    if ( IsIOSelected(anIObj) )
      Highlight(anIObj,HM);
    
    if ( UpdateVwr ) {
      if ( UpdMain ) myMainVwr->Update();
      if ( UpdColl ) myCollectorVwr->Update();
    }
  } else {
    if ( myObjects.IsBound(anIObj) ) {
      const Handle(AIS2D_GlobalStatus)& GStatus = myObjects(anIObj);
      GStatus->SubIntensityOff();
      for (ItL.Initialize(* GStatus->DisplayedModes());ItL.More();ItL.Next())
	    Unhighlight(anIObj);
      if (GStatus->IsHighlight())
    	Highlight(anIObj);
	} else
      myLocalContexts(myCurLocalIndex)->SubIntensityOff(anIObj);
      if ( IsSelected(anIObj) )
      Highlight(anIObj);
    
    if ( UpdateVwr) myMainVwr->Update();
  }
}

Standard_Boolean AIS2D_InteractiveContext::IsInCollector( const Handle(AIS2D_InteractiveObject)& anIObj ) const {

  if ( anIObj.IsNull() ) 	  
	  return Standard_False;
 
  if ( myObjects.IsBound(anIObj) ) 
      return ( myObjects( anIObj)->GraphicStatus() == AIS2D_DS_Erased );
  return Standard_False;
}

void AIS2D_InteractiveContext::Status( const Handle(AIS2D_InteractiveObject)& anIObj,
                                       TCollection_ExtendedString& aStatus ) const {

  aStatus = "";

  if ( anIObj.IsNull() ) return ;
  if ( myObjects.IsBound(anIObj) ) {
    aStatus += "\t ____________________________________________";
    aStatus += "\t| Known at Neutral Point:\n\tDisplayStatus:";
    const Handle(AIS2D_GlobalStatus)& theGStat = myObjects(anIObj);
    switch ( theGStat->GraphicStatus() ) {
    case AIS2D_DS_Displayed: {
    	aStatus +="\t| -->Displayed\n";
    	break;
    }
    case AIS2D_DS_Erased: {
	    aStatus +="\t| -->Erased\n";
	    break;
    }
    case AIS2D_DS_FullErased: {
	    aStatus +="\t| -->Full Erased\n";
	    break;
    
    }
    case AIS2D_DS_Temporary:
    case AIS2D_DS_None:
	    break;
    
	}
    aStatus += "\t| Active Display Modes in the MainViewer :\n";
    for ( ItL.Initialize(*(theGStat->DisplayedModes())); 
	      ItL.More(); ItL.Next() ) {
        aStatus += "\t|\t Mode ";
        aStatus += TCollection_AsciiString(ItL.Value());
        aStatus+="\n";
    }	
    if ( IsCurrent(anIObj) ) 
		aStatus +="\t| Current\n";
    //if ( IsSelected(anIObj) ) 
//		aStatus +="\t| Selected\n";

    aStatus += "\t| Active Selection Modes in the MainViewer :\n";
    for ( ItL.Initialize(*(theGStat->SelectionModes() )); 
	      ItL.More(); ItL.Next() ) {
        aStatus += "\t\t Mode ";
        aStatus += TCollection_AsciiString(ItL.Value());
        aStatus+="\n";
    }	
    aStatus += "\t ____________________________________________";
      
  }

}

void AIS2D_InteractiveContext::UpdateCurrentViewer() {

  if ( !myMainVwr.IsNull() )  myMainVwr->UpdateNew();

}

void AIS2D_InteractiveContext::UpdateCollector() {
  if ( !myCollectorVwr.IsNull() ) myCollectorVwr->UpdateNew();
}

void AIS2D_InteractiveContext::DisplayedObjects (AIS2D_ListOfIO& aListOfIO,
                                                 const Standard_Boolean OnlyFromNeutral) const
{
  AIS2D_DataMapIteratorOfDataMapOfIOStatus It( myObjects );
  
  if ( !HasOpenedContext() || OnlyFromNeutral ) {
    for ( ; It.More(); It.Next() ) 
      if ( It.Value()->GraphicStatus() == AIS2D_DS_Displayed )
	       aListOfIO.Append( It.Key() );
  } else {
    TColStd_MapOfTransient theMap;
    
    for ( ; It.More(); It.Next() ) {
      if ( It.Value()->GraphicStatus() == AIS2D_DS_Displayed )
	     theMap.Add(It.Key());
    }
    
    Standard_Integer NbDisp;
    for ( AIS2D_DataMapIteratorOfDataMapOfLC it1(myLocalContexts);
	      it1.More(); it1.Next() ) {
      const Handle(AIS2D_LocalContext)& LC = it1.Value();
      NbDisp =  LC->DisplayedObjects(theMap);
	}
		  
    Handle(AIS2D_InteractiveObject) curIO;
    Handle(Standard_Transient) Tr;
    for ( TColStd_MapIteratorOfMapOfTransient it2(theMap); it2.More(); it2.Next() ) {
	     Tr = it2.Key();
	     curIO = *((Handle(AIS2D_InteractiveObject)*) &Tr);
	     aListOfIO.Append(curIO);
    }
  }
}

void AIS2D_InteractiveContext::GetDefModes( const Handle(AIS2D_InteractiveObject)& anIObj,
				                            Standard_Integer& DispMode,
				                            AIS2D_TypeOfDetection& HiMode,
				                            Standard_Integer& SelMode) const {

  if ( anIObj.IsNull() ) return ;

  DispMode = anIObj->HasDisplayMode() ? anIObj->DisplayMode() : anIObj->DefaultDisplayMode();
  HiMode = anIObj->HasHighlightMode() ? anIObj->HighlightMode() : anIObj->DefaultHighlightMode();
  SelMode = anIObj->HasSelectionMode() ? anIObj->SelectionMode() : anIObj->DefaultSelectionMode();

}

void AIS2D_InteractiveContext::EraseGlobal(
		     const Handle(AIS2D_InteractiveObject)& anIObj, 
			 const Standard_Boolean /*UpdateVwr*/, 
			 const Standard_Boolean /*PutInCollector*/) {
  
  if ( anIObj.IsNull() ) return ;
  if ( !myObjects.IsBound( anIObj ) ) return;
  
}


void AIS2D_InteractiveContext::SetHighlightColor( const Quantity_NameOfColor aCol ) {
  
   if ( myHighlightColor != aCol ) {
    myHighlightColor = aCol;
    Standard_Integer anIndex = myMainVwr->InitializeColor(aCol);
    myMainVwr->View()->SetDefaultOverrideColor( anIndex );
  }
}

void AIS2D_InteractiveContext::SetSelectionColor( const Quantity_NameOfColor aCol ) {

  if ( mySelectionColor != aCol ) {
    mySelectionColor = aCol;
    Standard_Integer anIndex = myMainVwr->InitializeColor(aCol);
    myMainVwr->View()->SetDefaultOverrideColor(anIndex);
   }
}

void AIS2D_InteractiveContext::SetSubIntensityColor( const Quantity_NameOfColor aCol ) {
   mySubIntensity = aCol;
}

Standard_Integer AIS2D_InteractiveContext::HighestIndex() const {

  AIS2D_DataMapIteratorOfDataMapOfLC It( myLocalContexts );
  Standard_Integer HiInd = 0;
  for ( ; It.More(); It.Next() )
    HiInd = ( It.Key() > HiInd ) ? It.Key() : HiInd;
  return HiInd;

}

Standard_Integer AIS2D_InteractiveContext::InitializeColor( const Quantity_Color& aColor ) {

#ifdef IMP140901
  Handle(Aspect_ColorMap) colormap = myMainVwr->ColorMap();
#else
  Handle(Aspect_GenericColorMap) colormap = myMainVwr->ColorMap();
#endif
  Standard_Integer size = colormap->Size();
  Standard_Integer indColor = colormap->AddEntry(aColor);
  if( size != colormap->Size() ) {
    myMainVwr->SetColorMap( colormap );
  }
  return indColor;
  
}

Standard_Integer AIS2D_InteractiveContext::InitializeStyle( const Aspect_TypeOfLine aType ) {
	
  Handle(Aspect_TypeMap) typemap = myMainVwr->TypeMap();
  Standard_Integer size = typemap->Size();
  Standard_Integer indType = typemap->AddEntry(aType);
  if( size != typemap->Size() ) {
    myMainVwr->SetTypeMap( typemap );
  }

  return indType;

}

Standard_Integer AIS2D_InteractiveContext::InitializeWidth( const Aspect_WidthOfLine aWidth ) {
 
  Handle(Aspect_WidthMap) widthmap = myMainVwr->WidthMap();
  Standard_Integer size = widthmap->Size();
  Standard_Integer indWidth = widthmap->AddEntry(aWidth);
  if( size != widthmap->Size() ) {
    myMainVwr->SetWidthMap( widthmap );
  }

  return indWidth;
  
}

Standard_Integer AIS2D_InteractiveContext::InitializeMarker( const Aspect_TypeOfMarker aMark ) {
 
  Handle(Aspect_MarkMap) markmap = myMainVwr->MarkMap();
  Standard_Integer size = markmap->Size();
  Standard_Integer indMark = markmap->AddEntry(aMark);
  if( size != markmap->Size() ) {
    myMainVwr->SetMarkMap( markmap );
  }

  return indMark;
  
}

Standard_Integer AIS2D_InteractiveContext::InitializeFont( const Aspect_FontStyle& aFont ) {

  Handle(Aspect_FontMap) fontmap = myMainVwr->FontMap();
  Standard_Integer size = fontmap->Size();
  Standard_Integer indFont = fontmap->AddEntry(aFont);
  if( size != fontmap->Size() ) {
    myMainVwr->SetFontMap( fontmap );
  }
  return indFont;
 
}

void AIS2D_InteractiveContext::DrawObject( const Handle(AIS2D_InteractiveObject)& anIObj, 
                                           const Standard_Integer aDispMode ) {
    
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
         colorInd = InitializeColor( aColor );
         theLines->SetColorIndex( colorInd );
       }

       if ( !typeInd ) {
         typeInd = InitializeStyle( aType );
         theLines->SetTypeIndex( typeInd );
       }

       if ( !widthInd ) {
	     widthInd = InitializeWidth( aWidth );
         theLines->SetWidthIndex( widthInd );
       }

       if ( thePS->IsHLMode() ) {
         Handle(Graphic2d_Line) theHLines = thePS->GetHLPrimitives();
         theHLines->SetColorIndex( colorInd );
         theHLines->SetWidthIndex( widthInd );
         Standard_Integer theDashInd = InitializeStyle( Aspect_TOL_DASH );
         theHLines->SetTypeIndex( theDashInd );          
       }

    }  else {

      Handle(Graphic2d_Primitive) thePrim;

      for ( Standard_Integer i = 1; i <= anIObj->Length(); i++ ) {
       thePrim = anIObj->Primitive( i );
       thePrim->SetDisplayMode(aDispMode);
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
           colorInd = InitializeColor( aColor );
           theLine->SetColorIndex( colorInd );
         }

         if ( !typeInd ) {
           typeInd = InitializeStyle( aType );
           theLine->SetTypeIndex( typeInd );
         }

         if ( !widthInd ) {
	       widthInd = InitializeWidth( aWidth );
           theLine->SetWidthIndex( widthInd );
         }

         if ( !colorIntInd ) {
           colorIntInd = InitializeColor( aIntColor );
           theLine->SetInteriorColorIndex( colorIntInd );
         }
   
         theLine->SetTypeOfPolygonFilling( aTypeFill );
         theLine->SetInteriorPattern( aTile );
         theLine->SetDrawEdge( aDrawEdge );

         if ( theLine->IsKind(STANDARD_TYPE(Prs2d_Point) ) ) {

           Handle(Prs2d_Point) thePnt = Handle(Prs2d_Point)::DownCast(theLine);
           thePnt->SetIndex( InitializeMarker( thePnt->Marker() ) );
         }    

    } // end if the kind of primitive is Graphic2d_Line

  } // end for exploration of the interactive object 
 
 }  // end if IO is AIS2D_ProjShape
    
}

void AIS2D_InteractiveContext::ResetOriginalState(const Standard_Boolean /*updateviewer*/) {
   /*
  Standard_Boolean upd_main(Standard_False),upd_col(Standard_False);
  TColStd_ListIteratorOfListOfInteger itl;
  
  for (AIS_DataMapIteratorOfDataMapOfIOStatus it(myObjects);it.More();it.Next()){
    const Handle(AIS_InteractiveObject)& iobj = it.Key();
    const Handle(AIS_GlobalStatus)& STAT = it.Value();
    switch(STAT->GraphicStatus()){
    case AIS_DS_Displayed:{
      upd_main = Standard_True;
      
      // partie display...
      for(itl.Initialize(STAT->DisplayedModes());itl.More();itl.Next())
	myMainPM->Display(iobj,itl.Value());
      if(STAT->IsHilighted()){
	if(STAT->HilightColor()!=Quantity_NOC_WHITE)
	  HilightWithColor(iobj,STAT->HilightColor(),Standard_False);
	else
	  Hilight(iobj,Standard_False);
      }
      //partie selection
      for(itl.Initialize(STAT->SelectionModes());itl.More();itl.Next()){
	if(itl.Value()!=-1)
	  mgrSelector->Activate(iobj,itl.Value(),myMainSel);
      }
      break; 
    }
    case AIS_DS_Erased:{
      upd_col = Standard_True;
      EraseGlobal(iobj,Standard_False,Standard_True);
      break;
    }
    case AIS_DS_FullErased:{
      EraseGlobal(iobj,Standard_False,Standard_False);
      break;
    }
#ifndef DEB
    default:
      break;
#endif
    }
  }
  if(updateviewer){
    if(upd_main) 
      myMainVwr->Update();
    if(upd_col)
      myCollectorVwr->Update();
  }*/
}

Standard_Boolean AIS2D_InteractiveContext::AvailableToDetect( const Handle(AIS2D_InteractiveObject)& anIO ) const {
#ifndef OCC146
 if ( anIO->HighlightMode() == AIS2D_TOD_OBJECT) 
     return ( ! anIO->State() );
#endif
 return Standard_True;
}

void AIS2D_InteractiveContext::DoDetectObject( const Handle(AIS2D_InteractiveObject)& theIO,
                                               const Handle(Graphic2d_TransientManager)& theDrawer ) {
 theIO->ClearSeqDetPrim();
#ifdef OCC146
 if (!theIO->State())
#endif
 theIO->Unhighlight();
 myCurDetectMode = theIO->HighlightMode();
 switch ( myCurDetectMode ) {
 default :
 case AIS2D_TOD_OBJECT :
  if ( !IsIOSelected( theIO ) ) {
   theDrawer->Draw( theIO );  
#ifdef OCC146
  }
  {
#endif
   if ( !IsIOPicked( theIO ) ) 
     mySeqOfDetIO->Append( theIO );
  }
  break;
 case AIS2D_TOD_PRIMITIVE: {
  Standard_Integer pLength = theIO->PickList()->Length();
  Handle(Graphic2d_Primitive) thePrim;
   
  if ( pLength ) {
   for ( int i = 1; i <= pLength; i++ ) {  
    thePrim = theIO->Primitive(theIO->PickList()->Value(i));
    if ( ! thePrim->IsHighlighted() &&  
         ! IsPrimSelected ( theIO, thePrim ) ) {
      theDrawer->Draw( thePrim );       
#ifdef OCC146
    }
    {
#endif
      if ( ! IsPrimPicked( theIO, thePrim ) ) {
        theIO->AddDetectPrim( thePrim, 0 );
        if ( !IsIOPicked( theIO ) ) 
          mySeqOfDetIO->Append( theIO );
      }
    }
   }
   
  } else {
    thePrim = theIO->Primitive( theIO->PickedIndex() );
    if ( ! thePrim->IsHighlighted() ) {
      theDrawer->Draw( thePrim );   
#ifdef OCC146
    }
    {
#endif
      if ( ! IsPrimPicked( theIO, thePrim ) ) {
        theIO->AddDetectPrim( thePrim, 0 );
        if ( !IsIOPicked( theIO ) ) 
            mySeqOfDetIO->Append( theIO );
      }
    }
  }    
  }
  break;
 case AIS2D_TOD_ELEMENT: {
    Standard_Integer theInd, pLength = theIO->PickList()->Length();
    Handle(Graphic2d_Primitive) thePrim;
    if ( pLength ) {
      for ( int i = 1; i <= pLength; i++ ) {
        thePrim = theIO->Primitive(theIO->PickList()->Value(i));
        theInd = thePrim->PickedIndex();
        if ( ! thePrim->IsHighlighted() )  {

	  // SAV - begin
	  const TColStd_MapOfInteger& indices = thePrim->PickedIndices();
	  
	  if ( indices.Extent() ) {
	    theDrawer->DrawPickedElements( thePrim );

	    TColStd_MapIteratorOfMapOfInteger it( indices );
	    theIO->AddDetectPrim( thePrim, it.Key() );

	    if ( !IsIOPicked( theIO ) ) 
	      mySeqOfDetIO->Append( theIO );
	  }
	  else {
	  
	    for ( int j = 1; j <= thePrim->NumOfElemIndices(); ++j ) {
	      theDrawer->DrawElement( thePrim, j );
	      if ( ! IsElemPicked( theIO, thePrim, j ) ) {
		theIO->AddDetectPrim( thePrim, j );
		if ( !IsIOPicked( theIO ) ) 
		  mySeqOfDetIO->Append( theIO );
	      }
	    }
	  }

/*
         for ( int j = 1; j <= thePrim->NumOfElemIndices(); ++j ) {
           theDrawer->DrawElement( thePrim, j );
           if ( ! IsElemPicked( theIO, thePrim, j ) ) {
             theIO->AddDetectPrim( thePrim, j );
             if ( !IsIOPicked( theIO ) ) 
                  mySeqOfDetIO->Append( theIO );
           }
        }
*/
	}
        /*if ( ! IsElemSelected( theIO, thePrim, theInd ) ) {
             theDrawer->DrawElement( thePrim, theInd );
             if ( ! IsElemPicked( theIO, thePrim, theInd ) ) {
               theIO->AddDetectPrim( thePrim, theInd );
               if ( ! IsIOPicked( theIO ) ) 
                   mySeqOfDetIO->Append( theIO );
             }
             
          }
         */
      }
    } else {
      thePrim = theIO->Primitive(theIO->PickedIndex());
      theInd = thePrim->PickedIndex();
#ifdef BUG
      if ( theInd > 0 )
#else
      if ( theInd != 0 )
#endif
#ifdef OCC146
      {
#endif
        if ( ! IsElemSelected( theIO, thePrim, theInd ) ) {
           theDrawer->DrawElement( thePrim, theInd );
#ifdef OCC146
        }
        {
#endif
           if ( ! IsElemPicked( theIO, thePrim, theInd ) ) {
            theIO->AddDetectPrim( thePrim, theInd );
            if ( ! IsIOPicked( theIO ) ) 
              mySeqOfDetIO->Append( theIO );
           }
        }
#ifdef OCC146
      }
#endif
    }
   }
   break;
 case AIS2D_TOD_VERTEX: {
    Standard_Integer theInd, pLength = theIO->PickList()->Length();
    Handle(Graphic2d_Primitive) thePrim;
    if ( pLength ) {
     for ( int i = 1; i <= pLength; i++ ) {
        thePrim = theIO->Primitive(theIO->PickList()->Value(i));
        theInd = thePrim->PickedIndex();
        if ( ! thePrim->IsHighlighted() ) 
           for ( int j = 1; j <= thePrim->NumOfVertIndices(); ++j ) {
             theDrawer->DrawVertex( thePrim, j );
             if ( ! IsElemPicked( theIO, thePrim, -j ) ) {
              theIO->AddDetectPrim( thePrim, -j );
              if ( !IsIOPicked( theIO ) ) 
                  mySeqOfDetIO->Append( theIO );
             }
           }
        }
        /*if ( ! IsElemSelected( theIO, thePrim, theInd ) ) {
             theDrawer->DrawVertex( thePrim, -theInd );
             if ( ! IsElemPicked( theIO, thePrim, theInd ) ) {
               theIO->AddDetectPrim( thePrim, -theInd );
               if ( ! IsIOPicked( theIO ) ) 
                   mySeqOfDetIO->Append( theIO );
             }
        }*/
           
    } else {
      thePrim = theIO->Primitive(theIO->PickedIndex());
      theInd = thePrim->PickedIndex();
      if ( theInd < 0 )
#ifdef OCC146
      {
#endif
        if ( ! IsElemSelected( theIO, thePrim, theInd ) ) {
           theDrawer->DrawVertex( thePrim, -theInd );
#ifdef OCC146
        }
        {
#endif
           if ( ! IsElemPicked( theIO, thePrim, theInd ) ) {
             theIO->AddDetectPrim( thePrim, theInd );
             if ( !IsIOPicked( theIO ) ) 
               mySeqOfDetIO->Append( theIO );
           }
        }
#ifdef OCC146
      }
#endif

     }
    }
    break;
 }
         
}

Standard_Boolean AIS2D_InteractiveContext::IsIOPicked( const Handle(AIS2D_InteractiveObject)& anIO ) const { 
    
  for ( int i = 1; i <= mySeqOfDetIO->Length(); i++ ) 
      if ( anIO == mySeqOfDetIO->Value(i) )
          return Standard_True;
   return Standard_False;
  
}

Standard_Boolean AIS2D_InteractiveContext::IsIOSelected( const Handle(AIS2D_InteractiveObject)& anIO ) const { 
    
  for ( int i = 1; i <= mySeqOfSelIO->Length(); i++ ) 
      if ( anIO == mySeqOfSelIO->Value(i) )
          return Standard_True;
   return Standard_False;
  
}

Standard_Boolean AIS2D_InteractiveContext::IsPrimSelected( const Handle(AIS2D_InteractiveObject)& anIO,
                                                           const Handle(Graphic2d_Primitive)& aPrim ) const { 
    
   Handle(AIS2D_HSequenceOfPrimArchit) theSPA = anIO->GetSelectedSeqPrim();

   for ( int i = 1; i <= theSPA->Length(); i++ ) 
      if ( aPrim == theSPA->Value(i)->GetPrimitive() )
          return Standard_True;
   return Standard_False;
  
}

Standard_Boolean AIS2D_InteractiveContext::IsPrimPicked( const Handle(AIS2D_InteractiveObject)& anIO,
                                                         const Handle(Graphic2d_Primitive)& aPrim ) const { 
   Handle(AIS2D_HSequenceOfPrimArchit) theSPA = anIO->GetDetectSeqPrim();

   for ( int i = 1; i <= theSPA->Length(); i++ ) 
      if ( aPrim == theSPA->Value(i)->GetPrimitive() )
          return Standard_True;
   return Standard_False;
}

Standard_Boolean AIS2D_InteractiveContext::IsElemPicked( const Handle(AIS2D_InteractiveObject)& anIO,
                                                         const Handle(Graphic2d_Primitive)& aPrim,
                                                         const Standard_Integer ind ) const { 
  Handle(AIS2D_HSequenceOfPrimArchit) theSPA = anIO->GetDetectSeqPrim(); 
  for ( int i = 1; i <= theSPA->Length(); i++ ) 
      if ( aPrim == theSPA->Value(i)->GetPrimitive() && 
           ind == theSPA->Value(i)->GetIndex() )
        return Standard_True;
   return Standard_False;
}

Standard_Boolean AIS2D_InteractiveContext::IsElemSelected( const Handle(AIS2D_InteractiveObject)& anIO,
                                                           const Handle(Graphic2d_Primitive)& aPrim,
                                                           const Standard_Integer ind ) const { 
  Handle(AIS2D_HSequenceOfPrimArchit) theSPA = anIO->GetSelectedSeqPrim(); 
  for ( int i = 1; i <= theSPA->Length(); i++ ) 
      if ( aPrim == theSPA->Value(i)->GetPrimitive() && 
           ind == theSPA->Value(i)->GetIndex() )
        return Standard_True;
   return Standard_False;
}

void AIS2D_InteractiveContext::Highlight( const Handle(AIS2D_InteractiveObject)& anIObj,
                                          const Standard_Boolean updateVwr )  {
  if ( anIObj.IsNull() ) return;

  if ( !anIObj->HasInteractiveContext() ) 
      anIObj->SetContext( this );
  if ( !HasOpenedContext() ) {
    if ( !myObjects.IsBound(anIObj) ) return;
    Handle(AIS2D_GlobalStatus) aGS = myObjects(anIObj);
      
    aGS->SetHighlightStatus( Standard_True );
      
    switch ( aGS->GraphicStatus() ) {

    case AIS2D_DS_Displayed:	{

	 Standard_Integer DispMode,SelMode;
     AIS2D_TypeOfDetection HiMode;
	 GetDefModes( anIObj, DispMode, HiMode, SelMode );
     Standard_Integer pInd;
     anIObj->Highlight(myMainVwr->InitializeColor( mySelectionColor ));
     switch( HiMode ) {
	 default:
     case AIS2D_TOD_OBJECT:
       break;
     case AIS2D_TOD_PRIMITIVE:
       pInd = anIObj->PickedIndex();
       anIObj->Unhighlight();
       break;
     case AIS2D_TOD_ELEMENT:
       pInd = anIObj->PickedIndex();
       anIObj->Unhighlight();
       break;
     case AIS2D_TOD_VERTEX:
       pInd = anIObj->PickedIndex();
       anIObj->Unhighlight();
       break;
     } //end switch
        
	 if ( updateVwr) myMainVwr->Update();

	 break;
   }
   case AIS2D_DS_Erased:	{
     anIObj->Highlight(myCollectorVwr->InitializeColor( mySelectionColor ));
     if ( updateVwr ) myCollectorVwr->Update();
     break;
   }
   case AIS2D_DS_FullErased:
   case AIS2D_DS_Temporary:
   case AIS2D_DS_None:
     break;
  } //end switch
 }  // end if opened context isn't exists
  //else
  //    myLocalContexts(myCurLocalIndex)->Highlight(anIObj);
  
}

void AIS2D_InteractiveContext::HighlightWithColor( const Handle(AIS2D_InteractiveObject)& anIObj,
                                                   const Quantity_NameOfColor aCol,
                                                   const Standard_Boolean updateVwr )  {

  if ( anIObj.IsNull() ) return;

  if ( !anIObj->HasInteractiveContext() ) anIObj->SetContext(this);

  if ( !HasOpenedContext() ) {
      if ( !myObjects.IsBound(anIObj) ) return;

      const Handle(AIS2D_GlobalStatus)& aGS = myObjects(anIObj);
      //const TColStd_ListOfInteger& LL = * aGS->DisplayedModes();

      aGS->SetHighlightStatus (Standard_True);
      
      switch ( aGS->GraphicStatus() ) {
      case AIS2D_DS_Displayed:{
	    Standard_Integer DispMode, SelMode;
        AIS2D_TypeOfDetection HiMode;
	    GetDefModes( anIObj, DispMode, HiMode, SelMode );
		Standard_Integer indCol = myMainVwr->InitializeColor( aCol );
		anIObj->Highlight(indCol);
        //SetSelectionColor(aCol);       
	    aGS->SetHighlightColor(aCol);
	   // if ( updateVwr ) myMainVwr->Update();
	    break;
      }
      case AIS2D_DS_Erased: {
#ifdef DEB
	    Standard_Integer indCol = myCollectorVwr->InitializeColor( aCol );
#endif
		if ( updateVwr) myCollectorVwr->Update();
	    break;
      }
      case AIS2D_DS_FullErased:
      case AIS2D_DS_Temporary:
      case AIS2D_DS_None:
	break;
      }
    } // else
      // myLocalContexts(myCurLocalIndex)->Highlight(anIObj,aCol);
    if ( updateVwr) myMainVwr->Update();

}

void AIS2D_InteractiveContext::Unhighlight( const Handle(AIS2D_InteractiveObject)& anIObj,
                                            const Standard_Boolean updateVwr )  {
    if ( anIObj.IsNull() ) return;
    anIObj->Unhighlight();
    switch ( anIObj->HighlightMode() ) {
    default:    
    case AIS2D_TOD_OBJECT:
     break;
    case AIS2D_TOD_PRIMITIVE:
     if ( !anIObj->Primitive(anIObj->PickedIndex()).IsNull() )
         anIObj->Primitive(anIObj->PickedIndex())->Unhighlight();
     break;   
    case AIS2D_TOD_ELEMENT: 
     if ( !anIObj->Primitive(anIObj->PickedIndex()).IsNull() )
         anIObj->Primitive(anIObj->PickedIndex())->Unhighlight();
     break;
    case AIS2D_TOD_VERTEX:
     if ( !anIObj->Primitive(anIObj->PickedIndex()).IsNull() )
         anIObj->Primitive(anIObj->PickedIndex())->Unhighlight();
     break;
    }
   if ( updateVwr) myMainVwr->UpdateNew();
}

void AIS2D_InteractiveContext::Place(
				   const Standard_Integer XPix, 
				   const Standard_Integer YPix, 
				   const Handle(V2d_View)& aView,
                   const Handle(AIS2D_InteractiveObject)& anIO,
                   const Standard_Boolean isTemporary ) {

 Standard_Real XPS, YPS;
 aView->Convert( XPix, YPix, XPS, YPS );
 gp_GTrsf2d theGTrsf;
 gp_Trsf2d theTrsf;
 theTrsf.SetTranslationPart( gp_Vec2d(gp_Pnt2d(0.,0.), gp_Pnt2d(XPS, YPS) ) );
 theGTrsf.SetTrsf2d( theTrsf );
 anIO->SetTransform( theGTrsf );
 Handle(V2d_Viewer) theViewer = aView->Viewer();
 if ( isTemporary ) {
   
   Handle(Graphic2d_TransientManager) theDrawer = 
     Handle(Graphic2d_TransientManager)::DownCast( theViewer->View()->Drawer() );
   Standard_Integer ind = theViewer->InitializeColor(myHighlightColor);      
   theDrawer->SetOverride( Standard_True );
   theDrawer->SetOverrideColor( ind );
   Display( anIO, Standard_False );
   anIO->Highlight( ind );
   theDrawer->BeginDraw( aView->Driver() );
   theDrawer->Draw( anIO );
   theDrawer->EndDraw();
   theDrawer->SetOverride( Standard_False );
   return;
 } 
   if ( !IsDisplayed( anIO ) ) 
     Display( anIO );
   else 
     theViewer->Update();

}

void AIS2D_InteractiveContext::Drag(
				   const Standard_Integer XPix, 
				   const Standard_Integer YPix, 
				   const Handle(V2d_View)& aView,
                   const Standard_Boolean isTemporary ) {

 if ( ! mySeqOfSelIO->IsEmpty() ) {
  Handle(AIS2D_InteractiveObject) theIO;
  for ( int j = 1; j <= mySeqOfSelIO->Length(); ++j ) {
      theIO = mySeqOfSelIO->Value(j);
      if ( !isTemporary ) {
          Unhighlight( theIO, Standard_False );
          theIO->SetState(0);
          mySeqOfSelIO->Remove(j);              
      }
      Place( XPix, YPix, aView, theIO, isTemporary );
  }
 }
}

AIS2D_StatusOfDetection AIS2D_InteractiveContext::MoveTo(
				   const Standard_Integer XPix, 
				   const Standard_Integer YPix, 
				   const Handle(V2d_View)& aView ) {

 RectMoveTo = Standard_False; 
 if ( HasOpenedContext() && aView->Viewer() != myCollectorVwr ) {
    myWasLastMain = Standard_True;
    return myLocalContexts(myCurLocalIndex)->MoveTo( XPix, YPix, aView );
 }				
 Standard_Boolean is_main = Standard_True, UpdVwr = Standard_False;
 Handle(V2d_Viewer) theViewer = aView->Viewer();

 if ( theViewer == myMainVwr ) {
    myLastPicked = myLastinMain;
    myWasLastMain = Standard_True;
 } else if ( theViewer == myCollectorVwr ) {
    myLastPicked = myLastinColl;
    is_main = Standard_False;
    myWasLastMain = Standard_False;
 } else 
    return AIS2D_SOD_Error;
  
 AIS2D_StatusOfDetection theStat( AIS2D_SOD_Nothing );

 Handle(Graphic2d_TransientManager) theDrawer = 
    Handle(Graphic2d_TransientManager)::DownCast( theViewer->View()->Drawer() );
 
 Handle(Graphic2d_DisplayList) thePickList = aView->Pick( XPix, YPix, myDetectPrecision);
 
 if ( ! thePickList->IsEmpty() ) {
   
   if ( thePickList->Length() == 1 ) {
     Handle(Graphic2d_GraphicObject) theGO = thePickList->Value(1);
     if ( ! theGO->IsKind(STANDARD_TYPE(AIS2D_InteractiveObject) ) ) 
         return AIS2D_SOD_Nothing;
     Handle(AIS2D_InteractiveObject) theIO = Handle(AIS2D_InteractiveObject)::DownCast(theGO);
     if ( IsIOPicked( theIO ) ) 
       switch ( theIO->HighlightMode() ) {
       default:
       case AIS2D_TOD_OBJECT: 
          return AIS2D_SOD_OnlyOneDetected;
          break;
       case AIS2D_TOD_PRIMITIVE:
          if ( IsPrimPicked(theIO, theIO->Primitive(theIO->PickedIndex())) )
            return AIS2D_SOD_OnlyOneDetected;
          break;
       case AIS2D_TOD_ELEMENT: {
          Standard_Integer ind = theIO->PickedIndex();
          if ( IsElemPicked( theIO, theIO->Primitive(ind), theIO->Primitive(ind)->PickedIndex() ) )
              return AIS2D_SOD_OnlyOneDetected;
          else if ( ind < 0 )
              myResetDetect= Standard_True;

          }
          break;
       case AIS2D_TOD_VERTEX: {
          Standard_Integer ind = theIO->PickedIndex();
          if ( IsElemPicked( theIO, theIO->Primitive(ind), theIO->Primitive(ind)->PickedIndex() ) )
            return AIS2D_SOD_OnlyOneDetected;
          //else if ( ind > 0 )
          //  myResetDetect= Standard_True;

          }
          break; 
             
       }
#ifdef OCC146
     myResetDetect = Standard_True;
#endif
   }
  if ( myResetDetect ) {
     mySeqOfDetIO->Clear();
     Standard_Integer ind = myMainVwr->InitializeColor(myHighlightColor);   

     for ( int i = 1 ; i <= thePickList->Length(); i++ ) {
      Handle(Graphic2d_GraphicObject) theGOi = thePickList->Value(i);
      if (  theGOi->IsKind(STANDARD_TYPE(AIS2D_InteractiveObject) ) ) {
         
       myLastPicked = Handle(AIS2D_InteractiveObject)::DownCast( theGOi );
       ( is_main ) ? myLastinMain = myLastPicked : myLastinColl = myLastPicked;
       if ( myLastPicked->HasHighlightMode() ) 

         if ( AvailableToDetect(myLastPicked) ) {
             if ( myLastPicked->HighlightMode() == AIS2D_TOD_OBJECT )
#ifdef OCC146
               if (!myLastPicked->State())
#endif
                 myLastPicked->Highlight( ind );
             theDrawer->SetOverride(Standard_True);
             theDrawer->SetOverrideColor(ind);
             theDrawer->BeginDraw( aView->Driver() );
             DoDetectObject( myLastPicked, theDrawer );
             theDrawer->EndDraw();
             theDrawer->SetOverride(Standard_False);
          
         } else theStat = AIS2D_SOD_Selected;
       }
     } // end for
    
     myResetDetect= Standard_False;   
     
   } // end if myResetDetect is true

 } else {

    theStat = AIS2D_SOD_Nothing;
    theDrawer->RestoreArea( aView->Driver() );
    myResetDetect= Standard_True;
   
    if ( ! mySeqOfDetIO->IsEmpty() ) {
        Handle(AIS2D_InteractiveObject) theIO;
        for ( int i = 1; i <= mySeqOfDetIO->Length(); ++i ) {
          theIO = mySeqOfDetIO->Value(i);
          if ( theIO->HighlightMode() == AIS2D_TOD_ELEMENT ||
               theIO->HighlightMode() == AIS2D_TOD_VERTEX ) {
             theIO->ClearSeqDetPrim();
          } else {
             if ( ! theIO->State() ) 
               theIO->Unhighlight();   
          }
          theIO->ClearSeqDetPrim();
        }
       UpdVwr = Standard_True;
    }
    ( is_main ) ? myLastinMain.Nullify() : myLastinColl.Nullify();
    mySeqOfDetIO->Clear();

 }  // end if PickList isn't empty
 
 //if ( !myLastPicked.IsNull() )  myLastPicked->Unhighlight();
    
 if ( UpdVwr ) 
    is_main ? myMainVwr->Update() : myCollectorVwr->Update();
 myLastPicked.Nullify();
 myLastMoveView = aView;

 return theStat;		

}


//SAV
AIS2D_StatusOfDetection AIS2D_InteractiveContext
::MoveCircleTo( const Standard_Integer XPix,
	        const Standard_Integer YPix,
	        const Standard_Integer Radius,
	        const Handle(V2d_View)& aView )
{
  mySeqOfDetIO->Clear();

  Handle(Graphic2d_DisplayList) thePickList = 
    aView->PickByCircle( XPix, YPix, Radius );

  return Detect( thePickList, aView );
}


AIS2D_StatusOfDetection AIS2D_InteractiveContext::MoveTo(
				   const Standard_Integer Xmin, 
				   const Standard_Integer Ymin, 
                   const Standard_Integer Xmax, 
				   const Standard_Integer Ymax, 
				   const Handle(V2d_View)& aView ) {

 
 /*if ( HasOpenedContext() && aView->Viewer() != myCollectorVwr ) {
    myWasLastMain = Standard_True;
    return myLocalContexts(myCurLocalIndex)->MoveTo( Xmin, Ymin, Xmax, Ymax, aView );
 }
    */
 mySeqOfDetIO->Clear();

 Handle(Graphic2d_DisplayList) thePickList = aView->Pick( Xmin, Ymin, Xmax, Ymax, myPickMode );

  return Detect( thePickList, aView );
}

//SAV
AIS2D_StatusOfDetection AIS2D_InteractiveContext
::Detect( const Handle(Graphic2d_DisplayList)& dList, 
	  const Handle(V2d_View)& aView )
{
  AIS2D_StatusOfDetection theStat( AIS2D_SOD_Nothing );

 RectMoveTo = Standard_True;
 Handle(V2d_Viewer) theViewer = aView->Viewer();
  Standard_Boolean is_main = ( theViewer == myCollectorVwr ) ? 
    Standard_False : Standard_True;
 if ( is_main ) myWasLastMain = Standard_True;

 Handle(Graphic2d_TransientManager) theDrawer = 
    Handle(Graphic2d_TransientManager)::DownCast( theViewer->View()->Drawer() );
 
 
  if ( ! dList->IsEmpty() ) {
   Standard_Integer ind = ( is_main ) ? 
      myMainVwr->InitializeColor(myHighlightColor) : 
      myCollectorVwr->InitializeColor(myHighlightColor);
   Handle(AIS2D_InteractiveObject) theIO;
   theDrawer->SetOverride( Standard_True );
   theDrawer->SetOverrideColor( ind );
   theDrawer->BeginDraw( aView->Driver() );
         
    for ( int i = 1 ; i <= dList->Length(); i++ ) {
      Handle(Graphic2d_GraphicObject) theGOi = dList->Value(i);
    if ( theGOi->IsKind(STANDARD_TYPE(AIS2D_InteractiveObject))) { 
      theIO = Handle(AIS2D_InteractiveObject)::DownCast( theGOi );
      if ( theIO->HasHighlightMode() ) 
       if ( AvailableToDetect( theIO ) ) {
        if ( theIO->HighlightMode() == AIS2D_TOD_OBJECT )
             theIO->Highlight( ind );
	    
        DoDetectObject( theIO, theDrawer );
       
       } else theStat = AIS2D_SOD_Selected;
    } 
   } // end for
   theDrawer->EndDraw();
   theDrawer->SetOverride( Standard_False );     
 } else {

    theStat = AIS2D_SOD_Nothing;
    theDrawer->RestoreArea( aView->Driver() );
    if ( !mySeqOfDetIO->IsEmpty() )
      for ( int i = 1; i<= mySeqOfDetIO->Length(); ++i ) {
        mySeqOfDetIO->Value(i)->Unhighlight();
        mySeqOfDetIO->Value(i)->ClearSeqDetPrim();
      }
    mySeqOfDetIO->Clear();
    is_main ? myMainVwr->Update() : myCollectorVwr->Update();        
 }  // end if PickList isn't empty
  
 
 return theStat;		
}

AIS2D_StatusOfPick AIS2D_InteractiveContext::Select( const Standard_Boolean UpdateVwr ) {

 if ( HasOpenedContext() )
  if ( myWasLastMain )
      return myLocalContexts(myCurLocalIndex)->Select(UpdateVwr);
    else {
	  myLocalContexts( myCurLocalIndex )->SetSelected( myLastPicked, UpdateVwr );
	  return AIS2D_SOP_OneSelected;
  }

 if ( ! mySeqOfDetIO->IsEmpty() ) {
    Handle(AIS2D_InteractiveObject) theIO;
    Handle(AIS2D_HSequenceOfPrimArchit) thePA;
 
    switch ( myCurDetectMode ) {
    default :
    case AIS2D_TOD_OBJECT : {
      for ( int i = 1; i <= mySeqOfDetIO->Length(); ++i ) {
        theIO = mySeqOfDetIO->Value(i);
        if ( ! RectMoveTo ) {
          thePA = theIO->GetSelectedSeqPrim();
          if ( ! thePA->IsEmpty() ) {
            for ( int j = 1; j <= thePA->Length(); ++j )
               thePA->Value(j)->GetPrimitive()->Unhighlight(); 
            theIO->ClearSeqSelPrim();
          }

          if ( ! mySeqOfSelIO->IsEmpty() )
            for ( int j = 1; j <= mySeqOfSelIO->Length(); ++j ) {
               Unhighlight( mySeqOfSelIO->Value(i), Standard_False );
               mySeqOfSelIO->Value(i)->SetState(0);
               mySeqOfSelIO->Remove(i);              
             }
        }
        if ( ! theIO->State() ) {
          SetCurrentObject( theIO, Standard_False );
          mySeqOfSelIO->Append(theIO);
        }
      } 
     theIO->ClearSeqDetPrim();
    }
    break;

    case AIS2D_TOD_PRIMITIVE: 
    {
      for ( int i = 1; i <= mySeqOfDetIO->Length(); ++i ) 
      {
	Standard_Integer j;

        theIO = mySeqOfDetIO->Value(i);
        theIO->Highlight(myMainVwr->InitializeColor(mySelectionColor));
        theIO->Unhighlight();
        theIO->SetState(1);

	// unselect selected
        if ( !RectMoveTo ) 
	{
	  
          thePA = theIO->GetSelectedSeqPrim();
	  for ( j = 1; j <= thePA->Length(); ++j )
	  {
               thePA->Value(j)->GetPrimitive()->Unhighlight(); 
	  }
            theIO->ClearSeqSelPrim();
       
	  for ( j = 1; j <= mySeqOfSelIO->Length(); ++j ) 
	  {
	    // by EAV: 12.09.01
	    // Unhighlight( mySeqOfSelIO->Value(j), Standard_False );
            mySeqOfSelIO->Value(j)->SetState( 0 );
            mySeqOfSelIO->Remove(j);              
          }
             }

	// select detected
        thePA = theIO->GetDetectSeqPrim();
	for ( j = 1; j <= thePA->Length(); ++j ) 
	{
	  Handle(Graphic2d_Primitive) thePrim = thePA->Value(j)->GetPrimitive();
            if ( !thePrim ->IsHighlighted() ) {
               thePrim->Highlight();
               theIO->AddSelectPrim( thePrim, 0 );
            }
          }
	if ( ! IsIOSelected( theIO ) ) 
	  mySeqOfSelIO->Append( mySeqOfDetIO->Value(i) );
      
     } // end for
#ifndef OCC146
     theIO->ClearSeqDetPrim();   
#endif
    }
     break;
    case AIS2D_TOD_ELEMENT: {
       Standard_Integer theInd;
       for ( int i = 1; i <= mySeqOfDetIO->Length(); ++i ) {
         theIO = mySeqOfDetIO->Value(i);
         theIO->Highlight(myMainVwr->InitializeColor(mySelectionColor));
         theIO->Unhighlight();
         theIO->SetState(1);
         if ( !RectMoveTo ) {
          thePA = theIO->GetSelectedSeqPrim();
          if ( ! thePA->IsEmpty() ) {
              for ( int j = 1; j <= thePA->Length(); ++j )
                 thePA->Value(j)->GetPrimitive()->Unhighlight();
              theIO->ClearSeqSelPrim();
          }
          if ( ! mySeqOfSelIO->IsEmpty() )
            for ( int j = 1; j <= mySeqOfSelIO->Length(); ++j ) {
               Unhighlight( mySeqOfSelIO->Value(i), Standard_False );
               mySeqOfSelIO->Value(i)->SetState(0);
               mySeqOfSelIO->Remove(i);              
             }
         }
         thePA = theIO->GetDetectSeqPrim();
         if ( ! thePA->IsEmpty() ) {
           Handle(Graphic2d_Primitive) thePrim;
           for ( int j = 1; j <= thePA->Length(); ++j ) {
             thePrim = thePA->Value(j)->GetPrimitive();
             theInd = thePA->Value(j)->GetIndex();

	     //SAV - marking that highlighted elements became selected
	     // fixing regression of element single selection
	     if ( !thePrim->SetElementsSelected() )
	       thePrim->Highlight( theInd );
	     else
	       thePrim->Unhighlight();
             theIO->AddSelectPrim( thePrim, theInd );
           } 
         }
     if ( ! IsIOSelected( theIO ) ) mySeqOfSelIO->Append(mySeqOfDetIO->Value(i));
    } // end for
#ifndef OCC146
    theIO->ClearSeqDetPrim();   
#endif
  }
   break;
  case AIS2D_TOD_VERTEX: {
   Standard_Integer theInd;
   for ( int i = 1; i <= mySeqOfDetIO->Length(); ++i ) {
     theIO = mySeqOfDetIO->Value(i);
     theIO->Highlight(myMainVwr->InitializeColor(mySelectionColor));
     theIO->Unhighlight();
     theIO->SetState(1);
     if ( !RectMoveTo ) {
        thePA = theIO->GetSelectedSeqPrim();
        if ( ! thePA->IsEmpty() ) {
           for ( int j = 1; j <= thePA->Length(); ++j )
              thePA->Value(j)->GetPrimitive()->Unhighlight(); 
           theIO->ClearSeqSelPrim();
        }
        if ( ! mySeqOfSelIO->IsEmpty() )
            for ( int j = 1; j <= mySeqOfSelIO->Length(); ++j ) {
               Unhighlight( mySeqOfSelIO->Value(i), Standard_False );
               mySeqOfSelIO->Value(i)->SetState(0);
               mySeqOfSelIO->Remove(i);              
             }
     }
     thePA = theIO->GetDetectSeqPrim();
     if ( ! thePA->IsEmpty() ) {
         Handle(Graphic2d_Primitive) thePrim;
         for ( int j = 1; j <= thePA->Length(); ++j ) {
           thePrim = thePA->Value(j)->GetPrimitive();
           theInd = thePA->Value(j)->GetIndex();
           thePrim->Highlight( theInd );
           theIO->AddSelectPrim( thePrim, theInd );
         }
     }
     if ( ! IsIOSelected( theIO ) ) mySeqOfSelIO->Append(mySeqOfDetIO->Value(i));
    } // end for
#ifndef OCC146
    theIO->ClearSeqDetPrim();   
#endif
    }
    break;
  } // end switch
#ifndef OCC146
    mySeqOfDetIO->Clear();
#endif
 } else {

 // if sequence of detected objects is empty
   if ( ! mySeqOfSelIO->IsEmpty() ) {
     switch ( myCurDetectMode ) {
     default :
     case AIS2D_TOD_OBJECT : {
       Handle(AIS2D_InteractiveObject) theIO;
       for ( int i = 1; i <= mySeqOfSelIO->Length(); ++i ) {
         theIO = mySeqOfSelIO->Value(i);
         theIO->Unhighlight();
         theIO->SetState(0);
         if ( myObjects.IsBound(theIO) ) 
	       if ( myObjects(theIO)->IsSubIntensityOn() )
	         HighlightWithColor( theIO, mySubIntensity, Standard_False );
       }
     }
     break;
     case AIS2D_TOD_PRIMITIVE: {
       Handle(AIS2D_InteractiveObject) theIO;
       Handle(AIS2D_HSequenceOfPrimArchit) thePA;
       for ( int i = 1; i <= mySeqOfSelIO->Length(); ++i ) {
          theIO = mySeqOfSelIO->Value(i);
          thePA = theIO->GetSelectedSeqPrim();
          theIO->Unhighlight();
          if ( ! thePA->IsEmpty() ) {
            for ( int j = 1; j <= thePA->Length(); ++j ) 
               thePA->Value(j)->GetPrimitive()->Unhighlight();
            theIO->ClearSeqSelPrim();
          }
            theIO->SetState(0);
            if ( myObjects.IsBound(theIO) ) 
	        if ( myObjects(theIO)->IsSubIntensityOn() )
	          HighlightWithColor( theIO, mySubIntensity, Standard_False );
       }
       
     }
     break;
    case AIS2D_TOD_ELEMENT: {
      Handle(AIS2D_InteractiveObject) theIO;
      Handle(AIS2D_HSequenceOfPrimArchit) thePA;
      for ( int i = 1; i <= mySeqOfSelIO->Length(); ++i ) {
        theIO = mySeqOfSelIO->Value(i);
        thePA = theIO->GetSelectedSeqPrim();
        theIO->Unhighlight();
        if ( ! thePA->IsEmpty() ) {
          for ( int j = 1; j <= thePA->Length(); ++j ) {
             thePA->Value(j)->GetPrimitive()->Unhighlight();
	     // SAV - unselecting sub primitives 
	     thePA->Value(j)->GetPrimitive()->ClearSelectedElements();
	   }
          theIO->ClearSeqSelPrim();
        } 
        theIO->SetState(0);
        if ( myObjects.IsBound(theIO) ) 
	    if ( myObjects(theIO)->IsSubIntensityOn() )
	        HighlightWithColor( theIO, mySubIntensity, Standard_False );
      }
    }
    break;
    case AIS2D_TOD_VERTEX: {
      Handle(AIS2D_InteractiveObject) theIO;
      Handle(AIS2D_HSequenceOfPrimArchit) thePA;
      for ( int i = 1; i <= mySeqOfSelIO->Length(); ++i ) {
        theIO = mySeqOfSelIO->Value(i);
        thePA = theIO->GetSelectedSeqPrim();
        theIO->Unhighlight();
        if ( ! thePA->IsEmpty() ) {
          for ( int j = 1; j <= thePA->Length(); ++j ) 
             thePA->Value(j)->GetPrimitive()->Unhighlight();
          theIO->ClearSeqSelPrim();
        } 
        theIO->SetState(0);
        if ( myObjects.IsBound(theIO) ) 
	    if ( myObjects(theIO)->IsSubIntensityOn() )
	       HighlightWithColor( theIO, mySubIntensity, Standard_False );
      }
     }
    break;
   } // end switch
   mySeqOfSelIO->Clear();
  }  // end if sequence of detected objects isn't empty
  

  }
  if ( UpdateVwr ) myWasLastMain ? myMainVwr->Update() : UpdateCollector();

  Standard_Integer NS = mySeqOfSelIO->Length();
  if ( !NS ) return AIS2D_SOP_NothingSelected;
  if ( NS == 1 ) return AIS2D_SOP_OneSelected;

  return AIS2D_SOP_SeveralSelected;
 
}

AIS2D_StatusOfPick AIS2D_InteractiveContext::ShiftSelect( const Standard_Boolean UpdateVwr ) 
{  
  if ( HasOpenedContext() ) {
    if ( myWasLastMain )
      return myLocalContexts( myCurLocalIndex )->ShiftSelect( UpdateVwr );
    else {
      myLocalContexts( myCurLocalIndex )->AddOrRemoveSelected( myLastPicked, UpdateVwr );
      Standard_Integer NS = mySeqOfSelIO->Length();
      if ( !NS ) return AIS2D_SOP_NothingSelected;
      if ( NS == 1 ) return AIS2D_SOP_OneSelected;
      return AIS2D_SOP_SeveralSelected;
    }
  } //end if opened local context exists
  
  if ( myWasLastMain && !myLastinMain.IsNull() )
    AddOrRemoveCurObject( myLastinMain,UpdateVwr );
  else if ( !myWasLastMain && !myLastinColl.IsNull() )
    AddOrRemoveCurObject( myLastinColl, UpdateVwr );
    
  Standard_Integer NS = mySeqOfSelIO->Length();
  if ( NS == 0 ) return AIS2D_SOP_NothingSelected;
  if ( NS == 1 ) return AIS2D_SOP_OneSelected;
  return AIS2D_SOP_SeveralSelected;

}

AIS2D_StatusOfPick AIS2D_InteractiveContext::ShiftSelect(
					 const Standard_Integer /*XPMin*/, 
					 const Standard_Integer /*YPMin*/, 
					 const Standard_Integer /*XPMax*/, 
					 const Standard_Integer /*YPMax*/, 
					 const Handle(V2d_View)& /*aView*/,
					 const Standard_Boolean  /*UpdateVwr*/ ) 
{
/*  
  if ( HasOpenedContext() )
    return myLocalContexts( myCurLocalIndex )->ShiftSelect( XPMin, YPMin, XPMax, YPMax, aView, UpdateVwr );
  
  UnhighlightCurrent(Standard_False);
 // static Handle(StdSelect_ViewerSelector2d) aSelector;
  
  if ( aView->Viewer() == myMainVwr) {
   // aSelector= myMainSelector;
    myWasLastMain = Standard_True;}
  
  else if (aView->Viewer() == myCollectorVwr){
   // aSelector= myCollectorSel;
    myWasLastMain = Standard_False;}
  else
    return AIS2D_SOP_NothingSelected;
  aSelector->SetSensitivity( aView->Convert(mySensitivity) );
  aSelector->Pick( XPMin, YPMin, XPMax, YPMax, aView );
  AIS2D_Selection::SetCurrentSelection( myCurrentName.ToCString() );
  for ( aSelector->Init(); aSelector->More(); aSelector->Next() ) {
    const Handle(SelectMgr_SelectableObject)& SObj = aSelector->Picked()->Selectable();
    if ( !SObj.IsNull() ) {
      AIS2D_SelectStatus SelStat = AIS2D_Selection::Select(SObj);
      Standard_Integer mod = (SelStat == AIS2D_SS_Added) ? 1 : 0;
      ( * ( (Handle(AIS2D_InteractiveObject)* ) & SObj ) )->SetState(mod);
    }
  }
  
  HighlightCurrent( UpdateVwr );
  
  Standard_Integer NS = NbCurrents();
  if ( NS == 0 ) return AIS2D_SOP_NothingSelected;
  if ( NS == 1 ) return AIS2D_SOP_OneSelected;
  return AIS2D_SOP_SeveralSelected;
 */ 
    return AIS2D_SOP_NothingSelected;
}

 Standard_Integer AIS2D_InteractiveContext::NbSelected() const
{
     if ( mySeqOfSelIO->IsEmpty() ) 
       return 0;

     Standard_Integer nsel = 0;
     switch ( myCurDetectMode ) 
     {
       case AIS2D_TOD_ELEMENT:
       case AIS2D_TOD_VERTEX:
       case AIS2D_TOD_PRIMITIVE:
       {
	 for ( int i = 1; i <= mySeqOfSelIO->Length(); ++i ) 
	 {
	   Handle( AIS2D_InteractiveObject ) io = mySeqOfSelIO->Value( i );
	   if ( !io.IsNull() )
	   nsel += io->GetSelectedSeqPrim()->Length();
	 }
	 break;
       }
       default :
	 nsel = mySeqOfSelIO->Length(); break;
     }
     return nsel;
}

 void AIS2D_InteractiveContext::InitSelected() 
{
  myCurSelected = 1;
  myMaxSelected = NbSelected();
}

Standard_Boolean AIS2D_InteractiveContext::MoreSelected() const
{
  return myCurSelected <= myMaxSelected; 
}

void AIS2D_InteractiveContext::NextSelected() 
{
  myCurSelected++;
}

Handle( MMgt_TShared ) AIS2D_InteractiveContext::SelectedObject() const
{
  switch ( myCurDetectMode ) 
  {
    case AIS2D_TOD_ELEMENT:
    case AIS2D_TOD_VERTEX:
    case AIS2D_TOD_PRIMITIVE:
    {
      Standard_Integer current = 0;
      for ( int i = 1; i <= mySeqOfSelIO->Length(); ++i ) 
      {
	Handle( AIS2D_InteractiveObject ) io = mySeqOfSelIO->Value( i );
	if ( !io.IsNull() ) 
        {
	  Standard_Integer l = io->GetSelectedSeqPrim()->Length();
	  if ( myCurSelected <= current + l ) 
	  {
	    return io->GetSelectedSeqPrim()->Value( myCurSelected - current )->GetPrimitive();
	  }
	  current += l;
        }
      }
      break;
    }
    default: break;      
  }
  return mySeqOfSelIO->Value( myCurSelected );
}

// *******************************************************************************
//     Immediate Mode Management
// *******************************************************************************

void AIS2D_InteractiveContext::BeginImmediateDraw( const Handle(V2d_View)& aView ) {

 Handle(V2d_Viewer) theViewer = aView->Viewer();
 Handle(Graphic2d_TransientManager) theDrawer = 
     Handle(Graphic2d_TransientManager)::DownCast( theViewer->View()->Drawer() );
 theDrawer->BeginDraw( aView->Driver() );
 
}

void AIS2D_InteractiveContext::ImmediateDraw( const Handle(AIS2D_InteractiveObject)& anIObj,
                                              const Handle(V2d_View)& aView ) {

 Handle(V2d_Viewer) theViewer = aView->Viewer();
 Handle(Graphic2d_TransientManager) theDrawer = 
   Handle(Graphic2d_TransientManager)::DownCast( theViewer->View()->Drawer() );
 theDrawer->Draw( anIObj );
 
}

void AIS2D_InteractiveContext::EndImmediateDraw( const Handle(V2d_View)& aView ) {
 Handle(V2d_Viewer) theViewer = aView->Viewer();
 Handle(Graphic2d_TransientManager) theDrawer = 
   Handle(Graphic2d_TransientManager)::DownCast( theViewer->View()->Drawer() );
 theDrawer->EndDraw(); 
}

Standard_Boolean AIS2D_InteractiveContext::IsImmediateModeOn(const Handle(V2d_View)& /*aView*/) const {
return Standard_False; 
}

//SAV
void AIS2D_InteractiveContext::SetDetectionMode( const AIS2D_TypeOfDetection mode )
{
  myCurDetectMode = mode;
}
