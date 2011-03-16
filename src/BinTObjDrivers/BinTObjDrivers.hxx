// File:      BinTObjDrivers.hxx
// Created:   24.11.04 11:23:12
// Author:    Michael SAZONOV
// Copyright:   Open CASCADE  2007
// The original implementation Copyright: (C) RINA S.p.A

#ifndef BinTObjDrivers_HeaderFile
#define BinTObjDrivers_HeaderFile

#include <TObj_Common.hxx>
#include <Standard_GUID.hxx>

class Handle(BinMDF_ADriverTable);
class Handle(CDM_MessageDriver);

// Class for registering storage/retrieval drivers for TObj Bin persistence
//

class BinTObjDrivers 
{
 public:
  // ---------- PUBLIC METHODS ----------

  Standard_EXPORT static Handle(Standard_Transient) Factory
                        (const Standard_GUID& aGUID);
  // Returns a driver corresponding to <aGUID>. Used for plugin.

  Standard_EXPORT static void AddDrivers
                        (const Handle(BinMDF_ADriverTable)& aDriverTable,
                         const Handle(CDM_MessageDriver)&   aMsgDrv);

};

#endif

#ifdef _MSC_VER
#pragma once
#endif
