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

#ifndef _CDM_MetaData_HeaderFile
#define _CDM_MetaData_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <CDM_DocumentPointer.hxx>
#include <TCollection_ExtendedString.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
#include <CDM_Document.hxx>
#include <CDM_Application.hxx>
#include <Standard_OStream.hxx>
#include <CDM_MetaDataLookUpTable.hxx>


class CDM_MetaData;
DEFINE_STANDARD_HANDLE(CDM_MetaData, Standard_Transient)


class CDM_MetaData : public Standard_Transient
{

public:

  Standard_EXPORT static Handle(CDM_MetaData) LookUp (CDM_MetaDataLookUpTable& theLookUpTable,
                                                      const TCollection_ExtendedString& aFolder,
                                                      const TCollection_ExtendedString& aName,
                                                      const TCollection_ExtendedString& aPath,
                                                      const TCollection_ExtendedString& aFileName,
                                                      const Standard_Boolean ReadOnly);

  Standard_EXPORT static Handle(CDM_MetaData) LookUp (CDM_MetaDataLookUpTable& theLookUpTable,
                                                      const TCollection_ExtendedString& aFolder,
                                                      const TCollection_ExtendedString& aName,
                                                      const TCollection_ExtendedString& aPath,
                                                      const TCollection_ExtendedString& aVersion,
                                                      const TCollection_ExtendedString& aFileName,
                                                      const Standard_Boolean ReadOnly);

  Standard_EXPORT Standard_Boolean IsRetrieved() const;
  
  Standard_EXPORT Handle(CDM_Document) Document() const;
  
  //! returns the folder in which the meta-data has to be created
  //! or has to be found.
  Standard_EXPORT TCollection_ExtendedString Folder() const;
  
  //! returns the name under which the meta-data has to be created
  //! or has to be found.
  Standard_EXPORT TCollection_ExtendedString Name() const;
  
  //! returns the version under which the meta-data has to be found.
  //! Warning: raises NoSuchObject from Standard if no Version has been defined
  Standard_EXPORT TCollection_ExtendedString Version() const;
  
  //! indicates that the version has to be taken into account when
  //! searching the corresponding meta-data.
  Standard_EXPORT Standard_Boolean HasVersion() const;
  
  Standard_EXPORT TCollection_ExtendedString FileName() const;
  
  Standard_EXPORT Standard_OStream& Print (Standard_OStream& anOStream) const;
Standard_OStream& operator << (Standard_OStream& anOStream);
  
  Standard_EXPORT TCollection_ExtendedString Path() const;
  
  Standard_EXPORT void UnsetDocument();
  
  Standard_EXPORT Standard_Boolean IsReadOnly() const;
  
  Standard_EXPORT void SetIsReadOnly();
  
  Standard_EXPORT void UnsetIsReadOnly();
  
  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;


friend class CDM_Reference;
friend   
  //! associates database  information to  a document which
  //! has been stored.  The name of the  document is now the
  //! name which has beenused to store the data.
  Standard_EXPORT void CDM_Document::SetMetaData (const Handle(CDM_MetaData)& aMetaData);
friend   
  Standard_EXPORT void CDM_Application::SetDocumentVersion (const Handle(CDM_Document)& aDocument, const Handle(CDM_MetaData)& aMetaData) const;


  DEFINE_STANDARD_RTTIEXT(CDM_MetaData,Standard_Transient)

private:

  CDM_MetaData (const TCollection_ExtendedString& aFolder,
                const TCollection_ExtendedString& aName,
                const TCollection_ExtendedString& aPath,
                const TCollection_ExtendedString& aFileName,
                const Standard_Boolean ReadOnly);

  CDM_MetaData (const TCollection_ExtendedString& aFolder,
                const TCollection_ExtendedString& aName,
                const TCollection_ExtendedString& aPath,
                const TCollection_ExtendedString& aVersion,
                const TCollection_ExtendedString& aFileName,
                const Standard_Boolean ReadOnly);

  void SetDocument (const Handle(CDM_Document)& aDocument);

  Standard_Integer DocumentVersion (const Handle(CDM_Application)& anApplication);

private:

  Standard_Boolean myIsRetrieved;
  CDM_DocumentPointer myDocument;
  TCollection_ExtendedString myFolder;
  TCollection_ExtendedString myName;
  TCollection_ExtendedString myVersion;
  Standard_Boolean myHasVersion;
  TCollection_ExtendedString myFileName;
  TCollection_ExtendedString myPath;
  Standard_Integer myDocumentVersion;
  Standard_Boolean myIsReadOnly;


};







#endif // _CDM_MetaData_HeaderFile
