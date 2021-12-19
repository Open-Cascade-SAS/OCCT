// Created on: 2001-09-11
// Created by: Julia DOROVSKIKH
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#include <XmlMXCAFDoc.hxx>

#include <Message_Messenger.hxx>
#include <TNaming_NamedShape.hxx>
#include <XmlMDF_ADriverTable.hxx>
#include <XmlMNaming_NamedShapeDriver.hxx>
#include <XmlMXCAFDoc_AssemblyItemRefDriver.hxx>
#include <XmlMXCAFDoc_CentroidDriver.hxx>
#include <XmlMXCAFDoc_ColorDriver.hxx>
#include <XmlMXCAFDoc_DatumDriver.hxx>
#include <XmlMXCAFDoc_DimTolDriver.hxx>
#include <XmlMXCAFDoc_GraphNodeDriver.hxx>
#include <XmlMXCAFDoc_LengthUnitDriver.hxx>
#include <XmlMXCAFDoc_LocationDriver.hxx>
#include <XmlMXCAFDoc_MaterialDriver.hxx>
#include <XmlMXCAFDoc_VisMaterialDriver.hxx>
#include <XmlMXCAFDoc_NoteCommentDriver.hxx>
#include <XmlMXCAFDoc_NoteBinDataDriver.hxx>
#include <XmlMXCAFDoc_VisMaterialToolDriver.hxx>

//=======================================================================
//function : AddDrivers
//purpose  : 
//=======================================================================
void XmlMXCAFDoc::AddDrivers (const Handle(XmlMDF_ADriverTable)& aDriverTable,
                              const Handle(Message_Messenger)&   anMsgDrv)
{
  aDriverTable -> AddDriver (new XmlMXCAFDoc_CentroidDriver  (anMsgDrv));
  aDriverTable -> AddDriver (new XmlMXCAFDoc_ColorDriver     (anMsgDrv));
  aDriverTable -> AddDriver (new XmlMXCAFDoc_GraphNodeDriver (anMsgDrv));
  
  //oan: changes for sharing locations map
  Handle(XmlMDF_ADriver) aDriver;
  aDriverTable->GetDriver(STANDARD_TYPE(TNaming_NamedShape), aDriver);
  Handle(XmlMNaming_NamedShapeDriver) aNamedShapeDriver = 
    Handle(XmlMNaming_NamedShapeDriver)::DownCast (aDriver);
  
  Handle(XmlMXCAFDoc_LocationDriver) aLocationDriver = new XmlMXCAFDoc_LocationDriver (anMsgDrv);
  if( !aNamedShapeDriver.IsNull() )
  {
    aLocationDriver->SetSharedLocations( &(aNamedShapeDriver->GetShapesLocations()) );
  }
  
  aDriverTable -> AddDriver (aLocationDriver);
  aDriverTable -> AddDriver (new XmlMXCAFDoc_LengthUnitDriver (anMsgDrv));
  aDriverTable -> AddDriver (new XmlMXCAFDoc_AssemblyItemRefDriver(anMsgDrv));
  aDriverTable -> AddDriver (new XmlMXCAFDoc_DatumDriver      (anMsgDrv));
  aDriverTable -> AddDriver (new XmlMXCAFDoc_DimTolDriver     (anMsgDrv));
  aDriverTable -> AddDriver (new XmlMXCAFDoc_MaterialDriver   (anMsgDrv));
  aDriverTable -> AddDriver (new XmlMXCAFDoc_VisMaterialDriver(anMsgDrv));
  aDriverTable -> AddDriver (new XmlMXCAFDoc_NoteCommentDriver(anMsgDrv));
  aDriverTable -> AddDriver (new XmlMXCAFDoc_NoteBinDataDriver(anMsgDrv));
  aDriverTable -> AddDriver (new XmlMXCAFDoc_VisMaterialToolDriver (anMsgDrv));
}
