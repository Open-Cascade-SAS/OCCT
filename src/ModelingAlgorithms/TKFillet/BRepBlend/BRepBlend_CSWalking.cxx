// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2024 OPEN CASCADE SAS
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

#include <BRepBlend_CSWalking.hxx>

#include <Adaptor2d_Curve2d.hxx>
#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_HSurfaceTool.hxx>
#include <Adaptor3d_HVertex.hxx>
#include <Adaptor3d_Surface.hxx>
#include <Adaptor3d_TopolTool.hxx>
#include <BRepBlend_BlendTool.hxx>
#include <BRepBlend_Extremity.hxx>
#include <BRepBlend_HCurve2dTool.hxx>
#include <BRepBlend_HCurveTool.hxx>
#include <BRepBlend_Line.hxx>
#include <BRepBlend_PointOnRst.hxx>
#include <Blend_CSFunction.hxx>
#include <IntSurf.hxx>
#include <IntSurf_Transition.hxx>
#include <Precision.hxx>
#include <StdFail_NotDone.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <math_FunctionSetRoot.hxx>
#include <math_Gauss.hxx>

#ifdef OCCT_DEBUG
  #include <Geom_BSplineCurve.hxx>
  #include <TColStd_Array1OfInteger.hxx>
  #include <TColStd_Array1OfReal.hxx>
  #include <TColgp_Array1OfPnt2d.hxx>
  #ifdef DRAW
    #include <DrawTrSurf.hxx>
  #endif
  // POP pour NT
  #include <stdio.h>

static Standard_Integer IndexOfSection = 0;
extern Standard_Boolean Blend_GettraceDRAWSECT();

// Pour debug : visualisation de la section

static void Drawsect(const Handle(Adaptor3d_Surface)& surf,
                     const Handle(Adaptor3d_Curve)&   curv,
                     const Standard_Real              param,
                     Blend_CSFunction&                Func)
{
  gp_Pnt2d      p2d = Func.Pnt2d();
  Standard_Real pc  = Func.ParameterOnC();
  Blend_Point   BP(Adaptor3d_HSurfaceTool::Value(surf, p2d.X(), p2d.Y()),
                 BRepBlend_HCurveTool::Value(curv, pc),
                 param,
                 p2d.X(),
                 p2d.Y(),
                 pc);

  Standard_Integer hp, hk, hd, hp2d;
  Func.GetShape(hp, hk, hd, hp2d);
  TColStd_Array1OfReal TK(1, hk);
  Func.Knots(TK);
  TColStd_Array1OfInteger TMul(1, hk);
  Func.Mults(TMul);
  TColgp_Array1OfPnt   TP(1, hp);
  TColgp_Array1OfPnt2d TP2d(1, hp2d);
  TColStd_Array1OfReal TW(1, hp);
  Func.Section(BP, TP, TP2d, TW);
  Handle(Geom_BSplineCurve) sect = new Geom_BSplineCurve(TP, TW, TK, TMul, hd);
  IndexOfSection++;

  // POP pour NT
  // char name[100];
  char* name = new char[100];
  sprintf(name, "%s_%d", "Section", IndexOfSection);
  #ifdef DRAW
  DrawTrSurf::Set(name, sect);
  #endif
}

#endif

BRepBlend_CSWalking::BRepBlend_CSWalking(const Handle(Adaptor3d_Curve)&     Curv,
                                         const Handle(Adaptor3d_Surface)&   Surf,
                                         const Handle(Adaptor3d_TopolTool)& Domain)
    : done(Standard_False),
      surf(Surf),
      curv(Curv)
{
  domain = Domain;
}

