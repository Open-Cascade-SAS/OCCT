// File:	STEPSelections_SelectInstances.cxx
// Created:	Tue Mar 23 15:14:34 1999
// Author:	data exchange team
//		<det@friendox.nnov.matra-dtv.fr>


#include <STEPSelections_SelectInstances.ixx>
#include <Interface_Macros.hxx>
#include <StepShape_ShapeDefinitionRepresentation.hxx>
#include <StepShape_ContextDependentShapeRepresentation.hxx>
#include <StepShape_ShapeRepresentation.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepShape_FacetedBrep.hxx>
#include <StepShape_BrepWithVoids.hxx>
#include <StepShape_ManifoldSolidBrep.hxx>
#include <StepShape_ShellBasedSurfaceModel.hxx>
#include <StepShape_FacetedBrepAndBrepWithVoids.hxx>
#include <StepShape_GeometricSet.hxx>
#include <StepShape_FaceSurface.hxx>
#include <StepRepr_MappedItem.hxx>
#include <STEPConstruct_Assembly.hxx>
#include <StepRepr_ShapeRepresentationRelationship.hxx>
#include <TColStd_IndexedMapOfTransient.hxx>
#include <Interface_HGraph.hxx>

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
    DeclareAndCast(StepRepr_RepresentationRelationship,SRR,CDSR->RepresentationRelation());
    if ( SRR.IsNull() ) return ;
    
    Handle(StepRepr_Representation) rep;
    Standard_Boolean SRRReversed = STEPConstruct_Assembly::CheckSRRReversesNAUO ( graph.Model(), CDSR );
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


Standard_Boolean STEPSelections_SelectInstances::Explore(const Standard_Integer level,
							const Handle(Standard_Transient)& start,
							const Interface_Graph& G,
							Interface_EntityIterator& explored) const
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
