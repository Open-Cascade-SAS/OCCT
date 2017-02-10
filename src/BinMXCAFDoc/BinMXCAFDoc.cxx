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


#include <BinMDF_ADriverTable.hxx>
#include <BinMNaming_NamedShapeDriver.hxx>
#include <BinMXCAFDoc.hxx>
#include <BinMXCAFDoc_AssemblyItemRefDriver.hxx>
#include <BinMXCAFDoc_AreaDriver.hxx>
#include <BinMXCAFDoc_CentroidDriver.hxx>
#include <BinMXCAFDoc_ColorDriver.hxx>
#include <BinMXCAFDoc_ColorToolDriver.hxx>
#include <BinMXCAFDoc_DatumDriver.hxx>
#include <BinMXCAFDoc_GeomToleranceDriver.hxx>
#include <BinMXCAFDoc_DimensionDriver.hxx>
#include <BinMXCAFDoc_DimTolDriver.hxx>
#include <BinMXCAFDoc_DimTolToolDriver.hxx>
#include <BinMXCAFDoc_DocumentToolDriver.hxx>
#include <BinMXCAFDoc_GraphNodeDriver.hxx>
#include <BinMXCAFDoc_LayerToolDriver.hxx>
#include <BinMXCAFDoc_LocationDriver.hxx>
#include <BinMXCAFDoc_MaterialDriver.hxx>
#include <BinMXCAFDoc_MaterialToolDriver.hxx>
#include <BinMXCAFDoc_NoteDriver.hxx>
#include <BinMXCAFDoc_NoteBalloonDriver.hxx>
#include <BinMXCAFDoc_NoteBinDataDriver.hxx>
#include <BinMXCAFDoc_NoteCommentDriver.hxx>
#include <BinMXCAFDoc_NotesToolDriver.hxx>
#include <BinMXCAFDoc_ShapeToolDriver.hxx>
#include <BinMXCAFDoc_ViewDriver.hxx>
#include <BinMXCAFDoc_ViewToolDriver.hxx>
#include <BinMXCAFDoc_VolumeDriver.hxx>
#include <CDM_MessageDriver.hxx>
#include <TNaming_NamedShape.hxx>

//=======================================================================
//function :
//purpose  : 
//=======================================================================
void BinMXCAFDoc::AddDrivers(const Handle(BinMDF_ADriverTable)& theDriverTable,
			                       const Handle(CDM_MessageDriver)&   theMsgDrv) 
{
  theDriverTable->AddDriver( new BinMXCAFDoc_AreaDriver     (theMsgDrv));
  theDriverTable->AddDriver( new BinMXCAFDoc_CentroidDriver (theMsgDrv));
  theDriverTable->AddDriver( new BinMXCAFDoc_ColorDriver    (theMsgDrv));
  theDriverTable->AddDriver( new BinMXCAFDoc_GraphNodeDriver(theMsgDrv));
  
  //oan: changes for sharing locations map
  Handle(BinMDF_ADriver) aNSDriver;
  theDriverTable->GetDriver(STANDARD_TYPE(TNaming_NamedShape), aNSDriver);
  Handle(BinMNaming_NamedShapeDriver) aNamedShapeDriver =
    Handle(BinMNaming_NamedShapeDriver)::DownCast (aNSDriver);
  
  Handle(BinMXCAFDoc_LocationDriver) aLocationDriver = new BinMXCAFDoc_LocationDriver (theMsgDrv);
  if( !aNamedShapeDriver.IsNull() )
  {
    aLocationDriver->SetSharedLocations( &(aNamedShapeDriver->GetShapesLocations()) );
  }
  
  theDriverTable->AddDriver( aLocationDriver);
  theDriverTable->AddDriver( new BinMXCAFDoc_AssemblyItemRefDriver(theMsgDrv));
  theDriverTable->AddDriver( new BinMXCAFDoc_VolumeDriver      (theMsgDrv));
  theDriverTable->AddDriver( new BinMXCAFDoc_DatumDriver       (theMsgDrv));
  theDriverTable->AddDriver( new BinMXCAFDoc_GeomToleranceDriver (theMsgDrv));
  theDriverTable->AddDriver( new BinMXCAFDoc_DimensionDriver   (theMsgDrv));
  theDriverTable->AddDriver( new BinMXCAFDoc_DimTolDriver      (theMsgDrv));
  theDriverTable->AddDriver( new BinMXCAFDoc_MaterialDriver    (theMsgDrv));
  theDriverTable->AddDriver( new BinMXCAFDoc_NoteBalloonDriver (theMsgDrv));
  theDriverTable->AddDriver( new BinMXCAFDoc_NoteBinDataDriver (theMsgDrv));
  theDriverTable->AddDriver( new BinMXCAFDoc_NoteCommentDriver (theMsgDrv));
  theDriverTable->AddDriver( new BinMXCAFDoc_ViewDriver        (theMsgDrv));

  theDriverTable->AddDriver( new BinMXCAFDoc_ColorToolDriver   (theMsgDrv));
  theDriverTable->AddDriver( new BinMXCAFDoc_DocumentToolDriver(theMsgDrv));
  theDriverTable->AddDriver( new BinMXCAFDoc_LayerToolDriver   (theMsgDrv));
  theDriverTable->AddDriver( new BinMXCAFDoc_ShapeToolDriver   (theMsgDrv));
  theDriverTable->AddDriver( new BinMXCAFDoc_DimTolToolDriver  (theMsgDrv));
  theDriverTable->AddDriver( new BinMXCAFDoc_MaterialToolDriver(theMsgDrv));
  theDriverTable->AddDriver( new BinMXCAFDoc_NotesToolDriver   (theMsgDrv));
  theDriverTable->AddDriver( new BinMXCAFDoc_ViewToolDriver    (theMsgDrv));
}
