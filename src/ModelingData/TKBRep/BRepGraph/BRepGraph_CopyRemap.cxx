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

#include <BRepGraph_CopyRemap.hxx>

#include <BRepGraph.hxx>
#include <BRepGraph_UIDsView.hxx>

//=================================================================================================

BRepGraph_CopyRemap::BRepGraph_CopyRemap(const BRepGraph& theSourceGraph,
                                         BRepGraph&       theTargetGraph,
                                         const ItemMap&   theItemRemap,
                                         const Mode       theMode) noexcept
    : mySourceGraph(&theSourceGraph),
      myTargetGraph(&theTargetGraph),
      myItemRemap(&theItemRemap),
      myMode(theMode),
      myMappingKind(MappingKind::Explicit)
{
}

//=================================================================================================

BRepGraph_CopyRemap::BRepGraph_CopyRemap(const BRepGraph&                       theSourceGraph,
                                         BRepGraph&                             theTargetGraph,
                                         const BRepGraph_CopyRemap::MappingKind theMappingKind,
                                         const BRepGraph_CopyRemap::Mode        theMode) noexcept
    : mySourceGraph(&theSourceGraph),
      myTargetGraph(&theTargetGraph),
      myItemRemap(nullptr),
      myMode(theMode),
      myMappingKind(theMappingKind)
{
}

//=================================================================================================

BRepGraph_ItemId BRepGraph_CopyRemap::TargetItem(const BRepGraph_ItemId theSourceItem) const
{
  if (!theSourceItem.IsValid())
  {
    return BRepGraph_ItemId();
  }

  if (myMappingKind == MappingKind::Identity)
  {
    // In identity mode, the source item id IS the target item id.
    return theSourceItem;
  }

  const BRepGraph_ItemId* aFound = myItemRemap->Seek(theSourceItem);
  return aFound != nullptr ? *aFound : BRepGraph_ItemId();
}

//=================================================================================================

BRepGraph_ItemId BRepGraph_CopyRemap::TargetItemOrInvalid(
  const BRepGraph_ItemId theSourceItem) const
{
  return TargetItem(theSourceItem);
}

//=================================================================================================

bool BRepGraph_CopyRemap::HasTargetItem(const BRepGraph_ItemId theSourceItem) const
{
  return TargetItem(theSourceItem).IsValid();
}

//=================================================================================================

BRepGraph_ItemUID BRepGraph_CopyRemap::SourceUID(const BRepGraph_ItemId theSourceItem) const
{
  return mySourceGraph->UIDs().Of(theSourceItem);
}

//=================================================================================================

BRepGraph_ItemUID BRepGraph_CopyRemap::TargetUID(const BRepGraph_ItemId theTargetItem) const
{
  return myTargetGraph->UIDs().Of(theTargetItem);
}

//=================================================================================================

BRepGraph_ItemUID BRepGraph_CopyRemap::TargetUIDFromSource(
  const BRepGraph_ItemId theSourceItem) const
{
  return TargetUID(TargetItemOrInvalid(theSourceItem));
}
