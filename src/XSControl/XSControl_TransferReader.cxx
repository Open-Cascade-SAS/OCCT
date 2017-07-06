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
#include <Interface_Check.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_HGraph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Interface_MSG.hxx>
#include <Interface_SignLabel.hxx>
#include <Interface_Static.hxx>
#include <Message_Messenger.hxx>
#include <ShapeFix.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TopoDS_HShape.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <Transfer_ActorOfTransientProcess.hxx>
#include <Transfer_IteratorOfProcessForTransient.hxx>
#include <Transfer_ResultFromModel.hxx>
#include <Transfer_ResultFromTransient.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <Transfer_TransferOutput.hxx>
#include <Transfer_TransientProcess.hxx>
#include <TransferBRep.hxx>
#include <TransferBRep_BinderOfShape.hxx>
#include <TransferBRep_ShapeBinder.hxx>
#include <XSControl_Controller.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_Utils.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(XSControl_TransferReader,Standard_Transient)

//=======================================================================
//function : SetController
//purpose  : 
//=======================================================================

void XSControl_TransferReader::SetController(const Handle(XSControl_Controller)& control)
{
  myController = control;
  myActor.Nullify();
  Clear(-1);
}


//=======================================================================
//function : Actor
//purpose  : 
//=======================================================================

Handle(Transfer_ActorOfTransientProcess) XSControl_TransferReader::Actor ()
{
  if ( myActor.IsNull() && !myController.IsNull() && !myModel.IsNull())
    myActor = myController->ActorRead(myModel);
  return myActor;
}


//=======================================================================
//function : SetModel
//purpose  : 
//=======================================================================

void XSControl_TransferReader::SetModel(const Handle(Interface_InterfaceModel)& model)
{
  myModel = model;
  if (!myTP.IsNull()) myTP->SetModel(model);
}


//=======================================================================
//function : SetGraph
//purpose  : 
//=======================================================================

void XSControl_TransferReader::SetGraph(const Handle(Interface_HGraph)& graph)
{
  if (graph.IsNull())
  {
    myModel.Nullify();
  }
  else
    myModel = graph->Graph().Model();

  myGraph = graph;
  
  if (!myTP.IsNull()) myTP->SetGraph(graph);
}


//=======================================================================
//function : SetContext
//purpose  : 
//=======================================================================

void XSControl_TransferReader::SetContext(const Standard_CString name,
                                          const Handle(Standard_Transient)& ctx)
{
  myContext.Bind(name,ctx);
}


//=======================================================================
//function : GetContext
//purpose  : 
//=======================================================================

Standard_Boolean XSControl_TransferReader::GetContext
  (const Standard_CString name, const Handle(Standard_Type)& type,
   Handle(Standard_Transient)& ctx) const
{
  if (myContext.IsEmpty()) return Standard_False;
  if (!myContext.Find(name, ctx))
    ctx.Nullify();
  if (ctx.IsNull()) return Standard_False;
  if (type.IsNull()) return Standard_True;
  if (!ctx->IsKind(type)) ctx.Nullify();
  return !ctx.IsNull();
}


//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void XSControl_TransferReader::Clear (const Standard_Integer mode)
{
  if (mode & 1) {
    myResults.Clear();
    myShapeResult.Nullify();
  }
  if (mode & 2) {
    myModel.Nullify();
    myGraph.Nullify();
    myTP.Nullify();
    myActor.Nullify();
    myFileName.Clear();
  }  
}


//  ########################################################
//  ###########            RESULTATS            ############


//=======================================================================
//function : RecordResult
//purpose  : 
//=======================================================================

Standard_Boolean XSControl_TransferReader::RecordResult
  (const Handle(Standard_Transient)& ent)
{
  if (myModel.IsNull() || myTP.IsNull()) return Standard_False;
  Standard_Integer num = myModel->Number(ent);
  if (num == 0) return Standard_False;
  Handle(TCollection_HAsciiString) lab = myModel->StringLabel(ent);

  Handle(Transfer_ResultFromModel) res = new Transfer_ResultFromModel;
  res->Fill (myTP,ent);

  //   Cas du resultat Shape : pour resultat principal, faire HShape ...
  Handle(Transfer_Binder) binder = res->MainResult()->Binder();
  DeclareAndCast(TransferBRep_ShapeBinder,shb,binder);
  if (!shb.IsNull()) {
    Handle(Transfer_SimpleBinderOfTransient) trb = new Transfer_SimpleBinderOfTransient;
    trb->SetResult ( new TopoDS_HShape(shb->Result()) );
    trb->Merge(binder);
    res->MainResult()->SetBinder (trb);
  }

  res->SetFileName(myFileName.ToCString());
  myResults.Bind(num,res);
  return Standard_True;
}


//=======================================================================
//function : IsRecorded
//purpose  : 
//=======================================================================

Standard_Boolean XSControl_TransferReader::IsRecorded
  (const Handle(Standard_Transient)& ent) const
{
  if (myModel.IsNull()) return Standard_False;
  Standard_Integer num = myModel->Number(ent);
  if (num == 0) return Standard_False;
  if(!myResults.IsBound(num)) return Standard_False;
  return (myResults.Find(num)->DynamicType() == STANDARD_TYPE(Transfer_ResultFromModel) );
}


//=======================================================================
//function : HasResult
//purpose  : 
//=======================================================================

Standard_Boolean  XSControl_TransferReader::HasResult
  (const Handle(Standard_Transient)& ent) const
{
  if (myModel.IsNull()) return Standard_False;
  Standard_Integer num = myModel->Number(ent);
  if (num == 0) return Standard_False;
  if(!myResults.IsBound(num)) return Standard_False;
  DeclareAndCast(Transfer_ResultFromModel,fr,myResults.Find(num));
  if (fr.IsNull()) return Standard_False;
  return fr->HasResult();
}


