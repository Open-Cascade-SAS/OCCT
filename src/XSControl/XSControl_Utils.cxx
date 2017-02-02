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
#include <BRepTools.hxx>
#include <Interface_HArray1OfHAsciiString.hxx>
#include <Interface_Macros.hxx>
#include <Interface_MSG.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Transient.hxx>
#include <Standard_TypeMismatch.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TCollection_HExtendedString.hxx>
#include <TColStd_HArray1OfTransient.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>
#include <TColStd_HSequenceOfExtendedString.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_HShape.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Wire.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <TransferBRep.hxx>
#include <TransferBRep_ShapeBinder.hxx>
#include <TransferBRep_ShapeMapper.hxx>
#include <XSControl_Utils.hxx>

static TCollection_AsciiString    bufasc;
static TCollection_ExtendedString bufext;
static const Standard_ExtString   voidext = { 0 };


    XSControl_Utils::XSControl_Utils ()    {  }

//  #########################################################
//  #######           Fonctions  de   TRACE           #######

    void  XSControl_Utils::TraceLine (const Standard_CString line) const
{
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  sout<<line<<endl;
}

    void  XSControl_Utils::TraceLines (const Handle(Standard_Transient)& lines) const
{
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  Standard_Integer i,nb;
  DeclareAndCast(TColStd_HSequenceOfHAsciiString,linha,lines);
  if (!linha.IsNull()) {
    nb = linha->Length();
    for (i = 1; i <= nb; i ++)
      if (!linha->Value(i).IsNull()) sout<<linha->Value(i)->ToCString()<<endl;
    return;
  }
  DeclareAndCast(TColStd_HSequenceOfAsciiString,lina,lines);
  if (!lina.IsNull()) {
    nb = lina->Length();
    for (i = 1; i <= nb; i ++)
      sout<<lina->Value(i).ToCString()<<endl;
    return;
  }
  DeclareAndCast(TColStd_HSequenceOfHExtendedString,linhe,lines);
  if (!linhe.IsNull()) {
    nb = linhe->Length();
    for (i = 1; i <= nb; i ++)
      if (!linhe->Value(i).IsNull())
	sout << linhe->Value(i) << endl;
    return;
  }
  DeclareAndCast(TColStd_HSequenceOfExtendedString,linee,lines);
  if (!linee.IsNull()) {
    nb = linee->Length();
    for (i = 1; i <= nb; i ++)
      sout << linee->Value(i) << endl;
    return;
  }
  DeclareAndCast(TCollection_HAsciiString,lin1a,lines);
  if (!lin1a.IsNull()) sout<<lin1a;
  DeclareAndCast(TCollection_HExtendedString,lin1e,lines);
  if (!lin1e.IsNull()) sout<<lin1e;
}


//  #########################################################
//  #######   TRANSIENT : Quelques  acces  de  base   #######

    Standard_Boolean  XSControl_Utils::IsKind
  (const Handle(Standard_Transient)& item, const Handle(Standard_Type)& what) const
{
  if (item.IsNull()) return Standard_False;
  if (what.IsNull()) return Standard_False;
  return item->IsKind(what);
}

    Standard_CString  XSControl_Utils::TypeName
  (const Handle(Standard_Transient)& item, const Standard_Boolean nopk) const
{
  if (item.IsNull()) return "";
  DeclareAndCast(Standard_Type,atype,item);
  if (atype.IsNull()) atype = item->DynamicType();
  Standard_CString tn = atype->Name();
  if (!nopk) return tn;
  for (int i = 0; tn[i] != '\0'; i ++) {
    if (tn[i] == '_') return &tn[i+1];
  }
  return tn;
}


