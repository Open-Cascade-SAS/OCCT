// Created on: 1997-01-08
// Created by: Robert COUBLANC
// Copyright (c) 1997-1999 Matra Datavision
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
void AIS_ConnectedInteractive::Compute (const Handle(PrsMgr_PresentationManager3d)& thePrsMgr,
                                        const Handle(Prs3d_Presentation)&           thePrs,
                                        const Standard_Integer                      theMode)
{
  if (!(HasLocation() || HasConnection()))
  {
    return;
  }

  if (HasConnection())
  {
    thePrs->Clear (Standard_False);
    thePrs->RemoveAll();

    if (!myReference->HasInteractiveContext())
    {
      myReference->SetContext (GetContext());
    }
    thePrsMgr->Connect (this, myReference, theMode, theMode);
    if (thePrsMgr->Presentation (myReference, theMode)->MustBeUpdated())
    {
      thePrsMgr->Update (myReference, theMode);
    }
  }

  if (HasLocation())
  {
    Handle(Geom_Transformation) aPrsTrans = new Geom_Transformation (myLocation.Transformation());
    thePrsMgr->Transform (this, aPrsTrans, theMode);
  }
  thePrs->ReCompute();
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