//=======================================================================
//function : RecordedList
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfTransient) XSControl_TransferReader::RecordedList () const
{
  Handle(TColStd_HSequenceOfTransient) li = new TColStd_HSequenceOfTransient();
  if (myModel.IsNull())   return li;
  Standard_Integer i, nb = myModel->NbEntities();
  for (i = 1; i <= nb; i ++) {
    if(myResults.IsBound(i))
      if(!myResults.Find(i).IsNull()) li->Append (myModel->Value(i));
  }
  return li;
}


//=======================================================================
//function : Skip
//purpose  : 
//=======================================================================

Standard_Boolean XSControl_TransferReader::Skip(const Handle(Standard_Transient)& ent)
{
  if (myModel.IsNull() || myTP.IsNull()) return Standard_False;
  Standard_Integer num = myModel->Number(ent);
  if (num == 0) return Standard_False;
  myResults.Bind(num,ent);
  return Standard_True;
}


//=======================================================================
//function : IsSkipped
//purpose  : 
//=======================================================================

Standard_Boolean XSControl_TransferReader::IsSkipped
  (const Handle(Standard_Transient)& ent) const
{
  if (myModel.IsNull()) return Standard_False;
  Standard_Integer num = myModel->Number(ent);
  if (num == 0) return Standard_False;
  if(!myResults.IsBound(num)) return Standard_False;
  return (myResults.Find(num)->DynamicType() != STANDARD_TYPE(Transfer_ResultFromModel) );
}


//=======================================================================
//function : IsMarked
//purpose  : 
//=======================================================================

Standard_Boolean XSControl_TransferReader::IsMarked
  (const Handle(Standard_Transient)& ent) const
{
  if (myModel.IsNull()) return Standard_False;
  Standard_Integer num = myModel->Number(ent);
  if (num == 0) return Standard_False;
  if(!myResults.IsBound(num)) return Standard_False;
  if (myResults.Find(num).IsNull()) return Standard_False;
  return Standard_True;
}


//  #########    ACCES UN PEU PLUS FIN    #########


//=======================================================================
//function : FinalResult
//purpose  : 
//=======================================================================

Handle(Transfer_ResultFromModel) XSControl_TransferReader::FinalResult
       (const Handle(Standard_Transient)& ent) const
{
  Handle(Transfer_ResultFromModel) res;
  if (myModel.IsNull())   return res;
  Standard_Integer num = myModel->Number(ent);
  if (num == 0) return res;
  if(!myResults.IsBound(num)) return res;
  res = GetCasted(Transfer_ResultFromModel,myResults.Find(num));
  return res;
}


//=======================================================================
//function : FinalEntityLabel
//purpose  : 
//=======================================================================

Standard_CString XSControl_TransferReader::FinalEntityLabel
  (const Handle(Standard_Transient)& ent) const
{
  Handle(Transfer_ResultFromModel) resu = FinalResult (ent);
  if (resu.IsNull()) return "";
  return resu->MainLabel();
}


//=======================================================================
//function : FinalEntityNumber
//purpose  : 
//=======================================================================

Standard_Integer XSControl_TransferReader::FinalEntityNumber
  (const Handle(Standard_Transient)& ent) const
{
  Handle(Transfer_ResultFromModel) resu = FinalResult (ent);
  if (resu.IsNull()) return 0;
  return resu->MainNumber();
}


//=======================================================================
//function : ResultFromNumber
//purpose  : 
//=======================================================================

Handle(Transfer_ResultFromModel) XSControl_TransferReader::ResultFromNumber
  (const Standard_Integer num) const
{
  Handle(Transfer_ResultFromModel) res;
  if ( num<1 || num>myModel->NbEntities() ) return res;
  if(!myResults.IsBound(num)) return res;
  res = GetCasted(Transfer_ResultFromModel,myResults.Find(num));
  return res;
}


//=======================================================================
//function : TransientResult
//purpose  : 
//=======================================================================

Handle(Standard_Transient) XSControl_TransferReader::TransientResult
       (const Handle(Standard_Transient)& ent) const
{
  Handle(Standard_Transient) tres;
  Handle(Transfer_ResultFromModel) res = FinalResult(ent);
  if (res.IsNull()) return tres;
  Handle(Transfer_ResultFromTransient) mres = res->MainResult();
  if (mres.IsNull()) return tres;
  DeclareAndCast(Transfer_SimpleBinderOfTransient,bnd,mres->Binder());
  if (bnd.IsNull()) return tres;
  if (!bnd->HasResult()) return tres;
  return bnd->Result();
}


//=======================================================================
//function : ShapeResult
//purpose  : 
//=======================================================================

TopoDS_Shape XSControl_TransferReader::ShapeResult
  (const Handle(Standard_Transient)& ent) const
{
  TopoDS_Shape tres;    // DOIT RESTER NULL
  Handle(Transfer_ResultFromModel) res = FinalResult(ent);
  if (res.IsNull()) return tres;
  Handle(Transfer_ResultFromTransient) mres = res->MainResult();
  if (mres.IsNull()) return tres;
  XSControl_Utils xu;
  TopoDS_Shape sh = xu.BinderShape (mres->Binder());

//   Ouh la vilaine verrue
  Standard_Real tolang = Interface_Static::RVal("read.encoderegularity.angle");
  if (tolang <= 0 || sh.IsNull()) return sh;
  ShapeFix::EncodeRegularity (sh,tolang);
  return sh;
}


//=======================================================================
//function : ClearResult
//purpose  : 
//=======================================================================

