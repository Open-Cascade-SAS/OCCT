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

#ifndef _CDF_Application_HeaderFile
#define _CDF_Application_HeaderFile

#include <TCollection_ExtendedString.hxx>
#include <PCDM_ReaderStatus.hxx>
#include <CDF_TypeOfActivation.hxx>
#include <CDF_MetaDataDriver.hxx>
#include <CDM_Application.hxx>
#include <CDM_CanCloseStatus.hxx>
#include <Standard_IStream.hxx>
#include <NCollection_IndexedDataMap.hxx>

class Standard_GUID;
class CDM_Document;
class PCDM_Reader;
class CDM_MetaData;
class PCDM_RetrievalDriver;
class PCDM_StorageDriver;
class CDF_Directory;
class CDF_Application;
DEFINE_STANDARD_HANDLE(CDF_Application, CDM_Application)

class CDF_Application : public CDM_Application
{

public:
  //! plugs an application.
  //!
  //! Open is used
  //! - for opening a Document that has been created in an application
  //! - for opening a Document from the database
  //! - for opening a Document from a file.
  //! The Open methods always add the document in the session directory and
  //! calls the virtual Activate method. The document is considered to be
  //! opened until Close is used. To be storable, a document must be
  //! opened by an application since the application resources are
  //! needed to store it.
  Standard_EXPORT static Handle(CDF_Application) Load(const Standard_GUID& aGUID);

  //! Constructs an new empty document.
  //! This document will have the specified format.
  //! If InitDocument() is redefined for a specific
  //! application, the new document is handled by the
  //! applicative session.
  Standard_EXPORT virtual void NewDocument(const TCollection_ExtendedString& theFormat,
                                           Handle(CDM_Document)&             theDoc);

  //! Initialize a document for the applicative session.
  //! This virtual function is called by NewDocument
  //! and should be redefined for each specific application.
  Standard_EXPORT virtual void InitDocument(const Handle(CDM_Document)& theDoc) const;

  //! puts the document in the current session directory
  //! and calls the virtual method Activate on it.
  Standard_EXPORT void Open(const Handle(CDM_Document)& aDocument);

  Standard_EXPORT CDM_CanCloseStatus CanClose(const Handle(CDM_Document)& aDocument);

  //! removes the document of the current session directory
  //! and closes the document;
  Standard_EXPORT void Close(const Handle(CDM_Document)& aDocument);

  //! This method retrieves a document from the database.
  //! If the Document references other documents which have
  //! been updated, the latest version of these documents will
  //! be used if {UseStorageConfiguration} is Standard_True.
  //! The content of {aFolder}, {aName} and {aVersion} depends on
  //! the Database Manager system. If the DBMS is only based on
  //! the OS, {aFolder} is a directory and {aName} is the name of a
  //! file. In this case the use of the syntax with {aVersion}
  //! has no sense. For example:
  //!
  //! Handle(CDM_Document) theDocument=myApplication->Retrieve("/home/cascade","box.dsg");
  //! If the DBMS is EUCLID/Design Manager, {aFolder}, {aName}
  //! have the form they have in EUCLID/Design Manager. For example:
  //!
  //! Handle(CDM_Document) theDocument=myApplication->Retrieve("|user|cascade","box");
  //!
  //! Since  the version is not specified in  this syntax, the  latest will be used.
  //! A link is kept with the database through an instance of CDM_MetaData
  Standard_EXPORT Handle(CDM_Document) Retrieve(
    const TCollection_ExtendedString& aFolder,
    const TCollection_ExtendedString& aName,
    const Standard_Boolean            UseStorageConfiguration = Standard_True,
    const Handle(PCDM_ReaderFilter)&  theFilter               = Handle(PCDM_ReaderFilter)(),
    const Message_ProgressRange&      theRange                = Message_ProgressRange());

  //! This method retrieves  a  document from the database.
  //! If the  Document references other documents which have
  //! been  updated, the  latest version of  these documents
  //! will    be   used  if   {UseStorageConfiguration}  is
  //! Standard_True.  --  If the DBMS is  only  based on the
  //! OS, this syntax  should not be used.
  //!
  //! If the DBMS is EUCLID/Design Manager, {aFolder}, {aName}
  //! and  {aVersion} have the form they have in
  //! EUCLID/Design Manager. For example:
  //!
  //! Handle(CDM_Document) theDocument=myApplication->Retrieve("|user|cascade","box","2");
  //! A link is kept with the database through an instance
  //! of CDM_MetaData
  Standard_EXPORT Handle(CDM_Document) Retrieve(
    const TCollection_ExtendedString& aFolder,
    const TCollection_ExtendedString& aName,
    const TCollection_ExtendedString& aVersion,
    const Standard_Boolean            UseStorageConfiguration = Standard_True,
    const Handle(PCDM_ReaderFilter)&  theFilter               = Handle(PCDM_ReaderFilter)(),
    const Message_ProgressRange&      theRange                = Message_ProgressRange());

  Standard_EXPORT PCDM_ReaderStatus CanRetrieve(const TCollection_ExtendedString& theFolder,
                                                const TCollection_ExtendedString& theName,
                                                const bool                        theAppendMode);

  Standard_EXPORT PCDM_ReaderStatus CanRetrieve(const TCollection_ExtendedString& theFolder,
                                                const TCollection_ExtendedString& theName,
                                                const TCollection_ExtendedString& theVersion,
                                                const bool                        theAppendMode);

