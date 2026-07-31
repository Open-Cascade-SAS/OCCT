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

//! @file gp_Vec2f.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_Vec2<float> directly instead.

#ifndef _gp_Vec2f_hxx
#define _gp_Vec2f_hxx

#include <Standard_Macro.hxx>
#include <NCollection_Vec2.hxx>

Standard_HEADER_DEPRECATED(
  "gp_Vec2f.hxx is deprecated since OCCT 8.0.0. Use NCollection_Vec2<float> directly.")

  Standard_DEPRECATED("gp_Vec2f is deprecated, use NCollection_Vec2<float> directly")
typedef NCollection_Vec2<float> gp_Vec2f;

#endif // _gp_Vec2f_hxx
