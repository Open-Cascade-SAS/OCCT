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

#ifndef _BRepGraph_RevisionComponent_HeaderFile
#define _BRepGraph_RevisionComponent_HeaderFile

#include <BRepGraph_RevisionStatus.hxx>
#include <BRepGraph_RevisionHash.hxx>
#include <NCollection_LinearVector.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Transient.hxx>

#include <cstdint>

class BRepGraph;

//! Extension point for persistent graph components.
//!
//! A captured component is immutable and provides validation, hashing,
//! serialization, dependency ordering, and restoration of its live data.
class BRepGraph_RevisionComponent : public Standard_Transient
{
public:
  //! Retention policy declared by a live graph extension.
  enum class Retention
  {
    //! The extension has not declared whether its data belongs in a revision.
    Unspecified,
    //! The extension must provide exact immutable capture and restoration.
    Persistent,
    //! The extension is intentionally omitted from immutable revisions.
    Transient
  };

  //! Registry domain of the live component represented by this revision.
  enum class Domain : uint8_t
  {
    SupplementalStore,
    Layer,
    SupplementalLayer
  };

  //! Stable descriptor of a graph component.
  struct ComponentDescriptor
  {
    Standard_GUID StableGUID;
    uint32_t      SchemaVersion   = 1;
    Domain        ComponentDomain = Domain::Layer;
    Retention     RetentionKind   = Retention::Unspecified;
    bool          IsEditable      = false;
    bool          IsArchivable    = false;
  };

  [[nodiscard]] virtual const ComponentDescriptor& Descriptor() const = 0;

  //! Return the runtime generation of this immutable component state.
  [[nodiscard]] uint64_t Generation() const noexcept { return myGeneration; }

  //! Validate this immutable component against a core graph.
  virtual bool Validate(const BRepGraph&, BRepGraph_RevisionStatus::Diagnostics&) const
  {
    return true;
  }

  //! Return the component's deterministic semantic contribution.
  [[nodiscard]] virtual BRepGraph_RevisionHash SemanticHash() const = 0;

  //! Return the component's deterministic physical-storage contribution.
  [[nodiscard]] virtual BRepGraph_RevisionHash StorageHash() const { return SemanticHash(); }

  //! Return stable component GUIDs that must be restored before this component.
  [[nodiscard]] virtual const NCollection_Sequence<Standard_GUID>& Dependencies() const
  {
    static const NCollection_Sequence<Standard_GUID> THE_EMPTY;
    return THE_EMPTY;
  }

  //! Return canonical bytes suitable for content-addressed archival.
  [[nodiscard]] virtual const NCollection_LinearVector<uint8_t>& PersistentBytes() const = 0;

  //! Restore a fresh live component into a graph copy.
  virtual bool Restore(BRepGraph&, BRepGraph_RevisionStatus::Diagnostics&) const = 0;

protected:
  Standard_EXPORT explicit BRepGraph_RevisionComponent(const uint64_t theGeneration = 1);

private:
  uint64_t myGeneration;

public:
  DEFINE_STANDARD_RTTIEXT(BRepGraph_RevisionComponent, Standard_Transient)
};

#endif // _BRepGraph_RevisionComponent_HeaderFile
