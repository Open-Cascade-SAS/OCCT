
#include <StepAP214.ixx>

#include <Interface_Statics.hxx>
#include <StepAP214_Protocol.hxx>

StaticHandle(StepAP214_Protocol, proto);

Handle(StepAP214_Protocol) StepAP214::Protocol()

	{
		InitHandleVoid(StepAP214_Protocol, proto);
		return proto;
	}

