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

//! @file SelectMgr_VectorTypes.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _SelectMgr_VectorTypes_hxx
#define _SelectMgr_VectorTypes_hxx

#include <Standard_Macro.hxx>
#include <gp_Trsf.hxx>
#include <NCollection_Mat4.hxx>
#include <NCollection_Vec3.hxx>
#include <NCollection_Vec4.hxx>

Standard_HEADER_DEPRECATED(
  "SelectMgr_VectorTypes.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

  Standard_DEPRECATED("SelectMgr_Vec3 is deprecated, use NCollection_Vec3<double> directly")
typedef NCollection_Vec3<double> SelectMgr_Vec3;
Standard_DEPRECATED("SelectMgr_Vec4 is deprecated, use NCollection_Vec4<double> directly")
typedef NCollection_Vec4<double> SelectMgr_Vec4;
Standard_DEPRECATED("SelectMgr_Mat4 is deprecated, use NCollection_Mat4<double> directly")
typedef NCollection_Mat4<double> SelectMgr_Mat4;

#endif // _SelectMgr_VectorTypes_hxx
