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

// #1 svv   10.01.00 : porting on DEC
// smh#14 17.03.2000 : FRA62479 Clearing of gtool

#include <IFGraph_SubPartsIterator.hxx>
#include <IFSelect_CheckCounter.hxx>
#include <IFSelect_EditForm.hxx>
#include <IFSelect_IntParam.hxx>
#include <IFSelect_ModelCopier.hxx>
#include <IFSelect_Modifier.hxx>
#include <IFSelect_PacketList.hxx>
#include <IFSelect_ParamEditor.hxx>
#include <IFSelect_SelectDiff.hxx>
#include <IFSelect_SelectIntersection.hxx>
#include <IFSelect_Selection.hxx>
#include <IFSelect_SelectionIterator.hxx>
#include <IFSelect_SelectModelEntities.hxx>
#include <IFSelect_SelectPointed.hxx>
#include <IFSelect_SelectSignature.hxx>
#include <IFSelect_ShareOut.hxx>
#include <IFSelect_ShareOutResult.hxx>
#include <IFSelect_SignatureList.hxx>
#include <IFSelect_SignCounter.hxx>
#include <IFSelect_SignValidity.hxx>
#include <IFSelect_TransformStandard.hxx>
#include <IFSelect_WorkLibrary.hxx>
#include <IFSelect_WorkSession.hxx>
#include <Interface_Category.hxx>
#include <Interface_Check.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_CheckTool.hxx>
#include <Interface_CopyControl.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_GeneralModule.hxx>
#include <Interface_Graph.hxx>
#include <Interface_GTool.hxx>
#include <Interface_HGraph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_MSG.hxx>
#include <Interface_Protocol.hxx>
#include <Interface_ShareFlags.hxx>
#include <Interface_ShareTool.hxx>
#include <Interface_Static.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <OSD_Path.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Map.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_WorkSession, Standard_Transient)

#define Flag_Incorrect 2
//  (Bit Map n0 2)

static bool        errhand; // pb : only one at a time, but it goes so fast
static TCollection_AsciiString bufstr;

//  #################################################################

//=================================================================================================

IFSelect_WorkSession::IFSelect_WorkSession()
{
  theshareout = new IFSelect_ShareOut;
  theerrhand = errhand = true;
  thecopier            = new IFSelect_ModelCopier;
  thecopier->SetShareOut(theshareout);
  thecheckdone = false;
  thegtool     = new Interface_GTool;
  themodelstat = false;
}

//=================================================================================================

void IFSelect_WorkSession::SetErrorHandle(const bool toHandle)
{
  theerrhand = errhand = toHandle;
}

//=================================================================================================

void IFSelect_WorkSession::SetProtocol(const occ::handle<Interface_Protocol>& protocol)
{
  theprotocol = protocol;
  Interface_Protocol::SetActive(protocol);
  thegtool->SetProtocol(protocol);
}

//=================================================================================================

void IFSelect_WorkSession::SetSignType(const occ::handle<IFSelect_Signature>& signtype)
{
  thegtool->SetSignType(signtype);
  if (signtype.IsNull())
    thenames.UnBind("xst-sign-type");
  else
    thenames.Bind("xst-sign-type", signtype);
}

//=================================================================================================

occ::handle<IFSelect_Signature> IFSelect_WorkSession::SignType() const
{
  return occ::down_cast<IFSelect_Signature>(thegtool->SignType());
}

//=================================================================================================

void IFSelect_WorkSession::SetShareOut(const occ::handle<IFSelect_ShareOut>& shareout)
{
  theshareout = shareout;
  thecopier->SetShareOut(theshareout);
  // ... should add Params, Dispatches, etc...
}

//=================================================================================================

void IFSelect_WorkSession::SetModel(const occ::handle<Interface_InterfaceModel>& model,
                                    const bool                  clearpointed)
{
  if (myModel != model)
    theloaded.Clear();
  myModel = model;
  if (!thegtool.IsNull())
    thegtool->ClearEntities(); // smh#14 FRA62479
  myModel->SetGTool(thegtool);

  thegraph.Nullify();
  ComputeGraph(); // does something if Protocol present. Otherwise, does nothing
  ClearData(3);   // Reset CheckList, to be redone
  thecheckrun.Clear();

  //  UPDATE of SelectPointed  I.E.  clear their content
  if (clearpointed)
    ClearData(4);
  ClearData(0);
}

//=================================================================================================

IFSelect_ReturnStatus IFSelect_WorkSession::ReadFile(const char* filename)
{
  if (thelibrary.IsNull())
    return IFSelect_RetVoid;
  if (theprotocol.IsNull())
    return IFSelect_RetVoid;
  occ::handle<Interface_InterfaceModel> model;
  IFSelect_ReturnStatus            status = IFSelect_RetVoid;
  try
  {
    OCC_CATCH_SIGNALS
    int stat = thelibrary->ReadFile(filename, model, theprotocol);
    if (stat == 0)
      status = IFSelect_RetDone;
    else if (stat < 0)
      status = IFSelect_RetError;
    else
      status = IFSelect_RetFail;
  }
  catch (Standard_Failure const& anException)
  {
    Message_Messenger::StreamBuffer sout = Message::SendInfo();
    sout << "    ****    ReadFile Interruption by Exception :   ****\n";
    sout << anException.GetMessageString();
    sout << "\n    Abandon" << std::endl;
    status = IFSelect_RetFail;
  }
  if (status != IFSelect_RetDone)
    return status;
  if (model.IsNull())
    return IFSelect_RetVoid;
  SetModel(model);
  SetLoadedFile(filename);
  return status;
}

//=================================================================================================

IFSelect_ReturnStatus IFSelect_WorkSession::ReadStream(const char* theName,
                                                       std::istream&          theIStream)
{
  if (thelibrary.IsNull())
    return IFSelect_RetVoid;
  if (theprotocol.IsNull())
    return IFSelect_RetVoid;
  occ::handle<Interface_InterfaceModel> model;
  IFSelect_ReturnStatus            status = IFSelect_RetVoid;
  try
  {
    OCC_CATCH_SIGNALS
    int stat = thelibrary->ReadStream(theName, theIStream, model, theprotocol);
    if (stat == 0)
      status = IFSelect_RetDone;
    else if (stat < 0)
      status = IFSelect_RetError;
    else
      status = IFSelect_RetFail;
  }
  catch (Standard_Failure const& anException)
  {
    Message_Messenger::StreamBuffer sout = Message::SendInfo();
    sout << "    ****    ReadFile Interruption by Exception :   ****\n";
    sout << anException.GetMessageString();
    sout << "\n    Abandon" << std::endl;
    status = IFSelect_RetFail;
  }
  if (status != IFSelect_RetDone)
    return status;
  if (model.IsNull())
    return IFSelect_RetVoid;
  SetModel(model);
  SetLoadedFile(theName);
  return status;
}

//=================================================================================================

int IFSelect_WorkSession::NbStartingEntities() const
{
  return (myModel.IsNull() ? 0 : myModel->NbEntities());
}

//=================================================================================================

occ::handle<Standard_Transient> IFSelect_WorkSession::StartingEntity(const int num) const
{
  occ::handle<Standard_Transient> res; // Null by default
  if (myModel.IsNull())
    return res;
  if (num < 1 || num > myModel->NbEntities())
    return res;
  return myModel->Value(num);
}

//=================================================================================================

int IFSelect_WorkSession::StartingNumber(const occ::handle<Standard_Transient>& ent) const
{
  return (myModel.IsNull() ? 0 : myModel->Number(ent));
}

//=================================================================================================

