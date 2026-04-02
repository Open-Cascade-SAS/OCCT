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

  // Two independent hashes fill the 128-bit GUID.
  const size_t aHash1   = opencascade::hashBytes(aBuffer, static_cast<int>(anOff));
  const size_t aHalfOff = sizeof(aGraphUUID);
  const size_t aHash2 =
    opencascade::hashBytes(aBuffer + aHalfOff, static_cast<int>(anOff - aHalfOff));

  Standard_UUID aResultUUID;
  static_assert(sizeof(size_t) >= 8, "Expected 64-bit size_t");
  std::memcpy(&aResultUUID, &aHash1, 8);
  std::memcpy(reinterpret_cast<uint8_t*>(&aResultUUID) + 8, &aHash2, 8);
  return Standard_GUID(aResultUUID);
}
