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

//! @file ChFiDS_Regularities.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _ChFiDS_Regularities_hxx
#define _ChFiDS_Regularities_hxx

#include <Standard_Macro.hxx>
#include <ChFiDS_Regul.hxx>
#include <NCollection_List.hxx>

Standard_HEADER_DEPRECATED(
  "ChFiDS_Regularities.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

  Standard_DEPRECATED(
    "ChFiDS_Regularities is deprecated, use NCollection_List<ChFiDS_Regul> directly")
typedef NCollection_List<ChFiDS_Regul> ChFiDS_Regularities;
Standard_DEPRECATED("ChFiDS_ListIteratorOfRegularities is deprecated, use "
                    "NCollection_List<ChFiDS_Regul>::Iterator directly")
typedef NCollection_List<ChFiDS_Regul>::Iterator ChFiDS_ListIteratorOfRegularities;

#endif // _ChFiDS_Regularities_hxx
