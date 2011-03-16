
#include <RWStepAP214.ixx>
#include <RWHeaderSection.hxx>

#include <StepAP214_Protocol.hxx>

#include <StepAP214.hxx>
#include <RWStepAP214_ReadWriteModule.hxx>
#include <RWStepAP214_GeneralModule.hxx>
#include <Interface_GeneralLib.hxx>
#include <Interface_ReaderLib.hxx>
#include <StepData_WriterLib.hxx>


static int init = 0;

void RWStepAP214::Init()
	{
	  if (init) return;    init = 1;
	  RWHeaderSection::Init();
	  Handle(StepAP214_Protocol) proto = StepAP214::Protocol();
	  Interface_GeneralLib::SetGlobal (new RWStepAP214_GeneralModule,proto);
	  Interface_ReaderLib::SetGlobal  (new RWStepAP214_ReadWriteModule,proto);
	  StepData_WriterLib::SetGlobal   (new RWStepAP214_ReadWriteModule,proto);
	}
