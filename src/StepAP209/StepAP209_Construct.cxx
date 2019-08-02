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


#include <HeaderSection_FileDescription.hxx>
#include <HeaderSection_FileName.hxx>
#include <HeaderSection_FileSchema.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_HArray1OfHAsciiString.hxx>
#include <OSD_Process.hxx>
#include <Quantity_Date.hxx>
#include <StepAP203_CcDesignApproval.hxx>
#include <StepAP203_CcDesignDateAndTimeAssignment.hxx>
#include <StepAP203_CcDesignPersonAndOrganizationAssignment.hxx>
#include <StepAP203_CcDesignSecurityClassification.hxx>
#include <StepAP203_HArray1OfApprovedItem.hxx>
#include <StepAP203_HArray1OfClassifiedItem.hxx>
#include <StepAP203_HArray1OfDateTimeItem.hxx>
#include <StepAP203_HArray1OfPersonOrganizationItem.hxx>
#include <StepAP209_Construct.hxx>
#include <StepAP214_AppliedApprovalAssignment.hxx>
#include <StepAP214_AppliedDateAndTimeAssignment.hxx>
#include <StepAP214_AppliedPersonAndOrganizationAssignment.hxx>
#include <StepAP214_AppliedSecurityClassificationAssignment.hxx>
#include <StepAP214_ApprovalItem.hxx>
#include <StepAP214_DateAndTimeItem.hxx>
#include <StepAP214_HArray1OfApprovalItem.hxx>
#include <StepAP214_HArray1OfDateAndTimeItem.hxx>
#include <StepAP214_HArray1OfPersonAndOrganizationItem.hxx>
#include <StepAP214_HArray1OfSecurityClassificationItem.hxx>
#include <StepAP214_PersonAndOrganizationItem.hxx>
#include <StepAP214_SecurityClassificationItem.hxx>
#include <StepBasic_ApplicationContext.hxx>
#include <StepBasic_ApplicationProtocolDefinition.hxx>
#include <StepBasic_Approval.hxx>
#include <StepBasic_ApprovalDateTime.hxx>
#include <StepBasic_ApprovalPersonOrganization.hxx>
#include <StepBasic_ApprovalRole.hxx>
#include <StepBasic_ApprovalStatus.hxx>
#include <StepBasic_CalendarDate.hxx>
#include <StepBasic_CoordinatedUniversalTimeOffset.hxx>
#include <StepBasic_DateAndTime.hxx>
#include <StepBasic_DateTimeRole.hxx>
#include <StepBasic_DateTimeSelect.hxx>
#include <StepBasic_DesignContext.hxx>
#include <StepBasic_DimensionalExponents.hxx>
#include <StepBasic_HArray1OfNamedUnit.hxx>
#include <StepBasic_HArray1OfProduct.hxx>
#include <StepBasic_HArray1OfProductContext.hxx>
#include <StepBasic_HArray1OfUncertaintyMeasureWithUnit.hxx>
#include <StepBasic_LocalTime.hxx>
#include <StepBasic_MechanicalContext.hxx>
#include <StepBasic_Organization.hxx>
#include <StepBasic_Person.hxx>
#include <StepBasic_PersonAndOrganization.hxx>
#include <StepBasic_PersonAndOrganizationRole.hxx>
#include <StepBasic_PersonOrganizationSelect.hxx>
#include <StepBasic_Product.hxx>
#include <StepBasic_ProductCategory.hxx>
#include <StepBasic_ProductCategoryRelationship.hxx>
#include <StepBasic_ProductContext.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <StepBasic_ProductDefinitionContext.hxx>
#include <StepBasic_ProductDefinitionFormation.hxx>
#include <StepBasic_ProductDefinitionFormationRelationship.hxx>
#include <StepBasic_ProductRelatedProductCategory.hxx>
#include <StepBasic_SecurityClassification.hxx>
#include <StepBasic_SecurityClassificationLevel.hxx>
#include <StepBasic_SiUnitAndMassUnit.hxx>
#include <StepBasic_SiUnitAndThermodynamicTemperatureUnit.hxx>
#include <StepBasic_SiUnitAndTimeUnit.hxx>
#include <StepData_StepModel.hxx>
#include <StepElement_AnalysisItemWithinRepresentation.hxx>
#include <StepElement_ElementMaterial.hxx>
#include <StepFEA_CoordinateSystemType.hxx>
#include <StepFEA_Curve3dElementProperty.hxx>
#include <StepFEA_Curve3dElementRepresentation.hxx>
#include <StepFEA_CurveElementIntervalConstant.hxx>
#include <StepFEA_ElementGeometricRelationship.hxx>
#include <StepFEA_ElementRepresentation.hxx>
#include <StepFEA_FeaAxis2Placement3d.hxx>
#include <StepFEA_FeaModel.hxx>
#include <StepFEA_FeaModel3d.hxx>
#include <StepFEA_FeaModelDefinition.hxx>
#include <StepFEA_HArray1OfCurveElementInterval.hxx>
#include <StepFEA_Surface3dElementRepresentation.hxx>
#include <StepFEA_Volume3dElementRepresentation.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_Direction.hxx>
#include <StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext.hxx>
#include <StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx.hxx>
#include <StepRepr_GlobalUncertaintyAssignedContext.hxx>
#include <StepRepr_GlobalUnitAssignedContext.hxx>
#include <StepRepr_HArray1OfRepresentationItem.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <StepRepr_PropertyDefinitionRepresentation.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepRepr_ShapeRepresentationRelationship.hxx>
#include <StepRepr_StructuralResponseProperty.hxx>
#include <StepRepr_StructuralResponsePropertyDefinitionRepresentation.hxx>
#include <StepShape_ShapeDefinitionRepresentation.hxx>
#include <StepShape_ShapeRepresentation.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HArray1OfAsciiString.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <XSControl_WorkSession.hxx>

//#include <.hxx>
//=======================================================================
//function : StepAP209_Construct
//purpose  : 
//=======================================================================
StepAP209_Construct::StepAP209_Construct () 
{
}
     
//=======================================================================
//function : StepAP209_Construct
//purpose  : 
//=======================================================================

StepAP209_Construct::StepAP209_Construct (const Handle(XSControl_WorkSession) &WS)
     : STEPConstruct_Tool ( WS )
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

Standard_Boolean StepAP209_Construct::Init (const Handle(XSControl_WorkSession) &WS)
{
  return SetWS ( WS );
}


//=======================================================================
//function : IsDesing
//purpose  : 
//=======================================================================

Standard_Boolean StepAP209_Construct::IsDesing (const Handle(StepBasic_ProductDefinitionFormation) &PDF) const
{
  Interface_EntityIterator subs = Graph().Sharings(PDF);
  for (subs.Start(); subs.More(); subs.Next()) {
    Handle(StepBasic_ProductDefinitionFormationRelationship) PDFR = 
      Handle(StepBasic_ProductDefinitionFormationRelationship)::DownCast(subs.Value());
    if(PDFR.IsNull()) continue;
    if ( PDF == PDFR->RelatingProductDefinitionFormation() ) return Standard_True;
  }
  return Standard_False;
}


//=======================================================================
//function : IsAnalys
//purpose  : 
//=======================================================================

Standard_Boolean StepAP209_Construct::IsAnalys (const Handle(StepBasic_ProductDefinitionFormation) &PDF) const
{
  Interface_EntityIterator subs = Graph().Sharings(PDF);
  for (subs.Start(); subs.More(); subs.Next()) {
    Handle(StepBasic_ProductDefinitionFormationRelationship) PDFR = 
      Handle(StepBasic_ProductDefinitionFormationRelationship)::DownCast(subs.Value());
    if(PDFR.IsNull()) continue;
    if ( PDF == PDFR->RelatedProductDefinitionFormation() ) return Standard_True;
  }
  return Standard_False;
}


//=======================================================================
//function : GetElementMaterial
//purpose  : 
//=======================================================================

Handle(StepElement_HSequenceOfElementMaterial) StepAP209_Construct::GetElementMaterial() const
{
  Handle(StepElement_HSequenceOfElementMaterial) aSequence =
    new StepElement_HSequenceOfElementMaterial;
  Handle(Interface_InterfaceModel) model = Model();
  Standard_Integer nb = model->NbEntities();
  for(Standard_Integer i=1; i<=nb; i++) {
    Handle(Standard_Transient) anEntity = model->Value(i);
    if(anEntity->IsKind(STANDARD_TYPE(StepElement_ElementMaterial))) {
      Handle(StepElement_ElementMaterial) anElement =
        Handle(StepElement_ElementMaterial)::DownCast(anEntity);
      aSequence->Append(anElement);
    }
  }
  return aSequence;
}


//=======================================================================
//function : GetElemGeomRelat
//purpose  : 
//=======================================================================

Handle(StepFEA_HSequenceOfElementGeometricRelationship) StepAP209_Construct::GetElemGeomRelat() const
{
  Handle(StepFEA_HSequenceOfElementGeometricRelationship) aSequence =
    new StepFEA_HSequenceOfElementGeometricRelationship;
  Handle(Interface_InterfaceModel) model = Model();
  Standard_Integer nb = model->NbEntities();
  for(Standard_Integer i=1; i<=nb; i++) {
    Handle(Standard_Transient) anEntity = model->Value(i);
    if(anEntity->IsKind(STANDARD_TYPE(StepFEA_ElementGeometricRelationship))) {
      Handle(StepFEA_ElementGeometricRelationship) EGR =
        Handle(StepFEA_ElementGeometricRelationship)::DownCast(anEntity);
      aSequence->Append(EGR);
    }
  }
  return aSequence;
}


//=======================================================================
//function : GetShReprForElem
//purpose  : 
//=======================================================================

Handle(StepShape_ShapeRepresentation) StepAP209_Construct::GetShReprForElem
       (const Handle(StepFEA_ElementRepresentation) &ElemRepr) const
{
  Handle(StepShape_ShapeRepresentation) SR;
  if(ElemRepr.IsNull()) return SR;
  Interface_EntityIterator subs = Graph().Sharings(ElemRepr);
  for (subs.Start(); subs.More() && SR.IsNull() ; subs.Next()) {
    Handle(StepFEA_ElementGeometricRelationship) EGR =
      Handle(StepFEA_ElementGeometricRelationship)::DownCast(subs.Value());
    if(EGR.IsNull()) continue;
    Handle(StepElement_AnalysisItemWithinRepresentation) AIWR = EGR->Item();
    if(AIWR.IsNull()) continue;
    Handle(StepRepr_Representation) Repr = AIWR->Rep();
    if(Repr.IsNull()) continue;
    SR = Handle(StepShape_ShapeRepresentation)::DownCast(Repr);
  }
  return SR;
}



