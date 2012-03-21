// Created on: 2000-09-07
// Created by: TURIN Anatoliy
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

//AGV 15/10/01 : Add XmlOcaf support; add MessageDriver support

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

