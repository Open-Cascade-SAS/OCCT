// Created on: 1996-11-07
// Created by: Laurent BUCHARD
// Copyright (c) 1996-1999 Matra Datavision
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

#include <Adaptor2d_Curve2d.hxx>
#include <Adaptor3d_TopolTool.hxx>
#include <IntPatch_ALine.hxx>
#include <IntPatch_GLine.hxx>
#include <IntPatch_Line.hxx>
#include <IntPatch_LineConstructor.hxx>
#include <IntPatch_RLine.hxx>
#include <IntPatch_WLine.hxx>

#define XPU1009 1

#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <IntSurf_Quadric.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <Standard_ConstructionError.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <ElCLib.hxx>
#include <Geom2dInt_TheProjPCurOfGInter.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_Array1.hxx>

//=================================================================================================

static void Recadre(const occ::handle<Adaptor3d_Surface>& myHS1,
                    const occ::handle<Adaptor3d_Surface>& myHS2,
                    double&                               u1,
                    double&                               v1,
                    double&                               u2,
                    double&                               v2)
{
  double              f, l, lmf;
  GeomAbs_SurfaceType typs1 = myHS1->GetType();
  GeomAbs_SurfaceType typs2 = myHS2->GetType();

  bool myHS1IsUPeriodic, myHS1IsVPeriodic;
  switch (typs1)
  {
    case GeomAbs_Cylinder:
    case GeomAbs_Cone:
    case GeomAbs_Sphere: {
      myHS1IsUPeriodic = true;
      myHS1IsVPeriodic = false;
      break;
    }
    case GeomAbs_Torus: {
      myHS1IsUPeriodic = myHS1IsVPeriodic = true;
      break;
    }
    default: {
      //-- Le cas de biparametrees periodiques est gere en amont
      myHS1IsUPeriodic = myHS1IsVPeriodic = false;
      break;
    }
  }

  bool myHS2IsUPeriodic, myHS2IsVPeriodic;
  switch (typs2)
  {
    case GeomAbs_Cylinder:
    case GeomAbs_Cone:
    case GeomAbs_Sphere: {
      myHS2IsUPeriodic = true;
      myHS2IsVPeriodic = false;
      break;
    }
    case GeomAbs_Torus: {
      myHS2IsUPeriodic = myHS2IsVPeriodic = true;
      break;
    }
    default: {
      //-- Le cas de biparametrees periodiques est gere en amont
      myHS2IsUPeriodic = myHS2IsVPeriodic = false;
      break;
    }
  }
  if (myHS1IsUPeriodic)
  {
    lmf = M_PI + M_PI; //-- myHS1->UPeriod();
    f   = myHS1->FirstUParameter();
    l   = myHS1->LastUParameter();
    while (u1 < f)
    {
      u1 += lmf;
    }
    while (u1 > l)
    {
      u1 -= lmf;
    }
  }
  if (myHS1IsVPeriodic)
  {
    lmf = M_PI + M_PI; //-- myHS1->VPeriod();
    f   = myHS1->FirstVParameter();
    l   = myHS1->LastVParameter();
    while (v1 < f)
    {
      v1 += lmf;
    }
    while (v1 > l)
    {
      v1 -= lmf;
    }
  }
  if (myHS2IsUPeriodic)
  {
    lmf = M_PI + M_PI; //-- myHS2->UPeriod();
    f   = myHS2->FirstUParameter();
    l   = myHS2->LastUParameter();
    while (u2 < f)
    {
      u2 += lmf;
    }
    while (u2 > l)
    {
      u2 -= lmf;
    }
  }
  if (myHS2IsVPeriodic)
  {
    lmf = M_PI + M_PI; //-- myHS2->VPeriod();
    f   = myHS2->FirstVParameter();
    l   = myHS2->LastVParameter();
    while (v2 < f)
    {
      v2 += lmf;
    }
    while (v2 > l)
    {
      v2 -= lmf;
    }
  }
}

//=================================================================================================

static void Parameters(const occ::handle<Adaptor3d_Surface>& myHS1,
                       const occ::handle<Adaptor3d_Surface>& myHS2,
                       const gp_Pnt&                         Ptref,
                       double&                               U1,
                       double&                               V1,
                       double&                               U2,
                       double&                               V2)
{
  IntSurf_Quadric     quad1, quad2;
  GeomAbs_SurfaceType typs = myHS1->GetType();
  switch (typs)
  {
    case GeomAbs_Plane:
      quad1.SetValue(myHS1->Plane());
      break;
    case GeomAbs_Cylinder:
      quad1.SetValue(myHS1->Cylinder());
      break;
    case GeomAbs_Cone:
      quad1.SetValue(myHS1->Cone());
      break;
    case GeomAbs_Sphere:
      quad1.SetValue(myHS1->Sphere());
      break;
    case GeomAbs_Torus:
      quad1.SetValue(myHS1->Torus());
      break;
    default:
      throw Standard_ConstructionError("IntPatch_IntSS::MakeCurve");
  }

  typs = myHS2->GetType();
  switch (typs)
  {
    case GeomAbs_Plane:
      quad2.SetValue(myHS2->Plane());
      break;
    case GeomAbs_Cylinder:
      quad2.SetValue(myHS2->Cylinder());
      break;
    case GeomAbs_Cone:
      quad2.SetValue(myHS2->Cone());
      break;
    case GeomAbs_Sphere:
      quad2.SetValue(myHS2->Sphere());
      break;
    case GeomAbs_Torus:
      quad2.SetValue(myHS2->Torus());
      break;
    default:
      throw Standard_ConstructionError("IntPatch_IntSS::MakeCurve");
  }
  quad1.Parameters(Ptref, U1, V1);
  quad2.Parameters(Ptref, U2, V2);
}

//=================================================================================================

static double LocalFirstParameter(const occ::handle<IntPatch_Line>& L)
{
  double         firstp = 0.;
  IntPatch_IType typl   = L->ArcType();
  switch (typl)
  {
    case IntPatch_Analytic: {
      occ::handle<IntPatch_ALine> alin(occ::down_cast<IntPatch_ALine>(L));
      if (alin->HasFirstPoint())
      {
        firstp = alin->FirstPoint().ParameterOnLine();
      }
      else
      {
        bool included;
        firstp = alin->FirstParameter(included);
        if (!included)
        {
          firstp += Epsilon(firstp);
        }
      }
      return firstp;
    }

    case IntPatch_Restriction: {
      occ::handle<IntPatch_RLine> rlin(occ::down_cast<IntPatch_RLine>(L));
      if (rlin->HasFirstPoint())
      {
        firstp = rlin->FirstPoint().ParameterOnLine();
      }
      else
      {
        firstp = -Precision::Infinite(); // a voir selon le type de la ligne 2d
      }
      return firstp;
    }
    case IntPatch_Walking: {

      occ::handle<IntPatch_WLine> wlin(occ::down_cast<IntPatch_WLine>(L));
      if (wlin->HasFirstPoint())
      {
        firstp = wlin->FirstPoint().ParameterOnLine();
      }
      else
      {
        firstp = 1.;
      }
      return firstp;
    }

    default: {
      occ::handle<IntPatch_GLine> glin(occ::down_cast<IntPatch_GLine>(L));
      if (glin->HasFirstPoint())
      {
        firstp = glin->FirstPoint().ParameterOnLine();
      }
      else
      {
        switch (typl)
        {
          case IntPatch_Lin:
          case IntPatch_Parabola:
          case IntPatch_Hyperbola:
            firstp = -Precision::Infinite();
            break;

          case IntPatch_Circle:
          case IntPatch_Ellipse:
            firstp = 0.;
            break;
          default: {
          }
        }
      }
      return firstp;
    }
  }
  //  return firstp;
}

//=================================================================================================

static double LocalLastParameter(const occ::handle<IntPatch_Line>& L)
{
  double         lastp = 0.;
  IntPatch_IType typl  = L->ArcType();
  switch (typl)
  {
    case IntPatch_Analytic: {
      occ::handle<IntPatch_ALine> alin(occ::down_cast<IntPatch_ALine>(L));

      if (alin->HasLastPoint())
      {
        lastp = alin->LastPoint().ParameterOnLine();
      }
      else
      {
        bool included;
        lastp = alin->LastParameter(included);
        if (!included)
        {
          lastp -= Epsilon(lastp);
        }
      }
      return lastp;
    }

    case IntPatch_Restriction: {
      occ::handle<IntPatch_RLine> rlin(occ::down_cast<IntPatch_RLine>(L));

      if (rlin->HasLastPoint())
      {
        lastp = rlin->LastPoint().ParameterOnLine();
      }
      else
      {
        lastp = Precision::Infinite(); // a voir selon le type de la ligne 2d
      }
      return lastp;
    }
    case IntPatch_Walking: {
      occ::handle<IntPatch_WLine> wlin(occ::down_cast<IntPatch_WLine>(L));

      if (wlin->HasLastPoint())
      {
        lastp = wlin->LastPoint().ParameterOnLine();
      }
      else
      {
        lastp = wlin->NbPnts();
      }
      return lastp;
    }

    default: {
      occ::handle<IntPatch_GLine> glin(occ::down_cast<IntPatch_GLine>(L));

      if (glin->HasLastPoint())
      {
        lastp = glin->LastPoint().ParameterOnLine();
      }
      else
      {
        switch (typl)
        {
          case IntPatch_Lin:
          case IntPatch_Parabola:
          case IntPatch_Hyperbola:
            lastp = Precision::Infinite();
            break;

          case IntPatch_Circle:
          case IntPatch_Ellipse:
            lastp = M_PI + M_PI;
            break;
          default: {
          }
        }
      }
      return lastp;
    }
  }
}

