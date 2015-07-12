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
#include <StepVisual_FillAreaStyleColour.hxx>
#include <StepVisual_FillStyleSelect.hxx>

StepVisual_FillStyleSelect::StepVisual_FillStyleSelect () {  }

Standard_Integer StepVisual_FillStyleSelect::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_FillAreaStyleColour))) return 1;
//	if (ent->IsKind(STANDARD_TYPE(StepVisual_ExternallyDefinedTileStyle))) return 2;
//	if (ent->IsKind(STANDARD_TYPE(StepVisual_FillAreaStyleTiles))) return 3;
//	if (ent->IsKind(STANDARD_TYPE(StepVisual_ExternallyDefinedHatchStyle))) return 4;
//	if (ent->IsKind(STANDARD_TYPE(StepVisual_FillAreaStyleHatching))) return 5;
	return 0;
}

Handle(StepVisual_FillAreaStyleColour) StepVisual_FillStyleSelect::FillAreaStyleColour () const
{
	return GetCasted(StepVisual_FillAreaStyleColour,Value());
}