Standard_Boolean XSControl_TransferReader::ClearResult
  (const Handle(Standard_Transient)& ent, const Standard_Integer mode)
{
  if (myModel.IsNull()) return Standard_False;
  Standard_Integer num = myModel->Number(ent);
  if (num == 0) return Standard_False;
  if(!myResults.IsBound(num)) return Standard_False;
  if (mode < 0)
    myResults.ChangeFind(num).Nullify();
  else {
    DeclareAndCast(Transfer_ResultFromModel,resu,myResults.Find(num));
    if (resu.IsNull()) return Standard_False;
    resu->Strip (mode);
  }
  return Standard_True;
}


//  <<<< >>>>  ATTENTION, pas terrible : mieux vaudrait
//             faire une map inverse et la consulter
//             ou muscler ResultFromModel ...


//=======================================================================
//function : EntityFromResult
//purpose  : 
//=======================================================================

Handle(Standard_Transient) XSControl_TransferReader::EntityFromResult
       (const Handle(Standard_Transient)& res, const Standard_Integer mode) const
{
  Handle(Standard_Transient) nulh;
  //  cas de la shape
  XSControl_Utils xu;
  TopoDS_Shape sh = xu.BinderShape (res);
  if (!sh.IsNull()) return EntityFromShapeResult (sh,mode);

  Handle(Transfer_Binder) abinder;
  DeclareAndCast(Transfer_Binder,binder,res);
  Standard_Integer i,j,nb;

  if (mode == 0 || mode == 1) {
    //  on regarde dans le TransientProcess (Roots ou tous Mappeds)
    if (!myTP.IsNull()) {
      nb = (mode == 0 ? myTP->NbRoots() : myTP->NbMapped());
      for (j = 1; j <= nb; j ++) {
	i = (mode == 0 ? myModel->Number (myTP->Root(j)) : j);
	if (i == 0) continue;
	abinder = myTP->MapItem(i);
	if (abinder.IsNull()) continue;
	if (!binder.IsNull()) {
	  if (binder == abinder) return myTP->Mapped(i);
	  continue;
	}
	DeclareAndCast(Transfer_SimpleBinderOfTransient,trb,abinder);
	if (trb.IsNull()) continue;
	if (trb->Result() == res) return myTP->Mapped(i);
      }
    }
    return nulh;  // Null
  }

  //   Recherche dans myResults (racines)
  //     2 : Main only  3 : Main + one sub;  4 : all
  if (mode >= 2) {
    nb = myModel->NbEntities();
    for (i = 1; i <= nb; i ++) {
      Handle(Transfer_ResultFromModel) rec = ResultFromNumber (i);
      if (rec.IsNull()) return nulh;
      Handle(TColStd_HSequenceOfTransient) list = rec->Results (mode-2);
      Standard_Integer ir,nr = list->Length();
      for (ir = 1; ir <= nr; ir ++) {
        DeclareAndCast(Transfer_ResultFromTransient,rft,list->Value(ir));
        if (rft.IsNull()) continue;
        if (rft->Binder() == binder) return rft->Start();
      }
      
    }
  }

  //  autres cas non encore implementes
  return nulh;
}


//  <<<< >>>>  ATTENTION, encore moins bien que le precedent


//=======================================================================
//function : EntityFromShapeResult
//purpose  : 
//=======================================================================

Handle(Standard_Transient) XSControl_TransferReader::EntityFromShapeResult
  (const TopoDS_Shape& res, const Standard_Integer mode) const
{
  Handle(Standard_Transient) nulh, samesh, partner;
  if (res.IsNull()) return nulh;
  Standard_Integer i,j,nb;

  XSControl_Utils xu;
  if (mode == 0 || mode == 1 || mode == -1) {
    //  on regarde dans le TransientProcess
    if (!myTP.IsNull()) {
      nb = (mode == 0 ? myTP->NbRoots() : myTP->NbMapped());
      for (j = 1; j <= nb; j ++) {
	i = (mode == 0 ? myModel->Number (myTP->Root(j)) : j);
	if (i == 0) continue;
	Handle(Standard_Transient) ent = myTP->Mapped(i);
	TopoDS_Shape sh = TransferBRep::ShapeResult (myTP,ent);
	if (!sh.IsNull()) {
	  if (sh == res) return ent;
	  // priorites moindre : Same (tjrs) ou Partner (mode < 0)
	  if (sh.IsSame(res)) samesh = ent;
	  if (mode == -1 && sh.IsPartner(res)) partner= ent;
	}
      }
    }
    //    Ici, pas trouve de vraie egalite. Priorites moindres : Same puis Partner
    if (!samesh.IsNull())  return samesh;
    if (!partner.IsNull()) return partner;  // calcule si mode = -1
    return nulh;
  }

  //   Recherche dans myResults (racines)
  //     2 : Main only  3 : Main + one sub;  4 : all
  if (mode >= 2) {
    nb = myModel->NbEntities();
    for (i = 1; i <= nb; i ++) {
      Handle(Transfer_ResultFromModel) rec = ResultFromNumber (i);
      if (rec.IsNull()) continue;
      
      Handle(TColStd_HSequenceOfTransient) list = rec->Results (mode-2);
      Standard_Integer ir,nr = list->Length();
      for (ir = 1; ir <= nr; ir ++) {
        DeclareAndCast(Transfer_ResultFromTransient,rft,list->Value(ir));
        if (rft.IsNull()) continue;
        TopoDS_Shape sh = xu.BinderShape (rft->Binder());
        if (!sh.IsNull() && sh == res) return rft->Start();
      }
      
    }
  }

  return nulh;
}


