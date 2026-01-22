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

//! @file Expr_MapOfNamedUnknown.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_IndexedMap<opencascade::handle<Expr_NamedUnknown>> directly instead.

#ifndef _Expr_MapOfNamedUnknown_hxx
#define _Expr_MapOfNamedUnknown_hxx

#include <Standard_Macro.hxx>
#include <Expr_NamedUnknown.hxx>
#include <NCollection_IndexedMap.hxx>

Standard_HEADER_DEPRECATED("Expr_MapOfNamedUnknown.hxx is deprecated since OCCT 8.0.0. Use NCollection_IndexedMap<opencascade::handle<Expr_NamedUnknown>> directly.")

Standard_DEPRECATED("Expr_MapOfNamedUnknown is deprecated, use NCollection_IndexedMap<opencascade::handle<Expr_NamedUnknown>> directly")
typedef NCollection_IndexedMap<opencascade::handle<Expr_NamedUnknown>> Expr_MapOfNamedUnknown;

#endif // _Expr_MapOfNamedUnknown_hxx
