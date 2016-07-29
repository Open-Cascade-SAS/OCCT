// Created on: 1999-06-30
// Created by: Denis PASCAL
// Copyright (c) 1999 Matra Datavision
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

#ifndef _TDocStd_Application_HeaderFile
#define _TDocStd_Application_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <CDF_Application.hxx>
#include <CDM_MessageDriver.hxx>
#include <Standard_CString.hxx>
#include <Standard_Integer.hxx>
#include <Standard_IStream.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
#include <PCDM_ReaderStatus.hxx>
#include <PCDM_StoreStatus.hxx>

class Resource_Manager;
class Standard_NoSuchObject;
class TDocStd_Document;
class TCollection_ExtendedString;

class TDocStd_Application;
DEFINE_STANDARD_HANDLE(TDocStd_Application, CDF_Application)


//! The abstract root class for all application classes.
//! They are in charge of:
//! -   Creating documents
//! -   Storing documents and retrieving them
//! -   Initializing document views.
//! To create a useful OCAF-based application, you
//! derive a class from Application and implement
//! the methods below. You will have to redefine the
//! deferred (virtual) methods Formats,
//! InitDocument, and Resources, and override others.
//! The application is a container for a document,
//! which in its turn is the container of the data
//! framework made up of labels and attributes.
//! Besides furnishing a container for documents,
//! TDocStd_Application provides the following
//! services for them:
//! -   Creation of new documents
//! -   Activation of documents in sessions of an application
//! -   Storage and retrieval of documents
//! -   Initialization of document views.
//! Note:
//! If a client needs detailed information concerning
//! the events during the Open/Store operation, MessageDriver
//! inherited from CDM_MessageDriver of the
//! corresponding application has to be implemented.
//! If the MessageDriver is not defined all messages
//! will be (by default) directed to
//! CDM_NullMessageDriver and will be lost.
class TDocStd_Application : public CDF_Application
{

public:

  //! Constructs the new instance and registers it in CDM_Session
  Standard_EXPORT TDocStd_Application();
  
  //! Check if meta data driver was successfully loaded
  //! by the application constructor
  Standard_EXPORT Standard_Boolean IsDriverLoaded() const;

  //! Redefines message driver, by default outputs to cout.
  Standard_EXPORT virtual Handle(CDM_MessageDriver) MessageDriver() Standard_OVERRIDE;
  
  //! Returns resource manager defining supported persistent formats.
  //!
  //! Default implementation loads resource file with name ResourcesName(),
  //! unless field myResources is already initialized (either by
  //! previous call or in any other way).
  //!
  //! The resource manager should define:
  //!
  //! * Format name for each file extension supported:
  //!   - [Extension].FileFormat: [Format]
  //!
  //! * For each format supported (as returned by Formats()),
  //!   its extension, description string, and (when applicable)
  //!   GUIDs of storage and retrieval plugins:
  //!   - [Format].Description: [Description]
  //!   - [Format].FileExtension: [Extension]
  //!   - [Format].RetrievalPlugin: [GUID] (optional)
  //!   - [Format].StoragePlugin: [GUID] (optional)
  Standard_EXPORT virtual Handle(Resource_Manager) Resources() Standard_OVERRIDE;
  
  //! Returns the name of the file containing the
  //! resources of this application, for support of legacy
  //! method of loading formats data from resource files.
  //!
  //! Method DefineFormat() can be used to define all necessary
  //! parameters explicitly without actually using resource files.
  //!
  //! In a resource file, the application associates the 
  //! schema name of the document with the storage and
  //! retrieval plug-ins that are to be loaded for each
  //! document. On retrieval, the application reads the
  //! schema name in the heading of the CSF file and
  //! loads the plug-in indicated in the resource file.
  //! This plug-in instantiates the actual driver for
  //! transient-persistent conversion.
  //! Your application can bring this process into play
  //! by defining a class which inherits
  //! CDF_Application and redefines the function
  //! which returns the appropriate resources file. At
  //! this point, the function Retrieve and the class
  //! CDF_Store can be called. This allows you to
  //! deal with storage and retrieval of - as well as
  //! copying and pasting - documents.
  //! To implement a class like this, several virtual
  //! functions should be redefined. In particular, you
  //! must redefine the abstract function Resources
  //! inherited from the superclass CDM_Application.
  //!
  //! Default implementation returns empty string.
  Standard_EXPORT virtual Standard_CString ResourcesName();
  
  //! Sets up resources and registers read and storage drivers for
  //! the specified format.
  //! 
  //! @param theFormat - unique name for the format, used to identify it.
  //! @param theDescription - textual description of the format.
  //! @param theExtension - extension of the files in that format. 
  //!                       The same extension can be used by several formats.
  //! @param theReader - instance of the read driver for the format.
  //!                    Null value is allowed (no possibility to read).
  //! @param theWriter - instance of the write driver for the format.
  //!                    Null value is allowed (no possibility to write).
  Standard_EXPORT void DefineFormat (const TCollection_AsciiString& theFormat,
                                     const TCollection_AsciiString& theDescription,
                                     const TCollection_AsciiString& theExtension,
                                     const Handle(PCDM_RetrievalDriver)& theReader,
                                     const Handle(PCDM_StorageDriver)& theWriter);

  //! Returns the sequence of reading formats supported by the application.
  //!
  //! @param theFormats - sequence of reading formats. Output parameter.
  Standard_EXPORT void ReadingFormats(TColStd_SequenceOfAsciiString &theFormats);

  //! Returns the sequence of writing formats supported by the application.
  //!
  //! @param theFormats - sequence of writing formats. Output parameter.
  Standard_EXPORT void WritingFormats(TColStd_SequenceOfAsciiString &theFormats);

