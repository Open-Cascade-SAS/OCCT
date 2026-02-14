// Copyright (c) 2025 OPEN CASCADE SAS
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

#ifndef _MathUtils_Random_HeaderFile
#define _MathUtils_Random_HeaderFile

#include <cstdint>

namespace MathUtils
{

//! High-quality pseudo-random number generator based on xoshiro256**.
//!
//! xoshiro256** is a general-purpose PRNG designed by David Blackman
//! and Sebastiano Vigna. It has:
//! - 256-bit state (period 2^256 - 1)
//! - Passes all BigCrush statistical tests
//! - Very fast on 64-bit hardware
//! - Equidistributed to 4 dimensions
//!
//! Suitable for Monte Carlo methods, stochastic optimization,
//! and any application requiring high-quality randomness.
class RandomGenerator
{
public:
  //! Initialize with a seed value.
  //! Uses SplitMix64 to expand a single seed into the full 256-bit state,
  //! ensuring good initialization even from poor seeds.
  //! @param theSeed seed value (default 1)
  explicit RandomGenerator(const uint64_t theSeed = 1) { SetSeed(theSeed); }

  //! Re-seed the generator.
  //! @param theSeed seed value
  void SetSeed(const uint64_t theSeed)
  {
    // Use SplitMix64 to initialize the state from a single seed.
    // This ensures good state initialization even from sequential seeds.
    uint64_t aSeedState = theSeed;
    myState[0]          = splitmix64(aSeedState);
    myState[1]          = splitmix64(aSeedState);
    myState[2]          = splitmix64(aSeedState);
    myState[3]          = splitmix64(aSeedState);
  }

  //! Generate next 64-bit unsigned integer.
  //! @return pseudo-random value in [0, 2^64)
  uint64_t NextInt()
  {
    const uint64_t aResult = rotl(myState[1] * 5, 7) * 9;
    const uint64_t aT      = myState[1] << 17;

    myState[2] ^= myState[0];
    myState[3] ^= myState[1];
    myState[1] ^= myState[2];
    myState[0] ^= myState[3];

    myState[2] ^= aT;
    myState[3] = rotl(myState[3], 45);

    return aResult;
  }

  //! Generate next double in [0, 1).
  //! Uses 53 bits of randomness for full double precision.
  //! @return pseudo-random value in [0, 1)
  double NextReal() { return static_cast<double>(NextInt() >> 11) * (1.0 / 9007199254740992.0); }

private:
  //! 64-bit left rotation.
  static constexpr uint64_t rotl(const uint64_t theX, const int theK)
  {
    return (theX << theK) | (theX >> (64 - theK));
  }

  //! SplitMix64 step for seed expansion.
  //! Takes a reference to the seed state and advances it.
  static uint64_t splitmix64(uint64_t& theState)
  {
    theState += 0x9e3779b97f4a7c15ULL;
    uint64_t aZ = theState;
    aZ          = (aZ ^ (aZ >> 30)) * 0xbf58476d1ce4e5b9ULL;
    aZ          = (aZ ^ (aZ >> 27)) * 0x94d049bb133111ebULL;
    return aZ ^ (aZ >> 31);
  }

  uint64_t myState[4]; //!< 256-bit generator state
};

} // namespace MathUtils

#endif // _MathUtils_Random_HeaderFile
