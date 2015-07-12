// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <IGESBasic.hxx>
#include <IGESData_SpecificLib.hxx>
#include <IGESData_WriterLib.hxx>
#include <IGESGeom.hxx>
#include <IGESGeom_GeneralModule.hxx>
#include <IGESGeom_Protocol.hxx>
#include <IGESGeom_ReadWriteModule.hxx>
#include <IGESGeom_SpecificModule.hxx>
#include <Interface_GeneralLib.hxx>
#include <Interface_ReaderLib.hxx>

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
