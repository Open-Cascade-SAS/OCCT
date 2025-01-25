// Created on: 1992-09-21
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _IFSelect_ReturnStatus_HeaderFile
#define _IFSelect_ReturnStatus_HeaderFile

//! Qualifies an execution status :
//! RetVoid  : normal execution which created nothing, or
//! no data to process
//! RetDone  : normal execution with a result
//! RetError : error in command or input data, no execution
//! RetFail  : execution was run and has failed
//! RetStop  : indicates end or stop (such as Raise)
enum IFSelect_ReturnStatus
{
  IFSelect_RetVoid,
  IFSelect_RetDone,
  IFSelect_RetError,
  IFSelect_RetFail,
  IFSelect_RetStop
};

#endif // _IFSelect_ReturnStatus_HeaderFile