// modified by NIZHNY-MKK  Tue Apr  3 15:03:06 2001.BEGIN
//=================================================================================================

static double ComputeParametricTolerance(const double  theTol3d,
                                         const gp_Vec& theD1u,
                                         const gp_Vec& theD1v)
{
  double nad1u = theD1u.Magnitude();
  double nad1v = theD1v.Magnitude();
  double tolu = 0., tolv = 0.;
  if (nad1u > 1e-12)
    tolu = theTol3d / nad1u;
  else
    tolu = 0.1;
  if (nad1v > 1e-12)
    tolv = theTol3d / nad1v;
  else
    tolv = 0.1;
  double aTolerance = (tolu > tolv) ? tolu : tolv;
  return aTolerance;
}

// modified by NIZHNY-MKK  Tue Apr  3 15:03:11 2001.END

//=================================================================================================

IntPatch_LineConstructor::IntPatch_LineConstructor(const int) {}

//=================================================================================================

static int AppendSameVertexA(occ::handle<IntPatch_ALine>&       alig,
                             const occ::handle<IntPatch_ALine>& L,
                             const int                          index,
                             int*                               TabIndex)
{
  int i, a, n;
  a                              = 0;
  n                              = L->NbVertex();
  const IntPatch_Point& Vtxindex = L->Vertex(index);
  double                thetol1  = Vtxindex.Tolerance();
  for (i = 1; i <= n; i++)
  {
    if (i != index)
    {
      const IntPatch_Point& Vtxi    = L->Vertex(i);
      double                thetol2 = Vtxi.Tolerance();
      if (thetol2 < thetol1)
        thetol2 = thetol1;
      double d_4 = Vtxindex.Value().Distance(Vtxi.Value());
      if (d_4 <= thetol2)
      {
        alig->AddVertex(Vtxi);
        a++;
        TabIndex[i] = TabIndex[index];
      }
    }
  }
  return (a);
}

//=================================================================================================

static int AppendSameVertexG(occ::handle<IntPatch_GLine>&       glig,
                             const occ::handle<IntPatch_GLine>& L,
                             const int                          index,
                             const double                       decal,
                             int*                               TabIndex)
{
  int    i, a, n;
  double p1, p2, d; //,tol
  bool   aajouter;
  a                              = 0;
  n                              = L->NbVertex();
  const IntPatch_Point& Vtxindex = L->Vertex(index);
  double                thetol1  = Vtxindex.Tolerance();
  for (i = 1; i <= n; i++)
  {
    if (i != index)
    {
      const IntPatch_Point& Vtxi = L->Vertex(i);
      aajouter                   = false;
      double thetol2             = Vtxi.Tolerance();
      if (thetol2 < thetol1)
        thetol2 = thetol1;
      d = Vtxindex.Value().Distance(Vtxi.Value());
      if (d <= thetol2)
      {
        aajouter = true;
      }

      //-- Le test suivant a ete ajoute le 20 aout 98 (??? mefiance ???)
      else
      {
        p1 = Vtxindex.ParameterOnLine();
        p2 = Vtxi.ParameterOnLine();
        if (std::abs(p1 - p2) < Precision::PConfusion())
        {
          aajouter = true;
        }
      }
      if (aajouter)
      {
        p1                  = Vtxindex.ParameterOnLine();
        IntPatch_Point aVtx = Vtxi;
        aVtx.SetParameter(p1 + decal);
        glig->AddVertex(aVtx);
        a++;
        TabIndex[i] = TabIndex[index];
      }
    }
  }
  return (a);
}

//=================================================================================================

static int AppendSameVertexW(occ::handle<IntPatch_WLine>&       wlig,
                             const occ::handle<IntPatch_WLine>& L,
                             const int                          index,
                             const double                       par,
                             int*                               TabIndex)
{
  int i, a, n;
  a                              = 0;
  n                              = L->NbVertex();
  const IntPatch_Point& Vtxindex = L->Vertex(index);
  const gp_Pnt&         Pntindex = Vtxindex.Value();
  double                thetol1  = Vtxindex.Tolerance();
  for (i = 1; i <= n; i++)
  {
    if (i != index)
    {
      IntPatch_Point Vtxi    = L->Vertex(i);
      double         d_2     = Pntindex.Distance(Vtxi.Value());
      double         thetol2 = Vtxi.Tolerance();
      if (thetol2 < thetol1)
        thetol2 = thetol1;
      //-- le debugger voit 2 fois la variable d ici. ???? -> d_2
      if (d_2 <= thetol2)
      {
        Vtxi.SetParameter(par);
        double u1, v1, u2, v2;
        Vtxindex.ParametersOnS1(u1, v1);
        Vtxindex.ParametersOnS2(u2, v2);
        Vtxi.SetParameters(u1, v1, u2, v2);
        Vtxi.SetValue(Pntindex);
        wlig->AddVertex(Vtxi);
        a++;
        TabIndex[i] = TabIndex[index];
      }
    }
  }
  return (a);
}

//=================================================================================================

static int AppendSameVertexR(occ::handle<IntPatch_RLine>&       rlig,
                             const occ::handle<IntPatch_RLine>& L,
                             const int                          index,
                             int*                               TabIndex)
{
  int i, a, n;
  a                              = 0;
  n                              = L->NbVertex();
  const IntPatch_Point& Vtxindex = L->Vertex(index);
  double                thetol1  = Vtxindex.Tolerance();
  for (i = 1; i <= n; i++)
  {
    if (i != index)
    {
      const IntPatch_Point& Vtxi    = L->Vertex(i);
      double                d_3     = Vtxindex.Value().Distance(Vtxi.Value());
      double                thetol2 = Vtxi.Tolerance();
      if (thetol2 < thetol1)
        thetol2 = thetol1;
      if (d_3 < thetol2)
      {
        if (Vtxi.ParameterOnLine() != Vtxindex.ParameterOnLine())
        {
          IntPatch_Point Vtxicop = L->Vertex(i);
          Vtxicop.SetParameter(Vtxindex.ParameterOnLine());
          rlig->AddVertex(Vtxicop);
        }
        else
        {
          rlig->AddVertex(Vtxi);
        }
        a++;
        TabIndex[i] = TabIndex[index];
      }
    }
  }
  return (a);
}

//=================================================================================================

