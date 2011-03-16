#include <IGESGeom.ixx>
#include <IGESGeom_Protocol.hxx>
#include <IGESGeom_GeneralModule.hxx>
#include <IGESGeom_ReadWriteModule.hxx>
#include <IGESGeom_SpecificModule.hxx>
#include <Interface_GeneralLib.hxx>
#include <Interface_ReaderLib.hxx>
#include <IGESData_WriterLib.hxx>
#include <IGESData_SpecificLib.hxx>
#include <IGESBasic.hxx>

//  Ancillary data to work on a Package of IGES Entities with a Protocol
//  (Modules are created and loaded in appropriate libraries, once by Init)

static Handle(IGESGeom_Protocol) protocol;


    void  IGESGeom::Init ()
{
  IGESBasic::Init();
  if (protocol.IsNull()) {
    protocol = new IGESGeom_Protocol;
    Interface_GeneralLib::SetGlobal (new IGESGeom_GeneralModule,  protocol);
    Interface_ReaderLib::SetGlobal  (new IGESGeom_ReadWriteModule,protocol);
    IGESData_WriterLib::SetGlobal   (new IGESGeom_ReadWriteModule,protocol);
    IGESData_SpecificLib::SetGlobal (new IGESGeom_SpecificModule, protocol);
  }
}

    Handle(IGESGeom_Protocol)  IGESGeom::Protocol ()
{
  return protocol;
}
