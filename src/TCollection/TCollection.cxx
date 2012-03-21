// Created on: 1993-01-14
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <TCollection.ixx>

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

#define NB_PRIMES 12
static const Standard_Integer Primes[NB_PRIMES+1] = {
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
  209953,   // The following are Pierpont primes taken from Wikipedia [List of prime numbers]
  472393,
  995329
};  

Standard_Integer TCollection::NextPrimeForMap(const Standard_Integer N)
{
  Standard_Integer i;
  for (i = 0; i < NB_PRIMES; i++) 
    if (Primes[i] > N) break;
  return Primes[i];
}
