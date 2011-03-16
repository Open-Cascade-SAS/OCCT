#include <IGESDraw.ixx>
#include <IGESDraw_Protocol.hxx>
#include <IGESDraw_GeneralModule.hxx>
#include <IGESDraw_ReadWriteModule.hxx>
#include <IGESDraw_SpecificModule.hxx>
#include <Interface_GeneralLib.hxx>
#include <Interface_ReaderLib.hxx>
#include <IGESData_WriterLib.hxx>
#include <IGESData_SpecificLib.hxx>
#include <IGESDimen.hxx>

//  Ancillary data to work on a Package of IGES Entities with a Protocol
//  (Modules are created and loaded in appropriate libraries, once by Init)

static Handle(IGESDraw_Protocol) protocol;


    void  IGESDraw::Init ()
{
  IGESDimen::Init();
  if (protocol.IsNull()) {
    protocol = new IGESDraw_Protocol;
    Interface_GeneralLib::SetGlobal (new IGESDraw_GeneralModule,  protocol);
    Interface_ReaderLib::SetGlobal  (new IGESDraw_ReadWriteModule,protocol);
    IGESData_WriterLib::SetGlobal   (new IGESDraw_ReadWriteModule,protocol);
    IGESData_SpecificLib::SetGlobal (new IGESDraw_SpecificModule, protocol);
  }
}

    Handle(IGESDraw_Protocol)  IGESDraw::Protocol ()
{
  return protocol;
}
