// Created on: 1997-08-07
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

#ifndef _CDF_Session_HeaderFile
#define _CDF_Session_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Mutex.hxx>
#include <CDM_ApplicationDirectory.hxx>
class CDF_Directory;
class CDF_Application;
class CDF_MetaDataDriver;
class Standard_NoSuchObject;
class Standard_MultiplyDefined;

class CDF_Session;
DEFINE_STANDARD_HANDLE(CDF_Session, Standard_Transient)


class CDF_Session : public Standard_Transient
{

public:

  
  //! returns true if a session has been created.
  Standard_EXPORT static Standard_Boolean Exists();

  //! Creates a session if it does not exists yet
  Standard_EXPORT static Handle(CDF_Session) Create();

  //! returns the only one instance of Session
  //! that has been created.
  Standard_EXPORT static Handle(CDF_Session) CurrentSession();
  
  //! returns true if theApp is added to the session
  Standard_EXPORT Standard_Boolean AddApplication(const Handle(CDF_Application)& theApp, const Standard_ThreadId theID);

  //! returns true if theApp is added to the session
  Standard_EXPORT Standard_Boolean AddApplication(const CDF_Application* theApp, const Standard_ThreadId theID);

  //! returns true if theApp is removed from the session
  Standard_EXPORT Standard_Boolean RemoveApplication(const Standard_ThreadId theID);

  //! returns true if theApp is removed from the session
  Standard_EXPORT Standard_Boolean FindApplication(const Standard_ThreadId, Handle(CDF_Application)& theApp) const;


friend class CDF_Application;


  DEFINE_STANDARD_RTTIEXT(CDF_Session,Standard_Transient)

protected:

  //! Use "Create" session for creation of an instance
  CDF_Session();




private:


  CDM_ApplicationDirectory myAppDirectory;
  Handle(CDF_MetaDataDriver) myMetaDataDriver;
};







#endif // _CDF_Session_HeaderFile
