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

//! @file TShort_HArray2OfShortReal.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_HArray2<float> directly instead.

#ifndef _TShort_HArray2OfShortReal_hxx
#define _TShort_HArray2OfShortReal_hxx

#include <Standard_Macro.hxx>
#include <Standard_ShortReal.hxx>
#include <TShort_Array2OfShortReal.hxx>
#include <NCollection_HArray2.hxx>

Standard_HEADER_DEPRECATED("TShort_HArray2OfShortReal.hxx is deprecated since OCCT 8.0.0. Use "
                           "NCollection_HArray2<float> directly.")

  Standard_DEPRECATED(
    "TShort_HArray2OfShortReal is deprecated, use NCollection_HArray2<float> directly")
typedef NCollection_HArray2<float> TShort_HArray2OfShortReal;

#endif // _TShort_HArray2OfShortReal_hxx
