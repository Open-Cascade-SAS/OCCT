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

//! @file IGESBasic_Array2OfHArray1OfReal.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_Array2<opencascade::handle<TColStd_HArray1OfReal>> directly instead.

#ifndef _IGESBasic_Array2OfHArray1OfReal_hxx
#define _IGESBasic_Array2OfHArray1OfReal_hxx

#include <Standard_Macro.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <NCollection_Array2.hxx>

Standard_HEADER_DEPRECATED("IGESBasic_Array2OfHArray1OfReal.hxx is deprecated since OCCT 8.0.0. Use NCollection_Array2<opencascade::handle<TColStd_HArray1OfReal>> directly.")

Standard_DEPRECATED("IGESBasic_Array2OfHArray1OfReal is deprecated, use NCollection_Array2<opencascade::handle<TColStd_HArray1OfReal>> directly")
typedef NCollection_Array2<opencascade::handle<TColStd_HArray1OfReal>> IGESBasic_Array2OfHArray1OfReal;

#endif // _IGESBasic_Array2OfHArray1OfReal_hxx
