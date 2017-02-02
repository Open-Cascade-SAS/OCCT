// Created on: 1997-01-24
// Created by: Laurent BOURESCHE
// Copyright (c) 1997-1999 Matra Datavision
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


#include <Adaptor2d_HCurve2d.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_HVertex.hxx>
#include <Adaptor3d_TopolTool.hxx>
#include <Blend_FuncInv.hxx>
#include <Blend_Point.hxx>
#include <Blend_SurfCurvFuncInv.hxx>
#include <Blend_SurfPointFuncInv.hxx>
#include <Blend_SurfRstFunction.hxx>
#include <BRepBlend_BlendTool.hxx>
#include <BRepBlend_Extremity.hxx>
#include <BRepBlend_Line.hxx>
#include <BRepBlend_SurfRstLineBuilder.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <IntSurf.hxx>
#include <IntSurf_Transition.hxx>
#include <math_FunctionSetRoot.hxx>
#include <TopAbs.hxx>

#include <stdio.h>
#ifdef OCCT_DEBUG
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array1OfVec.hxx>
#include <TColgp_Array1OfVec2d.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <Geom_BSplineCurve.hxx>
#ifdef DRAW
#include <DrawTrSurf.hxx>
#endif
static Standard_Integer IndexOfSection = 0;
extern Standard_Boolean Blend_GettraceDRAWSECT(); 
#ifdef OCCT_DEBUG_BBPP_N_TRDERIV
// for debug : visualisation of the section
static Standard_Boolean BBPP(const Standard_Real param,
			     Blend_SurfRstFunction& Func,
			     const math_Vector& sol,
			     const Standard_Real tol,
			     Blend_Point& BP)
{
  if(!Func.IsSolution(sol,tol)) return 0;
  gp_Pnt pnts = Func.PointOnS();
  gp_Pnt pntrst = Func.PointOnRst();
  gp_Pnt2d p2ds = Func.Pnt2dOnS();
  gp_Pnt2d p2drst = Func.Pnt2dOnRst();
  Standard_Real w = Func.ParameterOnRst();
  BP = Blend_Point(pnts,pntrst,param,
		   p2ds.X(),p2ds.Y(),
		   p2drst.X(),p2drst.Y(),w);
  return 1;
}
static void tracederiv(Blend_SurfRstFunction& Func,
		       const Blend_Point& BP1,
		       const Blend_Point& BP2)
{
  Standard_Integer hp,hk,hd,hp2d,i;
  Func.GetShape(hp,hk,hd,hp2d);
  TColgp_Array1OfPnt TP1(1,hp);
  TColgp_Array1OfVec TDP1(1,hp);
  TColgp_Array1OfPnt2d TP2d1(1,hp2d);
  TColgp_Array1OfVec2d TDP2d1(1,hp2d);
  TColStd_Array1OfReal TW1(1,hp);
  TColStd_Array1OfReal TDW1(1,hp);
  Func.Section(BP1,TP1,TDP1,TP2d1,TDP2d1,TW1,TDW1);

  TColgp_Array1OfPnt TP2(1,hp);
  TColgp_Array1OfVec TDP2(1,hp);
  TColgp_Array1OfPnt2d TP2d2(1,hp2d);
  TColgp_Array1OfVec2d TDP2d2(1,hp2d);
  TColStd_Array1OfReal TW2(1,hp);
  TColStd_Array1OfReal TDW2(1,hp);
  Func.Section(BP2,TP2,TDP2,TP2d2,TDP2d2,TW2,TDW2);

  Standard_Real param1 = BP1.Parameter();
  Standard_Real param2 = BP2.Parameter();
  Standard_Real scal = 1./(param1-param2);

  cout<<endl;
  cout<<"control derivatives at point : "<<param1<<endl;

  for(i = 1; i <= hp; i++){
    cout<<endl;
    cout<<"point : "<<i<<endl;
    cout<<"dx calculated : "<<TDP1(i).X()<<endl;
    cout<<"dx estimated  : "<<scal*(TP1(i).X()-TP2(i).X())<<endl;
    cout<<"dy calculated : "<<TDP1(i).Y()<<endl;
    cout<<"dy estimated  : "<<scal*(TP1(i).Y()-TP2(i).Y())<<endl;
    cout<<"dz calculated : "<<TDP1(i).Z()<<endl;
    cout<<"dz estimated  : "<<scal*(TP1(i).Z()-TP2(i).Z())<<endl;
    cout<<"dw calculated : "<<TDW1(i)<<endl;
    cout<<"dw estimated  : "<<scal*(TW1(i)-TW2(i))<<endl;
  }
  for(i = 1; i <= hp2d; i++){
    cout<<endl;
    cout<<"point 2d : "<<i<<endl;
    cout<<"dx calculated : "<<TDP2d1(i).X()<<endl;
    cout<<"dx estimated  : "<<scal*(TP2d1(i).X()-TP2d2(i).X())<<endl;
    cout<<"dy calculated : "<<TDP2d1(i).Y()<<endl;
    cout<<"dy estimated  : "<<scal*(TP2d1(i).Y()-TP2d2(i).Y())<<endl;
  }
}
#endif
static void Drawsect(const Standard_Real param,
		     Blend_SurfRstFunction& Func)
{
  gp_Pnt pnts = Func.PointOnS();
  gp_Pnt pntrst = Func.PointOnRst();
  gp_Pnt2d p2ds = Func.Pnt2dOnS();
  gp_Pnt2d p2drst = Func.Pnt2dOnRst();
  Standard_Real w = Func.ParameterOnRst();
  Blend_Point BP(pnts,pntrst,param,
		 p2ds.X(),p2ds.Y(),
		 p2drst.X(),p2drst.Y(),w);
  Standard_Integer hp,hk,hd,hp2d;
  Func.GetShape(hp,hk,hd,hp2d);
  TColStd_Array1OfReal TK(1,hk);
  Func.Knots(TK);
  TColStd_Array1OfInteger TMul(1,hk);
  Func.Mults(TMul);
  TColgp_Array1OfPnt TP(1,hp);
  TColgp_Array1OfPnt2d TP2d(1,hp2d);
  TColStd_Array1OfReal TW(1,hp);
  Func.Section(BP,TP,TP2d,TW);
  Handle(Geom_BSplineCurve) sect = new Geom_BSplineCurve
    (TP,TW,TK,TMul,hd);
  IndexOfSection++;
#ifdef DRAW
  char tname[100];
  Standard_CString name = tname ;
  sprintf(name,"%s_%d","Section",IndexOfSection);
  DrawTrSurf::Set(name,sect);
#endif
}
#endif

//=======================================================================
//function :  ArcToRecadre
//purpose  : Find a suitable arc
//           PrevIndex is used to reject an already tested arc
//=======================================================================

