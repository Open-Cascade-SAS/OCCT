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


#include <StepVisual_PresentationSizeAssignmentSelect.ixx>
#include <Interface_Macros.hxx>

StepVisual_PresentationSizeAssignmentSelect::StepVisual_PresentationSizeAssignmentSelect () {  }

Standard_Integer StepVisual_PresentationSizeAssignmentSelect::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_PresentationView))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_PresentationArea))) return 2;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_AreaInSet))) return 3;
	return 0;
}

Handle(StepVisual_PresentationView) StepVisual_PresentationSizeAssignmentSelect::PresentationView () const
{
	return GetCasted(StepVisual_PresentationView,Value());
}

Handle(StepVisual_PresentationArea) StepVisual_PresentationSizeAssignmentSelect::PresentationArea () const
{
	return GetCasted(StepVisual_PresentationArea,Value());
}

Handle(StepVisual_AreaInSet) StepVisual_PresentationSizeAssignmentSelect::AreaInSet () const
{
	return GetCasted(StepVisual_AreaInSet,Value());
}
