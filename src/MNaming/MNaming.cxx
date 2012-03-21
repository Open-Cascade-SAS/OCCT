// Created by: DAUTRY Philippe
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

//      	-----------

// Version:	0.0
//Version	Date		Purpose
//		0.0	Aug  4 1997	Creation



#include <MNaming.ixx>

#include <MNaming_NamedShapeStorageDriver.hxx>
#include <MNaming_NamedShapeRetrievalDriver.hxx>
#include <MNaming_NamingStorageDriver.hxx>
#include <MNaming_NamingRetrievalDriver.hxx>
#include <MNaming_NamingRetrievalDriver_1.hxx>
#include <CDM_MessageDriver.hxx>

//=======================================================================
//function : AddStorageDriver
//purpose  : 
//=======================================================================

void MNaming::AddStorageDrivers
(const Handle(MDF_ASDriverHSequence)& aDriverSeq,const Handle(CDM_MessageDriver)& theMsgDriver)
{
  aDriverSeq->Append(new MNaming_NamedShapeStorageDriver(theMsgDriver)); 
  aDriverSeq->Append(new MNaming_NamingStorageDriver(theMsgDriver));
}


//=======================================================================
//function : AddRetrievalDriver
//purpose  : 
//=======================================================================

void MNaming::AddRetrievalDrivers
(const Handle(MDF_ARDriverHSequence)& aDriverSeq,const Handle(CDM_MessageDriver)& theMsgDriver)
{
  aDriverSeq->Append(new MNaming_NamedShapeRetrievalDriver(theMsgDriver)); 
  aDriverSeq->Append(new MNaming_NamingRetrievalDriver(theMsgDriver));
  aDriverSeq->Append(new MNaming_NamingRetrievalDriver_1(theMsgDriver));
}
