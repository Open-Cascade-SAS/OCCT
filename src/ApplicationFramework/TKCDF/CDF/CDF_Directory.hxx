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

#include <CDM_ListOfDocument.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
class CDM_Document;

class CDF_Directory;
DEFINE_STANDARD_HANDLE(CDF_Directory, Standard_Transient)

//! A directory is a collection of documents. There is only one instance
//! of a given document in a directory.
//! put.
class CDF_Directory : public Standard_Transient
{

public:
  //! Creates an empty directory.
  Standard_EXPORT CDF_Directory();

  //! adds a document into the directory.
  Standard_EXPORT void Add(const Handle(CDM_Document)& aDocument);

  //! removes the document.
  Standard_EXPORT void Remove(const Handle(CDM_Document)& aDocument);

  //! Returns true if the document aDocument is in the directory
  Standard_EXPORT Standard_Boolean Contains(const Handle(CDM_Document)& aDocument) const;

  //! returns the last document (if any) which has been added
  //! in the directory.
  Standard_EXPORT Handle(CDM_Document) Last();

  //! returns the number of documents of the directory.
  Standard_EXPORT Standard_Integer Length() const;

  //! returns true if the directory is empty.
  Standard_EXPORT Standard_Boolean IsEmpty() const;

  friend class CDF_DirectoryIterator;

  DEFINE_STANDARD_RTTIEXT(CDF_Directory, Standard_Transient)

protected:
private:
  Standard_EXPORT const CDM_ListOfDocument& List() const;

  CDM_ListOfDocument myDocuments;
};

#endif // _CDF_Directory_HeaderFile
