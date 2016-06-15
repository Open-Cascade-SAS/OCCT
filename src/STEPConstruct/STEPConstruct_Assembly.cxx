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

//:k8 abv 06.01.99: TR10: writing unique names for NAUOs
// :j4 16.03.99 gka S4134
// abv 18.11.99 renamed from StepPDR_MakeItem

#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Standard_Transient.hxx>
#include <StepBasic_ApplicationContext.hxx>
#include <StepBasic_DesignContext.hxx>
#include <StepBasic_HArray1OfProductContext.hxx>
#include <StepBasic_MechanicalContext.hxx>
#include <StepBasic_Product.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <StepBasic_ProductDefinitionFormationWithSpecifiedSource.hxx>
#include <STEPConstruct_Assembly.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepRepr_CharacterizedDefinition.hxx>
#include <StepRepr_HArray1OfRepresentationItem.hxx>
#include <StepRepr_ItemDefinedTransformation.hxx>
#include <StepRepr_NextAssemblyUsageOccurrence.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <StepRepr_RepresentationContext.hxx>
#include <StepRepr_ShapeRepresentationRelationshipWithTransformation.hxx>
#include <StepRepr_Transformation.hxx>
#include <StepShape_ContextDependentShapeRepresentation.hxx>
#include <StepShape_ShapeDefinitionRepresentation.hxx>
#include <StepShape_ShapeRepresentation.hxx>
#include <TCollection_HAsciiString.hxx>

