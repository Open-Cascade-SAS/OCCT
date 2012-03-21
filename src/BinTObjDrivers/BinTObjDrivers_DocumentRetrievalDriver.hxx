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

#ifndef BinTObjDrivers_DocumentRetrievalDriver_HeaderFile
#define BinTObjDrivers_DocumentRetrievalDriver_HeaderFile

#include <TObj_Common.hxx>
#include <BinLDrivers_DocumentRetrievalDriver.hxx>
#include <Handle_BinMDF_ADriverTable.hxx>

class Handle(CDM_MessageDriver);

// Retrieval driver of a TObj Bin document
//

class BinTObjDrivers_DocumentRetrievalDriver :
  public BinLDrivers_DocumentRetrievalDriver
{
 public:
  // ---------- PUBLIC METHODS ----------

  Standard_EXPORT BinTObjDrivers_DocumentRetrievalDriver ();
  // Constructor

  Standard_EXPORT virtual Handle(BinMDF_ADriverTable) AttributeDrivers
                        (const Handle(CDM_MessageDriver)& theMsgDriver);

 public:
  // Declaration of CASCADE RTTI
  DEFINE_STANDARD_RTTI (BinTObjDrivers_DocumentRetrievalDriver)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (BinTObjDrivers_DocumentRetrievalDriver,
                        BinLDrivers_DocumentRetrievalDriver)

#endif

#ifdef _MSC_VER
#pragma once
#endif
