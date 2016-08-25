// Created on: 1998-11-25
// Created by: Xuan PHAM PHU
// Copyright (c) 1998-1999 Matra Datavision
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

#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_makeTransition.hxx>

#include <TopOpeBRepDS_define.hxx>
#include <TopOpeBRepDS_EXPORT.hxx>
#include <TopOpeBRepDS_ProcessInterferencesTool.hxx>

#include <TopoDS.hxx>
#include <Precision.hxx>
#include <BRep_Tool.hxx>

#define M_FORWARD(st)  (st == TopAbs_FORWARD)
#define M_UNKNOWN(st)  (st == TopAbs_UNKNOWN)
#define M_REVERSED(st) (st == TopAbs_REVERSED)
#define M_INTERNAL(st) (st == TopAbs_INTERNAL)
#define M_EXTERNAL(st) (st == TopAbs_EXTERNAL)

Standard_EXPORT void FUN_scanloi(const TopOpeBRepDS_ListOfInterference& lI, 
				 TopOpeBRepDS_ListOfInterference& lFOR, Standard_Integer& FOR,
				 TopOpeBRepDS_ListOfInterference& lREV, Standard_Integer& REV,
				 TopOpeBRepDS_ListOfInterference& lINT, Standard_Integer& INT,
				 TopOpeBRepDS_ListOfInterference& lEXT, Standard_Integer& EXT)
{
  lFOR.Clear(); lREV.Clear(); lINT.Clear(); lEXT.Clear();
  FDS_assign(lI,lEXT);
  FOR=FUN_selectTRAORIinterference(lEXT,TopAbs_FORWARD,lFOR);
  REV=FUN_selectTRAORIinterference(lEXT,TopAbs_REVERSED,lREV);
  INT=FUN_selectTRAORIinterference(lEXT,TopAbs_INTERNAL,lINT);
  EXT = lEXT.Extent();
}

Standard_EXPORT Standard_Boolean FUN_ds_redu2d1d(const TopOpeBRepDS_DataStructure& BDS, const Standard_Integer ISE,
				    const Handle(TopOpeBRepDS_Interference)& I2d, const TopOpeBRepDS_ListOfInterference& l1d, TopOpeBRepDS_Transition& newT2d)
// attached to edge(ISE) : l1d = {I1d=(Tr1d(Esd),vG,Esd)}, I2d=(Tr2d(F),vG,E)
//                         - vG is not vertex of SE -
// purpose : I2d -> (newT2d(F),vG,E), with Esd is edge of F
//           returns true if set newT2d, false elsewhere.
// NYIxpu251198 treating interferences IB1 !=IA1, IB2 != IA2
{  
  TopAbs_ShapeEnum SB2,SA2; Standard_Integer IB2,IA2; TopOpeBRepDS_Kind GT2,ST2; Standard_Integer G2,S2; FDS_Idata(I2d,SB2,IB2,SA2,IA2,GT2,G2,ST2,S2);
  const TopOpeBRepDS_Transition T2d = I2d->Transition();
  TopAbs_Orientation O2 = T2d.Orientation(TopAbs_IN); newT2d.Index(IB2); newT2d.Set(O2);
  Standard_Boolean ok2 = (IB2 == IA2)&&(SB2 == TopAbs_FACE)&&(GT2 == TopOpeBRepDS_VERTEX);
  if (!ok2) return Standard_False;

  const TopoDS_Edge& SE = TopoDS::Edge(BDS.Shape(ISE));
  const TopoDS_Face& F  = TopoDS::Face(BDS.Shape(IB2)); Standard_Real tolF = BRep_Tool::Tolerance(F)*1.e2;//nyitol
  const TopoDS_Edge& E  = TopoDS::Edge(BDS.Shape(S2)); Standard_Real tolE = BRep_Tool::Tolerance(E)*1.e2;//nyitol
  Standard_Boolean EclosingF = FUN_tool_IsClosingE(E,F,F);
  if (EclosingF) { 
    TopAbs_State stb = T2d.Before(), sta = T2d.After();
    if (stb != sta) { // costs 1 projPonE
      Standard_Real pbef=0,paft=0,factor=1.e-4;
      Standard_Real parSE = FDS_Parameter(I2d);
      Standard_Real parE; Standard_Boolean ok = FUN_tool_parE(SE,parSE,E,parE,tolE);
      if (!ok) return Standard_False;
      gp_Pnt2d uv; ok = FUN_tool_paronEF(E,parE,F,uv,tolF);
      if (!ok) return Standard_False;
      
      TopOpeBRepTool_makeTransition MKT;
      TopAbs_State stb1 = TopAbs_UNKNOWN,sta1 = TopAbs_UNKNOWN; 
      ok = MKT.Initialize(SE,pbef,paft,parSE, F,uv, factor);
      if (ok) ok = MKT.SetRest(E,parE);
      if (ok) ok = MKT.MkTonE(stb1,sta1); 
      if (ok) {
        newT2d.Before(stb1);
        newT2d.After(sta1);
      }
      return ok;
    }
    return Standard_False;
  }

  TopOpeBRepDS_ListIteratorOfListOfInterference it1(l1d); 
  Standard_Boolean beforeIN1d=Standard_False, afterIN1d=Standard_False;
  // ------------------------------
  for (; it1.More(); it1.Next()){
    const Handle(TopOpeBRepDS_Interference)& I1d = it1.Value();
    TopAbs_ShapeEnum SB1,SA1; Standard_Integer IB1,IA1; TopOpeBRepDS_Kind GT1,ST1; Standard_Integer G1,S1; FDS_Idata(I1d,SB1,IB1,SA1,IA1,GT1,G1,ST1,S1);
    if (IB1 != IA1) continue;
    TopAbs_Orientation O1 = I1d->Transition().Orientation(TopAbs_IN);
    
    const TopoDS_Edge& Esd  = TopoDS::Edge(BDS.Shape(IB1));
    Standard_Boolean isedgeF = FUN_tool_inS(Esd,F);
    if (!isedgeF) continue;

    Standard_Boolean bIN = M_INTERNAL(O1) || M_REVERSED(O1);
    Standard_Boolean aIN = M_INTERNAL(O1) || M_FORWARD(O1);
    if (bIN && aIN) return Standard_False; //NYIRAISE I1d INTERNAL -> NO I2d!!
    if (bIN) beforeIN1d = Standard_True;
    if (aIN) afterIN1d = Standard_True;
  }//it1
  
  if (beforeIN1d) newT2d.Before(TopAbs_IN);
  if (afterIN1d)  newT2d.After(TopAbs_IN);
  return Standard_True;
} // redu2d1d


