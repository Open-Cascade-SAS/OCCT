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

#ifndef _PCDM_RetrievalDriver_HeaderFile
#define _PCDM_RetrievalDriver_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <PCDM_Reader.hxx>
#include <PCDM_ReferenceIterator.hxx>
#include <PCDM_SequenceOfReference.hxx>

class CDM_MetaData;
class Message_Messenger;
class TCollection_AsciiString;
class TCollection_ExtendedString;

class PCDM_RetrievalDriver;
DEFINE_STANDARD_HANDLE(PCDM_RetrievalDriver, PCDM_Reader)

class PCDM_RetrievalDriver : public PCDM_Reader
{
  friend Standard_EXPORT void PCDM_ReferenceIterator::Init (const Handle(CDM_MetaData)& aMetaData);

public:
  Standard_EXPORT static Standard_Integer DocumentVersion (
    const TCollection_ExtendedString& theFileName,
    const Handle(Message_Messenger)&  theMsgDriver);

  Standard_EXPORT static Standard_Integer ReferenceCounter (
    const TCollection_ExtendedString& theFileName,
    const Handle(Message_Messenger)&  theMsgDriver);

  Standard_EXPORT void SetFormat (const TCollection_ExtendedString& aformat);

  Standard_EXPORT TCollection_ExtendedString GetFormat() const;

  DEFINE_STANDARD_RTTIEXT(PCDM_RetrievalDriver,PCDM_Reader)

private:
  Standard_EXPORT static void References (
    const TCollection_ExtendedString& theFileName,
    PCDM_SequenceOfReference&         theReferences,
    const Handle(Message_Messenger)&  theMsgDriver);

  TCollection_ExtendedString myFormat;
};

#endif // _PCDM_RetrievalDriver_HeaderFile
