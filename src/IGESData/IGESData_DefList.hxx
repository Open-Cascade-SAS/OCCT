// Created on: 1992-04-06
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

#ifndef _IGESData_DefList_HeaderFile
#define _IGESData_DefList_HeaderFile

//! Some fields of an IGES entity may be
//! - Undefined
//! - Defined as a single item
//! - Defined as a list of items.
//! A typical example, which presents this kind of variation,
//! is a level number.
//! This enumeration allows you to identify which of the above is the case.
//! The semantics of the terms is as follows:
//! - DefNone indicates that the list is empty (there is not
//! even a single item).
//! - DefOne indicates that the list contains a single item.
//! - DefSeveral indicates that the list contains several items.
//! - ErrorOne indicates that the list contains one item, but
//! that this item is incorrect
//! - ErrorSeveral indicates that the list contains several
//! items, but that at least one of them is incorrect.
enum IGESData_DefList
{
  IGESData_DefNone,
  IGESData_DefOne,
  IGESData_DefSeveral,
  IGESData_ErrorOne,
  IGESData_ErrorSeveral
};

#endif // _IGESData_DefList_HeaderFile