//     methods for getting fea_model


//=======================================================================
//function : FeaModel
//purpose  : 
//=======================================================================

Handle(StepFEA_FeaModel) StepAP209_Construct::FeaModel (const Handle(StepBasic_Product) &Prod) const
{
  Handle(StepFEA_FeaModel) FM;
  if(Prod.IsNull()) return FM;
  Interface_EntityIterator subs = Graph().Sharings(Prod);
  for (subs.Start(); subs.More() && FM.IsNull() ; subs.Next()) {
    Handle(StepBasic_ProductDefinitionFormation) PDF =
      Handle(StepBasic_ProductDefinitionFormation)::DownCast(subs.Value());
    if ( PDF.IsNull() ) continue;
    FM = FeaModel(PDF);
  }
  return FM;
}


//=======================================================================
//function : FeaModel
//purpose  : 
//=======================================================================

Handle(StepFEA_FeaModel) StepAP209_Construct::FeaModel (const Handle(StepBasic_ProductDefinition) &PD) const
{
  //Handle(Interface_InterfaceModel) model = Model();
  //Standard_Integer nb = model->NbEntities();
  Handle(StepFEA_FeaModel) FM;
  if(PD.IsNull()) return FM;
  Interface_EntityIterator subs = Graph().Shareds(PD);
  for (subs.Start(); subs.More() && FM.IsNull() ; subs.Next()) {
    Handle(StepBasic_ProductDefinitionFormation) PDF =
      Handle(StepBasic_ProductDefinitionFormation)::DownCast(subs.Value());
    if ( PDF.IsNull() ) continue;
    FM = FeaModel(PDF);
  }
  return FM;
}


//=======================================================================
//function : FeaModel
//purpose  : 
//=======================================================================

Handle(StepFEA_FeaModel) StepAP209_Construct::FeaModel (const Handle(StepBasic_ProductDefinitionFormation) &PDF) const
{
  Handle(StepFEA_FeaModel) FM;
  if(PDF.IsNull()) return FM;
  Handle(StepBasic_ProductDefinitionFormation) PDF2;
  Interface_EntityIterator subs = Graph().Sharings(PDF);
  for (subs.Start(); subs.More(); subs.Next()) {
    Handle(StepBasic_ProductDefinitionFormationRelationship) PDFR = 
      Handle(StepBasic_ProductDefinitionFormationRelationship)::DownCast(subs.Value());
    if(PDFR.IsNull()) continue;
    PDF2 = PDFR->RelatedProductDefinitionFormation();
  }
  if(PDF2.IsNull() ) return FM;
  subs = Graph().Sharings(PDF2);
  for (subs.Start(); subs.More() && FM.IsNull() ; subs.Next()) {
    Handle(StepBasic_ProductDefinition) PD =
      Handle(StepBasic_ProductDefinition)::DownCast(subs.Value());
    if(PD.IsNull()) continue;
    Interface_EntityIterator subs2 = Graph().Sharings(PD);
    for (subs2.Start(); subs2.More() && FM.IsNull() ; subs2.Next()) {
      Handle(StepRepr_ProductDefinitionShape) PDS =
        Handle(StepRepr_ProductDefinitionShape)::DownCast(subs2.Value());
      if(PDS.IsNull()) continue;
      FM = FeaModel(PDS);
    }
  }
  return FM;
}


//=======================================================================
//function : FeaModel
//purpose  : 
//=======================================================================

Handle(StepFEA_FeaModel) StepAP209_Construct::FeaModel (const Handle(StepRepr_ProductDefinitionShape)& PDS) const
{
  Handle(StepFEA_FeaModel) FM;
  Interface_EntityIterator subs = Graph().Sharings(PDS);
  for (subs.Start(); subs.More() && FM.IsNull() ; subs.Next()) {
    Handle(StepFEA_FeaModelDefinition) FMD = Handle(StepFEA_FeaModelDefinition)::DownCast(subs.Value());
    if(FMD.IsNull()) continue;
    Interface_EntityIterator subs2 = Graph().Sharings(FMD);
    for (subs2.Start(); subs2.More() && FM.IsNull() ; subs2.Next()) {
      //ENTITY structural_response_property - SUBTYPE OF (property_definition);
      Handle(StepRepr_StructuralResponseProperty) SRP = 
        Handle(StepRepr_StructuralResponseProperty)::DownCast(subs2.Value());
      if(SRP.IsNull()) continue;
      Interface_EntityIterator subs3 = Graph().Sharings(SRP);
      for (subs3.Start(); subs3.More() && FM.IsNull() ; subs3.Next()) {
        //ENTITY structural_response_property_definition_representation -
        //SUBTYPE OF (property_definition_representation);
        Handle(StepRepr_StructuralResponsePropertyDefinitionRepresentation) SRPDR = 
          Handle(StepRepr_StructuralResponsePropertyDefinitionRepresentation)::DownCast(subs3.Value());
        if(SRPDR.IsNull()) continue;
        Handle(StepRepr_Representation) Repr = SRPDR->UsedRepresentation();
        if ( Repr.IsNull() ) continue;
        if (Repr->IsKind(STANDARD_TYPE(StepFEA_FeaModel)))
          FM = Handle(StepFEA_FeaModel)::DownCast(Repr);
      }
    }
  }
  return FM;
}


//=======================================================================
//function : GetFeaAxis2Placement3D
//purpose  : 
//=======================================================================

Handle(StepFEA_FeaAxis2Placement3d) StepAP209_Construct::GetFeaAxis2Placement3d
       (const Handle(StepFEA_FeaModel)& theFeaModel) const 
{
  Handle(StepFEA_FeaAxis2Placement3d) FA2P3D = new StepFEA_FeaAxis2Placement3d;
  if(theFeaModel.IsNull()) return FA2P3D;
  Interface_EntityIterator subs = Graph().Shareds(theFeaModel);
  for (subs.Start(); subs.More(); subs.Next()) {
    FA2P3D =  Handle(StepFEA_FeaAxis2Placement3d)::DownCast(subs.Value());
    if(FA2P3D.IsNull()) continue;
    return FA2P3D;
  }
  return FA2P3D;
}


//     methods for getting idealized_analysis_shape


//=======================================================================
//function : IdealShape
//purpose  : 
//=======================================================================

Handle(StepShape_ShapeRepresentation) StepAP209_Construct::IdealShape (const Handle(StepBasic_Product) &Prod) const
{
  Handle(StepShape_ShapeRepresentation) SR;
  if(Prod.IsNull()) return SR;
  Interface_EntityIterator subs = Graph().Sharings(Prod);
  for (subs.Start(); subs.More() && SR.IsNull() ; subs.Next()) {
    Handle(StepBasic_ProductDefinitionFormation) PDF =
      Handle(StepBasic_ProductDefinitionFormation)::DownCast(subs.Value());
    if ( PDF.IsNull() ) continue;
    SR = IdealShape(PDF);
  }
  return SR;
}


//=======================================================================
//function : IdealShape
//purpose  : 
//=======================================================================

Handle(StepShape_ShapeRepresentation) StepAP209_Construct::IdealShape (const Handle(StepBasic_ProductDefinition) &PD) const
{
  Handle(StepShape_ShapeRepresentation) SR;
  if(PD.IsNull()) return SR;
  Interface_EntityIterator subs = Graph().Shareds(PD);
  for (subs.Start(); subs.More() && SR.IsNull() ; subs.Next()) {
    Handle(StepBasic_ProductDefinitionFormation) PDF =
      Handle(StepBasic_ProductDefinitionFormation)::DownCast(subs.Value());
    if ( PDF.IsNull() ) continue;
    SR = IdealShape(PDF);
  }
  return SR;
}


//=======================================================================
//function : IdealShape
//purpose  : 
//=======================================================================

Handle(StepShape_ShapeRepresentation) StepAP209_Construct::IdealShape (const Handle(StepBasic_ProductDefinitionFormation) &PDF) const
{
  Handle(StepShape_ShapeRepresentation) SR;
  if(PDF.IsNull()) return SR;
  Handle(StepBasic_ProductDefinitionFormation) PDF2;
  Interface_EntityIterator subs = Graph().Sharings(PDF);
  for (subs.Start(); subs.More(); subs.Next()) {
    Handle(StepBasic_ProductDefinitionFormationRelationship) PDFR = 
      Handle(StepBasic_ProductDefinitionFormationRelationship)::DownCast(subs.Value());
    if(PDFR.IsNull()) continue;
    PDF2 = PDFR->RelatedProductDefinitionFormation();
  }
  if(PDF2.IsNull() ) return SR;
  subs = Graph().Sharings(PDF2);
  for (subs.Start(); subs.More() && SR.IsNull() ; subs.Next()) {
    Handle(StepBasic_ProductDefinition) PD =
      Handle(StepBasic_ProductDefinition)::DownCast(subs.Value());
    if(PD.IsNull()) continue;
    Interface_EntityIterator subs2 = Graph().Sharings(PD);
    for (subs2.Start(); subs2.More() && SR.IsNull() ; subs2.Next()) {
      Handle(StepRepr_ProductDefinitionShape) PDS =
        Handle(StepRepr_ProductDefinitionShape)::DownCast(subs2.Value());
      if(PDS.IsNull()) continue;
      SR = IdealShape(PDS);
    }
  }
  return SR;
}


//=======================================================================
//function : IdealShape
//purpose  : 
//=======================================================================

Handle(StepShape_ShapeRepresentation) StepAP209_Construct::IdealShape (const Handle(StepRepr_ProductDefinitionShape)& PDS) const
{
  Handle(StepShape_ShapeRepresentation) SR;
  Interface_EntityIterator subs = Graph().Sharings(PDS);
  for (subs.Start(); subs.More() && SR.IsNull() ; subs.Next()) {
    Handle(StepShape_ShapeDefinitionRepresentation) SDR =
      Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(subs.Value());
    if(SDR.IsNull()) continue;
    SR = Handle(StepShape_ShapeRepresentation)::DownCast(SDR->UsedRepresentation());
  }
  return SR;
}


