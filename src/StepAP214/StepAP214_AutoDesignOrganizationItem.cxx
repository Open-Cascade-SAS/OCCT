// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

#include <StepAP214_AutoDesignOrganizationItem.ixx>
#include <Interface_Macros.hxx>

StepAP214_AutoDesignOrganizationItem::StepAP214_AutoDesignOrganizationItem ()  {  }

Standard_Integer  StepAP214_AutoDesignOrganizationItem::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  Standard_Integer num = StepAP214_AutoDesignGeneralOrgItem::CaseNum(ent);
  if (num > 0) return num;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_Document))) return 9;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_PhysicallyModeledProductDefinition))) return 10;
  return 0;
}

Handle(StepBasic_Document)  StepAP214_AutoDesignOrganizationItem::Document () const
{  return GetCasted(StepBasic_Document,Value());  }

Handle(StepBasic_PhysicallyModeledProductDefinition)  StepAP214_AutoDesignOrganizationItem::PhysicallyModeledProductDefinition () const
{  return GetCasted(StepBasic_PhysicallyModeledProductDefinition,Value());  }
