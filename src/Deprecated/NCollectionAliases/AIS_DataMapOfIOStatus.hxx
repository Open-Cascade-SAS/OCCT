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

//! @file AIS_DataMapOfIOStatus.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _AIS_DataMapOfIOStatus_hxx
#define _AIS_DataMapOfIOStatus_hxx

#include <Standard_Macro.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_GlobalStatus.hxx>
#include <NCollection_DataMap.hxx>

Standard_HEADER_DEPRECATED(
  "AIS_DataMapOfIOStatus.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

  Standard_DEPRECATED("AIS_DataMapOfIOStatus is deprecated, use "
                      "NCollection_DataMap<opencascade::handle<AIS_InteractiveObject>, "
                      "opencascade::handle<AIS_GlobalStatus>> directly")
typedef NCollection_DataMap<opencascade::handle<AIS_InteractiveObject>,
                            opencascade::handle<AIS_GlobalStatus>>
  AIS_DataMapOfIOStatus;
Standard_DEPRECATED("AIS_DataMapIteratorOfDataMapOfIOStatus is deprecated, use "
                    "NCollection_DataMap<opencascade::handle<AIS_InteractiveObject>, "
                    "opencascade::handle<AIS_GlobalStatus>>::Iterator directly")
typedef NCollection_DataMap<opencascade::handle<AIS_InteractiveObject>,
                            opencascade::handle<AIS_GlobalStatus>>::Iterator
  AIS_DataMapIteratorOfDataMapOfIOStatus;

#endif // _AIS_DataMapOfIOStatus_hxx
