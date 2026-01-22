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

//! @file Expr_Array1OfGeneralExpression.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_Array1<opencascade::handle<Expr_GeneralExpression>> directly instead.

#ifndef _Expr_Array1OfGeneralExpression_hxx
#define _Expr_Array1OfGeneralExpression_hxx

#include <Standard_Macro.hxx>
#include <Expr_GeneralExpression.hxx>
#include <NCollection_Array1.hxx>

Standard_HEADER_DEPRECATED("Expr_Array1OfGeneralExpression.hxx is deprecated since OCCT 8.0.0. Use NCollection_Array1<opencascade::handle<Expr_GeneralExpression>> directly.")

Standard_DEPRECATED("Expr_Array1OfGeneralExpression is deprecated, use NCollection_Array1<opencascade::handle<Expr_GeneralExpression>> directly")
typedef NCollection_Array1<opencascade::handle<Expr_GeneralExpression>> Expr_Array1OfGeneralExpression;

#endif // _Expr_Array1OfGeneralExpression_hxx
