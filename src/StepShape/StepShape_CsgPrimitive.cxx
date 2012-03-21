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


#include <StepShape_CsgPrimitive.ixx>
#include <Interface_Macros.hxx>

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
