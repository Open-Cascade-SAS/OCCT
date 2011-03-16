#include <IGESGraph.ixx>
#include <IGESGraph_Protocol.hxx>
#include <IGESGraph_GeneralModule.hxx>
#include <IGESGraph_ReadWriteModule.hxx>
#include <IGESGraph_SpecificModule.hxx>
#include <Interface_GeneralLib.hxx>
#include <Interface_ReaderLib.hxx>
#include <IGESData_WriterLib.hxx>
#include <IGESData_SpecificLib.hxx>
#include <IGESBasic.hxx>

//  Ancillary data to work on a Package of IGES Entities with a Protocol
//  (Modules are created and loaded in appropriate libraries, once by Init)

static Handle(IGESGraph_Protocol) protocol;


    void  IGESGraph::Init ()
{
  IGESBasic::Init();
  if (protocol.IsNull()) {
    protocol = new IGESGraph_Protocol;
    Interface_GeneralLib::SetGlobal (new IGESGraph_GeneralModule,  protocol);
    Interface_ReaderLib::SetGlobal  (new IGESGraph_ReadWriteModule,protocol);
    IGESData_WriterLib::SetGlobal   (new IGESGraph_ReadWriteModule,protocol);
    IGESData_SpecificLib::SetGlobal (new IGESGraph_SpecificModule, protocol);
  }
}

    Handle(IGESGraph_Protocol)  IGESGraph::Protocol ()
{
  return protocol;
}
