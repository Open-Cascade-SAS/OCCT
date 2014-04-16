// Created on: 1997-04-22
// Created by: Guest Design
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

//		<g_design>


#include <Standard_NotImplemented.hxx>

#include <AIS_MultipleConnectedInteractive.ixx>

#include <PrsMgr_ModedPresentation.hxx>
#include <PrsMgr_Presentation3d.hxx>

static Standard_Boolean IsInSeq (const AIS_SequenceOfInteractive&      theSeq,
				 const Handle(AIS_InteractiveObject)&  theItem) 
{
  Standard_Integer I = theSeq.Length();
  while ( I>0 && theSeq.Value(I) != theItem) {    
    I--;
  }
  return (I>0);
}

static Standard_Integer RangeInSeq (const AIS_SequenceOfInteractive& theSeq ,
				    const Handle(AIS_InteractiveObject)&     theItem) 
{
  Standard_Integer I = theSeq.Length();
  while ( I>0 && theSeq.Value(I) != theItem) {    
    I--;
  }
  return I;
}


//=======================================================================
//function : AIS_MultipleConnectedInteractive
//purpose  : 
//=======================================================================

AIS_MultipleConnectedInteractive::AIS_MultipleConnectedInteractive
(const PrsMgr_TypeOfPresentation3d aTypeOfPresentation3d):
AIS_InteractiveObject(aTypeOfPresentation3d)
{    
  SetHilightMode(0);
}

//=======================================================================
//function : Type
//purpose  : 
//=======================================================================
AIS_KindOfInteractive AIS_MultipleConnectedInteractive::Type() const
{return AIS_KOI_Object;}

//=======================================================================
//function : Signature
//purpose  : 
//=======================================================================
Standard_Integer AIS_MultipleConnectedInteractive::Signature() const
{return 1;}

//=======================================================================
//function : Connect
//purpose  : 
//=======================================================================
void AIS_MultipleConnectedInteractive::Connect(const Handle(AIS_InteractiveObject)& anotherIObj)
{

  if (!IsInSeq (myReferences, anotherIObj)) {
    myReferences.Append(anotherIObj);
  }
}

//=======================================================================
//function : HasConnection
//purpose  : 
//=======================================================================
Standard_Boolean AIS_MultipleConnectedInteractive::HasConnection() const 
{
  return (myReferences.Length() != 0);
}

//=======================================================================
//function : Disconnect
//purpose  : 
//=======================================================================

void AIS_MultipleConnectedInteractive::Disconnect(const Handle(AIS_InteractiveObject)& anotherIObj)
{
  Standard_Integer I = RangeInSeq (myReferences, anotherIObj);
  if (I != 0) {
    myReferences.Remove(I);
    
  }
}

//=======================================================================
//function : DisconnectAll
//purpose  : 
//=======================================================================

void AIS_MultipleConnectedInteractive::DisconnectAll ()
{
/*  for(Standard_Integer i =1;i<=myPresentations.Length();i++)
    {
      Handle(PrsMgr_Presentation3d) P = Handle(PrsMgr_Presentation3d)::DownCast(myPresentations(i).Presentation());
      if(!P.IsNull()) {
	P->Presentation()->DisconnectAll(Graphic3d_TOC_DESCENDANT);
      }
    }*/
  myPreviousReferences = myReferences; // pour garder les poignees au chaud!!!!
  myReferences.Clear();
}

//=======================================================================
//function : Compute
//purpose  :
//=======================================================================
void AIS_MultipleConnectedInteractive::Compute (const Handle(PrsMgr_PresentationManager3d)& thePrsMgr,
                                                const Handle(Prs3d_Presentation)&           thePrs,
                                                const Standard_Integer                      theMode)
{
  thePrs->Clear (Standard_False);
  thePrs->RemoveAll();
  if (HasConnection())
  {
    for (Standard_Integer aRefIter = 1; aRefIter <= myReferences.Length(); ++aRefIter)
    {
      const Handle (AIS_InteractiveObject)& aRef = myReferences.Value (aRefIter);
      if (!aRef->HasInteractiveContext())
      {
        aRef->SetContext (GetContext());
      }

      thePrsMgr->Connect (this, aRef, theMode, theMode);
      if (thePrsMgr->Presentation (aRef, theMode)->MustBeUpdated())
      {
        thePrsMgr->Update (aRef, theMode);
      }
    }
  }

  thePrs->ReCompute();
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void AIS_MultipleConnectedInteractive::Compute(const Handle_Prs3d_Projector& aProjector,
                                               const Handle_Prs3d_Presentation& aPresentation)
{
// Standard_NotImplemented::Raise("AIS_MultipleConnectedInteractive::Compute(const Handle_Prs3d_Projector&, const Handle_Prs3d_Presentation&)");
 PrsMgr_PresentableObject::Compute( aProjector , aPresentation ) ;
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void AIS_MultipleConnectedInteractive::Compute(const Handle_Prs3d_Projector& aProjector,
                                               const Handle_Geom_Transformation& aTransformation,
                                               const Handle_Prs3d_Presentation& aPresentation)
{
// Standard_NotImplemented::Raise("AIS_MultipleConnectedInteractive::Compute(const Handle_Prs3d_Projector&, const Handle_Geom_Transformation&, const Handle_Prs3d_Presentation&)");
 PrsMgr_PresentableObject::Compute( aProjector , aTransformation , aPresentation ) ;
}

//=======================================================================
//function : ComputeSelection
//purpose  : 
//=======================================================================
void AIS_MultipleConnectedInteractive::ComputeSelection(const Handle(SelectMgr_Selection)& /*aSel*/,
                                                        const Standard_Integer /*aMode*/)
{
}
