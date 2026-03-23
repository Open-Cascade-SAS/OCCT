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

#ifndef _BRepGraph_NameLayer_HeaderFile
#define _BRepGraph_NameLayer_HeaderFile

#include <BRepGraph_Layer.hxx>
#include <TCollection_ExtendedString.hxx>

//! @brief Named layer storing a display name per topology node.
//!
//! Proof-of-concept layer demonstrating the Named Layers infrastructure.
//! Stores TCollection_ExtendedString per BRepGraph_NodeId.
//! Automatically migrates names during sewing, deduplication, and compaction.
class BRepGraph_NameLayer : public BRepGraph_Layer
{
public:
  //! Set a display name for a node.
  Standard_EXPORT void SetNodeName(BRepGraph_NodeId                    theNode,
                                   const TCollection_ExtendedString& theName);

  //! Find the display name for a node. Returns nullptr if not set.
  Standard_EXPORT const TCollection_ExtendedString* FindNodeName(BRepGraph_NodeId theNode) const;

  //! Remove the display name for a node.
  Standard_EXPORT void RemoveNodeName(BRepGraph_NodeId theNode);

  //! Return the number of named nodes.
  int NbNames() const { return myNames.Extent(); }

  // -- BRepGraph_Layer interface --

  Standard_EXPORT const TCollection_AsciiString& Name() const override;

  Standard_EXPORT void OnNodeRemoved(BRepGraph_NodeId theNode,
                                     BRepGraph_NodeId theReplacement) override;

  Standard_EXPORT void OnCompact(const NCollection_DataMap<int, int>& theVertexMap,
                                 const NCollection_DataMap<int, int>& theEdgeMap,
                                 const NCollection_DataMap<int, int>& theWireMap,
                                 const NCollection_DataMap<int, int>& theFaceMap,
                                 const NCollection_DataMap<int, int>& theShellMap,
                                 const NCollection_DataMap<int, int>& theSolidMap) override;

  Standard_EXPORT void InvalidateAll() override;

  Standard_EXPORT void Clear() override;

  DEFINE_STANDARD_RTTIEXT(BRepGraph_NameLayer, BRepGraph_Layer)

private:
  //! Remap a single NodeId through a per-kind index map.
  //! @return remapped NodeId, or invalid if not in map (removed during compact).
  static BRepGraph_NodeId remapNodeId(BRepGraph_NodeId                  theId,
                                      const NCollection_DataMap<int, int>& theMap);

  NCollection_DataMap<BRepGraph_NodeId, TCollection_ExtendedString> myNames;
};

#endif // _BRepGraph_NameLayer_HeaderFile
