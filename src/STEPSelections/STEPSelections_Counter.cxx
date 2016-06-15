// Created on: 1999-02-11
// Created by: Pavel DURANDIN
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
#include <StepBasic_ProductRelatedProductCategory.hxx>
#include <STEPConstruct_Assembly.hxx>
#include <StepGeom_CompositeCurve.hxx>
#include <StepGeom_CompositeCurveSegment.hxx>
#include <StepGeom_Curve.hxx>
#include <StepGeom_Surface.hxx>
#include <StepRepr_MappedItem.hxx>
#include <StepRepr_RepresentationMap.hxx>
#include <StepRepr_RepresentationRelationship.hxx>
#include <StepRepr_ShapeRepresentationRelationship.hxx>
#include <STEPSelections_Counter.hxx>
#include <StepShape_BrepWithVoids.hxx>
#include <StepShape_ClosedShell.hxx>
#include <StepShape_ConnectedFaceSet.hxx>
#include <StepShape_ContextDependentShapeRepresentation.hxx>
#include <StepShape_FaceSurface.hxx>
#include <StepShape_FacetedBrep.hxx>
#include <StepShape_FacetedBrepAndBrepWithVoids.hxx>
#include <StepShape_GeometricCurveSet.hxx>
#include <StepShape_GeometricSet.hxx>
#include <StepShape_GeometricSetSelect.hxx>
#include <StepShape_ManifoldSolidBrep.hxx>
#include <StepShape_OpenShell.hxx>
#include <StepShape_OrientedClosedShell.hxx>
#include <StepShape_ShapeDefinitionRepresentation.hxx>
#include <StepShape_ShapeRepresentation.hxx>
#include <StepShape_Shell.hxx>
#include <StepShape_ShellBasedSurfaceModel.hxx>

STEPSelections_Counter::STEPSelections_Counter()
{
  myNbFaces = 0;
  myNbShells = 0;
  myNbSolids = 0;
  myNbWires = 0;
  myNbEdges =0;
}

