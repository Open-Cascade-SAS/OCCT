// Created on: 1993-02-05
// Created by: Jacques GOUSSARD
// Copyright (c) 1993-1999 Matra Datavision
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

#include <Adaptor3d_Surface.hxx>
#include <Adaptor3d_HSurfaceTool.hxx>
#include <Adaptor3d_TopolTool.hxx>
#include <BndLib_AddSurface.hxx>
#include <Contap_ContAna.hxx>
#include <Contap_Contour.hxx>
#include <Contap_HContTool.hxx>
#include <Contap_HCurve2dTool.hxx>
#include <Contap_Line.hxx>
#include <Contap_SurfFunction.hxx>
#include <Contap_SurfProps.hxx>
#include <Contap_TheIWalking.hxx>
#include <Contap_ThePathPointOfTheSearch.hxx>
#include <Contap_TheSegmentOfTheSearch.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <IntSurf.hxx>
#include <IntSurf_InteriorPoint.hxx>
#include <IntSurf_PathPoint.hxx>
#include <NCollection_Sequence.hxx>
#include <math_FunctionSetRoot.hxx>
#include <Standard_ConstructionError.hxx>
#include <StdFail_NotDone.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <TopTrans_CurveTransition.hxx>

static const double Tolpetit = 1.e-10; // for squared distance

static const double tole = 5.e-6;

Contap_Contour::Contap_Contour()
    : done(false),
      modeset(false)
{
}

Contap_Contour::Contap_Contour(const gp_Vec& Direction)
    :

      done(false),
      modeset(true)
{
  mySFunc.Set(Direction);
  myAFunc.Set(Direction);
}

Contap_Contour::Contap_Contour(const gp_Vec& Direction, const double Angle)
    :

      done(false),
      modeset(true)
{
  mySFunc.Set(Direction, Angle);
  myAFunc.Set(Direction, Angle);
}

Contap_Contour::Contap_Contour(const gp_Pnt& Eye)
    :

      done(false),
      modeset(true)
{
  mySFunc.Set(Eye);
  myAFunc.Set(Eye);
}

Contap_Contour::Contap_Contour(const occ::handle<Adaptor3d_Surface>&   Surf,
                               const occ::handle<Adaptor3d_TopolTool>& Domain,
                               const gp_Vec&                           Direction)
    :

      done(false),
      modeset(true)
{
  Perform(Surf, Domain, Direction);
}

Contap_Contour::Contap_Contour(const occ::handle<Adaptor3d_Surface>&   Surf,
                               const occ::handle<Adaptor3d_TopolTool>& Domain,
                               const gp_Vec&                           Direction,
                               const double                            Angle)
    :

      done(false),
      modeset(true)
{
  Perform(Surf, Domain, Direction, Angle);
}

Contap_Contour::Contap_Contour(const occ::handle<Adaptor3d_Surface>&   Surf,
                               const occ::handle<Adaptor3d_TopolTool>& Domain,
                               const gp_Pnt&                           Eye)
    :

      done(false),
      modeset(true)
{
  Perform(Surf, Domain, Eye);
}

void Contap_Contour::Init(const gp_Vec& Direction)

{
  done    = false;
  modeset = true;
  mySFunc.Set(Direction);
  myAFunc.Set(Direction);
}

void Contap_Contour::Init(const gp_Vec& Direction, const double Angle)
{
  done    = false;
  modeset = true;
  mySFunc.Set(Direction, Angle);
  myAFunc.Set(Direction, Angle);
}

void Contap_Contour::Init(const gp_Pnt& Eye)
{
  done    = false;
  modeset = true;
  mySFunc.Set(Eye);
  myAFunc.Set(Eye);
}

void Contap_Contour::Perform(const occ::handle<Adaptor3d_Surface>&   Surf,
                             const occ::handle<Adaptor3d_TopolTool>& Domain)
{
  if (!modeset)
  {
    throw Standard_ConstructionError();
  }
  mySFunc.Set(Surf);
  myAFunc.Set(Surf);

  GeomAbs_SurfaceType typS = Adaptor3d_HSurfaceTool::GetType(Surf);
  switch (typS)
  {
    case GeomAbs_Plane:
    case GeomAbs_Sphere:
    case GeomAbs_Cylinder:
    case GeomAbs_Cone: {
      PerformAna(Domain); // Surf,Domain,Direction,0.,gp_Pnt(0.,0.,0.),1);
    }
    break;

    default: {
      Perform(Domain); // Surf,Domain,Direction,0.,gp_Pnt(0.,0.,0.),1);
    }
    break;
  }
}

void Contap_Contour::Perform(const occ::handle<Adaptor3d_Surface>&   Surf,
                             const occ::handle<Adaptor3d_TopolTool>& Domain,
                             const gp_Vec&                           Direction)

{
  Init(Direction);
  Perform(Surf, Domain);
}

void Contap_Contour::Perform(const occ::handle<Adaptor3d_Surface>&   Surf,
                             const occ::handle<Adaptor3d_TopolTool>& Domain,
                             const gp_Vec&                           Direction,
                             const double                            Angle)

{
  Init(Direction, Angle);
  Perform(Surf, Domain);
}

void Contap_Contour::Perform(const occ::handle<Adaptor3d_Surface>&   Surf,
                             const occ::handle<Adaptor3d_TopolTool>& Domain,
                             const gp_Pnt&                           Eye)

{
  Init(Eye);
  Perform(Surf, Domain);
}

static IntSurf_TypeTrans ComputeTransitionOnLine(Contap_SurfFunction&,
                                                 const double,
                                                 const double,
                                                 const gp_Vec&);

static IntSurf_TypeTrans ComputeTransitionOngpCircle(Contap_SurfFunction&, const gp_Circ&);

static IntSurf_TypeTrans ComputeTransitionOngpLine(Contap_SurfFunction&, const gp_Lin&);

static void ComputeInternalPoints(Contap_Line& Line,
                                  Contap_SurfFunction&,
                                  const double ureso,
                                  const double vreso);

static void ComputeInternalPointsOnRstr(Contap_Line&,
                                        const double,
                                        const double,
                                        Contap_SurfFunction&);

static void ProcessSegments(const Contap_TheSearch&,
                            NCollection_Sequence<Contap_Line>&,
                            const double,
                            Contap_SurfFunction&,
                            const occ::handle<Adaptor3d_TopolTool>&);

//-- --------------------------------------------------------------------------------
//-- Recherche des portions utiles sur les lignes

static void Recadre(const occ::handle<Adaptor3d_Surface>& myHS1, double& u1, double& v1)
{
  double              f, l, lmf;
  GeomAbs_SurfaceType typs1 = myHS1->GetType();

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
      myHS1IsUPeriodic = myHS1IsVPeriodic = false;
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
}

