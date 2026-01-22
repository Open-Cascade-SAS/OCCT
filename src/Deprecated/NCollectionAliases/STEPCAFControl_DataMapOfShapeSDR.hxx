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

//! @file STEPCAFControl_DataMapOfShapeSDR.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _STEPCAFControl_DataMapOfShapeSDR_hxx
#define _STEPCAFControl_DataMapOfShapeSDR_hxx

#include <Standard_Macro.hxx>
#include <StepShape_ShapeDefinitionRepresentation.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>

Standard_HEADER_DEPRECATED("STEPCAFControl_DataMapOfShapeSDR.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

Standard_DEPRECATED("STEPCAFControl_DataMapOfShapeSDR is deprecated, use NCollection_DataMap<TopoDS_Shape, opencascade::handle<StepShape_ShapeDefinitionRepresentation>, TopTools_ShapeMapHasher> directly")
typedef NCollection_DataMap<TopoDS_Shape, opencascade::handle<StepShape_ShapeDefinitionRepresentation>, TopTools_ShapeMapHasher> STEPCAFControl_DataMapOfShapeSDR;
Standard_DEPRECATED("STEPCAFControl_DataMapIteratorOfDataMapOfShapeSDR is deprecated, use NCollection_DataMap<TopoDS_Shape, opencascade::handle<StepShape_ShapeDefinitionRepresentation>, TopTools_ShapeMapHasher>::Iterator directly")
typedef NCollection_DataMap<TopoDS_Shape, opencascade::handle<StepShape_ShapeDefinitionRepresentation>, TopTools_ShapeMapHasher>::Iterator STEPCAFControl_DataMapIteratorOfDataMapOfShapeSDR;

#endif // _STEPCAFControl_DataMapOfShapeSDR_hxx
