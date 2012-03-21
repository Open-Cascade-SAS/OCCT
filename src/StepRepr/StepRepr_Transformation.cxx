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


#include <StepRepr_Transformation.ixx>
#include <Interface_Macros.hxx>

StepRepr_Transformation::StepRepr_Transformation () {  }

Standard_Integer StepRepr_Transformation::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepRepr_ItemDefinedTransformation))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepRepr_FunctionallyDefinedTransformation))) return 2;
	return 0;
}

Handle(StepRepr_ItemDefinedTransformation) StepRepr_Transformation::ItemDefinedTransformation () const
{
	return GetCasted(StepRepr_ItemDefinedTransformation,Value());
}

Handle(StepRepr_FunctionallyDefinedTransformation) StepRepr_Transformation::FunctionallyDefinedTransformation () const
{
	return GetCasted(StepRepr_FunctionallyDefinedTransformation,Value());
}
