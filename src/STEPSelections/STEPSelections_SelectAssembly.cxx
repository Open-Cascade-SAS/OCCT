// Created on: 1999-03-25
// Created by: data exchange team
// Copyright (c) 1999-1999 Matra Datavision
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


#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_Macros.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <StepRepr_MappedItem.hxx>
#include <StepRepr_NextAssemblyUsageOccurrence.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <STEPSelections_SelectAssembly.hxx>
#include <StepShape_ContextDependentShapeRepresentation.hxx>
#include <StepShape_ShapeDefinitionRepresentation.hxx>
#include <StepShape_ShapeRepresentation.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(STEPSelections_SelectAssembly,IFSelect_SelectExplore)

STEPSelections_SelectAssembly::STEPSelections_SelectAssembly():IFSelect_SelectExplore (-1){ }

Standard_Boolean STEPSelections_SelectAssembly::Explore(const Standard_Integer /*level*/,
						       const Handle(Standard_Transient)& start,
						       const Interface_Graph& G,
						       Interface_EntityIterator& explored) const
{
  if(start.IsNull()) return Standard_False;
  
  if(start->IsKind(STANDARD_TYPE(StepShape_ContextDependentShapeRepresentation))) {
    DeclareAndCast(StepShape_ContextDependentShapeRepresentation,sdsr,start);
    Handle(StepRepr_ProductDefinitionShape) pds = sdsr->RepresentedProductRelation();
    if(pds.IsNull()) return Standard_False; 
    Handle(Standard_Transient) ent = pds->Definition().ProductDefinitionRelationship();
    if(ent.IsNull()) return Standard_False;
    return (ent->IsKind(STANDARD_TYPE(StepRepr_NextAssemblyUsageOccurrence)));
  }
  
  if(start->IsKind(STANDARD_TYPE(StepRepr_MappedItem))) {
    DeclareAndCast(StepRepr_MappedItem,mapped,start);
    Interface_EntityIterator subs = G.Sharings(mapped);
    Handle(StepShape_ShapeRepresentation) shrep;
    for(subs.Start(); subs.More()&&shrep.IsNull(); subs.Next())
      if(subs.Value()->IsKind(STANDARD_TYPE(StepShape_ShapeRepresentation)))
	shrep = Handle(StepShape_ShapeRepresentation)::DownCast(subs.Value());
    if(shrep.IsNull()) return Standard_False;
    
    subs = G.Sharings(shrep);
    Handle(StepShape_ShapeDefinitionRepresentation) shdefrep;
    for(subs.Start(); subs.More()&&shdefrep.IsNull(); subs.Next())
      if(subs.Value()->IsKind(STANDARD_TYPE(StepShape_ShapeDefinitionRepresentation)))
	shdefrep = Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(subs.Value());
    if(shdefrep.IsNull()) return Standard_False;
    
    Handle(StepRepr_ProductDefinitionShape) pds = 
      Handle(StepRepr_ProductDefinitionShape)::DownCast ( shdefrep->Definition().PropertyDefinition() );
    if(pds.IsNull()) return Standard_False;
    Handle(Standard_Transient) ent = pds->Definition().ProductDefinitionRelationship();
    if(ent.IsNull()) return Standard_False;
    return (ent->IsKind(STANDARD_TYPE(StepRepr_NextAssemblyUsageOccurrence)));
  }
  
  Interface_EntityIterator subs = G.Shareds(start);
  subs.Start();
  Standard_Boolean isSome = subs.More();
  for (; subs.More(); subs.Next()) 
    explored.AddItem (subs.Value());
  
  return isSome;
}
	
TCollection_AsciiString STEPSelections_SelectAssembly::ExploreLabel() const
{
  return TCollection_AsciiString ("Assembly components");
}
      
    
    
    
    
    
    
