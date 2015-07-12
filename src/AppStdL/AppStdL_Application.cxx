// Created on: 2004-06-29
// Created by: Eugeny NAPALKOV 
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

//AGV 15/10/01 : Add XmlOcaf support; add MessageDriver support

#include <AppStdL_Application.hxx>
#include <CDM_COutMessageDriver.hxx>
#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>

//=======================================================================
//function : AppStdL_Application
//purpose  : 
//=======================================================================
AppStdL_Application::AppStdL_Application()
{
  myMessageDriver = new CDM_COutMessageDriver;
}

//=======================================================================
//function : MessageDriver
//purpose  : 
//=======================================================================

Handle(CDM_MessageDriver) AppStdL_Application::MessageDriver ()
{
  return myMessageDriver;
}

//=======================================================================
//function : Formats
//purpose  : 
//=======================================================================
void AppStdL_Application::Formats(TColStd_SequenceOfExtendedString& theFormats) 
{
  theFormats.Append("OCC-StdLite");
  theFormats.Append("XmlLOcaf");
  theFormats.Append("BinLOcaf");
}

//=======================================================================
//function : ResourcesName
//purpose  : 
//=======================================================================

Standard_CString AppStdL_Application::ResourcesName() {
  const Standard_CString aRes = "StandardLite";
  //const Standard_CString aRes = "Standard";
  return aRes;
}

