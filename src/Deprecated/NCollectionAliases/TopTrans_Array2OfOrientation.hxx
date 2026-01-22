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

//! @file TopTrans_Array2OfOrientation.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_Array2<TopAbs_Orientation> directly instead.

#ifndef _TopTrans_Array2OfOrientation_hxx
#define _TopTrans_Array2OfOrientation_hxx

#include <Standard_Macro.hxx>
#include <TopAbs_Orientation.hxx>
#include <NCollection_Array2.hxx>

Standard_HEADER_DEPRECATED("TopTrans_Array2OfOrientation.hxx is deprecated since OCCT 8.0.0. Use NCollection_Array2<TopAbs_Orientation> directly.")

Standard_DEPRECATED("TopTrans_Array2OfOrientation is deprecated, use NCollection_Array2<TopAbs_Orientation> directly")
typedef NCollection_Array2<TopAbs_Orientation> TopTrans_Array2OfOrientation;

#endif // _TopTrans_Array2OfOrientation_hxx
