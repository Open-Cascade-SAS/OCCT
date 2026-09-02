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

#include <BRepGraph_ItemStamp.hxx>

#include <Standard_HashUtils.hxx>

#include <functional>

bool BRepGraph_ItemStamp::operator==(const BRepGraph_ItemStamp& theOther) const
{
  if (!IsValid() && !theOther.IsValid())
  {
    return true;
  }
  if (myDomain != theOther.myDomain)
  {
    return false;
  }
  if (myMutationGen != theOther.myMutationGen || myGeneration != theOther.myGeneration
      || myRuntimeIdentity != theOther.myRuntimeIdentity)
  {
    return false;
  }
  if (myDomain == Domain::Node)
  {
    return myNodeUID == theOther.myNodeUID;
  }
  if (myDomain == Domain::Reference)
  {
    return myRefUID == theOther.myRefUID;
  }
  return myNodeUID == theOther.myNodeUID && myRefUID == theOther.myRefUID;
}

//=================================================================================================

bool BRepGraph_ItemStamp::IsSameItem(const BRepGraph_ItemStamp& theOther) const
{
  if (myDomain != theOther.myDomain)
  {
    return false;
  }
  if (myDomain == Domain::Node)
  {
    return myNodeUID == theOther.myNodeUID;
  }
  if (myDomain == Domain::Reference)
  {
    return myRefUID == theOther.myRefUID;
  }
  return myNodeUID == theOther.myNodeUID && myRefUID == theOther.myRefUID;
}

//=================================================================================================

size_t BRepGraph_ItemStamp::HashValue() const noexcept
{
  if (!IsValid())
  {
    return opencascade::hash(0);
  }

  const uint32_t aNodeKind =
    myDomain == Domain::Reference ? 0 : static_cast<uint32_t>(myNodeUID.Kind);
  const uint32_t aNodeCounter = myDomain == Domain::Reference ? 0 : myNodeUID.Counter;
  const uint32_t aRefKind     = myDomain == Domain::Node ? 0 : static_cast<uint32_t>(myRefUID.Kind);
  const uint32_t aRefCounter  = myDomain == Domain::Node ? 0 : myRefUID.Counter;
  const uint64_t aCombination[] = {(static_cast<uint64_t>(myDomain) << 32) | aNodeKind,
                                   (static_cast<uint64_t>(aNodeCounter) << 32) | aRefKind,
                                   (static_cast<uint64_t>(aRefCounter) << 32) | myMutationGen,
                                   myGeneration,
                                   std::hash<Standard_GUID>{}(myRuntimeIdentity)};
  return opencascade::hashBytes(aCombination, sizeof(aCombination));
}
