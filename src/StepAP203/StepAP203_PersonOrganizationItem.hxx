// Created on: 1999-11-26
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
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

#ifndef _StepAP203_PersonOrganizationItem_HeaderFile
#define _StepAP203_PersonOrganizationItem_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepAP203_Change;
class StepAP203_StartWork;
class StepAP203_ChangeRequest;
class StepAP203_StartRequest;
class StepRepr_ConfigurationItem;
class StepBasic_Product;
class StepBasic_ProductDefinitionFormation;
class StepBasic_ProductDefinition;
class StepBasic_Contract;
class StepBasic_SecurityClassification;


//! Representation of STEP SELECT type PersonOrganizationItem
class StepAP203_PersonOrganizationItem  : public StepData_SelectType
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor
  Standard_EXPORT StepAP203_PersonOrganizationItem();
  
  //! Recognizes a kind of PersonOrganizationItem select type
  //! 1 -> Change from StepAP203
  //! 2 -> StartWork from StepAP203
  //! 3 -> ChangeRequest from StepAP203
  //! 4 -> StartRequest from StepAP203
  //! 5 -> ConfigurationItem from StepRepr
  //! 6 -> Product from StepBasic
  //! 7 -> ProductDefinitionFormation from StepBasic
  //! 8 -> ProductDefinition from StepBasic
  //! 9 -> Contract from StepBasic
  //! 10 -> SecurityClassification from StepBasic
  //! 0 else
  Standard_EXPORT Standard_Integer CaseNum (const Handle(Standard_Transient)& ent) const;
  
  //! Returns Value as Change (or Null if another type)
  Standard_EXPORT Handle(StepAP203_Change) Change() const;
  
  //! Returns Value as StartWork (or Null if another type)
  Standard_EXPORT Handle(StepAP203_StartWork) StartWork() const;
  
  //! Returns Value as ChangeRequest (or Null if another type)
  Standard_EXPORT Handle(StepAP203_ChangeRequest) ChangeRequest() const;
  
  //! Returns Value as StartRequest (or Null if another type)
  Standard_EXPORT Handle(StepAP203_StartRequest) StartRequest() const;
  
  //! Returns Value as ConfigurationItem (or Null if another type)
  Standard_EXPORT Handle(StepRepr_ConfigurationItem) ConfigurationItem() const;
  
  //! Returns Value as Product (or Null if another type)
  Standard_EXPORT Handle(StepBasic_Product) Product() const;
  
  //! Returns Value as ProductDefinitionFormation (or Null if another type)
  Standard_EXPORT Handle(StepBasic_ProductDefinitionFormation) ProductDefinitionFormation() const;
  
  //! Returns Value as ProductDefinition (or Null if another type)
  Standard_EXPORT Handle(StepBasic_ProductDefinition) ProductDefinition() const;
  
  //! Returns Value as Contract (or Null if another type)
  Standard_EXPORT Handle(StepBasic_Contract) Contract() const;
  
  //! Returns Value as SecurityClassification (or Null if another type)
  Standard_EXPORT Handle(StepBasic_SecurityClassification) SecurityClassification() const;




protected:





private:





};







#endif // _StepAP203_PersonOrganizationItem_HeaderFile
