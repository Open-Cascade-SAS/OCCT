#include <IGESAppli.ixx>
#include <IGESAppli_Protocol.hxx>
#include <IGESAppli_GeneralModule.hxx>
#include <IGESAppli_ReadWriteModule.hxx>
#include <IGESAppli_SpecificModule.hxx>
#include <Interface_GeneralLib.hxx>
#include <Interface_ReaderLib.hxx>
#include <IGESData_WriterLib.hxx>
#include <IGESData_SpecificLib.hxx>
#include <IGESDefs.hxx>
#include <IGESDraw.hxx>


//  Ancillary data to work on a Package of IGES Entities with a Protocol
//  (Modules are created and loaded in appropriate libraries, once by Init)

static Handle(IGESAppli_Protocol) protocol;



    void  IGESAppli::Init ()
{
  IGESDefs::Init();
  IGESDraw::Init();
  if (protocol.IsNull()) {
    protocol = new IGESAppli_Protocol;
    Interface_GeneralLib::SetGlobal (new IGESAppli_GeneralModule,  protocol);
    Interface_ReaderLib::SetGlobal  (new IGESAppli_ReadWriteModule,protocol);
    IGESData_WriterLib::SetGlobal   (new IGESAppli_ReadWriteModule,protocol);
    IGESData_SpecificLib::SetGlobal (new IGESAppli_SpecificModule, protocol);
  }
}

    Handle(IGESAppli_Protocol)  IGESAppli::Protocol ()
{
  return protocol;
}