//     methods for getting nominal_design_shape


//=======================================================================
//function : NominShape
//purpose  : 
//=======================================================================

Handle(StepShape_ShapeRepresentation) StepAP209_Construct::NominShape (const Handle(StepBasic_Product) &Prod) const
{
  Handle(StepShape_ShapeRepresentation) SR;
  if(Prod.IsNull()) return SR;
  Interface_EntityIterator subs = Graph().Sharings(Prod);
  for (subs.Start(); subs.More() && SR.IsNull() ; subs.Next()) {
    Handle(StepBasic_ProductDefinitionFormation) PDF =
      Handle(StepBasic_ProductDefinitionFormation)::DownCast(subs.Value());
    if ( PDF.IsNull() ) continue;
    SR = NominShape(PDF);
  }
  return SR;
}


//=======================================================================
//function : NominShape
//purpose  : 
//=======================================================================

Handle(StepShape_ShapeRepresentation) StepAP209_Construct::NominShape (const Handle(StepBasic_ProductDefinitionFormation) &PDF) const
{
  Handle(StepShape_ShapeRepresentation) SR;
  if(PDF.IsNull()) return SR;
  Handle(StepBasic_ProductDefinitionFormation) PDF2;
  Interface_EntityIterator subs = Graph().Sharings(PDF);
  for (subs.Start(); subs.More(); subs.Next()) {
    Handle(StepBasic_ProductDefinitionFormationRelationship) PDFR = 
      Handle(StepBasic_ProductDefinitionFormationRelationship)::DownCast(subs.Value());
    if(PDFR.IsNull()) continue;
    PDF2 = PDFR->RelatingProductDefinitionFormation();
  }
  if(PDF2.IsNull() ) return SR;
  subs = Graph().Sharings(PDF2);
  for (subs.Start(); subs.More() && SR.IsNull() ; subs.Next()) {
    Handle(StepBasic_ProductDefinition) PD =
      Handle(StepBasic_ProductDefinition)::DownCast(subs.Value());
    if(PD.IsNull()) continue;
    Interface_EntityIterator subs2 = Graph().Sharings(PD);
    for (subs2.Start(); subs2.More() && SR.IsNull() ; subs2.Next()) {
      Handle(StepRepr_ProductDefinitionShape) PDS =
        Handle(StepRepr_ProductDefinitionShape)::DownCast(subs2.Value());
      if(PDS.IsNull()) continue;
      Interface_EntityIterator subs3 = Graph().Sharings(PDS);
      for (subs3.Start(); subs3.More() && SR.IsNull() ; subs3.Next()) {
        Handle(StepShape_ShapeDefinitionRepresentation) SDR =
          Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(subs3.Value());
        if(SDR.IsNull()) continue;
        SR = Handle(StepShape_ShapeRepresentation)::DownCast(SDR->UsedRepresentation());
      }
    }
  }
  return SR;
}



//=======================================================================
//function : GetElements1D
//purpose  : 
//=======================================================================

Handle(StepFEA_HSequenceOfElementRepresentation) StepAP209_Construct::GetElements1D
       (const Handle(StepFEA_FeaModel)& theFeaModel) const 
{
  return GetFeaElements(theFeaModel,STANDARD_TYPE(StepFEA_Curve3dElementRepresentation));
}


//=======================================================================
//function : GetElements2D
//purpose  : 
//=======================================================================

Handle(StepFEA_HSequenceOfElementRepresentation) StepAP209_Construct::GetElements2D
       (const Handle(StepFEA_FeaModel)& theFeaModel) const 
{
  return GetFeaElements(theFeaModel,STANDARD_TYPE(StepFEA_Surface3dElementRepresentation));
}


//=======================================================================
//function : GetElements3D
//purpose  : 
//=======================================================================

Handle(StepFEA_HSequenceOfElementRepresentation) StepAP209_Construct::GetElements3D
       (const Handle(StepFEA_FeaModel)& theFeaModel) const 
{
  return GetFeaElements(theFeaModel,STANDARD_TYPE(StepFEA_Volume3dElementRepresentation));
}


//=======================================================================
//function : GetFeaElements
//purpose  : 
//=======================================================================

Handle(StepFEA_HSequenceOfElementRepresentation) StepAP209_Construct::GetFeaElements
       (const Handle(StepFEA_FeaModel)& theFeaModel,
        const Handle(Standard_Type)& theType) const
{
  Handle(StepFEA_HSequenceOfElementRepresentation) aSequence;
  if( !theType->SubType(STANDARD_TYPE(StepFEA_ElementRepresentation)))
    return aSequence;
  
  Interface_EntityIterator anIter = Graph().Sharings(theFeaModel);
  anIter.Start();
  if(anIter.More())
    aSequence = new StepFEA_HSequenceOfElementRepresentation;
  
  for (; anIter.More(); anIter.Next()) {
    Handle(Standard_Transient) anEntity = anIter.Value();
    if(anEntity->IsKind(theType)) {
      Handle(StepFEA_ElementRepresentation) anElement =
        Handle(StepFEA_ElementRepresentation)::DownCast(anEntity);
      aSequence->Append(anElement);
    }
  }
  return aSequence;

}


//=======================================================================
//function : GetFeaElements
//purpose  : 
//=======================================================================

Handle(StepElement_HSequenceOfCurveElementSectionDefinition) StepAP209_Construct::GetCurElemSection
       (const Handle(StepFEA_Curve3dElementRepresentation)& ElemRepr) const
{
  Handle(StepElement_HSequenceOfCurveElementSectionDefinition) aSequence =
    new StepElement_HSequenceOfCurveElementSectionDefinition;
  if(ElemRepr.IsNull()) return aSequence;
  
  Handle(StepFEA_Curve3dElementProperty) C3dEP = ElemRepr->Property();
  if(C3dEP.IsNull()) return aSequence;

  Handle(StepFEA_HArray1OfCurveElementInterval) ACEI = C3dEP->IntervalDefinitions();
  if(ACEI.IsNull()) return aSequence;

  for(Standard_Integer i=1; i<=ACEI->Length(); i++) {
    Handle(StepFEA_CurveElementIntervalConstant) CEIC =
      Handle(StepFEA_CurveElementIntervalConstant)::DownCast(ACEI->Value(i));
    if(CEIC.IsNull()) continue;
    aSequence->Append(CEIC->Section());
  }
  return aSequence;
}


//=======================================================================
//function : CreateAnalysStructure
//purpose  : 
//=======================================================================

