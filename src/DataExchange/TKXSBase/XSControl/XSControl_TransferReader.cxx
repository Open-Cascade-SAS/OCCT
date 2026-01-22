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

//:   abv 09.04.99: S4136: remove parameter lastpreci
// szv#11:CASCADE30:01Feb00 BRepBuilderAPI::Precision(p) removed

#include <BRepBuilderAPI.hxx>
#include <BRepLib.hxx>
#include <IFSelect_CheckCounter.hxx>
#include <IFSelect_SignatureList.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_HGraph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_MSG.hxx>
#include <Interface_Static.hxx>
#include <Message_ProgressScope.hxx>
#include <ShapeFix.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <TopoDS_HShape.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>
#include <Transfer_ActorOfTransientProcess.hxx>
#include <Transfer_IteratorOfProcessForTransient.hxx>
#include <Transfer_ResultFromModel.hxx>
#include <Transfer_ResultFromTransient.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <Transfer_TransferOutput.hxx>
#include <Transfer_TransientProcess.hxx>
#include <TransferBRep.hxx>
#include <TransferBRep_ShapeBinder.hxx>
#include <XSControl_Controller.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_Utils.hxx>
#include <Message.hxx>

#include <cstdio>
IMPLEMENT_STANDARD_RTTIEXT(XSControl_TransferReader, Standard_Transient)

//=================================================================================================

void XSControl_TransferReader::SetController(const occ::handle<XSControl_Controller>& control)
{
  myController = control;
  myActor.Nullify();
  Clear(-1);
}

//=================================================================================================

occ::handle<Transfer_ActorOfTransientProcess> XSControl_TransferReader::Actor()
{
  if (myActor.IsNull() && !myController.IsNull() && !myModel.IsNull())
    myActor = myController->ActorRead(myModel);
  return myActor;
}

//=================================================================================================

void XSControl_TransferReader::SetModel(const occ::handle<Interface_InterfaceModel>& model)
{
  myModel = model;
  if (!myTP.IsNull())
    myTP->SetModel(model);
}

//=================================================================================================

void XSControl_TransferReader::SetGraph(const occ::handle<Interface_HGraph>& graph)
{
  if (graph.IsNull())
  {
    myModel.Nullify();
  }
  else
    myModel = graph->Graph().Model();

  myGraph = graph;

  if (!myTP.IsNull())
    myTP->SetGraph(graph);
}

//=================================================================================================

void XSControl_TransferReader::SetContext(const char* const                      name,
                                          const occ::handle<Standard_Transient>& ctx)
{
  myContext.Bind(name, ctx);
}

//=================================================================================================

bool XSControl_TransferReader::GetContext(const char* const                 name,
                                          const occ::handle<Standard_Type>& type,
                                          occ::handle<Standard_Transient>&  ctx) const
{
  if (myContext.IsEmpty())
    return false;
  if (!myContext.Find(name, ctx))
    ctx.Nullify();
  if (ctx.IsNull())
    return false;
  if (type.IsNull())
    return true;
  if (!ctx->IsKind(type))
    ctx.Nullify();
  return !ctx.IsNull();
}

//=================================================================================================

void XSControl_TransferReader::Clear(const int mode)
{
  if (mode & 1)
  {
    myResults.Clear();
    myShapeResult.Nullify();
  }
  if (mode & 2)
  {
    myModel.Nullify();
    myGraph.Nullify();
    myTP.Nullify();
    myActor.Nullify();
    myFileName.Clear();
  }
}

//  ########################################################
//  ###########            RESULTATS            ############

//=================================================================================================

bool XSControl_TransferReader::RecordResult(const occ::handle<Standard_Transient>& ent)
{
  if (myModel.IsNull() || myTP.IsNull())
    return false;
  int num = myModel->Number(ent);
  if (num == 0)
    return false;
  occ::handle<TCollection_HAsciiString> lab = myModel->StringLabel(ent);

  occ::handle<Transfer_ResultFromModel> res = new Transfer_ResultFromModel;
  res->Fill(myTP, ent);

  //   Case of Shape result : for main result, make HShape ...
  occ::handle<Transfer_Binder> binder = res->MainResult()->Binder();
  DeclareAndCast(TransferBRep_ShapeBinder, shb, binder);
  if (!shb.IsNull())
  {
    occ::handle<Transfer_SimpleBinderOfTransient> trb = new Transfer_SimpleBinderOfTransient;
    trb->SetResult(new TopoDS_HShape(shb->Result()));
    trb->Merge(binder);
    res->MainResult()->SetBinder(trb);
  }

  res->SetFileName(myFileName.ToCString());
  myResults.Bind(num, res);
  return true;
}

//=================================================================================================

bool XSControl_TransferReader::IsRecorded(const occ::handle<Standard_Transient>& ent) const
{
  if (myModel.IsNull())
    return false;
  int num = myModel->Number(ent);
  if (num == 0)
    return false;
  const occ::handle<Standard_Transient>* pRes = myResults.Seek(num);
  if (!pRes)
    return false;
  return ((*pRes)->DynamicType() == STANDARD_TYPE(Transfer_ResultFromModel));
}

//=================================================================================================

bool XSControl_TransferReader::HasResult(const occ::handle<Standard_Transient>& ent) const
{
  if (myModel.IsNull())
    return false;
  int num = myModel->Number(ent);
  if (num == 0)
    return false;
  const occ::handle<Standard_Transient>* pRes = myResults.Seek(num);
  if (!pRes)
    return false;
  DeclareAndCast(Transfer_ResultFromModel, fr, *pRes);
  if (fr.IsNull())
    return false;
  return fr->HasResult();
}

