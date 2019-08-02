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


#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_ShareTool.hxx>
#include <RWStepShape_RWFaceBound.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepShape_Edge.hxx>
#include <StepShape_EdgeLoop.hxx>
#include <StepShape_FaceBound.hxx>
#include <StepShape_Loop.hxx>
#include <StepShape_OrientedEdge.hxx>

RWStepShape_RWFaceBound::RWStepShape_RWFaceBound () {}

void RWStepShape_RWFaceBound::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_FaceBound)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"face_bound")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : bound ---

	Handle(StepShape_Loop) aBound;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"bound", ach, STANDARD_TYPE(StepShape_Loop), aBound);

	// --- own field : orientation ---

	Standard_Boolean aOrientation;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadBoolean (num,3,"orientation",ach,aOrientation);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aBound, aOrientation);
}


void RWStepShape_RWFaceBound::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_FaceBound)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- own field : bound ---

	SW.Send(ent->Bound());

	// --- own field : orientation ---

	SW.SendBoolean(ent->Orientation());
}


void RWStepShape_RWFaceBound::Share(const Handle(StepShape_FaceBound)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Bound());
}



void RWStepShape_RWFaceBound::Check
  (const Handle(StepShape_FaceBound)& ent,
   const Interface_ShareTool& aShto,
   Handle(Interface_Check)& ach) const
{
  Standard_Boolean theFBOri2 = Standard_True;
  Standard_Boolean theFBOri1 = ent->Orientation();
  Handle(StepShape_EdgeLoop) theEL1 =
    Handle(StepShape_EdgeLoop)::DownCast(ent->Bound());
  if (!theEL1.IsNull()) {
    Standard_Integer nbEdg = theEL1->NbEdgeList();
    for(Standard_Integer i=1; i<=nbEdg; i++) {
      Handle(StepShape_OrientedEdge) theOE1 = theEL1->EdgeListValue(i);
      Handle(StepShape_Edge) theEdg1 = theOE1->EdgeElement();
      Interface_EntityIterator myShRef = aShto.Sharings(theEdg1);
      myShRef.SelectType (STANDARD_TYPE(StepShape_OrientedEdge),Standard_True);
      Standard_Integer nbRef = myShRef.NbEntities();
      if(nbRef == 1) {
	//      ach.AddWarning("EdgeCurve only once referenced");
      }
      else if (nbRef ==2) {
	Handle(StepShape_OrientedEdge) theOE2;
	Handle(StepShape_OrientedEdge) refOE1 = 
	  Handle(StepShape_OrientedEdge)::DownCast(myShRef.Value());
	myShRef.Next();
	Handle(StepShape_OrientedEdge) refOE2 = 
	  Handle(StepShape_OrientedEdge)::DownCast(myShRef.Value());
	if(theOE1 == refOE1) theOE2 = refOE2;
	else if(theOE1 == refOE2) theOE2 = refOE1;
	
	// get the FaceBound orientation for theOE2
	
	Standard_Boolean sharOE2 = aShto.IsShared(theOE2);
	if(!sharOE2){
#ifdef OCCT_DEBUG
	  std::cout << "OrientedEdge2 not shared" <<std::endl;
#endif
	}
	else {
	  myShRef = aShto.Sharings(theOE2);
	  myShRef.SelectType (STANDARD_TYPE(StepShape_EdgeLoop),Standard_True);
	  myShRef.Start();
	  Handle(StepShape_EdgeLoop) theEL2 =
	    Handle(StepShape_EdgeLoop)::DownCast(myShRef.Value());
	  Standard_Boolean sharEL2 = aShto.IsShared(theEL2);
	  if(!sharEL2){
#ifdef OCCT_DEBUG
	    std::cout << "EdgeLoop2 not shared" <<std::endl;
#endif
	  }
	  else {
	    myShRef = aShto.Sharings(theEL2);
	    myShRef.SelectType (STANDARD_TYPE(StepShape_FaceBound),Standard_True);
	    myShRef.Start();
	    Handle(StepShape_FaceBound) theFB2 =
	      Handle(StepShape_FaceBound)::DownCast(myShRef.Value());
	    if (!theFB2.IsNull()) {
	      theFBOri2 = theFB2->Orientation();
	    }
	    else {
#ifdef OCCT_DEBUG
	      std::cout << "EdgeLoop not referenced by FaceBound" << std::endl;
#endif
	    }
	  }
	}
	
	// "cumulate" the FaceBound orientation with the OrientedEdge orientation
	
	Standard_Boolean theOEOri1 =
	  theFBOri1 ? theOE1->Orientation() : !(theOE1->Orientation());
	Standard_Boolean theOEOri2 =
	  theFBOri2 ? theOE2->Orientation() : !(theOE2->Orientation());
	
	// the orientation of the OrientedEdges must be opposite
	
	if(theOEOri1 == theOEOri2) {
	  ach->AddFail("ERROR: non 2-manifold topology");
	} 
      } //end if(nbRef == 2)
    } //end for(i=1; i<=nbEdg; ...)
  } //end if(!theEL1.IsNull)
  else {
#ifdef OCCT_DEBUG
    std::cout << "no EdgeLoop in FaceBound" << std::endl;
#endif
  }
}
