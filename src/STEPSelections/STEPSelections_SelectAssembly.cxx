// Created on: 1999-03-25
// Created by: data exchange team
// Copyright (c) 1999-1999 Matra Datavision
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



#include <STEPSelections_SelectAssembly.ixx>
#include <Interface_Macros.hxx>
#include <StepShape_ContextDependentShapeRepresentation.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <StepRepr_NextAssemblyUsageOccurrence.hxx>
#include <StepRepr_MappedItem.hxx>
#include <StepShape_ShapeRepresentation.hxx>
#include <StepShape_ShapeDefinitionRepresentation.hxx>

STEPSelections_SelectAssembly::STEPSelections_SelectAssembly():IFSelect_SelectExplore (-1){ }

Standard_Boolean STEPSelections_SelectAssembly::Explore(const Standard_Integer level,
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
      
    
    
    
    
    
    
