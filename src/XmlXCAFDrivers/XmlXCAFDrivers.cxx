// Created on: 2001-09-11
// Created by: Julia DOROVSKIKH
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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


#include <XmlXCAFDrivers.ixx>
#include <XmlXCAFDrivers_DocumentStorageDriver.hxx>
#include <XmlXCAFDrivers_DocumentRetrievalDriver.hxx>

#include <Standard_Failure.hxx>
#include <Standard_GUID.hxx>
#include <XmlDrivers.hxx>
#include <Plugin_Macro.hxx>

// avoid warnings on 'extern "C"' functions returning C++ classes
#ifdef WNT
#pragma warning(4:4190)
#endif

static Standard_GUID XSStorageDriver  ("f78ff496-a779-11d5-aab4-0050044b1af1");
static Standard_GUID XSRetrievalDriver("f78ff497-a779-11d5-aab4-0050044b1af1");

Handle(Standard_Transient) XmlXCAFDrivers::Factory(const Standard_GUID& aGUID)
{
  if(aGUID == XSStorageDriver)  
  {
    cout << "XmlXCAFDrivers : Storage Plugin" << endl;
    static Handle(XmlXCAFDrivers_DocumentStorageDriver) model_sd 
      = new XmlXCAFDrivers_DocumentStorageDriver
        ("Copyright: Open Cascade, 2001-2002"); // default copyright
    return model_sd;
  }
  if(aGUID == XSRetrievalDriver) 
  {  
    cout << "XmlXCAFDrivers : Retrieval Plugin" << endl;
    static Handle (XmlXCAFDrivers_DocumentRetrievalDriver) model_rd 
      = new XmlXCAFDrivers_DocumentRetrievalDriver;
    return model_rd;
  }
  
  return XmlDrivers::Factory (aGUID);
}

PLUGIN(XmlXCAFDrivers)
