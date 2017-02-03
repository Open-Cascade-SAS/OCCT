// Created on: 1997-10-22
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

#ifndef _CDM_Application_HeaderFile
#define _CDM_Application_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_ExtString.hxx>
class CDM_Reference;
class CDM_MetaData;
class CDM_Document;
class Resource_Manager;
class CDM_MessageDriver;
class TCollection_ExtendedString;


class CDM_Application;
DEFINE_STANDARD_HANDLE(CDM_Application, Standard_Transient)


class CDM_Application : public Standard_Transient
{

public:

  
  //! The manager returned by  this virtual  method will be
  //! used to search for Format.Retrieval  resource items.
  Standard_EXPORT virtual Handle(Resource_Manager) Resources() = 0;
  
  //! By default returns a NullMessageDriver;
  Standard_EXPORT virtual Handle(CDM_MessageDriver) MessageDriver();
  
  //! this method is called before the update of a document.
  //! By default, writes in MessageDriver().
  Standard_EXPORT virtual void BeginOfUpdate (const Handle(CDM_Document)& aDocument);
  
  //! this method is called affter the update of a document.
  //! By default, writes in MessageDriver().
  Standard_EXPORT virtual void EndOfUpdate (const Handle(CDM_Document)& aDocument, const Standard_Boolean theStatus, const TCollection_ExtendedString& ErrorString);
  
  //! writes the string in the application MessagerDriver.
  Standard_EXPORT void Write (const Standard_ExtString aString);


friend class CDM_Reference;
friend class CDM_MetaData;


  DEFINE_STANDARD_RTTIEXT(CDM_Application,Standard_Transient)

protected:

  
  Standard_EXPORT void SetDocumentVersion (const Handle(CDM_Document)& aDocument, const Handle(CDM_MetaData)& aMetaData) const;
  
  Standard_EXPORT void SetReferenceCounter (const Handle(CDM_Document)& aDocument, const Standard_Integer aReferenceCounter);



private:

  
  Standard_EXPORT virtual Handle(CDM_Document) Retrieve (const Handle(CDM_MetaData)& aMetaData, const Standard_Boolean UseStorageConfiguration) = 0;
  
  //! returns -1 if the metadata has no modification counter.
  Standard_EXPORT virtual Standard_Integer DocumentVersion (const Handle(CDM_MetaData)& aMetaData) = 0;



};







#endif // _CDM_Application_HeaderFile
