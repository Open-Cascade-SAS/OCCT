// File:        AppStdL_Application.cxx
// Created:     Jun 29 11:45:12 2004
// Author:      Eugeny NAPALKOV 
//  	    	<env@kivox>
// Copyright:   Matra Datavision 2004
// History  : AGV 15/10/01 : Add XmlOcaf support; add MessageDriver support

#include <AppStdL_Application.ixx>
#include <CDM_COutMessageDriver.hxx>

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

