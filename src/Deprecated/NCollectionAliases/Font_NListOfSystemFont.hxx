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

//! @file Font_NListOfSystemFont.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_List<opencascade::handle<Font_SystemFont>> directly instead.

#ifndef _Font_NListOfSystemFont_hxx
#define _Font_NListOfSystemFont_hxx

#include <Standard_Macro.hxx>
#include <NCollection_List.hxx>
#include <Font_SystemFont.hxx>

Standard_HEADER_DEPRECATED("Font_NListOfSystemFont.hxx is deprecated since OCCT 8.0.0. Use NCollection_List<opencascade::handle<Font_SystemFont>> directly.")

Standard_DEPRECATED("Font_NListOfSystemFont is deprecated, use NCollection_List<opencascade::handle<Font_SystemFont>> directly")
typedef NCollection_List<opencascade::handle<Font_SystemFont>> Font_NListOfSystemFont;

#endif // _Font_NListOfSystemFont_hxx