void BRepBlend_CSWalking::Perform(Blend_CSFunction& Func,
                                  //			      Blend_CSFuncInv& FuncInv,
                                  const Standard_Real    Pdep,
                                  const Standard_Real    Pmax,
                                  const Standard_Real    MaxStep,
                                  const Standard_Real    Tol3d,
                                  const Standard_Real    TolGuide,
                                  const math_Vector&     ParDep,
                                  const Standard_Real    Fleche,
                                  const Standard_Boolean Appro)
{
  done                   = Standard_False;
  iscomplete             = Standard_False;
  comptra                = Standard_False;
  line                   = new BRepBlend_Line();
  Standard_Integer Nbvar = Func.NbVariables();
  tolpoint3d             = Tol3d;
  tolgui                 = Abs(TolGuide);
  fleche                 = Abs(Fleche);
  rebrou                 = Standard_False;
  pasmax                 = Abs(MaxStep);
  math_Vector sol(1, Nbvar);

  firstsol = new TColStd_HArray1OfReal(1, Nbvar);

  if (Pmax - Pdep >= 0.)
  {
    sens = 1.;
  }
  else
  {
    sens = -1.;
  }

  Blend_Status        State;
  BRepBlend_Extremity ptf1, ptf2;

  param      = Pdep;
  firstparam = Pdep;
  Func.Set(param);

  if (Appro)
  {
    TopAbs_State situ;
    //    math_Vector tolerance(1,3),infbound(1,3),supbound(1,3);
    math_Vector tolerance(1, Nbvar), infbound(1, Nbvar), supbound(1, Nbvar);
    Func.GetTolerance(tolerance, tolpoint3d);
    Func.GetBounds(infbound, supbound);
    math_FunctionSetRoot rsnld(Func, tolerance, 30);

    rsnld.Perform(Func, ParDep, infbound, supbound);

    if (!rsnld.IsDone())
    {
      return;
    }
    rsnld.Root(sol);

    //    situ1 = Adaptor3d_TopolTool::Classify(surf1,gp_Pnt2d(sol(1),sol(2)),
    //				   Max(tolerance(1),tolerance(2)));
    //    situ2 = Adaptor3d_TopolTool::Classify(surf2,gp_Pnt2d(sol(3),sol(4)),
    //				   Max(tolerance(3),tolerance(4)));
    /*
situ = domain->Classify(gp_Pnt2d(sol(1),sol(2)),
     Min(tolerance(1),tolerance(2)));
*/
    situ = domain->Classify(Func.Pnt2d(), Min(tolerance(1), tolerance(2)));

    if (situ != TopAbs_IN)
    {
      return;
    }
  }
  else
  {
    sol = ParDep;
  }

  for (Standard_Integer i = 1; i <= Nbvar; i++)
  {
    firstsol->ChangeValue(i) = sol(i);
  }
  State = TestArret(Func, sol, Standard_False, Blend_OK);
  if (State != Blend_OK)
  {
    return;
  }
#ifdef OCCT_DEBUG
  if (Blend_GettraceDRAWSECT())
  {
    Drawsect(surf, curv, param, Func);
  }
#endif
  // Mettre a jour la ligne.
  line->Append(previousP);
  Standard_Real U, V, W;
  previousP.ParametersOnS(U, V);
  W = previousP.ParameterOnC();
  BRepBlend_Extremity P1(previousP.PointOnS(), U, V, previousP.Parameter(), tolpoint3d);
  BRepBlend_Extremity P2(previousP.PointOnC(), W, previousP.Parameter(), tolpoint3d);
  if (sens > 0.)
  {
    line->SetStartPoints(P1, P2);
  }
  else
  {
    line->SetEndPoints(P1, P2);
  }

  //  InternalPerform(Func,FuncInv,Pmax);
  InternalPerform(Func, sol, Pmax);

  done = Standard_True;
}

Standard_Boolean BRepBlend_CSWalking::Complete(Blend_CSFunction& Func,
                                               //					   Blend_CSFuncInv& FuncInv,
                                               const Standard_Real Pmin)
{
  if (!done)
  {
    throw StdFail_NotDone();
  }
  if (iscomplete)
  {
    return Standard_True;
  }

  /*
if (sens >0.) {
previousP = line->Point(1);
}
else {
previousP = line->Point(line->NbPoints());
}
*/

  sens = -sens;
  /*
param = previousP.Parameter();
previousP.ParametersOnS(sol(1),sol(2));
sol(3) = previousP.ParameterOnC();
*/
  Standard_Integer Nbvar = Func.NbVariables();
  math_Vector      sol(1, Nbvar);
  for (Standard_Integer i = 1; i <= Nbvar; i++)
  {
    sol(i) = firstsol->Value(i);
  }

  param = firstparam;

  //  InternalPerform(Func,FuncInv,Pmin);
  InternalPerform(Func, sol, Pmin);

  sens       = -sens;
  iscomplete = Standard_True;
  return Standard_True;
}

Blend_Status BRepBlend_CSWalking::TestArret(Blend_CSFunction&      Function,
                                            const math_Vector&     Sol,
                                            const Standard_Boolean TestDefl,
                                            const Blend_Status     State)

// On regarde si le point donne est solution.
// Si c est le cas,
//  On verifie le critere de fleche sur surf et curv
//   Si OK, on classifie les point sur surf
//    Si le point est  dedans : on retourne Blend_OK
//    sinon on resout le pb inverse sur la surface
//   sinon (fleche non OK)
//    on renvoie Blend_StepTooLarge.
// sinon on renvoie Blend_StepTooLarge.
//

