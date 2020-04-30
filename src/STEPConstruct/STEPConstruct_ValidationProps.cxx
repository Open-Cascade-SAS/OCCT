// Created on: 1999-09-09
// Created by: Andrey BETENEV
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


#include <APIHeaderSection_MakeHeader.hxx>
#include <gp_Pnt.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_Static.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <StepBasic_DerivedUnit.hxx>
#include <StepBasic_DerivedUnitElement.hxx>
#include <StepBasic_HArray1OfDerivedUnitElement.hxx>
#include <StepBasic_MeasureValueMember.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <StepBasic_SiUnitAndAreaUnit.hxx>
#include <StepBasic_SiUnitAndLengthUnit.hxx>
#include <StepBasic_Unit.hxx>
#include <STEPConstruct.hxx>
#include <STEPConstruct_UnitContext.hxx>
#include <STEPConstruct_ValidationProps.hxx>
#include <StepData_StepModel.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext.hxx>
#include <StepGeom_GeometricRepresentationItem.hxx>
#include <StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx.hxx>
#include <StepRepr_CharacterizedDefinition.hxx>
#include <StepRepr_GlobalUnitAssignedContext.hxx>
#include <StepRepr_HArray1OfRepresentationItem.hxx>
#include <StepRepr_MeasureRepresentationItem.hxx>
#include <StepRepr_NextAssemblyUsageOccurrence.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <StepRepr_PropertyDefinition.hxx>
#include <StepRepr_PropertyDefinitionRepresentation.hxx>
#include <StepRepr_RepresentationContext.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepRepr_RepresentationRelationship.hxx>
#include <StepRepr_ShapeAspect.hxx>
#include <StepRepr_ShapeRepresentationRelationship.hxx>
#include <StepShape_ContextDependentShapeRepresentation.hxx>
#include <StepShape_ShapeDefinitionRepresentation.hxx>
#include <StepShape_ShapeRepresentation.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <TransferBRep.hxx>
#include <TransferBRep_ShapeMapper.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_TransferWriter.hxx>
#include <XSControl_WorkSession.hxx>

//=======================================================================
//function : STEPConstruct_ValidationProps
//purpose  : 
//=======================================================================
STEPConstruct_ValidationProps::STEPConstruct_ValidationProps () 
{
}
     
//=======================================================================
//function : STEPConstruct_ValidationProps
//purpose  : 
//=======================================================================

STEPConstruct_ValidationProps::STEPConstruct_ValidationProps (const Handle(XSControl_WorkSession) &WS)
     : STEPConstruct_Tool(WS)
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

Standard_Boolean STEPConstruct_ValidationProps::Init (const Handle(XSControl_WorkSession) &WS)
{
  return SetWS ( WS );
}

//=======================================================================
//function : TransientResult   CORRECTED
//purpose  : 
//=======================================================================

static Handle(Transfer_SimpleBinderOfTransient) TransientResult (const Handle(Standard_Transient)& res)
{
  Handle(Transfer_SimpleBinderOfTransient) binder;
  if (res.IsNull()) return binder;
  binder = new Transfer_SimpleBinderOfTransient;
  binder->SetResult (res);
  return binder;
}

//=======================================================================
//function : FindTarget
//purpose  : 
//=======================================================================

