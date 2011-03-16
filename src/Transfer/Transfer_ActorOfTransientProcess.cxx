#include <Transfer_ActorOfTransientProcess.ixx>

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
