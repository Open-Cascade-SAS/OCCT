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
