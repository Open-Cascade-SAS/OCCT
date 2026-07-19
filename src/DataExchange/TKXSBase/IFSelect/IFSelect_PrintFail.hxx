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

#ifndef _IFSelect_PrintFail_HeaderFile
#define _IFSelect_PrintFail_HeaderFile

//! Indicates whether there will
//! be information on warnings as well as on failures. The
//! terms of this enumeration have the following semantics:
//! - IFSelect_FailOnly gives information on failures only
//! - IFSelect_FailAndWarn gives information on both
//! failures and warnings. used to pilot PrintCheckList
enum IFSelect_PrintFail
{
  IFSelect_FailOnly,
  IFSelect_FailAndWarn
};

#endif // _IFSelect_PrintFail_HeaderFile
