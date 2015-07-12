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
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopOpeBRepTool_define.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_mkTondgE.hxx>
#include <TopOpeBRepTool_TOOL.hxx>

#define M_FORWARD(sta)  (sta == TopAbs_FORWARD)
#define M_REVERSED(sta) (sta == TopAbs_REVERSED)
#define M_INTERNAL(sta) (sta == TopAbs_INTERNAL)
#define M_EXTERNAL(sta) (sta == TopAbs_EXTERNAL)

#define FORWARD  (1)
#define REVERSED (2)
#define INTERNAL (3)
#define EXTERNAL (4)
#define CLOSING  (5)

#define NOI      (0)
#define MKI1     (1)
#define MKI2     (2)
#define MKI12    (3)

static Standard_Real FUN_tola() 
{
  Standard_Real tola = Precision::Angular();
  return tola;
}

//=======================================================================
//function : TopOpeBRepTool_mkTondgE
//purpose  : 
//=======================================================================

TopOpeBRepTool_mkTondgE::TopOpeBRepTool_mkTondgE()
{
}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_mkTondgE::Initialize(const TopoDS_Edge& dgE, const TopoDS_Face& F,
					const gp_Pnt2d& uvi, const TopoDS_Face& Fi)
{
  isT2d = Standard_False; hasRest = Standard_False;
  myclE.Nullify(); myEpari.Clear();

  mydgE = dgE;
  myF = F;

  TopExp_Explorer exv(mydgE, TopAbs_VERTEX);const TopoDS_Vertex& v = TopoDS::Vertex(exv.Current()); 
  Standard_Real par = BRep_Tool::Parameter(v,mydgE);
  gp_Pnt2d uv; Standard_Boolean ok = FUN_tool_paronEF(mydgE,par,myF,uv);
  if (!ok) return Standard_False;  
  gp_Vec tmp; ok = TopOpeBRepTool_TOOL::NggeomF(uv,myF,tmp); myngf = gp_Dir(tmp);
  if (!ok) return Standard_False;
  
  myuvi = uvi;
  myFi = Fi;
  Standard_Boolean oki = TopOpeBRepTool_TOOL::NggeomF(myuvi,myFi,tmp); myngfi = gp_Dir(tmp);
  if (!oki) return Standard_False;

  Standard_Real dot = myngf.Dot(myngfi);
  isT2d = (Abs(1-Abs(dot)) < FUN_tola());
  return Standard_True;
}

//=======================================================================
//function : SetclE
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_mkTondgE::SetclE(const TopoDS_Edge& clE)
{
  myclE = clE;
  return Standard_True;
}



//=======================================================================
//function : IsT2d
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_mkTondgE::IsT2d() const
{
  return isT2d;
}

//=======================================================================
//function : SetRest
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_mkTondgE::SetRest(const Standard_Real pari, const TopoDS_Edge& Ei)
{
  hasRest = Standard_True;
  Standard_Boolean clEi = TopOpeBRepTool_TOOL::IsClosingE(Ei,myFi);
  if (clEi) {hasRest = Standard_False; return Standard_False;}

  myEpari.Bind(Ei,pari);
  return Standard_True;
}

//=======================================================================
//function : GetAllRest
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRepTool_mkTondgE::GetAllRest(TopTools_ListOfShape& lEi)
{    
  lEi.Clear();

  BRepAdaptor_Surface bs(myFi);
  Standard_Real tol3d = bs.Tolerance();
  Standard_Real tolu = bs.UResolution(tol3d);
  Standard_Real tolv = bs.VResolution(tol3d);
  TopExp_Explorer ex(myFi, TopAbs_EDGE);
  for (; ex.More(); ex.Next()){
    const TopoDS_Edge& ei = TopoDS::Edge(ex.Current());
    Standard_Boolean cli = TopOpeBRepTool_TOOL::IsClosingE(ei,myFi);
    if (cli) continue;

    Standard_Boolean isbi = myEpari.IsBound(ei);
    if (isbi) {lEi.Append(ei); continue;}

    Standard_Boolean isou,isov; gp_Dir2d d2d; gp_Pnt2d o2d;
    Standard_Boolean uviso = TopOpeBRepTool_TOOL::UVISO(ei,myFi, isou,isov, d2d,o2d);
    if (!uviso) continue;
    
    Standard_Boolean ok = Standard_False;
    if (isou) ok = Abs(o2d.X()-myuvi.X()) < tolu;
    if (isov) ok = Abs(o2d.Y()-myuvi.Y()) < tolv;
    if (!ok) continue;

    Standard_Real parei;
    TopOpeBRepTool_TOOL::ParISO(myuvi,ei,myFi, parei);
    myEpari.Bind(ei,parei);
    lEi.Append(ei);
  }
  Standard_Integer nEi = lEi.Extent();
  return nEi;
}

