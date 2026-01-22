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

//! @file BOPDS_VectorOfInterfVV.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_Vector<BOPDS_InterfVV> directly instead.

#ifndef _BOPDS_VectorOfInterfVV_hxx
#define _BOPDS_VectorOfInterfVV_hxx

#include <Standard_Macro.hxx>
#include <NCollection_Vector.hxx>
#include <BOPDS_Interf.hxx>

Standard_HEADER_DEPRECATED("BOPDS_VectorOfInterfVV.hxx is deprecated since OCCT 8.0.0. Use "
                           "NCollection_Vector<BOPDS_InterfVV> directly.")

  Standard_DEPRECATED(
    "BOPDS_VectorOfInterfVV is deprecated, use NCollection_Vector<BOPDS_InterfVV> directly")
typedef NCollection_Vector<BOPDS_InterfVV> BOPDS_VectorOfInterfVV;

#endif // _BOPDS_VectorOfInterfVV_hxx
