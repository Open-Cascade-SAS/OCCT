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

#ifndef _NCollection_Primes_HeaderFile
#define _NCollection_Primes_HeaderFile

#include <Standard_Integer.hxx>
#include <Standard_OutOfRange.hxx>

//! Namespace provides a collection of prime numbers.
//!
//! This class is used to store a collection of prime numbers that are used as
//! consecutive steps for the size of an array of buckets in a map. The prime
//! numbers are chosen to minimize the probability of having the same hash codes
//! for different map items. The class also provides a method to find the next
//! prime number greater than or equal to a given number.
namespace NCollection_Primes
{
  //! The array of prime numbers used as consecutive steps for
  //! size of array of buckets in the map.
  //! The prime numbers are used for array size with the hope that this will
  //! lead to less probability of having the same hash codes for
  //! different map items (note that all hash codes are modulo that size).
  //! The value of each next step is chosen to be ~2 times greater than previous.
  //! Though this could be thought as too much, actually the amount of
  //! memory overhead in that case is only ~15% as compared with total size of
  //! all auxiliary data structures (each map node takes ~24 bytes),
  //! and this proves to pay off in performance (see OCC13189).
  //!
  //! The following are Pierpont primes, prime numbers of the form 2^u * 3^v + 1
  static constexpr Standard_Integer PrimeVector[] = {101,      1009,      2003,      5003,      10007,      20011,
                                                     37003,    57037,     65003,     100019,    209953,     472393,
                                                     995329,   2359297,   4478977,   9437185,   17915905,   35831809,
                                                     71663617, 150994945, 301989889, 573308929, 1019215873, 2038431745};

  //! The number of prime numbers in the array.
  static constexpr Standard_Integer NbPrimes      = sizeof(PrimeVector) / sizeof(Standard_Integer);

  //! Returns the next prime number greater than or equal to theN.
  static Standard_Integer NextPrimeForMap(const Standard_Integer theN)
  {
    const Standard_Integer* aLow  = PrimeVector;
    const Standard_Integer* aHigh = PrimeVector + NbPrimes - 1;

    while (aLow <= aHigh)
    {
      const Standard_Integer* aMid = aLow + (aHigh - aLow) / 2;
      if (*aMid > theN)
      {
        if (aMid == PrimeVector || *(aMid - 1) <= theN)
        {
          return *aMid;
        }
        aHigh = aMid - 1;
      }
      else
      {
        aLow = aMid + 1;
      }
    }
    throw Standard_OutOfRange("NCollection_Primes::NextPrimeForMap() - requested too big size");
  }
};

#endif // _NCollection_Primes_HeaderFile
