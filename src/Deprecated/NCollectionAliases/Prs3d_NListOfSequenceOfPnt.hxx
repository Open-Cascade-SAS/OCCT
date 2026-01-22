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

//! @file Prs3d_NListOfSequenceOfPnt.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_List<opencascade::handle<TColgp_HSequenceOfPnt>> directly instead.

#ifndef _Prs3d_NListOfSequenceOfPnt_hxx
#define _Prs3d_NListOfSequenceOfPnt_hxx

#include <Standard_Macro.hxx>
#include <TColgp_HSequenceOfPnt.hxx>
#include <NCollection_List.hxx>

Standard_HEADER_DEPRECATED("Prs3d_NListOfSequenceOfPnt.hxx is deprecated since OCCT 8.0.0. Use NCollection_List<opencascade::handle<TColgp_HSequenceOfPnt>> directly.")

Standard_DEPRECATED("Prs3d_NListOfSequenceOfPnt is deprecated, use NCollection_List<opencascade::handle<TColgp_HSequenceOfPnt>> directly")
typedef NCollection_List<opencascade::handle<TColgp_HSequenceOfPnt>> Prs3d_NListOfSequenceOfPnt;

#endif // _Prs3d_NListOfSequenceOfPnt_hxx
