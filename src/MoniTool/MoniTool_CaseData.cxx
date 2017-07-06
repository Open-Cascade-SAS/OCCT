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


#include <Geom2d_CartesianPoint.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <gp_XY.hxx>
#include <gp_XYZ.hxx>
#include <Message_Msg.hxx>
#include <MoniTool_CaseData.hxx>
#include <OSD_Timer.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TopAbs.hxx>
#include <TopoDS_HShape.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_DataMap.hxx>

IMPLEMENT_STANDARD_RTTIEXT(MoniTool_CaseData,Standard_Transient)

static NCollection_DataMap<TCollection_AsciiString, Standard_Integer> defch;
static NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)> defms;
static Standard_Boolean stachr = Standard_False;

//static OSD_Timer chrono;
// because merdouille link dynamique & perf, ne creer le static qu au 1er usage
static OSD_Timer& chrono() {
  static OSD_Timer chr;
  return chr;
}


    MoniTool_CaseData::MoniTool_CaseData
  (const Standard_CString caseid, const Standard_CString name)
    : thesubst (0) , thecase (caseid) , thename (name)  
      {  thecheck = DefCheck(caseid);  }

    void  MoniTool_CaseData::SetCaseId (const Standard_CString caseid)
      {  thecase.Clear();  thecase.AssignCat (caseid);  thecheck = DefCheck(caseid);  thesubst = 0;  }

    void  MoniTool_CaseData::SetName   (const Standard_CString name)
      {  thename.Clear();  thename.AssignCat (name);  thesubst = 0;  }

    Standard_CString  MoniTool_CaseData::CaseId () const
      {  return thecase.ToCString();  }

    Standard_CString  MoniTool_CaseData::Name   () const
      {  return thename.ToCString();  }


    Standard_Boolean  MoniTool_CaseData::IsCheck   () const
      {  return  (thecheck > 0);  }

    Standard_Boolean  MoniTool_CaseData::IsWarning () const
      {  return (thecheck == 1);  }

    Standard_Boolean  MoniTool_CaseData::IsFail    () const
      {  return (thecheck == 2);  }

    void  MoniTool_CaseData::ResetCheck ()
      {  thecheck = 0;  }

    void  MoniTool_CaseData::SetWarning ()
      {  thecheck = 1;  }

    void  MoniTool_CaseData::SetFail    ()
      {  thecheck = 2;  }

//  ####    DATA    ####

    void  MoniTool_CaseData::SetChange  ()
      {  thesubst = -1;   }

    void  MoniTool_CaseData::SetReplace (const Standard_Integer num)
      {  thesubst = num;  }

    void  MoniTool_CaseData::AddData
  (const Handle(Standard_Transient)& val,
   const Standard_Integer kind, const Standard_CString name)
{
  TCollection_AsciiString aname(name);
  Standard_Integer subs = thesubst;

//  SetChange (calculer la position d apres Name)
  if (thesubst < 0) {
    if (name[0] != '\0') subs = NameNum (name);
  }
//  SetChange / SetReplace
  if (subs > 0 && subs <= thedata.Length()) {
    thedata.SetValue (subs,val);
    thekind.SetValue (subs,kind);
    if (aname.Length() > 0) thednam.SetValue (subs,aname);
//  Ajout Normal
  } else {
    thedata.Append (val);
    thekind.Append (kind);
    thednam.Append (aname);
  }
  thesubst = 0;
}

    void  MoniTool_CaseData::AddRaised (const Handle(Standard_Failure)& theException, const Standard_CString name)
      {  AddData ( theException,1,name);  }

    void  MoniTool_CaseData::AddShape
  (const TopoDS_Shape& sh, const Standard_CString name)
      {  AddData ( new TopoDS_HShape(sh), 4,name);  }

    void  MoniTool_CaseData::AddXYZ
  (const gp_XYZ& aXYZ, const Standard_CString name)
      {  AddData ( new Geom_CartesianPoint (aXYZ), 5,name);  }

    void  MoniTool_CaseData::AddXY
  (const gp_XY&  aXY, const Standard_CString name)
      {  AddData ( new Geom2d_CartesianPoint (aXY), 6,name);  }

    void  MoniTool_CaseData::AddReal
  (const Standard_Real val, const Standard_CString name)
      {  AddData ( new Geom2d_CartesianPoint (val,0.), 8,name);  }

    void  MoniTool_CaseData::AddReals
 (const Standard_Real v1, const Standard_Real v2, const Standard_CString name)
      {  AddData ( new Geom2d_CartesianPoint (v1,v2), 7,name);  }

    void  MoniTool_CaseData::AddCPU
  (const Standard_Real lastCPU, const Standard_Real curCPU,
   const Standard_CString name)
{
  Standard_Real cpu = curCPU;
  if (cpu == 0.) {
    Standard_Real sec;  Standard_Integer i1,i2;
    chrono().Show (sec,i1,i2,cpu);
  }
  cpu = cpu - lastCPU;
  AddData ( new Geom2d_CartesianPoint (cpu,0.), 9,name);
}

    Standard_Real  MoniTool_CaseData::GetCPU () const
{
  if (!stachr) { chrono().Start(); stachr = Standard_True; }
  Standard_Real sec,cpu;  Standard_Integer i1,i2;
  chrono().Show (sec,i1,i2,cpu);
  return cpu;
}

    Standard_Boolean  MoniTool_CaseData::LargeCPU
  (const Standard_Real maxCPU,
   const Standard_Real lastCPU, const Standard_Real curCPU) const
{
  Standard_Real cpu = curCPU;
  if (cpu == 0.) {
    Standard_Real sec;  Standard_Integer i1,i2;
    chrono().Show (sec,i1,i2,cpu);
  }
  cpu = cpu - lastCPU;
  return (cpu >= maxCPU);
}

    void  MoniTool_CaseData::AddGeom
  (const Handle(Standard_Transient)& val, const Standard_CString name)
      {  AddData (val,3,name);  }

    void  MoniTool_CaseData::AddEntity
  (const Handle(Standard_Transient)& val, const Standard_CString name)
      {  AddData (val,2,name);  }

    void  MoniTool_CaseData::AddText
  (const Standard_CString text, const Standard_CString name)
      {  AddData (new TCollection_HAsciiString(text),10,name);  }

    void  MoniTool_CaseData::AddInteger
  (const Standard_Integer val, const Standard_CString name)
{
  Standard_Real rval = val;
  AddData ( new Geom2d_CartesianPoint (rval,0.), 11,name);
}

    void  MoniTool_CaseData::AddAny
  (const Handle(Standard_Transient)& val, const Standard_CString name)
      {  AddData (val,0,name);  }


    void  MoniTool_CaseData::RemoveData (const Standard_Integer num)
{
  if (num < 1 || num > thedata.Length()) return;
  thedata.Remove(num);  thekind.Remove(num);  thednam.Remove(num);
}

