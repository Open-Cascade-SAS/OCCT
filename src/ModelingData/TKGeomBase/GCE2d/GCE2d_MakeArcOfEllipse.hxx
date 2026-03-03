// Copyright (c) 2026 OPEN CASCADE SAS
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

//! @file GCE2d_MakeArcOfEllipse.hxx
//! @brief Deprecated compatibility alias.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use GC_MakeArcOfEllipse2d.hxx instead.

#ifndef _GCE2d_MakeArcOfEllipse_HeaderFile
#define _GCE2d_MakeArcOfEllipse_HeaderFile

#include <Standard_Macro.hxx>
#include <GC_MakeArcOfEllipse2d.hxx>

using GCE2d_MakeArcOfEllipse Standard_DEPRECATED(
  "GCE2d_MakeArcOfEllipse is deprecated since OCCT 8.0.0. Use GC_MakeArcOfEllipse2d instead.")
= GC_MakeArcOfEllipse2d;

#endif // _GCE2d_MakeArcOfEllipse_HeaderFile
