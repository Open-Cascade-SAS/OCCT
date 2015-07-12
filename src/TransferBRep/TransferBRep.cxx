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
#include <BRep_Tool.hxx>
#include <BRepLib.hxx>
#include <Interface_Check.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Message_Msg.hxx>
#include <Message_Printer.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Transient.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_HShape.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_FinderProcess.hxx>
#include <Transfer_IteratorOfProcessForTransient.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <Transfer_TransientProcess.hxx>
#include <TransferBRep.hxx>
#include <TransferBRep_BinderOfShape.hxx>
#include <TransferBRep_ShapeBinder.hxx>
#include <TransferBRep_ShapeListBinder.hxx>
#include <TransferBRep_ShapeMapper.hxx>
#include <TransferBRep_TransferResultInfo.hxx>

#include <stdio.h>
//#include <TransferBRep_Analyzer.hxx>
static void  ShapeAppend
  (const Handle(Transfer_Binder)& binder,
   const Handle(TopTools_HSequenceOfShape)& shapes)
{
  if (binder.IsNull()) return;
  if (binder->IsKind(STANDARD_TYPE(TransferBRep_BinderOfShape))) {
    DeclareAndCast(TransferBRep_BinderOfShape,shbind,binder);
    if (shbind->HasResult()) shapes->Append (shbind->Result());
  }
  else if (binder->IsKind(STANDARD_TYPE(TransferBRep_ShapeListBinder))) {
    DeclareAndCast(TransferBRep_ShapeListBinder,slbind,binder);
    Standard_Integer i,nb = slbind->NbShapes();
    for (i = 1; i <= nb; i ++) shapes->Append (slbind->Shape(i));
  }
  else if (binder->IsKind(STANDARD_TYPE(Transfer_SimpleBinderOfTransient))) {
    DeclareAndCast(Transfer_SimpleBinderOfTransient,trbind,binder);
    DeclareAndCast(TopoDS_HShape,hs,trbind->Result());
    if (!hs.IsNull()) shapes->Append (hs->Shape());
  }
  Handle(Transfer_Binder) nextr = binder->NextResult();
  if (!nextr.IsNull()) ShapeAppend (nextr,shapes);
}


    TopoDS_Shape  TransferBRep::ShapeResult
  (const Handle(Transfer_Binder)& binder)
{
  TopoDS_Shape shape;
  Handle(Transfer_Binder) bnd = binder;
  while (!bnd.IsNull()) {
    DeclareAndCast(TransferBRep_BinderOfShape,shb,bnd);
    if (!shb.IsNull()) return shb->Result();
    DeclareAndCast(Transfer_SimpleBinderOfTransient,hsb,bnd);
    if (!hsb.IsNull()) {
      Handle(TopoDS_HShape) hsp = GetCasted(TopoDS_HShape,hsb->Result());
      if (!hsp.IsNull()) return hsp->Shape();
    }
    bnd = bnd->NextResult();
  }
  return shape;
}

    TopoDS_Shape  TransferBRep::ShapeResult
  (const Handle(Transfer_TransientProcess)& TP,
   const Handle(Standard_Transient)& ent)
{
  TopoDS_Shape shape;
  Handle(Transfer_Binder) binder = TP->Find(ent);
  if (binder.IsNull())  binder = GetCasted (Transfer_Binder,ent);
  if (!binder.IsNull()) return TransferBRep::ShapeResult (binder);
  DeclareAndCast(TopoDS_HShape,hsp,ent);
  if (!hsp.IsNull()) return hsp->Shape();
  return shape;
}


    void  TransferBRep::SetShapeResult
  (const Handle(Transfer_TransientProcess)& TP,
   const Handle(Standard_Transient)& ent, const TopoDS_Shape& result)
{
  if (result.IsNull() || ent.IsNull() || TP.IsNull()) return;
  TP->Bind (ent,new TransferBRep_ShapeBinder(result));
}


    Handle(TopTools_HSequenceOfShape)  TransferBRep::Shapes
  (const Handle(Transfer_TransientProcess)& TP, const Standard_Boolean roots)
{
  Handle(TopTools_HSequenceOfShape) shapes;
  if (TP.IsNull()) return shapes;
  shapes = new TopTools_HSequenceOfShape();

  Transfer_IteratorOfProcessForTransient list = 
    (roots ? TP->RootResult() : TP->CompleteResult());

  for (list.Start(); list.More(); list.Next()) {
    Handle(Transfer_Binder) binder = list.Value();
    ShapeAppend (binder,shapes);
  }
  return shapes;
}

    Handle(TopTools_HSequenceOfShape)  TransferBRep::Shapes
  (const Handle(Transfer_TransientProcess)& TP,
   const Handle(TColStd_HSequenceOfTransient)& list)
{
  Handle(TopTools_HSequenceOfShape) shapes;
  if (TP.IsNull() && list.IsNull()) return shapes;
  shapes = new TopTools_HSequenceOfShape();

  Standard_Integer ie, ne = list->Length();
  for (ie = 1; ie <= ne; ie ++) {
    Handle(Transfer_Binder) binder = TP->Find(list->Value(ie));
    ShapeAppend (binder,shapes);
  }

  return shapes;
}


    TopAbs_Orientation  TransferBRep::ShapeState
  (const Handle(Transfer_FinderProcess)& FP, const TopoDS_Shape& shape)
{
  if (FP.IsNull() || shape.IsNull()) return TopAbs_EXTERNAL;
  Handle(TransferBRep_ShapeMapper) sm = new TransferBRep_ShapeMapper(shape);
  Standard_Integer index = FP->MapIndex (sm);
  if (index == 0) return TopAbs_EXTERNAL;
  sm = Handle(TransferBRep_ShapeMapper)::DownCast(FP->Mapped(index));
  if (sm.IsNull()) return TopAbs_EXTERNAL;
  const TopoDS_Shape& mapped = sm->Value();
//  l egalite est assumee, on ne teste que l orientation
  if (mapped.Orientation() != shape.Orientation()) return TopAbs_REVERSED;
  return TopAbs_FORWARD;
}

    Handle(Transfer_Binder)  TransferBRep::ResultFromShape
  (const Handle(Transfer_FinderProcess)& FP, const TopoDS_Shape& shape)
{
  Handle(Transfer_Binder) res;
  if (FP.IsNull() || shape.IsNull()) return res;
  Handle(TransferBRep_ShapeMapper) sm = new TransferBRep_ShapeMapper(shape);
  return FP->Find (sm);
}

    Handle(Standard_Transient)  TransferBRep::TransientFromShape
  (const Handle(Transfer_FinderProcess)& FP, const TopoDS_Shape& shape)
{
  Handle(Standard_Transient) res;
  if (FP.IsNull() || shape.IsNull()) return res;
  Handle(TransferBRep_ShapeMapper) sm = new TransferBRep_ShapeMapper(shape);
  return FP->FindTransient (sm);
}


    void  TransferBRep::SetTransientFromShape
  (const Handle(Transfer_FinderProcess)& FP,
   const TopoDS_Shape& shape, const Handle(Standard_Transient)& result)
{
  if (FP.IsNull() || shape.IsNull()) return;
  Handle(TransferBRep_ShapeMapper) sm = new TransferBRep_ShapeMapper(shape);
  FP->BindTransient (sm,result);
}

    Handle(TransferBRep_ShapeMapper)  TransferBRep::ShapeMapper
  (const Handle(Transfer_FinderProcess)& FP,
   const TopoDS_Shape& shape)
{
  Handle(TransferBRep_ShapeMapper) mapper = new TransferBRep_ShapeMapper(shape);
  Standard_Integer index = FP->MapIndex (mapper);
  if (index == 0) return mapper;
  return Handle(TransferBRep_ShapeMapper)::DownCast(FP->Mapped(index));
}

