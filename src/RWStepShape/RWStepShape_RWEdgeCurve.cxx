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
#include <Precision.hxx>
#include <RWStepShape_RWEdgeCurve.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_Curve.hxx>
#include <StepShape_EdgeCurve.hxx>
#include <StepShape_EdgeLoop.hxx>
#include <StepShape_FaceBound.hxx>
#include <StepShape_OrientedEdge.hxx>
#include <StepShape_Vertex.hxx>
#include <StepShape_VertexPoint.hxx>

RWStepShape_RWEdgeCurve::RWStepShape_RWEdgeCurve () {}

void RWStepShape_RWEdgeCurve::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_EdgeCurve)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,5,ach,"edge_curve")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	data->ReadString (num,1,"name",ach,aName);

	// --- inherited field : edgeStart ---

	Handle(StepShape_Vertex) aEdgeStart;
	data->ReadEntity(num, 2,"edge_start", ach, STANDARD_TYPE(StepShape_Vertex), aEdgeStart);

	// --- inherited field : edgeEnd ---

	Handle(StepShape_Vertex) aEdgeEnd;
	data->ReadEntity(num, 3,"edge_end", ach, STANDARD_TYPE(StepShape_Vertex), aEdgeEnd);

	// --- own field : edgeGeometry ---

	Handle(StepGeom_Curve) aEdgeGeometry;
	data->ReadEntity(num, 4,"edge_geometry", ach, STANDARD_TYPE(StepGeom_Curve), aEdgeGeometry);

	// --- own field : sameSense ---

	Standard_Boolean aSameSense;
	data->ReadBoolean (num,5,"same_sense",ach,aSameSense);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aEdgeStart, aEdgeEnd, aEdgeGeometry, aSameSense);
}


void RWStepShape_RWEdgeCurve::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_EdgeCurve)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- inherited field edgeStart ---

	SW.Send(ent->EdgeStart());

	// --- inherited field edgeEnd ---

	SW.Send(ent->EdgeEnd());

	// --- own field : edgeGeometry ---

	SW.Send(ent->EdgeGeometry());

	// --- own field : sameSense ---

	SW.SendBoolean(ent->SameSense());
}


void RWStepShape_RWEdgeCurve::Share(const Handle(StepShape_EdgeCurve)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->EdgeStart());


	iter.GetOneItem(ent->EdgeEnd());


	iter.GetOneItem(ent->EdgeGeometry());
}



