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

#include <BRepGraph_Layer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepGraph_Layer, Standard_Transient)

//=================================================================================================

int BRepGraph_Layer::SubscribedKinds() const
{
  return 0;
}

//=================================================================================================

void BRepGraph_Layer::OnNodeModified(const BRepGraph_NodeId /*theNode*/) {}

//=================================================================================================

void BRepGraph_Layer::OnNodesModified(
  const NCollection_Vector<BRepGraph_NodeId>& /*theModifiedNodes*/)
{
}