//    ####    INTERROGATIONS    ####

    Standard_Integer  MoniTool_CaseData::NbData () const
      {  return thedata.Length();  }

    Handle(Standard_Transient)  MoniTool_CaseData::Data
  (const Standard_Integer nd) const
{
  Handle(Standard_Transient) val;
  if (nd < 1 || nd > thedata.Length()) return val;
  return thedata (nd);
}

Standard_Boolean  MoniTool_CaseData::GetData
  (const Standard_Integer nd, const Handle(Standard_Type)& type,
   Handle(Standard_Transient)& val) const
{
  if (type.IsNull()) return Standard_False;
  if (nd < 1 || nd > thedata.Length()) return Standard_False;
  Handle(Standard_Transient) v = thedata (nd);
  if (v.IsNull()) return Standard_False;
  if (!v->IsKind(type)) return Standard_False;
  val = v;
  return Standard_True;
}

    Standard_Integer  MoniTool_CaseData::Kind
  (const Standard_Integer nd) const
{
  if (nd < 1 || nd > thekind.Length()) return 0;
  return thekind (nd);
}


static  const TCollection_AsciiString&  nulname ()
{
  static TCollection_AsciiString nuln;
  return nuln;
}

    const TCollection_AsciiString&  MoniTool_CaseData::Name
  (const Standard_Integer nd) const
{
  if (nd < 1 || nd > thednam.Length()) return nulname();
  return thednam(nd);
}

static Standard_Integer NameKind (const Standard_CString name)
{
  char n0 = name[0];
  if (n0 == 'A' && name[1] == 'N' && name[2] == 'Y' && name[3] == '\0') return 0;
  if (n0 == 'E') {
    if (name[1] == 'X' && name[2] == '\0') return 1;
    if (name[1] == 'N' && name[2] == '\0') return 2;
    return 0;
  }
  if (n0 == 'G' && name[1] == '\0') return 3;
  if (n0 == 'S' && name[1] == 'H' && name[2] == '\0') return 4;
  if (n0 == 'X' && name[1] == 'Y') {
    if (name[2] == 'Z' && name[3] == '\0') return 5;
    if (name[2] == '\0') return 6;
  }
  if (n0 == 'U' && name[1] == 'V' && name[2] == '\0') return 6;
  if (n0 == 'R') {
    if (name[1] == '\0') return 8;
    if (name[1] == 'R' && name[2] == '\0') return 7;
  }
  if (n0 == 'C' && name[1] == 'P' && name[2] == 'U' && name[3] == '\0') return 9;
  if (n0 == 'T' && name[1] == '\0') return 10;
  if (n0 == 'I' && name[1] == '\0') return 11;

  return 0;
}

