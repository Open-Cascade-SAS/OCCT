// Created on: 2002-10-29
// Created by: Michael SAZONOV
// Copyright (c) 2002-2012 OPEN CASCADE SAS
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


#include <BinLDrivers.ixx>
#include <BinMDF.hxx>
#include <BinMDataStd.hxx>
#include <BinMFunction.hxx>
//#include <BinMNaming.hxx>
#include <BinMDocStd.hxx>
#include <Standard_Failure.hxx>
#include <BinLDrivers_DocumentStorageDriver.hxx>
#include <BinLDrivers_DocumentRetrievalDriver.hxx>
#include <Plugin_Macro.hxx>

// avoid warnings on 'extern "C"' functions returning C++ classes
#ifdef WNT
#pragma warning(4:4190)
#endif

static Standard_GUID BinLStorageDriver  ("13a56835-8269-11d5-aab2-0050044b1af1");
static Standard_GUID BinLRetrievalDriver("13a56836-8269-11d5-aab2-0050044b1af1");
#define CURRENT_DOCUMENT_VERSION 6

//=======================================================================
//function : Factory
//purpose  : PLUGIN FACTORY
//=======================================================================
Handle(Standard_Transient) BinLDrivers::Factory(const Standard_GUID& theGUID)
{
  if (theGUID == BinLStorageDriver)
  {
    cout << "BinLDrivers : Storage Plugin" << endl;
    static Handle(BinLDrivers_DocumentStorageDriver) model_sd =
      new BinLDrivers_DocumentStorageDriver;
    return model_sd;
  }

  if (theGUID == BinLRetrievalDriver)
  {
    cout << "BinLDrivers : Retrieval Plugin" << endl;
    static Handle(BinLDrivers_DocumentRetrievalDriver) model_rd =
      new BinLDrivers_DocumentRetrievalDriver;
    return model_rd;
  }

  Standard_Failure::Raise ("BinLDrivers : unknown GUID");
  return NULL;
}

//=======================================================================
//function : AttributeDrivers
//purpose  :
//=======================================================================

Handle(BinMDF_ADriverTable) BinLDrivers::AttributeDrivers 
                         (const Handle(CDM_MessageDriver)& aMsgDrv)
{
  Handle(BinMDF_ADriverTable) aTable = new BinMDF_ADriverTable;

  BinMDF        ::AddDrivers (aTable, aMsgDrv);
  BinMDataStd   ::AddDrivers (aTable, aMsgDrv);
  BinMFunction  ::AddDrivers (aTable, aMsgDrv);
  BinMDocStd    ::AddDrivers (aTable, aMsgDrv);

//  BinMNaming    ::AddDrivers (aTable, aMsgDrv);
//  BinMPrsStd    ::AddDrivers (aTable, aMsgDrv);
  return aTable;
}

//=======================================================================
//function : StorageVersion
//purpose  : 
//=======================================================================

TCollection_AsciiString BinLDrivers::StorageVersion()
{
  TCollection_AsciiString aVersionStr (CURRENT_DOCUMENT_VERSION);
  return aVersionStr;
}
PLUGIN(BinLDrivers)
