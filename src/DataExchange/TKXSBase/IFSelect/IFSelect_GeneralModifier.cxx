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

#include <IFSelect_Dispatch.hxx>
#include <IFSelect_GeneralModifier.hxx>
#include <IFSelect_Selection.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_GeneralModifier, Standard_Transient)

IFSelect_GeneralModifier::IFSelect_GeneralModifier(const bool maychangegraph)
{
  thechgr = maychangegraph;
}

bool IFSelect_GeneralModifier::MayChangeGraph() const
{
  return thechgr;
}

void IFSelect_GeneralModifier::SetDispatch(const occ::handle<IFSelect_Dispatch>& disp)
{
  thedisp = disp;
}

occ::handle<IFSelect_Dispatch> IFSelect_GeneralModifier::Dispatch() const
{
  return thedisp;
}

bool IFSelect_GeneralModifier::Applies(const occ::handle<IFSelect_Dispatch>& disp) const
{
  if (thedisp.IsNull())
    return true;
  if (thedisp != disp)
    return false;
  return true;
}

void IFSelect_GeneralModifier::SetSelection(const occ::handle<IFSelect_Selection>& sel)
{
  thesel = sel;
}

void IFSelect_GeneralModifier::ResetSelection()
{
  thesel.Nullify();
}

bool IFSelect_GeneralModifier::HasSelection() const
{
  return !thesel.IsNull();
}

occ::handle<IFSelect_Selection> IFSelect_GeneralModifier::Selection() const
{
  return thesel;
}
