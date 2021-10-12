// Created on: 1997-07-30
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

#ifndef _CDM_Document_HeaderFile
#define _CDM_Document_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <CDM_ListOfReferences.hxx>
#include <CDM_CanCloseStatus.hxx>
#include <TColStd_SequenceOfExtendedString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <Standard_OStream.hxx>
#include <CDM_ListOfDocument.hxx>

class CDM_MetaData;
class CDM_Application;
class CDM_Reference;
class Resource_Manager;

class CDM_Document;
DEFINE_STANDARD_HANDLE(CDM_Document, Standard_Transient)

//! An applicative document is an instance of a class inheriting CDM_Document.
//! These documents have the following properties:
//! - they can have references to other documents.
//! - the modifications of a document are propagated to the referencing
//! documents.
//! - a  document can be   stored in different formats, with  or
//! without a persistent model.
//! - the drivers  for  storing  and retrieving documents  are
//! plugged in when necessary.
//! - a  document has a modification counter. This counter is
//! incremented when the document is  modified.  When a document
//! is stored,  the current  counter  value is memorized as the
//! last storage  version of the   document.  A document  is
//! considered to be  modified   when the  counter value  is
//! different from the storage version.  Once  the document is
//! saved  the storage  version  and the  counter  value are
//! identical.  The document  is  now  not considered  to  be
//! modified.
//! - a reference is a link between two documents. A reference has two
//! components: the "From Document" and the "To Document". When
//! a reference is created, an identifier of the reference is generated.
//! This identifier is unique in the scope of the From Document and
//! is conserved during storage and retrieval. This means that the
//! referenced document will be always accessible through this
//! identifier.
//! - a reference memorizes the counter value of the To Document when
//! the reference is created. The From Document is considered to
//! be up to date relative to the To Document when the
//! reference counter value is equal to the To Document counter value.
//! -  retrieval of a document  having references does not imply
//! the retrieving of the referenced documents.
class CDM_Document : public Standard_Transient
{

public:

  
  //! The Update  method  will be called  once  for each
  //! reference, but it  should not perform any computation,
  //! to avoid multiple computation of a same document.
  Standard_EXPORT virtual void Update (const Handle(CDM_Document)& aToDocument, const Standard_Integer aReferenceIdentifier, const Standard_Address aModifContext);
  
  //! This method Update   will be called
  //! to signal the end   of the modified references list.
  //! The    document     should    be  recomputed     and
  //! UpdateFromDocuments  should be called.  Update should
  //! returns True in case  of success, false otherwise.  In
  //! case of Failure, additional information can be given in
  //! ErrorString.
  Standard_EXPORT virtual Standard_Boolean Update (TCollection_ExtendedString& ErrorString);
  
  //! The Storage Format is the key which is used to determine in the
  //! application resources the storage driver plugin, the file
  //! extension and other data used to store the document.
  Standard_EXPORT virtual TCollection_ExtendedString StorageFormat() const = 0;
  
  //! by default empties the extensions.
  Standard_EXPORT virtual void Extensions (TColStd_SequenceOfExtendedString& Extensions) const;
  
  //! This method can be redefined to extract another document in
  //! a different format. For example, to extract a Shape
  //! from an applicative document.
  Standard_EXPORT virtual Standard_Boolean GetAlternativeDocument (const TCollection_ExtendedString& aFormat, Handle(CDM_Document)& anAlternativeDocument);
  
  //! Creates a reference from this document to {anOtherDocument}.
  //! Returns a reference identifier. This reference identifier
  //! is unique in the document and will not be used for the
  //! next references, even after the storing of the document.
  //! If there is already a reference between the two documents,
  //! the reference is not created, but its reference identifier
  //! is returned.
  Standard_EXPORT Standard_Integer CreateReference (const Handle(CDM_Document)& anOtherDocument);
  
  //! Removes the reference between the From Document and the
  //! To Document identified by a reference identifier.
  Standard_EXPORT void RemoveReference (const Standard_Integer aReferenceIdentifier);
  