Standard_Integer BRepBlend_SurfRstLineBuilder::
   ArcToRecadre(const math_Vector& theSol,
		const Standard_Integer PrevIndex, 
		gp_Pnt2d& lastpt2d,
		gp_Pnt2d& pt2d,
		Standard_Real& ponarc) 
{
  Standard_Integer IndexSol = 0,  nbarc = 0;
  Standard_Boolean ok = Standard_False;
  Standard_Boolean byinter = (line->NbPoints() != 0), okinter = 0;
  Standard_Real distmin = RealLast();
  Standard_Real uprev = 0.,vprev = 0., prm =  0., dist = 0.;

  if(byinter) previousP.ParametersOnS(uprev,vprev);
  pt2d.SetCoord(theSol(1),theSol(2));
  lastpt2d.SetCoord(uprev,vprev);
  domain1->Init();

  while (domain1->More()) {
    nbarc++; ok = 0;
    if(byinter) { 
      ok = okinter = BRepBlend_BlendTool::Inters(pt2d,lastpt2d,
						 surf1,
						 domain1->Value(),prm,dist); 
    }
    if(!ok) ok = BRepBlend_BlendTool::Project(pt2d,surf1,
					      domain1->Value(),prm,dist);

    if (ok && (nbarc != PrevIndex) ) {
      if (dist<distmin || okinter) {
	distmin = dist;
	ponarc = prm;
	IndexSol = nbarc;
	if(okinter && (PrevIndex==0) ) break;
      }
    }
    domain1->Next();
  }
  return IndexSol;
}

//=======================================================================
//function : BRepBlend_SurfRstLineBuilder
//purpose  : 
//=======================================================================

