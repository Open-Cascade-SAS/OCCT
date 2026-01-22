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

//! @file StepElement_Array1OfVolumeElementPurpose.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_Array1<StepElement_VolumeElementPurpose> directly instead.

#ifndef _StepElement_Array1OfVolumeElementPurpose_hxx
#define _StepElement_Array1OfVolumeElementPurpose_hxx

#include <Standard_Macro.hxx>
#include <StepElement_VolumeElementPurpose.hxx>
#include <NCollection_Array1.hxx>

Standard_HEADER_DEPRECATED("StepElement_Array1OfVolumeElementPurpose.hxx is deprecated since OCCT 8.0.0. Use NCollection_Array1<StepElement_VolumeElementPurpose> directly.")

Standard_DEPRECATED("StepElement_Array1OfVolumeElementPurpose is deprecated, use NCollection_Array1<StepElement_VolumeElementPurpose> directly")
typedef NCollection_Array1<StepElement_VolumeElementPurpose> StepElement_Array1OfVolumeElementPurpose;

#endif // _StepElement_Array1OfVolumeElementPurpose_hxx