// Functions to collect transfer result information

//=======================================================================
//function : FillInfo
//purpose  : 
//=======================================================================

static void FillInfo (const Handle(Transfer_Binder)& Binder,
		      const Handle(Interface_Check)& Check,
		      const Handle(TransferBRep_TransferResultInfo)& Info)
{
  Standard_Integer R = 0, RW = 0, RF = 0, RWF = 0, NR = 0, NRW = 0, NRF = 0, NRWF = 0;
  if (Binder->HasResult())
    if (Check->HasWarnings() && Check->HasFailed()) RWF++;
    else if (Check->HasWarnings()) RW++;
    else if (Check->HasFailed()) RF++;
    else R++;
  else 
    if (Check->HasWarnings() && Check->HasFailed()) NRWF++;
    else if (Check->HasWarnings()) NRW++;
    else if (Check->HasFailed()) NRF++;
    else NR++;
  Info->Result()   += R;  Info->ResultWarning()   += RW;  Info->ResultFail()   += RF;  Info->ResultWarningFail()   += RWF;
  Info->NoResult() += NR; Info->NoResultWarning() += NRW; Info->NoResultFail() += NRF; Info->NoResultWarningFail() += NRWF;
}     
	     
//=======================================================================
//function : TransferResultInfo
//purpose  : 
//=======================================================================

 void TransferBRep::TransferResultInfo (const Handle(Transfer_TransientProcess)& TP,
					const Handle(TColStd_HSequenceOfTransient)& EntityTypes,
					Handle(TransferBRep_HSequenceOfTransferResultInfo)& InfoSeq)
{
  // create output Sequence in accordance with required ShapeTypes
  InfoSeq = new TransferBRep_HSequenceOfTransferResultInfo;
  if (TP.IsNull() || EntityTypes.IsNull()) return;
  Standard_Integer SeqLen = EntityTypes->Length();
  Standard_Integer i; // svv Jan11 2000 : porting on DEC
  for (i = 1; i <= SeqLen; i++) {
    InfoSeq->Append (new TransferBRep_TransferResultInfo);
  }
  
  // fill Sequence
  Standard_Integer NbMapped = TP->NbMapped();
  for (i = 1; i <= NbMapped; i++) {
    Handle(Standard_Transient) Entity = TP->Mapped (i);

    Handle(Transfer_Binder) Binder = TP->Find (Entity);
    if (Binder.IsNull()) continue;
    const Handle(Interface_Check) Check = Binder->Check ();
    
    // find appropriate element in the Sequence
    for (Standard_Integer index = 1; index <= SeqLen; index++) {
      if (Entity->IsKind (EntityTypes->Value(index)->DynamicType())) {
	Handle(TransferBRep_TransferResultInfo) Info = InfoSeq->Value (index);
	// fill element
	FillInfo (Binder, Check, Info);
      }
    }
  }
}