int IFSelect_WorkSession::NumberFromLabel(const char* val,
                                                       const int afternum) const
{
  int i, cnt = 0, num = atoi(val);
  if (num > 0 || myModel.IsNull())
    return num; // a direct n0 : won !
                //  Otherwise, consider it as a label; to be treated in CaseNonSensitive ...
  if (num > myModel->NbEntities())
  {
    num = 0;
    return num;
  }
  bool exact = false;
  int after = (afternum >= 0 ? afternum : -afternum);
  for (i = myModel->NextNumberForLabel(val, after, exact); i != 0;
       i = myModel->NextNumberForLabel(val, i, exact))
  {
    cnt++;
    if (num <= 0)
      num = i;
  }
  if (cnt == 1)
    return num;
  num = -num;

  return num;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> IFSelect_WorkSession::EntityLabel(
  const occ::handle<Standard_Transient>& ent) const
{
  if (myModel.IsNull() || ent.IsNull())
    return 0;
  if (!myModel->Contains(ent))
    return 0;
  return myModel->StringLabel(ent);
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> IFSelect_WorkSession::EntityName(
  const occ::handle<Standard_Transient>& ent) const
{
  if (myModel.IsNull() || ent.IsNull())
    return 0;
  Interface_ShareTool sht(thegraph->Graph());

  int                CN;
  occ::handle<Interface_GeneralModule> module;
  if (!thegtool->Select(ent, module, CN))
    return 0;
  return module->Name(CN, ent, sht);
}

//=================================================================================================

int IFSelect_WorkSession::CategoryNumber(const occ::handle<Standard_Transient>& ent) const
{
  return (myModel.IsNull() ? -1 : myModel->CategoryNumber(StartingNumber(ent)));
}

//=================================================================================================

const char* IFSelect_WorkSession::CategoryName(const occ::handle<Standard_Transient>& ent) const
{
  return Interface_Category::Name(CategoryNumber(ent));
}

//=================================================================================================

const char* IFSelect_WorkSession::ValidityName(const occ::handle<Standard_Transient>& ent) const
{
  return ((StartingNumber(ent) == 0) ? "" : IFSelect_SignValidity::CVal(ent, myModel));
}

//=================================================================================================

void IFSelect_WorkSession::ClearData(const int mode)
{
  switch (mode)
  {
    case 1: {
      theloaded.Clear();
      if (!myModel.IsNull())
      {
        myModel->Clear();
        myModel.Nullify();
      }
      ClearData(2);
      ClearData(4);
      thecheckrun.Clear();
      break;
    }
    case 2: {
      thegraph.Nullify();
      thecheckdone = false;
      thecheckana.Clear();
      break;
    }
    case 3: {
      thecheckdone = false;
      break;
    }
    case 4: {
      //  UPDATE of SelectPointed  I.E.  clear their content
      //  AS WELL AS editors (in fact, the EditForm)
      //  Counters  I.E.  we erase their content (to reevaluate)
      occ::handle<NCollection_HSequence<int>> list = ItemIdents(STANDARD_TYPE(IFSelect_SelectPointed));
      int                   nb   = list->Length();
      int                   i; // svv #1
      for (i = 1; i <= nb; i++)
      {
        DeclareAndCast(IFSelect_SelectPointed, sp, Item(list->Value(i)));
        if (!sp.IsNull())
          sp->Clear();
      }
      list = ItemIdents(STANDARD_TYPE(IFSelect_SignatureList));
      nb   = list->Length();
      for (i = 1; i <= nb; i++)
      {
        DeclareAndCast(IFSelect_SignatureList, sl, Item(list->Value(i)));
        if (!sl.IsNull())
          sl->Clear();
        DeclareAndCast(IFSelect_SignCounter, sc, sl);
        if (!sc.IsNull())
          sc->SetSelMode(-1);
      }
      list = ItemIdents(STANDARD_TYPE(IFSelect_EditForm));
      nb   = list->Length();
      occ::handle<Standard_Transient> nulent;
      for (i = 1; i <= nb; i++)
      {
        DeclareAndCast(IFSelect_EditForm, edf, Item(list->Value(i)));
        edf->ClearData();
      }
      theitems.Clear();
      break;
    }
    default:
      break;
  }
}

//=================================================================================================

bool IFSelect_WorkSession::ComputeGraph(const bool enforce)
{
  if (theprotocol.IsNull())
    return false;
  if (myModel.IsNull())
    return false;
  if (enforce)
    thegraph.Nullify();
  if (!thegraph.IsNull())
  {
    if (myModel->NbEntities() == thegraph->Graph().Size())
      return true;
    thegraph.Nullify();
  }
  if (myModel->NbEntities() == 0)
    return false;
  //  We must calculate the graph for good
  thegraph            = new Interface_HGraph(myModel, themodelstat);
  int nb = myModel->NbEntities();
  if (themodelstat)
  {
    int i; // svv #1
    for (i = 1; i <= nb; i++)
      thegraph->CGraph().SetStatus(i, 0);
    Interface_BitMap& bm = thegraph->CGraph().CBitMap();
    bm.AddFlag();
    bm.SetFlagName(Flag_Incorrect, "Incorrect");
  }
  ComputeCheck();
  thecheckdone = true;
  if (themodelstat)
  {
    //  Calculation of categories, now memorized in the model
    Interface_Category  categ(thegtool);
    Interface_ShareTool sht(thegraph);
    int    i = 1;
    for (; i <= nb; i++)
      myModel->SetCategoryNumber(i, categ.CatNum(myModel->Value(i), sht));
  }

  return true;
}

//=================================================================================================

occ::handle<Interface_HGraph> IFSelect_WorkSession::HGraph()
{
  ComputeGraph();
  return thegraph;
}

//=================================================================================================

const Interface_Graph& IFSelect_WorkSession::Graph()
{
  ComputeGraph();
  if (thegraph.IsNull())
    throw Standard_DomainError("IFSelect WorkSession : Graph not available");
  return thegraph->Graph();
}

//=================================================================================================

occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> IFSelect_WorkSession::Shareds(
  const occ::handle<Standard_Transient>& ent)
{
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> list;
  if (!ComputeGraph())
    return list;
  if (StartingNumber(ent) == 0)
    return list;
  return thegraph->Graph().Shareds(ent).Content();
}

//=================================================================================================

occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> IFSelect_WorkSession::Sharings(
  const occ::handle<Standard_Transient>& ent)
{
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> list;
  if (!ComputeGraph())
    return list;
  if (StartingNumber(ent) == 0)
    return list;
  return thegraph->Graph().Sharings(ent).Content();
}

//=================================================================================================

bool IFSelect_WorkSession::IsLoaded() const
{
  if (theprotocol.IsNull())
    return false;
  if (myModel.IsNull())
    return false;
  if (myModel->NbEntities() == 0)
    return false;
  if (thegraph.IsNull())
    return false;
  if (myModel->NbEntities() == thegraph->Graph().Size())
    return true;
  return false;
}

//=================================================================================================

bool IFSelect_WorkSession::ComputeCheck(const bool enforce)
{
  if (enforce)
    thecheckdone = false;
  if (thecheckdone)
    return true;
  if (!IsLoaded())
    return false;

  Interface_Graph&        CG = thegraph->CGraph();
  Interface_CheckTool     cht(thegraph);
  Interface_CheckIterator checklist = cht.VerifyCheckList();
  myModel->FillSemanticChecks(checklist, false);
  if (themodelstat)
  {
    //  And we update the Graph (BitMap) !  Flag Incorrect (STX + SEM)
    Interface_BitMap& BM = CG.CBitMap();
    BM.Init(false, Flag_Incorrect);
    int num, nb = CG.Size();
    for (checklist.Start(); checklist.More(); checklist.Next())
    {
      const occ::handle<Interface_Check>& chk = checklist.Value();
      if (!chk->HasFailed())
        continue;
      num = checklist.Number();
      if (num > 0 && num <= nb)
        BM.SetTrue(num, Flag_Incorrect);
    }
    for (num = 1; num <= nb; num++)
      if (myModel->IsErrorEntity(num))
        BM.SetTrue(num, Flag_Incorrect);
  }
  return true;
}

//=================================================================================================

Interface_CheckIterator IFSelect_WorkSession::ModelCheckList(const bool complete)
{
  Interface_CheckIterator checks;
  if (!IsLoaded())
  {
    checks.CCheck(0)->AddFail("DATA NOT AVAILABLE FOR CHECK");
    return checks;
  }
  Interface_CheckTool cht(Graph());
  checks = (complete ? cht.CompleteCheckList() : cht.AnalyseCheckList());
  checks.SetName((char*)(complete ? "Model Complete Check List" : "Model Syntactic Check List"));
  return checks;
}

//=================================================================================================

Interface_CheckIterator IFSelect_WorkSession::CheckOne(const occ::handle<Standard_Transient>& ent,
                                                       const bool            complete)
{
  Interface_CheckIterator checks;
  checks.SetModel(myModel);
  if (!IsLoaded())
  {
    checks.CCheck(0)->AddFail("DATA NOT AVAILABLE FOR CHECK");
    return checks;
  }
  int num = -1;
  if (ent.IsNull() || ent == myModel)
    num = 0;
  else
    num = myModel->Number(ent);

  occ::handle<Interface_Check> ach = myModel->Check(num, true);
  if (complete)
    ach->GetMessages(myModel->Check(num, false));
  if (num > 0)
    ach->SetEntity(ent);
  checks.Add(ach, num);
  checks.SetName("Data Check (One Entity)");
  return checks;
}

//  #####################################################################
//  ....                        THE VARIABLES                        ....

//=================================================================================================

int IFSelect_WorkSession::MaxIdent() const
{
  return theitems.Extent();
}

//=================================================================================================

occ::handle<Standard_Transient> IFSelect_WorkSession::Item(const int id) const
{
  occ::handle<Standard_Transient> res;
  if (id <= 0 || id > MaxIdent())
    return res;
  if (theitems.FindFromIndex(id).IsNull())
    return res;
  return theitems.FindKey(id);
}

//=================================================================================================

int IFSelect_WorkSession::ItemIdent(const occ::handle<Standard_Transient>& item) const
{
  if (item.IsNull())
    return 0;
  int id = theitems.FindIndex(item);
  if (id == 0)
    return 0;
  if (theitems.FindFromIndex(id).IsNull())
    return 0;
  return id;
}

//=================================================================================================

occ::handle<Standard_Transient> IFSelect_WorkSession::NamedItem(const char* name) const
{
  occ::handle<Standard_Transient> res;
  if (name[0] == '\0')
    return res;
  if (name[0] == '#')
  { // #nnn : not a name but an id number
    int id = atoi(&name[1]);
    return Item(id);
  }
  if (!thenames.Find(name, res))
    res.Nullify();
  return res;
}

//=================================================================================================

occ::handle<Standard_Transient> IFSelect_WorkSession::NamedItem(
  const occ::handle<TCollection_HAsciiString>& name) const
{
  occ::handle<Standard_Transient> res;
  if (!name.IsNull())
    res = NamedItem(name->ToCString());
  return res;
}

//=================================================================================================

int IFSelect_WorkSession::NameIdent(const char* name) const
{
  occ::handle<Standard_Transient> res;
  if (name[0] == '\0')
    return 0;
  if (name[0] == '#')
  { // #nnn : not a name but an id number
    int id = atoi(&name[1]);
    return id;
  }
  if (!thenames.Find(name, res))
    return 0;
  return ItemIdent(res);
}

//=================================================================================================

bool IFSelect_WorkSession::HasName(const occ::handle<Standard_Transient>& item) const
{
  if (item.IsNull())
    return false;
  int id = theitems.FindIndex(item);
  if (id == 0)
    return false;
  occ::handle<Standard_Transient> att = theitems.FindFromIndex(id);
  if (att.IsNull())
    return false;
  return att->IsKind(STANDARD_TYPE(TCollection_HAsciiString));
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> IFSelect_WorkSession::Name(
  const occ::handle<Standard_Transient>& item) const
{
  occ::handle<TCollection_HAsciiString> res;
  if (item.IsNull())
    return res;
  int id = theitems.FindIndex(item);
  if (id == 0)
    return res; // Null
  occ::handle<Standard_Transient> att = theitems.FindFromIndex(id);
  return GetCasted(TCollection_HAsciiString, att);
}

//=================================================================================================

int IFSelect_WorkSession::AddItem(const occ::handle<Standard_Transient>& item,
                                               const bool            active)
{
  if (item.IsNull())
    return 0;
  int id = theitems.FindIndex(item);
  if (id > 0)
  {
    occ::handle<Standard_Transient>& att = theitems.ChangeFromIndex(id);
    if (att.IsNull())
      att = item;
  }
  else
    id = theitems.Add(item, item);

  //  Special cases : Dispatch,Modifier
  if (active)
    SetActive(item, true);
  return id;
}

//=================================================================================================

int IFSelect_WorkSession::AddNamedItem(const char*            name,
                                                    const occ::handle<Standard_Transient>& item,
                                                    const bool            active)
{
  if (item.IsNull())
    return 0;
  if (name[0] == '#' || name[0] == '!')
    return 0;
  // #nnn : not a name but a number. !... : reserved (forbidden for a name)
  //   name already taken : we overwrite the old value
  if (name[0] != '\0')
    thenames.Bind(name, item);

  int id = theitems.FindIndex(item);
  if (id > 0)
  {
    occ::handle<Standard_Transient>& att = theitems.ChangeFromIndex(id);
    if (att.IsNull())
      att = item;
    if (name[0] != '\0')
    {
      //      if (!att->IsKind(STANDARD_TYPE(TCollection_HAsciiString))) overwriting allowed !
      att = new TCollection_HAsciiString(name);
    }
  }
  else if (name[0] != '\0')
    id = theitems.Add(item, new TCollection_HAsciiString(name));
  else
    id = theitems.Add(item, item);

  //  Special cases : Dispatch,Modifier
  if (active)
    SetActive(item, true);
  return id;
}

//=================================================================================================

bool IFSelect_WorkSession::SetActive(const occ::handle<Standard_Transient>& item,
                                                 const bool            mode)
{
  if (item->IsKind(STANDARD_TYPE(IFSelect_Dispatch)))
  {
    DeclareAndCast(IFSelect_Dispatch, disp, item);
    int num = theshareout->DispatchRank(disp);
    if (num > theshareout->NbDispatches())
      return false;
    if (mode)
    {
      if (num > 0)
        return false;
      theshareout->AddDispatch(disp);
      return true;
    }
    else
    {
      if (num <= theshareout->LastRun())
        return false;
      theshareout->RemoveDispatch(num);
      SetFileRoot(disp, ""); // if onlynamed : also cleans ShareOut
      return true;
    }
  }

  return false;
}

//=================================================================================================

bool IFSelect_WorkSession::RemoveNamedItem(const char* name)
{
  occ::handle<Standard_Transient> item = NamedItem(name);
  if (item.IsNull())
    return false;
  if (!RemoveItem(item))
    return false; // which takes care of everything
  return true;
}

//=================================================================================================

bool IFSelect_WorkSession::RemoveName(const char* name)
{
  occ::handle<Standard_Transient> item = NamedItem(name);
  if (item.IsNull())
    return false;
  theitems.Add(item, item); // remains but without name
  return thenames.UnBind(name);
}

//=================================================================================================

bool IFSelect_WorkSession::RemoveItem(const occ::handle<Standard_Transient>& item)
{
  if (item.IsNull())
    return false;
  int id = theitems.FindIndex(item);
  if (id == 0)
    return false;
  occ::handle<Standard_Transient>& att = theitems.ChangeFromIndex(id);
  if (att.IsNull())
    return false; // already canceled

  //  Special cases : Dispatch,Modifier
  theshareout->RemoveItem(item);

  //  Mark "Removed" in the Map (we cannot empty it)
  if (att->IsKind(STANDARD_TYPE(TCollection_HAsciiString)))
  {
    if (!thenames.UnBind(GetCasted(TCollection_HAsciiString, att)->ToCString()))
      return false;
  }
  att.Nullify(); // cf ChangeFromIndex
                 //  id = theitems.Add(item,att);
  return true;
}

//=================================================================================================

void IFSelect_WorkSession::ClearItems()
{
  thenames.Clear();
  theitems.Clear();
  theshareout->Clear(false);
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> IFSelect_WorkSession::ItemLabel(const int id) const
{
  occ::handle<TCollection_HAsciiString> res;
  occ::handle<Standard_Transient>       var = Item(id);
  if (var.IsNull())
    return res;
  DeclareAndCast(TCollection_HAsciiString, text, var);
  if (!text.IsNull())
  {
    res = new TCollection_HAsciiString("Text:");
    res->AssignCat(text);
    return res;
  }
  DeclareAndCast(IFSelect_IntParam, intpar, var);
  if (!intpar.IsNull())
  {
    res = new TCollection_HAsciiString(intpar->Value());
    res->Insert(1, "Integer:");
    return res;
  }
  DeclareAndCast(IFSelect_Selection, sel, var);
  if (!sel.IsNull())
  {
    res = new TCollection_HAsciiString("Selection:");
    res->AssignCat(sel->Label().ToCString());
    return res;
  }
  DeclareAndCast(IFSelect_GeneralModifier, mod, var);
  if (!mod.IsNull())
  {
    if (mod->IsKind(STANDARD_TYPE(IFSelect_Modifier)))
      res = new TCollection_HAsciiString("ModelModifier:");
    else
      res = new TCollection_HAsciiString("FileModifier:");
    res->AssignCat(mod->Label().ToCString());
    return res;
  }
  DeclareAndCast(IFSelect_Dispatch, disp, var);
  if (!disp.IsNull())
  {
    res = new TCollection_HAsciiString("Dispatch:");
    res->AssignCat(disp->Label().ToCString());
    return res;
  }
  DeclareAndCast(IFSelect_Transformer, tsf, var);
  if (!tsf.IsNull())
  {
    res = new TCollection_HAsciiString("Transformer:");
    res->AssignCat(tsf->Label().ToCString());
    return res;
  }
  DeclareAndCast(IFSelect_SignatureList, slc, var);
  if (!slc.IsNull())
  {
    res = new TCollection_HAsciiString("Counter:");
    res->AssignCat(slc->Name());
    return res;
  }
  DeclareAndCast(IFSelect_Signature, sig, var);
  if (!sig.IsNull())
  {
    res = new TCollection_HAsciiString("Signature:");
    res->AssignCat(sig->Name());
    return res;
  }
  DeclareAndCast(IFSelect_EditForm, edf, var);
  if (!edf.IsNull())
  {
    res = new TCollection_HAsciiString("EditForm:");
    res->AssignCat(edf->Label());
    return res;
  }
  DeclareAndCast(IFSelect_Editor, edt, var);
  if (!edt.IsNull())
  {
    res = new TCollection_HAsciiString("Editor:");
    res->AssignCat(edt->Label().ToCString());
    return res;
  }
  res = new TCollection_HAsciiString("VariableType:");
  res->AssignCat(var->DynamicType()->Name());
  return res;
}

//=================================================================================================

occ::handle<NCollection_HSequence<int>> IFSelect_WorkSession::ItemIdents(
  const occ::handle<Standard_Type>& type) const
{
  occ::handle<NCollection_HSequence<int>> list = new NCollection_HSequence<int>();
  int                   nb   = theitems.Extent();
  for (int i = 1; i <= nb; i++)
  {
    if (theitems.FindKey(i)->IsKind(type))
      list->Append(i);
  }
  return list;
}

//=================================================================================================

occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> IFSelect_WorkSession::ItemNames(
  const occ::handle<Standard_Type>& type) const
{
  occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> list = new NCollection_HSequence<occ::handle<TCollection_HAsciiString>>();
  NCollection_DataMap<TCollection_AsciiString, occ::handle<Standard_Transient>>::Iterator IT(thenames);
  for (; IT.More(); IT.Next())
  {
    if (IT.Value()->IsKind(type))
      list->Append(new TCollection_HAsciiString(IT.Key()));
  }
  return list;
}

// ..  Search by label : search in list(names) or iterative

//=================================================================================================

occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> IFSelect_WorkSession::ItemNamesForLabel(
  const char* label) const
{
  occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> list = new NCollection_HSequence<occ::handle<TCollection_HAsciiString>>();
  int                        i, nb = MaxIdent();
  for (i = 1; i <= nb; i++)
  {
    occ::handle<TCollection_HAsciiString> lab  = ItemLabel(i);
    occ::handle<Standard_Transient>       item = Item(i);
    if (lab.IsNull())
      continue;
    if (label[0] != '\0' && lab->Search(label) <= 0)
      continue;

    occ::handle<TCollection_HAsciiString> nom = Name(Item(i));
    if (nom.IsNull())
    {
      nom = new TCollection_HAsciiString(i);
      nom->Insert(1, '#');
    }
    else
      nom = new TCollection_HAsciiString(nom);
    list->Append(new TCollection_HAsciiString(lab));
  }
  return list;
}

//=================================================================================================

int IFSelect_WorkSession::NextIdentForLabel(const char* label,
                                                         const int id,
                                                         const int mode) const
{
  int nb = MaxIdent();
  for (int i = id + 1; i <= nb; i++)
  {
    occ::handle<TCollection_HAsciiString> lab = ItemLabel(i);
    if (lab.IsNull())
      continue;
    switch (mode)
    {
      case 0:
        if (!strcmp(lab->ToCString(), label))
          return i;
        break; // switch
      case 1:
        if (lab->Search(label) == 1)
          return i;
        break;
      case 2:
        if (lab->Search(label) > 0)
          return i;
        break;
      default:
        break; // break from switch
    }
  }
  return 0; // here : not found
}

//  #################################################################
//  ....                Parameters (Int and Text)                ....

//=================================================================================================

occ::handle<Standard_Transient> IFSelect_WorkSession::NewParamFromStatic(const char* statname,
                                                                    const char* name)
{
  occ::handle<Standard_Transient> param;
  occ::handle<Interface_Static>   stat = Interface_Static::Static(statname);
  if (stat.IsNull())
    return param;
  if (stat->Type() == Interface_ParamInteger)
  {
    occ::handle<IFSelect_IntParam> intpar = new IFSelect_IntParam;
    intpar->SetStaticName(statname);
    param = intpar;
  }
  else
  {
    param = stat->HStringValue();
  }
  if (param.IsNull())
    return param;
  if (AddNamedItem(name, param) == 0)
    param.Nullify();
  return param;
}

//=================================================================================================

occ::handle<IFSelect_IntParam> IFSelect_WorkSession::IntParam(const int id) const
{
  return occ::down_cast<IFSelect_IntParam>(Item(id));
}

//=================================================================================================

int IFSelect_WorkSession::IntValue(const occ::handle<IFSelect_IntParam>& par) const
{
  if (!par.IsNull())
    return par->Value();
  else
    return 0;
}

//=================================================================================================

occ::handle<IFSelect_IntParam> IFSelect_WorkSession::NewIntParam(const char* name)
{
  occ::handle<IFSelect_IntParam> intpar = new IFSelect_IntParam;
  if (AddNamedItem(name, intpar) == 0)
    intpar.Nullify();
  return intpar;
}

//=================================================================================================

bool IFSelect_WorkSession::SetIntValue(const occ::handle<IFSelect_IntParam>& par,
                                                   const int           val)
{
  if (ItemIdent(par) == 0)
    return false;
  par->SetValue(val);
  return true;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> IFSelect_WorkSession::TextParam(const int id) const
{
  return occ::down_cast<TCollection_HAsciiString>(Item(id));
}

//=================================================================================================

TCollection_AsciiString IFSelect_WorkSession::TextValue(
  const occ::handle<TCollection_HAsciiString>& par) const
{
  if (!par.IsNull())
    return TCollection_AsciiString(par->ToCString());
  else
    return TCollection_AsciiString();
}

occ::handle<TCollection_HAsciiString> IFSelect_WorkSession::NewTextParam(const char* name)
{
  occ::handle<TCollection_HAsciiString> textpar = new TCollection_HAsciiString("");
  if (AddNamedItem(name, textpar) == 0)
    textpar.Nullify();
  return textpar;
}

//=================================================================================================

bool IFSelect_WorkSession::SetTextValue(const occ::handle<TCollection_HAsciiString>& par,
                                                    const char*                  val)
{
  if (ItemIdent(par) == 0)
    return false;
  par->Clear();
  par->AssignCat(val);
  return true;
}

//  ########################################################################
//  ....                           SIGNATURES                           ....

//=================================================================================================

occ::handle<IFSelect_Signature> IFSelect_WorkSession::Signature(const int id) const
{
  return GetCasted(IFSelect_Signature, Item(id));
}

const char* IFSelect_WorkSession::SignValue(const occ::handle<IFSelect_Signature>& sign,
                                                 const occ::handle<Standard_Transient>& ent) const
{
  if (sign.IsNull() || myModel.IsNull())
    return "";
  if (StartingNumber(ent) == 0)
    return "";
  return sign->Value(ent, myModel);
}

//  ########################################################################
//  ....                        SELECTIONS & Cie                        ....

//=================================================================================================

occ::handle<IFSelect_Selection> IFSelect_WorkSession::Selection(const int id) const
{
  return GetCasted(IFSelect_Selection, Item(id));
}

//=================================================================================================

Interface_EntityIterator IFSelect_WorkSession::EvalSelection(
  const occ::handle<IFSelect_Selection>& sel) const
{
  Interface_EntityIterator iter;
  if (errhand)
  {
    errhand = false;
    try
    {
      OCC_CATCH_SIGNALS
      iter = EvalSelection(sel); // normal call (therefore, code not duplicated)
    }
    catch (Standard_Failure const& anException)
    {
      Message_Messenger::StreamBuffer sout = Message::SendInfo();
      sout << "    ****    EvalSelection Interrupted by Exception :   ****\n";
      sout << anException.GetMessageString();
      sout << "\n    Abandon" << std::endl;
    }
    errhand = theerrhand;
    return iter;
  }

  if (thegraph.IsNull())
    return iter;
  iter = sel->UniqueResult(thegraph->Graph());
  return iter;
}

//=================================================================================================

IFSelect_SelectionIterator IFSelect_WorkSession::Sources(
  const occ::handle<IFSelect_Selection>& sel) const
{
  return IFSelect_SelectionIterator(sel);
}

//=================================================================================================

occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> IFSelect_WorkSession::SelectionResult(
  const occ::handle<IFSelect_Selection>& sel) const
{
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> res;
  if (errhand)
  {
    errhand = false;
    try
    {
      OCC_CATCH_SIGNALS
      res = SelectionResult(sel); // appel normal (->code unique)
    }
    catch (Standard_Failure const& anException)
    {
      Message_Messenger::StreamBuffer sout = Message::SendInfo();
      sout << "    ****    SelectionResult Interrupted by Exception :   ****\n";
      sout << anException.GetMessageString();
      sout << "\n    Abandon" << std::endl;
    }
    errhand = theerrhand;
    return res;
  }

  if (!IsLoaded())
  {
    std::cout << " ***  Data for Evaluation not available  ***" << std::endl;
    return new NCollection_HSequence<occ::handle<Standard_Transient>>();
  }
  //  if (ItemIdent(sel) == 0)
  if (sel.IsNull())
  {
    std::cout << " Selection :  Unknown" << std::endl;
    return res;
  } // std::cout<<Handle
  return EvalSelection(sel).Content();
}

//=================================================================================================

occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> IFSelect_WorkSession::SelectionResultFromList(
  const occ::handle<IFSelect_Selection>&           sel,
  const occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>& list) const
{
  if (list.IsNull())
    return SelectionResult(sel);
  DeclareAndCast(IFSelect_SelectDeduct, deduct, sel);
  if (deduct.IsNull())
    return SelectionResult(sel);

  //   We will search for the last deduction in the input chain
  occ::handle<IFSelect_Selection> ssel, newinput;
  ssel = sel;
  int i, nb = MaxIdent();
  for (i = 1; i <= nb * 2; i++)
  {
    newinput = deduct->Input();
    deduct   = GetCasted(IFSelect_SelectDeduct, newinput);
    if (deduct.IsNull())
      break;
    ssel = newinput;
  }

  //  we're there (finally, we should be)
  //  ssel is the last selection examined, deduct its downcast
  //  input its Input (null if sel is not a deduction)
  deduct = GetCasted(IFSelect_SelectDeduct, ssel);

  deduct->Alternate()->SetList(list);

  //   We execute then we clean
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> res = SelectionResult(sel);
  ////  deduct->SetInput (newinput);
  return res;
}

//=================================================================================================

bool IFSelect_WorkSession::SetItemSelection(const occ::handle<Standard_Transient>& item,
                                                        const occ::handle<IFSelect_Selection>& sel)
{
  DeclareAndCast(IFSelect_Dispatch, disp, item);
  DeclareAndCast(IFSelect_GeneralModifier, modif, item);
  if (!disp.IsNull())
  {
    if (ItemIdent(disp) == 0)
      return false;
    //  Selection Nulle : Annuler FinalSelection
    if (!sel.IsNull() && ItemIdent(sel) == 0)
      return false;
    disp->SetFinalSelection(sel);
    return true;
  }
  if (!modif.IsNull())
  {
    if (ItemIdent(modif) == 0)
      return false;
    if (!sel.IsNull() && ItemIdent(sel) == 0)
      return false;
    //   Selection Nulle : Annuler Selection
    modif->SetSelection(sel);
    return true;
  }
  return false;
}

//=================================================================================================

bool IFSelect_WorkSession::ResetItemSelection(const occ::handle<Standard_Transient>& item)
{
  occ::handle<IFSelect_Selection> nulsel;
  return SetItemSelection(item, nulsel);
}

//=================================================================================================

occ::handle<IFSelect_Selection> IFSelect_WorkSession::ItemSelection(
  const occ::handle<Standard_Transient>& item) const
{
  occ::handle<IFSelect_Selection> sel;
  DeclareAndCast(IFSelect_Dispatch, disp, item);
  DeclareAndCast(IFSelect_GeneralModifier, modif, item);
  if (ItemIdent(disp) > 0)
    return disp->FinalSelection();
  if (ItemIdent(modif) > 0)
    return modif->Selection();
  return sel; // Null or unknown -> Null
}

//  ######################################################################
//  ....                        Les COMPTEURS                         ....

//=================================================================================================

occ::handle<IFSelect_SignCounter> IFSelect_WorkSession::SignCounter(const int id) const
{
  return GetCasted(IFSelect_SignCounter, Item(id));
}

//=================================================================================================

bool IFSelect_WorkSession::ComputeCounter(const occ::handle<IFSelect_SignCounter>& counter,
                                                      const bool              forced)
{
  if (counter.IsNull())
    return false;
  if (!ComputeGraph())
    return false;
  return counter->ComputeSelected(Graph(), forced);
}

//=================================================================================================

bool IFSelect_WorkSession::ComputeCounterFromList(
  const occ::handle<IFSelect_SignCounter>&         counter,
  const occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>& list,
  const bool                      clear)
{
  if (counter.IsNull())
    return false;
  if (clear)
    counter->Clear();
  if (list.IsNull())
    return ComputeCounter(counter, true);
  counter->AddList(list, myModel);
  return true;
}

//  ######################################################################
//  ....                        Les DISPATCHES                        ....

//=================================================================================================

occ::handle<NCollection_HSequence<int>> IFSelect_WorkSession::AppliedDispatches() const
{
  occ::handle<NCollection_HSequence<int>> list = new NCollection_HSequence<int>();
  int                   nb   = theshareout->NbDispatches();
  for (int i = 1; i <= nb; i++)
  {
    list->Append(ItemIdent(theshareout->Dispatch(i)));
  }
  return list;
}

//=================================================================================================

void IFSelect_WorkSession::ClearShareOut(const bool onlydisp)
{
  theshareout->Clear(onlydisp);
}

//=================================================================================================

occ::handle<IFSelect_Dispatch> IFSelect_WorkSession::Dispatch(const int id) const
{
  return GetCasted(IFSelect_Dispatch, Item(id));
}

//=================================================================================================

int IFSelect_WorkSession::DispatchRank(const occ::handle<IFSelect_Dispatch>& disp) const
{
  if (ItemIdent(disp) == 0)
    return 0;
  return theshareout->DispatchRank(disp);
}

//  ######################################################################
//  ....                        Les MODIFIERS                         ....

//=================================================================================================

void IFSelect_WorkSession::SetModelCopier(const occ::handle<IFSelect_ModelCopier>& copier)
{
  thecopier = copier;
  thecopier->SetShareOut(theshareout);
}

//=================================================================================================

int IFSelect_WorkSession::NbFinalModifiers(const bool formodel) const
{
  return theshareout->NbModifiers(formodel);
}

//=================================================================================================

occ::handle<NCollection_HSequence<int>> IFSelect_WorkSession::FinalModifierIdents(
  const bool formodel) const
{
  //  return ItemIdents(STANDARD_TYPE(IFSelect_Modifier));
  //  We give the list in ModelCopier order, which is authoritative
  occ::handle<NCollection_HSequence<int>> list = new NCollection_HSequence<int>();
  int                   nbm  = theshareout->NbModifiers(formodel);
  for (int i = 1; i <= nbm; i++)
    list->Append(ItemIdent(theshareout->GeneralModifier(formodel, i)));
  return list;
}

//=================================================================================================

occ::handle<IFSelect_GeneralModifier> IFSelect_WorkSession::GeneralModifier(
  const int id) const
{
  return GetCasted(IFSelect_GeneralModifier, Item(id));
}

//=================================================================================================

occ::handle<IFSelect_Modifier> IFSelect_WorkSession::ModelModifier(const int id) const
{
  return GetCasted(IFSelect_Modifier, Item(id));
}

//=================================================================================================

int IFSelect_WorkSession::ModifierRank(
  const occ::handle<IFSelect_GeneralModifier>& modif) const
{
  if (ItemIdent(modif) == 0)
    return 0;
  return theshareout->ModifierRank(modif);
}

//=================================================================================================

bool IFSelect_WorkSession::ChangeModifierRank(const bool formodel,
                                                          const int before,
                                                          const int after)
{
  return theshareout->ChangeModifierRank(formodel, before, after);
}

//=================================================================================================

void IFSelect_WorkSession::ClearFinalModifiers()
{
  occ::handle<NCollection_HSequence<int>> list = FinalModifierIdents(true);
  int                   nb   = list->Length();
  int                   i; // svv #1
  for (i = 1; i <= nb; i++)
    RemoveItem(GeneralModifier(list->Value(i)));
  list = FinalModifierIdents(false);
  nb   = list->Length();
  for (i = 1; i <= nb; i++)
    RemoveItem(GeneralModifier(list->Value(i)));
}

//=================================================================================================

bool IFSelect_WorkSession::SetAppliedModifier(
  const occ::handle<IFSelect_GeneralModifier>& modif,
  const occ::handle<Standard_Transient>&       item)
{
  if (ItemIdent(modif) == 0)
    return false;

  if (item.IsNull())
    return false;
  if (item == theshareout)
  {
    theshareout->AddModifier(modif, 0);
    return true;
  }
  if (item->IsKind(STANDARD_TYPE(IFSelect_Dispatch)))
  {
    DeclareAndCast(IFSelect_Dispatch, disp, item);
    theshareout->AddModifier(modif, 0);
    modif->SetDispatch(disp);
    return true;
  }
  if (item->IsKind(STANDARD_TYPE(IFSelect_TransformStandard)))
  {
    DeclareAndCast(IFSelect_TransformStandard, stf, item);
    DeclareAndCast(IFSelect_Modifier, tmod, modif);
    if (tmod.IsNull())
      return false;
    stf->AddModifier(tmod);
    theshareout->RemoveItem(modif);
    return true;
  }
  return false;
}

//=================================================================================================

bool IFSelect_WorkSession::ResetAppliedModifier(
  const occ::handle<IFSelect_GeneralModifier>& modif)
{
  if (ItemIdent(modif) == 0)
    return false;

  return theshareout->RemoveItem(modif);
}

//=================================================================================================

occ::handle<Standard_Transient> IFSelect_WorkSession::UsesAppliedModifier(
  const occ::handle<IFSelect_GeneralModifier>& modif) const
{
  occ::handle<Standard_Transient> res;
  if (ItemIdent(modif) == 0)
    return res;
  if (theshareout->ModifierRank(modif) == 0)
    return res;
  res = modif->Dispatch();
  if (res.IsNull())
    res = theshareout;
  return res;
}

//  #################################################################
//  ....                       Transformer                       ....

//=================================================================================================

occ::handle<IFSelect_Transformer> IFSelect_WorkSession::Transformer(const int id) const
{
  return GetCasted(IFSelect_Transformer, Item(id));
}

//=================================================================================================

int IFSelect_WorkSession::RunTransformer(const occ::handle<IFSelect_Transformer>& transf)
{
  int effect = 0;
  if (transf.IsNull() || !IsLoaded())
    return effect;
  occ::handle<Interface_InterfaceModel> newmod; // Null au depart
  Interface_CheckIterator          checks;
  checks.SetName("X-STEP WorkSession : RunTransformer");
  bool res = transf->Perform(thegraph->Graph(), theprotocol, checks, newmod);

  if (!checks.IsEmpty(false))
  {
    Message_Messenger::StreamBuffer sout = Message::SendInfo();
    sout << "  **    RunTransformer has produced Check Messages :    **" << std::endl;
    checks.Print(sout, myModel, false);
  }
  thecheckdone = false;
  thecheckrun  = checks;

  if (newmod.IsNull())
    return (res ? 1 : -1);
  //  UPDATE of SelectPointed
  occ::handle<NCollection_HSequence<int>> list = ItemIdents(STANDARD_TYPE(IFSelect_SelectPointed));
  int                   nb   = list->Length();
  for (int i = 1; i <= nb; i++)
  {
    DeclareAndCast(IFSelect_SelectPointed, sp, Item(list->Value(i)));
    sp->Update(transf);
  }
  if (newmod == myModel)
  {
    effect = (res ? 2 : -2);
    if (!res)
      return effect;
    occ::handle<Interface_Protocol> newproto = theprotocol;
    if (transf->ChangeProtocol(newproto))
    {
      effect      = 4;
      theprotocol = newproto;
      thegtool->SetProtocol(newproto);
    }
    return (ComputeGraph(true) ? 4 : -4);
  }
  else
  {
    effect = (res ? 3 : -3);
    if (!res)
      return effect;
    occ::handle<Interface_Protocol> newproto = theprotocol;
    if (transf->ChangeProtocol(newproto))
    {
      effect      = 5;
      theprotocol = newproto;
      thegtool->SetProtocol(newproto);
    }
    theoldel = myModel;
    SetModel(newmod, false);
  }
  return effect;
}

//=================================================================================================

int IFSelect_WorkSession::RunModifier(const occ::handle<IFSelect_Modifier>& modif,
                                                   const bool           copy)
{
  occ::handle<IFSelect_Selection> sel; // null
  return RunModifierSelected(modif, sel, copy);
}

//=================================================================================================

int IFSelect_WorkSession::RunModifierSelected(const occ::handle<IFSelect_Modifier>&  modif,
                                                           const occ::handle<IFSelect_Selection>& sel,
                                                           const bool            copy)
{
  if (ItemIdent(modif) == 0)
    return false;
  occ::handle<IFSelect_TransformStandard> stf = new IFSelect_TransformStandard;
  stf->SetCopyOption(copy);
  stf->SetSelection(sel);
  stf->AddModifier(modif);
  return RunTransformer(stf);
}

//=================================================================================================

occ::handle<IFSelect_Transformer> IFSelect_WorkSession::NewTransformStandard(const bool copy,
                                                                        const char* name)
{
  occ::handle<IFSelect_TransformStandard> stf = new IFSelect_TransformStandard;
  stf->SetCopyOption(copy);
  if (AddNamedItem(name, stf) == 0)
    stf.Nullify();
  return stf;
}

//    This is a direct action: could be done by a Transformer ...
//=================================================================================================

bool IFSelect_WorkSession::SetModelContent(const occ::handle<IFSelect_Selection>& sel,
                                                       const bool            keep)
{
  if (sel.IsNull() || !IsLoaded())
    return false;
  Interface_EntityIterator list = sel->UniqueResult(thegraph->Graph());
  if (list.NbEntities() == 0)
    return false;

  occ::handle<Interface_InterfaceModel> newmod = myModel->NewEmptyModel();
  Interface_CopyTool               TC(myModel, theprotocol);
  int                 i, nb = myModel->NbEntities();
  if (keep)
  {
    for (list.Start(); list.More(); list.Next())
      TC.TransferEntity(list.Value());
  }
  else
  {
    int* flags = new int[nb + 1];
    for (i = 0; i <= nb; i++)
      flags[i] = 0;
    for (list.Start(); list.More(); list.Next())
    {
      int num = myModel->Number(list.Value());
      if (num <= nb)
        flags[num] = 1;
    }
    for (i = 1; i <= nb; i++)
    {
      if (flags[i] == 0)
        TC.TransferEntity(myModel->Value(i));
    }
    delete[] flags;
  }
  TC.FillModel(newmod);
  if (newmod->NbEntities() == 0)
    return false;
  //    Update (don't forget SelectPointed)
  theoldel = myModel;
  SetModel(newmod, false);
  //  UPDATE of SelectPointed
  occ::handle<NCollection_HSequence<int>> pts = ItemIdents(STANDARD_TYPE(IFSelect_SelectPointed));
  nb                                     = pts->Length();
  for (i = 1; i <= nb; i++)
  {
    DeclareAndCast(IFSelect_SelectPointed, sp, Item(pts->Value(i)));
    sp->Update(TC.Control());
  }
  return true;
}

//  #################################################################
//  ....                        File Name                        ....

//=================================================================================================

occ::handle<TCollection_HAsciiString> IFSelect_WorkSession::FilePrefix() const
{
  return theshareout->Prefix();
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> IFSelect_WorkSession::DefaultFileRoot() const
{
  return theshareout->DefaultRootName();
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> IFSelect_WorkSession::FileExtension() const
{
  return theshareout->Extension();
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> IFSelect_WorkSession::FileRoot(
  const occ::handle<IFSelect_Dispatch>& disp) const
{
  return theshareout->RootName(theshareout->DispatchRank(disp));
}

//=================================================================================================

void IFSelect_WorkSession::SetFilePrefix(const char* name)
{
  theshareout->SetPrefix(new TCollection_HAsciiString(name));
}

//=================================================================================================

void IFSelect_WorkSession::SetFileExtension(const char* name)
{
  theshareout->SetExtension(new TCollection_HAsciiString(name));
}

//=================================================================================================

bool IFSelect_WorkSession::SetDefaultFileRoot(const char* name)
{
  occ::handle<TCollection_HAsciiString> defrt;
  if (name[0] != '\0')
    defrt = new TCollection_HAsciiString(name);
  return theshareout->SetDefaultRootName(defrt);
}

//=================================================================================================

bool IFSelect_WorkSession::SetFileRoot(const occ::handle<IFSelect_Dispatch>& disp,
                                                   const char*           namefile)
{
  int id = ItemIdent(disp);
  if (id == 0)
    return false;
  int nd = theshareout->DispatchRank(disp);
  /*  if (theonlynamed) {
      if      (nd == 0 && namefile[0] != 0)
        theshareout->AddDispatch(disp);
      else if (nd != 0 && namefile[0] == 0)
        theshareout->RemoveDispatch (nd);
    }  */
  if (nd == 0)
    return false;
  //  The order below prevented to change the root name on a given dispatch !
  //  if (theshareout->HasRootName(nd)) return false;
  occ::handle<TCollection_HAsciiString> filename;
  if (namefile[0] != '\0')
    filename = new TCollection_HAsciiString(namefile);
  return theshareout->SetRootName(nd, filename);
}

//=================================================================================================

const char* IFSelect_WorkSession::GiveFileRoot(const char* file) const
{
  OSD_Path path(file);
  if (!path.IsValid(TCollection_AsciiString(file)))
    return file; // tant pis ..
  bufstr = path.Name();
  return bufstr.ToCString();
}

//=================================================================================================

const char* IFSelect_WorkSession::GiveFileComplete(const char* file) const
{
  //  add if needed: Prefix; Extension
  bufstr.Clear();
  bufstr.AssignCat(file);
  int                 i, j = 0, nb = bufstr.Length();
  occ::handle<TCollection_HAsciiString> ext = FileExtension();
  if (!ext.IsNull())
  {
    char val0 = '\0';
    if (ext->Length() > 0)
      val0 = ext->Value(1);
    for (i = nb; i > 0; i--)
      if (bufstr.Value(i) == val0)
      {
        j = 1;
        break;
      }
    if (j == 0)
      bufstr.AssignCat(ext->ToCString());
  }
  occ::handle<TCollection_HAsciiString> pre = FilePrefix();
  if (!pre.IsNull())
  {
    char val1 = '\0';
    if (pre->Length() > 0)
      val1 = pre->Value(pre->Length());
    j = 0;
    for (i = nb; i > 0; i--)
      if (bufstr.Value(i) == val1)
      {
        j = 1;
        break;
      }
    if (j == 0)
      bufstr.Insert(1, pre->ToCString());
  }

  return bufstr.ToCString();
}

//=================================================================================================

void IFSelect_WorkSession::ClearFile()
{
  thecopier->ClearResult();
  theshareout->ClearResult(true);
}

//=================================================================================================

void IFSelect_WorkSession::EvaluateFile()
{
  ////...
  if (!IsLoaded())
    return;
  Interface_CheckIterator checks;
  if (errhand)
  {
    errhand = false;
    try
    {
      OCC_CATCH_SIGNALS
      EvaluateFile(); // normal call (therefore, code not duplicated)
    }
    catch (Standard_Failure const& anException)
    {
      Message_Messenger::StreamBuffer sout = Message::SendInfo();
      sout << "    ****    EvaluateFile Interrupted by Exception :   ****\n";
      sout << anException.GetMessageString();
      sout << "\n    Abandon" << std::endl;
      checks.CCheck(0)->AddFail("Exception Raised -> Abandon");
    }
    errhand     = theerrhand;
    thecheckrun = checks;
    return;
  }

  IFSelect_ShareOutResult R(theshareout, thegraph->Graph());
  checks = thecopier->Copy(R, thelibrary, theprotocol);
  if (!checks.IsEmpty(false))
  {
    Message_Messenger::StreamBuffer sout = Message::SendInfo();
    sout << "  **    EvaluateFile has produced Check Messages :    **" << std::endl;
    checks.Print(sout, myModel, false);
  }
  thecopier->SetRemaining(thegraph->CGraph());
  thecheckrun = checks;
}

//=================================================================================================

int IFSelect_WorkSession::NbFiles() const
{
  return thecopier->NbFiles();
}

//=================================================================================================

occ::handle<Interface_InterfaceModel> IFSelect_WorkSession::FileModel(const int num) const
{
  occ::handle<Interface_InterfaceModel> mod;
  if (num > 0 && num <= NbFiles())
    mod = thecopier->FileModel(num);
  return mod;
}

//=================================================================================================

TCollection_AsciiString IFSelect_WorkSession::FileName(const int num) const
{
  TCollection_AsciiString name;
  if (num > 0 && num <= NbFiles())
    name = thecopier->FileName(num);
  return name;
}

//=================================================================================================

void IFSelect_WorkSession::BeginSentFiles(const bool record)
{
  thecopier->BeginSentFiles(theshareout, record);
}

//=================================================================================================

occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> IFSelect_WorkSession::SentFiles() const
{
  return thecopier->SentFiles();
}

//  #########################################################################
//  ....    Transfer Action properly speaking: the big deal!    ....

//=================================================================================================

bool IFSelect_WorkSession::SendSplit()
{
  ////...
  Interface_CheckIterator checks;

  if (errhand)
  {
    errhand = false;
    try
    {
      OCC_CATCH_SIGNALS
      return SendSplit(); // normal call (therefore, code not duplicated)
    }
    catch (Standard_Failure const& anException)
    {
      Message_Messenger::StreamBuffer sout = Message::SendInfo();
      sout << "    ****    SendSplit Interrupted by Exception :   ****\n";
      sout << anException.GetMessageString();
      sout << "\n    Abandon" << std::endl;
      checks.CCheck(0)->AddFail("Exception Raised -> Abandon");
    }
    errhand     = theerrhand;
    thecheckrun = checks;
    return false;
  }

  if (thelibrary.IsNull())
  {
    checks.CCheck(0)->AddFail("WorkLibrary undefined");
    thecheckrun = checks;
    return false;
  }
  if (!IsLoaded())
  {
    Message_Messenger::StreamBuffer sout = Message::SendInfo();
    sout << " ***  Data for SendSplit not available  ***" << std::endl;
    checks.CCheck(0)->AddFail("Data not available");
    thecheckrun = checks;
    return false;
  }

  if (NbFiles() > 0)
    checks = thecopier->SendCopied(thelibrary, theprotocol);
  else
  {
    /*
    IFSelect_ShareOutResult eval (ShareOut(), thegraph->Graph());
    checks = thecopier->Send (eval, thelibrary, theprotocol);
    thecopier->SetRemaining (thegraph->CGraph());
    */
    //  Decomposer
    if (theshareout.IsNull())
      return false;
    int                i, nbd = theshareout->NbDispatches();
    int                nf   = 0;
    Message_Messenger::StreamBuffer sout = Message::SendInfo();
    sout << " SendSplit .. ";
    for (i = 1; i <= nbd; i++)
    {
      occ::handle<IFSelect_Dispatch> disp = theshareout->Dispatch(i);
      if (disp.IsNull())
        continue;
      IFGraph_SubPartsIterator packs(thegraph->Graph(), false);
      disp->Packets(thegraph->Graph(), packs);
      for (packs.Start(); packs.More(); packs.Next())
      {
        Interface_EntityIterator iter = packs.Entities();
        if (iter.NbEntities() == 0)
          continue;
        //  Write a list of entities
        occ::handle<IFSelect_SelectPointed> sp = new IFSelect_SelectPointed;
        sp->SetList(iter.Content());
        nf++;
        TCollection_AsciiString filnam(nf);
        filnam.Insert(1, "_");
        occ::handle<TCollection_HAsciiString> filepart;
        filepart = FileRoot(disp);
        if (!filepart.IsNull())
          filnam.Insert(1, filepart->ToCString());
        filepart = FilePrefix();
        if (!filepart.IsNull())
          filnam.Insert(1, filepart->ToCString());
        filepart = FileExtension();
        if (!filepart.IsNull())
          filnam.AssignCat(filepart->ToCString());
        IFSelect_ReturnStatus stat = SendSelected(filnam.ToCString(), sp);
        if (stat != IFSelect_RetDone)
          std::cout << "File " << filnam << " failed" << std::endl;
      }
    }
    sout << " .. Files Written : " << nf << std::endl;
  }
  thecheckrun = checks;
  return true;
}

//=================================================================================================

occ::handle<IFSelect_PacketList> IFSelect_WorkSession::EvalSplit() const
{
  occ::handle<IFSelect_PacketList> pks;
  if (!IsLoaded())
    return pks;
  IFSelect_ShareOutResult sho(ShareOut(), thegraph->Graph());
  return sho.Packets();
}

//=================================================================================================

Interface_EntityIterator IFSelect_WorkSession::SentList(const int newcount) const
{
  Interface_EntityIterator iter;
  if (!IsLoaded())
    return iter;
  const Interface_Graph& G  = thegraph->Graph();
  int       nb = G.Size();
  int       i;
  for (i = 1; i <= nb; i++)
  {
    int stat = G.Status(i);
    if ((stat > 0 && newcount < 0) || stat == newcount)
      iter.GetOneItem(G.Entity(i));
  }
  return iter;
}

//=================================================================================================

int IFSelect_WorkSession::MaxSendingCount() const
{
  int newcount = 0;
  if (!IsLoaded())
    return newcount;
  const Interface_Graph& G  = thegraph->Graph();
  int       nb = G.Size();
  int       i;
  for (i = 1; i <= nb; i++)
  {
    int stat = G.Status(i);
    if (stat > newcount)
      newcount = stat;
  }
  return newcount;
}

//=================================================================================================

bool IFSelect_WorkSession::SetRemaining(const IFSelect_RemainMode mode)
{
  Message_Messenger::StreamBuffer sout = Message::SendInfo();
  if (!IsLoaded())
    return false;
  if (mode == IFSelect_RemainForget)
  {
    int nb = thegraph->Graph().Size();
    for (int i = 1; i <= nb; i++)
      thegraph->CGraph().SetStatus(i, 0);
    theoldel.Nullify();
    return true;
  }
  else if (mode == IFSelect_RemainCompute)
  {
    occ::handle<Interface_InterfaceModel> newmod;
    Interface_CopyTool               TC(myModel, theprotocol);
    thecopier->CopiedRemaining(thegraph->Graph(), thelibrary, TC, newmod);
    if (newmod.IsNull())
    {
      sout << " No Remaining Data recorded" << std::endl;
      return false;
    }
    else if (newmod == myModel)
    {
      sout << " Remaining causes all original data to be kept" << std::endl;
      thecopier->SetRemaining(thegraph->CGraph());
      return false;
    }
    else
    {
      theoldel = myModel;
      SetModel(newmod, false);
      //  UPDATE of SelectPointed
      occ::handle<NCollection_HSequence<int>> list = ItemIdents(STANDARD_TYPE(IFSelect_SelectPointed));
      int                   nb   = list->Length();
      for (int i = 1; i <= nb; i++)
      {
        DeclareAndCast(IFSelect_SelectPointed, sp, Item(list->Value(i)));
        sp->Update(TC.Control());
      }
      return true;
    }
  }
  else if (mode == IFSelect_RemainDisplay)
  {
    int ne = 0;
    int nb = thegraph->Graph().Size();
    for (int i = 1; i <= nb; i++)
    {
      if (thegraph->Graph().Status(i) >= 0)
        ne++;
    }
    if (ne == 0)
    {
      sout << " - All entities are remaining, none yet sent" << std::endl;
      return true;
    }
    Interface_EntityIterator iter = SentList(0);
    nb                            = iter.NbEntities();
    if (nb == 0)
    {
      sout << " - No recorded remaining entities" << std::endl;
      return true;
    }
    sout << " --  Recorded Remaining (not yet sent) Entities  --" << std::endl;
    ListEntities(iter, 2, sout);
    sout << " -- Maximum Sending Count (i.e. duplication in files) " << MaxSendingCount()
         << std::endl;

    /*
        sout<< " - Now, dispatches are deactivated"<<std::endl;
        nb = theshareout->NbDispatches();
        for (int i = nb; i > theshareout->LastRun(); i --)
          theshareout->RemoveDispatch(i);
    */
    return true;
  }
  else if (mode == IFSelect_RemainUndo)
  {
    if (theoldel.IsNull())
      return false;
    SetModel(theoldel);
    theoldel.Nullify();
    return true;
  }
  else
    return false;
}

//=================================================================================================

IFSelect_ReturnStatus IFSelect_WorkSession::SendAll(const char* filename,
                                                    const bool computegraph)
{
  ////...
  Interface_CheckIterator checks;
  if (!IsLoaded())
    return IFSelect_RetVoid;
  if (thelibrary.IsNull())
  {
    checks.CCheck(0)->AddFail("WorkLibrary undefined");
    thecheckrun = checks;
    return IFSelect_RetError;
  }

  if (errhand)
  {
    errhand = false;
    try
    {
      OCC_CATCH_SIGNALS
      ComputeGraph(computegraph);
      checks = thecopier->SendAll(filename, thegraph->Graph(), thelibrary, theprotocol);
    }
    catch (Standard_Failure const& anException)
    {
      Message_Messenger::StreamBuffer sout = Message::SendInfo();
      sout << "    ****    SendAll Interrupted by Exception :   ****\n";
      sout << anException.GetMessageString();
      sout << "\n    Abandon" << std::endl;
      errhand = theerrhand;
      checks.CCheck(0)->AddFail("Exception Raised -> Abandon");
      thecheckrun = checks;
      return IFSelect_RetFail;
    }
  }
  else
    checks = thecopier->SendAll(filename, thegraph->Graph(), thelibrary, theprotocol);
  occ::handle<Interface_Check> aMainFail = checks.CCheck(0);
  if (!aMainFail.IsNull() && aMainFail->HasFailed())
  {
    return IFSelect_RetStop;
  }
  if (theloaded.Length() == 0)
    theloaded.AssignCat(filename);
  thecheckrun = checks;
  if (checks.IsEmpty(true))
    return IFSelect_RetDone;
  return IFSelect_RetError;
}

//=================================================================================================

IFSelect_ReturnStatus IFSelect_WorkSession::SendSelected(const char*            filename,
                                                         const occ::handle<IFSelect_Selection>& sel,
                                                         const bool computegraph)
{
  ////...
  if (!IsLoaded())
    return IFSelect_RetVoid;
  Interface_CheckIterator checks;
  if (thelibrary.IsNull())
  {
    checks.CCheck(0)->AddFail("WorkLibrary undefined");
    thecheckrun = checks;
    return IFSelect_RetVoid;
  }

  if (errhand)
  {
    errhand = false;
    try
    {
      OCC_CATCH_SIGNALS
      ComputeGraph(computegraph);
      return SendSelected(filename, sel); // normal call
    }
    catch (Standard_Failure const& anException)
    {
      Message_Messenger::StreamBuffer sout = Message::SendInfo();
      sout << "    ****    SendSelected Interrupted by Exception :   ****\n";
      sout << anException.GetMessageString();
      sout << "\n    Abandon" << std::endl;
      checks.CCheck(0)->AddFail("Exception Raised -> Abandon");
      errhand     = theerrhand;
      thecheckrun = checks;
      return IFSelect_RetFail;
    }
  }
  //  if (ItemIdent(sel) == 0) return 3;
  Interface_EntityIterator iter = sel->UniqueResult(thegraph->Graph());
  if (iter.NbEntities() == 0)
    return IFSelect_RetVoid;

  checks = thecopier->SendSelected(filename, thegraph->Graph(), thelibrary, theprotocol, iter);
  thecopier->SetRemaining(thegraph->CGraph());
  thecheckrun = checks;
  if (checks.IsEmpty(true))
    return IFSelect_RetDone;
  return IFSelect_RetError;
}

//=================================================================================================

IFSelect_ReturnStatus IFSelect_WorkSession::WriteFile(const char* filename)
{
  if (WorkLibrary().IsNull())
    return IFSelect_RetVoid;
  ComputeGraph(true);
  if (!IsLoaded())
    return IFSelect_RetVoid;
  return SendAll(filename);
}

//=================================================================================================

IFSelect_ReturnStatus IFSelect_WorkSession::WriteFile(const char*            filename,
                                                      const occ::handle<IFSelect_Selection>& sel)
{
  if (WorkLibrary().IsNull() || sel.IsNull())
    return IFSelect_RetVoid;
  ComputeGraph(true);
  if (!IsLoaded())
    return IFSelect_RetVoid;
  return SendSelected(filename, sel);
}

//  ################################################################
//  ....        Specific Actions on Selections        ....

//=================================================================================================

int IFSelect_WorkSession::NbSources(const occ::handle<IFSelect_Selection>& sel) const
{
  if (ItemIdent(sel) == 0)
    return 0;
  if (sel->IsKind(STANDARD_TYPE(IFSelect_SelectExtract))
      || sel->IsKind(STANDARD_TYPE(IFSelect_SelectDeduct)))
    return 1;
  if (sel->IsKind(STANDARD_TYPE(IFSelect_SelectControl)))
    return 2;
  if (sel->IsKind(STANDARD_TYPE(IFSelect_SelectCombine)))
    return GetCasted(IFSelect_SelectCombine, sel)->NbInputs();
  return 0;
}

//=================================================================================================

occ::handle<IFSelect_Selection> IFSelect_WorkSession::Source(const occ::handle<IFSelect_Selection>& sel,
                                                        const int            num) const
{
  occ::handle<IFSelect_Selection> sr;
  if (ItemIdent(sel) == 0)
    return sr;
  if (sel->IsKind(STANDARD_TYPE(IFSelect_SelectExtract)))
    sr = GetCasted(IFSelect_SelectExtract, sel)->Input();
  else if (sel->IsKind(STANDARD_TYPE(IFSelect_SelectDeduct)))
    sr = GetCasted(IFSelect_SelectDeduct, sel)->Input();
  else if (sel->IsKind(STANDARD_TYPE(IFSelect_SelectControl)))
  {
    if (num == 1)
      sr = GetCasted(IFSelect_SelectControl, sel)->MainInput();
    else if (num == 2)
      sr = GetCasted(IFSelect_SelectControl, sel)->SecondInput();
  }
  else if (sel->IsKind(STANDARD_TYPE(IFSelect_SelectCombine)))
    sr = GetCasted(IFSelect_SelectCombine, sel)->Input(num);
  return sr;
}

//=================================================================================================

bool IFSelect_WorkSession::IsReversedSelectExtract(
  const occ::handle<IFSelect_Selection>& sel) const
{
  if (ItemIdent(sel) == 0)
    return false;
  DeclareAndCast(IFSelect_SelectExtract, sxt, sel);
  if (sxt.IsNull())
    return false;
  return (!sxt->IsDirect());
}

//=================================================================================================

bool IFSelect_WorkSession::ToggleSelectExtract(const occ::handle<IFSelect_Selection>& sel)
{
  if (ItemIdent(sel) == 0)
    return false;
  DeclareAndCast(IFSelect_SelectExtract, sxt, sel);
  if (sxt.IsNull())
    return false;
  sxt->SetDirect(!sxt->IsDirect());
  return true;
}

//=================================================================================================

bool IFSelect_WorkSession::SetInputSelection(const occ::handle<IFSelect_Selection>& sel,
                                                         const occ::handle<IFSelect_Selection>& inp)
{
  if (ItemIdent(sel) == 0)
    return false;
  if (!inp.IsNull() && ItemIdent(inp) == 0)
    return false;
  DeclareAndCast(IFSelect_SelectExtract, sxt, sel);
  if (!sxt.IsNull())
  {
    sxt->SetInput(inp);
    return true;
  }
  DeclareAndCast(IFSelect_SelectDeduct, sdt, sel);
  if (!sdt.IsNull())
  {
    sdt->SetInput(inp);
    return true;
  }
  return false;
}

//=================================================================================================

bool IFSelect_WorkSession::SetControl(const occ::handle<IFSelect_Selection>& sel,
                                                  const occ::handle<IFSelect_Selection>& sc,
                                                  const bool            formain)
{
  DeclareAndCast(IFSelect_SelectControl, dsel, sel);
  if (ItemIdent(dsel) == 0)
    return false;
  if (ItemIdent(sc) == 0)
    return false;
  if (formain)
    dsel->SetMainInput(sc);
  else
    dsel->SetSecondInput(sc);
  return true;
}

//=================================================================================================

int IFSelect_WorkSession::CombineAdd(const occ::handle<IFSelect_Selection>& sel,
                                                  const occ::handle<IFSelect_Selection>& seladd,
                                                  const int            atnum)
{
  DeclareAndCast(IFSelect_SelectCombine, csel, sel);
  if (ItemIdent(csel) == 0)
    return 0;
  if (ItemIdent(seladd) == 0)
    return 0;
  csel->Add(seladd, atnum);
  return csel->NbInputs();
}

//=================================================================================================

bool IFSelect_WorkSession::CombineRemove(const occ::handle<IFSelect_Selection>& selcomb,
                                                     const occ::handle<IFSelect_Selection>& selrem)
{
  DeclareAndCast(IFSelect_SelectCombine, csel, selcomb);
  if (ItemIdent(csel) == 0)
    return false;
  if (ItemIdent(selrem) == 0)
    return false;
  int nb = csel->NbInputs();
  for (int i = nb; i > 0; i--)
  {
    if (csel->Input(i) == selrem)
    {
      csel->Remove(i);
      return true;
    }
  }
  return true;
}

//=================================================================================================

occ::handle<IFSelect_Selection> IFSelect_WorkSession::NewSelectPointed(
  const occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>& list,
  const char*                      name)
{
  occ::handle<IFSelect_SelectPointed> sel = new IFSelect_SelectPointed;
  if (!list.IsNull())
    sel->AddList(list);
  if (AddNamedItem(name, sel) == 0)
    sel.Nullify();
  return sel;
}

//=================================================================================================

bool IFSelect_WorkSession::SetSelectPointed(
  const occ::handle<IFSelect_Selection>&           sel,
  const occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>& list,
  const int                      mode) const
{
  DeclareAndCast(IFSelect_SelectPointed, sp, sel);
  if (sp.IsNull() || list.IsNull())
    return false;
  if (mode == 0)
    sp->Clear();
  if (mode >= 0)
    sp->AddList(list);
  else
    sp->RemoveList(list);
  return true;
}

//  ###########################################################################
//  ....         Analysis of a CheckIterator in relation to a graph        ....

//=================================================================================================

static void IFSelect_QueryProp(Interface_IntList&       list,
                               TCollection_AsciiString& ana,
                               const int   num,
                               const int                quoi)
{
  list.SetNumber(num);
  int i, nb = list.Length();
  for (i = 1; i <= nb; i++)
  {
    if (i > 1)
      list.SetNumber(num); // because recursive call + depth first
    int n = list.Value(i);
    //    is there a need to propagate?
    //  1 W/place  2 F/place  3 Wprop 4Wprop+W/place  5Wprop+F/place
    //  6 Fprop  7 Fprop+W/place  8 Fprop+F/place
    char val = ana.Value(n);
    switch (val)
    {
      case ' ':
        val = (quoi ? '3' : '6');
        break;
      case '1':
        val = (quoi ? '4' : '7');
        break;
      case '2':
        val = (quoi ? '5' : '8');
        break;
      case '3':
        val = (quoi ? ' ' : '6');
        break;
      case '4':
        val = (quoi ? ' ' : '7');
        break;
      case '5':
        val = (quoi ? ' ' : '8');
        break;
      case '6':
        val = ' ';
        break;
      case '7':
        val = ' ';
        break;
      case '8':
        val = ' ';
        break;
      default:
        val = ' ';
        break;
    }
    if (val == ' ')
      continue;
    ana.SetValue(n, val);
    IFSelect_QueryProp(list, ana, n, quoi);
  }
}

//=================================================================================================

void IFSelect_WorkSession::QueryCheckList(const Interface_CheckIterator& chl)
{
  if (!IsLoaded())
    return;
  int i, nb = myModel->NbEntities();
  thecheckana = TCollection_AsciiString(nb + 1, ' ');
  for (chl.Start(); chl.More(); chl.Next())
  {
    int               num = chl.Number();
    const occ::handle<Interface_Check>& ach = chl.Value();
    if (ach->HasFailed())
      thecheckana.SetValue(num, '2');
    else if (ach->HasWarnings())
      thecheckana.SetValue(num, '1');
  }
  //  analysis according to the graph ... codes : blc = nothing
  //  1 W/place  2 F/place  3 Wprop 4Wprop+W/place  5Wprop+F/place
  //  6 Fprop  7 Fprop+W/place  8 Fprop+F/place
  Interface_IntList list; // = thegraph->Graph().SharingNums(0);
                          //   two passes : first Warning, then Fail
  for (i = 1; i <= nb; i++)
  {
    char val  = thecheckana.Value(i);
    int  quoi = -1;
    if (val == '1' || val == '4' || val == '7')
      quoi = 0;
    if (quoi >= 0)
      IFSelect_QueryProp(list, thecheckana, i, quoi);
  }
  for (i = 1; i <= nb; i++)
  {
    char val  = thecheckana.Value(i);
    int  quoi = -1;
    if (val == '2' || val == '5' || val == '8')
      quoi = 1;
    if (quoi >= 0)
      IFSelect_QueryProp(list, thecheckana, i, quoi);
  }
}

//=================================================================================================

int IFSelect_WorkSession::QueryCheckStatus(const occ::handle<Standard_Transient>& ent) const
{
  if (!IsLoaded())
    return -1;
  int num = myModel->Number(ent);
  if (num == 0)
    return -1;
  if (thecheckana.Length() < num)
    return -1;
  char val = thecheckana.Value(num);
  //  codes : blc = rien -> 0
  //  1 W/place -> 1   2 F/place -> 2
  //  3 Wprop -> 10    4 Wprop+W/place -> 11    5 Wprop+F/place -> 12
  //  6 Fprop -> 20    7 Fprop+W/place -> 21    8 Fprop+F/place -> 22
  if (val == ' ')
    return 0;
  if (val == '1')
    return 1;
  if (val == '2')
    return 2;
  if (val == '3')
    return 10;
  if (val == '4')
    return 11;
  if (val == '5')
    return 12;
  if (val == '6')
    return 20;
  if (val == '7')
    return 21;
  if (val == '8')
    return 22;
  return 0;
}

//=================================================================================================

int IFSelect_WorkSession::QueryParent(const occ::handle<Standard_Transient>& entdad,
                                                   const occ::handle<Standard_Transient>& entson) const
{
  int ndad = StartingNumber(entdad);
  int nson = StartingNumber(entson);
  if (ndad < 1 || nson < 1)
    return -1;
  if (ndad == nson)
    return 0;
  //  we will calculate: for each immediate parent, of <son>, status with <dad> + 1
  //  nb: not protected against loops ...
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> list = thegraph->Graph().Sharings(entson).Content();
  if (list.IsNull())
    return -1;
  int i, nb = list->Length();
  for (i = 1; i <= nb; i++)
  {
    if (list->Value(i) == entdad)
      return 1;
    int stat = QueryParent(entdad, list->Value(i));
    if (stat >= 0)
      return stat + 1;
  }
  return -1; // not yet implemented ...
}

//  ###########################################################################
//  ....      Dumps and Evaluations, not easy to pass as arguments      ....

//  ####    ####    ####    ####    ####    ####    ####    ####    ####
//  ....        DumpShare        ....

//=================================================================================================

void IFSelect_WorkSession::SetParams(const NCollection_Vector<occ::handle<Standard_Transient>>& params,
                                     const NCollection_Vector<int>&           uselist)
{
  int             i, nbp = params.Length(), nbu = uselist.Length();
  occ::handle<IFSelect_ParamEditor> editor =
    new IFSelect_ParamEditor(nbp + nbu + 50, "Parameter Editor");
  for (i = params.Lower(); i <= params.Upper(); i++)
  {
    DeclareAndCast(Interface_TypedValue, val, params.Value(i));
    if (val.IsNull())
      continue;
    editor->AddValue(val);
  }
  AddNamedItem("xst-params-edit", editor);
  //  The EditForm
  occ::handle<IFSelect_EditForm> paramsall = editor->Form(false);
  AddNamedItem("xst-params-all", paramsall);

  //  We tackle the partial EditForms
  NCollection_Sequence<int> listgen, listload, listsend, listsplit, listread, listwrite;
  for (i = uselist.Lower(); i <= uselist.Upper(); i++)
  {
    int use = uselist.Value(i);
    switch (use)
    {
      case 1:
        listgen.Append(i);
        break;
      case 2:
        listread.Append(i);
        break;
      case 3:
        listsend.Append(i);
        break;
      case 4:
        listsplit.Append(i);
        break;
      case 5:
        listread.Append(i);
        break;
      case 6:
        listwrite.Append(i);
        break;
      default:
        break;
    }
  }
  occ::handle<IFSelect_EditForm> paramsgen =
    new IFSelect_EditForm(editor, listgen, false, true, "General Parameters");
  if (listgen.Length() > 0)
    AddNamedItem("xst-params-general", paramsgen);
  occ::handle<IFSelect_EditForm> paramsload =
    new IFSelect_EditForm(editor, listload, false, true, "Loading Parameters");
  if (listload.Length() > 0)
    AddNamedItem("xst-params-load", paramsload);
  occ::handle<IFSelect_EditForm> paramssend =
    new IFSelect_EditForm(editor, listsend, false, true, "Sending Parameters");
  if (listsend.Length() > 0)
    AddNamedItem("xst-params-send", paramssend);
  occ::handle<IFSelect_EditForm> paramsplit =
    new IFSelect_EditForm(editor, listsplit, false, true, "Split Parameters");
  if (listsplit.Length() > 0)
    AddNamedItem("xst-params-split", paramsplit);
  occ::handle<IFSelect_EditForm> paramsread = new IFSelect_EditForm(editor,
                                                               listread,
                                                               false,
                                                               true,
                                                               "Read(Transfer) Parameters");
  if (listread.Length() > 0)
    AddNamedItem("xst-params-read", paramsread);
  occ::handle<IFSelect_EditForm> paramswrite = new IFSelect_EditForm(editor,
                                                                listwrite,
                                                                false,
                                                                true,
                                                                "Write(Transfer) Parameters");
  if (listwrite.Length() > 0)
    AddNamedItem("xst-params-write", paramswrite);
}

//=================================================================================================

void IFSelect_WorkSession::TraceStatics(const int use,
                                        const int mode) const
{
  Message_Messenger::StreamBuffer sout = Message::SendInfo();
  if (use > 0)
  {
    if (mode == 0)
      sout << "******************************************" << std::endl;
    if (use == 1)
    {
      if (mode == 0)
        sout << "*****      General  Parameters       *****" << std::endl;
    }
    else if (use == 2)
    {
      if (mode == 0)
        sout << "*****            Load  File          *****" << std::endl;
    }
    else if (use == 3)
    {
      if (mode == 0)
        sout << "*****            Write File          *****" << std::endl;
    }
    else if (use == 4)
    {
      if (mode == 0)
        sout << "*****            Split File          *****" << std::endl;
    }
    else if (use == 5)
    {
      if (mode == 0)
        sout << "*****        Transfer (Read)         *****" << std::endl;
    }
    else if (use == 6)
    {
      if (mode == 0)
        sout << "*****        Transfer (Write)        *****" << std::endl;
    }
    if (mode == 0)
      sout << "******************************************" << std::endl << std::endl;
  }

  //    Echainements particuliers (use > 0)
  if (use == 5)
  {
    TraceStatics(-2, mode);
    if (mode == 0)
      sout << std::endl;
  }
  else if (use == 4 || use == 6)
  {
    TraceStatics(-3, mode);
    if (mode == 0)
      sout << std::endl;
  }

  //    Valeurs particulieres
  if (use == 1 || use == -1)
  { // General : trace
    if (mode == 0)
    {
      //      sout << "Trace Level   :
      //      "<<Message_PrinterOStream::Default()->GetTraceLevel()<<std::endl;
    }
  }
  else if (use == 4 || use == -4)
  { // Split : Prefix & cie
    if (mode == 0)
    {
      occ::handle<TCollection_HAsciiString> str = theshareout->Prefix();
      if (!str.IsNull())
        sout << "Prefix        : " << str->ToCString() << std::endl;
      else
        sout << "Prefix       not Defined" << std::endl;
      str = theshareout->DefaultRootName();
      if (!str.IsNull())
        sout << "Default Root  : " << str->ToCString() << std::endl;
      else
        sout << "Default Root not Defined" << std::endl;
      str = theshareout->Extension();
      if (!str.IsNull())
        sout << "Extension     : " << str->ToCString() << std::endl;
      else
        sout << "Extension    not defined" << std::endl;
    }
  }

  //  LIST THE STATICS
  //  Go through the ParamEditor ...

  //    Fin
  if (use > 0)
  {
    if (mode == 0)
      sout << "******************************************" << std::endl << std::endl;
  }
}

//=================================================================================================

void IFSelect_WorkSession::DumpShare() const
{
  Message_Messenger::StreamBuffer sout = Message::SendInfo();
  sout << "        **********  Definition ShareOut (Complete)  **********" << std::endl;

  occ::handle<TCollection_HAsciiString> str = theshareout->Prefix();
  if (!str.IsNull())
    sout << "Prefix       : " << str->ToCString() << std::endl;
  else
    sout << "Prefix       not Defined" << std::endl;
  str = theshareout->DefaultRootName();
  if (!str.IsNull())
    sout << "Default Root : " << str->ToCString() << std::endl;
  else
    sout << "Default Root not Defined" << std::endl;
  str = theshareout->Extension();
  if (!str.IsNull())
    sout << "Extension    : " << str->ToCString() << std::endl;
  else
    sout << "Extension    not defined" << std::endl;

  int lr = theshareout->LastRun();
  int nb = theshareout->NbDispatches();
  sout << "Nb Dispatches : " << nb << " (Last Run : " << lr << ") : " << std::endl;
  for (int i = 1; i <= nb; i++)
  {
    occ::handle<IFSelect_Dispatch> disp = theshareout->Dispatch(i);
    sout << "Dispatch n0 " << i;
    if (HasName(disp))
      sout << "   Name:" << Name(disp)->ToCString();
    sout << "   Label:" << disp->Label() << std::endl;
    occ::handle<IFSelect_Selection> sel = disp->FinalSelection();
    if (sel.IsNull())
      sout << "   No Final Selection Defined" << std::endl;
    else if (HasName(sel))
      sout << "   Final Selection : Name:" << Name(sel)->ToCString() << "  Label:" << sel->Label()
           << std::endl;
    else
      sout << "   Final Selection : " << sel->Label() << std::endl;
    if (disp->HasRootName())
      sout << "   File Root Name : " << disp->RootName()->ToCString() << std::endl;
    else
      sout << "   No specific file root name (see Default Root)" << std::endl;
  }
  int nbm = theshareout->NbModifiers(true);
  if (nbm > 0)
    sout << "  ***   " << nbm << " active Model Modifiers : see ListModifiers   ***" << std::endl;
  int nbf = theshareout->NbModifiers(false);
  if (nbf > 0)
    sout << "  ***   " << nbf << " active File  Modifiers : see ListModifiers   ***" << std::endl;
  if (nbm + nbf == 0)
    sout << "  ***   No active Modifiers   ***" << std::endl;
}

//  ####    ####    ####    ####    ####    ####    ####    ####    ####
//  ....        ListItems        ....

//=================================================================================================

void IFSelect_WorkSession::ListItems(const char* lab) const
{
  Message_Messenger::StreamBuffer sout = Message::SendInfo();
  sout << "        **********  Items in Session  **********" << std::endl;
  int                 nb = MaxIdent();
  occ::handle<TCollection_HAsciiString> str;
  if (lab[0] != '\0')
    str = new TCollection_HAsciiString(lab);
  for (int i = 1; i <= nb; i++)
  {
    const occ::handle<Standard_Transient>& var   = theitems.FindKey(i);
    occ::handle<TCollection_HAsciiString>  label = ItemLabel(i);
    if (label.IsNull())
      continue; //  -> item supprime
    if (!str.IsNull())
    {
      if (label->Location(str, 1, label->Length()) == 0)
        continue;
    }
    sout << "#" << i;
    if (HasName(var))
      sout << "	- Named : " << Name(var)->ToCString() << "	- ";
    else
      sout << " - (no name) - ";
    sout << var->DynamicType()->Name() << std::endl << "    " << label->ToCString() << std::endl;
  }
}

//  ####    ####    ####    ####    ####    ####    ####    ####    ####
//  ....        ListModifiers

//=================================================================================================

void IFSelect_WorkSession::ListFinalModifiers(const bool formodel) const
{
  Message_Messenger::StreamBuffer sout = Message::SendInfo();
  int                nb   = theshareout->NbModifiers(formodel);
  sout << "        **********  Modifiers in Session ";
  sout << (formodel ? "(For Model)" : "(For File)");
  sout << ": " << nb << "  **********" << std::endl;
  for (int i = 1; i <= nb; i++)
  {
    occ::handle<IFSelect_GeneralModifier> modif = theshareout->GeneralModifier(formodel, i);
    if (!modif.IsNull())
      sout << "Modifier n0." << i << "	: " << modif->Label();
    if (HasName(modif))
      sout << "	 Named as : " << Name(modif)->ToCString();
    sout << std::endl;
  }
}

//  ####    ####    ####    ####    ####    ####    ####    ####    ####
//  ....        DumpSelection        ....

//=================================================================================================

void IFSelect_WorkSession::DumpSelection(const occ::handle<IFSelect_Selection>& sel) const
{
  Message_Messenger::StreamBuffer sout = Message::SendInfo();
  if (ItemIdent(sel) == 0)
  {
    sout << "Selection :  Unknown" << std::endl; // sout<<Handle
    return;
  }
  sout << "        **********  Selection";
  if (HasName(sel))
    sout << " , Name : " << Name(sel)->ToCString();
  sout << "  **********" << std::endl;
  sout << "Label : " << sel->Label() << " . Input(s) : " << std::endl;
  int           nb = 0;
  IFSelect_SelectionIterator iter;
  sel->FillIterator(iter);
  for (; iter.More(); iter.Next())
  {
    nb++;
    const occ::handle<IFSelect_Selection>& newsel = iter.Value();
    sout << " -- " << newsel->Label() << std::endl;
  }
  sout << " Nb Inputs:" << nb << std::endl;
}

//              ##########################################
//              #########    Fonctions complementaires
//              ##########################################

//  ####    ####    ####    ####    ####    ####    ####    ####    ####
//  ....        DumpModel        ....

//=================================================================================================

occ::handle<IFSelect_Selection> IFSelect_WorkSession::GiveSelection(const char* selname) const
{
  char             nomsel[500];
  int np = -1, nf = -1, nivp = 0;
  for (int n = 0; selname[n] != '\0'; n++)
  {
    nomsel[n]     = selname[n];
    nomsel[n + 1] = '\0';
    if (selname[n] == '(')
    {
      np = n;
      nivp++;
    }
    if (selname[n] == ')')
    {
      nivp--;
      if (nivp <= 0)
        nf = n;
    }
    ////    if (selname[n] == ' ') { nb = n; break; }
  }

  occ::handle<IFSelect_Selection> sel;
  if (np >= 0)
  {
    nomsel[np] = 0;
  }
  if (nf >= 0)
  {
    nomsel[nf] = '\0';
  }
  occ::handle<Standard_Transient> item = NamedItem(nomsel);

  //  Parentheses? try Signature (later: parameterized Selection)
  //  NB: we count the levels of parentheses (reflected nesting)
  if (np > 0 && nf > 0)
  {
    occ::handle<IFSelect_SelectSignature> selsign;
    int                 debsign = np + 1;

    DeclareAndCast(IFSelect_Signature, sign, item);
    DeclareAndCast(IFSelect_SignCounter, cnt, item);
    if (!sign.IsNull())
      selsign = new IFSelect_SelectSignature(sign, &nomsel[debsign], false);
    else if (!cnt.IsNull())
      selsign = new IFSelect_SelectSignature(cnt, &nomsel[debsign], false);
    else
    {
      std::cout << selname << " : neither Signature nor Counter" << std::endl;
      return sel;
    }

    selsign->SetInput(new IFSelect_SelectModelEntities); // by default
    sel = selsign;
  }

  else
    sel = GetCasted(IFSelect_Selection, item);

  return sel;
}

//=================================================================================================

occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> IFSelect_WorkSession::GiveList(
  const occ::handle<Standard_Transient>& obj) const
{
  //   Already a list
  DeclareAndCast(NCollection_HSequence<occ::handle<Standard_Transient>>, list, obj);
  if (!list.IsNull())
    return list;

  //   Nothing at all: returns nothing at all
  if (obj.IsNull())
    return list;

  //   A selection: its result (standard)
  DeclareAndCast(IFSelect_Selection, sel, obj);
  if (!sel.IsNull())
  {
    Interface_EntityIterator iter = EvalSelection(sel);
    return iter.Content();
  }

  //   The model: its content
  list = new NCollection_HSequence<occ::handle<Standard_Transient>>();
  if (obj == myModel)
  {
    int i, nb = myModel->NbEntities();
    for (i = 1; i <= nb; i++)
      list->Append(myModel->Value(i));
  }

  //   A model entity: this entity
  else if (StartingNumber(obj) > 0)
    list->Append(obj);

  //   A Text: its interpretation
  else
  {
    DeclareAndCast(TCollection_HAsciiString, str, obj);
    if (!str.IsNull())
      return GiveList(str->ToCString());
  }

  //  If it's not all that: an empty list
  return list;
}

//=================================================================================================

occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> IFSelect_WorkSession::GiveList(
  const char* first,
  const char* second) const
{
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> list;
  if (!first || first[0] == '\0')
    return list;
  if (first[0] == ' ')
    return GiveList(&first[1], second);
  if (second && second[0] == ' ')
    return GiveList(first, &second[1]);

  //   NULL list will be interpreted as SelectionResult (raw selection)
  //   otherwise as SelectionResultFromList
  if (second && second[0] != '\0')
    list = GiveList(second, "");

  list = GiveListFromList(first, list);
  return list;
}

//=================================================================================================

occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> IFSelect_WorkSession::GiveListFromList(
  const char*            selname,
  const occ::handle<Standard_Transient>& ent) const
{
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> list;
  int                     num;

  //   LIST DEFINED BY DEFAULT (in this case, the input list is ignored)
  if (selname[0] == '(')
  {
    //  list of entities given one after another: (ID,ID,ID...)
    char                 entid[50];
    int     i, j = 0;
    NCollection_Map<int> numap;
    list = new NCollection_HSequence<occ::handle<Standard_Transient>>();
    for (i = 1; selname[i] != '\0'; i++)
    {
      if (selname[i] == ' ')
        continue;
      if (selname[i] == ',' || selname[i] == ')')
      {
        entid[j] = '\0';
        if (j == 0)
          continue;
        j   = 0;
        num = NumberFromLabel(entid);
        if (num <= 0 || !numap.Add(num))
          continue;
        occ::handle<Standard_Transient> anent = StartingEntity(num);
        if (!anent.IsNull())
          list->Append(anent);
        if (selname[i] == ')')
          break;
        continue;
      }
      entid[j] = selname[i];
      j++;
    }
    return list;
  }
  num = NumberFromLabel(selname);
  if (num > 0)
    return GiveList(StartingEntity(num));

  //  Other cases: is there an input list.
  //   Si OUI -> SelectionResultFromList.  Si NON -> SelectionResult
  //   If an isolated entity -> we make it a list

  list = GiveList(ent); // ent NULL -> list NULL otherwise interpreted

  //    Decomposition term1 term2 ...

  char nomsel[500];
  nomsel[0]          = '\0';
  int n = 0, nb = -1;
  for (n = 0; selname[n] != '\0'; n++)
  {
    nomsel[n]     = selname[n];
    nomsel[n + 1] = '\0';
    //    if (selname[n] == '(') { np = n; nivp ++; }
    //    if (selname[n] == ')') { nivp --;  if (nivp <= 0) nf = n; }
    if (selname[n] == ' ')
    {
      nb        = n;
      nomsel[n] = '\0';
      break;
    }
  }
  if (nomsel[0] == '\0')
    return list;

  occ::handle<IFSelect_Selection> sel = GiveSelection(nomsel);
  if (sel.IsNull())
  {
    std::cout << "Neither Entity Number/Label nor Selection :" << nomsel << std::endl;
    return list;
  }

  if (nb > 0)
    list = GiveListFromList(&selname[nb + 1], list);

  if (list.IsNull())
    list = SelectionResult(sel);
  else
    list = SelectionResultFromList(sel, list);

  return list;
}

//=================================================================================================

occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> IFSelect_WorkSession::GiveListCombined(
  const occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>& l1,
  const occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>& l2,
  const int                      mode) const
{
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> list;
  if (l1.IsNull() || l2.IsNull())
    return list;

  //  mode < 0 l1-l2  = 0 l1&l2  > 0 l1|l2 (l1+l2)
  NCollection_Map<occ::handle<Standard_Transient>> numap;
  int       i, n = l2->Length();
  for (i = n; i > 0; i--)
  {
    occ::handle<Standard_Transient> ent = l2->Value(i);
    if (ent.IsNull())
      continue;
    numap.Add(ent);
    if (mode > 0)
      list->Append(ent);
  }

  //  entities from l1 not already in l2
  n = l1->Length();
  for (i = n; i > 0; i--)
  {
    occ::handle<Standard_Transient> ent = l1->Value(i);
    if (ent.IsNull())
      continue;

    if (numap.Contains(ent))
    {
      //    in l1 and in l2
      if (mode == 0)
        list->Append(ent);
    }
    else
    {
      //    in l1 but not in l2
      if (mode != 0)
        list->Append(ent);
    }
  }

  list->Reverse();
  return list;
}

//=================================================================================================

void IFSelect_WorkSession::DumpModel(const int level, Standard_OStream& S)
{
  if (!IsLoaded())
  {
    S << " ***  Data for List not available  ***" << std::endl;
    return;
  }
  S << "\n        *****************************************************************\n";
  if (theloaded.Length() > 0)
    S << "        ********  Loaded File : " << theloaded.ToCString()
      << Interface_MSG::Blanks(32 - theloaded.Length()) << " ********" << std::endl;
  else
    S << "        ********  No name for Loaded File" << std::endl;
  if (level == 0)
  {
    S << "        ********  Short Dump of Header                           ********\n";
    S << "        *****************************************************************\n\n";
    myModel->DumpHeader(S);
    S << std::endl;
  }

  int     nbent = myModel->NbEntities();
  int     nbr   = 0;
  Interface_ShareFlags shar(thegraph->Graph());

  for (int i = 1; i <= nbent; i++)
  {
    if (!shar.IsShared(myModel->Value(i)))
      nbr++;
  }
  S << "        *****************************************************************\n"
    << "        ********  Model : " << nbent << " Entities, of which " << nbr << " Root(s)\n"
    << "        *****************************************************************\n"
    << std::endl;

  if (level <= 0)
    return;
  else if (level == 1)
  {
    S << "        ********  Root Entities  ********      ";
    ListEntities(shar.RootEntities(), 1, S);
  }
  else if (level == 2)
  {
    S << "        ********  Complete List  ********      ";
    ListEntities(myModel->Entities(), 1, S);
  }
  else if (level > 2)
  {
    IFSelect_PrintCount mode = IFSelect_ItemsByEntity;
    if (level == 5 || level == 8)
      mode = IFSelect_CountByItem;
    if (level == 6 || level == 9)
      mode = IFSelect_ListByItem;
    if (level == 7 || level == 10)
      mode = IFSelect_EntitiesByItem;
    PrintCheckList(S, ModelCheckList(), false, mode);
  }
  else
  {
    if (level == 3)
      S << "        ********  Check Model (Fails)  ********" << std::endl;
    else
      S << "        ********  Check Model (Complete)  ********" << std::endl;
    Interface_CheckTool     CT(Graph());
    Interface_CheckIterator C;
    if (theerrhand)
    {
      try
      {
        OCC_CATCH_SIGNALS
        if (level == 3)
          C = CT.CheckList();
        else
          C = CT.CompleteCheckList();
      }
      catch (Standard_Failure const&)
      {
        Message_Messenger::StreamBuffer sout = Message::SendInfo();
        sout << "    ****    DumpModel (Check) Interrupted by Exception    ****\n";
        S << "  ** **  Exception Raised during Check !  ** **\n";
        S << "  -->  what could be determined is listed" << std::endl;
      }
    }
    else if (level == 3)
      C = CT.CheckList();
    else
      C = CT.CompleteCheckList();

    //  Check List: if empty (not requested), naturally passed
    try
    {
      OCC_CATCH_SIGNALS
      C.Print(S, myModel, (level == 3));
    }
    catch (Standard_Failure const& anException)
    {
      Message_Messenger::StreamBuffer sout = Message::SendInfo();
      sout << "    ****    DumpModel Interrupted by Exception :   ****\n";
      sout << anException.GetMessageString();
      sout << "\n    Abandon" << std::endl;
    }
  }
  S << std::endl
    << "There are " << nbent << " Entities, of which " << nbr << " Root(s)" << std::endl;
}

//  ....        TraceDumpModel        ....  (Model + CheckList)

//=================================================================================================

void IFSelect_WorkSession::TraceDumpModel(const int mode)
{
  // clang-format off
  Message_Messenger::StreamBuffer sout = Message::SendInfo(); // should it be changed to SendTrace()?
  // clang-format on
  DumpModel(mode, sout);
  //  if      (mode <= 4)  {  DumpModel (mode,sout);  return;  }

  //  else if (mode <= 7) PrintCheckList (ModelCheckList(),false, mode-5);
  //  else if (mode <=10) PrintCheckList (ModelCheckList(),true , mode-8);
}

//  ....        DumpEntity        ....

//=================================================================================================

void IFSelect_WorkSession::DumpEntity(const occ::handle<Standard_Transient>& ent,
                                      const int            level,
                                      Standard_OStream&                 S) const
{
  if (!IsLoaded())
  {
    S << " ***  Data for List not available  ***" << std::endl;
    return;
  }
  int num = myModel->Number(ent);
  if (num == 0)
  {
    S << " ***  Entity to Dump not in the Model  ***" << std::endl;
    return;
  }
  if (thelibrary.IsNull())
  {
    S << " ***  WorkLibrary not defined  ***" << std::endl;
    return;
  }
  S << "        ********  Dumping Entity n0 " << num << " level:" << level << "  ********"
    << std::endl;
  thelibrary->DumpEntity(myModel, theprotocol, ent, S, level);
}

//  ....        DumpEntity        ....

//=================================================================================================

void IFSelect_WorkSession::TraceDumpEntity(const occ::handle<Standard_Transient>& ent,
                                           const int            level) const
{
  // clang-format off
  Message_Messenger::StreamBuffer sout = Message::SendInfo(); // should it be changed to SendTrace()?
  // clang-format on
  DumpEntity(ent, level, sout);
}

//  ....        PrintEntityStatus        ....

//=================================================================================================

void IFSelect_WorkSession::PrintEntityStatus(const occ::handle<Standard_Transient>& ent,
                                             Standard_OStream&                 S)
{
  int i, nb;
  int num = StartingNumber(ent);
  if (num == 0)
  {
    std::cout << " --  PrintEntityStatus : unknown" << std::endl;
    return;
  }

  S << "  Ent. n0/id:   ";
  myModel->Print(ent, S);
  occ::handle<TCollection_HAsciiString> hname = EntityName(ent);
  if (!hname.IsNull() && hname->Length() > 0)
    S << "	Name:" << hname->ToCString();
  S << std::endl;
  occ::handle<IFSelect_Signature> signtype = SignType();
  if (signtype.IsNull())
    S << "  Type(CDL):" << ent->DynamicType()->Name() << std::endl;
  else
    S << "  Type:" << signtype->Value(ent, myModel) << std::endl;
  S << "    Category : " << CategoryName(ent) << "    Validity : " << ValidityName(ent)
    << std::endl;
  Interface_CheckIterator chl = CheckOne(ent);
  chl.Print(S, myModel, false, false);

  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> list = Sharings(ent);
  if (list.IsNull())
    S << "  Root" << std::endl;
  else
  {
    nb = list->Length();
    if (nb == 0)
      S << "  Root";
    else
      S << "  Super-entities:" << nb << " : (n0/id):";
    for (i = 1; i <= nb; i++)
    {
      S << " ";
      myModel->Print(list->Value(i), S);
    }
    S << std::endl;
  }
  list = Shareds(ent);
  if (list.IsNull())
    S << "  No sub-entity" << std::endl;
  else
  {
    nb = list->Length();
    if (nb == 0)
      S << "  No sub-entity";
    else
      S << "  Sub-entities:" << nb << " , i.e. (n0/id):";
    for (i = 1; i <= nb; i++)
    {
      S << " ";
      myModel->Print(list->Value(i), S);
    }
    S << std::endl;
  }
}

//  ....        PrintCheckList        ....

//=================================================================================================

void IFSelect_WorkSession::PrintCheckList(Standard_OStream&              S,
                                          const Interface_CheckIterator& checklist,
                                          const bool         failsonly,
                                          const IFSelect_PrintCount      mode) const
{
  //  mode : 0  comptage   1 n0s entites   2 n0s+id ents
  if (mode == IFSelect_ItemsByEntity)
    checklist.Print(S, myModel, failsonly);
  else
  {
    const Interface_CheckIterator& chks = checklist;
    occ::handle<IFSelect_CheckCounter>  counter =
      new IFSelect_CheckCounter(mode > 1 && mode != IFSelect_CountSummary);
    counter->Analyse(chks, myModel, true, failsonly);
    counter->PrintList(S, myModel, mode);
  }
}

//  ....        PrintSignatureList        ....

//=================================================================================================

void IFSelect_WorkSession::PrintSignatureList(Standard_OStream&                     S,
                                              const occ::handle<IFSelect_SignatureList>& signlist,
                                              const IFSelect_PrintCount             mode) const
{
  if (signlist.IsNull())
    return;
  signlist->PrintList(S, myModel, mode);
}

//  ####    ####    ####    ####    ####    ####    ####    ####    ####
//  ....        EvaluateSelection        ....

//=================================================================================================

void IFSelect_WorkSession::EvaluateSelection(const occ::handle<IFSelect_Selection>& sel) const
{
  Message_Messenger::StreamBuffer sout = Message::SendInfo();
  if (errhand)
  {
    errhand = false;
    try
    {
      OCC_CATCH_SIGNALS
      EvaluateSelection(sel); // normal call (->unique code)
    }
    catch (Standard_Failure const& anException)
    {
      sout << "    ****    EvaluateSelection Interrupted by Exception    ****  Title\n";
      sout << anException.GetMessageString();
      sout << "\n    Abandon" << std::endl;
    }
    errhand = theerrhand;
    return;
  }

  if (!IsLoaded())
  {
    sout << " ***  Data for Evaluation not available  ***" << std::endl;
    return;
  }
  if (ItemIdent(sel) == 0)
  {
    sout << " Selection :  Unknown" << std::endl;
    return;
  } // sout<<Handle
  Interface_EntityIterator iter = EvalSelection(sel);
  ListEntities(iter, 1, sout);
  sout << "****  (Unique) RootResult, Selection 	: " << sel->Label() << std::endl;
}

//  ####    ####    ####    ####    ####    ####    ####    ####    ####
//  ....        EvaluateDispatch        ....

//=================================================================================================

void IFSelect_WorkSession::EvaluateDispatch(const occ::handle<IFSelect_Dispatch>& disp,
                                            const int           mode) const
{
  Message_Messenger::StreamBuffer sout = Message::SendInfo();
  if (errhand)
  {
    errhand = false;
    try
    {
      OCC_CATCH_SIGNALS
      EvaluateDispatch(disp, mode); // normal call (->unique code)
    }
    catch (Standard_Failure const& anException)
    {
      sout << "    ****    EvaluateDispatch Interrupted by Exception    ****  Title\n";
      sout << anException.GetMessageString();
      sout << "\n    Abandon" << std::endl;
    }
    errhand = theerrhand;
    return;
  }

  int numdisp = DispatchRank(disp);
  if (!IsLoaded())
  {
    sout << " ***  Data for List not available  ***" << std::endl;
    return;
  }
  if (theshareout->NbDispatches() < numdisp || numdisp <= 0)
  {
    sout << "Dispatch :  Unknown" << std::endl;
    return;
  } // sout<<Handle
  if (disp->FinalSelection().IsNull())
  {
    sout << "Dispatch  : No Final Selection" << std::endl;
    return;
  } // sout<<Handle
  sout << " --- Dispatch Label : " << disp->Label() << std::endl;

  IFSelect_ShareOutResult eval(disp, thegraph->Graph());
  eval.Evaluate();
  int            numpack = 0;
  occ::handle<IFSelect_PacketList> evres   = eval.Packets(mode ? true : false);
  int            nbpack  = evres->NbPackets();

  sout << "Nb Packets produced : " << nbpack << " :" << std::endl;
  for (numpack = 1; numpack <= nbpack; numpack++)
  {
    sout << "\n    ****    Packet n0 : " << numpack << " ****" << std::endl;
    if (!mode)
      std::cout << "Root Entities :" << std::endl;
    ListEntities(evres->Entities(numpack), (mode ? 2 : -1), sout);
  }

  ////  Interface_EntityIterator iterem = disp->Remainder(thegraph->Graph());
  if (mode == 0)
    return;
  if (mode == 1 || mode == 3)
  {
    sout << std::endl;
    if (evres->NbDuplicated(0, false) == 0)
      sout << "    ****    All the Model is taken into account    ****" << std::endl;
    else
    {
      sout << "    ****    Starting Entities not taken by this Dispatch    ****" << std::endl;
      ListEntities(evres->Duplicated(0, false), 2, sout);
    }
  }
  if (mode >= 2)
  {
    sout << "    ****    Entites in more than one packet    ****";
    int max = evres->HighestDuplicationCount();
    if (max < 2)
      sout << " :   There are none" << std::endl;
    else
    {
      int newcount;
      sout << std::endl;
      for (newcount = 2; newcount <= max; newcount++)
      {
        if (evres->NbDuplicated(newcount, false) == 0)
          continue;
        sout << "    ****   Entities put in " << newcount << " packets    ****" << std::endl;
        ListEntities(evres->Duplicated(newcount, false), 2, sout);
      }
    }
  }
}

//  ####    ####    ####    ####    ####    ####    ####    ####    ####
//  ....        EvaluateComplete        ....

//=================================================================================================

void IFSelect_WorkSession::EvaluateComplete(const int mode) const
{
  Message_Messenger::StreamBuffer sout = Message::SendInfo();
  if (errhand)
  {
    errhand = false;
    try
    {
      OCC_CATCH_SIGNALS
      EvaluateComplete(mode); // normal call (therefore, code not duplicated)
    }
    catch (Standard_Failure const& anException)
    {
      sout << "    ****    EvaluateComplete Interrupted by Exception :   ****\n";
      sout << anException.GetMessageString();
      sout << "\n    Abandon" << std::endl;
    }
    errhand = theerrhand;
    return;
  }

  if (!IsLoaded())
  {
    sout << " ***  Data for List not available  ***" << std::endl;
    return;
  }
  IFSelect_ShareOutResult eval(theshareout, thegraph->Graph());
  eval.Evaluate();
  sout << "\n********    Evaluation ShareOutResult (Complete)    ********\n";
  sout << "    ****    List of Packets    ****  Count : " << eval.NbPackets() << std::endl;
  if (mode == 0)
    sout << " ** (for each one : Root Entities)  **" << std::endl;
  else
    sout << " ** (for each one : Evaluated Content)  **" << std::endl;

  int            numpack = 0;
  occ::handle<IFSelect_PacketList> evres   = eval.Packets(mode ? true : false);
  int            nbpack  = evres->NbPackets();

  sout << "Nb Packets produced : " << nbpack << " :" << std::endl;
  for (numpack = 1; numpack <= nbpack; numpack++)
  {
    sout << "\n    ****    Packet n0 : " << numpack << " ****" << std::endl;
    if (!mode)
      std::cout << "Root Entities :" << std::endl;
    ListEntities(evres->Entities(numpack), (mode ? 2 : -1), sout);
  }
  if (mode == 0)
    return;
  if (mode == 1 || mode == 3)
  {
    sout << std::endl;
    if (evres->NbDuplicated(0, false) == 0)
      sout << "    ****    All the Model is taken into account    ****" << std::endl;
    else
    {
      sout << "    ****    Starting Entities Forgotten    ****" << std::endl;
      ListEntities(evres->Duplicated(0, false), 2, sout);
    }
  }
  if (mode >= 2)
  {
    sout << "    ****    Entites in more than one packet    ****" << std::endl;
    int max = evres->HighestDuplicationCount();
    if (max < 2)
      sout << " :   There are none" << std::endl;
    else
    {
      int newcount;
      sout << std::endl;
      for (newcount = 2; newcount <= max; newcount++)
      {
        if (evres->NbDuplicated(newcount, false) == 0)
          continue;
        sout << "    ****   Entities put in " << newcount << " packets    ****" << std::endl;
        ListEntities(evres->Duplicated(newcount, false), 2, sout);
      }
    }
  }
}

//  ####    ####    ####    ####    ####    ####    ####    ####    ####
//  ....      Routine Interne : ListEntities

//=================================================================================================

void IFSelect_WorkSession::ListEntities(const Interface_EntityIterator& iter,
                                        const int          mmode,
                                        Standard_OStream&               sout) const
{
  int              titre = 0;
  int mode  = (mmode < 0 ? -mmode : mmode);
  if (mmode >= 0)
    sout << " List of " << iter.NbEntities() << " Entities :" << std::endl;
  if (!IsLoaded())
  {
    sout << " ***  Data for List not available  ***" << std::endl;
    return;
  }
  Interface_ShareFlags tool(thegraph->Graph());

  try
  {
    OCC_CATCH_SIGNALS
    int newcount = -1;
    int mods     = 0;
    int cnt      = 0;
    for (iter.Start(); iter.More(); iter.Next())
    {
      if (!titre && mode == 1)
        sout << "Number/Id.           Category Validity    Type\n-----------          ----...."
             << std::endl;
      //          123456789 123456789 123456  123456789 123456789 123456
      if (!titre && mode == 0)
        sout << "  Keys : R Root   ? Unknown   * Unloaded" << std::endl;
      if (!titre && mode == 2)
        sout << "(";
      titre                                 = 1;
      const occ::handle<Standard_Transient>& ent = iter.Value();
      int                  num = myModel->Number(ent);
      if (mode == 1)
      {
        // n0 id (root?) category validity tracetype
        sout << Interface_MSG::Blanks(num, 6);
        myModel->Print(ent, sout, 0);
        if (!tool.IsShared(ent))
          sout << " #ROOT#";
        else
          sout << "       ";
        int catnum = myModel->CategoryNumber(num);
        if (catnum > 0)
          sout << "  " << Interface_Category::Name(catnum);
        sout << "  (" << ValidityName(ent) << ")  ";

        sout << " Type:" << myModel->TypeName(ent, false) << std::endl;
      }
      else if (mode == 2)
      {
        newcount++;
        if (newcount > 0)
          sout << ",";
        sout << num;
      }
      else
      {
        newcount++;
        mods = 0;
        cnt++;
        if (newcount >= 10)
        {
          sout << std::endl << "[" << cnt << "]:";
          newcount = 1;
        }
        if (newcount > 0)
          sout << "	";
        myModel->Print(ent, sout, 0);
        if (!tool.IsShared(ent))
        {
          if (mods == 0)
            sout << "(";
          sout << "R";
          mods++;
        }
        if (myModel->IsUnknownEntity(num))
        {
          sout << (mods == 0 ? '(' : ' ') << "?";
          mods++;
        }
        if (myModel->IsRedefinedContent(num))
        {
          sout << (mods == 0 ? '(' : ' ') << "*";
          mods++;
        }
        if (mods)
        {
          sout << ")";
          newcount++;
        }
      }
    }
    if (mode == 0)
      sout << std::endl;
    if (mode == 2)
      sout << ")" << std::endl;
  }
  catch (Standard_Failure const& anException)
  {
    sout << "    ****    ListEntities Interrupted by Exception :   ****\n";
    sout << anException.GetMessageString();
    sout << "\n    Abandon" << std::endl;
  }
}
