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

class CDM_Reference : public Standard_Transient
{

public:
  Standard_EXPORT occ::handle<CDM_Document> FromDocument();

  Standard_EXPORT occ::handle<CDM_Document> ToDocument();

  Standard_EXPORT int ReferenceIdentifier();

  Standard_EXPORT int DocumentVersion() const;

  Standard_EXPORT bool IsReadOnly() const;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const;

  friend class CDM_Document;

  DEFINE_STANDARD_RTTIEXT(CDM_Reference, Standard_Transient)

private:
  Standard_EXPORT CDM_Reference(const occ::handle<CDM_Document>& aFromDocument,
                                const occ::handle<CDM_Document>& aToDocument,
                                const int      aReferenceIdentifier,
                                const int      aToDocumentVersion);

  Standard_EXPORT CDM_Reference(const occ::handle<CDM_Document>&    aFromDocument,
                                const occ::handle<CDM_MetaData>&    aMetaData,
                                const int         aReferenceIdentifier,
                                const occ::handle<CDM_Application>& anApplication,
                                const int         aToDocumentVersion,
                                const bool         UseStorageConfiguration);

  Standard_EXPORT void Update(const occ::handle<CDM_MetaData>& aMetaData);

  //! compares the actual document version with the
  //! document version at the creation of the reference
  Standard_EXPORT bool IsUpToDate() const;

  Standard_EXPORT void SetIsUpToDate();

  Standard_EXPORT void UnsetToDocument(const occ::handle<CDM_MetaData>&    aMetaData,
                                       const occ::handle<CDM_Application>& anApplication);

  //! returns true if the ToDocument has been retrieved
  //! and opened.
  Standard_EXPORT bool IsOpened() const;

  Standard_EXPORT occ::handle<CDM_Document> Document() const;

  Standard_EXPORT occ::handle<CDM_MetaData> MetaData() const;

  Standard_EXPORT occ::handle<CDM_Application> Application() const;

  Standard_EXPORT bool UseStorageConfiguration() const;

  Standard_EXPORT bool IsInSession() const;

  Standard_EXPORT bool IsStored() const;

  occ::handle<CDM_Document>    myToDocument;
  CDM_DocumentPointer     myFromDocument;
  int        myReferenceIdentifier;
  occ::handle<CDM_Application> myApplication;
  occ::handle<CDM_MetaData>    myMetaData;
  int        myDocumentVersion;
  bool        myUseStorageConfiguration;
};

#endif // _CDM_Reference_HeaderFile
