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


#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <Transfer_ActorOfTransientProcess.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_ProcessForTransient.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <Transfer_TransientProcess.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Transfer_ActorOfTransientProcess,Transfer_ActorOfProcessForTransient)

Transfer_ActorOfTransientProcess::Transfer_ActorOfTransientProcess ()    {  }

Handle(Transfer_Binder)  Transfer_ActorOfTransientProcess::Transfer
  (const Handle(Standard_Transient)& start,
   const Handle(Transfer_TransientProcess)& TP)
{
  Handle(Standard_Transient) res = TransferTransient (start,TP);
  if (res.IsNull()) return NullResult();
  return TransientResult (res);
}

Handle(Transfer_Binder)  Transfer_ActorOfTransientProcess::Transferring
  (const Handle(Standard_Transient)& ent,
   const Handle(Transfer_ProcessForTransient)& TP)
{
  return Transfer(ent,Handle(Transfer_TransientProcess)::DownCast(TP));
}

Handle(Standard_Transient)  Transfer_ActorOfTransientProcess::TransferTransient
  (const Handle(Standard_Transient)& /*ent*/,
   const Handle(Transfer_TransientProcess)& /*TP*/)
{
  Handle(Standard_Transient) nulres;
  return nulres;
}
