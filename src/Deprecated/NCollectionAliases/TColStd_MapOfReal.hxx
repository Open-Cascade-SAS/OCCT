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

//! @file TColStd_MapOfReal.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _TColStd_MapOfReal_hxx
#define _TColStd_MapOfReal_hxx

#include <Standard_Macro.hxx>
#include <NCollection_Map.hxx>

Standard_HEADER_DEPRECATED("TColStd_MapOfReal.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

Standard_DEPRECATED("TColStd_MapOfReal is deprecated, use NCollection_Map<double> directly")
typedef NCollection_Map<double> TColStd_MapOfReal;
Standard_DEPRECATED("TColStd_MapIteratorOfMapOfReal is deprecated, use NCollection_Map<double>::Iterator directly")
typedef NCollection_Map<double>::Iterator TColStd_MapIteratorOfMapOfReal;

#endif // _TColStd_MapOfReal_hxx
