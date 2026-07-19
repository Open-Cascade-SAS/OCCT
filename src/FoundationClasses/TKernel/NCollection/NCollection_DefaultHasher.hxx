// Created by: Eugene Maltchikov
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef NCollection_DefaultHasher_HeaderFile
#define NCollection_DefaultHasher_HeaderFile

#include <functional>

#include <Standard_HashUtils.hxx>
#include <NCollection_DefineHasher.hxx>

/**
 * Purpose:     The  DefaultHasher  is a  Hasher  that is used by
 *              default in NCollection maps.
 *              To compute the  hash code of the key  is used the
 *              global function HashCode.
 *              To compare two keys is used  the  global function
 *              IsEqual.
 */
template <class TheKeyType>
struct NCollection_DefaultHasher
{
  size_t operator()(const TheKeyType& theKey) const noexcept
  {
    return HashCode<TheKeyType>(theKey);
  }

  bool operator()(const TheKeyType& theK1, const TheKeyType& theK2) const noexcept
  {
    return IsEqual<TheKeyType>(theK1, theK2);
  }

private:
  // For non-enums
  template <class T = TheKeyType>
  typename std::enable_if<!std::is_enum<T>::value, size_t>::type HashCode(
    const TheKeyType& theKey) const noexcept
  {
    return std::hash<TheKeyType>{}(theKey);
  }

  // For non-enums
  template <class T = TheKeyType>
  typename std::enable_if<!std::is_enum<T>::value, bool>::type IsEqual(
    const TheKeyType& theK1,
    const TheKeyType& theK2) const noexcept
  {
    return std::equal_to<TheKeyType>{}(theK1, theK2);
  }

  // For enums
  template <class T = TheKeyType>
  typename std::enable_if<std::is_enum<T>::value, size_t>::type HashCode(
    const TheKeyType& theKey) const noexcept
  {
    return static_cast<size_t>(theKey);
  }

  // For enums
  template <class T = TheKeyType>
  typename std::enable_if<std::is_enum<T>::value, bool>::type IsEqual(
    const TheKeyType& theK1,
    const TheKeyType& theK2) const noexcept
  {
    return theK1 == theK2;
  }
};

#define DEFINE_DEFAULT_HASHER_PURE(TheKeyType)                                                     \
  template <>                                                                                      \
  struct NCollection_DefaultHasher<TheKeyType>                                                     \
  {                                                                                                \
    size_t operator()(const TheKeyType theKey) const noexcept                                      \
    {                                                                                              \
      return static_cast<size_t>(theKey);                                                          \
    }                                                                                              \
                                                                                                   \
    bool operator()(const TheKeyType theK1, const TheKeyType theK2) const noexcept                 \
    {                                                                                              \
      return theK1 == theK2;                                                                       \
    }                                                                                              \
  };

/// Explicit specialization for bool.
DEFINE_DEFAULT_HASHER_PURE(bool)

/// Explicit specialization for char.
DEFINE_DEFAULT_HASHER_PURE(char)

/// Explicit specialization for signed char.
DEFINE_DEFAULT_HASHER_PURE(signed char)

/// Explicit specialization for unsigned char.
DEFINE_DEFAULT_HASHER_PURE(unsigned char)

/// Explicit specialization for wchar_t.
DEFINE_DEFAULT_HASHER_PURE(wchar_t)

/// Explicit specialization for char16_t.
DEFINE_DEFAULT_HASHER_PURE(char16_t)

/// Explicit specialization for char32_t.
DEFINE_DEFAULT_HASHER_PURE(char32_t)

/// Explicit specialization for short.
DEFINE_DEFAULT_HASHER_PURE(short)

/// Explicit specialization for int.
DEFINE_DEFAULT_HASHER_PURE(int)

/// Explicit specialization for long.
DEFINE_DEFAULT_HASHER_PURE(long)

/// Explicit specialization for long long.
DEFINE_DEFAULT_HASHER_PURE(long long)

/// Explicit specialization for unsigned short.
DEFINE_DEFAULT_HASHER_PURE(unsigned short)

/// Explicit specialization for unsigned int.
DEFINE_DEFAULT_HASHER_PURE(unsigned int)

/// Explicit specialization for unsigned long.
DEFINE_DEFAULT_HASHER_PURE(unsigned long)

/// Explicit specialization for unsigned long long.
DEFINE_DEFAULT_HASHER_PURE(unsigned long long)

/// Explicit specialization for pointer.
template <class TheKeyType>
struct NCollection_DefaultHasher<TheKeyType*>
{
  size_t operator()(const TheKeyType* theKey) const noexcept
  {
    return static_cast<size_t>(reinterpret_cast<std::uintptr_t>(theKey));
  }

  bool operator()(const TheKeyType* theK1, const TheKeyType* theK2) const noexcept
  {
    return theK1 == theK2;
  }
};

#endif
