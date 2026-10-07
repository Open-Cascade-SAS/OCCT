// Created on: 2016-02-23
// Created by: Kirill Gavrilov
// Copyright (c) 2016 OPEN CASCADE SAS
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

#if defined(_WIN32)
  #include <windows.h>
#endif

#include <TCollection_UtfStringTool.hxx>

#include <array>
#include <climits>
#include <cwchar>

//=================================================================================================

std::optional<TCollection_UtfStringTool::WideString> TCollection_UtfStringTool::FromLocale(
  std::string_view theString)
{
#if defined(_WIN32)
  if (theString.empty())
  {
    return WideString();
  }
  const int aSize = static_cast<int>(theString.size());
  const int aCount =
    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, theString.data(), aSize, nullptr, 0);
  if (aCount <= 0)
  {
    return std::nullopt;
  }
  WideString aResult(static_cast<size_t>(aCount), L'\0');
  if (MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, theString.data(), aSize, aResult.data(), aCount)
      != aCount)
  {
    return std::nullopt;
  }
  return aResult;
#else
  WideString     aResult;
  std::mbstate_t aState{};
  while (!theString.empty())
  {
    wchar_t      aValue = 0;
    const size_t aCount = std::mbrtowc(&aValue, theString.data(), theString.size(), &aState);
    if (aCount == static_cast<size_t>(-1) || aCount == static_cast<size_t>(-2))
    {
      return std::nullopt;
    }
    aResult.push_back(aValue);
    theString.remove_prefix(aCount == 0 ? 1 : aCount);
  }
  return aResult;
#endif
}

//=================================================================================================

std::optional<TCollection_UtfStringTool::ByteString> TCollection_UtfStringTool::ToLocale(
  std::wstring_view theString)
{
#if defined(_WIN32)
  if (theString.empty())
  {
    return ByteString();
  }
  const int aSize = static_cast<int>(theString.size());
  const int aCount =
    WideCharToMultiByte(CP_ACP, 0, theString.data(), aSize, nullptr, 0, nullptr, nullptr);
  if (aCount <= 0)
  {
    return std::nullopt;
  }
  ByteString aResult(static_cast<size_t>(aCount), '\0');
  if (WideCharToMultiByte(CP_ACP,
                          0,
                          theString.data(),
                          aSize,
                          aResult.data(),
                          aCount,
                          nullptr,
                          nullptr)
      != aCount)
  {
    return std::nullopt;
  }
  return aResult;
#else
  ByteString                   aResult;
  std::mbstate_t               aState{};
  std::array<char, MB_LEN_MAX> aBuffer{};
  for (wchar_t aValue : theString)
  {
    const size_t aCount = std::wcrtomb(aBuffer.data(), aValue, &aState);
    if (aCount == static_cast<size_t>(-1))
    {
      return std::nullopt;
    }
    aResult.append(aBuffer.data(), aCount);
  }
  // Emit any final shift sequence; the public caller appends the terminating null.
  const size_t aCount = std::wcrtomb(aBuffer.data(), L'\0', &aState);
  if (aCount == static_cast<size_t>(-1))
  {
    return std::nullopt;
  }
  aResult.append(aBuffer.data(), aCount - 1);
  return aResult;
#endif
}