//=================================================================================================

occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> XSControl_TransferReader::
  RecordedList() const
{
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> li =
    new NCollection_HSequence<occ::handle<Standard_Transient>>();
  if (myModel.IsNull())
    return li;
  int i, nb = myModel->NbEntities();
  for (i = 1; i <= nb; i++)
  {
    const occ::handle<Standard_Transient>* pRes = myResults.Seek(i);
    if (pRes && !(*pRes).IsNull())
      li->Append(myModel->Value(i));
  }
  return li;
}

//=================================================================================================

bool XSControl_TransferReader::Skip(const occ::handle<Standard_Transient>& ent)
{
  if (myModel.IsNull() || myTP.IsNull())
    return false;
  int num = myModel->Number(ent);
  if (num == 0)
    return false;
  myResults.Bind(num, ent);
  return true;
}

//=================================================================================================

bool XSControl_TransferReader::IsSkipped(const occ::handle<Standard_Transient>& ent) const
{
  if (myModel.IsNull())
    return false;
  int num = myModel->Number(ent);
  if (num == 0)
    return false;
  const occ::handle<Standard_Transient>* pRes = myResults.Seek(num);
  if (!pRes)
    return false;
  return ((*pRes)->DynamicType() != STANDARD_TYPE(Transfer_ResultFromModel));
}

//=================================================================================================

bool XSControl_TransferReader::IsMarked(const occ::handle<Standard_Transient>& ent) const
{
  if (myModel.IsNull())
    return false;
  int num = myModel->Number(ent);
  if (num == 0)
    return false;
  const occ::handle<Standard_Transient>* pRes = myResults.Seek(num);
  if (!pRes || (*pRes).IsNull())
    return false;
  return true;
}

//  #########    SLIGHTLY MORE REFINED ACCESS    #########

//=================================================================================================

occ::handle<Transfer_ResultFromModel> XSControl_TransferReader::FinalResult(
  const occ::handle<Standard_Transient>& ent) const
{
  occ::handle<Transfer_ResultFromModel> res;
  if (myModel.IsNull())
    return res;
  int num = myModel->Number(ent);
  if (num == 0)
    return res;
  const occ::handle<Standard_Transient>* pRes = myResults.Seek(num);
  if (!pRes)
    return res;
  res = GetCasted(Transfer_ResultFromModel, *pRes);
  return res;
}

//=================================================================================================

const char* XSControl_TransferReader::FinalEntityLabel(
  const occ::handle<Standard_Transient>& ent) const
{
  occ::handle<Transfer_ResultFromModel> resu = FinalResult(ent);
  if (resu.IsNull())
    return "";
  return resu->MainLabel();
}

//=================================================================================================

int XSControl_TransferReader::FinalEntityNumber(const occ::handle<Standard_Transient>& ent) const
{
  occ::handle<Transfer_ResultFromModel> resu = FinalResult(ent);
  if (resu.IsNull())
    return 0;
  return resu->MainNumber();
}

//=================================================================================================

occ::handle<Transfer_ResultFromModel> XSControl_TransferReader::ResultFromNumber(
  const int num) const
{
  occ::handle<Transfer_ResultFromModel> res;
  if (num < 1 || num > myModel->NbEntities())
    return res;
  const occ::handle<Standard_Transient>* pRes = myResults.Seek(num);
  if (!pRes)
    return res;
  res = GetCasted(Transfer_ResultFromModel, *pRes);
  return res;
}

//=================================================================================================

occ::handle<Standard_Transient> XSControl_TransferReader::TransientResult(
  const occ::handle<Standard_Transient>& ent) const
{
  occ::handle<Standard_Transient>       tres;
  occ::handle<Transfer_ResultFromModel> res = FinalResult(ent);
  if (res.IsNull())
    return tres;
  occ::handle<Transfer_ResultFromTransient> mres = res->MainResult();
  if (mres.IsNull())
    return tres;
  DeclareAndCast(Transfer_SimpleBinderOfTransient, bnd, mres->Binder());
  if (bnd.IsNull())
    return tres;
  if (!bnd->HasResult())
    return tres;
  return bnd->Result();
}

//=================================================================================================

TopoDS_Shape XSControl_TransferReader::ShapeResult(const occ::handle<Standard_Transient>& ent) const
{
  TopoDS_Shape                          tres; // DOIT RESTER NULL
  occ::handle<Transfer_ResultFromModel> res = FinalResult(ent);
  if (res.IsNull())
    return tres;
  occ::handle<Transfer_ResultFromTransient> mres = res->MainResult();
  if (mres.IsNull())
    return tres;
  XSControl_Utils xu;
  TopoDS_Shape    sh = xu.BinderShape(mres->Binder());

  //   Ooh the ugly wart
  double tolang = Interface_Static::RVal("read.encoderegularity.angle");
  if (tolang <= 0 || sh.IsNull())
    return sh;
  ShapeFix::EncodeRegularity(sh, tolang);
  return sh;
}

//=================================================================================================

bool XSControl_TransferReader::ClearResult(const occ::handle<Standard_Transient>& ent,
                                           const int                              mode)
{
  if (myModel.IsNull())
    return false;
  int num = myModel->Number(ent);
  if (num == 0)
    return false;
  occ::handle<Standard_Transient>* pRes = myResults.ChangeSeek(num);
  if (!pRes)
    return false;
  if (mode < 0)
    pRes->Nullify();
  else
  {
    DeclareAndCast(Transfer_ResultFromModel, resu, *pRes);
    if (resu.IsNull())
      return false;
    resu->Strip(mode);
  }
  return true;
}

