// Created on: 2002-12-06
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _StepAP209_Construct_HeaderFile
#define _StepAP209_Construct_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <STEPConstruct_Tool.hxx>
#include <StepElement_HSequenceOfElementMaterial.hxx>
#include <StepFEA_HSequenceOfElementGeometricRelationship.hxx>
#include <StepFEA_HSequenceOfElementRepresentation.hxx>
#include <Standard_Type.hxx>
#include <StepElement_HSequenceOfCurveElementSectionDefinition.hxx>
class XSControl_WorkSession;
class StepBasic_ProductDefinitionFormation;
class StepFEA_FeaModel;
class StepBasic_Product;
class StepFEA_FeaAxis2Placement3d;
class StepShape_ShapeRepresentation;
class StepFEA_Curve3dElementRepresentation;
class StepFEA_ElementRepresentation;
class StepBasic_ProductDefinition;
class StepData_StepModel;
class StepRepr_ProductDefinitionShape;

//! Basic tool for working with AP209 model
class StepAP209_Construct : public STEPConstruct_Tool
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an empty tool
  Standard_EXPORT StepAP209_Construct();

  //! Creates a tool and initializes it
  Standard_EXPORT StepAP209_Construct(const Handle(XSControl_WorkSession)& WS);

  //! Initializes tool; returns True if succeeded
  Standard_EXPORT Standard_Boolean Init(const Handle(XSControl_WorkSession)& WS);

  Standard_EXPORT Standard_Boolean
    IsDesing(const Handle(StepBasic_ProductDefinitionFormation)& PD) const;

  Standard_EXPORT Standard_Boolean
    IsAnalys(const Handle(StepBasic_ProductDefinitionFormation)& PD) const;

  Standard_EXPORT Handle(StepFEA_FeaModel) FeaModel(const Handle(StepBasic_Product)& Prod) const;

  Standard_EXPORT Handle(StepFEA_FeaModel) FeaModel(
    const Handle(StepBasic_ProductDefinitionFormation)& PDF) const;

  Standard_EXPORT Handle(StepFEA_FeaAxis2Placement3d) GetFeaAxis2Placement3d(
    const Handle(StepFEA_FeaModel)& theFeaModel) const;

  Standard_EXPORT Handle(StepShape_ShapeRepresentation) IdealShape(
    const Handle(StepBasic_Product)& Prod) const;

  Standard_EXPORT Handle(StepShape_ShapeRepresentation) IdealShape(
    const Handle(StepBasic_ProductDefinitionFormation)& PDF) const;

  Standard_EXPORT Handle(StepShape_ShapeRepresentation) NominShape(
    const Handle(StepBasic_Product)& Prod) const;

  Standard_EXPORT Handle(StepShape_ShapeRepresentation) NominShape(
    const Handle(StepBasic_ProductDefinitionFormation)& PDF) const;

  Standard_EXPORT Handle(StepElement_HSequenceOfElementMaterial) GetElementMaterial() const;

  Standard_EXPORT Handle(StepFEA_HSequenceOfElementGeometricRelationship) GetElemGeomRelat() const;

  Standard_EXPORT Handle(StepFEA_HSequenceOfElementRepresentation) GetElements1D(
    const Handle(StepFEA_FeaModel)& theFeaModel) const;

  Standard_EXPORT Handle(StepFEA_HSequenceOfElementRepresentation) GetElements2D(
    const Handle(StepFEA_FeaModel)& theFEAModel) const;

  Standard_EXPORT Handle(StepFEA_HSequenceOfElementRepresentation) GetElements3D(
    const Handle(StepFEA_FeaModel)& theFEAModel) const;

  //! Getting list of curve_element_section_definitions
  //! for given element_representation
  Standard_EXPORT Handle(StepElement_HSequenceOfCurveElementSectionDefinition) GetCurElemSection(
    const Handle(StepFEA_Curve3dElementRepresentation)& ElemRepr) const;

  Standard_EXPORT Handle(StepShape_ShapeRepresentation) GetShReprForElem(
    const Handle(StepFEA_ElementRepresentation)& ElemRepr) const;

  //! Create empty structure for idealized_analysis_shape
  Standard_EXPORT Standard_Boolean
    CreateAnalysStructure(const Handle(StepBasic_Product)& Prod) const;

  //! Create fea structure
  Standard_EXPORT Standard_Boolean CreateFeaStructure(const Handle(StepBasic_Product)& Prod) const;

  //! Put into model entities Applied... for AP209 instead of
  //! entities CcDesing... from AP203.
  Standard_EXPORT Standard_Boolean ReplaceCcDesingToApplied() const;

  //! Create approval.. , date.. , time.. , person.. and
  //! organization.. entities for analysis structure
  Standard_EXPORT Standard_Boolean
    CreateAddingEntities(const Handle(StepBasic_ProductDefinition)& AnaPD) const;

  //! Create AP203 structure from existing AP209 structure
  Standard_EXPORT Handle(StepData_StepModel) CreateAP203Structure() const;

  //! Create approval.. , date.. , time.. , person.. and
  //! organization.. entities for 203 structure
  Standard_EXPORT Standard_Boolean
    CreateAdding203Entities(const Handle(StepBasic_ProductDefinition)& PD,
                            Handle(StepData_StepModel)&                aModel) const;

  Standard_EXPORT Handle(StepFEA_FeaModel) FeaModel(
    const Handle(StepRepr_ProductDefinitionShape)& PDS) const;

  Standard_EXPORT Handle(StepFEA_FeaModel) FeaModel(
    const Handle(StepBasic_ProductDefinition)& PD) const;

  Standard_EXPORT Handle(StepShape_ShapeRepresentation) IdealShape(
    const Handle(StepBasic_ProductDefinition)& PD) const;

  Standard_EXPORT Handle(StepShape_ShapeRepresentation) IdealShape(
    const Handle(StepRepr_ProductDefinitionShape)& PDS) const;

protected:
  Standard_EXPORT Handle(StepFEA_HSequenceOfElementRepresentation) GetFeaElements(
    const Handle(StepFEA_FeaModel)& theFeaModel,
    const Handle(Standard_Type)&    theType) const;

private:
};

#endif // _StepAP209_Construct_HeaderFile
