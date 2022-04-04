// Created on : Thu Mar 24 18:30:12 2022 
// Created by: snn
// Generator: Express (EXPRESS -> CASCADE/XSTEP Translator) V2.0
// Copyright (c) Open CASCADE 2022
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

#include <RWStepVisual_RWTessellatedVertex.hxx>
#include <StepVisual_TessellatedVertex.hxx>
#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepVisual_CoordinatesList.hxx>
#include <StepShape_VertexPoint.hxx>
#include <Standard_Integer.hxx>

//=======================================================================
//function : RWStepVisual_RWTessellatedVertex
//purpose  : 
//=======================================================================

RWStepVisual_RWTessellatedVertex::RWStepVisual_RWTessellatedVertex() {}


//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepVisual_RWTessellatedVertex::ReadStep (const Handle(StepData_StepReaderData)& theData,
                                                 const Standard_Integer theNum,
                                                 Handle(Interface_Check)& theCheck,
                                                 const Handle(StepVisual_TessellatedVertex)& theEnt) const
{
  // Check number of parameters
  if (!theData->CheckNbParams(theNum, 4, theCheck, "tessellated_vertex"))
  {
    return;
  }

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  theData->ReadString(theNum, 1, "representation_item.name", theCheck, aRepresentationItem_Name);

  // Own fields of TessellatedVertex

  Handle(StepVisual_CoordinatesList) aCoordinates;
  theData->ReadEntity(theNum, 2, "coordinates", theCheck,
    STANDARD_TYPE(StepVisual_CoordinatesList), aCoordinates);

  Handle(StepShape_VertexPoint) aTopologicalLink;
  Standard_Boolean hasTopologicalLink = Standard_True;
  if (theData->IsParamDefined(theNum, 3))
  {
    theData->ReadEntity(theNum, 3, "topological_link", theCheck,
      STANDARD_TYPE(StepShape_VertexPoint), aTopologicalLink);
  }
  else
  {
    hasTopologicalLink = Standard_False;
    aTopologicalLink.Nullify();
  }

  Standard_Integer aPointIndex;
  theData->ReadInteger(theNum, 4, "point_index", theCheck, aPointIndex);

  // Initialize entity
  theEnt->Init(aRepresentationItem_Name, aCoordinates, hasTopologicalLink, aTopologicalLink, aPointIndex);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepVisual_RWTessellatedVertex::WriteStep (StepData_StepWriter& theSW,
                                                  const Handle(StepVisual_TessellatedVertex)& theEnt) const
{

  // Own fields of RepresentationItem

  theSW.Send(theEnt->Name());

  // Own fields of TessellatedVertex

  theSW.Send(theEnt->Coordinates());

  if (theEnt->HasTopologicalLink())
  {
    theSW.Send(theEnt->TopologicalLink());
  }
  else
  {
    theSW.SendUndef();
  }

  theSW.Send(theEnt->PointIndex());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepVisual_RWTessellatedVertex::Share (const Handle(StepVisual_TessellatedVertex)&theEnt,
                                              Interface_EntityIterator& theIter) const
{

  // Inherited fields of RepresentationItem

  // Own fields of TessellatedVertex

  theIter.AddItem(theEnt->Coordinates());

  if (theEnt->HasTopologicalLink())
  {
    theIter.AddItem(theEnt->TopologicalLink());
  }
}
