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

//#1 svv   10.01.00 : porting on DEC 
//smh#14 17.03.2000 : FRA62479 Clearing of gtool

#include <IFGraph_SubPartsIterator.hxx>
#include <IFSelect_CheckCounter.hxx>
#include <IFSelect_Dispatch.hxx>
#include <IFSelect_DispGlobal.hxx>
#include <IFSelect_DispPerCount.hxx>
#include <IFSelect_DispPerOne.hxx>
#include <IFSelect_EditForm.hxx>
#include <IFSelect_Editor.hxx>
#include <IFSelect_GeneralModifier.hxx>
#include <IFSelect_IntParam.hxx>
#include <IFSelect_ModelCopier.hxx>
#include <IFSelect_Modifier.hxx>
#include <IFSelect_PacketList.hxx>
#include <IFSelect_ParamEditor.hxx>
#include <IFSelect_SelectCombine.hxx>
#include <IFSelect_SelectControl.hxx>
#include <IFSelect_SelectDeduct.hxx>
#include <IFSelect_SelectDiff.hxx>
#include <IFSelect_SelectEntityNumber.hxx>
#include <IFSelect_SelectExtract.hxx>
#include <IFSelect_SelectIntersection.hxx>
#include <IFSelect_Selection.hxx>
#include <IFSelect_SelectionIterator.hxx>
#include <IFSelect_SelectModelEntities.hxx>
#include <IFSelect_SelectModelRoots.hxx>
#include <IFSelect_SelectPointed.hxx>
#include <IFSelect_SelectSignature.hxx>
#include <IFSelect_SelectUnion.hxx>
#include <IFSelect_ShareOut.hxx>
#include <IFSelect_ShareOutResult.hxx>
#include <IFSelect_Signature.hxx>
#include <IFSelect_SignatureList.hxx>
#include <IFSelect_SignCounter.hxx>
#include <IFSelect_SignValidity.hxx>
#include <IFSelect_Transformer.hxx>
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
#include <Interface_GeneralLib.hxx>
#include <Interface_GeneralModule.hxx>
#include <Interface_Graph.hxx>
#include <Interface_GTool.hxx>
#include <Interface_HGraph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Interface_MSG.hxx>
#include <Interface_Protocol.hxx>
#include <Interface_ReportEntity.hxx>
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
#include <TColStd_MapOfInteger.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_WorkSession,Standard_Transient)

#define Flag_Incorrect 2
//  (Bit Map n0 2)


static Standard_Boolean errhand;  // pb : un seul a la fois, mais ca va si vite
static TCollection_AsciiString bufstr;


//  #################################################################


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

