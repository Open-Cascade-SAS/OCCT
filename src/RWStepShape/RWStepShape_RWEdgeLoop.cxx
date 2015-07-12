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
#include <RWStepShape_RWEdgeLoop.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepShape_EdgeLoop.hxx>
#include <StepShape_HArray1OfOrientedEdge.hxx>
#include <StepShape_OrientedEdge.hxx>

RWStepShape_RWEdgeLoop::RWStepShape_RWEdgeLoop () {}

void RWStepShape_RWEdgeLoop::ReadStep
(const Handle(StepData_StepReaderData)& data,
 const Standard_Integer num,
 Handle(Interface_Check)& ach,
 const Handle(StepShape_EdgeLoop)& ent) const
{
  
  
  // --- Number of Parameter Control ---
  
  if (!data->CheckNbParams(num,2,ach,"edge_loop")) return;
  
  // --- inherited field : name ---
  
  Handle(TCollection_HAsciiString) aName;
  //szv#4:S4163:12Mar99 `Standard_Boolean stat0 =` not needed
  data->ReadString (num,1,"name",ach,aName);
  
  // --- own field : edgeList ---
  
  Handle(StepShape_HArray1OfOrientedEdge) aEdgeList;
  Handle(StepShape_OrientedEdge) anent;
  Standard_Integer nsub1;
  if (data->ReadSubList (num,2,"edge_list",ach,nsub1)) {
    Standard_Integer nb1 = data->NbParams(nsub1);
    aEdgeList = new StepShape_HArray1OfOrientedEdge (1, nb1);
    for (Standard_Integer i1 = 1; i1 <= nb1; i1 ++) {
      //szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
      if (data->ReadEntity (nsub1, i1,"oriented_edge", ach, STANDARD_TYPE(StepShape_OrientedEdge), anent))
	aEdgeList->SetValue(i1, anent);
    }
  }
  
  //--- Initialisation of the read entity ---
  
  ent->Init(aName, aEdgeList);
}


void RWStepShape_RWEdgeLoop::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_EdgeLoop)& ent) const
{
  
  // --- inherited field name ---
  
  SW.Send(ent->Name());
  
  // --- own field : edgeList ---
  
  SW.OpenSub();
  for (Standard_Integer i1 = 1;  i1 <= ent->NbEdgeList();  i1 ++) {
    SW.Send(ent->EdgeListValue(i1));
  }
  SW.CloseSub();
}

// ============================================================================
// Method : Share
// ============================================================================

void RWStepShape_RWEdgeLoop::Share(const Handle(StepShape_EdgeLoop)& ent, Interface_EntityIterator& iter) const
{

	Standard_Integer nbElem1 = ent->NbEdgeList();
	for (Standard_Integer is1=1; is1<=nbElem1; is1 ++) {
	  iter.GetOneItem(ent->EdgeListValue(is1));
	}

}



void RWStepShape_RWEdgeLoop::Check
  (const Handle(StepShape_EdgeLoop)& ent,
   const Interface_ShareTool& ,
   Handle(Interface_Check)& ach) const
{
//  cout << "------ calling CheckEdgeLoop ------" << endl;
  Standard_Boolean headToTail = Standard_True;
  //Standard_Boolean noIdentVtx = Standard_True; //szv#4:S4163:12Mar99 unused
  Standard_Integer nbEdg = ent->NbEdgeList();
  Handle(StepShape_OrientedEdge) theOE = ent->EdgeListValue(1);
  Handle(StepShape_Vertex) theVxFrst = theOE->EdgeStart();
  Handle(StepShape_Vertex) theVxLst  = theOE->EdgeEnd();
  if((nbEdg == 1) && (theVxFrst != theVxLst)){
    ach->AddFail("Edge loop composed of single Edge : Start and End Vertex of edge are not identical");
  }
  for(Standard_Integer i = 2; i <= nbEdg; i++)
    {
      theOE     = ent->EdgeListValue(i);
      Handle(StepShape_Vertex) theVxStrt = theOE->EdgeStart();
      if(theVxStrt != theVxLst){
	headToTail = Standard_False;
      }
      theVxLst = theOE->EdgeEnd();
      if(theVxStrt == theVxLst){
	ach->AddWarning("One edge_curve contains identical vertices");
      }
    }
  if(theVxFrst != theVxLst){
    headToTail = Standard_False;
  }
  if(!headToTail) {
    ach->AddFail("Error : Path does not head to tail");
  }
}
