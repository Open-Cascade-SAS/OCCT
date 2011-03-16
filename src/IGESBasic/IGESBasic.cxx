#include <IGESBasic.ixx>
#include <IGESBasic_Protocol.hxx>
#include <IGESBasic_GeneralModule.hxx>
#include <IGESBasic_ReadWriteModule.hxx>
#include <IGESBasic_SpecificModule.hxx>
#include <Interface_GeneralLib.hxx>
#include <Interface_ReaderLib.hxx>
#include <IGESData_WriterLib.hxx>
#include <IGESData_SpecificLib.hxx>
#include <IGESData.hxx>

//  Ancillary data to work on a Package of IGES Entities with a Protocol
//  (Modules are created and loaded in appropriate libraries, once by Init)

static Handle(IGESBasic_Protocol) protocol;


    void  IGESBasic::Init ()
{
  IGESData::Init();      // usefull for Undefined Type or Erroneous Entities
  if (protocol.IsNull()) {
    protocol = new IGESBasic_Protocol;
    Interface_GeneralLib::SetGlobal (new IGESBasic_GeneralModule,  protocol);
    Interface_ReaderLib::SetGlobal  (new IGESBasic_ReadWriteModule,protocol);
    IGESData_WriterLib::SetGlobal   (new IGESBasic_ReadWriteModule,protocol);
    IGESData_SpecificLib::SetGlobal (new IGESBasic_SpecificModule, protocol);
  }
}

    Handle(IGESBasic_Protocol)  IGESBasic::Protocol ()
{
  return protocol;
}