  //! Checks  status  after  Retrieve
  PCDM_ReaderStatus GetRetrieveStatus() const { return myRetrievableStatus; }

  //! Reads theDocument from standard SEEKABLE stream theIStream,
  //! the stream should support SEEK functionality
  Standard_EXPORT void Read(
    Standard_IStream&                theIStream,
    Handle(CDM_Document)&            theDocument,
    const Handle(PCDM_ReaderFilter)& theFilter = Handle(PCDM_ReaderFilter)(),
    const Message_ProgressRange&     theRange  = Message_ProgressRange());

  //! Returns instance of read driver for specified format.
  //!
  //! Default implementation uses plugin mechanism to load reader dynamically.
  //! For this to work, application resources should define GUID of
  //! the plugin as value of [Format].RetrievalPlugin, and "Plugin"
  //! resource should define name of plugin library to be loaded as
  //! value of [GUID].Location. Plugin library should provide
  //! method PLUGINFACTORY returning instance of the reader for the
  //! same GUID (see Plugin_Macro.hxx).
  //!
  //! In case if reader is not available, will raise Standard_NoSuchObject
  //! or other exception if raised by plugin loader.
  Standard_EXPORT virtual Handle(PCDM_Reader) ReaderFromFormat(
    const TCollection_ExtendedString& aFormat);

  //! Returns instance of storage driver for specified format.
  //!
  //! Default implementation uses plugin mechanism to load driver dynamically.
  //! For this to work, application resources should define GUID of
  //! the plugin as value of [Format].StoragePlugin, and "Plugin"
  //! resource should define name of plugin library to be loaded as
  //! value of [GUID].Location. Plugin library should provide
  //! method PLUGINFACTORY returning instance of the reader for the
  //! same GUID (see Plugin_Macro.hxx).
  //!
  //! In case if driver is not available, will raise Standard_NoSuchObject
  //! or other exception if raised by plugin loader.
  Standard_EXPORT virtual Handle(PCDM_StorageDriver) WriterFromFormat(
    const TCollection_ExtendedString& aFormat);

  //! try to  retrieve a Format  directly in the  file or in
  //! application   resource  by using   extension. returns
  //! True if found;
  Standard_EXPORT Standard_Boolean Format(const TCollection_ExtendedString& aFileName,
                                          TCollection_ExtendedString&       theFormat);

  Standard_EXPORT Standard_ExtString DefaultFolder();

  Standard_EXPORT Standard_Boolean SetDefaultFolder(const Standard_ExtString aFolder);

  //! returns MetaDatdDriver of this application
  Standard_EXPORT Handle(CDF_MetaDataDriver) MetaDataDriver() const;

  DEFINE_STANDARD_RTTIEXT(CDF_Application, CDM_Application)

  Handle(CDF_MetaDataDriver) myMetaDataDriver;
  Handle(CDF_Directory)      myDirectory;

private:
  //! Informs the  application that aDocument has  been
  //! activated. A document is activated when it is created or
  //! retrieved.
  //! aTypeOfActivation will be:
  //! - CDF_TOA_New if the document is a new one
  //! (even empty or retrieved from the database for
  //! the first time).
  //! - CDF_TOA_Unchanged if the document was already
  //! retrieved but had no changes since the previous retrieval.
  //! - CDF_TOA_Modified if the document was already
  //! retrieved and modified since the previous retrieval.
  //! You do not need to call <Activate>, but you should  redefine
  //! this method to implement application specific behavior.
  Standard_EXPORT virtual void Activate(const Handle(CDM_Document)& aDocument,
                                        const CDF_TypeOfActivation  aTypeOfActivation);

  Standard_EXPORT Handle(CDM_Document) Retrieve(
    const Handle(CDM_MetaData)&      aMetaData,
    const Standard_Boolean           UseStorageConfiguration,
    const Handle(PCDM_ReaderFilter)& theFilter = Handle(PCDM_ReaderFilter)(),
    const Message_ProgressRange&     theRange  = Message_ProgressRange()) Standard_OVERRIDE;

  Standard_EXPORT Handle(CDM_Document) Retrieve(
    const Handle(CDM_MetaData)&      aMetaData,
    const Standard_Boolean           UseStorageConfiguration,
    const Standard_Boolean           IsComponent,
    const Handle(PCDM_ReaderFilter)& theFilter = Handle(PCDM_ReaderFilter)(),
    const Message_ProgressRange&     theRange  = Message_ProgressRange());

  Standard_EXPORT Standard_Integer DocumentVersion(const Handle(CDM_MetaData)& theMetaData)
    Standard_OVERRIDE;

  Standard_EXPORT CDF_TypeOfActivation TypeOfActivation(const Handle(CDM_MetaData)& aMetaData);

  Standard_EXPORT PCDM_ReaderStatus CanRetrieve(const Handle(CDM_MetaData)& aMetaData,
                                                const bool                  theAppendMode);

protected:
  Standard_EXPORT CDF_Application();

  PCDM_ReaderStatus myRetrievableStatus;
  NCollection_IndexedDataMap<TCollection_ExtendedString, Handle(PCDM_RetrievalDriver)> myReaders;
  NCollection_IndexedDataMap<TCollection_ExtendedString, Handle(PCDM_StorageDriver)>   myWriters;

private:
  TCollection_ExtendedString myDefaultFolder;
};

#endif // _CDF_Application_HeaderFile
