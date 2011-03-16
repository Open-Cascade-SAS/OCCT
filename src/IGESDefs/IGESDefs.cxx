#include <IGESDefs.ixx>
#include <IGESDefs_Protocol.hxx>
#include <IGESDefs_GeneralModule.hxx>
#include <IGESDefs_ReadWriteModule.hxx>
#include <IGESDefs_SpecificModule.hxx>
#include <Interface_GeneralLib.hxx>
#include <Interface_ReaderLib.hxx>
#include <IGESData_WriterLib.hxx>
#include <IGESData_SpecificLib.hxx>
#include <IGESGraph.hxx>

//  Ancillary data to work on a Package of IGES Entities with a Protocol
//  (Modules are created and loaded in appropriate libraries, once by Init)

static Handle(IGESDefs_Protocol) protocol;


    void  IGESDefs::Init ()
{
  IGESGraph::Init();
  if (protocol.IsNull()) {
    protocol = new IGESDefs_Protocol;
    Interface_GeneralLib::SetGlobal (new IGESDefs_GeneralModule,  protocol);
    Interface_ReaderLib::SetGlobal  (new IGESDefs_ReadWriteModule,protocol);
    IGESData_WriterLib::SetGlobal   (new IGESDefs_ReadWriteModule,protocol);
    IGESData_SpecificLib::SetGlobal (new IGESDefs_SpecificModule, protocol);
  }
}

    Handle(IGESDefs_Protocol)  IGESDefs::Protocol ()
{
  return protocol;
}
