#include "stdafx.h"

#include "ISession2D_ObjectOwner.h"
#include <PrsMgr_PresentationManager2d.hxx>

IMPLEMENT_STANDARD_HANDLE(ISession2D_ObjectOwner,SelectMgr_EntityOwner)
IMPLEMENT_STANDARD_RTTIEXT(ISession2D_ObjectOwner,SelectMgr_EntityOwner)

ISession2D_ObjectOwner::ISession2D_ObjectOwner(const Standard_Integer aPriority) //= 0
:SelectMgr_EntityOwner(aPriority)
{}
ISession2D_ObjectOwner::ISession2D_ObjectOwner(const Handle(SelectMgr_SelectableObject)& aSO,
                       const Standard_Integer aPriority ) //= 0
:SelectMgr_EntityOwner(aSO,aPriority)
{}


// Presentation Management
void ISession2D_ObjectOwner::Hilight(const Handle(PrsMgr_PresentationManager)& aPM,
                                     const Standard_Integer aMode) // = 0
{
  ASSERT(aPM->IsKind(STANDARD_TYPE(PrsMgr_PresentationManager2d)));
  if (myGo.IsNull())
  {
    Handle(Graphic2d_View) aView = Handle(PrsMgr_PresentationManager2d)::DownCast(aPM)->StructureManager();
    myGo = new Graphic2d_GraphicObject(aView); 
  }

  Handle(Graphic2d_Text) text;
  text = new Graphic2d_Text(myGo, myDumpMessage, 0, 0, 0,Aspect_TOT_SOLID,1.5);
  text->SetZoomable(Standard_False);
  myGo->Display();
}

void ISession2D_ObjectOwner::Unhilight(const Handle(PrsMgr_PresentationManager)& aPM,
                                       const Standard_Integer aMode) // = 0
{
  if (myGo.IsNull()) return;
  myGo->RemovePrimitives();
  myGo->Display();
  myGo->Remove();
}

