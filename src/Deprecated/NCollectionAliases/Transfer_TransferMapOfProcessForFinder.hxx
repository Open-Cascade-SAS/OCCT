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

//! @file Transfer_TransferMapOfProcessForFinder.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_IndexedDataMap<opencascade::handle<Transfer_Finder>,
//!             opencascade::handle<Transfer_Binder>, Transfer_FindHasher> directly instead.

#ifndef _Transfer_TransferMapOfProcessForFinder_hxx
#define _Transfer_TransferMapOfProcessForFinder_hxx

#include <Standard_Macro.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_Finder.hxx>
#include <Transfer_FindHasher.hxx>

Standard_HEADER_DEPRECATED(
  "Transfer_TransferMapOfProcessForFinder.hxx is deprecated since OCCT 8.0.0. Use "
  "NCollection_IndexedDataMap<opencascade::handle<Transfer_Finder>, "
  "opencascade::handle<Transfer_Binder>, Transfer_FindHasher> directly.")

  Standard_DEPRECATED("Transfer_TransferMapOfProcessForFinder is deprecated, use "
                      "NCollection_IndexedDataMap<opencascade::handle<Transfer_Finder>, "
                      "opencascade::handle<Transfer_Binder>, Transfer_FindHasher> directly")
typedef NCollection_IndexedDataMap<opencascade::handle<Transfer_Finder>,
                                   opencascade::handle<Transfer_Binder>,
                                   Transfer_FindHasher>
  Transfer_TransferMapOfProcessForFinder;

#endif // _Transfer_TransferMapOfProcessForFinder_hxx