//  ProductDefinition (pour Relationship)
//  ContextDependentShapeRepresentation qui contient la Relationship
//  Relationship
//=======================================================================
//function : STEPConstruct_Assembly
//purpose  : 
//=======================================================================
STEPConstruct_Assembly::STEPConstruct_Assembly ()
{  
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void STEPConstruct_Assembly::Init (const Handle(StepShape_ShapeDefinitionRepresentation)& aSDR,
				   const Handle(StepShape_ShapeDefinitionRepresentation)& SDR0,
				   const Handle(StepGeom_Axis2Placement3d)& Ax0,
				   const Handle(StepGeom_Axis2Placement3d)& AxLoc)
{
  thesdr = aSDR;
  thesdr0 = SDR0;
  thesr  = Handle(StepShape_ShapeRepresentation)::DownCast(aSDR->UsedRepresentation());
  thesr0 = Handle(StepShape_ShapeRepresentation)::DownCast(SDR0->UsedRepresentation());
  theval.Nullify();
  theax0 = Ax0;
  theloc = AxLoc;
}

//=======================================================================
//function : MakeMappedItem
//purpose  : 
//=======================================================================

//void STEPConstruct_Assembly::MakeMappedItem ()
//{
  //  not yet implemented
//}

//=======================================================================
//function : MakeRelationship
//purpose  : 
//=======================================================================

void STEPConstruct_Assembly::MakeRelationship ()
{
  // get PDs for assembly (sdr0) and component (sdr)
  Handle(StepBasic_ProductDefinition) PDED  =
    thesdr->Definition().PropertyDefinition()->Definition().ProductDefinition();
  Handle(StepBasic_ProductDefinition) PDING =
    thesdr0->Definition().PropertyDefinition()->Definition().ProductDefinition();

  // create NAUO
  //:k8 abv 06 Jan 99: TR10: writing unique names for NAUOs  !!!!!
  Handle(StepRepr_NextAssemblyUsageOccurrence) NAUO =
    new StepRepr_NextAssemblyUsageOccurrence;
  static Standard_Integer id = 0;
  Handle(TCollection_HAsciiString) ocid   = new TCollection_HAsciiString(++id);
  Handle(TCollection_HAsciiString) ocname = new TCollection_HAsciiString("");
  Handle(TCollection_HAsciiString) ocdesc = new TCollection_HAsciiString("");
  Handle(TCollection_HAsciiString) refdes;  // reste nulle
  NAUO->Init (ocid,ocname,Standard_True,ocdesc,PDING,PDED,Standard_False,refdes);

  // create PDS for link CDSR->PDS->NAUO
  Handle(StepRepr_ProductDefinitionShape) PDS =
    new StepRepr_ProductDefinitionShape;
  Handle(TCollection_HAsciiString) pdsname = new TCollection_HAsciiString("Placement");
  Handle(TCollection_HAsciiString) pdsdesc = new TCollection_HAsciiString("Placement of an item");
  StepRepr_CharacterizedDefinition CD;
  CD.SetValue(NAUO);
  PDS->Init (pdsname,Standard_True,pdsdesc,CD);

  // create transformation
  Handle(StepRepr_ItemDefinedTransformation) ItemDef =
    new StepRepr_ItemDefinedTransformation;
  Handle(TCollection_HAsciiString) idname = new TCollection_HAsciiString("");
  Handle(TCollection_HAsciiString) idescr = new TCollection_HAsciiString("");
  ItemDef->Init (idname,idescr,theax0,theloc);

  // create SRRWT
  Handle(StepRepr_ShapeRepresentationRelationshipWithTransformation) SRRWT =
    new StepRepr_ShapeRepresentationRelationshipWithTransformation;
  Handle(TCollection_HAsciiString) stname = new TCollection_HAsciiString("");
  Handle(TCollection_HAsciiString) stescr = new TCollection_HAsciiString("");
  StepRepr_Transformation StepTrans;
  StepTrans.SetValue (ItemDef);
  SRRWT->Init (stname,stescr,thesr,thesr0,StepTrans);

  // create CDSR (final result, root)
  Handle(StepShape_ContextDependentShapeRepresentation) CDSR =
    new StepShape_ContextDependentShapeRepresentation;
  CDSR->Init (SRRWT,PDS);

  theval = CDSR;
}

//=======================================================================
//function : ItemValue
//purpose  : 
//=======================================================================

Handle(Standard_Transient)  STEPConstruct_Assembly::ItemValue () const
{
  if (theval.IsNull()) return Handle(Standard_Transient) (thesr);
  return theval;
}

//=======================================================================
//function : ItemLocation
//purpose  : 
//=======================================================================

Handle(StepGeom_Axis2Placement3d)  STEPConstruct_Assembly::ItemLocation () const
{
  return theloc;
}

//=======================================================================
//function : GetNAUO
//purpose  : 
//=======================================================================

Handle(StepRepr_NextAssemblyUsageOccurrence) STEPConstruct_Assembly::GetNAUO () const
{
  Handle(StepShape_ContextDependentShapeRepresentation) CDSR = 
    Handle(StepShape_ContextDependentShapeRepresentation)::DownCast ( ItemValue() );
  if ( ! CDSR.IsNull() ) {
    Handle(StepBasic_ProductDefinitionRelationship) PDR = 
      CDSR->RepresentedProductRelation()->Definition().ProductDefinitionRelationship();
    return Handle(StepRepr_NextAssemblyUsageOccurrence)::DownCast ( PDR );
  }
  return 0;
}

//=======================================================================
//function : CheckSRRReversesNAUO
//purpose  : 
//=======================================================================

Standard_Boolean STEPConstruct_Assembly::CheckSRRReversesNAUO(const Interface_Graph& theGraph,
							      const Handle(StepShape_ContextDependentShapeRepresentation) &CDSR)
{
  Handle(StepRepr_NextAssemblyUsageOccurrence) nauo = 
    Handle(StepRepr_NextAssemblyUsageOccurrence)::DownCast 
	  ( CDSR->RepresentedProductRelation()->Definition().ProductDefinitionRelationship() );
  if ( nauo.IsNull() ) {
#ifdef OCCT_DEBUG
    cout << "Warning: No NAUO found in CDSR !" << endl;
#endif
    return Standard_False;
  }

  Handle(StepBasic_ProductDefinition) pd1, pd2;
  Handle(StepRepr_Representation) rep1 = CDSR->RepresentationRelation()->Rep1();
  Handle(StepRepr_Representation) rep2 = CDSR->RepresentationRelation()->Rep2();
  
  // find SDRs corresponding to Rep1 and Rep2 and remember their PDs
  Handle(Standard_Type) tSDR = STANDARD_TYPE(StepShape_ShapeDefinitionRepresentation);
  Interface_EntityIterator anIter = theGraph.Sharings(rep1);
  for (; anIter.More() && pd1.IsNull(); anIter.Next()) {
      Handle(Standard_Transient) enti = anIter.Value();
    if (enti->DynamicType() == tSDR) {
      Handle(StepShape_ShapeDefinitionRepresentation) SDR =
        Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(enti);
      if ( SDR->UsedRepresentation() == rep1 ) 
        pd1 = SDR->Definition().PropertyDefinition()->Definition().ProductDefinition();
    }
  }
  
  anIter = theGraph.Sharings(rep2);
  for (; anIter.More() && pd2.IsNull(); anIter.Next()) {
      Handle(Standard_Transient) enti = anIter.Value();
    if (enti->DynamicType() == tSDR) {
      Handle(StepShape_ShapeDefinitionRepresentation) SDR =
        Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(enti);
      if ( SDR->UsedRepresentation() == rep2 ) 
        pd2 = SDR->Definition().PropertyDefinition()->Definition().ProductDefinition();
    }
  }

  // checks..
  
  if ( pd1 == nauo->RelatedProductDefinition() && // OK
       pd2 == nauo->RelatingProductDefinition() ) return Standard_False;

  if ( pd2 == nauo->RelatedProductDefinition() && // Reversed
       pd1 == nauo->RelatingProductDefinition() ) {
    return Standard_True;
  }

#ifdef OCCT_DEBUG
  cout << "Warning: SRR and NAUO are incompatible" << endl;
//  cout << "NAUO = " << Model->StringLabel(nauo)->ToCString() << 
//       ",\tCDSR = " << Model->StringLabel(CDSR)->ToCString() << endl;
//  cout << "Rep1 = " << Model->StringLabel(rep1)->ToCString() << 
//       ",\tRep2 = " << Model->StringLabel(rep2)->ToCString() << endl;
//  cout << "PD1  = " << Model->StringLabel(pd1)->ToCString() << 
//       ",\tPD2  = " << Model->StringLabel(pd2)->ToCString() << endl;
//  cout << "Rel1 = " << Model->StringLabel(nauo->RelatingProductDefinition())->ToCString() << 
//       ",\tRel2 = " << Model->StringLabel(nauo->RelatedProductDefinition())->ToCString() << endl;
#endif

  if ( pd2 == nauo->RelatedProductDefinition() || //:k3 abv 25 Nov 98: rp1sd.stp - bad assemblies
       pd1 == nauo->RelatingProductDefinition() ) {
    return Standard_True;
  }

  return Standard_False;
}