Standard_Boolean StepAP209_Construct::CreateAnalysStructure (const Handle(StepBasic_Product) &Prod) const
{
  if(Prod.IsNull()) return Standard_False;
  Interface_EntityIterator subs = Graph().Sharings(Prod);
  Handle(StepBasic_ProductDefinitionFormation) PDF;
  for (subs.Start(); subs.More() && PDF.IsNull() ; subs.Next()) {
    PDF = Handle(StepBasic_ProductDefinitionFormation)::DownCast(subs.Value());
    if(PDF.IsNull()) continue;
  }
  if(PDF.IsNull()) return Standard_False;
  //if( IsDesing(PDF) || IsAnalys(PDF) ) return Standard_False;

  // find nominal_design_shape:
  Handle(StepShape_ShapeRepresentation) SR;
  Handle(StepBasic_ProductDefinition) PD;
  subs = Graph().Sharings(PDF);
  for (subs.Start(); subs.More() && SR.IsNull() ; subs.Next()) {
    PD = Handle(StepBasic_ProductDefinition)::DownCast(subs.Value());
    if(PD.IsNull()) continue;
    Interface_EntityIterator subs2 = Graph().Sharings(PD);
    for (subs2.Start(); subs2.More() && SR.IsNull() ; subs2.Next()) {
      Handle(StepRepr_ProductDefinitionShape) PDS =
        Handle(StepRepr_ProductDefinitionShape)::DownCast(subs2.Value());
      if(PDS.IsNull()) continue;
      Interface_EntityIterator subs3 = Graph().Sharings(PDS);
      for (subs3.Start(); subs3.More() && SR.IsNull() ; subs3.Next()) {
        Handle(StepShape_ShapeDefinitionRepresentation) SDR =
          Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(subs3.Value());
        if(SDR.IsNull()) continue;
        SR = Handle(StepShape_ShapeRepresentation)::DownCast(SDR->UsedRepresentation());
      }
    }
  }
  if(SR.IsNull()) return Standard_False; // no nominal_design_shape
  
  // create structure:
  ReplaceCcDesingToApplied();
  Handle(StepData_StepModel) smodel = Handle(StepData_StepModel)::DownCast(Model());

  // replace existing contexts for using AP209
  Handle(StepBasic_ProductContext) OldProdCtx = Prod->FrameOfReferenceValue(1);
  if(!OldProdCtx.IsNull()) {
    Handle(StepBasic_ProductContext) ProdCtx = new StepBasic_ProductContext;
    ProdCtx->Init(OldProdCtx->Name(),
                  OldProdCtx->FrameOfReference(),
                  OldProdCtx->DisciplineType());
    smodel->ReplaceEntity(smodel->Number(OldProdCtx),ProdCtx);
    smodel->SetIdentLabel(ProdCtx, smodel->Number(ProdCtx));
    Handle(StepBasic_HArray1OfProductContext) HAPC = Prod->FrameOfReference();
    HAPC->SetValue(1,ProdCtx);
    Prod->SetFrameOfReference(HAPC);
  }
  Handle(StepBasic_ProductDefinitionContext) OldPDCtx = PD->FrameOfReference();
  if(!OldPDCtx.IsNull()) {
    Handle(StepBasic_ProductDefinitionContext) PDCtx = new StepBasic_ProductDefinitionContext;
    PDCtx->Init(OldPDCtx->Name(),
                OldPDCtx->FrameOfReference(),
                OldPDCtx->LifeCycleStage());
    smodel->ReplaceEntity(smodel->Number(OldPDCtx),PDCtx);
    smodel->SetIdentLabel(PDCtx, smodel->Number(PDCtx));
    PD->SetFrameOfReference(PDCtx);
  }

  // add idealized_analys_shape:
  Handle(StepShape_ShapeRepresentation) AnaSR = new StepShape_ShapeRepresentation;
  Handle(StepRepr_RepresentationItem) RI = new StepRepr_RepresentationItem;
  RI = SR->ItemsValue(1);
  smodel->AddWithRefs(RI); // add new representation_item
  smodel->SetIdentLabel(RI, smodel->Number(RI));
  Handle(StepRepr_HArray1OfRepresentationItem) ARI = new StepRepr_HArray1OfRepresentationItem(1,1);
  ARI->SetValue(1,RI);
  AnaSR->Init(new TCollection_HAsciiString("idealized_analysis_shape"),
  	      ARI, SR->ContextOfItems());
  smodel->AddWithRefs(AnaSR); // add idealized_analys_shape
  smodel->SetIdentLabel(AnaSR, smodel->Number(AnaSR));
  
  // add product:
  Handle(StepBasic_Product) AnaProd = new StepBasic_Product;
  AnaProd->Init(new TCollection_HAsciiString(""), new TCollection_HAsciiString("analysis"),
                new TCollection_HAsciiString("analysis product"), Prod->FrameOfReference());
  smodel->AddWithRefs(AnaProd);
  smodel->SetIdentLabel(AnaProd, smodel->Number(AnaProd));

  // add product_definition_formation:
  Handle(StepBasic_ProductDefinitionFormation) AnaPDF =
    new StepBasic_ProductDefinitionFormation;
  AnaPDF->Init(new TCollection_HAsciiString(""),
               new TCollection_HAsciiString("analysis version"), AnaProd);
  smodel->AddWithRefs(AnaPDF);
  smodel->SetIdentLabel(AnaPDF, smodel->Number(AnaPDF));

  // add product_definition_formation_relationship:
  Handle(StepBasic_ProductDefinitionFormationRelationship) PDFR =
    new StepBasic_ProductDefinitionFormationRelationship;
  PDFR->Init(new TCollection_HAsciiString(""),
             new TCollection_HAsciiString("analysis design version relationship"),
             new TCollection_HAsciiString(""), PDF, AnaPDF);
  smodel->AddWithRefs(PDFR);
  smodel->SetIdentLabel(PDFR, smodel->Number(PDFR));

  // add product_definition:
  Handle(StepBasic_ProductDefinition) AnaPD = new StepBasic_ProductDefinition;
  Handle(StepBasic_ProductDefinitionContext) AnaPDC =
    new StepBasic_ProductDefinitionContext;
  Handle(StepBasic_ApplicationContext) AC = Prod->FrameOfReferenceValue(1)->FrameOfReference();
  AnaPDC->Init(new TCollection_HAsciiString("analysis"), AC,
               new TCollection_HAsciiString("analysis") );
  smodel->AddWithRefs(AnaPDC); // add new product_definition_context
  smodel->SetIdentLabel(AnaPDC, smodel->Number(AnaPDC));
  AnaPD->Init(new TCollection_HAsciiString("analysis"), 
              new TCollection_HAsciiString("analysis discipline product definition"),
              AnaPDF, AnaPDC);
  smodel->AddWithRefs(AnaPD); // add new product_definition
  smodel->SetIdentLabel(AnaPD, smodel->Number(AnaPD));

  // add product_definition_shape:
  Handle(StepRepr_ProductDefinitionShape) AnaPDS = new StepRepr_ProductDefinitionShape;
  StepRepr_CharacterizedDefinition ChDef;
  ChDef.SetValue(AnaPD);
  AnaPDS->Init(new TCollection_HAsciiString(""), Standard_True,
               new TCollection_HAsciiString("analysis shape"), ChDef);
  smodel->AddWithRefs(AnaPDS);
  smodel->SetIdentLabel(AnaPDS, smodel->Number(AnaPDS));

  // add shape_definition_representation:
  Handle(StepShape_ShapeDefinitionRepresentation) AnaSDR = new StepShape_ShapeDefinitionRepresentation;
  StepRepr_RepresentedDefinition RepDef;
  RepDef.SetValue(AnaPDS);
  AnaSDR->Init(RepDef, AnaSR);
  smodel->AddWithRefs(AnaSDR);
  smodel->SetIdentLabel(AnaSDR, smodel->Number(AnaSDR));

  // add shape_representation_relationship:
  Handle(StepRepr_ShapeRepresentationRelationship) SRR =
    new StepRepr_ShapeRepresentationRelationship;
  SRR->Init(new TCollection_HAsciiString("basis"), new TCollection_HAsciiString(""), AnaSR, SR);
  smodel->AddWithRefs(SRR);
  smodel->SetIdentLabel(SRR, smodel->Number(SRR));

  CreateAddingEntities(AnaPD);


  WS()->ComputeGraph(Standard_True);
  WS()->ComputeCheck(Standard_True);

  return Standard_True;
}


//=======================================================================
//function : CreateFeaStructure
//purpose  : 
//=======================================================================

Standard_Boolean StepAP209_Construct::CreateFeaStructure (const Handle(StepBasic_Product) &Prod) const
{
  if(Prod.IsNull()) {
#ifdef OCCT_DEBUG
    std::cout<<"Prod.IsNull()"<<std::endl;
#endif
    return Standard_False;
  }
  Handle(StepShape_ShapeRepresentation) AnaSR = IdealShape(Prod);
  if(AnaSR.IsNull()) {
#ifdef OCCT_DEBUG
    std::cout<<"AnaSR.IsNull()"<<std::endl;
#endif
    return Standard_False;
  }
  Handle(StepRepr_ProductDefinitionShape) AnaPDS;
  Interface_EntityIterator subs = Graph().Sharings(AnaSR);
  for (subs.Start(); subs.More() && AnaPDS.IsNull(); subs.Next()) {
    Handle(StepShape_ShapeDefinitionRepresentation) SDR =
      Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(subs.Value());
    if ( SDR.IsNull() ) continue;
    AnaPDS = Handle(StepRepr_ProductDefinitionShape)::DownCast(SDR->Definition().Value());
  }

  //Handle(Interface_InterfaceModel) model = Model();
  Handle(StepData_StepModel) smodel = Handle(StepData_StepModel)::DownCast(Model());

  // add fea_model_definition
  Handle(StepFEA_FeaModelDefinition) FMD = new StepFEA_FeaModelDefinition;
  FMD->Init(new TCollection_HAsciiString("FEA_MODEL"),
            new TCollection_HAsciiString("FEA_MODEL"), AnaPDS, StepData_LFalse);
  smodel->AddWithRefs(FMD);
  smodel->SetIdentLabel(FMD, smodel->Number(FMD));

  // add fea_axis2_placement_3d
  Handle(StepFEA_FeaAxis2Placement3d) FA2P3D = new StepFEA_FeaAxis2Placement3d;
  Handle(StepGeom_CartesianPoint) SGCP = new StepGeom_CartesianPoint;
  SGCP->Init3D(new TCollection_HAsciiString(""), 0., 0., 0.);
  Handle(TColStd_HArray1OfReal) ArrTmp = new TColStd_HArray1OfReal(1,3);
  ArrTmp->SetValue(1,0.);
  ArrTmp->SetValue(2,0.);
  ArrTmp->SetValue(3,1.);
  Handle(StepGeom_Direction) SGD1 = new StepGeom_Direction;
  SGD1->Init(new TCollection_HAsciiString(""), ArrTmp);
  ArrTmp->SetValue(1,1.);
  ArrTmp->SetValue(2,0.);
  ArrTmp->SetValue(3,0.);
  Handle(StepGeom_Direction) SGD2 = new StepGeom_Direction;
  SGD2->Init(new TCollection_HAsciiString(""), ArrTmp);
  FA2P3D->Init(new TCollection_HAsciiString("FEA_BASIC_COORD_SYSTEM"),
               SGCP, Standard_True, SGD1, Standard_True, SGD2, StepFEA_Cartesian,
               new TCollection_HAsciiString("FEA_BASIC_COORD_SYSTEM"));
  smodel->AddWithRefs(FA2P3D);
  smodel->SetIdentLabel(FA2P3D, smodel->Number(FA2P3D));

  // create context for fea_model
  Handle(StepShape_ShapeRepresentation) NS = NominShape(Prod);
  Handle(StepRepr_RepresentationContext) RC = NS->ContextOfItems();
  Handle(StepGeom_GeometricRepresentationContext) GeoCtx;
  Handle(StepBasic_HArray1OfNamedUnit) OldHANU;
  if(RC->IsKind(STANDARD_TYPE(StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx))) {
    Handle(StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx) GeoUnitCtxNS =
      Handle(StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx)::DownCast(RC);
    GeoCtx = GeoUnitCtxNS->GeometricRepresentationContext();
    OldHANU = GeoUnitCtxNS->GlobalUnitAssignedContext()->Units();
  }
  if(RC->IsKind(STANDARD_TYPE(StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext))) {
    Handle(StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext) GeoUnitCtxNS =
      Handle(StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext)::DownCast(RC);
    GeoCtx = GeoUnitCtxNS->GeometricRepresentationContext();
    OldHANU = GeoUnitCtxNS->GlobalUnitAssignedContext()->Units();
  }
  Handle(StepBasic_HArray1OfNamedUnit) NewHANU = new StepBasic_HArray1OfNamedUnit(1,OldHANU->Length()+3);
  for(Standard_Integer i=1; i<=OldHANU->Length(); i++)
    NewHANU->SetValue(i,OldHANU->Value(i));
  // create SiUnitAndTimeUnit
  Handle(StepBasic_SiUnitAndTimeUnit) SUTU = new StepBasic_SiUnitAndTimeUnit;
  SUTU->Init(Standard_False,StepBasic_spExa,StepBasic_sunSecond);
  smodel->AddWithRefs(SUTU);
  smodel->SetIdentLabel(SUTU, smodel->Number(SUTU));
  NewHANU->SetValue(OldHANU->Length()+1,SUTU);
  //create SiUnitAndMassUnit
  Handle(StepBasic_SiUnitAndMassUnit) SUMU = new StepBasic_SiUnitAndMassUnit;
  SUMU->Init(Standard_True,StepBasic_spKilo,StepBasic_sunGram);
  smodel->AddWithRefs(SUMU);
  smodel->SetIdentLabel(SUMU, smodel->Number(SUMU));
  NewHANU->SetValue(OldHANU->Length()+2,SUMU);
  // create SiUnitAndThermodynamicTemperatureUnit
  Handle(StepBasic_SiUnitAndThermodynamicTemperatureUnit) SUTTU =
    new StepBasic_SiUnitAndThermodynamicTemperatureUnit;
  SUTTU->Init(Standard_False,StepBasic_spExa,StepBasic_sunDegreeCelsius);
  smodel->AddWithRefs(SUTTU);
  smodel->SetIdentLabel(SUTTU, smodel->Number(SUTTU));
  NewHANU->SetValue(OldHANU->Length()+3,SUTTU);
  
  Handle(StepRepr_GlobalUnitAssignedContext) NewUnitCtx = new StepRepr_GlobalUnitAssignedContext;
  NewUnitCtx->Init(new TCollection_HAsciiString(""), new TCollection_HAsciiString(""),NewHANU);
  Handle(StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext) NewGeoCtx =
    new StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext;
  NewGeoCtx->Init(new TCollection_HAsciiString("REP_CONTEXT_FEA"),
                  new TCollection_HAsciiString("3D"), GeoCtx, NewUnitCtx);
  smodel->AddWithRefs(NewGeoCtx);
  smodel->SetIdentLabel(NewGeoCtx, smodel->Number(NewGeoCtx));

  // create fea_model_3d
  Handle(StepFEA_FeaModel3d) FM = new StepFEA_FeaModel3d;
  FM->SetName(new TCollection_HAsciiString("FEA_MODEL"));
  Handle(StepRepr_HArray1OfRepresentationItem) HARI = 
    new StepRepr_HArray1OfRepresentationItem(1,1);
  HARI->SetValue(1,FA2P3D);
  Handle(TColStd_HArray1OfAsciiString) HAAS = new TColStd_HArray1OfAsciiString(1,1);
  HAAS->SetValue(1,"FEA_SOLVER");
  FM->Init(new TCollection_HAsciiString("FEA_MODEL"), HARI,
           NewGeoCtx, new TCollection_HAsciiString(""),
           HAAS, new TCollection_HAsciiString("ANALYSIS_MODEL"),
           new TCollection_HAsciiString("LINEAR_STATIC"));
  smodel->AddWithRefs(FM);
  smodel->SetIdentLabel(FM, smodel->Number(FM));

  // add structural_response_property
  Handle(StepRepr_StructuralResponseProperty) SRP =
    new StepRepr_StructuralResponseProperty;
  StepRepr_CharacterizedDefinition ChDef2;
  ChDef2.SetValue(FMD);
  SRP->Init(new TCollection_HAsciiString("STRUCT_RESP_PROP"), Standard_True,
            new TCollection_HAsciiString("STRUCTURAL_RESPONSE_PROPERTY"),ChDef2);
  smodel->AddWithRefs(SRP);
  smodel->SetIdentLabel(SRP, smodel->Number(SRP));

  // add structural_response_property_definition_representation
  Handle(StepRepr_StructuralResponsePropertyDefinitionRepresentation) SRPDR =
    new StepRepr_StructuralResponsePropertyDefinitionRepresentation;
  StepRepr_RepresentedDefinition RepDef2;
  RepDef2.SetValue(SRP);
  SRPDR->Init(RepDef2,FM);
  smodel->AddWithRefs(SRPDR);
  smodel->SetIdentLabel(SRPDR, smodel->Number(SRPDR));

  WS()->ComputeGraph(Standard_True);
  WS()->ComputeCheck(Standard_True);

  return Standard_True;
}


