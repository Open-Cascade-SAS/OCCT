// File:	StepAP214_ExternalIdentificationItem.cxx
// Created:	Wed May 10 15:09:05 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

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
