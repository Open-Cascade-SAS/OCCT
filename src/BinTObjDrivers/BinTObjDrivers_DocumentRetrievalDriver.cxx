// Created on: 2004-11-24
// Created by: Michael SAZONOV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

// The original implementation Copyright: (C) RINA S.p.A

#include <BinTObjDrivers_DocumentRetrievalDriver.hxx>
#include <BinLDrivers.hxx>
#include <BinTObjDrivers.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinTObjDrivers_DocumentRetrievalDriver,BinLDrivers_DocumentRetrievalDriver)

//=======================================================================
//function : BinTObjDrivers_DocumentRetrievalDriver
//purpose  : 
//=======================================================================

BinTObjDrivers_DocumentRetrievalDriver::BinTObjDrivers_DocumentRetrievalDriver()
     : BinLDrivers_DocumentRetrievalDriver ()
{
}

//=======================================================================
//function : AttributeDrivers
//purpose  : 
//=======================================================================

Handle(BinMDF_ADriverTable)
        BinTObjDrivers_DocumentRetrievalDriver::AttributeDrivers
                        (const Handle(CDM_MessageDriver)& theMsgDrv) 
{
  // Standard drivers
  Handle(BinMDF_ADriverTable) aTable = BinLDrivers::AttributeDrivers (theMsgDrv);

  // Native drivers
  BinTObjDrivers::AddDrivers(aTable, theMsgDrv);

  return aTable;
}
