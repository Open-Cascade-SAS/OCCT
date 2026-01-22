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

//! @file IGESGeom_HArray1OfBoundary.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_HArray1<opencascade::handle<IGESGeom_Boundary>> directly instead.

#ifndef _IGESGeom_HArray1OfBoundary_hxx
#define _IGESGeom_HArray1OfBoundary_hxx

#include <Standard_Macro.hxx>
#include <IGESGeom_Array1OfBoundary.hxx>
#include <NCollection_HArray1.hxx>

Standard_HEADER_DEPRECATED("IGESGeom_HArray1OfBoundary.hxx is deprecated since OCCT 8.0.0. Use NCollection_HArray1<opencascade::handle<IGESGeom_Boundary>> directly.")

Standard_DEPRECATED("IGESGeom_HArray1OfBoundary is deprecated, use NCollection_HArray1<opencascade::handle<IGESGeom_Boundary>> directly")
typedef NCollection_HArray1<opencascade::handle<IGESGeom_Boundary>> IGESGeom_HArray1OfBoundary;

#endif // _IGESGeom_HArray1OfBoundary_hxx
