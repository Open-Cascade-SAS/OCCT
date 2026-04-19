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

#include <BRepGraph_VersionStamp.hxx>

#include <Standard_UUID.hxx>

#include <cstring>

//=================================================================================================

Standard_GUID BRepGraph_VersionStamp::ToGUID(const Standard_GUID& theGraphGUID) const
{
  // Pack fields into a flat byte buffer to avoid struct padding issues.
  const Standard_UUID aGraphUUID = theGraphGUID.ToUUID();
  const uint8_t       aDomain    = static_cast<uint8_t>(myDomain);

  size_t aCounter = 0;
  int    aKind    = 0;
  if (myDomain == Domain::Entity)
  {
    aCounter = myUID.Counter();
    aKind    = static_cast<int>(myUID.Kind());
  }
  else if (myDomain == Domain::Ref)
  {
    aCounter = myRefUID.Counter();
    aKind    = static_cast<int>(myRefUID.Kind());
  }

  uint8_t aBuffer[sizeof(aGraphUUID) + sizeof(aDomain) + sizeof(aCounter) + sizeof(aKind)
                  + sizeof(myMutationGen) + sizeof(myGeneration)];
  size_t  anOff = 0;
  std::memcpy(aBuffer + anOff, &aGraphUUID, sizeof(aGraphUUID));
  anOff += sizeof(aGraphUUID);
  std::memcpy(aBuffer + anOff, &aDomain, sizeof(aDomain));
  anOff += sizeof(aDomain);
  std::memcpy(aBuffer + anOff, &aCounter, sizeof(aCounter));
  anOff += sizeof(aCounter);
  std::memcpy(aBuffer + anOff, &aKind, sizeof(aKind));
  anOff += sizeof(aKind);
  std::memcpy(aBuffer + anOff, &myMutationGen, sizeof(myMutationGen));
  anOff += sizeof(myMutationGen);
  std::memcpy(aBuffer + anOff, &myGeneration, sizeof(myGeneration));
  anOff += sizeof(myGeneration);

  // Four independent hashes fill the 128-bit GUID.
  // Each hash covers a different slice of the input buffer so that
  // the result is well-distributed even on 32-bit platforms where
  // size_t is only 4 bytes.
  const size_t aQuarter = anOff / 4;
  const size_t aHash1 = opencascade::hashBytes(aBuffer, static_cast<int>(aQuarter));
  const size_t aHash2 = opencascade::hashBytes(aBuffer + aQuarter, static_cast<int>(aQuarter));
  const size_t aHash3 = opencascade::hashBytes(aBuffer + aQuarter * 2, static_cast<int>(aQuarter));
  const size_t aHash4 = opencascade::hashBytes(aBuffer + aQuarter * 3,
                                               static_cast<int>(anOff - aQuarter * 3));

  // Pack four hashes into a 128-bit UUID (16 bytes = 4 x 4 bytes).
  // Truncate each size_t hash to uint32_t to avoid buffer overflow on 64-bit
  // platforms where sizeof(size_t) > 4.
  Standard_UUID aResultUUID;
  constexpr size_t THE_QUARTER = sizeof(Standard_UUID) / 4; // 4 bytes
  static_assert(THE_QUARTER == sizeof(uint32_t), "UUID quarter must be 4 bytes");
  const uint32_t aH1 = static_cast<uint32_t>(aHash1);
  const uint32_t aH2 = static_cast<uint32_t>(aHash2);
  const uint32_t aH3 = static_cast<uint32_t>(aHash3);
  const uint32_t aH4 = static_cast<uint32_t>(aHash4);
  uint8_t* aDst = reinterpret_cast<uint8_t*>(&aResultUUID);
  std::memcpy(aDst, &aH1, THE_QUARTER);
  std::memcpy(aDst + THE_QUARTER, &aH2, THE_QUARTER);
  std::memcpy(aDst + THE_QUARTER * 2, &aH3, THE_QUARTER);
  std::memcpy(aDst + THE_QUARTER * 3, &aH4, THE_QUARTER);
  return Standard_GUID(aResultUUID);
}
