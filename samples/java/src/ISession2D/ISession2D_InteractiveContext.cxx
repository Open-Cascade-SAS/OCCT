#include <ISession2D_InteractiveContext.ixx>

#include <ISession2D_ObjectOwner.hxx>
#include <V2d_Viewer.hxx>
#include <V2d_View.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_Shape.hxx>
#include <PrsMgr_PresentationManager2d.hxx>
#include <SelectMgr_SelectionManager.hxx>
#include <SelectMgr_SelectableObject.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <StdSelect_ViewerSelector2d.hxx>
#include <StdSelect_TextProjector2d.hxx>
#include <TColStd_MapIteratorOfMapOfTransient.hxx>
#include <Graphic2d_Buffer.hxx>


ISession2D_InteractiveContext::ISession2D_InteractiveContext()
{
}

ISession2D_InteractiveContext::ISession2D_InteractiveContext(const Handle(V2d_Viewer)& aViewer)
{
  Initialize(aViewer);
}

 void ISession2D_InteractiveContext::Initialize(const Handle(V2d_Viewer)& aViewer) 
{
  myViewer = aViewer;
  myPrsmgr           = new PrsMgr_PresentationManager2d(myViewer->View());
  mySelectionManager = new SelectMgr_SelectionManager();
  mySelector         = new StdSelect_ViewerSelector2d();

  mySelector->Set(3); 
  // set Sensitivity very very important for SensitiveCurve !!
  mySelector->SetSensitivity (0.5);   // en mm
  mySelectionManager->Add(mySelector);

  myHilightColorIndex = 5;       // for dynamic highlight
  IsAreasDisplayed = Standard_False;

  myPrsmgr->SetHighlightColor(8); // For Static highlight
}

 void ISession2D_InteractiveContext::Display(const Handle(AIS_InteractiveObject)& anObject,
					     const Standard_Boolean Redraw) 
{
  myObjects.Add(anObject);
 
  Standard_Integer DispMode ,HiMode,SelMode;
  GetDefModes(anObject,DispMode ,HiMode,SelMode);

  myPrsmgr->Display(anObject,DispMode); 

  mySelectionManager->Load(anObject,mySelector);
  mySelectionManager->Activate(anObject,SelMode,mySelector);

  if (Redraw) 
    myViewer->Update();
}

 void ISession2D_InteractiveContext::Display(const Handle(AIS_InteractiveObject)& anObject,
					     const Standard_Integer aDisplayMode,
					     const Standard_Integer aSelectionMode,
					     const Standard_Boolean Redraw) 
{
  myObjects.Add(anObject);
  anObject->SetDisplayMode(aDisplayMode);

  anObject->SetSelectionMode(aSelectionMode);

  myObjects.Add(anObject);
  myPrsmgr->Display(anObject,aDisplayMode); 

  mySelectionManager->Load(anObject,mySelector);
  mySelectionManager->Activate(anObject,aSelectionMode,mySelector);
  if (Redraw) myViewer->Update();
}

 void ISession2D_InteractiveContext::Erase(const Handle(AIS_InteractiveObject)& anObject,
					   const Standard_Boolean Redraw) 
{
  if (myObjects.Contains(anObject)) {
    Standard_Integer DispMode ,HiMode,SelMode;
    GetDefModes(anObject,DispMode ,HiMode,SelMode);
    myPrsmgr->Erase(anObject,DispMode);
    mySelectionManager->Deactivate(anObject,mySelector);
    if (Redraw) myViewer->Update();
  }
}

 void ISession2D_InteractiveContext::DisplayAll(const Standard_Boolean Redraw) 
{
  TColStd_MapIteratorOfMapOfTransient It(myObjects);
  for(;It.More();It.Next()){
    Handle(AIS_InteractiveObject) anObject = Handle(AIS_InteractiveObject)::DownCast(It.Key());
    Standard_Integer DispMode ,HiMode,SelMode;
    GetDefModes(anObject,DispMode ,HiMode,SelMode);
    myPrsmgr->Display(anObject,DispMode); 
    mySelectionManager->Load(anObject,mySelector);
    mySelectionManager->Activate(anObject,HiMode,mySelector);
  }
  if (Redraw) myViewer->Update();
}

 void ISession2D_InteractiveContext::EraseAll(const Standard_Boolean Redraw) 
{
  TColStd_MapIteratorOfMapOfTransient It(myObjects);
  for(;It.More();It.Next()){
    Handle(AIS_InteractiveObject) anObject = Handle(AIS_InteractiveObject)::DownCast(It.Key());
    Standard_Integer DispMode ,HiMode,SelMode;
    GetDefModes(anObject,DispMode ,HiMode,SelMode);
    myPrsmgr->Erase(anObject,DispMode); 
    mySelectionManager->Deactivate(anObject,mySelector);
  }
  if (Redraw) myViewer->Update();
}

 Standard_Boolean ISession2D_InteractiveContext::IsDisplayed(const Handle(AIS_InteractiveObject)& anObject,
							     const Standard_Integer aMode) 
{
  return myPrsmgr->IsDisplayed(anObject,aMode);
}

 void ISession2D_InteractiveContext::Redisplay(const Handle(AIS_InteractiveObject)& anObject,
					       const Standard_Boolean Redraw,
					       const Standard_Boolean ) 
{
  if (myObjects.Contains(anObject)) {
    Standard_Integer DispMode ,HiMode,SelMode;
    GetDefModes(anObject,DispMode ,HiMode,SelMode);
    myPrsmgr->Update(anObject,DispMode); 
    
    // WARNING : IMPLEMENTATION IS NOT FINISH !!!!!!!!
    // NEED TO UPDATE THE OTHER MODES, AND SELECTION !!!!!

    //  mySelectionManager->Load(anObject,mySelector);
    //  mySelectionManager->Activate(anObject,SelMode,mySelector);

    if (Redraw) myViewer->Update();
  }
}

 void ISession2D_InteractiveContext::Clear(const Handle(AIS_InteractiveObject)& ,
					   const Standard_Boolean ) 
{
  Standard_Failure::Raise("NotYetImplemented");
}

 void ISession2D_InteractiveContext::Remove(const Handle(AIS_InteractiveObject)& anObject,
					    const Standard_Boolean Redraw) 
{
  if (myObjects.Contains(anObject)) {
    Erase(anObject,Redraw);
    myObjects.Remove(anObject);
  }
}

 void ISession2D_InteractiveContext::Highlight(const Handle(AIS_InteractiveObject)& anObject,
					       const Standard_Boolean Redraw) 
{
  if (myObjects.Contains(anObject)) {
    Standard_Integer DispMode ,HiMode,SelMode;
    GetDefModes(anObject,DispMode ,HiMode,SelMode);
    myPrsmgr->Highlight(anObject,HiMode);
    if(Redraw) myViewer->Update();
  }
}

 void ISession2D_InteractiveContext::Unhighlight(const Handle(AIS_InteractiveObject)& anObject,
						 const Standard_Boolean Redraw) 
{
  if (myObjects.Contains(anObject)) {
    Standard_Integer DispMode ,HiMode,SelMode;
    GetDefModes(anObject,DispMode ,HiMode,SelMode);
    myPrsmgr->Unhighlight(anObject,HiMode);
    if(Redraw) myViewer->Update();
  }
}

 Standard_Boolean ISession2D_InteractiveContext::IsHilighted(const Handle(AIS_InteractiveObject)& anObject) 
{
  if (myObjects.Contains(anObject)) {
    Standard_Integer DispMode ,HiMode,SelMode;
    GetDefModes(anObject,DispMode ,HiMode,SelMode);
    return myPrsmgr->IsHighlighted(anObject,HiMode);
  }
  return Standard_False;
}

 Standard_Boolean ISession2D_InteractiveContext::NewProjector(const Handle(V2d_View)& aView,
							      Handle(Select2D_Projector)& NewProjector) 
{
  // to be changed : here we build a new projector each time.
  // or we have to define a new one only if we change :
  //  the View
  //  the view zoom factor
  //  the view panning
  //  the fonts changes.
  NewProjector = new StdSelect_TextProjector2d(aView);
  return Standard_True;
}

