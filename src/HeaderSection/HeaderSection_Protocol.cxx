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

#include <HeaderSection_Protocol.ixx>

#include <HeaderSection_FileName.hxx>
#include <HeaderSection_FileDescription.hxx>
#include <HeaderSection_FileSchema.hxx>

#include <StepData_UndefinedEntity.hxx>

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