{
  gp_Pnt            pt1, pt2;
  gp_Vec            V1, V2;
  gp_Vec            Tgp1, Nor1;
  gp_Vec2d          V12d;
  gp_Pnt2d          pt2d;
  Standard_Real     pOnC;
  Blend_Status      State1, State2;
  IntSurf_TypeTrans tras = IntSurf_Undecided;

  if (Function.IsSolution(Sol, tolpoint3d))
  {
    pt1  = Function.PointOnS();
    pt2  = Function.PointOnC();
    pt2d = Function.Pnt2d();
    pOnC = Function.ParameterOnC();
    V1   = Function.TangentOnS();
    V2   = Function.TangentOnC();
    V12d = Function.Tangent2d();

    if (TestDefl)
    {
      // Verification du critere de fleche sur chaque surface
      // et sur la ligne guide

      State1 = CheckDeflectionOnSurf(pt1,
                                     //				     gp_Pnt2d(sol(1),sol(2)),
                                     pt2d,
                                     V1,
                                     V12d);
      //      State2 = CheckDeflectionOnCurv(pt2,sol(3),V2);
      // Pour des pb dans les cheminements point/face on met
      // temporairement le test sur la courbe au placard.
      //      State2 = CheckDeflectionOnCurv(pt2,pOnC,V2);
      State2 = Blend_StepTooSmall;
    }
    else
    {
      State1 = Blend_OK;
      State2 = Blend_OK;
    }

    if (State1 == Blend_Backward)
    {
      State1 = Blend_StepTooLarge;
      rebrou = Standard_True;
    }

    if (State2 == Blend_Backward)
    {
      State2 = Blend_StepTooLarge;
      rebrou = Standard_True;
    }

    if (State1 == Blend_StepTooLarge || State2 == Blend_StepTooLarge)
    {
      return Blend_StepTooLarge;
    }

    if (!comptra)
    {
      //      Function.Tangent(sol(1),sol(2),Tgp1,Nor1);
      Function.Tangent(pt2d.X(), pt2d.Y(), Tgp1, Nor1);
      Standard_Real testra = Tgp1.Dot(Nor1.Crossed(V1));
      if (Abs(testra) > Precision::Confusion())
      {
        if (testra < 0.)
        {
          tras = IntSurf_In;
        }
        else if (testra > 0.)
        {
          tras = IntSurf_Out;
        }
        comptra = Standard_True;
        line->Set(tras);
      }
    }

    if (State1 == Blend_OK || State2 == Blend_OK)
    {
      previousP.SetValue(Function.PointOnS(),
                         Function.PointOnC(),
                         param,
                         //			 sol(1),sol(2),
                         //			 sol(3),
                         pt2d.X(),
                         pt2d.Y(),
                         pOnC,
                         V1,
                         V2,
                         V12d);

      return State;
    }
    if (State1 == Blend_StepTooSmall && State2 == Blend_StepTooSmall)
    {
      previousP.SetValue(Function.PointOnS(),
                         Function.PointOnC(),
                         param,
                         //			 sol(1),sol(2),
                         //			 sol(3),
                         pt2d.X(),
                         pt2d.Y(),
                         pOnC,
                         V1,
                         V2,
                         V12d);
      if (State == Blend_OK)
      {
        return Blend_StepTooSmall;
      }
      else
      {
        return State;
      }
    }

    if (State == Blend_OK)
    {
      return Blend_SamePoints;
    }
    else
    {
      return State;
    }
  }
  else
  {
    return Blend_StepTooLarge;
  }
}

