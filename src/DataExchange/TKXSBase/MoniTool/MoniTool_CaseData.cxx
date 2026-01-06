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
#include <TopoDS_HShape.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_DataMap.hxx>

IMPLEMENT_STANDARD_RTTIEXT(MoniTool_CaseData, Standard_Transient)

static NCollection_DataMap<TCollection_AsciiString, int>                             defch;
static NCollection_DataMap<TCollection_AsciiString, occ::handle<Standard_Transient>> defms;
static bool                                                                          stachr = false;

// static OSD_Timer chrono;
//  because mess of dynamic link & perf, only create the static on 1st usage
static OSD_Timer& chrono()
{
  static OSD_Timer chr;
  return chr;
}

MoniTool_CaseData::MoniTool_CaseData(const char* caseid, const char* name)
    : thesubst(0),
      thecase(caseid),
      thename(name)
{
  thecheck = DefCheck(caseid);
}

void MoniTool_CaseData::SetCaseId(const char* caseid)
{
  thecase.Clear();
  thecase.AssignCat(caseid);
  thecheck = DefCheck(caseid);
  thesubst = 0;
}

void MoniTool_CaseData::SetName(const char* name)
{
  thename.Clear();
  thename.AssignCat(name);
  thesubst = 0;
}

const char* MoniTool_CaseData::CaseId() const
{
  return thecase.ToCString();
}

const char* MoniTool_CaseData::Name() const
{
  return thename.ToCString();
}

bool MoniTool_CaseData::IsCheck() const
{
  return (thecheck > 0);
}

bool MoniTool_CaseData::IsWarning() const
{
  return (thecheck == 1);
}

bool MoniTool_CaseData::IsFail() const
{
  return (thecheck == 2);
}

void MoniTool_CaseData::ResetCheck()
{
  thecheck = 0;
}

void MoniTool_CaseData::SetWarning()
{
  thecheck = 1;
}

void MoniTool_CaseData::SetFail()
{
  thecheck = 2;
}

//  ####    DATA    ####

void MoniTool_CaseData::SetChange()
{
  thesubst = -1;
}

void MoniTool_CaseData::SetReplace(const int num)
{
  thesubst = num;
}

void MoniTool_CaseData::AddData(const occ::handle<Standard_Transient>& val,
                                const int                              kind,
                                const char*                            name)
{
  TCollection_AsciiString aname(name);
  int                     subs = thesubst;

  //  SetChange (calculate position from Name)
  if (thesubst < 0)
  {
    if (name[0] != '\0')
      subs = NameNum(name);
  }
  //  SetChange / SetReplace
  if (subs > 0 && subs <= thedata.Length())
  {
    thedata.SetValue(subs, val);
    thekind.SetValue(subs, kind);
    if (aname.Length() > 0)
      thednam.SetValue(subs, aname);
    //  Ajout Normal
  }
  else
  {
    thedata.Append(val);
    thekind.Append(kind);
    thednam.Append(aname);
  }
  thesubst = 0;
}

void MoniTool_CaseData::AddRaised(const Standard_Failure& theException, const char* name)
{
  // Store exception type and message as text (since Standard_Failure is no longer Standard_Transient)
  TCollection_AsciiString aText(theException.ExceptionType());
  aText += ": ";
  aText += theException.GetMessageString();
  AddText(aText.ToCString(), name);
}

void MoniTool_CaseData::AddShape(const TopoDS_Shape& sh, const char* name)
{
  AddData(new TopoDS_HShape(sh), 4, name);
}

void MoniTool_CaseData::AddXYZ(const gp_XYZ& aXYZ, const char* name)
{
  AddData(new Geom_CartesianPoint(aXYZ), 5, name);
}

void MoniTool_CaseData::AddXY(const gp_XY& aXY, const char* name)
{
  AddData(new Geom2d_CartesianPoint(aXY), 6, name);
}

void MoniTool_CaseData::AddReal(const double val, const char* name)
{
  AddData(new Geom2d_CartesianPoint(val, 0.), 8, name);
}

void MoniTool_CaseData::AddReals(const double v1, const double v2, const char* name)
{
  AddData(new Geom2d_CartesianPoint(v1, v2), 7, name);
}

void MoniTool_CaseData::AddCPU(const double lastCPU, const double curCPU, const char* name)
{
  double cpu = curCPU;
  if (cpu == 0.)
  {
    double sec;
    int    i1, i2;
    chrono().Show(sec, i1, i2, cpu);
  }
  cpu = cpu - lastCPU;
  AddData(new Geom2d_CartesianPoint(cpu, 0.), 9, name);
}

