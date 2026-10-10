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

#ifndef _BRepGraph_RevisionDiff_HeaderFile
#define _BRepGraph_RevisionDiff_HeaderFile

#include <BRepGraph_ItemId.hxx>
#include <BRepGraph_ItemUID.hxx>
#include <BRepGraph_RefUID.hxx>
#include <BRepGraph_UID.hxx>
#include <NCollection_LinearVector.hxx>
#include <Standard_GUID.hxx>

//! Directional changes from one immutable graph revision to another.
class BRepGraph_RevisionDiff
{
public:
  //! Direction of a reported usage-link change.
  enum class ChangeKind
  {
    Created,
    Modified,
    Removed
  };

  //! A usage-link change. Until a dedicated usage-link UID is available, CoEdge UIDs
  //! are used for topology usage links.
  struct UsageLinkChange
  {
    BRepGraph_UID LinkUID;
    ChangeKind    Kind = ChangeKind::Modified;
  };

  //! A root-product membership change.
  struct RootChange
  {
    BRepGraph_UID ProductUID;
    bool          IsAdded = false;
  };

  //! A local-ID mapping for callers that retain graph-local addresses.
  struct LocalIdRemap
  {
    BRepGraph_ItemUID UID;
    BRepGraph_ItemId  LocalId;
  };

  NCollection_LinearVector<BRepGraph_UID>    CreatedUIDs;
  NCollection_LinearVector<BRepGraph_UID>    ModifiedUIDs;
  NCollection_LinearVector<BRepGraph_UID>    RemovedUIDs;
  NCollection_LinearVector<BRepGraph_RefUID> CreatedRefUIDs;
  NCollection_LinearVector<BRepGraph_RefUID> ModifiedRefUIDs;
  NCollection_LinearVector<BRepGraph_RefUID> RemovedRefUIDs;

  NCollection_LinearVector<UsageLinkChange> UsageLinkChanges;
  NCollection_LinearVector<RootChange>      RootChanges;
  NCollection_LinearVector<Standard_GUID>   LayerChangesByGUID;
  NCollection_LinearVector<Standard_GUID>   SupplementChangesByStoreGUID;
  NCollection_LinearVector<LocalIdRemap>    LocalIdRemapsForDiagnostics;

  //! True when no directional changes were reported.
  [[nodiscard]] bool IsEmpty() const noexcept
  {
    return CreatedUIDs.IsEmpty() && ModifiedUIDs.IsEmpty() && RemovedUIDs.IsEmpty()
           && CreatedRefUIDs.IsEmpty() && ModifiedRefUIDs.IsEmpty() && RemovedRefUIDs.IsEmpty()
           && UsageLinkChanges.IsEmpty() && RootChanges.IsEmpty() && LayerChangesByGUID.IsEmpty()
           && SupplementChangesByStoreGUID.IsEmpty() && LocalIdRemapsForDiagnostics.IsEmpty();
  }

  //! Remove all reported changes.
  void Clear() noexcept
  {
    CreatedUIDs.Clear();
    ModifiedUIDs.Clear();
    RemovedUIDs.Clear();
    CreatedRefUIDs.Clear();
    ModifiedRefUIDs.Clear();
    RemovedRefUIDs.Clear();
    UsageLinkChanges.Clear();
    RootChanges.Clear();
    LayerChangesByGUID.Clear();
    SupplementChangesByStoreGUID.Clear();
    LocalIdRemapsForDiagnostics.Clear();
  }
};

#endif // _BRepGraph_RevisionDiff_HeaderFile
