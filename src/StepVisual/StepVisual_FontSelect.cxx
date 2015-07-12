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
#include <StepVisual_ExternallyDefinedTextFont.hxx>
#include <StepVisual_FontSelect.hxx>
#include <StepVisual_PreDefinedTextFont.hxx>

StepVisual_FontSelect::StepVisual_FontSelect () {  }

Standard_Integer StepVisual_FontSelect::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_PreDefinedTextFont))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_ExternallyDefinedTextFont))) return 2;
	return 0;
}

Handle(StepVisual_PreDefinedTextFont) StepVisual_FontSelect::PreDefinedTextFont () const
{
	return GetCasted(StepVisual_PreDefinedTextFont,Value());
}

Handle(StepVisual_ExternallyDefinedTextFont) StepVisual_FontSelect::ExternallyDefinedTextFont () const
{
	return GetCasted(StepVisual_ExternallyDefinedTextFont,Value());
}