//  <<<< >>>>  CAUTION, not great : it would be better
//             to make an inverse map and consult it
//             or beef up ResultFromModel ...

//=================================================================================================

occ::handle<Standard_Transient> XSControl_TransferReader::EntityFromResult(
  const occ::handle<Standard_Transient>& res,
  const int                              mode) const
{
  occ::handle<Standard_Transient> nulh;
  //  case of the shape
  XSControl_Utils xu;
  TopoDS_Shape    sh = xu.BinderShape(res);
  if (!sh.IsNull())
    return EntityFromShapeResult(sh, mode);

  occ::handle<Transfer_Binder> abinder;
  DeclareAndCast(Transfer_Binder, binder, res);
  int i, j, nb;

  if (mode == 0 || mode == 1)
  {
    //  we look in the TransientProcess (Roots or all Mappeds)
    if (!myTP.IsNull())
    {
      nb = (mode == 0 ? myTP->NbRoots() : myTP->NbMapped());
      for (j = 1; j <= nb; j++)
      {
        i = (mode == 0 ? myModel->Number(myTP->Root(j)) : j);
        if (i == 0)
          continue;
        abinder = myTP->MapItem(i);
        if (abinder.IsNull())
          continue;
        if (!binder.IsNull())
        {
          if (binder == abinder)
            return myTP->Mapped(i);
          continue;
        }
        DeclareAndCast(Transfer_SimpleBinderOfTransient, trb, abinder);
        if (trb.IsNull())
          continue;
        if (trb->Result() == res)
          return myTP->Mapped(i);
      }
    }
    return nulh; // Null
  }

  //   Search in myResults (roots)
  //     2 : Main only  3 : Main + one sub;  4 : all
  if (mode >= 2)
  {
    nb = myModel->NbEntities();
    for (i = 1; i <= nb; i++)
    {
      occ::handle<Transfer_ResultFromModel> rec = ResultFromNumber(i);
      if (rec.IsNull())
        return nulh;
      occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> list =
        rec->Results(mode - 2);
      int ir, nr = list->Length();
      for (ir = 1; ir <= nr; ir++)
      {
        DeclareAndCast(Transfer_ResultFromTransient, rft, list->Value(ir));
        if (rft.IsNull())
          continue;
        if (rft->Binder() == binder)
          return rft->Start();
      }
    }
  }

  //  other cases not yet implemented
  return nulh;
}

//  <<<< >>>>  CAUTION, even worse than the previous one

//=================================================================================================

occ::handle<Standard_Transient> XSControl_TransferReader::EntityFromShapeResult(
  const TopoDS_Shape& res,
  const int           mode) const
{
  occ::handle<Standard_Transient> nulh, samesh, partner;
  if (res.IsNull())
    return nulh;
  int i, j, nb;

  XSControl_Utils xu;
  if (mode == 0 || mode == 1 || mode == -1)
  {
    //  we look in the TransientProcess
    if (!myTP.IsNull())
    {
      nb = (mode == 0 ? myTP->NbRoots() : myTP->NbMapped());
      for (j = 1; j <= nb; j++)
      {
        i = (mode == 0 ? myModel->Number(myTP->Root(j)) : j);
        if (i == 0)
          continue;
        occ::handle<Standard_Transient> ent = myTP->Mapped(i);
        TopoDS_Shape                    sh  = TransferBRep::ShapeResult(myTP, ent);
        if (!sh.IsNull())
        {
          if (sh == res)
            return ent;
          // lesser priorities : Same (always) or Partner (mode < 0)
          if (sh.IsSame(res))
            samesh = ent;
          if (mode == -1 && sh.IsPartner(res))
            partner = ent;
        }
      }
    }
    //    Here, no true equality found. Lesser priorities: Same then Partner
    if (!samesh.IsNull())
      return samesh;
    if (!partner.IsNull())
      return partner; // calculated if mode = -1
    return nulh;
  }

  //   Search in myResults (roots)
  //     2 : Main only  3 : Main + one sub;  4 : all
  if (mode >= 2)
  {
    nb = myModel->NbEntities();
    for (i = 1; i <= nb; i++)
    {
      occ::handle<Transfer_ResultFromModel> rec = ResultFromNumber(i);
      if (rec.IsNull())
        continue;

      occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> list =
        rec->Results(mode - 2);
      int ir, nr = list->Length();
      for (ir = 1; ir <= nr; ir++)
      {
        DeclareAndCast(Transfer_ResultFromTransient, rft, list->Value(ir));
        if (rft.IsNull())
          continue;
        TopoDS_Shape sh = xu.BinderShape(rft->Binder());
        if (!sh.IsNull() && sh == res)
          return rft->Start();
      }
    }
  }

  return nulh;
}

//=================================================================================================

occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> XSControl_TransferReader::
  EntitiesFromShapeList(const occ::handle<NCollection_HSequence<TopoDS_Shape>>& res,
                        const int                                               mode) const
{
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> lt =
    new NCollection_HSequence<occ::handle<Standard_Transient>>();
  if (res.IsNull())
    return lt;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> shapes;

  //  We convert res to a map, for rapid presence test
  int i, j, nb = res->Length();
  if (nb == 0)
    return lt;
  for (i = 1; i <= nb; i++)
    shapes.Add(res->Value(i));

  //  Now, search and registration

  XSControl_Utils xu;
  if (mode == 0 || mode == 1)
  {
    //  we look in the TransientProcess
    if (!myTP.IsNull())
    {
      nb = (mode == 0 ? myTP->NbRoots() : myTP->NbMapped());
      for (j = 1; j <= nb; j++)
      {
        i = (mode == 0 ? myModel->Number(myTP->Root(j)) : j);
        if (i == 0)
          continue;
        TopoDS_Shape sh = xu.BinderShape(myTP->MapItem(i));
        if (!sh.IsNull() && shapes.Contains(sh))
        {
          lt->Append(myTP->Mapped(i));
          j = nb; // skl (for looking for entities in checkbrep)
        }
      }
    }
  }

  //   Search in myResults (roots)
  //     2 : Main only  3 : Main + one sub;  4 : all
  if (mode >= 2)
  {
    nb = myModel->NbEntities();
    for (i = 1; i <= nb; i++)
    {
      occ::handle<Transfer_ResultFromModel> rec = ResultFromNumber(i);
      if (rec.IsNull())
        continue;

      occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> list =
        rec->Results(mode - 2);
      int ir, nr = list->Length();
      for (ir = 1; ir <= nr; ir++)
      {
        DeclareAndCast(Transfer_ResultFromTransient, rft, list->Value(i));
        if (rft.IsNull())
          continue;
        TopoDS_Shape sh = xu.BinderShape(rft->Binder());
        if (!sh.IsNull() && shapes.Contains(sh))
          lt->Append(rft->Start());
      }
    }
  }

  return lt;
}

//  <<<< >>>>  ATTENTION, level pas traite (utile ?) -> ResultFromModel

//=================================================================================================

Interface_CheckIterator XSControl_TransferReader::CheckList(
  const occ::handle<Standard_Transient>& ent,
  const int                              level) const
{
  Interface_CheckIterator chl;
  if (myModel.IsNull() || ent.IsNull())
    return chl;
  //  Check-List COMPLETE ... the entire Model
  if (ent == myModel)
  {
    int i, nb = myModel->NbEntities();
    for (i = 1; i <= nb; i++)
    {
      occ::handle<Transfer_ResultFromModel> rec = ResultFromNumber(i);
      if (!rec.IsNull())
      {
        Interface_CheckIterator chiter = rec->CheckList(false, 2);
        chl.Merge(chiter);
      }
    }
  }
  //  Check-List on a LIST ...
  else if (ent->IsKind(STANDARD_TYPE(NCollection_HSequence<occ::handle<Standard_Transient>>)))
  {
    DeclareAndCast(NCollection_HSequence<occ::handle<Standard_Transient>>, list, ent);
    int i, nb = list->Length();
    for (i = 1; i <= nb; i++)
    {
      occ::handle<Transfer_ResultFromModel> rec = FinalResult(list->Value(i));
      if (!rec.IsNull())
      {
        Interface_CheckIterator chiter = rec->CheckList(false, level);
        chl.Merge(chiter);
      }
    }
  }

  //  otherwise, Check-List on an entity: Last or FinalResult
  else if (level < 0)
  {
    if (myTP.IsNull())
      return chl;
    chl.Add(myTP->Check(ent), myModel->Number(ent));
  }
  else
  {
    occ::handle<Transfer_ResultFromModel> rec = FinalResult(ent);
    if (rec.IsNull())
      return chl;
    chl = rec->CheckList(false, level); // manque level ...
  }
  if (ent == myModel)
    chl.SetName("XSControl : CheckList complete Model");
  else if (level < 0)
    chl.SetName("XSControl : CheckList Last");
  else if (level == 0)
    chl.SetName("XSControl : CheckList Final Main");
  else if (level == 1)
    chl.SetName("XSControl : CheckList Final Main+Subs");
  else if (level >= 2)
    chl.SetName("XSControl : CheckList Final Complete");
  return chl;
}

//=================================================================================================

bool XSControl_TransferReader::HasChecks(const occ::handle<Standard_Transient>& ent,
                                         const bool                             failsonly) const
{
  occ::handle<Transfer_ResultFromModel> resu = FinalResult(ent);
  if (resu.IsNull())
    return false;
  int stat = resu->ComputeCheckStatus(false);
  if (stat == 0)
    return false;
  if (stat > 1)
    return true;
  return (!failsonly);
}

//=================================================================================================

occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> XSControl_TransferReader::
  CheckedList(const occ::handle<Standard_Transient>& ent,
              const Interface_CheckStatus            withcheck,
              const bool                             level) const
{
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> res =
    new NCollection_HSequence<occ::handle<Standard_Transient>>();
  if (ent.IsNull())
    return res;

  if (ent == myModel)
  {
    int i, nb = myModel->NbEntities();
    for (i = 1; i <= nb; i++)
    {
      occ::handle<Transfer_ResultFromModel> rec = ResultFromNumber(i);
      if (!rec.IsNull())
        res->Append(rec->CheckedList(withcheck, level));
    }
  }
  else if (ent->IsKind(STANDARD_TYPE(NCollection_HSequence<occ::handle<Standard_Transient>>)))
  {
    DeclareAndCast(NCollection_HSequence<occ::handle<Standard_Transient>>, list, ent);
    int i, nb = list->Length();
    for (i = 1; i <= nb; i++)
    {
      occ::handle<Transfer_ResultFromModel> rec = FinalResult(list->Value(i));
      if (!rec.IsNull())
        res->Append(rec->CheckedList(withcheck, level));
    }
  }
  else
  {
    occ::handle<Transfer_ResultFromModel> rec = FinalResult(ent);
    if (!rec.IsNull())
      res = rec->CheckedList(withcheck, level);
  }
  return res;
}

//  ########################################################
//  ###########            TRANSFERT            ############
//  ########################################################

