// Created on: 2000-01-11
// Created by: Andrey BETENEV
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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


#include <STEPConstruct.ixx>
#include <TransferBRep.hxx>
#include <TransferBRep_ShapeMapper.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>

#include <StepBasic_ProductDefinition.hxx>
#include <StepBasic_ProductDefinitionRelationship.hxx>
#include <StepRepr_PropertyDefinition.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>

#ifdef DEBUG
void DumpBinder (const Handle(Transfer_Binder) &binder)
{
  Handle(Transfer_Binder) bbb = binder;
  while ( ! bbb.IsNull() ) {
    Handle(Transfer_SimpleBinderOfTransient) bx = 
      Handle(Transfer_SimpleBinderOfTransient)::DownCast ( bbb );
    if ( ! bx.IsNull() ) {
      cout << "--> " << bx->ResultTypeName() << " " << *(void**)&bx->Result() << endl;
    }
    else cout << "--> ???" << endl;
    bbb = bbb->NextResult();
  }
  cout << endl;
}
#endif

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
#ifdef DEB
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
#ifdef DEB
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
