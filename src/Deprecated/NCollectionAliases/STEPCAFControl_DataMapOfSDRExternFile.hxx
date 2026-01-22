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

//! @file STEPCAFControl_DataMapOfSDRExternFile.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _STEPCAFControl_DataMapOfSDRExternFile_hxx
#define _STEPCAFControl_DataMapOfSDRExternFile_hxx

#include <Standard_Macro.hxx>
#include <StepShape_ShapeDefinitionRepresentation.hxx>
#include <STEPCAFControl_ExternFile.hxx>
#include <NCollection_DataMap.hxx>

Standard_HEADER_DEPRECATED("STEPCAFControl_DataMapOfSDRExternFile.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

Standard_DEPRECATED("STEPCAFControl_DataMapOfSDRExternFile is deprecated, use NCollection_DataMap<opencascade::handle<StepShape_ShapeDefinitionRepresentation>, opencascade::handle<STEPCAFControl_ExternFile>> directly")
typedef NCollection_DataMap<opencascade::handle<StepShape_ShapeDefinitionRepresentation>, opencascade::handle<STEPCAFControl_ExternFile>> STEPCAFControl_DataMapOfSDRExternFile;
Standard_DEPRECATED("STEPCAFControl_DataMapIteratorOfDataMapOfSDRExternFile is deprecated, use NCollection_DataMap<opencascade::handle<StepShape_ShapeDefinitionRepresentation>, opencascade::handle<STEPCAFControl_ExternFile>>::Iterator directly")
typedef NCollection_DataMap<opencascade::handle<StepShape_ShapeDefinitionRepresentation>, opencascade::handle<STEPCAFControl_ExternFile>>::Iterator STEPCAFControl_DataMapIteratorOfDataMapOfSDRExternFile;

#endif // _STEPCAFControl_DataMapOfSDRExternFile_hxx
