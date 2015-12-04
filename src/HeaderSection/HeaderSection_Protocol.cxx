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


#include <HeaderSection_FileDescription.hxx>
#include <HeaderSection_FileName.hxx>
#include <HeaderSection_FileSchema.hxx>
#include <HeaderSection_Protocol.hxx>
#include <Standard_Type.hxx>
#include <StepData_UndefinedEntity.hxx>

IMPLEMENT_STANDARD_RTTIEXT(HeaderSection_Protocol,StepData_Protocol)

static Standard_CString schemaName = "header_section";

HeaderSection_Protocol::HeaderSection_Protocol () { }

Standard_Integer HeaderSection_Protocol::TypeNumber(const 
Handle(Standard_Type)& atype) const
{
	if      (atype == STANDARD_TYPE(HeaderSection_FileName)) return 1;
	else if (atype == STANDARD_TYPE(HeaderSection_FileDescription)) return 2;
	else if (atype == STANDARD_TYPE(HeaderSection_FileSchema)) return 3;
	else if (atype == STANDARD_TYPE(StepData_UndefinedEntity)) return 4;
	else    return 0;
}

Standard_CString HeaderSection_Protocol::SchemaName() const
	{	return schemaName; }