//=================================================================================================

bool XSControl_TransferReader::BeginTransfer()
{
  if (myModel.IsNull())
    return false;
  if (Actor().IsNull())
    return false;
  myShapeResult.Nullify();

  if (myTP.IsNull())
    myTP = new Transfer_TransientProcess(myModel->NbEntities());

  occ::handle<Transfer_ActorOfTransientProcess> actor;
  myTP->SetActor(actor); // -> Reset
  actor = Actor();
  myTP->SetActor(actor); // Set properly
  myTP->SetErrorHandle(true);
  NCollection_DataMap<TCollection_AsciiString, occ::handle<Standard_Transient>>& aTPContext =
    myTP->Context();
  aTPContext = myContext;
  return true;
}

//=================================================================================================

bool XSControl_TransferReader::Recognize(const occ::handle<Standard_Transient>& ent)
{
  if (myActor.IsNull())
    return false;
  return myActor->Recognize(ent);
}

//=================================================================================================

int XSControl_TransferReader::TransferOne(const occ::handle<Standard_Transient>& ent,
                                          const bool                             rec,
                                          const Message_ProgressRange&           theProgress)
{
  if (myActor.IsNull() || myModel.IsNull())
    return 0;

  if (myTP.IsNull())
  {
    if (!BeginTransfer())
      return 0;
  }

  Message_Messenger::StreamBuffer sout  = myTP->Messenger()->SendInfo();
  int                             level = myTP->TraceLevel();

  Transfer_TransferOutput TP(myTP, myModel);
  if (myGraph.IsNull())
    myTP->SetModel(myModel);
  else
    myTP->SetGraph(myGraph);

  //  for the log-file
  if (level > 1)
  {
    int                                   num = myModel->Number(ent);
    occ::handle<TCollection_HAsciiString> lab = myModel->StringLabel(ent);
    sout << "\n*******************************************************************\n";
    sout << "******           Transferring one Entity                     ******" << std::endl;
    if (!lab.IsNull())
      sout << "******    N0 in file : " << Interface_MSG::Blanks(num, 5) << num
           << "      Ident : " << lab->ToCString() << Interface_MSG::Blanks(14 - lab->Length())
           << "******\n";
    sout << "******    Type : " << myModel->TypeName(ent, false)
         << Interface_MSG::Blanks((int)(44 - strlen(myModel->TypeName(ent, false)))) << "******";
    sout << "\n*******************************************************************\n";
  }

  //  only difference between TransferRoots and TransferOne
  int                                    res = 0;
  const occ::handle<Standard_Transient>& obj = ent;
  TP.Transfer(obj, theProgress);
  if (theProgress.UserBreak())
    return res;
  myTP->SetRoot(obj);

  //  Result ...
  occ::handle<Transfer_Binder> binder = myTP->Find(obj);
  if (binder.IsNull())
    return res;
  if (rec)
    RecordResult(obj);

  if (!binder->HasResult())
    return res;
  res++;

  return res;
}

//=================================================================================================

int XSControl_TransferReader::TransferList(
  const occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>& list,
  const bool                                                                 rec,
  const Message_ProgressRange&                                               theProgress)
{
  if (myActor.IsNull() || myModel.IsNull())
    return 0;

  if (myTP.IsNull())
  {
    if (!BeginTransfer())
      return 0;
  }

  int level = myTP->TraceLevel();

  Transfer_TransferOutput TP(myTP, myModel);
  if (myGraph.IsNull())
    myTP->SetModel(myModel);
  else
    myTP->SetGraph(myGraph);

  int i, nb = list->Length();

  //   For the log-file
  if (level > 0)
  {
    Message_Messenger::StreamBuffer sout = myTP->Messenger()->SendInfo();
    sout << "\n*******************************************************************\n";
    sout << "******           Transferring a list of " << Interface_MSG::Blanks(nb, 5)
         << " Entities       ******" << std::endl;
    sout << "\n*******************************************************************\n";

    occ::handle<IFSelect_SignatureList> sl = new IFSelect_SignatureList;
    for (i = 1; i <= nb; i++)
      sl->Add(list->Value(i), myModel->TypeName(list->Value(i), false));
    sl->SetName("Entities to Transfer");
    sl->PrintCount(sout);
    sout << "\n*******************************************************************\n";
  }

  //  only difference between TransferRoots and TransferOne
  int res = 0;
  nb      = list->Length();
  occ::handle<Standard_Transient> obj;
  Message_ProgressScope           aPS(theProgress, nullptr, nb);
  for (i = 1; i <= nb && aPS.More(); i++)
  {
    obj = list->Value(i);
    TP.Transfer(obj, aPS.Next());
    myTP->SetRoot(obj);

    //  Result ...
    occ::handle<Transfer_Binder> binder = myTP->Find(obj);
    if (binder.IsNull())
      continue;
    if (rec)
      RecordResult(obj);

    if (!binder->HasResult())
      continue;
    res++;
  }
  return res;
}

//  <<<< >>>>  Graph passing: judicious?

//=================================================================================================

