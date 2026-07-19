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

//! @file TColGeom_HArray2OfSurface.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_HArray2<opencascade::handle<Geom_Surface>> directly instead.

#ifndef _TColGeom_HArray2OfSurface_hxx
#define _TColGeom_HArray2OfSurface_hxx

#include <Standard_Macro.hxx>
#include <TColGeom_Array2OfSurface.hxx>
#include <NCollection_HArray2.hxx>

Standard_HEADER_DEPRECATED("TColGeom_HArray2OfSurface.hxx is deprecated since OCCT 8.0.0. Use "
                           "NCollection_HArray2<opencascade::handle<Geom_Surface>> directly.")

  Standard_DEPRECATED("TColGeom_HArray2OfSurface is deprecated, use "
                      "NCollection_HArray2<opencascade::handle<Geom_Surface>> directly")
typedef NCollection_HArray2<opencascade::handle<Geom_Surface>> TColGeom_HArray2OfSurface;

#endif // _TColGeom_HArray2OfSurface_hxx
