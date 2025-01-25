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

#include <BRepBlend_Walking.hxx>

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
#include <Blend_FuncInv.hxx>
#include <Blend_Function.hxx>
#include <Blend_Point.hxx>
#include <CSLib.hxx>
#include <CSLib_NormalStatus.hxx>
#include <ChFiDS_ElSpine.hxx>
#include <Extrema_ExtPC.hxx>
#include <Extrema_ExtPS.hxx>
#include <IntSurf.hxx>
#include <IntSurf_Transition.hxx>
#include <Precision.hxx>
#include <StdFail_NotDone.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array2OfVec.hxx>
#include <gce_MakePln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <math_FunctionSetRoot.hxx>
#include <math_Gauss.hxx>

#ifdef DRAW
  #include <DrawTrSurf.hxx>
#endif

#ifdef OCCT_DEBUG
  #include <Geom_BSplineCurve.hxx>
  #include <TColStd_Array1OfInteger.hxx>
  #include <TColStd_Array1OfReal.hxx>
  #include <TColgp_Array1OfPnt2d.hxx>
  #ifdef DRAW
    #include <Draw.hxx>
    #include <DrawTrSurf_BSplineCurve.hxx>
  #endif
  // POP pour NT
  #include <stdio.h>

static Standard_Boolean sectioncalculee;
static Standard_Integer IndexOfSection   = 0;
static Standard_Integer IndexOfRejection = 0;
static Standard_Integer nbcomputedsection;
extern Standard_Boolean Blend_GettraceDRAWSECT();
extern Standard_Boolean Blend_GetcontextNOTESTDEFL();

// Pour debug : visualisation de la section
static void Drawsect(const Handle(Adaptor3d_Surface)& surf1,
                     const Handle(Adaptor3d_Surface)& surf2,
                     const math_Vector&               sol,
                     const Standard_Real              param,
                     Blend_Function&                  Func,
                     const Blend_Status               State)
{
  //  if(!sectioncalculee) return;
  Blend_Point      BP(Adaptor3d_HSurfaceTool::Value(surf1, sol(1), sol(2)),
                 Adaptor3d_HSurfaceTool::Value(surf2, sol(3), sol(4)),
                 param,
                 sol(1),
                 sol(2),
                 sol(3),
                 sol(4));
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

  // POP pour NT
  // char name[100];
  char* name = new char[100];
  if ((State == Blend_StepTooLarge) || (State == Blend_SamePoints))
  {
    IndexOfRejection++;
    sprintf(name, "%s_%d", "Rejection", IndexOfRejection);
  }
  else
  {
    IndexOfSection++;
    sprintf(name, "%s_%d", "Section", IndexOfSection);
  }
  #ifdef DRAW
  Handle(DrawTrSurf_BSplineCurve) BS = new (DrawTrSurf_BSplineCurve)(sect);
  BS->ClearPoles();
  BS->ClearKnots();
  if (State == Blend_StepTooLarge)
    BS->SetColor(Draw_violet);
  if (State == Blend_SamePoints)
    BS->SetColor(Draw_rose);
  Draw::Set(name, BS);
  #endif
}

static void Drawsect(const Handle(Adaptor3d_Surface)& surf1,
                     const Handle(Adaptor3d_Surface)& surf2,
                     const math_Vector&               sol,
                     const Standard_Real              param,
                     Blend_Function&                  Func)
{
  Drawsect(surf1, surf2, sol, param, Func, Blend_OK);
}
#endif

BRepBlend_Walking::BRepBlend_Walking(const Handle(Adaptor3d_Surface)&   Surf1,
                                     const Handle(Adaptor3d_Surface)&   Surf2,
                                     const Handle(Adaptor3d_TopolTool)& Domain1,
                                     const Handle(Adaptor3d_TopolTool)& Domain2,
                                     const Handle(ChFiDS_ElSpine)&      HGuide)
    : sol(1, 4),
      surf1(Surf1),
      surf2(Surf2),
      ToCorrectOnRst1(Standard_False),
      ToCorrectOnRst2(Standard_False),
      done(Standard_False),
      clasonS1(Standard_True),
      clasonS2(Standard_True),
      check2d(Standard_True),
      check(Standard_True),
      twistflag1(Standard_False),
      twistflag2(Standard_False)

{
  domain1    = Domain1;
  domain2    = Domain2;
  recdomain1 = Domain1;
  recdomain2 = Domain2;
  hguide     = HGuide;
}

void BRepBlend_Walking::SetDomainsToRecadre(const Handle(Adaptor3d_TopolTool)& Domain1,
                                            const Handle(Adaptor3d_TopolTool)& Domain2)
{
  recdomain1 = Domain1;
  recdomain2 = Domain2;
}

void BRepBlend_Walking::AddSingularPoint(const Blend_Point& P)
{
  if (jalons.Length() == 0)
  {
    jalons.Append(P);
  }
  else
  {
    Standard_Integer ii, jj;
    Standard_Real    tp = P.Parameter(), ti = jalons.First().Parameter();
    for (jj = 1, ii = 1; ii <= jalons.Length() && tp > ti; ii++)
    {
      jj = ii;
      ti = jalons.Value(jj).Parameter();
    }
    if (tp > ti)
      jalons.InsertAfter(jj, P);
    else
      jalons.InsertBefore(jj, P);
  }
}

void BRepBlend_Walking::Perform(Blend_Function&        Func,
                                Blend_FuncInv&         FuncInv,
                                const Standard_Real    Pdep,
                                const Standard_Real    Pmax,
                                const Standard_Real    MaxStep,
                                const Standard_Real    Tol3d,
                                const Standard_Real    TolGuide,
                                const math_Vector&     ParDep,
                                const Standard_Real    Fleche,
                                const Standard_Boolean Appro)
{
  done                           = Standard_False;
  iscomplete                     = Standard_False;
  comptra                        = Standard_False;
  Standard_Boolean doextremities = 1;
  if (line.IsNull())
    line = new BRepBlend_Line();
  else
  {
    line->Clear();
    doextremities = 0;
  }
  tolpoint3d = Tol3d;
  tolgui     = Abs(TolGuide);
  fleche     = Abs(Fleche);
  rebrou     = Standard_False;
  pasmax     = Abs(MaxStep);
  if (Pmax - Pdep >= 0.)
  {
    sens = 1.;
  }
  else
  {
    sens = -1.;
  }

  Blend_Status State;

  param = Pdep;
  Func.Set(param);

  if (Appro)
  {
    TopAbs_State situ1, situ2;
    math_Vector  tolerance(1, 4), infbound(1, 4), supbound(1, 4);
    Func.GetTolerance(tolerance, tolpoint3d);
    Func.GetBounds(infbound, supbound);
    math_FunctionSetRoot rsnld(Func, tolerance, 30);

    rsnld.Perform(Func, ParDep, infbound, supbound);

    if (!rsnld.IsDone())
    {
      return;
    }
    rsnld.Root(sol);

    if (clasonS1)
      situ1 = domain1->Classify(gp_Pnt2d(sol(1), sol(2)), Min(tolerance(1), tolerance(2)), 0);
    else
      situ1 = TopAbs_IN;
    if (clasonS2)
      situ2 = domain2->Classify(gp_Pnt2d(sol(3), sol(4)), Min(tolerance(3), tolerance(4)), 0);
    else
      situ2 = TopAbs_IN;

    if (situ1 != TopAbs_IN || situ2 != TopAbs_IN)
    {
      return;
    }
  }
  else
  {
    sol = ParDep;
  }

#ifdef OCCT_DEBUG
  sectioncalculee = 0;
#endif
  State = TestArret(Func, Blend_OK, Standard_False);
  if (State != Blend_OK)
  {
    return;
  }
#ifdef OCCT_DEBUG
  if (Blend_GettraceDRAWSECT())
  {
    Drawsect(surf1, surf2, sol, param, Func);
  }
  nbcomputedsection = 1;
#endif
  // Mettre a jour la ligne.
  // Correct first parameter if needed
  if (ToCorrectOnRst1 || ToCorrectOnRst2)
    previousP.SetParameter(CorrectedParam);
  line->Append(previousP);

  if (doextremities)
  {
    BRepBlend_Extremity ptf1(previousP.PointOnS1(), sol(1), sol(2), tolpoint3d);
    BRepBlend_Extremity ptf2(previousP.PointOnS2(), sol(3), sol(4), tolpoint3d);
    if (!previousP.IsTangencyPoint())
    {
      ptf1.SetTangent(previousP.TangentOnS1());
      ptf2.SetTangent(previousP.TangentOnS2());
    }

    if (sens > 0.)
    {
      line->SetStartPoints(ptf1, ptf2);
    }
    else
    {
      line->SetEndPoints(ptf1, ptf2);
    }
  }

  InternalPerform(Func, FuncInv, Pmax);

  done = Standard_True;
}

Standard_Boolean BRepBlend_Walking::PerformFirstSection(Blend_Function&     Func,
                                                        const Standard_Real Pdep,
                                                        math_Vector&        ParDep,
                                                        const Standard_Real Tol3d,
                                                        const Standard_Real TolGuide,
                                                        TopAbs_State&       Pos1,
                                                        TopAbs_State&       Pos2)
{
  iscomplete = Standard_False;
  comptra    = Standard_False;
  line       = new BRepBlend_Line();
  tolpoint3d = Tol3d;
  tolgui     = Abs(TolGuide);

  Pos1 = Pos2 = TopAbs_UNKNOWN;

  param = Pdep;
  Func.Set(param);

  math_Vector tolerance(1, 4), infbound(1, 4), supbound(1, 4);
  Func.GetTolerance(tolerance, tolpoint3d);
  Func.GetBounds(infbound, supbound);
  math_FunctionSetRoot rsnld(Func, tolerance, 30);

  rsnld.Perform(Func, ParDep, infbound, supbound);

  if (!rsnld.IsDone())
  {
    return Standard_False;
  }
  rsnld.Root(sol);
  ParDep = sol;
  Pos1   = domain1->Classify(gp_Pnt2d(sol(1), sol(2)), Min(tolerance(1), tolerance(2)), 0);
  Pos2   = domain2->Classify(gp_Pnt2d(sol(3), sol(4)), Min(tolerance(3), tolerance(4)), 0);
  if (Pos1 != TopAbs_IN || Pos2 != TopAbs_IN)
  {
    return Standard_False;
  }

  TestArret(Func, Blend_OK, Standard_False);
#ifdef OCCT_DEBUG
  if (Blend_GettraceDRAWSECT())
  {
    Drawsect(surf1, surf2, sol, param, Func);
  }
#endif
  return Standard_True;
}