static void LineConstructor(NCollection_Sequence<Contap_Line>&      slin,
                            const occ::handle<Adaptor3d_TopolTool>& Domain,
                            Contap_Line&                            L,
                            const occ::handle<Adaptor3d_Surface>&   Surf)
{

  //-- ------------------------------------------------------------
  //-- on decoupe la ligne en portions  entre 2 vertex
  constexpr double Tol  = Precision::PConfusion();
  Contap_IType     typl = L.TypeContour();
  //-- std::cout<<"\n ----------- Ligne Constructor "<<std::endl;
  if (typl == Contap_Walking)
  {
    double u1, v1, u2, v2;
    int    nbvtx = L.NbVertex();
    //-- std::cout<<" WLine -> "<<nbvtx<<" vtx"<<std::endl;
    for (int i = 1; i < nbvtx; i++)
    {
      int firstp = (int)L.Vertex(i).ParameterOnLine();
      int lastp  = (int)L.Vertex(i + 1).ParameterOnLine();
      if (firstp != lastp)
      {
        int                    pmid = (firstp + lastp) / 2; //-- entiers
        const IntSurf_PntOn2S& Pmid = L.Point(pmid);
        Pmid.Parameters(u1, v1, u2, v2);
        Recadre(Surf, u2, v2);
        TopAbs_State in2 = Domain->Classify(gp_Pnt2d(u2, v2), Tol);
        if (in2 == TopAbs_OUT)
        {
        }
        else
        {
          //-- std::cout<<"ContapWLine      : firtsp="<<firstp<<" lastp="<<lastp<<"
          // Vtx:"<<i<<","<<i+1<<std::endl;
          occ::handle<IntSurf_LineOn2S> LineOn2S = new IntSurf_LineOn2S();
          LineOn2S->Add(L.Point(firstp));
          for (int j = firstp + 1; j <= lastp; j++)
          {
            double aSqDist = L.Point(j).Value().SquareDistance(L.Point(j - 1).Value());
            if (aSqDist > gp::Resolution())
              LineOn2S->Add(L.Point(j));
          }
          if (LineOn2S->NbPoints() < 2)
            continue;
          Contap_Line Line;
          Line.SetLineOn2S(LineOn2S);
          Contap_Point pvtx = L.Vertex(i);
          pvtx.SetParameter(1);
          Line.Add(pvtx);

          pvtx = L.Vertex(i + 1);
          pvtx.SetParameter(LineOn2S->NbPoints());
          Line.Add(pvtx);
          Line.SetTransitionOnS(L.TransitionOnS());
          slin.Append(Line);
        }
      }
    }
  }
  else if (typl == Contap_Lin)
  {
    double u2, v2; // u1,v1;
    int    nbvtx = L.NbVertex();
    //-- std::cout<<" Lin -> "<<nbvtx<<" vtx"<<std::endl;
    for (int i = 1; i < nbvtx; i++)
    {
      double firstp = L.Vertex(i).ParameterOnLine();
      double lastp  = L.Vertex(i + 1).ParameterOnLine();
      if (firstp != lastp)
      {
        double pmid = (firstp + lastp) * 0.5;
        gp_Pnt Pmid = ElCLib::Value(pmid, L.Line());
        if (Adaptor3d_HSurfaceTool::GetType(Surf) == GeomAbs_Cylinder)
        {
          ElSLib::Parameters(Adaptor3d_HSurfaceTool::Cylinder(Surf), Pmid, u2, v2);
        }
        else if (Adaptor3d_HSurfaceTool::GetType(Surf) == GeomAbs_Cone)
        {
          ElSLib::Parameters(Adaptor3d_HSurfaceTool::Cone(Surf), Pmid, u2, v2);
        }
        else
        {
          //-- std::cout<<" Pb ds Contap_ContourGen_2.gxx (type)"<<std::endl;
        }

        Recadre(Surf, u2, v2);
        TopAbs_State in2 = Domain->Classify(gp_Pnt2d(u2, v2), Tol);
        if (in2 == TopAbs_OUT)
        {
        }
        else
        {
          //-- std::cout<<"Contap Lin      : firtsp="<<firstp<<" lastp="<<lastp<<"
          // Vtx:"<<i<<","<<i+1<<std::endl;
          Contap_Line Line;
          Line.SetValue(L.Line());
          Contap_Point pvtx = L.Vertex(i);
          Line.Add(pvtx);

          pvtx = L.Vertex(i + 1);
          Line.Add(pvtx);
          Line.SetTransitionOnS(L.TransitionOnS());
          slin.Append(Line);
        }
      }
    }
  }
  else if (typl == Contap_Circle)
  {
    double u2, v2; // u1,v1,
    int    nbvtx = L.NbVertex();
    //-- std::cout<<" Circ -> "<<nbvtx<<" vtx"<<std::endl;
    bool novtx = true;
    if (nbvtx)
      novtx = false;
    for (int i = 1; i < nbvtx || novtx; i++)
    {
      double firstp = 0, lastp = M_PI + M_PI;
      if (novtx == false)
      {
        firstp = L.Vertex(i).ParameterOnLine();
        lastp  = L.Vertex(i + 1).ParameterOnLine();
      }
      if (std::abs(firstp - lastp) > 0.000000001)
      {
        double pmid = (firstp + lastp) * 0.5;
        gp_Pnt Pmid = ElCLib::Value(pmid, L.Circle());
        if (Adaptor3d_HSurfaceTool::GetType(Surf) == GeomAbs_Cylinder)
        {
          ElSLib::Parameters(Adaptor3d_HSurfaceTool::Cylinder(Surf), Pmid, u2, v2);
        }
        else if (Adaptor3d_HSurfaceTool::GetType(Surf) == GeomAbs_Cone)
        {
          ElSLib::Parameters(Adaptor3d_HSurfaceTool::Cone(Surf), Pmid, u2, v2);
        }
        else if (Adaptor3d_HSurfaceTool::GetType(Surf) == GeomAbs_Sphere)
        {
          ElSLib::Parameters(Adaptor3d_HSurfaceTool::Sphere(Surf), Pmid, u2, v2);
        }
        else
        {
          //-- std::cout<<" Pb ds Contap_ContourGen_2.gxx (typep)"<<std::endl;
        }

        Recadre(Surf, u2, v2);
        TopAbs_State in2 = Domain->Classify(gp_Pnt2d(u2, v2), Tol);
        if (in2 == TopAbs_OUT)
        {
        }
        else
        {
          //-- std::cout<<"Contap Circle     : firtsp="<<firstp<<" lastp="<<lastp<<"
          // Vtx:"<<i<<","<<i+1<<std::endl;
          Contap_Line Line;
          Line.SetValue(L.Circle());
          if (novtx == false)
          {
            Contap_Point pvtx = L.Vertex(i);
            Line.Add(pvtx);
            pvtx = L.Vertex(i + 1);
            Line.Add(pvtx);
          }
          Line.SetTransitionOnS(L.TransitionOnS());
          slin.Append(Line);
        }
      }
      novtx = false;
    }
    if (nbvtx)
    {
      double firstp = L.Vertex(nbvtx).ParameterOnLine();
      double lastp  = L.Vertex(1).ParameterOnLine() + M_PI + M_PI;
      if (std::abs(firstp - lastp) > 0.0000000001)
      {
        double pmid = (firstp + lastp) * 0.5;
        gp_Pnt Pmid = ElCLib::Value(pmid, L.Circle());
        if (Adaptor3d_HSurfaceTool::GetType(Surf) == GeomAbs_Cylinder)
        {
          ElSLib::Parameters(Adaptor3d_HSurfaceTool::Cylinder(Surf), Pmid, u2, v2);
        }
        else if (Adaptor3d_HSurfaceTool::GetType(Surf) == GeomAbs_Cone)
        {
          ElSLib::Parameters(Adaptor3d_HSurfaceTool::Cone(Surf), Pmid, u2, v2);
        }
        else if (Adaptor3d_HSurfaceTool::GetType(Surf) == GeomAbs_Sphere)
        {
          ElSLib::Parameters(Adaptor3d_HSurfaceTool::Sphere(Surf), Pmid, u2, v2);
        }
        else
        {
          //-- std::cout<<" Pb ds Contap_ContourGen_2.gxx (typep)"<<std::endl;
        }

        Recadre(Surf, u2, v2);
        TopAbs_State in2 = Domain->Classify(gp_Pnt2d(u2, v2), Tol);
        if (in2 == TopAbs_OUT)
        {
        }
        else
        {
          //-- std::cout<<"Contap Circle  *Compl* : firtsp="<<firstp<<" lastp="<<lastp<<"
          // Vtx:"<<i<<","<<i+1<<std::endl;
          Contap_Line Line;
          Line.SetValue(L.Circle());
          Contap_Point pvtx = L.Vertex(nbvtx);
          Line.Add(pvtx);

          pvtx = L.Vertex(1);
          pvtx.SetParameter(pvtx.ParameterOnLine() + M_PI + M_PI);
          Line.Add(pvtx);
          Line.SetTransitionOnS(L.TransitionOnS());
          slin.Append(Line);
        }
      }
    }
  }
  else
  {
    //-- std::cout<<" ni WLine ni Lin ni Circ "<<std::endl;
    slin.Append(L);
  }
  //--
}

//-- --------------------------------------------------------------------------------

static void KeepInsidePoints(const Contap_TheSearchInside&                solins,
                             const Contap_TheSearch&                      solrst,
                             Contap_SurfFunction&                         Func,
                             NCollection_Sequence<IntSurf_InteriorPoint>& seqpins)

{
  int                                   Nba = solrst.NbSegments();
  int                                   Nbp, indp, inda;
  double                                U, V, paramproj;
  gp_Pnt2d                              toproj, Ptproj;
  bool                                  projok, tokeep;
  const occ::handle<Adaptor3d_Surface>& Surf = Func.Surface();

  Nbp = solins.NbPoints();
  for (indp = 1; indp <= Nbp; indp++)
  {
    tokeep                           = true;
    const IntSurf_InteriorPoint& pti = solins.Value(indp);
    pti.Parameters(U, V);
    toproj = gp_Pnt2d(U, V);
    for (inda = 1; inda <= Nba; inda++)
    {
      const occ::handle<Adaptor2d_Curve2d>& thearc = solrst.Segment(inda).Curve();
      projok = Contap_HContTool::Project(thearc, toproj, paramproj, Ptproj);
      if (projok)
      {
        gp_Pnt pprojete = Adaptor3d_HSurfaceTool::Value(Surf, Ptproj.X(), Ptproj.Y());
        if (pti.Value().Distance(pprojete) <= Precision::Confusion())
        {
          tokeep = false;
          break;
        }
      }
    }
    if (tokeep)
    {
      seqpins.Append(pti);
    }
  }
}

