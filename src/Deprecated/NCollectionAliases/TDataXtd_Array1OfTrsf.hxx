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

//! @file TDataXtd_Array1OfTrsf.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_Array1<gp_Trsf> directly instead.

#ifndef _TDataXtd_Array1OfTrsf_hxx
#define _TDataXtd_Array1OfTrsf_hxx

#include <Standard_Macro.hxx>
#include <gp_Trsf.hxx>
#include <NCollection_Array1.hxx>

Standard_HEADER_DEPRECATED("TDataXtd_Array1OfTrsf.hxx is deprecated since OCCT 8.0.0. Use "
                           "NCollection_Array1<gp_Trsf> directly.")

  Standard_DEPRECATED(
    "TDataXtd_Array1OfTrsf is deprecated, use NCollection_Array1<gp_Trsf> directly")
typedef NCollection_Array1<gp_Trsf> TDataXtd_Array1OfTrsf;

#endif // _TDataXtd_Array1OfTrsf_hxx