  //! Removes all references having this document for From Document.
  Standard_EXPORT void RemoveAllReferences();
  
  //! Returns the To Document  of the reference identified by
  //! aReferenceIdentifier. If the ToDocument is stored and
  //! has not yet been retrieved, this method will retrieve it.
  Standard_EXPORT Handle(CDM_Document) Document (const Standard_Integer aReferenceIdentifier) const;
  
  //! returns True if   the  To Document of the  reference
  //! identified by aReferenceIdentifier is in session,  False
  //! if it corresponds to a not yet retrieved document.
  Standard_EXPORT Standard_Boolean IsInSession (const Standard_Integer aReferenceIdentifier) const;
  
  //! returns True if   the  To Document of the  reference
  //! identified by aReferenceIdentifier has already been stored,
  //! False  otherwise.
  Standard_EXPORT Standard_Boolean IsStored (const Standard_Integer aReferenceIdentifier) const;
  
  //! returns the name of the metadata of the To Document of
  //! the reference identified by aReferenceIdentifier.
  Standard_EXPORT TCollection_ExtendedString Name (const Standard_Integer aReferenceIdentifier) const;
  
  //! call  virtual  method   Update  on  all   referencing
  //! documents.   This method keeps  the list  of the --
  //! documents  to process.It may  be the starting of an
  //! update -- cycle. If  not,  the reentrant calls made by
  //! Update  method (without argument)  will append the
  //! referencing documents to the list and call the Update method
  //! (with arguments). Only the first call to UpdateFromDocuments
  //! generate call to Update().
  Standard_EXPORT void UpdateFromDocuments (const Standard_Address aModifContext) const;
  
  //! returns the number of references having this document as
  //! From Document.
  Standard_EXPORT Standard_Integer ToReferencesNumber() const;
  
  //! returns the number of references having this document as
  //! To Document.
  Standard_EXPORT Standard_Integer FromReferencesNumber() const;
  
  //! returns True is this document references aDocument;
  Standard_EXPORT Standard_Boolean ShallowReferences (const Handle(CDM_Document)& aDocument) const;
  
  //! returns True is this document references aDocument;
  Standard_EXPORT Standard_Boolean DeepReferences (const Handle(CDM_Document)& aDocument) const;
  
  //! Copies a  reference  to  this document.   This  method
  //! avoid retrieval of referenced document.  The arguments
  //! are  the  original  document  and a  valid  reference
  //! identifier Returns the  local identifier.
  Standard_EXPORT Standard_Integer CopyReference (const Handle(CDM_Document)& aFromDocument, const Standard_Integer aReferenceIdentifier);
  
  //! indicates  that  this document cannot be   modified.
  Standard_EXPORT Standard_Boolean IsReadOnly() const;
  
  //! indicates that the referenced document cannot be modified,
  Standard_EXPORT Standard_Boolean IsReadOnly (const Standard_Integer aReferenceIdentifier) const;
  
  Standard_EXPORT void SetIsReadOnly();
  
  Standard_EXPORT void UnsetIsReadOnly();
  
  //! Indicates that this document has been modified.
  //! This method increments the modification counter.
  Standard_EXPORT void Modify();
  
  //! returns the current modification counter.
  Standard_EXPORT Standard_Integer Modifications() const;
  
  Standard_EXPORT void UnModify();
  
  //! returns true if the modification counter found in the given
  //! reference is equal to the actual modification counter of
  //! the To Document. This method is able to deal with a reference
  //! to a not retrieved document.
  Standard_EXPORT Standard_Boolean IsUpToDate (const Standard_Integer aReferenceIdentifier) const;
  
  //! Resets the modification counter in the given reference
  //! to the actual modification counter of its To Document.
  //! This method should be called after the application has updated
  //! this document.
  Standard_EXPORT void SetIsUpToDate (const Standard_Integer aReferenceIdentifier);
  
