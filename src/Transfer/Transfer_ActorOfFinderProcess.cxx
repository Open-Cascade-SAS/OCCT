#include <Transfer_ActorOfFinderProcess.ixx>
#include <Transfer_TransientMapper.hxx>

Transfer_ActorOfFinderProcess::Transfer_ActorOfFinderProcess ()    {  themodetrans = 0;  }

Standard_Integer& Transfer_ActorOfFinderProcess::ModeTrans ()
{  return themodetrans;  }

Handle(Transfer_Binder)  Transfer_ActorOfFinderProcess::Transfer
  (const Handle(Transfer_Finder)& fnd,
   const Handle(Transfer_FinderProcess)& FP)
{
  Handle(Transfer_TransientMapper) tm = Handle(Transfer_TransientMapper)::DownCast (fnd);
  if (tm.IsNull()) return NullResult();
  Handle(Standard_Transient) res = TransferTransient (tm->Value(),FP);
  if (res.IsNull()) return NullResult();
  return TransientResult (res);
}

Handle(Transfer_Binder)  Transfer_ActorOfFinderProcess::Transferring
  (const Handle(Transfer_Finder)& ent,
   const Handle(Transfer_ProcessForFinder)& TP)
{
  return Transfer(ent,Handle(Transfer_FinderProcess)::DownCast(TP));
}

Handle(Standard_Transient)  Transfer_ActorOfFinderProcess::TransferTransient
  (const Handle(Standard_Transient)& /*ent*/,
   const Handle(Transfer_FinderProcess)& )
{
  Handle(Standard_Transient) nulres;
  return nulres;
}
