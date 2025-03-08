// Copyright (c) 2024 OPEN CASCADE SAS
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

#include <NCollection_Primes.hxx>

#include <Standard_OutOfRange.hxx>

#include <algorithm>
#include <array>

namespace
{
// The array of prime numbers used as consecutive steps for
// size of array of buckets in the map.
// The prime numbers are used for array size with the hope that this will
// lead to less probability of having the same hash codes for
// different map items (note that all hash codes are modulo that size).
// The value of each next step is chosen to be ~2 times greater than previous.
// Though this could be thought as too much, actually the amount of
// memory overhead in that case is only ~15% as compared with total size of
// all auxiliary data structures (each map node takes ~24 bytes),
// and this proves to pay off in performance (see OCC13189).
constexpr std::array<int, 24> THE_PRIME_VECTOR = {
  101,      1009,     2003,     5003,      10007,     20011,     37003,      57037,
  65003,    100019,   209953,   472393,    995329,    2359297,   4478977,    9437185,
  17915905, 35831809, 71663617, 150994945, 301989889, 573308929, 1019215873, 2038431745};
} // namespace

int NCollection_Primes::NextPrimeForMap(const int theN)
{
  auto aResult = std::lower_bound(THE_PRIME_VECTOR.begin(), THE_PRIME_VECTOR.end(), theN + 1);
  if (aResult == THE_PRIME_VECTOR.end())
  {
    throw Standard_OutOfRange("NCollection_Primes::NextPrimeForMap() - requested too big size");
  }
  return *aResult;
}