Standard_Boolean STEPConstruct_ValidationProps::FindTarget (const TopoDS_Shape &Shape,
							    StepRepr_CharacterizedDefinition &target,
							    Handle(StepRepr_RepresentationContext) &Context,
							    const Standard_Boolean instance)
{
  // find the target STEP entity corresponding to a shape
  Handle(TransferBRep_ShapeMapper) mapper = TransferBRep::ShapeMapper ( FinderProcess(), Shape );
  Handle(Transfer_Binder) binder = FinderProcess()->Find ( mapper );
  
  // if requested, try to find instance of assembly
  if ( instance ) {
/*
    Handle(StepRepr_NextAssemblyUsageOccurrence) NAUO;
    Standard_Boolean found = myAssemblyPD.IsNull()?
      FinderProcess()->FindTypedTransient (mapper,STANDARD_TYPE(StepRepr_NextAssemblyUsageOccurrence), NAUO) :
	 STEPConstruct::FindNAUO (binder,myAssemblyPD,NAUO);
    if ( found ) {
      //skl find CDSR using NAUO:
      Handle(StepShape_ContextDependentShapeRepresentation) CDSR
      Interface_EntityIterator subs1 = graph.Sharings(NAUO);
      for (subs1.Start(); subs1.More(); subs1.Next()) {
        Handle(StepRepr_ProductDefinitionShape) PDS = 
          Handle(StepRepr_ProductDefinitionShape)::DownCast(subs1.Value());
        if(PDS.IsNull()) continue;
        //IsPDS=Standard_True;
        Interface_EntityIterator subs2 = graph.Sharings(PDS);
        for (subs2.Start(); CDSR.IsNull() && subs2.More(); subs2.Next()) {
          CDSR = Handle(StepShape_ContextDependentShapeRepresentation)::DownCast(subs2.Value());
        }
      }
      if(!CDSR.IsNull()) {
        target.SetValue ( CDSR->RepresentedProductRelation() );
        Context = CDSR->RepresentationRelation()->Rep2()->ContextOfItems();
      }
#ifdef OCCT_DEBUG
      else std::cout << "INSTANCE: CDRS from NAUO NOT found" << std::endl;
#endif
    }
#ifdef OCCT_DEBUG
    else std::cout << "INSTANCE: NAUO NOT found" << std::endl;
#endif
*/
  }

  // for Compounds, search for SDR
  else if ( Shape.ShapeType() == TopAbs_COMPOUND ) {
    Handle(StepBasic_ProductDefinition) ProdDef;
    if ( FinderProcess()->FindTypedTransient (mapper,STANDARD_TYPE(StepBasic_ProductDefinition), ProdDef) ) {
      Handle(StepRepr_ProductDefinitionShape) PDS;
      Interface_EntityIterator subs1 = Graph().Sharings(ProdDef);
      for (subs1.Start(); PDS.IsNull() && subs1.More(); subs1.Next()) {
        PDS = Handle(StepRepr_ProductDefinitionShape)::DownCast(subs1.Value());
      }
      target.SetValue ( PDS );
#ifdef OCCT_DEBUG
//      std::cout << "COMPOUND: SDR found: " << sdr->DynamicType()->Name() << std::endl;
#endif
    }
    else {
#ifdef OCCT_DEBUG
      std::cout << "COMPOUND: ProdDef NOT found" << std::endl;
#endif
      Handle(StepShape_ShapeRepresentation) SR;
      if(FinderProcess()->FindTypedTransient(mapper,STANDARD_TYPE(StepShape_ShapeRepresentation),SR)) {
        Handle(StepRepr_ProductDefinitionShape) PDS;
        Interface_EntityIterator subs1 = Graph().Sharings(SR);
        for (subs1.Start(); PDS.IsNull() && subs1.More(); subs1.Next()) {
          Handle(StepShape_ShapeDefinitionRepresentation) SDR =
            Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(subs1.Value());
          if(SDR.IsNull()) continue;
          PDS = Handle(StepRepr_ProductDefinitionShape)::DownCast(SDR->Definition().PropertyDefinition());
        }
        if(PDS.IsNull()) {
          subs1 = Graph().Sharings(SR);
          for (subs1.Start(); PDS.IsNull() && subs1.More(); subs1.Next()) {
            Handle(StepRepr_RepresentationRelationship) RR =
              Handle(StepRepr_RepresentationRelationship)::DownCast(subs1.Value());
            if(RR.IsNull()) continue;
            Handle(StepShape_ShapeRepresentation) SR1;
            if(RR->Rep1()==SR)
              SR1 = Handle(StepShape_ShapeRepresentation)::DownCast(RR->Rep2());
            else SR1 = Handle(StepShape_ShapeRepresentation)::DownCast(RR->Rep1());
            if(!SR1.IsNull()) {
              Interface_EntityIterator subs2 = Graph().Sharings(SR1);
              for (subs2.Start(); PDS.IsNull() && subs2.More(); subs2.Next()) {
                Handle(StepShape_ShapeDefinitionRepresentation) SDR =
                  Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(subs2.Value());
                if(SDR.IsNull()) continue;
                PDS = Handle(StepRepr_ProductDefinitionShape)::DownCast(SDR->Definition().PropertyDefinition());
              }
            }
          }
        }
        if(!PDS.IsNull()) {
          target.SetValue(PDS);
          Context = SR->ContextOfItems();
        }
      }
    }
  }

  // for others, search for GEOMETRIC_REPRESENTATION_ITEM
  else {
    Handle(StepGeom_GeometricRepresentationItem) item;
    if ( FinderProcess()->FindTypedTransient (mapper,STANDARD_TYPE(StepGeom_GeometricRepresentationItem), item) ) {
#ifdef OCCT_DEBUG
//      std::cout << Shape.TShape()->DynamicType()->Name() << ": GeomRepItem found: " << item->DynamicType()->Name() << std::endl;
#endif
      // find PDS (GRI <- SR <- SDR -> PDS)
      Handle(StepRepr_ProductDefinitionShape) PDS;
      Interface_EntityIterator subs = Graph().Sharings(item);
      for (subs.Start(); PDS.IsNull() && subs.More(); subs.Next()) {
#ifdef OCCT_DEBUG
//	std::cout << "Parsing back refs: found " << subs.Value()->DynamicType()->Name() << std::endl;
#endif
        if ( ! subs.Value()->IsKind(STANDARD_TYPE(StepShape_ShapeRepresentation)) ) continue;
        Handle(StepShape_ShapeRepresentation) sr = 
          Handle(StepShape_ShapeRepresentation)::DownCast ( subs.Value() );
        Context = sr->ContextOfItems();
        Interface_EntityIterator sub2 = Graph().Sharings(subs.Value());
	for (sub2.Start(); sub2.More(); sub2.Next()) {
          if ( ! sub2.Value()->IsKind(STANDARD_TYPE(StepShape_ShapeDefinitionRepresentation)) ) continue;
          Handle(StepShape_ShapeDefinitionRepresentation) sdr = 
            Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(sub2.Value());
          PDS = Handle(StepRepr_ProductDefinitionShape)::DownCast ( sdr->Definition().PropertyDefinition() );
        }
      }
      if ( ! PDS.IsNull() ) {
        // find SHAPE_ASPECT or create it with all associated info if not yet exists
        Handle(StepRepr_ShapeAspect) aspect;
        Handle(Transfer_Binder) bbb = binder;
        while ( ! bbb.IsNull() ) {
          Handle(Transfer_SimpleBinderOfTransient) bx = 
            Handle(Transfer_SimpleBinderOfTransient)::DownCast ( bbb );
          if ( ! bx.IsNull() ) {
            Handle(StepRepr_ShapeAspect) asp = 
              Handle(StepRepr_ShapeAspect)::DownCast ( bx->Result() );
            if ( ! asp.IsNull() && asp->OfShape() == PDS ) {
              aspect = asp;
              break;
            }
	  }
          bbb = bbb->NextResult();
        }
        if ( aspect.IsNull() ) {
//	if ( ! FinderProcess()->FindTypedTransient (mapper,STANDARD_TYPE(StepRepr_ShapeAspect), aspect ) ||
//	     aspect->OfShape() != PDS )
#ifdef OCCT_DEBUG
          std::cout << Shape.TShape()->DynamicType()->Name() << ": SHAPE_ASPECT NOT found, creating" << std::endl;
#endif
	  // create aspect and all related data
          Handle(TCollection_HAsciiString) AspectName = new TCollection_HAsciiString ( "" );
          Handle(TCollection_HAsciiString) AspectDescr = new TCollection_HAsciiString ( "" );
          aspect = new StepRepr_ShapeAspect;
          aspect->Init ( AspectName, AspectDescr, PDS, StepData_LFalse );
			
          StepRepr_CharacterizedDefinition SA;
          SA.SetValue ( aspect );
	  
          Handle(TCollection_HAsciiString) PropDefName = 
            new TCollection_HAsciiString ( "shape with specific properties" );
          Handle(TCollection_HAsciiString) PropDefDescr = new TCollection_HAsciiString ( "properties for subshape" );
          Handle(StepRepr_PropertyDefinition) propdef = new StepRepr_PropertyDefinition;
          propdef->Init ( PropDefName, Standard_True, PropDefDescr, SA );
	  	  
          Handle(TCollection_HAsciiString) SRName = new TCollection_HAsciiString ( "" );
          Handle(StepShape_ShapeRepresentation) SR = new StepShape_ShapeRepresentation;
          Handle(StepRepr_HArray1OfRepresentationItem) SRItems = new StepRepr_HArray1OfRepresentationItem ( 1, 1 );
          SRItems->SetValue ( 1, item );
          SR->Init ( SRName, SRItems, Context );
	  
          Handle(StepShape_ShapeDefinitionRepresentation) SDR = new StepShape_ShapeDefinitionRepresentation;
          StepRepr_RepresentedDefinition RD;
          RD.SetValue ( propdef );
          SDR->Init ( RD, SR );
	  
	  // record SHAPE_ASPECT in the map
          binder->AddResult ( TransientResult ( aspect ) );

	  // add SDR and all the data into model
          Model()->AddWithRefs ( SDR );
        }
	// SHAPE_ASPECT found, but we also need context: FIND IT !!!!
        else { 
#ifdef OCCT_DEBUG
          std::cout << Shape.TShape()->DynamicType()->Name() << ": SHAPE_ASPECT found" << std::endl;
#endif
          Handle(StepRepr_ProductDefinitionShape) aPDS = aspect->OfShape();
          Interface_EntityIterator asubs = Graph().Sharings(aPDS);
          for (asubs.Start(); Context.IsNull() && asubs.More(); asubs.Next()) {
            if ( ! asubs.Value()->IsKind(STANDARD_TYPE(StepShape_ShapeDefinitionRepresentation)) ) continue;
            Handle(StepShape_ShapeDefinitionRepresentation) sdr = 
              Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(asubs.Value());
            Context = sdr->UsedRepresentation()->ContextOfItems();
          }
        }
  
        if ( ! aspect.IsNull() ) target.SetValue ( aspect );
      }
#ifdef OCCT_DEBUG
      else  std::cout << Shape.TShape()->DynamicType()->Name() << ": PDS NOT found, fail to create SHAPE_ASPECT" << std::endl;
#endif
    }
#ifdef OCCT_DEBUG
    else std::cout << Shape.TShape()->DynamicType()->Name() << ": GeomRepItem NOT found" << std::endl;
#endif
  }

  
  // if target not found and shape has location, try the same shape without location
  return ! target.IsNull();
}

