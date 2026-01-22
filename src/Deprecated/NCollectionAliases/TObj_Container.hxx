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

//! @file TObj_Container.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _TObj_Container_hxx
#define _TObj_Container_hxx

#include <Standard_Macro.hxx>
#include <NCollection_DataMap.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HExtendedString.hxx>
#include <TDF_Label.hxx>
#include <TObj_SequenceOfObject.hxx>

Standard_HEADER_DEPRECATED(
  "TObj_Container.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

  Standard_DEPRECATED(
    "TObj_DataMapOfNameLabel is deprecated, use "
    "NCollection_DataMap<opencascade::handle<TCollection_HExtendedString>, TDF_Label> directly")
typedef NCollection_DataMap<opencascade::handle<TCollection_HExtendedString>, TDF_Label>
  TObj_DataMapOfNameLabel;
Standard_DEPRECATED("TObj_DataMapOfObjectHSequenceOcafObjects is deprecated, use "
                    "NCollection_DataMap<opencascade::handle<TObj_Object>, "
                    "opencascade::handle<TObj_HSequenceOfObject>> directly")
typedef NCollection_DataMap<opencascade::handle<TObj_Object>,
                            opencascade::handle<TObj_HSequenceOfObject>>
  TObj_DataMapOfObjectHSequenceOcafObjects;
Standard_DEPRECATED("TObj_DataMapOfStringPointer is deprecated, use "
                    "NCollection_DataMap<TCollection_AsciiString, void*> directly")
typedef NCollection_DataMap<TCollection_AsciiString, void*> TObj_DataMapOfStringPointer;

#endif // _TObj_Container_hxx
