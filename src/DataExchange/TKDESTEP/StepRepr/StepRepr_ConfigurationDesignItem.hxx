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

#ifndef _StepRepr_ConfigurationDesignItem_HeaderFile
#define _StepRepr_ConfigurationDesignItem_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepBasic_ProductDefinition;
class StepBasic_ProductDefinitionFormation;

//! Representation of STEP SELECT type ConfigurationDesignItem
class StepRepr_ConfigurationDesignItem : public StepData_SelectType
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT StepRepr_ConfigurationDesignItem();

  //! Recognizes a kind of ConfigurationDesignItem select type
  //! 1 -> ProductDefinition from StepBasic
  //! 2 -> ProductDefinitionFormation from StepBasic
  //! 0 else
  Standard_EXPORT Standard_Integer CaseNum(const Handle(Standard_Transient)& ent) const;

  //! Returns Value as ProductDefinition (or Null if another type)
  Standard_EXPORT Handle(StepBasic_ProductDefinition) ProductDefinition() const;

  //! Returns Value as ProductDefinitionFormation (or Null if another type)
  Standard_EXPORT Handle(StepBasic_ProductDefinitionFormation) ProductDefinitionFormation() const;

protected:
private:
};

#endif // _StepRepr_ConfigurationDesignItem_HeaderFile