//=======================================================================
//function : TransferResultInfo
//purpose  : 
//=======================================================================

 void TransferBRep::TransferResultInfo (const Handle(Transfer_FinderProcess)& FP,
					const Handle(TColStd_HSequenceOfInteger)& ShapeTypes,
					Handle(TransferBRep_HSequenceOfTransferResultInfo)& InfoSeq)
{
  // create output Sequence in accordance with required ShapeTypes
  InfoSeq = new TransferBRep_HSequenceOfTransferResultInfo;
  if (FP.IsNull() || ShapeTypes.IsNull()) return;
  Standard_Integer SeqLen = ShapeTypes->Length();
  Standard_Integer i; // svv Jan11 2000 : porting on DEC
  for (i = 1; i <= SeqLen; i++) {
    InfoSeq->Append (new TransferBRep_TransferResultInfo);
  }
  
  // fill Sequence
  Standard_Integer NbMapped = FP->NbMapped();
  for (i = 1; i <= NbMapped; i++) {
    Handle(TransferBRep_ShapeMapper) Mapper = Handle(TransferBRep_ShapeMapper)::DownCast (FP->Mapped (i));
    Handle(Transfer_Binder) Binder = FP->Find (Mapper);
    if (Binder.IsNull()) continue;
    const Handle(Interface_Check) Check = Binder->Check ();
    
    TopoDS_Shape S = Mapper->Value();
    TopAbs_ShapeEnum ShapeType = S.ShapeType();
    
    // find appropriate element in the Sequence
    for (Standard_Integer index = 1; index <= SeqLen; index++) {
//JR/Hp :
      TopAbs_ShapeEnum CurrentType = (TopAbs_ShapeEnum)ShapeTypes->Value (index);
//      TopAbs_ShapeEnum CurrentType = (TopAbs_ShapeEnum)ShapeTypes->Value (index);
      if (CurrentType == ShapeType || CurrentType == TopAbs_SHAPE) {
	Handle(TransferBRep_TransferResultInfo) Info = InfoSeq->Value (index);
	// fill element
	FillInfo (Binder, Check, Info);
      }
    }
  }
}

//  ########  CHECK LOURD  ########

// # # # # # #    Enchainement General du CHECK LOURD    # # # # # #

