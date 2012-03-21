// Created on: 1997-08-26
// Created by: Guest Design
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



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


