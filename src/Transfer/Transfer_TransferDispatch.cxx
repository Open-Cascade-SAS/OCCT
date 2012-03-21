// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <Transfer_TransferDispatch.ixx>
#include <Transfer_DispatchControl.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <Interface_Macros.hxx>



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