int XSControl_TransferReader::TransferRoots(const Interface_Graph&       G,
                                            const Message_ProgressRange& theProgress)
{
  if (myModel != G.Model())
    return -1;
  if (!BeginTransfer())
    return -1;
  int level = myTP->TraceLevel();

  Transfer_TransferOutput TP(myTP, myModel);
  if (myGraph.IsNull())
    myTP->SetModel(myModel);
  else
    myTP->SetGraph(myGraph);

  //   For the log-file
  if (level > 0)
  {
    Interface_EntityIterator        roots = G.RootEntities();
    int                             nb    = roots.NbEntities();
    Message_Messenger::StreamBuffer sout  = myTP->Messenger()->SendInfo();
    sout << "\n*******************************************************************\n";
    sout << "******           Transferring the " << Interface_MSG::Blanks(nb, 5)
         << " Root Entities        ******" << std::endl;
    sout << "\n*******************************************************************\n";
    occ::handle<IFSelect_SignatureList> sl = new IFSelect_SignatureList;
    for (roots.Start(); roots.More(); roots.Next())
      sl->Add(roots.Value(), myModel->TypeName(roots.Value(), false));
    sl->SetName("Entities to Transfer");
    sl->PrintCount(sout);
    sout << "\n*******************************************************************\n";
  }

  TP.TransferRoots(G, theProgress);
  if (theProgress.UserBreak())
    return -1;

  //  The transferred entities are noted as "asmain"
  int i, n = myTP->NbMapped();
  for (i = 1; i <= n; i++)
  {
    occ::handle<Standard_Transient> ent = myTP->Mapped(i);
    occ::handle<Transfer_Binder>    bnd = myTP->MapItem(i);
    if (bnd.IsNull())
      continue;
    if (!bnd->HasResult())
      continue;
    RecordResult(ent);
  }

  //  Result ... we carefully note the Shapes
  myShapeResult = TransferBRep::Shapes(myTP, true);
  // ????  Et ici, il faut alimenter Imagine ...
  return myShapeResult->Length();
}

//=================================================================================================

void XSControl_TransferReader::TransferClear(const occ::handle<Standard_Transient>& ent,
                                             const int                              level)
{
  if (myTP.IsNull())
    return;
  if (ent == myModel)
  {
    myTP->Clear();
    return;
  }

  myTP->RemoveResult(ent, level);
  ClearResult(ent, -1);
}

//=================================================================================================

void XSControl_TransferReader::PrintStats(Standard_OStream& sout,
                                          const int         what,
                                          const int         mode) const
{
  //  To be improved ... !
  sout << "\n*******************************************************************\n";
  sout << "******        Statistics on Transfer (Read)                  ******" << std::endl;
  sout << "\n*******************************************************************\n";
  if (what > 10)
  {
    sout << " ***  Not yet implemented" << std::endl;
    return;
  }
  if (what < 10)
  {
    sout << "******        Data recorded on Last Transfer                 ******" << std::endl;
    PrintStatsProcess(myTP, what, mode);
  }
  //  remaining what = 10: we list the roots of final results
  sout << "******        Final Results                                  ******" << std::endl;
  if (myModel.IsNull())
  {
    sout << "****    Model unknown" << std::endl;
    return;
  }
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> list = RecordedList();
  int                                                                 i, nb = list->Length();
  occ::handle<IFSelect_SignatureList>                                 counter;
  if (mode > 2)
    counter = new IFSelect_SignatureList(mode == 6);
  IFSelect_PrintCount pcm = IFSelect_CountByItem;
  if (mode == 6)
    pcm = IFSelect_ListByItem;

  sout << "****    Nb Recorded : " << nb << " : entities n0s : ";
  for (i = 1; i <= nb; i++)
  {
    occ::handle<Standard_Transient> ent = list->Value(i);
    if (mode == 0)
    {
      sout << "  " << myModel->Number(ent);
      continue;
    }
    if (mode == 1 || mode == 2)
    {
      sout << "[ " << Interface_MSG::Blanks(i, 6) << " ]:";
      myModel->Print(ent, sout);
      sout << "  Type:" << myModel->TypeName(ent, false);
    }
    if (mode >= 3 && mode <= 6)
    {
      counter->Add(ent, myModel->TypeName(ent, false));
    }
  }
  if (!counter.IsNull())
    counter->PrintList(sout, myModel, pcm);

  sout << std::endl;
}

//  ########################################################
//  ###########            TRANSFERT            ############

//=================================================================================================

Interface_CheckIterator XSControl_TransferReader::LastCheckList() const
{
  Interface_CheckIterator chl;
  if (!myTP.IsNull())
    chl = myTP->CheckList(false);
  return chl;
}

//=================================================================================================

occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> XSControl_TransferReader::
  LastTransferList(const bool roots) const
{
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> li =
    new NCollection_HSequence<occ::handle<Standard_Transient>>();
  if (myTP.IsNull())
    return li;
  int i, j, nb = (roots ? myTP->NbRoots() : myTP->NbMapped());
  for (j = 1; j <= nb; j++)
  {
    i                                = (roots ? myModel->Number(myTP->Root(j)) : j);
    occ::handle<Transfer_Binder> bnd = myTP->MapItem(i);
    if (bnd.IsNull())
      continue;
    if (!bnd->HasResult())
      continue;
    li->Append(myTP->Mapped(i));
  }
  return li;
}

//=================================================================================================

const occ::handle<NCollection_HSequence<TopoDS_Shape>>& XSControl_TransferReader::ShapeResultList(
  const bool rec)
{
  if (!rec)
  {
    if (myShapeResult.IsNull())
      myShapeResult = TransferBRep::Shapes(myTP, true);
    if (myShapeResult.IsNull())
      myShapeResult = new NCollection_HSequence<TopoDS_Shape>();
  }
  else
  {
    if (myShapeResult.IsNull())
      myShapeResult = new NCollection_HSequence<TopoDS_Shape>();
    if (myModel.IsNull())
      return myShapeResult;
    occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> li = RecordedList();
    myShapeResult = new NCollection_HSequence<TopoDS_Shape>();
    int          i, nb = myModel->NbEntities();
    TopoDS_Shape sh;
    for (i = 1; i <= nb; i++)
    {
      sh = ShapeResult(myModel->Value(i));
      if (!sh.IsNull())
        myShapeResult->Append(sh);
    }
  }
  return myShapeResult;
}

