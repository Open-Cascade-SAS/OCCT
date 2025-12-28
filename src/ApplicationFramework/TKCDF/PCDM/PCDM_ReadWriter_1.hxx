// Created on: 1997-12-09
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

#ifndef _PCDM_ReadWriter_1_HeaderFile
#define _PCDM_ReadWriter_1_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <PCDM_ReadWriter.hxx>
#include <Standard_Integer.hxx>
#include <PCDM_Reference.hxx>
#include <NCollection_Sequence.hxx>
#include <TCollection_ExtendedString.hxx>
class TCollection_AsciiString;
class Storage_Data;
class CDM_Document;
class TCollection_ExtendedString;
class Message_Messenger;

class PCDM_ReadWriter_1 : public PCDM_ReadWriter
{

public:
  Standard_EXPORT PCDM_ReadWriter_1();

  //! returns PCDM_ReadWriter_1.
  Standard_EXPORT TCollection_AsciiString Version() const override;

  Standard_EXPORT void WriteReferenceCounter(
    const occ::handle<Storage_Data>& aData,
    const occ::handle<CDM_Document>& aDocument) const override;

  Standard_EXPORT void WriteReferences(
    const occ::handle<Storage_Data>&  aData,
    const occ::handle<CDM_Document>&  aDocument,
    const TCollection_ExtendedString& theReferencerFileName) const override;

  Standard_EXPORT void WriteExtensions(const occ::handle<Storage_Data>& aData,
                                       const occ::handle<CDM_Document>& aDocument) const override;

  Standard_EXPORT void WriteVersion(const occ::handle<Storage_Data>& aData,
                                    const occ::handle<CDM_Document>& aDocument) const override;

  Standard_EXPORT int ReadReferenceCounter(
    const TCollection_ExtendedString&     aFileName,
    const occ::handle<Message_Messenger>& theMsgDriver) const override;

  Standard_EXPORT void ReadReferences(
    const TCollection_ExtendedString&     aFileName,
    NCollection_Sequence<PCDM_Reference>& theReferences,
    const occ::handle<Message_Messenger>& theMsgDriver) const override;

  Standard_EXPORT void ReadExtensions(
    const TCollection_ExtendedString&                 aFileName,
    NCollection_Sequence<TCollection_ExtendedString>& theExtensions,
    const occ::handle<Message_Messenger>&             theMsgDriver) const override;

  Standard_EXPORT int ReadDocumentVersion(
    const TCollection_ExtendedString&     aFileName,
    const occ::handle<Message_Messenger>& theMsgDriver) const override;

  DEFINE_STANDARD_RTTIEXT(PCDM_ReadWriter_1, PCDM_ReadWriter)

private:
  Standard_EXPORT static void ReadUserInfo(
    const TCollection_ExtendedString&                 aFileName,
    const TCollection_AsciiString&                    Start,
    const TCollection_AsciiString&                    End,
    NCollection_Sequence<TCollection_ExtendedString>& theUserInfo,
    const occ::handle<Message_Messenger>&             theMsgDriver);
};

#endif // _PCDM_ReadWriter_1_HeaderFile
