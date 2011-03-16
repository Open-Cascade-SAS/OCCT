// File:      XmlTObjDrivers_DocumentStorageDriver.hxx
// Created:   24.11.04 18:56:20
// Author:    Michael SAZONOV
// Copyright:   Open CASCADE  2007
// The original implementation Copyright: (C) RINA S.p.A

#ifndef XmlTObjDrivers_DocumentStorageDriver_HeaderFile
#define XmlTObjDrivers_DocumentStorageDriver_HeaderFile

#include <TObj_Common.hxx>
#include <XmlLDrivers_DocumentStorageDriver.hxx>
#include <Handle_XmlMDF_ADriverTable.hxx>

class Handle(CDM_MessageDriver);

//  Block of comments describing class XmlTObjDrivers_DocumentStorageDriver
//

class XmlTObjDrivers_DocumentStorageDriver :
  public XmlLDrivers_DocumentStorageDriver
{
 public:
  // ---------- PUBLIC METHODS ----------

  Standard_EXPORT XmlTObjDrivers_DocumentStorageDriver
                        (const TCollection_ExtendedString& theCopyright);
  // Constructor

  Standard_EXPORT virtual Handle(XmlMDF_ADriverTable) AttributeDrivers
                        (const Handle(CDM_MessageDriver)& theMsgDriver);

 public:
  // Declaration of CASCADE RTTI
  DEFINE_STANDARD_RTTI (XmlTObjDrivers_DocumentStorageDriver)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (XmlTObjDrivers_DocumentStorageDriver,
                        XmlLDrivers_DocumentStorageDriver)

#endif

#ifdef _MSC_VER
#pragma once
#endif
