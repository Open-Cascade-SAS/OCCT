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

#ifndef BinTObjDrivers_DocumentStorageDriver_HeaderFile
#define BinTObjDrivers_DocumentStorageDriver_HeaderFile

#include <TObj_Common.hxx>
#include <BinLDrivers_DocumentStorageDriver.hxx>
#include <Handle_BinMDF_ADriverTable.hxx>

class Handle(CDM_MessageDriver);

//  Block of comments describing class BinTObjDrivers_DocumentStorageDriver
//

class BinTObjDrivers_DocumentStorageDriver :
  public BinLDrivers_DocumentStorageDriver
{
 public:
  // ---------- PUBLIC METHODS ----------

  Standard_EXPORT BinTObjDrivers_DocumentStorageDriver();
  // Constructor

  Standard_EXPORT virtual Handle(BinMDF_ADriverTable) AttributeDrivers
                        (const Handle(CDM_MessageDriver)& theMsgDriver);

 public:
  // Declaration of CASCADE RTTI
  DEFINE_STANDARD_RTTI (BinTObjDrivers_DocumentStorageDriver)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (BinTObjDrivers_DocumentStorageDriver,
                        BinLDrivers_DocumentStorageDriver)

#endif

#ifdef _MSC_VER
#pragma once
#endif