//=======================================================================
//function : AddProp   CORRECTED
//purpose  : 
//=======================================================================

Standard_Boolean STEPConstruct_ValidationProps::AddProp (const StepRepr_CharacterizedDefinition &target,
							 const Handle(StepRepr_RepresentationContext) &Context,
							 const Handle(StepRepr_RepresentationItem) &Prop,
							 const Standard_CString Descr)
{
  // FINALLY, create a structure of 5 entities describing a link between a shape and its property
  Handle(TCollection_HAsciiString) PropDefName = 
    new TCollection_HAsciiString ( "geometric validation property" );
  Handle(TCollection_HAsciiString) PropDefDescr = new TCollection_HAsciiString ( Descr );
  Handle(StepRepr_PropertyDefinition) propdef = new StepRepr_PropertyDefinition;
  propdef->Init ( PropDefName, Standard_True, PropDefDescr, target );
	  	  
  Handle(TCollection_HAsciiString) SRName = new TCollection_HAsciiString ( Descr );
  Handle(StepRepr_Representation) rep = new StepRepr_Representation;
  Handle(StepRepr_HArray1OfRepresentationItem) SRItems = new StepRepr_HArray1OfRepresentationItem ( 1, 1 );
  SRItems->SetValue ( 1, Prop );
  rep->Init ( SRName, SRItems, Context );
	  
  Handle(StepRepr_PropertyDefinitionRepresentation) PrDR = new StepRepr_PropertyDefinitionRepresentation;
  StepRepr_RepresentedDefinition RD;
  RD.SetValue ( propdef );
  PrDR->Init ( RD, rep );
	  
  // record SDR in order to have it written to the file
  Model()->AddWithRefs ( PrDR );

  // for AP203, add subschema name
  if ( Interface_Static::IVal("write.step.schema") ==3 ) {
    APIHeaderSection_MakeHeader mkHdr ( Handle(StepData_StepModel)::DownCast ( Model() ) );
    Handle(TCollection_HAsciiString) subSchema = 
      new TCollection_HAsciiString ( "GEOMETRIC_VALIDATION_PROPERTIES_MIM" );
    mkHdr.AddSchemaIdentifier ( subSchema );
  }
  
  return Standard_True;
}

