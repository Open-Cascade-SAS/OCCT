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


#include <Interface_Macros.hxx>
#include <Standard_Transient.hxx>
#include <StepShape_ClosedShell.hxx>
#include <StepShape_Edge.hxx>
#include <StepShape_Face.hxx>
#include <StepShape_FaceBound.hxx>
#include <StepShape_OpenShell.hxx>
#include <StepShape_Path.hxx>
#include <StepShape_ReversibleTopologyItem.hxx>

StepShape_ReversibleTopologyItem::StepShape_ReversibleTopologyItem () {  }

Standard_Integer StepShape_ReversibleTopologyItem::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepShape_Edge))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepShape_Path))) return 2;
	if (ent->IsKind(STANDARD_TYPE(StepShape_Face))) return 3;
	if (ent->IsKind(STANDARD_TYPE(StepShape_FaceBound))) return 4;
	if (ent->IsKind(STANDARD_TYPE(StepShape_ClosedShell))) return 5;
	if (ent->IsKind(STANDARD_TYPE(StepShape_OpenShell))) return 6;
	return 0;
}

Handle(StepShape_Edge) StepShape_ReversibleTopologyItem::Edge () const
{
	return GetCasted(StepShape_Edge,Value());
}

Handle(StepShape_Path) StepShape_ReversibleTopologyItem::Path () const
{
	return GetCasted(StepShape_Path,Value());
}

Handle(StepShape_Face) StepShape_ReversibleTopologyItem::Face () const
{
	return GetCasted(StepShape_Face,Value());
}

Handle(StepShape_FaceBound) StepShape_ReversibleTopologyItem::FaceBound () const
{
	return GetCasted(StepShape_FaceBound,Value());
}

Handle(StepShape_ClosedShell) StepShape_ReversibleTopologyItem::ClosedShell () const
{
	return GetCasted(StepShape_ClosedShell,Value());
}

Handle(StepShape_OpenShell) StepShape_ReversibleTopologyItem::OpenShell () const
{
	return GetCasted(StepShape_OpenShell,Value());
}