  //! returns the number of documents handled by the current applicative session.
  Standard_EXPORT Standard_Integer NbDocuments() const;
  
  //! Constructs the new document aDoc.
  //! aDoc is identified by the index index which is
  //! any integer between 1 and n where n is the
  //! number of documents returned by NbDocument.
  //! Example
  //! Handle(TDocStd_Application)
  //! anApp;
  //! if (!CafTest::Find(A)) return 1;
  //! Handle(TDocStd) aDoc;
  //! Standard_Integer nbdoc = anApp->NbDocuments();
  //! for (Standard_Integer i = 1; i <= nbdoc; i++) {
  //! aApp->GetDocument(i,aDoc);
  Standard_EXPORT void GetDocument (const Standard_Integer index, Handle(TDocStd_Document)& aDoc) const;
  
  //! Constructs the empty new document aDoc.
  //! This document will have the format format.
  //! If InitDocument is redefined for a specific
  //! application, the new document is handled by the
  //! applicative session.
  Standard_EXPORT virtual void NewDocument (const TCollection_ExtendedString& format, Handle(TDocStd_Document)& aDoc);
  
  //! Initialize the document aDoc for the applicative session.
  //! This virtual function is called by NewDocument
  //! and is to be redefined for each specific application.
  //! Modified flag (different of disk version)
  //! =============
  //! to open/save a document
  //! =======================
  Standard_EXPORT virtual void InitDocument (const Handle(TDocStd_Document)& aDoc) const;
  
  //! Close the given document. the document is not any more
  //! handled by the applicative session.
  Standard_EXPORT void Close (const Handle(TDocStd_Document)& aDoc);
  
  //! Returns an index for the document found in the
  //! path path in this applicative session.
  //! If the returned value is 0, the document is not
  //! present in the applicative session.
  //! This method can be used for the interactive part
  //! of an application. For instance, on a call to
  //! Open, the document to be opened may already
  //! be in memory. IsInSession checks to see if this
  //! is the case. Open can be made to depend on
  //! the value of the index returned: if IsInSession
  //! returns 0, the document is opened; if it returns
  //! another value, a message is displayed asking the
  //! user if he wants to override the version of the
  //! document in memory.
  //! Example:
  //! Standard_Integer insession = A->IsInSession(aDoc);
  //! if (insession > 0) {
  //! cout << "document " << insession << " is already in session" << endl;
  //! return 0;
  //! }
  Standard_EXPORT Standard_Integer IsInSession (const TCollection_ExtendedString& path) const;
  
  //! Retrieves the document aDoc stored under the
  //! name aName in the directory directory.
  //! In order not to override a version of aDoc which
  //! is already in memory, this method can be made
  //! to depend on the value returned by IsInSession.
  Standard_EXPORT PCDM_ReaderStatus Open (const TCollection_ExtendedString& path, Handle(TDocStd_Document)& aDoc);

  //! Retrieves aDoc from standard SEEKABLE stream theIStream.
  //! the stream should support SEEK fuctionality
  Standard_EXPORT PCDM_ReaderStatus Open (Standard_IStream& theIStream, Handle(TDocStd_Document)& theDoc);

  
  //! Save the  active document  in the file  <name> in the
  //! path <path> ; o verwrites  the file  if  it already exists.
  Standard_EXPORT PCDM_StoreStatus SaveAs (const Handle(TDocStd_Document)& aDoc, const TCollection_ExtendedString& path);

  //! Save theDoc to standard SEEKABLE stream theOStream.
  //! the stream should support SEEK fuctionality
  Standard_EXPORT PCDM_StoreStatus SaveAs(const Handle(TDocStd_Document)& theDoc, Standard_OStream& theOStream);
  
  //! Save aDoc active document.
  //! Exceptions:
  //! Standard_NotImplemented if the document
  //! was not retrieved in the applicative session by using Open.
  Standard_EXPORT PCDM_StoreStatus Save (const Handle(TDocStd_Document)& aDoc);
  
  //! Save the  active document  in the file  <name> in the
  //! path <path>  .  overwrite  the file  if  it
  //! already exist.
  Standard_EXPORT PCDM_StoreStatus SaveAs (const Handle(TDocStd_Document)& aDoc, const TCollection_ExtendedString& path, TCollection_ExtendedString& theStatusMessage);

  //! Save theDoc TO standard SEEKABLE stream theOStream.
  //! the stream should support SEEK fuctionality
  Standard_EXPORT PCDM_StoreStatus SaveAs(const Handle(TDocStd_Document)& theDoc, Standard_OStream& theOStream, TCollection_ExtendedString& theStatusMessage);
  
  //! Save the document overwriting the previous file
  Standard_EXPORT PCDM_StoreStatus Save (const Handle(TDocStd_Document)& aDoc, TCollection_ExtendedString& theStatusMessage);

  //! Notification that is fired at each OpenTransaction event.
  Standard_EXPORT virtual void OnOpenTransaction (const Handle(TDocStd_Document)& theDoc);
  
  //! Notification that is fired at each CommitTransaction event.
  Standard_EXPORT virtual void OnCommitTransaction (const Handle(TDocStd_Document)& theDoc);
  
  //! Notification that is fired at each AbortTransaction event.
  Standard_EXPORT virtual void OnAbortTransaction (const Handle(TDocStd_Document)& theDoc);

  DEFINE_STANDARD_RTTIEXT(TDocStd_Application,CDF_Application)

protected:
  Handle(CDM_MessageDriver) myMessageDriver;
  Handle(Resource_Manager) myResources;
  Standard_Boolean myIsDriverLoaded;
};

#endif // _TDocStd_Application_HeaderFile
