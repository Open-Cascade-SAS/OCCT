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
#include <StepGeom_Axis2Placement.hxx>
#include <StepGeom_Axis2Placement2d.hxx>
#include <StepGeom_Axis2Placement3d.hxx>

StepGeom_Axis2Placement::StepGeom_Axis2Placement () {  }

Standard_Integer StepGeom_Axis2Placement::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepGeom_Axis2Placement2d))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepGeom_Axis2Placement3d))) return 2;
	return 0;
}

Handle(StepGeom_Axis2Placement2d) StepGeom_Axis2Placement::Axis2Placement2d () const
{
	return GetCasted(StepGeom_Axis2Placement2d,Value());
}

Handle(StepGeom_Axis2Placement3d) StepGeom_Axis2Placement::Axis2Placement3d () const
{
	return GetCasted(StepGeom_Axis2Placement3d,Value());
}
