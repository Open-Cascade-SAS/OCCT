#include "stdafx.h"

#include <ISession2D_InteractiveContext.h>

IMPLEMENT_STANDARD_HANDLE(ISession2D_InteractiveContext,MMgt_TShared)
IMPLEMENT_STANDARD_RTTIEXT(ISession2D_InteractiveContext,MMgt_TShared)

#include <Aspect_Window.hxx>
#include <Aspect_Background.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_SelectionManager.hxx>
#include <V2d_Viewer.hxx>
#include <V2d_View.hxx>
#include <PrsMgr_PresentationManager2d.hxx>
#include <StdSelect_ViewerSelector2d.hxx>
#include <SelectMgr_SelectableObject.hxx>
#include <TColStd_MapIteratorOfMapOfTransient.hxx>

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

}

void ISession2D_InteractiveContext::Display(const Handle(AIS_InteractiveObject)& anObject,
				                       const Standard_Boolean Redraw) 
{
   myMapOfObject.Add(anObject);
   myPrsmgr->Display(anObject);//,anObject->DisplayMode()); 
    
   mySelectionManager->Load(anObject,mySelector);
   mySelectionManager->Activate(anObject,0,mySelector);

  if (Redraw) myViewer->Update();
}

void ISession2D_InteractiveContext::Erase(const Handle(AIS_InteractiveObject)& anObject,
			                         const Standard_Boolean Redraw) 
{
  if (!anObject.IsNull())
  {
    myPrsmgr->Erase(anObject);//,anObject->DisplayMode());
    mySelectionManager
      ->Deactivate(anObject,mySelector);
    if (Redraw) myViewer->Update();
  }
}

void ISession2D_InteractiveContext::Move(const Handle(V2d_View)& aView,
                                    const Standard_Integer x1,
                                    const Standard_Integer y1) 
{
  if (!aBuffer.IsNull())
  {
    aBuffer->UnPost(); 
    aBuffer->Clear();
  }

  if (myViewer->IsActive())  // A propos de la grille !!!
    {      
      aView->ShowHit(x1,y1);
    }

  aBuffer =new Graphic2d_Buffer(aView->View(),0,0,3,5);
  mySelector->Pick(x1,y1,aView);  
  
  Handle(SelectMgr_SelectableObject) aSelectableObject; 

  mySelector->Init();

  while (mySelector->More())
  {
      aSelectableObject = Handle(SelectMgr_SelectableObject)::DownCast(mySelector->Picked()->Selectable());

      myPrsmgr->Dump(aBuffer,aSelectableObject) ;
      aBuffer->Post();

    mySelector->Next();
  }
}

void ISession2D_InteractiveContext::Pick(const Handle(V2d_View)& aView,
                                    const Standard_Integer x1,
                                    const Standard_Integer y1) 
{

  if (myViewer->IsActive())  // A propos de la grille !!!
    {      
      aView->ShowHit(x1,y1);
    }

  mySelector->Pick(x1,y1,aView);  
 
  Handle(SelectMgr_SelectableObject) aSelectableObject; 
  mySelector->Init();
  while (mySelector->More())
  {      
    Handle(SelectMgr_EntityOwner) aOwn= mySelector->Picked();
    aSelectableObject = Handle(SelectMgr_SelectableObject)::DownCast(aOwn->Selectable());
    myPrsmgr->ColorHighlight(aSelectableObject,5,0) ;
    mySelector->Next();
  }

  if (myViewer->IsActive())  // A propos de la grille !!!
    {
      aView->ShowHit(x1,y1);
    }
}


void ISession2D_InteractiveContext::DisplayAreas() 
{
  myViewer->InitActiveViews();
  while(myViewer->MoreActiveViews())
    {
      Handle(V2d_View)  aView  = myViewer->ActiveView();
      mySelector->DisplayAreas(aView);    
      myViewer->NextActiveViews();
    }
 myViewer->Update();
}

void ISession2D_InteractiveContext::ClearAreas() 
{
   mySelector->ClearAreas();    
   myViewer->Update();
}

void ISession2D_InteractiveContext::EraseAll()
{
    TColStd_MapIteratorOfMapOfTransient anIterator(myMapOfObject);
    for (;anIterator.More();anIterator.Next())
    {
        Handle(Standard_Transient) aTransient = anIterator.Key();
        Handle(AIS_InteractiveObject) anObject = Handle(AIS_InteractiveObject)::DownCast(aTransient);

        if (!anObject.IsNull())
        {
            myPrsmgr->Erase(anObject);
            mySelectionManager
            ->Deactivate(Handle(AIS_InteractiveObject)::DownCast(anObject),mySelector);
        }
        else
        {
            Standard_CString ObjectTypeName = aTransient->DynamicType()->Name();
        }
    }
    myViewer->Update();
}
