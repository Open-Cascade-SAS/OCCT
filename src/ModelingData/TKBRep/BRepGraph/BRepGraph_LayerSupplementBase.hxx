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

#ifndef _BRepGraph_LayerSupplementBase_HeaderFile
#define _BRepGraph_LayerSupplementBase_HeaderFile

#include <BRepGraph_Layer.hxx>

class BRepGraphSupInc_CopyContext;
struct BRepGraphSupInc_ItemUID;

//! @brief Base class for layers whose data depends on supplemental stores.
//!
//! Supplemental layers are copied only after the supplemental stores have been
//! migrated. Their dedicated registry calls CopySupplementalTo() with both the
//! core and supplemental remapping contexts.
class BRepGraph_LayerSupplementBase : public BRepGraph_Layer
{
public:
  //! Dispatch a generic item removal to the matching typed removal callback.
  //! This virtual entry point allows supplemental layers to react to complete
  //! core item removal without sharing the core layer registry.
  Standard_EXPORT void OnItemRemoved(const BRepGraph_ItemId theItem) noexcept override;

  //! Called after a supplemental definition has been removed from its store.
  //! Default implementation does nothing.
  Standard_EXPORT virtual void OnSupplementRemoved(const BRepGraphSupInc_ItemUID& theUID) noexcept;

  //! Copy supplemental-aware data after both source stores have migrated.
  //! @param[in] theCopy core source-to-target item mapping
  //! @param[in] theSupplementCopy supplemental source-to-target UID mapping
  Standard_EXPORT virtual void CopySupplementalTo(
    const BRepGraph_CopyRemap&          theCopy,
    const BRepGraphSupInc_CopyContext& theSupplementCopy) const = 0;

  //! Supplemental layers are copied exclusively through CopySupplementalTo().
  Standard_EXPORT void CopyTo(const BRepGraph_CopyRemap& theCopy) const final override;

  DEFINE_STANDARD_RTTIEXT(BRepGraph_LayerSupplementBase, BRepGraph_Layer)
};

#endif // _BRepGraph_LayerSupplementBase_HeaderFile