double MoniTool_CaseData::GetCPU() const
{
  if (!stachr)
  {
    chrono().Start();
    stachr = true;
  }
  double sec, cpu;
  int    i1, i2;
  chrono().Show(sec, i1, i2, cpu);
  return cpu;
}

bool MoniTool_CaseData::LargeCPU(const double maxCPU,
                                 const double lastCPU,
                                 const double curCPU) const
{
  double cpu = curCPU;
  if (cpu == 0.)
  {
    double sec;
    int    i1, i2;
    chrono().Show(sec, i1, i2, cpu);
  }
  cpu = cpu - lastCPU;
  return (cpu >= maxCPU);
}

void MoniTool_CaseData::AddGeom(const occ::handle<Standard_Transient>& val, const char* name)
{
  AddData(val, 3, name);
}

void MoniTool_CaseData::AddEntity(const occ::handle<Standard_Transient>& val, const char* name)
{
  AddData(val, 2, name);
}

void MoniTool_CaseData::AddText(const char* text, const char* name)
{
  AddData(new TCollection_HAsciiString(text), 10, name);
}

void MoniTool_CaseData::AddInteger(const int val, const char* name)
{
  double rval = val;
  AddData(new Geom2d_CartesianPoint(rval, 0.), 11, name);
}

void MoniTool_CaseData::AddAny(const occ::handle<Standard_Transient>& val, const char* name)
{
  AddData(val, 0, name);
}

void MoniTool_CaseData::RemoveData(const int num)
{
  if (num < 1 || num > thedata.Length())
    return;
  thedata.Remove(num);
  thekind.Remove(num);
  thednam.Remove(num);
}

//    ####    INTERROGATIONS    ####

int MoniTool_CaseData::NbData() const
{
  return thedata.Length();
}

occ::handle<Standard_Transient> MoniTool_CaseData::Data(const int nd) const
{
  occ::handle<Standard_Transient> val;
  if (nd < 1 || nd > thedata.Length())
    return val;
  return thedata(nd);
}

bool MoniTool_CaseData::GetData(const int                         nd,
                                const occ::handle<Standard_Type>& type,
                                occ::handle<Standard_Transient>&  val) const
{
  if (type.IsNull())
    return false;
  if (nd < 1 || nd > thedata.Length())
    return false;
  occ::handle<Standard_Transient> v = thedata(nd);
  if (v.IsNull())
    return false;
  if (!v->IsKind(type))
    return false;
  val = v;
  return true;
}

int MoniTool_CaseData::Kind(const int nd) const
{
  if (nd < 1 || nd > thekind.Length())
    return 0;
  return thekind(nd);
}

const TCollection_AsciiString& MoniTool_CaseData::Name(const int nd) const
{
  if (nd < 1 || nd > thednam.Length())
    return TCollection_AsciiString::EmptyString();
  return thednam(nd);
}

static int NameKind(const char* name)
{
  char n0 = name[0];
  if (n0 == 'A' && name[1] == 'N' && name[2] == 'Y' && name[3] == '\0')
    return 0;
  if (n0 == 'E')
  {
    if (name[1] == 'X' && name[2] == '\0')
      return 1;
    if (name[1] == 'N' && name[2] == '\0')
      return 2;
    return 0;
  }
  if (n0 == 'G' && name[1] == '\0')
    return 3;
  if (n0 == 'S' && name[1] == 'H' && name[2] == '\0')
    return 4;
  if (n0 == 'X' && name[1] == 'Y')
  {
    if (name[2] == 'Z' && name[3] == '\0')
      return 5;
    if (name[2] == '\0')
      return 6;
  }
  if (n0 == 'U' && name[1] == 'V' && name[2] == '\0')
    return 6;
  if (n0 == 'R')
  {
    if (name[1] == '\0')
      return 8;
    if (name[1] == 'R' && name[2] == '\0')
      return 7;
  }
  if (n0 == 'C' && name[1] == 'P' && name[2] == 'U' && name[3] == '\0')
    return 9;
  if (n0 == 'T' && name[1] == '\0')
    return 10;
  if (n0 == 'I' && name[1] == '\0')
    return 11;

  return 0;
}

