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

#include <Transfer_ActorOfProcessForFinder.hxx>

#include <Standard_DomainError.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <Transfer_ActorOfProcessForFinder.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_Finder.hxx>
#include <Transfer_FindHasher.hxx>
#include <Transfer_IteratorOfProcessForFinder.hxx>
#include <Transfer_ProcessForFinder.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <Transfer_TransferMapOfProcessForFinder.hxx>

//=================================================================================================

Transfer_ActorOfProcessForFinder::Transfer_ActorOfProcessForFinder() {}

//=================================================================================================

Standard_Boolean Transfer_ActorOfProcessForFinder::Recognize(
  const Handle(Transfer_Finder)& /*start*/)
{
  return Standard_True;
}

//=================================================================================================

Handle(Transfer_Binder) Transfer_ActorOfProcessForFinder::Transferring(
  const Handle(Transfer_Finder)& /*start*/,
  const Handle(Transfer_ProcessForFinder)& /*TP*/,
  const Message_ProgressRange& /*theProgress*/)
{
  return NullResult();
}

//=================================================================================================

Handle(Transfer_SimpleBinderOfTransient) Transfer_ActorOfProcessForFinder::TransientResult(
  const Handle(Standard_Transient)& res) const
{
  Handle(Transfer_SimpleBinderOfTransient) binder;
  if (res.IsNull())
    return binder;
  binder = new Transfer_SimpleBinderOfTransient;
  binder->SetResult(res);
  return binder;
}

//=================================================================================================

Handle(Transfer_Binder) Transfer_ActorOfProcessForFinder::NullResult() const
{
  Handle(Transfer_Binder) binder;
  return binder;
}

//=================================================================================================

void Transfer_ActorOfProcessForFinder::SetNext(const Handle(Transfer_ActorOfProcessForFinder)& next)
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

Handle(Transfer_ActorOfProcessForFinder) Transfer_ActorOfProcessForFinder::Next() const
{
  return thenext;
}

//=================================================================================================

void Transfer_ActorOfProcessForFinder::SetLast(const Standard_Boolean mode)
{
  thelast = mode;
}

//=================================================================================================

Standard_Boolean Transfer_ActorOfProcessForFinder::IsLast() const
{
  return thelast;
}
