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
#include <StepRepr_Representation.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepVisual_PresentationSet.hxx>
#include <StepVisual_StyleContextSelect.hxx>

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