BRepBlend_SurfRstLineBuilder::BRepBlend_SurfRstLineBuilder
(const Handle(Adaptor3d_HSurface)&  Surf1,
 const Handle(Adaptor3d_TopolTool)& Domain1,
 const Handle(Adaptor3d_HSurface)&  Surf2,
 const Handle(Adaptor2d_HCurve2d)&  Rst,
 const Handle(Adaptor3d_TopolTool)& Domain2):
 sol(1,3),surf1(Surf1), domain1(Domain1),
 surf2(Surf2), rst(Rst), domain2(Domain2)
{
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepBlend_SurfRstLineBuilder::Perform(Blend_SurfRstFunction&  Func,
					   Blend_FuncInv&          Finv,
					   Blend_SurfPointFuncInv& FinvP,
					   Blend_SurfCurvFuncInv&  FinvC,
					   const Standard_Real     Pdep,
					   const Standard_Real     Pmax,
					   const Standard_Real     MaxStep,
					   const Standard_Real     TolGuide,
					   const math_Vector&      ParDep,
					   const Standard_Real     Tolesp,
					   const Standard_Real     Fleche,
					   const Standard_Boolean  Appro) 
{
  done = Standard_False;
  iscomplete = Standard_False;
  comptra = Standard_False;
  line = new BRepBlend_Line();
  tolesp = Abs(Tolesp);
  tolgui = Abs(TolGuide);
  fleche = Abs(Fleche);
  rebrou = Standard_False;
  pasmax = Abs(MaxStep);
  
  if (Pmax-Pdep >= 0.) {
    sens = 1.;
  }
  else {
    sens = -1.;
  }
  
  Blend_Status State;
  
  param = Pdep;
  Func.Set(param);
  
  if (Appro) {
    TopAbs_State siturst,situs;
    Standard_Boolean decroch;
    math_Vector tolerance(1,3),infbound(1,3),supbound(1,3);
    Func.GetTolerance(tolerance,tolesp);
    Func.GetBounds(infbound,supbound);
    math_FunctionSetRoot rsnld(Func,tolerance,30);
    
    rsnld.Perform(Func,ParDep,infbound,supbound);
    
    if (!rsnld.IsDone()) {
      return;
    }
    rsnld.Root(sol);
    if (!CheckInside(Func,siturst,situs,decroch)) {
      return;
    }
  }
  else {
    sol = ParDep;
  }

  State = TestArret(Func,Standard_False,Blend_OK);
  if (State!=Blend_OK) {
    return;
  }
#ifdef OCCT_DEBUG
  if (Blend_GettraceDRAWSECT()){
    Drawsect(param,Func);
  }
#endif
  // Update the line.
  line->Append(previousP);
  Standard_Real U,V;
  previousP.ParametersOnS(U,V);
//  W = previousP.ParameterOnC();
  
  BRepBlend_Extremity ptf1(previousP.PointOnS(),
			   U,V,previousP.Parameter(),tolesp);
  BRepBlend_Extremity ptf2(previousP.PointOnC(),
			   U,V,previousP.Parameter(),tolesp);
  if (!previousP.IsTangencyPoint()) {
    ptf1.SetTangent(previousP.TangentOnS());
    ptf2.SetTangent(previousP.TangentOnC());
  }
  if (sens>0.) {    
    line->SetStartPoints(ptf1, ptf2);    
  }
  else {
    line->SetEndPoints(ptf1, ptf2);
    
  }
  InternalPerform(Func,Finv,FinvP,FinvC,Pmax);
  done = Standard_True;
}

//=======================================================================
//function : PerformFirstSection
//purpose  : 
//=======================================================================

Standard_Boolean BRepBlend_SurfRstLineBuilder::PerformFirstSection
(Blend_SurfRstFunction&  Func,
 Blend_FuncInv&          Finv,
 Blend_SurfPointFuncInv& FinvP,
 Blend_SurfCurvFuncInv&  FinvC,
 const Standard_Real     Pdep,
 const Standard_Real     Pmax,
 const math_Vector&      ParDep,
 const Standard_Real     Tolesp,
 const Standard_Real     TolGuide,
 const Standard_Boolean  RecRst,
 const Standard_Boolean  RecP,
 const Standard_Boolean  RecS,
 Standard_Real&          Psol,   
 math_Vector&            ParSol)
{
  done = Standard_False;
  iscomplete = Standard_False;
  comptra = Standard_False;
  line = new BRepBlend_Line();
  tolesp = Abs(Tolesp);
  tolgui = Abs(TolGuide);
  rebrou = Standard_False;
  
  if (Pmax-Pdep >= 0.) {
    sens = 1.;
  }
  else {
    sens = -1.;
  }
  Blend_Status State = Blend_OnRst12;
  Standard_Real trst = 0.;
  Standard_Boolean recadp,recadrst,recads;
  Standard_Real wp,wrst,ws;
  Standard_Real U = 0.,V = 0.;
  math_Vector infbound(1,3),supbound(1,3),tolerance(1,3);
  math_Vector solinvp(1,3),solinvrst(1,4),solinvs(1,3);
  Handle(Adaptor3d_HVertex) Vtxp,Vtxrst,Vtxs,Vtxc;
  Standard_Boolean IsVtxp = 0,IsVtxrst = 0,IsVtxs = 0;
  Handle(Adaptor2d_HCurve2d) Arc;
  wp = wrst = ws = Pmax;
  param = Pdep;
  Func.Set(param);
  Func.GetTolerance(tolerance,tolesp);
  Func.GetBounds(infbound,supbound);

  math_FunctionSetRoot rsnld(Func,tolerance,30);
  rsnld.Perform(Func,ParDep,infbound,supbound);
  if (!rsnld.IsDone()) return Standard_False;
  rsnld.Root(sol);

  recads = RecS && Recadre(FinvC,solinvs,Arc,IsVtxs,Vtxs);
  if (recads) {
    ws = solinvs(1);
  }
  recadp = RecP && Recadre(FinvP,solinvp,IsVtxp,Vtxp);
  if (recadp) {
    wp = solinvp(1);
  }
  recadrst = RecRst && Recadre(Func,Finv,solinvrst,IsVtxrst,Vtxrst);
  if (recadrst) {
    wrst = solinvrst(2);
  }
  if (!recads && !recadp && !recadrst) return Standard_False;
  if (recadp && recadrst) {
    if(sens*(wrst-wp) > tolgui){ //first one leaves the domain
      wrst = wp;
      U = solinvp(2);
      V = solinvp(3);
      trst = BRepBlend_BlendTool::Parameter(Vtxp,rst);
      IsVtxrst = IsVtxp;
      Vtxrst = Vtxp;
    }
    else{                        
      U = solinvrst(3);
      V = solinvrst(4);
      trst = solinvrst(1);
    }
  }
  else if(recadp){
    wrst = wp;
    U = solinvp(2);
    V = solinvp(3);
    trst = BRepBlend_BlendTool::Parameter(Vtxp,rst);
    IsVtxrst = IsVtxp;
    Vtxrst = Vtxp;
    recadrst = Standard_True;
  }
  else if(recadrst){
    U = solinvrst(3);
    V = solinvrst(4);
    trst = solinvrst(1);
  }
  if(recads && recadrst){
    if(Abs(ws - wrst) < tolgui){
      State = Blend_OnRst12;
      param = 0.5*(ws+wrst);
      sol(1) = U;
      sol(2) = V;
      sol(3) = solinvs(2);
    }
    else if(sens*(ws-wrst)<0){
      // ground on surf
      State = Blend_OnRst1;
      param = ws;
      Arc->Value(solinvs(3)).Coord(U,V);
      sol(1) = U;
      sol(2) = V;
      sol(3) = solinvs(2);
    }
    else{
      // ground on rst
      State = Blend_OnRst2;
      param = wrst;
      sol(1) = U;
      sol(2) = V;
      sol(3) = trst;
    }
    Func.Set(param);
  }
  else if(recads){
    // ground on surf
    State = Blend_OnRst1;
    param = ws;
    Arc->Value(solinvs(3)).Coord(U,V);
    sol(1) = U;
    sol(2) = V;
    sol(3) = solinvs(2);
    Func.Set(param);
  }
  else if(recadrst){
    // ground on rst
    State = Blend_OnRst2;
    param = wrst;
    sol(1) = U;
    sol(2) = V;
    sol(3) = trst;
    Func.Set(param);
  }
  State = TestArret(Func,Standard_False,State);
  Psol = param;
  ParSol = sol;
  return Standard_True;
}  

//=======================================================================
//function : Complete
//purpose  : 
//=======================================================================

Standard_Boolean BRepBlend_SurfRstLineBuilder::Complete(Blend_SurfRstFunction&  Func,
							Blend_FuncInv&          Finv,
							Blend_SurfPointFuncInv& FinvP,
							Blend_SurfCurvFuncInv&  FinvC,
							const Standard_Real     Pmin) 
{
  if (!done) {throw StdFail_NotDone();}
  if (iscomplete) {return Standard_True;}
  if (sens >0.) {
    previousP = line->Point(1);
  }
  else {
    previousP = line->Point(line->NbPoints());
  }
  sens = -sens;
  param = previousP.Parameter();
  previousP.ParametersOnS(sol(1),sol(2));
  sol(3) = previousP.ParameterOnC();

  InternalPerform(Func,Finv,FinvP,FinvC,Pmin);
  iscomplete = Standard_True;
  return Standard_True;
}

//=======================================================================
//function : InternalPerform
//purpose  : 
//=======================================================================

void BRepBlend_SurfRstLineBuilder::InternalPerform(Blend_SurfRstFunction&  Func,
						   Blend_FuncInv&          Finv,
						   Blend_SurfPointFuncInv& FinvP,
						   Blend_SurfCurvFuncInv&  FinvC,
						   const Standard_Real     Bound) 
{
  Standard_Real stepw = pasmax;
  Standard_Integer nbp = line->NbPoints();
  if(nbp >= 2){ //The last step is reproduced if it is not too small.
    if(sens < 0.){
      stepw = (line->Point(2).Parameter() - line->Point(1).Parameter());
    }
    else{
      stepw = (line->Point(nbp).Parameter() - line->Point(nbp - 1).Parameter());
    }
    stepw = Max(stepw,100.*tolgui);
  }
  Standard_Real parprec = param;
  if (sens*(parprec - Bound) >= -tolgui) {
    return;
  }
  Blend_Status State = Blend_OnRst12;
  TopAbs_State situonc = TopAbs_UNKNOWN, situons = TopAbs_UNKNOWN;
  Standard_Boolean decroch = Standard_False;
  Standard_Boolean Arrive,recadp,recadrst,recads,echecrecad;
  Standard_Real wp,wrst,ws;
  Standard_Real U = 0.,V = 0.;
  Standard_Real trst = 0.;
  math_Vector infbound(1,3),supbound(1,3);
  math_Vector parinit(1,3),tolerance(1,3);
  math_Vector solinvp(1,3),solinvrst(1,4),solinvs(1,3);
  Handle(Adaptor3d_HVertex) Vtxp,Vtxrst,Vtxs,Vtxc;
  Standard_Boolean IsVtxp = 0,IsVtxrst = 0,IsVtxs = 0;
  BRepBlend_Extremity Extrst,Exts;
  Handle(Adaptor2d_HCurve2d) Arc;

  //IntSurf_Transition Tline,Tarc;

  Func.GetTolerance(tolerance,tolesp);
  Func.GetBounds(infbound,supbound);

  math_FunctionSetRoot rsnld(Func,tolerance,30);
  parinit = sol;

  Arrive = Standard_False;
  param = parprec + sens*stepw;
  if(sens *(param - Bound) > 0.) {
    stepw = sens*(Bound - parprec)*0.5;
    param = parprec + sens*stepw;
  }

  while (!Arrive) {
    Standard_Boolean bonpoint = 1;
#ifdef OCCT_DEBUG_BBPP_N_TRDERIV
    //debdebdebdebdebdeb
    Func.Set(param);
    rsnld.Perform(Func,parinit,infbound,supbound);
    if (rsnld.IsDone()) {
      rsnld.Root(sol);
      Blend_Point bp1;
      if(BBPP(param,Func,sol,tolesp,bp1)){
	Standard_Real dw = 1.e-10;
	Func.Set(param+dw);
	rsnld.Perform(Func,parinit,infbound,supbound);
	if (rsnld.IsDone()) {
	  rsnld.Root(sol);
	  Blend_Point bp2;
	  if(BBPP(param+dw,Func,sol,tolesp,bp2)){
	    tracederiv(Func,bp1,bp2);
	  }
	}
      }
    }
    //debdebdebdebdebdeb
#endif
    Func.Set(param);
    rsnld.Perform(Func,parinit,infbound,supbound);
    
    if (rsnld.IsDone()) {
      rsnld.Root(sol);
      if(!CheckInside(Func,situonc,situons,decroch) && line->NbPoints() == 1){
	State = Blend_StepTooLarge;
	bonpoint = 0;
      }
    }
    else {
      State = Blend_StepTooLarge;
      bonpoint = 0;
    }
    if(bonpoint){
      wp = wrst = ws = Bound;
      recadp = recadrst = recads = Standard_False;
      echecrecad = Standard_False;
      if (situons == TopAbs_OUT || situons == TopAbs_ON) {
	// pb inverse rst/rst
	recads = Recadre(FinvC,solinvs,Arc,IsVtxs,Vtxs);
	if (recads) {
	  ws = solinvs(1);
	  // It is necessary to reevaluate the deviation (BUC60360)
	  gp_Vec t, n;
	  Func.Set(ws);
	  Arc->Value(solinvs(3)).Coord(U,V);
	  sol(1) = U;
	  sol(2) = V;
	  sol(3) = solinvs(2);
	  decroch = Func.Decroch(sol, t, n); 
	}
	else {
	  echecrecad = Standard_True;
	}
      }
      if (situonc == TopAbs_OUT || situonc == TopAbs_ON) {
	// pb inverse point/surf
	recadp = Recadre(FinvP,solinvp,IsVtxp,Vtxp);
	if (recadp) {
	  wp = solinvp(1);
	}
	else {
	  echecrecad = Standard_True;
	}
      }
      if (decroch) {
	// pb inverse rst/surf
	recadrst = Recadre(Func,Finv,solinvrst,IsVtxrst,Vtxrst);
	if (recadrst) {
	  wrst = solinvrst(2);
	}
	else {
	  echecrecad = Standard_True;
	}
      }
      decroch = 0;
      if(recadp || recads || recadrst) echecrecad = Standard_False; 
      if (!echecrecad) {
	if (recadp && recadrst) {
	  if(sens*(wrst-wp) > tolgui){ //first one leaves the domain
	    wrst = wp;
	    U = solinvp(2);
	    V = solinvp(3);
	    trst = BRepBlend_BlendTool::Parameter(Vtxp,rst);
	    IsVtxrst = IsVtxp;
	    Vtxrst = Vtxp;
	  }
	  else{                        
	    decroch = 1;
	    U = solinvrst(3);
	    V = solinvrst(4);
	    trst = solinvrst(1);
	  }
	}
	else if(recadp){
	  wrst = wp;
	  U = solinvp(2);
	  V = solinvp(3);
	  trst = BRepBlend_BlendTool::Parameter(Vtxp,rst);
	  IsVtxrst = IsVtxp;
	  Vtxrst = Vtxp;
	  recadrst = Standard_True;
	}
	else if(recadrst){
	  decroch = 1;
	  U = solinvrst(3);
	  V = solinvrst(4);
	  trst = solinvrst(1);
	}
	if(recads && recadrst){
	  if(Abs(ws - wrst) < tolgui){
	    State = Blend_OnRst12;
	    param = 0.5*(ws+wrst);
	    sol(1) = U;
	    sol(2) = V;
	    sol(3) = solinvs(3);
	  }
	  else if(sens*(ws-wrst)<0){
	    // ground on surf
	    decroch = 0;
	    State = Blend_OnRst1;
	    param = ws;
	    Arc->Value(solinvs(3)).Coord(U,V);
	    sol(1) = U;
	    sol(2) = V;
	    sol(3) = solinvs(2);
	  }
	  else{
	    // ground on rst
	    State = Blend_OnRst2;
	    param = wrst;
	    sol(1) = U;
	    sol(2) = V;
	    sol(3) = trst;
	  }
	  Func.Set(param);
	}
	else if(recads){
	  // ground on surf
	  State = Blend_OnRst1;
	  param = ws;
	  Arc->Value(solinvs(3)).Coord(U,V);
	  sol(1) = U;
	  sol(2) = V;
	  sol(3) = solinvs(2);
	  Func.Set(param);
	}
	else if(recadrst){
	  // ground on rst
	  State = Blend_OnRst2;
	  param = wrst;
	  sol(1) = U;
	  sol(2) = V;
	  sol(3) = trst;
	  Func.Set(param);
	}
	else {
	  State = Blend_OK;
	}
	State = TestArret(Func,Standard_True,State);
      }
      else{
	// Failed reframing. Leave with PointsConfondus
#ifdef OCCT_DEBUG
	cout<<"SurfRstLineBuilder : failed reframing"<<endl;
#endif
	State = Blend_SamePoints;
      }
    }
    
    switch (State) {
    case Blend_OK :
      {
#ifdef OCCT_DEBUG
	if (Blend_GettraceDRAWSECT()){
	  Drawsect(param,Func);
	}
#endif
	// Update the line.
	if (sens>0.) {
	  line->Append(previousP);
	}
	else {
	  line->Prepend(previousP);
	}
	parinit = sol;
	parprec = param;
	
	if (param == Bound) {
	  Arrive = Standard_True;
	  Exts.SetValue(previousP.PointOnS(),
			sol(1),sol(2),
			previousP.Parameter(),tolesp);
	  MakeExtremity(Extrst,Standard_False,rst,sol(3),IsVtxrst,Vtxrst);
	  // Indicate end on Bound.
	}
	else {
	  param = param + sens*stepw;
	  if (sens*(param - Bound) > - tolgui) {
	    param = Bound;
	  }
	}
      }
      break;
      
    case Blend_StepTooLarge :
      {
	stepw = stepw/2.;
	if (Abs(stepw) < tolgui) {
	  previousP.ParametersOnS(U,V);
	  Exts.SetValue(previousP.PointOnS(),U,V,
			previousP.Parameter(),tolesp);
	  Extrst.SetValue(previousP.PointOnC(),
			  previousP.ParameterOnC(),
			  previousP.Parameter(),tolesp);
	  Arrive = Standard_True;
	  if (line->NbPoints()>=2) {
	    // Indicate that one stops during the processing
#ifdef OCCT_DEBUG
	    cout<<"SurfRstLineBuilder : No advancement in the processing"<<endl;
#endif
	  }
	}
	else {
	  param = parprec + sens*stepw;  // no risk to exceed Bound.
	}
      }
      break;
      
    case Blend_StepTooSmall :
      {
#ifdef OCCT_DEBUG
	if (Blend_GettraceDRAWSECT()){
	  Drawsect(param,Func);
	}
#endif
	// Update the line.
	if (sens>0.) {
	  line->Append(previousP);
	}
	else {
	  line->Prepend(previousP);
	}
	parinit = sol;
	parprec = param;
	
	stepw = Min(1.5*stepw,pasmax);
	if (param == Bound) {
	  Arrive = Standard_True;
	  Exts.SetValue(previousP.PointOnS(),sol(1),sol(2),
			previousP.Parameter(),tolesp);
	  MakeExtremity(Extrst,Standard_False,rst,sol(3),IsVtxrst,Vtxrst);
	  // Indicate end on Bound.
	}
	else {
	  param = param + sens*stepw;
	  if (sens*(param - Bound) > - tolgui) {
	    param = Bound;
	  }
	}
      }
      break;
      
    case Blend_OnRst1  :
      {
#ifdef OCCT_DEBUG
	if (Blend_GettraceDRAWSECT()){
	  Drawsect(param,Func);
	}
#endif
	if (sens>0.) {
	  line->Append(previousP);
	}
	else {
	  line->Prepend(previousP);
	}
	MakeExtremity(Exts,Standard_True,Arc,solinvs(3),IsVtxs,Vtxs);
	MakeExtremity(Extrst,Standard_False,rst,sol(3),IsVtxrst,Vtxrst);
	Arrive = Standard_True;
      }
      break;
      
    case Blend_OnRst2  :
      {
#ifdef OCCT_DEBUG
	if (Blend_GettraceDRAWSECT()){
	  Drawsect(param,Func);
	}
#endif
	if (sens>0.) {
	  line->Append(previousP);
	}
	else {
	  line->Prepend(previousP);
	}
	Exts.SetValue(previousP.PointOnS(),sol(1),sol(2),
		      previousP.Parameter(),tolesp);
	MakeExtremity(Extrst,Standard_False,rst,sol(3),IsVtxrst,Vtxrst);
	Arrive = Standard_True;
      }
      break;
      
    case Blend_OnRst12  :
      {
#ifdef OCCT_DEBUG
	if (Blend_GettraceDRAWSECT()){
	  Drawsect(param,Func);
	}
#endif
	if (sens>0.) {
	  line->Append(previousP);
	}
	else {
	  line->Prepend(previousP);
	}
	MakeExtremity(Exts,Standard_True,Arc,solinvs(1),IsVtxs,Vtxs);
	MakeExtremity(Extrst,Standard_False,rst,sol(3),IsVtxrst,Vtxrst);
	Arrive = Standard_True;
      }
      break;
      
    case Blend_SamePoints :
      {
	// Stop
#ifdef OCCT_DEBUG
	cout << "SurfRstLineBuilder Points mixed in the processing" << endl;
#endif
	previousP.ParametersOnS(U,V);
	Exts.SetValue(previousP.PointOnS(),U,V,
		      previousP.Parameter(),tolesp);
	Extrst.SetValue(previousP.PointOnC(),
			previousP.ParameterOnC(),
			previousP.Parameter(),tolesp);
	Arrive = Standard_True;
      }
      break;
    default:
      break;
    }
    if (Arrive) {
      if (sens > 0.) {
	line->SetEndPoints(Exts,Extrst);
	decrochfin = decroch;
      }
      else {
	line->SetStartPoints(Exts,Extrst);
	decrochdeb = decroch;
      }
    }
  }
}

//=======================================================================
//function : Recadre
//purpose  : Reframe section Surface / Restriction
//=======================================================================

Standard_Boolean BRepBlend_SurfRstLineBuilder::Recadre(Blend_SurfCurvFuncInv&    FinvC,
						       math_Vector&              Solinv,
						       Handle(Adaptor2d_HCurve2d)& Arc,
						       Standard_Boolean&         IsVtx,
						       Handle(Adaptor3d_HVertex)&  Vtx) 
{
  Standard_Boolean recadre = Standard_False;

  gp_Pnt2d pt2d, lastpt2d;
  Standard_Integer IndexSol, nbarc;
  Standard_Real pmin;

  IndexSol = ArcToRecadre(sol, 0, lastpt2d, pt2d, pmin);
 
  IsVtx = Standard_False;
  if (IndexSol == 0) {
    return Standard_False;
  }

  domain1->Init();
  nbarc = 1;
  while (nbarc < IndexSol) {
    nbarc++;
    domain1->Next();
  }
  Arc = domain1->Value();

  FinvC.Set(Arc);
  
  math_Vector toler(1,3),infb(1,3),supb(1,3);
  FinvC.GetTolerance(toler,tolesp);
  FinvC.GetBounds(infb,supb);
  Solinv(1) = param;
  Solinv(2) = sol(3);
  Solinv(3) = pmin;
  
  math_FunctionSetRoot rsnld(FinvC,toler,30);
  rsnld.Perform(FinvC,Solinv,infb,supb);

  if (!rsnld.IsDone()) {
#ifdef OCCT_DEBUG
    cout << "SurfRstLineBuilder : RSNLD not done "<< endl << endl;
#endif
  }
  else {
      // It is necessary to check the value of the function
    rsnld.Root(Solinv);
    recadre = FinvC.IsSolution(Solinv,tolesp);
  }

  // In case of fail, it is checked if another arc 
  // can be useful (case of output at the proximity of a vertex)
  if (!recadre) {

    IndexSol =  ArcToRecadre(sol, IndexSol, 
			     lastpt2d, pt2d, pmin);
    if (IndexSol == 0) {
      return Standard_False; // No other solution
    }

    domain1->Init();
    nbarc = 1;
    while (nbarc < IndexSol) {
      nbarc++;
      domain1->Next();
    }

    Arc = domain1->Value();
    FinvC.Set(Arc);
  
    FinvC.GetTolerance(toler,tolesp);
    FinvC.GetBounds(infb,supb);

    Solinv(3) = pmin;
  
    math_FunctionSetRoot aRsnld(FinvC,toler,30);
    aRsnld.Perform(FinvC,Solinv,infb,supb);

    if (!aRsnld.IsDone()) {
#ifdef OCCT_DEBUG
      cout << "SurfRstLineBuilder : RSNLD not done "<< endl << endl;
#endif
    }
    else {
      // It is necessary to check the value of the function
      aRsnld.Root(Solinv);
      recadre = FinvC.IsSolution(Solinv,tolesp);
    }
  }  

  if (recadre) {
    Standard_Real w = Solinv(2);
    if(w < rst->FirstParameter() - toler(2)||
       w > rst->LastParameter() + toler(2)){
      return Standard_False;
    }
    domain1->Initialize(Arc);
    domain1->InitVertexIterator();
    IsVtx = !domain1->MoreVertex();
    while (!IsVtx) {
      Vtx = domain1->Vertex();
      if (Abs(BRepBlend_BlendTool::Parameter(Vtx,Arc)-Solinv(3)) <=
	  BRepBlend_BlendTool::Tolerance(Vtx,Arc)) {
	IsVtx = Standard_True;
      }
      else {
	domain1->NextVertex();
	IsVtx = !domain1->MoreVertex();
      }
    }
    if (!domain1->MoreVertex()) {
      IsVtx = Standard_False;
    }
    return Standard_True;
  } 
  return Standard_False;
}

//=======================================================================
//function : Recadre
//purpose  : 
//=======================================================================

Standard_Boolean BRepBlend_SurfRstLineBuilder::Recadre(Blend_SurfRstFunction&   Func,
						       Blend_FuncInv&           Finv,
						       math_Vector&             Solinv,
						       Standard_Boolean&        IsVtx,
						       Handle(Adaptor3d_HVertex)& Vtx) 
{
  math_Vector toler(1,4),infb(1,4),supb(1,4);
  Finv.GetTolerance(toler,tolesp);
  Finv.GetBounds(infb,supb);
  Solinv(1) = sol(3);
  Solinv(2) = param;
  Solinv(3) = sol(1);
  Solinv(4) = sol(2);

  math_FunctionSetRoot rsnld(Finv,toler,30);
  rsnld.Perform(Finv,Solinv,infb,supb);
  if (!rsnld.IsDone()) {
#ifdef OCCT_DEBUG
    cout << "SurfRstLineBuilder :RSNLD not done "<< endl;
#endif
    return Standard_False;
  }
  rsnld.Root(Solinv);
  
  if(Finv.IsSolution(Solinv,tolesp)){
    gp_Pnt2d p2d(Solinv(3),Solinv(4));
    TopAbs_State situ = domain1->Classify(p2d,Min(toler(3),toler(4)),0);
    if ((situ != TopAbs_IN) && (situ != TopAbs_ON)) {
      return Standard_False;
    }
    domain2->Initialize(rst);
    domain2->InitVertexIterator();
    IsVtx = !domain2->MoreVertex();
    while (!IsVtx) {
      Vtx = domain2->Vertex();
      if (Abs(BRepBlend_BlendTool::Parameter(Vtx,rst)-Solinv(1)) <=
	  BRepBlend_BlendTool::Tolerance(Vtx,rst)) {
	IsVtx = Standard_True;
      }
      else {
	domain2->NextVertex();
	IsVtx = !domain2->MoreVertex();
      }
    }
    if (!domain2->MoreVertex()) {
      IsVtx = Standard_False;
    }
    // The section is recalculated by direct resolution, otherwise 
    // incoherences between the parameter and the ground caused by yawn are returned.

    math_Vector infbound(1,3),supbound(1,3);
    math_Vector parinit(1,3),tolerance(1,3);
    Func.GetTolerance(tolerance,tolesp);
    Func.GetBounds(infbound,supbound);

    math_FunctionSetRoot rsnld2(Func,tolerance,30);
    parinit(1) = Solinv(3);
    parinit(2) = Solinv(4);
    parinit(3) = Solinv(1);
    Func.Set(Solinv(2));
    rsnld2.Perform(Func,parinit,infbound,supbound);
    if(!rsnld2.IsDone()) return Standard_False;
    rsnld2.Root(parinit);
    Solinv(3) = parinit(1);
    Solinv(4) = parinit(2);
    Solinv(1) = parinit(3);
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : Recadre
//purpose  : 
//=======================================================================

Standard_Boolean BRepBlend_SurfRstLineBuilder::Recadre(Blend_SurfPointFuncInv&  FinvP,
						       math_Vector&             Solinv,
						       Standard_Boolean&        IsVtx,
						       Handle(Adaptor3d_HVertex)& Vtx) 
{
  // Le point.
  gp_Pnt2d p2drst;
  Standard_Real firstrst = rst->FirstParameter();
  Standard_Real lastrst = rst->LastParameter();
  Standard_Real wpoint = firstrst;
  if((sol(3) - firstrst) > (lastrst - sol(3))) wpoint = lastrst;
  p2drst = rst->Value(wpoint);
  gp_Pnt thepoint = surf2->Value(p2drst.X(),p2drst.Y());

  FinvP.Set(thepoint);
  math_Vector toler(1,3),infb(1,3),supb(1,3);
  FinvP.GetTolerance(toler,tolesp);
  FinvP.GetBounds(infb,supb);
  Solinv(1) = param;
  Solinv(2) = sol(1);
  Solinv(3) = sol(2);

  math_FunctionSetRoot rsnld(FinvP,toler,30);
  rsnld.Perform(FinvP,Solinv,infb,supb);
  if (!rsnld.IsDone()) {
#ifdef OCCT_DEBUG
    cout << "SurfRstLineBuilder :RSNLD not done "<< endl;
#endif
    return Standard_False;
  }
  rsnld.Root(Solinv);
  
  if(FinvP.IsSolution(Solinv,tolesp)){
    gp_Pnt2d p2d(Solinv(2),Solinv(3));
    TopAbs_State situ = domain1->Classify(p2d,Min(toler(2),toler(3)),0);
    if ((situ != TopAbs_IN) && (situ != TopAbs_ON)) {
      return Standard_False;
    }
    domain2->Initialize(rst);
    domain2->InitVertexIterator();
    IsVtx = !domain2->MoreVertex();
    while (!IsVtx) {
      Vtx = domain2->Vertex();
      if (Abs(BRepBlend_BlendTool::Parameter(Vtx,rst)-wpoint) <=
	  BRepBlend_BlendTool::Tolerance(Vtx,rst)) {
	IsVtx = Standard_True;
      }
      else {
	domain2->NextVertex();
	IsVtx = !domain2->MoreVertex();
      }
    }
    if (!domain2->MoreVertex()) {
      IsVtx = Standard_False;
    }
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : Transition
//purpose  : 
//=======================================================================

void BRepBlend_SurfRstLineBuilder::Transition(const Standard_Boolean          OnFirst,
					      const Handle(Adaptor2d_HCurve2d)& Arc,
					      const Standard_Real             Param,
					      IntSurf_Transition&             TLine,
					      IntSurf_Transition&             TArc) 
{
  Standard_Boolean computetranstionaveclacorde = 0;
  gp_Vec tgline;
  Blend_Point prevprev;

  if(previousP.IsTangencyPoint()){
    if(line->NbPoints() < 2) return;
    computetranstionaveclacorde = 1;
    if(sens < 0){
      prevprev = line->Point(2);
    }
    else {
      prevprev = line->Point(line->NbPoints() - 1);
    }
  }
  gp_Pnt2d p2d;
  gp_Vec2d dp2d;
  
  gp_Pnt pbid;
  gp_Vec d1u,d1v,normale,tgrst;
  
  Arc->D1(Param,p2d,dp2d);
  if (OnFirst) {
    surf1->D1(p2d.X(),p2d.Y(),pbid,d1u,d1v);
    if(!computetranstionaveclacorde) tgline = previousP.TangentOnS1();
    else tgline = gp_Vec(prevprev.PointOnS(),previousP.PointOnS());
  }
  else {
    surf2->D1(p2d.X(),p2d.Y(),pbid,d1u,d1v);
    if(!computetranstionaveclacorde) tgline = previousP.TangentOnS2();
    else tgline = gp_Vec(prevprev.PointOnC(),previousP.PointOnC());
  }
  
  tgrst.SetLinearForm(dp2d.X(),d1u,dp2d.Y(),d1v);
  normale = d1u.Crossed(d1v);
  
  IntSurf::MakeTransition(tgline,tgrst,normale,TLine,TArc);
}

//=======================================================================
//function : MakeExtremity
//purpose  : 
//=======================================================================

void BRepBlend_SurfRstLineBuilder::MakeExtremity(BRepBlend_Extremity&            Extrem,
						 const Standard_Boolean          OnFirst,
						 const Handle(Adaptor2d_HCurve2d)& Arc,
						 const Standard_Real             Param,
						 const Standard_Boolean          IsVtx,
						 const Handle(Adaptor3d_HVertex)&  Vtx) 
{
  IntSurf_Transition Tline,Tarc;
  Standard_Real prm;
  Handle(Adaptor3d_TopolTool) Iter;
  if (OnFirst) {
    Extrem.SetValue(previousP.PointOnS(),
		    sol(1),sol(2),
		    previousP.Parameter(),tolesp);
    if (!previousP.IsTangencyPoint()) 
      Extrem.SetTangent(previousP.TangentOnS());
    Iter = domain1;
  }
  else {
    Extrem.SetValue(previousP.PointOnC(),
		    sol(3),
		    previousP.Parameter(),tolesp);
    if (!previousP.IsTangencyPoint()) 
      Extrem.SetTangent(previousP.TangentOnC());    
    Iter = domain2;
  }
  
  Iter->Init();
  if (!IsVtx) {
    Transition(OnFirst,Arc,Param,Tline,Tarc);
    Extrem.AddArc(Arc,Param,Tline,Tarc);
  }
  else {
    Extrem.SetVertex(Vtx);
    while (Iter->More()) {
      Handle(Adaptor2d_HCurve2d) arc = Iter->Value();
      if (arc != Arc) {
	Iter->Initialize(arc);
	Iter->InitVertexIterator();
	while (Iter->MoreVertex()) {
	  if (Iter->Identical(Vtx,Iter->Vertex())) {
	    prm = BRepBlend_BlendTool::Parameter(Vtx,arc);
	    Transition(OnFirst,arc,prm,Tline,Tarc);
	    Extrem.AddArc(arc,prm,Tline,Tarc);
	  }
	  Iter->NextVertex();
	}
      }
      else {
	Transition(OnFirst,arc,Param,Tline,Tarc);
	Extrem.AddArc(arc,Param,Tline,Tarc);
      }
      Iter->Next();
    }
  }
}

//=======================================================================
//function : CheckDeflectionOnSurf
//purpose  : 
//=======================================================================

Blend_Status BRepBlend_SurfRstLineBuilder::CheckDeflectionOnSurf(const Blend_Point& CurPoint)
{
  //Controls 3d of Blend_CSWalking.

  //rule by tests in U4 corresponds to 11.478 d
  const Standard_Real CosRef3D = 0.98;
  Standard_Real Cosi=0, Cosi2=0;
  Standard_Boolean curpointistangent = CurPoint.IsTangencyPoint();
  Standard_Boolean prevpointistangent = previousP.IsTangencyPoint();

  gp_Pnt Psurf = CurPoint.PointOnS();
  gp_Vec Tgsurf;
  if(!curpointistangent){
    Tgsurf = CurPoint.TangentOnS();
  }
  gp_Pnt prevP = previousP.PointOnS();
  gp_Vec prevTg;
  if(!prevpointistangent){
    prevTg = previousP.TangentOnS();
  }
  Standard_Real Norme,prevNorme = 0.;
  gp_Vec Corde(prevP,Psurf);
  Norme = Corde.SquareMagnitude();
//  if(!curpointistangent) curNorme = Tgsurf.SquareMagnitude();
  if(!prevpointistangent) prevNorme = prevTg.SquareMagnitude();

  if (Norme <= tolesp*tolesp){
    // it can be necessary to force same point
    return Blend_SamePoints;
  }
  if(!prevpointistangent){
    if(prevNorme <= tolesp*tolesp) {
      return Blend_SamePoints;
    }
    Cosi = sens*Corde*prevTg;
    if (Cosi <0.) { // angle 3d>pi/2. --> return back
      return Blend_Backward;
    }
    
    Cosi2 = Cosi * Cosi / prevNorme / Norme;
    if (Cosi2 < CosRef3D) { 
      return Blend_StepTooLarge;
    }
  }
  
  if(!curpointistangent){
    // Check if it is necessary to control the sign of prevtg*Tgsurf
    Cosi = sens*Corde*Tgsurf;
    Cosi2 = Cosi * Cosi / Tgsurf.SquareMagnitude() / Norme;
    if (Cosi2 < CosRef3D || Cosi < 0.) { 
      return Blend_StepTooLarge;
    }
  }  

  if(!curpointistangent && !prevpointistangent){
    // Estimation of the current arrow
    Standard_Real FlecheCourante = 
      (prevTg.Normalized().XYZ()-Tgsurf.Normalized().XYZ()).SquareModulus()*Norme/64.;
    
    if (FlecheCourante <= 0.25*fleche*fleche) {
      return Blend_StepTooSmall;
    }
    if (FlecheCourante > fleche*fleche) {
      // not too great : 
      return Blend_StepTooLarge;
    }
  }
  return Blend_OK;
}


//=======================================================================
//function : CheckDeflectionOnRst
//purpose  : 
//=======================================================================

Blend_Status BRepBlend_SurfRstLineBuilder::CheckDeflectionOnRst(const Blend_Point& CurPoint)
{
  //Controls 3D of Blend_CSWalking.

  // rule by tests in U4 corresponds to 11.478 d
  const Standard_Real CosRef3D = 0.98;
  Standard_Real Cosi, Cosi2;
  Standard_Boolean curpointistangent = CurPoint.IsTangencyPoint();
  Standard_Boolean prevpointistangent = previousP.IsTangencyPoint();

  gp_Pnt Psurf = CurPoint.PointOnC();
  gp_Vec Tgsurf;
  if(!curpointistangent){
    Tgsurf = CurPoint.TangentOnC();
  }
  gp_Pnt prevP = previousP.PointOnC();
  gp_Vec prevTg;
  if(!prevpointistangent){
    prevTg = previousP.TangentOnC();
  }
  Standard_Real Norme,prevNorme = 0.;
  gp_Vec Corde(prevP,Psurf);
  Norme = Corde.SquareMagnitude();
//  if(!curpointistangent) curNorme = Tgsurf.SquareMagnitude();
  if(!prevpointistangent) prevNorme = prevTg.SquareMagnitude();

  if (Norme <= tolesp*tolesp){
    // it can be necessary to force same point
    return Blend_SamePoints;
  }
  if(!prevpointistangent){
    if(prevNorme <= tolesp*tolesp) {
      return Blend_SamePoints;
    }
    Cosi = sens*Corde*prevTg;
    if (Cosi <0.) { // angle 3d>pi/2. --> return back
      return Blend_Backward;
    }
    
    Cosi2 = Cosi * Cosi / prevNorme / Norme;
    if (Cosi2 < CosRef3D) { 
      return Blend_StepTooLarge;
    }
  }
  
  if(!curpointistangent){
    // Check if it is necessary to control the sign of prevtg*Tgsurf
    Cosi = sens*Corde*Tgsurf;
    Cosi2 = Cosi * Cosi / Tgsurf.SquareMagnitude() / Norme;
    if (Cosi2 < CosRef3D || Cosi < 0.) { 
      return Blend_StepTooLarge;
    }
  }  

  if(!curpointistangent && !prevpointistangent){
    // Estimation of the current arrow
    Standard_Real FlecheCourante = 
      (prevTg.Normalized().XYZ()-Tgsurf.Normalized().XYZ()).SquareModulus()*Norme/64.;
    
    if (FlecheCourante <= 0.25*fleche*fleche) {
      return Blend_StepTooSmall;
    }
    if (FlecheCourante > fleche*fleche) {
      // not too great
      return Blend_StepTooLarge;
    }
  }
  return Blend_OK;
}

static IntSurf_TypeTrans ConvOrToTra(const TopAbs_Orientation O)
{
  if(O == TopAbs_FORWARD) return IntSurf_In;
  return IntSurf_Out;
}

//=======================================================================
//function : TestArret
//purpose  : 
//=======================================================================

Blend_Status BRepBlend_SurfRstLineBuilder::TestArret(Blend_SurfRstFunction& Func,
						     const Standard_Boolean TestDeflection,
						     const Blend_Status     State) 
{
  gp_Pnt pts,ptrst;
  gp_Pnt2d pt2drst;
  gp_Vec tgs,tgrst;
  gp_Vec2d tg2ds,tg2drst;
  Blend_Status StateS,StateRst;
  IntSurf_TypeTrans tras = IntSurf_Undecided, trarst = IntSurf_Undecided;
  Blend_Point curpoint;

  if (Func.IsSolution(sol,tolesp)) {
    Standard_Boolean curpointistangent = Func.IsTangencyPoint();
    pts = Func.PointOnS();
    ptrst = Func.PointOnRst();
    pt2drst = Func.Pnt2dOnRst();
    if(curpointistangent){
      curpoint.SetValue(pts,ptrst,param,sol(1),sol(2),
			pt2drst.X(),pt2drst.Y(),sol(3));
    }
    else{
      tgs     = Func.TangentOnS();
      tgrst   = Func.TangentOnRst();
      tg2ds   = Func.Tangent2dOnS();
      tg2drst = Func.Tangent2dOnRst();

      curpoint.SetValue(pts,ptrst,param,sol(1),sol(2),
			pt2drst.X(),pt2drst.Y(),sol(3),
			tgs,tgrst,tg2ds,tg2drst);
    }
    if (TestDeflection) {
      StateS = CheckDeflectionOnSurf(curpoint);
      StateRst = CheckDeflectionOnRst(curpoint);
    }
    else {
      StateS = StateRst = Blend_OK;
    }
    if (StateS == Blend_Backward) {
      StateS = Blend_StepTooLarge;
      rebrou= Standard_True;
    }
    if (StateRst == Blend_Backward) {
      StateRst = Blend_StepTooLarge;
      rebrou = Standard_True;
    }
    if (StateS == Blend_StepTooLarge ||
	StateRst == Blend_StepTooLarge) {
      return Blend_StepTooLarge;
    }

    if (!comptra && !curpointistangent) {
      gp_Vec tgsecs,nors;
      Func.Decroch(sol,nors,tgsecs);
      nors.Normalize();
      Standard_Real testra = tgsecs.Dot(nors.Crossed(tgs));
      if (Abs(testra) > tolesp) {
	if (testra < 0.) {
	  tras = IntSurf_In;
	}
	else if (testra >0.) {
	  tras = IntSurf_Out;
	}
	gp_Pnt2d p2drstref; gp_Vec2d tg2drstref;
	rst->D1(sol(3),p2drstref,tg2drstref);
	testra = tg2drst.Dot(tg2drstref);
	TopAbs_Orientation Or = domain2->Orientation(rst);
	if (Abs(testra) > 1.e-8) {
	  if (testra < 0.) {
	    trarst = ConvOrToTra(TopAbs::Reverse(Or));
	  }
	  else if (testra >0.) {
	    trarst = ConvOrToTra(Or);
	  }
	  comptra = Standard_True;
	  line->Set(tras,trarst);
	}
      }
    }
    if (StateS == Blend_OK ||
	StateRst == Blend_OK ) {
      previousP = curpoint;
      return State;
    }
    if (StateS == Blend_StepTooSmall &&
	StateRst == Blend_StepTooSmall) {
      previousP = curpoint;
      if (State == Blend_OK) {
	return Blend_StepTooSmall;
      }
      else {
	return State;
      }
    }
    if (State == Blend_OK) {
      return Blend_SamePoints;
    }
    else {
      return State;
    }
  }
  return Blend_StepTooLarge;
}

//=======================================================================
//function : CheckInside
//purpose  : 
//=======================================================================

Standard_Boolean BRepBlend_SurfRstLineBuilder::CheckInside(Blend_SurfRstFunction& Func,
							   TopAbs_State&          SituOnC,
							   TopAbs_State&          SituOnS,
							   Standard_Boolean&      Decroch)
{
  math_Vector tolerance(1,3);
  Func.GetTolerance(tolerance,tolesp);
  //face pcurve.
  Standard_Real w = sol(3);
  if(w < rst->FirstParameter() - tolerance(3)||
     w > rst->LastParameter() + tolerance(3)){
    SituOnC = TopAbs_OUT;
  }
  else if (w > rst->FirstParameter() &&
	   w < rst->LastParameter()){
    SituOnC = TopAbs_IN;
  }
  else SituOnC = TopAbs_ON;

  //face surface
  gp_Pnt2d p2d(sol(1),sol(2));
  SituOnS = domain1->Classify(p2d,Min(tolerance(1),tolerance(2)),0);

  //lost contact
  gp_Vec tgs,nors;
  Decroch = Func.Decroch(sol,tgs,nors);

  return (SituOnC == TopAbs_IN && SituOnS == TopAbs_IN && !Decroch);
}

