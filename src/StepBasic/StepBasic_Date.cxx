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


#include <Standard_Type.hxx>
#include <StepBasic_Date.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_Date,Standard_Transient)

StepBasic_Date::StepBasic_Date ()  {}

void StepBasic_Date::Init(
	const Standard_Integer aYearComponent)
{
	// --- classe own fields ---
	yearComponent = aYearComponent;
}


void StepBasic_Date::SetYearComponent(const Standard_Integer aYearComponent)
{
	yearComponent = aYearComponent;
}

Standard_Integer StepBasic_Date::YearComponent() const
{
	return yearComponent;
}