//=======================================================================
//function : ReplaceCcDesingToApplied
//purpose  : Put into model entities Applied... for AP209 instead of
//           entities CcDesing... from AP203
//=======================================================================

Standard_Boolean StepAP209_Construct::ReplaceCcDesingToApplied() const
{
  Handle(StepData_StepModel) smodel = Handle(StepData_StepModel)::DownCast(Model());
  Standard_Integer nb = smodel->NbEntities();
  for(Standard_Integer i=1; i<=nb; i++) {
    Handle(Standard_Transient) anEntity = smodel->Value(i);
    if(anEntity->IsKind(STANDARD_TYPE(StepAP203_CcDesignApproval))) {
      Handle(StepAP203_CcDesignApproval) ent = Handle(StepAP203_CcDesignApproval)::DownCast(anEntity);
      Handle(StepAP214_AppliedApprovalAssignment) nent = new StepAP214_AppliedApprovalAssignment;
      Handle(StepAP203_HArray1OfApprovedItem) HAAI203 = ent->Items();
      Handle(StepAP214_HArray1OfApprovalItem) HAAI214 =
        new StepAP214_HArray1OfApprovalItem(1,HAAI203->Length());
      for(Standard_Integer j=1; j<=HAAI203->Length(); j++) {
        StepAP214_ApprovalItem AI214;
        AI214.SetValue(HAAI203->Value(j).Value());
        HAAI214->SetValue(j,AI214);
      }
      nent->Init(ent->AssignedApproval(), HAAI214);
      smodel->ReplaceEntity(i,nent);
      smodel->SetIdentLabel(nent, smodel->Number(nent));
    }
    else if(anEntity->IsKind(STANDARD_TYPE(StepAP203_CcDesignPersonAndOrganizationAssignment))) {
      Handle(StepAP203_CcDesignPersonAndOrganizationAssignment) ent =
        Handle(StepAP203_CcDesignPersonAndOrganizationAssignment)::DownCast(anEntity);
      Handle(StepAP214_AppliedPersonAndOrganizationAssignment) nent =
        new StepAP214_AppliedPersonAndOrganizationAssignment;
      Handle(StepAP203_HArray1OfPersonOrganizationItem) HAPOI203 = ent->Items();
      Handle(StepAP214_HArray1OfPersonAndOrganizationItem) HAPOI214 =
        new StepAP214_HArray1OfPersonAndOrganizationItem(1,HAPOI203->Length());
      for(Standard_Integer j=1; j<=HAPOI203->Length(); j++) {
        StepAP214_PersonAndOrganizationItem POI214;
        POI214.SetValue(HAPOI203->Value(j).Value());
        HAPOI214->SetValue(j,POI214);
      }
      nent->Init(ent->AssignedPersonAndOrganization(), ent->Role(), HAPOI214);
      smodel->ReplaceEntity(i,nent);
      smodel->SetIdentLabel(nent, smodel->Number(nent));
    }
    else if(anEntity->IsKind(STANDARD_TYPE(StepAP203_CcDesignDateAndTimeAssignment))) {
      Handle(StepAP203_CcDesignDateAndTimeAssignment) ent =
        Handle(StepAP203_CcDesignDateAndTimeAssignment)::DownCast(anEntity);
      Handle(StepAP214_AppliedDateAndTimeAssignment) nent = new StepAP214_AppliedDateAndTimeAssignment;
      Handle(StepAP203_HArray1OfDateTimeItem) HADTI203 = ent->Items();
      Handle(StepAP214_HArray1OfDateAndTimeItem) HADTI214 =
        new StepAP214_HArray1OfDateAndTimeItem(1,HADTI203->Length());
      for(Standard_Integer j=1; j<=HADTI203->Length(); j++) {
        StepAP214_DateAndTimeItem DTI214;
        DTI214.SetValue(HADTI203->Value(j).Value());
        HADTI214->SetValue(j,DTI214);
      }
      nent->Init(ent->AssignedDateAndTime(), ent->Role(), HADTI214);
      smodel->ReplaceEntity(i,nent);
      smodel->SetIdentLabel(nent, smodel->Number(nent));
    }
    else if(anEntity->IsKind(STANDARD_TYPE(StepAP203_CcDesignSecurityClassification))) {
      Handle(StepAP203_CcDesignSecurityClassification) ent =
        Handle(StepAP203_CcDesignSecurityClassification)::DownCast(anEntity);
      Handle(StepAP214_AppliedSecurityClassificationAssignment) nent =
        new StepAP214_AppliedSecurityClassificationAssignment;
      Handle(StepAP203_HArray1OfClassifiedItem) HACI203 = ent->Items();
      Handle(StepAP214_HArray1OfSecurityClassificationItem) HASCI214 =
        new StepAP214_HArray1OfSecurityClassificationItem(1,HACI203->Length());
      for(Standard_Integer j=1; j<=HACI203->Length(); j++) {
        StepAP214_SecurityClassificationItem SCI214;
        SCI214.SetValue(HACI203->Value(j).Value());
        HASCI214->SetValue(j,SCI214);
      }
      nent->Init(ent->AssignedSecurityClassification(), HASCI214);
      smodel->ReplaceEntity(i,nent);
      smodel->SetIdentLabel(nent, smodel->Number(nent));
    }
  }

  return Standard_True;
}


//=======================================================================
//function : CreateAddingEntities
//purpose  : create approval.. , date.. , time.. , person.. and
//           organization.. entities for analysis structure
//=======================================================================

