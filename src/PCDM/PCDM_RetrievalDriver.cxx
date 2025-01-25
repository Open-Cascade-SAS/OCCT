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

#include <Message_Messenger.hxx>
#include <CDM_MetaData.hxx>
#include <PCDM_ReadWriter.hxx>
#include <PCDM_RetrievalDriver.hxx>
#include <Resource_Manager.hxx>
#include <Standard_Type.hxx>
#include <TCollection_ExtendedString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(PCDM_RetrievalDriver, PCDM_Reader)

void PCDM_RetrievalDriver::References(const TCollection_ExtendedString& aFileName,
                                      PCDM_SequenceOfReference&         theReferences,
                                      const Handle(Message_Messenger)&  theMsgDriver)
{
  PCDM_ReadWriter::Reader(aFileName)->ReadReferences(aFileName, theReferences, theMsgDriver);
}

Standard_Integer PCDM_RetrievalDriver::DocumentVersion(
  const TCollection_ExtendedString& aFileName,
  const Handle(Message_Messenger)&  theMsgDriver)
{
  return PCDM_ReadWriter::Reader(aFileName)->ReadDocumentVersion(aFileName, theMsgDriver);
}

Standard_Integer PCDM_RetrievalDriver::ReferenceCounter(
  const TCollection_ExtendedString& aFileName,
  const Handle(Message_Messenger)&  theMsgDriver)
{
  return PCDM_ReadWriter::Reader(aFileName)->ReadReferenceCounter(aFileName, theMsgDriver);
}

void PCDM_RetrievalDriver::SetFormat(const TCollection_ExtendedString& aformat)
{
  myFormat = aformat;
}

TCollection_ExtendedString PCDM_RetrievalDriver::GetFormat() const
{
  return myFormat;
}