//  #######       TRANSIENT : Fonctions de liste       #######

    Handle(Standard_Transient)  XSControl_Utils::TraValue
  (const Handle(Standard_Transient)& seqval, const Standard_Integer num) const
{
  Handle(Standard_Transient) val;
  if (num < 1) return val;
  if (seqval.IsNull()) return val;
  DeclareAndCast(TColStd_HSequenceOfHAsciiString,seqs,seqval);
  if (!seqs.IsNull())
    {  if (num <= seqs->Length()) val = seqs->Value(num);  return val;  }
  DeclareAndCast(TColStd_HSequenceOfTransient,seqt,seqval);
  if (!seqt.IsNull())
    {  if (num <= seqt->Length()) val = seqt->Value(num);  return val;  }
//  throw Standard_TypeMismatch("XSControl_Utils::SeqTraValue");
  return val;
}

    Handle(TColStd_HSequenceOfTransient) XSControl_Utils::NewSeqTra () const
      {  return  new TColStd_HSequenceOfTransient();  }

    void  XSControl_Utils::AppendTra
  (const Handle(TColStd_HSequenceOfTransient)& seqval,
   const Handle(Standard_Transient)& traval) const
      {  seqval->Append (traval);  }


//  #######           DATES           #######

    Standard_CString  XSControl_Utils::DateString
  (const Standard_Integer yy, const Standard_Integer mm,
   const Standard_Integer dd, const Standard_Integer hh,
   const Standard_Integer mn, const Standard_Integer ss) const
{
  char ladate[50];
  Interface_MSG::TDate (ladate, yy,mm,dd,hh,mn,ss);
  bufasc.Clear();  bufasc.AssignCat(ladate);
  return bufasc.ToCString();
}

    void  XSControl_Utils::DateValues
  (const Standard_CString text,
   Standard_Integer& yy, Standard_Integer& mm, Standard_Integer& dd,
   Standard_Integer& hh, Standard_Integer& mn, Standard_Integer& ss) const
      {  Interface_MSG::NDate (text, yy,mm,dd,hh,mn,ss);  }


//  ##########################################################
//  #######           STRING : Ascii de base           #######

    Standard_CString  XSControl_Utils::ToCString
  (const Handle(TCollection_HAsciiString)& strval) const
      {
//JR/Hp
         Standard_CString astr = (Standard_CString ) (strval.IsNull() ? "" : strval->ToCString());
         return astr ;
//         return (strval.IsNull() ? "" : strval->ToCString());
      }

    Standard_CString  XSControl_Utils::ToCString
  (const TCollection_AsciiString& strval) const
      {  return strval.ToCString();  }

    Handle(TCollection_HAsciiString)  XSControl_Utils::ToHString
  (const Standard_CString strcon) const
      {  return new TCollection_HAsciiString(strcon);  }

    TCollection_AsciiString  XSControl_Utils::ToAString
  (const Standard_CString strcon) const
      {  return TCollection_AsciiString(strcon);  }

//  #######         STRING : Extended de base         #######

    Standard_ExtString  XSControl_Utils::ToEString
  (const Handle(TCollection_HExtendedString)& strval) const
      {  return (strval.IsNull() ? voidext : strval->ToExtString());  }

    Standard_ExtString  XSControl_Utils::ToEString
  (const TCollection_ExtendedString& strval) const
      {  return strval.ToExtString();  }

    Handle(TCollection_HExtendedString)  XSControl_Utils::ToHString
  (const Standard_ExtString strcon) const
      {  return new TCollection_HExtendedString(strcon);  }

    TCollection_ExtendedString  XSControl_Utils::ToXString
  (const Standard_ExtString strcon) const
      {  return TCollection_ExtendedString(strcon);  }

