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

//! @file IntTools_DataMapOfSurfaceSampleBox.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _IntTools_DataMapOfSurfaceSampleBox_hxx
#define _IntTools_DataMapOfSurfaceSampleBox_hxx

#include <Standard_Macro.hxx>
#include <IntTools_SurfaceRangeSample.hxx>
#include <Bnd_Box.hxx>
#include <NCollection_DataMap.hxx>

Standard_HEADER_DEPRECATED("IntTools_DataMapOfSurfaceSampleBox.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

Standard_DEPRECATED("IntTools_DataMapOfSurfaceSampleBox is deprecated, use NCollection_DataMap<IntTools_SurfaceRangeSample, Bnd_Box> directly")
typedef NCollection_DataMap<IntTools_SurfaceRangeSample, Bnd_Box> IntTools_DataMapOfSurfaceSampleBox;
Standard_DEPRECATED("IntTools_DataMapIteratorOfDataMapOfSurfaceSampleBox is deprecated, use NCollection_DataMap<IntTools_SurfaceRangeSample, Bnd_Box>::Iterator directly")
typedef NCollection_DataMap<IntTools_SurfaceRangeSample, Bnd_Box>::Iterator IntTools_DataMapIteratorOfDataMapOfSurfaceSampleBox;

#endif // _IntTools_DataMapOfSurfaceSampleBox_hxx