  //! associates a comment with this document.
  Standard_EXPORT void SetComment (const TCollection_ExtendedString& aComment);
  
  //! appends a comment into comments of this document.
  Standard_EXPORT void AddComment (const TCollection_ExtendedString& aComment);
  
  //! associates a comments with this document.
  Standard_EXPORT void SetComments (const TColStd_SequenceOfExtendedString& aComments);
  
  //! returns the associated comments through <aComments>.
  //! Returns empty sequence if no comments are associated.
  Standard_EXPORT void Comments (TColStd_SequenceOfExtendedString& aComments) const;
  
  //! Returns the first of associated comments.
  //! By default the comment is an empty string.
  Standard_EXPORT Standard_ExtString Comment() const;
  
  Standard_EXPORT Standard_Boolean IsStored() const;
  
  //! returns  the value of  the modification counter at the
  //! time of storage. By default returns 0.
  Standard_EXPORT Standard_Integer StorageVersion() const;
  
  //! associates database  information to  a document which
  //! has been stored.  The name of the  document is now the
  //! name which has beenused to store the data.
  Standard_EXPORT void SetMetaData (const Handle(CDM_MetaData)& aMetaData);
  
  Standard_EXPORT void UnsetIsStored();
  
  Standard_EXPORT Handle(CDM_MetaData) MetaData() const;
  
  Standard_EXPORT TCollection_ExtendedString Folder() const;
  
  //! defines the folder in which the object should be stored.
  Standard_EXPORT void SetRequestedFolder (const TCollection_ExtendedString& aFolder);
  
  Standard_EXPORT TCollection_ExtendedString RequestedFolder() const;
  
  Standard_EXPORT Standard_Boolean HasRequestedFolder() const;
  
  //! defines the name under which the object should be stored.
  Standard_EXPORT void SetRequestedName (const TCollection_ExtendedString& aName);
  
  //! Determines under which the document is going to be store.
  //! By default the name of the document will be used.
  //! If the document has no name its presentation will be used.
  Standard_EXPORT TCollection_ExtendedString RequestedName();
  
  Standard_EXPORT void SetRequestedPreviousVersion (const TCollection_ExtendedString& aPreviousVersion);
  
  Standard_EXPORT void UnsetRequestedPreviousVersion();
  
  Standard_EXPORT Standard_Boolean HasRequestedPreviousVersion() const;
  
  Standard_EXPORT TCollection_ExtendedString RequestedPreviousVersion() const;
  
  //! defines the Comment with  which the object should be stored.
  Standard_EXPORT void SetRequestedComment (const TCollection_ExtendedString& aComment);
  
  Standard_EXPORT TCollection_ExtendedString RequestedComment() const;
  
  //! read (or rereads) the following resource.
  Standard_EXPORT void LoadResources();
  
  Standard_EXPORT Standard_Boolean FindFileExtension();
  
  //! gets the Desktop.Domain.Application.`FileFormat`.FileExtension resource.
  Standard_EXPORT TCollection_ExtendedString FileExtension();
  
  Standard_EXPORT Standard_Boolean FindDescription();
  
  //! gets the `FileFormat`.Description resource.
  Standard_EXPORT TCollection_ExtendedString Description();
  
  //! returns  true  if the   version is greater  than   the
  //! storage version
  Standard_EXPORT Standard_Boolean IsModified() const;
  
  Standard_EXPORT Standard_OStream& Print (Standard_OStream& anOStream) const;
Standard_OStream& operator << (Standard_OStream& anOStream);
  
  Standard_EXPORT Standard_Boolean IsOpened() const;
  
  Standard_EXPORT void Open (const Handle(CDM_Application)& anApplication);
  
  Standard_EXPORT CDM_CanCloseStatus CanClose() const;
  
  Standard_EXPORT void Close();
  
  Standard_EXPORT const Handle(CDM_Application)& Application() const;
  
