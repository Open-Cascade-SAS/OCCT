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


#include <StepVisual_PresentationRepresentationSelect.ixx>
#include <Interface_Macros.hxx>

StepVisual_PresentationRepresentationSelect::StepVisual_PresentationRepresentationSelect () {  }

Standard_Integer StepVisual_PresentationRepresentationSelect::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_PresentationRepresentation))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_PresentationSet))) return 2;
	return 0;
}

Handle(StepVisual_PresentationRepresentation) StepVisual_PresentationRepresentationSelect::PresentationRepresentation () const
{
	return GetCasted(StepVisual_PresentationRepresentation,Value());
}

Handle(StepVisual_PresentationSet) StepVisual_PresentationRepresentationSelect::PresentationSet () const
{
	return GetCasted(StepVisual_PresentationSet,Value());
}