void STEPSelections_Counter::Count(const Interface_Graph& graph,
				   const Handle(Standard_Transient)& start)
{
  if(start.IsNull()) return;
  
  if (start->IsKind(STANDARD_TYPE(StepBasic_ProductRelatedProductCategory))) return;
  
  if (start->IsKind(STANDARD_TYPE(StepShape_ShapeDefinitionRepresentation))) {
    DeclareAndCast(StepShape_ShapeDefinitionRepresentation,sdr,start);
    Count(graph,sdr->UsedRepresentation());
    Interface_EntityIterator subs = graph.Shareds(start);
    for (subs.Start(); subs.More(); subs.Next()) {
      DeclareAndCast(StepShape_ContextDependentShapeRepresentation,anitem,subs.Value());
      if (anitem.IsNull()) continue;
      Count(graph,anitem);
    }
    return;
  }
  
  if (start->IsKind(STANDARD_TYPE(StepShape_ShapeRepresentation))) {
    DeclareAndCast(StepShape_ShapeRepresentation,sr,start);
    Standard_Integer nb = sr->NbItems();
    for (Standard_Integer i = 1; i <= nb; i++) {
      Handle(StepRepr_RepresentationItem) anitem = sr->ItemsValue(i);
      Count(graph,anitem);
    }
    return;
  }
    
  if (start->IsKind(STANDARD_TYPE(StepShape_FacetedBrep))) {
    DeclareAndCast(StepShape_FacetedBrep,fbr,start);
    myMapOfSolids.Add(start);
    myNbSolids++;
    AddShell(fbr->Outer());
    return;
  }
  
  if (start->IsKind(STANDARD_TYPE(StepShape_BrepWithVoids))) {
    DeclareAndCast(StepShape_BrepWithVoids,brwv,start);
    myMapOfSolids.Add(start);
    myNbSolids++;
    AddShell(brwv->Outer());
    Standard_Integer nbvoids = brwv->NbVoids();
    for(Standard_Integer i = 1; i <= nbvoids; i++)
      AddShell(brwv->VoidsValue(i));
    return;
  }
  
  if (start->IsKind(STANDARD_TYPE(StepShape_ManifoldSolidBrep))) {
    DeclareAndCast(StepShape_ManifoldSolidBrep,msbr,start);
    myMapOfSolids.Add(start);
    myNbSolids++;
    AddShell(msbr->Outer());
    return;
  }
  
  if (start->IsKind(STANDARD_TYPE(StepShape_ShellBasedSurfaceModel))) {
    DeclareAndCast(StepShape_ShellBasedSurfaceModel,sbsm,start);
    Standard_Integer nbItems = sbsm->NbSbsmBoundary();
    for(Standard_Integer i = 1; i <= nbItems; i++) {
      Handle(StepShape_OpenShell) osh = sbsm->SbsmBoundaryValue(i).OpenShell();
      if(!osh.IsNull()) AddShell(osh);
      Handle(StepShape_ClosedShell) csh = sbsm->SbsmBoundaryValue(i).ClosedShell();
      if(!csh.IsNull()) AddShell(csh);
    }
    return;  
  }
  
  if (start->IsKind(STANDARD_TYPE(StepShape_FacetedBrepAndBrepWithVoids))) {
    DeclareAndCast(StepShape_FacetedBrepAndBrepWithVoids,fbwv,start);
    myMapOfSolids.Add(start);
    myNbSolids++;
    AddShell(fbwv->Outer());
    Standard_Integer nbvoids = fbwv->NbVoids();
    for(Standard_Integer i = 1; i <= nbvoids; i++)
      AddShell(fbwv->VoidsValue(i));
    return;
  }
  
  if (start->IsKind(STANDARD_TYPE(StepShape_GeometricSet))) {
    DeclareAndCast(StepShape_GeometricSet,gs,start);
    Standard_Integer nbElem = gs->NbElements();
    for (Standard_Integer i = 1; i <= nbElem ; i++) {
      StepShape_GeometricSetSelect aGSS = gs->ElementsValue(i);
      Handle(Standard_Transient) ent = aGSS.Value();
      Handle(StepGeom_CompositeCurve) ccurve = Handle(StepGeom_CompositeCurve)::DownCast(ent);
      if(!ccurve.IsNull()) {
	myNbWires++;
	myMapOfWires.Add(ccurve);
	AddCompositeCurve(ccurve);
      } else 
	if(ent->IsKind(STANDARD_TYPE(StepGeom_Curve))) {
	  myNbEdges++;
	  myMapOfEdges.Add(ent);
	} else
	  if(ent->IsKind(STANDARD_TYPE(StepGeom_Surface))) {
	    myNbFaces++;
	    myMapOfFaces.Add(ent);
	  }
    } 
  }

  
  if (start->IsKind(STANDARD_TYPE(StepRepr_MappedItem))) {
    DeclareAndCast(StepRepr_MappedItem,mi,start);
    Count(graph,mi->MappingTarget());
    Handle(StepRepr_RepresentationMap) map =  mi->MappingSource();
    if(map.IsNull()) return;
    Count(graph,map->MappedRepresentation());
    return;
  }
  
  if (start->IsKind(STANDARD_TYPE(StepShape_FaceSurface))) {
    myNbFaces++;
    myMapOfFaces.Add(start);
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
	Count(graph,SDR);
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
      Count(graph,anitem);
    } 
    return;
  } 
}

void STEPSelections_Counter::Clear()
{
  myMapOfFaces.Clear();
  myMapOfShells.Clear();
  myMapOfSolids.Clear();
  myMapOfWires.Clear();
  myMapOfEdges.Clear();
  myNbFaces = 0;
  myNbShells = 0;
  myNbSolids = 0;
  myNbWires = 0;
  myNbEdges =0;
}

void STEPSelections_Counter::AddShell(const Handle(StepShape_ConnectedFaceSet)& cfs)
{
  myMapOfShells.Add(cfs);
  myNbShells++;
  Standard_Integer nbf = cfs->NbCfsFaces();
  for(Standard_Integer i =1; i <= nbf; i++)
    myMapOfFaces.Add(cfs->CfsFacesValue(i));
  myNbFaces+=nbf;
  return;
}

void STEPSelections_Counter::AddCompositeCurve(const Handle(StepGeom_CompositeCurve)& ccurve)
{
  Standard_Integer nbs = ccurve->NbSegments();
  for ( Standard_Integer i=1; i <= nbs; i++ ) {
//  #ifdef AIX   CKY : common code for all platforms: Handle() not Handle()&
    Handle(StepGeom_CompositeCurveSegment) ccs = ccurve->SegmentsValue ( i );
    Handle(StepGeom_Curve) crv = ccs->ParentCurve();

    if(crv->IsKind(STANDARD_TYPE(StepGeom_CompositeCurve)))
      AddCompositeCurve(Handle(StepGeom_CompositeCurve)::DownCast(crv));
    else {
      myNbEdges++;
      myMapOfEdges.Add(crv);
    }
  }
}
