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

//! @file Poly_ListOfTriangulation.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_List<opencascade::handle<Poly_Triangulation>> directly instead.

#ifndef _Poly_ListOfTriangulation_hxx
#define _Poly_ListOfTriangulation_hxx

#include <Standard_Macro.hxx>
#include <Poly_Triangulation.hxx>

Standard_HEADER_DEPRECATED("Poly_ListOfTriangulation.hxx is deprecated since OCCT 8.0.0. Use "
                           "NCollection_List<opencascade::handle<Poly_Triangulation>> directly.")

  Standard_DEPRECATED("Poly_ListOfTriangulation is deprecated, use "
                      "NCollection_List<opencascade::handle<Poly_Triangulation>> directly")
typedef NCollection_List<opencascade::handle<Poly_Triangulation>> Poly_ListOfTriangulation;

#endif // _Poly_ListOfTriangulation_hxx
