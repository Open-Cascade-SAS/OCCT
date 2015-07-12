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


#include <Interface_Macros.hxx>
#include <Standard_Transient.hxx>
#include <StepAP214_AutoDesignDateAndPersonAssignment.hxx>
#include <StepAP214_AutoDesignDatedItem.hxx>
#include <StepBasic_ApprovalPersonOrganization.hxx>
#include <StepBasic_ProductDefinitionEffectivity.hxx>

StepAP214_AutoDesignDatedItem::StepAP214_AutoDesignDatedItem () {  }

Standard_Integer StepAP214_AutoDesignDatedItem::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepBasic_ApprovalPersonOrganization))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepAP214_AutoDesignDateAndPersonAssignment))) return 2;
	if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionEffectivity))) return 3;
	return 0;
}

Handle(StepBasic_ApprovalPersonOrganization) StepAP214_AutoDesignDatedItem::ApprovalPersonOrganization () const
{
	return GetCasted(StepBasic_ApprovalPersonOrganization,Value());
}

Handle(StepAP214_AutoDesignDateAndPersonAssignment) StepAP214_AutoDesignDatedItem::AutoDesignDateAndPersonAssignment () const
{
	return GetCasted(StepAP214_AutoDesignDateAndPersonAssignment,Value());
}

Handle(StepBasic_ProductDefinitionEffectivity)  StepAP214_AutoDesignDatedItem::ProductDefinitionEffectivity () const
{
  return GetCasted(StepBasic_ProductDefinitionEffectivity,Value());
}
