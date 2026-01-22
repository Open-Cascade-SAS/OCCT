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

//! @file TopOpeBRepDS_MapOfPoint.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _TopOpeBRepDS_MapOfPoint_hxx
#define _TopOpeBRepDS_MapOfPoint_hxx

#include <Standard_Macro.hxx>
#include <Standard_Integer.hxx>
#include <TopOpeBRepDS_PointData.hxx>
#include <NCollection_DataMap.hxx>

Standard_HEADER_DEPRECATED(
  "TopOpeBRepDS_MapOfPoint.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

  Standard_DEPRECATED("TopOpeBRepDS_MapOfPoint is deprecated, use NCollection_DataMap<int, "
                      "TopOpeBRepDS_PointData> directly")
typedef NCollection_DataMap<int, TopOpeBRepDS_PointData> TopOpeBRepDS_MapOfPoint;
Standard_DEPRECATED("TopOpeBRepDS_DataMapIteratorOfMapOfPoint is deprecated, use "
                    "NCollection_DataMap<int, TopOpeBRepDS_PointData>::Iterator directly")
typedef NCollection_DataMap<int, TopOpeBRepDS_PointData>::Iterator
  TopOpeBRepDS_DataMapIteratorOfMapOfPoint;

#endif // _TopOpeBRepDS_MapOfPoint_hxx
