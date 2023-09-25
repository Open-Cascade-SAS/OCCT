// Created on: 1993-08-04
// Created by: Herve LEGRAND
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _STEPConstruct_Part_HeaderFile
#define _STEPConstruct_Part_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class StepShape_ShapeDefinitionRepresentation;
class StepBasic_ProductRelatedProductCategory;
class StepShape_ShapeRepresentation;
class TCollection_HAsciiString;
class StepBasic_ApplicationContext;
class StepBasic_ProductContext;
class StepBasic_ProductDefinitionContext;
class StepBasic_Product;
class StepBasic_ProductDefinitionFormation;
class StepBasic_ProductDefinition;
class StepData_StepModel;
class StepRepr_ProductDefinitionShape;


//! Provides tools for creating STEP structures associated
//! with part (SDR), such as PRODUCT, PDF etc., as
//! required by current schema
//! Also allows to investigate and modify this data
class STEPConstruct_Part 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT STEPConstruct_Part();
  
  Standard_EXPORT void MakeSDR (const Handle(StepShape_ShapeRepresentation)& aShape,
                                const Handle(TCollection_HAsciiString)& aName,
                                const Handle(StepBasic_ApplicationContext)& AC,
                                Handle(StepData_StepModel)& theStepModel);
  
  Standard_EXPORT void ReadSDR (const Handle(StepShape_ShapeDefinitionRepresentation)& aShape);
  
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns SDR or Null if not done
  Standard_EXPORT Handle(StepShape_ShapeDefinitionRepresentation) SDRValue() const;
  
  //! Returns SDR->UsedRepresentation() or Null if not done
  Standard_EXPORT Handle(StepShape_ShapeRepresentation) SRValue() const;
  
  Standard_EXPORT Handle(StepBasic_ProductContext) PC() const;
  
  Standard_EXPORT Handle(TCollection_HAsciiString) PCname() const;
  
  Standard_EXPORT Handle(TCollection_HAsciiString) PCdisciplineType() const;
  
  Standard_EXPORT void SetPCname (const Handle(TCollection_HAsciiString)& name);
  
  Standard_EXPORT void SetPCdisciplineType (const Handle(TCollection_HAsciiString)& label);
  
  Standard_EXPORT Handle(StepBasic_ApplicationContext) AC() const;
  
  Standard_EXPORT Handle(TCollection_HAsciiString) ACapplication() const;
  
  Standard_EXPORT void SetACapplication (const Handle(TCollection_HAsciiString)& text);
  
  Standard_EXPORT Handle(StepBasic_ProductDefinitionContext) PDC() const;
  
  Standard_EXPORT Handle(TCollection_HAsciiString) PDCname() const;
  
  Standard_EXPORT Handle(TCollection_HAsciiString) PDCstage() const;
  
  Standard_EXPORT void SetPDCname (const Handle(TCollection_HAsciiString)& label);
  
  Standard_EXPORT void SetPDCstage (const Handle(TCollection_HAsciiString)& label);
  
  Standard_EXPORT Handle(StepBasic_Product) Product() const;
  
  Standard_EXPORT Handle(TCollection_HAsciiString) Pid() const;
  
  Standard_EXPORT Handle(TCollection_HAsciiString) Pname() const;
  
  Standard_EXPORT Handle(TCollection_HAsciiString) Pdescription() const;
  
  Standard_EXPORT void SetPid (const Handle(TCollection_HAsciiString)& id);
  
  Standard_EXPORT void SetPname (const Handle(TCollection_HAsciiString)& label);
  
  Standard_EXPORT void SetPdescription (const Handle(TCollection_HAsciiString)& text);
  
  Standard_EXPORT Handle(StepBasic_ProductDefinitionFormation) PDF() const;
  
  Standard_EXPORT Handle(TCollection_HAsciiString) PDFid() const;
  
  Standard_EXPORT Handle(TCollection_HAsciiString) PDFdescription() const;
  
  Standard_EXPORT void SetPDFid (const Handle(TCollection_HAsciiString)& id);
  
  Standard_EXPORT void SetPDFdescription (const Handle(TCollection_HAsciiString)& text);
  
  Standard_EXPORT Handle(StepBasic_ProductDefinition) PD() const;
  
  Standard_EXPORT Handle(TCollection_HAsciiString) PDdescription() const;
  
  Standard_EXPORT void SetPDdescription (const Handle(TCollection_HAsciiString)& text);
  
  Standard_EXPORT Handle(StepRepr_ProductDefinitionShape) PDS() const;
  
  Standard_EXPORT Handle(TCollection_HAsciiString) PDSname() const;
  
  Standard_EXPORT Handle(TCollection_HAsciiString) PDSdescription() const;
  
  Standard_EXPORT void SetPDSname (const Handle(TCollection_HAsciiString)& label);
  
  Standard_EXPORT void SetPDSdescription (const Handle(TCollection_HAsciiString)& text);
  
  Standard_EXPORT Handle(StepBasic_ProductRelatedProductCategory) PRPC() const;
  
  Standard_EXPORT Handle(TCollection_HAsciiString) PRPCname() const;
  
  Standard_EXPORT Handle(TCollection_HAsciiString) PRPCdescription() const;
  
  Standard_EXPORT void SetPRPCname (const Handle(TCollection_HAsciiString)& label);
  
  Standard_EXPORT void SetPRPCdescription (const Handle(TCollection_HAsciiString)& text);




protected:





private:



  Standard_Boolean myDone;
  Handle(StepShape_ShapeDefinitionRepresentation) mySDR;
  Handle(StepBasic_ProductRelatedProductCategory) myPRPC;


};







#endif // _STEPConstruct_Part_HeaderFile
