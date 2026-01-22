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

//! @file CDM_NamesDirectory.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_DataMap<TCollection_ExtendedString, int> directly instead.

#ifndef _CDM_NamesDirectory_hxx
#define _CDM_NamesDirectory_hxx

#include <Standard_Macro.hxx>
#include <TColStd_DataMapOfStringInteger.hxx>

Standard_HEADER_DEPRECATED("CDM_NamesDirectory.hxx is deprecated since OCCT 8.0.0. Use "
                           "NCollection_DataMap<TCollection_ExtendedString, int> directly.")

  Standard_DEPRECATED("CDM_NamesDirectory is deprecated, use "
                      "NCollection_DataMap<TCollection_ExtendedString, int> directly")
typedef NCollection_DataMap<TCollection_ExtendedString, int> CDM_NamesDirectory;

#endif // _CDM_NamesDirectory_hxx