Standard_Boolean BRepBlend_Walking::PerformFirstSection(Blend_Function&        Func,
                                                        Blend_FuncInv&         FuncInv,
                                                        const Standard_Real    Pdep,
                                                        const Standard_Real    Pmax,
                                                        const math_Vector&     ParDep,
                                                        const Standard_Real    Tol3d,
                                                        const Standard_Real    TolGuide,
                                                        const Standard_Boolean RecOnS1,
                                                        const Standard_Boolean RecOnS2,
                                                        Standard_Real&         Psol,
                                                        math_Vector&           ParSol)

{
  iscomplete = Standard_False;
  comptra    = Standard_False;
  line       = new BRepBlend_Line();

  Standard_Real    w1, w2, extrapol;
  Standard_Boolean recad1, recad2;

  tolpoint3d = Tol3d;
  tolgui     = Abs(TolGuide);
  if (Pmax - Pdep >= 0.0)
  {
    sens = 1.;
  }
  else
  {
    sens = -1.;
  }
  extrapol = Abs(Pmax - Pdep) / 50.0; // 2%

  Blend_Status State;

  param = Pdep;
  Func.Set(param);

  math_Vector               tolerance(1, 4), infbound(1, 4), supbound(1, 4);
  math_Vector               solrst1(1, 4), solrst2(1, 4);
  BRepBlend_Extremity       Ext1, Ext2;
  Standard_Integer          Index1 = 0, Index2 = 0, nbarc;
  Standard_Boolean          Isvtx1 = Standard_False, Isvtx2 = Standard_False;
  Handle(Adaptor3d_HVertex) Vtx1, Vtx2;
  gp_Pnt2d                  p2d;
  Standard_Real             CorrectedU = 0., CorrectedV = 0.;
  gp_Pnt                    CorrectedPnt;

  Func.GetTolerance(tolerance, tolpoint3d);
  Func.GetBounds(infbound, supbound);
  math_FunctionSetRoot rsnld(Func, tolerance, 30);

  rsnld.Perform(Func, ParDep, infbound, supbound);

  if (!rsnld.IsDone())
  {
    return Standard_False;
  }
  rsnld.Root(sol);

  w1 = w2 = Pmax;

  recad1 = RecOnS1 && Recadre(FuncInv, Standard_True, sol, solrst1, Index1, Isvtx1, Vtx1, extrapol);
  if (recad1)
  {
    w1 = solrst1(2);
  }

  recad2 =
    RecOnS2 && Recadre(FuncInv, Standard_False, sol, solrst2, Index2, Isvtx2, Vtx2, extrapol);
  if (recad2)
  {
    w2 = solrst2(2);
  }

  if (!recad1 && !recad2)
  {
    return Standard_False;
  }

  if (recad1 && recad2)
  {
    if (Abs(w1 - w2) <= tolgui)
    {
      // sol sur 1 et 2 a la fois
      State     = Blend_OnRst12;
      param     = w1;
      ParSol(1) = solrst2(3);
      ParSol(2) = solrst2(4);
      ParSol(3) = solrst1(3);
      ParSol(4) = solrst1(4);
    }
    else if (sens * (w2 - w1) < 0.0)
    { // on garde le plus grand
      // sol sur 1
      State = Blend_OnRst1;
      param = w1;

      recdomain1->Init();
      nbarc = 1;
      while (nbarc < Index1)
      {
        nbarc++;
        recdomain1->Next();
      }
      p2d       = BRepBlend_HCurve2dTool::Value(recdomain1->Value(), solrst1(1));
      ParSol(1) = p2d.X();
      ParSol(2) = p2d.Y();
      ParSol(3) = solrst1(3);
      ParSol(4) = solrst1(4);
    }
    else
    {
      // sol sur 2
      State = Blend_OnRst2;
      param = w2;

      recdomain2->Init();
      nbarc = 1;
      while (nbarc < Index2)
      {
        nbarc++;
        recdomain2->Next();
      }
      p2d       = BRepBlend_HCurve2dTool::Value(recdomain2->Value(), solrst2(1));
      ParSol(1) = solrst2(3);
      ParSol(2) = solrst2(4);
      ParSol(3) = p2d.X();
      ParSol(4) = p2d.Y();
    }
  }
  else if (recad1)
  {
    // sol sur 1
    State = Blend_OnRst1;
    param = w1;
    recdomain1->Init();
    nbarc = 1;
    while (nbarc < Index1)
    {
      nbarc++;
      recdomain1->Next();
    }
    p2d                = BRepBlend_HCurve2dTool::Value(recdomain1->Value(), solrst1(1));
    ParSol(1)          = p2d.X();
    ParSol(2)          = p2d.Y();
    ParSol(3)          = solrst1(3);
    ParSol(4)          = solrst1(4);
    gp_Pnt thePntOnRst = Adaptor3d_HSurfaceTool::Value(surf1, ParSol(1), ParSol(2));
    if (CorrectExtremityOnOneRst(1,
                                 ParSol(3),
                                 ParSol(4),
                                 param,
                                 thePntOnRst,
                                 CorrectedU,
                                 CorrectedV,
                                 CorrectedPnt,
                                 CorrectedParam))
      ToCorrectOnRst1 = Standard_True;
  }
  else
  { // if (recad2) {
    // sol sur 2
    State = Blend_OnRst2;
    param = w2;

    recdomain2->Init();
    nbarc = 1;
    while (nbarc < Index2)
    {
      nbarc++;
      recdomain2->Next();
    }
    p2d                = BRepBlend_HCurve2dTool::Value(recdomain2->Value(), solrst2(1));
    ParSol(1)          = solrst2(3);
    ParSol(2)          = solrst2(4);
    ParSol(3)          = p2d.X();
    ParSol(4)          = p2d.Y();
    gp_Pnt thePntOnRst = Adaptor3d_HSurfaceTool::Value(surf2, ParSol(3), ParSol(4));
    if (CorrectExtremityOnOneRst(2,
                                 ParSol(1),
                                 ParSol(2),
                                 param,
                                 thePntOnRst,
                                 CorrectedU,
                                 CorrectedV,
                                 CorrectedPnt,
                                 CorrectedParam))
      ToCorrectOnRst2 = Standard_True;
  }

  Psol = param;
  sol  = ParSol;
  Func.Set(param);
  State = TestArret(Func, State, Standard_False);
  switch (State)
  {
    case Blend_OnRst1: {
#ifdef OCCT_DEBUG
      if (Blend_GettraceDRAWSECT())
      {
        Drawsect(surf1, surf2, sol, param, Func);
      }
#endif
      MakeExtremity(Ext1, Standard_True, Index1, solrst1(1), Isvtx1, Vtx1);
      if (ToCorrectOnRst1)
        Ext2.SetValue(CorrectedPnt, CorrectedU, CorrectedV, tolpoint3d);
      else
        Ext2.SetValue(previousP.PointOnS2(), sol(3), sol(4), tolpoint3d);
    }
    break;

    case Blend_OnRst2: {
#ifdef OCCT_DEBUG
      if (Blend_GettraceDRAWSECT())
      {
        Drawsect(surf1, surf2, sol, param, Func);
      }
#endif
      if (ToCorrectOnRst2)
        Ext1.SetValue(CorrectedPnt, CorrectedU, CorrectedV, tolpoint3d);
      else
        Ext1.SetValue(previousP.PointOnS1(), sol(1), sol(2), tolpoint3d);
      MakeExtremity(Ext2, Standard_False, Index2, solrst2(1), Isvtx2, Vtx2);
    }
    break;

    case Blend_OnRst12: {
#ifdef OCCT_DEBUG
      if (Blend_GettraceDRAWSECT())
      {
        Drawsect(surf1, surf2, sol, param, Func);
      }
#endif
      MakeExtremity(Ext1, Standard_True, Index1, solrst1(1), Isvtx1, Vtx1);
      MakeExtremity(Ext2, Standard_False, Index2, solrst2(1), Isvtx2, Vtx2);
    }
    break;
    default: {
      throw Standard_Failure("BRepBlend_Walking::PerformFirstSection : echec");
    }
  }
  if (sens < 0.0)
  {
    line->SetEndPoints(Ext1, Ext2);
  }
  else
  {
    line->SetStartPoints(Ext1, Ext2);
  }
  return Standard_True;
}

Standard_Boolean BRepBlend_Walking::Continu(Blend_Function&     Func,
                                            Blend_FuncInv&      FuncInv,
                                            const Standard_Real P)
{
  if (!done)
  {
    throw StdFail_NotDone();
  }
  const Blend_Point& firstBP = line->Point(1);
  const Blend_Point& lastBP  = line->Point(line->NbPoints());

  if (P < firstBP.Parameter())
  {
    sens      = -1.;
    previousP = firstBP;
  }
  else if (P > lastBP.Parameter())
  {
    sens      = 1.;
    previousP = lastBP;
  }

  param = previousP.Parameter();
  previousP.ParametersOnS1(sol(1), sol(2));
  previousP.ParametersOnS2(sol(3), sol(4));

  InternalPerform(Func, FuncInv, P);
  return Standard_True;
}

Standard_Boolean BRepBlend_Walking::Continu(Blend_Function&        Func,
                                            Blend_FuncInv&         FuncInv,
                                            const Standard_Real    P,
                                            const Standard_Boolean OnS1)
{
  if (!done)
  {
    throw StdFail_NotDone();
  }
  BRepBlend_Extremity Ext1, Ext2;
  if (sens < 0.)
  {
    Ext1 = line->StartPointOnFirst();
    Ext2 = line->StartPointOnSecond();
    if ((OnS1 && Ext1.NbPointOnRst() == 0) || (!OnS1 && Ext2.NbPointOnRst() == 0))
    {
      return Standard_False;
    }
    previousP = line->Point(1);
  }
  else
  {
    Ext1 = line->EndPointOnFirst();
    Ext2 = line->EndPointOnSecond();
    if ((OnS1 && Ext1.NbPointOnRst() == 0) || (!OnS1 && Ext2.NbPointOnRst() == 0))
    {
      return Standard_False;
    }
    previousP = line->Point(line->NbPoints());
  }

  Standard_Integer length = line->NbPoints();
  param                   = previousP.Parameter();
  previousP.ParametersOnS1(sol(1), sol(2));
  previousP.ParametersOnS2(sol(3), sol(4));

  if (OnS1)
    clasonS1 = Standard_False;
  else
    clasonS2 = Standard_False;

  InternalPerform(Func, FuncInv, P);

  clasonS1 = Standard_True;
  clasonS2 = Standard_True;

  Standard_Integer newlength = line->NbPoints();
  if (sens < 0.)
  {
    if ((OnS1 && line->StartPointOnSecond().NbPointOnRst() == 0)
        || (!OnS1 && line->StartPointOnFirst().NbPointOnRst() == 0))
    {
      line->Remove(1, newlength - length);
      line->SetStartPoints(Ext1, Ext2);
      return Standard_False;
    }
  }
  else
  {
    if ((OnS1 && line->EndPointOnSecond().NbPointOnRst() == 0)
        || (!OnS1 && line->EndPointOnFirst().NbPointOnRst() == 0))
    {
      line->Remove(length, newlength);
      line->SetEndPoints(Ext1, Ext2);
      return Standard_False;
    }
  }
  return Standard_True;
}

