// Created on: 2000-07-03
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

#ifndef _StepRepr_QuantifiedAssemblyComponentUsage_HeaderFile
#define _StepRepr_QuantifiedAssemblyComponentUsage_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_AssemblyComponentUsage.hxx>
#include <Standard_Boolean.hxx>
class StepBasic_MeasureWithUnit;
class TCollection_HAsciiString;
class StepBasic_ProductDefinition;


class StepRepr_QuantifiedAssemblyComponentUsage;
DEFINE_STANDARD_HANDLE(StepRepr_QuantifiedAssemblyComponentUsage, StepRepr_AssemblyComponentUsage)

//! Representation of STEP entity QuantifiedAssemblyComponentUsage
class StepRepr_QuantifiedAssemblyComponentUsage : public StepRepr_AssemblyComponentUsage
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepRepr_QuantifiedAssemblyComponentUsage();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aProductDefinitionRelationship_Id, const Handle(TCollection_HAsciiString)& aProductDefinitionRelationship_Name, const Standard_Boolean hasProductDefinitionRelationship_Description, const Handle(TCollection_HAsciiString)& aProductDefinitionRelationship_Description, const Handle(StepBasic_ProductDefinition)& aProductDefinitionRelationship_RelatingProductDefinition, const Handle(StepBasic_ProductDefinition)& aProductDefinitionRelationship_RelatedProductDefinition, const Standard_Boolean hasAssemblyComponentUsage_ReferenceDesignator, const Handle(TCollection_HAsciiString)& aAssemblyComponentUsage_ReferenceDesignator, const Handle(StepBasic_MeasureWithUnit)& aQuantity);

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aProductDefinitionRelationship_Id, const Handle(TCollection_HAsciiString)& aProductDefinitionRelationship_Name, const Standard_Boolean hasProductDefinitionRelationship_Description, const Handle(TCollection_HAsciiString)& aProductDefinitionRelationship_Description, const StepBasic_ProductDefinitionOrReference& aProductDefinitionRelationship_RelatingProductDefinition, const StepBasic_ProductDefinitionOrReference& aProductDefinitionRelationship_RelatedProductDefinition, const Standard_Boolean hasAssemblyComponentUsage_ReferenceDesignator, const Handle(TCollection_HAsciiString)& aAssemblyComponentUsage_ReferenceDesignator, const Handle(StepBasic_MeasureWithUnit)& aQuantity);
  
  //! Returns field Quantity
  Standard_EXPORT Handle(StepBasic_MeasureWithUnit) Quantity() const;
  
  //! Set field Quantity
  Standard_EXPORT void SetQuantity (const Handle(StepBasic_MeasureWithUnit)& Quantity);




  DEFINE_STANDARD_RTTIEXT(StepRepr_QuantifiedAssemblyComponentUsage,StepRepr_AssemblyComponentUsage)

protected:




private:


  Handle(StepBasic_MeasureWithUnit) theQuantity;


};







#endif // _StepRepr_QuantifiedAssemblyComponentUsage_HeaderFile
