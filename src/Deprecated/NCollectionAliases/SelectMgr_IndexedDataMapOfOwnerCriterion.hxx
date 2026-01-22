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

//! @file SelectMgr_IndexedDataMapOfOwnerCriterion.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_IndexedDataMap<opencascade::handle<SelectMgr_EntityOwner>,
//!             SelectMgr_SortCriterion> directly instead.

#ifndef _SelectMgr_IndexedDataMapOfOwnerCriterion_hxx
#define _SelectMgr_IndexedDataMapOfOwnerCriterion_hxx

#include <Standard_Macro.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_SortCriterion.hxx>
#include <NCollection_IndexedDataMap.hxx>

Standard_HEADER_DEPRECATED(
  "SelectMgr_IndexedDataMapOfOwnerCriterion.hxx is deprecated since OCCT 8.0.0. Use "
  "NCollection_IndexedDataMap<opencascade::handle<SelectMgr_EntityOwner>, SelectMgr_SortCriterion> "
  "directly.")

  Standard_DEPRECATED("SelectMgr_IndexedDataMapOfOwnerCriterion is deprecated, use "
                      "NCollection_IndexedDataMap<opencascade::handle<SelectMgr_EntityOwner>, "
                      "SelectMgr_SortCriterion> directly")
typedef NCollection_IndexedDataMap<opencascade::handle<SelectMgr_EntityOwner>,
                                   SelectMgr_SortCriterion>
  SelectMgr_IndexedDataMapOfOwnerCriterion;

#endif // _SelectMgr_IndexedDataMapOfOwnerCriterion_hxx