static int NameRank(const char* name)
{
  for (int i = 0; name[i] != '\0'; i++)
  {
    if (name[i] == ':' && name[i + 1] != '\0')
      return atoi(&name[i + 1]);
  }
  return 1;
}

int MoniTool_CaseData::NameNum(const char* name) const
{
  if (!name || name[0] == '\0')
    return 0;
  int nd, nn = 0, nb = NbData();
  for (nd = 1; nd <= nb; nd++)
  {
    if (thednam(nd).IsEqual(name))
      return nd;
  }

  int kind = NameKind(name);
  if (kind < 0)
    return 0;
  int num = NameRank(name);

  for (nd = 1; nd <= nb; nd++)
  {
    if (thekind(nd) == kind)
    {
      nn++;
      if (nn == num)
        return nd;
    }
  }
  return 0;
}

//  ####    RETURN OF VALUES    ####

TopoDS_Shape MoniTool_CaseData::Shape(const int nd) const
{
  TopoDS_Shape               sh;
  occ::handle<TopoDS_HShape> hs = occ::down_cast<TopoDS_HShape>(Data(nd));
  if (!hs.IsNull())
    sh = hs->Shape();
  return sh;
}

bool MoniTool_CaseData::XYZ(const int nd, gp_XYZ& val) const
{
  occ::handle<Geom_CartesianPoint> p = occ::down_cast<Geom_CartesianPoint>(Data(nd));
  if (p.IsNull())
    return false;
  val = p->Pnt().XYZ();
  return true;
}

bool MoniTool_CaseData::XY(const int nd, gp_XY& val) const
{
  occ::handle<Geom2d_CartesianPoint> p = occ::down_cast<Geom2d_CartesianPoint>(Data(nd));
  if (p.IsNull())
    return false;
  val = p->Pnt2d().XY();
  return true;
}

bool MoniTool_CaseData::Reals(const int nd, double& v1, double& v2) const
{
  occ::handle<Geom2d_CartesianPoint> p = occ::down_cast<Geom2d_CartesianPoint>(Data(nd));
  if (p.IsNull())
    return false;
  v1 = p->X();
  v2 = p->Y();
  return true;
}

bool MoniTool_CaseData::Real(const int nd, double& val) const
{
  occ::handle<Geom2d_CartesianPoint> p = occ::down_cast<Geom2d_CartesianPoint>(Data(nd));
  if (p.IsNull())
    return false;
  val = p->X();
  return true;
}

bool MoniTool_CaseData::Text(const int nd, const char*& text) const
{
  occ::handle<TCollection_HAsciiString> t = occ::down_cast<TCollection_HAsciiString>(Data(nd));
  if (t.IsNull())
    return false;
  text = t->ToCString();
  return true;
}

bool MoniTool_CaseData::Integer(const int nd, int& val) const
{
  occ::handle<Geom2d_CartesianPoint> p = occ::down_cast<Geom2d_CartesianPoint>(Data(nd));
  //  if (p.IsNull()) return false;
  if (thekind(nd) != 11)
    return false;
  double rval = p->X();
  val         = (int)rval;
  return true;
}

//  ####    MESSAGES AND DEFINITIONS    ####

Message_Msg MoniTool_CaseData::Msg() const
{
  const char* defm = DefMsg(thecase.ToCString());

  //  A REPRENDRE COMPLETEMENT !  Il faut analyser defm = mescode + variables
  Message_Msg mess;
  mess.Set(defm);

  return mess;
}

void MoniTool_CaseData::SetDefWarning(const char* acode)
{
  defch.Bind(acode, 1);
}

void MoniTool_CaseData::SetDefFail(const char* acode)
{
  defch.Bind(acode, 2);
}

int MoniTool_CaseData::DefCheck(const char* acode)
{
  int val;
  if (!defch.Find(acode, val))
    val = 0;
  return val;
}

void MoniTool_CaseData::SetDefMsg(const char* casecode, const char* mesdef)
{
  occ::handle<TCollection_HAsciiString> str = new TCollection_HAsciiString(mesdef);
  defms.Bind(casecode, str);
}

const char* MoniTool_CaseData::DefMsg(const char* casecode)
{
  occ::handle<Standard_Transient> aTStr;
  if (!defms.Find(casecode, aTStr))
    return "";
  occ::handle<TCollection_HAsciiString> str = occ::down_cast<TCollection_HAsciiString>(aTStr);
  if (str.IsNull())
    return "";
  return str->ToCString();
}