//  ****    UTILITAIRE DE STATISTIQUES GENERALES

// BinderStatus returns a value:
// 0 Binder Null.   1 void  2 Warning only  3 Fail only
// 11 Result OK. 12 Result+Warning. 13 Result+Fail

//=================================================================================================

static int BinderStatus(const occ::handle<Transfer_Binder>& binder, char* mess)
{
  int stat = 0;
  mess[0]  = '\0';
  if (binder.IsNull())
  {
    Sprintf(mess, "(no data recorded)");
    return 0;
  }
  Interface_CheckStatus cst = binder->Check()->Status();
  if (cst == Interface_CheckOK)
  {
    stat = 11;
    if (binder->HasResult())
      Sprintf(mess, "%s", binder->ResultTypeName());
    else
    {
      Sprintf(mess, "(no result)");
      stat = 1;
    }
  }
  else if (cst == Interface_CheckWarning)
  {
    stat = 12;
    if (binder->HasResult())
      Sprintf(mess, "%s  (+ warning)", binder->ResultTypeName());
    else
    {
      Sprintf(mess, "(warning)");
      stat = 2;
    }
  }
  else if (cst == Interface_CheckFail)
  {
    stat = 13;
    if (binder->HasResult())
      Sprintf(mess, "%s  (+ FAIL)", binder->ResultTypeName());
    else
    {
      Sprintf(mess, "(FAIL)");
      stat = 3;
    }
  }
  return stat;
}

//=================================================================================================

static void PrintPercent(const occ::handle<Message_Messenger>& sout,
                         const char* const                     mess,
                         const int                             nb,
                         const int                             nl)
{
  if (nb <= 0 || nl == 0)
    return;
  Message_Messenger::StreamBuffer aSender = sout->SendInfo();
  aSender << "******      " << mess << ": ";
  if (nb == nl)
    aSender << "100 %" << std::endl;
  else if (nb * 100 / nl == 0)
    aSender << "< 1 %" << std::endl;
  else
    aSender << (nb * 100 / nl < 10 ? "  " : " ") << nb * 100 / nl << " %" << std::endl;
}

//=================================================================================================

void XSControl_TransferReader::PrintStatsProcess(const occ::handle<Transfer_TransientProcess>& TP,
                                                 const int                                     what,
                                                 const int                                     mode)
{
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> list; // null
  XSControl_TransferReader::PrintStatsOnList(TP, list, what, mode);
}

//=================================================================================================

