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
#include <StepAP214_AutoDesignOrganizationItem.hxx>
#include <StepBasic_Document.hxx>
#include <StepBasic_PhysicallyModeledProductDefinition.hxx>

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
