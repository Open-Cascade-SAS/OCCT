// File:      XmlTObjDrivers_DocumentRetrievalDriver.hxx
// Created:   24.11.04 18:41:43
// Author:    Michael SAZONOV
// Copyright:   Open CASCADE  2007
// The original implementation Copyright: (C) RINA S.p.A

#ifndef XmlTObjDrivers_DocumentRetrievalDriver_HeaderFile
#define XmlTObjDrivers_DocumentRetrievalDriver_HeaderFile

#include <TObj_Common.hxx>
#include <XmlLDrivers_DocumentRetrievalDriver.hxx>
#include <Handle_XmlMDF_ADriverTable.hxx>

class Handle(CDM_MessageDriver);

// Retrieval driver of a TObj XML document
//

class XmlTObjDrivers_DocumentRetrievalDriver :
  public XmlLDrivers_DocumentRetrievalDriver
{
 public:
  // ---------- PUBLIC METHODS ----------

  Standard_EXPORT XmlTObjDrivers_DocumentRetrievalDriver ();
  // Constructor

  Standard_EXPORT virtual Handle(XmlMDF_ADriverTable) AttributeDrivers
                        (const Handle(CDM_MessageDriver)& theMsgDriver);

 public:
  // Declaration of CASCADE RTTI
  DEFINE_STANDARD_RTTI (XmlTObjDrivers_DocumentRetrievalDriver)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (XmlTObjDrivers_DocumentRetrievalDriver,
                        XmlLDrivers_DocumentRetrievalDriver)

#endif

#ifdef _MSC_VER
#pragma once
#endif
