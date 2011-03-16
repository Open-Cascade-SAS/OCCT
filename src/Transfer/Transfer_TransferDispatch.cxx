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
