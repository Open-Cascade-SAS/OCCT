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

//! @file Approx_HArray1OfAdHSurface.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_HArray1<opencascade::handle<Adaptor3d_Surface>> directly instead.

#ifndef _Approx_HArray1OfAdHSurface_hxx
#define _Approx_HArray1OfAdHSurface_hxx

#include <Standard_Macro.hxx>
#include <Adaptor3d_Surface.hxx>
#include <Approx_Array1OfAdHSurface.hxx>
#include <NCollection_HArray1.hxx>

Standard_HEADER_DEPRECATED("Approx_HArray1OfAdHSurface.hxx is deprecated since OCCT 8.0.0. Use "
                           "NCollection_HArray1<opencascade::handle<Adaptor3d_Surface>> directly.")

  Standard_DEPRECATED("Approx_HArray1OfAdHSurface is deprecated, use "
                      "NCollection_HArray1<opencascade::handle<Adaptor3d_Surface>> directly")
typedef NCollection_HArray1<opencascade::handle<Adaptor3d_Surface>> Approx_HArray1OfAdHSurface;

#endif // _Approx_HArray1OfAdHSurface_hxx