IFSelect_WorkSession::IFSelect_WorkSession ()
{
  theshareout  = new IFSelect_ShareOut;
  theerrhand   = errhand = Standard_True;
  thecopier    = new IFSelect_ModelCopier;
  thecopier->SetShareOut (theshareout);
  thecheckdone = Standard_False;
  thegtool     = new Interface_GTool;
  themodelstat = Standard_False;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::SetErrorHandle (const Standard_Boolean toHandle)
{
  theerrhand = errhand = toHandle;
}
 

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::SetProtocol
  (const Handle(Interface_Protocol)& protocol)
{ 
  theprotocol = protocol;
  Interface_Protocol::SetActive(protocol);
  thegtool->SetProtocol (protocol);
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::SetSignType (const Handle(IFSelect_Signature)& signtype)
{
  thegtool->SetSignType (signtype);
  if (signtype.IsNull()) thenames.UnBind("xst-sign-type");
  else thenames.Bind("xst-sign-type",signtype);
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(IFSelect_Signature)  IFSelect_WorkSession::SignType () const
{
  return Handle(IFSelect_Signature)::DownCast (thegtool->SignType());
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void  IFSelect_WorkSession::SetShareOut
    (const Handle(IFSelect_ShareOut)& shareout)
{
  theshareout = shareout;
  thecopier->SetShareOut (theshareout);
// ... faudrait ajouter les Params, Dispatches, etc...
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void  IFSelect_WorkSession::SetModel
    (const Handle(Interface_InterfaceModel)& model,
     const Standard_Boolean clearpointed)
{
  if (myModel != model)
    theloaded.Clear();
  myModel = model;
  if (!thegtool.IsNull()) thegtool->ClearEntities(); //smh#14 FRA62479
  myModel->SetGTool (thegtool);
  
  thegraph.Nullify();
  ComputeGraph();    // fait qqchose si Protocol present. Sinon, ne fait rien
  ClearData(3);      // RAZ CheckList, a refaire
  thecheckrun.Clear();
  
//  MISE A JOUR des SelectPointed  C-A-D  on efface leur contenu
  if (clearpointed) ClearData(4);
  ClearData(0);
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

IFSelect_ReturnStatus  IFSelect_WorkSession::ReadFile
                                        (const Standard_CString filename)
{
  if (thelibrary.IsNull()) return IFSelect_RetVoid;
  if (theprotocol.IsNull()) return IFSelect_RetVoid;
  Handle(Interface_InterfaceModel) model;
  IFSelect_ReturnStatus status = IFSelect_RetVoid;
  try {
    OCC_CATCH_SIGNALS
    Standard_Integer stat = thelibrary->ReadFile (filename,model,theprotocol);
    if (stat == 0) status = IFSelect_RetDone;
    else if (stat < 0) status = IFSelect_RetError;
    else status = IFSelect_RetFail;
  }
  catch(Standard_Failure const& anException) {
    Handle(Message_Messenger) sout = Message::DefaultMessenger();
    sout<<"    ****    Interruption ReadFile par Exception :   ****\n";
    sout << anException.GetMessageString();
    sout<<"\n    Abandon"<<endl;
    status = IFSelect_RetFail;
  }
  if (status != IFSelect_RetDone) return status;
  if (model.IsNull()) return IFSelect_RetVoid;
  SetModel (model);
  SetLoadedFile (filename);
  return status;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer IFSelect_WorkSession::NbStartingEntities () const
{
  return (myModel.IsNull()? 0 : myModel->NbEntities());
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(Standard_Transient) IFSelect_WorkSession::StartingEntity (const Standard_Integer num) const
{
  Handle(Standard_Transient) res;  // Null par defaut
  if (myModel.IsNull()) return res;
  if (num < 1 || num > myModel->NbEntities()) return res;
  return myModel->Value(num);
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer IFSelect_WorkSession::StartingNumber (const Handle(Standard_Transient)& ent) const
{
  return (myModel.IsNull()? 0 : myModel->Number(ent));
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer IFSelect_WorkSession::NumberFromLabel
  (const Standard_CString val, const Standard_Integer afternum) const
{
  Standard_Integer i, cnt = 0, num = atoi(val);
  if (num > 0 || myModel.IsNull()) return num;    // un n0 direct : gagne !
//  Sinon, on considere que c est un label; a traiter en CaseNonSensitive ...
  if (num > myModel->NbEntities())  { num = 0; return num; }
  Standard_Boolean exact = Standard_False;
  Standard_Integer after = (afternum >= 0 ? afternum : -afternum);
  for (i = myModel->NextNumberForLabel (val, after, exact)  ; i != 0;
       i = myModel->NextNumberForLabel (val, i, exact)) {
    cnt ++;
    if (num <= 0) num = i;
  }
  if (cnt == 1) return num;
  num = -num;

  return num;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) IFSelect_WorkSession::EntityLabel (const Handle(Standard_Transient)& ent) const
{
  if (myModel.IsNull() || ent.IsNull()) return 0;
  if (!myModel->Contains(ent)) return 0;
  return myModel->StringLabel(ent);
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) IFSelect_WorkSession::EntityName (const Handle(Standard_Transient)& ent) const
{
  if (myModel.IsNull() || ent.IsNull()) return 0;
  Interface_ShareTool sht(thegraph->Graph());

  Standard_Integer CN;
  Handle(Interface_GeneralModule) module;
  if (!thegtool->Select (ent,module,CN)) return 0;
  return module->Name (CN,ent,sht);
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer IFSelect_WorkSession::CategoryNumber (const Handle(Standard_Transient)& ent) const
{
  return (myModel.IsNull()? -1 : myModel->CategoryNumber(StartingNumber(ent)));
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_CString IFSelect_WorkSession::CategoryName (const Handle(Standard_Transient)& ent) const
{
  return Interface_Category::Name (CategoryNumber (ent));
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_CString IFSelect_WorkSession::ValidityName (const Handle(Standard_Transient)& ent) const
{
  return ((StartingNumber(ent) == 0)? "" : IFSelect_SignValidity::CVal (ent,myModel));
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::ClearData (const Standard_Integer mode)
{
  switch (mode) {
    case 1 : {
      theloaded.Clear();
      if (!myModel.IsNull()) {
        myModel->Clear();
        myModel.Nullify();
      }
      ClearData(2);  ClearData(4);
      thecheckrun.Clear();
      break;
    }
    case 2 : {  thegraph.Nullify();  thecheckdone = Standard_False;  thecheckana.Clear();  break;  }
    case 3 : {  thecheckdone = Standard_False;  break;  }
    case 4 : {
//  MISE A JOUR des SelectPointed  C-A-D  on efface leur contenu
//  AINSI que des editeurs (en fait, les EditForm)
//  Des compteurs  C-A-D  on efface leur contenu (a reevaluer)
      Handle(TColStd_HSequenceOfInteger) list =
	ItemIdents(STANDARD_TYPE(IFSelect_SelectPointed));
      Standard_Integer nb = list->Length();
      Standard_Integer i; // svv #1 
      for (i = 1; i <= nb; i ++) {
	DeclareAndCast(IFSelect_SelectPointed,sp,Item(list->Value(i)));
	if (!sp.IsNull()) sp->Clear();
      }
      list = ItemIdents(STANDARD_TYPE(IFSelect_SignatureList));
      nb = list->Length();
      for (i = 1; i <= nb; i ++) {
	DeclareAndCast(IFSelect_SignatureList,sl,Item(list->Value(i)));
	if (!sl.IsNull()) sl->Clear();
	DeclareAndCast(IFSelect_SignCounter,sc,sl);
	if (!sc.IsNull()) sc->SetSelMode(-1);
      }
      list = ItemIdents(STANDARD_TYPE(IFSelect_EditForm));
      nb = list->Length();
      Handle(Standard_Transient) nulent;
      for (i = 1; i <= nb; i ++) {
	DeclareAndCast(IFSelect_EditForm,edf,Item(list->Value(i)));
	edf->ClearData ();
      }
      theitems.Clear();
      break;
    }
    default : break;
  }
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean  IFSelect_WorkSession::ComputeGraph
  (const Standard_Boolean enforce)
{
  if (theprotocol.IsNull()) return Standard_False;
  if (myModel.IsNull()) return Standard_False;
  if (enforce) thegraph.Nullify();
  if (!thegraph.IsNull()) {
    if (myModel->NbEntities() == thegraph->Graph().Size()) return Standard_True;
    thegraph.Nullify();
  }
  if (myModel->NbEntities() == 0) return Standard_False;
  //  Il faut calculer le graphe pour de bon
  thegraph = new Interface_HGraph (myModel,themodelstat);
  Standard_Integer nb = myModel->NbEntities();
  if(themodelstat)
  {
    Standard_Integer i; // svv #1
    for (i = 1; i <= nb; i ++) thegraph->CGraph().SetStatus(i,0);
    Interface_BitMap& bm = thegraph->CGraph().CBitMap();
    bm.AddFlag();
    bm.SetFlagName (Flag_Incorrect,"Incorrect");
  }
  ComputeCheck();
  thecheckdone = Standard_True;
  if(themodelstat)
  {
    //  Calcul des categories, a present memorisees dans le modele
    Interface_Category categ(thegtool);
    Interface_ShareTool sht(thegraph);
    Standard_Integer i =1;
    for ( ; i <= nb; i ++)
      myModel->SetCategoryNumber(i,categ.CatNum(myModel->Value(i),sht));
  }

  return Standard_True;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(Interface_HGraph) IFSelect_WorkSession::HGraph ()
{
  ComputeGraph();
  return thegraph;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

const Interface_Graph& IFSelect_WorkSession::Graph ()
{
  ComputeGraph();
  if (thegraph.IsNull()) throw Standard_DomainError("IFSelect WorkSession : Graph not available");
  return thegraph->Graph();
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfTransient) IFSelect_WorkSession::Shareds
  (const Handle(Standard_Transient)& ent)
{
  Handle(TColStd_HSequenceOfTransient) list;
  if (!ComputeGraph()) return list;
  if (StartingNumber(ent) == 0) return list;
  return thegraph->Graph().Shareds(ent).Content();
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfTransient) IFSelect_WorkSession::Sharings
  (const Handle(Standard_Transient)& ent)
{
  Handle(TColStd_HSequenceOfTransient) list;
  if (!ComputeGraph()) return list;
  if (StartingNumber(ent) == 0) return list;
  return thegraph->Graph().Sharings(ent).Content();
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_WorkSession::IsLoaded () const
{
  if (theprotocol.IsNull()) return Standard_False;
  if (myModel.IsNull()) return Standard_False;
  if (myModel->NbEntities() == 0) return Standard_False;
  if (thegraph.IsNull()) return Standard_False;
  if (myModel->NbEntities() == thegraph->Graph().Size()) return Standard_True;
  return Standard_False;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_WorkSession::ComputeCheck
  (const Standard_Boolean enforce)
{
  if (enforce) thecheckdone = Standard_False;
  if (thecheckdone) return Standard_True;
  if (!IsLoaded()) return Standard_False;

  Interface_Graph& CG = thegraph->CGraph();
  Interface_CheckTool cht(thegraph);
  Interface_CheckIterator checklist = cht.VerifyCheckList();
  myModel->FillSemanticChecks(checklist,Standard_False);
  if(themodelstat)
  {
    //  Et on met a jour le Graphe (BitMap) !  Flag Incorrect (STX + SEM)
    Interface_BitMap& BM = CG.CBitMap();
    BM.Init (Standard_False,Flag_Incorrect);
    Standard_Integer num, nb = CG.Size();
    for (checklist.Start(); checklist.More(); checklist.Next()) {
      const Handle(Interface_Check) chk = checklist.Value();
      if (!chk->HasFailed()) continue;
      num = checklist.Number();
      if (num > 0 && num <= nb) BM.SetTrue (num,Flag_Incorrect);
    }
    for (num = 1; num <= nb; num ++)
      if (myModel->IsErrorEntity (num)) BM.SetTrue (num,Flag_Incorrect);
  }
  return Standard_True;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Interface_CheckIterator IFSelect_WorkSession::ModelCheckList
  (const Standard_Boolean complete)
{
  Interface_CheckIterator checks;
  if (!IsLoaded()) {
    checks.CCheck(0)->AddFail("DATA NOT AVAILABLE FOR CHECK");
    return checks;
  }
  Interface_CheckTool cht(Graph());
  checks = (complete ? cht.CompleteCheckList() : cht.AnalyseCheckList());
  checks.SetName
    ((char*)(complete ? "Model Complete Check List" : "Model Syntactic Check List"));
  return checks;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Interface_CheckIterator IFSelect_WorkSession::CheckOne
  (const Handle(Standard_Transient)& ent,
   const Standard_Boolean complete)
{
  Interface_CheckIterator checks;
  checks.SetModel(myModel);
  if (!IsLoaded()) {
    checks.CCheck(0)->AddFail("DATA NOT AVAILABLE FOR CHECK");
    return checks;
  }
  Standard_Integer num = -1;
  if (ent.IsNull() || ent == myModel) num = 0;
  else num = myModel->Number(ent);

  Handle(Interface_Check) ach = myModel->Check (num,Standard_True);
  if (complete) ach->GetMessages (myModel->Check (num,Standard_False));
  if (num > 0) ach->SetEntity(ent);
  checks.Add (ach,num);
  checks.SetName ("Data Check (One Entity)");
  return checks;
}


//  #####################################################################
//  ....                        LES VARIABLES                        ....

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer IFSelect_WorkSession::MaxIdent () const
{
  return theitems.Extent();
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(Standard_Transient) IFSelect_WorkSession::Item
  (const Standard_Integer id) const
{
  Handle(Standard_Transient) res;
  if (id <= 0 || id > MaxIdent()) return res;
  if (theitems.FindFromIndex(id).IsNull()) return res;
  return theitems.FindKey(id);
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer IFSelect_WorkSession::ItemIdent
  (const Handle(Standard_Transient)& item) const
{
  if (item.IsNull()) return 0;
  Standard_Integer id = theitems.FindIndex(item);
  if (id == 0) return 0;
  if (theitems.FindFromIndex(id).IsNull()) return 0;
  return id;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(Standard_Transient) IFSelect_WorkSession::NamedItem
  (const Standard_CString name) const
{
  Handle(Standard_Transient) res;
  if (name[0] == '\0') return res;
  if (name[0] == '#') {    // #nnn : pas un nom mais un n0 id.
    Standard_Integer id = atoi( &name[1] );
    return Item(id);
  }
  if (!thenames.Find(name,res))
    res.Nullify();
  return res;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(Standard_Transient) IFSelect_WorkSession::NamedItem
  (const Handle(TCollection_HAsciiString)& name) const
{
  Handle(Standard_Transient) res;
  if (!name.IsNull()) res = NamedItem (name->ToCString());
  return res;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer IFSelect_WorkSession::NameIdent
  (const Standard_CString name) const
{
  Handle(Standard_Transient) res;
  if (name[0] == '\0') return 0;
  if (name[0] == '#') {    // #nnn : pas un nom mais un n0 id.
    Standard_Integer id = atoi( &name[1] );
    return id;
  }
  if (!thenames.Find(name,res))
    return 0;
  return ItemIdent(res);
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_WorkSession::HasName
  (const Handle(Standard_Transient)& item) const
{
  if (item.IsNull()) return Standard_False;
  Standard_Integer id = theitems.FindIndex(item);
  if (id == 0) return Standard_False;
  Handle(Standard_Transient) att = theitems.FindFromIndex(id);
  if (att.IsNull()) return Standard_False;
  return att->IsKind(STANDARD_TYPE(TCollection_HAsciiString));
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) IFSelect_WorkSession::Name
  (const Handle(Standard_Transient)& item) const
{
  Handle(TCollection_HAsciiString) res;
  if (item.IsNull()) return res;
  Standard_Integer id = theitems.FindIndex(item);
  if (id == 0) return res;  // Null
  Handle(Standard_Transient) att = theitems.FindFromIndex(id);
  return GetCasted(TCollection_HAsciiString,att);
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer IFSelect_WorkSession::AddItem
  (const Handle(Standard_Transient)& item,
   const Standard_Boolean active)
{
  if (item.IsNull()) return 0;
  Standard_Integer id = theitems.FindIndex(item);
  if (id > 0) {
    Handle(Standard_Transient)& att = theitems.ChangeFromIndex(id);
    if (att.IsNull()) att = item;
  }
  else id = theitems.Add(item,item);

//  Cas particuliers : Dispatch,Modifier
  if (active) SetActive(item,Standard_True);
  return id;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer IFSelect_WorkSession::AddNamedItem
  (const Standard_CString name, const Handle(Standard_Transient)& item,
   const Standard_Boolean active)
{
  if (item.IsNull()) return 0;
  if (name[0] == '#' || name[0] == '!') return 0;
// #nnn : pas un nom mais un numero. !... : reserve (interdit pour un nom)
//   nom deja pris : on ecrase l ancienne valeur
  if (name[0] != '\0')
    thenames.Bind(name, item);

  Standard_Integer  id = theitems.FindIndex(item);
  if (id > 0) {
    Handle(Standard_Transient)& att = theitems.ChangeFromIndex(id);
    if (att.IsNull()) att = item;
    if (name[0] != '\0') {
//      if (!att->IsKind(STANDARD_TYPE(TCollection_HAsciiString))) ecrasement admis !
	att = new TCollection_HAsciiString(name);
    }
  }
  else if (name[0] != '\0')
    id = theitems.Add(item,new TCollection_HAsciiString(name));
  else id = theitems.Add(item,item);

//  Cas particuliers : Dispatch,Modifier
  if (active) SetActive(item,Standard_True);
  return id;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_WorkSession::SetActive
  (const Handle(Standard_Transient)& item, const Standard_Boolean mode)
{
  if (item->IsKind(STANDARD_TYPE(IFSelect_Dispatch))) {
    DeclareAndCast(IFSelect_Dispatch,disp,item);
    Standard_Integer num = theshareout->DispatchRank(disp);
    if ( num > theshareout->NbDispatches()) return Standard_False;
    if ( mode) {
      if (num >  0) return Standard_False;
      theshareout->AddDispatch (disp);
      return Standard_True;
    } else {
      if (num <= theshareout->LastRun()) return Standard_False;
      theshareout->RemoveDispatch(num);
      SetFileRoot(disp,"");    // si onlynamed : nettoie aussi ShareOut
      return Standard_True;
    }
  }

  return Standard_False;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_WorkSession::RemoveNamedItem
  (const Standard_CString name)
{
  Handle(Standard_Transient) item = NamedItem(name);
  if (item.IsNull()) return Standard_False;
  if (!RemoveItem(item)) return Standard_False;    // qui se charge de tout
  return Standard_True;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_WorkSession::RemoveName
  (const Standard_CString name)
{
  Handle(Standard_Transient) item = NamedItem(name);
  if (item.IsNull()) return Standard_False;
  theitems.Add(item,item);    // reste mais sans nom
  return thenames.UnBind(name);
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_WorkSession::RemoveItem
  (const Handle(Standard_Transient)& item)
{
  if (item.IsNull()) return Standard_False;
  Standard_Integer id = theitems.FindIndex(item);
  if (id == 0) return Standard_False;
  Handle(Standard_Transient)& att = theitems.ChangeFromIndex(id);
  if (att.IsNull()) return Standard_False;    // deja annulle

//  Cas particuliers : Dispatch,Modifier
  theshareout->RemoveItem(item);

//  Marquer "Removed" dans la Map (on ne peut pas la vider)
  if (att->IsKind(STANDARD_TYPE(TCollection_HAsciiString))) {
    if (!thenames.UnBind
	(GetCasted(TCollection_HAsciiString,att)->ToCString()))
      return Standard_False;
  }
  att.Nullify();  // cf ChangeFromIndex
//  id = theitems.Add(item,att);
  return Standard_True;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::ClearItems ()
{
  thenames.Clear();
  theitems.Clear();
  theshareout->Clear(Standard_False);
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) IFSelect_WorkSession::ItemLabel
  (const Standard_Integer id) const
{
  Handle(TCollection_HAsciiString) res;
  Handle(Standard_Transient) var = Item(id);
  if (var.IsNull()) return res;
  DeclareAndCast(TCollection_HAsciiString,text,var);
  if (!text.IsNull()) {
    res = new TCollection_HAsciiString("Text:");
    res->AssignCat(text);
    return res;
  }
  DeclareAndCast(IFSelect_IntParam,intpar,var);
  if (!intpar.IsNull()) {
    res = new TCollection_HAsciiString(intpar->Value());
    res->Insert(1,"Integer:");
    return res;
  }
  DeclareAndCast(IFSelect_Selection,sel,var);
  if (!sel.IsNull()) {
    res = new TCollection_HAsciiString("Selection:");
    res->AssignCat(sel->Label().ToCString());
    return res;
  }
  DeclareAndCast(IFSelect_GeneralModifier,mod,var);
  if (!mod.IsNull()) {
    if (mod->IsKind(STANDARD_TYPE(IFSelect_Modifier)))
      res    = new TCollection_HAsciiString("ModelModifier:");
    else res = new TCollection_HAsciiString("FileModifier:");
    res->AssignCat(mod->Label().ToCString());
    return res;
  }
  DeclareAndCast(IFSelect_Dispatch,disp,var);
  if (!disp.IsNull()) {
    res = new TCollection_HAsciiString("Dispatch:");
    res->AssignCat(disp->Label().ToCString());
    return res;
  }
  DeclareAndCast(IFSelect_Transformer,tsf,var);
  if (!tsf.IsNull()) {
    res = new TCollection_HAsciiString("Transformer:");
    res->AssignCat(tsf->Label().ToCString());
    return res;
  }
  DeclareAndCast(IFSelect_SignatureList,slc,var);
  if (!slc.IsNull()) {
    res = new TCollection_HAsciiString("Counter:");
    res->AssignCat(slc->Name());
    return res;
  }
  DeclareAndCast(IFSelect_Signature,sig,var);
  if (!sig.IsNull()) {
    res = new TCollection_HAsciiString("Signature:");
    res->AssignCat(sig->Name());
    return res;
  }
  DeclareAndCast(IFSelect_EditForm,edf,var);
  if (!edf.IsNull()) {
    res = new TCollection_HAsciiString("EditForm:");
    res->AssignCat(edf->Label());
    return res;
  }
  DeclareAndCast(IFSelect_Editor,edt,var);
  if (!edt.IsNull()) {
    res = new TCollection_HAsciiString("Editor:");
    res->AssignCat(edt->Label().ToCString());
    return res;
  }
  res = new TCollection_HAsciiString("VariableType:");
  res->AssignCat(var->DynamicType()->Name());
  return res;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfInteger) IFSelect_WorkSession::ItemIdents
  (const Handle(Standard_Type)& type) const
{
  Handle(TColStd_HSequenceOfInteger) list =
    new TColStd_HSequenceOfInteger();
  Standard_Integer nb = theitems.Extent();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (theitems.FindKey(i)->IsKind(type)) list->Append(i);
  }
  return list;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfHAsciiString) IFSelect_WorkSession::ItemNames
  (const Handle(Standard_Type)& type) const
{
  Handle(TColStd_HSequenceOfHAsciiString) list =
    new TColStd_HSequenceOfHAsciiString();
  NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)>::Iterator IT(thenames);
  for (; IT.More(); IT.Next()){
    if (IT.Value()->IsKind(type)) list->Append
      (new TCollection_HAsciiString(IT.Key()));
  }
  return list;
}


// ..  Recherche par label : recherche en liste(noms) ou iterative

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfHAsciiString) IFSelect_WorkSession::ItemNamesForLabel
  (const Standard_CString label) const
{
  Handle(TColStd_HSequenceOfHAsciiString) list =
    new TColStd_HSequenceOfHAsciiString();
  Standard_Integer i,  nb = MaxIdent();
  for (i = 1; i <= nb; i ++) {
    Handle(TCollection_HAsciiString) lab = ItemLabel(i);
    Handle(Standard_Transient) item = Item(i);
    if (lab.IsNull()) continue;
    if (label[0] != '\0' && lab->Search(label) <= 0) continue;

    Handle(TCollection_HAsciiString) nom = Name(Item(i));
    if (nom.IsNull()) { nom = new TCollection_HAsciiString(i); nom->Insert(1,'#'); }
    else nom = new TCollection_HAsciiString (nom);
    list->Append (new TCollection_HAsciiString(lab));
  }
  return list;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer IFSelect_WorkSession::NextIdentForLabel
  (const Standard_CString label, const Standard_Integer id,
   const Standard_Integer mode) const
{
  Standard_Integer nb = MaxIdent();
  for (Standard_Integer i = id+1; i <= nb; i ++) {
    Handle(TCollection_HAsciiString) lab = ItemLabel(i);
    if (lab.IsNull()) continue;
    switch (mode) {
      case 0 : if (!strcmp(lab->ToCString(),label)) return i;  break; // switch
      case 1 : if (lab->Search(label) == 1 ) return i;  break;
      case 2 : if (lab->Search(label)  > 0 ) return i;  break;
      default : break;  // break du switch
    }
  }
  return 0;  // ici : pas trouve
}


//  #################################################################
//  ....                Parametres (Int et Text)                ....

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(Standard_Transient) IFSelect_WorkSession::NewParamFromStatic
  (const Standard_CString statname, const Standard_CString name)
{
  Handle(Standard_Transient) param;
  Handle(Interface_Static) stat = Interface_Static::Static(statname);
  if (stat.IsNull()) return param;
  if (stat->Type() == Interface_ParamInteger) {
    Handle(IFSelect_IntParam) intpar = new IFSelect_IntParam;
    intpar->SetStaticName (statname);
    param = intpar;
  } else {
    param = stat->HStringValue();
  }
  if (param.IsNull()) return param;
  if ( AddNamedItem (name, param) == 0 ) param.Nullify();
  return param;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(IFSelect_IntParam) IFSelect_WorkSession::IntParam
  (const Standard_Integer id) const
{  return Handle(IFSelect_IntParam)::DownCast(Item(id));  }


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer IFSelect_WorkSession::IntValue
  (const Handle(IFSelect_IntParam)& par) const
{
  if (!par.IsNull()) return par->Value();
  else return 0;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(IFSelect_IntParam) IFSelect_WorkSession::NewIntParam
  (const Standard_CString name)
{
  Handle(IFSelect_IntParam) intpar = new IFSelect_IntParam;
  if ( AddNamedItem (name, intpar) == 0 ) intpar.Nullify();
  return intpar;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_WorkSession::SetIntValue
  (const Handle(IFSelect_IntParam)& par, const Standard_Integer val)
{
  if (ItemIdent(par) == 0) return Standard_False;
  par->SetValue(val);
  return Standard_True;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) IFSelect_WorkSession::TextParam
  (const Standard_Integer id) const
{  return Handle(TCollection_HAsciiString)::DownCast(Item(id));  }


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

TCollection_AsciiString IFSelect_WorkSession::TextValue
  (const Handle(TCollection_HAsciiString)& par) const 
{
  if (!par.IsNull()) return TCollection_AsciiString(par->ToCString());
  else return TCollection_AsciiString();
}


Handle(TCollection_HAsciiString) IFSelect_WorkSession::NewTextParam
  (const Standard_CString name)
{
  Handle(TCollection_HAsciiString) textpar = new TCollection_HAsciiString("");
  if ( AddNamedItem (name, textpar) == 0 ) textpar.Nullify();
  return textpar;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean  IFSelect_WorkSession::SetTextValue
  (const Handle(TCollection_HAsciiString)& par, const Standard_CString val)
{
  if (ItemIdent(par) == 0) return Standard_False;
  par->Clear();  par->AssignCat(val);
  return Standard_True;
}

//  ########################################################################
//  ....                           SIGNATURES                           ....

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(IFSelect_Signature) IFSelect_WorkSession::Signature
  (const Standard_Integer id) const 
{  return GetCasted(IFSelect_Signature,Item(id));  }

    Standard_CString  IFSelect_WorkSession::SignValue
  (const Handle(IFSelect_Signature)& sign, const Handle(Standard_Transient)& ent) const
{
  if (sign.IsNull() || myModel.IsNull()) return "";
  if (StartingNumber(ent) == 0) return "";
  return sign->Value (ent,myModel);
}

//  ########################################################################
//  ....                        SELECTIONS & Cie                        ....

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(IFSelect_Selection) IFSelect_WorkSession::Selection
  (const Standard_Integer id) const 
{  return GetCasted(IFSelect_Selection,Item(id));  }


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Interface_EntityIterator IFSelect_WorkSession::EvalSelection
  (const Handle(IFSelect_Selection)& sel) const 
{
  Interface_EntityIterator iter;
  if (errhand) {
    errhand = Standard_False;
    try {
      OCC_CATCH_SIGNALS
      iter = EvalSelection(sel);    // appel normal (donc, code pas duplique)
    }
    catch (Standard_Failure const& anException) {
      Handle(Message_Messenger) sout = Message::DefaultMessenger();
      sout<<"    ****    Interruption EvalSelection par Exception :   ****\n";
      sout<<anException.GetMessageString();
      sout<<"\n    Abandon"<<endl;
    }
    errhand = theerrhand;
    return iter;
  }

  if (thegraph.IsNull()) return iter;
 iter = sel->UniqueResult(thegraph->Graph());
  return iter;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

IFSelect_SelectionIterator IFSelect_WorkSession::Sources
  (const Handle(IFSelect_Selection)& sel) const 
{
  return IFSelect_SelectionIterator (sel);
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfTransient) IFSelect_WorkSession::SelectionResult
  (const Handle(IFSelect_Selection)& sel) const 
{
  Handle(TColStd_HSequenceOfTransient)  res;
  if (errhand) {
    errhand = Standard_False;
    try {
      OCC_CATCH_SIGNALS
      res = SelectionResult(sel);    // appel normal (->code unique)
    }
    catch (Standard_Failure const& anException) {
      Handle(Message_Messenger) sout = Message::DefaultMessenger();
      sout<<"    ****    Interruption SelectionResult par Exception :   ****\n";
      sout<<anException.GetMessageString();
      sout<<"\n    Abandon"<<endl;
    }
    errhand = theerrhand;
    return res;
  }

  if (!IsLoaded()) {
    cout<< " ***  Data for Evaluation not available  ***"<<endl;
    return new TColStd_HSequenceOfTransient();
  }
//  if (ItemIdent(sel) == 0)
  if (sel.IsNull())
    {  cout << " Selection :  Unknown"<<endl;  return res;  } //cout<<Handle
  return EvalSelection (sel).Content();
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfTransient) IFSelect_WorkSession::SelectionResultFromList
  (const Handle(IFSelect_Selection)& sel,
   const Handle(TColStd_HSequenceOfTransient)& list) const
{
  if (list.IsNull()) return SelectionResult (sel);
  DeclareAndCast(IFSelect_SelectDeduct,deduct,sel);
  if (deduct.IsNull()) return SelectionResult (sel);

//   On va chercher la derniere deduction de la chaine des inputs
  Handle(IFSelect_Selection) ssel, newinput;
  ssel = sel;
  Standard_Integer i, nb = MaxIdent();
  for (i = 1; i <= nb * 2; i ++) {
    newinput  = deduct->Input();
    deduct = GetCasted(IFSelect_SelectDeduct,newinput);
    if (deduct.IsNull()) break;
    ssel   = newinput;
  }
  
//  on y est (enfin, on devrait)
//  ssel est la derniere selection auscultee,  deduct son downcast
//  input son Input (nulle  si sel  pas une deduction)
  deduct = GetCasted(IFSelect_SelectDeduct,ssel);

  deduct->Alternate()->SetList (list);

//   On execute puis on nettoie
  Handle(TColStd_HSequenceOfTransient) res = SelectionResult (sel);
////  deduct->SetInput (newinput);
  return res;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_WorkSession::SetItemSelection
  (const Handle(Standard_Transient)& item,
   const Handle(IFSelect_Selection)& sel)
{
  DeclareAndCast(IFSelect_Dispatch,disp,item);
  DeclareAndCast(IFSelect_GeneralModifier,modif,item);
  if (!disp.IsNull()) {
    if (ItemIdent(disp) == 0) return Standard_False;
//  Selection Nulle : Annuler FinalSelection
    if (!sel.IsNull() && ItemIdent(sel) == 0) return Standard_False;
    disp->SetFinalSelection(sel);
    return Standard_True;
  }
  if (!modif.IsNull()) {
    if (ItemIdent(modif) == 0) return Standard_False;
    if (!sel.IsNull() && ItemIdent(sel) == 0) return Standard_False;
//   Selection Nulle : Annuler Selection
    modif->SetSelection(sel);
    return Standard_True;
  }
  return Standard_False;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_WorkSession::ResetItemSelection
  (const Handle(Standard_Transient)& item)
{
  Handle(IFSelect_Selection) nulsel;
  return SetItemSelection (item,nulsel);
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(IFSelect_Selection) IFSelect_WorkSession::ItemSelection
  (const Handle(Standard_Transient)& item) const
{
  Handle(IFSelect_Selection) sel;
  DeclareAndCast(IFSelect_Dispatch,disp,item);
  DeclareAndCast(IFSelect_GeneralModifier,modif,item);
  if (ItemIdent(disp)  > 0) return disp->FinalSelection();
  if (ItemIdent(modif) > 0) return modif->Selection();
  return sel;         // Nul ou inconnu -> Null
}

//  ######################################################################
//  ....                        Les COMPTEURS                         ....

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(IFSelect_SignCounter) IFSelect_WorkSession::SignCounter
  (const Standard_Integer id) const 
{  return GetCasted(IFSelect_SignCounter,Item(id));  }


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_WorkSession::ComputeCounter
  (const Handle(IFSelect_SignCounter)& counter, const Standard_Boolean forced)
{
  if (counter.IsNull()) return Standard_False;
  if (!ComputeGraph())  return Standard_False;
  return counter->ComputeSelected (Graph(),forced);
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_WorkSession::ComputeCounterFromList
  (const Handle(IFSelect_SignCounter)& counter,
   const Handle(TColStd_HSequenceOfTransient)& list,
   const Standard_Boolean clear)
{
  if (counter.IsNull()) return Standard_False;
  if (clear) counter->Clear();
  if (list.IsNull()) return ComputeCounter (counter,Standard_True);
  counter->AddList (list,myModel);
  return Standard_True;
}

//  ######################################################################
//  ....                        Les DISPATCHES                        ....

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfInteger) IFSelect_WorkSession::AppliedDispatches
  () const 
{
  Handle(TColStd_HSequenceOfInteger) list = new TColStd_HSequenceOfInteger();
  Standard_Integer nb = theshareout->NbDispatches();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    list->Append (ItemIdent(theshareout->Dispatch(i)));
  }
  return list;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::ClearShareOut (const Standard_Boolean onlydisp)
{
  theshareout->Clear(onlydisp);
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(IFSelect_Dispatch) IFSelect_WorkSession::Dispatch
  (const Standard_Integer id) const 
{
  return GetCasted(IFSelect_Dispatch,Item(id));
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer IFSelect_WorkSession::DispatchRank
  (const Handle(IFSelect_Dispatch)& disp) const
{
  if (ItemIdent(disp) == 0) return 0;
  return theshareout->DispatchRank(disp);
}

//  ######################################################################
//  ....                        Les MODIFIERS                         ....

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::SetModelCopier (const Handle(IFSelect_ModelCopier)& copier)
{
  thecopier = copier;
  thecopier->SetShareOut(theshareout);
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer IFSelect_WorkSession::NbFinalModifiers (const Standard_Boolean formodel) const 
{
  return theshareout->NbModifiers(formodel);
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfInteger) IFSelect_WorkSession::FinalModifierIdents
				   (const Standard_Boolean formodel) const 
{
//  return ItemIdents(STANDARD_TYPE(IFSelect_Modifier));
//  On donne la liste dans l ordre du ModelCopier, qui fait foi
  Handle(TColStd_HSequenceOfInteger) list = new TColStd_HSequenceOfInteger();
  Standard_Integer nbm = theshareout->NbModifiers(formodel);
  for (Standard_Integer i = 1; i <= nbm; i ++)
    list->Append(ItemIdent(theshareout->GeneralModifier(formodel,i)));
  return list;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(IFSelect_GeneralModifier) IFSelect_WorkSession::GeneralModifier (const Standard_Integer id) const
{
  return GetCasted(IFSelect_GeneralModifier,Item(id));
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(IFSelect_Modifier) IFSelect_WorkSession::ModelModifier (const Standard_Integer id) const 
{
  return GetCasted(IFSelect_Modifier,Item(id));
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer IFSelect_WorkSession::ModifierRank (const Handle(IFSelect_GeneralModifier)& modif) const
{
  if (ItemIdent(modif) == 0) return 0;
  return theshareout->ModifierRank(modif);
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_WorkSession::ChangeModifierRank
  (const Standard_Boolean formodel,
   const Standard_Integer before, const Standard_Integer after)
{
  return theshareout->ChangeModifierRank(formodel,before,after);
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::ClearFinalModifiers ()
{
  Handle(TColStd_HSequenceOfInteger) list = FinalModifierIdents (Standard_True);
  Standard_Integer nb = list->Length();
  Standard_Integer i; // svv #1
  for (i = 1; i <= nb; i ++)
    RemoveItem(GeneralModifier(list->Value(i)));
  list = FinalModifierIdents (Standard_False);
  nb = list->Length();
  for (i = 1; i <= nb; i ++)
    RemoveItem(GeneralModifier(list->Value(i)));
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_WorkSession::SetAppliedModifier
  (const Handle(IFSelect_GeneralModifier)& modif,
   const Handle(Standard_Transient)& item)
{
  if (ItemIdent(modif) == 0) return Standard_False;

  if (item.IsNull()) return Standard_False;
  if (item == theshareout) {
    theshareout->AddModifier(modif,0);
    return Standard_True;
  }
  if (item->IsKind(STANDARD_TYPE(IFSelect_Dispatch))) {
    DeclareAndCast(IFSelect_Dispatch,disp,item);
    theshareout->AddModifier(modif,0);
    modif->SetDispatch(disp);
    return Standard_True;
  }
  if (item->IsKind(STANDARD_TYPE(IFSelect_TransformStandard))) {
    DeclareAndCast(IFSelect_TransformStandard,stf,item);
    DeclareAndCast(IFSelect_Modifier,tmod,modif);
    if (tmod.IsNull()) return Standard_False;
    stf->AddModifier (tmod);
    theshareout->RemoveItem(modif);
    return Standard_True;
  }
  return Standard_False;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_WorkSession::ResetAppliedModifier
  (const Handle(IFSelect_GeneralModifier)& modif)
{
  if (ItemIdent(modif) == 0) return Standard_False;

  return theshareout->RemoveItem(modif);
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(Standard_Transient) IFSelect_WorkSession::UsesAppliedModifier
  (const Handle(IFSelect_GeneralModifier)& modif) const
{
  Handle(Standard_Transient) res;
  if (ItemIdent(modif) == 0) return res;
  if (theshareout->ModifierRank(modif) == 0) return res;
  res = modif->Dispatch();
  if (res.IsNull()) res = theshareout;
  return res;
}

//  #################################################################
//  ....                       Transformer                       ....

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(IFSelect_Transformer) IFSelect_WorkSession::Transformer (const Standard_Integer id) const
{
  return GetCasted(IFSelect_Transformer,Item(id));
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer IFSelect_WorkSession::RunTransformer
  (const Handle(IFSelect_Transformer)& transf)
{
  Standard_Integer effect = 0;
  if (transf.IsNull() || !IsLoaded()) return effect;
  Handle(Interface_InterfaceModel) newmod;    // Null au depart
  Interface_CheckIterator checks;
  checks.SetName("X-STEP WorkSession : RunTransformer");
  Standard_Boolean res = transf->Perform
    (thegraph->Graph(),theprotocol,checks,newmod);

  if (!checks.IsEmpty(Standard_False)) {
    Handle(Message_Messenger) sout = Message::DefaultMessenger();
    sout<<"  **    RunTransformer has produced Check Messages :    **"<<endl;
    checks.Print (sout,myModel,Standard_False);
  }
  thecheckdone = Standard_False;
  thecheckrun  = checks;

  if (newmod.IsNull()) return (res ? 1 : -1);
//  MISE A JOUR des SelectPointed
  Handle(TColStd_HSequenceOfInteger) list =
    ItemIdents(STANDARD_TYPE(IFSelect_SelectPointed));
  Standard_Integer nb = list->Length();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    DeclareAndCast(IFSelect_SelectPointed,sp,Item(list->Value(i)));
    sp->Update(transf);
  }
  if (newmod == myModel) {
    effect = (res ? 2 : -2);
    if (!res) return effect;
    Handle(Interface_Protocol) newproto = theprotocol;
    if (transf->ChangeProtocol(newproto))
      {  effect = 4;  theprotocol = newproto;  thegtool->SetProtocol(newproto);  }
    return (ComputeGraph(Standard_True) ? 4 : -4);
  } else {
    effect = (res ? 3 : -3);
    if (!res) return effect;
    Handle(Interface_Protocol) newproto = theprotocol;
    if (transf->ChangeProtocol(newproto))
      {  effect = 5;  theprotocol = newproto;  thegtool->SetProtocol(newproto);  }
    theoldel = myModel;
    SetModel(newmod,Standard_False);
  }
  return effect;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer IFSelect_WorkSession::RunModifier
  (const Handle(IFSelect_Modifier)& modif, const Standard_Boolean copy)
{
  Handle(IFSelect_Selection) sel;  // null
  return RunModifierSelected (modif,sel,copy);
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer IFSelect_WorkSession::RunModifierSelected
  (const Handle(IFSelect_Modifier)& modif,
   const Handle(IFSelect_Selection)& sel,  const Standard_Boolean copy)
{
  if (ItemIdent(modif) == 0) return Standard_False;
  Handle(IFSelect_TransformStandard) stf = new IFSelect_TransformStandard;
  stf->SetCopyOption(copy);
  stf->SetSelection (sel);
  stf->AddModifier (modif);
  return RunTransformer (stf);
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(IFSelect_Transformer) IFSelect_WorkSession::NewTransformStandard
  (const Standard_Boolean copy, const Standard_CString name)
{
  Handle(IFSelect_TransformStandard) stf = new IFSelect_TransformStandard;
  stf->SetCopyOption(copy);
  if (AddNamedItem (name, stf) == 0) stf.Nullify();
  return stf;
}


//    Ceci est une action directe : pourrait etre fait par un Transformer ...
//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_WorkSession::SetModelContent
  (const Handle(IFSelect_Selection)& sel, const Standard_Boolean keep)
{
  if (sel.IsNull() || !IsLoaded()) return Standard_False;
  Interface_EntityIterator list = sel->UniqueResult(thegraph->Graph());
  if (list.NbEntities() == 0) return Standard_False;

  Handle(Interface_InterfaceModel) newmod  = myModel->NewEmptyModel();
  Interface_CopyTool TC(myModel,theprotocol);
  Standard_Integer i, nb = myModel->NbEntities();
  if (keep) {
    for (list.Start(); list.More(); list.Next())
      TC.TransferEntity (list.Value());
  } else {
    Standard_Integer* flags = new Standard_Integer[nb+1];
    for (i = 0; i <= nb; i ++) flags[i] = 0;
    for (list.Start(); list.More(); list.Next()) {
      Standard_Integer num = myModel->Number(list.Value());
      if (num <= nb) flags[num] = 1;
    }
    for (i = 1; i <= nb; i ++) {
      if (flags[i] == 0) TC.TransferEntity (myModel->Value(i));
    }
    delete [] flags;
  }
  TC.FillModel(newmod);
  if (newmod->NbEntities() == 0) return Standard_False;
//    Mettre a jour (ne pas oublier SelectPointed)
  theoldel = myModel;
  SetModel(newmod,Standard_False);
//  MISE A JOUR des SelectPointed
  Handle(TColStd_HSequenceOfInteger) pts =
    ItemIdents(STANDARD_TYPE(IFSelect_SelectPointed));
  nb = pts->Length();
  for (i = 1; i <= nb; i ++) {
    DeclareAndCast(IFSelect_SelectPointed,sp,Item(pts->Value(i)));
    sp->Update(TC.Control());
  }
  return Standard_True;
}


//  #################################################################
//  ....                        File Name                        ....

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) IFSelect_WorkSession::FilePrefix () const
{
  return theshareout->Prefix();
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) IFSelect_WorkSession::DefaultFileRoot () const
{
  return theshareout->DefaultRootName();
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) IFSelect_WorkSession::FileExtension () const
{
  return theshareout->Extension();
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) IFSelect_WorkSession::FileRoot
  (const Handle(IFSelect_Dispatch)& disp) const
{
  return theshareout->RootName(theshareout->DispatchRank(disp));
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::SetFilePrefix (const Standard_CString name)
{
  theshareout->SetPrefix (new TCollection_HAsciiString(name));
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::SetFileExtension (const Standard_CString name)
{
  theshareout->SetExtension (new TCollection_HAsciiString(name));
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean  IFSelect_WorkSession::SetDefaultFileRoot
  (const Standard_CString name)
{
  Handle(TCollection_HAsciiString) defrt;
  if (name[0] != '\0') defrt = new TCollection_HAsciiString(name);
  return theshareout->SetDefaultRootName (defrt);
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_WorkSession::SetFileRoot
  (const Handle(IFSelect_Dispatch)& disp, const Standard_CString namefile)
{
  Standard_Integer id = ItemIdent(disp);
  if (id == 0) return Standard_False;
  Standard_Integer nd = theshareout->DispatchRank(disp);
/*  if (theonlynamed) {
    if      (nd == 0 && namefile[0] != 0)
      theshareout->AddDispatch(disp);
    else if (nd != 0 && namefile[0] == 0)
      theshareout->RemoveDispatch (nd);
  }  */
  if (nd == 0) return Standard_False;
//  The order below prevented to change the root name on a given dispatch !
//  if (theshareout->HasRootName(nd)) return Standard_False;
  Handle(TCollection_HAsciiString) filename;
  if (namefile[0] != '\0') filename = new TCollection_HAsciiString (namefile);
  return theshareout->SetRootName(nd,filename);
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_CString IFSelect_WorkSession::GiveFileRoot
  (const Standard_CString file) const
{
  OSD_Path path (file);
  if (!path.IsValid(TCollection_AsciiString(file))) return file; // tant pis ..
  bufstr = path.Name();
  return bufstr.ToCString();
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_CString IFSelect_WorkSession::GiveFileComplete
  (const Standard_CString file) const
{
//  ajouter si besoin : Prefix; Extension
  bufstr.Clear(); bufstr.AssignCat (file);
  Standard_Integer i,j = 0,nb = bufstr.Length();
  Handle(TCollection_HAsciiString) ext = FileExtension ();
  if (!ext.IsNull()) {
    char val0 = '\0';  if (ext->Length() > 0) val0 = ext->Value(1);
    for (i = nb; i > 0; i --)  if (bufstr.Value(i) == val0) { j = 1; break; }
    if (j == 0) bufstr.AssignCat (ext->ToCString());
  }
  Handle(TCollection_HAsciiString) pre = FilePrefix ();
  if (!pre.IsNull()) {
    char val1 = '\0';  if (pre->Length() > 0) val1 = pre->Value(pre->Length());
    j = 0;
    for (i = nb; i > 0; i --)  if (bufstr.Value(i) == val1) { j = 1; break; }
    if (j == 0) bufstr.Insert (1,pre->ToCString());
  }

  return bufstr.ToCString();
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::ClearFile ()
{
  thecopier->ClearResult();
  theshareout->ClearResult(Standard_True);
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::EvaluateFile ()
{
////...
  if (!IsLoaded()) return;
  Interface_CheckIterator checks;
  if (errhand) {
    errhand = Standard_False;
    try {
      OCC_CATCH_SIGNALS
      EvaluateFile();    // appel normal (donc, code pas duplique)
    }
    catch (Standard_Failure const& anException) {
      Handle(Message_Messenger) sout = Message::DefaultMessenger();
      sout<<"    ****    Interruption EvaluateFile par Exception :   ****\n";
      sout<<anException.GetMessageString();
      sout<<"\n    Abandon"<<endl;
      checks.CCheck(0)->AddFail ("Exception Raised -> Abandon");
    }
    errhand = theerrhand;
    thecheckrun = checks;
    return;
  }

  IFSelect_ShareOutResult R(theshareout,thegraph->Graph());
  checks = thecopier->Copy (R,thelibrary,theprotocol);
  if (!checks.IsEmpty(Standard_False)) {
    Handle(Message_Messenger) sout = Message::DefaultMessenger();
    sout<<"  **    EvaluateFile has produced Check Messages :    **"<<endl;
    checks.Print (sout,myModel,Standard_False);
  } 
  thecopier->SetRemaining (thegraph->CGraph());
  thecheckrun = checks;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer IFSelect_WorkSession::NbFiles () const 
{
  return thecopier->NbFiles();
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(Interface_InterfaceModel) IFSelect_WorkSession::FileModel
  (const Standard_Integer num) const 
{
  Handle(Interface_InterfaceModel) mod;
  if (num > 0 && num <= NbFiles()) mod = thecopier->FileModel(num);
  return mod;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

TCollection_AsciiString IFSelect_WorkSession::FileName
  (const Standard_Integer num) const 
{
  TCollection_AsciiString name;
  if (num > 0 && num <= NbFiles()) name = thecopier->FileName(num);
  return name;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::BeginSentFiles (const Standard_Boolean record)
{
  thecopier->BeginSentFiles(theshareout,record);
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfHAsciiString) IFSelect_WorkSession::SentFiles () const
{
  return thecopier->SentFiles();
}


//  #########################################################################
//  ....    Action de Transfert proprement dite : la grande affaire !    ....

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_WorkSession::SendSplit ()
{
////...
  Interface_CheckIterator checks;

  if (errhand) {
    errhand = Standard_False;
    try {
      OCC_CATCH_SIGNALS
      return SendSplit();   // appel normal (donc, code pas duplique)
    }
    catch (Standard_Failure const& anException) {
      Handle(Message_Messenger) sout = Message::DefaultMessenger();
      sout<<"    ****    Interruption SendSplit par Exception :   ****\n";
      sout<<anException.GetMessageString();
      sout<<"\n    Abandon"<<endl;
      checks.CCheck(0)->AddFail ("Exception Raised -> Abandon");
    }
    errhand = theerrhand;
    thecheckrun = checks;
    return Standard_False;
  }

  if (thelibrary.IsNull()) {
    checks.CCheck(0)->AddFail("WorkLibrary undefined");
    thecheckrun = checks;
    return Standard_False;
  }
  if (!IsLoaded()) {
    Handle(Message_Messenger) sout = Message::DefaultMessenger();
    sout<< " ***  Data for SendSplit not available  ***"<<endl;
    checks.CCheck(0)->AddFail("Data not available");
    thecheckrun = checks;
    return Standard_False;
  }

  if (NbFiles() > 0) checks = thecopier->SendCopied (thelibrary,theprotocol);
  else {
    /*
    IFSelect_ShareOutResult eval (ShareOut(), thegraph->Graph());
    checks = thecopier->Send (eval, thelibrary, theprotocol);
    thecopier->SetRemaining (thegraph->CGraph());
    */
//  Decomposer
    if (theshareout.IsNull()) return Standard_False;
    Standard_Integer i, nbd = theshareout->NbDispatches();
    Standard_Integer nf = 0;
    Handle(Message_Messenger) sout = Message::DefaultMessenger();
    sout<<" SendSplit .. ";
    for (i = 1; i <= nbd; i ++) {
      Handle(IFSelect_Dispatch) disp = theshareout->Dispatch(i);
      if (disp.IsNull()) continue;
      IFGraph_SubPartsIterator packs(thegraph->Graph(),Standard_False);
      disp->Packets (thegraph->Graph(),packs);
      for (packs.Start(); packs.More(); packs.Next()) {
	Interface_EntityIterator iter = packs.Entities();
	if (iter.NbEntities() == 0) continue;
//  Ecrire une liste d entites
	Handle(IFSelect_SelectPointed) sp = new IFSelect_SelectPointed;
	sp->SetList (iter.Content());
	nf ++;
	TCollection_AsciiString filnam (nf);
	filnam.Insert (1,"_");
	Handle(TCollection_HAsciiString) filepart;
	filepart = FileRoot(disp);
	if (!filepart.IsNull()) filnam.Insert(1,filepart->ToCString());
	filepart = FilePrefix();
	if (!filepart.IsNull()) filnam.Insert(1,filepart->ToCString());
	filepart = FileExtension();
	if (!filepart.IsNull()) filnam.AssignCat (filepart->ToCString());
	IFSelect_ReturnStatus stat = SendSelected (filnam.ToCString(),sp);
	if (stat != IFSelect_RetDone) cout<<"File "<<filnam<<" failed"<<endl;
      }
    }
    sout<<" .. Files Written : "<<nf<<endl;
  }
  thecheckrun = checks;
  return Standard_True;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(IFSelect_PacketList) IFSelect_WorkSession::EvalSplit () const
{
  Handle(IFSelect_PacketList) pks;
  if (!IsLoaded()) return pks;
  IFSelect_ShareOutResult sho (ShareOut(), thegraph->Graph());
  return sho.Packets();
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Interface_EntityIterator IFSelect_WorkSession::SentList
  (const Standard_Integer newcount) const
{
  Interface_EntityIterator iter;
  if (!IsLoaded()) return iter;
  const Interface_Graph& G = thegraph->Graph();
  Standard_Integer nb = G.Size();
  Standard_Integer i;
  for ( i = 1; i <= nb; i ++) {
    Standard_Integer stat = G.Status(i);
    if ( (stat > 0 && newcount < 0) || stat == newcount)
      iter.GetOneItem(G.Entity(i));
  }
  return iter;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer IFSelect_WorkSession::MaxSendingCount () const
{
  Standard_Integer newcount = 0;
  if (!IsLoaded()) return newcount;
  const Interface_Graph& G = thegraph->Graph();
  Standard_Integer nb = G.Size();
  Standard_Integer  i;
  for (i = 1; i <= nb; i ++) {
    Standard_Integer stat = G.Status(i);
    if (stat > newcount) newcount = stat;
  }
  return newcount;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_WorkSession::SetRemaining
  (const IFSelect_RemainMode mode)
{
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (!IsLoaded()) return Standard_False;
  if (mode == IFSelect_RemainForget) {
    Standard_Integer nb = thegraph->Graph().Size();
    for (Standard_Integer i = 1; i <= nb; i ++)
      thegraph->CGraph().SetStatus (i,0);
    theoldel.Nullify();
    return Standard_True;
  } else if (mode == IFSelect_RemainCompute) {
    Handle(Interface_InterfaceModel) newmod;
    Interface_CopyTool TC(myModel,theprotocol);
    thecopier->CopiedRemaining (thegraph->Graph(),thelibrary,TC,newmod);
    if (newmod.IsNull()) {
      sout<<" No Remaining Data recorded"<<endl;  return Standard_False;
    } else if (newmod == myModel) {
      sout<<" Remaining causes all original data to be kept"<<endl;
      thecopier->SetRemaining (thegraph->CGraph());
      return Standard_False;
    } else {
      theoldel = myModel;
      SetModel(newmod,Standard_False);
//  MISE A JOUR des SelectPointed
      Handle(TColStd_HSequenceOfInteger) list =
	ItemIdents(STANDARD_TYPE(IFSelect_SelectPointed));
      Standard_Integer nb = list->Length();
      for (Standard_Integer i = 1; i <= nb; i ++) {
	DeclareAndCast(IFSelect_SelectPointed,sp,Item(list->Value(i)));
	sp->Update(TC.Control());
      }
      return Standard_True;
    }
  } else if (mode == IFSelect_RemainDisplay) {
    Standard_Integer ne = 0;
    Standard_Integer nb = thegraph->Graph().Size();
    for (Standard_Integer i = 1; i <= nb; i ++)
      {  if (thegraph->Graph().Status(i) >= 0) ne ++;  }
    if (ne == 0) {
      sout<<" - All entities are remaining, none yet sent"<<endl;  return Standard_True;
    }
    Interface_EntityIterator iter = SentList(0);
    nb = iter.NbEntities();
    if (nb == 0) {
      sout<<" - No recorded remaining entities"<<endl;  return Standard_True;
    }
    sout <<" --  Recorded Remaining (not yet sent) Entities  --"<<endl;
    ListEntities(iter,2);
    sout << " -- Maximum Sending Count (i.e. duplication in files) "<<
      MaxSendingCount() << endl;

/*
    sout<< " - Now, dispatches are deactivated"<<endl;
    nb = theshareout->NbDispatches();
    for (Standard_Integer i = nb; i > theshareout->LastRun(); i --)
      theshareout->RemoveDispatch(i);
*/
    return Standard_True;
  } else if (mode == IFSelect_RemainUndo) {
    if (theoldel.IsNull()) return Standard_False;
    SetModel(theoldel);  theoldel.Nullify();
    return Standard_True;
  }
  else return Standard_False;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

IFSelect_ReturnStatus IFSelect_WorkSession::SendAll
  (const Standard_CString filename, const Standard_Boolean computegraph)
{
////...
  Interface_CheckIterator checks;
  if (!IsLoaded()) return IFSelect_RetVoid;
  if (thelibrary.IsNull()) {
    checks.CCheck(0)->AddFail("WorkLibrary undefined");
    thecheckrun = checks;
    return IFSelect_RetError;
  }

  if (errhand) {
    errhand = Standard_False;
    try {
      OCC_CATCH_SIGNALS
      ComputeGraph(computegraph);
      checks = thecopier->SendAll(filename,thegraph->Graph(),thelibrary,theprotocol);
    }
    catch (Standard_Failure const& anException) {
      Handle(Message_Messenger) sout = Message::DefaultMessenger();
      sout<<"    ****    Interruption SendAll par Exception :   ****\n";
      sout<<anException.GetMessageString();
      sout<<"\n    Abandon"<<endl;
      errhand = theerrhand;
      checks.CCheck(0)->AddFail ("Exception Raised -> Abandon");
      thecheckrun = checks;
      return IFSelect_RetFail;
    }
  }
  else checks = thecopier->SendAll(filename,thegraph->Graph(),thelibrary,theprotocol);
  Handle(Interface_Check) aMainFail = checks.CCheck(0);
  if (!aMainFail.IsNull() && aMainFail->HasFailed ())
  {
    return IFSelect_RetStop;
  }
  if (theloaded.Length() == 0) theloaded.AssignCat(filename);
  thecheckrun = checks;
  if (checks.IsEmpty(Standard_True)) return IFSelect_RetDone;
  return IFSelect_RetError;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

IFSelect_ReturnStatus IFSelect_WorkSession::SendSelected
  (const Standard_CString filename,
   const Handle(IFSelect_Selection)& sel,
   const Standard_Boolean computegraph)
{
////...
  if (!IsLoaded()) return IFSelect_RetVoid;
  Interface_CheckIterator checks;
  if (thelibrary.IsNull()) {
    checks.CCheck(0)->AddFail("WorkLibrary undefined");
    thecheckrun = checks;
    return IFSelect_RetVoid;
  }

  if (errhand) {
    errhand = Standard_False;
    try {
      OCC_CATCH_SIGNALS
      ComputeGraph(computegraph);
      return SendSelected (filename,sel);    // appel normal
    }
    catch (Standard_Failure const& anException) {
      Handle(Message_Messenger) sout = Message::DefaultMessenger();
      sout<<"    ****    Interruption SendSelected par Exception :   ****\n";
      sout<<anException.GetMessageString();
      sout<<"\n    Abandon"<<endl;
      checks.CCheck(0)->AddFail ("Exception Raised -> Abandon");
      errhand = theerrhand;
      thecheckrun = checks;
      return IFSelect_RetFail;
    }
  }
//  if (ItemIdent(sel) == 0) return 3;
  Interface_EntityIterator iter = sel->UniqueResult(thegraph->Graph());
  if (iter.NbEntities() == 0) return IFSelect_RetVoid;

  checks = thecopier->SendSelected
      (filename,thegraph->Graph(),thelibrary,theprotocol,iter);
  thecopier->SetRemaining (thegraph->CGraph());
  thecheckrun = checks;
  if (checks.IsEmpty(Standard_True)) return IFSelect_RetDone;
  return IFSelect_RetError;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

IFSelect_ReturnStatus IFSelect_WorkSession::WriteFile
  (const Standard_CString filename)
{
  if (WorkLibrary().IsNull()) return IFSelect_RetVoid;
  ComputeGraph(Standard_True);
  if (!IsLoaded()) return IFSelect_RetVoid;
  return SendAll (filename);
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

IFSelect_ReturnStatus IFSelect_WorkSession::WriteFile
  (const Standard_CString filename, const Handle(IFSelect_Selection)& sel)
{
  if (WorkLibrary().IsNull() || sel.IsNull()) return IFSelect_RetVoid;
  ComputeGraph(Standard_True);
  if (!IsLoaded()) return IFSelect_RetVoid;
  return SendSelected (filename,sel);
}

//  ################################################################
//  ....        Actions particulieres sur les Selections        ....

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer IFSelect_WorkSession::NbSources
  (const Handle(IFSelect_Selection)& sel) const
{
  if (ItemIdent(sel) == 0) return 0;
  if (sel->IsKind(STANDARD_TYPE(IFSelect_SelectExtract)) ||
      sel->IsKind(STANDARD_TYPE(IFSelect_SelectDeduct))  )  return 1;
  if (sel->IsKind(STANDARD_TYPE(IFSelect_SelectControl)) )  return 2;
  if (sel->IsKind(STANDARD_TYPE(IFSelect_SelectCombine)) )  return
    GetCasted(IFSelect_SelectCombine,sel)->NbInputs();
  return 0;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(IFSelect_Selection) IFSelect_WorkSession::Source
  (const Handle(IFSelect_Selection)& sel, const Standard_Integer num) const
{
  Handle(IFSelect_Selection) sr;
  if (ItemIdent(sel) == 0) return sr;
  if      (sel->IsKind(STANDARD_TYPE(IFSelect_SelectExtract)) )  sr =
    GetCasted(IFSelect_SelectExtract,sel)->Input();
  else if (sel->IsKind(STANDARD_TYPE(IFSelect_SelectDeduct))  )  sr =
    GetCasted(IFSelect_SelectDeduct,sel)->Input();
  else if (sel->IsKind(STANDARD_TYPE(IFSelect_SelectControl)) )  {
    if      (num == 1) sr = GetCasted(IFSelect_SelectControl,sel)->MainInput();
    else if (num == 2) sr = GetCasted(IFSelect_SelectControl,sel)->SecondInput();
  }
  else if (sel->IsKind(STANDARD_TYPE(IFSelect_SelectCombine)) )  sr =
    GetCasted(IFSelect_SelectCombine,sel)->Input(num);
  return sr;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_WorkSession::IsReversedSelectExtract
  (const Handle(IFSelect_Selection)& sel) const
{
  if (ItemIdent(sel) == 0) return Standard_False;
  DeclareAndCast(IFSelect_SelectExtract,sxt,sel);
  if (sxt.IsNull()) return Standard_False;
  return (!sxt->IsDirect());
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_WorkSession::ToggleSelectExtract
  (const Handle(IFSelect_Selection)& sel)
{
  if (ItemIdent(sel) == 0) return Standard_False;
  DeclareAndCast(IFSelect_SelectExtract,sxt,sel);
  if (sxt.IsNull()) return Standard_False;
  sxt->SetDirect(!sxt->IsDirect());
  return Standard_True;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_WorkSession::SetInputSelection
  (const Handle(IFSelect_Selection)& sel,
   const Handle(IFSelect_Selection)& inp)
{
  if (ItemIdent(sel) == 0) return Standard_False;
  if (!inp.IsNull() && ItemIdent(inp) == 0) return Standard_False;
  DeclareAndCast(IFSelect_SelectExtract,sxt,sel);
  if (!sxt.IsNull()) { sxt->SetInput(inp);  return Standard_True; }
  DeclareAndCast(IFSelect_SelectDeduct,sdt,sel);
  if (!sdt.IsNull()) { sdt->SetInput(inp);  return Standard_True; }
  return Standard_False;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_WorkSession::SetControl
  (const Handle(IFSelect_Selection)& sel,
   const Handle(IFSelect_Selection)& sc,
   const Standard_Boolean formain)
{
  DeclareAndCast(IFSelect_SelectControl,dsel,sel);
  if (ItemIdent(dsel) == 0) return Standard_False;
  if (ItemIdent(sc)   == 0) return Standard_False;
  if (formain) dsel->SetMainInput   (sc);
  else         dsel->SetSecondInput (sc);
  return Standard_True;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer IFSelect_WorkSession::CombineAdd
  (const Handle(IFSelect_Selection)& sel,
   const Handle(IFSelect_Selection)& seladd,
   const Standard_Integer atnum)
{
  DeclareAndCast(IFSelect_SelectCombine,csel,sel);
  if (ItemIdent(csel) == 0) return 0;
  if (ItemIdent(seladd) == 0) return 0;
  csel->Add (seladd,atnum);
  return csel->NbInputs();
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_WorkSession::CombineRemove
  (const Handle(IFSelect_Selection)& selcomb,
   const Handle(IFSelect_Selection)& selrem)
{
  DeclareAndCast(IFSelect_SelectCombine,csel,selcomb);
  if (ItemIdent(csel) == 0) return Standard_False;
  if (ItemIdent(selrem) == 0) return Standard_False;
  Standard_Integer nb = csel->NbInputs();
  for (Standard_Integer i = nb; i > 0; i --) {
    if (csel->Input(i) == selrem) {
      csel->Remove(i);
      return Standard_True;
    }
  }
  return Standard_True;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(IFSelect_Selection) IFSelect_WorkSession::NewSelectPointed
  (const Handle(TColStd_HSequenceOfTransient)& list,
   const Standard_CString name)
{
  Handle(IFSelect_SelectPointed) sel = new IFSelect_SelectPointed;
  if (!list.IsNull()) sel->AddList (list);
  if (AddNamedItem (name,sel) == 0) sel.Nullify();
  return sel;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_WorkSession::SetSelectPointed
  (const Handle(IFSelect_Selection)& sel,
   const Handle(TColStd_HSequenceOfTransient)& list,
   const Standard_Integer mode) const
{
  DeclareAndCast(IFSelect_SelectPointed,sp,sel);
  if (sp.IsNull() || list.IsNull()) return Standard_False;
  if (mode == 0) sp->Clear();
  if (mode >= 0) sp->AddList(list);
  else sp->RemoveList(list);
  return Standard_True;
}


//  ###########################################################################
//  ....         Analyse d un CheckIterator par rapport a un graphe        ....

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

static void IFSelect_QueryProp (Interface_IntList& list,
				TCollection_AsciiString& ana,
				const Standard_Integer num, const int quoi)
{
  list.SetNumber(num);
  Standard_Integer i, nb = list.Length();
  for (i = 1; i <= nb; i ++) {
    if (i > 1) list.SetNumber(num);  // because recursive call + depth first
    Standard_Integer n = list.Value(i);
//    y a t il lieu de propager ?
//  1 W/place  2 F/place  3 Wprop 4Wprop+W/place  5Wprop+F/place
//  6 Fprop  7 Fprop+W/place  8 Fprop+F/place
    char val = ana.Value(n);
    switch (val) {
      case ' ' : val = (quoi ? '3' : '6');  break;
      case '1' : val = (quoi ? '4' : '7');  break;
      case '2' : val = (quoi ? '5' : '8');  break;
      case '3' : val = (quoi ? ' ' : '6');  break;
      case '4' : val = (quoi ? ' ' : '7');  break;
      case '5' : val = (quoi ? ' ' : '8');  break;
      case '6' : val = ' ';  break;
      case '7' : val = ' ';  break;
      case '8' : val = ' ';  break;
      default  : val = ' ';  break;
    }
    if (val == ' ') continue;
    ana.SetValue(n,val);
    IFSelect_QueryProp (list,ana,n,quoi);
  }
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::QueryCheckList (const Interface_CheckIterator& chl)
{
  if (!IsLoaded()) return;
  Standard_Integer i,nb = myModel->NbEntities();
  thecheckana = TCollection_AsciiString (nb+1,' ');
  for (chl.Start(); chl.More(); chl.Next()) {
    Standard_Integer num = chl.Number();
    const Handle(Interface_Check) ach = chl.Value();
    if (ach->HasFailed())        thecheckana.SetValue(num,'2');
    else if (ach->HasWarnings()) thecheckana.SetValue(num,'1');
  }
//  analyse selon le graphe ... codes : blc = rien
//  1 W/place  2 F/place  3 Wprop 4Wprop+W/place  5Wprop+F/place
//  6 Fprop  7 Fprop+W/place  8 Fprop+F/place
  Interface_IntList list;// = thegraph->Graph().SharingNums(0);
//   deux passes : d abord Warning, puis Fail
  for (i = 1; i <= nb; i ++) {
    char val = thecheckana.Value(i);
    int quoi = -1;
    if (val == '1' || val == '4' || val == '7') quoi = 0;
    if (quoi >= 0) IFSelect_QueryProp (list,thecheckana,i,quoi);
  }
  for (i = 1; i <= nb; i ++) {
    char val = thecheckana.Value(i);
    int quoi = -1;
    if (val == '2' || val == '5' || val == '8') quoi = 1;
    if (quoi >= 0) IFSelect_QueryProp (list,thecheckana,i,quoi);
  }
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer IFSelect_WorkSession::QueryCheckStatus
  (const Handle(Standard_Transient)& ent) const
{
  if (!IsLoaded()) return -1;
  Standard_Integer num = myModel->Number(ent);
  if (num == 0) return -1;
  if (thecheckana.Length() < num) return -1;
  Standard_Character val = thecheckana.Value (num);
//  codes : blc = rien -> 0
//  1 W/place -> 1   2 F/place -> 2
//  3 Wprop -> 10    4 Wprop+W/place -> 11    5 Wprop+F/place -> 12
//  6 Fprop -> 20    7 Fprop+W/place -> 21    8 Fprop+F/place -> 22
  if (val == ' ') return 0;
  if (val == '1') return 1;
  if (val == '2') return 2;
  if (val == '3') return 10;
  if (val == '4') return 11;
  if (val == '5') return 12;
  if (val == '6') return 20;
  if (val == '7') return 21;
  if (val == '8') return 22;
  return 0;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer IFSelect_WorkSession::QueryParent
  (const Handle(Standard_Transient)& entdad,
   const Handle(Standard_Transient)& entson) const
{
  Standard_Integer ndad = StartingNumber(entdad);
  Standard_Integer nson = StartingNumber(entson);
  if (ndad < 1 || nson < 1) return -1;
  if (ndad == nson) return 0;
//  on va calculer : pour chaque pere immediat, de <son>, status avec <dad> + 1
//  nb : pas protege contre les boucles ...
  Handle(TColStd_HSequenceOfTransient) list =
    thegraph->Graph().Sharings(entson).Content();
  if (list.IsNull()) return -1;
  Standard_Integer i, nb = list->Length();
  for (i = 1; i <= nb; i ++) {
    if (list->Value(i) == entdad) return 1;
    Standard_Integer stat = QueryParent ( entdad,list->Value(i) );
    if (stat >= 0) return stat+1;
  }
  return -1;  // not yet implemented ...
}

//  ###########################################################################
//  ....      Dumps et Evaluations, pas faciles a passer en arguments      ....

//  ####    ####    ####    ####    ####    ####    ####    ####    ####
//  ....        DumpShare        ....

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::SetParams
  (const NCollection_Vector<Handle(Standard_Transient)>& params,
   const NCollection_Vector<Standard_Integer>&   uselist)
{
  Standard_Integer i, nbp = params.Length(), nbu = uselist.Length();
  Handle(IFSelect_ParamEditor) editor = new IFSelect_ParamEditor
    (nbp+nbu+50,"Parameter Editor");
  for (i = params.Lower(); i <= params.Upper(); i ++) {
    DeclareAndCast(Interface_TypedValue,val,params.Value(i));
    if (val.IsNull()) continue;
    editor->AddValue(val);
  }
  AddNamedItem("xst-params-edit",editor);
//  Les EditForm
  Handle(IFSelect_EditForm) paramsall = editor->Form(Standard_False);
  AddNamedItem("xst-params-all",paramsall);

//  On attaque les EditForms partielles
  TColStd_SequenceOfInteger listgen,listload,listsend,listsplit,listread,listwrite;
  for (i = uselist.Lower(); i <= uselist.Upper(); i ++) {
    Standard_Integer use = uselist.Value(i);
    switch (use) {
    case 1 : listgen.Append(i);   break;
    case 2 : listread.Append(i);  break;
    case 3 : listsend.Append(i);  break;
    case 4 : listsplit.Append(i); break;
    case 5 : listread.Append(i);  break;
    case 6 : listwrite.Append(i); break;
    default : break;
    }
  }
  Handle(IFSelect_EditForm) paramsgen  = new IFSelect_EditForm
    (editor,listgen ,Standard_False,Standard_True,"General Parameters");
  if (listgen.Length() > 0)   AddNamedItem("xst-params-general",paramsgen);
  Handle(IFSelect_EditForm) paramsload = new IFSelect_EditForm
    (editor,listload,Standard_False,Standard_True,"Loading Parameters");
  if (listload.Length() > 0)  AddNamedItem("xst-params-load",  paramsload);
  Handle(IFSelect_EditForm) paramssend = new IFSelect_EditForm
    (editor,listsend,Standard_False,Standard_True,"Sending Parameters");
  if (listsend.Length() > 0)  AddNamedItem("xst-params-send",  paramssend);
  Handle(IFSelect_EditForm) paramsplit = new IFSelect_EditForm
    (editor,listsplit,Standard_False,Standard_True,"Split Parameters");
  if (listsplit.Length() > 0) AddNamedItem("xst-params-split", paramsplit);
  Handle(IFSelect_EditForm) paramsread = new IFSelect_EditForm
    (editor,listread,Standard_False,Standard_True,"Read(Transfer) Parameters");
  if (listread.Length() > 0)  AddNamedItem("xst-params-read",  paramsread);
  Handle(IFSelect_EditForm) paramswrite = new IFSelect_EditForm
    (editor,listwrite,Standard_False,Standard_True,"Write(Transfer) Parameters");
  if (listwrite.Length() > 0) AddNamedItem("xst-params-write", paramswrite);
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::TraceStatics
  (const Standard_Integer use, const Standard_Integer mode) const
{
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (use > 0) {
    if (mode == 0)   sout<<"******************************************"<<endl;
    if        (use == 1) {
      if (mode == 0) sout<<"*****      General  Parameters       *****"<<endl;
    } else if (use == 2) {
      if (mode == 0) sout<<"*****            Load  File          *****"<<endl;
    } else if (use == 3) {
      if (mode == 0) sout<<"*****            Write File          *****"<<endl;
    } else if (use == 4) {
      if (mode == 0) sout<<"*****            Split File          *****"<<endl;
    } else if (use == 5) {
      if (mode == 0) sout<<"*****        Transfer (Read)         *****"<<endl;
    } else if (use == 6) {
      if (mode == 0) sout<<"*****        Transfer (Write)        *****"<<endl;
    }
    if (mode == 0)   sout<<"******************************************"<<endl<<endl;
  }

//    Echainements particuliers (use > 0)
  if (use == 5) {
    TraceStatics (-2,mode);
    if (mode == 0) sout<<endl;
  } else if (use == 4 || use == 6) {
    TraceStatics (-3,mode);
    if (mode == 0) sout<<endl;
  }

//    Valeurs particulieres
  if (use == 1 || use == -1) {  // General : trace
    if (mode == 0) {
//      sout << "Trace Level   : "<<Message_PrinterOStream::Default()->GetTraceLevel()<<endl;
    }
  } else if (use == 4 || use == -4) {  // Split : Prefix & cie
    if (mode == 0) {
      Handle(TCollection_HAsciiString) str = theshareout->Prefix();
      if (!str.IsNull()) sout << "Prefix        : "<<str->ToCString()<<endl;
      else sout << "Prefix       not Defined" << endl;
      str = theshareout->DefaultRootName();
      if (!str.IsNull()) sout << "Default Root  : "<<str->ToCString()<<endl;
      else sout << "Default Root not Defined" << endl;
      str = theshareout->Extension();
      if (!str.IsNull()) sout << "Extension     : "<<str->ToCString()<<endl;
      else sout << "Extension    not defined" << endl;
    }
  }

//  LISTER  LES  STATICS
//  Passer par les ParamEditor ...

//    Fin
  if (use > 0) {
    if (mode == 0)   sout<<"******************************************"<<endl<<endl;
  }
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::DumpShare () const 
{
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  sout<<"        **********  Definition ShareOut (Complete)  **********"<<endl;

  Handle(TCollection_HAsciiString) str = theshareout->Prefix();
  if (!str.IsNull()) sout << "Prefix       : " << str->ToCString() << endl;
  else sout << "Prefix       not Defined" << endl;
  str = theshareout->DefaultRootName();
  if (!str.IsNull()) sout << "Default Root : " << str->ToCString() << endl;
  else sout << "Default Root not Defined" << endl;
  str = theshareout->Extension();
  if (!str.IsNull()) sout << "Extension    : " << str->ToCString() << endl;
  else sout << "Extension    not defined" << endl;

  Standard_Integer lr = theshareout->LastRun();
  Standard_Integer nb = theshareout->NbDispatches();
  sout << "Nb Dispatches : " << nb <<" (Last Run : " << lr << ") : "<<endl;
  for (Standard_Integer i = 1; i <= nb; i ++) {
    Handle(IFSelect_Dispatch) disp = theshareout->Dispatch(i);
    sout << "Dispatch n0 " << i;
    if (HasName(disp)) sout << "   Name:"<< Name(disp)->ToCString();
    sout << "   Label:" << disp->Label() << endl;
    Handle(IFSelect_Selection) sel = disp->FinalSelection();
    if (sel.IsNull()) sout << "   No Final Selection Defined" << endl;
    else if (HasName(sel)) sout << "   Final Selection : Name:"
      << Name(sel)->ToCString() << "  Label:" << sel->Label() << endl;
    else sout << "   Final Selection : " << sel->Label() << endl;
    if (disp->HasRootName())
      sout<<"   File Root Name : "<<disp->RootName()->ToCString()<<endl;
    else sout<<"   No specific file root name (see Default Root)"<<endl;
  }
  Standard_Integer nbm = theshareout->NbModifiers(Standard_True);
  if (nbm > 0) sout<<
    "  ***   "<<nbm<<" active Model Modifiers : see ListModifiers   ***"<<endl;
  Standard_Integer nbf = theshareout->NbModifiers(Standard_False);
  if (nbf > 0) sout<<
    "  ***   "<<nbf<<" active File  Modifiers : see ListModifiers   ***"<<endl;
  if (nbm+nbf == 0) sout<<"  ***   No active Modifiers   ***"<<endl;
}

//  ####    ####    ####    ####    ####    ####    ####    ####    ####
//  ....        ListItems        ....

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::ListItems (const Standard_CString lab) const 
{
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  sout<< "        **********  Items in Session  **********"<<endl;
  Standard_Integer nb = MaxIdent();
  Handle(TCollection_HAsciiString) str;
  if (lab[0] != '\0') str = new TCollection_HAsciiString (lab);
  for (Standard_Integer i = 1; i <= nb; i ++) {
    const Handle(Standard_Transient)& var = theitems.FindKey(i);
    Handle(TCollection_HAsciiString) label = ItemLabel(i);
    if (label.IsNull()) continue;    //  -> item supprime
    if (!str.IsNull())  {  if (label->Location(str,1,label->Length()) == 0) continue; }
    sout<<"#"<<i;
    if (HasName(var)) sout<<"	- Named : "<<Name(var)->ToCString()<<"	- ";
    else sout<<" - (no name) - ";
    sout<<var->DynamicType()->Name()<<endl<<"    "<<label->ToCString()<<endl;
  }
}

//  ####    ####    ####    ####    ####    ####    ####    ####    ####
//  ....        ListModifiers

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::ListFinalModifiers
  (const Standard_Boolean formodel) const 
{
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  Standard_Integer nb = theshareout->NbModifiers(formodel);
  sout<< "        **********  Modifiers in Session ";
  sout<<(formodel ? "(For Model)" : "(For File)");
  sout<<": "<<nb<<"  **********"<<endl;
  for (Standard_Integer i = 1; i <= nb; i ++) {
    Handle(IFSelect_GeneralModifier) modif =
      theshareout->GeneralModifier(formodel,i);
    if (!modif.IsNull()) sout<<"Modifier n0."<<i<<"	: "<<modif->Label();
    if (HasName(modif)) sout << "	 Named as : " << Name(modif)->ToCString();
    sout<<endl;
  }
}

//  ####    ####    ####    ####    ####    ####    ####    ####    ####
//  ....        DumpSelection        ....

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::DumpSelection
  (const Handle(IFSelect_Selection)& sel) const 
{
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (ItemIdent(sel) == 0) {
    sout << "Selection :  Unknown"<<endl;  //sout<<Handle
    return;
  }
  sout << "        **********  Selection";
  if (HasName(sel)) sout << " , Name : " << Name(sel)->ToCString();
  sout <<"  **********"<<endl;
  sout<< "Label : " << sel->Label() << " . Input(s) : "<< endl;
  Standard_Integer nb = 0;
  IFSelect_SelectionIterator iter; sel->FillIterator(iter);
  for (; iter.More(); iter.Next()) {
    nb ++; 
    Handle(IFSelect_Selection) newsel = iter.Value();
    sout<<" -- "<<newsel->Label()<<endl;
  }
  sout << " Nb Inputs:"<<nb<<endl;
}


//              ##########################################
//              #########    Fonctions complementaires
//              ##########################################

//  ####    ####    ####    ####    ####    ####    ####    ####    ####
//  ....        DumpModel        ....

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(IFSelect_Selection) IFSelect_WorkSession::GiveSelection
  (const Standard_CString selname) const
{
  char nomsel[500];
  Standard_Integer np = -1, nf = -1, nivp = 0;
  for (Standard_Integer n = 0; selname[n] != '\0'; n ++) {
    nomsel[n] = selname[n];  nomsel[n+1] = '\0';
    if (selname[n] == '(') { np = n; nivp ++; }
    if (selname[n] == ')') { nivp --;  if (nivp <= 0) nf = n; }
////    if (selname[n] == ' ') { nb = n; break; }
  }

  Handle(IFSelect_Selection) sel;
  if (np >= 0)
  {
    nomsel[np] = 0;
  }
  if (nf >= 0)
  {
    nomsel[nf] = '\0';
  }
  Handle(Standard_Transient) item = NamedItem(nomsel);

//  Parentheses ? essayer Signature (plus tard : Selection parametree)
//  NB : on compte les niveaux de parentheses (imbrications repercutees)
  if (np > 0 && nf > 0) {
    Handle(IFSelect_SelectSignature) selsign;
    Standard_Integer debsign = np+1;

    DeclareAndCast(IFSelect_Signature,sign,item);
    DeclareAndCast(IFSelect_SignCounter,cnt,item);
    if (!sign.IsNull()) selsign =
      new IFSelect_SelectSignature (sign,&nomsel[debsign],Standard_False);
    else if (!cnt.IsNull()) selsign =
      new IFSelect_SelectSignature (cnt,&nomsel[debsign],Standard_False);
    else {
      cout<<selname<<" : neither Signature nor Counter"<<endl;
      return sel;
    }

    selsign->SetInput (new IFSelect_SelectModelEntities);  // par defaut
    sel = selsign;
  }

  else sel = GetCasted(IFSelect_Selection,item);

  return sel;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfTransient) IFSelect_WorkSession::GiveList
  (const Handle(Standard_Transient)& obj) const
{
//   Deja une liste
  DeclareAndCast(TColStd_HSequenceOfTransient,list,obj);
  if (!list.IsNull()) return list;

//   Rien du tout : retourne rien du tout
  if (obj.IsNull()) return list;

//   Une selection : son resultat (standard)
  DeclareAndCast(IFSelect_Selection,sel,obj);
  if (!sel.IsNull()) {
    Interface_EntityIterator iter = EvalSelection(sel);
    return iter.Content();
  }

//   Le modele : son contenu
  list = new TColStd_HSequenceOfTransient();
  if (obj == myModel) {
    Standard_Integer i, nb = myModel->NbEntities();
    for (i = 1; i <= nb; i ++)  list->Append (myModel->Value(i));
  }

//   Une entite du modele : cette entite
  else if (StartingNumber(obj) > 0) list->Append (obj);

//   Un Texte : son interpretation
  else {
    DeclareAndCast(TCollection_HAsciiString,str,obj);
    if (!str.IsNull()) return GiveList (str->ToCString());
  }

//  Si c est pas tout ca : une liste vide
  return list;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfTransient) IFSelect_WorkSession::GiveList
  (const Standard_CString first, const Standard_CString second) const
{
  Handle(TColStd_HSequenceOfTransient) list; 
  if (!first || first[0] == '\0') return list;
  if (first[0]  == ' ') return GiveList (&first[1],second);
  if (second && second[0] == ' ') return GiveList (first,&second[1]);

//   list NULLE sera interpretee comme SelectionResult (selection toute crue)
//   sinon comme SelectionResultFromList
  if (second && second[0] != '\0') list = GiveList (second,"");

  list = GiveListFromList (first,list);
  return list;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfTransient) IFSelect_WorkSession::GiveListFromList
  (const Standard_CString selname, const Handle(Standard_Transient)& ent) const
{
  Handle(TColStd_HSequenceOfTransient) list;
  Standard_Integer num;

//   LISTE DEFINIE D OFFICE (en ce cas, la liste d entree est ignoree)
  if (selname[0] == '(') {
//  liste d entites donnees a la queue leu leu : (ID,ID,ID...)
    char entid[50];     Standard_Integer i,j = 0;
    TColStd_MapOfInteger numap;
    list = new TColStd_HSequenceOfTransient();
    for (i = 1; selname[i] != '\0'; i ++) {
      if (selname[i] == ' ') continue;
      if (selname[i] == ',' || selname[i] == ')') {
	entid[j] = '\0';
	if (j == 0) continue;
	j = 0;
	num = NumberFromLabel (entid);
	if (num <= 0 || !numap.Add (num)) continue;
	Handle(Standard_Transient) anent = StartingEntity(num);
	if (!anent.IsNull()) list->Append (anent);
	if (selname[i] == ')') break;
	continue;
      }
      entid[j] = selname[i];  j ++;
    }
    return list;
  }
  num = NumberFromLabel (selname);
  if (num > 0)  return GiveList(StartingEntity(num));

//  Autres cas : y atil une liste d entree.
//   Si OUI -> SelectionResultFromList.  Si NON -> SelectionResult
//   Si une entite isolee -> on en fait une liste

  list = GiveList(ent);  // ent NULL -> list NULL sinon intreprete

//    Decomposition term1 term2 ...

  char nomsel[500];  nomsel[0] = '\0';
  Standard_Integer n= 0 , nb = -1;
  for (n = 0; selname[n] != '\0'; n ++) {
    nomsel[n] = selname[n];  nomsel[n+1] = '\0';
//    if (selname[n] == '(') { np = n; nivp ++; }
//    if (selname[n] == ')') { nivp --;  if (nivp <= 0) nf = n; }
    if (selname[n] == ' ') { nb = n; nomsel[n] = '\0'; break; }
  }
  if (nomsel[0] == '\0') return list;

  Handle(IFSelect_Selection) sel = GiveSelection (nomsel);
  if (sel.IsNull())  {
    cout<<"Neither Entity Number/Label nor Selection :"<<nomsel<<endl;
    return list;
  }

  if (nb > 0) list = GiveListFromList (&selname[nb+1],list);

  if (list.IsNull()) list = SelectionResult (sel);
  else               list = SelectionResultFromList (sel,list);

  return list;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfTransient) IFSelect_WorkSession::GiveListCombined
  (const Handle(TColStd_HSequenceOfTransient)& l1,
   const Handle(TColStd_HSequenceOfTransient)& l2,
   const Standard_Integer mode) const
{
  Handle(TColStd_HSequenceOfTransient) list;
  if (l1.IsNull() || l2.IsNull()) return list;

//  mode < 0 l1-l2  = 0 l1&l2  > 0 l1|l2 (l1+l2)
  TColStd_MapOfTransient numap;
  Standard_Integer i,n = l2->Length();
  for (i = n; i > 0; i --)  {
    Handle(Standard_Transient) ent = l2->Value(i);
    if (ent.IsNull()) continue;
    numap.Add (ent);
    if (mode > 0) list->Append(ent);
  }

//  ents de l1 pas deja dans l2
  n = l1->Length();
  for (i = n; i > 0; i --) {
    Handle(Standard_Transient) ent = l1->Value(i);
    if (ent.IsNull()) continue;

    if (numap.Contains(ent)) {
//    dans l1 et dans l2
      if (mode == 0) list->Append(ent);
    } else {
//    dans l1 mais pas dans l2
      if (mode != 0) list->Append(ent);
    }
  }

  list->Reverse();
  return list;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::DumpModel
  (const Standard_Integer level, const Handle(Message_Messenger)& S)
{
  if (!IsLoaded())
    {  S<< " ***  Data for List not available  ***"<<endl;  return;  }
  S << "\n        *****************************************************************\n";
  if (theloaded.Length() > 0)
    S << "        ********  Loaded File : "<<theloaded.ToCString()<<Interface_MSG::Blanks(32-theloaded.Length())<<" ********"<<endl;
  else S << "        ********  No name for Loaded File"<<endl;
  if (level == 0) {
    S<<"        ********  Short Dump of Header                           ********\n";
  S << "        *****************************************************************\n\n";
    myModel->DumpHeader(S);  S<<endl;
  }

  Standard_Integer nbent = myModel->NbEntities();
  Standard_Integer nbr = 0;
  Interface_ShareFlags shar(thegraph->Graph());

  for (Standard_Integer i = 1; i <= nbent; i ++) {
    if (!shar.IsShared(myModel->Value(i))) nbr ++;
  }
  S << "        *****************************************************************\n";
  S << "        ********  Model : "<<nbent<<" Entities, of which "<<nbr<<" Root(s)\n";
  S << "        *****************************************************************\n"<<endl;

  if (level <= 0) return;
  else if (level == 1) {
    S<<"        ********  Root Entities  ********      ";
    ListEntities (shar.RootEntities(),1);
  } else if (level == 2) {
    S<<"        ********  Complete List  ********      ";
    ListEntities (myModel->Entities(),1);
  } else if (level > 2) {
    IFSelect_PrintCount mode = IFSelect_ItemsByEntity;
    if (level == 5 || level ==  8) mode = IFSelect_CountByItem;
    if (level == 6 || level ==  9) mode = IFSelect_ListByItem;
    if (level == 7 || level == 10) mode = IFSelect_EntitiesByItem;
    PrintCheckList (ModelCheckList(),Standard_False, mode);
  } else {
    if (level == 3) S << "        ********  Check Model (Fails)  ********"<<endl;
    else            S << "        ********  Check Model (Complete)  ********"<<endl;
    Interface_CheckTool CT (Graph());
    Interface_CheckIterator C;
    if (theerrhand) {
      try {
        OCC_CATCH_SIGNALS
	if (level == 3) C = CT.CheckList();
	else    C = CT.CompleteCheckList();
      }
      catch (Standard_Failure) {
	Handle(Message_Messenger) sout = Message::DefaultMessenger();
	sout<<"    ****    Interruption DumpModel (Check) par Exception    ****\n";
	S<<"  ** **  Exception Raised during Check !  ** **\n";
	S<<"  -->  what could be determined is listed"<<endl;
      }
    }
    else if (level == 3) C = CT.CheckList();
    else         C = CT.CompleteCheckList();

//  Check List : si vide (pas demandee), naturellement passee
    try {
      OCC_CATCH_SIGNALS
      C.Print(S,myModel, (level == 3));
    }
    catch (Standard_Failure const& anException) {
      Handle(Message_Messenger) sout = Message::DefaultMessenger();
      sout<<"    ****    Interruption DumpModel par Exception :   ****\n";
      sout<<anException.GetMessageString();
      sout<<"\n    Abandon"<<endl;
    }

  }
  S<<endl<<"There are "<<nbent<<" Entities, of which "<<nbr<<" Root(s)"<<endl;
}

//  ....        TraceDumpModel        ....  (Model + CheckList)

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::TraceDumpModel
  (const Standard_Integer mode)
{
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  DumpModel (mode,sout);
//  if      (mode <= 4)  {  DumpModel (mode,sout);  return;  }

//  else if (mode <= 7) PrintCheckList (ModelCheckList(),Standard_False, mode-5);
//  else if (mode <=10) PrintCheckList (ModelCheckList(),Standard_True , mode-8);
}

//  ....        DumpEntity        ....

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::DumpEntity
  (const Handle(Standard_Transient)& ent, const Standard_Integer level,
   const Handle(Message_Messenger)& S) const
{
  if (!IsLoaded())
    {  S<< " ***  Data for List not available  ***"<<endl;  return;  }
  Standard_Integer num = myModel->Number(ent);
  if (num == 0) { S<<" ***  Entity to Dump not in the Model  ***"<<endl; return; }
  if (thelibrary.IsNull()) { S<<" ***  WorkLibrary not defined  ***"<<endl; return; }
  S << "        ********  Dumping Entity n0 "<<num
    <<" level:"<<level<<"  ********"<<endl;
  thelibrary->DumpEntity (myModel,theprotocol,ent,S,level);
}

//  ....        DumpEntity        ....

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::TraceDumpEntity
  (const Handle(Standard_Transient)& ent, const Standard_Integer level) const
{
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  DumpEntity (ent,level,sout);
}

//  ....        PrintEntityStatus        ....

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::PrintEntityStatus
  (const Handle(Standard_Transient)& ent, const Handle(Message_Messenger)& S)
{
  Standard_Integer i,nb;
  Standard_Integer num = StartingNumber(ent);
  if (num == 0)  {  cout<<" --  PrintEntityStatus : unknown"<<endl;  return;  }

  S <<"  Ent. n0/id:   ";
  myModel->Print(ent,S);
  Handle(TCollection_HAsciiString) hname = EntityName(ent);
  if (!hname.IsNull() && hname->Length() > 0) S<<"	Name:"<<hname->ToCString();
  S<<endl;
  Handle(IFSelect_Signature) signtype = SignType();
  if (signtype.IsNull()) S<<"  Type(CDL):"<<ent->DynamicType()->Name()<<endl;
  else S <<"  Type:"<<signtype->Value (ent,myModel)<<endl;
  S <<"    Category : " <<CategoryName (ent)
    <<"    Validity : " <<ValidityName (ent) << endl;
  Interface_CheckIterator chl = CheckOne (ent);
  chl.Print (S,myModel,Standard_False,Standard_False);

  Handle(TColStd_HSequenceOfTransient) list = Sharings(ent);
  if (list.IsNull()) S<<"  Root"<<endl;
  else {
    nb = list->Length();
    if (nb == 0) S<<"  Root";
    else S<<"  Super-entities:"<<nb<<" : (n0/id):";
    for (i = 1; i <= nb; i ++)  {  S<<" "; myModel->Print(list->Value(i),S);  }
    S<<endl;
  }
  list = Shareds (ent);
  if (list.IsNull()) S<<"  No sub-entity"<<endl;
  else {
    nb = list->Length();
    if (nb == 0) S<<"  No sub-entity";
    else S<<"  Sub-entities:"<<nb<<" , i.e. (n0/id):";
    for (i = 1; i <= nb; i ++)  {  S<<" "; myModel->Print(list->Value(i),S);  }
    S<<endl;
  }
}

//  ....        PrintCheckList        ....

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::PrintCheckList
  (const Interface_CheckIterator& checklist,
   const Standard_Boolean failsonly, const IFSelect_PrintCount mode) const
{
//  mode : 0  comptage   1 n0s entites   2 n0s+id ents
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (mode == IFSelect_ItemsByEntity) checklist.Print (sout,myModel,failsonly);
  else {
    Interface_CheckIterator chks = checklist;
    Handle(IFSelect_CheckCounter) counter =
      new IFSelect_CheckCounter (mode>1 && mode != IFSelect_CountSummary);
    counter->Analyse (chks,myModel,Standard_True,failsonly);
    counter->PrintList  (sout,myModel,mode);
  }
}

//  ....        PrintSignatureList        ....

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::PrintSignatureList
  (const Handle(IFSelect_SignatureList)& signlist,
   const IFSelect_PrintCount mode) const
{
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (signlist.IsNull()) return;
  signlist->PrintList (sout,myModel,mode);
}

//  ####    ####    ####    ####    ####    ####    ####    ####    ####
//  ....        EvaluateSelection        ....

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::EvaluateSelection
  (const Handle(IFSelect_Selection)& sel) const 
{
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (errhand) {
    errhand = Standard_False;
    try {
      OCC_CATCH_SIGNALS
      EvaluateSelection(sel);    // appel normal (->code unique)
    }
    catch (Standard_Failure const& anException) {
      sout<<"    ****    Interruption EvaluateSelection par Exception    ****  Intitule\n";
      sout<<anException.GetMessageString();
      sout<<"\n    Abandon"<<endl;
    }
    errhand = theerrhand;
    return;
  }

  if (!IsLoaded())
    {  sout<< " ***  Data for Evaluation not available  ***"<<endl;  return;  }
  if (ItemIdent(sel) == 0)
    {  sout << " Selection :  Unknown"<<endl;  return;  }  //sout<<Handle
  Interface_EntityIterator iter = EvalSelection (sel);
  ListEntities (iter,1);
  sout << "****  (Unique) RootResult, Selection 	: "
    <<sel->Label()<<endl;
}


//  ####    ####    ####    ####    ####    ####    ####    ####    ####
//  ....        EvaluateDispatch        ....

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::EvaluateDispatch
  (const Handle(IFSelect_Dispatch)& disp, const Standard_Integer mode) const 
{
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (errhand) {
    errhand = Standard_False;
    try {
      OCC_CATCH_SIGNALS
      EvaluateDispatch(disp,mode);    // appel normal (->code unique)
    }
    catch (Standard_Failure const& anException) {
      sout<<"    ****    Interruption EvaluateDispatch par Exception    ****  Intitule\n";
      sout<<anException.GetMessageString();
      sout<<"\n    Abandon"<<endl;
    }
    errhand = theerrhand;
    return;
  }

  Standard_Integer numdisp = DispatchRank(disp);
  if (!IsLoaded())
    {  sout<< " ***  Data for List not available  ***"<<endl;  return;  }
  if (theshareout->NbDispatches() < numdisp || numdisp <= 0)
    { sout<<"Dispatch :  Unknown"<<endl; return; } //sout<<Handle
  if (disp->FinalSelection().IsNull())
    { sout<<"Dispatch  : No Final Selection"<<endl; return; }//sout<<Handle
  sout<<" --- Dispatch Label : "<<disp->Label()<<endl; 

  IFSelect_ShareOutResult eval(disp,thegraph->Graph());
  eval.Evaluate();
  Standard_Integer numpack = 0;
  Handle(IFSelect_PacketList) evres =
    eval.Packets (mode ? Standard_True : Standard_False);
  Standard_Integer nbpack = evres->NbPackets();

  sout<<"Nb Packets produced : "<<nbpack<<" :"<<endl;
  for (numpack = 1; numpack <= nbpack; numpack ++) {
    sout<<"\n    ****    Packet n0 : "<<numpack<<" ****"<<endl;
    if (!mode) cout<<"Root Entities :"<<endl;
    ListEntities (evres->Entities(numpack), (mode ? 2 : -1));
  }

////  Interface_EntityIterator iterem = disp->Remainder(thegraph->Graph());
  if (mode == 0) return;
  if (mode == 1 || mode == 3) {
    sout<<endl;
    if (evres->NbDuplicated(0,Standard_False) == 0)
      sout<<"    ****    All the Model is taken into account    ****"<<endl;
    else {
      sout<<"    ****    Starting Entities not taken by this Dispatch    ****"<<endl;
      ListEntities (evres->Duplicated(0,Standard_False),2);
    }
  }
  if (mode >= 2) {
    sout<<"    ****    Entites in more than one packet    ****";
    Standard_Integer max = evres->HighestDuplicationCount();
    if (max < 2) sout<<" :   There are none"<<endl;
    else {
      Standard_Integer newcount;
      sout<<endl;
      for (newcount = 2; newcount <= max; newcount ++) {
	if (evres->NbDuplicated(newcount,Standard_False) == 0) continue;
	sout<<"    ****   Entities put in "<<newcount<<" packets    ****"<<endl;
	ListEntities (evres->Duplicated(newcount,Standard_False),2);
      }
    }
  }
}


//  ####    ####    ####    ####    ####    ####    ####    ####    ####
//  ....        EvaluateComplete        ....

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::EvaluateComplete
  (const Standard_Integer mode) const 
{
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (errhand) {
    errhand = Standard_False;
    try {
      OCC_CATCH_SIGNALS
      EvaluateComplete(mode);    // appel normal (donc, code pas duplique)
    }
    catch (Standard_Failure const& anException) {
      sout<<"    ****    Interruption EvaluateComplete par Exception :   ****\n";
      sout<<anException.GetMessageString();
      sout<<"\n    Abandon"<<endl;
    }
    errhand = theerrhand;
    return;
  }

  if (!IsLoaded())
    {  sout<< " ***  Data for List not available  ***"<<endl;  return;  }
  IFSelect_ShareOutResult eval(theshareout,thegraph->Graph());
  eval.Evaluate();
  sout<<"\n********    Evaluation ShareOutResult (Complete)    ********\n";
  sout<<"    ****    List of Packets    ****  Count : "<<eval.NbPackets()<<endl;
  if (mode == 0) sout << " ** (for each one : Root Entities)  **"<<endl;
  else sout << " ** (for each one : Evaluated Content)  **"<<endl;

  Standard_Integer numpack = 0;
  Handle(IFSelect_PacketList) evres =
    eval.Packets (mode ? Standard_True : Standard_False);
  Standard_Integer nbpack = evres->NbPackets();

  sout<<"Nb Packets produced : "<<nbpack<<" :"<<endl;
  for (numpack = 1; numpack <= nbpack; numpack ++) {
    sout<<"\n    ****    Packet n0 : "<<numpack<<" ****"<<endl;
    if (!mode) cout<<"Root Entities :"<<endl;
    ListEntities (evres->Entities(numpack), (mode ? 2: -1));
  }
  if (mode == 0) return;
  if (mode == 1 || mode == 3) {
    sout<<endl;
    if (evres->NbDuplicated(0,Standard_False) == 0)
      sout<<"    ****    All the Model is taken into account    ****"<<endl;
    else {
      sout<<"    ****    Starting Entities Forgotten    ****"<<endl;
      ListEntities (evres->Duplicated(0,Standard_False),2);
    }
  }
  if (mode >= 2) {
    sout<<"    ****    Entites in more than one packet    ****"<<endl;
    Standard_Integer max = evres->HighestDuplicationCount();
    if (max < 2) sout<<" :   There are none"<<endl;
    else {
      Standard_Integer newcount;
      sout<<endl;
      for (newcount = 2; newcount <= max; newcount ++) {
	if (evres->NbDuplicated(newcount,Standard_False) == 0) continue;
	sout<<"    ****   Entities put in "<<newcount<<" packets    ****"<<endl;
	ListEntities (evres->Duplicated(newcount,Standard_False),2);
      }
    }
  }
}


//  ####    ####    ####    ####    ####    ####    ####    ####    ####
//  ....      Routine Interne : ListEntities

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void IFSelect_WorkSession::ListEntities
  (const Interface_EntityIterator& iter, const Standard_Integer mmode) const 
{
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  int titre = 0;
  Standard_Integer mode = (mmode < 0 ? -mmode : mmode);
  if (mmode >= 0) sout << " List of " << iter.NbEntities() << " Entities :"<<endl;
  if (!IsLoaded())
    {  sout<< " ***  Data for List not available  ***"<<endl;  return;  }
  Interface_ShareFlags tool(thegraph->Graph());

  try {
    OCC_CATCH_SIGNALS
    int newcount = -1; int mods = 0; int cnt = 0;
    for (iter.Start(); iter.More(); iter.Next()) {
      if (!titre && mode == 1) sout
	<< "Number/Id.           Category Validity    Type\n-----------          ----...."<<endl;
//          123456789 123456789 123456  123456789 123456789 123456
      if (!titre && mode == 0) sout<<"  Keys : R Root   ? Unknown   * Unloaded"<<endl;
      if (!titre && mode == 2) sout<<"(";
      titre = 1;
      Handle(Standard_Transient) ent = iter.Value();
      Standard_Integer num = myModel->Number(ent);
      if (mode == 1) {
    // n0 id (root?) category validity tracetype
	sout<<Interface_MSG::Blanks (num,6);
	myModel->Print(ent,sout,0);
	if (!tool.IsShared(ent)) sout << " #ROOT#";
	else                     sout << "       ";
	Standard_Integer catnum = myModel->CategoryNumber(num);
	if (catnum > 0) sout<<"  "<<Interface_Category::Name (catnum);
	sout << "  (" << ValidityName (ent) << ")  ";

	sout<<" Type:"<<myModel->TypeName (ent, Standard_False)<<endl;
      } else if (mode == 2) {
	newcount ++;
	if (newcount > 0) sout<<",";
	sout<<num;
      } else {
	newcount ++;  mods = 0; cnt ++;
	if      (newcount >= 10) { sout << endl<<"["<<cnt<<"]:"; newcount = 1; }
	if (newcount > 0)  sout << "	";
	myModel->Print(ent,sout,0);
	if (!tool.IsShared(ent)) { if(mods == 0) sout<<"("; sout<<"R"; mods++; }
	if (myModel->IsUnknownEntity(num)) { sout<<(mods==0 ? '(' : ' ')<<"?"; mods ++; }
	if (myModel->IsRedefinedContent(num)) { sout<<(mods==0 ? '(' : ' ')<<"*"; mods ++; }
	if (mods) { sout<<")"; newcount ++; }
      }
    }
    if (mode == 0) sout<<endl;
    if (mode == 2) sout<<")"<<endl;
  }
  catch (Standard_Failure const& anException) {
    sout<<"    ****    Interruption ListEntities par Exception :   ****\n";
    sout<<anException.GetMessageString();
    sout<<"\n    Abandon"<<endl;
  }
}