Standard_EXPORT Standard_Boolean FUN_ds_GetTr(
//                                 const TopOpeBRepDS_DataStructure& BDS,
                                 const TopOpeBRepDS_DataStructure& ,
                                 const Standard_Integer ISE,
//				 const Standard_Integer G,
				 const Standard_Integer ,
                                 const TopOpeBRepDS_ListOfInterference& LIG,
				 TopAbs_State& stb, Standard_Integer& isb, Standard_Integer& bdim,
				 TopAbs_State& sta, Standard_Integer& isa, Standard_Integer& adim)
// LIG = {I=(Tr,G,S)} attached to edge<ISE>
// purpose : returns newT(stb(isb,seb),sta(isa,sea)), 
// we assume IN1d > I2d > I3d
//           OUT3d > OUT2d > OUT1d
{ 
  TopOpeBRepDS_ListOfInterference LIGcopy; FDS_copy(LIG,LIGcopy); 
  TopOpeBRepDS_ListOfInterference l3d; FDS_assign(LIG,LIGcopy);
  FUN_selectSKinterference(LIGcopy,TopOpeBRepDS_FACE,l3d);
  TopOpeBRepDS_ListOfInterference l2d; FDS_assign(LIG,LIGcopy);
  FUN_ds_hasI2d(ISE,LIGcopy,l2d);
  TopOpeBRepDS_ListOfInterference l1d; FDS_assign(LIG,LIGcopy);
  FUN_selectTRASHAinterference(LIGcopy,TopAbs_EDGE,l1d);
  
  TopOpeBRepDS_ListOfInterference l1dFOR,l1dREV,l1dINT,l1dEXT; Standard_Integer FOR1d,REV1d,INT1d,EXT1d;
  ::FUN_scanloi(l1d, l1dFOR,FOR1d, l1dREV,REV1d, l1dINT,INT1d, l1dEXT,EXT1d);
  Standard_Boolean beforeIN1d = (REV1d + INT1d > 0); Standard_Boolean beforeOU1d = (FOR1d + EXT1d) != 0;
  Standard_Boolean afterIN1d  = (FOR1d + INT1d > 0); Standard_Boolean afterOU1d  = (REV1d + EXT1d) != 0;
  
  TopOpeBRepDS_ListOfInterference l2dFOR,l2dREV,l2dINT,l2dEXT; Standard_Integer FOR2d,REV2d,INT2d,EXT2d;
  ::FUN_scanloi(l2d, l2dFOR,FOR2d, l2dREV,REV2d, l2dINT,INT2d, l2dEXT,EXT2d);
  Standard_Boolean beforeIN2d = (REV2d + INT2d > 0); Standard_Boolean beforeOU2d = (FOR2d + EXT2d) != 0;
  Standard_Boolean afterIN2d  = (FOR2d + INT2d > 0); Standard_Boolean afterOU2d  = (REV2d + EXT2d) != 0;

  TopOpeBRepDS_ListOfInterference l3dFOR,l3dREV,l3dINT,l3dEXT; Standard_Integer FOR3d,REV3d,INT3d,EXT3d;
  ::FUN_scanloi(l3d, l3dFOR,FOR3d, l3dREV,REV3d, l3dINT,INT3d, l3dEXT,EXT3d);
  Standard_Boolean beforeIN3d = (REV3d + INT3d > 0); Standard_Boolean beforeOU3d = (FOR3d + EXT3d) != 0;
  Standard_Boolean afterIN3d  = (FOR3d + INT3d > 0); Standard_Boolean afterOU3d  = (REV3d + EXT3d) != 0;

  // state before
  stb = TopAbs_UNKNOWN; isb=0; bdim=0;
  if      (beforeIN1d) {    
    stb = TopAbs_IN; bdim = 1;
    TopOpeBRepDS_ListOfInterference l1INb; FDS_copy(l1dREV,l1INb); FDS_copy(l1dINT,l1INb);
    isb = l1INb.First()->Transition().IndexBefore();
  }
  else if (beforeIN2d) {  
    stb = TopAbs_IN;  bdim = 2;
    TopOpeBRepDS_ListOfInterference l2INb; FDS_copy(l2dREV,l2INb); FDS_copy(l2dINT,l2INb);
    isb = l2INb.First()->Transition().IndexBefore();
  }
  else if (beforeIN3d) {  
    stb = TopAbs_IN; bdim = 3;
    TopOpeBRepDS_ListOfInterference l3INb; FDS_copy(l3dREV,l3INb); FDS_copy(l3dINT,l3INb);
    isb = l3INb.First()->Transition().IndexBefore();   
  }
  else if (beforeOU3d) { 
    stb = TopAbs_OUT; bdim = 3;
    TopOpeBRepDS_ListOfInterference l3OUb; FDS_copy(l3dFOR,l3OUb); FDS_copy(l3dEXT,l3OUb);
    isb = l3OUb.First()->Transition().IndexBefore();  
  }
  else if (beforeOU2d) {
    stb = TopAbs_OUT; bdim = 2;
    TopOpeBRepDS_ListOfInterference l2OUb; FDS_copy(l2dFOR,l2OUb); FDS_copy(l2dEXT,l2OUb);
    isb = l2OUb.First()->Transition().IndexBefore();  
  }
  else if (beforeOU1d) {
    stb = TopAbs_OUT; bdim = 1;
    TopOpeBRepDS_ListOfInterference l1OUb; FDS_copy(l1dFOR,l1OUb); FDS_copy(l1dEXT,l1OUb);
    isb = l1OUb.First()->Transition().IndexBefore();  
  }

  // state after
  sta = TopAbs_UNKNOWN; isa=0; adim=0;
  if      (afterIN1d) { 
    sta = TopAbs_IN; adim = 1;
    TopOpeBRepDS_ListOfInterference l1INb; FDS_copy(l1dFOR,l1INb); FDS_copy(l1dINT,l1INb);
    isa = l1INb.First()->Transition().IndexAfter();
  }
  else if (afterIN2d) { 
    sta = TopAbs_IN; adim = 2;
    TopOpeBRepDS_ListOfInterference l2INb; FDS_copy(l2dFOR,l2INb); FDS_copy(l2dINT,l2INb);
    isa = l2INb.First()->Transition().IndexAfter();
  }
  else if (afterIN3d) { 
    sta = TopAbs_IN; adim = 3;
    TopOpeBRepDS_ListOfInterference l3INb; FDS_copy(l3dFOR,l3INb); FDS_copy(l3dINT,l3INb);
    isa = l3INb.First()->Transition().IndexAfter();   
  }
  else if (afterOU3d) {
    sta = TopAbs_OUT; adim = 3;
    TopOpeBRepDS_ListOfInterference l3OUb; FDS_copy(l3dREV,l3OUb); FDS_copy(l3dEXT,l3OUb);
    isa = l3OUb.First()->Transition().IndexAfter();  
  }
  else if (afterOU2d) {
    sta = TopAbs_OUT; adim = 2; 
    TopOpeBRepDS_ListOfInterference l2OUb; FDS_copy(l2dREV,l2OUb); FDS_copy(l2dEXT,l2OUb); 
    isa = l2OUb.First()->Transition().IndexAfter();  
  }
  else if (afterOU1d) {
    sta = TopAbs_OUT; adim = 1; 
    TopOpeBRepDS_ListOfInterference l1OUb; FDS_copy(l1dREV,l1OUb); FDS_copy(l1dEXT,l1OUb); 
    isa = l1OUb.First()->Transition().IndexAfter();  
  }
  return Standard_True;
}
