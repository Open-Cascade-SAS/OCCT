// Copyright (c) 1998-1999 Matra Datavision
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

//! @file FSD_BStream.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.1.0.
//!             Use the underlying type directly instead.

#ifndef _FSD_BStream_HeaderFile
#define _FSD_BStream_HeaderFile

#include <Standard_Macro.hxx>
#include <stdio.h>

Standard_HEADER_DEPRECATED("FSD_BStream.hxx is deprecated since OCCT 8.1.0. "
                           "Use the underlying type directly instead.")

  Standard_DEPRECATED("FSD_BStream is deprecated, use the underlying type directly")
typedef FILE* FSD_BStream;

#endif
