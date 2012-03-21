// Created on: 2005-04-18
// Created by: Eugeny NAPALKOV
// Copyright (c) 2005-2012 OPEN CASCADE SAS
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


#include <BinXCAFDrivers.ixx>
#include <BinXCAFDrivers_DocumentStorageDriver.hxx>
#include <BinXCAFDrivers_DocumentRetrievalDriver.hxx>
#include <BinMXCAFDoc.hxx>

#include <Standard_Failure.hxx>
#include <BinDrivers.hxx>
#include <Plugin_Macro.hxx>

// avoid warnings on 'extern "C"' functions returning C++ classes
#ifdef WNT
#pragma warning(4:4190)
#endif

static Standard_GUID BinXCAFStorageDriver  ("a78ff496-a779-11d5-aab4-0050044b1af1");
static Standard_GUID BinXCAFRetrievalDriver("a78ff497-a779-11d5-aab4-0050044b1af1");


//=======================================================================
//function :
//purpose  : 
//=======================================================================
Handle(Standard_Transient) BinXCAFDrivers::Factory(const Standard_GUID& theGUID) {

  if (theGUID == BinXCAFStorageDriver)
  {
    cout << "BinXCAFDrivers : Storage Plugin" << endl;
    static Handle(BinXCAFDrivers_DocumentStorageDriver) model_sd =
      new BinXCAFDrivers_DocumentStorageDriver;
    return model_sd;
  }

  if (theGUID == BinXCAFRetrievalDriver)
  {
    cout << "BinXCAFDrivers : Retrieval Plugin" << endl;
    static Handle(BinXCAFDrivers_DocumentRetrievalDriver) model_rd =
      new BinXCAFDrivers_DocumentRetrievalDriver;
    return model_rd;
  }


  Standard_Failure::Raise ("XCAFBinDrivers : unknown GUID");
  return NULL;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Handle(BinMDF_ADriverTable) BinXCAFDrivers::AttributeDrivers(const Handle(CDM_MessageDriver)& aMsgDrv) {
  // Standard Drivers
  Handle(BinMDF_ADriverTable) aTable = BinDrivers::AttributeDrivers(aMsgDrv);

  // XCAF Drivers
  BinMXCAFDoc::AddDrivers(aTable, aMsgDrv);

  return aTable;
}

PLUGIN(BinXCAFDrivers)
