// File:        AppStd_Application.cxx
// Created:     Sep 7 15:01:46 2000
// Author:      TURIN Anatoliy <ati@nnov.matra-dtv.fr>
// Copyright:   Matra Datavision 2000
// History  : AGV 15/10/01 : Add XmlOcaf support; add MessageDriver support

#include <AppStd_Application.ixx>
#include <CDM_COutMessageDriver.hxx>

//=======================================================================
//function : AppStd_Application
//purpose  : 
//=======================================================================

AppStd_Application::AppStd_Application()
{
  myMessageDriver = new CDM_COutMessageDriver;
}

//=======================================================================
//function : MessageDriver
//purpose  : 
//=======================================================================

Handle(CDM_MessageDriver) AppStd_Application::MessageDriver ()
{
  return myMessageDriver;
}

//=======================================================================
//function : Formats
//purpose  : 
//=======================================================================
void AppStd_Application::Formats(TColStd_SequenceOfExtendedString& theFormats) 
{
  theFormats.Append("XmlOcaf");
  theFormats.Append("BinOcaf");
  theFormats.Append("MDTV-Standard");
}

//=======================================================================
//function : ResourcesName
//purpose  : 
//=======================================================================

Standard_CString AppStd_Application::ResourcesName() {
  const Standard_CString aRes = "Standard";
  return aRes;
}