//=======================================================================
//function : AddProp   CORRECTED
//purpose  : 
//=======================================================================

Standard_Boolean STEPConstruct_ValidationProps::AddProp (const TopoDS_Shape &Shape,
							 const Handle(StepRepr_RepresentationItem) &Prop,
							 const Standard_CString Descr,
							 const Standard_Boolean instance)
{
  StepRepr_CharacterizedDefinition target;
  Handle(StepRepr_RepresentationContext) Context;
  if ( ! FindTarget ( Shape, target, Context, instance ) ) return Standard_False;
  return AddProp ( target, Context, Prop, Descr );
}

//=======================================================================
//function : AddVolume   CORRECTED
//purpose  : 
//=======================================================================

Standard_Boolean STEPConstruct_ValidationProps::AddVolume (const TopoDS_Shape &Shape, 
							   const Standard_Real Vol)
{
  Handle(StepBasic_MeasureValueMember) Val = new StepBasic_MeasureValueMember;
  Val->SetReal ( Vol );
  //Val->SetName ( "solid volume" );
  Val->SetName ( "VOLUME_MEASURE");

  // for volume unit, either take existing or create a new
  if ( volUnit.DerivedUnit().IsNull() ) {
    Handle(StepBasic_SiUnitAndLengthUnit) MM = new StepBasic_SiUnitAndLengthUnit;
    MM->Init ( Standard_True, StepBasic_spMilli, StepBasic_sunMetre );
  
    Handle(StepBasic_DerivedUnitElement) DUE = new StepBasic_DerivedUnitElement;
    DUE->Init ( MM, 3. );
  
    Handle(StepBasic_HArray1OfDerivedUnitElement) DUElems = 
      new StepBasic_HArray1OfDerivedUnitElement ( 1, 1 );
    DUElems->SetValue ( 1, DUE );

    Handle(StepBasic_DerivedUnit) DU = new StepBasic_DerivedUnit;
    DU->Init ( DUElems );

    volUnit.SetValue ( DU );
  }
  
  Handle(TCollection_HAsciiString) MRIName = new TCollection_HAsciiString ( "volume measure" );
  Handle(StepRepr_MeasureRepresentationItem) MRI = new StepRepr_MeasureRepresentationItem;
  MRI->Init ( MRIName, Val, volUnit );
  
  return AddProp ( Shape, MRI, "volume" );
}