Standard_Boolean BRepBlend_Walking::Complete(Blend_Function&     Func,
                                             Blend_FuncInv&      FuncInv,
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

  if (sens > 0.)
  {
    previousP = line->Point(1);
  }
  else
  {
    previousP = line->Point(line->NbPoints());
  }

  sens = -sens;

  param = previousP.Parameter();
  previousP.ParametersOnS1(sol(1), sol(2));
  previousP.ParametersOnS2(sol(3), sol(4));

  InternalPerform(Func, FuncInv, Pmin);

  iscomplete = Standard_True;
  return Standard_True;
}

void BRepBlend_Walking::ClassificationOnS1(const Standard_Boolean C)
{
  clasonS1 = C;
}

void BRepBlend_Walking::ClassificationOnS2(const Standard_Boolean C)
{
  clasonS2 = C;
}

void BRepBlend_Walking::Check2d(const Standard_Boolean C)
{
  check2d = C;
}

void BRepBlend_Walking::Check(const Standard_Boolean C)
{
  check = C;
}

Blend_Status BRepBlend_Walking::TestArret(Blend_Function&        Function,
                                          const Blend_Status     State,
                                          const Standard_Boolean TestDefl,
                                          const Standard_Boolean TestSolu,
                                          const Standard_Boolean TestLengthStep)

// On regarde si le point donne est solution.
// Si c est le cas,
//  On verifie le critere de fleche sur surf1 et surf2
//   Si OK, on classifie les points sur surf1 et sur surf2.
//    Si les deux sont dedans : on retourne Blend_OK
//    sinon si un seul est dedans
//     on resout le pb inverse sur la restriction concernee
//    sinon on resout le pb inverse sur la surface pour laquelle
//     le point est le plus loin.
//   sinon (fleche non OK)
//    on renvoie Blend_StepTooLarge.
// sinon on renvoie Blend_StepTooLarge.
//

