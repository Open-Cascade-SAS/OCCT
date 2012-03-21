// Created on: 2004-11-24
// Created by: Michael SAZONOV
// Copyright (c) 2004-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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
