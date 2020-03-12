// Created on : Sat May 02 12:41:15 2020 
// Created by: Irina KRYLOVA
// Generator:	Express (EXPRESS -> CASCADE/XSTEP Translator) V3.0
// Copyright (c) Open CASCADE 2020
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

#include <RWStepKinematics_RWKinematicJoint.hxx>

#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepKinematics_KinematicJoint.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepShape_Vertex.hxx>

//=======================================================================
//function : RWStepKinematics_RWKinematicJoint
//purpose  :
//=======================================================================
RWStepKinematics_RWKinematicJoint::RWStepKinematics_RWKinematicJoint() {}


//=======================================================================
//function : ReadStep
//purpose  :
//=======================================================================
void RWStepKinematics_RWKinematicJoint::ReadStep (const Handle(StepData_StepReaderData)& theData,
                                                  const Standard_Integer theNum,
                                                  Handle(Interface_Check)& theArch,
                                                  const Handle(StepKinematics_KinematicJoint)& theEnt) const
{
  // Check number of parameters
  if ( ! theData->CheckNbParams(theNum,3,theArch,"kinematic_joint") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  theData->ReadString (theNum, 1, "representation_item.name", theArch, aRepresentationItem_Name);

  // Inherited fields of Edge

  Handle(StepShape_Vertex) aEdge_EdgeStart;
  theData->ReadEntity (theNum, 2, "edge.edge_start", theArch, STANDARD_TYPE(StepShape_Vertex), aEdge_EdgeStart);

  Handle(StepShape_Vertex) aEdge_EdgeEnd;
  theData->ReadEntity (theNum, 3, "edge.edge_end", theArch, STANDARD_TYPE(StepShape_Vertex), aEdge_EdgeEnd);

  // Initialize entity
  theEnt->Init(aRepresentationItem_Name,
            aEdge_EdgeStart,
            aEdge_EdgeEnd);
}

//=======================================================================
//function : WriteStep
//purpose  :
//=======================================================================
void RWStepKinematics_RWKinematicJoint::WriteStep (StepData_StepWriter& theSW,
                                                   const Handle(StepKinematics_KinematicJoint)& theEnt) const
{

  // Own fields of RepresentationItem

  theSW.Send (theEnt->Name());

  // Own fields of Edge

  theSW.Send (theEnt->EdgeStart());

  theSW.Send (theEnt->EdgeEnd());
}

//=======================================================================
//function : Share
//purpose  :
//=======================================================================
void RWStepKinematics_RWKinematicJoint::Share (const Handle(StepKinematics_KinematicJoint)& theEnt,
                                               Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Inherited fields of Edge

  iter.AddItem (theEnt->StepShape_Edge::EdgeStart());

  iter.AddItem (theEnt->StepShape_Edge::EdgeEnd());
}