//=======================================================================
//function : AddArea   CORRECTED
//purpose  : 
//=======================================================================

Standard_Boolean STEPConstruct_ValidationProps::AddArea (const TopoDS_Shape &Shape, 
							 const Standard_Real Area)
{
  Handle(StepBasic_MeasureValueMember) Val = new StepBasic_MeasureValueMember;
  Val->SetReal ( Area );
  //Val->SetName ( "surface area" );
  Val->SetName ( "AREA_MEASURE" );

  // for area unit, either take existing or create a new
  if ( areaUnit.DerivedUnit().IsNull() ) {
    Handle(StepBasic_SiUnitAndLengthUnit) MM = new StepBasic_SiUnitAndLengthUnit;
    MM->Init ( Standard_True, StepBasic_spMilli, StepBasic_sunMetre );
  
    Handle(StepBasic_DerivedUnitElement) DUE = new StepBasic_DerivedUnitElement;
    DUE->Init ( MM, 2. );
  
    Handle(StepBasic_HArray1OfDerivedUnitElement) DUElems = 
      new StepBasic_HArray1OfDerivedUnitElement ( 1, 1 );
    DUElems->SetValue ( 1, DUE );

    Handle(StepBasic_DerivedUnit) DU = new StepBasic_DerivedUnit;
    DU->Init ( DUElems );

    areaUnit.SetValue ( DU );
  }
  
  Handle(TCollection_HAsciiString) MRIName = new TCollection_HAsciiString ( "surface area measure" );
  Handle(StepRepr_MeasureRepresentationItem) MRI = new StepRepr_MeasureRepresentationItem;
  MRI->Init ( MRIName, Val, areaUnit );
  
  return AddProp ( Shape, MRI, "surface area" );
}

