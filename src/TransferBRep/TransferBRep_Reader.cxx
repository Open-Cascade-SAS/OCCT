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


#include <BRep_Builder.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_CheckTool.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Interface_Protocol.hxx>
#include <Message_Messenger.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Transient.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_ActorOfTransientProcess.hxx>
#include <Transfer_TransferOutput.hxx>
#include <Transfer_TransientProcess.hxx>
#include <TransferBRep.hxx>
#include <TransferBRep_Reader.hxx>

TransferBRep_Reader::TransferBRep_Reader ()
    : theDone (Standard_False) , theFilest (0) , theNewpr (Standard_False)
      {    theShapes = new TopTools_HSequenceOfShape();  theTransi = new TColStd_HSequenceOfTransient();  }

    void  TransferBRep_Reader::SetProtocol
  (const Handle(Interface_Protocol)& protocol)
      {  theProto = protocol;  }

    Handle(Interface_Protocol)  TransferBRep_Reader::Protocol () const
      {  return theProto;  }

    void  TransferBRep_Reader::SetActor
  (const Handle(Transfer_ActorOfTransientProcess)& actor)
      {  theActor = actor;  }

    Handle(Transfer_ActorOfTransientProcess) TransferBRep_Reader::Actor () const
      {  return theActor;  }

    void  TransferBRep_Reader::SetFileStatus (const Standard_Integer status)
      {  theFilest = status;  }

    Standard_Integer  TransferBRep_Reader::FileStatus () const
      { return theFilest;  }

    Standard_Boolean  TransferBRep_Reader::FileNotFound () const
      {  return (theFilest < 0);  }

    Standard_Boolean  TransferBRep_Reader::SyntaxError () const
      {  return (theFilest > 0);  }


    void  TransferBRep_Reader::SetModel
  (const Handle(Interface_InterfaceModel)& model)
{
  theModel = model;
  Clear();
}

    Handle(Interface_InterfaceModel)  TransferBRep_Reader::Model () const
      {  return theModel;  }


    void  TransferBRep_Reader::Clear ()
{
  theDone = Standard_False;
  theShapes->Clear();  theTransi->Clear();
}

    Standard_Boolean  TransferBRep_Reader::CheckStatusModel
  (const Standard_Boolean withprint) const
{
  Interface_CheckTool cht (theModel,theProto);
  Interface_CheckIterator chl = cht.CompleteCheckList();
  if ( withprint && ! theProc.IsNull() ) 
    chl.Print (theProc->Messenger(), theModel, Standard_False);
  return chl.IsEmpty(Standard_True);
}

    Interface_CheckIterator  TransferBRep_Reader::CheckListModel () const
{
  Interface_CheckTool cht (theModel,theProto);
  Interface_CheckIterator chl = cht.CompleteCheckList();
  return chl;
}

    Standard_Boolean&  TransferBRep_Reader::ModeNewTransfer ()
      {  return theNewpr;  }

    Standard_Boolean  TransferBRep_Reader::BeginTransfer ()
{
  theDone = Standard_False;
  if (theModel.IsNull()) return Standard_False;

  if (theNewpr || theProc.IsNull())
    theProc = new Transfer_TransientProcess (theModel->NbEntities());
  else theProc->Clear();
  theProc->SetErrorHandle(Standard_True);
  theProc->SetModel (theModel);
  PrepareTransfer();
  theProc->SetActor (theActor);
  return Standard_True;
}

    void  TransferBRep_Reader::EndTransfer ()
{
  theShapes->Append ( TransferBRep::Shapes (theProc,Standard_True) );
  Standard_Integer i,nb = theProc->NbRoots();
  for (i = 1; i <= nb; i ++) {
    Handle(Standard_Transient) ent = theProc->Root(i);
    Handle(Standard_Transient) res = theProc->FindTransient(ent);
    if (!res.IsNull()) theTransi->Append (res);
  }
  theDone = Standard_True;
}


    void  TransferBRep_Reader::PrepareTransfer ()    {  }

    void  TransferBRep_Reader::TransferRoots ()
{
  Clear();
  if (!BeginTransfer()) return;
  Transfer_TransferOutput TP (theProc,theModel);

  TP.TransferRoots(theProto);
  EndTransfer();
}

    Standard_Boolean TransferBRep_Reader::Transfer (const Standard_Integer num)
{
  if (!BeginTransfer()) return Standard_False;
  if (num <= 0 || num > theModel->NbEntities()) return Standard_False;
  Handle(Standard_Transient) ent = theModel->Value(num);
  Transfer_TransferOutput TP (theProc,theModel);

  if (theProc->TraceLevel() > 1) {
    Handle(Message_Messenger) sout = theProc->Messenger();
    sout<<"--  Transfer(Read) : ";  
    theModel->Print (ent,sout);  
    sout<<endl;
  }
  TP.Transfer(ent);
  theProc->SetRoot(ent);
  EndTransfer();
  return Standard_True;
}

    void  TransferBRep_Reader::TransferList
  (const Handle(TColStd_HSequenceOfTransient)& list)
{
  if (!BeginTransfer()) return;
  if (list.IsNull()) return;
  Transfer_TransferOutput TP (theProc,theModel);
  Standard_Integer i, nb = list->Length();
  Handle(Message_Messenger) sout = theProc->Messenger();

  if (theProc->TraceLevel() > 1) 
    sout<<"--  Transfer(Read-List) : "<<nb<<" Items"<<endl;
  for (i = 1; i <= nb; i ++) {
    Handle(Standard_Transient) ent = list->Value(i);
    if (theModel->Number(ent) == 0) continue;

    if (theProc->TraceLevel() > 1) 
    {
      sout<<"--  Transfer(Read-List), Item "<<i<<" : ";  
      theModel->Print (ent,sout);  
      sout<<endl;
    }
    TP.Transfer(ent);
    theProc->SetRoot(ent);
  }
  EndTransfer();
}

    Standard_Boolean  TransferBRep_Reader::IsDone () const
      {  return theDone;  }

