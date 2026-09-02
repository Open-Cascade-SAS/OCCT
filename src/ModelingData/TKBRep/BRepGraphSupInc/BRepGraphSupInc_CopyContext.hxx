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

#ifndef _BRepGraphSupInc_CopyContext_HeaderFile
#define _BRepGraphSupInc_CopyContext_HeaderFile

#include <BRepGraphSupInc_ItemUID.hxx>
#include <BRepGraph_NodeId.hxx>
#include <NCollection_FlatDataMap.hxx>

class BRepGraphSupInc_Storage;
class BRepGraphSupInc_Store;

//! Copy state shared by standalone supplemental-incidence stores.
//! It records source-to-target runtime UID and core-node mappings needed to copy references
//! between stores.
class BRepGraphSupInc_CopyContext
{
public:
  //! Copy operation mode.
  enum class Mode
  {
    //! Copy into a distinct target storage.
    Copy,
    //! Copy while preparing the target for compaction.
    Compact
  };

  //! Construct a context for one source and one target storage.
  //! @param[in] theSource storage providing records to copy
  //! @param[in,out] theTarget storage receiving copied records
  //! @param[in] theMode requested copy operation mode
  Standard_EXPORT BRepGraphSupInc_CopyContext(const BRepGraphSupInc_Storage& theSource,
                                              BRepGraphSupInc_Storage&       theTarget,
                                              const Mode                     theMode = Mode::Copy);

  //! Return the source storage fixed at construction.
  //! @return source storage
  [[nodiscard]] const BRepGraphSupInc_Storage& Source() const { return mySource; }

  //! Return the target storage fixed at construction.
  //! @return target storage
  [[nodiscard]] BRepGraphSupInc_Storage& Target() const { return myTarget; }

  //! Return the requested copy operation mode.
  //! @return operation mode
  [[nodiscard]] Mode CopyMode() const { return myMode; }

  //! Add a source-to-target runtime UID mapping.
  //! @param[in] theSourceUID active runtime UID in Source()
  //! @param[in] theTargetUID corresponding active runtime UID in Target()
  //! @return false when either UID is invalid or a conflicting mapping exists.
  Standard_EXPORT bool AddMapping(const BRepGraphSupInc_ItemUID& theSourceUID,
                                  const BRepGraphSupInc_ItemUID& theTargetUID);

  //! Return the mapped target UID, or an invalid UID when none exists.
  //! @param[in] theSourceUID source runtime UID to resolve
  //! @return target runtime UID or an invalid UID
  [[nodiscard]] Standard_EXPORT BRepGraphSupInc_ItemUID
    TargetUID(const BRepGraphSupInc_ItemUID& theSourceUID) const;

  //! Return true when a source UID has a target mapping.
  //! @param[in] theSourceUID source runtime UID to query
  //! @return true when a mapping is present
  [[nodiscard]] Standard_EXPORT bool HasTargetUID(
    const BRepGraphSupInc_ItemUID& theSourceUID) const;

  //! Add a source-to-target core-node mapping used by graph-owned stores.
  //! @param[in] theSourceNode source core node
  //! @param[in] theTargetNode target core node
  //! @return false when either node is invalid or an existing mapping conflicts
  Standard_EXPORT bool AddNodeMapping(const BRepGraph_NodeId theSourceNode,
                                      const BRepGraph_NodeId theTargetNode);

  //! Return the mapped target node, or the source node for a distinct-storage copy without mapping.
  //! Compact copies require an explicit node mapping.
  //! @param[in] theSourceNode source core node to resolve
  //! @return mapped target node, identity source node, or an invalid node
  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId
    TargetNode(const BRepGraph_NodeId theSourceNode) const;

  //! Return true when a source core node is explicitly mapped to a target node.
  //! @param[in] theSourceNode source core node to query
  //! @return true when the context contains an explicit node mapping
  [[nodiscard]] Standard_EXPORT bool HasTargetNode(const BRepGraph_NodeId theSourceNode) const;

  //! Return a currently registered target store by its stable GUID.
  //! @param[in] theID stable store-type GUID
  //! @return registered target store or a null handle
  [[nodiscard]] Standard_EXPORT occ::handle<BRepGraphSupInc_Store> TargetStore(
    const Standard_GUID& theID) const;

private:
  friend class BRepGraphSupInc_Storage;

  //! Retain an unregistered target store validated during node-copy preflight.
  [[nodiscard]] bool addPrevalidatedTargetStore(const Standard_GUID&                      theID,
                                                const occ::handle<BRepGraphSupInc_Store>& theStore);

  //! Return an unregistered target store validated during node-copy preflight.
  [[nodiscard]] occ::handle<BRepGraphSupInc_Store> prevalidatedTargetStore(
    const Standard_GUID& theID) const;

private:
  const BRepGraphSupInc_Storage&                                             mySource;
  BRepGraphSupInc_Storage&                                                   myTarget;
  Mode                                                                       myMode;
  NCollection_FlatDataMap<BRepGraphSupInc_ItemUID, BRepGraphSupInc_ItemUID>  myMappings;
  NCollection_FlatDataMap<BRepGraph_NodeId, BRepGraph_NodeId>                myNodeMappings;
  NCollection_FlatDataMap<Standard_GUID, occ::handle<BRepGraphSupInc_Store>> myPrevalidatedStores;
};

#endif // _BRepGraphSupInc_CopyContext_HeaderFile
