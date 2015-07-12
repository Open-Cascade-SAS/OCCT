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
#include <StepVisual_AnnotationText.hxx>
#include <StepVisual_CompositeText.hxx>
#include <StepVisual_TextLiteral.hxx>
#include <StepVisual_TextOrCharacter.hxx>

StepVisual_TextOrCharacter::StepVisual_TextOrCharacter () {  }

Standard_Integer StepVisual_TextOrCharacter::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_AnnotationText))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_CompositeText))) return 2;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_TextLiteral))) return 3;
	return 0;
}

Handle(StepVisual_AnnotationText) StepVisual_TextOrCharacter::AnnotationText () const
{
	return GetCasted(StepVisual_AnnotationText,Value());
}

Handle(StepVisual_CompositeText) StepVisual_TextOrCharacter::CompositeText () const
{
	return GetCasted(StepVisual_CompositeText,Value());
}

Handle(StepVisual_TextLiteral) StepVisual_TextOrCharacter::TextLiteral () const
{
	return GetCasted(StepVisual_TextLiteral,Value());
}
