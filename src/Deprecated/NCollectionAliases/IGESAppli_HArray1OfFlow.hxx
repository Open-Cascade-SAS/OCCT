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

//! @file IGESAppli_HArray1OfFlow.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_HArray1<opencascade::handle<IGESAppli_Flow>> directly instead.

#ifndef _IGESAppli_HArray1OfFlow_hxx
#define _IGESAppli_HArray1OfFlow_hxx

#include <Standard_Macro.hxx>
#include <IGESAppli_Flow.hxx>
#include <IGESAppli_Array1OfFlow.hxx>
#include <NCollection_HArray1.hxx>

Standard_HEADER_DEPRECATED("IGESAppli_HArray1OfFlow.hxx is deprecated since OCCT 8.0.0. Use NCollection_HArray1<opencascade::handle<IGESAppli_Flow>> directly.")

Standard_DEPRECATED("IGESAppli_HArray1OfFlow is deprecated, use NCollection_HArray1<opencascade::handle<IGESAppli_Flow>> directly")
typedef NCollection_HArray1<opencascade::handle<IGESAppli_Flow>> IGESAppli_HArray1OfFlow;

#endif // _IGESAppli_HArray1OfFlow_hxx
