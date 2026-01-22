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

//! @file Extrema_HUBTreeOfSphere.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _Extrema_HUBTreeOfSphere_hxx
#define _Extrema_HUBTreeOfSphere_hxx

#include <Standard_Macro.hxx>
#include <NCollection_UBTreeFiller.hxx>
#include <NCollection_Handle.hxx>
#include <Bnd_Sphere.hxx>

Standard_HEADER_DEPRECATED("Extrema_HUBTreeOfSphere.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

Standard_DEPRECATED("Extrema_UBTreeOfSphere is deprecated, use NCollection_UBTree<int, Bnd_Sphere> directly")
typedef NCollection_UBTree<int, Bnd_Sphere> Extrema_UBTreeOfSphere;
Standard_DEPRECATED("Extrema_UBTreeFillerOfSphere is deprecated, use NCollection_UBTreeFiller<int, Bnd_Sphere> directly")
typedef NCollection_UBTreeFiller<int, Bnd_Sphere> Extrema_UBTreeFillerOfSphere;
Standard_DEPRECATED("Extrema_HUBTreeOfSphere is deprecated, use NCollection_Handle<Extrema_UBTreeOfSphere> directly")
typedef NCollection_Handle<Extrema_UBTreeOfSphere> Extrema_HUBTreeOfSphere;

#endif // _Extrema_HUBTreeOfSphere_hxx