Blend_Status BRepBlend_CSWalking::CheckDeflectionOnSurf(const gp_Pnt&   Psurf,
                                                        const gp_Pnt2d& Ponsurf,
                                                        const gp_Vec&   Tgsurf,
                                                        const gp_Vec2d& Tgonsurf)
{
  // regle par tests dans U4 correspond a 11.478 d
  const Standard_Real CosRef3D = 0.98;

  const Standard_Real CosRef2D = 0.88; // correspond a 25 d

  Standard_Real Norme, prevNorme, Cosi, Cosi2; // JAG MODIF 25.04.94
  Standard_Real FlecheCourante;
  Standard_Real Du, Dv, Duv;
  Standard_Real paramu, paramv, tolu, tolv;
  //  TColgp_Array1OfPnt Poles(1,4);
  //  gp_Pnt POnCurv,Milieu;
  gp_Pnt   prevP;
  gp_Vec   prevTg;
  gp_Vec2d previousd2d;

  prevP  = previousP.PointOnS();
  prevTg = previousP.TangentOnS();
  tolu   = Adaptor3d_HSurfaceTool::UResolution(surf, tolpoint3d);
  tolv   = Adaptor3d_HSurfaceTool::VResolution(surf, tolpoint3d);

  gp_Vec Corde(prevP, Psurf);
  Norme     = Corde.SquareMagnitude();
  prevNorme = prevTg.SquareMagnitude(); // JAG MODIF 25.04.94

  const Standard_Real toler3d = tolpoint3d;
  if (Norme <= toler3d * toler3d || prevNorme <= toler3d * toler3d)
  { // JAG MODIF 25.04.94
    // il faudra peut etre  forcer meme point  JAG MODIF 25.04.94
    return Blend_SamePoints;
  }
  Cosi = sens * Corde * prevTg;
  if (Cosi < 0.)
  { // angle 3d>pi/2. --> retour arriere
    return Blend_Backward;
  }

  Cosi2 = Cosi * Cosi / prevNorme / Norme;
  if (Cosi2 < CosRef3D)
  {
    return Blend_StepTooLarge;
  }

  previousP.ParametersOnS(paramu, paramv);
  previousd2d = previousP.Tangent2d();

  Du  = Ponsurf.X() - paramu;
  Dv  = Ponsurf.Y() - paramv;
  Duv = Du * Du + Dv * Dv;
  if ((Abs(Du) < tolu && Abs(Dv) < tolv) || // JAG MODIF 25.04.94
      (Abs(previousd2d.X()) < tolu && Abs(previousd2d.Y()) < tolv))
  {
    // il faudra peut etre  forcer meme point   JAG MODIF 25.04.94
    return Blend_SamePoints; // point confondu 2d
  }
  Cosi = sens * (Du * previousd2d.X() + Dv * previousd2d.Y());
  if (Cosi < 0)
  {
    return Blend_Backward;
  }

  // Voir s il faut faire le controle sur le signe de prevtg*Tgsurf
  Cosi  = sens * Corde * Tgsurf;
  Cosi2 = Cosi * Cosi / Tgsurf.SquareMagnitude() / Norme;
  if (Cosi2 < CosRef3D || Cosi < 0.)
  {
    return Blend_StepTooLarge;
  }

  // Voir s il faut faire le controle sur le signe de Cosi
  Cosi  = sens * (Du * Tgonsurf.X() + Dv * Tgonsurf.Y()) / Tgonsurf.Magnitude();
  Cosi2 = Cosi * Cosi / Duv;
  if (Cosi2 < CosRef2D || Cosi < 0.)
  {
    return Blend_StepTooLarge;
  }

  // Estimation de la fleche courante
  /*
Norme = Sqrt(Norme)/3.;
Poles(1) = prevP;
Poles(4) = Psurf;
Poles(2) = Poles(1).XYZ() + sens*Norme* prevTg.Normalized().XYZ();
Poles(3) = Poles(4).XYZ() - sens*Norme* Tgsurf.Normalized().XYZ();
BzCLib::PntPole(0.5,Poles,POnCurv);
Milieu = (Poles(1).XYZ() + Poles(4).XYZ())*0.5;
FlecheCourante = Milieu.Distance(POnCurv);

if (FlecheCourante <= 0.5*fleche) {
*/
  FlecheCourante =
    (prevTg.Normalized().XYZ() - Tgsurf.Normalized().XYZ()).SquareModulus() * Norme / 64.;

  if (FlecheCourante <= 0.25 * fleche * fleche)
  {
    return Blend_StepTooSmall;
  }
  if (FlecheCourante > fleche * fleche)
  {
    // pas trop grand : commentaire interessant
    return Blend_StepTooLarge;
  }

  return Blend_OK;
}

