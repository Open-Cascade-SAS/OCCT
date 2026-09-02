// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <Standard_SHA256.hxx>

#include <algorithm>
#include <cstring>
#include <limits>

namespace
{
constexpr std::array<uint32_t, 64> THE_ROUND_CONSTANTS = {
  0x428a2f98u, 0x71374491u, 0xb5c0fbcfu, 0xe9b5dba5u, 0x3956c25bu, 0x59f111f1u,
  0x923f82a4u, 0xab1c5ed5u, 0xd807aa98u, 0x12835b01u, 0x243185beu, 0x550c7dc3u,
  0x72be5d74u, 0x80deb1feu, 0x9bdc06a7u, 0xc19bf174u, 0xe49b69c1u, 0xefbe4786u,
  0x0fc19dc6u, 0x240ca1ccu, 0x2de92c6fu, 0x4a7484aau, 0x5cb0a9dcu, 0x76f988dau,
  0x983e5152u, 0xa831c66du, 0xb00327c8u, 0xbf597fc7u, 0xc6e00bf3u, 0xd5a79147u,
  0x06ca6351u, 0x14292967u, 0x27b70a85u, 0x2e1b2138u, 0x4d2c6dfcu, 0x53380d13u,
  0x650a7354u, 0x766a0abbu, 0x81c2c92eu, 0x92722c85u, 0xa2bfe8a1u, 0xa81a664bu,
  0xc24b8b70u, 0xc76c51a3u, 0xd192e819u, 0xd6990624u, 0xf40e3585u, 0x106aa070u,
  0x19a4c116u, 0x1e376c08u, 0x2748774cu, 0x34b0bcb5u, 0x391c0cb3u, 0x4ed8aa4au,
  0x5b9cca4fu, 0x682e6ff3u, 0x748f82eeu, 0x78a5636fu, 0x84c87814u, 0x8cc70208u,
  0x90befffau, 0xa4506cebu, 0xbef9a3f7u, 0xc67178f2u};

constexpr uint32_t rotateRight(const uint32_t theValue, const unsigned int theShift)
{
  return (theValue >> theShift) | (theValue << (32u - theShift));
}

uint32_t readUInt32(const uint8_t* theBytes)
{
  return (static_cast<uint32_t>(theBytes[0]) << 24)
         | (static_cast<uint32_t>(theBytes[1]) << 16)
         | (static_cast<uint32_t>(theBytes[2]) << 8)
         | static_cast<uint32_t>(theBytes[3]);
}

void writeUInt32(const uint32_t theValue, uint8_t* theBytes)
{
  theBytes[0] = static_cast<uint8_t>(theValue >> 24);
  theBytes[1] = static_cast<uint8_t>(theValue >> 16);
  theBytes[2] = static_cast<uint8_t>(theValue >> 8);
  theBytes[3] = static_cast<uint8_t>(theValue);
}
} // namespace

//=================================================================================================

TCollection_AsciiString Standard_SHA256::Digest::ToString() const
{
  static constexpr char THE_HEX[] = "0123456789abcdef";
  char                  aText[65];
  for (size_t anIndex = 0; anIndex < myBytes.size(); ++anIndex)
  {
    aText[anIndex * 2]     = THE_HEX[myBytes[anIndex] >> 4];
    aText[anIndex * 2 + 1] = THE_HEX[myBytes[anIndex] & 0x0fu];
  }
  aText[64] = '\0';
  return TCollection_AsciiString(aText);
}

//=================================================================================================

Standard_SHA256::Standard_SHA256()
    : myState{0x6a09e667u,
              0xbb67ae85u,
              0x3c6ef372u,
              0xa54ff53au,
              0x510e527fu,
              0x9b05688cu,
              0x1f83d9abu,
              0x5be0cd19u}
{
}

//=================================================================================================

bool Standard_SHA256::Append(const void* theData, const size_t theSize)
{
  if (theSize == 0)
  {
    return true;
  }
  if (theData == nullptr)
  {
    return false;
  }

  if constexpr (sizeof(size_t) > sizeof(uint64_t))
  {
    if (theSize > std::numeric_limits<uint64_t>::max())
    {
      return false;
    }
  }
  const uint64_t aSize = static_cast<uint64_t>(theSize);
  if (aSize > std::numeric_limits<uint64_t>::max() - myByteCount)
  {
    return false;
  }

  const uint8_t* aData      = static_cast<const uint8_t*>(theData);
  size_t         aRemaining = theSize;
  myByteCount += aSize;

  if (myBufferSize != 0)
  {
    const size_t aCopied = std::min(aRemaining, myBuffer.size() - myBufferSize);
    std::memcpy(myBuffer.data() + myBufferSize, aData, aCopied);
    myBufferSize += aCopied;
    aData += aCopied;
    aRemaining -= aCopied;
    if (myBufferSize == myBuffer.size())
    {
      processBlock(myBuffer.data());
      myBufferSize = 0;
    }
  }

  while (aRemaining >= myBuffer.size())
  {
    processBlock(aData);
    aData += myBuffer.size();
    aRemaining -= myBuffer.size();
  }

  if (aRemaining != 0)
  {
    std::memcpy(myBuffer.data(), aData, aRemaining);
    myBufferSize = aRemaining;
  }
  return true;
}

