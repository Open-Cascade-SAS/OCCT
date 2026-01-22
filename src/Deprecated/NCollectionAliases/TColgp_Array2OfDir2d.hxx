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

//! @file TColgp_Array2OfDir2d.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_Array2<gp_Dir2d> directly instead.

#ifndef _TColgp_Array2OfDir2d_hxx
#define _TColgp_Array2OfDir2d_hxx

#include <Standard_Macro.hxx>
#include <gp_Dir2d.hxx>
#include <NCollection_Array2.hxx>

Standard_HEADER_DEPRECATED("TColgp_Array2OfDir2d.hxx is deprecated since OCCT 8.0.0. Use NCollection_Array2<gp_Dir2d> directly.")

Standard_DEPRECATED("TColgp_Array2OfDir2d is deprecated, use NCollection_Array2<gp_Dir2d> directly")
typedef NCollection_Array2<gp_Dir2d> TColgp_Array2OfDir2d;

#endif // _TColgp_Array2OfDir2d_hxx
