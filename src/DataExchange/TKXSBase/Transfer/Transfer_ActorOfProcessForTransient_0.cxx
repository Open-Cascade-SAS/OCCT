// Created on: 1992-02-03
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
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

#include <Transfer_ActorOfProcessForTransient.hxx>

#include <Standard_DomainError.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_IteratorOfProcessForTransient.hxx>
#include <Transfer_ProcessForTransient.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <Transfer_Binder.hxx>

//=================================================================================================

Transfer_ActorOfProcessForTransient::Transfer_ActorOfProcessForTransient() {}

//=================================================================================================

bool Transfer_ActorOfProcessForTransient::Recognize(
  const occ::handle<Standard_Transient>& /*start*/)
{
  return true;
}

//=================================================================================================

occ::handle<Transfer_Binder> Transfer_ActorOfProcessForTransient::Transferring(
  const occ::handle<Standard_Transient>& /*start*/,
  const occ::handle<Transfer_ProcessForTransient>& /*TP*/,
  const Message_ProgressRange& /*theProgress*/)
{
  return NullResult();
}

//=================================================================================================

occ::handle<Transfer_SimpleBinderOfTransient> Transfer_ActorOfProcessForTransient::TransientResult(
  const occ::handle<Standard_Transient>& res) const
{
  occ::handle<Transfer_SimpleBinderOfTransient> binder;
  if (res.IsNull())
    return binder;
  binder = new Transfer_SimpleBinderOfTransient;
  binder->SetResult(res);
  return binder;
}

//=================================================================================================

occ::handle<Transfer_Binder> Transfer_ActorOfProcessForTransient::NullResult() const
{
  occ::handle<Transfer_Binder> binder;
  return binder;
}

//=================================================================================================

void Transfer_ActorOfProcessForTransient::SetNext(
  const occ::handle<Transfer_ActorOfProcessForTransient>& next)
{
  if (thenext == next)
    return;
  if (thenext.IsNull())
    thenext = next;
  else if (thenext->IsLast())
  {
    next->SetNext(thenext);
    thenext = next;
  }
  else
    thenext->SetNext(next);
}

//=================================================================================================

occ::handle<Transfer_ActorOfProcessForTransient> Transfer_ActorOfProcessForTransient::Next() const
{
  return thenext;
}

//=================================================================================================

void Transfer_ActorOfProcessForTransient::SetLast(const bool mode)
{
  thelast = mode;
}

//=================================================================================================

bool Transfer_ActorOfProcessForTransient::IsLast() const
{
  return thelast;
}
