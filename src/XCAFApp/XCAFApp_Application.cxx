// File:        XCAFApp_Application.cxx
// Created:     Wed May 24 11:02:31 2000
// Author:      data exchange team
//              <det@strelox.nnov.matra-dtv.fr>

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
