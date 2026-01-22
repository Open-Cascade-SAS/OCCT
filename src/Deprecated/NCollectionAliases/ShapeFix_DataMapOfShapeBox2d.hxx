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

//! @file ShapeFix_DataMapOfShapeBox2d.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _ShapeFix_DataMapOfShapeBox2d_hxx
#define _ShapeFix_DataMapOfShapeBox2d_hxx

#include <Standard_Macro.hxx>
#include <TopoDS_Shape.hxx>
#include <Bnd_Box2d.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>

Standard_HEADER_DEPRECATED("ShapeFix_DataMapOfShapeBox2d.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

Standard_DEPRECATED("ShapeFix_DataMapOfShapeBox2d is deprecated, use NCollection_DataMap<TopoDS_Shape, Bnd_Box2d, TopTools_ShapeMapHasher> directly")
typedef NCollection_DataMap<TopoDS_Shape, Bnd_Box2d, TopTools_ShapeMapHasher> ShapeFix_DataMapOfShapeBox2d;
Standard_DEPRECATED("ShapeFix_DataMapIteratorOfDataMapOfShapeBox2d is deprecated, use NCollection_DataMap<TopoDS_Shape, Bnd_Box2d, TopTools_ShapeMapHasher>::Iterator directly")
typedef NCollection_DataMap<TopoDS_Shape, Bnd_Box2d, TopTools_ShapeMapHasher>::Iterator ShapeFix_DataMapIteratorOfDataMapOfShapeBox2d;

#endif // _ShapeFix_DataMapOfShapeBox2d_hxx