/*
Interface_CheckIterator TransferBRep::BRepCheck
  (const TopoDS_Shape& shape, const Standard_Integer lev)
{
  Interface_CheckIterator result;
  TransferBRep_Analyzer ana;
  ana.Check (shape,lev);
  return ana.CheckResult ();
}
*/

//  ###  conversion resultat -> starting

    Interface_CheckIterator  TransferBRep::ResultCheckList
  (const Interface_CheckIterator& chl,
   const Handle(Transfer_FinderProcess)& FP,
   const Handle(Interface_InterfaceModel)& model)
{
  Interface_CheckIterator  nchl;
  if (FP.IsNull() || model.IsNull()) return nchl;
  nchl.SetModel(model);
  for (chl.Start(); chl.More(); chl.Next()) {
    Standard_Integer num = 0;
    Handle(Interface_Check) ach = chl.Value();
    if (ach->NbFails() + ach->NbWarnings() == 0) continue;
    DeclareAndCast(Transfer_Finder,starting,ach->Entity());
    Handle(Standard_Transient) ent;
    if (!starting.IsNull()) ent = FP->FindTransient(starting);
    if (!ent.IsNull()) {
      ach->SetEntity(ent);
      num = model->Number(ent);
    }
    nchl.Add (ach,num);
  }
  return nchl;
}

    Handle(TColStd_HSequenceOfTransient)  TransferBRep::Checked
  (const Interface_CheckIterator& chl, const Standard_Boolean alsoshapes)
{
  Handle(TColStd_HSequenceOfTransient) ls = new TColStd_HSequenceOfTransient();
  for (chl.Start(); chl.More(); chl.Next()) {
    const Handle(Interface_Check) ach = chl.Value();
    if (ach->NbFails() + ach->NbWarnings() == 0) continue;
    Handle(Standard_Transient) ent = ach->Entity();
    if (ent.IsNull()) continue;
    if (!alsoshapes) {
      if (ent->IsKind(STANDARD_TYPE(TransferBRep_BinderOfShape)) ||
	  ent->IsKind(STANDARD_TYPE(TopoDS_HShape)) ||
	  ent->IsKind(STANDARD_TYPE(TransferBRep_ShapeMapper)) ) continue;
    }
    ls->Append(ent);
  }
  return ls;
}

    Handle(TopTools_HSequenceOfShape)  TransferBRep::CheckedShapes
  (const Interface_CheckIterator& chl)
{
  Handle(TopTools_HSequenceOfShape) ls = new TopTools_HSequenceOfShape();
  for (chl.Start(); chl.More(); chl.Next()) {
    const Handle(Interface_Check) ach = chl.Value();
    if (ach->NbFails() + ach->NbWarnings() == 0) continue;
    Handle(Standard_Transient) ent = ach->Entity();
    if (ent.IsNull()) continue;
    DeclareAndCast(TopoDS_HShape,hs,ent);
    DeclareAndCast(TransferBRep_BinderOfShape,sb,ent);
    DeclareAndCast(TransferBRep_ShapeMapper,sm,ent);
    if (!hs.IsNull()) ls->Append (hs->Shape());
    if (!sb.IsNull()) ls->Append (sb->Result());
    if (!sm.IsNull()) ls->Append (sm->Value());
  }
  return ls;
}

    Interface_CheckIterator  TransferBRep::CheckObject
  (const Interface_CheckIterator& chl, const Handle(Standard_Transient)& obj)
{
  TopoDS_Shape S;
  DeclareAndCast(TopoDS_HShape,hs,obj);
  DeclareAndCast(TransferBRep_BinderOfShape,sb,obj);
  DeclareAndCast(TransferBRep_ShapeMapper,sm,obj);
  if (!hs.IsNull()) S = hs->Shape();
  if (!sb.IsNull()) S = sb->Result();
  if (!sm.IsNull()) S = sm->Value();
  Interface_CheckIterator nchl;

  for (chl.Start(); chl.More(); chl.Next()) {
    const Handle(Interface_Check) ach = chl.Value();
    if (ach->NbFails() + ach->NbWarnings() == 0) continue;
    Handle(Standard_Transient) ent = ach->Entity();
    if (ent.IsNull()) continue;
    if (S.IsNull()) {
      if (ent == obj) {
	Handle(Interface_Check) bch(ach);  bch->SetEntity(ent);
	nchl.Add (bch,0);
      }
    } else {
      TopoDS_Shape sh;
      DeclareAndCast(TopoDS_HShape,hsh,ent);
      DeclareAndCast(TransferBRep_BinderOfShape,sbs,ent);
      DeclareAndCast(TransferBRep_ShapeMapper,smp,ent);
      if (!hsh.IsNull()) sh = hsh->Shape();
      if (!sbs.IsNull()) sh = sbs->Result();
      if (!smp.IsNull()) sh = smp->Value();
      if (sh == S) {
	Handle(Interface_Check) bch(ach);  bch->SetEntity(ent);
	nchl.Add (bch,0);
      }
    }
  }
  return nchl;
}