{
  gp_Pnt            pt1, pt2;
  gp_Vec            V1, V2;
  gp_Vec            Tgp1, Tgp2, Nor1, Nor2;
  gp_Vec2d          V12d, V22d;
  Blend_Status      State1, State2;
  IntSurf_TypeTrans tras1, tras2;
  Blend_Point       curpoint;
  Standard_Boolean  loctwist1 = Standard_False, loctwist2 = Standard_False;
  Standard_Real     tolsolu = tolpoint3d;

  if (!TestSolu)
    tolsolu *= 1000; // Ca doit toujours etre bon
  if (Function.IsSolution(sol, tolsolu))
  {
#ifdef OCCT_DEBUG
    sectioncalculee = 1;
#endif
    Standard_Boolean curpointistangent = Function.IsTangencyPoint();
    pt1                                = Function.PointOnS1();
    pt2                                = Function.PointOnS2();
    if (curpointistangent)
    {
      curpoint.SetValue(pt1, pt2, param, sol(1), sol(2), sol(3), sol(4));
    }
    else
    {
      V1   = Function.TangentOnS1();
      V2   = Function.TangentOnS2();
      V12d = Function.Tangent2dOnS1();
      V22d = Function.Tangent2dOnS2();
      curpoint.SetValue(pt1, pt2, param, sol(1), sol(2), sol(3), sol(4), V1, V2, V12d, V22d);
      if (Function.TwistOnS1())
        loctwist1 = Standard_True;
      if (Function.TwistOnS2())
        loctwist2 = Standard_True;
    }

    if (TestDefl && check)
    {
      // Verification du critere de fleche sur chaque surface
      // et sur la ligne guide

      State1 = CheckDeflection(Standard_True, curpoint);
      State2 = CheckDeflection(Standard_False, curpoint);
    }
    else
    {
      State1 = Blend_OK;
      State2 = Blend_OK;
      if (TestLengthStep)
      {
        // On verifie juste que le pas n'est pas trop grand
        // (Cas des prolongements foireux)
        Standard_Real curparamu, curparamv, prevparamu, prevparamv;
        math_Vector   inf(1, 4), sup(1, 4);
        Function.GetBounds(inf, sup);
        sup -= inf;
        sup *= 0.05; // Pas max : 5% du domaine

        curpoint.ParametersOnS1(curparamu, curparamv);
        previousP.ParametersOnS1(prevparamu, prevparamv);
        if (Abs(curparamu - prevparamu) > sup(1))
          State1 = Blend_StepTooLarge;
        if (Abs(curparamv - prevparamv) > sup(2))
          State1 = Blend_StepTooLarge;
        curpoint.ParametersOnS2(curparamu, curparamv);
        previousP.ParametersOnS2(prevparamu, prevparamv);
        if (Abs(curparamu - prevparamu) > sup(3))
          State2 = Blend_StepTooLarge;
        if (Abs(curparamv - prevparamv) > sup(4))
          State2 = Blend_StepTooLarge;
      }
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

    // Ici seulement on peut statuer sur le twist
    // Car les rejet ont ete effectue (BUC60322)
    if (loctwist1)
      twistflag1 = Standard_True;
    if (loctwist2)
      twistflag2 = Standard_True;

    if (!comptra && !curpointistangent)
    {
      Function.Tangent(sol(1), sol(2), sol(3), sol(4), Tgp1, Tgp2, Nor1, Nor2);
      Nor1.Normalize();
      Nor2.Normalize();
      Standard_Real testra = Tgp1.Dot(Nor1.Crossed(V1));
      if (Abs(testra) > Precision::Confusion())
      {
        tras1 = IntSurf_In;
        if ((testra > 0. && !loctwist1) || (testra < 0. && loctwist1))
        {
          tras1 = IntSurf_Out;
        }

        testra = Tgp2.Dot(Nor2.Crossed(V2));
        if (Abs(testra) > Precision::Confusion())
        {
          tras2 = IntSurf_Out;
          if ((testra > 0. && !loctwist2) || (testra < 0. && loctwist2))
          {
            tras2 = IntSurf_In;
          }
          comptra = Standard_True;
          line->Set(tras1, tras2);
        }
      }
    }

    if (State1 == Blend_OK || State2 == Blend_OK)
    {
      previousP = curpoint;
      return State;
    }

    if (State1 == Blend_StepTooSmall && State2 == Blend_StepTooSmall)
    {
      previousP = curpoint;
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

Blend_Status BRepBlend_Walking::CheckDeflection(const Standard_Boolean OnFirst,
                                                const Blend_Point&     CurPoint)
{
  // regle par tests dans U4 correspond a 11.478 d
  const Standard_Real CosRef3D = 0.98;

  const Standard_Real CosRef2D = 0.88; // correspond a 25 d

  Standard_Real Norme, Cosi, Cosi2;
  Standard_Real prevNorme = 0.;
  Standard_Real FlecheCourante;
  Standard_Real Du, Dv, Duv;
  Standard_Real tolu, tolv;

  gp_Pnt           Psurf;
  gp_Vec           Tgsurf;
  gp_Vec2d         Tgonsurf;
  Standard_Real    curparamu, curparamv;
  Standard_Boolean curpointistangent = CurPoint.IsTangencyPoint();

  gp_Pnt           prevP;
  gp_Vec           prevTg;
  gp_Vec2d         previousd2d;
  Standard_Real    prevparamu, prevparamv;
  Standard_Boolean prevpointistangent = previousP.IsTangencyPoint();

  if (OnFirst)
  {
    Psurf = CurPoint.PointOnS1();
    if (!curpointistangent)
    {
      Tgsurf = CurPoint.TangentOnS1();
    }
    prevP = previousP.PointOnS1();
    if (!prevpointistangent)
    {
      prevTg = previousP.TangentOnS1();
    }
    tolu = Adaptor3d_HSurfaceTool::UResolution(surf1, tolpoint3d);
    tolv = Adaptor3d_HSurfaceTool::VResolution(surf1, tolpoint3d);
  }
  else
  {
    Psurf = CurPoint.PointOnS2();
    if (!curpointistangent)
    {
      Tgsurf = CurPoint.TangentOnS2();
    }
    prevP = previousP.PointOnS2();
    if (!prevpointistangent)
    {
      prevTg = previousP.TangentOnS2();
    }
    tolu = Adaptor3d_HSurfaceTool::UResolution(surf2, tolpoint3d);
    tolv = Adaptor3d_HSurfaceTool::VResolution(surf2, tolpoint3d);
  }

  gp_Vec Corde(prevP, Psurf);
  Norme = Corde.SquareMagnitude();
  //  if(!curpointistangent) curNorme = Tgsurf.SquareMagnitude();
  if (!prevpointistangent)
    prevNorme = prevTg.SquareMagnitude();

  const Standard_Real toler3d = 0.01 * tolpoint3d;
  if (Norme <= toler3d * toler3d)
  {
    // il faudra peut etre  forcer meme point
    return Blend_SamePoints;
  }
  if (!prevpointistangent)
  {
    if (prevNorme <= toler3d * toler3d)
    {
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
  }

  if (!curpointistangent)
  {
    // Voir s il faut faire le controle sur le signe de prevtg*Tgsurf
    Cosi  = sens * Corde * Tgsurf;
    Cosi2 = Cosi * Cosi / Tgsurf.SquareMagnitude() / Norme;
    if (Cosi2 < CosRef3D || Cosi < 0.)
    {
      return Blend_StepTooLarge;
    }
  }

  if (check2d)
  {
    if (OnFirst)
    {
      CurPoint.ParametersOnS1(curparamu, curparamv);
      if (!curpointistangent)
        Tgonsurf = CurPoint.Tangent2dOnS1();
      previousP.ParametersOnS1(prevparamu, prevparamv);
      if (!prevpointistangent)
        previousd2d = previousP.Tangent2dOnS1();
    }
    else
    {
      CurPoint.ParametersOnS2(curparamu, curparamv);
      if (!curpointistangent)
        Tgonsurf = CurPoint.Tangent2dOnS2();
      previousP.ParametersOnS2(prevparamu, prevparamv);
      if (!prevpointistangent)
        previousd2d = previousP.Tangent2dOnS2();
    }

    Du  = curparamu - prevparamu;
    Dv  = curparamv - prevparamv;
    Duv = Du * Du + Dv * Dv;
    //    SqrtDuv = Sqrt(Duv);
    if (Abs(Du) < tolu && Abs(Dv) < tolv)
    {
      // il faudra peut etre  forcer meme point
      return Blend_SamePoints; // point confondu 2d
    }
    if (!prevpointistangent)
    {
      if (Abs(previousd2d.X()) < tolu && Abs(previousd2d.Y()) < tolv)
      {
        // il faudra peut etre  forcer meme point
        return Blend_SamePoints; // point confondu 2d
      }
      Cosi = sens * (Du * previousd2d.X() + Dv * previousd2d.Y());
      if (Cosi < 0)
      {
        return Blend_Backward;
      }
    }
    if (!curpointistangent)
    {
      // Voir s il faut faire le controle sur le signe de Cosi
      Cosi  = sens * (Du * Tgonsurf.X() + Dv * Tgonsurf.Y()) / Tgonsurf.Magnitude();
      Cosi2 = Cosi * Cosi / Duv;
      if (Cosi2 < CosRef2D || Cosi < 0.)
      {
        return Blend_StepTooLarge;
      }
    }
  }
  if (!curpointistangent && !prevpointistangent)
  {
    // Estimation de la fleche courante
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
  }
  return Blend_OK;
}

Standard_Integer BRepBlend_Walking::ArcToRecadre(const Standard_Boolean OnFirst,
                                                 const math_Vector&     theSol,
                                                 const Standard_Integer PrevIndex,
                                                 gp_Pnt2d&              lastpt2d,
                                                 gp_Pnt2d&              pt2d,
                                                 Standard_Real&         ponarc)
{
  Standard_Integer            IndexSol = 0, nbarc = 0;
  Standard_Boolean            ok      = Standard_False;
  Standard_Boolean            byinter = (line->NbPoints() != 0), okinter = 0;
  Standard_Real               distmin = RealLast();
  Standard_Real               uprev = 0., vprev = 0., prm = 0., dist = 0.;
  Handle(Adaptor3d_TopolTool) Iter;

  if (OnFirst)
  {
    if (byinter)
      previousP.ParametersOnS1(uprev, vprev);
    pt2d.SetCoord(theSol(1), theSol(2));
    Iter = recdomain1;
  }
  else
  {
    if (byinter)
      previousP.ParametersOnS2(uprev, vprev);
    pt2d.SetCoord(theSol(3), theSol(4));
    Iter = recdomain2;
  }
  lastpt2d.SetCoord(uprev, vprev);
  Iter->Init();
  while (Iter->More())
  {
    nbarc++;
    ok = 0;
    if (OnFirst)
    {
      if (byinter)
      {
        ok = okinter = BRepBlend_BlendTool::Inters(pt2d, lastpt2d, surf1, Iter->Value(), prm, dist);
      }
      if (!ok)
        ok = BRepBlend_BlendTool::Project(pt2d, surf1, Iter->Value(), prm, dist);
    }
    else
    {
      if (byinter)
      {
        ok = okinter = BRepBlend_BlendTool::Inters(pt2d, lastpt2d, surf2, Iter->Value(), prm, dist);
      }
      if (!ok)
        ok = BRepBlend_BlendTool::Project(pt2d, surf2, Iter->Value(), prm, dist);
    }
    if (ok && (nbarc != PrevIndex))
    {
      if (dist < distmin || okinter)
      {
        distmin  = dist;
        ponarc   = prm;
        IndexSol = nbarc;
        if (okinter && (PrevIndex == 0))
          break;
      }
    }
    Iter->Next();
  }
  return IndexSol;
}

Standard_Boolean BRepBlend_Walking::Recadre(Blend_FuncInv&             FuncInv,
                                            const Standard_Boolean     OnFirst,
                                            const math_Vector&         theSol,
                                            math_Vector&               solrst,
                                            Standard_Integer&          Indexsol,
                                            Standard_Boolean&          IsVtx,
                                            Handle(Adaptor3d_HVertex)& Vtx,
                                            const Standard_Real        Extrap)

{
  Standard_Boolean jalons_Trouve = Standard_False;
  Standard_Boolean recadre       = Standard_True, ok;
  Standard_Boolean byinter       = (line->NbPoints() != 0);
  Standard_Integer LeJalon       = 0;

  Standard_Integer nbarc;
  Standard_Real    dist, prm, pmin, vtol;
  gp_Pnt2d         pt2d, lastpt2d;

  math_Vector toler(1, 4), infb(1, 4), supb(1, 4), valsol(1, 4);

  Handle(Adaptor2d_Curve2d)   thecur;
  Handle(Adaptor3d_TopolTool) Iter;

  if (OnFirst)
    Iter = recdomain1;
  else
    Iter = recdomain2;

  Indexsol = ArcToRecadre(OnFirst, theSol, 0, lastpt2d, pt2d, pmin);
  IsVtx    = Standard_False;
  if (Indexsol == 0)
  {
    return Standard_False;
  }

  Iter->Init();
  nbarc = 1;
  while (nbarc < Indexsol)
  {
    nbarc++;
    Iter->Next();
  }

  Handle(Adaptor2d_Curve2d) thearc = Iter->Value();

  if (OnFirst)
  {
    thecur = BRepBlend_BlendTool::CurveOnSurf(thearc, surf1);
  }
  else
  {
    thecur = BRepBlend_BlendTool::CurveOnSurf(thearc, surf2);
  }

  // Le probleme a resoudre
  FuncInv.Set(OnFirst, thecur);
  FuncInv.GetBounds(infb, supb);
  infb(2) -= Extrap;
  supb(2) += Extrap;

  FuncInv.GetTolerance(toler, 0.1 * tolpoint3d); // Il vaut mieux garder un peu de marge
  math_FunctionSetRoot rsnld(FuncInv, toler, 35);
  toler *= 10; // Mais on fait les tests correctements

  // Calcul d'un point d'init
  Standard_Real ufirst, ulast;
  BRepBlend_BlendTool::Bounds(thecur, ufirst, ulast);
  // Pour aider a trouver les coins singuliers on recadre eventuelement le paramtere
  if (Abs(pmin - ufirst) < Abs(ulast - ufirst) / 1000)
  {
    pmin = ufirst;
  }
  if (Abs(pmin - ulast) < Abs(ulast - ufirst) / 1000)
  {
    pmin = ulast;
  }

  if (byinter)
  {
    Standard_Real lastParam = previousP.Parameter();
    // Verifie que le recadrage n'est pas un jalons
    if (jalons.Length() != 0)
    {
      Standard_Real    t1, t2, t;
      Standard_Boolean Cherche = Standard_True;
      Standard_Integer ii;
      if (lastParam < param)
      {
        t1 = lastParam;
        t2 = param;
      }
      else
      {
        t1 = param;
        t2 = lastParam;
      }
      for (ii = 1; ii <= jalons.Length() && Cherche; ii++)
      {
        t = jalons.Value(ii).Parameter();
        if (((t1 < t) && (t2 > t)) || (t == param))
        {
          jalons_Trouve = Standard_True;
          LeJalon       = ii;
          Cherche       = Standard_False; // Ne marche que si l'on sort simultanement
        }
        else
          Cherche = t < t2; // On s'arrete si t>=t2;
      }
    }
    if (!jalons_Trouve)
    {
      // Initialisation par Interpolation
      Standard_Real lambda, u, v;
      gp_Pnt2d      Pnt, Pnt1, Pnt2; //,  POnC;
      thecur->D0(pmin, Pnt);
      if (OnFirst)
      {
        previousP.ParametersOnS2(u, v);
        Pnt1.SetCoord(u, v);
        Pnt2.SetCoord(theSol(3), theSol(4));
      }
      else
      {
        previousP.ParametersOnS1(u, v);
        Pnt1.SetCoord(u, v);
        Pnt2.SetCoord(theSol(1), theSol(2));
      }

      lambda = Pnt.Distance(lastpt2d);
      if (lambda > 1.e-12)
        lambda /= Pnt.Distance(lastpt2d) + Pnt.Distance(pt2d);
      else
        lambda = 0;
      solrst(1) = pmin;
      solrst(2) = (1 - lambda) * lastParam + lambda * param;
      solrst(3) = (1 - lambda) * Pnt1.X() + lambda * Pnt2.X();
      solrst(4) = (1 - lambda) * Pnt1.Y() + lambda * Pnt2.Y();
    }
  }
  else
  { // sinon on initialise par le dernier point calcule
    solrst(1) = pmin;
    solrst(2) = param;
    if (OnFirst)
    {
      solrst(3) = theSol(3);
      solrst(4) = theSol(4);
    }
    else
    {
      solrst(3) = theSol(1);
      solrst(4) = theSol(2);
    }
  }

  if (jalons_Trouve)
  { // On recupere le jalon
    Blend_Point      MonJalon;
    Standard_Boolean periodic;
    Standard_Real    uperiod = 0, vperiod = 0;
    gp_Pnt2d         Pnt;
    Standard_Real    distaux;
    MonJalon  = jalons.Value(LeJalon);
    solrst(2) = MonJalon.Parameter();
    if (OnFirst)
    {
      MonJalon.ParametersOnS2(solrst(3), solrst(4));
      periodic = (surf2->IsUPeriodic() || surf2->IsVPeriodic());
    }
    else
    {
      MonJalon.ParametersOnS1(solrst(3), solrst(4));
      periodic = (surf1->IsUPeriodic() || surf1->IsVPeriodic());
    }

    // Recadrage eventuelle pour le cas periodique
    if (periodic)
    {
      Handle(Adaptor3d_Surface) surf;
      if (OnFirst)
        surf = surf2;
      else
        surf = surf1;

      lastpt2d = thecur->Value(pmin);

      if (surf->IsUPeriodic())
      {
        uperiod = surf->UPeriod();
        if (solrst(3) - lastpt2d.X() > uperiod * 0.6)
          solrst(3) -= uperiod;
        if (solrst(3) - lastpt2d.X() < -uperiod * 0.6)
          solrst(3) += uperiod;
      }
      if (surf->IsVPeriodic())
      {
        vperiod = surf->VPeriod();
        if (solrst(4) - lastpt2d.Y() > vperiod * 0.6)
          solrst(4) -= vperiod;
        if (solrst(4) - lastpt2d.Y() < -vperiod * 0.6)
          solrst(4) += vperiod;
      }
    }

    // Pour le parametre sur arc il faut projeter...
    pt2d.SetCoord(solrst(3), solrst(4));
    Pnt       = thecur->Value(ufirst);
    dist      = pt2d.Distance(Pnt);
    solrst(1) = ufirst;
    Pnt       = thecur->Value(ulast);
    distaux   = pt2d.Distance(Pnt);
    if (distaux < dist)
    {
      solrst(1) = ulast;
      dist      = distaux;
    }

    if (dist > Precision::PConfusion())
    {
      prm = pmin;
      if (OnFirst)
      {
        ok = BRepBlend_BlendTool::Project(pt2d, surf1, thearc, prm, distaux);
      }
      else
      {
        ok = BRepBlend_BlendTool::Project(pt2d, surf2, thearc, prm, distaux);
      }
      if (ok && (pt2d.Distance(thecur->Value(prm)) < dist))
        solrst(1) = prm;
      else
        solrst(1) = pmin;
    }
    // On verifie le jalon
    jalons_Trouve = (FuncInv.IsSolution(solrst, tolpoint3d));
  }

  if (!jalons_Trouve)
  {
    // Resolution...
    rsnld.Perform(FuncInv, solrst, infb, supb);
    if (!rsnld.IsDone())
    {
#ifdef OCCT_DEBUG
      std::cout << "Walking::Recadre : RSNLD not done " << std::endl;
#endif
      recadre = Standard_False;
    }
    else
    {
      rsnld.Root(solrst);
      recadre = FuncInv.IsSolution(solrst, tolpoint3d);
    }
  }

  // En cas d'echecs, on regarde si un autre arc
  // peut faire l'affaire (cas des sorties a proximite d'un vertex)
  dist = (ulast - ufirst) / 100;
  if ((!recadre) && ((Abs(pmin - ulast) < dist) || (Abs(pmin - ufirst) < dist)))
  {
    Indexsol = ArcToRecadre(OnFirst, theSol, Indexsol, lastpt2d, pt2d, pmin);
    if (Indexsol == 0)
    {
      return Standard_False;
    }

    Iter->Init();
    nbarc = 1;
    while (nbarc < Indexsol)
    {
      nbarc++;
      Iter->Next();
    }
    thearc = Iter->Value();

    if (OnFirst)
    {
      thecur = BRepBlend_BlendTool::CurveOnSurf(thearc, surf1);
    }
    else
    {
      thecur = BRepBlend_BlendTool::CurveOnSurf(thearc, surf2);
    }
    solrst(1) = pmin;
    // Le probleme a resoudre
    FuncInv.Set(OnFirst, thecur);
    FuncInv.GetBounds(infb, supb);
    FuncInv.GetTolerance(toler, 0.1 * tolpoint3d); // Il vaut mieux garder un peu de marge
    math_FunctionSetRoot aRsnld(FuncInv, toler, 35);
    toler *= 10; // Mais on fait les tests correctements
    // Resolution...
    aRsnld.Perform(FuncInv, solrst, infb, supb);

    if (!aRsnld.IsDone())
    {
#ifdef OCCT_DEBUG
      std::cout << "Walking::Recadre : RSNLD not done " << std::endl;
#endif
      recadre = Standard_False;
    }
    else
    {
      aRsnld.Root(solrst);
      recadre = FuncInv.IsSolution(solrst, tolpoint3d);
    }
  }

  if (recadre)
  {
    // Classification topologique
    if (OnFirst)
    {
      thecur = BRepBlend_BlendTool::CurveOnSurf(thearc, surf1);
    }
    else
    {
      thecur = BRepBlend_BlendTool::CurveOnSurf(thearc, surf2);
    }
    BRepBlend_BlendTool::Bounds(thecur, ufirst, ulast);

    Iter->Initialize(thearc);
    Iter->InitVertexIterator();
    IsVtx = !Iter->MoreVertex();
    while (!IsVtx)
    {
      Vtx  = Iter->Vertex();
      vtol = 0.4 * Abs(ulast - ufirst); // Un majorant de la tolerance
      if (vtol > Max(BRepBlend_BlendTool::Tolerance(Vtx, thearc), toler(1)))
        vtol = Max(BRepBlend_BlendTool::Tolerance(Vtx, thearc), toler(1));
      if (Abs(BRepBlend_BlendTool::Parameter(Vtx, thearc) - solrst(1)) <= vtol)
      {
        IsVtx = Standard_True; // On est dans la boule du vertex ou
                               // le vertex est dans la "boule" du recadrage
      }
      else
      {
        Iter->NextVertex();
        IsVtx = !Iter->MoreVertex();
      }
    }
    if (!Iter->MoreVertex())
    {
      IsVtx = Standard_False;
    }
    return Standard_True;
  }
  return Standard_False;
}

void BRepBlend_Walking::Transition(const Standard_Boolean           OnFirst,
                                   const Handle(Adaptor2d_Curve2d)& A,
                                   const Standard_Real              Param,
                                   IntSurf_Transition&              TLine,
                                   IntSurf_Transition&              TArc)
{
  Standard_Boolean computetranstionaveclacorde = 0;
  gp_Vec           tgline;
  Blend_Point      prevprev;

  if (previousP.IsTangencyPoint())
  {
    if (line->NbPoints() < 2)
      return;
    computetranstionaveclacorde = 1;
    if (sens < 0)
    {
      prevprev = line->Point(2);
    }
    else
    {
      prevprev = line->Point(line->NbPoints() - 1);
    }
  }
  gp_Pnt2d p2d;
  gp_Vec2d dp2d;

  gp_Pnt             pbid;
  gp_Vec             d1u, d1v, normale, tgrst;
  gp_Dir             thenormal;
  CSLib_NormalStatus stat;

  BRepBlend_HCurve2dTool::D1(A, Param, p2d, dp2d);
  if (OnFirst)
  {
    Adaptor3d_HSurfaceTool::D1(surf1, p2d.X(), p2d.Y(), pbid, d1u, d1v);
    if (!computetranstionaveclacorde)
      tgline = previousP.TangentOnS1();
    else
      tgline = gp_Vec(prevprev.PointOnS1(), previousP.PointOnS1());
  }
  else
  {
    Adaptor3d_HSurfaceTool::D1(surf2, p2d.X(), p2d.Y(), pbid, d1u, d1v);
    if (!computetranstionaveclacorde)
      tgline = previousP.TangentOnS2();
    else
      tgline = gp_Vec(prevprev.PointOnS2(), previousP.PointOnS2());
  }

  tgrst.SetLinearForm(dp2d.X(), d1u, dp2d.Y(), d1v);

  CSLib::Normal(d1u, d1v, 1.e-9, stat, thenormal);
  if (stat == CSLib_Defined)
    normale.SetXYZ(thenormal.XYZ());
  else
  {
    Handle(Adaptor3d_Surface) surf;
    if (OnFirst)
      surf = surf1;
    else
      surf = surf2;
    Standard_Integer   iu, iv;
    TColgp_Array2OfVec Der(0, 2, 0, 2);
    Adaptor3d_HSurfaceTool::D2(surf,
                               p2d.X(),
                               p2d.Y(),
                               pbid,
                               Der(1, 0),
                               Der(0, 1),
                               Der(2, 0),
                               Der(0, 2),
                               Der(1, 1));
    Der(2, 1) = Adaptor3d_HSurfaceTool::DN(surf, p2d.X(), p2d.Y(), 2, 1);
    Der(1, 2) = Adaptor3d_HSurfaceTool::DN(surf, p2d.X(), p2d.Y(), 1, 2);
    Der(2, 2) = Adaptor3d_HSurfaceTool::DN(surf, p2d.X(), p2d.Y(), 2, 2);
    CSLib::Normal(2,
                  Der,
                  1.e-9,
                  p2d.X(),
                  p2d.Y(),
                  Adaptor3d_HSurfaceTool::FirstUParameter(surf),
                  Adaptor3d_HSurfaceTool::LastUParameter(surf),
                  Adaptor3d_HSurfaceTool::FirstVParameter(surf),
                  Adaptor3d_HSurfaceTool::LastVParameter(surf),
                  stat,
                  thenormal,
                  iu,
                  iv);
    normale.SetXYZ(thenormal.XYZ());
#ifdef OCCT_DEBUG
    if (stat == CSLib_InfinityOfSolutions)
      std::cout << "BRepBlend_Walking::Transition : Infinite de Normal" << std::endl;
#endif
  }

  IntSurf::MakeTransition(tgline, tgrst, normale, TLine, TArc);
}

void BRepBlend_Walking::MakeExtremity(BRepBlend_Extremity&             Extrem,
                                      const Standard_Boolean           OnFirst,
                                      const Standard_Integer           Index,
                                      const Standard_Real              Param,
                                      const Standard_Boolean           IsVtx,
                                      const Handle(Adaptor3d_HVertex)& Vtx)
{
  IntSurf_Transition          Tline, Tarc;
  Standard_Integer            nbarc;
  Handle(Adaptor3d_TopolTool) Iter;

  if (OnFirst)
  {
    Extrem.SetValue(previousP.PointOnS1(), sol(1), sol(2), previousP.Parameter(), tolpoint3d);
    if (!previousP.IsTangencyPoint())
      Extrem.SetTangent(previousP.TangentOnS1());
    Iter = recdomain1;
  }
  else
  {
    Extrem.SetValue(previousP.PointOnS2(), sol(3), sol(4), previousP.Parameter(), tolpoint3d);
    if (!previousP.IsTangencyPoint())
      Extrem.SetTangent(previousP.TangentOnS2());
    Iter = recdomain2;
  }

  Iter->Init();
  nbarc = 1;

  while (nbarc < Index)
  {
    nbarc++;
    Iter->Next();
  }

  Transition(OnFirst, Iter->Value(), Param, Tline, Tarc);
  Extrem.AddArc(Iter->Value(), Param, Tline, Tarc);
  if (IsVtx)
    Extrem.SetVertex(Vtx);
}

void BRepBlend_Walking::MakeSingularExtremity(BRepBlend_Extremity&             Extrem,
                                              const Standard_Boolean           OnFirst,
                                              const Handle(Adaptor3d_HVertex)& Vtx)
{
  IntSurf_Transition          Tline, Tarc;
  Handle(Adaptor3d_TopolTool) Iter;
  Standard_Real               prm;

  if (OnFirst)
  {
    Iter = recdomain1;
    if (!previousP.IsTangencyPoint())
      Extrem.SetTangent(previousP.TangentOnS1());
  }
  else
  {
    if (!previousP.IsTangencyPoint())
      Extrem.SetTangent(previousP.TangentOnS2());
    Iter = recdomain2;
  }

  Iter->Init();
  Extrem.SetVertex(Vtx);
  while (Iter->More())
  {
    Handle(Adaptor2d_Curve2d) arc = Iter->Value();
    Iter->Initialize(arc);
    Iter->InitVertexIterator();
    while (Iter->MoreVertex())
    {
      if (Iter->Identical(Vtx, Iter->Vertex()))
      {
        prm = BRepBlend_BlendTool::Parameter(Vtx, arc);
        Transition(OnFirst, arc, prm, Tline, Tarc);
        Extrem.AddArc(arc, prm, Tline, Tarc);
      }
      Iter->NextVertex();
    }
    Iter->Next();
  }
}

static const Standard_Real CosRef3D = 0.88;

static void RecadreIfPeriodic(Standard_Real&      NewU,
                              Standard_Real&      NewV,
                              const Standard_Real OldU,
                              const Standard_Real OldV,
                              const Standard_Real UPeriod,
                              const Standard_Real VPeriod)
{
  if (UPeriod > 0.)
  {
    Standard_Real sign = (NewU < OldU) ? 1 : -1;
    while (Abs(NewU - OldU) > UPeriod / 2)
      NewU += sign * UPeriod;
  }
  if (VPeriod > 0.)
  {
    Standard_Real sign = (NewV < OldV) ? 1 : -1;
    while (Abs(NewV - OldV) > VPeriod / 2)
      NewV += sign * VPeriod;
  }
}

static void evalpinit(math_Vector&           parinit,
                      const Blend_Point&     previousP,
                      const Standard_Real    parprec,
                      const Standard_Real    param,
                      const math_Vector&     infbound,
                      const math_Vector&     supbound,
                      const Standard_Boolean classonS1,
                      const Standard_Boolean classonS2)
{
  if (previousP.IsTangencyPoint())
  {
    previousP.ParametersOnS1(parinit(1), parinit(2));
    previousP.ParametersOnS2(parinit(3), parinit(4));
  }
  else
  {
    Standard_Real    u1, v1, u2, v2;
    Standard_Real    du1, dv1, du2, dv2;
    Standard_Boolean Inside = Standard_True;
    previousP.ParametersOnS1(u1, v1);
    previousP.ParametersOnS2(u2, v2);
    previousP.Tangent2dOnS1().Coord(du1, dv1);
    previousP.Tangent2dOnS2().Coord(du2, dv2);
    Standard_Real step = param - parprec;
    u1 += step * du1;
    v1 += step * dv1;
    if (classonS1)
    {
      if ((u1 < infbound(1)) || (u1 > supbound(1)))
        Inside = Standard_False;
      if ((v1 < infbound(2)) || (v1 > supbound(2)))
        Inside = Standard_False;
    }
    u2 += step * du2;
    v2 += step * dv2;
    if (classonS2)
    {
      if ((u2 < infbound(3)) || (u2 > supbound(3)))
        Inside = Standard_False;
      if ((v2 < infbound(4)) || (v2 > supbound(4)))
        Inside = Standard_False;
    }

    if (Inside)
    {
      parinit(1) = u1;
      parinit(2) = v1;
      parinit(3) = u2;
      parinit(4) = v2;
    }
    else
    { // on ne joue pas au plus malin
      previousP.ParametersOnS1(parinit(1), parinit(2));
      previousP.ParametersOnS2(parinit(3), parinit(4));
    }
  }
}

void BRepBlend_Walking::InternalPerform(Blend_Function&     Func,
                                        Blend_FuncInv&      FuncInv,
                                        const Standard_Real Bound)
{
  Standard_Real Cosi = 0., Cosi2 = 0.;

  Standard_Real    stepw = pasmax;
  Standard_Integer nbp   = line->NbPoints();
  if (nbp >= 2)
  { // On reprend le dernier step s'il n est pas trop petit.
    if (sens < 0.)
    {
      stepw = (line->Point(2).Parameter() - line->Point(1).Parameter());
    }
    else
    {
      stepw = (line->Point(nbp).Parameter() - line->Point(nbp - 1).Parameter());
    }
    stepw = Max(stepw, 100. * tolgui);
  }
  Standard_Real parprec = param;
  gp_Vec        TgOnGuide, PrevTgOnGuide;
  gp_Pnt        PtOnGuide;
  hguide->D1(parprec, PtOnGuide, TgOnGuide);
  PrevTgOnGuide = TgOnGuide;

  if (sens * (parprec - Bound) >= -tolgui)
  {
    return;
  }
  Blend_Status              State = Blend_OnRst12;
  TopAbs_State              situ1 = TopAbs_IN, situ2 = TopAbs_IN;
  Standard_Real             w1, w2;
  Standard_Integer          Index1 = 0, Index2 = 0, nbarc;
  Standard_Boolean          Arrive, recad1, recad2, control;
  Standard_Boolean          Isvtx1 = Standard_False, Isvtx2 = Standard_False, echecrecad;
  gp_Pnt2d                  p2d;
  math_Vector               tolerance(1, 4), infbound(1, 4), supbound(1, 4), parinit(1, 4);
  math_Vector               solrst1(1, 4), solrst2(1, 4);
  Handle(Adaptor3d_HVertex) Vtx1, Vtx2;
  BRepBlend_Extremity       Ext1, Ext2;

  // IntSurf_Transition Tline,Tarc;

  Func.GetTolerance(tolerance, tolpoint3d);
  Func.GetBounds(infbound, supbound);

  math_FunctionSetRoot rsnld(Func, tolerance, 30);
  parinit = sol;

  Arrive = Standard_False;
  param  = parprec + sens * stepw;
  if (sens * (param - Bound) > 0.)
  {
    stepw = sens * (Bound - parprec) * 0.5;
    param = parprec + sens * stepw;
  }

  evalpinit(parinit, previousP, parprec, param, infbound, supbound, clasonS1, clasonS2);

  while (!Arrive)
  {
#ifdef OCCT_DEBUG
    sectioncalculee = 0;
    nbcomputedsection++;
#endif

    hguide->D1(param, PtOnGuide, TgOnGuide);
    // Check deflection on guide
    Cosi = PrevTgOnGuide * TgOnGuide;
    if (Cosi < gp::Resolution()) // angle>=pi/2 or null magnitude
      Cosi2 = 0.;
    else
      Cosi2 = Cosi * Cosi / PrevTgOnGuide.SquareMagnitude() / TgOnGuide.SquareMagnitude();
    if (Cosi2 < CosRef3D) // angle 3d too great
    {
      State = Blend_StepTooLarge;
      stepw = stepw / 2.;
      param = parprec + sens * stepw; // on ne risque pas de depasser Bound.
      if (Abs(stepw) < tolgui)
      {
        Ext1.SetValue(previousP.PointOnS1(), sol(1), sol(2), previousP.Parameter(), tolpoint3d);
        Ext2.SetValue(previousP.PointOnS2(), sol(3), sol(4), previousP.Parameter(), tolpoint3d);
        if (!previousP.IsTangencyPoint())
        {
          Ext1.SetTangent(previousP.TangentOnS1());
          Ext2.SetTangent(previousP.TangentOnS2());
        }
        Arrive = Standard_True;
      }
      continue;
    }
    PrevTgOnGuide = TgOnGuide;
    //////////////////////////

    Standard_Boolean bonpoint = 1;
    Func.Set(param);
    rsnld.Perform(Func, parinit, infbound, supbound);

    if (!rsnld.IsDone())
    {
      State    = Blend_StepTooLarge;
      bonpoint = 0;
    }
    else
    {
      rsnld.Root(sol);

      if (clasonS1)
        situ1 = domain1->Classify(gp_Pnt2d(sol(1), sol(2)), Min(tolerance(1), tolerance(2)), 0);
      else
        situ1 = TopAbs_IN;
      if (clasonS2)
        situ2 = domain2->Classify(gp_Pnt2d(sol(3), sol(4)), Min(tolerance(3), tolerance(4)), 0);
      else
        situ2 = TopAbs_IN;
    }
    if (bonpoint && line->NbPoints() == 1 && (situ1 != TopAbs_IN || situ2 != TopAbs_IN))
    {
      State    = Blend_StepTooLarge;
      bonpoint = 0;
    }
    if (bonpoint)
    {
      w1 = w2    = Bound;
      recad1     = Standard_False;
      recad2     = Standard_False;
      echecrecad = Standard_False;
      control    = Standard_False;

      if (situ1 == TopAbs_OUT || situ1 == TopAbs_ON)
      {
        // pb inverse sur surf1
        // Si le recadrage s'effectue dans le sens de la progression a une tolerance pres,
        // on a pris la mauvaise solution.
        recad1 = Recadre(FuncInv, Standard_True, sol, solrst1, Index1, Isvtx1, Vtx1);

        if (recad1)
        {
          Standard_Real wtemp;
          wtemp = solrst1(2);
          if ((param - wtemp) / sens >= -10 * tolgui)
          {
            w1      = solrst1(2);
            control = Standard_True;
          }
          else
          {
            echecrecad = Standard_True;
            recad1     = Standard_False;
            State      = Blend_StepTooLarge;
            bonpoint   = 0;
            stepw      = stepw / 2.;
          }
        }
        else
        {
          echecrecad = Standard_True;
        }
      }
      if (situ2 == TopAbs_OUT || situ2 == TopAbs_ON)
      {
        // pb inverse sur surf2
        // Si le recadrage s'effectue dans le sens de la progression a une tolerance pres,
        // on a pris la mauvaise solution.
        recad2 = Recadre(FuncInv, Standard_False, sol, solrst2, Index2, Isvtx2, Vtx2);

        if (recad2)
        {
          Standard_Real wtemp;
          wtemp = solrst2(2);
          if ((param - wtemp) / sens >= -10 * tolgui)
          {
            w2      = solrst2(2);
            control = Standard_True;
          }
          else
          {
            echecrecad = Standard_True;
            recad2     = Standard_False;
            State      = Blend_StepTooLarge;
            bonpoint   = 0;
            stepw      = stepw / 2.;
          }
        }
        else
        {
          echecrecad = Standard_True;
        }
      }

      // Que faut il controler
      if (recad1 && recad2)
      {
        if (Abs(w1 - w2) <= 10 * tolgui)
        {
          // pas besoin de controler les recadrage
          // Le control pouvant se planter (cf model blend10)
          // La tolerance est choisie grossse afin, de permetre au
          // cheminement suivant, de poser quelques sections ...
          control = Standard_False;
        }
        else if (sens * (w1 - w2) < 0.)
        {
          // sol sur 1 ?
          recad2 = Standard_False;
        }
        else
        {
          // sol sur 2 ?
          recad1 = Standard_False;
        }
      }

      // Controle effectif des recadrage
      if (control)
      {
        TopAbs_State situ;
        if (recad1 && clasonS2)
        {
          situ =
            recdomain2->Classify(gp_Pnt2d(solrst1(3), solrst1(4)), Min(tolerance(3), tolerance(4)));
          if (situ == TopAbs_OUT)
          {
            recad1     = Standard_False;
            echecrecad = Standard_True;
          }
        }
        else if (recad2 && clasonS1)
        {
          situ =
            recdomain1->Classify(gp_Pnt2d(solrst2(3), solrst2(4)), Min(tolerance(1), tolerance(1)));
          if (situ == TopAbs_OUT)
          {
            recad2     = Standard_False;
            echecrecad = Standard_True;
          }
        }
      }

      if (recad1 || recad2)
        echecrecad = Standard_False;

      if (!echecrecad)
      {
        if (recad1 && recad2)
        {
          // sol sur 1 et 2 a la fois
          //  On passe par les arcs , pour ne pas avoir de probleme
          //  avec les surfaces periodiques.
          State = Blend_OnRst12;
          param = (w1 + w2) / 2;
          gp_Pnt Pnt1, Pnt2;
          p2d    = BRepBlend_HCurve2dTool::Value(recdomain1->Value(), solrst1(1));
          sol(1) = p2d.X();
          sol(2) = p2d.Y();
          Pnt1   = Adaptor3d_HSurfaceTool::Value(surf1, sol(1), sol(2));
          p2d    = BRepBlend_HCurve2dTool::Value(recdomain2->Value(), solrst2(1));
          sol(3) = p2d.X();
          sol(4) = p2d.Y();
          Pnt2   = Adaptor3d_HSurfaceTool::Value(surf2, sol(3), sol(4));
          const Standard_Real TolProd = 1.e-5;
          Standard_Real       SavedParams[2];
          Standard_Boolean    SameDirs[2] = {Standard_False, Standard_False};
          ChFiDS_ElSpine&     theElSpine  = *hguide;
          SavedParams[0]                  = theElSpine.GetSavedFirstParameter();
          SavedParams[1]                  = theElSpine.GetSavedLastParameter();
          for (Standard_Integer ind = 0; ind < 2; ind++)
          {
            if (!Precision::IsInfinite(SavedParams[ind]))
            {
              // Check the original first and last parameters of guide curve
              // for equality to found parameter <param>:
              // check equality of tangent to guide curve and
              // normal to plane built on 3 points:
              // point on guide curve and points on restrictions of adjacent
              // surfaces.
              gp_Pnt Pnt0;
              gp_Vec Dir0;
              hguide->D1(SavedParams[ind], Pnt0, Dir0);
              Standard_Real Length = Dir0.Magnitude();
              if (Length <= gp::Resolution())
                continue;
              Dir0 /= Length;
              gce_MakePln PlaneBuilder(Pnt0, Pnt1, Pnt2);
              if (!PlaneBuilder.IsDone())
                continue;
              gp_Pln        thePlane = PlaneBuilder.Value();
              gp_Dir        DirPlane = thePlane.Axis().Direction();
              gp_Vec        theProd  = Dir0 ^ DirPlane;
              Standard_Real ProdMod  = theProd.Magnitude();
              if (ProdMod <= TolProd)
                SameDirs[ind] = Standard_True;
            }
          }
          Standard_Real theParam = Precision::Infinite();
          // Choose the closest parameter
          if (SameDirs[0] && SameDirs[1])
            theParam = (Abs(param - SavedParams[0]) < Abs(param - SavedParams[1])) ? SavedParams[0]
                                                                                   : SavedParams[1];
          else if (SameDirs[0])
            theParam = SavedParams[0];
          else if (SameDirs[1])
            theParam = SavedParams[1];

          Standard_Real    NewU, NewV, NewParam;
          gp_Pnt           NewPnt;
          Standard_Boolean Corrected =
            CorrectExtremityOnOneRst(1, sol(3), sol(4), param, Pnt1, NewU, NewV, NewPnt, NewParam);
          if (Corrected)
          {
            if (Abs(param - NewParam) < Abs(param - theParam))
              theParam = NewParam;
          }

          if (!Precision::IsInfinite(theParam))
            param = theParam;
        }
        else if (recad1)
        {
          // sol sur 1
          State = Blend_OnRst1;
          param = w1;
          recdomain1->Init();
          nbarc = 1;
          while (nbarc < Index1)
          {
            nbarc++;
            recdomain1->Next();
          }
          p2d    = BRepBlend_HCurve2dTool::Value(recdomain1->Value(), solrst1(1));
          sol(1) = p2d.X();
          sol(2) = p2d.Y();
          sol(3) = solrst1(3);
          sol(4) = solrst1(4);
          gp_Pnt           thePntOnRst = Adaptor3d_HSurfaceTool::Value(surf1, sol(1), sol(2));
          Standard_Real    NewU, NewV, NewParam;
          gp_Pnt           NewPnt;
          Standard_Boolean Corrected = CorrectExtremityOnOneRst(1,
                                                                sol(3),
                                                                sol(4),
                                                                param,
                                                                thePntOnRst,
                                                                NewU,
                                                                NewV,
                                                                NewPnt,
                                                                NewParam);
          if (Corrected)
          {
            param  = NewParam;
            sol(3) = NewU;
            sol(4) = NewV;
          }
        }
        else if (recad2)
        {
          // sol sur 2
          State = Blend_OnRst2;
          param = w2;

          recdomain2->Init();
          nbarc = 1;
          while (nbarc < Index2)
          {
            nbarc++;
            recdomain2->Next();
          }
          p2d    = BRepBlend_HCurve2dTool::Value(recdomain2->Value(), solrst2(1));
          sol(1) = solrst2(3);
          sol(2) = solrst2(4);
          sol(3) = p2d.X();
          sol(4) = p2d.Y();
          gp_Pnt           thePntOnRst = Adaptor3d_HSurfaceTool::Value(surf2, sol(3), sol(4));
          Standard_Real    NewU, NewV, NewParam;
          gp_Pnt           NewPnt;
          Standard_Boolean Corrected = CorrectExtremityOnOneRst(2,
                                                                sol(1),
                                                                sol(2),
                                                                param,
                                                                thePntOnRst,
                                                                NewU,
                                                                NewV,
                                                                NewPnt,
                                                                NewParam);
          if (Corrected)
          {
            param  = NewParam;
            sol(1) = NewU;
            sol(2) = NewV;
          }
        }
        else
        {
          State = Blend_OK;
        }

        Standard_Boolean testdefl = 1;
#ifdef OCCT_DEBUG
        testdefl = !Blend_GetcontextNOTESTDEFL();
#endif
        if (recad1 || recad2)
        {
          Func.Set(param);
          // Il vaut mieux un pas non orthodoxe que pas de recadrage!! PMN
          State = TestArret(Func,
                            State,
                            (testdefl && (Abs(stepw) > 3 * tolgui)),
                            Standard_False,
                            Standard_True);
        }
        else
        {
          State = TestArret(Func, State, testdefl);
        }
      }
      else
      {
        // Ou bien le pas max est mal regle. On divise.
        //	if(line->NbPoints() == 1) State = Blend_StepTooLarge;
        if (stepw > 2 * tolgui)
          State = Blend_StepTooLarge;
        // Sinon echec recadrage. On sort avec PointsConfondus
        else
        {
#ifdef OCCT_DEBUG
          std::cout << "Echec recadrage" << std::endl;
#endif
          State = Blend_SamePoints;
        }
      }
    }

#ifdef OCCT_DEBUG
    if (Blend_GettraceDRAWSECT())
    {
      Drawsect(surf1, surf2, sol, param, Func, State);
    }
#endif
    switch (State)
    {
      case Blend_OK: {
        // Mettre a jour la ligne.
        if (sens > 0.)
        {
          line->Append(previousP);
        }
        else
        {
          line->Prepend(previousP);
        }
#ifdef DRAW
        Standard_Integer nbpts = line->NbPoints();
        char             name[100];
        sprintf(name, "pg%d", nbpts);
        DrawTrSurf::Set(name, PtOnGuide);
        sprintf(name, "p1_%d", nbpts);
        DrawTrSurf::Set(name, previousP.PointOnS1());
        sprintf(name, "p2_%d", nbpts);
        DrawTrSurf::Set(name, previousP.PointOnS2());
#endif

        parprec = param;

        if (param == Bound)
        {
          Arrive = Standard_True;
          Ext1.SetValue(previousP.PointOnS1(), sol(1), sol(2), previousP.Parameter(), tolpoint3d);
          Ext2.SetValue(previousP.PointOnS2(), sol(3), sol(4), previousP.Parameter(), tolpoint3d);
          if (!previousP.IsTangencyPoint())
          {
            Ext1.SetTangent(previousP.TangentOnS1());
            Ext2.SetTangent(previousP.TangentOnS2());
          }

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
        evalpinit(parinit, previousP, parprec, param, infbound, supbound, clasonS1, clasonS2);
      }
      break;

      case Blend_StepTooLarge: {
        stepw = stepw / 2.;
        if (Abs(stepw) < tolgui)
        {
          Ext1.SetValue(previousP.PointOnS1(), sol(1), sol(2), previousP.Parameter(), tolpoint3d);
          Ext2.SetValue(previousP.PointOnS2(), sol(3), sol(4), previousP.Parameter(), tolpoint3d);
          if (!previousP.IsTangencyPoint())
          {
            Ext1.SetTangent(previousP.TangentOnS1());
            Ext2.SetTangent(previousP.TangentOnS2());
          }
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
          evalpinit(parinit, previousP, parprec, param, infbound, supbound, clasonS1, clasonS2);
        }
      }
      break;

      case Blend_StepTooSmall: {
        // Mettre a jour la ligne.
        if (sens > 0.)
        {
          line->Append(previousP);
        }
        else
        {
          line->Prepend(previousP);
        }
#ifdef DRAW
        Standard_Integer nbpts = line->NbPoints();
        char             name[100];
        sprintf(name, "pg%d", nbpts);
        DrawTrSurf::Set(name, PtOnGuide);
        sprintf(name, "p1_%d", nbpts);
        DrawTrSurf::Set(name, previousP.PointOnS1());
        sprintf(name, "p2_%d", nbpts);
        DrawTrSurf::Set(name, previousP.PointOnS2());
#endif

        parprec = param;

        stepw = Min(1.5 * stepw, pasmax);
        if (param == Bound)
        {
          Arrive = Standard_True;
          Ext1.SetValue(previousP.PointOnS1(), sol(1), sol(2), previousP.Parameter(), tolpoint3d);
          Ext2.SetValue(previousP.PointOnS2(), sol(3), sol(4), previousP.Parameter(), tolpoint3d);
          if (!previousP.IsTangencyPoint())
          {
            Ext1.SetTangent(previousP.TangentOnS1());
            Ext2.SetTangent(previousP.TangentOnS2());
          }
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
        evalpinit(parinit, previousP, parprec, param, infbound, supbound, clasonS1, clasonS2);
      }
      break;

      case Blend_OnRst1: {
        if (sens > 0.)
        {
          line->Append(previousP);
        }
        else
        {
          line->Prepend(previousP);
        }
#ifdef DRAW
        Standard_Integer nbpts = line->NbPoints();
        char             name[100];
        sprintf(name, "pg%d", nbpts);
        DrawTrSurf::Set(name, PtOnGuide);
        sprintf(name, "p1_%d", nbpts);
        DrawTrSurf::Set(name, previousP.PointOnS1());
        sprintf(name, "p2_%d", nbpts);
        DrawTrSurf::Set(name, previousP.PointOnS2());
#endif

        MakeExtremity(Ext1, Standard_True, Index1, solrst1(1), Isvtx1, Vtx1);
        // On blinde le cas singulier ou un des recadrage a planter
        if (previousP.PointOnS1().IsEqual(previousP.PointOnS2(), 2.0 * tolpoint3d))
        {
          Ext2.SetValue(previousP.PointOnS1(), sol(3), sol(4), tolpoint3d);
          if (Isvtx1)
            MakeSingularExtremity(Ext2, Standard_False, Vtx1);
        }
        else
        {
          Ext2.SetValue(previousP.PointOnS2(), sol(3), sol(4), previousP.Parameter(), tolpoint3d);
        }
        Arrive = Standard_True;
      }
      break;

      case Blend_OnRst2: {
        if (sens > 0.)
        {
          line->Append(previousP);
        }
        else
        {
          line->Prepend(previousP);
        }
#ifdef DRAW
        Standard_Integer nbpts = line->NbPoints();
        char             name[100];
        sprintf(name, "pg%d", nbpts);
        DrawTrSurf::Set(name, PtOnGuide);
        sprintf(name, "p1_%d", nbpts);
        DrawTrSurf::Set(name, previousP.PointOnS1());
        sprintf(name, "p2_%d", nbpts);
        DrawTrSurf::Set(name, previousP.PointOnS2());
#endif

        // On blinde le cas singulier ou un des recadrage a plante
        if (previousP.PointOnS1().IsEqual(previousP.PointOnS2(), 2.0 * tolpoint3d))
        {
          Ext1.SetValue(previousP.PointOnS2(), sol(1), sol(2), tolpoint3d);
          if (Isvtx2)
            MakeSingularExtremity(Ext1, Standard_True, Vtx2);
        }
        else
        {
          Ext1.SetValue(previousP.PointOnS1(), sol(1), sol(2), previousP.Parameter(), tolpoint3d);
        }
        MakeExtremity(Ext2, Standard_False, Index2, solrst2(1), Isvtx2, Vtx2);
        Arrive = Standard_True;
      }
      break;

      case Blend_OnRst12: {
        if (sens > 0.)
        {
          line->Append(previousP);
        }
        else
        {
          line->Prepend(previousP);
        }
#ifdef DRAW
        Standard_Integer nbpts = line->NbPoints();
        char             name[100];
        sprintf(name, "pg%d", nbpts);
        DrawTrSurf::Set(name, PtOnGuide);
        sprintf(name, "p1_%d", nbpts);
        DrawTrSurf::Set(name, previousP.PointOnS1());
        sprintf(name, "p2_%d", nbpts);
        DrawTrSurf::Set(name, previousP.PointOnS2());
#endif

        if ((Isvtx1 != Isvtx2)
            && (previousP.PointOnS1().IsEqual(previousP.PointOnS2(), 2.0 * tolpoint3d)))
        {
          // On blinde le cas singulier ou un seul recadrage
          // est reconnu comme vertex.
          if (Isvtx1)
          {
            Isvtx2 = Standard_True;
            Vtx2   = Vtx1;
          }
          else
          {
            Isvtx1 = Standard_True;
            Vtx1   = Vtx2;
          }
        }

        MakeExtremity(Ext1, Standard_True, Index1, solrst1(1), Isvtx1, Vtx1);
        MakeExtremity(Ext2, Standard_False, Index2, solrst2(1), Isvtx2, Vtx2);
        Arrive = Standard_True;
      }
      break;

      case Blend_SamePoints: {
        // On arrete
#ifdef OCCT_DEBUG
        std::cout << " Points confondus dans le cheminement" << std::endl;
#endif
        Ext1.SetValue(previousP.PointOnS1(), sol(1), sol(2), previousP.Parameter(), tolpoint3d);
        Ext2.SetValue(previousP.PointOnS2(), sol(3), sol(4), previousP.Parameter(), tolpoint3d);
        if (!previousP.IsTangencyPoint())
        {
          Ext1.SetTangent(previousP.TangentOnS1());
          Ext2.SetTangent(previousP.TangentOnS2());
        }
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
        line->SetEndPoints(Ext1, Ext2);
      }
      else
      {
        line->SetStartPoints(Ext1, Ext2);
      }
    }
  }
}

Standard_Boolean BRepBlend_Walking::CorrectExtremityOnOneRst(const Standard_Integer IndexOfRst,
                                                             const Standard_Real    theU,
                                                             const Standard_Real    theV,
                                                             const Standard_Real    theParam,
                                                             const gp_Pnt&          thePntOnRst,
                                                             Standard_Real&         NewU,
                                                             Standard_Real&         NewV,
                                                             gp_Pnt&                NewPoint,
                                                             Standard_Real&         NewParam) const
{
  const Standard_Real TolAng = 0.001; // bug OCC25701

  ChFiDS_ElSpine& theElSpine = *hguide;
  if (theElSpine.NbVertices() == 0)
    return Standard_False;

  Handle(Adaptor3d_TopolTool) DomainOfRst = (IndexOfRst == 1) ? recdomain1 : recdomain2;
  Handle(Adaptor3d_Surface)   SurfOfRst   = (IndexOfRst == 1) ? surf1 : surf2;
  Handle(Adaptor3d_Surface)   AnotherSurf = (IndexOfRst == 1) ? surf2 : surf1;

  // Correct point on surface 2
  // First we find right <param>
  Standard_Real Ends[2];
  Ends[0]                       = BRepBlend_HCurve2dTool::FirstParameter(DomainOfRst->Value());
  Ends[1]                       = BRepBlend_HCurve2dTool::LastParameter(DomainOfRst->Value());
  Standard_Real GlobalMinSqDist = Precision::Infinite();
  Standard_Real ParamOnGuide    = 0;
  gp_Pnt        PointOnGuide;
  for (Standard_Integer k = 0; k < 2; k++)
  {
    gp_Pnt2d      P2dOnEnd = BRepBlend_HCurve2dTool::Value(DomainOfRst->Value(), Ends[k]);
    gp_Pnt        PntOnEnd = Adaptor3d_HSurfaceTool::Value(SurfOfRst, P2dOnEnd.X(), P2dOnEnd.Y());
    Extrema_ExtPC projoncurv(PntOnEnd, theElSpine);
    if (!projoncurv.IsDone())
      continue;
    Standard_Real    MinSqDist = Precision::Infinite();
    Standard_Integer imin      = 0;
    for (Standard_Integer ind = 1; ind <= projoncurv.NbExt(); ind++)
    {
      Standard_Real aSqDist = projoncurv.SquareDistance(ind);
      if (aSqDist < MinSqDist)
      {
        MinSqDist = aSqDist;
        imin      = ind;
      }
    }
    if (MinSqDist < GlobalMinSqDist)
    {
      GlobalMinSqDist = MinSqDist;
      ParamOnGuide    = projoncurv.Point(imin).Parameter();
      PointOnGuide    = projoncurv.Point(imin).Value();
    }
  }
  NewParam = ParamOnGuide;
  if (hguide->IsPeriodic())
  {
    Standard_Real Period = hguide->Period();
    Standard_Real sign   = (NewParam < theParam) ? 1 : -1;
    while (Abs(NewParam - theParam) > Period / 2)
      NewParam += sign * Period;
  }

  // Second we find right point and tangent on guide
  GlobalMinSqDist = Precision::Infinite();
  gp_Ax1 theAx1;
  for (Standard_Integer ind = 1; ind <= theElSpine.NbVertices(); ind++)
  {
    const gp_Ax1& anAx1   = theElSpine.VertexWithTangent(ind);
    gp_Pnt        aPnt    = anAx1.Location();
    Standard_Real aSqDist = PointOnGuide.SquareDistance(aPnt);
    if (aSqDist < GlobalMinSqDist)
    {
      GlobalMinSqDist = aSqDist;
      theAx1          = anAx1;
    }
  }
  const gp_Pnt& Pnt0 = theAx1.Location();
  const gp_Dir& Dir0 = theAx1.Direction();
  // Check new point: is it real solution?
  gp_Pnt      OldPonGuide = hguide->Value(theParam);
  gp_Pnt      PntOnSurf2  = Adaptor3d_HSurfaceTool::Value(AnotherSurf, theU, theV); // old point
  gce_MakePln PlaneBuilder(thePntOnRst, OldPonGuide, PntOnSurf2);
  if (!PlaneBuilder.IsDone())
    return Standard_False;
  gp_Pln        OldPlane = PlaneBuilder.Value();
  gp_Dir        OldDir   = OldPlane.Axis().Direction();
  Standard_Real Angle    = OldDir.Angle(Dir0);
  if (Angle > M_PI / 2)
    Angle = M_PI - Angle;
  if (Angle > TolAng)
    return Standard_False;
  ///////////////////////////////////////
  // Project the point(theU,theV) on the plane(Pnt0,Dir0)
  gp_Vec aVec(Pnt0, PntOnSurf2);
  gp_Vec aTranslation((aVec.XYZ() * Dir0.XYZ()) * Dir0.XYZ());
  gp_Pnt PntOnPlane = PntOnSurf2.Translated(-aTranslation);

  // Check new point again: does point on restriction belong to the plane?
  PlaneBuilder = gce_MakePln(thePntOnRst, Pnt0, PntOnPlane);
  if (!PlaneBuilder.IsDone())
    return Standard_False;
  gp_Pln        NewPlane      = PlaneBuilder.Value();
  const gp_Dir& DirOfNewPlane = NewPlane.Axis().Direction();
  Angle                       = Dir0.Angle(DirOfNewPlane);
  if (Angle > M_PI / 2)
    Angle = M_PI - Angle;
  if (Angle > TolAng)
    return Standard_False;
  ////////////////////////////////////////////////////////////////////////

  // Project the point <PntOnPlane> on the surface 2
  Extrema_ExtPS projonsurf(PntOnPlane,
                           *AnotherSurf,
                           Precision::PConfusion(),
                           Precision::PConfusion(),
                           Extrema_ExtFlag_MIN);
  if (projonsurf.IsDone())
  {
    Standard_Real    MinSqDist = Precision::Infinite();
    Standard_Integer imin      = 0;
    for (Standard_Integer ind = 1; ind <= projonsurf.NbExt(); ind++)
    {
      Standard_Real aSqDist = projonsurf.SquareDistance(ind);
      if (aSqDist < MinSqDist)
      {
        MinSqDist = aSqDist;
        imin      = ind;
      }
    }
    if (imin)
    {
      Extrema_POnSurf NewPOnSurf2 = projonsurf.Point(imin);
      NewPoint                    = NewPOnSurf2.Value();
      NewPOnSurf2.Parameter(NewU, NewV);
      Standard_Real uperiod = (AnotherSurf->IsUPeriodic()) ? AnotherSurf->UPeriod() : 0.;
      Standard_Real vperiod = (AnotherSurf->IsVPeriodic()) ? AnotherSurf->VPeriod() : 0.;
      RecadreIfPeriodic(NewU, NewV, theU, theV, uperiod, vperiod);
      return Standard_True;
    }
  }

  return Standard_False;
}