//=======================================================================
//function : AddCentroid   CORRECTED
//purpose  : 
//=======================================================================

Standard_Boolean STEPConstruct_ValidationProps::AddCentroid (const TopoDS_Shape &Shape, 
							     const gp_Pnt &Pnt,
							     const Standard_Boolean instance)
{
  Handle(TCollection_HAsciiString) CPName = new TCollection_HAsciiString ( "centre point" );
  Handle(StepGeom_CartesianPoint) CP = new StepGeom_CartesianPoint;
  CP->Init3D ( CPName, Pnt.X(), Pnt.Y(), Pnt.Z() );
  
  return AddProp ( Shape, CP, "centroid", instance );
}

//=======================================================================
//function : LoadProps   CORRECTED
//purpose  : 
//=======================================================================

Standard_Boolean STEPConstruct_ValidationProps::LoadProps (TColStd_SequenceOfTransient &seq) const
{
  // parse on PropertyDefinitionRepresentations
  Standard_Integer nb = Model()->NbEntities();
  Handle(Standard_Type) tPDR = STANDARD_TYPE(StepRepr_PropertyDefinitionRepresentation);
  for (Standard_Integer i = 1; i <= nb; i ++) {
    Handle(Standard_Transient) enti = Model()->Value(i);
    if ( ! enti->IsKind (tPDR) ) continue;
    
    Handle(StepRepr_PropertyDefinitionRepresentation) PDR = 
      Handle(StepRepr_PropertyDefinitionRepresentation)::DownCast ( enti );
    
    // Check that PDR is for validation props.
    Handle(StepRepr_PropertyDefinition) PD = PDR->Definition().PropertyDefinition();
    if (!PD.IsNull() && !PD->Name().IsNull())
    {
      // Note: according to "Recommended Practices for Geometric and Assembly Validation Properties" Release 4.4
      // as of Augist 17, 2016, item 4.6, the name of PropertyDefinition should be "geometric validation property"
      // with words separated by spaces; however older versions of the same RP document used underscores.
      // To be able to read files written using older convention, we convert all underscores to spaces for this check.
      TCollection_AsciiString aName = PD->Name()->String();
      aName.ChangeAll('_', ' ', Standard_False);
      aName.LowerCase();
      if (aName != "geometric validation property")
        continue;
    }

    seq.Append ( PDR );
  }
  return seq.Length() >0;
}
    
//=======================================================================
//function : GetPropPD   CORRECTED
//purpose  : 
//=======================================================================

Handle(StepBasic_ProductDefinition) STEPConstruct_ValidationProps::GetPropPD (const Handle(StepRepr_PropertyDefinition) &PD) const
{
  StepRepr_CharacterizedDefinition CD = PD->Definition();
    
  // detect target entity of valprop
  Handle(StepBasic_ProductDefinition) ProdDef;
  Handle(StepRepr_PropertyDefinition) PDS = CD.ProductDefinitionShape();
  if ( PDS.IsNull() ) {
    Handle(StepRepr_ShapeAspect) SA = CD.ShapeAspect();
    if ( SA.IsNull() ) {
#ifdef OCCT_DEBUG
      Message_Messenger::StreamBuffer sout = Message::SendInfo();
      sout << "Error: Cannot find target entity (SA) for geometric_validation_property "; 
      Model()->PrintLabel (PD, sout);
      sout << std::endl;
#endif
      return ProdDef;
    }
    Interface_EntityIterator subs = Graph().Sharings(SA);
    for (subs.Start(); subs.More(); subs.Next()) {
      PDS = Handle(StepRepr_PropertyDefinition)::DownCast ( subs.Value() );
      if ( PDS.IsNull() ) return ProdDef;
      Interface_EntityIterator subs1 = Graph().Shareds(PDS);
      for (subs1.Start(); ProdDef.IsNull() && subs1.More(); subs1.Next()) {
	ProdDef = Handle(StepBasic_ProductDefinition)::DownCast ( subs1.Value() );
      }
    }
  }
  else {
    Interface_EntityIterator subs = Graph().Shareds(PDS);
    for (subs.Start(); ProdDef.IsNull() && subs.More(); subs.Next()) {
      ProdDef = Handle(StepBasic_ProductDefinition)::DownCast ( subs.Value() );
    }
  }
#ifdef OCCT_DEBUG
  if ( ProdDef.IsNull() ) {
    Message_Messenger::StreamBuffer sout = Message::SendInfo();
    sout << "Error: Cannot find target entity (SDR) for geometric_validation_property "; 
    Model()->PrintLabel (PD, sout);
    sout << std::endl;
  }
#endif
  return ProdDef;
}

