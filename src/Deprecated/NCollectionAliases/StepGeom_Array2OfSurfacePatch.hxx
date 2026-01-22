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

//! @file StepGeom_Array2OfSurfacePatch.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_Array2<opencascade::handle<StepGeom_SurfacePatch>> directly instead.

#ifndef _StepGeom_Array2OfSurfacePatch_hxx
#define _StepGeom_Array2OfSurfacePatch_hxx

#include <Standard_Macro.hxx>
#include <StepGeom_SurfacePatch.hxx>
#include <NCollection_Array2.hxx>

Standard_HEADER_DEPRECATED("StepGeom_Array2OfSurfacePatch.hxx is deprecated since OCCT 8.0.0. Use NCollection_Array2<opencascade::handle<StepGeom_SurfacePatch>> directly.")

Standard_DEPRECATED("StepGeom_Array2OfSurfacePatch is deprecated, use NCollection_Array2<opencascade::handle<StepGeom_SurfacePatch>> directly")
typedef NCollection_Array2<opencascade::handle<StepGeom_SurfacePatch>> StepGeom_Array2OfSurfacePatch;

#endif // _StepGeom_Array2OfSurfacePatch_hxx
