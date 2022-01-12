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

#ifndef _CDM_Reference_HeaderFile
#define _CDM_Reference_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <CDM_DocumentPointer.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
class CDM_Document;
class CDM_Application;
class CDM_MetaData;


class CDM_Reference;
DEFINE_STANDARD_HANDLE(CDM_Reference, Standard_Transient)


class CDM_Reference : public Standard_Transient
{

public:

  
  Standard_EXPORT Handle(CDM_Document) FromDocument();
  
  Standard_EXPORT Handle(CDM_Document) ToDocument();
  
  Standard_EXPORT Standard_Integer ReferenceIdentifier();
  
  Standard_EXPORT Standard_Integer DocumentVersion() const;
  
  Standard_EXPORT Standard_Boolean IsReadOnly() const;
  
  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;


friend class CDM_Document;


  DEFINE_STANDARD_RTTIEXT(CDM_Reference,Standard_Transient)

protected:




private:

  
  Standard_EXPORT CDM_Reference(const Handle(CDM_Document)& aFromDocument, const Handle(CDM_Document)& aToDocument, const Standard_Integer aReferenceIdentifier, const Standard_Integer aToDocumentVersion);
  
  Standard_EXPORT CDM_Reference(const Handle(CDM_Document)& aFromDocument, const Handle(CDM_MetaData)& aMetaData, const Standard_Integer aReferenceIdentifier, const Handle(CDM_Application)& anApplication, const Standard_Integer aToDocumentVersion, const Standard_Boolean UseStorageConfiguration);
  
  Standard_EXPORT void Update (const Handle(CDM_MetaData)& aMetaData);
  
  //! compares the actual document version with the
  //! document version at the creation of the reference
  Standard_EXPORT Standard_Boolean IsUpToDate() const;
  
  Standard_EXPORT void SetIsUpToDate();
  
  Standard_EXPORT void UnsetToDocument (const Handle(CDM_MetaData)& aMetaData, const Handle(CDM_Application)& anApplication);
  
  //! returns  true if the  ToDocument has been retrieved
  //! and opened.
  Standard_EXPORT Standard_Boolean IsOpened() const;
  
  Standard_EXPORT Handle(CDM_Document) Document() const;
  
  Standard_EXPORT Handle(CDM_MetaData) MetaData() const;
  
  Standard_EXPORT Handle(CDM_Application) Application() const;
  
  Standard_EXPORT Standard_Boolean UseStorageConfiguration() const;
  
  Standard_EXPORT Standard_Boolean IsInSession() const;
  
  Standard_EXPORT Standard_Boolean IsStored() const;

  Handle(CDM_Document) myToDocument;
  CDM_DocumentPointer myFromDocument;
  Standard_Integer myReferenceIdentifier;
  Handle(CDM_Application) myApplication;
  Handle(CDM_MetaData) myMetaData;
  Standard_Integer myDocumentVersion;
  Standard_Boolean myUseStorageConfiguration;


};







#endif // _CDM_Reference_HeaderFile