//  #######        STRING : Ascii <-> Extended        #######

    Standard_ExtString  XSControl_Utils::AsciiToExtended (const Standard_CString str) const
{
  bufext.Clear();  bufext = TCollection_ExtendedString (str);
  return bufext.ToExtString();
}

    Standard_Boolean  XSControl_Utils::IsAscii (const Standard_ExtString str) const
{
  bufext.Clear();  bufext.AssignCat (str);
  return bufext.IsAscii();
}

    Standard_CString  XSControl_Utils::ExtendedToAscii (const Standard_ExtString str) const
{
  bufext.Clear();  bufext.AssignCat (str);  bufasc.Clear();
  Standard_Integer i, nb = bufext.Length();
  for (i = 1; i <= nb; i ++) {
    int unext = bufext.Value(i);
    unext = unext & 127;
    char uncar = char(unext);
    bufasc.AssignCat (uncar);
  }
  return bufasc.ToCString();
}

//  #######              STRING : LISTES              #######

    Standard_CString  XSControl_Utils::CStrValue
  (const Handle(Standard_Transient)& list, const Standard_Integer num) const
{
  DeclareAndCast(TColStd_HSequenceOfHAsciiString,linha,list);
  if (!linha.IsNull()) {
//JR/Hp
    Standard_CString astr = (Standard_CString )
    (num > linha->Length() ? "" : linha->Value(num)->ToCString());
    return astr ;
//    return (num > linha->Length() ? "" : linha->Value(num)->ToCString());
  }

  DeclareAndCast(TColStd_HSequenceOfAsciiString,lina,list);
  if (!lina.IsNull()) {
//JR/Hp
    Standard_CString astr = (Standard_CString ) 
    (num > lina->Length() ? "" : lina->Value(num).ToCString());  
    return astr ;
//    return (num > lina->Length() ? "" : lina->Value(num).ToCString());  
  }

  DeclareAndCast(TColStd_HSequenceOfHExtendedString,linhe,list);
  if (!linhe.IsNull()) {
//JR/Hp
    Standard_CString astr = (Standard_CString ) 
    (num > linhe->Length() ? "" : ExtendedToAscii(linhe->Value(num)->ToExtString()));  
    return astr ;
//   return (num > linhe->Length() ? "" : ExtendedToAscii(linhe->Value(num)->ToExtString()));  
  }

  DeclareAndCast(TColStd_HSequenceOfExtendedString,linee,list);
  if (!linee.IsNull()) {
//JR/Hp
    Standard_CString astr = (Standard_CString ) 
    (num > linee->Length() ? "" : ExtendedToAscii(linee->Value(num).ToExtString()));  
    return astr ;
//    return (num > linee->Length() ? "" : ExtendedToAscii(linee->Value(num).T
  }

  DeclareAndCast(TCollection_HAsciiString,lin1a,list);
  if (!lin1a.IsNull()) return lin1a->ToCString();
  DeclareAndCast(TCollection_HExtendedString,lin1e,list);
  if (!lin1e.IsNull()) return ExtendedToAscii(lin1e->ToExtString());
  return "";
}


    Standard_ExtString  XSControl_Utils::EStrValue
  (const Handle(Standard_Transient)& list, const Standard_Integer num) const
{
  DeclareAndCast(TColStd_HSequenceOfHAsciiString,linha,list);
  if (!linha.IsNull()) return
    (num > linha->Length() ? voidext : AsciiToExtended(linha->Value(num)->ToCString()));

  DeclareAndCast(TColStd_HSequenceOfAsciiString,lina,list);
  if (!lina.IsNull())
    (num > lina->Length() ? voidext : AsciiToExtended(lina->Value(num).ToCString()));

  DeclareAndCast(TColStd_HSequenceOfHExtendedString,linhe,list);
  if (!linhe.IsNull()) return
    (num > linhe->Length() ? voidext : linhe->Value(num)->ToExtString());

  DeclareAndCast(TColStd_HSequenceOfExtendedString,linee,list);
  if (!linee.IsNull()) return
    (num > linee->Length() ? voidext : linee->Value(num).ToExtString());

  DeclareAndCast(TCollection_HAsciiString,lin1a,list);
  if (!lin1a.IsNull()) return AsciiToExtended(lin1a->ToCString());
  DeclareAndCast(TCollection_HExtendedString,lin1e,list);
  if (!lin1e.IsNull()) return lin1e->ToExtString();
  return voidext;
}

    Handle(TColStd_HSequenceOfHAsciiString) XSControl_Utils::NewSeqCStr () const
      {  return  new TColStd_HSequenceOfHAsciiString();  }

    void  XSControl_Utils::AppendCStr
  (const Handle(TColStd_HSequenceOfHAsciiString)& seqval,
   const Standard_CString strval) const
      {  seqval->Append (new TCollection_HAsciiString(strval));  }

    Handle(TColStd_HSequenceOfHExtendedString) XSControl_Utils::NewSeqEStr () const
      {  return  new TColStd_HSequenceOfHExtendedString();  }

    void  XSControl_Utils::AppendEStr
  (const Handle(TColStd_HSequenceOfHExtendedString)& seqval,
   const Standard_ExtString strval) const
      {  seqval->Append (new TCollection_HExtendedString(strval));  }


