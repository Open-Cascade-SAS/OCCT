// Created on: 1997-12-18
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

#ifndef _PCDM_Reader_HeaderFile
#define _PCDM_Reader_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <PCDM_ReaderStatus.hxx>
#include <Standard_Transient.hxx>
#include <Standard_IStream.hxx>
#include <Storage_Data.hxx>
#include <Message_ProgressRange.hxx>

class CDM_Document;
class TCollection_ExtendedString;
class CDM_Application;
class PCDM_ReaderFilter;

class PCDM_Reader : public Standard_Transient
{

public:
  //! retrieves the content of the file into a new Document.
  Standard_EXPORT virtual void Read(
    const TCollection_ExtendedString& aFileName,
    const occ::handle<CDM_Document>&       aNewDocument,
    const occ::handle<CDM_Application>&    anApplication,
    const occ::handle<PCDM_ReaderFilter>&  theFilter   = occ::handle<PCDM_ReaderFilter>(),
    const Message_ProgressRange&      theProgress = Message_ProgressRange()) = 0;

  Standard_EXPORT virtual void Read(
    Standard_IStream&                theIStream,
    const occ::handle<Storage_Data>&      theStorageData,
    const occ::handle<CDM_Document>&      theDoc,
    const occ::handle<CDM_Application>&   theApplication,
    const occ::handle<PCDM_ReaderFilter>& theFilter   = occ::handle<PCDM_ReaderFilter>(),
    const Message_ProgressRange&     theProgress = Message_ProgressRange()) = 0;

  PCDM_ReaderStatus GetStatus() const;

  DEFINE_STANDARD_RTTIEXT(PCDM_Reader, Standard_Transient)

protected:
  PCDM_ReaderStatus myReaderStatus;

};

#include <PCDM_Reader.lxx>

#endif // _PCDM_Reader_HeaderFile
