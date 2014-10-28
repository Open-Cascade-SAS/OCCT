// Created on: 2001-07-09
// Created by: Julia DOROVSKIKH
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#include <XmlDrivers.ixx>

#include <XmlDrivers_DocumentStorageDriver.hxx>
#include <XmlDrivers_DocumentRetrievalDriver.hxx>
#include <XmlMDF_ADriverTable.hxx>
#include <XmlMDF.hxx>
#include <XmlMDataStd.hxx>
#include <XmlMDataXtd.hxx>
#include <XmlMDocStd.hxx>
#include <XmlMNaming.hxx>
#include <XmlMPrsStd.hxx>
#include <XmlMFunction.hxx>

#include <Standard_GUID.hxx>

#include <locale.h>
#include <time.h>

#include <Plugin_Macro.hxx>

static Standard_GUID XmlStorageDriver  ("03a56820-8269-11d5-aab2-0050044b1af1");
static Standard_GUID XmlRetrievalDriver("03a56822-8269-11d5-aab2-0050044b1af1");

//=======================================================================
//function : Factory
//purpose  : PLUGIN FACTORY
//=======================================================================
Handle(Standard_Transient) XmlDrivers::Factory(const Standard_GUID& theGUID)
{
  if (theGUID == XmlStorageDriver)
  {
#ifdef OCCT_DEBUG
    cout << "XmlDrivers : Storage Plugin" << endl;
#endif
    static Handle(XmlDrivers_DocumentStorageDriver) model_sd =
      new XmlDrivers_DocumentStorageDriver
        ("Copyright: Open Cascade, 2001-2002"); // default copyright
    return model_sd;
  }

  if (theGUID == XmlRetrievalDriver)
  {
#ifdef OCCT_DEBUG
    cout << "XmlDrivers : Retrieval Plugin" << endl;
#endif
    static Handle (XmlDrivers_DocumentRetrievalDriver) model_rd =
      new XmlDrivers_DocumentRetrievalDriver ();
    return model_rd;
  }
 
  Standard_Failure::Raise ("XmlDrivers : unknown GUID");
  return NULL;
}

//=======================================================================
//function : AttributeDrivers
//purpose  : 
//=======================================================================
Handle(XmlMDF_ADriverTable) XmlDrivers::AttributeDrivers
                (const Handle(CDM_MessageDriver)& theMessageDriver)
{
  Handle(XmlMDF_ADriverTable) aTable = new XmlMDF_ADriverTable();
  //
  XmlMDF        ::AddDrivers (aTable, theMessageDriver);
  XmlMDataStd   ::AddDrivers (aTable, theMessageDriver);
  XmlMDataXtd   ::AddDrivers (aTable, theMessageDriver);  
  XmlMNaming    ::AddDrivers (aTable, theMessageDriver);
  XmlMFunction  ::AddDrivers (aTable, theMessageDriver); 
  XmlMDocStd    ::AddDrivers (aTable, theMessageDriver); 
  XmlMPrsStd    ::AddDrivers (aTable, theMessageDriver); 
  //
  return aTable;
}

#ifdef _MSC_VER
#pragma warning(disable:4190) /* disable warning on C++ type returned by C function; should be OK for C++ usage */
#endif

// Declare entry point PLUGINFACTORY
PLUGIN(XmlDrivers)