static Standard_Boolean FUN_getEc(const TopoDS_Face& f, const TopoDS_Vertex& v, TopoDS_Edge& cle)
{
  TopExp_Explorer exe(f,TopAbs_EDGE); 
  for (; exe.More(); exe.Next()){
    const TopoDS_Edge& e  = TopoDS::Edge(exe.Current());
    Standard_Boolean closed = TopOpeBRepTool_TOOL::IsClosingE(e,f);
    if (!closed) continue;
    TopExp_Explorer exv(e,TopAbs_VERTEX);
    for (; exv.More(); exv.Next()){
      if (exv.Current().IsSame(v)) {cle = e; return Standard_True;}
    }
  }
  return Standard_False;
}

static Standard_Boolean FUN_MkTonE(const gp_Vec& faxis, const gp_Vec& dirINcle, const gp_Vec& xxi,
		      const gp_Vec& /*ngf*/, Standard_Real& par1, Standard_Real& par2, Standard_Boolean& outin)
{
   // tgi  / (tgi,xxi,faxis) is direct :
  gp_Vec tgi = xxi.Crossed(faxis);
    
  // ******************** getting par1, par2
  // at par1 : tr(dge, ei/fi) = forward
  // at par2 : tr(dge, ei/fi) = forward
  Standard_Real tola = FUN_tola();
  Standard_Real dot1 = dirINcle.Dot(xxi);
  Standard_Boolean isONi = (Abs(dot1) < tola);
  
  // par1 = ang  -> inout
  // par2 = Cang -> outin
  Standard_Real ang = 1.e7;
  if (isONi) {
    Standard_Real dot = dirINcle.Dot(tgi);
    ang = (dot > 0) ? 0 : M_PI;
//    outin = (ang > 0); -xpu190499
    outin = Standard_True;
  }
  else {
    if (!isONi) ang = TopOpeBRepTool_TOOL::Matter(dirINcle,tgi.Reversed(),faxis);    
    //Standard_Real dot = isONi ? 0 : (dirINcle^tgi).Dot(ngf);
    Standard_Real dot = isONi ? 0 : (dirINcle^tgi).Dot(faxis);
    if (dot1 < 0) outin = (dot > 0);
    else          outin = (dot < 0);
  }//!isONi

  Standard_Real Cang = (ang > M_PI) ? ang-M_PI : ang+M_PI;
  par1 = outin ? ang : Cang;
  par2 = outin ? Cang : ang;
  return Standard_True;
}//FUN_MkTonE

//=======================================================================
//function : MkTonE
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_mkTondgE::MkTonE(Standard_Integer& mkT, Standard_Real& par1, Standard_Real& par2)
{
  if (isT2d) return Standard_False;

  mkT = NOI; par1=par2=1.e7;
  // v : 
  TopExp_Explorer exv(mydgE, TopAbs_VERTEX);
  const TopoDS_Vertex& v = TopoDS::Vertex(exv.Current());
  // myclE :
  if (myclE.IsNull()) {
    Standard_Boolean find = FUN_getEc(myF,v,myclE);
    if (!find) return Standard_False;
  }

  // dirINcle : tangent to cle at v oriented INSIDE 1d(cle) 
  Standard_Integer ovcle; gp_Vec dirINcle; Standard_Boolean ok = TopOpeBRepTool_TOOL::TgINSIDE(v,myclE,dirINcle,ovcle);
  if (!ok) return NOI;
   
  // faxis : describes f's axis for parametrization of <dgE>
  gp_Vec faxis = myngf;
  if (ovcle == FORWARD) faxis.Reverse();

  // xxi : normal to fi oriented INSIDE 3d(fi) 
  gp_Vec xxi; ok = TopOpeBRepTool_TOOL::NggeomF(myuvi,myFi,xxi);    
  if (!ok) return Standard_False;
  if (M_FORWARD(myFi.Orientation())) xxi.Reverse();

  // mkT, par1, par2 : 
  Standard_Boolean outin; ok = FUN_MkTonE(faxis,dirINcle,xxi,myngf, par1,par2,outin);  
  if (ok) mkT = MKI12;
  return ok;
}