static void AddLine(const occ::handle<IntPatch_Line>& L,
                    const int                         i,
                    const int                         j,
                    //	     const GeomAbs_SurfaceType TypeS1,
                    const GeomAbs_SurfaceType,
                    //	     const GeomAbs_SurfaceType TypeS2,
                    const GeomAbs_SurfaceType,
                    int*                                              TabIndex,
                    NCollection_Sequence<occ::handle<IntPatch_Line>>& slin)
{
  int IndexFirstVertex = 1;
  int IndexLastVertex  = 2;
  if (i == j)
  {
    IndexLastVertex = 1;
  }
  IntPatch_IType typl = L->ArcType();
  switch (typl)
  {
    case IntPatch_Analytic: {
      occ::handle<IntPatch_ALine> ALine(occ::down_cast<IntPatch_ALine>(L));
      occ::handle<IntPatch_ALine> alig;
      if (L->TransitionOnS1() == IntSurf_Undecided)
        alig = new IntPatch_ALine(ALine->Curve(), L->IsTangent());
      else if (L->TransitionOnS1() == IntSurf_Touch)
        alig =
          new IntPatch_ALine(ALine->Curve(), L->IsTangent(), L->SituationS1(), L->SituationS2());
      else
        alig = new IntPatch_ALine(ALine->Curve(),
                                  L->IsTangent(),
                                  L->TransitionOnS1(),
                                  L->TransitionOnS2());
      alig->AddVertex(ALine->Vertex(i));
      IndexLastVertex += AppendSameVertexA(alig, ALine, i, TabIndex);
      if (i != j)
      {
        alig->AddVertex(ALine->Vertex(j));
        IndexLastVertex += AppendSameVertexA(alig, ALine, j, TabIndex);
      }
      alig->SetFirstPoint(IndexFirstVertex);
      alig->SetLastPoint(IndexLastVertex);
      slin.Append(alig);
      break;
    }
    case IntPatch_Walking: { //-- ****************************************
      occ::handle<IntPatch_WLine>          WLine(occ::down_cast<IntPatch_WLine>(L));
      const occ::handle<IntSurf_LineOn2S>& Lori           = WLine->Curve();
      occ::handle<IntSurf_LineOn2S>        LineOn2S       = new IntSurf_LineOn2S();
      int                                  ParamMinOnLine = (int)WLine->Vertex(i).ParameterOnLine();
      int                                  ParamMaxOnLine = (int)WLine->Vertex(j).ParameterOnLine();
      for (int k = ParamMinOnLine; k <= ParamMaxOnLine; k++)
      {
        LineOn2S->Add(Lori->Value(k));
      }
      occ::handle<IntPatch_WLine> wlig;
      if (L->TransitionOnS1() == IntSurf_Undecided)
        wlig = new IntPatch_WLine(LineOn2S, L->IsTangent());
      else if (L->TransitionOnS1() == IntSurf_Touch)
        wlig = new IntPatch_WLine(LineOn2S, L->IsTangent(), L->SituationS1(), L->SituationS2());
      else
        wlig =
          new IntPatch_WLine(LineOn2S, L->IsTangent(), L->TransitionOnS1(), L->TransitionOnS2());
      if (WLine->HasArcOnS1())
      {
        wlig->SetArcOnS1(WLine->GetArcOnS1());
      }
      if (WLine->HasArcOnS2())
      {
        wlig->SetArcOnS2(WLine->GetArcOnS2());
      }
      IntPatch_Point Vtx = WLine->Vertex(i);
      Vtx.SetParameter(1);
      wlig->AddVertex(Vtx);
      IndexLastVertex += AppendSameVertexW(wlig, WLine, i, 1, TabIndex);
      if (i != j)
      {
        Vtx = WLine->Vertex(j);
        Vtx.SetParameter(LineOn2S->NbPoints());
        wlig->AddVertex(Vtx);
        IndexLastVertex += AppendSameVertexW(wlig, WLine, j, LineOn2S->NbPoints(), TabIndex);
      }
      wlig->SetFirstPoint(IndexFirstVertex);
      wlig->SetLastPoint(IndexLastVertex);
      wlig->SetPeriod(WLine->U1Period(), WLine->V1Period(), WLine->U2Period(), WLine->V2Period());
      wlig->ComputeVertexParameters(Precision::Confusion());
      slin.Append(wlig);
      //-- **********************************************************************

      break;
    }
    case IntPatch_Restriction: {
      occ::handle<IntPatch_RLine> RLine(occ::down_cast<IntPatch_RLine>(L));
      IndexLastVertex  = 2;
      IndexFirstVertex = 1;
      occ::handle<IntPatch_RLine> rlig;
      if (L->TransitionOnS1() == IntSurf_Undecided)
        rlig = new IntPatch_RLine(L->IsTangent());
      else if (L->TransitionOnS1() == IntSurf_Touch)
        rlig = new IntPatch_RLine(L->IsTangent(), L->SituationS1(), L->SituationS2());
      else
        rlig = new IntPatch_RLine(L->IsTangent(), L->TransitionOnS1(), L->TransitionOnS2());
      if (RLine->IsArcOnS1())
      {
        rlig->SetArcOnS1(RLine->ArcOnS1());
      }
      if (RLine->IsArcOnS2())
      {
        rlig->SetArcOnS2(RLine->ArcOnS2());
      }

      rlig->AddVertex(RLine->Vertex(i));
#if XPU1009
      IndexLastVertex += AppendSameVertexR(rlig, RLine, i, TabIndex);
#endif
      for (int k = i + 1; k < j; k++)
      {
        rlig->AddVertex(RLine->Vertex(k));
        IndexLastVertex++;
      }
      if (i != j)
      {
        rlig->AddVertex(RLine->Vertex(j));
#if XPU1009
        IndexLastVertex += AppendSameVertexR(rlig, RLine, j, TabIndex);
#endif
      }
      rlig->SetFirstPoint(IndexFirstVertex);
      rlig->SetLastPoint(IndexLastVertex);
      rlig->ComputeVertexParameters(Precision::Confusion());
      slin.Append(rlig);
      break;
    }
    case IntPatch_Lin:
    case IntPatch_Parabola:
    case IntPatch_Hyperbola:
    case IntPatch_Circle:
    case IntPatch_Ellipse: {
      occ::handle<IntPatch_GLine> GLine(occ::down_cast<IntPatch_GLine>(L));
      occ::handle<IntPatch_GLine> glig;
      switch (typl)
      {
        case IntPatch_Lin:
          if (L->TransitionOnS1() == IntSurf_Undecided)
            glig = new IntPatch_GLine(GLine->Line(), L->IsTangent());
          else if (L->TransitionOnS1() == IntSurf_Touch)
            glig =
              new IntPatch_GLine(GLine->Line(), L->IsTangent(), L->SituationS1(), L->SituationS2());
          else
            glig = new IntPatch_GLine(GLine->Line(),
                                      L->IsTangent(),
                                      L->TransitionOnS1(),
                                      L->TransitionOnS2());
          break;
        case IntPatch_Parabola:
          if (L->TransitionOnS1() == IntSurf_Undecided)
            glig = new IntPatch_GLine(GLine->Parabola(), L->IsTangent());
          else if (L->TransitionOnS1() == IntSurf_Touch)
            glig = new IntPatch_GLine(GLine->Parabola(),
                                      L->IsTangent(),
                                      L->SituationS1(),
                                      L->SituationS2());
          else
            glig = new IntPatch_GLine(GLine->Parabola(),
                                      L->IsTangent(),
                                      L->TransitionOnS1(),
                                      L->TransitionOnS2());
          break;
        case IntPatch_Hyperbola:
          if (L->TransitionOnS1() == IntSurf_Undecided)
            glig = new IntPatch_GLine(GLine->Hyperbola(), L->IsTangent());
          else if (L->TransitionOnS1() == IntSurf_Touch)
            glig = new IntPatch_GLine(GLine->Hyperbola(),
                                      L->IsTangent(),
                                      L->SituationS1(),
                                      L->SituationS2());
          else
            glig = new IntPatch_GLine(GLine->Hyperbola(),
                                      L->IsTangent(),
                                      L->TransitionOnS1(),
                                      L->TransitionOnS2());
          break;
        case IntPatch_Circle:
          if (L->TransitionOnS1() == IntSurf_Undecided)
            glig = new IntPatch_GLine(GLine->Circle(), L->IsTangent());
          else if (L->TransitionOnS1() == IntSurf_Touch)
            glig = new IntPatch_GLine(GLine->Circle(),
                                      L->IsTangent(),
                                      L->SituationS1(),
                                      L->SituationS2());
          else
            glig = new IntPatch_GLine(GLine->Circle(),
                                      L->IsTangent(),
                                      L->TransitionOnS1(),
                                      L->TransitionOnS2());
          break;
        case IntPatch_Ellipse:
        default:
          if (L->TransitionOnS1() == IntSurf_Undecided)
            glig = new IntPatch_GLine(GLine->Ellipse(), L->IsTangent());
          else if (L->TransitionOnS1() == IntSurf_Touch)
            glig = new IntPatch_GLine(GLine->Ellipse(),
                                      L->IsTangent(),
                                      L->SituationS1(),
                                      L->SituationS2());
          else
            glig = new IntPatch_GLine(GLine->Ellipse(),
                                      L->IsTangent(),
                                      L->TransitionOnS1(),
                                      L->TransitionOnS2());
          break;
      }
      glig->AddVertex(GLine->Vertex(i));
      IndexLastVertex += AppendSameVertexG(glig, GLine, i, 0, TabIndex);
      if (i != j)
      {
        if ((typl == IntPatch_Circle || typl == IntPatch_Ellipse) && i > j)
        {
          IntPatch_Point Vtx = GLine->Vertex(j);
          Vtx.SetParameter(GLine->Vertex(j).ParameterOnLine() + M_PI + M_PI);
          glig->AddVertex(Vtx);
          IndexLastVertex += AppendSameVertexG(glig, GLine, j, M_PI + M_PI, TabIndex);
        }
        else
        {
          glig->AddVertex(GLine->Vertex(j));
          IndexLastVertex += AppendSameVertexG(glig, GLine, j, 0, TabIndex);
        }
      }
      glig->SetFirstPoint(IndexFirstVertex);
      glig->SetLastPoint(IndexLastVertex);
      slin.Append(glig);
      break;
    }
    default: {
      throw Standard_ConstructionError("IntPatch_LineConstructor::AddLine");
    }
    break;
  }
}

//=================================================================================================

occ::handle<IntPatch_Line> IntPatch_LineConstructor::Line(const int l) const
{
  return (slin.Value(l));
}

//=================================================================================================

int IntPatch_LineConstructor::NbLines() const
{
  return (slin.Length());
}

//=================================================================================================

static double GetVertexTolerance(const IntPatch_Point& vtx/*,
					const occ::handle<Adaptor3d_TopolTool>& aDomain1,
					const occ::handle<Adaptor3d_TopolTool>& aDomain2*/)
{
  double tol = vtx.Tolerance();
  //    if (aDomain1->Has3d() && vtx.IsVertexOnS1()) {
  //      double tolv = aDomain1->Tol3d(vtx.VertexOnS1());
  //      if (tolv > tol) tol = tolv;
  //    }
  //    if (aDomain2->Has3d() && vtx.IsVertexOnS2()) {
  //      double tolv = aDomain2->Tol3d(vtx.VertexOnS2());
  //      if (tolv > tol) tol = tolv;
  //    }
  return tol;
}

//=================================================================================================