Standard_Boolean StepAP209_Construct::CreateAddingEntities
  (const Handle(StepBasic_ProductDefinition) &AnaPD) const
{
  Handle(StepData_StepModel) smodel = Handle(StepData_StepModel)::DownCast(Model());
  Handle(StepBasic_ProductDefinitionFormation) AnaPDF = AnaPD->Formation();
  Handle(StepBasic_Product) AnaProd = AnaPDF->OfProduct();

  Handle(StepBasic_ApprovalStatus) AS = new StepBasic_ApprovalStatus;
  AS->Init(new TCollection_HAsciiString("approved"));
  smodel->AddEntity(AS);
  smodel->SetIdentLabel(AS, smodel->Number(AS));
  Handle(StepBasic_Approval) Appr = new StepBasic_Approval;
  Appr->Init(AS, new TCollection_HAsciiString("approved"));
  smodel->AddWithRefs(Appr);
  smodel->SetIdentLabel(Appr, smodel->Number(Appr));

  Handle(StepBasic_SecurityClassificationLevel) SCL = new StepBasic_SecurityClassificationLevel;
  SCL->Init(new TCollection_HAsciiString("unclassified"));
  smodel->AddEntity(SCL);
  smodel->SetIdentLabel(SCL, smodel->Number(SCL));
  Handle(StepBasic_SecurityClassification) SC = new StepBasic_SecurityClassification;
  SC->Init(new TCollection_HAsciiString(""), new TCollection_HAsciiString(""), SCL);
  smodel->AddWithRefs(SC);
  smodel->SetIdentLabel(SC, smodel->Number(SC));

  Handle(StepAP214_AppliedApprovalAssignment) AAA = new StepAP214_AppliedApprovalAssignment;
  Handle(StepAP214_HArray1OfApprovalItem) HAAI = new StepAP214_HArray1OfApprovalItem(1,3);
  StepAP214_ApprovalItem AI1;
  AI1.SetValue(AnaPD);
  HAAI->SetValue(1,AI1);
  StepAP214_ApprovalItem AI2;
  AI2.SetValue(AnaPDF);
  HAAI->SetValue(2,AI2);
  StepAP214_ApprovalItem AI3;
  AI3.SetValue(SC);
  HAAI->SetValue(3,AI3);
  AAA->Init(Appr, HAAI);
  smodel->AddWithRefs(AAA);
  smodel->SetIdentLabel(AAA, smodel->Number(AAA));

  Handle(StepAP214_AppliedSecurityClassificationAssignment) ASCA =
    new StepAP214_AppliedSecurityClassificationAssignment;
  Handle(StepAP214_HArray1OfSecurityClassificationItem) HASCI =
    new StepAP214_HArray1OfSecurityClassificationItem(1,1);
  StepAP214_SecurityClassificationItem SCI;
  SCI.SetValue(AnaPDF);
  HASCI->SetValue(1,SCI);
  ASCA->Init(SC,HASCI);
  smodel->AddWithRefs(ASCA);
  smodel->SetIdentLabel(ASCA, smodel->Number(ASCA));

  OSD_Process sys;
  Quantity_Date date = sys.SystemDate ();

  Handle(StepBasic_CalendarDate) CDate = new StepBasic_CalendarDate;
  CDate->Init(date.Year(), date.Day(), date.Month());
  smodel->AddEntity(CDate);
  smodel->SetIdentLabel(CDate, smodel->Number(CDate));
  Handle(StepBasic_CoordinatedUniversalTimeOffset) CUTO = new StepBasic_CoordinatedUniversalTimeOffset;
  CUTO->Init(0, Standard_True, 0, StepBasic_aobAhead);
  smodel->AddEntity(CUTO);
  smodel->SetIdentLabel(CUTO, smodel->Number(CUTO));
  Handle(StepBasic_LocalTime) LT = new StepBasic_LocalTime;
  LT->Init(date.Hour(), Standard_True, date.Minute(), Standard_True,
           (Standard_Real)date.Second(), CUTO);
  smodel->AddWithRefs(LT);
  smodel->SetIdentLabel(LT, smodel->Number(LT));
  Handle(StepBasic_DateAndTime) DAT = new StepBasic_DateAndTime;
  DAT->Init(CDate,LT);
  smodel->AddWithRefs(DAT);
  smodel->SetIdentLabel(DAT, smodel->Number(DAT));

  Handle(StepBasic_DateTimeRole) DTR = new StepBasic_DateTimeRole;
  DTR->Init(new TCollection_HAsciiString("classification_date"));
  smodel->AddEntity(DTR);
  smodel->SetIdentLabel(DTR, smodel->Number(DTR));
  Handle(StepAP214_AppliedDateAndTimeAssignment) ADTA = new StepAP214_AppliedDateAndTimeAssignment;
  Handle(StepAP214_HArray1OfDateAndTimeItem) HADTI = new StepAP214_HArray1OfDateAndTimeItem(1,1);
  StepAP214_DateAndTimeItem DTI1;
  DTI1.SetValue(SC);
  HADTI->SetValue(1,DTI1);
  ADTA->Init(DAT,DTR,HADTI);
  smodel->AddWithRefs(ADTA);
  smodel->SetIdentLabel(ADTA, smodel->Number(ADTA));

  DTR = new StepBasic_DateTimeRole;
  DTR->Init(new TCollection_HAsciiString("creation_date"));
  smodel->AddEntity(DTR);
  smodel->SetIdentLabel(DTR, smodel->Number(DTR));
  ADTA = new StepAP214_AppliedDateAndTimeAssignment;
  HADTI = new StepAP214_HArray1OfDateAndTimeItem(1,1);
  StepAP214_DateAndTimeItem DTI2;
  DTI2.SetValue(AnaPD);
  HADTI->SetValue(1,DTI2);
  ADTA->Init(DAT,DTR,HADTI);
  smodel->AddWithRefs(ADTA);
  smodel->SetIdentLabel(ADTA, smodel->Number(ADTA));

  Handle(StepBasic_ApprovalDateTime) ADT = new StepBasic_ApprovalDateTime;
  StepBasic_DateTimeSelect DTS;
  DTS.SetValue(DAT);
  ADT->Init(DTS,Appr);
  smodel->AddWithRefs(ADT);
  smodel->SetIdentLabel(ADT, smodel->Number(ADT));

  Handle(StepBasic_Person) Pers = new StepBasic_Person;
  Handle(Interface_HArray1OfHAsciiString) HAHAS = new Interface_HArray1OfHAsciiString(1,1);
  HAHAS->SetValue(1,new TCollection_HAsciiString(""));
  Pers->Init(new TCollection_HAsciiString("1"), Standard_True,
             new TCollection_HAsciiString("last_name"), Standard_True,
             new TCollection_HAsciiString("first_name"), Standard_True,
             HAHAS, Standard_True, HAHAS, Standard_True, HAHAS);
  smodel->AddEntity(Pers);
  smodel->SetIdentLabel(Pers, smodel->Number(Pers));
  Handle(StepBasic_Organization) Org = new StepBasic_Organization;
  Org->Init(Standard_True, new TCollection_HAsciiString("1"),
            new TCollection_HAsciiString("organisation"),
            new TCollection_HAsciiString("organisation_description"));
  smodel->AddEntity(Org);
  smodel->SetIdentLabel(Org, smodel->Number(Org));
  Handle(StepBasic_PersonAndOrganization) PO = new StepBasic_PersonAndOrganization;
  PO->Init(Pers,Org);
  smodel->AddWithRefs(PO);
  smodel->SetIdentLabel(PO, smodel->Number(PO));

  Handle(StepBasic_PersonAndOrganizationRole) POR = new StepBasic_PersonAndOrganizationRole;
  POR->Init(new TCollection_HAsciiString("analysis_owner"));
  smodel->AddEntity(POR);
  smodel->SetIdentLabel(POR, smodel->Number(POR));
  Handle(StepAP214_AppliedPersonAndOrganizationAssignment) APOA =
    new StepAP214_AppliedPersonAndOrganizationAssignment;
  Handle(StepAP214_HArray1OfPersonAndOrganizationItem) HAPOI =
    new StepAP214_HArray1OfPersonAndOrganizationItem(1,1);
  StepAP214_PersonAndOrganizationItem POI1;
  POI1.SetValue(AnaProd);
  HAPOI->SetValue(1,POI1);
  APOA->Init(PO,POR,HAPOI);
  smodel->AddWithRefs(APOA);
  smodel->SetIdentLabel(APOA, smodel->Number(APOA));

  POR = new StepBasic_PersonAndOrganizationRole;
  POR->Init(new TCollection_HAsciiString("creator"));
  smodel->AddEntity(POR);
  smodel->SetIdentLabel(POR, smodel->Number(POR));
  APOA = new StepAP214_AppliedPersonAndOrganizationAssignment;
  HAPOI = new StepAP214_HArray1OfPersonAndOrganizationItem(1,1);
  StepAP214_PersonAndOrganizationItem POI2;
  POI2.SetValue(AnaPD);
  HAPOI->SetValue(1,POI2);
  APOA->Init(PO,POR,HAPOI);
  smodel->AddWithRefs(APOA);
  smodel->SetIdentLabel(APOA, smodel->Number(APOA));

  POR = new StepBasic_PersonAndOrganizationRole;
  POR->Init(new TCollection_HAsciiString("analysis_owner"));
  smodel->AddEntity(POR);
  smodel->SetIdentLabel(POR, smodel->Number(POR));
  APOA = new StepAP214_AppliedPersonAndOrganizationAssignment;
  HAPOI = new StepAP214_HArray1OfPersonAndOrganizationItem(1,1);
  StepAP214_PersonAndOrganizationItem POI3;
  POI3.SetValue(AnaPD);
  HAPOI->SetValue(1,POI3);
  APOA->Init(PO,POR,HAPOI);
  smodel->AddWithRefs(APOA);
  smodel->SetIdentLabel(APOA, smodel->Number(APOA));

  POR = new StepBasic_PersonAndOrganizationRole;
  POR->Init(new TCollection_HAsciiString("classification_officer"));
  smodel->AddEntity(POR);
  smodel->SetIdentLabel(POR, smodel->Number(POR));
  APOA = new StepAP214_AppliedPersonAndOrganizationAssignment;
  HAPOI = new StepAP214_HArray1OfPersonAndOrganizationItem(1,1);
  StepAP214_PersonAndOrganizationItem POI4;
  POI4.SetValue(SC);
  HAPOI->SetValue(1,POI4);
  APOA->Init(PO,POR,HAPOI);
  smodel->AddWithRefs(APOA);
  smodel->SetIdentLabel(APOA, smodel->Number(APOA));

  POR = new StepBasic_PersonAndOrganizationRole;
  POR->Init(new TCollection_HAsciiString("creator"));
  smodel->AddEntity(POR);
  smodel->SetIdentLabel(POR, smodel->Number(POR));
  APOA = new StepAP214_AppliedPersonAndOrganizationAssignment;
  HAPOI = new StepAP214_HArray1OfPersonAndOrganizationItem(1,1);
  StepAP214_PersonAndOrganizationItem POI5;
  POI5.SetValue(AnaPDF);
  HAPOI->SetValue(1,POI5);
  APOA->Init(PO,POR,HAPOI);
  smodel->AddWithRefs(APOA);
  smodel->SetIdentLabel(APOA, smodel->Number(APOA));

  Handle(StepBasic_ApprovalRole) AR = new StepBasic_ApprovalRole;
  AR->Init(new TCollection_HAsciiString("approver"));
  smodel->AddEntity(AR);
  smodel->SetIdentLabel(AR, smodel->Number(AR));
  Handle(StepBasic_ApprovalPersonOrganization) APO = new StepBasic_ApprovalPersonOrganization;
  StepBasic_PersonOrganizationSelect POS;
  POS.SetValue(PO);
  APO->Init(POS,Appr,AR);
  smodel->AddWithRefs(APO);
  smodel->SetIdentLabel(APO, smodel->Number(APO));


  return Standard_True;
}


