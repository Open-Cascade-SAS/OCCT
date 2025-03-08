// Copyright (c) 2012 Leonhard Gruenschloss (leonhard@gruenschloss.org)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef _OpenGl_HaltonSampler_H
#define _OpenGl_HaltonSampler_H

#include <vector>

//! Compute points of the Halton sequence with digit-permutations for different bases.
class OpenGl_HaltonSampler
{
public:
  //! Return the number of supported dimensions.
  static unsigned get_num_dimensions() { return 3u; }

public:
  //! Init the permutation arrays using Faure-permutations.
  OpenGl_HaltonSampler() { initFaure(); }

  //! Return the Halton sample for the given dimension (component) and index.
  //! The client must have called initFaure() at least once before.
  //! dimension must be smaller than the value returned by get_num_dimensions().
  float sample(unsigned theDimension, unsigned theIndex) const
  {
    switch (theDimension)
    {
      case 0:
        return halton2(theIndex);
      case 1:
        return halton3(theIndex);
      case 2:
        return halton5(theIndex);
    }
    return 0.0f;
  }

private:
  //! Init the permutation arrays using Faure-permutations.
  void initFaure();

  static unsigned short invert(unsigned short                     theBase,
                               unsigned short                     theDigits,
                               unsigned short                     theIndex,
                               const std::vector<unsigned short>& thePerm)
  {
    unsigned short aResult = 0;
    for (unsigned short i = 0; i < theDigits; ++i)
    {
      aResult = aResult * theBase + thePerm[theIndex % theBase];
      theIndex /= theBase;
    }
    return aResult;
  }

  void initTables(const std::vector<std::vector<unsigned short>>& thePerm)
  {
    for (unsigned short i = 0; i < 243; ++i)
    {
      myPerm3[i] = invert(3, 5, i, thePerm[3]);
    }
    for (unsigned short i = 0; i < 125; ++i)
    {
      myPerm5[i] = invert(5, 3, i, thePerm[5]);
    }
  }

  //! Special case: radical inverse in base 2, with direct bit reversal.
  float halton2(unsigned theIndex) const
  {
    theIndex = (theIndex << 16) | (theIndex >> 16);
    theIndex = ((theIndex & 0x00ff00ff) << 8) | ((theIndex & 0xff00ff00) >> 8);
    theIndex = ((theIndex & 0x0f0f0f0f) << 4) | ((theIndex & 0xf0f0f0f0) >> 4);
    theIndex = ((theIndex & 0x33333333) << 2) | ((theIndex & 0xcccccccc) >> 2);
    theIndex = ((theIndex & 0x55555555) << 1) | ((theIndex & 0xaaaaaaaa) >> 1);

    // clang-format off
    union Result { unsigned u; float f; } aResult; // Write reversed bits directly into floating-point mantissa.

    // clang-format on
    aResult.u = 0x3f800000u | (theIndex >> 9);
    return aResult.f - 1.0f;
  }

  float halton3(unsigned theIndex) const
  {
    return (myPerm3[theIndex % 243u] * 14348907u + myPerm3[(theIndex / 243u) % 243u] * 59049u
            + myPerm3[(theIndex / 59049u) % 243u] * 243u + myPerm3[(theIndex / 14348907u) % 243u])
           * float(0.999999999999999 / 3486784401u); // Results in [0,1).
  }

  float halton5(unsigned theIndex) const
  {
    return (myPerm5[theIndex % 125u] * 1953125u + myPerm5[(theIndex / 125u) % 125u] * 15625u
            + myPerm5[(theIndex / 15625u) % 125u] * 125u + myPerm5[(theIndex / 1953125u) % 125u])
           * float(0.999999999999999 / 244140625u); // Results in [0,1).
  }

private:
  unsigned short myPerm3[243];
  unsigned short myPerm5[125];
};

inline void OpenGl_HaltonSampler::initFaure()
{
  const unsigned                           THE_MAX_BASE = 5u;
  std::vector<std::vector<unsigned short>> aPerms(THE_MAX_BASE + 1);
  for (unsigned k = 1; k <= 3; ++k) // Keep identity permutations for base 1, 2, 3.
  {
    aPerms[k].resize(k);
    for (unsigned i = 0; i < k; ++i)
    {
      aPerms[k][i] = static_cast<unsigned short>(i);
    }
  }

  for (unsigned aBase = 4; aBase <= THE_MAX_BASE; ++aBase)
  {
    aPerms[aBase].resize(aBase);
    const unsigned b = aBase / 2;
    if (aBase & 1) // odd
    {
      for (unsigned i = 0; i < aBase - 1; ++i)
      {
        aPerms[aBase][i + (i >= b)] = aPerms[aBase - 1][i] + (aPerms[aBase - 1][i] >= b);
      }
      aPerms[aBase][b] = static_cast<unsigned short>(b);
    }
    else // even
    {
      for (unsigned i = 0; i < b; ++i)
      {
        aPerms[aBase][i]     = 2 * aPerms[b][i];
        aPerms[aBase][b + i] = 2 * aPerms[b][i] + 1;
      }
    }
  }
  initTables(aPerms);
}

#endif // _OpenGl_HaltonSampler_H
