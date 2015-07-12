// Created on: 2000-05-10
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

#ifndef _StepAP214_ExternalIdentificationItem_HeaderFile
#define _StepAP214_ExternalIdentificationItem_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepBasic_DocumentFile;
class StepAP214_ExternallyDefinedClass;
class StepAP214_ExternallyDefinedGeneralProperty;
class StepBasic_ProductDefinition;


//! Representation of STEP SELECT type ExternalIdentificationItem
class StepAP214_ExternalIdentificationItem  : public StepData_SelectType
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor
  Standard_EXPORT StepAP214_ExternalIdentificationItem();
  
  //! Recognizes a kind of ExternalIdentificationItem select type
  //! 1 -> DocumentFile from StepBasic
  //! 2 -> ExternallyDefinedClass from StepAP214
  //! 3 -> ExternallyDefinedGeneralProperty from StepAP214
  //! 4 -> ProductDefinition from StepBasic
  //! 0 else
  Standard_EXPORT Standard_Integer CaseNum (const Handle(Standard_Transient)& ent) const;
  
  //! Returns Value as DocumentFile (or Null if another type)
  Standard_EXPORT Handle(StepBasic_DocumentFile) DocumentFile() const;
  
  //! Returns Value as ExternallyDefinedClass (or Null if another type)
  Standard_EXPORT Handle(StepAP214_ExternallyDefinedClass) ExternallyDefinedClass() const;
  
  //! Returns Value as ExternallyDefinedGeneralProperty (or Null if another type)
  Standard_EXPORT Handle(StepAP214_ExternallyDefinedGeneralProperty) ExternallyDefinedGeneralProperty() const;
  
  //! Returns Value as ProductDefinition (or Null if another type)
  Standard_EXPORT Handle(StepBasic_ProductDefinition) ProductDefinition() const;




protected:





private:





};







#endif // _StepAP214_ExternalIdentificationItem_HeaderFile
