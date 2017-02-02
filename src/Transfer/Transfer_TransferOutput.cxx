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

//szv#4 S4163

#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Protocol.hxx>
#include <Interface_ShareFlags.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_Transient.hxx>
#include <Transfer_ActorOfTransientProcess.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_TransferFailure.hxx>
#include <Transfer_TransferOutput.hxx>
#include <Transfer_TransientProcess.hxx>

Transfer_TransferOutput::Transfer_TransferOutput (const Handle(Transfer_ActorOfTransientProcess)& actor,
						  const Handle(Interface_InterfaceModel)& amodel)
{
  theproc  = new Transfer_TransientProcess (amodel->NbEntities());
  theproc->SetActor(actor);
  themodel = amodel;
//  thescope = Standard_False;
//  theundef = Transfer_UndefIgnore;
}

Transfer_TransferOutput::Transfer_TransferOutput (const Handle(Transfer_TransientProcess)& proc,
						  const Handle(Interface_InterfaceModel)& amodel)
{
  theproc  = proc;
  themodel = amodel;
//  thescope = Standard_False; //szv#4:S4163:12Mar99 initialization needed
//  theundef = Transfer_UndefIgnore;
}

//Standard_Boolean&  Transfer_TransferOutput::ScopeMode ()
//{  return  thescope;  }

Handle(Interface_InterfaceModel)  Transfer_TransferOutput::Model () const
{  return themodel;  }

Handle(Transfer_TransientProcess)  Transfer_TransferOutput::TransientProcess () const
{  return theproc;  }

void  Transfer_TransferOutput::Transfer (const Handle(Standard_Transient)& obj)
{
  if (themodel->Number(obj) == 0) throw Transfer_TransferFailure("TransferOutput : Transfer, entities do not come from same initial model");
//  Standard_Integer scope = 0;
//  if (thescope) scope = theproc->NewScope (obj);

  //:1 modified by ABV 5 Nov 97
  //:1  if (!theproc->Transfer(obj)) return;    // auparavant, traitement Undefined
//  Standard_Boolean ok = 
  theproc->Transfer ( obj );  
//  if (scope > 0) theproc->EndScope (scope);
//  if ( ! ok ) return;                               

/*  switch (theundef) {
    case Transfer_UndefIgnore  : return;
    case Transfer_UndefFailure : throw Transfer_TransferFailure("TransferOutput : Transfer Undefined as Failure");
    case Transfer_UndefContent : break; // on ne sait pas traiter ...
    case Transfer_UndefUser    : break; // idem
  }
*/
}


//  Resultats :
//   Pour transferer tout simplement toutes les racines d'un modele d'interface
//   Chacune est notee "Root" dans le Process final

void Transfer_TransferOutput::TransferRoots ()
{  TransferRoots(Interface_Protocol::Active());  }

void Transfer_TransferOutput::TransferRoots (const Handle(Interface_Protocol)& protocol)
{
  theproc->SetRootManagement (Standard_False);
  Interface_ShareFlags tool(themodel,protocol);
  Interface_EntityIterator list = tool.RootEntities();
  for (list.Start(); list.More(); list.Next()) {
    Handle(Standard_Transient) ent = list.Value();
//    Standard_Integer scope = 0;
//    if (thescope) scope = theproc->NewScope (ent);
    if (theproc->Transfer(ent)) theproc->SetRoot(ent);
//    if (scope > 0) theproc->EndScope (scope);
  }
}

void Transfer_TransferOutput::TransferRoots (const Interface_Graph& G)
{
  theproc->SetRootManagement (Standard_False);
  Interface_ShareFlags tool(G);
  theproc->SetModel (G.Model());
  Interface_EntityIterator list = tool.RootEntities();
  for (list.Start(); list.More(); list.Next()) {
    Handle(Standard_Transient) ent = list.Value();
//    Standard_Integer scope = 0;
//    if (thescope) scope = theproc->NewScope (ent);
    if (theproc->Transfer(ent)) theproc->SetRoot(ent);
//    if (scope > 0) theproc->EndScope (scope);
  }
}


Interface_EntityIterator  Transfer_TransferOutput::ListForStatus (const Standard_Boolean normal,
								  const Standard_Boolean roots) const
{
  Interface_EntityIterator list;
  Standard_Integer max = (roots ? theproc->NbRoots() : theproc->NbMapped());
  for (Standard_Integer i = 1; i <= max; i ++) {
    const Handle(Transfer_Binder)& binder =
      (roots ? theproc->RootItem(i) : theproc->MapItem(i));
    if (binder.IsNull()) continue;
    Transfer_StatusExec statex = binder->StatusExec();
    Standard_Boolean ok =
      (statex == Transfer_StatusInitial || statex == Transfer_StatusDone);
    if (ok == normal) list.AddItem
      ( (roots ? theproc->Root(i) : theproc->Mapped(i)) );
  }
  return list;
}

Handle(Interface_InterfaceModel)  Transfer_TransferOutput::ModelForStatus
  (const Handle(Interface_Protocol)& protocol,
   const Standard_Boolean normal, const Standard_Boolean roots) const
{
  Handle(Interface_InterfaceModel) newmod;
  if (themodel.IsNull()) return newmod;
  newmod = themodel->NewEmptyModel();
  Interface_EntityIterator list = ListForStatus (normal, roots);
  for (list.Start(); list.More(); list.Next())
    newmod->AddWithRefs (list.Value(),protocol);
  return newmod;
}