//  ##########################################################
//  #######           SHAPES : Acces de base           #######

    TopoDS_Shape  XSControl_Utils::CompoundFromSeq
  (const Handle(TopTools_HSequenceOfShape)& seqval) const
{
  BRep_Builder B;
  TopoDS_Compound C;
  B.MakeCompound(C);
  Standard_Integer i,n = seqval->Length();
  for (i = 1; i <= n ; i ++)  B.Add(C,seqval->Value(i));
  return C;
}

    TopAbs_ShapeEnum  XSControl_Utils::ShapeType
  (const TopoDS_Shape& shape, const Standard_Boolean compound) const
{
  if (shape.IsNull()) return TopAbs_SHAPE;
  TopAbs_ShapeEnum res = shape.ShapeType();
  if (!compound || res != TopAbs_COMPOUND) return res;
  res = TopAbs_SHAPE;
  for (TopoDS_Iterator iter(shape); iter.More(); iter.Next()) {
    TopoDS_Shape sh = iter.Value();
    if (sh.IsNull()) continue;
    TopAbs_ShapeEnum typ = sh.ShapeType();
    if (typ == TopAbs_COMPOUND) typ = ShapeType (sh,compound);
    if (res == TopAbs_SHAPE) res = typ;
//   Egalite : OK;  Pseudo-Egalite : EDGE/WIRE ou FACE/SHELL
    else if (res == TopAbs_EDGE  && typ == TopAbs_WIRE)  res = typ;
    else if (res == TopAbs_WIRE  && typ == TopAbs_EDGE)  continue;
    else if (res == TopAbs_FACE  && typ == TopAbs_SHELL) res = typ;
    else if (res == TopAbs_SHELL && typ == TopAbs_FACE)  continue;
    else if (res != typ) return TopAbs_COMPOUND;
  }
  return res;
}

    TopoDS_Shape  XSControl_Utils::SortedCompound
  (const TopoDS_Shape& shape, const TopAbs_ShapeEnum type,
   const Standard_Boolean explore, const Standard_Boolean compound) const
{
  if (shape.IsNull()) return shape;
  TopAbs_ShapeEnum typ = shape.ShapeType();
  TopoDS_Shape sh, sh0;
  Standard_Integer nb = 0;

//  Compound : on le prend, soit tel quel, soit son contenu
  if (typ == TopAbs_COMPOUND || typ == TopAbs_COMPSOLID) {
    TopoDS_Compound C;
    BRep_Builder B;
    B.MakeCompound (C);
    for (TopoDS_Iterator it(shape); it.More(); it.Next()) {
      sh0 = SortedCompound (it.Value(),type,explore,compound);
      if (sh0.IsNull()) continue;
      sh = sh0;
      typ = sh.ShapeType();
      if (typ == TopAbs_COMPOUND && !compound) {
	for (TopoDS_Iterator it2 (sh); it2.More(); it2.Next())
	  {  nb ++;  sh = it2.Value();  B.Add (C, sh);  }
      }
      else  {  nb ++;  B.Add (C,sh);  }
    }
    if (nb == 0) C.Nullify();
    else if (nb == 1) return sh;
    return C;
  }

//   Egalite : OK;  Pseudo-Egalite : EDGE/WIRE ou FACE/SHELL
  if (typ == type) return shape;
  if (typ == TopAbs_EDGE && type == TopAbs_WIRE) {
    BRep_Builder B;
    TopoDS_Wire W;
    B.MakeWire (W);
    B.Add (W,shape);  // ne passe pas ! : TopoDS::Edge(shape)
    return W;
  }
  if (typ == TopAbs_FACE && type == TopAbs_SHELL) {
    BRep_Builder B;
    TopoDS_Shell S;
    B.MakeShell (S);
    B.Add (S,shape);  // ne passe pas ! : TopoDS::Face(shape));
    S.Closed (BRep_Tool::IsClosed (S));
    return S;
  }

//   Le reste : selon exploration
  if (!explore) {
    TopoDS_Shape nulsh;
    return nulsh;
  }

//  Ici, on doit explorer
//  SOLID + mode COMPOUND : reconduire les SHELLs
  if (typ == TopAbs_SOLID && compound) {
    TopoDS_Compound C;
    BRep_Builder B;
    B.MakeCompound (C);
    for (TopoDS_Iterator it(shape); it.More(); it.Next()) {
      sh0 = SortedCompound (it.Value(),type,explore,compound);
      if (sh0.IsNull()) continue;
      sh = sh0;
      nb ++;  B.Add (C,sh);
    }
    if (nb == 0) C.Nullify();
    else if (nb == 1) return sh;
    return C;
  }

//  Exploration classique
  TopoDS_Compound CC;
  BRep_Builder BB;
  BB.MakeCompound(CC);
  for (TopExp_Explorer expl (shape,type); expl.More(); expl.Next()) {
    nb ++;  sh = expl.Current();
    BB.Add (CC,sh);
  }
  if (nb == 0) CC.Nullify();
  else if (nb == 1) return sh;
  return CC;
}


