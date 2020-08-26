// Created on: 1997-08-08
// Created by: Jean-Louis Frenkel
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <CDF_Application.hxx>
#include <CDF_Directory.hxx>
#include <CDF_FWOSDriver.hxx>
#include <CDF_MetaDataDriver.hxx>
#include <CDF_MetaDataDriverFactory.hxx>
#include <CDF_Session.hxx>
#include <PCDM.hxx>
#include <PCDM_Reader.hxx>
#include <Plugin.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_GUID.hxx>
#include <Standard_MultiplyDefined.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_Type.hxx>
#include <TCollection_ExtendedString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(CDF_Session,Standard_Transient)

static Handle(CDF_Session) THE_CS;
static Standard_Mutex THE_MUTEX;

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
CDF_Session::CDF_Session ()
{  
  Standard_MultiplyDefined_Raise_if(!THE_CS.IsNull()," a session already exists");
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
Handle(CDF_Session) CDF_Session::Create()
{
  Standard_Mutex::Sentry aLocker(THE_MUTEX);
  if (THE_CS.IsNull())
    THE_CS = new CDF_Session;
  return THE_CS;
}

//=======================================================================
//function : Exists
//purpose  : 
//=======================================================================
Standard_Boolean CDF_Session::Exists() {
  return !THE_CS.IsNull();
}

//=======================================================================
//function : CurrentSession
//purpose  : 
//=======================================================================
Handle(CDF_Session) CDF_Session::CurrentSession() {
  Standard_NoSuchObject_Raise_if(THE_CS.IsNull(), "no session has been created");
  return THE_CS;
}

//=======================================================================
//function : AddApplication
//purpose  : adds the application to the session with unique name
//=======================================================================
Standard_Boolean CDF_Session::AddApplication(const Handle(CDF_Application)& theApp,
  const Standard_ThreadId theID)
{
  return AddApplication(theApp.get(), theID);
}

//=======================================================================
//function : AddApplication
//purpose  : adds the application to the session with unique name
//=======================================================================
Standard_Boolean CDF_Session::AddApplication(const CDF_Application* theApp,
  const Standard_ThreadId theID)
{
  Standard_Boolean aRetValue(Standard_False);
  if (theApp)
  {
    Standard_Mutex::Sentry aLocker(THE_MUTEX);
    if (!myAppDirectory.IsBound(theID))
    {
      Handle(CDF_Application) anApp(theApp);
      aRetValue = myAppDirectory.Bind(theID, anApp);
    }
  }
  return aRetValue;
}

//=======================================================================
//function : FindApplication
//purpose  : 
//=======================================================================
Standard_Boolean CDF_Session::FindApplication(const Standard_ThreadId theID, Handle(CDF_Application)& theApp) const
{
  Standard_Mutex::Sentry aLocker(THE_MUTEX);
  if (myAppDirectory.IsBound(theID))
  {
    theApp = Handle(CDF_Application)::DownCast (myAppDirectory.Find(theID));
    return ! theApp.IsNull();
  }
  return Standard_False;
}
//=======================================================================
//function : RemoveApplication
//purpose  : removes the application with name=<theName> from the session
//=======================================================================
Standard_Boolean CDF_Session::RemoveApplication(const Standard_ThreadId theID)
{
  Standard_Boolean aRetValue(Standard_False);
  Standard_Mutex::Sentry aLocker(THE_MUTEX);
  if (myAppDirectory.IsBound(theID))
  {
    aRetValue = myAppDirectory.UnBind(theID);
  }
  return aRetValue;
}
