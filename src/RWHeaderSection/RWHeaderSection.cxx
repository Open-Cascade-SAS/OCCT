
#include <RWHeaderSection.ixx>
/// #include <EuclidStandard.hxx>

#include <StepData.hxx>

#include <HeaderSection_Protocol.hxx>

#include <HeaderSection.hxx>
#include <RWHeaderSection_ReadWriteModule.hxx>
#include <RWHeaderSection_GeneralModule.hxx>


static Handle(RWHeaderSection_ReadWriteModule) rwm;
static Handle(RWHeaderSection_GeneralModule) rwg;


void RWHeaderSection::Init()
{
///   EuclidStandard::Init();
  Handle(HeaderSection_Protocol) proto = HeaderSection::Protocol();
  StepData::AddHeaderProtocol(proto);
  if (rwm.IsNull()) rwm = new RWHeaderSection_ReadWriteModule;
  if (rwg.IsNull()) rwg = new RWHeaderSection_GeneralModule;
}


