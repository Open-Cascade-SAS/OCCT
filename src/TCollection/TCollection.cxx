// Created on: 1993-01-14
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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

#include <TCollection.hxx>

#include <Standard_OutOfRange.hxx>

// The array of prime numbers used as consequtive steps for 
// size of array of buckets in the map.
// The prime numbers are used for array size with the hope that this will 
// lead to less probablility of having the same hash codes for
// different map items (note that all hash codes are modulo that size).
// The value of each next step is chosen to be ~2 times greater than previous.
// Though this could be thought as too much, actually the amount of 
// memory overhead in that case is only ~15% as compared with total size of
// all auxiliary data structures (each map node takes ~24 bytes), 
// and this proves to pay off in performance (see OCC13189).
#define THE_NB_PRIMES 24
static const Standard_Integer THE_TCollection_Primes[THE_NB_PRIMES] =
{
         101,
        1009,
        2003,
        5003,
       10007,
       20011,
       37003,
       57037,
       65003,
      100019,
      209953, // The following are Pierpont primes [List of prime numbers]
      472393,
      995329,
     2359297,
     4478977,
     9437185,
    17915905,
    35831809,
    71663617,
   150994945,
   301989889,
   573308929,
  1019215873,
  2038431745
};

// =======================================================================
// function : NextPrimeForMap
// purpose  :
// =======================================================================
Standard_Integer TCollection::NextPrimeForMap(const Standard_Integer N)
{
  for (Standard_Integer aPrimeIter = 0; aPrimeIter < THE_NB_PRIMES; ++aPrimeIter)
  {
    if (THE_TCollection_Primes[aPrimeIter] > N)
    {
      return THE_TCollection_Primes[aPrimeIter];
    }
  }
  throw Standard_OutOfRange ("TCollection::NextPrimeForMap() - requested too big size");
}
