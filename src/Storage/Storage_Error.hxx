// Created on: 1996-04-30
// Created by: cle
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _Storage_Error_HeaderFile
#define _Storage_Error_HeaderFile

//! Error codes returned by the ErrorStatus
//! function on a Storage_Data set of data during a
//! storage or retrieval operation :
//! -   Storage_VSOk : no problem has been detected
//! -   Storage_VSOpenError : an error has
//! occurred when opening the driver
//! -   Storage_VSModeError : the driver has not
//! been opened in the correct mode
//! -   Storage_VSCloseError : an error has
//! occurred when closing the driver
//! -   Storage_VSAlreadyOpen : the driver is   already open
//! -   Storage_VSNotOpen : the driver is not   open
//! -   Storage_VSSectionNotFound : a section
//! has not been found in the driver
//! -   Storage_VSWriteError : an error occurred when writing the driver
//! -   Storage_VSFormatError : the file format is wrong
//! -   Storage_VSUnknownType : a type is not known from the schema
//! -   Storage_VSTypeMismatch : trying to read   a wrong type
//! -   Storage_VSInternalError : an internal error  has been detected
//! -   Storage_VSExtCharParityError : an error
//! has occurred while reading 16 bit characte
enum Storage_Error
{
Storage_VSOk,
Storage_VSOpenError,
Storage_VSModeError,
Storage_VSCloseError,
Storage_VSAlreadyOpen,
Storage_VSNotOpen,
Storage_VSSectionNotFound,
Storage_VSWriteError,
Storage_VSFormatError,
Storage_VSUnknownType,
Storage_VSTypeMismatch,
Storage_VSInternalError,
Storage_VSExtCharParityError,
Storage_VSWrongFileDriver
};

#endif // _Storage_Error_HeaderFile
