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

//! @file Graphic3d_Vec2.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _Graphic3d_Vec2_hxx
#define _Graphic3d_Vec2_hxx

#include <Standard_Macro.hxx>
#include <NCollection_Vec2.hxx>
#include <Standard_TypeDef.hxx>

Standard_HEADER_DEPRECATED("Graphic3d_Vec2.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

Standard_DEPRECATED("Graphic3d_Vec2 is deprecated, use NCollection_Vec2<float> directly")
typedef NCollection_Vec2<float> Graphic3d_Vec2;
Standard_DEPRECATED("Graphic3d_Vec2d is deprecated, use NCollection_Vec2<double> directly")
typedef NCollection_Vec2<double> Graphic3d_Vec2d;
Standard_DEPRECATED("Graphic3d_Vec2i is deprecated, use NCollection_Vec2<int> directly")
typedef NCollection_Vec2<int> Graphic3d_Vec2i;
Standard_DEPRECATED("Graphic3d_Vec2u is deprecated, use NCollection_Vec2<unsigned int> directly")
typedef NCollection_Vec2<unsigned int> Graphic3d_Vec2u;
Standard_DEPRECATED("Graphic3d_Vec2ub is deprecated, use NCollection_Vec2<uint8_t> directly")
typedef NCollection_Vec2<uint8_t> Graphic3d_Vec2ub;
Standard_DEPRECATED("Graphic3d_Vec2b is deprecated, use NCollection_Vec2<char> directly")
typedef NCollection_Vec2<char> Graphic3d_Vec2b;

#endif // _Graphic3d_Vec2_hxx
