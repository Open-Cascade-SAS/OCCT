#include <HeaderSection_FileDescription.ixx>


HeaderSection_FileDescription::HeaderSection_FileDescription ()  {}

void HeaderSection_FileDescription::Init(
	const Handle(Interface_HArray1OfHAsciiString)& aDescription,
	const Handle(TCollection_HAsciiString)& aImplementationLevel)
{
	// --- class own fields ---
	description = aDescription;
	implementationLevel = aImplementationLevel;
}


void HeaderSection_FileDescription::SetDescription(const Handle(Interface_HArray1OfHAsciiString)& aDescription)
{
	description = aDescription;
}

Handle(Interface_HArray1OfHAsciiString) HeaderSection_FileDescription::Description() const
{
	return description;
}

Handle(TCollection_HAsciiString) HeaderSection_FileDescription::DescriptionValue(const Standard_Integer num) const
{
	return description->Value(num);
}

Standard_Integer HeaderSection_FileDescription::NbDescription () const
{
	if (description.IsNull()) return 0;
	return description->Length();
}

void HeaderSection_FileDescription::SetImplementationLevel(const Handle(TCollection_HAsciiString)& aImplementationLevel)
{
	implementationLevel = aImplementationLevel;
}

Handle(TCollection_HAsciiString) HeaderSection_FileDescription::ImplementationLevel() const
{
	return implementationLevel;
}
