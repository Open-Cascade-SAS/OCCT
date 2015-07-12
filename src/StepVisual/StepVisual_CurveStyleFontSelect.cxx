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
#include <StepVisual_CurveStyleFont.hxx>
#include <StepVisual_CurveStyleFontSelect.hxx>
#include <StepVisual_ExternallyDefinedCurveFont.hxx>
#include <StepVisual_PreDefinedCurveFont.hxx>

StepVisual_CurveStyleFontSelect::StepVisual_CurveStyleFontSelect () {  }

Standard_Integer StepVisual_CurveStyleFontSelect::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_CurveStyleFont))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_PreDefinedCurveFont))) return 2;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_ExternallyDefinedCurveFont))) return 3;
	return 0;
}

Handle(StepVisual_CurveStyleFont) StepVisual_CurveStyleFontSelect::CurveStyleFont () const
{
	return GetCasted(StepVisual_CurveStyleFont,Value());
}

Handle(StepVisual_PreDefinedCurveFont) StepVisual_CurveStyleFontSelect::PreDefinedCurveFont () const
{
	return GetCasted(StepVisual_PreDefinedCurveFont,Value());
}

Handle(StepVisual_ExternallyDefinedCurveFont) StepVisual_CurveStyleFontSelect::ExternallyDefinedCurveFont () const
{
	return GetCasted(StepVisual_ExternallyDefinedCurveFont,Value());
}
