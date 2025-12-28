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

#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
#include <CDM_MetaDataLookUpTable.hxx>
#include <Message_ProgressRange.hxx>

class CDM_MetaData;
class CDM_Document;
class Resource_Manager;
class Message_Messenger;
class PCDM_ReaderFilter;

class CDM_Application : public Standard_Transient
{

public:
  //! The manager returned by this virtual method will be
  //! used to search for Format.Retrieval resource items.
  Standard_EXPORT virtual occ::handle<Resource_Manager> Resources() = 0;

  //! Returns default messenger;
  Standard_EXPORT virtual occ::handle<Message_Messenger> MessageDriver();

  //! this method is called before the update of a document.
  //! By default, writes in MessageDriver().
  Standard_EXPORT virtual void BeginOfUpdate(const occ::handle<CDM_Document>& aDocument);

  //! this method is called after the update of a document.
  //! By default, writes in MessageDriver().
  Standard_EXPORT virtual void EndOfUpdate(const occ::handle<CDM_Document>&  aDocument,
                                           const bool                        theStatus,
                                           const TCollection_ExtendedString& ErrorString);

  //! writes the string in the application MessagerDriver.
  Standard_EXPORT void Write(const char16_t* aString);

  //! Returns the application name.
  Standard_EXPORT virtual TCollection_ExtendedString Name() const;

  //! Returns the application version.
  Standard_EXPORT virtual TCollection_AsciiString Version() const;

  //! Returns MetaData LookUpTable
  Standard_EXPORT virtual NCollection_DataMap<TCollection_ExtendedString,
                                              occ::handle<CDM_MetaData>>&
    MetaDataLookUpTable();

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const;

  friend class CDM_Reference;
  friend class CDM_MetaData;

  DEFINE_STANDARD_RTTIEXT(CDM_Application, Standard_Transient)

protected:
  Standard_EXPORT CDM_Application();

  Standard_EXPORT void SetDocumentVersion(const occ::handle<CDM_Document>& aDocument,
                                          const occ::handle<CDM_MetaData>& aMetaData) const;

  Standard_EXPORT void SetReferenceCounter(const occ::handle<CDM_Document>& aDocument,
                                           const int                        aReferenceCounter);

private:
  Standard_EXPORT virtual occ::handle<CDM_Document> Retrieve(
    const occ::handle<CDM_MetaData>&      aMetaData,
    const bool                            UseStorageConfiguration,
    const occ::handle<PCDM_ReaderFilter>& theFilter = occ::handle<PCDM_ReaderFilter>(),
    const Message_ProgressRange&          theRange  = Message_ProgressRange()) = 0;

  //! returns -1 if the metadata has no modification counter.
  Standard_EXPORT virtual int DocumentVersion(const occ::handle<CDM_MetaData>& aMetaData) = 0;

  occ::handle<Message_Messenger>                                             myMessenger;
  NCollection_DataMap<TCollection_ExtendedString, occ::handle<CDM_MetaData>> myMetaDataLookUpTable;
};

#endif // _CDM_Application_HeaderFile
