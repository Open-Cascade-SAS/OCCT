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
#include <Interface_Macros.hxx>
#include <Interface_Protocol.hxx>
#include <Standard_Transient.hxx>
#include <Transfer_DispatchControl.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <Transfer_TransferDispatch.hxx>
#include <Transfer_TransientProcess.hxx>

Transfer_TransferDispatch::Transfer_TransferDispatch
  (const Handle(Interface_InterfaceModel)& amodel,
   const Interface_GeneralLib& lib)
      : Interface_CopyTool (amodel,lib)
{  SetControl (new Transfer_DispatchControl
	       (amodel, new Transfer_TransientProcess(amodel->NbEntities())));  }


   Transfer_TransferDispatch::Transfer_TransferDispatch
  (const Handle(Interface_InterfaceModel)& amodel,
   const Handle(Interface_Protocol)& protocol)
      : Interface_CopyTool (amodel,protocol)
{  SetControl (new Transfer_DispatchControl
	       (amodel,new Transfer_TransientProcess(amodel->NbEntities())));  }


   Transfer_TransferDispatch::Transfer_TransferDispatch
  (const Handle(Interface_InterfaceModel)& amodel)
      :  Interface_CopyTool (amodel)
{  SetControl (new Transfer_DispatchControl
	       (amodel,new Transfer_TransientProcess(amodel->NbEntities())));  }



    Handle(Transfer_TransientProcess)  Transfer_TransferDispatch::TransientProcess
  () const 
{  return Handle(Transfer_DispatchControl)::DownCast(Control())->TransientProcess();  }

    Standard_Boolean Transfer_TransferDispatch::Copy
  (const Handle(Standard_Transient)& entfrom,
   Handle(Standard_Transient)& entto,
   const Standard_Boolean mapped, const Standard_Boolean errstat)
{
  Handle(Transfer_Binder) result = TransientProcess()->Transferring(entfrom);
  if (result.IsNull())
    return Interface_CopyTool::Copy(entfrom,entto,mapped,errstat);

  if (!result->IsKind(STANDARD_TYPE(Transfer_SimpleBinderOfTransient)))
    return Standard_False;  // Produit qq chose, mais quoi ?
  entto = GetCasted(Transfer_SimpleBinderOfTransient,result)->Result();
  return Standard_True;
}