Blend_Status BRepBlend_CSWalking::CheckDeflectionOnCurv(const gp_Pnt&       Pcurv,
                                                        const Standard_Real Param,
                                                        const gp_Vec&       Tgcurv)
{
  // regle par tests dans U4 correspond a 11.478 d
  const Standard_Real CosRef3D = 0.98;

  Standard_Real Norme, prevNorme, Cosi, Cosi2; // JAG MODIF 25.04.94
  Standard_Real FlecheCourante;
  Standard_Real Du, paramu, tolu;
  //  TColgp_Array1OfPnt Poles(1,4);
  //  gp_Pnt POnCurv,Milieu;
  gp_Pnt prevP;
  gp_Vec prevTg;

  prevP  = previousP.PointOnC();
  prevTg = previousP.TangentOnC();
  tolu   = BRepBlend_HCurveTool::Resolution(curv, tolpoint3d);

  gp_Vec Corde(prevP, Pcurv);
  Norme     = Corde.SquareMagnitude();
  prevNorme = prevTg.SquareMagnitude(); // JAG MODIF 25.04.94

  const Standard_Real toler3d = tolpoint3d;
  // if (Norme <= tolesp*tolesp || prevNorme <= tolesp*tolesp) { // JAG MODIF 25.04.94
  if (Norme <= toler3d * toler3d)
  { // le 95.01.10
    // il faudra peut etre  forcer meme point  JAG MODIF 25.04.94
    return Blend_SamePoints;
  }
  else if (prevNorme > toler3d * toler3d)
  {
    Cosi = sens * Corde * prevTg;
    if (Cosi < 0.)
    { // angle 3d>pi/2. --> retour arriere
      return Blend_Backward;
    }

    Cosi2 = Cosi * Cosi / prevNorme / Norme;
    if (Cosi2 < CosRef3D)
    {
      return Blend_StepTooLarge;
    }
  }

  paramu = previousP.ParameterOnC();
  Du     = Param - paramu;
  if (Abs(Du) < tolu)
  {
    // il faudra peut etre  forcer meme point   JAG MODIF 25.04.94
    return Blend_SamePoints; // point confondu 2d
  }

  // Voir s il faut faire le controle sur le signe de prevtg*Tgsurf

  if (Tgcurv.Magnitude() <= tolpoint3d)
  {
    return Blend_SamePoints; // GROS BOBARD EN ATTENDANT
  }

  Cosi  = sens * Corde * Tgcurv;
  Cosi2 = Cosi * Cosi / Tgcurv.SquareMagnitude() / Norme;
  if (Cosi2 < CosRef3D || Cosi < 0.)
  {
    return Blend_StepTooLarge;
  }

  if (prevNorme > toler3d * toler3d)
  {
    // Estimation de la fleche courante
    /*
Norme = Sqrt(Norme)/3.;
Poles(1) = prevP;
Poles(4) = Pcurv;
Poles(2) = Poles(1).XYZ() + sens*Norme* prevTg.Normalized().XYZ();
Poles(3) = Poles(4).XYZ() - sens*Norme* Tgcurv.Normalized().XYZ();
BzCLib::PntPole(0.5,Poles,POnCurv);
Milieu = (Poles(1).XYZ() + Poles(4).XYZ())*0.5;
FlecheCourante = Milieu.Distance(POnCurv);
if (FlecheCourante <= 0.5*fleche) {
*/
    FlecheCourante =
      (prevTg.Normalized().XYZ() - Tgcurv.Normalized().XYZ()).SquareModulus() * Norme / 64.;

    if (FlecheCourante <= 0.25 * fleche * fleche)
    {
      return Blend_StepTooSmall;
    }
    if (FlecheCourante > fleche * fleche)
    {
      // pas trop grand : commentaire interessant
      return Blend_StepTooLarge;
    }
  }
  return Blend_OK;
}

