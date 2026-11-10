// Copyright (c) 2026 OPEN CASCADE SAS
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

#ifndef NCollection_UtfString_HeaderFile
#define NCollection_UtfString_HeaderFile

#include <Standard_Macro.hxx>

#include <TCollection_UtfString.hxx>
#include <TCollection_UtfStringTool.hxx>

//! Compatibility alias; use TCollection_UtfString in new code.
template <typename Type>
using NCollection_UtfString Standard_DEPRECATED_STD("Use TCollection_UtfString instead") =
  TCollection_UtfString<Type>;

//! Compatibility alias for the null-terminated locale converter.
using NCollection_UtfStringTool Standard_DEPRECATED_STD("Use TCollection_UtfStringTool instead") =
  TCollection_UtfStringTool;

#endif // NCollection_UtfString_HeaderFile
