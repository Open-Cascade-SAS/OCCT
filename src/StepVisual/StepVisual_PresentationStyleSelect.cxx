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
#include <StepVisual_CurveStyle.hxx>
#include <StepVisual_FillAreaStyle.hxx>
#include <StepVisual_NullStyleMember.hxx>
#include <StepVisual_PointStyle.hxx>
#include <StepVisual_PresentationStyleSelect.hxx>
#include <StepVisual_SurfaceStyleUsage.hxx>
#include <StepVisual_TextStyle.hxx>

StepVisual_PresentationStyleSelect::StepVisual_PresentationStyleSelect () {  }

Standard_Integer StepVisual_PresentationStyleSelect::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_PointStyle))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_CurveStyle))) return 2;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_SurfaceStyleUsage))) return 3;
//	if (ent->IsKind(STANDARD_TYPE(StepVisual_SymbolStyle))) return 4;
//	if (ent->IsKind(STANDARD_TYPE(StepVisual_FillAreaStyle))) return 5;
//	if (ent->IsKind(STANDARD_TYPE(StepVisual_TextStyle))) return 6;
  if (ent->IsKind(STANDARD_TYPE(StepVisual_NullStyleMember))) return 7;
	return 0;
}

Handle(StepVisual_PointStyle) StepVisual_PresentationStyleSelect::PointStyle () const
{
	return GetCasted(StepVisual_PointStyle,Value());
}

Handle(StepVisual_CurveStyle) StepVisual_PresentationStyleSelect::CurveStyle () const
{
	return GetCasted(StepVisual_CurveStyle,Value());
}

Handle(StepVisual_NullStyleMember) StepVisual_PresentationStyleSelect::NullStyle () const
{
	return GetCasted(StepVisual_NullStyleMember,Value());
}

Handle(StepVisual_SurfaceStyleUsage) StepVisual_PresentationStyleSelect::SurfaceStyleUsage () const
{
	return GetCasted(StepVisual_SurfaceStyleUsage,Value());
}