//=======================================================================
//function : PrintResultInfo
//purpose  : 
//=======================================================================

void TransferBRep::PrintResultInfo(const Handle(Message_Printer)& Printer,
				   const Message_Msg& Header,
				   const Handle(TransferBRep_TransferResultInfo)& ResultInfo,
				   const Standard_Boolean printEmpty)
{
  Standard_Integer R, RW, RF, RWF, NR, NRW, NRF, NRWF;
  R    = ResultInfo->Result();
  RW   = ResultInfo->ResultWarning();
  RF   = ResultInfo->ResultFail();
  RWF  = ResultInfo->ResultWarningFail();
  NR   = ResultInfo->NoResult();
  NRW  = ResultInfo->NoResultWarning();
  NRF  = ResultInfo->NoResultFail();
  NRWF = ResultInfo->NoResultWarningFail();
  
  Message_Msg aLocalHeader = Header;
  Printer->Send (aLocalHeader, Message_Info, Standard_True);
  
  Message_Msg EPMSG30 ("Result.Print.MSG30"); //    Result: %d
  EPMSG30.Arg (R);
  Printer->Send (EPMSG30, Message_Info, Standard_True);
  if(printEmpty || (RW > 0 )) {
    Message_Msg EPMSG32 ("Result.Print.MSG32"); //    Result + Warning(s): %d
    EPMSG32.Arg (RW);
    Printer->Send (EPMSG32, Message_Info, Standard_True);
  }
  if(printEmpty || (RF > 0 )) {
    Message_Msg EPMSG34 ("Result.Print.MSG34"); //    Result + Fail(s): %d
    EPMSG34.Arg (RF);
    Printer->Send (EPMSG34, Message_Info, Standard_True);
  }
  if(printEmpty || (RWF > 0)) {
    Message_Msg EPMSG36 ("Result.Print.MSG36"); //    Result + Warning(s) + Fail(s): %d
    EPMSG36.Arg (RWF);
    Printer->Send (EPMSG36, Message_Info, Standard_True);
  }
  Message_Msg EPMSG38 ("Result.Print.MSG38"); //    TOTAL Result: %d
  EPMSG38.Arg (R + RW + RF + RWF);
  Printer->Send (EPMSG38, Message_Info, Standard_True);
  if(printEmpty || (NR > 0)) {
    Message_Msg EPMSG40 ("Result.Print.MSG40"); //    No Result: %d
    EPMSG40.Arg (NR);
    Printer->Send (EPMSG40, Message_Info, Standard_True);
  }
  if(printEmpty || (NRW > 0)) {
    Message_Msg EPMSG42 ("Result.Print.MSG42"); //    No Result + Warning(s): %d
    EPMSG42.Arg (NRW);
    Printer->Send (EPMSG42, Message_Info, Standard_True);
  }
  if(printEmpty || (NRF > 0)) {
    Message_Msg EPMSG44 ("Result.Print.MSG44"); //    No Result + Fail(s): %d
    EPMSG44.Arg (NRF);
    Printer->Send (EPMSG44, Message_Info, Standard_True);
  }
  if(printEmpty || (NRWF > 0)) {
    Message_Msg EPMSG46 ("Result.Print.MSG46"); //    No Result + Warning(s) + Fail(s): %d
    EPMSG46.Arg (NRWF);
    Printer->Send (EPMSG46, Message_Info, Standard_True);
  }
  
  Message_Msg EPMSG48 ("Result.Print.MSG48"); //    TOTAL No Result: %d
  EPMSG48.Arg (NR + NRW + NRF + NRWF);
  Printer->Send (EPMSG48, Message_Info, Standard_True);

}
