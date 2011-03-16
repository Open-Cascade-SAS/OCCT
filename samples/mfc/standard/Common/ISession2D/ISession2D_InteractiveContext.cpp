// File:	ISession2D_InteractiveContext.cxx
// Created:	Thu Jul 10 18:53:50 1997
// Author:	David Enjolras
//		<den@zodiax.paris3.matra-dtv.fr>


#include "stdafx.h"
#include <OCC_MainFrame.h>// use for Message Bar

#include "ISession2D_InteractiveContext.h"
#include <SelectMgr_EntityOwner.hxx>
#include <PrsMgr_PresentationManager2d.hxx>
#include <SelectMgr_SelectionManager.hxx>
#include <StdSelect_TextProjector2d.hxx>

IMPLEMENT_STANDARD_HANDLE(ISession2D_InteractiveContext,MMgt_TShared)
IMPLEMENT_STANDARD_RTTIEXT(ISession2D_InteractiveContext,MMgt_TShared)

#include "ISession2D_ObjectOwner.h"
#include "TColStd_MapIteratorOfMapOfTransient.hxx"

 ISession2D_InteractiveContext::ISession2D_InteractiveContext()
{
}

 ISession2D_InteractiveContext::ISession2D_InteractiveContext(const Handle(V2d_Viewer)& aViewer)
{
  Initialize(aViewer);
}

