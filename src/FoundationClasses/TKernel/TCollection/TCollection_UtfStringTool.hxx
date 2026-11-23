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

#include <NCollection_LinearVector.hxx>
#include <Standard.hxx>

#include <optional>
#include <string_view>

//! Locale conversion helpers with OCCT-allocated owned results.
//! Locale changes require external synchronization with conversion.
class TCollection_UtfStringTool
{
public:
  using ByteString = NCollection_LinearVector<char>;
  using WideString = NCollection_LinearVector<wchar_t>;

  //! Construct a converter with no retained locale result.
  TCollection_UtfStringTool() = default;

  //! Convert null-terminated locale input; nullptr means empty.
  //! @return borrowed null-terminated wide buffer, or nullptr on failure
  //! The next call or converter destruction invalidates the result.
  Standard_EXPORT wchar_t* FromLocale(const char* theString);

  //! Convert null-terminated wide input to a signed byte capacity.
  //! @return true on success; POSIX empty output returns false
  //! Failure clears the first byte of a positive-size buffer.
  Standard_EXPORT static bool ToLocale(const wchar_t* theString, char* theBuffer, int theCapacity);

  //! Convert a bounded byte string using the C locale (POSIX) or ANSI code page (Windows).
  //! @return owned wide code units, or no value on conversion failure
  Standard_EXPORT static std::optional<WideString> FromLocale(std::string_view theString);

  //! Convert bounded wide code units using the C locale (POSIX) or ANSI code page (Windows).
  //! Windows retains system substitution rules. Allocation failures propagate.
  //! @return owned bytes, or no value on conversion failure
  Standard_EXPORT static std::optional<ByteString> ToLocale(std::wstring_view theString);

private:
  WideString myWideBuffer; //!< Retained null-terminated result of pointer-based conversion.
};

#endif // TCollection_UtfStringTool_HeaderFile
