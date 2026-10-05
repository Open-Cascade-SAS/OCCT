// Created on: 1993-07-07
// Created by: Jean Claude VAUTHIER
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

//! @file GeomLib_DenominatorMultiplierPtr.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.1.0.
//!             Use the underlying type directly instead.

#ifndef _GeomLib_DenominatorMultiplierPtr_HeaderFile
#define _GeomLib_DenominatorMultiplierPtr_HeaderFile

#include <Standard_Macro.hxx>

class GeomLib_DenominatorMultiplier;

Standard_HEADER_DEPRECATED("GeomLib_DenominatorMultiplierPtr.hxx is deprecated since OCCT 8.1.0. "
                           "Use the underlying type directly instead.")

  Standard_DEPRECATED(
    "GeomLib_DenominatorMultiplierPtr is deprecated, use the underlying type directly")
typedef GeomLib_DenominatorMultiplier* GeomLib_DenominatorMultiplierPtr;

#endif // _GeomLib_DenominatorMultiplierPtr_HeaderFile
