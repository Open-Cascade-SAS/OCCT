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
#include <Blend_CurvPointFuncInv.hxx>
#include <Blend_Point.hxx>
#include <Blend_RstRstFunction.hxx>
#include <Blend_SurfCurvFuncInv.hxx>
#include <BRepBlend_BlendTool.hxx>
#include <BRepBlend_Extremity.hxx>
#include <BRepBlend_Line.hxx>
#include <BRepBlend_RstRstLineBuilder.hxx>
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
//-----------------------------------------------------
// For debug : visualisation of the section
static Standard_Boolean BBPP(const Standard_Real param,
			     Blend_RstRstFunction& Func,
			     const math_Vector& sol,
			     const Standard_Real tol,
			     Blend_Point& BP)
{
  if(!Func.IsSolution(sol,tol)) return 0;
  gp_Pnt pntrst1    = Func.PointOnRst1();
  gp_Pnt pntrst2    = Func.PointOnRst2();
  gp_Pnt2d p2drst1  = Func.Pnt2dOnRst1();
  gp_Pnt2d p2drst2  = Func.Pnt2dOnRst2();
  Standard_Real w1  = Func.ParameterOnRst1();
  Standard_Real w2  = Func.ParameterOnRst2();
  BP = Blend_Point(pntrst1, pntrst2, param,
		   p2drst1.X(), p2drst1.Y(),
		   p2drst2.X(), p2drst2.Y(), w1, w2);
  return 1;
}


