// Created on: 1997-08-26
// Created by: Guest Design
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <MPrsStd.ixx>
#include <MDF_ASDriverHSequence.hxx>
#include <MDF_ARDriverHSequence.hxx>
#include <MPrsStd_AISPresentationStorageDriver.hxx>
#include <MPrsStd_AISPresentationRetrievalDriver.hxx>
#include <MPrsStd_AISPresentationRetrievalDriver_1.hxx>
//#include <MPrsStd_AISViewerStorageDriver.hxx>
//#include <MPrsStd_AISViewerRetrievalDriver.hxx> 
#include <MPrsStd_PositionStorageDriver.hxx>
#include <MPrsStd_PositionRetrievalDriver.hxx>

//=======================================================================
//function : AddStorageDriver
//purpose  : 
//=======================================================================

void MPrsStd::AddStorageDrivers
(const Handle(MDF_ASDriverHSequence)& aDriverSeq, const Handle(CDM_MessageDriver)& theMsgDriver)
{
  aDriverSeq->Append(new MPrsStd_AISPresentationStorageDriver(theMsgDriver));
//  aDriverSeq->Append(new MPrsStd_AISViewerStorageDriver());
  aDriverSeq->Append(new MPrsStd_PositionStorageDriver(theMsgDriver));
}


//=======================================================================
//function : AddRetrievalDriver
//purpose  : 
//=======================================================================

void MPrsStd::AddRetrievalDrivers
(const Handle(MDF_ARDriverHSequence)& aDriverSeq, const Handle(CDM_MessageDriver)& theMsgDriver)
{
  aDriverSeq->Append(new MPrsStd_AISPresentationRetrievalDriver(theMsgDriver));
  aDriverSeq->Append(new MPrsStd_AISPresentationRetrievalDriver_1(theMsgDriver));
//  aDriverSeq->Append(new MPrsStd_AISViewerRetrievalDriver());
  aDriverSeq->Append(new MPrsStd_PositionRetrievalDriver(theMsgDriver));
}