//=======================================================================
//function : EntitiesFromShapeList
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfTransient) XSControl_TransferReader::EntitiesFromShapeList
       (const Handle(TopTools_HSequenceOfShape)& res,
        const Standard_Integer mode) const
{
  Handle(TColStd_HSequenceOfTransient) lt = new TColStd_HSequenceOfTransient();
  if (res.IsNull()) return lt;
  TopTools_MapOfShape shapes;

  //  On convertit res en une map, pour test de presence rapide
  Standard_Integer i, j, nb = res->Length();
  if (nb == 0) return lt;
  for (i = 1; i <= nb; i ++)  shapes.Add (res->Value(i));

  //  A present, recherche et enregistrement

  XSControl_Utils xu;
  if (mode == 0 || mode == 1) {
    //  on regarde dans le TransientProcess
    if (!myTP.IsNull()) {
      nb = (mode == 0 ? myTP->NbRoots() : myTP->NbMapped());
      for (j = 1; j <= nb; j ++) {
	i = (mode == 0 ? myModel->Number (myTP->Root(j)) : j);
	if (i == 0) continue;
	TopoDS_Shape sh = xu.BinderShape (myTP->MapItem(i));
	if (!sh.IsNull() && shapes.Contains(sh)) {
	  lt->Append (myTP->Mapped(i));
          j=nb; //skl (for looking for entities in checkbrep)
        }
      }
    }
  }

  //   Recherche dans myResults (racines)
  //     2 : Main only  3 : Main + one sub;  4 : all
  if (mode >= 2) {
    nb = myModel->NbEntities();
    for (i = 1; i <= nb; i ++) {
      Handle(Transfer_ResultFromModel) rec = ResultFromNumber (i);
      if (rec.IsNull()) continue;
      
      Handle(TColStd_HSequenceOfTransient) list = rec->Results (mode-2);
      Standard_Integer ir,nr = list->Length();
      for (ir = 1; ir <= nr; ir ++) {
        DeclareAndCast(Transfer_ResultFromTransient,rft,list->Value(i));
        if (rft.IsNull()) continue;
        TopoDS_Shape sh = xu.BinderShape (rft->Binder());
        if (!sh.IsNull() && shapes.Contains(sh)) lt->Append (rft->Start());
      }
      
    }
  }

  return lt;
}


//  <<<< >>>>  ATTENTION, level pas traite (utile ?) -> ResultFromModel


//=======================================================================
//function : CheckList
//purpose  : 
//=======================================================================

Interface_CheckIterator XSControl_TransferReader::CheckList
  (const Handle(Standard_Transient)& ent, const Standard_Integer level) const
{
  Interface_CheckIterator chl;
  if (myModel.IsNull() || ent.IsNull()) return chl;
  //  Check-List COMPLETE ... tout le Modele
  if (ent == myModel) {
    Standard_Integer i,nb = myModel->NbEntities();
    for (i = 1; i <= nb; i ++) {
      Handle(Transfer_ResultFromModel) rec = ResultFromNumber (i);
      if (!rec.IsNull()) {
	Interface_CheckIterator chiter  = rec->CheckList (Standard_False,2);
	chl.Merge (chiter);
      }
    }
  }
  //  Check-List sur une LISTE ...
  else if (ent->IsKind(STANDARD_TYPE(TColStd_HSequenceOfTransient))) {
    DeclareAndCast(TColStd_HSequenceOfTransient,list,ent);
    Standard_Integer i,nb = list->Length();
    for (i = 1; i <= nb; i ++) {
      Handle(Transfer_ResultFromModel) rec = FinalResult (list->Value(i));
      if (!rec.IsNull()) {
	Interface_CheckIterator chiter = rec->CheckList (Standard_False,level);
	chl.Merge (chiter);
      }
    }
  }

  //  sinon, Check-List sur une entite : Last ou FinalResult
  else if (level < 0) {
    if (myTP.IsNull()) return chl;
    chl.Add (myTP->Check(ent),myModel->Number(ent));
  } else {
    Handle(Transfer_ResultFromModel) rec = FinalResult (ent);
    if (rec.IsNull()) return chl;
    chl = rec->CheckList(Standard_False,level);  // manque level ...
  }
  if (ent == myModel) chl.SetName ("XSControl : CheckList complete Model");
  else if (level <  0) chl.SetName ("XSControl : CheckList Last");
  else if (level == 0) chl.SetName ("XSControl : CheckList Final Main");
  else if (level == 1) chl.SetName ("XSControl : CheckList Final Main+Subs");
  else if (level >= 2) chl.SetName ("XSControl : CheckList Final Complete");
  return chl;
}


//=======================================================================
//function : HasChecks
//purpose  : 
//=======================================================================

Standard_Boolean XSControl_TransferReader::HasChecks
  (const Handle(Standard_Transient)& ent, const Standard_Boolean failsonly) const
{
  Handle(Transfer_ResultFromModel) resu = FinalResult (ent);
  if (resu.IsNull()) return Standard_False;
  Standard_Integer stat = resu->ComputeCheckStatus (Standard_False);
  if (stat == 0) return Standard_False;
  if (stat >  1) return Standard_True;
  return (!failsonly);
}


//=======================================================================
//function : CheckedList
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfTransient)  XSControl_TransferReader::CheckedList
       (const Handle(Standard_Transient)& ent,
        const Interface_CheckStatus withcheck, const Standard_Boolean level) const
{
  Handle(TColStd_HSequenceOfTransient) res = new TColStd_HSequenceOfTransient();
  if (ent.IsNull()) return res;

  if (ent == myModel) {
    Standard_Integer i,nb = myModel->NbEntities();
    for (i = 1; i <= nb; i ++) {
      Handle(Transfer_ResultFromModel) rec = ResultFromNumber (i);
      if (!rec.IsNull()) res->Append (rec->CheckedList(withcheck,level));
    }
  } else if (ent->IsKind(STANDARD_TYPE(TColStd_HSequenceOfTransient))) {
    DeclareAndCast(TColStd_HSequenceOfTransient,list,ent);
    Standard_Integer i,nb = list->Length();
    for (i = 1; i <= nb; i ++) {
      Handle(Transfer_ResultFromModel) rec = FinalResult (list->Value(i));
      if (!rec.IsNull()) res->Append (rec->CheckedList(withcheck,level));
    }
  } else {
    Handle(Transfer_ResultFromModel) rec = FinalResult (ent);
    if (!rec.IsNull()) res = rec->CheckedList(withcheck,level);
  }
  return res;
}


