// Created on: 1999-03-10
// Created by: data exchange team
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

#ifndef _StepAP214_PresentedItemSelect_HeaderFile
#define _StepAP214_PresentedItemSelect_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepBasic_ProductDefinitionRelationship;
class StepBasic_ProductDefinition;



class StepAP214_PresentedItemSelect  : public StepData_SelectType
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns a PresentedItemSelect SelectType
  Standard_EXPORT StepAP214_PresentedItemSelect();
  
  //! Recognizes a PresentedItemSelect Kind Entity that is :
  //! 1 -> ProductDefinition,
  //! 2 -> ProductDefinitionRelationship,
  //! 0 else
  Standard_EXPORT Standard_Integer CaseNum (const Handle(Standard_Transient)& ent) const;
  
  //! returns Value as a ProductDefinitionRelationship (Null if another type)
  Standard_EXPORT Handle(StepBasic_ProductDefinitionRelationship) ProductDefinitionRelationship() const;
  
  //! returns Value as a ProductDefinition (Null if another type)
  Standard_EXPORT Handle(StepBasic_ProductDefinition) ProductDefinition() const;




protected:





private:





};







#endif // _StepAP214_PresentedItemSelect_HeaderFile