static bool IsSegmentSmall(const occ::handle<IntPatch_WLine>& WLine,
				       const int ivFirst,
				       const int ivLast/*,
				       const double TolArc*/)
{
  const IntPatch_Point& vtxF = WLine->Vertex(ivFirst);
  const IntPatch_Point& vtxL = WLine->Vertex(ivLast);
  int                   ipF  = (int)vtxF.ParameterOnLine();
  int                   ipL  = (int)vtxL.ParameterOnLine();
  if (ipF >= ipL)
    return true;

  double tolF = GetVertexTolerance(vtxF);
  double tolL = GetVertexTolerance(vtxL);
  double tol  = std::max(tolF, tolL);

  double len = 0.;
  gp_Pnt p1  = WLine->Point(ipF).Value();
  for (int i = ipF + 1; i <= ipL; i++)
  {
    const gp_Pnt& p2 = WLine->Point(i).Value();
    len += p1.Distance(p2);
    if (len > tol)
      break;
    p1 = p2;
  }
  return len <= tol;
}

//=================================================================================================

static bool TestWLineIsARLine(const NCollection_Sequence<occ::handle<IntPatch_Line>>& slinref,
                              const occ::handle<IntPatch_WLine>&                      wlin,
                              const double                                            tol2d)
{
  int nbpnt     = wlin->NbPnts();
  int indicepnt = nbpnt / 2;
  if (indicepnt < 1)
    return (false);
  const IntSurf_PntOn2S& POn2S  = wlin->Point(indicepnt);
  const IntSurf_PntOn2S& POn2S1 = wlin->Point(indicepnt + 1);
  int                    lastl  = slinref.Length();
  for (int i = 1; i <= lastl; i++)
  {
    if (slinref.Value(i)->ArcType() == IntPatch_Restriction)
    {
      occ::handle<IntPatch_RLine>& rlin = *((occ::handle<IntPatch_RLine>*)&(slinref(i)));
      for (int is = 0; is < 2; is++)
      {
        bool onFirst = is == 0;
        if ((onFirst && rlin->IsArcOnS1()) || (!onFirst && rlin->IsArcOnS2()))
        {
          occ::handle<Adaptor2d_Curve2d> arc;
          double                         u, v, u1, v1;
          if (onFirst)
          {
            arc = rlin->ArcOnS1();
            POn2S.ParametersOnS1(u, v);
            POn2S1.ParametersOnS1(u1, v1);
          }
          else
          {
            arc = rlin->ArcOnS2();
            POn2S.ParametersOnS2(u, v);
            POn2S1.ParametersOnS2(u1, v1);
          }
          if (indicepnt == 1)
          {
            u = (u + u1) * 0.5;
            v = (v + v1) * 0.5;
          }
          const Adaptor2d_Curve2d& C2d = *arc;
          gp_Pnt2d                 PObt, P2d(u, v);
          double par = Geom2dInt_TheProjPCurOfGInter::FindParameter(C2d, P2d, 1e-7);
          PObt       = C2d.Value(par);
          if (PObt.Distance(P2d) < tol2d)
          {
            return true;
          }
        }
      }
    }
  }
  return false;
}

//=================================================================================================

static bool TestIfWLineIsRestriction(
  const NCollection_Sequence<occ::handle<IntPatch_Line>>& slinref,
  const occ::handle<IntPatch_WLine>&                      wlin,
  const occ::handle<Adaptor3d_Surface>&                   S1,
  const occ::handle<Adaptor3d_TopolTool>&                 D1,
  const occ::handle<Adaptor3d_Surface>&                   S2,
  const occ::handle<Adaptor3d_TopolTool>&                 D2,
  double                                                  TolArc)
{

  int    NbPnts = wlin->NbPnts();
  int    allon1 = 0, allon2 = 0, i;
  double tol2d1 = 0., tol2d2 = 0.;
  for (i = 1; i <= NbPnts; i++)
  {
    const IntSurf_PntOn2S& Pmid = wlin->Point(i);
    double                 u1, v1, u2, v2;
    Pmid.Parameters(u1, v1, u2, v2);
    //-- Estimation d un majorant de Toluv a partir de Tol
    gp_Pnt ap;
    gp_Vec ad1u, ad1v;
    double tol;
    //------------------------------------------
    S1->D1(u1, v1, ap, ad1u, ad1v);
    tol = ComputeParametricTolerance(TolArc, ad1u, ad1v);
    if (tol > tol2d1)
      tol2d1 = tol;
    //--
    if (allon1 + 1 == i && D1->IsThePointOn(gp_Pnt2d(u1, v1), tol))
    {
      allon1++;
    }
    //------------------------------------------
    S2->D1(u2, v2, ap, ad1u, ad1v);
    tol = ComputeParametricTolerance(TolArc, ad1u, ad1v);
    if (tol > tol2d2)
      tol2d2 = tol;
    //--
    if (allon2 + 1 == i && D2->IsThePointOn(gp_Pnt2d(u2, v2), tol))
    {
      allon2++;
    }
    if (allon1 != i && allon2 != i)
      break;
  }
  if (allon1 == NbPnts || allon2 == NbPnts)
  {
#ifdef OCCT_DEBUG
    std::cout << " IntPatch_LineConstructor.gxx :  CC**ONS" << (allon1 == NbPnts ? 1 : 2)
              << "** Traitement WLIne + ARC CLASS " << std::endl;
#endif
    double tol2d = std::max(tol2d1, tol2d2);
    return TestWLineIsARLine(slinref, wlin, tol2d);
  }
  return false;
}

//=================================================================================================

static bool ProjectOnArc(const double                          u,
                         const double                          v,
                         const occ::handle<Adaptor2d_Curve2d>& arc,
                         const occ::handle<Adaptor3d_Surface>& surf,
                         const double                          TolArc,
                         double&                               par,
                         double&                               dist)
{
  gp_Pnt aPbid;
  gp_Vec ad1u, ad1v;
  surf->D1(u, v, aPbid, ad1u, ad1v);
  double                   tol2d = ComputeParametricTolerance(TolArc, ad1u, ad1v);
  const Adaptor2d_Curve2d& C2d   = *arc;
  gp_Pnt2d                 aP(u, v), aPprj;
  par   = Geom2dInt_TheProjPCurOfGInter::FindParameter(C2d, aP, 1e-7);
  aPprj = C2d.Value(par);
  dist  = aPprj.Distance(aP);
  return dist <= tol2d;
}

//=================================================================================================