void XSControl_TransferReader::PrintStatsOnList(
  const occ::handle<Transfer_TransientProcess>&                              TP,
  const occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>& list,
  const int                                                                  what,
  const int                                                                  mode)
{
  Message_Messenger::StreamBuffer sout = TP->Messenger()->SendInfo();

  char mess[250];
  if (TP.IsNull())
    return;
  if (what == 0)
  {
    TP->PrintStats(0, sout);
    return;
  }

  sout << "\n*******************************************************************\n";
  sout << "******        Statistics on Transfer Process (Read)          ******" << std::endl;
  if (what == 1)
    sout << "******        Individual Transfers  (Roots)                  ******\n";
  if (what == 2)
    sout << "******        All recorded data about Transfer               ******\n";
  if (what == 3)
    sout << "******        Abnormal records                               ******\n";
  if (what == 1 || what == 2 || what == 3)
  {
    if (mode == 0)
      sout << "******        (n0s of recorded entities)                     ******\n";
    if (mode == 1)
      sout << "******        (per entity : type + result)                   ******\n";
    if (mode == 2)
      sout << "******        (per entity : type + result/status)            ******\n";
    if (mode == 3)
      sout << "******        (count per type of entity)                     ******\n";
    if (mode == 4)
      sout << "******        (count per type of result)                     ******\n";
    if (mode == 5)
      sout << "******   (count per couple entity-type / result-type/status) ******\n";
    if (mode == 6)
      sout << "******   (list per couple entity-type / result-type/status)  ******\n";
  }
  if (what == 4)
    sout << "******        Check messages                                 ******\n";
  if (what == 5)
    sout << "******        Fail  messages                                 ******\n";
  sout << "*******************************************************************\n";

  //  Case what = 1,2,3: content of TP (binders)

  bool                                  nolist = list.IsNull();
  occ::handle<Interface_InterfaceModel> model  = TP->Model();
  if (what >= 1 && what <= 3)
  {

    int stat;
    int nbw = 0, nbf = 0, nbr = 0, nbrw = 0, nbrf = 0, nbnr = 0, nbi = 0;
    Transfer_IteratorOfProcessForTransient itrp(true);
    if (what == 1)
      itrp = TP->RootResult(true);
    if (what == 2)
      itrp = TP->CompleteResult(true);
    if (what == 3)
      itrp = TP->AbnormalResult();
    int i = 0, nb = itrp.Number();
    if (!nolist)
      itrp.Filter(list);
    int                                 nl = itrp.Number(); // after filtering
    occ::handle<IFSelect_SignatureList> counter;
    if (mode > 2)
      counter = new IFSelect_SignatureList(mode == 6);
    bool                notrec = (!nolist && mode > 2); // note the "no record"
    IFSelect_PrintCount pcm    = IFSelect_CountByItem;
    if (mode == 6)
      pcm = IFSelect_ListByItem;

    sout << "****        Entities in Model   : " << model->NbEntities() << std::endl;
    sout << "****        Nb Items (Transfer) : " << nb << std::endl;
    if (!nolist)
      sout << "****        Nb Items (Listed)   : " << nl << std::endl;

    for (itrp.Start(); itrp.More(); itrp.Next())
    {
      nbi++;
      const occ::handle<Transfer_Binder>&    binder = itrp.Value();
      const occ::handle<Standard_Transient>& ent    = itrp.Starting();
      if (binder.IsNull())
      {
        nbnr++;
        if (notrec)
          counter->Add(ent, "(not recorded)");
        else if (mode == 1 || mode == 2)
        {
          sout << "[" << Interface_MSG::Blanks(nbi, 4) << nbi << " ]:";
          model->Print(ent, sout);
          sout << "   " << model->TypeName(ent, false) << "  (not recorded)" << std::endl;
          continue;
        }
      }
      if (mode == 0)
      {
        sout << "  " << model->Number(ent);
        continue;
      }
      if (mode != 3)
      {
        stat = BinderStatus(binder, mess);
        // 0 Binder Null.   1 void  2 Warning only  3 Fail only
        // 11 Result OK. 12 Result+Warning. 13 Result+Fail
        if (stat == 2)
          nbw++;
        if (stat == 3)
          nbf++;
        if (stat == 11)
          nbr++;
        if (stat == 12)
          nbrw++;
        if (stat == 13)
          nbrf++;
      }

      //  mode : 0 list num;  1 : num+label + type + result (abrege);  2 : complet
      if (mode == 1 || mode == 2)
      {
        sout << "[" << Interface_MSG::Blanks(i, 4) << i << " ]:";
        model->Print(ent, sout);
        sout << "   " << model->TypeName(ent, false);
        sout << "	Result:" << mess << std::endl;
        if (mode == 1)
          continue;

        const occ::handle<Interface_Check>& ch = binder->Check();
        int                                 newi, newnbw = ch->NbWarnings(), newnbf = ch->NbFails();

        if (newnbw > 0)
        {
          sout << " - Warnings : " << newnbw << ":\n";
          for (newi = 1; newi <= newnbw; newi++)
            sout << ch->CWarning(newi) << std::endl;
        }
        if (newnbf > 0)
        {
          sout << " - Fails : " << newnbf << ":\n";
          for (newi = 1; newi <= newnbf; newi++)
            sout << ch->CFail(newi) << std::endl;
        }
        continue;
      }

      //  mode : 3, counts per type of starting entity (class type)
      //         4 : counts per result type and/or status
      //         5 : counts per couple (starting type / result type/status)
      //         6 : idem plus gives for each item, the list of numbers of
      //                  entities in the starting model
      if (mode >= 3 && mode <= 6)
      {
        // IFSelect_PrintCount newpcm = IFSelect_CountByItem;
        // if (mode == 6) newpcm = IFSelect_ListByItem;
        if (mode == 3)
          counter->Add(ent, model->TypeName(ent, false));
        if (mode == 4)
          counter->Add(ent, mess);
        if (mode >= 5)
        {
          TCollection_AsciiString mest(model->TypeName(ent, false));
          mest.AssignCat("	-> ");
          mest.AssignCat(mess);
          // Sprintf(mest,"%s	-> %s",model->TypeName(ent,false),mess);
          counter->Add(ent, mest.ToCString());
        }
      }

      //    End of iteration
    }
    if (!counter.IsNull())
      counter->PrintList(sout, model, pcm);
    else
      sout << std::endl;
    //    Pourcentages
    if (mode != 3 && nbi > 0)
    {
      sout << "******        Percentages according Transfer Status          ******" << std::endl;
      PrintPercent(TP->Messenger(), "Result          ", nbr + nbrw, nl);
      PrintPercent(TP->Messenger(), "Result + FAIL   ", nbrf, nl);
      PrintPercent(TP->Messenger(), "FAIL, no Result ", nbf, nl);
      PrintPercent(TP->Messenger(), "Just Warning    ", nbw, nl);
      PrintPercent(TP->Messenger(), "Nothing Recorded", nbnr, nl);
      /*      if (nbr+nbrw > 0)
          sout<<"******      Result          : "<< (nbr+nbrw)*100/nl<<" %"<<std::endl;
            if (nbrf > 0)
          sout<<"******      Result + FAIL   : "<< (nbrf)*100/nl<<" %"<<std::endl;
            if (nbf > 0)
          sout<<"******      FAIL, no Result : "<< (nbf)*100/nl<<" %"<<std::endl;
            if (nbw > 0)
          sout<<"******      Just Warning    : "<< (nbw)*100/nl<<" %"<<std::endl;
            if (nbnr > 0)
          sout<<"******      Nothing Recorded: "<< (nbnr)*100/nl<<" %"<<std::endl; */
    }
    return;
  }

  //  Case what = 4,5: check-list

  if (what == 4 || what == 5)
  {

    Interface_CheckIterator chl = TP->CheckList(false);
    chl.SetName("** TRANSFER READ CHECK **");
    if (mode == 0)
    {
      chl.Print(sout, model, (what == 5));
    }
    else
    {
      IFSelect_PrintCount pcm = IFSelect_CountByItem;
      if (mode == 2)
        pcm = IFSelect_ListByItem;
      occ::handle<IFSelect_CheckCounter> counter = new IFSelect_CheckCounter(true);
      counter->Analyse(chl, model, true, (what == 5));
      counter->PrintList(sout, model, pcm);
    }
  }
}