void RWStepShape_RWEdgeCurve::Check
  (const Handle(StepShape_EdgeCurve)& ent,
   const Interface_ShareTool& aShto,
   Handle(Interface_Check)& ach) const
{
//  std::cout << "------ calling CheckEdgeCurve ------" << std::endl;
  
  Handle(StepShape_OrientedEdge) theOE1, theOE2;
  Handle(StepShape_FaceBound)    theFOB1, theFOB2;
  //Handle(StepShape_FaceSurface)  theFS1, theFS2;

  Standard_Boolean theOEOri1 = Standard_True;
  Standard_Boolean theOEOri2 = Standard_True;
  Standard_Boolean theFBOri1 = Standard_True;
  Standard_Boolean theFBOri2 = Standard_True;
  //Standard_Boolean theFSOri1 = Standard_True;
  //Standard_Boolean theFSOri2 = Standard_True;
  Standard_Boolean Cumulated1, Cumulated2;

  // 1- First Vertex != LastVertex but First VertexPoint == Last VertexPoint
  // Remark : time comsuming process but useful !
  // If this append, we can drop one of the two vertices and replace it 
  // everywhere it is referenced. Side effect : tolerance problem !!!

  Handle(StepShape_VertexPoint) StartVertex = 
    Handle(StepShape_VertexPoint)::DownCast(ent->EdgeStart());
  Handle(StepShape_VertexPoint) EndVertex   = 
    Handle(StepShape_VertexPoint)::DownCast(ent->EdgeEnd());

  if (StartVertex != EndVertex) {

    Handle(StepGeom_CartesianPoint) StartPoint = 
      Handle(StepGeom_CartesianPoint)::DownCast(StartVertex->VertexGeometry());
    Handle(StepGeom_CartesianPoint) EndPoint   = 
      Handle(StepGeom_CartesianPoint)::DownCast(EndVertex->VertexGeometry());

    // it can also be a degenerated pcurve

    if (!StartPoint.IsNull() && !EndPoint.IsNull()) {
      Standard_Real Dist = Sqrt
	((StartPoint->CoordinatesValue(1) - EndPoint->CoordinatesValue(1)) *
	 (StartPoint->CoordinatesValue(1) - EndPoint->CoordinatesValue(1)) +
	 (StartPoint->CoordinatesValue(2) - EndPoint->CoordinatesValue(2)) *
	 (StartPoint->CoordinatesValue(2) - EndPoint->CoordinatesValue(2)) +
	 (StartPoint->CoordinatesValue(3) - EndPoint->CoordinatesValue(3)) *
	 (StartPoint->CoordinatesValue(3) - EndPoint->CoordinatesValue(3)));
      if (Dist < Precision::Confusion() ) {
	ach->AddWarning("Two instances of Vertex have equal (within uncertainty) coordinates");
      }
    }
  }
  
  // 2- Two-Manifold Topology

  Standard_Boolean sharEC = aShto.IsShared(ent);
  Standard_Integer nbRef;
  if(!sharEC){
    ach->AddFail("ERROR: EdgeCurve not referenced");
  }
  else {
    Interface_EntityIterator myShRef = aShto.Sharings(ent);
    myShRef.SelectType (STANDARD_TYPE(StepShape_OrientedEdge),Standard_True);
    nbRef = myShRef.NbEntities();
    if (nbRef ==2) {
      theOE1 = Handle(StepShape_OrientedEdge)::DownCast(myShRef.Value());
      theOEOri1 = theOE1->Orientation();
      myShRef.Next();
      theOE2 = Handle(StepShape_OrientedEdge)::DownCast(myShRef.Value());
      theOEOri2 = theOE2->Orientation();
      
      // get the FaceBound orientation for theOE1
      
      Standard_Boolean sharOE1 = aShto.IsShared(theOE1);
      if(!sharOE1){
#ifdef OCCT_DEBUG
	std::cout << "OrientedEdge1 not shared" <<std::endl;
#endif
      }
      else {
	myShRef = aShto.Sharings(theOE1);
	myShRef.SelectType (STANDARD_TYPE(StepShape_EdgeLoop),Standard_True);
	nbRef = myShRef.NbEntities();
	if (nbRef == 1) {
	  myShRef.Start();
	  Handle(StepShape_EdgeLoop) theEL1 =
	    Handle(StepShape_EdgeLoop)::DownCast(myShRef.Value());
	  Standard_Boolean sharEL1 = aShto.IsShared(theEL1);
	  if(!sharEL1) {
#ifdef OCCT_DEBUG
	    std::cout << "EdgeLoop1 not shared" <<std::endl;
#endif
	  }
	  else {
	    myShRef = aShto.Sharings(theEL1);
	    myShRef.SelectType (STANDARD_TYPE(StepShape_FaceBound),Standard_True);
#ifdef OCCT_DEBUG
	    nbRef = 
#endif
	      myShRef.NbEntities();
	    myShRef.Start();
	    theFOB1 = Handle(StepShape_FaceBound)::DownCast(myShRef.Value());
	    if (!theFOB1.IsNull()) {
	      theFBOri1 = theFOB1->Orientation();
	    }
	    else {
#ifdef OCCT_DEBUG
	      std::cout << "EdgeLoop not referenced by FaceBound" << std::endl;
#endif
	    }
	  }
	}
	else {
	  if (nbRef == 0) {
#ifdef OCCT_DEBUG
	    std::cout << "OrientedEdge not referenced" << std::endl;
#endif
          }
	  else {
	    if (aShto.NbTypedSharings(theOE1,
				      STANDARD_TYPE(StepShape_EdgeLoop)) > 1) {
#ifdef OCCT_DEBUG
	      std::cout << "OrientedEdge referenced more than once" << std::endl;
#endif
            }
          }
	}
      }

      // get the FaceBound orientation for theOE2

      Standard_Boolean sharOE2 = aShto.IsShared(theOE2);
      if(!sharOE2){
#ifdef OCCT_DEBUG
	std::cout << "OrientedEdge2 not shared" <<std::endl;
#endif
      }
      else {
	myShRef = aShto.Sharings(theOE2);
#ifdef OCCT_DEBUG
// 	Standard_Integer nbRef = 
#endif
// unused	  myShRef.NbEntities();	
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
          // unused Standard_Integer nbRef = myShRef.NbEntities();	
	  myShRef.Start();
	  theFOB2 = Handle(StepShape_FaceBound)::DownCast(myShRef.Value());
	  if (!theFOB2.IsNull()) {
	    theFBOri2 = theFOB2->Orientation();
	  }
	  else {
#ifdef OCCT_DEBUG
	    std::cout << "EdgeLoop not referenced by FaceBound" << std::endl;
#endif
	  }
	}
      }
        
        // "cumulate" the FaceBound and the OrientedEdge orientation
        
        Cumulated1 = theFBOri1 ^ theOEOri1;
      Cumulated2 = theFBOri2 ^ theOEOri2;
      
      // the orientation of the OrientedEdges must be opposite
      
      if (Cumulated1 == Cumulated2) {
	ach->AddFail("ERROR: non 2-manifold topology");
      }
    }
  }
}

