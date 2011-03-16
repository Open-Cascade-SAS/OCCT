#include <IGESDimen.ixx>
#include <IGESDimen_Protocol.hxx>
#include <IGESDimen_GeneralModule.hxx>
#include <IGESDimen_ReadWriteModule.hxx>
#include <IGESDimen_SpecificModule.hxx>
#include <Interface_GeneralLib.hxx>
#include <Interface_ReaderLib.hxx>
#include <IGESData_WriterLib.hxx>
#include <IGESData_SpecificLib.hxx>
#include <IGESGeom.hxx>
#include <IGESGraph.hxx>

//  Ancillary data to work on a Package of IGES Entities with a Protocol
//  (Modules are created and loaded in appropriate libraries, once by Init)

static Handle(IGESDimen_Protocol) protocol;


    void  IGESDimen::Init ()
{
  IGESGeom::Init();
  IGESGraph::Init();
  if (protocol.IsNull()) {
    protocol = new IGESDimen_Protocol;
    Interface_GeneralLib::SetGlobal (new IGESDimen_GeneralModule,  protocol);
    Interface_ReaderLib::SetGlobal  (new IGESDimen_ReadWriteModule,protocol);
    IGESData_WriterLib::SetGlobal   (new IGESDimen_ReadWriteModule,protocol);
    IGESData_SpecificLib::SetGlobal (new IGESDimen_SpecificModule, protocol);
  }
}

    Handle(IGESDimen_Protocol)  IGESDimen::Protocol ()
{
  return protocol;
}