static void TestWLineToRLine(const NCollection_Sequence<occ::handle<IntPatch_Line>>& slinref,
                             NCollection_Sequence<occ::handle<IntPatch_Line>>&       slin,
                             const occ::handle<Adaptor3d_Surface>&                   mySurf1,
                             const occ::handle<Adaptor3d_TopolTool>&                 myDom1,
                             const occ::handle<Adaptor3d_Surface>&                   mySurf2,
                             const occ::handle<Adaptor3d_TopolTool>&                 myDom2,
                             const double                                            TolArc)
{

  int                          lastwline = slin.Length();
  occ::handle<IntPatch_WLine>& WLine = *((occ::handle<IntPatch_WLine>*)&(slin.Value(lastwline)));

  int nbvtx = WLine->NbVertex();
  if (nbvtx < 2)
    return;
  int ParamMinOnLine = (int)WLine->Vertex(1).ParameterOnLine();
  int ParamMaxOnLine = (int)WLine->Vertex(nbvtx).ParameterOnLine();
  if (ParamMinOnLine >= ParamMaxOnLine)
    return;
  int midInd = (ParamMaxOnLine + ParamMinOnLine) / 2;

  NCollection_Sequence<int> indicesV1, indicesV2;
  int                       iv;
  for (iv = 1; iv <= nbvtx; iv++)
  {
    int plin = (int)WLine->Vertex(iv).ParameterOnLine();
    if (plin == ParamMinOnLine)
      indicesV1.Append(iv);
    else if (plin == ParamMaxOnLine)
      indicesV2.Append(iv);
  }

  bool isRLine = false;

  typedef void (IntSurf_PntOn2S::*PiParOnS)(double&, double&) const;
  typedef bool (IntPatch_Point::*PQuery)() const;
  typedef const occ::handle<Adaptor2d_Curve2d>& (IntPatch_Point::*PArcOnS)() const;
  typedef double (IntPatch_Point::*PParOnArc)() const;

  // cycle for both surfaces
  int is;
  for (is = 0; is < 2; is++)
  {
    bool onFirst = is == 0;
    if ((onFirst && WLine->HasArcOnS1()) || (!onFirst && WLine->HasArcOnS2()))
    {
      PiParOnS                         piParOnS;
      PQuery                           pIsOnDomS;
      PArcOnS                          pArcOnS;
      PParOnArc                        pParOnArc;
      occ::handle<Adaptor3d_Surface>   surf;
      occ::handle<Adaptor3d_TopolTool> aDomain;
      if (onFirst)
      {
        piParOnS  = &IntSurf_PntOn2S::ParametersOnS1;
        pIsOnDomS = &IntPatch_Point::IsOnDomS1;
        pArcOnS   = &IntPatch_Point::ArcOnS1;
        pParOnArc = &IntPatch_Point::ParameterOnArc1;
        surf      = mySurf1;
        aDomain   = myDom1;
      }
      else
      {
        piParOnS  = &IntSurf_PntOn2S::ParametersOnS2;
        pIsOnDomS = &IntPatch_Point::IsOnDomS2;
        pArcOnS   = &IntPatch_Point::ArcOnS2;
        pParOnArc = &IntPatch_Point::ParameterOnArc2;
        surf      = mySurf2;
        aDomain   = myDom2;
      }

      // resolve arcs for vertices not having a link to an arc
      double                                              utst, vtst;
      NCollection_Array1<double>                          paramsResolved(1, nbvtx);
      NCollection_Array1<occ::handle<Standard_Transient>> arcsResolved(1, nbvtx);
      arcsResolved.Init(occ::handle<Adaptor2d_Curve2d>());
      for (iv = 1; iv <= nbvtx; iv++)
      {
        if (!(WLine->Vertex(iv).*pIsOnDomS)())
        {
          int ip = (int)WLine->Vertex(iv).ParameterOnLine();
          (WLine->Point(ip).*piParOnS)(utst, vtst);
          double distmin = RealLast();
          for (aDomain->Init(); aDomain->More(); aDomain->Next())
          {
            const occ::handle<Adaptor2d_Curve2d>& arc = aDomain->Value();
            double                                par, dist;
            if (ProjectOnArc(utst, vtst, arc, surf, TolArc, par, dist) && dist < distmin)
            {
              arcsResolved(iv)   = arc;
              paramsResolved(iv) = par;
              distmin            = dist;
            }
          }
        }
      }

      // prepare list of common arcs for both ends of wline
      NCollection_IndexedMap<occ::handle<Standard_Transient>> mapArcsV1, mapArcs;
      int                                                     i;
      for (i = 1; i <= indicesV1.Length(); i++)
      {
        iv = indicesV1(i);
        occ::handle<Adaptor2d_Curve2d> arc;
        if ((WLine->Vertex(iv).*pIsOnDomS)())
          arc = (WLine->Vertex(iv).*pArcOnS)();
        else
          arc = occ::down_cast<Adaptor2d_Curve2d>(arcsResolved(iv));
        if (!arc.IsNull())
          mapArcsV1.Add(arc);
      }
      for (i = 1; i <= indicesV2.Length(); i++)
      {
        iv = indicesV2(i);
        occ::handle<Adaptor2d_Curve2d> arc;
        if ((WLine->Vertex(iv).*pIsOnDomS)())
          arc = (WLine->Vertex(iv).*pArcOnS)();
        else
          arc = occ::down_cast<Adaptor2d_Curve2d>(arcsResolved(iv));
        if (!arc.IsNull() && mapArcsV1.Contains(arc))
          mapArcs.Add(arc);
      }

      // for each common arc
      for (int ia = 1; ia <= mapArcs.Extent(); ia++)
      {
        const occ::handle<Adaptor2d_Curve2d> arc(occ::down_cast<Adaptor2d_Curve2d>(mapArcs(ia)));
        // get end vertices of wline linked with this arc
        int iv1 = 0, iv2 = 0;
        for (i = 1; i <= indicesV1.Length() && iv1 == 0; i++)
        {
          iv = indicesV1(i);
          occ::handle<Adaptor2d_Curve2d> arc1;
          if ((WLine->Vertex(iv).*pIsOnDomS)())
            arc1 = (WLine->Vertex(iv).*pArcOnS)();
          else
            arc1 = occ::down_cast<Adaptor2d_Curve2d>(arcsResolved(iv));
          if (!arc1.IsNull() && arc1 == arc)
            iv1 = iv;
        }
        for (i = 1; i <= indicesV2.Length() && iv2 == 0; i++)
        {
          iv = indicesV2(i);
          occ::handle<Adaptor2d_Curve2d> arc1;
          if ((WLine->Vertex(iv).*pIsOnDomS)())
            arc1 = (WLine->Vertex(iv).*pArcOnS)();
          else
            arc1 = occ::down_cast<Adaptor2d_Curve2d>(arcsResolved(iv));
          if (!arc1.IsNull() && arc1 == arc)
            iv2 = iv;
        }
        if (!iv1 || !iv2)
        {
#ifdef OCCT_DEBUG
          std::cout << " Pb getting vertices linked with arc" << std::endl;
#endif
          continue;
        }
        double par1 =
          (arcsResolved(iv1).IsNull() ? (WLine->Vertex(iv1).*pParOnArc)() : paramsResolved(iv1));
        double par2 =
          (arcsResolved(iv2).IsNull() ? (WLine->Vertex(iv2).*pParOnArc)() : paramsResolved(iv2));
#ifdef OCCT_DEBUG
        std::cout << "****** Parameters on arc on S" << is + 1 << ": " << par1 << " " << par2
                  << std::endl;
#endif

        // check that the middle point is on arc
        (WLine->Point(midInd).*piParOnS)(utst, vtst);
        if (midInd == ParamMinOnLine)
        {
          double utst1 = 0.0, vtst1 = 0.0;
          (WLine->Point(midInd + 1).*piParOnS)(utst1, vtst1);
          utst = (utst + utst1) * 0.5;
          vtst = (vtst + vtst1) * 0.5;
        }
        double par, dist;
        if (!ProjectOnArc(utst, vtst, arc, surf, TolArc, par, dist))
        {
#ifdef OCCT_DEBUG
          std::cout << " Pb en projection ds IntPatch_LineConstructor" << std::endl;
#endif
          continue;
        }

        //-- codage de la WLine en RLine
        occ::handle<IntPatch_RLine> rlig =
          new IntPatch_RLine(true, IntSurf_Unknown, IntSurf_Unknown);
        if (onFirst)
          rlig->SetArcOnS1(arc);
        else
          rlig->SetArcOnS2(arc);

        occ::handle<IntSurf_LineOn2S>        LineOn2S = new IntSurf_LineOn2S();
        const occ::handle<IntSurf_LineOn2S>& Lori     = WLine->Curve();
        int                                  ivmin, ivmax;
        double                               parmin, parmax;
        bool                                 reverse = false;
        NCollection_Sequence<int>*           pIndVmin, *pIndVmax;
        if (par1 < par2)
        {
          for (i = ParamMinOnLine; i <= ParamMaxOnLine; i++)
          {
            LineOn2S->Add(Lori->Value(i));
          }
          ivmin    = iv1;
          ivmax    = iv2;
          parmin   = par1;
          parmax   = par2;
          pIndVmin = &indicesV1;
          pIndVmax = &indicesV2;
        }
        else
        {
          for (i = ParamMaxOnLine; i >= ParamMinOnLine; i--)
          {
            LineOn2S->Add(Lori->Value(i));
          }
          ivmin    = iv2;
          ivmax    = iv1;
          parmin   = par2;
          parmax   = par1;
          pIndVmin = &indicesV2;
          pIndVmax = &indicesV1;
          reverse  = true;
        }
        rlig->Add(LineOn2S);
        IntSurf_Transition TransitionUndecided;
        IntPatch_Point     VtxFirst = WLine->Vertex(ivmin);
        VtxFirst.SetParameter(parmin);
        if (!arcsResolved(ivmin).IsNull())
          VtxFirst.SetArc(onFirst, arc, parmin, TransitionUndecided, TransitionUndecided);
        if (reverse)
          VtxFirst.ReverseTransition(); //-- inversion des transitions
        rlig->AddVertex(VtxFirst);
        for (i = 1; i <= pIndVmin->Length(); i++)
        {
          iv = pIndVmin->Value(i);
          if (iv != ivmin)
          {
            IntPatch_Point Vtx = WLine->Vertex(iv);
            Vtx.SetParameter(parmin);
            if (!arcsResolved(iv).IsNull())
              Vtx.SetArc(onFirst, arc, parmin, TransitionUndecided, TransitionUndecided);
            if (reverse)
              Vtx.ReverseTransition();
            rlig->AddVertex(Vtx);
          }
        }
        for (i = 1; i <= pIndVmax->Length(); i++)
        {
          iv = pIndVmax->Value(i);
          if (iv != ivmax)
          {
            IntPatch_Point Vtx = WLine->Vertex(iv);
            Vtx.SetParameter(parmax);
            if (!arcsResolved(iv).IsNull())
              Vtx.SetArc(onFirst, arc, parmax, TransitionUndecided, TransitionUndecided);
            if (reverse)
              Vtx.ReverseTransition();
            rlig->AddVertex(Vtx);
          }
        }
        IntPatch_Point VtxLast = WLine->Vertex(ivmax);
        VtxLast.SetParameter(parmax);
        if (!arcsResolved(ivmax).IsNull())
          VtxLast.SetArc(onFirst, arc, parmax, TransitionUndecided, TransitionUndecided);
        if (reverse)
          VtxLast.ReverseTransition();
        rlig->AddVertex(VtxLast);
        rlig->SetFirstPoint(1);
        rlig->SetLastPoint(indicesV1.Length() + indicesV2.Length());
        slin.Append(rlig);
        isRLine = true;
      }
    }
  }

  if (isRLine || TestIfWLineIsRestriction(slinref, WLine, mySurf1, myDom1, mySurf2, myDom2, TolArc))
  {
    slin.Remove(lastwline);
  }
}

//=================================================================================================

