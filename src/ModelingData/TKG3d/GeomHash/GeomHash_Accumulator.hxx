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

#ifndef _GeomHash_Accumulator_HeaderFile
#define _GeomHash_Accumulator_HeaderFile

#include <Standard_HashUtils.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>

#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <type_traits>

template <class TheHashType = uint64_t>
class GeomHash_Accumulator
{
public:
  static_assert(std::is_same<TheHashType, uint32_t>::value
                  || std::is_same<TheHashType, uint64_t>::value,
                "GeomHash_Accumulator supports uint32_t and uint64_t");

  using result_type = TheHashType;

  GeomHash_Accumulator()
      : myValue(opencascade::FNVHash::optimalSeed<TheHashType>())
  {
  }

  void AppendByte(const uint8_t theValue) { append(&theValue, sizeof(theValue)); }

  void AppendBool(const bool theValue) { AppendByte(theValue ? 1u : 0u); }

  void AppendInt(const int theValue) { AppendUInt32(static_cast<uint32_t>(theValue)); }

  void AppendUInt32(const uint32_t theValue)
  {
    std::array<uint8_t, 4> aBytes;
    for (int anIndex = 0; anIndex < 4; ++anIndex)
    {
      aBytes[static_cast<size_t>(anIndex)] = static_cast<uint8_t>(theValue >> (anIndex * 8));
    }
    append(aBytes.data(), aBytes.size());
  }

  void AppendUInt64(const uint64_t theValue)
  {
    std::array<uint8_t, 8> aBytes;
    for (int anIndex = 0; anIndex < 8; ++anIndex)
    {
      aBytes[static_cast<size_t>(anIndex)] = static_cast<uint8_t>(theValue >> (anIndex * 8));
    }
    append(aBytes.data(), aBytes.size());
  }

  void AppendDouble(double theValue)
  {
    uint64_t aBits = 0;
    if (std::isnan(theValue))
    {
      aBits = 0x7ff8000000000000ull;
    }
    else
    {
      if (theValue == 0.0)
      {
        theValue = 0.0;
      }
      std::memcpy(&aBits, &theValue, sizeof(aBits));
    }

    std::array<uint8_t, 8> aBytes;
    for (int anIndex = 0; anIndex < 8; ++anIndex)
    {
      aBytes[static_cast<size_t>(anIndex)] = static_cast<uint8_t>(aBits >> (anIndex * 8));
    }
    append(aBytes.data(), aBytes.size());
  }

  void AppendFloat(float theValue)
  {
    uint32_t aBits = 0;
    if (std::isnan(theValue))
    {
      aBits = 0x7fc00000u;
    }
    else
    {
      if (theValue == 0.0f)
      {
        theValue = 0.0f;
      }
      std::memcpy(&aBits, &theValue, sizeof(aBits));
    }
    AppendUInt32(aBits);
  }

  void AppendPoint(const gp_Pnt& thePoint)
  {
    AppendDouble(thePoint.X());
    AppendDouble(thePoint.Y());
    AppendDouble(thePoint.Z());
  }

  void AppendPoint2d(const gp_Pnt2d& thePoint)
  {
    AppendDouble(thePoint.X());
    AppendDouble(thePoint.Y());
  }

  result_type Finish() const { return myValue; }

private:
  void append(const void* theData, const size_t theSize)
  {
    if constexpr (sizeof(TheHashType) == 8)
    {
      myValue = opencascade::FNVHash::FNVHash64A(theData, static_cast<int>(theSize), myValue);
    }
    else
    {
      myValue = opencascade::FNVHash::FNVHash1A(theData, static_cast<int>(theSize), myValue);
    }
  }

  TheHashType myValue;
};

#endif // _GeomHash_Accumulator_HeaderFile
