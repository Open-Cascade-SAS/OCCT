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

#ifndef _CDF_Directory_HeaderFile
#define _CDF_Directory_HeaderFile

#include <Standard.hxx>

#include <CDM_Document.hxx>
#include <NCollection_List.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
class CDM_Document;

//! A directory is a collection of documents. There is only one instance
//! of a given document in a directory.
//! put.
class CDF_Directory : public Standard_Transient
{

public:
  //! Creates an empty directory.
  Standard_EXPORT CDF_Directory();

  //! adds a document into the directory.
  Standard_EXPORT void Add(const occ::handle<CDM_Document>& aDocument);

  //! removes the document.
  Standard_EXPORT void Remove(const occ::handle<CDM_Document>& aDocument);

  //! Returns true if the document aDocument is in the directory
  Standard_EXPORT bool Contains(const occ::handle<CDM_Document>& aDocument) const;

  //! returns the last document (if any) which has been added
  //! in the directory.
  Standard_EXPORT occ::handle<CDM_Document> Last();

  //! returns the number of documents of the directory.
  Standard_EXPORT int Length() const;

  //! returns true if the directory is empty.
  Standard_EXPORT bool IsEmpty() const;

  friend class CDF_DirectoryIterator;

  DEFINE_STANDARD_RTTIEXT(CDF_Directory, Standard_Transient)

private:
  Standard_EXPORT const NCollection_List<occ::handle<CDM_Document>>& List() const;

  NCollection_List<occ::handle<CDM_Document>> myDocuments;
};

#endif // _CDF_Directory_HeaderFile
