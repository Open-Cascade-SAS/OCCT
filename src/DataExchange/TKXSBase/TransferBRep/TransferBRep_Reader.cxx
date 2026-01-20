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
#include <Interface_Protocol.hxx>
#include <Message_Messenger.hxx>
#include <Message_ProgressScope.hxx>
#include <Standard_Transient.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_ActorOfTransientProcess.hxx>
#include <Transfer_TransferOutput.hxx>
#include <Transfer_TransientProcess.hxx>
#include <TransferBRep.hxx>
#include <TransferBRep_Reader.hxx>

TransferBRep_Reader::TransferBRep_Reader()
    : theDone(false),
      theFilest(0),
      theNewpr(false)
{
  theShapes = new NCollection_HSequence<TopoDS_Shape>();
  theTransi = new NCollection_HSequence<occ::handle<Standard_Transient>>();
}

void TransferBRep_Reader::SetProtocol(const occ::handle<Interface_Protocol>& protocol)
{
  theProto = protocol;
}

occ::handle<Interface_Protocol> TransferBRep_Reader::Protocol() const
{
  return theProto;
}

void TransferBRep_Reader::SetActor(const occ::handle<Transfer_ActorOfTransientProcess>& actor)
{
  theActor = actor;
}

occ::handle<Transfer_ActorOfTransientProcess> TransferBRep_Reader::Actor() const
{
  return theActor;
}

void TransferBRep_Reader::SetFileStatus(const int status)
{
  theFilest = status;
}

int TransferBRep_Reader::FileStatus() const
{
  return theFilest;
}

bool TransferBRep_Reader::FileNotFound() const
{
  return (theFilest < 0);
}

bool TransferBRep_Reader::SyntaxError() const
{
  return (theFilest > 0);
}

void TransferBRep_Reader::SetModel(const occ::handle<Interface_InterfaceModel>& model)
{
  theModel = model;
  Clear();
}

occ::handle<Interface_InterfaceModel> TransferBRep_Reader::Model() const
{
  return theModel;
}

void TransferBRep_Reader::Clear()
{
  theDone = false;
  theShapes->Clear();
  theTransi->Clear();
}

bool TransferBRep_Reader::CheckStatusModel(const bool withprint) const
{
  Interface_CheckTool     cht(theModel, theProto);
  Interface_CheckIterator chl = cht.CompleteCheckList();
  if (withprint && !theProc.IsNull() && !theProc->Messenger().IsNull())
  {
    Message_Messenger::StreamBuffer aBuffer = theProc->Messenger()->SendInfo();
    chl.Print(aBuffer, theModel, false);
  }
  return chl.IsEmpty(true);
}

Interface_CheckIterator TransferBRep_Reader::CheckListModel() const
{
  Interface_CheckTool     cht(theModel, theProto);
  Interface_CheckIterator chl = cht.CompleteCheckList();
  return chl;
}

bool& TransferBRep_Reader::ModeNewTransfer()
{
  return theNewpr;
}

bool TransferBRep_Reader::BeginTransfer()
{
  theDone = false;
  if (theModel.IsNull())
    return false;

  if (theNewpr || theProc.IsNull())
    theProc = new Transfer_TransientProcess(theModel->NbEntities());
  else
    theProc->Clear();
  theProc->SetErrorHandle(true);
  theProc->SetModel(theModel);
  PrepareTransfer();
  theProc->SetActor(theActor);
  return true;
}

void TransferBRep_Reader::EndTransfer()
{
  theShapes->Append(TransferBRep::Shapes(theProc, true));
  int i, nb = theProc->NbRoots();
  for (i = 1; i <= nb; i++)
  {
    occ::handle<Standard_Transient> ent = theProc->Root(i);
    occ::handle<Standard_Transient> res = theProc->FindTransient(ent);
    if (!res.IsNull())
      theTransi->Append(res);
  }
  theDone = true;
}

void TransferBRep_Reader::PrepareTransfer() {}

void TransferBRep_Reader::TransferRoots(const Message_ProgressRange& theProgress)
{
  Clear();
  if (!BeginTransfer())
    return;
  Transfer_TransferOutput TP(theProc, theModel);

  TP.TransferRoots(theProto, theProgress);
  EndTransfer();
}

