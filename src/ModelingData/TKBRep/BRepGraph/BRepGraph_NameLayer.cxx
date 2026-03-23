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

#include <BRepGraph_NameLayer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepGraph_NameLayer, BRepGraph_Layer)

static const TCollection_AsciiString THE_LAYER_NAME("Name");

//=================================================================================================

const TCollection_AsciiString& BRepGraph_NameLayer::Name() const
{
  return THE_LAYER_NAME;
}

//=================================================================================================

void BRepGraph_NameLayer::SetNodeName(const BRepGraph_NodeId              theNode,
                                      const TCollection_ExtendedString& theName)
{
  myNames.Bind(theNode, theName);
}

//=================================================================================================

const TCollection_ExtendedString* BRepGraph_NameLayer::FindNodeName(const BRepGraph_NodeId theNode) const
{
  return myNames.Seek(theNode);
}

//=================================================================================================

void BRepGraph_NameLayer::RemoveNodeName(const BRepGraph_NodeId theNode)
{
  myNames.UnBind(theNode);
}

//=================================================================================================

void BRepGraph_NameLayer::OnNodeRemoved(const BRepGraph_NodeId theNode,
                                        const BRepGraph_NodeId theReplacement)
{
  if (theReplacement.IsValid())
  {
    // Migration: copy name from old node to replacement (if old has name and replacement doesn't).
    const TCollection_ExtendedString* aName = myNames.Seek(theNode);
    if (aName != nullptr && myNames.Seek(theReplacement) == nullptr)
    {
      myNames.Bind(theReplacement, *aName);
    }
  }
  myNames.UnBind(theNode);
}

//=================================================================================================

void BRepGraph_NameLayer::OnCompact(
  const NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theRemapMap)
{
  NCollection_DataMap<BRepGraph_NodeId, TCollection_ExtendedString> aRemapped;
  for (NCollection_DataMap<BRepGraph_NodeId, TCollection_ExtendedString>::Iterator
         anIter(myNames); anIter.More(); anIter.Next())
  {
    const BRepGraph_NodeId* aNewId = theRemapMap.Seek(anIter.Key());
    if (aNewId != nullptr)
      aRemapped.Bind(*aNewId, anIter.Value());
  }
  myNames = std::move(aRemapped);
}

//=================================================================================================

void BRepGraph_NameLayer::InvalidateAll()
{
  // NameLayer stores concrete values, not cached computations - nothing to invalidate.
}

//=================================================================================================

void BRepGraph_NameLayer::Clear()
{
  myNames.Clear();
}
