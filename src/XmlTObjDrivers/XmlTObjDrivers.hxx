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
