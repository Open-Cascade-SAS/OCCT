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

//! @file Bnd_HArray1OfBox2d.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_HArray1<Bnd_Box2d> directly instead.

#ifndef _Bnd_HArray1OfBox2d_hxx
#define _Bnd_HArray1OfBox2d_hxx

#include <Standard_Macro.hxx>
#include <Bnd_Array1OfBox2d.hxx>
#include <NCollection_HArray1.hxx>

Standard_HEADER_DEPRECATED("Bnd_HArray1OfBox2d.hxx is deprecated since OCCT 8.0.0. Use "
                           "NCollection_HArray1<Bnd_Box2d> directly.")

  Standard_DEPRECATED(
    "Bnd_HArray1OfBox2d is deprecated, use NCollection_HArray1<Bnd_Box2d> directly")
typedef NCollection_HArray1<Bnd_Box2d> Bnd_HArray1OfBox2d;

#endif // _Bnd_HArray1OfBox2d_hxx