static Standard_Integer NameRank (const Standard_CString name)
{
  for (Standard_Integer i = 0; name[i] != '\0'; i ++) {
    if (name[i] == ':' && name[i+1] != '\0') return atoi(&name[i+1]);
  }
  return 1;
}

    Standard_Integer  MoniTool_CaseData::NameNum
  (const Standard_CString name) const
{
  if (!name || name[0] == '\0') return 0;
  Standard_Integer nd, nn = 0, nb = NbData();
  for (nd = 1; nd <= nb; nd ++) {
    if (thednam(nd).IsEqual(name)) return nd;
  }

  Standard_Integer kind = NameKind (name);
  if (kind < 0) return 0;
  Standard_Integer num  = NameRank (name);

  for (nd = 1; nd <= nb; nd ++) {
    if (thekind(nd) == kind) {
      nn ++;
      if (nn == num) return nd;
    }
  }
  return 0;
}


//  ####    RETOUR DES VALEURS    ####

    TopoDS_Shape  MoniTool_CaseData::Shape
  (const Standard_Integer nd) const
{
  TopoDS_Shape sh;
  Handle(TopoDS_HShape) hs = Handle(TopoDS_HShape)::DownCast (Data(nd));
  if (!hs.IsNull()) sh = hs->Shape();
  return sh;
}

    Standard_Boolean  MoniTool_CaseData::XYZ
  (const Standard_Integer nd, gp_XYZ& val) const
{
  Handle(Geom_CartesianPoint) p = Handle(Geom_CartesianPoint)::DownCast(Data(nd));
  if (p.IsNull()) return Standard_False;
  val = p->Pnt().XYZ();
  return Standard_True;
}

    Standard_Boolean   MoniTool_CaseData::XY
  (const Standard_Integer nd, gp_XY& val) const
{
  Handle(Geom2d_CartesianPoint) p = Handle(Geom2d_CartesianPoint)::DownCast(Data(nd));
  if (p.IsNull()) return Standard_False;
  val = p->Pnt2d().XY();
  return Standard_True;
}

    Standard_Boolean   MoniTool_CaseData::Reals
  (const Standard_Integer nd,
   Standard_Real& v1, Standard_Real& v2) const
{
  Handle(Geom2d_CartesianPoint) p = Handle(Geom2d_CartesianPoint)::DownCast(Data(nd));
  if (p.IsNull()) return Standard_False;
  v1 = p->X();  v2 = p->Y();
  return Standard_True;
}


    Standard_Boolean   MoniTool_CaseData::Real
  (const Standard_Integer nd,
   Standard_Real& val) const
{
  Handle(Geom2d_CartesianPoint) p = Handle(Geom2d_CartesianPoint)::DownCast(Data(nd));
  if (p.IsNull()) return Standard_False;
  val = p->X();
  return Standard_True;
}

    Standard_Boolean   MoniTool_CaseData::Text
  (const Standard_Integer nd,
   Standard_CString& text) const
{
  Handle(TCollection_HAsciiString) t = Handle(TCollection_HAsciiString)::DownCast(Data(nd));
  if (t.IsNull()) return Standard_False;
  text = t->ToCString();
  return Standard_True;
}

    Standard_Boolean   MoniTool_CaseData::Integer
  (const Standard_Integer nd,
   Standard_Integer& val) const
{
  Handle(Geom2d_CartesianPoint) p = Handle(Geom2d_CartesianPoint)::DownCast(Data(nd));
//  if (p.IsNull()) return Standard_False;
  if (thekind(nd) != 11) return Standard_False;
  Standard_Real rval = p->X();
  val = (Standard_Integer)rval;
  return Standard_True;
}


//  ####    MESSAGES ET DEFINITIONS    ####

Message_Msg  MoniTool_CaseData::Msg () const
{
  Standard_CString defm = DefMsg (thecase.ToCString());

//  A REPRENDRE COMPLETEMENT !  Il faut analyser defm = mescode + variables
  Message_Msg mess;
  mess.Set (defm);

  return mess;
}


    void  MoniTool_CaseData::SetDefWarning (const Standard_CString acode)
      {  defch.Bind(acode,1);  }

    void  MoniTool_CaseData::SetDefFail (const Standard_CString acode)
      {  defch.Bind(acode,2);  }

    Standard_Integer  MoniTool_CaseData::DefCheck (const Standard_CString acode)
{
  Standard_Integer val;
  if (!defch.Find(acode, val))
    val = 0;
  return val;
}


    void  MoniTool_CaseData::SetDefMsg
  (const Standard_CString casecode, const Standard_CString mesdef)
{
  Handle(TCollection_HAsciiString) str = new TCollection_HAsciiString (mesdef);
  defms.Bind(casecode,str);
}

    Standard_CString MoniTool_CaseData::DefMsg (const Standard_CString casecode)
{
  Handle(Standard_Transient) aTStr;
  if (!defms.Find(casecode, aTStr)) return "";
  Handle(TCollection_HAsciiString) str = Handle(TCollection_HAsciiString)::DownCast(aTStr);
  if (str.IsNull()) return "";
  return str->ToCString();
}