//  ########################################################
//  ###########            TRANSFERT            ############
//  ########################################################


//=======================================================================
//function : BeginTransfer
//purpose  : 
//=======================================================================

Standard_Boolean XSControl_TransferReader::BeginTransfer ()
{
  if (myModel.IsNull()) return Standard_False;
  if (Actor().IsNull())  return Standard_False;
  myShapeResult.Nullify();

  if (myTP.IsNull()) myTP = new Transfer_TransientProcess
    (myModel->NbEntities());

  Handle(Transfer_ActorOfTransientProcess) actor;
  myTP->SetActor (actor);        // -> RAZ
  actor = Actor();
  myTP->SetActor (actor);        // Set proprement dit
  myTP->SetErrorHandle (Standard_True);
  NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)>& aTPContext = myTP->Context();
  aTPContext = myContext;
  return Standard_True;
}


//=======================================================================
//function : Recognize
//purpose  : 
//=======================================================================

Standard_Boolean XSControl_TransferReader::Recognize
  (const Handle(Standard_Transient)& ent)
{
  if (myActor.IsNull()) return Standard_False;
  return myActor->Recognize (ent);
}


//=======================================================================
//function : TransferOne
//purpose  : 
//=======================================================================

Standard_Integer XSControl_TransferReader::TransferOne
  (const Handle(Standard_Transient)& ent, const Standard_Boolean rec)
{
  if (myActor.IsNull() || myModel.IsNull()) return 0;

  if (myTP.IsNull())  {  if (!BeginTransfer()) return 0;  }

  Handle(Message_Messenger) sout = myTP->Messenger();
  Standard_Integer level = myTP->TraceLevel();
  

  Transfer_TransferOutput TP (myTP,myModel);
  if (myGraph.IsNull()) myTP->SetModel(myModel);
  else                  myTP->SetGraph(myGraph);

  //  pour le log-file
  if (level > 1) {
    Standard_Integer num = myModel->Number(ent);
    Handle(TCollection_HAsciiString) lab = myModel->StringLabel(ent);
    sout<<"\n*******************************************************************\n";
    sout << "******           Transferring one Entity                     ******"<<endl;
    if (!lab.IsNull())
      sout<<"******    N0 in file : "<<Interface_MSG::Blanks(num,5)<<num
	  <<"      Ident : "<<lab->ToCString()
	  <<  Interface_MSG::Blanks(14 - lab->Length())<<"******\n";
    sout << "******    Type : "<<myModel->TypeName(ent,Standard_False)
        <<  Interface_MSG::Blanks((Standard_Integer) (44 - strlen(myModel->TypeName(ent,Standard_False))))
	<<  "******";
    sout<<"\n*******************************************************************\n";
  }

  //  seule difference entre TransferRoots et TransferOne
  Standard_Integer res = 0;
  Handle(Standard_Transient) obj = ent;
  TP.Transfer (obj);
  myTP->SetRoot (obj);

  //  Resultat ...
  Handle(Transfer_Binder) binder = myTP->Find (obj);
  if (binder.IsNull()) return res;
  if (rec) RecordResult (obj);

  if (!binder->HasResult()) return res;
  res ++;

  return res;
}


//=======================================================================
//function : TransferList
//purpose  : 
//=======================================================================

Standard_Integer XSControl_TransferReader::TransferList
  (const Handle(TColStd_HSequenceOfTransient)& list, const Standard_Boolean rec)
{
  if (myActor.IsNull() || myModel.IsNull()) return 0;

  if (myTP.IsNull())  {  if (!BeginTransfer()) return 0;  }

  Handle(Message_Messenger) sout = myTP->Messenger();
  Standard_Integer level = myTP->TraceLevel();

  Transfer_TransferOutput TP (myTP,myModel);
  if (myGraph.IsNull()) myTP->SetModel(myModel);
  else                  myTP->SetGraph(myGraph);

  Standard_Integer i,nb = list->Length();

  //   Pour le log-file
  if (level > 0) {
    sout<<"\n*******************************************************************\n";
    sout << "******           Transferring a list of "<<Interface_MSG::Blanks(nb,5)<<" Entities       ******"<<endl;
    sout<<"\n*******************************************************************\n";

    Handle(IFSelect_SignatureList) sl = new IFSelect_SignatureList;
    for (i = 1; i <= nb; i ++)
      sl->Add (list->Value(i), myModel->TypeName(list->Value(i),Standard_False));
    sl->SetName ("Entities to Transfer");
    sl->PrintCount (sout);
    sout<<"\n*******************************************************************\n";
  }

  //  seule difference entre TransferRoots et TransferOne
  Standard_Integer res = 0;
  nb = list->Length();
  Handle(Standard_Transient) obj;

  for (i = 1; i <= nb; i ++) {
    obj = list->Value(i);
    TP.Transfer (obj);
    myTP->SetRoot (obj);

    //  Resultat ...
    Handle(Transfer_Binder) binder = myTP->Find (obj);
    if (binder.IsNull()) continue;
    if (rec) RecordResult (obj);

    if (!binder->HasResult()) continue;
    res ++;
  }
  return res;
}


//  <<<< >>>>  passage Graph : judicieux ?


//=======================================================================
//function : TransferRoots
//purpose  : 
//=======================================================================

