#include <Transfer_TransferInput.ixx>
#include <Transfer_Binder.hxx>
#include <Transfer_IteratorOfProcessForTransient.hxx>
#include <Transfer_IteratorOfProcessForFinder.hxx>
#include <Transfer_VoidBinder.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <Transfer_MultipleBinder.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <Interface_Macros.hxx>


    Transfer_TransferInput::Transfer_TransferInput ()    {  }


//  Resultats : Pour le Modele ...

    Interface_EntityIterator  Transfer_TransferInput::Entities
  (Transfer_TransferIterator& list) const
{
  Interface_EntityIterator iter;
  for (list.Start(); list.More(); list.Next()) {
    Handle(Transfer_Binder) binder = list.Value();
    if (binder.IsNull()) continue;
    if (binder->IsKind(STANDARD_TYPE(Transfer_VoidBinder))) continue;

// Vrai resultat : doit etre transient (simple ou liste)
    DeclareAndCast(Transfer_SimpleBinderOfTransient,transb,binder);
    DeclareAndCast(Transfer_MultipleBinder,multi,binder);
    Standard_Boolean OK = Standard_False;
    if (!transb.IsNull()) {
      OK = Standard_True;
      if (transb->HasResult()) iter.AddItem(transb->Result());
    }
    else if (!multi.IsNull()) {
      Handle(TColStd_HSequenceOfTransient) mulres = multi->MultipleResult();
      Standard_Integer nbres = 0;
      if (!mulres.IsNull()) nbres = mulres->Length();
      for (Standard_Integer i = 1; i <= nbres; i ++)
	iter.AddItem(mulres->Value(i));
    }
    else Transfer_TransferFailure::Raise
      ("TransferInput : Entities, one of the Results is not Transient Handle");
  }
  return iter;
}


    void Transfer_TransferInput::FillModel
  (const Handle(Transfer_TransientProcess)& proc,
   const Handle(Interface_InterfaceModel)& amodel) const
{
  Transfer_TransferIterator list = proc->CompleteResult();
  Interface_EntityIterator  iter = Entities (list);
  amodel->GetFromTransfer(iter);
}

    void Transfer_TransferInput::FillModel
  (const Handle(Transfer_TransientProcess)& proc,
   const Handle(Interface_InterfaceModel)& amodel,
   const Handle(Interface_Protocol)& proto,
   const Standard_Boolean roots) const
{
  Transfer_TransferIterator list;
  if (roots) list = proc->RootResult();
  else       list = proc->CompleteResult();
  Interface_EntityIterator  iter = Entities (list);
  for (iter.Start(); iter.More(); iter.Next())
    amodel->AddWithRefs (iter.Value(), proto);
}


    void Transfer_TransferInput::FillModel
  (const Handle(Transfer_FinderProcess)& proc,
   const Handle(Interface_InterfaceModel)& amodel) const
{
  Transfer_TransferIterator list = proc->CompleteResult();
  Interface_EntityIterator  iter = Entities (list);
  amodel->GetFromTransfer(iter);
}

    void Transfer_TransferInput::FillModel
  (const Handle(Transfer_FinderProcess)& proc,
   const Handle(Interface_InterfaceModel)& amodel,
   const Handle(Interface_Protocol)& proto,
   const Standard_Boolean roots) const
{
  Transfer_TransferIterator list;
  if (roots) list = proc->RootResult();
  else       list = proc->CompleteResult();
  Interface_EntityIterator  iter = Entities (list);
  for (iter.Start(); iter.More(); iter.Next())
    amodel->AddWithRefs (iter.Value(), proto);
}
