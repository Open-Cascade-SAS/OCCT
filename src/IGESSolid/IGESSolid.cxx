#include <IGESSolid.ixx>
#include <IGESSolid_Protocol.hxx>
#include <IGESSolid_GeneralModule.hxx>
#include <IGESSolid_ReadWriteModule.hxx>
#include <IGESSolid_SpecificModule.hxx>
#include <Interface_GeneralLib.hxx>
#include <Interface_ReaderLib.hxx>
#include <IGESData_WriterLib.hxx>
#include <IGESData_SpecificLib.hxx>
#include <IGESGeom.hxx>

//  Ancillary data to work on a Package of IGES Entities with a Protocol
//  (Modules are created and loaded in appropriate libraries, once by Init)

static Handle(IGESSolid_Protocol) protocol;



    void  IGESSolid::Init ()
{
  IGESGeom::Init();
  if (protocol.IsNull()) {
    protocol = new IGESSolid_Protocol;
    Interface_GeneralLib::SetGlobal (new IGESSolid_GeneralModule,  protocol);
    Interface_ReaderLib::SetGlobal  (new IGESSolid_ReadWriteModule,protocol);
    IGESData_WriterLib::SetGlobal   (new IGESSolid_ReadWriteModule,protocol);
    IGESData_SpecificLib::SetGlobal (new IGESSolid_SpecificModule, protocol);
  }
}

    Handle(IGESSolid_Protocol)  IGESSolid::Protocol ()
{
  return protocol;
}