//-----------------------------------------------------
static void tracederiv(Blend_RstRstFunction& Func,
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
  Func.Section(BP1, TP1,TDP1,TP2d1,TDP2d1,TW1,TDW1);

  TColgp_Array1OfPnt TP2(1,hp);
  TColgp_Array1OfVec TDP2(1,hp);
  TColgp_Array1OfPnt2d TP2d2(1,hp2d);
  TColgp_Array1OfVec2d TDP2d2(1,hp2d);
  TColStd_Array1OfReal TW2(1,hp);
  TColStd_Array1OfReal TDW2(1,hp);
  Func.Section(BP2,TP2,TDP2,TP2d2,TDP2d2,TW2,TDW2);

  Standard_Real param1 = BP1.Parameter();
  Standard_Real param2 = BP2.Parameter();
  Standard_Real scal = 1./ (param1 - param2);

  cout<<endl;
  cout<<"control of derivatives at point : "<<param1<<endl;

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

//-----------------------------------------------------
static void Drawsect(const Standard_Real param,
		     Blend_RstRstFunction& Func)
{
  gp_Pnt pntrst1   = Func.PointOnRst1();
  gp_Pnt pntrst2   = Func.PointOnRst2();
  gp_Pnt2d p2drst1 = Func.Pnt2dOnRst1();
  gp_Pnt2d p2drst2 = Func.Pnt2dOnRst2();
  Standard_Real u  = Func.ParameterOnRst1();
  Standard_Real v  = Func.ParameterOnRst2();
  Blend_Point BP(pntrst1, pntrst2, param,
		 p2drst1.X(), p2drst1.Y(),
		 p2drst2.X(), p2drst2.Y(), u, v);
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
//function : BRepBlend_RstRstLineBuilder
//purpose  : 
//=======================================================================

BRepBlend_RstRstLineBuilder::BRepBlend_RstRstLineBuilder
(const Handle(Adaptor3d_HSurface)&  Surf1,
 const Handle(Adaptor2d_HCurve2d)&  Rst1,
 const Handle(Adaptor3d_TopolTool)& Domain1,
 const Handle(Adaptor3d_HSurface)&  Surf2,
 const Handle(Adaptor2d_HCurve2d)&  Rst2,
 const Handle(Adaptor3d_TopolTool)& Domain2):
 sol(1,2), surf1(Surf1), domain1(Domain1),
 surf2(Surf2), domain2(Domain2), rst1(Rst1), rst2(Rst2)
{
}

//=======================================================================
//function : Perform
//purpose  : launch the processing
//=======================================================================

void BRepBlend_RstRstLineBuilder::Perform(Blend_RstRstFunction&   Func,
					  Blend_SurfCurvFuncInv& Finv1,
					  Blend_CurvPointFuncInv& FinvP1,
					  Blend_SurfCurvFuncInv& Finv2,
					  Blend_CurvPointFuncInv& FinvP2,
					  const Standard_Real     Pdep,
					  const Standard_Real     Pmax,
					  const Standard_Real     MaxStep,
					  const Standard_Real     TolGuide,
					  const math_Vector&      ParDep,
					  const Standard_Real     Tolesp,
					  const Standard_Real     Fleche,
					  const Standard_Boolean  Appro) 
{
  done       = Standard_False;
  iscomplete = Standard_False;
  comptra    = Standard_False;
  line       = new BRepBlend_Line();
  tolesp     = Abs(Tolesp);
  tolgui     = Abs(TolGuide);
  fleche     = Abs(Fleche);
  rebrou     = Standard_False;
  pasmax     = Abs(MaxStep);
  
  if (Pmax - Pdep >= 0.) {
    sens = 1.;
  }
  else {
    sens = -1.;
  }
  
  Blend_Status State;
  
  param = Pdep;
  Func.Set(param);
  
  if (Appro) {
    TopAbs_State siturst1, siturst2;
    Blend_DecrochStatus decroch;
    math_Vector tolerance(1, 2), infbound(1, 2), supbound(1, 2);
    Func.GetTolerance(tolerance, tolesp);
    Func.GetBounds(infbound, supbound);
    math_FunctionSetRoot rsnld(Func, tolerance, 30);
    
    rsnld.Perform(Func, ParDep, infbound, supbound);
    
    if (!rsnld.IsDone()) {
      return;
    }
    rsnld.Root(sol);
    if (!CheckInside(Func, siturst1, siturst2, decroch)) {
      return;
    }
  }
  else {
    sol = ParDep;
  }

  State = TestArret(Func, Standard_False, Blend_OK);
  if (State != Blend_OK) {
    return;
  }
#ifdef OCCT_DEBUG
  if (Blend_GettraceDRAWSECT()){
    Drawsect(param, Func);
  }
#endif
  // Update the line.
  line->Append(previousP);
  Standard_Real U, V;
  U = previousP.ParameterOnC1();
  V = previousP.ParameterOnC2();
  BRepBlend_Extremity ptf1 (previousP.PointOnC1(),
			    U, previousP.Parameter(),tolesp);
  BRepBlend_Extremity ptf2 (previousP.PointOnC2(),
			    V, previousP.Parameter(),tolesp);
  if (!previousP.IsTangencyPoint()) {
    ptf1.SetTangent(previousP.TangentOnC1());
    ptf2.SetTangent(previousP.TangentOnC2());
  }
  
  if (sens > 0.) {
    line->SetStartPoints(ptf1, ptf2);
  }
  else {
    line->SetEndPoints(ptf1, ptf2);
  }

  InternalPerform(Func, Finv1, FinvP1, Finv2, FinvP2, Pmax);
  done = Standard_True;
}

//=======================================================================
//function : PerformFirstSection
//purpose  : Creation of the first section
//=======================================================================

Standard_Boolean BRepBlend_RstRstLineBuilder::PerformFirstSection
(Blend_RstRstFunction&   Func,
 Blend_SurfCurvFuncInv&  Finv1,
 Blend_CurvPointFuncInv& FinvP1,
 Blend_SurfCurvFuncInv&  Finv2,
 Blend_CurvPointFuncInv& FinvP2,
 const Standard_Real     Pdep,
 const Standard_Real     Pmax,
 const math_Vector&      ParDep,
 const Standard_Real     Tolesp,
 const Standard_Real     TolGuide,
 const Standard_Boolean  RecRst1,
 const Standard_Boolean  RecP1,
 const Standard_Boolean  RecRst2,
 const Standard_Boolean  RecP2,
 Standard_Real&          Psol,   
 math_Vector&            ParSol)
{
  done       = Standard_False;
  iscomplete = Standard_False;
  comptra    = Standard_False;
  line       = new BRepBlend_Line();
  tolesp     = Abs(Tolesp);
  tolgui     = Abs(TolGuide);
  rebrou     = Standard_False;
  
  if (Pmax - Pdep >= 0.) {
    sens = 1.;
  }
  else {
    sens = -1.;
  }
  
  Standard_Boolean recadp1, recadp2, recadrst1, recadrst2;
  Standard_Real wp1, wp2, wrst1, wrst2;
  Blend_Status State = Blend_OnRst12;
  Standard_Real trst11 = 0., trst12 = 0., trst21 = 0., trst22 = 0.;
  math_Vector infbound(1, 2), supbound(1, 2), tolerance(1, 2);
  math_Vector solinvp1(1, 2), solinvp2(1, 2), solinvrst1(1, 3), solinvrst2(1, 3);
  Handle(Adaptor3d_HVertex) Vtxp1, Vtxp2, Vtxrst1, Vtxrst2, Vtxc;
  Standard_Boolean IsVtxp1 = 0, IsVtxp2 = 0, IsVtxrst1 = 0, IsVtxrst2 = 0;
  Handle(Adaptor2d_HCurve2d) Arc;
  wp1   = wp2 = wrst1 = wrst2 = Pmax;
  param = Pdep;
  Func.Set(param);
  Func.GetTolerance(tolerance, tolesp);
  Func.GetBounds(infbound, supbound);

  math_FunctionSetRoot rsnld(Func, tolerance, 30);
  rsnld.Perform(Func, ParDep, infbound, supbound);
  if (!rsnld.IsDone()) return Standard_False;
  rsnld.Root(sol);

  recadrst1 = RecRst1 && Recadre1(Func, Finv1, solinvrst1, IsVtxrst1, Vtxrst1);
  if (recadrst1) {
    wrst1 = solinvrst1(1);
  }

  recadp1 = RecP1 && Recadre1(FinvP1, solinvp1, IsVtxp1, Vtxp1);
  if (recadp1) {
    wp1 = solinvp1(1);
  }

  recadrst2 = RecRst2 && Recadre2(Func, Finv2, solinvrst2, IsVtxrst2, Vtxrst2);
  if (recadrst2) {
    wrst2 = solinvrst2(1);
  }

  recadp2 = RecP2 && Recadre2(FinvP2, solinvp2, IsVtxp2, Vtxp2);
  if (recadp2) {
    wp2 = solinvp2(1);
  }

  if (!recadrst1 && !recadp1 && !recadrst2 && !recadp2) return Standard_False;


  // it is checked if the contact was lost or domain 1 was left
  if (recadp1 && recadrst1) {
    if (sens * (wrst1 - wp1) > tolgui){ //at first one leaves the domain
      wrst1     = wp1;
      trst12    = solinvp1(2);
      trst11    = BRepBlend_BlendTool::Parameter(Vtxp1, rst1);
      IsVtxrst2 = IsVtxp1;
      Vtxrst2   = Vtxp1;
      recadrst1 = Standard_False;
    }
    else { // the contact is lost
      trst11  = solinvrst1(3);
      trst12  = solinvrst1(2);
      recadp1 = Standard_False;
    }
  }
  else if (recadp1) {
    wrst1     = wp1;
    trst12    = solinvp1(2);
    trst11    = BRepBlend_BlendTool::Parameter(Vtxp1, rst1);
    IsVtxrst1 = IsVtxp1;
    Vtxrst1   = Vtxp1;
  }
  else if (recadrst1) {
    trst11  = solinvrst1(3);
    trst12  = solinvrst1(2);
  }

  // it is checked if the contact was lost or domain 2 was left
  if (recadp2 && recadrst2) {
    if (sens * (wrst2 - wp2) > tolgui) { //at first one leaves the domain
      wrst2     = wp2;
      trst21    = solinvp2(2);
      trst22    = BRepBlend_BlendTool::Parameter(Vtxp2, rst2);
      IsVtxrst2 = IsVtxp2;
      Vtxrst2   = Vtxp2;
      recadrst2 = Standard_False;
    }
    else { 
      trst22  = solinvrst2(3);
      trst21  = solinvrst2(2);
      recadp2 = Standard_False;
    }
  }
  else if (recadp2) {
    wrst2     = wp2;
    trst21    = solinvp2(2);
    trst22    = BRepBlend_BlendTool::Parameter(Vtxp2, rst2);
    IsVtxrst2 = IsVtxp2;
    Vtxrst2   = Vtxp2;
  }
  else if (recadrst2) {
    trst22  = solinvrst2(3);
    trst21  = solinvrst2(2);
  }

  // it is checked on which curve the contact is lost earlier
  if (recadrst1 && recadrst2) {
    if (Abs(wrst1 - wrst2) < tolgui) {
      State    = Blend_OnRst12;
      param    = 0.5 * (wrst1 + wrst2);
      sol(1)   = trst11;
      sol(2)   = trst22;
    }
    else if (sens * (wrst1 - wrst2) < 0) {
      // contact lost on Rst1
      State   = Blend_OnRst1;
      param   = wrst1;
      sol(1)  = trst11;
      sol(2)  = trst12;
    }
    else {
      // contact lost on rst2
      State   = Blend_OnRst2;
      param   = wrst2;
      sol(1)  = trst21;
      sol(2)  = trst22;
    }
  Func.Set(param);
  }
  else if (recadrst1) {
    // ground on rst1
    State   = Blend_OnRst1;
    param   = wrst1;
    sol(1)  = trst11;
    sol(2)  = trst12;
    Func.Set(param);
  }
  else if (recadrst2) {
    // ground on rst2
    State   = Blend_OnRst2;
    param   = wrst2;
    sol(1)  = trst21;
    sol(2)  = trst22;
    Func.Set(param);
  }
  // it is checked on which curves one leaves first
  else if (recadp1 && recadp2) {
    if (Abs(wrst1 - wrst2) < tolgui) {
      State  = Blend_OnRst12;
      param  = 0.5 * (wrst1 + wrst2);
      sol(1) = trst11;
      sol(2) = trst22;
    }
    else if (sens * (wrst1 - wrst2) < 0) {
      // sol on Rst1
      State  = Blend_OnRst1;
      param  = wrst1;
      sol(1) = trst11;
      sol(2) = trst12;
    }
    else {
      // ground on rst2
      State  = Blend_OnRst2;
      param  = wrst2;
      sol(1) = trst21;
      sol(2) = trst22;
    }
    Func.Set(param);
  }
  else if (recadp1) {
    // ground on rst1
    State  = Blend_OnRst1;
    param  = wrst1;
    sol(1) = trst11;
    sol(2) = trst12;
    Func.Set(param);
  }
  else if (recadp2) {
    // ground on rst2
    State  = Blend_OnRst2;
    param  = wrst2;
    sol(1) = trst21;
    sol(2) = trst22;
    Func.Set(param);
  }

  State  = TestArret(Func, Standard_False, State);
  Psol   = param;
  ParSol = sol;
  return Standard_True;
}  

//=======================================================================
//function : Complete
//purpose  : 
//=======================================================================

Standard_Boolean BRepBlend_RstRstLineBuilder::Complete(Blend_RstRstFunction&   Func,
						       Blend_SurfCurvFuncInv&  Finv1,
						       Blend_CurvPointFuncInv& FinvP1,
						       Blend_SurfCurvFuncInv&  Finv2,
						       Blend_CurvPointFuncInv& FinvP2, 
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
  sens   = -sens;
  param  = previousP.Parameter();
  sol(1) = previousP.ParameterOnC1();
  sol(2) = previousP.ParameterOnC2();

  InternalPerform(Func, Finv1, FinvP1, Finv2, FinvP2, Pmin);
  iscomplete = Standard_True;
  return Standard_True;
}

//=======================================================================
//function : InternalPerform
//purpose  : algorithm of processing without extremities
//=======================================================================

void BRepBlend_RstRstLineBuilder::InternalPerform(Blend_RstRstFunction&   Func,
						  Blend_SurfCurvFuncInv&  Finv1,
					          Blend_CurvPointFuncInv& FinvP1,
					          Blend_SurfCurvFuncInv&  Finv2,
					          Blend_CurvPointFuncInv& FinvP2,
					          const Standard_Real     Bound) 
{
  Standard_Real stepw  = pasmax;
  Standard_Integer nbp = line->NbPoints();
  if(nbp >= 2){ //The last step is redone if it is not too small.
    if(sens < 0.){
      stepw = (line->Point(2).Parameter() - line->Point(1).Parameter());
    }
    else{
      stepw = (line->Point(nbp).Parameter() - line->Point(nbp - 1).Parameter());
    }
    stepw = Max(stepw, 100. * tolgui);
  }
  Standard_Real parprec = param;
  if (sens* (parprec - Bound) >= -tolgui) {
    return;
  }
  Blend_Status State = Blend_OnRst12;
  Standard_Real trst11 = 0., trst12 = 0., trst21 = 0., trst22 = 0.;
  TopAbs_State situonc1 = TopAbs_UNKNOWN, situonc2 = TopAbs_UNKNOWN;
  Blend_DecrochStatus decroch = Blend_NoDecroch;
  Standard_Boolean Arrive, recadp1, recadp2, recadrst1, recadrst2, echecrecad;
  Standard_Real wp1, wp2, wrst1, wrst2;
  math_Vector infbound(1, 2), supbound(1, 2);
  math_Vector parinit(1, 2), tolerance(1, 2);
  math_Vector solinvp1(1, 2),  solinvp2(1, 2), solinvrst1(1, 3), solinvrst2(1, 3);
  Handle(Adaptor3d_HVertex) Vtxp1, Vtxp2, Vtxrst1, Vtxrst2;
  Standard_Boolean IsVtxp1 = 0, IsVtxp2 = 0, IsVtxrst1 = 0, IsVtxrst2 = 0;
  BRepBlend_Extremity Extrst1, Extrst2;

  //IntSurf_Transition Tline, Tarc;

  Func.GetTolerance(tolerance, tolesp);
  Func.GetBounds(infbound, supbound);

  math_FunctionSetRoot rsnld(Func, tolerance, 30);
  parinit = sol;

  Arrive = Standard_False;
  param = parprec + sens * stepw;
  if (sens * (param - Bound) > 0.) {
    stepw = sens * (Bound - parprec) * 0.5;
    param = parprec + sens * stepw;
  }

  while (!Arrive) {
    Standard_Boolean bonpoint = 1;
#ifdef OCCT_DEBUG_BBPP_N_TRDERIV
    //debdebdebdebdebdeb
    Func.Set(param);
    rsnld.Perform(Func, parinit, infbound, supbound);
    if (rsnld.IsDone()) {
      rsnld.Root(sol);
      Blend_Point bp1;
      if(BBPP(param, Func, sol, tolesp, bp1)){
	Standard_Real dw = 1.e-10;
	Func.Set(param + dw);
	rsnld.Perform(Func, parinit, infbound, supbound);
	if (rsnld.IsDone()) {
	  rsnld.Root(sol);
	  Blend_Point bp2;
	  if(BBPP(param + dw, Func, sol, tolesp, bp2)){
	    tracederiv(Func, bp1, bp2);
	  }
	}
      }
    }
    //debdebdebdebdebdeb
#endif
    Func.Set(param);
    rsnld.Perform(Func, parinit, infbound, supbound);
    
    if (rsnld.IsDone()) {
      rsnld.Root(sol);
      if(!CheckInside(Func, situonc1, situonc2, decroch) && line->NbPoints() == 1){
	State = Blend_StepTooLarge;
	bonpoint = 0;
      }
    }
    else {
      State = Blend_StepTooLarge;
      bonpoint = 0;
    }
    if(bonpoint){
      wp1     = wp2 = wrst1 = wrst2 = Bound;
      recadp1 = recadp2 = recadrst1 = recadrst2 = Standard_False;
      echecrecad = Standard_False;
      if (situonc1 != TopAbs_IN) {
	// pb inversion rst/rst
	recadp1 = Recadre1(FinvP1, solinvp1, IsVtxp1, Vtxp1);
	if (recadp1) {
	  wp1 = solinvp1(1);
	}
	else {
	  echecrecad = Standard_True;
	}
      }

      if (situonc2 != TopAbs_IN) {
	// pb inversion point/surf
	recadp2 = Recadre2(FinvP2, solinvp2, IsVtxp2, Vtxp2);
	if (recadp2) {
	  wp2 = solinvp2(1);
	}
	else {
	  echecrecad = Standard_True;
	}
      }

      if (decroch == Blend_DecrochRst1 || decroch == Blend_DecrochBoth) {
	// pb inversion rst1/surf1
	recadrst1 = Recadre1(Func, Finv1, solinvrst1, IsVtxrst1, Vtxrst1);
	if (recadrst1) {
	  wrst1 = solinvrst1(1);
	}
	else {
	  echecrecad = Standard_True;
	}
      }

      if (decroch == Blend_DecrochRst2 || decroch == Blend_DecrochBoth) {
	// pb inverse rst2/surf2
	recadrst2 = Recadre2(Func, Finv2, solinvrst2, IsVtxrst2, Vtxrst2);
	if (recadrst2) {
	  wrst2 = solinvrst2(1);
	}
	else {
	  echecrecad = Standard_True;
	}
      }

      decroch = Blend_NoDecroch;
      if (recadp1 || recadp2 || recadrst1 || recadrst2) echecrecad = Standard_False;
 
      if (!echecrecad) {
        // it is checked if the contact was lost or domain 1 was left
        if (recadp1 && recadrst1) {
          if (sens * (wrst1 - wp1) > tolgui){ //first one leaves the domain
            wrst1     = wp1;
	    trst12    = solinvp1(2);
	    trst11    = BRepBlend_BlendTool::Parameter(Vtxp1, rst1);
	    IsVtxrst2 = IsVtxp1;
	    Vtxrst2   = Vtxp1;
            recadrst1 = Standard_False;
	  }
	  else { // contact is lost
	    trst11  = solinvrst1(3);
	    trst12  = solinvrst1(2);
            recadp1 = Standard_False;
	  }
	}
	else if (recadp1) {
	  wrst1     = wp1;
	  trst12    = solinvp1(2);
	  trst11    = BRepBlend_BlendTool::Parameter(Vtxp1, rst1);
	  IsVtxrst1 = IsVtxp1;
	  Vtxrst1   = Vtxp1;
	}
	else if (recadrst1) {
	  trst11  = solinvrst1(3);
	  trst12  = solinvrst1(2);
	}

	// it is checked if the contact was lost or domain 2 was left
	if (recadp2 && recadrst2) {
	  if (sens * (wrst2 - wp2) > tolgui) { //first one leaves the domain
	    wrst2     = wp2;
	    trst21    = solinvp2(2);
	    trst22    = BRepBlend_BlendTool::Parameter(Vtxp2, rst2);
	    IsVtxrst2 = IsVtxp2;
	    Vtxrst2   = Vtxp2;
            recadrst2 = Standard_False;
	  }
	  else { 
	    trst22  = solinvrst2(3);
	    trst21  = solinvrst2(2);
            recadp2 = Standard_False;
	  }
	}
	else if (recadp2) {
	  wrst2     = wp2;
	  trst21    = solinvp2(2);
	  trst22    = BRepBlend_BlendTool::Parameter(Vtxp2, rst2);
	  IsVtxrst2 = IsVtxp2;
	  Vtxrst2   = Vtxp2;
	}
	else if (recadrst2) {
	  trst22  = solinvrst2(3);
	  trst21  = solinvrst2(2);
	}

        // it is checked on which curve the contact is lost earlier
	if (recadrst1 && recadrst2) {
	  if (Abs(wrst1 - wrst2) < tolgui) {
            State    = Blend_OnRst12;
	    decroch  = Blend_DecrochBoth;
	    param    = 0.5 * (wrst1 + wrst2);
	    sol(1)   = trst11;
	    sol(2)   = trst22;
	  }
	  else if (sens * (wrst1 - wrst2) < 0) {
	    // contact is lost on Rst1
            State   = Blend_OnRst1;
	    decroch = Blend_DecrochRst1; 
	    param   = wrst1;
	    sol(1)  = trst11;
	    sol(2)  = trst12;
	  }
	  else {
	    // contact is lost on rst2
            State   = Blend_OnRst2;
	    decroch = Blend_DecrochRst2;
	    param   = wrst2;
	    sol(1)  = trst21;
	    sol(2)  = trst22;
	  }
	  Func.Set(param);
	}
	else if (recadrst1) {
	  // ground on rst1
	  State   = Blend_OnRst1;
          decroch = Blend_DecrochRst1;
	  param   = wrst1;
	  sol(1)  = trst11;
	  sol(2)  = trst12;
	  Func.Set(param);
	}
	else if (recadrst2) {
	  // ground on rst2
	  State   = Blend_OnRst2;
          decroch = Blend_DecrochRst2;
	  param   = wrst2;
	  sol(1)  = trst21;
	  sol(2)  = trst22;
	  Func.Set(param);
	}
	//  it is checked on which curve the contact is lost earlier
	else if (recadp1 && recadp2) {
	  if (Abs(wrst1 - wrst2) < tolgui) {
	    State  = Blend_OnRst12;
	    param  = 0.5 * (wrst1 + wrst2);
	    sol(1) = trst11;
	    sol(2) = trst22;
	  }
	  else if (sens * (wrst1 - wrst2) < 0) {
	    // ground on Rst1
	    State  = Blend_OnRst1;
	    param  = wrst1;
	    sol(1) = trst11;
	    sol(2) = trst12;
	  }
	  else {
	    // ground on rst2
	    State  = Blend_OnRst2;
	    param  = wrst2;
	    sol(1) = trst21;
	    sol(2) = trst22;
	  }
	  Func.Set(param);
	}
	else if (recadp1) {
	  // ground on rst1
	  State  = Blend_OnRst1;
	  param  = wrst1;
	  sol(1) = trst11;
	  sol(2) = trst12;
	  Func.Set(param);
	}
	else if (recadp2) {
	  // ground on rst2
	  State  = Blend_OnRst2;
	  param  = wrst2;
	  sol(1) = trst21;
	  sol(2) = trst22;
	  Func.Set(param);
	}
	else {
	  State = Blend_OK;
	}

	State = TestArret(Func, Standard_True, State);
      }
      else{
	// reframing failed. Leave with PointsConfondus
#ifdef OCCT_DEBUG
	cout<<"reframing failed"<<endl;
#endif
	State = Blend_SamePoints;
      }
    }
    
    switch (State) {
    case Blend_OK :
      {
#ifdef OCCT_DEBUG
	if (Blend_GettraceDRAWSECT()){
	  Drawsect(param, Func);
	}
#endif
	// Update the line.
	if (sens > 0.) {
	  line->Append(previousP);
	}
	else {
	  line->Prepend(previousP);
	}
	parinit = sol;
	parprec = param;
	
	if (param == Bound) {
	  Arrive = Standard_True;
	  Extrst1.SetValue(previousP.PointOnC1(),
			   previousP.ParameterOnC1(),
			   previousP.Parameter(), tolesp);
	  MakeExtremity(Extrst2, Standard_False, rst2, sol(2), IsVtxrst2, Vtxrst2);
	  // Show that end is on Bound.
	}
	else {
	  param = param + sens * stepw;
	  if (sens * (param - Bound) > - tolgui) {
	    param = Bound;
	  }
	}
      }
      break;
      
    case Blend_StepTooLarge :
      {
	stepw = stepw / 2.;
	if (Abs(stepw) < tolgui) {
	  Extrst1.SetValue(previousP.PointOnC1(),
			   previousP.ParameterOnC1(),
			   previousP.Parameter(), tolesp);
	  Extrst2.SetValue(previousP.PointOnC2(),
			   previousP.ParameterOnC2(),
			   previousP.Parameter(), tolesp);
	  Arrive = Standard_True;
#ifdef OCCT_DEBUG
	  if (line->NbPoints()>=2) {
	    // Show that there is a stop during processing 
	    cout<<"No more advancement in the processing"<<endl;
	  }
#endif
	}
	else {
	  param = parprec + sens * stepw;  // there is no risk to exceed Bound.
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
	if (sens > 0.) {
	  line->Append(previousP);
	}
	else {
	  line->Prepend(previousP);
	}
	parinit = sol;
	parprec = param;
	
	stepw   = Min(1.5 * stepw, pasmax);
	if (param == Bound) {
	  Arrive = Standard_True;
	  Extrst1.SetValue(previousP.PointOnC1(),
			   previousP.ParameterOnC1(),
			   previousP.Parameter(), tolesp);
	  MakeExtremity(Extrst2, Standard_False, rst2, sol(2), IsVtxrst2, Vtxrst2);
	  // Indicate that end is on Bound.
	}
	else {
	  param = param + sens * stepw;
	  if (sens * (param - Bound) > - tolgui) {
	    param = Bound;
	  }
	}
      }
      break;
      
    case Blend_OnRst1  :
      {
#ifdef OCCT_DEBUG
	if (Blend_GettraceDRAWSECT()){
	  Drawsect(param, Func);
	}
#endif
	if (sens > 0.) {
	  line->Append(previousP);
	}
	else {
	  line->Prepend(previousP);
	}
	MakeExtremity(Extrst1, Standard_True, rst1, sol(1), IsVtxrst1, Vtxrst1);
	MakeExtremity(Extrst2, Standard_False, rst2, sol(2), IsVtxrst2, Vtxrst2);
	Arrive = Standard_True;
      }
      break;
      
    case Blend_OnRst2  :
      {
#ifdef OCCT_DEBUG
	if (Blend_GettraceDRAWSECT()){
	  Drawsect(param, Func);
	}
#endif
	if (sens>0.) {
	  line->Append(previousP);
	}
	else {
	  line->Prepend(previousP);
	}

        MakeExtremity(Extrst1, Standard_True, rst1, sol(1), IsVtxrst1, Vtxrst1);
	MakeExtremity(Extrst2, Standard_False, rst2, sol(2), IsVtxrst2, Vtxrst2);
	Arrive = Standard_True;
      }
      break;
      
    case Blend_OnRst12  :
      {
#ifdef OCCT_DEBUG
	if (Blend_GettraceDRAWSECT()){
	  Drawsect(param, Func);
	}
#endif
	if (sens > 0.) {
	  line->Append(previousP);
	}
	else {
	  line->Prepend(previousP);
	}

        MakeExtremity(Extrst1, Standard_True, rst1, sol(1), IsVtxrst1, Vtxrst1);
	MakeExtremity(Extrst2, Standard_False, rst2, sol(2), IsVtxrst2, Vtxrst2);
	Arrive = Standard_True;
      }
      break;
      
    case Blend_SamePoints :
      {
	// Stop
#ifdef OCCT_DEBUG
	cout << " Mixed points in the processing" << endl;
#endif
	Extrst1.SetValue(previousP.PointOnC1(),
			 previousP.ParameterOnC1(),
			 previousP.Parameter(), tolesp);
	Extrst2.SetValue(previousP.PointOnC2(),
			 previousP.ParameterOnC2(),
			 previousP.Parameter(), tolesp);
	Arrive = Standard_True;
      }
      break;
    default:
      break;
    }
    if (Arrive) {
      if (sens > 0.) {
	line->SetEndPoints(Extrst1, Extrst2);
	decrochfin = decroch;
      }
      else {
	line->SetStartPoints(Extrst1, Extrst2);
	decrochdeb = decroch;
      }
    }
  }
}


//=======================================================================
//function : Recadre1
//purpose  : Contact lost on 1
//=======================================================================

Standard_Boolean BRepBlend_RstRstLineBuilder::Recadre1(Blend_RstRstFunction&    Func,
						       Blend_SurfCurvFuncInv&   Finv,
						       math_Vector&             Solinv,
						       Standard_Boolean&        IsVtx,
						       Handle(Adaptor3d_HVertex)& Vtx) 
{
  math_Vector toler(1, 3), infb(1, 3), supb(1, 3);
  Finv.GetTolerance(toler, tolesp);
  Finv.GetBounds(infb, supb);
  Solinv(1) = param;
  Solinv(2) = sol(2);
  Solinv(3) = sol(1);
 
  // The point where contact is not lost is found
  math_FunctionSetRoot rsnld(Finv, toler, 30);
  rsnld.Perform(Finv, Solinv, infb, supb);
  if (!rsnld.IsDone()) {
#ifdef OCCT_DEBUG
    cout << "RSNLD not done "<< endl << endl;
#endif
    return Standard_False;
  }

  rsnld.Root(Solinv);

  // It is necessary to check if the function value meets the
  // second restriction
  if (Finv.IsSolution(Solinv, tolesp)) {
    Standard_Real w = Solinv(2);
    if(w < rst2->FirstParameter() - toler(2)||
       w > rst2->LastParameter() + toler(2)){
      return Standard_False;
    }
 
    // it is checked if it is on a Vertex
    domain1->Initialize(rst1);
    domain1->InitVertexIterator();
    IsVtx = !domain1->MoreVertex();
    while (!IsVtx) {
      Vtx = domain1->Vertex();
      if (Abs(BRepBlend_BlendTool::Parameter(Vtx, rst1)-Solinv(3)) <=
	  BRepBlend_BlendTool::Tolerance(Vtx, rst1)) {
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
    // The section is recalculated by direct solution, otherwise return 
    // incoherences between the parameter and the ground caused by yawn.

    math_Vector infbound(1, 2), supbound(1, 2);
    math_Vector parinit(1, 2), tolerance(1, 2);
    Func.GetTolerance(tolerance, tolesp);
    Func.GetBounds(infbound, supbound);

    math_FunctionSetRoot rsnld2(Func, tolerance, 30);
    parinit(1) = Solinv(3);
    parinit(2) = Solinv(2);
    Func.Set(Solinv(1));
    rsnld2.Perform(Func, parinit, infbound, supbound);
    if(!rsnld2.IsDone()) return Standard_False;
    rsnld2.Root(parinit);
    Solinv(2) = parinit(2);
    Solinv(3) = parinit(1);
    return Standard_True;
  }
  return Standard_False;
}





//=======================================================================
//function : Recadre2
//purpose  : Contact lost on Rst2
//=======================================================================

Standard_Boolean BRepBlend_RstRstLineBuilder::Recadre2(Blend_RstRstFunction&    Func,
						       Blend_SurfCurvFuncInv&   Finv,
						       math_Vector&             Solinv,
						       Standard_Boolean&        IsVtx,
						       Handle(Adaptor3d_HVertex)& Vtx) 
{
  math_Vector toler(1, 3), infb(1, 3), supb(1, 3);
  Finv.GetTolerance(toler, tolesp);
  Finv.GetBounds(infb, supb);
  Solinv(1) = param;
  Solinv(2) = sol(1);
  Solinv(3) = sol(2);
 
  math_FunctionSetRoot rsnld(Finv, toler, 30);
  rsnld.Perform(Finv, Solinv, infb, supb);
  if (!rsnld.IsDone()) {
#ifdef OCCT_DEBUG
    cout << "RSNLD not done "<< endl << endl;
#endif
    return Standard_False;
  }

  rsnld.Root(Solinv);

  // It is necessary to check the value of the function
  if (Finv.IsSolution(Solinv, tolesp)) {
    Standard_Real w = Solinv(2);
    if(w < rst1->FirstParameter() - toler(2)||
       w > rst1->LastParameter() + toler(2)){
      return Standard_False;
    }
 
    domain2->Initialize(rst2);
    domain2->InitVertexIterator();
    IsVtx = !domain2->MoreVertex();
    while (!IsVtx) {
      Vtx = domain2->Vertex();
      if (Abs(BRepBlend_BlendTool::Parameter(Vtx, rst2)-Solinv(3)) <=
	  BRepBlend_BlendTool::Tolerance(Vtx, rst2)) {
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
    // The section is recalculated by direct solution, otherwise return 
    // incoherences between the parameter and the ground caused by yawn.
   
    math_Vector infbound(1, 2), supbound(1, 2);
    math_Vector parinit(1,2), tolerance(1,2);
    Func.GetTolerance(tolerance, tolesp);
    Func.GetBounds(infbound, supbound);

    math_FunctionSetRoot rsnld2(Func, tolerance, 30);
    parinit(1) = Solinv(2);
    parinit(2) = Solinv(3);
    Func.Set(Solinv(1));
    rsnld2.Perform(Func, parinit, infbound, supbound);
    if(!rsnld2.IsDone()) return Standard_False;
    rsnld2.Root(parinit);
    Solinv(2) = parinit(1);
    Solinv(3) = parinit(2);
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : Recadre
//purpose  : This is the end of curve rst1
//=======================================================================

Standard_Boolean BRepBlend_RstRstLineBuilder::Recadre1(Blend_CurvPointFuncInv&  FinvP,
						       math_Vector&             Solinv,
						       Standard_Boolean&        IsVtx,
						       Handle(Adaptor3d_HVertex)& Vtx) 
{
  // One is located on the last or the first point, following the
  // direction of processing.
  gp_Pnt2d p2drst1;
  Standard_Real firstrst1 = rst1->FirstParameter();
  Standard_Real lastrst1  = rst1->LastParameter();
  Standard_Real upoint    = firstrst1;

  if((sol(1) - firstrst1) > (lastrst1 - sol(1))) upoint = lastrst1;
  p2drst1 = rst1->Value(upoint);
  gp_Pnt thepoint = surf1->Value(p2drst1.X(), p2drst1.Y());

  FinvP.Set(thepoint);
  math_Vector toler(1,2), infb(1, 2), supb(1, 2);
  FinvP.GetTolerance(toler, tolesp);
  FinvP.GetBounds(infb, supb);
  Solinv(1) = param;
  Solinv(2) = sol(2);

  math_FunctionSetRoot rsnld(FinvP, toler, 30);
  rsnld.Perform(FinvP, Solinv, infb, supb);
  if (!rsnld.IsDone()) {
#ifdef OCCT_DEBUG
    cout << "RSNLD not done "<< endl << endl;
#endif
    return Standard_False;
  }
  rsnld.Root(Solinv);
  
  if(FinvP.IsSolution(Solinv, tolesp)){
    gp_Pnt2d p2drst2  = rst2->Value(Solinv(2));
    TopAbs_State situ = domain2->Classify(p2drst2, toler(2), 0);
    if ((situ != TopAbs_IN) && (situ != TopAbs_ON)) {
      return Standard_False;
    }
    domain1->Initialize(rst1);
    domain1->InitVertexIterator();
    IsVtx = !domain1->MoreVertex();
    while (!IsVtx) {
      Vtx = domain1->Vertex();
      if (Abs(BRepBlend_BlendTool::Parameter(Vtx, rst1) - upoint) <=
	  BRepBlend_BlendTool::Tolerance(Vtx, rst1)) {
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
//function : Recadre2
//purpose  : This is the end of curve rst2
//=======================================================================

Standard_Boolean BRepBlend_RstRstLineBuilder::Recadre2(Blend_CurvPointFuncInv&  FinvP,
						       math_Vector&             Solinv,
						       Standard_Boolean&        IsVtx,
						       Handle(Adaptor3d_HVertex)& Vtx) 
{
  // One is located on the last or the first point, following the 
  // direction of processing.
  gp_Pnt2d p2drst2;
  Standard_Real firstrst2 = rst2->FirstParameter();
  Standard_Real lastrst2  = rst2->LastParameter();
  Standard_Real vpoint    = firstrst2;

  if((sol(2) - firstrst2) > (lastrst2 - sol(2))) vpoint = lastrst2;
  p2drst2 = rst2->Value(vpoint);
  gp_Pnt thepoint = surf2->Value(p2drst2.X(), p2drst2.Y());

  FinvP.Set(thepoint);
  math_Vector toler(1,2), infb(1, 2), supb(1, 2);
  FinvP.GetTolerance(toler, tolesp);
  FinvP.GetBounds(infb, supb);
  Solinv(1) = param;
  Solinv(2) = sol(1);

  math_FunctionSetRoot rsnld(FinvP, toler, 30);
  rsnld.Perform(FinvP, Solinv, infb, supb);
  if (!rsnld.IsDone()) {
#ifdef OCCT_DEBUG
    cout << "RSNLD not done "<< endl << endl;
#endif
    return Standard_False;
  }
  rsnld.Root(Solinv);
  
  if(FinvP.IsSolution(Solinv, tolesp)){
    gp_Pnt2d p2drst1  = rst1->Value(Solinv(2));
    TopAbs_State situ = domain1->Classify(p2drst1, toler(2), 0);
    if ((situ != TopAbs_IN) && (situ != TopAbs_ON)) {
      return Standard_False;
    }
    domain2->Initialize(rst2);
    domain2->InitVertexIterator();
    IsVtx = !domain2->MoreVertex();
    while (!IsVtx) {
      Vtx = domain2->Vertex();
      if (Abs(BRepBlend_BlendTool::Parameter(Vtx, rst2) - vpoint) <=
	  BRepBlend_BlendTool::Tolerance(Vtx, rst2)) {
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

void BRepBlend_RstRstLineBuilder::Transition(const Standard_Boolean          OnFirst,
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
  gp_Vec d1u, d1v, normale, tgrst;
  
  Arc->D1(Param, p2d, dp2d);
  if (OnFirst) {
    surf1->D1(p2d.X(), p2d.Y(), pbid, d1u, d1v);
    if(!computetranstionaveclacorde) tgline = previousP.TangentOnC1();
    else tgline = gp_Vec(prevprev.PointOnC1(), previousP.PointOnC1());
  }
  else {
    surf2->D1(p2d.X(), p2d.Y(), pbid, d1u, d1v);
    if(!computetranstionaveclacorde) tgline = previousP.TangentOnC2();
    else tgline = gp_Vec(prevprev.PointOnC2(), previousP.PointOnC2());
  }
  
  tgrst.SetLinearForm(dp2d.X(), d1u, dp2d.Y(), d1v);
  normale = d1u.Crossed(d1v);
  
  IntSurf::MakeTransition(tgline, tgrst, normale, TLine, TArc);
}

//=======================================================================
//function : MakeExtremity
//purpose  : produce the extremity of a curve
//=======================================================================

void BRepBlend_RstRstLineBuilder::MakeExtremity(BRepBlend_Extremity&            Extrem,
						const Standard_Boolean          OnFirst,
					        const Handle(Adaptor2d_HCurve2d)& Arc,
					        const Standard_Real             Param,
					        const Standard_Boolean          IsVtx,
					        const Handle(Adaptor3d_HVertex)&  Vtx) 
{
  IntSurf_Transition Tline, Tarc;
  Standard_Real prm;
  Handle(Adaptor3d_TopolTool) Iter;
  if (OnFirst) {
    Extrem.SetValue(previousP.PointOnC1(),
		    sol(1),
		    previousP.Parameter(), tolesp);
    if (!previousP.IsTangencyPoint()) 
      Extrem.SetTangent(previousP.TangentOnC1());
    Iter = domain1;
  }
  else {
    Extrem.SetValue(previousP.PointOnC2(),
		    sol(2),
		    previousP.Parameter(), tolesp);
    if (!previousP.IsTangencyPoint()) 
      Extrem.SetTangent(previousP.TangentOnC1());
    Iter = domain2;
  }
  
  Iter->Init();
  if (!IsVtx) {
    Transition(OnFirst, Arc, Param, Tline, Tarc);
    Extrem.AddArc(Arc, Param, Tline, Tarc);
  }
  else {
    Extrem.SetVertex(Vtx);
    while (Iter->More()) {
      Handle(Adaptor2d_HCurve2d) arc = Iter->Value();
     if (arc != Arc) {
	Iter->Initialize(arc);
	Iter->InitVertexIterator();
	while (Iter->MoreVertex()) {
	  if (Iter->Identical(Vtx, Iter->Vertex())) {
	    prm = BRepBlend_BlendTool::Parameter(Vtx, arc);
	    Transition(OnFirst, arc, prm, Tline, Tarc);
	    Extrem.AddArc(arc, prm, Tline, Tarc);
	  }
	  Iter->NextVertex();
	}
      }
      else {
	Transition(OnFirst, arc, Param, Tline, Tarc);
	Extrem.AddArc(arc, Param, Tline, Tarc);
      }
      Iter->Next();
    }
  }
}

//=======================================================================
//function : CheckDeflectionOnRst1
//purpose  : 
//=======================================================================

Blend_Status BRepBlend_RstRstLineBuilder::CheckDeflectionOnRst1(const Blend_Point& CurPoint)
{
  //Controls 3d of Blend_CSWalking.

  // rule by tests in U4 corresponds to 11.478 
  const Standard_Real CosRef3D = 0.98;
  Standard_Real Cosi, Cosi2;
  Standard_Boolean curpointistangent  = CurPoint.IsTangencyPoint();
  Standard_Boolean prevpointistangent = previousP.IsTangencyPoint();

  gp_Pnt Psurf = CurPoint.PointOnC1();
  gp_Vec Tgsurf;
  if(!curpointistangent){
    Tgsurf = CurPoint.TangentOnC1();
  }
  gp_Pnt prevP = previousP.PointOnC1();
  gp_Vec prevTg;
  if(!prevpointistangent){
    prevTg = previousP.TangentOnC1();
  }
  Standard_Real Norme;
  Standard_Real prevNorme = 0.;
  gp_Vec Corde(prevP, Psurf);
  Norme = Corde.SquareMagnitude();
  if (!prevpointistangent) prevNorme = prevTg.SquareMagnitude();

  if (Norme <= tolesp * tolesp) {
    // it can be necessary to force the same point
    return Blend_SamePoints;
  }
  if(!prevpointistangent){
    if (prevNorme <= tolesp * tolesp) {
      return Blend_SamePoints;
    }
    Cosi = sens * Corde * prevTg;
    if (Cosi < 0.) { // angle 3d>pi/2. --> return back
      return Blend_Backward;
    }
    
    Cosi2 = Cosi * Cosi / prevNorme / Norme;
    if (Cosi2 < CosRef3D) { 
      return Blend_StepTooLarge;
    }
  }
  
  if(!curpointistangent){
    // Check if it is necessary to control the sign of prevtg*Tgsurf
    Cosi = sens * Corde * Tgsurf;
    Cosi2 = Cosi * Cosi / Tgsurf.SquareMagnitude() / Norme;
    if (Cosi2 < CosRef3D || Cosi < 0.) { 
      return Blend_StepTooLarge;
    }
  }  

  if (!curpointistangent && !prevpointistangent) {
    // Estimation of the current arrow
    Standard_Real FlecheCourante = 
      (prevTg.Normalized().XYZ() - Tgsurf.Normalized().XYZ()).SquareModulus() * Norme / 64.;
    
    if (FlecheCourante <= 0.25 * fleche * fleche) {
      return Blend_StepTooSmall;
    }
    if (FlecheCourante > fleche * fleche) {
      // not too great
      return Blend_StepTooLarge;
    }
  }
  return Blend_OK;
}


//=======================================================================
//function : CheckDeflectionOnRst2
//purpose  : 
//=======================================================================

Blend_Status BRepBlend_RstRstLineBuilder::CheckDeflectionOnRst2(const Blend_Point& CurPoint)
{
  //3D Controls of Blend_CSWalking.

  // rule by tests in U4 corresponding to 11.478 d
  const Standard_Real CosRef3D = 0.98;
  Standard_Real Cosi, Cosi2;
  Standard_Boolean curpointistangent  = CurPoint.IsTangencyPoint();
  Standard_Boolean prevpointistangent = previousP.IsTangencyPoint();

  gp_Pnt Psurf = CurPoint.PointOnC2();
  gp_Vec Tgsurf;

  if (!curpointistangent) {
    Tgsurf = CurPoint.TangentOnC2();
  }
  gp_Pnt prevP = previousP.PointOnC2();
  gp_Vec prevTg;
  if (!prevpointistangent) {
    prevTg = previousP.TangentOnC2();
  }
  Standard_Real Norme;
  Standard_Real prevNorme = 0.;
  gp_Vec Corde(prevP, Psurf);
  Norme = Corde.SquareMagnitude();
  if (!prevpointistangent) prevNorme = prevTg.SquareMagnitude();

  if (Norme <= tolesp * tolesp){
    // it can be necessary to force the same point
    return Blend_SamePoints;
  }
  if (!prevpointistangent) {
    if (prevNorme <= tolesp * tolesp) {
      return Blend_SamePoints;
    }
    Cosi = sens * Corde * prevTg;
    if (Cosi < 0.) { // angle 3d>pi/2. --> return back
      return Blend_Backward;
    }
    
    Cosi2 = Cosi * Cosi / prevNorme / Norme;
    if (Cosi2 < CosRef3D) { 
      return Blend_StepTooLarge;
    }
  }
  
  if (!curpointistangent) {
    // Check if it is necessary to control the sign of prevtg*Tgsurf
    Cosi  = sens * Corde * Tgsurf;
    Cosi2 = Cosi * Cosi / Tgsurf.SquareMagnitude() / Norme;
    if (Cosi2 < CosRef3D || Cosi < 0.) { 
      return Blend_StepTooLarge;
    }
  }  

  if(!curpointistangent && !prevpointistangent){
    // Estimation of the current arrow
    Standard_Real FlecheCourante = 
      (prevTg.Normalized().XYZ() - Tgsurf.Normalized().XYZ()).SquareModulus() * Norme/64.;
    
    if (FlecheCourante <= 0.25 * fleche * fleche) {
      return Blend_StepTooSmall;
    }
    if (FlecheCourante > fleche * fleche) {
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

Blend_Status BRepBlend_RstRstLineBuilder::TestArret(Blend_RstRstFunction& Func,
						    const Standard_Boolean TestDeflection,
						    const Blend_Status     State) 
{
  gp_Pnt ptrst1, ptrst2;
  gp_Pnt2d pt2drst1, pt2drst2;
  gp_Vec tgrst1, tgrst2;
  gp_Vec2d tg2drst1, tg2drst2;
  Blend_Status StateRst1, StateRst2;
  IntSurf_TypeTrans trarst1 = IntSurf_Undecided, trarst2 = IntSurf_Undecided;
  Blend_Point curpoint;

  if (Func.IsSolution(sol, tolesp)) {
    Standard_Boolean curpointistangent = Func.IsTangencyPoint();
    ptrst1   = Func.PointOnRst1();
    ptrst2   = Func.PointOnRst2();
    pt2drst1 = Func.Pnt2dOnRst1();
    pt2drst2 = Func.Pnt2dOnRst2();

    if(curpointistangent){
      curpoint.SetValue(ptrst1, ptrst2, param, pt2drst1.X(), pt2drst1.Y(), 
			pt2drst2.X(), pt2drst2.Y(), sol(1), sol(2));
    }
    else{
      tgrst1   = Func.TangentOnRst1();
      tgrst2   = Func.TangentOnRst2();
      tg2drst1 = Func.Tangent2dOnRst1();
      tg2drst2 = Func.Tangent2dOnRst2();
      curpoint.SetValue(ptrst1, ptrst2, param, pt2drst1.X(), pt2drst1.Y(), 
			pt2drst2.X(), pt2drst2.Y(), sol(1), sol(2),
			tgrst1, tgrst2, tg2drst1, tg2drst2);
    }
    if (TestDeflection) {
      StateRst1 = CheckDeflectionOnRst1(curpoint);
      StateRst2 = CheckDeflectionOnRst2(curpoint);
    }
    else {
      StateRst1 = StateRst2 = Blend_OK;
    }
    if (StateRst1 == Blend_Backward) {
      StateRst1 = Blend_StepTooLarge;
      rebrou    = Standard_True;
    }
    if (StateRst2 == Blend_Backward) {
      StateRst2 = Blend_StepTooLarge;
      rebrou    = Standard_True;
    }
    if (StateRst1 == Blend_StepTooLarge ||
	StateRst2 == Blend_StepTooLarge) {
      return Blend_StepTooLarge;
    }

    if (!comptra && !curpointistangent) {
      gp_Pnt2d p2drstref;
      gp_Vec2d tg2drstref;
      rst1->D1(sol(1), p2drstref, tg2drstref);
      Standard_Real testra = tg2drst1.Dot(tg2drstref);
      TopAbs_Orientation Or = domain1->Orientation(rst1);

      if (Abs(testra) > tolesp) {
	if (testra < 0.) {
          trarst1 = ConvOrToTra(TopAbs::Reverse(Or));
	}
	else if (testra >0.) {
	  trarst1 = ConvOrToTra(Or);
	}

	rst2->D1(sol(2), p2drstref, tg2drstref);
	testra = tg2drst2.Dot(tg2drstref);

	Or = domain2->Orientation(rst2);
	if (Abs(testra) > tolesp) {
	  if (testra < 0.) {
	    trarst2 = ConvOrToTra(TopAbs::Reverse(Or));
	  }
	  else if (testra >0.) {
	    trarst2 = ConvOrToTra(Or);
	  }
	  comptra = Standard_True;
	  line->Set(trarst1, trarst2);
	}
      }
    }
    if (StateRst1 == Blend_OK ||
	StateRst2 == Blend_OK ) {
      previousP = curpoint;
      return State;
    }
    if (StateRst1 == Blend_StepTooSmall &&
	StateRst2 == Blend_StepTooSmall) {
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

Standard_Boolean BRepBlend_RstRstLineBuilder::CheckInside(Blend_RstRstFunction&  Func,
							  TopAbs_State&          SituOnC1,
							  TopAbs_State&          SituOnC2,
							  Blend_DecrochStatus &  Decroch)
{
//  Standard_Boolean inside = Standard_True;
  math_Vector tolerance(1, 2);
  Func.GetTolerance(tolerance, tolesp);

  //face pcurve 1.
  Standard_Real v = sol(1);
  if(v < rst1->FirstParameter() - tolerance(2)||
     v > rst1->LastParameter() + tolerance(2)){
    SituOnC1 = TopAbs_OUT;
  }
  else if (v > rst1->FirstParameter() &&
	   v < rst1->LastParameter()){
    SituOnC1 = TopAbs_IN;
  }
  else SituOnC1 = TopAbs_ON;

  //face pcurve 2.
  v = sol(2);
  if(v < rst2->FirstParameter() - tolerance(2)||
     v > rst2->LastParameter() + tolerance(2)){
    SituOnC2 = TopAbs_OUT;
  }
  else if (v > rst2->FirstParameter() &&
	   v < rst2->LastParameter()){
    SituOnC2 = TopAbs_IN;
  }
  else SituOnC2 = TopAbs_ON;


  //lost contact
  gp_Vec tgrst1, norst1, tgrst2, norst2;
  Decroch = Func.Decroch(sol,tgrst1, norst1, tgrst2, norst2);

  return (SituOnC1 == TopAbs_IN && SituOnC2 == TopAbs_IN && Decroch == Blend_NoDecroch);
}



