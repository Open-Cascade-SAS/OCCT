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

#include <BRepGraph_LayerSupplementBase.hxx>

#include <BRepGraphSupInc_CopyContext.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepGraph_LayerSupplementBase, BRepGraph_Layer)

//=================================================================================================

void BRepGraph_LayerSupplementBase::OnItemRemoved(const BRepGraph_ItemId theItem) noexcept
{
  BRepGraph_Layer::OnItemRemoved(theItem);
}

//=================================================================================================

void BRepGraph_LayerSupplementBase::OnSupplementRemoved(const BRepGraphSupInc_ItemUID&) noexcept {}

//=================================================================================================

void BRepGraph_LayerSupplementBase::CopyTo(const BRepGraph_CopyRemap&) const
{
  // Supplemental layers copy only after their stores have supplied UID remapping.
}