//  #######               SHAPES : Liste               #######

    TopoDS_Shape  XSControl_Utils::ShapeValue
  (const Handle(TopTools_HSequenceOfShape)& seqval, const Standard_Integer num) const
{
  TopoDS_Shape shape;
  if (seqval.IsNull()) return shape;
  if (num > 0 && num <= seqval->Length()) shape = seqval->Value(num);
  return shape;
}

    Handle(TopTools_HSequenceOfShape) XSControl_Utils::NewSeqShape () const
      {  return  new TopTools_HSequenceOfShape();  }

    void  XSControl_Utils::AppendShape
  (const Handle(TopTools_HSequenceOfShape)& seqval, const TopoDS_Shape& shape) const
      {  seqval->Append (shape);  }

//  #######            SHAPES <-> Transient            #######

    Handle(Standard_Transient)  XSControl_Utils::ShapeBinder
  (const TopoDS_Shape& shape, const Standard_Boolean hs) const
      {  if (hs) return new TopoDS_HShape            (shape);
         else    return new TransferBRep_ShapeBinder (shape);  }

    TopoDS_Shape  XSControl_Utils::BinderShape
  (const Handle(Standard_Transient)& tr) const
{
  TopoDS_Shape sh;
  DeclareAndCast(Transfer_Binder,sb,tr);
  if (!sb.IsNull()) return TransferBRep::ShapeResult(sb);
  DeclareAndCast(TransferBRep_ShapeMapper,sm,tr);
  if (!sm.IsNull()) return sm->Value();
  DeclareAndCast(TopoDS_HShape,hs,tr);
  if (!hs.IsNull()) return hs->Shape();
  return sh;
}


