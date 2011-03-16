
#include <HeaderSection.ixx>

#include <Interface_Statics.hxx>
#include <HeaderSection_Protocol.hxx>

StaticHandle(HeaderSection_Protocol, proto);

Handle(HeaderSection_Protocol) HeaderSection::Protocol()

	{
		InitHandleVoid(HeaderSection_Protocol, proto);
		return proto;
	}

