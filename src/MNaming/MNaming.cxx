// Created by: DAUTRY Philippe
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
#include <MNaming_NamingRetrievalDriver_2.hxx>
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
  aDriverSeq->Append(new MNaming_NamingRetrievalDriver_2(theMsgDriver));
}
