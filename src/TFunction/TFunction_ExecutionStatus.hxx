// Created on: 1999-06-10
// Created by: Vladislav ROMASHKO
// Copyright (c) 1999 Matra Datavision
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

#ifndef _TFunction_ExecutionStatus_HeaderFile
#define _TFunction_ExecutionStatus_HeaderFile


enum TFunction_ExecutionStatus
{
TFunction_ES_WrongDefinition,
TFunction_ES_NotExecuted,
TFunction_ES_Executing,
TFunction_ES_Succeeded,
TFunction_ES_Failed
};

#endif // _TFunction_ExecutionStatus_HeaderFile
