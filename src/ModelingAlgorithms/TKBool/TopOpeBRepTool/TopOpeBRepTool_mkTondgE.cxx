// Created on: 1999-03-23
// Created by: Xuan PHAM PHU
// Copyright (c) 1999-1999 Matra Datavision
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

#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopOpeBRepTool_GEOMETRY.hxx>
#include <TopOpeBRepTool_PROJECT.hxx>
#include <TopOpeBRepTool_TOPOLOGY.hxx>
#include <TopOpeBRepTool_mkTondgE.hxx>
#include <TopOpeBRepTool_TOOL.hxx>

#define M_FORWARD(sta) (sta == TopAbs_FORWARD)
#define M_REVERSED(sta) (sta == TopAbs_REVERSED)
#define M_INTERNAL(sta) (sta == TopAbs_INTERNAL)
#define M_EXTERNAL(sta) (sta == TopAbs_EXTERNAL)

#define FORWARD (1)
#define REVERSED (2)
#define INTERNAL (3)
#define EXTERNAL (4)
#define CLOSING (5)

#define NOI (0)
#define MKI1 (1)
#define MKI2 (2)
#define MKI12 (3)

static double FUN_tola()
{
  double tola = Precision::Angular();
  return tola;
}

//=================================================================================================

TopOpeBRepTool_mkTondgE::TopOpeBRepTool_mkTondgE() {}

//=================================================================================================

bool TopOpeBRepTool_mkTondgE::Initialize(const TopoDS_Edge& dgE,
                                                     const TopoDS_Face& F,
                                                     const gp_Pnt2d&    uvi,
                                                     const TopoDS_Face& Fi)
{
  isT2d   = false;
  hasRest = false;
  myclE.Nullify();
  myEpari.Clear();

  mydgE = dgE;
  myF   = F;

  TopExp_Explorer      exv(mydgE, TopAbs_VERTEX);
  const TopoDS_Vertex& v   = TopoDS::Vertex(exv.Current());
  double        par = BRep_Tool::Parameter(v, mydgE);
  gp_Pnt2d             uv;
  bool     ok = FUN_tool_paronEF(mydgE, par, myF, uv);
  if (!ok)
    return false;
  gp_Vec tmp;
  ok    = TopOpeBRepTool_TOOL::NggeomF(uv, myF, tmp);
  myngf = gp_Dir(tmp);
  if (!ok)
    return false;

  myuvi                = uvi;
  myFi                 = Fi;
  bool oki = TopOpeBRepTool_TOOL::NggeomF(myuvi, myFi, tmp);
  myngfi               = gp_Dir(tmp);
  if (!oki)
    return false;

  double dot = myngf.Dot(myngfi);
  isT2d             = (std::abs(1 - std::abs(dot)) < FUN_tola());
  return true;
}

//=================================================================================================

bool TopOpeBRepTool_mkTondgE::SetclE(const TopoDS_Edge& clE)
{
  myclE = clE;
  return true;
}

//=================================================================================================

bool TopOpeBRepTool_mkTondgE::IsT2d() const
{
  return isT2d;
}

//=================================================================================================

bool TopOpeBRepTool_mkTondgE::SetRest(const double pari, const TopoDS_Edge& Ei)
{
  hasRest               = true;
  bool clEi = TopOpeBRepTool_TOOL::IsClosingE(Ei, myFi);
  if (clEi)
  {
    hasRest = false;
    return false;
  }

  myEpari.Bind(Ei, pari);
  return true;
}

//=================================================================================================

int TopOpeBRepTool_mkTondgE::GetAllRest(NCollection_List<TopoDS_Shape>& lEi)
{
  lEi.Clear();

  BRepAdaptor_Surface bs(myFi);
  double       tol3d = bs.Tolerance();
  double       tolu  = bs.UResolution(tol3d);
  double       tolv  = bs.VResolution(tol3d);
  TopExp_Explorer     ex(myFi, TopAbs_EDGE);
  for (; ex.More(); ex.Next())
  {
    const TopoDS_Edge& ei  = TopoDS::Edge(ex.Current());
    bool   cli = TopOpeBRepTool_TOOL::IsClosingE(ei, myFi);
    if (cli)
      continue;

    bool isbi = myEpari.IsBound(ei);
    if (isbi)
    {
      lEi.Append(ei);
      continue;
    }

    bool isou, isov;
    gp_Dir2d         d2d;
    gp_Pnt2d         o2d;
    bool uviso = TopOpeBRepTool_TOOL::UVISO(ei, myFi, isou, isov, d2d, o2d);
    if (!uviso)
      continue;

    bool ok = false;
    if (isou)
      ok = std::abs(o2d.X() - myuvi.X()) < tolu;
    if (isov)
      ok = std::abs(o2d.Y() - myuvi.Y()) < tolv;
    if (!ok)
      continue;

    double parei;
    TopOpeBRepTool_TOOL::ParISO(myuvi, ei, myFi, parei);
    myEpari.Bind(ei, parei);
    lEi.Append(ei);
  }
  int nEi = lEi.Extent();
  return nEi;
}