//=================================================================================================

bool Standard_SHA256::Finish(Digest& theDigest) const
{
  if (myByteCount > std::numeric_limits<uint64_t>::max() / 8u)
  {
    return false;
  }

  Standard_SHA256 aFinal(*this);
  const uint64_t  aBitCount = myByteCount * 8u;

  aFinal.myBuffer[aFinal.myBufferSize++] = 0x80u;
  if (aFinal.myBufferSize > 56)
  {
    std::fill(aFinal.myBuffer.begin() + static_cast<std::ptrdiff_t>(aFinal.myBufferSize),
              aFinal.myBuffer.end(),
              0u);
    aFinal.processBlock(aFinal.myBuffer.data());
    aFinal.myBufferSize = 0;
  }

  std::fill(aFinal.myBuffer.begin() + static_cast<std::ptrdiff_t>(aFinal.myBufferSize),
            aFinal.myBuffer.begin() + 56,
            0u);
  for (int anIndex = 0; anIndex < 8; ++anIndex)
  {
    aFinal.myBuffer[56 + static_cast<size_t>(anIndex)] =
      static_cast<uint8_t>(aBitCount >> ((7 - anIndex) * 8));
  }
  aFinal.processBlock(aFinal.myBuffer.data());

  for (size_t anIndex = 0; anIndex < aFinal.myState.size(); ++anIndex)
  {
    writeUInt32(aFinal.myState[anIndex], theDigest.myBytes.data() + anIndex * 4);
  }
  return true;
}

//=================================================================================================

bool Standard_SHA256::Hash(const void* theData, const size_t theSize, Digest& theDigest)
{
  Standard_SHA256 aHash;
  return aHash.Append(theData, theSize) && aHash.Finish(theDigest);
}

//=================================================================================================

void Standard_SHA256::processBlock(const uint8_t* theBlock)
{
  std::array<uint32_t, 64> aWords;
  for (size_t anIndex = 0; anIndex < 16; ++anIndex)
  {
    aWords[anIndex] = readUInt32(theBlock + anIndex * 4);
  }
  for (size_t anIndex = 16; anIndex < aWords.size(); ++anIndex)
  {
    const uint32_t aPrev15 = aWords[anIndex - 15];
    const uint32_t aPrev2  = aWords[anIndex - 2];
    const uint32_t aSigma0 =
      rotateRight(aPrev15, 7) ^ rotateRight(aPrev15, 18) ^ (aPrev15 >> 3);
    const uint32_t aSigma1 =
      rotateRight(aPrev2, 17) ^ rotateRight(aPrev2, 19) ^ (aPrev2 >> 10);
    aWords[anIndex] =
      aWords[anIndex - 16] + aSigma0 + aWords[anIndex - 7] + aSigma1;
  }

  uint32_t aA = myState[0];
  uint32_t aB = myState[1];
  uint32_t aC = myState[2];
  uint32_t aD = myState[3];
  uint32_t aE = myState[4];
  uint32_t aF = myState[5];
  uint32_t aG = myState[6];
  uint32_t aH = myState[7];

  for (size_t anIndex = 0; anIndex < aWords.size(); ++anIndex)
  {
    const uint32_t aSum1 =
      rotateRight(aE, 6) ^ rotateRight(aE, 11) ^ rotateRight(aE, 25);
    const uint32_t aChoice = (aE & aF) ^ (~aE & aG);
    const uint32_t aTemp1 =
      aH + aSum1 + aChoice + THE_ROUND_CONSTANTS[anIndex] + aWords[anIndex];
    const uint32_t aSum0 =
      rotateRight(aA, 2) ^ rotateRight(aA, 13) ^ rotateRight(aA, 22);
    const uint32_t aMajority = (aA & aB) ^ (aA & aC) ^ (aB & aC);
    const uint32_t aTemp2    = aSum0 + aMajority;

    aH = aG;
    aG = aF;
    aF = aE;
    aE = aD + aTemp1;
    aD = aC;
    aC = aB;
    aB = aA;
    aA = aTemp1 + aTemp2;
  }

  myState[0] += aA;
  myState[1] += aB;
  myState[2] += aC;
  myState[3] += aD;
  myState[4] += aE;
  myState[5] += aF;
  myState[6] += aG;
  myState[7] += aH;
}
