// Created on: 1997-11-03
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

#ifndef _PCDM_StorageDriver_HeaderFile
#define _PCDM_StorageDriver_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TCollection_ExtendedString.hxx>
#include <PCDM_StoreStatus.hxx>
#include <PCDM_Writer.hxx>
#include <PCDM_SequenceOfDocument.hxx>
class PCDM_Document;
class CDM_Document;

class PCDM_StorageDriver;
DEFINE_STANDARD_HANDLE(PCDM_StorageDriver, PCDM_Writer)

//! persistent implementation of storage.
//!
//! The  application  must redefine one the two Make()
//! methods.  The first one, if  the application wants to
//! put only one document in the storage file.
//!
//! The  second  method  should  be   redefined  to  put
//! additional document  that   could  be  used by   the
//! retrieval instead of the principal document, depending
//! on the schema used during the retrieval.  For example,
//! a    second     document   could   be     a   standard
//! CDMShape_Document.    This  means   that a   client
//! application will already be able to extract a CDMShape_Document
//! of the file, if the Shape Schema remains unchanged.
class PCDM_StorageDriver : public PCDM_Writer
{

public:
  //! raises NotImplemented.
  Standard_EXPORT virtual Handle(PCDM_Document) Make(const Handle(CDM_Document)& aDocument);

  //! By default, puts  in the Sequence the  document returns
  //! by the previous Make method.
  Standard_EXPORT virtual void Make(const Handle(CDM_Document)& aDocument,
                                    PCDM_SequenceOfDocument&    Documents);

  //! Warning! raises DriverError if an error occurs during inside the
  //! Make method.
  //! stores the content of the Document into a new file.
  //!
  //! by  default  Write will  use Make   method to build a persistent
  //! document and the Schema method to write the persistent document.
  Standard_EXPORT virtual void Write(
    const Handle(CDM_Document)&       aDocument,
    const TCollection_ExtendedString& aFileName,
    const Message_ProgressRange&      theRange = Message_ProgressRange()) Standard_OVERRIDE;

  //! Write <theDocument> to theOStream
  Standard_EXPORT virtual void Write(
    const Handle(CDM_Document)&  theDocument,
    Standard_OStream&            theOStream,
    const Message_ProgressRange& theRange = Message_ProgressRange()) Standard_OVERRIDE;

  Standard_EXPORT void SetFormat(const TCollection_ExtendedString& aformat);

  Standard_EXPORT TCollection_ExtendedString GetFormat() const;

  Standard_EXPORT Standard_Boolean IsError() const;

  Standard_EXPORT void SetIsError(const Standard_Boolean theIsError);

  Standard_EXPORT PCDM_StoreStatus GetStoreStatus() const;

  Standard_EXPORT void SetStoreStatus(const PCDM_StoreStatus theStoreStatus);

  DEFINE_STANDARD_RTTIEXT(PCDM_StorageDriver, PCDM_Writer)

protected:
private:
  TCollection_ExtendedString myFormat;
  Standard_Boolean           myIsError;
  PCDM_StoreStatus           myStoreStatus;
};

#endif // _PCDM_StorageDriver_HeaderFile