//=======================================================================
//function : GetPropCDSR   CORRECTED
//purpose  : 
//=======================================================================

Handle(StepRepr_NextAssemblyUsageOccurrence) STEPConstruct_ValidationProps::GetPropNAUO (const Handle(StepRepr_PropertyDefinition) &PD) const
{
  StepRepr_CharacterizedDefinition CD = PD->Definition();
    
  // detect target entity of valprop
  Handle(StepRepr_NextAssemblyUsageOccurrence) NAUO;
  Handle(StepRepr_PropertyDefinition) PDS = CD.ProductDefinitionShape();
  if ( PDS.IsNull() ) return NAUO; // not found
  Interface_EntityIterator subs = Graph().Shareds(PDS);
  for (subs.Start(); NAUO.IsNull() && subs.More(); subs.Next()) {
    NAUO = Handle(StepRepr_NextAssemblyUsageOccurrence)::DownCast ( subs.Value() );
  }
  return NAUO;
}

//=======================================================================
//function : GetPropShape   CORRECTED
//purpose  : 
//=======================================================================

TopoDS_Shape STEPConstruct_ValidationProps::GetPropShape (const Handle(StepBasic_ProductDefinition) &ProdDef) const
{
  // find target shape
  TopoDS_Shape S;
  Handle(Transfer_Binder) binder = TransientProcess()->Find(ProdDef);
  if ( ! binder.IsNull() && binder->HasResult() ) {
    S = TransferBRep::ShapeResult ( TransientProcess(), binder );
  }
  //if ( S.IsNull() ) { // for subshape (via shape_aspect)
  //  Handle(StepRepr_Representation) rep = SDR->UsedRepresentation();
  //  for ( Standard_Integer j=1; S.IsNull() && j <= rep->NbItems(); j++ ) {
  //    binder = TransientProcess()->Find(rep->ItemsValue(j));
  //    if ( ! binder.IsNull() && binder->HasResult() ) {
  //	S = TransferBRep::ShapeResult ( TransientProcess(), binder );
  //    }
  //  }
  //}
#ifdef OCCT_DEBUG
  if ( S.IsNull() ) {
    Message_Messenger::StreamBuffer sout = Message::SendInfo();
    sout << "Warning: Entity "; 
    Model()->PrintLabel (ProdDef, sout);
    sout << " is not mapped to shape" << std::endl;
  }
#endif
  return S;
}
    
//=======================================================================
//function : GetPropShape   CORRECTED
//purpose  : 
//=======================================================================

TopoDS_Shape STEPConstruct_ValidationProps::GetPropShape (const Handle(StepRepr_PropertyDefinition) &PD) const
{
  Handle(StepBasic_ProductDefinition) ProdDef = GetPropPD ( PD );
  TopoDS_Shape S;
  if ( ! ProdDef.IsNull() ) S = GetPropShape ( ProdDef );
  return S;
}

//=======================================================================
//function : GetPropReal
//purpose  : 
//=======================================================================

