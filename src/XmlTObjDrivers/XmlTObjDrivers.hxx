// File:      XmlTObjDrivers.hxx
// Created:   24.11.04 11:11:08
// Author:    Michael SAZONOV
// Copyright:   Open CASCADE  2007
// The original implementation Copyright: (C) RINA S.p.A

#ifndef XmlTObjDrivers_HeaderFile
#define XmlTObjDrivers_HeaderFile

#include <TObj_Common.hxx>
#include <Standard_GUID.hxx>

class Handle(XmlMDF_ADriverTable);
class Handle(CDM_MessageDriver);

// Class for registering storage/retrieval drivers for TObj XML persistence
//

class XmlTObjDrivers 
{
 public:
  // ---------- PUBLIC METHODS ----------

  Standard_EXPORT static Handle(Standard_Transient) Factory
                        (const Standard_GUID& aGUID);
  // Returns a driver corresponding to <aGUID>. Used for plugin.

  Standard_EXPORT static void AddDrivers
                        (const Handle(XmlMDF_ADriverTable)& aDriverTable,
                         const Handle(CDM_MessageDriver)& anMsgDrv);

};

#endif

#ifdef _MSC_VER
#pragma once
#endif
