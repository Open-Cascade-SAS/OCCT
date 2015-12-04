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


#include <Interface_GeneralLib.hxx>
#include <Interface_InterfaceError.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Protocol.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <Transfer_ActorDispatch.hxx>
#include <Transfer_ActorOfTransientProcess.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_TransferDispatch.hxx>
#include <Transfer_TransientProcess.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Transfer_ActorDispatch,Transfer_ActorOfTransientProcess)

Transfer_ActorDispatch::Transfer_ActorDispatch
  (const Handle(Interface_InterfaceModel)& amodel,
   const Interface_GeneralLib& lib)
    :  thetool (amodel,lib)
{
  SetLast(Standard_True);  // actor par defaut
  thetool.TransientProcess()->SetActor(this);
}

    Transfer_ActorDispatch::Transfer_ActorDispatch
  (const Handle(Interface_InterfaceModel)& amodel,
   const Handle(Interface_Protocol)& protocol)
    :  thetool (amodel,protocol)
{
  SetLast(Standard_True);  // actor par defaut
  thetool.TransientProcess()->SetActor(this);
}

    Transfer_ActorDispatch::Transfer_ActorDispatch
  (const Handle(Interface_InterfaceModel)& amodel)
    :  thetool (amodel)
{
  SetLast(Standard_True);  // actor par defaut
  thetool.TransientProcess()->SetActor(this);
}


    void  Transfer_ActorDispatch::AddActor
  (const Handle(Transfer_ActorOfTransientProcess)& actor)
      {  thetool.TransientProcess()->SetActor(actor);  }

    Transfer_TransferDispatch&  Transfer_ActorDispatch::TransferDispatch ()
      {  return thetool;  }


    Handle(Transfer_Binder)  Transfer_ActorDispatch::Transfer
  (const Handle(Standard_Transient)& start,
   const Handle(Transfer_TransientProcess)& /*TP*/)
{
  thetool.TransferEntity(start);
  return thetool.TransientProcess()->Find(start);
}