//=======================================================================
//function : MkTonE
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_mkTondgE::MkTonE(const TopoDS_Edge& ei, Standard_Integer& mkT, Standard_Real& par1, Standard_Real& par2)
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
  mkT = NOI; par1=par2=1.e7;
  hasRest = myEpari.IsBound(ei);
  if (!hasRest) return Standard_False;
  const Standard_Real pari = myEpari.Find(ei);

  Standard_Real pfi,pli; FUN_tool_bounds(ei,pfi,pli);
  Standard_Real tolpi = TopOpeBRepTool_TOOL::TolP(ei,myFi);
  Standard_Boolean onfi = (Abs(pari-pfi) < tolpi), onli = (Abs(pari-pli) < tolpi);
  gp_Vec tgin1di;Standard_Boolean ok = TopOpeBRepTool_TOOL::TggeomE(pari,ei, tgin1di);
  if (!ok) return Standard_False;
  if (onli) tgin1di.Reverse();

  // v :
  TopExp_Explorer exv(mydgE, TopAbs_VERTEX);
  const TopoDS_Vertex& v = TopoDS::Vertex(exv.Current());
  // myclE :
  if (myclE.IsNull()) {
    Standard_Boolean find = FUN_getEc(myF,v,myclE);
    if (!find) return Standard_False;
  }

  // dirINcle : tangent to cle at v oriented INSIDE 1d(cle) 
  Standard_Integer ovcle; gp_Vec dirINcle; ok = TopOpeBRepTool_TOOL::TgINSIDE(v,myclE,dirINcle,ovcle);
  if (!ok) return NOI;

  if (isT2d && !hasRest) return Standard_False; // no transition to compute
   
  // faxis : describes f's axis for parametrization of <dgE>
  gp_Vec faxis = myngf;
  if (ovcle == FORWARD) faxis.Reverse();
  
  gp_Dir xxi; //isT2d=true : normal to ei oriented INSIDE 2d(fi) 
              //             normal to fi oriented INSIDE 3d(fi)
  gp_Dir xxri;//isT2d=true : oriented inside 1d(ei)  
              //             oriented inside 2d(fi)  


  TopoDS_Vertex vclo; Standard_Boolean closedi = TopOpeBRepTool_TOOL::ClosedE(ei,vclo);//@190499
  Standard_Boolean outin; 
  if (isT2d) {      
    // xxi : 
    ok = TopOpeBRepTool_TOOL::XX(myuvi,myFi,pari,ei,xxi);
    if (!ok) return Standard_False;
    
    // mkT,par1,par2
    ok = FUN_MkTonE(faxis,dirINcle,xxi,myngf, par1,par2, outin);
    if (!ok) return Standard_False;
    
    if (!onfi && !onli) {mkT = MKI12; return Standard_True;}// => the same for all edges of lei @190499
    if (closedi)        {mkT = MKI12; return Standard_True;}// onfi || onli @190499
    
    // xxri : 
    xxri = tgin1di;

  }//isT2d
  else {
    // xxi : 
    gp_Vec tmp; ok = TopOpeBRepTool_TOOL::NggeomF(myuvi,myFi,tmp); xxi = gp_Dir(tmp);   
    if (!ok) return Standard_False;
    if (M_FORWARD(myFi.Orientation())) xxi.Reverse();

    // mkT,par1,par2
    ok = FUN_MkTonE(faxis,dirINcle,xxi,myngf, par1,par2, outin);
    if (!ok) return Standard_False;     

    //// modified by jgv, 21.11.01 for BUC61053 ////
    ok = TopOpeBRepTool_TOOL::XX(myuvi,myFi, pari,ei, xxri);
    if (!ok) return Standard_False;
    
    mkT = MKI12;  // without restrictions.
    gp_Vec tgi = xxi.Crossed(faxis);//tgi /(tgi,xxi,faxis) is direct :
    Standard_Real dot = tgi.Dot(xxri);
    if (Abs(dot) < FUN_tola())
      {
      if ((!onfi && !onli) || closedi)
        { mkT = MKI12; return Standard_True; }
      else
        dot = tgi.Dot(tgin1di);
      }
    Standard_Boolean keepang = (dot > 0);  
    if (keepang) mkT = outin ? MKI1 : MKI2;
    else         mkT = outin ? MKI2 : MKI1;
    return Standard_True;
    ////////////////////////////////////////////////
/*
    // xxri : 
    Standard_Real ddot = tgin1di.Dot(faxis);
    Standard_Boolean tgaxis = Abs(1-(Abs(ddot))) < FUN_tola(); //=true : edge is tangent to sphere's axis
    if (tgaxis) {
      ok = TopOpeBRepTool_TOOL::XX(myuvi,myFi, pari,ei, xxri);
      if (!ok) return Standard_False;
    }
    else {
      if ((!onfi) && (!onli)) {mkT = MKI12; return Standard_True;} // @190499
      if (closedi)            {mkT = MKI12; return Standard_True;}// onfi || onli @190499
      xxri = tgin1di;
    }*/
  }//!isT2d

  mkT = MKI12;  // without restrictions.
  gp_Vec tgi = xxi.Crossed(faxis);//tgi /(tgi,xxi,faxis) is direct :
  Standard_Real dot = tgi.Dot(xxri);
  Standard_Boolean keepang = (dot > 0);  
  if (keepang) mkT = outin ? MKI1 : MKI2;
  else         mkT = outin ? MKI2 : MKI1;

  return Standard_True;

}