void ISession2D_InteractiveContext::Initialize(const Handle(V2d_Viewer)& aViewer) 
{
  ASSERT(!aViewer.IsNull());
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

void ISession2D_InteractiveContext::Display   (const Handle(AIS_InteractiveObject)& anObject,
                                               const Standard_Boolean Redraw ) 
{
  ASSERT(!anObject.IsNull());
  myObjects.Add(anObject);
 
  Standard_Integer DispMode ,HiMode,SelMode;
  GetDefModes(anObject,DispMode ,HiMode,SelMode);

  myPrsmgr->Display(anObject,DispMode); 

  mySelectionManager->Load(anObject,mySelector);
  mySelectionManager->Activate(anObject,SelMode,mySelector);

  if (Redraw) myViewer->Update();
}

void ISession2D_InteractiveContext::Display   (const Handle(AIS_InteractiveObject)& anObject,
                                               const Standard_Integer aDisplayMode, 
				                               const Standard_Integer aSelectionMode, 
				                               const Standard_Boolean Redraw ) 
{
  ASSERT(!anObject.IsNull());
  myObjects.Add(anObject);
  ASSERT(anObject->AcceptDisplayMode(aDisplayMode));
  anObject->SetDisplayMode(aDisplayMode);

  anObject->SetSelectionMode(aSelectionMode);

  myObjects.Add(anObject);
  myPrsmgr->Display(anObject,aDisplayMode); 

  mySelectionManager->Load(anObject,mySelector);
  mySelectionManager->Activate(anObject,aSelectionMode,mySelector);
  if (Redraw) myViewer->Update();
}

void ISession2D_InteractiveContext::Erase     (const Handle(AIS_InteractiveObject)& anObject,
                                               const Standard_Boolean Redraw ) 
{
  ASSERT(!myPrsmgr.IsNull());
  ASSERT(!anObject.IsNull());
  ASSERT(myObjects.Contains(anObject));
  Standard_Integer DispMode ,HiMode,SelMode;
  GetDefModes(anObject,DispMode ,HiMode,SelMode);
  myPrsmgr->Erase(anObject,DispMode);
  mySelectionManager->Deactivate(anObject,mySelector);
  if (Redraw) myViewer->Update();
}

void ISession2D_InteractiveContext::DisplayAll(const Standard_Boolean Redraw )
{
  ASSERT(!myPrsmgr.IsNull());

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

void ISession2D_InteractiveContext::EraseAll  (const Standard_Boolean Redraw )
{
  ASSERT(!myPrsmgr.IsNull());
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
		     
Standard_Boolean  ISession2D_InteractiveContext::IsDisplayed(const Handle(AIS_InteractiveObject)& anObject,
                                                             const Standard_Integer aMode) 
{
  ASSERT(!myPrsmgr.IsNull());
  ASSERT(!anObject.IsNull());
  return myPrsmgr->IsDisplayed(anObject,aMode);
}

void ISession2D_InteractiveContext::Redisplay(const Handle(AIS_InteractiveObject)& anObject,
                                              const Standard_Boolean Redraw,
                	                          const Standard_Boolean allmodes)
{
  ASSERT(!myPrsmgr.IsNull());
  ASSERT(!anObject.IsNull());
  ASSERT(myObjects.Contains(anObject));

  Standard_Integer DispMode ,HiMode,SelMode;
  GetDefModes(anObject,DispMode ,HiMode,SelMode);
  myPrsmgr->Update(anObject,DispMode); 

  // WARNING : IMPLEMENTATION IS NOT FINISH !!!!!!!!
  // NEED TO UPDATE THE OTHER MODES, AND SELECTION !!!!!

  //  mySelectionManager->Load(anObject,mySelector);
  //  mySelectionManager->Activate(anObject,SelMode,mySelector);

  if (Redraw) myViewer->Update();

}

void ISession2D_InteractiveContext::Clear     (const Handle(AIS_InteractiveObject)& anObject,
                        const Standard_Boolean Redraw)
{
  ASSERT(!myPrsmgr.IsNull());
  ASSERT(!anObject.IsNull());
  ASSERT(myObjects.Contains(anObject));

  Standard_Failure::Raise("NotYetImplemented");
}

void ISession2D_InteractiveContext::Remove    (const Handle(AIS_InteractiveObject)& anObject,
                        const Standard_Boolean Redraw)
{
  ASSERT(!myPrsmgr.IsNull());
  ASSERT(!anObject.IsNull());
  ASSERT(myObjects.Contains(anObject));
  Erase(anObject,Redraw);
  myObjects.Remove(anObject);
}

void ISession2D_InteractiveContext::Highlight   (const Handle(AIS_InteractiveObject)& anObject,
                                               const Standard_Boolean Redraw)
{
  ASSERT(!myPrsmgr.IsNull());
  ASSERT(!anObject.IsNull());
  ASSERT(myObjects.Contains(anObject));
  Standard_Integer DispMode ,HiMode,SelMode;
  GetDefModes(anObject,DispMode ,HiMode,SelMode);
  myPrsmgr->Highlight(anObject,HiMode);
  if(Redraw) myViewer->Update();
}
    	    
void ISession2D_InteractiveContext::Unhighlight (const Handle(AIS_InteractiveObject)& anObject,
                        const Standard_Boolean Redraw)
{
  ASSERT(!myPrsmgr.IsNull());
  ASSERT(!anObject.IsNull());
  ASSERT(myObjects.Contains(anObject));
  Standard_Integer DispMode ,HiMode,SelMode;
  GetDefModes(anObject,DispMode ,HiMode,SelMode);
  myPrsmgr->Unhighlight(anObject,HiMode);
  if(Redraw) myViewer->Update();
}

Standard_Boolean  ISession2D_InteractiveContext::IsHilighted(const Handle(AIS_InteractiveObject)& anObject)
{
  ASSERT(!myPrsmgr.IsNull());
  ASSERT(!anObject.IsNull());
  ASSERT(myObjects.Contains(anObject));
  Standard_Integer DispMode ,HiMode,SelMode;
  GetDefModes(anObject,DispMode ,HiMode,SelMode);
  return myPrsmgr->IsHighlighted(anObject,HiMode);
}

Standard_Boolean ISession2D_InteractiveContext::NewProjector(const Handle(V2d_View)& aView,
                                                             Handle(Select2D_Projector)& NewProjector) // out parameter.
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
/*private*/ void ISession2D_InteractiveContext::ManageDetected(TColStd_MapOfTransient& aNewMapOfDetectedOwner)
{
  Standard_Boolean NeedRedraw = Standard_False;
  //======================================================================
  //  Treatement on the old Selection :
  //  for each owner : 
  //        if the object is decomposed : Unhighlight the Owner,
  //                                 else Unhighlight the Object,except if selected  
  //======================================================================

  TColStd_MapIteratorOfMapOfTransient anIterator(myMapOfDetectedOwner);
  for (;anIterator.More();
        anIterator.Next())
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
  if (!anIterator.More()) ((OCC_MainFrame*)AfxGetMainWnd())->SetStatusMessage("Nothing Detected");
  for (;anIterator.More();
        anIterator.Next())
    {
      // for all owner :
      Handle(ISession2D_ObjectOwner) TheOwner = 
          Handle(ISession2D_ObjectOwner)::DownCast(anIterator.Key());
      // update the message bar
      ((OCC_MainFrame*)AfxGetMainWnd())->SetStatusMessage(TheOwner->DumpMessage().ToCString());
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
    ASSERT(aOwner->IsKind(STANDARD_TYPE(ISession2D_ObjectOwner)));
    Handle(ISession2D_ObjectOwner) aNewDetectedObjectOwner = 
       Handle(ISession2D_ObjectOwner)::DownCast(aOwner);
    aNewMapOfDetectedOwner.Add(aNewDetectedObjectOwner);
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
    ASSERT(aOwner->IsKind(STANDARD_TYPE(ISession2D_ObjectOwner)));
    Handle(ISession2D_ObjectOwner) aNewDetectedObjectOwner = 
       Handle(ISession2D_ObjectOwner)::DownCast(aOwner);
    aNewMapOfDetectedOwner.Add(aNewDetectedObjectOwner);
  }
 ManageDetected(aNewMapOfDetectedOwner); 
}

void ISession2D_InteractiveContext::Pick(const Standard_Boolean MultiSelection)// = Standard_False
{
  // We have a Map of all the seletected owners : myMapOfSelectedOwner;
  // if we are not in MultiSelection. 
  //    UnHighLight and keep out the map all the last selected objects

  if (!MultiSelection)
    {
     TColStd_MapIteratorOfMapOfTransient anIterator(myMapOfSelectedOwner);
     for (;anIterator.More();
           anIterator.Next())
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
  for (;anIterator.More();
        anIterator.Next())
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

//=======================================================================
//function : GetDefModes
//purpose  : 
//=======================================================================

void ISession2D_InteractiveContext::GetDefModes(const Handle(AIS_InteractiveObject)& anObject,
				     Standard_Integer& DispMode,
				     Standard_Integer& HiMode,
				     Standard_Integer& SelMode)  
{
  DispMode = anObject->HasDisplayMode() ? anObject->DisplayMode() : 
                                          anObject->DefaultDisplayMode();
  
  HiMode = anObject->HasHilightMode()? anObject->HilightMode():DispMode;

  SelMode = anObject->HasSelectionMode()? anObject->SelectionMode() : 
                                          0;
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


