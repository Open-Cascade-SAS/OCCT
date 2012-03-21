// Created on: 1997-08-08
// Created by: Jean-Louis Frenkel
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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



#include <CDF_Session.ixx>
#include <TCollection_ExtendedString.hxx>
#include <CDF_MetaDataDriverFactory.hxx>
#include <Plugin.hxx>
#include <Standard_GUID.hxx>
#include <PCDM.hxx>
#include <PCDM_Reader.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>

static Handle(CDF_Session) CS;

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
CDF_Session::CDF_Session () : myHasCurrentApplication(Standard_False)
{  
  Standard_MultiplyDefined_Raise_if(!CS.IsNull()," a session already exists");
  myDirectory = new CDF_Directory();
  CS = this;
}

//=======================================================================
//function : Exists
//purpose  : 
//=======================================================================
Standard_Boolean CDF_Session::Exists() {
  return !CS.IsNull();
}

//=======================================================================
//function : Directory
//purpose  : 
//=======================================================================
Handle(CDF_Directory) CDF_Session::Directory() const {
  
  return CS->myDirectory;
}

//=======================================================================
//function : CurrentSession
//purpose  : 
//=======================================================================
Handle(CDF_Session) CDF_Session::CurrentSession() {
  Standard_NoSuchObject_Raise_if(CS.IsNull(), "no session has been created");
  return CS;
}

//=======================================================================
//function : HasCurrentApplication
//purpose  : 
//=======================================================================
Standard_Boolean CDF_Session::HasCurrentApplication() const {
  return myHasCurrentApplication;
}

//=======================================================================
//function : CurrentApplication
//purpose  : 
//=======================================================================
Handle(CDF_Application) CDF_Session::CurrentApplication() const {
  Standard_NoSuchObject_Raise_if(!myHasCurrentApplication,"there is no current application in the session");
  return myCurrentApplication;
}

//=======================================================================
//function : SetCurrentApplication
//purpose  : 
//=======================================================================
void CDF_Session::SetCurrentApplication(const Handle(CDF_Application)& anApplication) {
  myCurrentApplication  = anApplication;
  myHasCurrentApplication = Standard_True;
}

//=======================================================================
//function : UnsetCurrentApplication
//purpose  : 
//=======================================================================
void CDF_Session::UnsetCurrentApplication() {
  myHasCurrentApplication = Standard_False;
  myCurrentApplication.Nullify();
}

//=======================================================================
//function : MetaDataDriver
//purpose  : 
//=======================================================================
Handle(CDF_MetaDataDriver) CDF_Session::MetaDataDriver() const {
  Standard_NoSuchObject_Raise_if(myMetaDataDriver.IsNull(),"no metadatadriver has been provided; this session is not able to store or retrieve files.");
  return myMetaDataDriver;
}

//=======================================================================
//function : LoadDriver
//purpose  : 
//=======================================================================
void CDF_Session::LoadDriver() {
   myMetaDataDriver=Handle(CDF_MetaDataDriverFactory)::DownCast(Plugin::Load(Standard_GUID("a148e300-5740-11d1-a904-080036aaa103")))->Build();
}
