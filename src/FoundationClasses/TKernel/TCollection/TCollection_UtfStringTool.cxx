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

#include <algorithm>
#include <array>
#include <utility>
#include <climits>
#include <cwchar>

//==================================================================================================

std::optional<TCollection_UtfStringTool::WideString> TCollection_UtfStringTool::FromLocale(
  std::string_view theString)
{
#if defined(_WIN32)
  if (theString.empty())
  {
    return WideString();
  }
  if (theString.size() > static_cast<size_t>(INT_MAX))
  {
    return std::nullopt;
  }
  const int aSize = static_cast<int>(theString.size());
  const int aCount =
    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, theString.data(), aSize, nullptr, 0);
  if (aCount <= 0)
  {
    return std::nullopt;
  }
  WideString aResult(static_cast<size_t>(aCount), L'\0');
  if (MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, theString.data(), aSize, aResult.Data(), aCount)
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
    aResult.Append(aValue);
    theString.remove_prefix(aCount == 0 ? 1 : aCount);
  }
  return aResult;
#endif
}

//==================================================================================================

std::optional<TCollection_UtfStringTool::ByteString> TCollection_UtfStringTool::ToLocale(
  std::wstring_view theString)
{
#if defined(_WIN32)
  if (theString.empty())
  {
    return ByteString();
  }
  if (theString.size() > static_cast<size_t>(INT_MAX))
  {
    return std::nullopt;
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
                          aResult.Data(),
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
    for (size_t anIndex = 0; anIndex < aCount; ++anIndex)
    {
      aResult.Append(aBuffer[anIndex]);
    }
  }
  // Emit any final shift sequence; the public caller appends the terminating null.
  const size_t aCount = std::wcrtomb(aBuffer.data(), L'\0', &aState);
  if (aCount == static_cast<size_t>(-1))
  {
    return std::nullopt;
  }
  for (size_t anIndex = 0; anIndex + 1 < aCount; ++anIndex)
  {
    aResult.Append(aBuffer[anIndex]);
  }
  return aResult;
#endif
}

//==================================================================================================

wchar_t* TCollection_UtfStringTool::FromLocale(const char* theString)
{
  std::optional<WideString> aResult =
    FromLocale(theString != nullptr ? std::string_view(theString) : std::string_view());
  myWideBuffer.Clear(false);
  if (!aResult)
  {
    return nullptr;
  }
  myWideBuffer = std::move(*aResult);
  myWideBuffer.Append(wchar_t(0));
  return myWideBuffer.Data();
}

//==================================================================================================

bool TCollection_UtfStringTool::ToLocale(const wchar_t* theString, char* theBuffer, int theCapacity)
{
  const std::optional<ByteString> aResult =
    ToLocale(theString != nullptr ? std::wstring_view(theString) : std::wstring_view());
  if (aResult
#if !defined(_WIN32)
      && !aResult->IsEmpty()
#endif
      && theBuffer != nullptr && theCapacity > 0
      && aResult->Size() < static_cast<size_t>(theCapacity))
  {
    std::copy_n(aResult->Data(), aResult->Size(), theBuffer);
    theBuffer[aResult->Size()] = '\0';
    return true;
  }
  if (theBuffer != nullptr && theCapacity > 0)
  {
    theBuffer[0] = '\0';
  }
  return false;
}