// Manage Detected : update The Display
 void ISession2D_InteractiveContext::ManageDetected(TColStd_MapOfTransient& aNewMapOfDetectedOwner) 
{
  Standard_Boolean NeedRedraw = Standard_False;
  //======================================================================
  //  Treatement on the old Selection :
  //  for each owner : 
  //        if the object is decomposed : Unhighlight the Owner,
  //                                 else Unhighlight the Object,except if selected  
  //======================================================================

  TColStd_MapIteratorOfMapOfTransient anIterator(myMapOfDetectedOwner);
  for (;anIterator.More();anIterator.Next())
    {
      // for all owner :
      Handle(ISession2D_ObjectOwner) TheOwner = 
	Handle(ISession2D_ObjectOwner)::DownCast(anIterator.Key());
      
      Handle(AIS_InteractiveObject) TheObject = 
	Handle(AIS_InteractiveObject)::DownCast(TheOwner->Selectable());

      // if the owner is not still detected
      if (!aNewMapOfDetectedOwner.Contains(TheOwner))
	{
	  NeedRedraw = Standard_True;
	  // if the corresponding object is a Shape and is not Decomposed
	  Handle(AIS_Shape) anAISShape = Handle(AIS_Shape)::DownCast(TheObject);
	  Standard_Boolean TreatementOnCompleteObject = Standard_True;
	  if (!anAISShape.IsNull())
	    if (anAISShape->AcceptShapeDecomposition() && anAISShape->HasSelectionMode())
	      TreatementOnCompleteObject = Standard_False;
	  if(TreatementOnCompleteObject)
	    {
              // if the corresponding object is not Selected
              if (!myMapOfSelectedOwner.Contains(TheOwner))
		{
		  Standard_Integer DispMode ,HiMode,SelMode;
		  GetDefModes(TheObject,DispMode ,HiMode,SelMode);
		  if ( myPrsmgr->IsHighlighted(TheObject))
		    myPrsmgr->Unhighlight(TheObject,HiMode);
		}
	    }
	  else
	    {
	      // UnHighlight the old detected Owner, 
	      TheOwner->Unhilight(myPrsmgr);
	    }
	}
    }

  //======================================================================
  //      Treatement on the New Selection
  //======================================================================

  anIterator.Initialize(aNewMapOfDetectedOwner);

  for (;anIterator.More();anIterator.Next())
    {
      // for all owner :
      Handle(ISession2D_ObjectOwner) TheOwner = 
	Handle(ISession2D_ObjectOwner)::DownCast(anIterator.Key());
      Handle(AIS_InteractiveObject) TheObject = 
	Handle(AIS_InteractiveObject)::DownCast(TheOwner->Selectable());
      
      // if the owner is not alwraidy detected
      if (!myMapOfDetectedOwner.Contains(TheOwner))
	{
          NeedRedraw = Standard_True;
	  
	  // if the corresponding object is a Shape and is not Decomposed
	  Handle(AIS_Shape) anAISShape = Handle(AIS_Shape)::DownCast(TheObject);
	  Standard_Boolean TreatementOnCompleteObject = Standard_True;
	  if (!anAISShape.IsNull())
	    if (anAISShape->AcceptShapeDecomposition() && anAISShape->HasSelectionMode())
	      TreatementOnCompleteObject = Standard_False;
	  if(TreatementOnCompleteObject)
	    {
	      // if the corresponding object is not Selected
	      if (!myMapOfSelectedOwner.Contains(TheOwner))
		{
		  Standard_Integer DispMode ,HiMode,SelMode;
		  GetDefModes(TheObject,DispMode ,HiMode,SelMode);
  	          if (!myPrsmgr->IsHighlighted(TheObject))
		    myPrsmgr->ColorHighlight(TheObject,myHilightColorIndex,HiMode);
		}
	    }
          else
	    {
	      // The Object Is Decomposed --> Specific Treatement on the owner
	      TheOwner->Hilight(myPrsmgr);
	    }
        }
    }
  //======================================================================
  //     set the new detected map
  //======================================================================

  // set The new detected Owner
  myMapOfDetectedOwner = aNewMapOfDetectedOwner;
  
  if (NeedRedraw) myViewer->Update();
}

 void ISession2D_InteractiveContext::Move(const Standard_Integer x1,
					  const Standard_Integer y1,
					  const Handle(V2d_View)& aView) 
{
  Handle(Select2D_Projector)  aProjector;
  if (NewProjector(aView,aProjector)) // if we need a new projector
    mySelector->Set(aProjector);

  mySelector->Pick(x1,y1,aView);  
  TColStd_MapOfTransient aNewMapOfDetectedOwner;
  mySelector->Init();
  if(mySelector->More())
    {    
      Handle(SelectMgr_EntityOwner) aOwner = mySelector->OnePicked();
      if (aOwner->IsKind(STANDARD_TYPE(ISession2D_ObjectOwner))) {
	Handle(ISession2D_ObjectOwner) aNewDetectedObjectOwner = 
	  Handle(ISession2D_ObjectOwner)::DownCast(aOwner);
	aNewMapOfDetectedOwner.Add(aNewDetectedObjectOwner);
      }
    }

  ManageDetected(aNewMapOfDetectedOwner);
}

 void ISession2D_InteractiveContext::Move(const Standard_Integer x1,
					  const Standard_Integer y1,
					  const Standard_Integer x2,
					  const Standard_Integer y2,
					  const Handle(V2d_View)& aView) 
{
  Handle(Select2D_Projector)  aProjector;
  if (NewProjector(aView,aProjector)) // if we need a new projector
    mySelector->Set(aProjector);

  mySelector->Pick(x1,y1,x2,y2,aView);  
  TColStd_MapOfTransient aNewMapOfDetectedOwner;
  for(mySelector->Init();mySelector->More();mySelector->Next())
    {    
      Handle(SelectMgr_EntityOwner) aOwner = mySelector->Picked();
      if (aOwner->IsKind(STANDARD_TYPE(ISession2D_ObjectOwner))) {
	Handle(ISession2D_ObjectOwner) aNewDetectedObjectOwner = 
	  Handle(ISession2D_ObjectOwner)::DownCast(aOwner);
	aNewMapOfDetectedOwner.Add(aNewDetectedObjectOwner);
      }
    }
  ManageDetected(aNewMapOfDetectedOwner); 
}

 void ISession2D_InteractiveContext::Pick(const Standard_Boolean MultiSelection) 
{
  // We have a Map of all the seletected owners : myMapOfSelectedOwner;
  // if we are not in MultiSelection. 
  //    UnHighLight and keep out the map all the last selected objects

  if (!MultiSelection)
    {
      TColStd_MapIteratorOfMapOfTransient anIterator(myMapOfSelectedOwner);
      for (;anIterator.More();anIterator.Next())
	{
	  Handle(ISession2D_ObjectOwner) TheOwner = 
	    Handle(ISession2D_ObjectOwner)::DownCast(anIterator.Key());
	  Handle(AIS_InteractiveObject) TheObject = 
	    Handle(AIS_InteractiveObject)::DownCast(TheOwner->Selectable());
	  
	  // UnHighLight The Last Selected ObjectObject.
	  Unhighlight(TheObject,  // Object
		      Standard_False);   // Redraw
	}
      myMapOfSelectedOwner.Clear();
    }

  TColStd_MapIteratorOfMapOfTransient anIterator(myMapOfDetectedOwner);
  for (;anIterator.More();anIterator.Next())
    {
      // for all owner :
      Handle(ISession2D_ObjectOwner) TheOwner = 
	Handle(ISession2D_ObjectOwner)::DownCast(anIterator.Key());
      myMapOfSelectedOwner.Add(TheOwner);
      
      Handle(AIS_InteractiveObject) TheObject = 
	Handle(AIS_InteractiveObject)::DownCast(TheOwner->Selectable());
      Highlight(TheObject,  // Object
		Standard_False);   // Redraw
    }
  myViewer->Update();
}

 void ISession2D_InteractiveContext::GetDefModes(const Handle(AIS_InteractiveObject)& anObject,
						 Standard_Integer& DispMode,
						 Standard_Integer& HiMode,
						 Standard_Integer& SelMode) const
{
  DispMode = anObject->HasDisplayMode() ? anObject->DisplayMode() : 
                                          anObject->DefaultDisplayMode();
  
  HiMode = anObject->HasHilightMode() ? anObject->HilightMode() : DispMode;
  
  SelMode = anObject->HasSelectionMode() ? anObject->SelectionMode() : 0;

}

 void ISession2D_InteractiveContext::DisplayAreas() 
{
  if (IsAreasDisplayed) return;
  myViewer->InitActiveViews();
  while(myViewer->MoreActiveViews())
    {
      Handle(V2d_View)  aView  = myViewer->ActiveView();
      mySelector->DisplayAreas(aView);    
      myViewer->NextActiveViews();
    }
  myViewer->Update();
  IsAreasDisplayed= Standard_True;
}

 void ISession2D_InteractiveContext::ClearAreas() 
{
  if (!IsAreasDisplayed) return;
  mySelector->ClearAreas();    
  myViewer->Update();
  IsAreasDisplayed= Standard_False;
}

