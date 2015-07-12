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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

#include <Standard_Transient.hxx>
#include <StepAP214_ExternalIdentificationItem.hxx>
#include <StepAP214_ExternallyDefinedClass.hxx>
#include <StepAP214_ExternallyDefinedGeneralProperty.hxx>
#include <StepBasic_DocumentFile.hxx>
#include <StepBasic_ProductDefinition.hxx>

//=======================================================================
//function : StepAP214_ExternalIdentificationItem
//purpose  : 
//=======================================================================
StepAP214_ExternalIdentificationItem::StepAP214_ExternalIdentificationItem ()
{
}

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepAP214_ExternalIdentificationItem::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_DocumentFile))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepAP214_ExternallyDefinedClass))) return 2;
  if (ent->IsKind(STANDARD_TYPE(StepAP214_ExternallyDefinedGeneralProperty))) return 3;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinition))) return 4;
  return 0;
}

//=======================================================================
//function : DocumentFile
//purpose  : 
//=======================================================================

Handle(StepBasic_DocumentFile) StepAP214_ExternalIdentificationItem::DocumentFile () const
{
  return Handle(StepBasic_DocumentFile)::DownCast(Value());
}

//=======================================================================
//function : ExternallyDefinedClass
//purpose  : 
//=======================================================================

Handle(StepAP214_ExternallyDefinedClass) StepAP214_ExternalIdentificationItem::ExternallyDefinedClass () const
{
  return Handle(StepAP214_ExternallyDefinedClass)::DownCast(Value());
}

//=======================================================================
//function : ExternallyDefinedGeneralProperty
//purpose  : 
//=======================================================================

Handle(StepAP214_ExternallyDefinedGeneralProperty) StepAP214_ExternalIdentificationItem::ExternallyDefinedGeneralProperty () const
{
  return Handle(StepAP214_ExternallyDefinedGeneralProperty)::DownCast(Value());
}

//=======================================================================
//function : ProductDefinition
//purpose  : 
//=======================================================================

Handle(StepBasic_ProductDefinition) StepAP214_ExternalIdentificationItem::ProductDefinition () const
{
  return Handle(StepBasic_ProductDefinition)::DownCast(Value());
}