void IntPatch_LineConstructor::Perform(
  const NCollection_Sequence<occ::handle<IntPatch_Line>>& slinref,
  const occ::handle<IntPatch_Line>&                       L,
  const occ::handle<Adaptor3d_Surface>&                   mySurf1,
  const occ::handle<Adaptor3d_TopolTool>&                 myDom1,
  const occ::handle<Adaptor3d_Surface>&                   mySurf2,
  const occ::handle<Adaptor3d_TopolTool>&                 myDom2,
  const double                                            TolArc)
{

  int    i = 1, nbvtx;
  double firstp, lastp;
  // clang-format off
  double Tol = Precision::PConfusion()*100.; // JMB le 13 Jan 2000. Report de la correction du PRO19653
  // clang-format on
  GeomAbs_SurfaceType typs1 = mySurf1->GetType();
  GeomAbs_SurfaceType typs2 = mySurf2->GetType();

  IntPatch_IType typl = L->ArcType();
  if (typl == IntPatch_Analytic)
  {
    double                      u1, v1, u2, v2;
    occ::handle<IntPatch_ALine> ALine(occ::down_cast<IntPatch_ALine>(L));
    slin.Clear();
    nbvtx = ALine->NbVertex();
    //-- -------------------------------------------------------------------
    int* TabIndex = new int[nbvtx + 2];
    int  numline  = 0;
    for (i = 1; i <= nbvtx; i++)
    {
      // for(int i=1;i<=nbvtx;i++) {
      TabIndex[i] = 0;
    }
    //-- -------------------------------------------------------------------
    for (i = 1; i < nbvtx; i++)
    {
      const IntPatch_Point& ALine_Vertex_i   = ALine->Vertex(i);
      const IntPatch_Point& ALine_Vertex_ip1 = ALine->Vertex(i + 1);
      firstp                                 = ALine_Vertex_i.ParameterOnLine();
      lastp                                  = ALine_Vertex_ip1.ParameterOnLine();
      if (firstp != lastp)
      {
        double pmid = (firstp + lastp) * 0.5;
        gp_Pnt Pmid = ALine->Value(pmid);
        Parameters(mySurf1, mySurf2, Pmid, u1, v1, u2, v2);
        Recadre(mySurf1, mySurf2, u1, v1, u2, v2);
        TopAbs_State in1, in2;
        in1 = myDom1->Classify(gp_Pnt2d(u1, v1), Tol, false);
        in2 = (in1 != TopAbs_OUT) ? myDom2->Classify(gp_Pnt2d(u2, v2), Tol, false) : TopAbs_OUT;
        if (in1 == TopAbs_OUT || in2 == TopAbs_OUT)
        {
        }
        else
        {
          //-- std::cout<<"Analytic   : firtsp="<<firstp<<" lastp="<<lastp<<"
          // Vtx:"<<i<<","<<i+1<<std::endl;
          TabIndex[i] = TabIndex[i + 1] = ++numline;
          AddLine(L, i, i + 1, typs1, typs2, TabIndex, slin);
        }
      }
    }
    //-- -------------------------------------------------------------------
    //-- On recherche les vertex interference Edge Edge Externe
    //-- Ces vertex ne figurent sur aucune ligne et sont Restriction
    //-- sur les 2 edges
    for (i = 1; i <= nbvtx; i++)
    {
      if (TabIndex[i] == 0)
      {
        const IntPatch_Point& ALine_Vertex_i = ALine->Vertex(i);
        if (ALine_Vertex_i.IsOnDomS1() && ALine_Vertex_i.IsOnDomS2())
        {
          TabIndex[i] = ++numline;
          AddLine(L, i, i, typs1, typs2, TabIndex, slin);
        }
      }
    }
    delete[] TabIndex;
    //-- -------------------------------------------------------------------
    return;
  }
  else if (typl == IntPatch_Walking)
  {
    double                      u1, v1, u2, v2;
    occ::handle<IntPatch_WLine> WLine(occ::down_cast<IntPatch_WLine>(L));
    slin.Clear();
    nbvtx = WLine->NbVertex();
    //-- -------------------------------------------------------------------
    int* TabIndex = new int[nbvtx + 2];
    int  numline  = 0;
    for (i = 1; i <= nbvtx; i++)
    {
      // for(int i=1;i<=nbvtx;i++)  {
      TabIndex[i] = 0;
    }
    //-- -------------------------------------------------------------------
    for (i = 1; i < nbvtx; i++)
    {
      const IntPatch_Point& WLineVertex_i   = WLine->Vertex(i);
      const IntPatch_Point& WLineVertex_ip1 = WLine->Vertex(i + 1);
      firstp                                = WLineVertex_i.ParameterOnLine();
      lastp                                 = WLineVertex_ip1.ParameterOnLine();
      if (firstp != lastp && !IsSegmentSmall(WLine, i, i + 1 /*,TolArc*/))
      {
        int pmid;
        pmid           = (int)((firstp + lastp) / 2);
        int int_lastp  = (int)lastp;
        int int_firstp = (int)firstp;
        if (pmid == int_lastp)
          pmid = int_firstp;
        const IntSurf_PntOn2S& Pmid = WLine->Point(pmid);
        Pmid.Parameters(u1, v1, u2, v2);
        Recadre(mySurf1, mySurf2, u1, v1, u2, v2);

        // modified by NIZHNY-MKK  Tue Apr  3 15:03:40 2001.BEGIN
        //------------------------------------------
        gp_Pnt ap;
        gp_Vec ad1u, ad1v;
        mySurf1->D1(u1, v1, ap, ad1u, ad1v);
        double aTolerance = ComputeParametricTolerance(TolArc, ad1u, ad1v);
        //------------------------------------------

        // TopAbs_State in1 = myDom1->Classify(gp_Pnt2d(u1,v1),Tol,false);
        TopAbs_State in1 = myDom1->Classify(gp_Pnt2d(u1, v1), aTolerance, false);
        // TopAbs_State in2 = (in1!=TopAbs_OUT)?
        // myDom2->Classify(gp_Pnt2d(u2,v2),Tol,false) : TopAbs_OUT;
        TopAbs_State in2 = TopAbs_OUT;
        if (in1 != TopAbs_OUT)
        {
          //------------------------------------------
          mySurf2->D1(u2, v2, ap, ad1u, ad1v);
          aTolerance = ComputeParametricTolerance(TolArc, ad1u, ad1v);
          //------------------------------------------
          in2 = myDom2->Classify(gp_Pnt2d(u2, v2), aTolerance, false);
        }
        // modified by NIZHNY-MKK  Tue Apr  3 15:06:31 2001.END

        // modified by NIZHNY-OFV  Wed Jun 13 17:31:23 2001
        // --purpose: If on a face (lastp-firstp) == 1,
        //            sometimes it could mean a bad parametrisation of WLine.
        //            In this case we try to classify the "virtual" WLine point:
        //            the geometrical point between two vertices. This emulates
        //            situation when (lastp-firstp) != 1.
        if (std::abs(int_lastp - int_firstp) == 1)
        {
          double                 vFu1, vFv1, vFu2, vFv2, vLu1, vLv1, vLu2, vLv2;
          const IntSurf_PntOn2S& vF = WLineVertex_i.PntOn2S();
          const IntSurf_PntOn2S& vL = WLineVertex_ip1.PntOn2S();
          vF.Parameters(vFu1, vFv1, vFu2, vFv2);
          Recadre(mySurf1, mySurf2, vFu1, vFv1, vFu2, vFv2);
          vL.Parameters(vLu1, vLv1, vLu2, vLv2);
          Recadre(mySurf1, mySurf2, vLu1, vLv1, vLu2, vLv2);
          if (in1 != TopAbs_IN)
          {
            double   du, dv;
            gp_Pnt2d pvF(vFu1, vFv1);
            gp_Pnt2d pvL(vLu1, vLv1);
            gp_Pnt2d pPm(u1, v1);
            double   dpvFpPm = pvF.Distance(pPm);
            double   dpvLpPm = pvL.Distance(pPm);
            if (dpvFpPm > dpvLpPm)
            {
              du = (vFu1 + u1) * 0.5;
              dv = (vFv1 + v1) * 0.5;
            }
            else
            {
              du = (vLu1 + u1) * 0.5;
              dv = (vLv1 + v1) * 0.5;
            }
            mySurf1->D1(du, dv, ap, ad1u, ad1v);
            aTolerance = ComputeParametricTolerance(TolArc, ad1u, ad1v);
            in1        = myDom1->Classify(gp_Pnt2d(du, dv), aTolerance, false);
          }
          if (in2 != TopAbs_IN)
          {
            double   du, dv;
            gp_Pnt2d pvF(vFu2, vFv2);
            gp_Pnt2d pvL(vLu2, vLv2);
            gp_Pnt2d pPm(u2, v2);
            double   dpvFpPm = pvF.Distance(pPm);
            double   dpvLpPm = pvL.Distance(pPm);
            if (dpvFpPm > dpvLpPm)
            {
              du = (vFu2 + u2) * 0.5;
              dv = (vFv2 + v2) * 0.5;
            }
            else
            {
              du = (vLu2 + u2) * 0.5;
              dv = (vLv2 + v2) * 0.5;
            }
            mySurf2->D1(du, dv, ap, ad1u, ad1v);
            aTolerance = ComputeParametricTolerance(TolArc, ad1u, ad1v);
            in2        = myDom2->Classify(gp_Pnt2d(du, dv), aTolerance, false);
          }
        } // end of if(std::abs(int_lastp-int_firstp) == 1)

        if (in1 != TopAbs_OUT && in2 != TopAbs_OUT)
        {
          bool                   LignetropPetite = false;
          double                 u1a, v1a, u2a, v2a;
          const IntSurf_PntOn2S& Pmid1 = WLine->Point((int)firstp);
          Pmid1.Parameters(u1a, v1a, u2a, v2a);
          Recadre(mySurf1, mySurf2, u1a, v1a, u2a, v2a);

          const IntSurf_PntOn2S& Pmid2 = WLine->Point((int)lastp);
          double                 u1b, v1b, u2b, v2b;
          Pmid2.Parameters(u1b, v1b, u2b, v2b);
          Recadre(mySurf1, mySurf2, u1b, v1b, u2b, v2b);

          double dd12_u = std::abs(u1a - u1b);
          double dd12_v = std::abs(v1a - v1b);
          if (dd12_u + dd12_v < 1e-12)
          {
            dd12_u = std::abs(u1 - u1b);
            dd12_v = std::abs(v1 - v1b);
            if (dd12_u + dd12_v < 1e-12)
            {
              LignetropPetite = true;
            }
          }
          if (!LignetropPetite)
          {
            //-- std::cout<<"WLine      : firtsp="<<firstp<<" lastp="<<lastp<<"
            // Vtx:"<<i<<","<<i+1<<std::endl;
            TabIndex[i] = TabIndex[i + 1] = ++numline;
            AddLine(L, i, i + 1, typs1, typs2, TabIndex, slin);
            // clang-format off
	      TestWLineToRLine(slinref,slin,mySurf1,myDom1,mySurf2,myDom2,TolArc); //-- on teste la derniere entree de slin
                                      // clang-format on
          }
        } // end of if (in1 != TopAbs_OUT && in2 != TopAbs_OUT)
      } // end of if(firstp!=lastp && !IsSegmentSmall(WLine,i,i+1/*,TolArc*/))
    } // end of for(i=1;i<nbvtx;i++)

    //-- -------------------------------------------------------------------
    //-- On recherche les vertex interference Edge Edge Externe
    //-- Ces vertex ne figurent sur aucune ligne et sont Restriction
    //-- sur les 2 edges
    for (i = 1; i <= nbvtx; i++)
    {
      if (TabIndex[i] == 0)
      {
        const IntPatch_Point& WLine_Vertex_i = WLine->Vertex(i);
        if (WLine_Vertex_i.IsOnDomS1() && WLine_Vertex_i.IsOnDomS2())
        {
          TabIndex[i] = ++numline;
          AddLine(L, i, i, typs1, typs2, TabIndex, slin);
        }
      }
    }
    delete[] TabIndex;
    //-- -------------------------------------------------------------------
    return;
  }
  else if (typl != IntPatch_Restriction)
  { // JAG 01.07.96
    double                      u1, v1, u2, v2;
    occ::handle<IntPatch_GLine> GLine(occ::down_cast<IntPatch_GLine>(L));
    slin.Clear();
    nbvtx = GLine->NbVertex();
    //-- -------------------------------------------------------------------
    int* TabIndex = new int[nbvtx + 2];
    int  numline  = 0;
    //    for(int i=1;i<=nbvtx;i++) {
    for (i = 1; i <= nbvtx; i++)
    {
      TabIndex[i] = 0;
    }
    //-- -------------------------------------------------------------------
    bool intrvtested = false;
    for (i = 1; i < nbvtx; i++)
    {
      firstp = GLine->Vertex(i).ParameterOnLine();
      lastp  = GLine->Vertex(i + 1).ParameterOnLine();
      if (std::abs(firstp - lastp) > Precision::PConfusion())
      {
        intrvtested = true;
        double pmid = (firstp + lastp) * 0.5;
        gp_Pnt Pmid;
        if (typl == IntPatch_Lin)
        {
          Pmid = ElCLib::Value(pmid, GLine->Line());
        }
        else if (typl == IntPatch_Circle)
        {
          Pmid = ElCLib::Value(pmid, GLine->Circle());
        }
        else if (typl == IntPatch_Ellipse)
        {
          Pmid = ElCLib::Value(pmid, GLine->Ellipse());
        }
        else if (typl == IntPatch_Hyperbola)
        {
          Pmid = ElCLib::Value(pmid, GLine->Hyperbola());
        }
        else if (typl == IntPatch_Parabola)
        {
          Pmid = ElCLib::Value(pmid, GLine->Parabola());
        }
        Parameters(mySurf1, mySurf2, Pmid, u1, v1, u2, v2);
        Recadre(mySurf1, mySurf2, u1, v1, u2, v2);

        gp_Vec Du, Dv;
        gp_Pnt P;
        myDom1->Init();
        if (myDom2->More())
        {
          mySurf1->D1(u1, v1, P, Du, Dv);
          Tol = ComputeParametricTolerance(myDom1->Tol3d(myDom1->Value()), Du, Dv);
        }
        TopAbs_State in1 = myDom1->Classify(gp_Pnt2d(u1, v1), Tol, false);

        myDom2->Init();
        if (in1 != TopAbs_OUT && myDom2->More())
        {
          mySurf2->D1(u2, v2, P, Du, Dv);
          Tol = ComputeParametricTolerance(myDom2->Tol3d(myDom2->Value()), Du, Dv);
        }
        TopAbs_State in2 =
          (in1 != TopAbs_OUT) ? myDom2->Classify(gp_Pnt2d(u2, v2), Tol, false) : TopAbs_OUT;
        // modified by NIZHNY-OFV  Wed May 30 17:04:08 2001.BEGIN
        // --purpose: section algo with infinite prism works now!!!
        if (in1 == TopAbs_UNKNOWN)
          in1 = TopAbs_OUT;
        if (in2 == TopAbs_UNKNOWN)
          in2 = TopAbs_OUT;
        // modified by NIZHNY-OFV  Wed May 30 17:05:47 2001.END
        if (in1 == TopAbs_OUT || in2 == TopAbs_OUT)
        {
        }
        else
        {
          //-- std::cout<<"GLine      : firtsp="<<firstp<<" lastp="<<lastp<<"
          // Vtx:"<<i<<","<<i+1<<std::endl;
          TabIndex[i] = TabIndex[i + 1] = ++numline;
          AddLine(L, i, i + 1, typs1, typs2, TabIndex, slin);
        }
      }
    }
    if (typl == IntPatch_Circle || typl == IntPatch_Ellipse)
    {
      firstp         = GLine->Vertex(nbvtx).ParameterOnLine();
      lastp          = M_PI + M_PI + GLine->Vertex(1).ParameterOnLine();
      double cadrinf = LocalFirstParameter(L);
      double cadrsup = LocalLastParameter(L);
      double acadr   = (firstp + lastp) * 0.5;
      while (acadr < cadrinf)
      {
        acadr += M_PI + M_PI;
      }
      while (acadr > cadrsup)
      {
        acadr -= M_PI + M_PI;
      }
      if (acadr >= cadrinf && acadr <= cadrsup)
      {
        if (std::abs(firstp - lastp) > Precision::PConfusion())
        {
          intrvtested = true;
          double pmid = (firstp + lastp) * 0.5;
          gp_Pnt Pmid;
          if (typl == IntPatch_Circle)
          {
            Pmid = ElCLib::Value(pmid, GLine->Circle());
          }
          else
          {
            Pmid = ElCLib::Value(pmid, GLine->Ellipse());
          }
          Parameters(mySurf1, mySurf2, Pmid, u1, v1, u2, v2);
          Recadre(mySurf1, mySurf2, u1, v1, u2, v2);
          TopAbs_State in1 = myDom1->Classify(gp_Pnt2d(u1, v1), Tol, false);
          TopAbs_State in2 =
            (in1 != TopAbs_OUT) ? myDom2->Classify(gp_Pnt2d(u2, v2), Tol, false) : TopAbs_OUT;
          // modified by NIZHNY-OFV  Wed May 30 17:04:08 2001.BEGIN
          // --purpose: section algo with infinite prism works now!!!
          if (in1 == TopAbs_UNKNOWN)
            in1 = TopAbs_OUT;
          if (in2 == TopAbs_UNKNOWN)
            in2 = TopAbs_OUT;
          // modified by NIZHNY-OFV  Wed May 30 17:05:47 2001.END
          if (in1 == TopAbs_OUT || in2 == TopAbs_OUT)
          {
          }
          else
          {
            //-- std::cout<<"GLine  bis : firtsp="<<firstp<<" lastp="<<lastp<<"
            // Vtx:"<<i<<","<<i+1<<std::endl;
            TabIndex[nbvtx] = TabIndex[1] = ++numline;
            AddLine(L, nbvtx, 1, typs1, typs2, TabIndex, slin);
          }
        }
      }
    }
    if (!intrvtested)
    {
      // on garde a priori. Il faudrait un point 2d sur chaque
      // surface pour prendre la decision. Sera fait dans
      // l`appelant
      // if(nbvtx) {
      //	TabIndex[nbvtx]=TabIndex[1]=++numline;
      //	AddLine(L,1,nbvtx,typs1,typs2,TabIndex,slin);
      //}
    }
    //-- -------------------------------------------------------------------
    //-- On recherche les vertex interference Edge Edge Externe
    //-- Ces vertex ne figurent sur aucune ligne et sont Restriction
    //-- sur les 2 edges
    for (i = 1; i <= nbvtx; i++)
    {
      if (TabIndex[i] == 0)
      {
        const IntPatch_Point& GLine_Vertex_i = GLine->Vertex(i);
        if (GLine_Vertex_i.IsOnDomS1() && GLine_Vertex_i.IsOnDomS2())
        {
          TabIndex[i] = ++numline;
          AddLine(L, i, i, typs1, typs2, TabIndex, slin);
        }
      }
    }
    delete[] TabIndex;
    //-- -------------------------------------------------------------------
    return;
  }
  else
  { //-- Restriction
    occ::handle<IntPatch_RLine> RLine(occ::down_cast<IntPatch_RLine>(L));
    slin.Clear();
    int  NbVtx    = RLine->NbVertex();
    bool RestOnS1 = RLine->IsArcOnS1();
    bool RestOnS2 = RLine->IsArcOnS2();
    //-- -------------------------------------------------------------------
    int* TabIndex = new int[NbVtx + 2];
    // int numline=0;
    for (i = 1; i <= NbVtx; i++)
    {
      TabIndex[i] = 0;
    }
    //-- -------------------------------------------------------------------
    for (i = 1; i < NbVtx; i++)
    {
      const IntPatch_Point& Vtx1 = RLine->Vertex(i);
      const IntPatch_Point& Vtx2 = RLine->Vertex(i + 1);
      if (RestOnS1 && RestOnS2)
      {
        AddLine(L, i, i + 1, typs1, typs2, TabIndex, slin);
      }
      else if (RestOnS1)
      { //-- On na classifie pas sur 1
        double u0 = Vtx1.ParameterOnLine();
        double u1 = Vtx2.ParameterOnLine();
        if (std::abs(u1 - u0) > Precision::PConfusion())
        {
          double u = (999.0 * u0 + u1) * 0.001;

          gp_Pnt   P0   = Vtx1.Value();
          gp_Pnt2d Px2d = RLine->ArcOnS1()->Value(u);
          gp_Pnt   Px   = mySurf1->Value(Px2d.X(), Px2d.Y());
          gp_Vec   P0Px = gp_Vec(P0, Px);

          double U1, V1, U2, V2;
          Vtx1.PntOn2S().Parameters(U1, V1, U2, V2);

          gp_Vec D1u, D1v;
          gp_Pnt P;
          mySurf2->D1(U2, V2, P, D1u, D1v);
          myDom2->Init();
          if (myDom2->More())
            Tol = ComputeParametricTolerance(myDom2->Tol3d(myDom2->Value()), D1u, D1v);

          //-- le 23 mars 1999
          TopAbs_State bornin = myDom2->Classify(gp_Pnt2d(U2, V2), Tol, false);
          if (bornin != TopAbs_OUT)
          {
            double U1t, V1t, U2t, V2t;
            Vtx2.PntOn2S().Parameters(U1t, V1t, U2t, V2t);
            bornin = myDom2->Classify(gp_Pnt2d(U2t, V2t), Tol, false);
          }
          if (bornin == TopAbs_OUT)
            continue;

          //-- Attention , on faisait  une estimatoin de deltau et deltav
          //-- Maintenant :
          //-- POPx . D1u = deltau * D1u.D1u  + deltav * D1u.D1v
          //-- POPx . D1v = deltau * D1u.D1v  + deltav * D1v.D1v
          //--
          //-- deltau=
          double D1uD1v, TgD1u, TgD1v, D1uD1u, D1vD1v, DIS;
          // double DeltaU,DeltaV;
          D1uD1u = D1u.Dot(D1u);
          D1vD1v = D1v.Dot(D1v);
          D1uD1v = D1u.Dot(D1v);
          TgD1u  = P0Px.Dot(D1u);
          TgD1v  = P0Px.Dot(D1v);
          DIS    = D1uD1u * D1vD1v - D1uD1v * D1uD1v;

          double deltau = 1e-10;
          double deltav = 1e-10;
          if (DIS < -1e-10 || DIS > 1e-10)
          {
            deltau = (TgD1u * D1vD1v - TgD1v * D1uD1v) / DIS;
            deltav = (TgD1v * D1uD1u - TgD1u * D1uD1v) / DIS;
          }

          U2 += deltau;
          V2 += deltav;
          if (bornin != TopAbs_OUT)
          {
            TopAbs_State in2 = myDom2->Classify(gp_Pnt2d(U2, V2), Tol, false);
            deltau *= 0.05;
            deltav *= 0.05;
            if (in2 == TopAbs_OUT)
            {
              in2 = myDom2->Classify(gp_Pnt2d(U2 + deltau, V2), Tol, false);
            }
            if (in2 == TopAbs_OUT)
            {
              in2 = myDom2->Classify(gp_Pnt2d(U2 - deltau, V2), Tol, false);
            }
            if (in2 == TopAbs_OUT)
            {
              in2 = myDom2->Classify(gp_Pnt2d(U2, V2 + deltav), Tol, false);
            }
            if (in2 == TopAbs_OUT)
            {
              in2 = myDom2->Classify(gp_Pnt2d(U2, V2 - deltav), Tol, false);
            }

            if (in2 != TopAbs_OUT)
            {
              //-- std::cout<<"RLine ons1 : u0    ="<<u0<<" u1   ="<<u1<<"
              // Vtx:"<<i<<","<<i+1<<std::endl;
              AddLine(L, i, i + 1, typs1, typs2, TabIndex, slin);
            }
          }
        }
      }
      else
      {
        double u0 = Vtx1.ParameterOnLine();
        double u1 = Vtx2.ParameterOnLine();
        if (std::abs(u1 - u0) > Precision::PConfusion())
        {
          double u = (999.0 * u0 + u1) * 0.001;

          gp_Pnt   P0   = Vtx1.Value();
          gp_Pnt2d Px2d = RLine->ArcOnS2()->Value(u);
          gp_Pnt   Px   = mySurf2->Value(Px2d.X(), Px2d.Y());
          gp_Vec   P0Px = gp_Vec(P0, Px);

          double U1, V1, U2, V2;
          Vtx1.PntOn2S().Parameters(U1, V1, U2, V2);

          gp_Vec D1u, D1v;
          gp_Pnt P;
          mySurf1->D1(U1, V1, P, D1u, D1v);
          myDom1->Init();
          if (myDom2->More())
            Tol = ComputeParametricTolerance(myDom1->Tol3d(myDom1->Value()), D1u, D1v);

          //-- le 23 mars 1999
          TopAbs_State bornin = myDom1->Classify(gp_Pnt2d(U1, V1), Tol, false);
          if (bornin != TopAbs_OUT)
          {
            double U1t, V1t, U2t, V2t;
            Vtx2.PntOn2S().Parameters(U1t, V1t, U2t, V2t);
            bornin = myDom1->Classify(gp_Pnt2d(U1t, V1t), Tol, false);
          }
          if (bornin == TopAbs_OUT)
            continue;

          //-- Attention , on faisait  une estimatoin de deltau et deltav
          //-- Maintenant :
          //-- POPx . D1u = deltau * D1u.D1u  + deltav * D1u.D1v
          //-- POPx . D1v = deltau * D1u.D1v  + deltav * D1v.D1v
          //--
          //-- deltau=
          double D1uD1v, TgD1u, TgD1v, D1uD1u, D1vD1v, DIS;
          // double DeltaU,DeltaV;
          D1uD1u = D1u.Dot(D1u);
          D1vD1v = D1v.Dot(D1v);
          D1uD1v = D1u.Dot(D1v);
          TgD1u  = P0Px.Dot(D1u);
          TgD1v  = P0Px.Dot(D1v);
          DIS    = D1uD1u * D1vD1v - D1uD1v * D1uD1v;

          double deltau = 1e-10;
          double deltav = 1e-10;
          if (DIS < -1e-10 || DIS > 1e-10)
          {
            deltau = (TgD1u * D1vD1v - TgD1v * D1uD1v) / DIS;
            deltav = (TgD1v * D1uD1u - TgD1u * D1uD1v) / DIS;
          }

          U1 += deltau;
          V1 += deltav;

          if (bornin != TopAbs_OUT)
          {
            TopAbs_State in2 = myDom1->Classify(gp_Pnt2d(U1, V1), Tol, false);
            deltau *= 0.05;
            deltav *= 0.05;
            if (in2 == TopAbs_OUT)
            {
              in2 = myDom1->Classify(gp_Pnt2d(U1 + deltau, V1), Tol, false);
            }
            if (in2 == TopAbs_OUT)
            {
              in2 = myDom1->Classify(gp_Pnt2d(U1 - deltau, V1), Tol, false);
            }
            if (in2 == TopAbs_OUT)
            {
              in2 = myDom1->Classify(gp_Pnt2d(U1, V1 + deltav), Tol, false);
            }
            if (in2 == TopAbs_OUT)
            {
              in2 = myDom1->Classify(gp_Pnt2d(U1, V1 - deltav), Tol, false);
            }

            if (in2 != TopAbs_OUT)
            {
              //-- std::cout<<"RLine ons2 : u0    ="<<u0<<" u1   ="<<u1<<"
              // Vtx:"<<i<<","<<i+1<<std::endl;

              AddLine(L, i, i + 1, typs1, typs2, TabIndex, slin);
            }
          }
        }
      }
    }
    delete[] TabIndex;
  }
}
