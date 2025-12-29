// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <MoniTool_Macros.hxx>
#include <Standard_Transient.hxx>
#include <StepShape_ClosedShell.hxx>
#include <StepShape_Face.hxx>
#include <StepShape_FaceBound.hxx>
#include <StepShape_OpenShell.hxx>
#include <StepShape_Path.hxx>
#include <StepShape_ReversibleTopologyItem.hxx>

StepShape_ReversibleTopologyItem::StepShape_ReversibleTopologyItem() = default;

int StepShape_ReversibleTopologyItem::CaseNum(const occ::handle<Standard_Transient>& ent) const
{
  if (ent.IsNull())
    return 0;
  if (ent->IsKind(STANDARD_TYPE(StepShape_Edge)))
    return 1;
  if (ent->IsKind(STANDARD_TYPE(StepShape_Path)))
    return 2;
  if (ent->IsKind(STANDARD_TYPE(StepShape_Face)))
    return 3;
  if (ent->IsKind(STANDARD_TYPE(StepShape_FaceBound)))
    return 4;
  if (ent->IsKind(STANDARD_TYPE(StepShape_ClosedShell)))
    return 5;
  if (ent->IsKind(STANDARD_TYPE(StepShape_OpenShell)))
    return 6;
  return 0;
}

occ::handle<StepShape_Edge> StepShape_ReversibleTopologyItem::Edge() const
{
  return GetCasted(StepShape_Edge, Value());
}

occ::handle<StepShape_Path> StepShape_ReversibleTopologyItem::Path() const
{
  return GetCasted(StepShape_Path, Value());
}

occ::handle<StepShape_Face> StepShape_ReversibleTopologyItem::Face() const
{
  return GetCasted(StepShape_Face, Value());
}

occ::handle<StepShape_FaceBound> StepShape_ReversibleTopologyItem::FaceBound() const
{
  return GetCasted(StepShape_FaceBound, Value());
}

occ::handle<StepShape_ClosedShell> StepShape_ReversibleTopologyItem::ClosedShell() const
{
  return GetCasted(StepShape_ClosedShell, Value());
}

occ::handle<StepShape_OpenShell> StepShape_ReversibleTopologyItem::OpenShell() const
{
  return GetCasted(StepShape_OpenShell, Value());
}
