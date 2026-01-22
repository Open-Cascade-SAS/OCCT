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

//! @file IntPolyh_ListOfCouples.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _IntPolyh_ListOfCouples_hxx
#define _IntPolyh_ListOfCouples_hxx

#include <Standard_Macro.hxx>
#include <NCollection_List.hxx>
#include <IntPolyh_Couple.hxx>

Standard_HEADER_DEPRECATED("IntPolyh_ListOfCouples.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

Standard_DEPRECATED("IntPolyh_ListOfCouples is deprecated, use NCollection_List<IntPolyh_Couple> directly")
typedef NCollection_List<IntPolyh_Couple> IntPolyh_ListOfCouples;
Standard_DEPRECATED("IntPolyh_ListIteratorOfListOfCouples is deprecated, use NCollection_List<IntPolyh_Couple>::Iterator directly")
typedef NCollection_List<IntPolyh_Couple>::Iterator IntPolyh_ListIteratorOfListOfCouples;

#endif // _IntPolyh_ListOfCouples_hxx
