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


#include <Interface_EntityIterator.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Interface_Protocol.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_FinderProcess.hxx>
#include <Transfer_IteratorOfProcessForFinder.hxx>
#include <Transfer_IteratorOfProcessForTransient.hxx>
#include <Transfer_MultipleBinder.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <Transfer_TransferFailure.hxx>
#include <Transfer_TransferInput.hxx>
#include <Transfer_TransferIterator.hxx>
#include <Transfer_TransientProcess.hxx>
#include <Transfer_VoidBinder.hxx>

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
    if (!transb.IsNull()) {
      if (transb->HasResult()) iter.AddItem(transb->Result());
    }
    else if (!multi.IsNull()) {
      Handle(TColStd_HSequenceOfTransient) mulres = multi->MultipleResult();
      Standard_Integer nbres = 0;
      if (!mulres.IsNull()) nbres = mulres->Length();
      for (Standard_Integer i = 1; i <= nbres; i ++)
	iter.AddItem(mulres->Value(i));
    }
    else throw Transfer_TransferFailure("TransferInput : Entities, one of the Results is not Transient Handle");
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
