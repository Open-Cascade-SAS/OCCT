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

#ifndef _IGESData_DefType_HeaderFile
#define _IGESData_DefType_HeaderFile

//! Some fields of an IGES entity may be
//! - Undefined
//! - Defined as a positive integer
//! - Defined as a reference to a specialized entity.
//! A typical example of this kind of variation is color.
//! This enumeration allows you to identify which of the above is the case.
//! The semantics of the terms are as follows:
//! - DefVoid indicates that the item contained in the field is undefined
//! - DefValue indicates that the item is defined as an immediate
//! positive integer value (i.e. not a pointer)
//! - DefReference indicates that the item is defined as an entity
//! - DefAny indicates the item could not be determined
//! - ErrorVal indicates that the item is defined as an integer
//! but its value is incorrect (it could be out of range, for example)
//! - ErrorRef indicates that the item is defined as an entity but
//! is not of the required type.
enum IGESData_DefType
{
IGESData_DefVoid,
IGESData_DefValue,
IGESData_DefReference,
IGESData_DefAny,
IGESData_ErrorVal,
IGESData_ErrorRef
};

#endif // _IGESData_DefType_HeaderFile