//   ######    RESULTAT : SHAPES    ######

    Standard_Integer  TransferBRep_Reader::NbShapes () const
      {  return theShapes->Length();  }

    Handle(TopTools_HSequenceOfShape)  TransferBRep_Reader::Shapes () const
      {  return theShapes;  }

    const TopoDS_Shape&  TransferBRep_Reader::Shape
  (const Standard_Integer num) const
      {  return theShapes->Value(num);  }

    TopoDS_Shape  TransferBRep_Reader::OneShape () const
{
  TopoDS_Shape res;
  Standard_Integer nb = theShapes->Length();
  if (nb == 0) return res;
  else if (nb == 1) return theShapes->Value(1);
  else {
    TopoDS_Compound C;
    BRep_Builder B;
    B.MakeCompound(C);
    for (Standard_Integer i = 1; i <= nb; i ++)  B.Add (C,theShapes->Value(i));
    return C;
  }
}

    TopoDS_Shape  TransferBRep_Reader::ShapeResult
  (const Handle(Standard_Transient)& ent) const
      {  return TransferBRep::ShapeResult (theProc,ent);  }

//   ######    RESULTAT : TRANSIENTS    ######

    Standard_Integer  TransferBRep_Reader::NbTransients () const
      {  return theTransi->Length();  }

    Handle(TColStd_HSequenceOfTransient)  TransferBRep_Reader::Transients () const
      {  return theTransi;  }

    Handle(Standard_Transient)  TransferBRep_Reader::Transient
  (const Standard_Integer num) const
      {  return theTransi->Value(num);  }


//   ######    CHECKS    ######

    Standard_Boolean  TransferBRep_Reader::CheckStatusResult
  (const Standard_Boolean withprint) const
{
  Interface_CheckIterator chl;
  if (!theProc.IsNull()) chl = theProc->CheckList(Standard_False);
  if (withprint && ! theProc.IsNull()) 
    chl.Print (theProc->Messenger(), theModel, Standard_False);
  return chl.IsEmpty(Standard_True);
}

    Interface_CheckIterator  TransferBRep_Reader::CheckListResult () const
{
  if (!theProc.IsNull()) return theProc->CheckList(Standard_False);
  Interface_CheckIterator chbid;  return chbid;
}

    Handle(Transfer_TransientProcess)  TransferBRep_Reader::TransientProcess
  () const
      {  return theProc;  }

TransferBRep_Reader::~TransferBRep_Reader()
{}
     
