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

//! @file HLRBRep_ListOfBPnt2D.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _HLRBRep_ListOfBPnt2D_hxx
#define _HLRBRep_ListOfBPnt2D_hxx

#include <Standard_Macro.hxx>
#include <HLRBRep_BiPnt2D.hxx>
#include <NCollection_List.hxx>

Standard_HEADER_DEPRECATED(
  "HLRBRep_ListOfBPnt2D.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

  Standard_DEPRECATED(
    "HLRBRep_ListOfBPnt2D is deprecated, use NCollection_List<HLRBRep_BiPnt2D> directly")
typedef NCollection_List<HLRBRep_BiPnt2D> HLRBRep_ListOfBPnt2D;
Standard_DEPRECATED("HLRBRep_ListIteratorOfListOfBPnt2D is deprecated, use "
                    "NCollection_List<HLRBRep_BiPnt2D>::Iterator directly")
typedef NCollection_List<HLRBRep_BiPnt2D>::Iterator HLRBRep_ListIteratorOfListOfBPnt2D;

#endif // _HLRBRep_ListOfBPnt2D_hxx
