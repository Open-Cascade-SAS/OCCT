// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <StepAP214_GroupItem.ixx>
#include <Interface_Macros.hxx>

StepAP214_GroupItem::StepAP214_GroupItem () {  }

Standard_Integer StepAP214_GroupItem::CaseNum(const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepGeom_GeometricRepresentationItem))) return 1;
  
  return 0;
}

Handle(StepGeom_GeometricRepresentationItem) StepAP214_GroupItem::GeometricRepresentationItem() const
{
  return GetCasted(StepGeom_GeometricRepresentationItem,Value());
}

