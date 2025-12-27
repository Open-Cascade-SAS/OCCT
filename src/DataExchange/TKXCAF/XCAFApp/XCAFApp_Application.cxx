// Created on: 2000-05-24
// Created by: data exchange team
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#include <Resource_Manager.hxx>
#include <Standard_Dump.hxx>
#include <TDF_Label.hxx>
#include <TPrsStd_DriverTable.hxx>
#include <XCAFApp_Application.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFPrs_Driver.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFApp_Application, TDocStd_Application)

//=================================================================================================

occ::handle<XCAFApp_Application> XCAFApp_Application::GetApplication()
{
  static occ::handle<XCAFApp_Application> locApp;
  if (locApp.IsNull())
    locApp = new XCAFApp_Application;
  return locApp;
}

//=================================================================================================

XCAFApp_Application::XCAFApp_Application()
{
  // register driver for presentation
  occ::handle<TPrsStd_DriverTable> table = TPrsStd_DriverTable::Get();
  table->AddDriver(XCAFPrs_Driver::GetID(), new XCAFPrs_Driver);
}

//=================================================================================================

const char* XCAFApp_Application::ResourcesName()
{
  return static_cast<const char*>("XCAF");
  //  return static_cast<const char*>("Standard");
}

//=================================================================================================

void XCAFApp_Application::InitDocument(const occ::handle<CDM_Document>& aDoc) const
{
  XCAFDoc_DocumentTool::Set(occ::down_cast<TDocStd_Document>(aDoc)->Main());
}

//=================================================================================================

void XCAFApp_Application::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDocStd_Application)
}