/*
Standard_Boolean BRepBlend_CSWalking::Recadre(Blend_FuncInv& FuncInv,
                      const Standard_Boolean OnFirst,
                      const math_Vector& sol,
                      math_Vector& solrst,
                      Standard_Integer& Indexsol,
                      Standard_Boolean& IsVtx,
                      Handle(Adaptor3d_HVertex)& Vtx)

{
  Standard_Integer nbarc;
  Standard_Boolean ok,yamin;
  Standard_Real dist,distmin,prm,pmin;
  gp_Pnt2d pt2d;
  math_Vector toler(1,4),infb(1,4),supb(1,4),valsol(1,4);

  Handle(Adaptor3d_TopolTool) Iter;
  TopAbs_State situ;

  yamin = Standard_False;
  nbarc = 0;
  distmin = RealLast();
  if (OnFirst) {
    pt2d.SetCoord(sol(1),sol(2));
    Iter = domain1;
  }
  else {
    pt2d.SetCoord(sol(3),sol(4));
    Iter = domain2;
  }
  Iter->Init();
  while (Iter->More()) {
    nbarc++;
    if (OnFirst) {
      ok = BRepBlend_BlendTool::Project(pt2d,surf1,Iter->Value(),prm,dist);
    }
    else {
      ok = BRepBlend_BlendTool::Project(pt2d,surf2,Iter->Value(),prm,dist);
    }
    if (ok) {
      if (dist<distmin) {
    yamin = Standard_True;
    distmin = dist;
    pmin = prm;
    Indexsol = nbarc;
      }
    }
    Iter->Next();
  }

  IsVtx = Standard_False;
  if (!yamin) {
    return Standard_False;
  }

  Iter->Init();
  nbarc = 1;
  while (nbarc < Indexsol) {
    nbarc++;
    Iter->Next();
  }

  Handle(Adaptor2d_Curve2d) thearc = Iter->Value();
  Handle(Adaptor2d_Curve2d) thecur;
  if (OnFirst) {
    thecur = BRepBlend_BlendTool::CurveOnSurf(thearc,surf1);
  }
  else {
    thecur = BRepBlend_BlendTool::CurveOnSurf(thearc,surf2);
  }

  FuncInv.Set(OnFirst,thecur);

  FuncInv.GetTolerance(toler,tolesp);
  FuncInv.GetBounds(infb,supb);
  solrst(1) = pmin;
  solrst(2) = param;

  if (OnFirst) {
    solrst(3) = sol(3);
    solrst(4) = sol(4);
  }
  else {
    solrst(3) = sol(1);
    solrst(4) = sol(2);
  }

  math_FunctionSetRoot rsnld(FuncInv,toler,30);
  rsnld.Perform(FuncInv,solrst,infb,supb);



  if (!rsnld.IsDone()) {
#ifdef OCCT_DEBUG
    std::cout << "RSNLD not done "<< std::endl << std::endl;
#endif
    return Standard_False;
  }

  // On doit verifier la valeur de la fonction
  rsnld.Root(solrst);

  if (FuncInv.IsSolution(solrst,tolesp)) {

//    if (OnFirst) {
//      situ = Adaptor3d_TopolTool::Classify(surf2,gp_Pnt2d(solrst(3),solrst(4)),
//				    Max(toler(3),toler(4)));
//
//    }
//    else {
//      situ = Adaptor3d_TopolTool::Classify(surf1,gp_Pnt2d(solrst(3),solrst(4)),
//				    Max(toler(3),toler(4)));
//    }

    if (OnFirst) {
      situ = domain2->Classify(gp_Pnt2d(solrst(3),solrst(4)),
                   Min(toler(3),toler(4)));

    }
    else {
      situ = domain1->Classify(gp_Pnt2d(solrst(3),solrst(4)),
                   Min(toler(3),toler(4)));
    }


    if ((situ != TopAbs_IN) && (situ != TopAbs_ON)) {
      return Standard_False;
    }

    Iter->Initialize(thearc);
    Iter->InitVertexIterator();
    IsVtx = !Iter->MoreVertex();
    while (!IsVtx) {
      Vtx = Iter->Vertex();
      if (Abs(BRepBlend_BlendTool::Parameter(Vtx,thearc)-solrst(1)) <=
      BRepBlend_BlendTool::Tolerance(Vtx,thearc)) {
    IsVtx = Standard_True;
      }
      else {
    Iter->NextVertex();
    IsVtx = !Iter->MoreVertex();
      }
    }
    if (!Iter->MoreVertex()) {
      IsVtx = Standard_False;
    }

    return Standard_True;
  }

  return Standard_False;
}

*/

void BRepBlend_CSWalking::Transition(const Handle(Adaptor2d_Curve2d)& A,
                                     const Standard_Real              Param,
                                     IntSurf_Transition&              TLine,
                                     IntSurf_Transition&              TArc)
{
  gp_Pnt2d p2d;
  gp_Vec2d dp2d;

  gp_Pnt pbid;
  gp_Vec d1u, d1v, normale, tgrst;

  BRepBlend_HCurve2dTool::D1(A, Param, p2d, dp2d);
  Adaptor3d_HSurfaceTool::D1(surf, p2d.X(), p2d.Y(), pbid, d1u, d1v);

  tgrst.SetLinearForm(dp2d.X(), d1u, dp2d.Y(), d1v);
  normale = d1u.Crossed(d1v);

  IntSurf::MakeTransition(previousP.TangentOnS(), tgrst, normale, TLine, TArc);
}

void BRepBlend_CSWalking::MakeExtremity(BRepBlend_Extremity&             Extrem,
                                        const Standard_Integer           Index,
                                        const Standard_Real              Param,
                                        const Standard_Boolean           IsVtx,
                                        const Handle(Adaptor3d_HVertex)& Vtx)
{
  IntSurf_Transition          Tline, Tarc;
  Standard_Real               prm, U, V;
  Standard_Integer            nbarc;
  Handle(Adaptor3d_TopolTool) Iter;

  //  Extrem.SetValue(previousP.PointOnS(),sol(1),sol(2),tolesp);
  previousP.ParametersOnS(U, V);
  Extrem.SetValue(previousP.PointOnS(), U, V, previousP.Parameter(), tolpoint3d);
  Iter = domain;

  Iter->Init();
  nbarc = 1;

  if (!IsVtx)
  {
    while (nbarc < Index)
    {
      nbarc++;
      Iter->Next();
    }
    Transition(Iter->Value(), Param, Tline, Tarc);
    Extrem.AddArc(Iter->Value(), Param, Tline, Tarc);
  }

  else
  {
    Extrem.SetVertex(Vtx);
    while (Iter->More())
    {
      Handle(Adaptor2d_Curve2d) arc = Iter->Value();
      if (nbarc != Index)
      {
        Iter->Initialize(arc);
        Iter->InitVertexIterator();
        while (Iter->MoreVertex())
        {
          //	  if (Adaptor3d_TopolTool::Identical(Vtx,Iter.Vertex())) {
          if (Iter->Identical(Vtx, Iter->Vertex()))
          {
            prm = BRepBlend_BlendTool::Parameter(Vtx, arc);
            Transition(arc, prm, Tline, Tarc);
            Extrem.AddArc(arc, prm, Tline, Tarc);
          }
          Iter->NextVertex();
        }
      }
      else
      {
        Transition(arc, Param, Tline, Tarc);
        Extrem.AddArc(arc, Param, Tline, Tarc);
      }
      nbarc++;
      Iter->Next();
    }
  }
}