//=======================================================================
//function : CreateAP203Structure
//purpose  : 
//=======================================================================

Handle(StepData_StepModel) StepAP209_Construct::CreateAP203Structure() const
{
  Handle(StepData_StepModel) smodel = Handle(StepData_StepModel)::DownCast(Model());
  Handle(StepData_StepModel) nmodel;// = new StepData_StepModel;
  if(smodel.IsNull()) return nmodel;
  //nmodel->SetProtocol(smodel->Protocol());
  Handle(StepBasic_ProductDefinitionFormation) PDF;
  Handle(StepBasic_ProductDefinition) PD;
  Handle(StepRepr_ProductDefinitionShape) PDS;
  Handle(StepShape_ShapeDefinitionRepresentation) SDR;
  Standard_Integer nb = smodel->NbEntities();
  for(Standard_Integer i=1; i<=nb; i++) {
    if(smodel->Value(i)->IsKind(STANDARD_TYPE(StepShape_ShapeDefinitionRepresentation))) {
      SDR = Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(smodel->Value(i));
      PDS = Handle(StepRepr_ProductDefinitionShape)::DownCast(SDR->Definition().Value());
      if(PDS.IsNull()) continue;
      PD = Handle(StepBasic_ProductDefinition)::DownCast(PDS->Definition().Value());
      if(PD.IsNull()) continue;
      Handle(StepBasic_ProductDefinitionFormation) PDF1 = PD->Formation();
      if(IsDesing(PDF1)) {
        PDF = PDF1;
        i = nb;
      }
    }
  }
  if(PDF.IsNull()) return nmodel;
  nmodel = new StepData_StepModel;
  nmodel->SetProtocol(smodel->Protocol());
  
  Handle(StepBasic_Product) Prod = PDF->OfProduct();
  nmodel->AddWithRefs(Prod);

  // adding categories:
  Handle(StepBasic_HArray1OfProduct) HAProd = new StepBasic_HArray1OfProduct(1,1);
  HAProd->SetValue(1,Prod);
  Handle(StepBasic_ProductRelatedProductCategory) PRPC = new StepBasic_ProductRelatedProductCategory;
  PRPC->Init(new TCollection_HAsciiString("design"),
             Standard_True, Prod->Name(), HAProd); // may be Prod->Description() - ???
  nmodel->AddEntity(PRPC);
  Handle(StepBasic_ProductCategory) PCat = new StepBasic_ProductCategory;
  PCat->Init(new TCollection_HAsciiString("part"),
             Standard_True, Prod->Name()); // may be Prod->Description() - ???
  nmodel->AddEntity(PCat);
  //nmodel->SetIdentLabel(PCat, smodel->Number(PCat));
  Handle(StepBasic_ProductCategoryRelationship) PCR = new StepBasic_ProductCategoryRelationship;
  PCR->Init(new TCollection_HAsciiString(""), Standard_True, 
            Prod->Name(), PCat, PRPC); // may be Prod->Description() - ???
  nmodel->AddWithRefs(PCR);

  nmodel->AddWithRefs(PDF);
  nmodel->AddWithRefs(PD);

  // replacing contexts:
  Handle(StepBasic_ApplicationContext) ApplCtx;
  Handle(StepBasic_ProductContext) ProdCtx = Prod->FrameOfReferenceValue(1);
  if(!ProdCtx.IsNull()) {
    Handle(StepBasic_MechanicalContext) MechCtx = new StepBasic_MechanicalContext;
    MechCtx->Init(ProdCtx->Name(), ProdCtx->FrameOfReference(),
                  ProdCtx->DisciplineType());
    nmodel->ReplaceEntity(nmodel->Number(ProdCtx),MechCtx);
    Handle(StepBasic_HArray1OfProductContext) HAPC = new StepBasic_HArray1OfProductContext(1,1);
    HAPC->SetValue(1,MechCtx);
    Prod->SetFrameOfReference(HAPC);
    ApplCtx = MechCtx->FrameOfReference();
  }
  Handle(StepBasic_ProductDefinitionContext) PDCtx = PD->FrameOfReference();
  if(!PDCtx.IsNull()) {
    Handle(StepBasic_DesignContext) DesCtx = new StepBasic_DesignContext;
    DesCtx->Init(PDCtx->Name(), PDCtx->FrameOfReference(),
                 PDCtx->LifeCycleStage());
    nmodel->ReplaceEntity(nmodel->Number(PDCtx),DesCtx);
    PD->SetFrameOfReference(DesCtx);
    ApplCtx = DesCtx->FrameOfReference();
  }
  if(!ApplCtx.IsNull()) {
    Handle(StepBasic_ApplicationProtocolDefinition) APD;
    Interface_EntityIterator subs = Graph().Sharings(ApplCtx);
    for (subs.Start(); subs.More() && APD.IsNull(); subs.Next()) {
      APD = Handle(StepBasic_ApplicationProtocolDefinition)::DownCast(subs.Value());
      if(APD.IsNull()) continue;
      nmodel->AddWithRefs(APD);
    }
  }

  CreateAdding203Entities(PD,nmodel);
            
  // adding geometry part
  nmodel->AddWithRefs(SDR);

  // adding DimensionalExponents
  Handle(StepBasic_DimensionalExponents) DimExp = new StepBasic_DimensionalExponents;
  DimExp->Init(1.,0.,0.,0.,0.,0.,0.);
  nmodel->AddWithRefs(DimExp);
  DimExp = new StepBasic_DimensionalExponents;
  DimExp->Init(0.,0.,0.,0.,0.,0.,0.);
  nmodel->AddWithRefs(DimExp);

  // writing HeaderSection
  nmodel->ClearHeader();
  Handle(HeaderSection_FileName) FN = Handle(HeaderSection_FileName)::
    DownCast(smodel->HeaderEntity(STANDARD_TYPE(HeaderSection_FileName)));
  if(!FN.IsNull()) {
    FN->SetPreprocessorVersion(new TCollection_HAsciiString("AP209 -> SDRB Convertor"));
    nmodel->AddHeaderEntity(FN);
  }
  Handle(HeaderSection_FileSchema) FS = Handle(HeaderSection_FileSchema)::
    DownCast(smodel->HeaderEntity(STANDARD_TYPE(HeaderSection_FileSchema)));
  if(!FS.IsNull())
    nmodel->AddHeaderEntity(FS);
  Handle(HeaderSection_FileDescription) FD = Handle(HeaderSection_FileDescription)::
    DownCast(smodel->HeaderEntity(STANDARD_TYPE(HeaderSection_FileDescription)));
  if(!FD.IsNull()) {
    Handle(Interface_HArray1OfHAsciiString) HAAS = new Interface_HArray1OfHAsciiString(1,1);
    HAAS->SetValue(1,new TCollection_HAsciiString("STEP AP203 file generated from STEP AP209"));
    FD->SetDescription(HAAS);
    nmodel->AddHeaderEntity(FD);
  }

//  WS()->SetModel(nmodel);

  return nmodel;
}


//=======================================================================
//function : CreateAdding203Entities
//purpose  : create approval.. , date.. , time.. , person.. and
//           organization.. entities for analysis structure
//=======================================================================

