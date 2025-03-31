// Copyright (c) 2023 OPEN CASCADE SAS
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

#ifndef _Standard_HashUtils_HeaderFile
#define _Standard_HashUtils_HeaderFile

#include <Standard_Macro.hxx>

#include <cstdint>
#include <cstddef>
#include <functional>
#include <type_traits>

namespace opencascade
{
//! Implementation of Murmur hash with autodetect of sizeof(size_t).
//!
//! The default value for the seed is optimal for general cases at a certain hash size.
namespace MurmurHash
{
uint32_t MurmurHash2A(const void* theKey, int theLen, uint32_t theSeed);
uint64_t MurmurHash64A(const void* theKey, int theLen, uint64_t theSeed);

template <typename T1, typename T = size_t>
typename std::enable_if<sizeof(T) == 8, uint64_t>::type hash_combine(
  const T1& theValue,
  const int theLen  = sizeof(T1),
  const T   theSeed = 0xA329F1D3A586ULL)
{
  return MurmurHash::MurmurHash64A(&theValue, theLen, theSeed);
}

template <typename T1, typename T = size_t>
typename std::enable_if<sizeof(T) != 8, T>::type hash_combine(const T1& theValue,
                                                              const int theLen  = sizeof(T1),
                                                              const T   theSeed = 0xc70f6907U)
{
  return static_cast<T>(MurmurHash::MurmurHash2A(&theValue, theLen, theSeed));
}

template <typename T = size_t>
constexpr T optimalSeed()
{
  return sizeof(T) == 8 ? static_cast<T>(0xA329F1D3A586ULL) : static_cast<T>(0xc70f6907U);
}
}; // namespace MurmurHash

//! Implementation of FNV-1a with autodetect of sizeof(size_t).
//! This function should work on unsigned char, otherwise it does not
//! correctly implement the FNV-1a algorithm.
//! The existing behaviour is retained for backwards compatibility.
//!
//! The default value for the seed is optimal for general cases at a certain hash size.
namespace FNVHash
{
uint32_t FNVHash1A(const void* theKey, int theLen, uint32_t theSeed);
uint64_t FNVHash64A(const void* theKey, int theLen, uint64_t theSeed);

template <typename T1, typename T = size_t>
static typename std::enable_if<sizeof(T) == 8, uint64_t>::type hash_combine(
  const T1& theValue,
  const int theLen  = sizeof(T1),
  const T   theSeed = 14695981039346656037ULL)
{
  return FNVHash::FNVHash64A(&theValue, theLen, theSeed);
}

template <typename T1, typename T = size_t>
static typename std::enable_if<sizeof(T) != 8, T>::type hash_combine(const T1& theValue,
                                                                     const int theLen = sizeof(T1),
                                                                     const T theSeed  = 2166136261U)
{
  return static_cast<T>(FNVHash::FNVHash1A(&theValue, theLen, theSeed));
}

template <typename T = size_t>
constexpr T optimalSeed()
{
  return sizeof(T) == 8 ? static_cast<T>(14695981039346656037ULL) : static_cast<T>(2166136261U);
}
}; // namespace FNVHash

template <typename T1, typename T = size_t>
T hash(const T1 theValue) noexcept
{
  return opencascade::MurmurHash::hash_combine<T1, T>(theValue);
}

template <typename T1, typename T = size_t>
T hashBytes(const T1* theKey, int theLen)
{
  return opencascade::MurmurHash::hash_combine<T1, T>(*theKey, theLen);
}

template <typename T1, typename T = size_t>
T hash_combine(const T1 theValue, const int theLen, const T theSeed)
{
  return opencascade::MurmurHash::hash_combine<T1, T>(theValue, theLen, theSeed);
}
} // namespace opencascade

#include <Standard_HashUtils.lxx>

#endif
