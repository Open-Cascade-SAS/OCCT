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

#include <Message_Messenger.hxx>
#include <Plugin_Macro.hxx>
#include <Standard_GUID.hxx>
#include <TDocStd_Application.hxx>
#include <TCollection_AsciiString.hxx>
#include <XmlLDrivers.hxx>
#include <XmlLDrivers_DocumentRetrievalDriver.hxx>
#include <XmlLDrivers_DocumentStorageDriver.hxx>
#include <XmlMDataStd.hxx>
#include <XmlMDF.hxx>
#include <XmlMDF_ADriverTable.hxx>
#include <XmlMDocStd.hxx>
#include <XmlMFunction.hxx>

#include <time.h>
static Standard_GUID XmlLStorageDriver("13a56820-8269-11d5-aab2-0050044b1af1");
static Standard_GUID XmlLRetrievalDriver("13a56822-8269-11d5-aab2-0050044b1af1");

//=======================================================================
// function : Factory
// purpose  : PLUGIN FACTORY
//=======================================================================
const Handle(Standard_Transient)& XmlLDrivers::Factory(const Standard_GUID& theGUID)
{
  if (theGUID == XmlLStorageDriver)
  {
#ifdef OCCT_DEBUG
    std::cout << "XmlLDrivers : Storage Plugin" << std::endl;
#endif
    static Handle(Standard_Transient) model_sd = new XmlLDrivers_DocumentStorageDriver(
      "Copyright: Open Cascade, 2001-2002"); // default copyright
    return model_sd;
  }

  if (theGUID == XmlLRetrievalDriver)
  {
#ifdef OCCT_DEBUG
    std::cout << "XmlLDrivers : Retrieval Plugin" << std::endl;
#endif
    static Handle(Standard_Transient) model_rd = new XmlLDrivers_DocumentRetrievalDriver();
    return model_rd;
  }

  throw Standard_Failure("XmlLDrivers : unknown GUID");
}

#define SLENGTH 80

//=======================================================================
// function : CreationDate
// purpose  : mm/dd/yy
//=======================================================================
TCollection_AsciiString XmlLDrivers::CreationDate()
{
  Standard_Character nowstr[SLENGTH];
  time_t             nowbin;
  struct tm*         nowstruct;

  if (time(&nowbin) == (time_t)-1)
  {
#ifdef OCCT_DEBUG
    std::cerr << "Storage ERROR : Could not get time of day from time()" << std::endl;
#endif
  }

  nowstruct = localtime(&nowbin);

  if (strftime(nowstr, SLENGTH, "%Y-%m-%d", nowstruct) == (size_t)0)
  {
#ifdef OCCT_DEBUG
    std::cerr << "Storage ERROR : Could not get string from strftime()" << std::endl;
#endif
  }

  return nowstr;
}

//=================================================================================================

void XmlLDrivers::DefineFormat(const Handle(TDocStd_Application)& theApp)
{
  theApp->DefineFormat("XmlLOcaf",
                       "Xml Lite OCAF Document",
                       "xmll",
                       new XmlLDrivers_DocumentRetrievalDriver,
                       new XmlLDrivers_DocumentStorageDriver("Copyright: Open Cascade, 2001-2002"));
}

//=================================================================================================

Handle(XmlMDF_ADriverTable) XmlLDrivers::AttributeDrivers(
  const Handle(Message_Messenger)& theMessageDriver)
{
  Handle(XmlMDF_ADriverTable) aTable = new XmlMDF_ADriverTable();
  //
  XmlMDF ::AddDrivers(aTable, theMessageDriver);
  XmlMDataStd ::AddDrivers(aTable, theMessageDriver);
  XmlMFunction ::AddDrivers(aTable, theMessageDriver);
  XmlMDocStd ::AddDrivers(aTable, theMessageDriver);
  //
  return aTable;
}

// Declare entry point PLUGINFACTORY
PLUGIN(XmlLDrivers)