Standard_Boolean StepAP209_Construct::CreateAdding203Entities
  (const Handle(StepBasic_ProductDefinition) &PD,
   Handle(StepData_StepModel) &aModel) const
{
  Handle(StepData_StepModel) smodel = Handle(StepData_StepModel)::DownCast(Model());
  Handle(StepBasic_ProductDefinitionFormation) PDF = PD->Formation();
  Handle(StepBasic_Product) Prod = PDF->OfProduct();

  // create SecurityClassification
  Handle(StepBasic_SecurityClassification) SC;
  Interface_EntityIterator subs = Graph().Sharings(PDF);
  for (subs.Start(); subs.More() && SC.IsNull(); subs.Next()) {
    Handle(StepAP214_AppliedSecurityClassificationAssignment) ASCA =
      Handle(StepAP214_AppliedSecurityClassificationAssignment)::DownCast(subs.Value());
    if(ASCA.IsNull()) continue;
    SC = ASCA->AssignedSecurityClassification();
  }
  if(SC.IsNull()) {
    // create new
    Handle(StepBasic_SecurityClassificationLevel) SCL = new StepBasic_SecurityClassificationLevel;
    SCL->Init(new TCollection_HAsciiString("unclassified"));
    SC = new StepBasic_SecurityClassification;
    SC->Init(new TCollection_HAsciiString(""), new TCollection_HAsciiString(""), SCL);
  }
  aModel->AddWithRefs(SC);
  Handle(StepAP203_CcDesignSecurityClassification) DSC =
    new StepAP203_CcDesignSecurityClassification;
  Handle(StepAP203_HArray1OfClassifiedItem) HACI = new StepAP203_HArray1OfClassifiedItem(1,1);
  StepAP203_ClassifiedItem CI;
  CI.SetValue(PDF);
  HACI->SetValue(1,CI);
  DSC->Init(SC,HACI);
  aModel->AddWithRefs(DSC);

  // create CcDesignApproval
  Handle(StepBasic_DateAndTime) DT; 
  subs = Graph().Sharings(PD);
  for (subs.Start(); subs.More(); subs.Next()) {
    Handle(StepAP214_AppliedApprovalAssignment) AAA =
      Handle(StepAP214_AppliedApprovalAssignment)::DownCast(subs.Value());
    if(!AAA.IsNull()) {
      Handle(StepAP214_HArray1OfApprovalItem) HAAI214 = AAA->Items();
      Handle(StepAP203_HArray1OfApprovedItem) HAAI =
        new StepAP203_HArray1OfApprovedItem(1,HAAI214->Length());
      for(Standard_Integer i=1; i<=HAAI214->Length(); i++) {
        StepAP203_ApprovedItem AI;
        AI.SetValue(AAA->ItemsValue(i).Value());
        HAAI->SetValue(i,AI);
      }
      Handle(StepAP203_CcDesignApproval) DA = new StepAP203_CcDesignApproval;
      DA->Init(AAA->AssignedApproval(),HAAI);
      aModel->AddWithRefs(DA);
      // find ApprovalDateTime for Approval
      Interface_EntityIterator subs2 = Graph().Sharings(AAA->AssignedApproval());
      for (subs2.Start(); subs2.More(); subs2.Next()) {
        Handle(StepBasic_ApprovalDateTime) ADT =
          Handle(StepBasic_ApprovalDateTime)::DownCast(subs2.Value());
        if(ADT.IsNull()) continue;
        aModel->AddWithRefs(ADT);
        Handle(StepBasic_DateAndTime) DT1 =
          Handle(StepBasic_DateAndTime)::DownCast(ADT->DateTime().Value());
        if(DT1.IsNull()) continue;
        DT = DT1;
      }
    }
  }
  subs = Graph().Sharings(PDF);
  for (subs.Start(); subs.More(); subs.Next()) {
    Handle(StepAP214_AppliedApprovalAssignment) AAA =
      Handle(StepAP214_AppliedApprovalAssignment)::DownCast(subs.Value());
    if(!AAA.IsNull()) {
      Handle(StepAP214_HArray1OfApprovalItem) HAAI214 = AAA->Items();
      Handle(StepAP203_HArray1OfApprovedItem) HAAI =
        new StepAP203_HArray1OfApprovedItem(1,HAAI214->Length());
      for(Standard_Integer i=1; i<=HAAI214->Length(); i++) {
        StepAP203_ApprovedItem AI;
        AI.SetValue(AAA->ItemsValue(i).Value());
        HAAI->SetValue(i,AI);
      }
      Handle(StepAP203_CcDesignApproval) DA = new StepAP203_CcDesignApproval;
      DA->Init(AAA->AssignedApproval(),HAAI);
      aModel->AddWithRefs(DA);
      // find ApprovalDateTime for Approval
      Interface_EntityIterator subs2 = Graph().Sharings(AAA->AssignedApproval());
      for (subs2.Start(); subs2.More(); subs2.Next()) {
        Handle(StepBasic_ApprovalDateTime) ADT =
          Handle(StepBasic_ApprovalDateTime)::DownCast(subs2.Value());
        if(ADT.IsNull()) continue;
        aModel->AddWithRefs(ADT);
        Handle(StepBasic_DateAndTime) DT1 =
          Handle(StepBasic_DateAndTime)::DownCast(ADT->DateTime().Value());
        if(DT1.IsNull()) continue;
        DT = DT1;
      }
    }
  }
  subs = Graph().Sharings(SC);
  for (subs.Start(); subs.More(); subs.Next()) {
    Handle(StepAP214_AppliedApprovalAssignment) AAA =
      Handle(StepAP214_AppliedApprovalAssignment)::DownCast(subs.Value());
    if(!AAA.IsNull()) {
      Handle(StepAP214_HArray1OfApprovalItem) HAAI214 = AAA->Items();
      Handle(StepAP203_HArray1OfApprovedItem) HAAI =
        new StepAP203_HArray1OfApprovedItem(1,HAAI214->Length());
      for(Standard_Integer i=1; i<=HAAI214->Length(); i++) {
        StepAP203_ApprovedItem AI;
        AI.SetValue(AAA->ItemsValue(i).Value());
        HAAI->SetValue(i,AI);
      }
      Handle(StepAP203_CcDesignApproval) DA = new StepAP203_CcDesignApproval;
      DA->Init(AAA->AssignedApproval(),HAAI);
      aModel->AddWithRefs(DA);
      // find ApprovalDateTime for Approval
      Interface_EntityIterator subs2 = Graph().Sharings(AAA->AssignedApproval());
      for (subs2.Start(); subs2.More(); subs2.Next()) {
        Handle(StepBasic_ApprovalDateTime) ADT =
          Handle(StepBasic_ApprovalDateTime)::DownCast(subs2.Value());
        if(ADT.IsNull()) continue;
        aModel->AddWithRefs(ADT);
        Handle(StepBasic_DateAndTime) DT1 =
          Handle(StepBasic_DateAndTime)::DownCast(ADT->DateTime().Value());
        if(DT1.IsNull()) continue;
        DT = DT1;
      }
    }
  }

  if(aModel->Number(DT)>0) {
    // create CcDesignDateAndTimeAssignment
    subs = Graph().Sharings(DT);
    for (subs.Start(); subs.More(); subs.Next()) {
      Handle(StepAP214_AppliedDateAndTimeAssignment)ADTA =
        Handle(StepAP214_AppliedDateAndTimeAssignment)::DownCast(subs.Value());
      if(ADTA.IsNull()) continue;
      Handle(StepAP214_HArray1OfDateAndTimeItem) HADTI214 = ADTA->Items();
      Handle(StepAP203_HArray1OfDateTimeItem) HADTI =
        new StepAP203_HArray1OfDateTimeItem(1,HADTI214->Length());
      for(Standard_Integer i=1; i<=HADTI214->Length(); i++) {
        StepAP203_DateTimeItem DTI;
        DTI.SetValue(ADTA->ItemsValue(i).Value());
        HADTI->SetValue(i,DTI);
      }
      Handle(StepAP203_CcDesignDateAndTimeAssignment) DDTA =
        new StepAP203_CcDesignDateAndTimeAssignment;
      DDTA->Init(DT, ADTA->Role(), HADTI);
      aModel->AddWithRefs(DDTA);
    }
  }

  // create Person.. and Organization.. entities
  subs = Graph().Sharings(Prod);
  for (subs.Start(); subs.More(); subs.Next()) {
    Handle(StepAP214_AppliedPersonAndOrganizationAssignment) APOA =
      Handle(StepAP214_AppliedPersonAndOrganizationAssignment)::DownCast(subs.Value());
    if(APOA.IsNull()) continue;
    Handle(StepAP214_HArray1OfPersonAndOrganizationItem) HAPOI214 = APOA->Items();
    Handle(StepAP203_HArray1OfPersonOrganizationItem) HAPOI =
      new StepAP203_HArray1OfPersonOrganizationItem(1,HAPOI214->Length());
    for(Standard_Integer i=1; i<=HAPOI214->Length(); i++) {
      StepAP203_PersonOrganizationItem POI;
      POI.SetValue(HAPOI214->Value(i).Value());
      HAPOI->SetValue(i,POI);
    }
    Handle(StepAP203_CcDesignPersonAndOrganizationAssignment) DPOA =
      new StepAP203_CcDesignPersonAndOrganizationAssignment;
    DPOA->Init(APOA->AssignedPersonAndOrganization(), APOA->Role(), HAPOI);
    aModel->AddWithRefs(DPOA);
  }
  subs = Graph().Sharings(PD);
  for (subs.Start(); subs.More(); subs.Next()) {
    Handle(StepAP214_AppliedPersonAndOrganizationAssignment) APOA =
      Handle(StepAP214_AppliedPersonAndOrganizationAssignment)::DownCast(subs.Value());
    if(APOA.IsNull()) continue;
    Handle(StepAP214_HArray1OfPersonAndOrganizationItem) HAPOI214 = APOA->Items();
    Handle(StepAP203_HArray1OfPersonOrganizationItem) HAPOI =
      new StepAP203_HArray1OfPersonOrganizationItem(1,HAPOI214->Length());
    for(Standard_Integer i=1; i<=HAPOI214->Length(); i++) {
      StepAP203_PersonOrganizationItem POI;
      POI.SetValue(HAPOI214->Value(i).Value());
      HAPOI->SetValue(i,POI);
    }
    Handle(StepAP203_CcDesignPersonAndOrganizationAssignment) DPOA =
      new StepAP203_CcDesignPersonAndOrganizationAssignment;
    DPOA->Init(APOA->AssignedPersonAndOrganization(), APOA->Role(), HAPOI);
    aModel->AddWithRefs(DPOA);
  }
  subs = Graph().Sharings(PDF);
  for (subs.Start(); subs.More(); subs.Next()) {
    Handle(StepAP214_AppliedPersonAndOrganizationAssignment) APOA =
      Handle(StepAP214_AppliedPersonAndOrganizationAssignment)::DownCast(subs.Value());
    if(APOA.IsNull()) continue;
    Handle(StepAP214_HArray1OfPersonAndOrganizationItem) HAPOI214 = APOA->Items();
    Handle(StepAP203_HArray1OfPersonOrganizationItem) HAPOI =
      new StepAP203_HArray1OfPersonOrganizationItem(1,HAPOI214->Length());
    for(Standard_Integer i=1; i<=HAPOI214->Length(); i++) {
      StepAP203_PersonOrganizationItem POI;
      POI.SetValue(HAPOI214->Value(i).Value());
      HAPOI->SetValue(i,POI);
    }
    Handle(StepAP203_CcDesignPersonAndOrganizationAssignment) DPOA =
      new StepAP203_CcDesignPersonAndOrganizationAssignment;
    DPOA->Init(APOA->AssignedPersonAndOrganization(), APOA->Role(), HAPOI);
    aModel->AddWithRefs(DPOA);
  }
  subs = Graph().Sharings(SC);
  for (subs.Start(); subs.More(); subs.Next()) {
    Handle(StepAP214_AppliedPersonAndOrganizationAssignment) APOA =
      Handle(StepAP214_AppliedPersonAndOrganizationAssignment)::DownCast(subs.Value());
    if(APOA.IsNull()) continue;
    Handle(StepAP214_HArray1OfPersonAndOrganizationItem) HAPOI214 = APOA->Items();
    Handle(StepAP203_HArray1OfPersonOrganizationItem) HAPOI =
      new StepAP203_HArray1OfPersonOrganizationItem(1,HAPOI214->Length());
    for(Standard_Integer i=1; i<=HAPOI214->Length(); i++) {
      StepAP203_PersonOrganizationItem POI;
      POI.SetValue(HAPOI214->Value(i).Value());
      HAPOI->SetValue(i,POI);
    }
    Handle(StepAP203_CcDesignPersonAndOrganizationAssignment) DPOA =
      new StepAP203_CcDesignPersonAndOrganizationAssignment;
    DPOA->Init(APOA->AssignedPersonAndOrganization(), APOA->Role(), HAPOI);
    aModel->AddWithRefs(DPOA);
  }
  
  return Standard_True;
}