static bool FUN_getEc(const TopoDS_Face& f, const TopoDS_Vertex& v, TopoDS_Edge& cle)
{
  TopExp_Explorer exe(f, TopAbs_EDGE);
  for (; exe.More(); exe.Next())
  {
    const TopoDS_Edge& e      = TopoDS::Edge(exe.Current());
    bool   closed = TopOpeBRepTool_TOOL::IsClosingE(e, f);
    if (!closed)
      continue;
    TopExp_Explorer exv(e, TopAbs_VERTEX);
    for (; exv.More(); exv.Next())
    {
      if (exv.Current().IsSame(v))
      {
        cle = e;
        return true;
      }
    }
  }
  return false;
}

static bool FUN_MkTonE(const gp_Vec& faxis,
                                   const gp_Vec& dirINcle,
                                   const gp_Vec& xxi,
                                   const gp_Vec& /*ngf*/,
                                   double&    par1,
                                   double&    par2,
                                   bool& outin)
{
  // tgi  / (tgi,xxi,faxis) is direct :
  gp_Vec tgi = xxi.Crossed(faxis);

  // ******************** getting par1, par2
  // at par1 : tr(dge, ei/fi) = forward
  // at par2 : tr(dge, ei/fi) = forward
  double    tola  = FUN_tola();
  double    dot1  = dirINcle.Dot(xxi);
  bool isONi = (std::abs(dot1) < tola);

  // par1 = ang  -> inout
  // par2 = Cang -> outin
  double ang = 1.e7;
  if (isONi)
  {
    double dot = dirINcle.Dot(tgi);
    ang               = (dot > 0) ? 0 : M_PI;
    //    outin = (ang > 0); -xpu190499
    outin = true;
  }
  else
  {
    if (!isONi)
      ang = TopOpeBRepTool_TOOL::Matter(dirINcle, tgi.Reversed(), faxis);
    // double dot = isONi ? 0 : (dirINcle^tgi).Dot(ngf);
    double dot = isONi ? 0 : (dirINcle ^ tgi).Dot(faxis);
    if (dot1 < 0)
      outin = (dot > 0);
    else
      outin = (dot < 0);
  } //! isONi

  double Cang = (ang > M_PI) ? ang - M_PI : ang + M_PI;
  par1               = outin ? ang : Cang;
  par2               = outin ? Cang : ang;
  return true;
} // FUN_MkTonE

//=================================================================================================

bool TopOpeBRepTool_mkTondgE::MkTonE(int& mkT,
                                                 double&    par1,
                                                 double&    par2)
{
  if (isT2d)
    return false;

  mkT  = NOI;
  par1 = par2 = 1.e7;
  // v :
  TopExp_Explorer      exv(mydgE, TopAbs_VERTEX);
  const TopoDS_Vertex& v = TopoDS::Vertex(exv.Current());
  // myclE :
  if (myclE.IsNull())
  {
    bool find = FUN_getEc(myF, v, myclE);
    if (!find)
      return false;
  }

  // dirINcle : tangent to cle at v oriented INSIDE 1d(cle)
  int ovcle;
  gp_Vec           dirINcle;
  bool ok = TopOpeBRepTool_TOOL::TgINSIDE(v, myclE, dirINcle, ovcle);
  if (!ok)
    return NOI;

  // faxis : describes f's axis for parametrization of <dgE>
  gp_Vec faxis = myngf;
  if (ovcle == FORWARD)
    faxis.Reverse();

  // xxi : normal to fi oriented INSIDE 3d(fi)
  gp_Vec xxi;
  ok = TopOpeBRepTool_TOOL::NggeomF(myuvi, myFi, xxi);
  if (!ok)
    return false;
  if (M_FORWARD(myFi.Orientation()))
    xxi.Reverse();

  // mkT, par1, par2 :
  bool outin;
  ok = FUN_MkTonE(faxis, dirINcle, xxi, myngf, par1, par2, outin);
  if (ok)
    mkT = MKI12;
  return ok;
}

//=================================================================================================

bool TopOpeBRepTool_mkTondgE::MkTonE(const TopoDS_Edge& ei,
                                                 int&  mkT,
                                                 double&     par1,
                                                 double&     par2)
