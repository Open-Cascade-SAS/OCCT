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
#include <StepRepr_ProductDefinitionShape.hxx>
#include <StepRepr_ShapeAspect.hxx>
#include <StepRepr_ShapeAspectRelationship.hxx>
#include <StepRepr_ShapeDefinition.hxx>

StepRepr_ShapeDefinition::StepRepr_ShapeDefinition () {  }

Standard_Integer StepRepr_ShapeDefinition::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepRepr_ProductDefinitionShape))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepRepr_ShapeAspect))) return 2;
	if (ent->IsKind(STANDARD_TYPE(StepRepr_ShapeAspectRelationship))) return 3;
	return 0;
}

Handle(StepRepr_ProductDefinitionShape) StepRepr_ShapeDefinition::ProductDefinitionShape () const
{
	return GetCasted(StepRepr_ProductDefinitionShape,Value());
}

Handle(StepRepr_ShapeAspect) StepRepr_ShapeDefinition::ShapeAspect () const
{
	return GetCasted(StepRepr_ShapeAspect,Value());
}

Handle(StepRepr_ShapeAspectRelationship) StepRepr_ShapeDefinition::ShapeAspectRelationship () const
{
	return GetCasted(StepRepr_ShapeAspectRelationship,Value());
}
