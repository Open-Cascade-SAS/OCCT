// Created on: 2000-05-10
// Created by: Andrey BETENEV
// Copyright (c) 2000-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

#include <StepAP214_ExternalIdentificationItem.ixx>

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