// isT2d = true :
//   prequesitory : f,fi are tangent on v
//                  dge interfers on v with ei
//   purpose : the compute of transition on dge / ei
//             at par1 : Tr(dge,e)= ou/in,
//             at par2 : Tr(dge,e)= in/ou
//
// isT2d = false :
//   prequesitory : dge interfers on v with fi
//   purpose : the compute of transition on dge / fi
//             at par1 : Tr(dge,e)= ou/in,
//             at par2 : Tr(dge,e)= in/ou
//
{
  mkT  = NOI;
  par1 = par2 = 1.e7;
  hasRest     = myEpari.IsBound(ei);
  if (!hasRest)
    return false;
  const double pari = myEpari.Find(ei);

  double pfi, pli;
  FUN_tool_bounds(ei, pfi, pli);
  double    tolpi = TopOpeBRepTool_TOOL::TolP(ei, myFi);
  bool onfi = (std::abs(pari - pfi) < tolpi), onli = (std::abs(pari - pli) < tolpi);
  gp_Vec           tgin1di;
  bool ok = TopOpeBRepTool_TOOL::TggeomE(pari, ei, tgin1di);
  if (!ok)
    return false;
  if (onli)
    tgin1di.Reverse();

  // v :
  TopExp_Explorer      exv(mydgE, TopAbs_VERTEX);
  const TopoDS_Vertex& v = TopoDS::Vertex(exv.Current());
  // myclE :
  if (myclE.IsNull())
  {
    bool find = FUN_getEc(myF, v, myclE);
    if (!find)
      return false;
  }

  // dirINcle : tangent to cle at v oriented INSIDE 1d(cle)
  int ovcle;
  gp_Vec           dirINcle;
  ok = TopOpeBRepTool_TOOL::TgINSIDE(v, myclE, dirINcle, ovcle);
  if (!ok)
    return NOI;

  if (isT2d && !hasRest)
    return false; // no transition to compute

  // faxis : describes f's axis for parametrization of <dgE>
  gp_Vec faxis = myngf;
  if (ovcle == FORWARD)
    faxis.Reverse();

  gp_Dir xxi;  // isT2d=true : normal to ei oriented INSIDE 2d(fi)
               //              normal to fi oriented INSIDE 3d(fi)
  gp_Dir xxri; // isT2d=true : oriented inside 1d(ei)
               //              oriented inside 2d(fi)

  TopoDS_Vertex    vclo;
  bool closedi = TopOpeBRepTool_TOOL::ClosedE(ei, vclo); //@190499
  bool outin;
  if (isT2d)
  {
    // xxi :
    ok = TopOpeBRepTool_TOOL::XX(myuvi, myFi, pari, ei, xxi);
    if (!ok)
      return false;

    // mkT,par1,par2
    ok = FUN_MkTonE(faxis, dirINcle, xxi, myngf, par1, par2, outin);
    if (!ok)
      return false;

    // clang-format off
    if (!onfi && !onli) {mkT = MKI12; return true;}// => the same for all edges of lei @190499
    // clang-format on
    if (closedi)
    {
      mkT = MKI12;
      return true;
    } // onfi || onli @190499

    // xxri :
    xxri = tgin1di;

  } // isT2d
  else
  {
    // xxi :
    gp_Vec tmp;
    ok  = TopOpeBRepTool_TOOL::NggeomF(myuvi, myFi, tmp);
    xxi = gp_Dir(tmp);
    if (!ok)
      return false;
    if (M_FORWARD(myFi.Orientation()))
      xxi.Reverse();

    // mkT,par1,par2
    ok = FUN_MkTonE(faxis, dirINcle, xxi, myngf, par1, par2, outin);
    if (!ok)
      return false;

    //// modified by jgv, 21.11.01 for BUC61053 ////
    ok = TopOpeBRepTool_TOOL::XX(myuvi, myFi, pari, ei, xxri);
    if (!ok)
      return false;

    mkT               = MKI12;              // without restrictions.
    gp_Vec        tgi = xxi.Crossed(faxis); // tgi /(tgi,xxi,faxis) is direct :
    double dot = tgi.Dot(xxri);
    if (std::abs(dot) < FUN_tola())
    {
      if ((!onfi && !onli) || closedi)
      {
        mkT = MKI12;
        return true;
      }
      else
        dot = tgi.Dot(tgin1di);
    }
    bool keepang = (dot > 0);
    if (keepang)
      mkT = outin ? MKI1 : MKI2;
    else
      mkT = outin ? MKI2 : MKI1;
    return true;
    ////////////////////////////////////////////////
    /*
        // xxri :
        double ddot = tgin1di.Dot(faxis);
    // clang-format off
        bool tgaxis = std::abs(1-(std::abs(ddot))) < FUN_tola(); //=true : edge is
    tangent to sphere's axis
    // clang-format on
        if (tgaxis) {
          ok = TopOpeBRepTool_TOOL::XX(myuvi,myFi, pari,ei, xxri);
          if (!ok) return false;
        }
        else {
          if ((!onfi) && (!onli)) {mkT = MKI12; return true;} // @190499
          if (closedi)            {mkT = MKI12; return true;}// onfi || onli @190499
          xxri = tgin1di;
        }*/
  } //! isT2d

  mkT                      = MKI12;              // without restrictions.
  gp_Vec           tgi     = xxi.Crossed(faxis); // tgi /(tgi,xxi,faxis) is direct :
  double    dot     = tgi.Dot(xxri);
  bool keepang = (dot > 0);
  if (keepang)
    mkT = outin ? MKI1 : MKI2;
  else
    mkT = outin ? MKI2 : MKI1;

  return true;
}
