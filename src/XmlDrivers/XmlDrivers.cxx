// File:      XmlDrivers.cxx
// Created:   Mon Jul  9 12:29:49 MSK DST 2001
// Author:    Julia DOROVSKIKH
// Copyright: Matra Datavision 2001

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

// avoid warnings on 'extern "C"' functions returning C++ classes
#ifdef WNT
#pragma warning(4:4190)
#endif

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
    cout << "XmlDrivers : Storage Plugin" << endl;
    static Handle(XmlDrivers_DocumentStorageDriver) model_sd =
      new XmlDrivers_DocumentStorageDriver
        ("Copyright: Open Cascade, 2001-2002"); // default copyright
    return model_sd;
  }

  if (theGUID == XmlRetrievalDriver)
  {
    cout << "XmlDrivers : Retrieval Plugin" << endl;
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
                (const Handle_CDM_MessageDriver& theMessageDriver)
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

// Declare entry point PLUGINFACTORY
PLUGIN(XmlDrivers)
