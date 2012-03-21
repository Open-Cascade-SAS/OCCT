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


#include <RWStepShape_RWFaceBound.ixx>
#include <StepShape_Loop.hxx>
#include <StepShape_EdgeLoop.hxx>
#include <StepShape_OrientedEdge.hxx>
#include <StepShape_Edge.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepShape_FaceBound.hxx>


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
#ifdef DEB
	  cout << "OrientedEdge2 not shared" <<endl;
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
#ifdef DEB
	    cout << "EdgeLoop2 not shared" <<endl;
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
#ifdef DEB
	      cout << "EdgeLoop not referenced by FaceBound" << endl;
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
#ifdef DEB
    cout << "no EdgeLoop in FaceBound" << endl;
#endif
  }
}
