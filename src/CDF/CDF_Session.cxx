// File:	CDF_Session.cxx
// Created:	Fri Aug  8 11:04:07 1997
// Author:	Jean-Louis Frenkel
//		<rmi@frilox.paris1.matra-dtv.fr>


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