Standard_Boolean STEPConstruct_ValidationProps::GetPropReal (const Handle(StepRepr_RepresentationItem) &item,
							     Standard_Real &Val, Standard_Boolean &isArea) const
{
  // decode volume & area
  if ( ! item->IsKind(STANDARD_TYPE(StepRepr_MeasureRepresentationItem)) ) 
    return Standard_False;
  
  Handle(StepRepr_MeasureRepresentationItem) mri = 
    Handle(StepRepr_MeasureRepresentationItem)::DownCast ( item );

  Handle(StepBasic_MeasureWithUnit) M = mri->Measure();
  TCollection_AsciiString Name = M->ValueComponentMember()->Name();
  StepBasic_Unit Unit = M->UnitComponent();

  Standard_Real scale = 1.;
  Handle(StepBasic_DerivedUnit) DU = Unit.DerivedUnit();
  if ( ! DU.IsNull() ) {
    for(Standard_Integer ind = 1; ind <= DU->NbElements(); ind++) {
      Handle(StepBasic_DerivedUnitElement) DUE = DU->ElementsValue(ind);
      Standard_Real exp = DUE->Exponent();
      Handle(StepBasic_NamedUnit) NU = DUE->Unit();
      STEPConstruct_UnitContext unit;
      unit.ComputeFactors(NU);
      if(unit.LengthDone()) {
	Standard_Real lengthFactor = unit.LengthFactor();
	scale *= pow(lengthFactor,exp);
      }
    }
  }
  else {
    Handle(StepBasic_NamedUnit) NU = Unit.NamedUnit();
    if(!NU.IsNull()) {
      STEPConstruct_UnitContext unit;
      unit.ComputeFactors(NU);
      if(unit.AreaDone())
	scale =  unit.AreaFactor();
      if(unit.VolumeDone())
	scale =  unit.VolumeFactor();
    }
  }
  
  Val = M->ValueComponent() * scale;
  
  if ( Name == "AREA_MEASURE" ) isArea = Standard_True;
  else if ( Name == "VOLUME_MEASURE" ) isArea = Standard_False; 
  else {
#ifdef OCCT_DEBUG
    std::cout << "Warning: Measure " << Model()->StringLabel ( M )->String() << " is neither area not volume" << std::endl;
#endif
    return Standard_False;
  }
  return Standard_True;
}

//=======================================================================
//function : GetPropPnt
//purpose  : 
//=======================================================================

Standard_Boolean STEPConstruct_ValidationProps::GetPropPnt (const Handle(StepRepr_RepresentationItem) &item,
							    const Handle(StepRepr_RepresentationContext) &Context,
							    gp_Pnt &Pnt) const
{
  // centroid
  if ( ! item->IsKind(STANDARD_TYPE(StepGeom_CartesianPoint)) ) 
    return Standard_False;
  
  Handle(StepGeom_CartesianPoint) P = Handle(StepGeom_CartesianPoint)::DownCast ( item );
  if ( P.IsNull() || P->NbCoordinates() != 3 ) {
#ifdef OCCT_DEBUG
    std::cout << "Warning: Point " << Model()->StringLabel ( P )->String() << " is not valid for centroid" << std::endl;
#endif
    return Standard_False;
  }

  gp_Pnt pos ( P->CoordinatesValue(1), P->CoordinatesValue(2), P->CoordinatesValue(3) );
	
  // scale according to units
  if ( ! Context.IsNull() ) {
    Handle(StepRepr_GlobalUnitAssignedContext) theGUAC;
    if (Context->IsKind(STANDARD_TYPE(StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext))) {
      DeclareAndCast(StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext, theGRCAGAUC,Context);
      theGUAC = theGRCAGAUC->GlobalUnitAssignedContext();
    }
    else if (Context->IsKind(STANDARD_TYPE(StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx))) {
      DeclareAndCast(StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx,
		     theGRCAGAUC,Context);
      theGUAC = theGRCAGAUC->GlobalUnitAssignedContext();
    }
    if ( ! theGUAC.IsNull() ) {
      STEPConstruct_UnitContext UnitTool;
      UnitTool.ComputeFactors(theGUAC);
      gp_Pnt zero(0,0,0);
      pos.Scale ( zero, UnitTool.LengthFactor() );
    }
  }
  Pnt = pos;

  return Standard_True;
}

//=======================================================================
//function : SetAssemblyShape   CORRECTED
//purpose  : 
//=======================================================================

void STEPConstruct_ValidationProps::SetAssemblyShape (const TopoDS_Shape& shape)
{
  Handle(TransferBRep_ShapeMapper) mapper = TransferBRep::ShapeMapper(FinderProcess(),shape);
  FinderProcess()->FindTypedTransient(mapper,STANDARD_TYPE(StepBasic_ProductDefinition),myAssemblyPD);
}
