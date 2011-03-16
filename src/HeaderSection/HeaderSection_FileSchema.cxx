#include <HeaderSection_FileSchema.ixx>


HeaderSection_FileSchema::HeaderSection_FileSchema ()  {}

void HeaderSection_FileSchema::Init(
	const Handle(Interface_HArray1OfHAsciiString)& aSchemaIdentifiers)
{
	// --- class own fields ---
	schemaIdentifiers = aSchemaIdentifiers;
}


void HeaderSection_FileSchema::SetSchemaIdentifiers(const Handle(Interface_HArray1OfHAsciiString)& aSchemaIdentifiers)
{
	schemaIdentifiers = aSchemaIdentifiers;
}

Handle(Interface_HArray1OfHAsciiString) HeaderSection_FileSchema::SchemaIdentifiers() const
{
	return schemaIdentifiers;
}

Handle(TCollection_HAsciiString) HeaderSection_FileSchema::SchemaIdentifiersValue(const Standard_Integer num) const
{
	return schemaIdentifiers->Value(num);
}

Standard_Integer HeaderSection_FileSchema::NbSchemaIdentifiers () const
{
	if (schemaIdentifiers.IsNull()) return 0;
	return schemaIdentifiers->Length();
}