void BRepBlend_CSWalking::InternalPerform(Blend_CSFunction& Func,
                                          //				    Blend_CSFuncInv& FuncInv,
                                          math_Vector&        sol,
                                          const Standard_Real Bound)
{
  Standard_Real stepw   = pasmax;
  Standard_Real parprec = param;

  Blend_Status     State;
  TopAbs_State     situ;
  Standard_Real    w, U, V;
  Standard_Integer nbarc;
  Standard_Integer Index = 0;
  Standard_Boolean Isvtx = Standard_False;
  Standard_Integer Nbvar = Func.NbVariables();
  Standard_Boolean Arrive, recad, echecrecad;
  gp_Pnt2d         p2d;
  //  math_Vector tolerance(1,3),infbound(1,3),supbound(1,3),parinit(1,3);
  //  math_Vector solrst(1,3);
  math_Vector tolerance(1, Nbvar), infbound(1, Nbvar), supbound(1, Nbvar), parinit(1, Nbvar);
  math_Vector solrst(1, Nbvar);
  Handle(Adaptor3d_HVertex) Vtx;
  BRepBlend_Extremity       Exts, Extc;

  // IntSurf_Transition Tline,Tarc;

  Func.GetTolerance(tolerance, tolpoint3d);
  Func.GetBounds(infbound, supbound);

  math_FunctionSetRoot rsnld(Func, tolerance, 30);
  parinit = sol;

  param  = parprec + sens * stepw;
  Arrive = (sens * (param - Bound) > 0.);

  //  if (Arrive) {
  //    line->Clear();
  //  }

  while (!Arrive)
  {
    Func.Set(param);
    rsnld.Perform(Func, parinit, infbound, supbound);

    if (!rsnld.IsDone())
    {
      State = Blend_StepTooLarge;
    }
    else
    {
      rsnld.Root(sol);

      //      situ1 = Adaptor3d_TopolTool::Classify(surf1,gp_Pnt2d(sol(1),sol(2)),
      //				     Max(tolerance(1),tolerance(2)));
      //      situ2 = Adaptor3d_TopolTool::Classify(surf2,gp_Pnt2d(sol(3),sol(4)),
      //				     Max(tolerance(3),tolerance(4)));
      /*
        situ = domain->Classify(gp_Pnt2d(sol(1),sol(2)),
              Min(tolerance(1),tolerance(2)));
  */
      situ = domain->Classify(Func.Pnt2d(), Min(tolerance(1), tolerance(2)));

      w          = Bound;
      recad      = Standard_False;
      echecrecad = Standard_False;

      if (situ == TopAbs_OUT || situ == TopAbs_ON)
      {
        // pb inverse sur surf
        //	recad = Recadre(FuncInv,sol,solrst,Index,Isvtx,Vtx);
        Isvtx = Standard_False; // en attendant Recadre
        if (recad)
        {
          w = solrst(2);
        }
        else
        {
          echecrecad = Standard_True;
        }
      }
      if (!echecrecad)
      {
        if (recad)
        {
          // sol sur surf
          State = Blend_OnRst1;
          param = w;
          domain->Init();
          nbarc = 1;
          while (nbarc < Index)
          {
            nbarc++;
            domain->Next();
          }
          p2d    = BRepBlend_HCurve2dTool::Value(domain->Value(), solrst(1));
          sol(1) = p2d.X();
          sol(2) = p2d.Y();
          sol(3) = solrst(3);
          Func.Set(param);
        }
        else
        {
          State = Blend_OK;
        }
        State = TestArret(Func, sol, Standard_True, State);
      }
      else
      {
        // Echec recadrage. On sort avec PointsConfondus
        State = Blend_SamePoints;
      }
    }

    switch (State)
    {
      case Blend_OK: {
#ifdef OCCT_DEBUG
        if (Blend_GettraceDRAWSECT())
        {
          Drawsect(surf, curv, param, Func);
        }
#endif

        // Mettre a jour la ligne.
        if (sens > 0.)
        {
          line->Append(previousP);
        }
        else
        {
          line->Prepend(previousP);
        }
        parinit = sol;
        parprec = param;

        if (param == Bound)
        {
          Arrive = Standard_True;
          /*
      Exts.SetValue(previousP.PointOnS(),sol(1),sol(2),tolesp);
      Extc.SetValue(previousP.PointOnC(),sol(3),tolesp);
  */
          previousP.ParametersOnS(U, V);
          Exts.SetValue(previousP.PointOnS(), U, V, previousP.Parameter(), tolpoint3d);
          Extc.SetValue(previousP.PointOnC(),
                        previousP.ParameterOnC(),
                        previousP.Parameter(),
                        tolpoint3d);
          // Indiquer que fin sur Bound.
        }
        else
        {
          param = param + sens * stepw;
          if (sens * (param - Bound) > -tolgui)
          {
            param = Bound;
          }
        }
      }
      break;

      case Blend_StepTooLarge: {
        stepw = stepw / 2.;
        if (Abs(stepw) < tolgui)
        {
          /*
      Exts.SetValue(previousP.PointOnS(),sol(1),sol(2),tolesp);
      Extc.SetValue(previousP.PointOnC(),sol(3),sol(4),tolesp);
  */
          previousP.ParametersOnS(U, V);
          Exts.SetValue(previousP.PointOnS(), U, V, previousP.Parameter(), tolpoint3d);
          Extc.SetValue(previousP.PointOnC(),
                        previousP.ParameterOnC(),
                        previousP.Parameter(),
                        tolpoint3d);
          Arrive = Standard_True;
          if (line->NbPoints() >= 2)
          {
            // Indiquer qu on s arrete en cours de cheminement
          }
          //	  else {
          //	    line->Clear();
          //	  }
        }
        else
        {
          param = parprec + sens * stepw; // on ne risque pas de depasser Bound.
        }
      }
      break;

      case Blend_StepTooSmall: {
#ifdef OCCT_DEBUG
        if (Blend_GettraceDRAWSECT())
        {
          Drawsect(surf, curv, param, Func);
        }
#endif
        // Mettre a jour la ligne.
        if (sens > 0.)
        {
          line->Append(previousP);
        }
        else
        {
          line->Prepend(previousP);
        }
        parinit = sol;
        parprec = param;

        stepw = Min(1.5 * stepw, pasmax);
        if (param == Bound)
        {
          Arrive = Standard_True;
          /*
      Exts.SetValue(previousP.PointOnS(),sol(1),sol(2),tolesp);
      Extc.SetValue(previousP.PointOnC(),sol(3),tolesp);
  */
          previousP.ParametersOnS(U, V);
          Exts.SetValue(previousP.PointOnS(), U, V, previousP.Parameter(), tolpoint3d);
          Extc.SetValue(previousP.PointOnC(),
                        previousP.ParameterOnC(),
                        previousP.Parameter(),
                        tolpoint3d);
          // Indiquer que fin sur Bound.
        }
        else
        {
          param = param + sens * stepw;
          if (sens * (param - Bound) > -tolgui)
          {
            param = Bound;
          }
        }
      }
      break;

      case Blend_OnRst1: {
#ifdef OCCT_DEBUG
        if (Blend_GettraceDRAWSECT())
        {
          Drawsect(surf, curv, param, Func);
        }
#endif
        if (sens > 0.)
        {
          line->Append(previousP);
        }
        else
        {
          line->Prepend(previousP);
        }
        MakeExtremity(Exts, Index, solrst(1), Isvtx, Vtx);
        //	Extc.SetValue(previousP.PointOnC(),sol(3),tolesp);
        Extc.SetValue(previousP.PointOnC(),
                      previousP.ParameterOnC(),
                      previousP.Parameter(),
                      tolpoint3d);
        Arrive = Standard_True;
      }
      break;

      case Blend_SamePoints: {
        // On arrete
        std::cout << " Points confondus dans le cheminement" << std::endl;
        /*
    Exts.SetValue(previousP.PointOnS(),sol(1),sol(2),tolesp);
    Extc.SetValue(previousP.PointOnC(),sol(3),tolesp);
  */
        previousP.ParametersOnS(U, V);
        Exts.SetValue(previousP.PointOnS(), U, V, previousP.Parameter(), tolpoint3d);
        Extc.SetValue(previousP.PointOnC(),
                      previousP.ParameterOnC(),
                      previousP.Parameter(),
                      tolpoint3d);
        Arrive = Standard_True;
      }
      break;

      default:
        break;
    }
    if (Arrive)
    {
      if (sens > 0.)
      {
        line->SetEndPoints(Exts, Extc);
      }
      else
      {
        line->SetStartPoints(Exts, Extc);
      }
    }
  }
}