  //! A  referenced  document  may  indicate   through  this
  //! virtual  method that it does  not allow the closing of
  //! aDocument  which  it references through  the reference
  //! aReferenceIdentifier. By default returns Standard_True.
  Standard_EXPORT virtual Standard_Boolean CanCloseReference (const Handle(CDM_Document)& aDocument, const Standard_Integer aReferenceIdentifier) const;
  
  //! A referenced document may update its internal
  //! data structure when {aDocument} which it references
  //! through the reference {aReferenceIdentifier} is being closed.
  //! By default this method does nothing.
  Standard_EXPORT virtual void CloseReference (const Handle(CDM_Document)& aDocument, const Standard_Integer aReferenceIdentifier);
  
  //! returns true if  the   document corresponding to  the
  //! given   reference has    been retrieved  and  opened.
  //! Otherwise returns false. This method does not retrieve
  //! the referenced document
  Standard_EXPORT Standard_Boolean IsOpened (const Standard_Integer aReferenceIdentifier) const;
  
  Standard_EXPORT void CreateReference (const Handle(CDM_MetaData)& aMetaData, const Standard_Integer aReferenceIdentifier, const Handle(CDM_Application)& anApplication, const Standard_Integer aToDocumentVersion, const Standard_Boolean UseStorageConfiguration);
  
  Standard_EXPORT Standard_Integer CreateReference (const Handle(CDM_MetaData)& aMetaData, const Handle(CDM_Application)& anApplication, const Standard_Integer aDocumentVersion, const Standard_Boolean UseStorageConfiguration);
  
  Standard_EXPORT Standard_Integer ReferenceCounter() const;
  
  //! the following method should be used instead:
  //!
  //! Update(me:mutable; ErrorString: out ExtendedString from TCollection)
  //! returns Boolean from Standard
  Standard_EXPORT virtual void Update();

  Standard_EXPORT Handle(CDM_Reference) Reference (const Standard_Integer aReferenceIdentifier) const;

  Standard_EXPORT void SetModifications (const Standard_Integer Modifications);

  Standard_EXPORT void SetReferenceCounter (const Standard_Integer aReferenceCounter);

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

friend class CDM_Reference;
friend class CDM_ReferenceIterator;
friend class CDM_Application;


  DEFINE_STANDARD_RTTIEXT(CDM_Document,Standard_Transient)

protected:

  
  Standard_EXPORT CDM_Document();
  
  Standard_EXPORT ~CDM_Document();

  Standard_Boolean myResourcesAreLoaded;

private:

  
  //! the manager returned by  this method will be
  //! used to search for the following resource items.
  Standard_EXPORT Handle(Resource_Manager) StorageResource();
  
  Standard_EXPORT void AddToReference (const Handle(CDM_Reference)& aReference);
  
  Standard_EXPORT void AddFromReference (const Handle(CDM_Reference)& aReference);
  
  Standard_EXPORT void RemoveFromReference (const Standard_Integer aReferenceIdentifier);


  TColStd_SequenceOfExtendedString myComments;
  CDM_ListOfReferences myFromReferences;
  CDM_ListOfReferences myToReferences;
  Standard_Integer myVersion;
  Standard_Integer myActualReferenceIdentifier;
  Standard_Integer myStorageVersion;
  Handle(CDM_MetaData) myMetaData;
  TCollection_ExtendedString myRequestedComment;
  TCollection_ExtendedString myRequestedFolder;
  Standard_Boolean myRequestedFolderIsDefined;
  TCollection_ExtendedString myRequestedName;
  Standard_Boolean myRequestedNameIsDefined;
  Standard_Boolean myRequestedPreviousVersionIsDefined;
  TCollection_ExtendedString myRequestedPreviousVersion;
  TCollection_ExtendedString myFileExtension;
  TCollection_ExtendedString myDescription;
  Standard_Boolean myFileExtensionWasFound;
  Standard_Boolean myDescriptionWasFound;
  Handle(CDM_Application) myApplication;
};







#endif // _CDM_Document_HeaderFile