Standard_Integer XSControl_TransferReader::TransferRoots(const Interface_Graph& G)
{
  if (myModel != G.Model()) return -1;
  if (!BeginTransfer()) return -1;
  Handle(Message_Messenger) sout = myTP->Messenger();
  Standard_Integer level = myTP->TraceLevel();

  Transfer_TransferOutput TP (myTP,myModel);
  if (myGraph.IsNull()) myTP->SetModel(myModel);
  else                  myTP->SetGraph(myGraph);

  //   Pour le log-file
  if (level > 0) {
    Interface_EntityIterator roots = G.RootEntities();
    Standard_Integer nb = roots.NbEntities();
    sout<<"\n*******************************************************************\n";
    sout << "******           Transferring the "<<Interface_MSG::Blanks(nb,5)<<" Root Entities        ******"<<endl;
    sout<<"\n*******************************************************************\n";
    Handle(IFSelect_SignatureList) sl = new IFSelect_SignatureList;
    for (roots.Start(); roots.More(); roots.Next())
      sl->Add (roots.Value(),myModel->TypeName(roots.Value(),Standard_False));
    sl->SetName ("Entities to Transfer");
    sl->PrintCount (sout);
    sout<<"\n*******************************************************************\n";
  }

  TP.TransferRoots (G);

  //  Les entites transferees sont notees "asmain"
  Standard_Integer i,n = myTP->NbMapped();
  for (i = 1; i <= n; i ++) {
    Handle(Standard_Transient) ent = myTP->Mapped(i);
    Handle(Transfer_Binder)    bnd = myTP->MapItem(i);
    if (bnd.IsNull()) continue;
    if (!bnd->HasResult()) continue;
    RecordResult (ent);
  }

  //  Resultat ... on note soigneuseument les Shapes
  myShapeResult = TransferBRep::Shapes (myTP,Standard_True);
  // ????  Et ici, il faut alimenter Imagine ...
  return myShapeResult->Length();
}


//=======================================================================
//function : TransferClear
//purpose  : 
//=======================================================================

void XSControl_TransferReader::TransferClear(const Handle(Standard_Transient)& ent,
                                             const Standard_Integer level)
{
  if (myTP.IsNull()) return;
  if (ent == myModel) {  myTP->Clear();  return;  }

  myTP->RemoveResult (ent,level);
  ClearResult (ent,-1);
 
}


//=======================================================================
//function : PrintStats
//purpose  : 
//=======================================================================

void XSControl_TransferReader::PrintStats
  (const Standard_Integer what, const Standard_Integer mode) const
{
  Handle(Message_Messenger) sout = myTP->Messenger();
  //  A ameliorer ... !
  sout<<"\n*******************************************************************\n";
  sout << "******        Statistics on Transfer (Read)                  ******"<<endl;
  sout<<"\n*******************************************************************\n";
  if (what > 10)  {  sout<<" ***  Not yet implemented"<<endl;  return;  }
  if (what < 10)  {
    sout << "******        Data recorded on Last Transfer                 ******"<<endl;
    PrintStatsProcess (myTP,what,mode);
  }
  //  reste  what = 10 : on liste les racines des final results
  sout << "******        Final Results                                  ******"<<endl;
  if (myModel.IsNull())  {  sout<<"****    Model unknown"<<endl;  return;  }
  Handle(TColStd_HSequenceOfTransient) list = RecordedList();
  Standard_Integer i, nb = list->Length();
  Handle(IFSelect_SignatureList) counter;
  if (mode > 2) counter = new IFSelect_SignatureList (mode == 6);
  IFSelect_PrintCount pcm = IFSelect_CountByItem;
  if (mode == 6) pcm = IFSelect_ListByItem;

  sout<<"****    Nb Recorded : "<<nb<<" : entities n0s : ";
  for (i = 1; i <= nb; i ++) {
    Handle(Standard_Transient) ent = list->Value(i);
    if (mode == 0)  {  sout<<"  "<<myModel->Number(ent); continue;  }
    if (mode == 1 || mode == 2) {
      sout<<"[ "<<Interface_MSG::Blanks (i,6)<<" ]:";
      myModel->Print (ent,sout);
      sout<<"  Type:"<<myModel->TypeName(ent,Standard_False);
    }
    if (mode >= 3 && mode <= 6) {
      counter->Add (ent,myModel->TypeName(ent,Standard_False));
    }
  }
  if (!counter.IsNull()) counter->PrintList(sout,myModel,pcm);

  sout<<endl;
}


//  ########################################################
//  ###########            TRANSFERT            ############


//=======================================================================
//function : LastCheckList
//purpose  : 
//=======================================================================

Interface_CheckIterator  XSControl_TransferReader::LastCheckList () const
{
  Interface_CheckIterator chl;
  if (!myTP.IsNull()) chl = myTP->CheckList (Standard_False);
  return chl;
}


//=======================================================================
//function : LastTransferList
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfTransient) XSControl_TransferReader::LastTransferList
       (const Standard_Boolean roots) const
{
  Handle(TColStd_HSequenceOfTransient) li = new TColStd_HSequenceOfTransient();
  if (myTP.IsNull()) return li;
  Standard_Integer i,j,nb =
    (roots ? myTP->NbRoots() : myTP->NbMapped());
  for (j = 1; j <= nb; j ++) {
    i = (roots ? myModel->Number (myTP->Root(j)) : j);
    Handle(Transfer_Binder) bnd = myTP->MapItem(i);
    if (bnd.IsNull()) continue;
    if (!bnd->HasResult()) continue;
    li->Append (myTP->Mapped(i));
  }
  return li;
}


//=======================================================================
//function : ShapeResultList
//purpose  : 
//=======================================================================