bool TransferBRep_Reader::Transfer(const int       num,
                                               const Message_ProgressRange& theProgress)
{
  if (!BeginTransfer())
    return false;
  if (num <= 0 || num > theModel->NbEntities())
    return false;
  occ::handle<Standard_Transient> ent = theModel->Value(num);
  Transfer_TransferOutput    TP(theProc, theModel);

  if (theProc->TraceLevel() > 1)
  {
    Message_Messenger::StreamBuffer sout = theProc->Messenger()->SendInfo();
    sout << "--  Transfer(Read) : ";
    theModel->Print(ent, sout);
    sout << std::endl;
  }
  TP.Transfer(ent, theProgress);
  theProc->SetRoot(ent);
  EndTransfer();
  return true;
}

void TransferBRep_Reader::TransferList(const occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>& list,
                                       const Message_ProgressRange&                theProgress)
{
  if (!BeginTransfer())
    return;
  if (list.IsNull())
    return;
  Transfer_TransferOutput         TP(theProc, theModel);
  int                i, nb = list->Length();
  Message_Messenger::StreamBuffer sout = theProc->Messenger()->SendInfo();

  if (theProc->TraceLevel() > 1)
    sout << "--  Transfer(Read-List) : " << nb << " Items" << std::endl;
  Message_ProgressScope aPS(theProgress, NULL, nb);
  for (i = 1; i <= nb && aPS.More(); i++)
  {
    Message_ProgressRange      aRange = aPS.Next();
    occ::handle<Standard_Transient> ent    = list->Value(i);
    if (theModel->Number(ent) == 0)
      continue;

    if (theProc->TraceLevel() > 1)
    {
      sout << "--  Transfer(Read-List), Item " << i << " : ";
      theModel->Print(ent, sout);
      sout << std::endl;
    }
    TP.Transfer(ent, aRange);
    theProc->SetRoot(ent);
  }
  EndTransfer();
}

bool TransferBRep_Reader::IsDone() const
{
  return theDone;
}

//   ######    RESULTAT : SHAPES    ######

int TransferBRep_Reader::NbShapes() const
{
  return theShapes->Length();
}

occ::handle<NCollection_HSequence<TopoDS_Shape>> TransferBRep_Reader::Shapes() const
{
  return theShapes;
}

const TopoDS_Shape& TransferBRep_Reader::Shape(const int num) const
{
  return theShapes->Value(num);
}

TopoDS_Shape TransferBRep_Reader::OneShape() const
{
  TopoDS_Shape     res;
  int nb = theShapes->Length();
  if (nb == 0)
    return res;
  else if (nb == 1)
    return theShapes->Value(1);
  else
  {
    TopoDS_Compound C;
    BRep_Builder    B;
    B.MakeCompound(C);
    for (int i = 1; i <= nb; i++)
      B.Add(C, theShapes->Value(i));
    return C;
  }
}

TopoDS_Shape TransferBRep_Reader::ShapeResult(const occ::handle<Standard_Transient>& ent) const
{
  return TransferBRep::ShapeResult(theProc, ent);
}

//   ######    RESULTAT : TRANSIENTS    ######

int TransferBRep_Reader::NbTransients() const
{
  return theTransi->Length();
}

occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> TransferBRep_Reader::Transients() const
{
  return theTransi;
}

occ::handle<Standard_Transient> TransferBRep_Reader::Transient(const int num) const
{
  return theTransi->Value(num);
}

//   ######    CHECKS    ######

bool TransferBRep_Reader::CheckStatusResult(const bool withprint) const
{
  Interface_CheckIterator chl;
  if (!theProc.IsNull())
    chl = theProc->CheckList(false);
  if (withprint && !theProc.IsNull() && !theProc->Messenger().IsNull())
  {
    Message_Messenger::StreamBuffer aBuffer = theProc->Messenger()->SendInfo();
    chl.Print(aBuffer, theModel, false);
  }
  return chl.IsEmpty(true);
}

Interface_CheckIterator TransferBRep_Reader::CheckListResult() const
{
  if (!theProc.IsNull())
    return theProc->CheckList(false);
  Interface_CheckIterator chbid;
  return chbid;
}

occ::handle<Transfer_TransientProcess> TransferBRep_Reader::TransientProcess() const
{
  return theProc;
}

TransferBRep_Reader::~TransferBRep_Reader() {}
