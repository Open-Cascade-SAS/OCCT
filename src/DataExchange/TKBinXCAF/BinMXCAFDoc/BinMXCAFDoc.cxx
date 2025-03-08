// Created on: 2005-04-18
// Created by: Eugeny NAPALKOV
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#include <BinMXCAFDoc.hxx>

#include <BinMDF_ADriverTable.hxx>
#include <BinMXCAFDoc_AssemblyItemRefDriver.hxx>
#include <BinMXCAFDoc_CentroidDriver.hxx>
#include <BinMXCAFDoc_ColorDriver.hxx>
#include <BinMXCAFDoc_DatumDriver.hxx>
#include <BinMXCAFDoc_DimTolDriver.hxx>
#include <BinMXCAFDoc_GraphNodeDriver.hxx>
#include <BinMXCAFDoc_LengthUnitDriver.hxx>
#include <BinMXCAFDoc_LocationDriver.hxx>
#include <BinMXCAFDoc_MaterialDriver.hxx>
#include <BinMXCAFDoc_NoteBinDataDriver.hxx>
#include <BinMXCAFDoc_NoteCommentDriver.hxx>
#include <BinMXCAFDoc_VisMaterialDriver.hxx>
#include <BinMXCAFDoc_VisMaterialToolDriver.hxx>
#include <Message_Messenger.hxx>
#include <TNaming_NamedShape.hxx>

//=================================================================================================

void BinMXCAFDoc::AddDrivers(const Handle(BinMDF_ADriverTable)& theDriverTable,
                             const Handle(Message_Messenger)&   theMsgDrv)
{
  theDriverTable->AddDriver(new BinMXCAFDoc_CentroidDriver(theMsgDrv));
  theDriverTable->AddDriver(new BinMXCAFDoc_ColorDriver(theMsgDrv));
  theDriverTable->AddDriver(new BinMXCAFDoc_GraphNodeDriver(theMsgDrv));

  // oan: changes for sharing locations map
  Handle(BinMDF_ADriver) aNSDriver;
  theDriverTable->GetDriver(STANDARD_TYPE(TNaming_NamedShape), aNSDriver);
  Handle(BinMNaming_NamedShapeDriver) aNamedShapeDriver =
    Handle(BinMNaming_NamedShapeDriver)::DownCast(aNSDriver);

  Handle(BinMXCAFDoc_LocationDriver) aLocationDriver = new BinMXCAFDoc_LocationDriver(theMsgDrv);
  if (!aNamedShapeDriver.IsNull())
  {
    aLocationDriver->SetNSDriver(aNamedShapeDriver);
  }

  theDriverTable->AddDriver(aLocationDriver);
  theDriverTable->AddDriver(new BinMXCAFDoc_LengthUnitDriver(theMsgDrv));
  theDriverTable->AddDriver(new BinMXCAFDoc_AssemblyItemRefDriver(theMsgDrv));
  theDriverTable->AddDriver(new BinMXCAFDoc_DatumDriver(theMsgDrv));
  theDriverTable->AddDriver(new BinMXCAFDoc_DimTolDriver(theMsgDrv));
  theDriverTable->AddDriver(new BinMXCAFDoc_MaterialDriver(theMsgDrv));
  theDriverTable->AddDriver(new BinMXCAFDoc_VisMaterialDriver(theMsgDrv));
  theDriverTable->AddDriver(new BinMXCAFDoc_NoteBinDataDriver(theMsgDrv));
  theDriverTable->AddDriver(new BinMXCAFDoc_NoteCommentDriver(theMsgDrv));
  theDriverTable->AddDriver(new BinMXCAFDoc_VisMaterialToolDriver(theMsgDrv));
}
