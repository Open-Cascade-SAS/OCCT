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

#ifndef TCollection_UtfStringTool_HeaderFile
#define TCollection_UtfStringTool_HeaderFile

#include <NCollection_Allocator.hxx>
#include <Standard.hxx>

#include <optional>
#include <string>
#include <string_view>

//! Locale conversion helpers with OCCT-allocated owned results.
//! Locale changes require external synchronization with conversion.
class TCollection_UtfStringTool
{
public:
  using ByteString = std::basic_string<char, std::char_traits<char>, NCollection_Allocator<char>>;
  using WideString =
    std::basic_string<wchar_t, std::char_traits<wchar_t>, NCollection_Allocator<wchar_t>>;

  //! Convert a bounded byte string using the C locale (POSIX) or ANSI code page (Windows).
  //! @return owned wide code units, or no value on conversion failure
  Standard_EXPORT static std::optional<WideString> FromLocale(std::string_view theString);

  //! Convert bounded wide code units using the C locale (POSIX) or ANSI code page (Windows).
  //! Windows retains system substitution rules. Allocation failures propagate.
  //! @return owned bytes, or no value on conversion failure
  Standard_EXPORT static std::optional<ByteString> ToLocale(std::wstring_view theString);
};

#endif // TCollection_UtfStringTool_HeaderFile