const Handle(TopTools_HSequenceOfShape) & XSControl_TransferReader::ShapeResultList
  (const Standard_Boolean rec)
{
  if (!rec) {
    if (myShapeResult.IsNull()) myShapeResult = TransferBRep::Shapes (myTP,Standard_True);
    if (myShapeResult.IsNull()) myShapeResult = new TopTools_HSequenceOfShape();
  } else {
    if (myShapeResult.IsNull()) myShapeResult = new TopTools_HSequenceOfShape();
    if (myModel.IsNull()) return myShapeResult;
    Handle(TColStd_HSequenceOfTransient) li = RecordedList();
    myShapeResult = new TopTools_HSequenceOfShape();
    Standard_Integer i, nb = myModel->NbEntities();
    TopoDS_Shape sh;
    for (i = 1; i <= nb; i ++) {
      sh = ShapeResult (myModel->Value(i));
      if (!sh.IsNull()) myShapeResult->Append(sh);
    }
  }
  return myShapeResult;
}


//  ****    UTILITAIRE DE STATISTIQUES GENERALES

// BinderStatus retourne une valeur :
// 0 Binder Null.   1 void  2 Warning seul  3 Fail seul
// 11 Resultat OK. 12 Resultat+Warning. 13 Resultat+Fail

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
static Standard_Integer BinderStatus (const Handle(Transfer_Binder)& binder, char* mess)
{
  Standard_Integer stat = 0;
  mess[0] = '\0';
  if (binder.IsNull())  {  sprintf (mess,"(no data recorded)");  return 0;  }
  Interface_CheckStatus cst = binder->Check()->Status();
  if (cst == Interface_CheckOK) {
    stat = 11;
    if (binder->HasResult()) sprintf(mess,"%s",binder->ResultTypeName());
    else { sprintf(mess,"(no result)"); stat = 1; }
  } else if (cst == Interface_CheckWarning) {
    stat = 12;
    if (binder->HasResult()) sprintf(mess,"%s  (+ warning)",binder->ResultTypeName());
    else { sprintf(mess,"(warning)"); stat = 2; }
  } else if (cst == Interface_CheckFail) {
    stat = 13;
    if (binder->HasResult()) sprintf(mess,"%s  (+ FAIL)",binder->ResultTypeName());
    else { sprintf(mess,"(FAIL)"); stat = 3; }
  }
  return stat;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================
static void PrintPercent(const Handle(Message_Messenger)& sout, const Standard_CString mess,
                         const Standard_Integer nb, const Standard_Integer nl)
{
  if (nb <= 0 || nl == 0) return;
  sout<<"******      "<<mess<<": ";
  if      (nb == nl)       sout<<"100 %"<<endl;
  else if (nb*100/nl == 0) sout<<"< 1 %"<<endl;
  else            sout<<(nb*100/nl < 10 ? "  " : " ")<<nb*100/nl<<" %"<<endl;
}


//=======================================================================
//function : PrintStatsProcess
//purpose  : 
//=======================================================================

void XSControl_TransferReader::PrintStatsProcess(const Handle(Transfer_TransientProcess)& TP,
                                                 const Standard_Integer what,
                                                 const Standard_Integer mode)
{
  Handle(TColStd_HSequenceOfTransient) list;  // null
  XSControl_TransferReader::PrintStatsOnList (TP,list,what,mode);
}


//=======================================================================
//function : PrintStatsOnList
//purpose  : 
//=======================================================================

void XSControl_TransferReader::PrintStatsOnList(const Handle(Transfer_TransientProcess)& TP,
                                                const Handle(TColStd_HSequenceOfTransient)& list,
                                                const Standard_Integer what,
                                                const Standard_Integer mode)
{
  Handle(Message_Messenger) sout = TP->Messenger();
  char mess[250];
  if (TP.IsNull()) return;
  if (what == 0) {  TP->PrintStats(0,sout);  return;  }

  sout<<"\n*******************************************************************\n";
  sout << "******        Statistics on Transfer Process (Read)          ******"<<endl;
  if (what == 1) sout << "******        Individual Transfers  (Roots)                  ******\n";
  if (what == 2) sout << "******        All recorded data about Transfer               ******\n";
  if (what == 3) sout << "******        Abnormal records                               ******\n";
  if (what == 1 || what == 2 || what == 3) {
    if (mode == 0) sout<<"******        (n0s of recorded entities)                     ******\n";
    if (mode == 1) sout<<"******        (per entity : type + result)                   ******\n";
    if (mode == 2) sout<<"******        (per entity : type + result/status)            ******\n";
    if (mode == 3) sout<<"******        (count per type of entity)                     ******\n";
    if (mode == 4) sout<<"******        (count per type of result)                     ******\n";
    if (mode == 5) sout<<"******   (count per couple entity-type / result-type/status) ******\n";
    if (mode == 6) sout<<"******   (list per couple entity-type / result-type/status)  ******\n";
  }
  if (what == 4) sout << "******        Check messages                                 ******\n";
  if (what == 5) sout << "******        Fail  messages                                 ******\n";
  sout<<"*******************************************************************\n";

  //  Cas what = 1,2,3 : contenu du TP (binders)

  Standard_Boolean nolist = list.IsNull();
  Handle(Interface_InterfaceModel) model = TP->Model();
  if (what >= 1 && what <= 3) {

    Standard_Integer stat;
    Standard_Integer nbv = 0, nbw = 0, nbf = 0, nbr = 0, nbrw = 0, nbrf = 0, nbnr = 0, nbi = 0;
    Transfer_IteratorOfProcessForTransient itrp(Standard_True);
    if (what == 1) itrp = TP->RootResult(Standard_True);
    if (what == 2) itrp = TP->CompleteResult(Standard_True);
    if (what == 3) itrp = TP->AbnormalResult();
    Standard_Integer i = 0, nb = itrp.Number();
    if (!nolist) itrp.Filter (list);
    Standard_Integer nl = itrp.Number();    // apres filtrage
    Handle(IFSelect_SignatureList) counter;
    if (mode > 2) counter = new IFSelect_SignatureList (mode == 6);
    Standard_Boolean notrec = (!nolist && mode > 2);  // noter les "no record"
    IFSelect_PrintCount pcm = IFSelect_CountByItem;
    if (mode == 6) pcm = IFSelect_ListByItem;

    sout  <<"****        Entities in Model   : "<<model->NbEntities()<<endl;
    sout  <<"****        Nb Items (Transfer) : "<<nb<<endl;
    if (!nolist)
      sout<<"****        Nb Items (Listed)   : "<<nl<<endl;

    for (itrp.Start(); itrp.More(); itrp.Next()) {
      nbi ++;
      Handle(Transfer_Binder) binder = itrp.Value();
      Handle(Standard_Transient) ent = itrp.Starting();
      if (binder.IsNull())  {
	nbnr ++;
	if (notrec) counter->Add(ent,"(not recorded)");
	else if (mode == 1 || mode == 2) {
	  sout<<"["<<Interface_MSG::Blanks (nbi,4)<<nbi<<" ]:";
	  model->Print (ent,sout);
	  sout<<"   "<<model->TypeName(ent,Standard_False)<<"  (not recorded)"<<endl;
	  continue;
	}
      }
      if (mode == 0)  {  sout<<"  "<<model->Number(ent); continue;  }
      if (mode != 3) {
	stat = BinderStatus(binder,mess);
        // 0 Binder Null.   1 void  2 Warning seul  3 Fail seul
        // 11 Resultat OK. 12 Resultat+Warning. 13 Resultat+Fail
	if (stat ==  0 || stat == 1) nbv ++;
	if (stat ==  2) nbw ++;
	if (stat ==  3) nbf ++;
	if (stat == 11) nbr ++;
	if (stat == 12) nbrw ++;
	if (stat == 13) nbrf ++;
      }

      //  mode : 0 list num;  1 : num+label + type + result (abrege);  2 : complet
      if (mode == 1 || mode == 2) {
	sout<<"["<<Interface_MSG::Blanks (i,4)<<i<<" ]:";
	model->Print (ent,sout);
	sout<<"   "<<model->TypeName(ent,Standard_False);
	sout<<"	Result:"<<mess<<endl;
	if (mode == 1) continue;

	const Handle(Interface_Check)& ch = binder->Check();
	Standard_Integer newi,newnbw = ch->NbWarnings(), newnbf = ch->NbFails();

	if (newnbw > 0) {
	  sout<<" - Warnings : "<<newnbw<<":\n";
	  for (newi = 1; newi <= newnbw; newi ++) sout<<ch->CWarning(newi)<<endl;
	}
	if (newnbf > 0) {
	  sout<<" - Fails : "<<newnbf<<":\n";
	  for (newi = 1; newi <= newnbf; newi ++) sout<<ch->CFail(newi)<<endl;
	}
	continue;
      }

      //  mode : 3, counts per type of starting entity (class type)
      //         4 : counts per result type and/or status
      //         5 : counts per couple (starting type / result type/status)
      //         6 : idem plus gives for each item, the list of numbers of
      //                  entities in the starting model
      if (mode >= 3 && mode <= 6) {
        //IFSelect_PrintCount newpcm = IFSelect_CountByItem;
        //if (mode == 6) newpcm = IFSelect_ListByItem;
	if (mode == 3) counter->Add (ent,model->TypeName(ent,Standard_False));
	if (mode == 4) counter->Add (ent,mess);
	if (mode >= 5) {
	  TCollection_AsciiString mest (model->TypeName(ent,Standard_False));
	  mest.AssignCat("	-> ");
	  mest.AssignCat(mess);
          //sprintf(mest,"%s	-> %s",model->TypeName(ent,Standard_False),mess);
	  counter->Add (ent,mest.ToCString());
	}
      }

      //    Fin de l iteration
    }
    if (!counter.IsNull()) counter->PrintList(sout,model,pcm);
    else sout<<endl;
    //    Pourcentages
    if (mode != 3 && nbi > 0) {
      sout << "******        Percentages according Transfer Status          ******"<<endl;
      PrintPercent (sout,"Result          ",nbr+nbrw,nl);
      PrintPercent (sout,"Result + FAIL   ",nbrf,nl);
      PrintPercent (sout,"FAIL, no Result ",nbf,nl);
      PrintPercent (sout,"Just Warning    ",nbw,nl);
      PrintPercent (sout,"Nothing Recorded",nbnr,nl);
/*      if (nbr+nbrw > 0)
	sout<<"******      Result          : "<< (nbr+nbrw)*100/nl<<" %"<<endl;
      if (nbrf > 0)
	sout<<"******      Result + FAIL   : "<< (nbrf)*100/nl<<" %"<<endl;
      if (nbf > 0)
	sout<<"******      FAIL, no Result : "<< (nbf)*100/nl<<" %"<<endl;
      if (nbw > 0)
	sout<<"******      Just Warning    : "<< (nbw)*100/nl<<" %"<<endl;
      if (nbnr > 0)
	sout<<"******      Nothing Recorded: "<< (nbnr)*100/nl<<" %"<<endl; */
    }
    return;
  }

  //  Cas  what = 4,5 : check-list

  if (what == 4 || what == 5) {

    Interface_CheckIterator chl = TP->CheckList(Standard_False);
    chl.SetName("** TRANSFER READ CHECK **");
    if (mode == 0) chl.Print (sout,model,(what == 5));
    else {
      IFSelect_PrintCount pcm = IFSelect_CountByItem;
      if (mode == 2) pcm = IFSelect_ListByItem;
      Handle(IFSelect_CheckCounter) counter = new IFSelect_CheckCounter(Standard_True);
      counter->Analyse   (chl,model,Standard_True,(what == 5));
      counter->PrintList (sout,model,pcm);
    }
  }

}