static void ComputeTangency(const Contap_TheSearch&                  solrst,
                            const occ::handle<Adaptor3d_TopolTool>&  Domain,
                            Contap_SurfFunction&                     Func,
                            NCollection_Sequence<IntSurf_PathPoint>& seqpdep,
                            NCollection_Array1<int>&                 Destination)
{

  int i, k;
  int NbPoints  = solrst.NbPoints();
  int seqlength = 0;

  double             theparam, test;
  bool               fairpt;
  TopAbs_Orientation arcorien, vtxorien;
  bool               ispassing;

  math_Vector X(1, 2);
  math_Vector F(1, 1);
  math_Matrix D(1, 1, 1, 2);

  gp_Vec   normale, vectg, tg3drst, v1, v2;
  gp_Dir2d dirtg;
  gp_Vec2d tg2drst;
  gp_Pnt2d pt2d;

  IntSurf_PathPoint                     PPoint;
  const occ::handle<Adaptor3d_Surface>& Surf = Func.Surface();

  for (i = 1; i <= NbPoints; i++)
  {

    if (Destination(i) == 0)
    {

      const Contap_ThePathPointOfTheSearch& PStart = solrst.Point(i);
      const occ::handle<Adaptor2d_Curve2d>& thearc = PStart.Arc();
      theparam                                     = PStart.Parameter();
      gp_Pnt2d Ptoproj                             = Contap_HCurve2dTool::Value(thearc, theparam);
      //-- lbr le 15 mai 97
      //-- On elimine les points qui sont egalement present sur une restriction solution
      bool SurUneRestrictionSolution = false;
      for (int restriction = 1;
           SurUneRestrictionSolution == false && restriction <= solrst.NbSegments();
           restriction++)
      {
        const occ::handle<Adaptor2d_Curve2d>& thearcsol = solrst.Segment(restriction).Curve();
        double                                paramproj;
        gp_Pnt2d                              pproj;
        bool projok = Contap_HContTool::Project(thearcsol, Ptoproj, paramproj, pproj);
        if (projok)
        {
          // gp_Pnt pprojete = Adaptor3d_HSurfaceTool::Value(Surf,Ptoproj.X(),Ptoproj.Y());
          // IFV - begin
          gp_Pnt pprojete = Adaptor3d_HSurfaceTool::Value(Surf, pproj.X(), pproj.Y());
          // IFV - end
          if ((PStart.Value()).Distance(pprojete) <= Precision::Confusion())
          {
            SurUneRestrictionSolution = true;
          }
        }
      }
      if (SurUneRestrictionSolution == false)
      {
        arcorien  = Domain->Orientation(thearc);
        ispassing = (arcorien == TopAbs_INTERNAL || arcorien == TopAbs_EXTERNAL);

        Contap_HCurve2dTool::D1(thearc, theparam, pt2d, tg2drst);
        X(1) = pt2d.X();
        X(2) = pt2d.Y();
        PPoint.SetValue(PStart.Value(), X(1), X(2));

        Func.Values(X, F, D);
        if (Func.IsTangent())
        {
          PPoint.SetTangency(true);
          Destination(i) = seqlength + 1;
          if (!PStart.IsNew())
          {
            const occ::handle<Adaptor3d_HVertex>& vtx = PStart.Vertex();
            for (k = i + 1; k <= NbPoints; k++)
            {
              if (Destination(k) == 0)
              {
                const Contap_ThePathPointOfTheSearch& PStart2 = solrst.Point(k);
                if (!PStart2.IsNew())
                {
                  const occ::handle<Adaptor3d_HVertex>& vtx2 = PStart2.Vertex();
                  if (Domain->Identical(vtx, vtx2))
                  {
                    const occ::handle<Adaptor2d_Curve2d>& thearc2 = PStart2.Arc();
                    theparam                                      = PStart2.Parameter();
                    arcorien                                      = Domain->Orientation(thearc2);
                    ispassing =
                      ispassing && (arcorien == TopAbs_INTERNAL || arcorien == TopAbs_EXTERNAL);

                    pt2d = Contap_HCurve2dTool::Value(thearc2, theparam);
                    X(1) = pt2d.X();
                    X(2) = pt2d.Y();
                    PPoint.AddUV(X(1), X(2));
                    Destination(k) = seqlength + 1;
                  }
                }
              }
            }
          }
          PPoint.SetPassing(ispassing);
          seqpdep.Append(PPoint);
          seqlength++;
        }
        else
        { // on a un point de depart potentiel

          vectg = Func.Direction3d();
          dirtg = Func.Direction2d();

          gp_Pnt ptbid;
          //	Adaptor3d_HSurfaceTool::D1(Surf,X(1),X(2),ptbid,v1,v2);
          Contap_SurfProps::DerivAndNorm(Surf, X(1), X(2), ptbid, v1, v2, normale);
          tg3drst = tg2drst.X() * v1 + tg2drst.Y() * v2;
          //	normale = v1.Crossed(v2);
          if (normale.SquareMagnitude() < RealEpsilon())
          {
            //-- std::cout<<"\n*** Contap_ContourGen_2.gxx  Normale Nulle en U:"<<X(1)<<"
            // V:"<<X(2)<<std::endl;
          }
          else
          {
            test = vectg.Dot(normale.Crossed(tg3drst));

            if (PStart.IsNew())
            {
              double tbis = vectg.Normalized().Dot(tg3drst.Normalized());
              if (std::abs(tbis) < 1. - tole)
              {

                if ((test < 0. && arcorien == TopAbs_FORWARD)
                    || (test > 0. && arcorien == TopAbs_REVERSED))
                {
                  vectg.Reverse();
                  dirtg.Reverse();
                }
                PPoint.SetDirections(vectg, dirtg);
              }
              else
              { // on garde le point comme point d`arret (tangent)
                PPoint.SetTangency(true);
              }
              PPoint.SetPassing(ispassing);
              Destination(i) = seqlength + 1;
              seqpdep.Append(PPoint);
              seqlength++;
            }
            else
            { // traiter la transition complexe
              gp_Dir bidnorm(1., 1., 1.);

              bool                     tobeverified = false;
              TopAbs_Orientation       LocTrans;
              TopTrans_CurveTransition comptrans;
              comptrans.Reset(vectg, bidnorm, 0.);
              if (arcorien != TopAbs_INTERNAL && arcorien != TopAbs_EXTERNAL)
              {
                // pour essai
                const occ::handle<Adaptor3d_HVertex>& vtx = PStart.Vertex();
                vtxorien                                  = Domain->Orientation(vtx);
                test                                      = test / (vectg.Magnitude());
                test = test / ((normale.Crossed(tg3drst)).Magnitude());

                if (std::abs(test) <= tole)
                {
                  tobeverified = true;
                  LocTrans     = TopAbs_EXTERNAL; // et pourquoi pas INTERNAL
                }
                else
                {
                  if ((test > 0. && arcorien == TopAbs_FORWARD)
                      || (test < 0. && arcorien == TopAbs_REVERSED))
                  {
                    LocTrans = TopAbs_FORWARD;
                  }
                  else
                  {
                    LocTrans = TopAbs_REVERSED;
                  }
                  if (arcorien == TopAbs_REVERSED)
                  {
                    tg3drst.Reverse();
                  } // pas deja fait ???
                }

                comptrans.Compare(tole, tg3drst, bidnorm, 0., LocTrans, vtxorien);
              }
              Destination(i) = seqlength + 1;
              for (k = i + 1; k <= NbPoints; k++)
              {
                if (Destination(k) == 0)
                {
                  const Contap_ThePathPointOfTheSearch& PStart2 = solrst.Point(k);
                  if (!PStart2.IsNew())
                  {
                    const occ::handle<Adaptor3d_HVertex>& vtx2 = PStart2.Vertex();
                    if (Domain->Identical(PStart.Vertex(), vtx2))
                    {
                      const occ::handle<Adaptor2d_Curve2d>& thearc2 = PStart2.Arc();
                      theparam                                      = PStart2.Parameter();
                      arcorien                                      = Domain->Orientation(thearc2);

                      Contap_HCurve2dTool::D1(thearc2, theparam, pt2d, tg2drst);
                      X(1) = pt2d.X();
                      X(2) = pt2d.Y();
                      PPoint.AddUV(X(1), X(2));

                      if (arcorien != TopAbs_INTERNAL && arcorien != TopAbs_EXTERNAL)
                      {
                        ispassing = false;
                        tg3drst   = tg2drst.X() * v1 + tg2drst.Y() * v2;
                        test      = vectg.Dot(normale.Crossed(tg3drst));
                        test      = test / (vectg.Magnitude());
                        test      = test / ((normale.Crossed(tg3drst)).Magnitude());

                        vtxorien = Domain->Orientation(vtx2);
                        if (std::abs(test) <= tole)
                        {
                          tobeverified = true;
                          LocTrans     = TopAbs_EXTERNAL; // et pourquoi pas INTERNAL
                        }
                        else
                        {
                          if ((test > 0. && arcorien == TopAbs_FORWARD)
                              || (test < 0. && arcorien == TopAbs_REVERSED))
                          {
                            LocTrans = TopAbs_FORWARD;
                          }
                          else
                          {
                            LocTrans = TopAbs_REVERSED;
                          }
                          if (arcorien == TopAbs_REVERSED)
                          {
                            tg3drst.Reverse();
                          } // deja fait????
                        }

                        comptrans.Compare(tole, tg3drst, bidnorm, 0., LocTrans, vtxorien);
                      }
                      Destination(k) = seqlength + 1;
                    }
                  }
                }
              }
              fairpt = true;
              if (!ispassing)
              {
                TopAbs_State Before = comptrans.StateBefore();
                TopAbs_State After  = comptrans.StateAfter();
                if ((Before == TopAbs_UNKNOWN) || (After == TopAbs_UNKNOWN))
                {
                  fairpt = false;
                }
                else if (Before == TopAbs_IN)
                {
                  if (After == TopAbs_IN)
                  {
                    ispassing = true;
                  }
                  else
                  {
                    vectg.Reverse();
                    dirtg.Reverse();
                  }
                }
                else
                {
                  if (After != TopAbs_IN)
                  {
                    fairpt = false;
                  }
                }
              }

              // evite de partir le long d une restriction solution

              if (fairpt && tobeverified)
              {
                for (k = i; k <= NbPoints; k++)
                {
                  if (Destination(k) == seqlength + 1)
                  {
                    theparam                                      = solrst.Point(k).Parameter();
                    const occ::handle<Adaptor2d_Curve2d>& thearc2 = solrst.Point(k).Arc();
                    arcorien                                      = Domain->Orientation(thearc2);

                    if (arcorien == TopAbs_FORWARD || arcorien == TopAbs_REVERSED)
                    {
                      Contap_HCurve2dTool::D1(thearc2, theparam, pt2d, tg2drst);
                      tg3drst  = tg2drst.X() * v1 + tg2drst.Y() * v2;
                      vtxorien = Domain->Orientation(solrst.Point(k).Vertex());
                      if ((arcorien == TopAbs_FORWARD && vtxorien == TopAbs_REVERSED)
                          || (arcorien == TopAbs_REVERSED && vtxorien == TopAbs_FORWARD))
                      {
                        tg3drst.Reverse();
                      }
                      test = vectg.Normalized().Dot(tg3drst.Normalized());
                      if (test >= 1. - tole)
                      {
                        fairpt = false;
                        break;
                      }
                    }
                  }
                }
              }

              if (fairpt)
              {
                PPoint.SetDirections(vectg, dirtg);
                PPoint.SetPassing(ispassing);
                seqpdep.Append(PPoint);
                seqlength++;
              }
              else
              { // il faut remettre en "ordre" si on ne garde pas le point.
                for (k = i; k <= NbPoints; k++)
                {
                  if (Destination(k) == seqlength + 1)
                  {
                    Destination(k) = -Destination(k);
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

IntSurf_TypeTrans ComputeTransitionOnLine(Contap_SurfFunction& SFunc,
                                          const double         u,
                                          const double         v,
                                          const gp_Vec&        tgline)
{
  gp_Vec d1u, d1v;
  gp_Pnt pntbid;
  // gp_Vec tglineuv;

  Adaptor3d_HSurfaceTool::D1(SFunc.Surface(), u, v, pntbid, d1u, d1v);

  //------------------------------------------------------
  //--   Calcul de la tangente dans l espace uv        ---
  //------------------------------------------------------

  double det, d1uT, d1vT, normu2, normv2, d1ud1v, alpha, beta;
  d1uT   = d1u.Dot(tgline);
  d1vT   = d1v.Dot(tgline);
  normu2 = d1u.Dot(d1u);
  normv2 = d1v.Dot(d1v);
  d1ud1v = d1u.Dot(d1v);
  det    = normu2 * normv2 - d1ud1v * d1ud1v;
  if (det < RealEpsilon())
  {
    //-- On ne doit pas passer ici !!
    //-- std::cout<<" Probleme !!!"<<std::endl ;
    return IntSurf_Undecided;
  }

  alpha = (d1uT * normv2 - d1vT * d1ud1v) / det;
  beta  = (normu2 * d1vT - d1ud1v * d1uT) / det;
  //-----------------------------------------------------
  //--  Calcul du Gradient de la fonction Utilisee     --
  //--  pour le contour apparent                       --
  //-----------------------------------------------------

  double      v1, v2;
  math_Vector X(1, 2);
  math_Matrix Df(1, 1, 1, 2);
  X(1) = u;
  X(2) = v;
  SFunc.Derivatives(X, Df);
  v1 = Df(1, 1);
  v2 = Df(1, 2);

  //-----------------------------------------------------
  //-- On calcule si la fonction                       --
  //--        F(.) = Normale . Dir_Regard              --
  //-- Croit Losrque l on se deplace sur la Gauche     --
  //--  de la direction de deplacement sur la ligne.   --
  //-----------------------------------------------------

  det = -v1 * beta + v2 * alpha;

  if (det < RealEpsilon())
  { // revoir le test jag 940620
    return IntSurf_Undecided;
  }
  if (det > 0.0)
  {
    return (IntSurf_Out);
  }
  return (IntSurf_In);
}

void ProcessSegments(const Contap_TheSearch&                 solrst,
                     NCollection_Sequence<Contap_Line>&      slin,
                     const double                            TolArc,
                     Contap_SurfFunction&                    SFunc,
                     const occ::handle<Adaptor3d_TopolTool>& Domain)

{
  int i, j, k;
  int nbedg = solrst.NbSegments();
  int Nblines, Nbpts;

  occ::handle<Adaptor2d_Curve2d> arcRef;
  Contap_Point                   ptvtx;

  Contap_ThePathPointOfTheSearch PStartf, PStartl;

  bool        dofirst, dolast, procf, procl;
  double      paramf = 0., paraml = 0., U;
  Contap_Line theline;

  gp_Vec tgline; //,norm1,norm2;
  gp_Pnt valpt;

  gp_Vec   d1u, d1v;
  gp_Pnt2d p2d;
  gp_Vec2d d2d;

  for (i = 1; i <= nbedg; i++)
  {

    const Contap_TheSegmentOfTheSearch& thesegsol = solrst.Segment(i);
    theline.SetValue(thesegsol.Curve());

    // Traitement des points debut/fin du segment solution.

    dofirst = false;
    dolast  = false;
    procf   = false;
    procl   = false;

    if (thesegsol.HasFirstPoint())
    {
      dofirst = true;
      PStartf = thesegsol.FirstPoint();
      paramf  = PStartf.Parameter();
    }
    if (thesegsol.HasLastPoint())
    {
      dolast  = true;
      PStartl = thesegsol.LastPoint();
      paraml  = PStartl.Parameter();
    }

    // determination de la transition
    if (dofirst && dolast)
    {
      U = (paramf + paraml) / 2.;
    }
    else if (dofirst)
    {
      U = paramf + 1.0;
    }
    else if (dolast)
    {
      U = paraml - 1.0;
    }
    else
    {
      U = 0.0;
    }

    Contap_HCurve2dTool::D1(thesegsol.Curve(), U, p2d, d2d);
    Adaptor3d_HSurfaceTool::D1(SFunc.Surface(), p2d.X(), p2d.Y(), valpt, d1u, d1v);
    tgline.SetLinearForm(d2d.X(), d1u, d2d.Y(), d1v);
    IntSurf_TypeTrans tral = ComputeTransitionOnLine(SFunc, p2d.X(), p2d.Y(), tgline);

    theline.SetTransitionOnS(tral);

    if (dofirst || dolast)
    {
      Nblines = slin.Length();
      for (j = 1; j <= Nblines; j++)
      {
        Nbpts = slin(j).NbVertex();
        for (k = 1; k <= Nbpts; k++)
        {
          ptvtx = slin(j).Vertex(k);
          if (dofirst)
          {
            if (ptvtx.Value().Distance(PStartf.Value()) <= TolArc)
            {
              slin(j).Vertex(k).SetMultiple();
              ptvtx.SetMultiple();
              ptvtx.SetParameter(paramf);
              theline.Add(ptvtx);
              procf = true;
            }
          }
          if (dolast)
          {
            if (ptvtx.Value().Distance(PStartl.Value()) <= TolArc)
            {
              slin(j).Vertex(k).SetMultiple();
              ptvtx.SetMultiple();
              ptvtx.SetParameter(paraml);
              theline.Add(ptvtx);
              procl = true;
            }
          }
        }
        // Si on a traite le pt debut et/ou fin, on ne doit pas recommencer si
        // il (ils) correspond(ent) a un point multiple.

        if (procf)
        {
          dofirst = false;
        }
        if (procl)
        {
          dolast = false;
        }
      }
    }

    // Si on n a pas trouve le point debut et./ou fin sur une des lignes
    // d intersection, il faut quand-meme le placer sur la restriction solution

    if (dofirst)
    {

      p2d = Contap_HCurve2dTool::Value(thesegsol.Curve(), paramf);
      ptvtx.SetValue(PStartf.Value(), p2d.X(), p2d.Y());
      ptvtx.SetParameter(paramf);
      if (!PStartf.IsNew())
      {
        ptvtx.SetVertex(PStartf.Vertex());
      }
      theline.Add(ptvtx);
    }
    if (dolast)
    {
      p2d = Contap_HCurve2dTool::Value(thesegsol.Curve(), paraml);
      ptvtx.SetValue(PStartl.Value(), p2d.X(), p2d.Y());
      ptvtx.SetParameter(paraml);
      if (!PStartl.IsNew())
      {
        ptvtx.SetVertex(PStartl.Vertex());
      }
      theline.Add(ptvtx);
    }

    // il faut chercher le points internal sur les restrictions solutions.
    if (thesegsol.HasFirstPoint() && thesegsol.HasLastPoint())
    {
      ComputeInternalPointsOnRstr(theline, paramf, paraml, SFunc);
    }
    LineConstructor(slin, Domain, theline, SFunc.Surface()); //-- lbr
    //-- slin.Append(theline);
    theline.Clear();
  }
}

void ComputeInternalPointsOnRstr(Contap_Line&         Line,
                                 const double         Paramf,
                                 const double         Paraml,
                                 Contap_SurfFunction& SFunc)
{
  // On recherche les points ou la tangente a la ligne de contour et
  // la direction sont alignees.
  // 1ere etape : recherche de changement de signe.
  // 2eme etape : localisation de la solution par dichotomie

  int      indexinf, indexsup, i;
  gp_Vec   tgt, vecref, vectest, vtestb, vecregard, d1u, d1v;
  gp_Pnt   pcour;
  gp_Pnt2d p2d;
  gp_Vec2d d2d;
  bool     found, ok = false, toutvu, solution;
  double   paramp = 0., paraminf, paramsup, toler;

  if (Line.TypeContour() != Contap_Restriction)
  {
    return;
  }

  const occ::handle<Adaptor2d_Curve2d>& thearc = Line.Arc();

  const occ::handle<Adaptor3d_Surface>& Surf = SFunc.Surface();
  Contap_TFunction                      TypeFunc(SFunc.FunctionType());

  int Nbpnts = Contap_HContTool::NbSamplesOnArc(thearc);
  indexinf   = 1;
  vecregard  = SFunc.Direction();
  toler      = Contap_HCurve2dTool::Resolution(thearc, Precision::Confusion());
  found      = false;

  do
  {
    paraminf = ((Nbpnts - indexinf) * Paramf + (indexinf - 1) * Paraml) / (Nbpnts - 1);
    Contap_HCurve2dTool::D1(thearc, paraminf, p2d, d2d);
    Adaptor3d_HSurfaceTool::D1(Surf, p2d.X(), p2d.Y(), pcour, d1u, d1v);
    tgt.SetLinearForm(d2d.X(), d1u, d2d.Y(), d1v);

    if (tgt.Magnitude() > gp::Resolution())
    {
      if (TypeFunc == Contap_ContourPrs || TypeFunc == Contap_DraftPrs)
      {
        vecregard.SetXYZ(pcour.XYZ() - SFunc.Eye().XYZ());
      }
      vecref = vecregard.Crossed(tgt);

      if (vecref.Magnitude() <= gp::Resolution())
      {
        indexinf++;
      }
      else
      {
        found = true;
      }
    }
    else
    {
      indexinf++;
    }
  } while ((indexinf <= Nbpnts) && (!found));

  indexsup = indexinf + 1;
  toutvu   = (indexsup > Nbpnts);
  while (!toutvu)
  {
    paramsup = ((Nbpnts - indexsup) * Paramf + (indexsup - 1) * Paraml) / (Nbpnts - 1);
    Contap_HCurve2dTool::D1(thearc, paramsup, p2d, d2d);
    Adaptor3d_HSurfaceTool::D1(Surf, p2d.X(), p2d.Y(), pcour, d1u, d1v);
    tgt.SetLinearForm(d2d.X(), d1u, d2d.Y(), d1v);

    if (tgt.Magnitude() > gp::Resolution())
    {
      if (TypeFunc == Contap_ContourPrs || TypeFunc == Contap_DraftPrs)
      {
        vecregard.SetXYZ(pcour.XYZ() - SFunc.Eye().XYZ());
      }
      vectest = vecregard.Crossed(tgt);
    }
    else
    {
      vectest = gp_Vec(0., 0., 0.);
    }
    if (vectest.Magnitude() <= gp::Resolution())
    {
      // On cherche un vrai changement de signe
      indexsup++;
    }
    else
    {
      if (vectest.Dot(vecref) < 0.)
      {
        // Essayer de converger
        // std::cout << "Changement de signe detecte" << std::endl;
        solution = false;
        while (!solution)
        {
          paramp = (paraminf + paramsup) / 2.;
          Contap_HCurve2dTool::D1(thearc, paramp, p2d, d2d);
          Adaptor3d_HSurfaceTool::D1(Surf, p2d.X(), p2d.Y(), pcour, d1u, d1v);
          tgt.SetLinearForm(d2d.X(), d1u, d2d.Y(), d1v);

          if (tgt.Magnitude() > gp::Resolution())
          {
            if (TypeFunc == Contap_ContourPrs || TypeFunc == Contap_DraftPrs)
            {
              vecregard.SetXYZ(pcour.XYZ() - SFunc.Eye().XYZ());
            }
            vtestb = vecregard.Crossed(tgt);
          }
          else
          {
            vtestb = gp_Vec(0., 0., 0.);
          }

          if ((vtestb.Magnitude() <= gp::Resolution()) || (std::abs(paramp - paraminf) <= toler)
              || (std::abs(paramp - paramsup) <= toler))
          {
            // on est a la solution
            solution = true;
            ok       = true;
          }
          else if (vtestb.Dot(vecref) < 0.)
          {
            paramsup = paramp;
          }
          else
          {
            paraminf = paramp;
          }
        }

        if (ok)
        {
          // On verifie que le point trouve ne correspond pas a un ou des
          // vertex deja existant(s). On teste sur le parametre paramp.
          for (i = 1; i <= Line.NbVertex(); i++)
          {
            Contap_Point& thevtx = Line.Vertex(i);
            if (std::abs(thevtx.ParameterOnLine() - paramp) <= toler)
            {
              thevtx.SetInternal();
              ok = false; // on a correspondence
            }
          }
          if (ok)
          { // il faut alors rajouter le point
            Contap_Point internalp(pcour, p2d.X(), p2d.Y());
            internalp.SetParameter(paramp);
            internalp.SetInternal();
            Line.Add(internalp);
          }
        }
        paramsup = ((Nbpnts - indexsup) * Paramf + (indexsup - 1) * Paraml) / (Nbpnts - 1);
      }
      vecref   = vectest;
      indexinf = indexsup;
      indexsup++;
      paraminf = paramsup;
    }
    toutvu = (indexsup > Nbpnts);
  }
}

void ComputeInternalPoints(Contap_Line&         Line,
                           Contap_SurfFunction& SFunc,
                           const double         ureso,
                           const double         vreso)

{
  // On recherche les points ou la tangente a la ligne de contour et
  // la direction sont alignees.
  // 1ere etape : recheche de changement de signe.
  // 2eme etape : localisation de la solution par simili dichotomie

  int    indexinf, indexsup, index;
  gp_Vec tgt, vecref, vectest, vtestb, vecregard;
  // gp_Pnt pprec,pcour;
  bool   found, ok = false, toutvu, solution;
  double paramp = 0., U, V;

  math_Vector XInf(1, 2), XSup(1, 2), X(1, 2), F(1, 1);
  math_Matrix DF(1, 1, 1, 2);
  math_Vector toler(1, 2), infb(1, 2), supb(1, 2);

  if (Line.TypeContour() != Contap_Walking)
  {
    return;
  }

  int                                   Nbpnts = Line.NbPnts();
  const occ::handle<Adaptor3d_Surface>& Surf   = SFunc.Surface();
  Contap_TFunction                      TypeFunc(SFunc.FunctionType());

  // clang-format off
  toler(1) = ureso; //-- Trop long !!! Adaptor3d_HSurfaceTool::UResolution(Surf,SFunc.Tolerance());
  toler(2) = vreso; //---Beaucoup trop long !!! Adaptor3d_HSurfaceTool::VResolution(Surf,SFunc.Tolerance());
  // clang-format on
  infb(1) = Adaptor3d_HSurfaceTool::FirstUParameter(Surf);
  infb(2) = Adaptor3d_HSurfaceTool::FirstVParameter(Surf);
  supb(1) = Adaptor3d_HSurfaceTool::LastUParameter(Surf);
  supb(2) = Adaptor3d_HSurfaceTool::LastVParameter(Surf);

  math_FunctionSetRoot rsnld(SFunc, toler, 30);

  indexinf  = 1;
  vecregard = SFunc.Direction();

  found = false;
  do
  {
    Line.Point(indexinf).ParametersOnS2(XInf(1), XInf(2));
    SFunc.Values(XInf, F, DF);
    if (!SFunc.IsTangent())
    {
      tgt = SFunc.Direction3d();
      if (TypeFunc == Contap_ContourPrs || TypeFunc == Contap_DraftPrs)
      {
        vecregard.SetXYZ(Line.Point(indexinf).Value().XYZ() - SFunc.Eye().XYZ());
      }
      vecref = vecregard.Crossed(tgt);

      if (vecref.Magnitude() <= gp::Resolution())
      {
        indexinf++;
      }
      else
      {
        found = true;
      }
    }
    else
    {
      indexinf++;
    }
  } while ((indexinf <= Nbpnts) && (!found));

  indexsup = indexinf + 1;
  toutvu   = (indexsup > Nbpnts);
  while (!toutvu)
  {
    Line.Point(indexsup).ParametersOnS2(XSup(1), XSup(2));
    SFunc.Values(XSup, F, DF);
    if (!SFunc.IsTangent())
    {
      tgt = SFunc.Direction3d();

      if (TypeFunc == Contap_ContourPrs || TypeFunc == Contap_DraftPrs)
      {
        vecregard.SetXYZ(Line.Point(indexsup).Value().XYZ() - SFunc.Eye().XYZ());
      }
      vectest = vecregard.Crossed(tgt);
    }
    else
    {
      vectest = gp_Vec(0., 0., 0.);
    }
    if (vectest.Magnitude() <= gp::Resolution())
    {
      // On cherche un vrai changement de signe
      indexsup++;
    }
    else
    {
      if (vectest.Dot(vecref) < 0.)
      {
        // Essayer de converger
        // std::cout << "Changement de signe detecte" << std::endl;
        solution = false;
        while (!solution)
        {
          // Selecting the middle point between XInf and XSup leads situation, where X values almost
          // do not change. To prevent this situation, select shifted point instead of middle.
          const double aCoef = 2. / 3.;
          X(1)               = XInf(1) + aCoef * (XSup(1) - XInf(1));
          X(2)               = XInf(2) + aCoef * (XSup(2) - XInf(2));
          rsnld.Perform(SFunc, X, infb, supb);

          if (!rsnld.IsDone())
          {
            std::cout << "Echec recherche internal points" << std::endl;
            solution = true;
            ok       = false;
          }
          else
          {

            rsnld.Root(X);
            SFunc.Values(X, F, DF);
            if (std::abs(F(1)) <= SFunc.Tolerance())
            {

              if (!SFunc.IsTangent())
              {
                tgt = SFunc.Direction3d();
                if (TypeFunc == Contap_ContourPrs || TypeFunc == Contap_DraftPrs)
                {
                  vecregard.SetXYZ(SFunc.Point().XYZ() - SFunc.Eye().XYZ());
                }
                vtestb = vecregard.Crossed(tgt);
              }
              else
              {
                vtestb = gp_Vec(0., 0., 0.);
              }
              if ((vtestb.Magnitude() <= gp::Resolution())
                  || (std::abs(X(1) - XInf(1)) <= toler(1) && std::abs(X(2) - XInf(2)) <= toler(2))
                  || (std::abs(X(1) - XSup(1)) <= toler(1) && std::abs(X(2) - XSup(2)) <= toler(2)))
              {
                // on est a la solution
                solution = true;
                ok       = true;
              }
              else if (vtestb.Dot(vecref) < 0.)
              {
                XSup = X;
              }
              else
              {
                XInf = X;
              }
            }
            else
            { // on n est pas sur une solution
              std::cout << "Echec recherche internal points" << std::endl;
              solution = true;
              ok       = false;
            }
          }
        }

        if (ok)
        {
          bool newpoint = false;
          Line.Point(indexinf).ParametersOnS2(U, V);
          gp_Vec2d vinf(X(1) - U, X(2) - V);
          if (std::abs(vinf.X()) <= toler(1) && std::abs(vinf.Y()) <= toler(2))
          {
            paramp = indexinf;
          }
          else
          {
            for (index = indexinf + 1; index <= indexsup; index++)
            {
              Line.Point(index).ParametersOnS2(U, V);
              gp_Vec2d vsup(X(1) - U, X(2) - V);
              if (std::abs(vsup.X()) <= toler(1) && std::abs(vsup.Y()) <= toler(2))
              {
                paramp = index;
                break;
              }
              else if (vinf.Dot(vsup) < 0.)
              {
                // on est entre les 2 points
                paramp = index;
                IntSurf_PntOn2S pt2s;
                pt2s.SetValue(SFunc.Point(), false, X(1), X(2));
                Line.LineOn2S()->InsertBefore(index, pt2s);

                //-- Il faut decaler les parametres des vertex situes entre
                //-- index et NbPnts ###################################
                for (int v = 1; v <= Line.NbVertex(); v++)
                {
                  Contap_Point& Vertex = Line.Vertex(v);
                  if (Vertex.ParameterOnLine() >= index)
                  {
                    Vertex.SetParameter(Vertex.ParameterOnLine() + 1);
                  }
                }

                Nbpnts   = Nbpnts + 1;
                indexsup = indexsup + 1;
                newpoint = true;
                break;
              }
              else
              {
                vinf = vsup;
              }
            }
          }

          int v;
          if (!newpoint)
          {
            // on est sur un point de cheminement. On regarde alors
            // la correspondance avec un vertex existant.
            newpoint = true;
            for (v = 1; v <= Line.NbVertex(); v++)
            {
              Contap_Point& Vertex = Line.Vertex(v);
              if (Vertex.ParameterOnLine() == paramp)
              {
                Vertex.SetInternal();
                newpoint = false;
              }
            }
          }

          if (newpoint && paramp > 1. && paramp < Nbpnts)
          {
            // on doit creer un nouveau vertex.
            Contap_Point internalp(SFunc.Point(), X(1), X(2));
            internalp.SetParameter(paramp);
            internalp.SetInternal();
            Line.Add(internalp);
          }
        }
        Line.Point(indexsup).ParametersOnS2(XSup(1), XSup(2));
      }
      vecref   = vectest;
      indexinf = indexsup;
      indexsup++;
      XInf = XSup;
    }
    toutvu = (indexsup > Nbpnts);
  }
}

void Contap_Contour::Perform(const occ::handle<Adaptor3d_TopolTool>& Domain)
{

  done = false;
  slin.Clear();

  int                i, j, k, Nbvt1, Nbvt2, ivt1, ivt2;
  int                NbPointRst, NbPointIns;
  int                Nblines, Nbpts, indfirst, indlast;
  double             U, V;
  gp_Pnt2d           pt2d;
  gp_Vec2d           d2d;
  gp_Pnt             ptonsurf;
  gp_Vec             d1u, d1v, normale, tgtrst, tgline;
  double             currentparam;
  IntSurf_Transition TLine, TArc;

  Contap_Line                    theline;
  Contap_Point                   ptdeb, ptfin;
  Contap_ThePathPointOfTheSearch PStartf, PStartl;

  //  double TolArc = 1.e-5;
  double TolArc = Precision::Confusion();

  const occ::handle<Adaptor3d_Surface>& Surf = mySFunc.Surface();

  double EpsU  = Adaptor3d_HSurfaceTool::UResolution(Surf, Precision::Confusion());
  double EpsV  = Adaptor3d_HSurfaceTool::VResolution(Surf, Precision::Confusion());
  double Preci = std::min(EpsU, EpsV);
  //  double Fleche = 5.e-1;
  //  double Pas    = 5.e-2;
  double Fleche = 0.01;
  double Pas    = 0.005;
  //   lbr: Il y avait Pas 0.2 -> Manque des Inters sur restr ; devrait faire un mini de 5 pts par
  //   lignes
  //-- le 23 janvier 98 0.05 -> 0.01

  //-- ******************************************************************************** Janvier 98
  Bnd_Box B1;
  bool    Box1OK = true;

  double Uinf = Surf->FirstUParameter();
  double Vinf = Surf->FirstVParameter();
  double Usup = Surf->LastUParameter();
  double Vsup = Surf->LastVParameter();

  bool Uinfinfinite = Precision::IsNegativeInfinite(Uinf);
  bool Usupinfinite = Precision::IsPositiveInfinite(Usup);
  bool Vinfinfinite = Precision::IsNegativeInfinite(Vinf);
  bool Vsupinfinite = Precision::IsPositiveInfinite(Vsup);

  if (Uinfinfinite || Usupinfinite || Vinfinfinite || Vsupinfinite)
  {
    Box1OK = false;
  }
  else
  {
    BndLib_AddSurface::Add(*Surf, 1e-8, B1);
  }
  double x0, y0, z0, x1, y1, z1, dx, dy, dz;
  if (Box1OK)
  {
    B1.Get(x0, y0, z0, x1, y1, z1);
    dx = x1 - x0;
    dy = y1 - y0;
    dz = z1 - z0;
  }
  else
  {
    dx = dy = dz = 1.0;
  }
  if (dx < dy)
    dx = dy;
  if (dx < dz)
    dx = dz;
  if (dx > 10000.0)
    dx = 10000.0;
  Fleche *= dx;
  TolArc *= dx;
  //-- ********************************************************************************

  // gp_Pnt valpt;

  // jag 940616  SFunc.Set(1.e-8); // tolerance sur la fonction
  mySFunc.Set(Precision::Confusion()); // tolerance sur la fonction

  bool RecheckOnRegularity = true;
  solrst.Perform(myAFunc, Domain, TolArc, TolArc, RecheckOnRegularity);

  if (!solrst.IsDone())
  {
    return;
  }

  NbPointRst = solrst.NbPoints();
  NCollection_Sequence<IntSurf_PathPoint> seqpdep;
  NCollection_Array1<int>                 Destination(1, NbPointRst + 1);
  Destination.Init(0);
  if (NbPointRst != 0)
  {
    ComputeTangency(solrst, Domain, mySFunc, seqpdep, Destination);
  }

  // jag 940616  solins.Perform(SFunc,Surf,Domain,1.e-6); // 1.e-6 : tolerance dans l espace.
  solins.Perform(mySFunc, Surf, Domain, Precision::Confusion());

  NbPointIns = solins.NbPoints();
  NCollection_Sequence<IntSurf_InteriorPoint> seqpins;

  if (NbPointIns != 0)
  {
    bool bKeepAllPoints = false;
    // IFV begin
    if (solrst.NbSegments() <= 0)
    {
      if (mySFunc.FunctionType() == Contap_ContourStd)
      {
        const occ::handle<Adaptor3d_Surface>& SurfToCheck = mySFunc.Surface();
        if (Adaptor3d_HSurfaceTool::GetType(SurfToCheck) == GeomAbs_Torus)
        {
          gp_Torus aTor     = Adaptor3d_HSurfaceTool::Torus(SurfToCheck);
          gp_Dir   aTorDir  = aTor.Axis().Direction();
          gp_Dir   aProjDir = mySFunc.Direction();

          if (aTorDir.Dot(aProjDir) < Precision::Confusion())
          {
            bKeepAllPoints = true;
          }
        }
      }
    }

    if (bKeepAllPoints)
    {
      int Nbp = solins.NbPoints(), indp;
      for (indp = 1; indp <= Nbp; indp++)
      {
        const IntSurf_InteriorPoint& pti = solins.Value(indp);
        seqpins.Append(pti);
      }
    }
    // IFV - end
    else
    {
      KeepInsidePoints(solins, solrst, mySFunc, seqpins);
    }
  }

  if (seqpdep.Length() != 0 || seqpins.Length() != 0)
  {

    bool               theToFillHoles = true;
    Contap_TheIWalking iwalk(Preci, Fleche, Pas, theToFillHoles);
    iwalk.Perform(seqpdep, seqpins, mySFunc, Surf);
    if (!iwalk.IsDone())
    {
      return;
    }

    Nblines = iwalk.NbLines();
    for (j = 1; j <= Nblines; j++)
    {
      IntSurf_TypeTrans                                 TypeTransOnS = IntSurf_Undecided;
      const occ::handle<Contap_TheIWLineOfTheIWalking>& iwline       = iwalk.Value(j);
      Nbpts                                                          = iwline->NbPoints();
      theline.SetLineOn2S(iwline->Line());

      // jag 941018 On calcule une seule fois la transition

      tgline = iwline->TangentVector(k);
      iwline->Line()->Value(k).ParametersOnS2(U, V);
      TypeTransOnS = ComputeTransitionOnLine(mySFunc, U, V, tgline);
      theline.SetTransitionOnS(TypeTransOnS);

      //---------------------------------------------------------------------
      //-- On ajoute a la liste des vertex les 1er et dernier points de la  -
      //-- ligne de cheminement si ceux-ci ne sont pas presents             -
      //---------------------------------------------------------------------

      if (iwline->HasFirstPoint())
      {
        indfirst                         = iwline->FirstPointIndex();
        const IntSurf_PathPoint& PPoint  = seqpdep(indfirst);
        int                      themult = PPoint.Multiplicity();
        for (i = NbPointRst; i >= 1; i--)
        {
          if (Destination(i) == indfirst)
          {
            PPoint.Parameters(themult, U, V);
            ptdeb.SetValue(PPoint.Value(), U, V);
            ptdeb.SetParameter(1.0);

            const Contap_ThePathPointOfTheSearch& PStart     = solrst.Point(i);
            const occ::handle<Adaptor2d_Curve2d>& currentarc = PStart.Arc();
            currentparam                                     = PStart.Parameter();
            if (!iwline->IsTangentAtBegining())
            {

              Contap_HCurve2dTool::D1(currentarc, currentparam, pt2d, d2d);
              Contap_SurfProps::DerivAndNorm(Surf, pt2d.X(), pt2d.Y(), ptonsurf, d1u, d1v, normale);
              tgtrst = d2d.X() * d1u;
              tgtrst.Add(d2d.Y() * d1v);

              IntSurf::MakeTransition(PPoint.Direction3d(), tgtrst, normale, TLine, TArc);
            }
            else
            { // a voir. En effet, on a cheminer. Si on est sur un point
              // debut, on sait qu'on rentre dans la matiere
              TLine.SetValue();
              TArc.SetValue();
            }

            ptdeb.SetArc(currentarc, currentparam, TLine, TArc);

            if (!solrst.Point(i).IsNew())
            {
              ptdeb.SetVertex(PStart.Vertex());
            }
            theline.Add(ptdeb);
            themult--;
          }
        }
      }
      else
      {
        iwline->Value(1).ParametersOnS2(U, V);
        ptdeb.SetValue(theline.Point(1).Value(), U, V);
        ptdeb.SetParameter(1.0);
        theline.Add(ptdeb);
      }

      if (iwline->HasLastPoint())
      {
        indlast                          = iwline->LastPointIndex();
        const IntSurf_PathPoint& PPoint  = seqpdep(indlast);
        int                      themult = PPoint.Multiplicity();
        for (i = NbPointRst; i >= 1; i--)
        {
          if (Destination(i) == indlast)
          {
            PPoint.Parameters(themult, U, V);
            ptfin.SetValue(PPoint.Value(), U, V);
            ptfin.SetParameter((double)(Nbpts));
            const Contap_ThePathPointOfTheSearch& PStart     = solrst.Point(i);
            const occ::handle<Adaptor2d_Curve2d>& currentarc = PStart.Arc();
            currentparam                                     = PStart.Parameter();

            if (!iwline->IsTangentAtEnd())
            {

              Contap_HCurve2dTool::D1(currentarc, currentparam, pt2d, d2d);

              Contap_SurfProps::DerivAndNorm(Surf, pt2d.X(), pt2d.Y(), ptonsurf, d1u, d1v, normale);
              tgtrst = d2d.X() * d1u;
              tgtrst.Add(d2d.Y() * d1v);
              IntSurf::MakeTransition(PPoint.Direction3d().Reversed(),
                                      tgtrst,
                                      normale,
                                      TLine,
                                      TArc);
            }
            else
            {
              TLine.SetValue();
              TArc.SetValue();
            }

            ptfin.SetArc(currentarc, currentparam, TLine, TArc);

            if (!solrst.Point(i).IsNew())
            {
              ptfin.SetVertex(PStart.Vertex());
            }
            theline.Add(ptfin);
            themult--;
          }
        }
      }
      else
      {
        iwline->Value(Nbpts).ParametersOnS2(U, V);
        ptfin.SetValue(theline.Point(Nbpts).Value(), U, V);
        ptfin.SetParameter((double)(Nbpts));
        theline.Add(ptfin);
      }

      ComputeInternalPoints(theline, mySFunc, EpsU, EpsV);
      LineConstructor(slin, Domain, theline, Surf); //-- lbr
      //-- slin.Append(theline);
      theline.ResetSeqOfVertex();
    }

    Nblines = slin.Length();
    for (j = 1; j <= Nblines - 1; j++)
    {
      const Contap_Line& theli = slin(j);
      Nbvt1                    = theli.NbVertex();
      for (ivt1 = 1; ivt1 <= Nbvt1; ivt1++)
      {
        if (!theli.Vertex(ivt1).IsOnArc())
        {
          const gp_Pnt& pttg1 = theli.Vertex(ivt1).Value();

          for (k = j + 1; k <= Nblines; k++)
          {
            const Contap_Line& theli2 = slin(k);
            Nbvt2                     = theli2.NbVertex();
            for (ivt2 = 1; ivt2 <= Nbvt2; ivt2++)
            {
              if (!theli2.Vertex(ivt2).IsOnArc())
              {
                const gp_Pnt& pttg2 = theli2.Vertex(ivt2).Value();

                if (pttg1.Distance(pttg2) <= TolArc)
                {
                  theli.Vertex(ivt1).SetMultiple();
                  theli2.Vertex(ivt2).SetMultiple();
                }
              }
            }
          }
        }
      }
    }
  }

  // jag 940620 On ajoute le traitement des restrictions solutions.

  if (solrst.NbSegments() != 0)
  {
    ProcessSegments(solrst, slin, TolArc, mySFunc, Domain);
  }

  // Ajout crad pour depanner CMA en attendant mieux
  if (solrst.NbSegments() != 0)
  {

    Nblines = slin.Length();
    for (j = 1; j <= Nblines; j++)
    {
      const Contap_Line& theli = slin(j);
      if (theli.TypeContour() == Contap_Walking)
      {
        Nbvt1 = theli.NbVertex();
        for (ivt1 = 1; ivt1 <= Nbvt1; ivt1++)
        {
          Contap_Point& ptvt = theli.Vertex(ivt1);
          if (!ptvt.IsOnArc() && !ptvt.IsMultiple())
          {
            double Up, Vp;
            ptvt.Parameters(Up, Vp);
            gp_Pnt2d toproj(Up, Vp);
            bool     projok;
            for (k = 1; k <= Nblines; k++)
            {
              if (slin(k).TypeContour() == Contap_Restriction)
              {
                const occ::handle<Adaptor2d_Curve2d>& thearc = slin(k).Arc();
                double                                paramproj;
                gp_Pnt2d                              Ptproj;
                projok = Contap_HContTool::Project(thearc, toproj, paramproj, Ptproj);

                if (projok)
                {
                  double dist = Ptproj.Distance(gp_Pnt2d(Up, Vp));
                  if (dist <= Preci)
                  {
                    // Calcul de la transition

                    Contap_HCurve2dTool::D1(thearc, paramproj, Ptproj, d2d);
                    //		    Adaptor3d_HSurfaceTool::D1(Surf,Ptproj.X(),Ptproj.Y(),
                    //				       ptonsurf,d1u,d1v);
                    //		    normale = d1u.Crossed(d1v);

                    Contap_SurfProps::DerivAndNorm(Surf,
                                                   Ptproj.X(),
                                                   Ptproj.Y(),
                                                   ptonsurf,
                                                   d1u,
                                                   d1v,
                                                   normale);

                    tgtrst = d2d.X() * d1u;
                    tgtrst.Add(d2d.Y() * d1v);
                    int Paraml = (int)ptvt.ParameterOnLine();

                    if (Paraml == theli.NbPnts())
                    {
                      tgline = gp_Vec(theli.Point(Paraml - 1).Value(), ptvt.Value());
                    }
                    else
                    {
                      tgline = gp_Vec(ptvt.Value(), theli.Point(Paraml + 1).Value());
                    }
                    IntSurf::MakeTransition(tgline, tgtrst, normale, TLine, TArc);
                    ptvt.SetArc(thearc, paramproj, TLine, TArc);
                    ptvt.SetMultiple();
                    ptdeb.SetValue(ptonsurf, Ptproj.X(), Ptproj.Y());
                    ptdeb.SetParameter(paramproj);
                    ptdeb.SetMultiple();
                    slin(k).Add(ptdeb);
                    break;
                  }
                  else
                  {
                    projok = false;
                  }
                }
              }
              else
              {
                projok = false;
              }
              if (projok)
              {
                break;
              }
            }
          }
        }
      }
    }
  }
  done = true;
}

static bool FindLine(Contap_Line&                          Line,
                     const occ::handle<Adaptor3d_Surface>& Surf,
                     const gp_Pnt2d&                       Pt2d,
                     gp_Pnt&                               Ptref,
                     double&                               Paramin,
                     gp_Vec&                               Tgmin,
                     gp_Vec&                               Norm)
{
  //  int i;
  gp_Pnt pt, ptmin;
  gp_Vec tg;
  double para, dist;
  double dismin = RealLast();

  Contap_SurfProps::Normale(Surf, Pt2d.X(), Pt2d.Y(), Ptref, Norm);

  if (Line.TypeContour() == Contap_Lin)
  {
    gp_Lin lin(Line.Line());
    para = ElCLib::Parameter(lin, Ptref);
    ElCLib::D1(para, lin, pt, tg);
    dist = pt.Distance(Ptref) + std::abs(Norm.Dot(lin.Direction()));
  }
  else
  { // Contap__Circle
    gp_Circ cir(Line.Circle());
    para = ElCLib::Parameter(cir, Ptref);
    ElCLib::D1(para, cir, pt, tg);
    dist = pt.Distance(Ptref) + std::abs(Norm.Dot(tg / cir.Radius()));
  }
  if (dist < dismin)
  {
    dismin  = dist;
    Paramin = para;
    ptmin   = pt;
    Tgmin   = tg;
  }
  if (ptmin.SquareDistance(Ptref) <= Tolpetit)
  {
    return true;
  }
  else
  {
    return false;
  }
}

static void PutPointsOnLine(const Contap_TheSearch&               solrst,
                            const occ::handle<Adaptor3d_Surface>& Surf,
                            NCollection_Sequence<Contap_Line>&    slin)

{
  int i, l; //,index;
  int NbPoints = solrst.NbPoints();

  double theparam;

  IntSurf_Transition TLine, TArc;
  bool               goon;

  gp_Pnt2d pt2d;
  gp_Vec2d d2d;

  gp_Pnt ptonsurf;
  gp_Vec vectg, normale, tgtrst;
  double paramlin = 0.0;

  int nbLin = slin.Length();
  for (l = 1; l <= nbLin; l++)
  {
    Contap_Line& Line = slin.ChangeValue(l);
    for (i = 1; i <= NbPoints; i++)
    {

      const Contap_ThePathPointOfTheSearch& PStart = solrst.Point(i);
      const occ::handle<Adaptor2d_Curve2d>& thearc = PStart.Arc();
      theparam                                     = PStart.Parameter();

      Contap_HCurve2dTool::D1(thearc, theparam, pt2d, d2d);
      goon = FindLine(Line, Surf, pt2d, ptonsurf, paramlin, vectg, normale);

      Contap_Point PPoint;

      if (goon)
      {
        gp_Vec d1u, d1v;
        gp_Pnt bidpt;
        Adaptor3d_HSurfaceTool::D1(Surf, pt2d.X(), pt2d.Y(), bidpt, d1u, d1v);
        PPoint.SetValue(ptonsurf, pt2d.X(), pt2d.Y());
        if (normale.Magnitude() < RealEpsilon())
        {
          TLine.SetValue();
          TArc.SetValue();
        }
        else
        {
          // Petit test qui devrait permettre de bien traiter les pointes
          // des cones, et les sommets d`une sphere. Il faudrait peut-etre
          // rajouter une methode dans SurfProps

          if (std::abs(d2d.Y()) <= Precision::Confusion())
          {
            tgtrst = d1v.Crossed(normale);
            if (d2d.X() < 0.0)
              tgtrst.Reverse();
          }
          else
          {
            tgtrst.SetLinearForm(d2d.X(), d1u, d2d.Y(), d1v);
          }
          IntSurf::MakeTransition(vectg, tgtrst, normale, TLine, TArc);
        }

        PPoint.SetArc(thearc, theparam, TLine, TArc);
        PPoint.SetParameter(paramlin);
        if (!PStart.IsNew())
        {
          PPoint.SetVertex(PStart.Vertex());
        }
        Line.Add(PPoint);
      }
    }
  }
}

//----------------------------------------------------------------------------------
//-- Orientation des contours Apparents quand ceux-ci sont des lignes ou des cercles
//-- On prend un point de la ligne ou du cercle ---> P
//-- On projete ce point sur la surface P ---> u,v
//-- et on evalue la transition au point u,v
//----------------------------------------------------------------------------------

IntSurf_TypeTrans ComputeTransitionOngpLine(Contap_SurfFunction& SFunc, const gp_Lin& L)
{
  const occ::handle<Adaptor3d_Surface>& Surf = SFunc.Surface();
  GeomAbs_SurfaceType                   typS = Adaptor3d_HSurfaceTool::GetType(Surf);
  gp_Pnt                                P;
  gp_Vec                                T;
  ElCLib::D1(0.0, L, P, T);
  double u = 0., v = 0.;
  switch (typS)
  {
    case GeomAbs_Cylinder: {
      ElSLib::Parameters(Adaptor3d_HSurfaceTool::Cylinder(Surf), P, u, v);
      break;
    }
    case GeomAbs_Cone: {
      ElSLib::Parameters(Adaptor3d_HSurfaceTool::Cone(Surf), P, u, v);
      break;
    }
    case GeomAbs_Sphere: {
      ElSLib::Parameters(Adaptor3d_HSurfaceTool::Sphere(Surf), P, u, v);
      break;
    }
    default:
      break;
  }
  return (ComputeTransitionOnLine(SFunc, u, v, T));
}

IntSurf_TypeTrans ComputeTransitionOngpCircle(Contap_SurfFunction& SFunc, const gp_Circ& C)
{
  const occ::handle<Adaptor3d_Surface>& Surf = SFunc.Surface();
  GeomAbs_SurfaceType                   typS = Adaptor3d_HSurfaceTool::GetType(Surf);
  gp_Pnt                                P;
  gp_Vec                                T;
  ElCLib::D1(0.0, C, P, T);
  double u = 0., v = 0.;
  switch (typS)
  {
    case GeomAbs_Cylinder: {
      ElSLib::Parameters(Adaptor3d_HSurfaceTool::Cylinder(Surf), P, u, v);
      break;
    }
    case GeomAbs_Cone: {
      ElSLib::Parameters(Adaptor3d_HSurfaceTool::Cone(Surf), P, u, v);
      break;
    }
    case GeomAbs_Sphere: {
      ElSLib::Parameters(Adaptor3d_HSurfaceTool::Sphere(Surf), P, u, v);
      break;
    }
    default:
      break;
  }
  return (ComputeTransitionOnLine(SFunc, u, v, T));
}

void Contap_Contour::PerformAna(const occ::handle<Adaptor3d_TopolTool>& Domain)
{

  done = false;
  slin.Clear();

  double TolArc = 1.e-5;

  int nbCont, nbPointRst, i;
  // gp_Circ cirsol;
  // gp_Lin linsol;
  Contap_ContAna                        contana;
  Contap_Line                           theline;
  const occ::handle<Adaptor3d_Surface>& Surf = mySFunc.Surface();
  Contap_TFunction                      TypeFunc(mySFunc.FunctionType());
  bool                                  PerformSolRst = true;

  GeomAbs_SurfaceType typS = Adaptor3d_HSurfaceTool::GetType(Surf);

  switch (typS)
  {
    case GeomAbs_Plane: {
      gp_Pln pl(Adaptor3d_HSurfaceTool::Plane(Surf));
      switch (TypeFunc)
      {
        case Contap_ContourStd: {
          gp_Dir Dirpln(pl.Axis().Direction());
          if (std::abs(mySFunc.Direction().Dot(Dirpln)) > Precision::Angular())
          {
            // Aucun point du plan n`est solution, en particulier aucun point
            // sur restriction.
            PerformSolRst = false;
          }
        }
        break;
        case Contap_ContourPrs: {
          gp_Pnt Eye(mySFunc.Eye());
          if (pl.Distance(Eye) > Precision::Confusion())
          {
            // Aucun point du plan n`est solution, en particulier aucun point
            // sur restriction.
            PerformSolRst = false;
          }
        }
        break;
        case Contap_DraftStd: {
          gp_Dir Dirpln(pl.Axis().Direction());
          double Sina = std::sin(mySFunc.Angle());
          if (std::abs(mySFunc.Direction().Dot(Dirpln) + Sina) > // voir SurfFunction
              Precision::Angular())
          {

            PerformSolRst = false;
          }
        }
        break;
        case Contap_DraftPrs:
        default: {
        }
      }
    }
    break;

    case GeomAbs_Sphere: {
      switch (TypeFunc)
      {
        case Contap_ContourStd: {
          contana.Perform(Adaptor3d_HSurfaceTool::Sphere(Surf), mySFunc.Direction());
        }
        break;
        case Contap_ContourPrs: {
          contana.Perform(Adaptor3d_HSurfaceTool::Sphere(Surf), mySFunc.Eye());
        }
        break;
        case Contap_DraftStd: {
          contana.Perform(Adaptor3d_HSurfaceTool::Sphere(Surf),
                          mySFunc.Direction(),
                          mySFunc.Angle());
        }
        break;
        case Contap_DraftPrs:
        default: {
        }
      }
    }
    break;

    case GeomAbs_Cylinder: {
      switch (TypeFunc)
      {
        case Contap_ContourStd: {
          contana.Perform(Adaptor3d_HSurfaceTool::Cylinder(Surf), mySFunc.Direction());
        }
        break;
        case Contap_ContourPrs: {
          contana.Perform(Adaptor3d_HSurfaceTool::Cylinder(Surf), mySFunc.Eye());
        }
        break;
        case Contap_DraftStd: {
          contana.Perform(Adaptor3d_HSurfaceTool::Cylinder(Surf),
                          mySFunc.Direction(),
                          mySFunc.Angle());
        }
        break;
        case Contap_DraftPrs:
        default: {
        }
      }
    }
    break;

    case GeomAbs_Cone: {
      switch (TypeFunc)
      {
        case Contap_ContourStd: {
          contana.Perform(Adaptor3d_HSurfaceTool::Cone(Surf), mySFunc.Direction());
        }
        break;
        case Contap_ContourPrs: {
          contana.Perform(Adaptor3d_HSurfaceTool::Cone(Surf), mySFunc.Eye());
        }
        break;
        case Contap_DraftStd: {
          contana.Perform(Adaptor3d_HSurfaceTool::Cone(Surf), mySFunc.Direction(), mySFunc.Angle());
        }
        break;
        case Contap_DraftPrs:
        default: {
        }
      }
      default:
        break;
    }
    break;
  }

  if (typS != GeomAbs_Plane)
  {

    if (!contana.IsDone())
    {
      return;
    }

    nbCont = contana.NbContours();

    if (contana.NbContours() == 0)
    {
      done = true;
      return;
    }

    GeomAbs_CurveType typL = contana.TypeContour();
    if (typL == GeomAbs_Circle)
    {
      theline.SetValue(contana.Circle());
      IntSurf_TypeTrans TransCircle;
      TransCircle = ComputeTransitionOngpCircle(mySFunc, contana.Circle());
      theline.SetTransitionOnS(TransCircle);
      slin.Append(theline);
    }
    else if (typL == GeomAbs_Line)
    {
      for (i = 1; i <= nbCont; i++)
      {
        theline.SetValue(contana.Line(i));
        IntSurf_TypeTrans TransLine;
        TransLine = ComputeTransitionOngpLine(mySFunc, contana.Line(i));
        theline.SetTransitionOnS(TransLine);
        slin.Append(theline);
        theline.Clear();
      }

      /*
      if (typS == GeomAbs_Cone) {
      double u,v;
      gp_Cone thecone(Adaptor3d_HSurfaceTool::Cone(Surf));
      ElSLib::Parameters(thecone,thecone.Apex(),u,v);
      Contap_Point vtxapex(thecone.Apex(),u,v);
      vtxapex.SetInternal();
      vtxapex.SetMultiple();
      for (i=1; i<=nbCont i++) {
      slin.ChangeValue(i).Add(vtxapex);
      }
      }
      */
    }
  }

  if (PerformSolRst)
  {

    solrst.Perform(myAFunc, Domain, TolArc, TolArc);
    if (!solrst.IsDone())
    {
      return;
    }
    nbPointRst = solrst.NbPoints();

    if (nbPointRst != 0)
    {
      PutPointsOnLine(solrst, Surf, slin);
    }

    if (solrst.NbSegments() != 0)
    {
      ProcessSegments(solrst, slin, TolArc, mySFunc, Domain);
    }

    //-- lbr
    // bool oneremov;
    int                       nblinto = slin.Length();
    NCollection_Sequence<int> SeqToDestroy;

    //-- std::cout<<" Construct Contour_3   nblin = "<<nblinto<<std::endl;
    for (i = 1; i <= nblinto; i++)
    {
      //-- std::cout<<" nbvtx : "<<slin.Value(i).NbVertex()<<std::endl;
      //--if(slin.Value(i).NbVertex() > 1) {
      if (slin.Value(i).TypeContour() != Contap_Restriction)
      {
        LineConstructor(slin, Domain, slin.ChangeValue(i), Surf);
        SeqToDestroy.Append(i);
      }
      //-- }
    }
    for (i = SeqToDestroy.Length(); i >= 1; i--)
    {
      slin.Remove(SeqToDestroy.Value(i));
    }
  }

  done = true;
}
