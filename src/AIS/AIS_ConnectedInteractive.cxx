// Created on: 1997-01-08
// Created by: Robert COUBLANC
// Copyright (c) 1997-1999 Matra Datavision
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



#include <Standard_NotImplemented.hxx>

#include <AIS_ConnectedInteractive.ixx>
#include <SelectMgr_EntityOwner.hxx>
#include <Select3D_SensitiveEntity.hxx>
#include <Geom_Transformation.hxx>

#include <PrsMgr_ModedPresentation.hxx>
#include <PrsMgr_Presentation3d.hxx>
#include <OSD_Timer.hxx>


//=======================================================================
//function : AIS_ConnectedInteractive
//purpose  : 
//=======================================================================
AIS_ConnectedInteractive::AIS_ConnectedInteractive(const PrsMgr_TypeOfPresentation3d aTypeOfPresentation3d):
AIS_InteractiveObject(aTypeOfPresentation3d)
{    
  SetHilightMode(0);
}

//=======================================================================
//function : Type
//purpose  : 
//=======================================================================
AIS_KindOfInteractive AIS_ConnectedInteractive::Type() const
{return AIS_KOI_Object;}

Standard_Integer AIS_ConnectedInteractive::Signature() const
{return 0; }




//=======================================================================
//function : Connect
//purpose  : 
//=======================================================================
void AIS_ConnectedInteractive::
Connect(const Handle(AIS_InteractiveObject)& anotherIObj)
{
  // To have the time to Disconnect below, 
  // the old is kept for a while. 
  if(myReference==anotherIObj) return;
  myOldReference = myReference;
//Disconnect();
  myReference = anotherIObj ;
}

//=======================================================================
//function : Connect
//purpose  : 
//=======================================================================
void AIS_ConnectedInteractive::
Connect(const Handle(AIS_InteractiveObject)& anotherIobj, 
	const TopLoc_Location& aLocation)
{
  if(myLocation!=aLocation)
    myLocation = aLocation;
  if(myReference!=anotherIobj) {
    myOldReference = myReference; // necessary to disconnect below..
//  Disconnect();
    myReference = anotherIobj;}
  
}


//=======================================================================
//function : Disconnect
//purpose  : 
//=======================================================================

void AIS_ConnectedInteractive::Disconnect()
{
  for(Standard_Integer i =1;i<=myPresentations.Length();i++)
    {
      Handle(PrsMgr_Presentation3d) P = Handle(PrsMgr_Presentation3d)::DownCast(myPresentations(i).Presentation());
      if(!P.IsNull()) {
	P->Presentation()->DisconnectAll(Graphic3d_TOC_DESCENDANT);
      }
    }
}
//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void AIS_ConnectedInteractive::
Compute(const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
	const Handle(Prs3d_Presentation)& aPresentation, 
	const Standard_Integer aMode)
{
  static Handle(Geom_Transformation) myPrsTrans ;

  if(!(HasLocation() ||HasConnection())) return;
  
  if(HasConnection()){
    aPresentation->Clear(Standard_False);
    aPresentation->RemoveAll();
    aPresentationManager->Connect( this,myReference, aMode, aMode);
    if(aPresentationManager->Presentation(myReference,aMode)->MustBeUpdated())
      aPresentationManager->Update(myReference,aMode);
    
  }
  if(HasLocation()){
    myPrsTrans = new Geom_Transformation(myLocation.Transformation());
    aPresentationManager->Transform(this, myPrsTrans, aMode);  }
  aPresentation->ReCompute();
}

void AIS_ConnectedInteractive::Compute(const Handle_Prs3d_Projector& aProjector, const Handle_Geom_Transformation& aTransformation, const Handle_Prs3d_Presentation& aPresentation)
{
// Standard_NotImplemented::Raise("AIS_ConnectedInteractive::Compute(const Handle_Prs3d_Projector&, const Handle_Geom_Transformation&, const Handle_Prs3d_Presentation&)");
  PrsMgr_PresentableObject::Compute( aProjector , aTransformation , aPresentation ) ;
}

void AIS_ConnectedInteractive::Compute(const Handle_Prs3d_Projector& aProjector, const Handle_Prs3d_Presentation& aPresentation)
{
// Standard_NotImplemented::Raise("AIS_ConnectedInteractive::Compute(const Handle_Prs3d_Projector&, const Handle_Prs3d_Presentation&)");
 PrsMgr_PresentableObject::Compute( aProjector , aPresentation ) ;
}

void AIS_ConnectedInteractive::Compute(const Handle_PrsMgr_PresentationManager2d& aPresentationManager2d, const Handle_Graphic2d_GraphicObject& aGraphicObject, const int anInteger)
{
// Standard_NotImplemented::Raise("AIS_ConnectedInteractive::Compute(const Handle_PrsMgr_PresentationManager2d&, const Handle_Graphic2d_GraphicObject&, const int)");
 PrsMgr_PresentableObject::Compute( aPresentationManager2d ,aGraphicObject,anInteger) ;
}

//=======================================================================
//function : ComputeSelection
//purpose  : 
//=======================================================================

void AIS_ConnectedInteractive::ComputeSelection(const Handle(SelectMgr_Selection)& aSel, 
                                                const Standard_Integer aMode)
{
  if(!(HasLocation() ||HasConnection())) return;
  
  aSel->Clear();
  if(!myReference->HasSelection(aMode))
    myReference->UpdateSelection(aMode);

  const Handle(SelectMgr_Selection)& TheRefSel = myReference->Selection(aMode);
  Handle(SelectMgr_EntityOwner) OWN = new SelectMgr_EntityOwner(this);
  Handle(Select3D_SensitiveEntity) SE3D, SNew;
  
  if(TheRefSel->IsEmpty())
    myReference->UpdateSelection(aMode);
  for(TheRefSel->Init();TheRefSel->More();TheRefSel->Next())
  {
    SE3D = Handle(Select3D_SensitiveEntity)::DownCast(TheRefSel->Sensitive());
    if(!SE3D.IsNull())
    {
      // Get the copy of SE3D
      SNew = SE3D->GetConnected(myLocation);
      if(aMode==0)
      {
        SNew->Set(OWN);
        // In case if SE3D caches some location-dependent data
        // that must be updated after setting OWN
        SNew->SetLocation(myLocation);
      }
      aSel->Add(SNew);
    }
  }
}

void AIS_ConnectedInteractive::UpdateLocation()
{
// Standard_NotImplemented::Raise("AIS_ConnectedInteractive::UpdateLocation()");
 SelectMgr_SelectableObject::UpdateLocation() ;
}
void AIS_ConnectedInteractive::UpdateLocation(const Handle(SelectMgr_Selection)& Sel)
{
// Standard_NotImplemented::Raise("AIS_ConnectedInteractive::UpdateLocation(const Handle(SelectMgr_Selection)& Sel)");
 SelectMgr_SelectableObject::UpdateLocation(Sel) ;
}
/*void AIS_ConnectedInteractive::UpdateLocation(const Handle_Prs3d_Presentation& aPresentation)
{
// Standard_NotImplemented::Raise("AIS_ConnectedInteractive::UpdateLocation(const Handle_Prs3d_Presentation&)");
 SelectMgr_SelectableObject::UpdateLocation(aPresentation) ;
}*/
