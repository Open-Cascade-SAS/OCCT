// Created on: 1999-03-23
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
#include <Interface_HGraph.hxx>
#include <Interface_Macros.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <STEPConstruct_Assembly.hxx>
#include <StepRepr_MappedItem.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepRepr_ShapeRepresentationRelationship.hxx>
#include <STEPSelections_SelectInstances.hxx>
#include <StepShape_BrepWithVoids.hxx>
#include <StepShape_ContextDependentShapeRepresentation.hxx>
#include <StepShape_FaceSurface.hxx>
#include <StepShape_FacetedBrep.hxx>
#include <StepShape_FacetedBrepAndBrepWithVoids.hxx>
#include <StepShape_GeometricSet.hxx>
#include <StepShape_ManifoldSolidBrep.hxx>
#include <StepShape_ShapeDefinitionRepresentation.hxx>
#include <StepShape_ShapeRepresentation.hxx>
#include <StepShape_ShellBasedSurfaceModel.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_IndexedMapOfTransient.hxx>

IMPLEMENT_STANDARD_RTTIEXT(STEPSelections_SelectInstances,IFSelect_SelectExplore)

static Handle(Interface_HGraph) myGraph;
static Interface_EntityIterator myEntities;

STEPSelections_SelectInstances::STEPSelections_SelectInstances():IFSelect_SelectExplore (-1){ }

static void AddAllSharings(const Handle(Standard_Transient)& start,
			    const Interface_Graph& graph,
			    Interface_EntityIterator& explored) 
{
  if(start.IsNull()) return;
  Interface_EntityIterator subs = graph.Shareds(start);
  for (subs.Start(); subs.More(); subs.Next()) {
    explored.AddItem(subs.Value()); 
    AddAllSharings(subs.Value(), graph, explored);
  }
}
    

static void AddInstances(const Handle(Standard_Transient)& start,
			 const Interface_Graph& graph,
			 Interface_EntityIterator& explored)
{
  if(start.IsNull()) return;
  
  explored.AddItem(start);
  if (start->IsKind(STANDARD_TYPE(StepShape_ShapeDefinitionRepresentation))) {
    DeclareAndCast(StepShape_ShapeDefinitionRepresentation,sdr,start);
    AddInstances(sdr->UsedRepresentation(),graph,explored);
    Interface_EntityIterator subs = graph.Shareds(start);
    for (subs.Start(); subs.More(); subs.Next()) {
      DeclareAndCast(StepShape_ContextDependentShapeRepresentation,anitem,subs.Value());
      if (anitem.IsNull()) continue;
      AddInstances(anitem,graph,explored);
    }
    return;
  }
  
  if (start->IsKind(STANDARD_TYPE(StepShape_ShapeRepresentation))) {
    DeclareAndCast(StepShape_ShapeRepresentation,sr,start);
    Standard_Integer nb = sr->NbItems();
    for (Standard_Integer i = 1; i <= nb; i++) {
      Handle(StepRepr_RepresentationItem) anitem = sr->ItemsValue(i);
      AddInstances(anitem,graph,explored);
    }
    return;
  }
  
  if (start->IsKind(STANDARD_TYPE(StepShape_FacetedBrep))||
      start->IsKind(STANDARD_TYPE(StepShape_BrepWithVoids))||
      start->IsKind(STANDARD_TYPE(StepShape_ManifoldSolidBrep))||
      start->IsKind(STANDARD_TYPE(StepShape_ShellBasedSurfaceModel))||
      start->IsKind(STANDARD_TYPE(StepShape_FacetedBrepAndBrepWithVoids))||
      start->IsKind(STANDARD_TYPE(StepShape_GeometricSet))||
      start->IsKind(STANDARD_TYPE(StepShape_FaceSurface))||
      start->IsKind(STANDARD_TYPE(StepRepr_MappedItem))) {
    AddAllSharings(start, graph, explored);
    return;
  }

  if (start->IsKind(STANDARD_TYPE(StepShape_ContextDependentShapeRepresentation))) {
    DeclareAndCast(StepShape_ContextDependentShapeRepresentation,CDSR,start);
    Handle(StepRepr_RepresentationRelationship) SRR = CDSR->RepresentationRelation();
    if ( SRR.IsNull() ) return ;
    
    Handle(StepRepr_Representation) rep;
    Standard_Boolean SRRReversed = STEPConstruct_Assembly::CheckSRRReversesNAUO ( graph, CDSR );
    if(SRRReversed)
      rep = SRR->Rep2();
    else
      rep = SRR->Rep1();
    
    Interface_EntityIterator subs = graph.Sharings(rep);
    for (subs.Start(); subs.More(); subs.Next()) 
      if ( subs.Value()->IsKind(STANDARD_TYPE(StepShape_ShapeDefinitionRepresentation))) {
	DeclareAndCast(StepShape_ShapeDefinitionRepresentation,SDR,subs.Value());
	AddInstances(SDR,graph,explored);
      }
    //???
    return;
  } 
  
  if (start->IsKind (STANDARD_TYPE(StepRepr_ShapeRepresentationRelationship)) ) {
    DeclareAndCast(StepRepr_ShapeRepresentationRelationship,und,start);
    for (Standard_Integer i = 1; i <= 2; i ++) {
      Handle(Standard_Transient) anitem;
      if (i == 1) anitem = und->Rep1();
      if (i == 2) anitem = und->Rep2();
      AddInstances(anitem,graph,explored);
    } 
    return;
  }
  
}
     
     
Interface_EntityIterator STEPSelections_SelectInstances::RootResult(const Interface_Graph &G) const
{
  if(myGraph.IsNull()||(G.Model()!=myGraph->Graph().Model()))
    {
      
      Interface_EntityIterator roots = G.RootEntities();
      myGraph = new Interface_HGraph(G);
      myEntities.Destroy();
      for (roots.Start(); roots.More(); roots.Next())
	AddInstances(roots.Value(), G, myEntities);
    }

  if(HasInput()||HasAlternate()) {
    Interface_EntityIterator select = InputResult(G);
    Standard_Integer nbSelected = select.NbEntities();
    TColStd_IndexedMapOfTransient filter (nbSelected);
    for(select.Start(); select.More(); select.Next())
      filter.Add(select.Value());
    Interface_EntityIterator result;
    for(myEntities.Start(); myEntities.More(); myEntities.Next()) 
      if(filter.Contains(myEntities.Value()))
	result.AddItem(myEntities.Value());
    return result;
  }
  else
    return myEntities;
}


Standard_Boolean STEPSelections_SelectInstances::Explore(const Standard_Integer,
							const Handle(Standard_Transient)&,
							const Interface_Graph&,
							Interface_EntityIterator&) const
{
  return Standard_False;
}

Standard_Boolean STEPSelections_SelectInstances::HasUniqueResult() const
{
  return Standard_True;
}

TCollection_AsciiString STEPSelections_SelectInstances::ExploreLabel() const
{
  return TCollection_AsciiString ("Instances");
}
