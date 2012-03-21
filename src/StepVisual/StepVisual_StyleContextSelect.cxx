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


#include <StepVisual_StyleContextSelect.ixx>
#include <Interface_Macros.hxx>

StepVisual_StyleContextSelect::StepVisual_StyleContextSelect () {  }

Standard_Integer StepVisual_StyleContextSelect::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepRepr_Representation))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepRepr_RepresentationItem))) return 2;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_PresentationSet))) return 3;
	return 0;
}

Handle(StepRepr_Representation) StepVisual_StyleContextSelect::Representation () const
{
	return GetCasted(StepRepr_Representation,Value());
}

Handle(StepRepr_RepresentationItem) StepVisual_StyleContextSelect::RepresentationItem () const
{
	return GetCasted(StepRepr_RepresentationItem,Value());
}

Handle(StepVisual_PresentationSet) StepVisual_StyleContextSelect::PresentationSet () const
{
	return GetCasted(StepVisual_PresentationSet,Value());
}
