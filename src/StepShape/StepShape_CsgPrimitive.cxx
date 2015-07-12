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
#include <StepShape_Block.hxx>
#include <StepShape_CsgPrimitive.hxx>
#include <StepShape_RightAngularWedge.hxx>
#include <StepShape_RightCircularCone.hxx>
#include <StepShape_RightCircularCylinder.hxx>
#include <StepShape_Sphere.hxx>
#include <StepShape_Torus.hxx>

StepShape_CsgPrimitive::StepShape_CsgPrimitive () {  }

Standard_Integer StepShape_CsgPrimitive::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepShape_Sphere))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepShape_Block))) return 2;
	if (ent->IsKind(STANDARD_TYPE(StepShape_RightAngularWedge))) return 3;
	if (ent->IsKind(STANDARD_TYPE(StepShape_Torus))) return 4;
	if (ent->IsKind(STANDARD_TYPE(StepShape_RightCircularCone))) return 5;
	if (ent->IsKind(STANDARD_TYPE(StepShape_RightCircularCylinder))) return 6;
	return 0;
}

Handle(StepShape_Sphere) StepShape_CsgPrimitive::Sphere () const
{
	return GetCasted(StepShape_Sphere,Value());
}

Handle(StepShape_Block) StepShape_CsgPrimitive::Block () const
{
	return GetCasted(StepShape_Block,Value());
}

Handle(StepShape_RightAngularWedge) StepShape_CsgPrimitive::RightAngularWedge () const
{
	return GetCasted(StepShape_RightAngularWedge,Value());
}

Handle(StepShape_Torus) StepShape_CsgPrimitive::Torus () const
{
	return GetCasted(StepShape_Torus,Value());
}

Handle(StepShape_RightCircularCone) StepShape_CsgPrimitive::RightCircularCone () const
{
	return GetCasted(StepShape_RightCircularCone,Value());
}

Handle(StepShape_RightCircularCylinder) StepShape_CsgPrimitive::RightCircularCylinder () const
{
	return GetCasted(StepShape_RightCircularCylinder,Value());
}
