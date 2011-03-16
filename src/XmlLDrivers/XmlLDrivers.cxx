// File:      XmlLDrivers.cxx
// Created:   Mon Jul  9 12:29:49 MSK DST 2001
// Author:    Julia DOROVSKIKH
// Copyright: Matra Datavision 2001

#include <XmlLDrivers.ixx>

#include <XmlLDrivers_DocumentStorageDriver.hxx>
#include <XmlLDrivers_DocumentRetrievalDriver.hxx>
#include <XmlMDF_ADriverTable.hxx>
#include <XmlMDF.hxx>
#include <XmlMDataStd.hxx>
#include <XmlMDocStd.hxx>
#include <XmlMFunction.hxx>
#include <Standard_GUID.hxx>

#include <locale.h>
#include <time.h>
#include <Plugin_Macro.hxx>

// avoid warnings on 'extern "C"' functions returning C++ classes
#ifdef WNT
#pragma warning(4:4190)
#endif

static Standard_GUID XmlLStorageDriver  ("13a56820-8269-11d5-aab2-0050044b1af1");
static Standard_GUID XmlLRetrievalDriver("13a56822-8269-11d5-aab2-0050044b1af1");
#define CURRENT_DOCUMENT_VERSION 6

//=======================================================================
//function : Factory
//purpose  : PLUGIN FACTORY
//=======================================================================
Handle(Standard_Transient) XmlLDrivers::Factory(const Standard_GUID& theGUID)
{
  if (theGUID == XmlLStorageDriver)
  {
    cout << "XmlLDrivers : Storage Plugin" << endl;
    static Handle(XmlLDrivers_DocumentStorageDriver) model_sd =
      new XmlLDrivers_DocumentStorageDriver
        ("Copyright: Open Cascade, 2001-2002"); // default copyright
    return model_sd;
  }

  if (theGUID == XmlLRetrievalDriver)
  {
    cout << "XmlLDrivers : Retrieval Plugin" << endl;
    static Handle (XmlLDrivers_DocumentRetrievalDriver) model_rd =
      new XmlLDrivers_DocumentRetrievalDriver ();
    return model_rd;
  }
 
  Standard_Failure::Raise ("XmlLDrivers : unknown GUID");
  return NULL;
}

#define SLENGTH 80
//=======================================================================
//function : CreationDate
//purpose  : mm/dd/yy
//=======================================================================
TCollection_AsciiString XmlLDrivers::CreationDate ()
{
  const TCollection_AsciiString anOldNumLocale =
    (Standard_CString) setlocale (LC_NUMERIC, NULL);
  setlocale(LC_NUMERIC, "C");

  Standard_Character nowstr[SLENGTH];
  time_t nowbin;
  struct tm *nowstruct;

  if (time(&nowbin) == (time_t) - 1)
    cerr << "Storage ERROR : Could not get time of day from time()" << endl;
  
  nowstruct = localtime(&nowbin);
  
  if (strftime(nowstr, SLENGTH, "%Y-%m-%d", nowstruct) == (size_t) 0)
    cerr << "Storage ERROR : Could not get string from strftime()" << endl;

  setlocale(LC_NUMERIC, (char *) anOldNumLocale.ToCString()) ;
  return nowstr;
}

//=======================================================================
//function : AttributeDrivers
//purpose  : 
//=======================================================================
Handle(XmlMDF_ADriverTable) XmlLDrivers::AttributeDrivers
                (const Handle_CDM_MessageDriver& theMessageDriver)
{
  Handle(XmlMDF_ADriverTable) aTable = new XmlMDF_ADriverTable();
  //
  XmlMDF        ::AddDrivers (aTable, theMessageDriver);
  XmlMDataStd   ::AddDrivers (aTable, theMessageDriver);
  XmlMFunction  ::AddDrivers (aTable, theMessageDriver);
  XmlMDocStd    ::AddDrivers (aTable, theMessageDriver); 
  //
  return aTable;
}

//=======================================================================
//function : StorageVersion
//purpose  : Document storage version
//=======================================================================

TCollection_AsciiString XmlLDrivers::StorageVersion()
{
  TCollection_AsciiString aVersionStr (CURRENT_DOCUMENT_VERSION);
  return aVersionStr;
}

// Declare entry point PLUGINFACTORY
PLUGIN(XmlLDrivers)
