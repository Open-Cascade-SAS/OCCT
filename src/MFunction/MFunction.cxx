// Created on: 1999-06-24
// Created by: Vladislav ROMASHKO
// Copyright (c) 1999-1999 Matra Datavision
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

#include <MFunction.ixx>

#include <MDF_ARDriverHSequence.hxx>
#include <MDF_ASDriverHSequence.hxx>
#include <CDM_MessageDriver.hxx>
#include <MFunction_FunctionStorageDriver.hxx>
#include <MFunction_FunctionRetrievalDriver.hxx>


//=======================================================================
//function : AddStorageDrivers
//purpose  : 
//=======================================================================

void MFunction::AddStorageDrivers(const Handle(MDF_ASDriverHSequence)& aDriverSeq, const Handle(CDM_MessageDriver)& theMsgDriver)
{ 
  aDriverSeq->Append(new MFunction_FunctionStorageDriver(theMsgDriver));
}

//=======================================================================
//function : AddRetrievalDrivers
//purpose  : 
//=======================================================================

void MFunction::AddRetrievalDrivers(const Handle(MDF_ARDriverHSequence)& aDriverSeq,const Handle(CDM_MessageDriver)& theMsgDriver)
{
  aDriverSeq->Append(new MFunction_FunctionRetrievalDriver(theMsgDriver));
}
