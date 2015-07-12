// Created on: 2000-01-11
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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


#include <StepBasic_ProductDefinition.hxx>
#include <StepBasic_ProductDefinitionRelationship.hxx>
#include <STEPConstruct.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <StepRepr_PropertyDefinition.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepShape_ContextDependentShapeRepresentation.hxx>
#include <StepShape_ShapeDefinitionRepresentation.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_FinderProcess.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <Transfer_TransientProcess.hxx>
#include <TransferBRep.hxx>
#include <TransferBRep_ShapeMapper.hxx>

//=======================================================================
//function : FindEntity
//purpose  : 
//=======================================================================
Handle(StepRepr_RepresentationItem) STEPConstruct::FindEntity (const Handle(Transfer_FinderProcess) &FinderProcess,
							       const TopoDS_Shape &Shape)
{
  Handle(StepRepr_RepresentationItem) item;
  Handle(TransferBRep_ShapeMapper) mapper = TransferBRep::ShapeMapper ( FinderProcess, Shape );
  FinderProcess->FindTypedTransient (mapper,STANDARD_TYPE(StepRepr_RepresentationItem), item);
#ifdef OCCT_DEBUG
  if ( item.IsNull() ) cout << Shape.TShape()->DynamicType()->Name() << ": RepItem not found" << endl;
  else cout << Shape.TShape()->DynamicType()->Name() << ": RepItem found: " << item->DynamicType()->Name() << endl;
#endif
  return item;
}

//=======================================================================
//function : FindEntity
//purpose  : 
//=======================================================================

Handle(StepRepr_RepresentationItem) STEPConstruct::FindEntity (const Handle(Transfer_FinderProcess) &FinderProcess,
							       const TopoDS_Shape &Shape,
							       TopLoc_Location &Loc)
{
  Handle(StepRepr_RepresentationItem) item;
  Loc = Shape.Location();
  Handle(TransferBRep_ShapeMapper) mapper = TransferBRep::ShapeMapper ( FinderProcess, Shape );
  if ( ! FinderProcess->FindTypedTransient (mapper,STANDARD_TYPE(StepRepr_RepresentationItem), item) && 
       ! Loc.IsIdentity() ) {
      Loc.Identity();
      TopoDS_Shape S = Shape;
      S.Location (Loc);
      mapper = TransferBRep::ShapeMapper ( FinderProcess, S );
      FinderProcess->FindTypedTransient (mapper,STANDARD_TYPE(StepRepr_RepresentationItem), item);
  }
#ifdef OCCT_DEBUG
  if ( item.IsNull() ) cout << Shape.TShape()->DynamicType()->Name() << ": RepItem not found" << endl;
  else if ( Loc != Shape.Location() ) cout << Shape.TShape()->DynamicType()->Name() << ": RepItem found for shape without location: " << item->DynamicType()->Name() << endl;
  else cout << Shape.TShape()->DynamicType()->Name() << ": RepItem found: " << item->DynamicType()->Name() << endl;
#endif  
  return item;
}

//=======================================================================
//function : FindShape
//purpose  : 
//=======================================================================

TopoDS_Shape STEPConstruct::FindShape (const Handle(Transfer_TransientProcess) &TransientProcess,
				       const Handle(StepRepr_RepresentationItem) &item)
{
  TopoDS_Shape S;
  Handle(Transfer_Binder) binder = TransientProcess->Find(item);
  if ( ! binder.IsNull() && binder->HasResult() ) {
    S = TransferBRep::ShapeResult ( TransientProcess, binder );
  }
  return S;
}

//=======================================================================
//function : FindCDSR
//purpose  : 
//=======================================================================

Standard_Boolean STEPConstruct::FindCDSR
  (const Handle(Transfer_Binder)& ComponentBinder,
   const Handle(StepShape_ShapeDefinitionRepresentation)& AssemblySDR,
   Handle(StepShape_ContextDependentShapeRepresentation)& ComponentCDSR)
{
  Standard_Boolean result = Standard_False;

  Handle(StepRepr_PropertyDefinition) PropD = AssemblySDR->Definition().PropertyDefinition();
  if (!PropD.IsNull()) {
    Handle(StepBasic_ProductDefinition) AssemblyPD = PropD->Definition().ProductDefinition();
    if (!AssemblyPD.IsNull()) {
      Handle(Transfer_Binder) binder = ComponentBinder;
      Handle(Transfer_SimpleBinderOfTransient) trb;
      Handle(StepRepr_ProductDefinitionShape) PDS;
      Handle(StepBasic_ProductDefinitionRelationship) NAUO;
      Handle(StepBasic_ProductDefinition) ComponentPD;
      while (!binder.IsNull() && !result) {
	trb = Handle(Transfer_SimpleBinderOfTransient)::DownCast(binder);
	if (!trb.IsNull()) {
	  ComponentCDSR = Handle(StepShape_ContextDependentShapeRepresentation)::DownCast(trb->Result());
	  if (!ComponentCDSR.IsNull()) {
	    PDS = ComponentCDSR->RepresentedProductRelation();
	    if (!PDS.IsNull()) {
	      NAUO = PDS->Definition().ProductDefinitionRelationship();
	      if (!NAUO.IsNull()) {
		ComponentPD = NAUO->RelatingProductDefinition();
		result = (ComponentPD == AssemblyPD);
	      }
	    }
	  }
	}
	binder = binder->NextResult();
      }
    }
  }
  return result;
}