//  ##########################################################
//  #######        LISTES : Fonctions Generales        #######

    Standard_Integer  XSControl_Utils::SeqLength
  (const Handle(Standard_Transient)& seqval) const
{
  if (seqval.IsNull()) return 0;
  DeclareAndCast(TColStd_HSequenceOfHAsciiString,seqs,seqval);
  if (!seqs.IsNull()) return seqs->Length();
  DeclareAndCast(TColStd_HSequenceOfAsciiString,seqa,seqval);
  if (!seqa.IsNull()) return seqa->Length();
  DeclareAndCast(TColStd_HSequenceOfHExtendedString,seqe,seqval);
  if (!seqe.IsNull()) return seqe->Length();
  DeclareAndCast(TColStd_HSequenceOfHExtendedString,seqx,seqval);
  if (!seqx.IsNull()) return seqx->Length();

  DeclareAndCast(TColStd_HSequenceOfTransient,seqt,seqval);
  if (!seqt.IsNull()) return seqt->Length();
  DeclareAndCast(TopTools_HSequenceOfShape,seqh,seqval);
  if (!seqh.IsNull()) return seqh->Length();
  DeclareAndCast(TColStd_HSequenceOfInteger,seqi,seqval);
  if (!seqi.IsNull()) return seqi->Length();
//  throw Standard_TypeMismatch("XSControl_Utils::SeqLength");
  return 0;
}

    Handle(Standard_Transient)  XSControl_Utils::SeqToArr
  (const Handle(Standard_Transient)& seqval, const Standard_Integer first) const
{
  Standard_Integer i,lng;
  Handle(Standard_Transient) val;
  if (seqval.IsNull()) return val;
  DeclareAndCast(TColStd_HSequenceOfHAsciiString,seqs,seqval);
  if (!seqs.IsNull()) {
    lng = seqs->Length();
    Handle(Interface_HArray1OfHAsciiString) arrs =
      new Interface_HArray1OfHAsciiString(first,lng-first+1);
    for (i = 1; i <= lng; i ++)  arrs->SetValue (i-first+1,seqs->Value(i));
    return arrs;
  }
  DeclareAndCast(TColStd_HSequenceOfTransient,seqt,seqval);
  if (!seqt.IsNull()) {
    lng = seqt->Length();
    Handle(TColStd_HArray1OfTransient) arrt =
      new TColStd_HArray1OfTransient(first,lng-first+1);
    for (i = 1; i <= lng; i ++)  arrt->SetValue (i-first+1,seqt->Value(i));
    return arrt;
  }
  throw Standard_TypeMismatch("XSControl_Utils::SeqToArr");
}


    Handle(Standard_Transient)  XSControl_Utils::ArrToSeq
  (const Handle(Standard_Transient)& arrval) const
{
  Standard_Integer i,first,last;
  Handle(Standard_Transient) val;
  if (arrval.IsNull()) return val;
  DeclareAndCast(Interface_HArray1OfHAsciiString,arrs,arrval);
  if (!arrs.IsNull()) {
    first = arrs->Lower();
    last  = arrs->Upper();
    Handle(TColStd_HSequenceOfHAsciiString) seqs =
      new TColStd_HSequenceOfHAsciiString();
    for (i = first; i <= last; i ++)  seqs->Append (arrs->Value(i));
    return seqs;
  }
  DeclareAndCast(TColStd_HArray1OfTransient,arrt,arrval);
  if (!arrt.IsNull()) {
    first = arrt->Lower();
    last  = arrt->Upper();
    Handle(TColStd_HSequenceOfTransient) seqt =
      new TColStd_HSequenceOfTransient();
    for (i = first; i <= last; i ++)  seqt->Append (arrt->Value(i));
    return seqt;
  }
  throw Standard_TypeMismatch("XSControl_Utils::ArrToSeq");
}


    Standard_Integer  XSControl_Utils::SeqIntValue
  (const Handle(TColStd_HSequenceOfInteger)& seqval,
   const Standard_Integer num) const
{
  if (seqval.IsNull()) return 0;
  return seqval->Value(num);
}
