// Created on: 2000-05-24
// Created by: data exchange team
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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


#include <XCAFApp_Application.ixx>
#include <TPrsStd_DriverTable.hxx>
#include <XCAFPrs_Driver.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <TDF_Label.hxx>

//=======================================================================
//function : GetApplication
//purpose  : 
//=======================================================================

Handle(XCAFApp_Application) XCAFApp_Application::GetApplication() 
{
  static Handle(XCAFApp_Application) locApp;
  if ( locApp.IsNull() ) locApp = new XCAFApp_Application;
  return locApp;
}

//=======================================================================
//function : XCAFApp_Application
//purpose  : 
//=======================================================================

XCAFApp_Application::XCAFApp_Application()
{
  // register driver for presentation
  Handle(TPrsStd_DriverTable) table  = TPrsStd_DriverTable::Get();
  table->AddDriver (XCAFPrs_Driver::GetID(), new XCAFPrs_Driver);
}

//=======================================================================
//function : Formats
//purpose  : 
//=======================================================================

void XCAFApp_Application::Formats(TColStd_SequenceOfExtendedString& Formats) 
{
  Formats.Append(TCollection_ExtendedString ("MDTV-XCAF"));  
  Formats.Append(TCollection_ExtendedString ("XmlXCAF"));
  Formats.Append(TCollection_ExtendedString ("XmlOcaf"));
  Formats.Append(TCollection_ExtendedString ("MDTV-Standard"));
}

//=======================================================================
//function : ResourcesName
//purpose  : 
//=======================================================================

Standard_CString XCAFApp_Application::ResourcesName() 
{
  return Standard_CString("XCAF");
//  return Standard_CString("Standard");
}

//=======================================================================
//function : InitDocument
//purpose  : 
//=======================================================================

void XCAFApp_Application::InitDocument(const Handle(TDocStd_Document)& aDoc) const
{
  XCAFDoc_DocumentTool::Set(aDoc->Main());
}
