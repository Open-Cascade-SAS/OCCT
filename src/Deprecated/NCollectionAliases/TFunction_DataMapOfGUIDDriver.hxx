// Copyright (c) 2025 OPEN CASCADE SAS
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

//! @file TFunction_DataMapOfGUIDDriver.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _TFunction_DataMapOfGUIDDriver_hxx
#define _TFunction_DataMapOfGUIDDriver_hxx

#include <Standard_Macro.hxx>
#include <TFunction_Driver.hxx>
#include <Standard_GUID.hxx>
#include <NCollection_DataMap.hxx>

Standard_HEADER_DEPRECATED("TFunction_DataMapOfGUIDDriver.hxx is deprecated since OCCT 8.0.0. Use "
                           "NCollection types directly.")

  Standard_DEPRECATED(
    "TFunction_DataMapOfGUIDDriver is deprecated, use NCollection_DataMap<Standard_GUID, "
    "opencascade::handle<TFunction_Driver>> directly")
typedef NCollection_DataMap<Standard_GUID, opencascade::handle<TFunction_Driver>>
  TFunction_DataMapOfGUIDDriver;
Standard_DEPRECATED(
  "TFunction_DataMapIteratorOfDataMapOfGUIDDriver is deprecated, use "
  "NCollection_DataMap<Standard_GUID, opencascade::handle<TFunction_Driver>>::Iterator directly")
typedef NCollection_DataMap<Standard_GUID, opencascade::handle<TFunction_Driver>>::Iterator
  TFunction_DataMapIteratorOfDataMapOfGUIDDriver;

#endif // _TFunction_DataMapOfGUIDDriver_hxx
