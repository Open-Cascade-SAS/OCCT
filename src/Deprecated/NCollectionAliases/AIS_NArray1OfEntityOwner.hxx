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

//! @file AIS_NArray1OfEntityOwner.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_Array1<opencascade::handle<SelectMgr_EntityOwner>> directly instead.

#ifndef _AIS_NArray1OfEntityOwner_hxx
#define _AIS_NArray1OfEntityOwner_hxx

#include <Standard_Macro.hxx>
#include <NCollection_Array1.hxx>
#include <SelectMgr_EntityOwner.hxx>

Standard_HEADER_DEPRECATED(
  "AIS_NArray1OfEntityOwner.hxx is deprecated since OCCT 8.0.0. Use "
  "NCollection_Array1<opencascade::handle<SelectMgr_EntityOwner>> directly.")

  Standard_DEPRECATED("AIS_NArray1OfEntityOwner is deprecated, use "
                      "NCollection_Array1<opencascade::handle<SelectMgr_EntityOwner>> directly")
typedef NCollection_Array1<opencascade::handle<SelectMgr_EntityOwner>> AIS_NArray1OfEntityOwner;

#endif // _AIS_NArray1OfEntityOwner_hxx
