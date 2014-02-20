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

//  General Purpose Include file
//  provides all required Include directives to Load and Transfer (to Cas.Cade)
//  the content of a Step File

#include <StepFile_Read.hxx>
#include <StepData_StepModel.hxx>
#include <Transfer_TransferOutput.hxx>
//#include <Transfer_PersistentProcess.hxx>
//#include <Transfer_IteratorOfPersistentProcess.hxx>
#include <Transfer_Binder.hxx>

//  and, to allow easy DownCasting :
#include <Interface_Macros.hxx>
