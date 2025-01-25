// Copyright (c) 1992-1999 Matra Datavision
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

#include <Adaptor2d_Curve2d.hxx>
#include <Adaptor3d_HVertex.hxx>
#include <Adaptor3d_Surface.hxx>
#include <Adaptor3d_TopolTool.hxx>
#include <Bnd_Box2d.hxx>
#include <Bnd_Range.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <Extrema_ExtPC.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Parabola.hxx>
#include <IntAna_IntQuadQuad.hxx>
#include <IntAna_ListOfCurve.hxx>
#include <IntAna_QuadQuadGeo.hxx>
#include <IntAna_Quadric.hxx>
#include <IntPatch_ALine.hxx>
#include <IntPatch_ArcFunction.hxx>
#include <IntPatch_GLine.hxx>
#include <IntPatch_HInterTool.hxx>
#include <IntPatch_ImpImpIntersection.hxx>
#include <IntPatch_RLine.hxx>
#include <IntPatch_ThePathPointOfTheSOnBounds.hxx>
#include <IntPatch_TheSegmentOfTheSOnBounds.hxx>
#include <IntPatch_WLine.hxx>
#include <IntSurf.hxx>
#include <NCollection_IncAllocator.hxx>
#include <Standard_DivideByZero.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <gce_MakeLin.hxx>
#include <gp.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Pln.hxx>
#include <gp_Sphere.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>

#include <algorithm>

static void PutPointsOnLine(const Handle(Adaptor3d_Surface)& S1,
                            const Handle(Adaptor3d_Surface)& S2,
                            const IntPatch_SequenceOfPathPointOfTheSOnBounds&,
                            const IntPatch_SequenceOfLine&,
                            const Standard_Boolean,
                            const Handle(Adaptor3d_TopolTool)&,
                            const IntSurf_Quadric&,
                            const IntSurf_Quadric&,
                            const Standard_Boolean,
                            const Standard_Real);

static Standard_Boolean MultiplePoint(const IntPatch_SequenceOfPathPointOfTheSOnBounds& listpnt,
                                      const Handle(Adaptor3d_TopolTool)&                Domain,
                                      const IntSurf_Quadric&                            QuadSurf,
                                      const gp_Vec&                                     Normale,
                                      const IntPatch_SequenceOfLine&                    slin,
                                      TColStd_Array1OfInteger&                          Done,
                                      TColStd_Array1OfInteger&                          UsedLine,
                                      const Standard_Integer                            Index,
                                      const Standard_Boolean                            OnFirst,
                                      const Standard_Real                               theToler);

static Standard_Boolean PointOnSecondDom(const IntPatch_SequenceOfPathPointOfTheSOnBounds& listpnt,
                                         const Handle(Adaptor3d_TopolTool)&                Domain,
                                         const IntSurf_Quadric&                            QuadSurf,
                                         const gp_Vec&                                     Normale,
                                         const gp_Vec&                                     Vtgint,
                                         const Handle(IntPatch_Line)&                      lin,
                                         TColStd_Array1OfInteger&                          Done,
                                         const Standard_Integer                            Index,
                                         const Standard_Real theToler);

static Standard_Boolean SingleLine(const gp_Pnt&,
                                   const Handle(IntPatch_Line)&,
                                   const Standard_Real,
                                   Standard_Real&,
                                   gp_Vec&);

static Standard_Boolean FindLine(gp_Pnt&                          Psurf,
                                 const IntPatch_SequenceOfLine&   slin,
                                 const Standard_Real              Tol,
                                 TColStd_ListOfReal&              theLParams,
                                 gp_Vec&                          Vtgtint,
                                 Standard_Integer&                theLineIdx,
                                 Standard_Integer                 OnlyThisLine,
                                 const Handle(Adaptor2d_Curve2d)& thearc,
                                 Standard_Real&                   theparameteronarc,
                                 gp_Pnt&                          thepointonarc,
                                 const IntSurf_Quadric&           QuadSurf1,
                                 const IntSurf_Quadric&           QuadSurf2,
                                 Standard_Real&                   theOutputToler);

static void ProcessSegments(const IntPatch_SequenceOfSegmentOfTheSOnBounds&,
                            IntPatch_SequenceOfLine&,
                            const IntSurf_Quadric&,
                            const IntSurf_Quadric&,
                            const Standard_Boolean,
                            const Standard_Real);

static void ProcessRLine(IntPatch_SequenceOfLine&,
                         const IntSurf_Quadric&,
                         const IntSurf_Quadric&,
                         const Standard_Real,
                         const Standard_Boolean theIsReqToKeepRLine);

//-- le calcul de dist est completement faux ds la routine ci dessous a revoir (lbr le 18 nov 97)
Standard_Boolean IntersectionWithAnArc(gp_Pnt&                          PSurf,
                                       const Handle(IntPatch_ALine)&    alin,
                                       Standard_Real&                   para,
                                       const Handle(Adaptor2d_Curve2d)& thearc,
                                       Standard_Real&                   _theparameteronarc,
                                       gp_Pnt&                          thepointonarc,
                                       const IntSurf_Quadric&           QuadSurf,
                                       const Standard_Real              u0alin,
                                       const Standard_Real              u1alin)
{
  Standard_Real dtheta, theta;
#ifdef OCCT_DEBUG
  // Standard_Real u,v,A,B,C,cost,sint,sign;
#endif
  //-- recherche bete du point le plus proche de thearc->Value(...)
  dtheta           = (u1alin - u0alin) * 0.01;
  Standard_Real du = 0.000000001;
  if (du >= dtheta)
    du = dtheta / 2;
  Standard_Real distmin = RealLast();

  Standard_Real thetamin = 0.;

  Standard_Real theparameteronarc = _theparameteronarc;
  for (Standard_Real _theta = u0alin + dtheta; _theta <= u1alin - dtheta; _theta += dtheta)
  {
    gp_Pnt        P = alin->Value(_theta);
    Standard_Real d = P.Distance(PSurf);
    if (d < distmin)
    {
      thetamin = _theta;
      distmin  = d;
    }
  }

  Standard_Real bestpara = 0., besttheta = 0., bestdist = 0., distinit = 0.;

  //-- Distance initiale
  {
    gp_Pnt        pp0 = alin->Value(thetamin);
    Standard_Real ua0, va0;
    QuadSurf.Parameters(pp0, ua0, va0);
    gp_Pnt2d p2d;
    gp_Vec2d d2d;
    thearc->D1(theparameteronarc, p2d, d2d);
    gp_Vec2d PaPr(gp_Pnt2d(ua0, va0), p2d);
    distinit = PaPr.Magnitude();
  }
  theta = thetamin;
  //-- recherche a partir de theta et theparameteronarc
  Standard_Boolean cpasok = Standard_True;
  Standard_Integer nbiter = 0;
  Standard_Real    drmax  = (thearc->LastParameter() - thearc->FirstParameter()) * 0.05;
  Standard_Real    damax  = (u1alin - u0alin) * 0.05;

  bestdist = RealLast();

  do
  {
    Standard_Real ua0, va0, ua1, va1;
    //-- alin->Curve().InternalUVValue(theta,ua0,va0,A,B,C,cost,sint,sign);
    //-- alin->Curve().InternalUVValue(theta+du,ua1,va1,A,B,C,cost,sint,sign);
    gp_Pnt pp0 = alin->Value(theta);
    gp_Pnt pp1 = alin->Value(theta + du);
    QuadSurf.Parameters(pp0, ua0, va0);
    QuadSurf.Parameters(pp1, ua1, va1);

    gp_Vec2d D1a((ua1 - ua0) / du, (va1 - va0) / du);
    gp_Pnt2d p2d;
    gp_Vec2d d2d;
    thearc->D1(theparameteronarc, p2d, d2d);
    gp_Vec2d PaPr(gp_Pnt2d(ua0, va0), p2d);

    Standard_Real pbd = PaPr.Magnitude();
    if (bestdist > pbd)
    {
      bestdist  = pbd;
      bestpara  = theparameteronarc;
      besttheta = theta;
    }

    D1a.SetCoord(-D1a.X(), -D1a.Y());

    Standard_Real d = D1a.X() * d2d.Y() - D1a.Y() * d2d.X();

    Standard_Real da = (-PaPr.X()) * d2d.Y() - (-PaPr.Y()) * d2d.X();
    Standard_Real dr = D1a.X() * (-PaPr.Y()) - D1a.Y() * (-PaPr.X());
    if (Abs(d) > 1e-15)
    {
      da /= d;
      dr /= d;
    }
    else
    {
      if (Abs(PaPr.X()) > Abs(PaPr.Y()))
      {
        Standard_Real xx = PaPr.X();
        xx *= 0.5;
        if (D1a.X())
        {
          da = -xx / D1a.X();
        }
        if (d2d.X())
        {
          dr = -xx / d2d.X();
        }
      }
      else
      {
        Standard_Real yy = PaPr.Y();
        yy *= 0.5;
        if (D1a.Y())
        {
          da = -yy / D1a.Y();
        }
        if (d2d.Y())
        {
          dr = -yy / d2d.Y();
        }
      }
    }
    //--     Standard_Real da = -PaPr.Dot(D1a);
    //--     Standard_Real dr = -PaPr.Dot(d2d);

    if (da < -damax)
      da = -damax;
    else if (da > damax)
      da = damax;
    if (dr < -drmax)
      dr = -drmax;
    else if (dr > drmax)
      dr = drmax;

    if (Abs(da) < 1e-10 && Abs(dr) < 1e-10)
    {
      para               = theta;
      PSurf              = alin->Value(para);
      _theparameteronarc = theparameteronarc;
      thepointonarc      = alin->Value(para);
      cpasok             = Standard_False;
      //--      printf("\nt:%d",nbiter);
      return (Standard_True);
    }
    else
    {
      theta += da;
      theparameteronarc += dr;
      if (theparameteronarc > thearc->LastParameter())
      {
        theparameteronarc = thearc->LastParameter();
      }
      if (theparameteronarc < thearc->FirstParameter())
      {
        theparameteronarc = thearc->FirstParameter();
      }
      if (theta < u0alin)
      {
        theta = u0alin;
      }
      if (theta > u1alin - du)
      {
        theta = u1alin - du - du;
      }
    }
    nbiter++;
  } while (cpasok && nbiter < 20);
  if (bestdist < distinit)
  {
    para               = besttheta;
    PSurf              = alin->Value(para);
    _theparameteronarc = bestpara;
    thepointonarc      = alin->Value(para);
    //--     printf("\nT:%d",nbiter);
    return (Standard_True);
  }
  //--   printf("\nF:%d",nbiter);
  return (Standard_False);
}

//-- ======================================================================
static void Recadre(const Handle(Adaptor3d_Surface)& myHS1,
                    const Handle(Adaptor3d_Surface)& myHS2,
                    Standard_Real&                   u1,
                    Standard_Real&                   v1,
                    Standard_Real&                   u2,
                    Standard_Real&                   v2)
{
  Standard_Real       f, l, lmf, fpls2;
  GeomAbs_SurfaceType typs1 = myHS1->GetType();
  GeomAbs_SurfaceType typs2 = myHS2->GetType();

  Standard_Boolean myHS1IsUPeriodic, myHS1IsVPeriodic;
  switch (typs1)
  {
    case GeomAbs_Cylinder:
    case GeomAbs_Cone:
    case GeomAbs_Sphere: {
      myHS1IsUPeriodic = Standard_True;
      myHS1IsVPeriodic = Standard_False;
      break;
    }
    case GeomAbs_Torus: {
      myHS1IsUPeriodic = myHS1IsVPeriodic = Standard_True;
      break;
    }
    default: {
      //-- Le cas de biparametrees periodiques est gere en amont
      myHS1IsUPeriodic = myHS1IsVPeriodic = Standard_False;
      break;
    }
  }

  Standard_Boolean myHS2IsUPeriodic, myHS2IsVPeriodic;
  switch (typs2)
  {
    case GeomAbs_Cylinder:
    case GeomAbs_Cone:
    case GeomAbs_Sphere: {
      myHS2IsUPeriodic = Standard_True;
      myHS2IsVPeriodic = Standard_False;
      break;
    }
    case GeomAbs_Torus: {
      myHS2IsUPeriodic = myHS2IsVPeriodic = Standard_True;
      break;
    }
    default: {
      //-- Le cas de biparametrees periodiques est gere en amont
      myHS2IsUPeriodic = myHS2IsVPeriodic = Standard_False;
      break;
    }
  }
  if (myHS1IsUPeriodic)
  {
    lmf   = M_PI + M_PI; //-- myHS1->UPeriod();
    f     = myHS1->FirstUParameter();
    l     = myHS1->LastUParameter();
    fpls2 = 0.5 * (f + l);
    while ((u1 < f) && ((fpls2 - u1) > (u1 + lmf - fpls2)))
    {
      u1 += lmf;
    }
    while ((u1 > l) && ((u1 - fpls2) > (fpls2 - (u1 - lmf))))
    {
      u1 -= lmf;
    }
  }
  if (myHS1IsVPeriodic)
  {
    lmf   = M_PI + M_PI; //-- myHS1->VPeriod();
    f     = myHS1->FirstVParameter();
    l     = myHS1->LastVParameter();
    fpls2 = 0.5 * (f + l);
    while ((v1 < f) && ((fpls2 - v1) > (v1 + lmf - fpls2)))
    {
      v1 += lmf;
    }
    while ((v1 > l) && ((v1 - fpls2) > (fpls2 - (v1 - lmf))))
    {
      v1 -= lmf;
    }
    //--    while(v1 < f) { v1+=lmf; }
    //--    while(v1 > l) { v1-=lmf; }
  }
  if (myHS2IsUPeriodic)
  {
    lmf   = M_PI + M_PI; //-- myHS2->UPeriod();
    f     = myHS2->FirstUParameter();
    l     = myHS2->LastUParameter();
    fpls2 = 0.5 * (f + l);
    while ((u2 < f) && ((fpls2 - u2) > (u2 + lmf - fpls2)))
    {
      u2 += lmf;
    }
    while ((u2 > l) && ((u2 - fpls2) > (fpls2 - (u2 - lmf))))
    {
      u2 -= lmf;
    }
    //-- while(u2 < f) { u2+=lmf; }
    //-- while(u2 > l) { u2-=lmf; }
  }
  if (myHS2IsVPeriodic)
  {
    lmf   = M_PI + M_PI; //-- myHS2->VPeriod();
    f     = myHS2->FirstVParameter();
    l     = myHS2->LastVParameter();
    fpls2 = 0.5 * (f + l);
    while ((v2 < f) && ((fpls2 - v2) > (v2 + lmf - fpls2)))
    {
      v2 += lmf;
    }
    while ((v2 > l) && ((v2 - fpls2) > (fpls2 - (v2 - lmf))))
    {
      v2 -= lmf;
    }
    //-- while(v2 < f) { v2+=lmf; }
    //-- while(v2 > l) { v2-=lmf; }
  }
}

//=================================================================================================

void PutPointsOnLine(const Handle(Adaptor3d_Surface)&                  S1,
                     const Handle(Adaptor3d_Surface)&                  S2,
                     const IntPatch_SequenceOfPathPointOfTheSOnBounds& listpnt,
                     const IntPatch_SequenceOfLine&                    slin,
                     const Standard_Boolean                            OnFirst,
                     const Handle(Adaptor3d_TopolTool)&                Domain,
                     const IntSurf_Quadric&                            QuadSurf,
                     const IntSurf_Quadric&                            OtherQuad,
                     const Standard_Boolean                            multpoint,
                     const Standard_Real                               Tolarc)
{
  // Traitement des point (de listpnt) de depart. On les replace sur
  // la ligne d intersection, en leur affectant la transition correcte sur
  // cette ligne.
  Standard_Integer nbpnt = listpnt.Length();
  Standard_Integer nblin = slin.Length();

  if (!slin.Length() || !nbpnt)
  {
    return;
  }
  //
  Standard_Integer i, k;
  Standard_Integer linenumber;
  Standard_Real    currentparameter, tolerance;
  Standard_Real    U1, V1, U2, V2;
  Standard_Boolean goon;

  gp_Pnt Psurf, ptbid;
  gp_Vec Normale, Vtgint, Vtgrst;

  gp_Vec   d1u, d1v;
  gp_Pnt2d p2d;
  gp_Vec2d d2d;

  IntSurf_Transition Transline, Transarc;

  Handle(Adaptor2d_Curve2d) currentarc;
  Handle(Adaptor3d_HVertex) vtx, vtxbis;

  IntPatch_Point                      solpnt;
  IntPatch_ThePathPointOfTheSOnBounds currentpointonrst;
  IntPatch_IType                      TheType;

  TColStd_Array1OfInteger UsedLine(1, nblin);
  TColStd_Array1OfInteger Done(1, nbpnt);
  for (i = 1; i <= nbpnt; i++)
    Done(i) = 0; //-- Initialisation a la main

  for (i = 1; i <= nbpnt; i++)
  {
    if (Done(i) != 1)
    {
      currentpointonrst = listpnt.Value(i);
      Psurf             = currentpointonrst.Value(); // Point dans l espace
      tolerance         = currentpointonrst.Tolerance();

      // On recherche d abord si on a correspondance avec un "point multiple"
      UsedLine.Init(0);

      goon = Standard_True;
      if (multpoint)
      {
#if 1
        Normale          = QuadSurf.Normale(Psurf); // Normale a la surface au point
        currentarc       = currentpointonrst.Arc();
        currentparameter = currentpointonrst.Parameter();
        currentarc->D1(currentparameter, p2d, d2d);
        QuadSurf.D1(p2d.X(), p2d.Y(), ptbid, d1u, d1v);
        Vtgrst.SetLinearForm(d2d.X(), d1u, d2d.Y(), d1v);
#endif
        goon = MultiplePoint(listpnt,
                             Domain,
                             QuadSurf,
                             Normale,
                             slin,
                             Done,
                             UsedLine,
                             i,
                             OnFirst,
                             Tolarc);
      }
      if (goon)
      {
        Standard_Boolean linefound = Standard_False;

        for (Standard_Integer indiceline = 1; indiceline <= slin.Length(); indiceline++)
        {
          if (UsedLine(indiceline) != 0)
            continue;
          linenumber = indiceline;

          //-- Attention , les points peuvent etre deplaces
          //-- il faut reprendre le point original
          currentpointonrst = listpnt.Value(i);
          currentarc        = currentpointonrst.Arc();
          currentparameter  = currentpointonrst.Parameter();
          Psurf             = currentpointonrst.Value(); // Point dans l espace
          tolerance         = currentpointonrst.Tolerance();
          //--

          //  Modified by skv - Thu Jan 15 15:57:15 2004 OCC4455 Begin
          if (!currentpointonrst.IsNew())
          {
            Handle(Adaptor3d_HVertex) aVtx    = currentpointonrst.Vertex();
            Standard_Real             aVtxTol = aVtx->Resolution(currentarc);
            Standard_Real             aTolAng = 0.01 * tolerance;

            tolerance = Max(tolerance, aVtxTol);

            gp_Vec aNorm1 = QuadSurf.Normale(Psurf);
            gp_Vec aNorm2 = OtherQuad.Normale(Psurf);
            //
            if (aNorm1.Magnitude() > gp::Resolution() && aNorm2.Magnitude() > gp::Resolution())
            {
              if (aNorm1.IsParallel(aNorm2, aTolAng))
                tolerance = Sqrt(tolerance);
            } //
          }
          //  Modified by skv - Thu Jan 15 15:57:15 2004 OCC4455 End
          gp_Pnt pointonarc;
          Vtgint.SetCoord(0, 0, 0);
          Standard_Real      aVertTol = Tolarc;
          TColStd_ListOfReal aLParams;
          linefound = FindLine(Psurf,
                               slin,
                               tolerance,
                               aLParams,
                               Vtgint,
                               linenumber,
                               indiceline,
                               currentarc,
                               currentparameter,
                               pointonarc,
                               QuadSurf,
                               OtherQuad,
                               aVertTol);
          if (linefound)
          {
#if 1
            Normale    = QuadSurf.Normale(Psurf); // Normale a la surface au point
            currentarc = currentpointonrst.Arc();
            //-- currentparameter = currentpointonrst.Parameter();
            currentarc->D1(currentparameter, p2d, d2d);
            QuadSurf.D1(p2d.X(), p2d.Y(), ptbid, d1u, d1v);
            Vtgrst.SetLinearForm(d2d.X(), d1u, d2d.Y(), d1v);
#endif

            const Handle(IntPatch_Line)& lin = slin.Value(linenumber);
            TheType                          = lin->ArcType();

            if (!OnFirst)
            { // on cherche la correspondance entre point sur domaine
              // de la premiere surface et point sur domaine de la
              // deuxieme surface

              goon = PointOnSecondDom(listpnt,
                                      Domain,
                                      QuadSurf,
                                      Normale,
                                      Vtgint,
                                      lin,
                                      Done,
                                      i,
                                      aVertTol);
            }

            if (goon)
            {
              //-- Modification du 4 avril 97    tolerance->Tolarc
              //-- on replace sur le vertex la tolerance d entree et
              //-- non la tolerance qui a servi au FindLine
              solpnt.SetValue(Psurf, aVertTol, Standard_False);

              U1 = p2d.X();
              V1 = p2d.Y();
              OtherQuad.Parameters(Psurf, U2, V2);

              if (OnFirst)
              {
                Recadre(S1, S2, U1, V1, U2, V2);
                solpnt.SetParameters(U1, V1, U2, V2);
              }
              else
              {
                Recadre(S1, S2, U2, V2, U1, V1);
                solpnt.SetParameters(U2, V2, U1, V1);
              }

              if (!currentpointonrst.IsNew())
              {
                vtx = currentpointonrst.Vertex();
                solpnt.SetVertex(OnFirst, vtx);
              }
              else
              {
                //-- goon = Standard_False; ????
              }

              if (Normale.SquareMagnitude() < 1e-16)
              {
                Transline.SetValue(Standard_True, IntSurf_Undecided);
                Transarc.SetValue(Standard_True, IntSurf_Undecided);
              }
              else
              {
                IntSurf::MakeTransition(Vtgint, Vtgrst, Normale, Transline, Transarc);
              }
              solpnt.SetArc(OnFirst, currentarc, currentparameter, Transline, Transarc);

              for (TColStd_ListIteratorOfListOfReal anItr(aLParams); anItr.More(); anItr.Next())
              {
                solpnt.SetParameter(anItr.Value());
                if (TheType == IntPatch_Analytic)
                {
                  Handle(IntPatch_ALine)::DownCast(lin)->AddVertex(solpnt);
                }
                else
                {
                  Handle(IntPatch_GLine)::DownCast(lin)->AddVertex(solpnt);
                }
              }

              Done(i) = 1;

              if (goon)
              {
                for (k = i + 1; k <= nbpnt; k++)
                {
                  if (Done(k) != 1)
                  {
                    currentpointonrst = listpnt.Value(k);
                    if (!currentpointonrst.IsNew())
                    {
                      vtxbis = currentpointonrst.Vertex();
                      if (vtx.IsNull())
                      {
                      }
                      else if (Domain->Identical(vtx, vtxbis))
                      {
                        solpnt.SetVertex(OnFirst, vtxbis);
                        solpnt.SetTolerance(Tolarc);
                        currentarc       = currentpointonrst.Arc();
                        currentparameter = currentpointonrst.Parameter();

                        //		      currentarc->D1(currentparameter,ptbid,Vtgrst);
                        currentarc->D1(currentparameter, p2d, d2d);
                        Vtgrst.SetLinearForm(d2d.X(), d1u, d2d.Y(), d1v);
                        if (Normale.SquareMagnitude() < 1e-16)
                        {
                          Transline.SetValue(Standard_True, IntSurf_Undecided);
                          Transarc.SetValue(Standard_True, IntSurf_Undecided);
                        }
                        else
                        {
                          IntSurf::MakeTransition(Vtgint, Vtgrst, Normale, Transline, Transarc);
                        }
                        solpnt.SetArc(OnFirst, currentarc, currentparameter, Transline, Transarc);
                        if (TheType == IntPatch_Analytic)
                        {
                          Handle(IntPatch_ALine)::DownCast(lin)->AddVertex(solpnt);
                        }
                        else
                        {
                          Handle(IntPatch_GLine)::DownCast(lin)->AddVertex(solpnt);
                        }
                        Done(k) = 1;
                      }
                    }
                  }
                }
              }
            }
          }
          else
          {
            Done(i) = 1; // il faudra tester si IsNew ou pas
            // et traiter en consequence
          }
        }
      }
    }
  }
}

Standard_Boolean MultiplePoint(const IntPatch_SequenceOfPathPointOfTheSOnBounds& listpnt,
                               const Handle(Adaptor3d_TopolTool)&                Domain,
                               const IntSurf_Quadric&                            QuadSurf,
                               const gp_Vec&                                     Normale,
                               const IntPatch_SequenceOfLine&                    slin,
                               TColStd_Array1OfInteger&                          Done,
                               TColStd_Array1OfInteger&                          UsedLine,
                               const Standard_Integer                            Index,
                               const Standard_Boolean                            OnFirst,
                               const Standard_Real                               theToler)
{
  // Traitement des points "multiples".

  Standard_Integer k, ii, jj, nbvtx;
  Standard_Integer nblin = slin.Length();
  IntPatch_IType   TheType;

  IntSurf_Transition Transline, Transarc;

  IntPatch_Point            intpt;
  Handle(Adaptor2d_Curve2d) currentarc;
  Handle(Adaptor3d_HVertex) vtx, vtxbis;

  Standard_Integer                    nbpnt             = listpnt.Length();
  IntPatch_ThePathPointOfTheSOnBounds currentpointonrst = listpnt.Value(Index);
  IntPatch_ThePathPointOfTheSOnBounds otherpt;
  gp_Pnt                              Point = currentpointonrst.Value();
  TColStd_Array1OfInteger             localdone(1, nbpnt);
  localdone.Init(0);
  for (ii = 1; ii <= nbpnt; ii++)
  {
    localdone(ii) = Done(ii);
  }

  Standard_Real currentparameter;
  Standard_Real Paraint;
  gp_Vec        Vtgint, Vtgrst;
  gp_Pnt        ptbid;

  gp_Vec   d1u, d1v;
  gp_Pnt2d p2d;
  gp_Vec2d d2d;

  Standard_Boolean goon;

  Standard_Boolean Retvalue = Standard_True;

  for (ii = 1; ii <= nblin; ii++)
  {
    const Handle(IntPatch_Line)& slinValueii = slin.Value(ii);
    TheType                                  = slinValueii->ArcType();
    if (TheType == IntPatch_Analytic)
    {
      nbvtx = Handle(IntPatch_ALine)::DownCast(slinValueii)->NbVertex();
    }
    else
    {
      nbvtx = Handle(IntPatch_GLine)::DownCast(slinValueii)->NbVertex();
    }
    jj = 1;
    while (jj <= nbvtx)
    {
      if (TheType == IntPatch_Analytic)
      {
        intpt = Handle(IntPatch_ALine)::DownCast(slinValueii)->Vertex(jj);
      }
      else
      {
        intpt = Handle(IntPatch_GLine)::DownCast(slinValueii)->Vertex(jj);
      }
      if (intpt.IsMultiple()
          && ((OnFirst && !intpt.IsOnDomS1()) || (!OnFirst && !intpt.IsOnDomS2())))
      {
        if (Point.Distance(intpt.Value()) <= intpt.Tolerance())
        {
          Retvalue             = Standard_False;
          Standard_Boolean foo = SingleLine(Point, slinValueii, intpt.Tolerance(), Paraint, Vtgint);
          if (!foo)
          {
            return Standard_False; // ne doit pas se produire
          }

          if (!currentpointonrst.IsNew())
          {
            goon = Standard_True;
            vtx  = currentpointonrst.Vertex();
            intpt.SetVertex(OnFirst, vtx);
          }
          else
          {
            goon = Standard_False;
          }
          currentarc       = currentpointonrst.Arc();
          currentparameter = currentpointonrst.Parameter();
          currentarc->D1(currentparameter, p2d, d2d);
          QuadSurf.D1(p2d.X(), p2d.Y(), ptbid, d1u, d1v);
          Vtgrst.SetLinearForm(d2d.X(), d1u, d2d.Y(), d1v);

          //-- Si la normale est nulle (apex d un cone) On simule une transition UNKNOWN
          if (Normale.SquareMagnitude() < 1e-16)
          {
            Transline.SetValue(Standard_True, IntSurf_Undecided);
            Transarc.SetValue(Standard_True, IntSurf_Undecided);
          }
          else
          {
            IntSurf::MakeTransition(Vtgint, Vtgrst, Normale, Transline, Transarc);
          }

          //-- Avant, on ne mettait pas ce point (17 nov 97)
          //--printf("\n ImpImp_0  : Point(%g,%g,%g)  intpt(%g,%g,%g) \n",
          //-- Point.X(),Point.Y(),Point.Z(),intpt.Value().X(),intpt.Value().Y(),intpt.Value().Z());
          intpt.SetValue(Point);

          intpt.SetArc(OnFirst, currentarc, currentparameter, Transline, Transarc);
          intpt.SetTolerance(theToler);

          if (TheType == IntPatch_Analytic)
          {
            Handle(IntPatch_ALine)::DownCast(slinValueii)->Replace(jj, intpt);
          }
          else
          {
            Handle(IntPatch_GLine)::DownCast(slinValueii)->Replace(jj, intpt);
          }
          localdone(Index) = 1;
          if (goon)
          {
            for (k = Index + 1; k <= nbpnt; k++)
            {
              if (Done(k) != 1)
              {
                otherpt = listpnt.Value(k);
                if (!otherpt.IsNew())
                {
                  vtxbis = otherpt.Vertex();
                  if (Domain->Identical(vtx, vtxbis))
                  {
                    intpt.SetVertex(OnFirst, vtxbis);
                    currentarc       = otherpt.Arc();
                    currentparameter = otherpt.Parameter();

                    currentarc->D1(currentparameter, p2d, d2d);
                    Vtgrst.SetLinearForm(d2d.X(), d1u, d2d.Y(), d1v);
                    if (Normale.SquareMagnitude() < 1e-16)
                    {
                      Transline.SetValue(Standard_True, IntSurf_Undecided);
                      Transarc.SetValue(Standard_True, IntSurf_Undecided);
                    }
                    else
                    {
                      IntSurf::MakeTransition(Vtgint, Vtgrst, Normale, Transline, Transarc);
                    }
                    intpt.SetArc(OnFirst, currentarc, currentparameter, Transline, Transarc);
                    intpt.SetTolerance(theToler);
                    if (TheType == IntPatch_Analytic)
                    {
                      Handle(IntPatch_ALine)::DownCast(slinValueii)->AddVertex(intpt);
                    }
                    else
                    {
                      Handle(IntPatch_GLine)::DownCast(slinValueii)->AddVertex(intpt);
                    }
                    UsedLine(ii) = 1;
                    Retvalue     = Standard_True;
                    localdone(k) = 1;
                  }
                }
              }
            }
          }
          //--           jj = nbvtx +1;
        }
        //--         else {
        jj = jj + 1;
        //--        }
      }
      else
      {
        jj = jj + 1;
      }
    }
  }

  for (ii = 1; ii <= nbpnt; ii++)
  {
    Done(ii) = localdone(ii);
  }

  return Retvalue;
}

Standard_Boolean PointOnSecondDom(const IntPatch_SequenceOfPathPointOfTheSOnBounds& listpnt,
                                  const Handle(Adaptor3d_TopolTool)&                Domain,
                                  const IntSurf_Quadric&                            QuadSurf,
                                  const gp_Vec&                                     Normale,
                                  const gp_Vec&                                     Vtgint,
                                  const Handle(IntPatch_Line)&                      lin,
                                  TColStd_Array1OfInteger&                          Done,
                                  const Standard_Integer                            Index,
                                  const Standard_Real                               theToler)

// Duplication des points sur domaine de l autre surface.
// On sait que le vertex sous-jacent est PntRef

{
  Standard_Integer k, jj, nbvtx;
  IntPatch_IType   TheType;

  IntSurf_Transition        Transline, Transarc;
  IntPatch_Point            intpt;
  Handle(Adaptor2d_Curve2d) currentarc;
  Handle(Adaptor3d_HVertex) vtx, vtxbis;
  gp_Pnt                    ptbid;
  gp_Vec                    Vtgrst;

  gp_Vec   d1u, d1v;
  gp_Pnt2d p2d;
  gp_Vec2d d2d;

  Standard_Integer                    nbpnt             = listpnt.Length();
  IntPatch_ThePathPointOfTheSOnBounds currentpointonrst = listpnt.Value(Index);
  Standard_Real                       currentparameter;

  Standard_Boolean goon;
  Standard_Boolean Retvalue = Standard_True;

  TheType = lin->ArcType();
  if (TheType == IntPatch_Analytic)
  {
    nbvtx = Handle(IntPatch_ALine)::DownCast(lin)->NbVertex();
  }
  else
  {
    nbvtx = Handle(IntPatch_GLine)::DownCast(lin)->NbVertex();
  }
  jj = 1;
  while (jj <= nbvtx)
  {
    if (TheType == IntPatch_Analytic)
    {
      intpt = Handle(IntPatch_ALine)::DownCast(lin)->Vertex(jj);
    }
    else
    {
      intpt = Handle(IntPatch_GLine)::DownCast(lin)->Vertex(jj);
    }
    if (!intpt.IsOnDomS2())
    {
      if (currentpointonrst.Value().Distance(intpt.Value()) <= intpt.Tolerance())
      {
        Retvalue = Standard_False;
        if (!currentpointonrst.IsNew())
        {
          goon = Standard_True;
          vtx  = currentpointonrst.Vertex();
          intpt.SetVertex(Standard_False, vtx);
        }
        else
        {
          goon = Standard_False;
        }
        currentarc       = currentpointonrst.Arc();
        currentparameter = currentpointonrst.Parameter();
        currentarc->D1(currentparameter, p2d, d2d);
        QuadSurf.D1(p2d.X(), p2d.Y(), ptbid, d1u, d1v);
        Vtgrst.SetLinearForm(d2d.X(), d1u, d2d.Y(), d1v);
        if (Normale.SquareMagnitude() < 1e-16)
        {
          Transline.SetValue(Standard_True, IntSurf_Undecided);
          Transarc.SetValue(Standard_True, IntSurf_Undecided);
        }
        else
        {
          IntSurf::MakeTransition(Vtgint, Vtgrst, Normale, Transline, Transarc);
        }
        intpt.SetArc(Standard_False, currentarc, currentparameter, Transline, Transarc);
        intpt.SetTolerance(theToler);

        if (TheType == IntPatch_Analytic)
        {
          Handle(IntPatch_ALine)::DownCast(lin)->Replace(jj, intpt);
        }
        else
        {
          Handle(IntPatch_GLine)::DownCast(lin)->Replace(jj, intpt);
        }
        Done(Index) = 1;

        if (goon)
        {
          for (k = Index + 1; k <= nbpnt; k++)
          {
            if (Done(k) != 1)
            {
              currentpointonrst = listpnt.Value(k);
              if (!currentpointonrst.IsNew())
              {
                vtxbis = currentpointonrst.Vertex();
                if (Domain->Identical(vtx, vtxbis))
                {
                  intpt.SetVertex(Standard_False, vtxbis);
                  currentarc       = currentpointonrst.Arc();
                  currentparameter = currentpointonrst.Parameter();
                  currentarc->D1(currentparameter, p2d, d2d);
                  Vtgrst.SetLinearForm(d2d.X(), d1u, d2d.Y(), d1v);
                  if (Normale.SquareMagnitude() < 1e-16)
                  {
                    Transline.SetValue(Standard_True, IntSurf_Undecided);
                    Transarc.SetValue(Standard_True, IntSurf_Undecided);
                  }
                  else
                  {
                    IntSurf::MakeTransition(Vtgint, Vtgrst, Normale, Transline, Transarc);
                  }
                  intpt.SetArc(Standard_False, currentarc, currentparameter, Transline, Transarc);
                  intpt.SetTolerance(theToler);
                  if (TheType == IntPatch_Analytic)
                  {
                    Handle(IntPatch_ALine)::DownCast(lin)->AddVertex(intpt);
                  }
                  else
                  {
                    Handle(IntPatch_GLine)::DownCast(lin)->AddVertex(intpt);
                  }
                  Done(k) = 1;
                }
              }
            }
          }
        }
        //-- jj = nbvtx + 1;
        jj++;
      }
      else
      {
        jj = jj + 1;
      }
    }
    else
    {
      jj = jj + 1;
    }
    if (TheType == IntPatch_Analytic)
    {
      nbvtx = Handle(IntPatch_ALine)::DownCast(lin)->NbVertex();
    }
    else
    {
      nbvtx = Handle(IntPatch_GLine)::DownCast(lin)->NbVertex();
    }
  }
  return Retvalue;
}

Standard_Boolean FindLine(gp_Pnt&                          Psurf,
                          const IntPatch_SequenceOfLine&   slin,
                          const Standard_Real              Tol,
                          TColStd_ListOfReal&              theLParams,
                          gp_Vec&                          Vtgtint,
                          Standard_Integer&                theLineIdx,
                          Standard_Integer                 OnlyThisLine,
                          const Handle(Adaptor2d_Curve2d)& thearc,
                          Standard_Real&                   theparameteronarc,
                          gp_Pnt&                          thepointonarc,
                          const IntSurf_Quadric&           QuadSurf1,
                          const IntSurf_Quadric&           QuadSurf2,
                          Standard_Real&                   theOutputToler)
{
  if ((QuadSurf1.Distance(Psurf) > Tol) || (QuadSurf2.Distance(Psurf) > Tol))
    return Standard_False;

  // Traitement du point de depart ayant pour representation Psurf
  // dans l espace. On recherche la ligne d intersection contenant ce point.
  // On a en sortie la ligne, et le parametre et sa tangente du point sur
  // la ligne d intersection.
  const Standard_Real aSqTol     = Tol * Tol;
  Standard_Real       aSqDistMin = RealLast();
  Standard_Real       aSqDist, para;
  Standard_Real       lower, upper;
  gp_Pnt              pt;
  Standard_Integer    i;
  IntPatch_IType      typarc;

  Standard_Real    aParaInt = RealLast();
  Standard_Integer nblin    = slin.Length();
  for (i = 1; i <= nblin; i++)
  {
    if (OnlyThisLine)
    {
      i     = OnlyThisLine;
      nblin = 0;
    }
    const Handle(IntPatch_Line)& lin = slin.Value(i);
    typarc                           = lin->ArcType();
    if (typarc == IntPatch_Analytic)
    {
      Standard_Boolean foo;
      lower = Handle(IntPatch_ALine)::DownCast(lin)->FirstParameter(foo);
      upper = Handle(IntPatch_ALine)::DownCast(lin)->LastParameter(foo);
    }
    else
    {
      if (Handle(IntPatch_GLine)::DownCast(lin)->HasFirstPoint())
      {
        lower = Handle(IntPatch_GLine)::DownCast(lin)->FirstPoint().ParameterOnLine();
      }
      else
      {
        lower = RealFirst();
      }
      if (Handle(IntPatch_GLine)::DownCast(lin)->HasLastPoint())
      {
        upper = Handle(IntPatch_GLine)::DownCast(lin)->LastPoint().ParameterOnLine();
      }
      else
      {
        upper = RealLast();
      }
    }

    switch (typarc)
    {
      case IntPatch_Lin: {
        para = ElCLib::Parameter(Handle(IntPatch_GLine)::DownCast(lin)->Line(), Psurf);
        if (para <= upper && para >= lower)
        {
          pt      = ElCLib::Value(para, Handle(IntPatch_GLine)::DownCast(lin)->Line());
          aSqDist = Psurf.SquareDistance(pt);
          if ((aSqDist < aSqTol) && (aSqDist < aSqDistMin))
          {
            aSqDistMin = aSqDist;
            aParaInt   = para;
            theLineIdx = i;
          }
        }
      }
      break;
      case IntPatch_Circle: {
        para = ElCLib::Parameter(Handle(IntPatch_GLine)::DownCast(lin)->Circle(), Psurf);
        if ((para <= upper && para >= lower)
            || (para + 2. * M_PI <= upper && para + 2. * M_PI >= lower)
            || (para - 2. * M_PI <= upper && para - 2. * M_PI >= lower))
        {
          pt      = ElCLib::Value(para, Handle(IntPatch_GLine)::DownCast(lin)->Circle());
          aSqDist = Psurf.SquareDistance(pt);
          if ((aSqDist < aSqTol) && (aSqDist < aSqDistMin))
          {
            aSqDistMin = aSqDist;
            aParaInt   = para;
            theLineIdx = i;
          }
        }
      }
      break;
      case IntPatch_Ellipse: {
        para = ElCLib::Parameter(Handle(IntPatch_GLine)::DownCast(lin)->Ellipse(), Psurf);
        if ((para <= upper && para >= lower)
            || (para + 2. * M_PI <= upper && para + 2. * M_PI >= lower)
            || (para - 2. * M_PI <= upper && para - 2. * M_PI >= lower))
        {
          pt      = ElCLib::Value(para, Handle(IntPatch_GLine)::DownCast(lin)->Ellipse());
          aSqDist = Psurf.SquareDistance(pt);
          if ((aSqDist < aSqTol) && (aSqDist < aSqDistMin))
          {
            aSqDistMin = aSqDist;
            aParaInt   = para;
            theLineIdx = i;
          }
        }
      }
      break;
      case IntPatch_Parabola: {
#if 0 	
	para = ElCLib::Parameter(Handle(IntPatch_GLine)::DownCast (lin)->Parabola(),Psurf);
	if (para <= upper && para >= lower) {
	  pt = ElCLib::Value(para,Handle(IntPatch_GLine)::DownCast (lin)->Parabola());
	  dist = Psurf.Distance(pt);
	  if (dist< distmin) {
	    distmin = dist;
	    Paraint = para;
	    Range = i;
	  }
	}
#else
        //-- Le calcul du parametre sur une parabole est mal fait ds ElCLib. Il ne tient pas compte
        //-- de la meilleure facon de calculer (axe X ou axe Y). Bilan : Si la parabole est tres
        //-- pointue (focal de l'ordre de 1e-2 et si le point est a un parametre grand, ca foire. )
        //-- On ne peut pas modifier faciolement ds ElCLib car on ne passe pas la focale. ...
        const gp_Parab& Parab = Handle(IntPatch_GLine)::DownCast(lin)->Parabola();
        para                  = ElCLib::Parameter(Parab, Psurf);
        if (para <= upper && para >= lower)
        {
          Standard_Integer amelioration = 0;
          //-- cout<<"\n ****** \n";
          do
          {
            Standard_Real parabis = para + 0.0000001;

            pt      = ElCLib::Value(para, Parab);
            aSqDist = Psurf.SquareDistance(pt);

            const gp_Pnt        ptbis   = ElCLib::Value(parabis, Parab);
            const Standard_Real distbis = Psurf.Distance(ptbis);
            const Standard_Real aDist   = Sqrt(aSqDist);
            const Standard_Real ddist   = distbis - aDist;

            //--cout<<" para: "<<para<<"    dist:"<<dist<<"   ddist:"<<ddist<<endl;

            if ((aSqDist < aSqTol) && (aSqDist < aSqDistMin))
            {
              aSqDistMin = aSqDist;
              aParaInt   = para;
              theLineIdx = i;
            }
            if (aSqDist < Precision::SquarePConfusion())
            {
              amelioration = 100;
            }

            if (ddist > 1.0e-9 || ddist < -1.0e-9)
            {
              para = para - aDist * (parabis - para) / ddist;
            }
            else
            {
              amelioration = 100;
            }
          } while (++amelioration < 5);
        }

#endif
      }
      break;
      case IntPatch_Hyperbola: {
        para = ElCLib::Parameter(Handle(IntPatch_GLine)::DownCast(lin)->Hyperbola(), Psurf);
        if (para <= upper && para >= lower)
        {
          pt      = ElCLib::Value(para, Handle(IntPatch_GLine)::DownCast(lin)->Hyperbola());
          aSqDist = Psurf.SquareDistance(pt);
          if ((aSqDist < aSqTol) && (aSqDist < aSqDistMin))
          {
            aSqDistMin = aSqDist;
            aParaInt   = para;
            theLineIdx = i;
          }
        }
      }
      break;

      case IntPatch_Analytic: {
        Handle(IntPatch_ALine) alin(Handle(IntPatch_ALine)::DownCast(lin));
        TColStd_ListOfReal     aLParams;
        alin->FindParameter(Psurf, aLParams);
        if (!aLParams.IsEmpty())
        {
          // All found distances are already in some internal tolerance
          // set in alin->FindParameter(...) method.

          aSqDist = RealLast();
          for (TColStd_ListIteratorOfListOfReal anItr(aLParams); anItr.More(); anItr.Next())
          {
            pt                       = alin->Value(anItr.Value());
            const Standard_Real aSqD = Psurf.SquareDistance(pt);
            if (aSqD < aSqDist)
            {
              aSqDist = aSqD;
            }
          }

          if (aSqDist < aSqDistMin)
          {
            aSqDistMin = aSqDist;
            theLParams = aLParams;
            theLineIdx = i;
          }
        }
        else
        {
          //-- le point n a pas ete trouve par bete projection.
          //-- on essaie l intersection avec la restriction en 2d
          Standard_Real theparamonarc = theparameteronarc;
          // #ifdef OCCT_DEBUG
          //	  Standard_Real anpara=para;
          // #endif
          gp_Pnt           CopiePsurf    = Psurf;
          Standard_Boolean IntersectIsOk = IntersectionWithAnArc(CopiePsurf,
                                                                 alin,
                                                                 para,
                                                                 thearc,
                                                                 theparamonarc,
                                                                 thepointonarc,
                                                                 QuadSurf1,
                                                                 lower,
                                                                 upper);
          aSqDist                        = CopiePsurf.SquareDistance(Psurf);
          if (IntersectIsOk)
          {
            if (aSqDist < aSqTol)
            {
              theparameteronarc = theparamonarc;
              Psurf             = thepointonarc;
              aSqDistMin        = aSqDist;
              theLParams.Append(para);
              theLineIdx = i;
            }
          }
        }
      }
      break;

      case IntPatch_Walking: // impossible . c est pour eviter les warnings
      {
      }
      case IntPatch_Restriction: // impossible . c est pour eviter les warnings
      {
      }
    }
  }

  if (aSqDistMin == RealLast())
    return Standard_False;

  theOutputToler = Max(theOutputToler, Sqrt(aSqDistMin));

  typarc = slin.Value(theLineIdx)->ArcType();

  // Computation of tangent vector
  switch (typarc)
  {
    case IntPatch_Lin:
      theLParams.Append(aParaInt);
      Vtgtint = (*((Handle(IntPatch_GLine)*)&slin(theLineIdx)))->Line().Direction();
      break;
    case IntPatch_Circle:
      theLParams.Append(aParaInt);
      Vtgtint = ElCLib::DN(aParaInt, (*((Handle(IntPatch_GLine)*)&slin(theLineIdx)))->Circle(), 1);
      break;
    case IntPatch_Ellipse:
      theLParams.Append(aParaInt);
      Vtgtint = ElCLib::DN(aParaInt, (*((Handle(IntPatch_GLine)*)&slin(theLineIdx)))->Ellipse(), 1);
      break;
    case IntPatch_Parabola:
      theLParams.Append(aParaInt);
      Vtgtint =
        ElCLib::DN(aParaInt, (*((Handle(IntPatch_GLine)*)&slin(theLineIdx)))->Parabola(), 1);
      break;
    case IntPatch_Hyperbola:
      theLParams.Append(aParaInt);
      Vtgtint =
        ElCLib::DN(aParaInt, (*((Handle(IntPatch_GLine)*)&slin(theLineIdx)))->Hyperbola(), 1);
      break;

    case IntPatch_Analytic: {
      if (!Handle(IntPatch_ALine)::DownCast(slin(theLineIdx))->D1(theLParams.Last(), pt, Vtgtint))
      {
        // Previously (before the fix #29807) this code tried to process case
        // when Handle(IntPatch_ALine)::D1(...) method returns FALSE and
        // computed Vtgtint input argument value. Currently, any singularities
        // must be processed by high-level algorithms (IntPatch_SpecialPoints class).
        // Therefore this code has been deleted as deprecated.

        Vtgtint.SetCoord(0.0, 0.0, 0.0);
      }
    }
    break;
    case IntPatch_Walking: // impossible . c est pour eviter les warnings
    {
    }
    case IntPatch_Restriction: // impossible . c est pour eviter les warnings
    {
    }
  }
  return Standard_True;
}

//=======================================================================
// function : SingleLine
// purpose  : Traitement du point de depart ayant pour representation Psurf
//            dans l espace. On le replace sur la ligne d intersection; On a en sortie
//            son parametre et sa tangente sur la ligne d intersection.
//            La fonction renvoie False si le point projete est a une distance
//            superieure a Tol du point a projeter.
//=======================================================================
Standard_Boolean SingleLine(const gp_Pnt&                Psurf,
                            const Handle(IntPatch_Line)& lin,
                            const Standard_Real          Tol,
                            Standard_Real&               Paraint,
                            gp_Vec&                      Vtgtint)
{
  IntPatch_IType typarc = lin->ArcType();

  Standard_Real    parproj = 0.;
  gp_Vec           tgint;
  gp_Pnt           ptproj;
  Standard_Boolean retvalue;

  switch (typarc)
  {
    case IntPatch_Lin:
      parproj = ElCLib::Parameter(Handle(IntPatch_GLine)::DownCast(lin)->Line(), Psurf);
      ElCLib::D1(parproj, Handle(IntPatch_GLine)::DownCast(lin)->Line(), ptproj, tgint);
      break;
    case IntPatch_Circle:
      parproj = ElCLib::Parameter(Handle(IntPatch_GLine)::DownCast(lin)->Circle(), Psurf);
      ElCLib::D1(parproj, Handle(IntPatch_GLine)::DownCast(lin)->Circle(), ptproj, tgint);
      break;
    case IntPatch_Ellipse:
      parproj = ElCLib::Parameter(Handle(IntPatch_GLine)::DownCast(lin)->Ellipse(), Psurf);
      ElCLib::D1(parproj, Handle(IntPatch_GLine)::DownCast(lin)->Ellipse(), ptproj, tgint);
      break;
    case IntPatch_Parabola:
      parproj = ElCLib::Parameter(Handle(IntPatch_GLine)::DownCast(lin)->Parabola(), Psurf);
      ElCLib::D1(parproj, Handle(IntPatch_GLine)::DownCast(lin)->Parabola(), ptproj, tgint);
      break;
    case IntPatch_Hyperbola:
      parproj = ElCLib::Parameter(Handle(IntPatch_GLine)::DownCast(lin)->Hyperbola(), Psurf);
      ElCLib::D1(parproj, Handle(IntPatch_GLine)::DownCast(lin)->Hyperbola(), ptproj, tgint);
      break;
    case IntPatch_Analytic: {
      Handle(IntPatch_ALine) alin(Handle(IntPatch_ALine)::DownCast(lin));
      TColStd_ListOfReal     aLParams;
      alin->FindParameter(Psurf, aLParams);
      if (!aLParams.IsEmpty())
      {
        ptproj  = Psurf;
        parproj = aLParams.Last();
        gp_Pnt aPtemp;
        if (!alin->D1(parproj, aPtemp, tgint))
        {
          // Previously (before the fix #29807) this code tried to process case
          // when Handle(IntPatch_ALine)::D1(...) method returns FALSE and
          // computed Vtgtint input argument value. Currently, any singularities
          // must be processed by high-level algorithms (IntPatch_SpecialPoints class).
          // Therefore this code has been deleted as deprecated.

          tgint.SetCoord(0.0, 0.0, 0.0);
        }
      }
      else
      {
        //-- cout << "---- Pb sur ligne analytique dans SingleLine" << endl;
        //-- cout << "     Find Parameter"<<endl;
        return Standard_False;
      }
    }
    break;
    case IntPatch_Walking: // impossible . c est pour eviter les warnings
    {
    }
    case IntPatch_Restriction: // impossible . c est pour eviter les warnings
    {
    }
  }

  if (Psurf.Distance(ptproj) <= Tol)
  {
    Paraint  = parproj;
    Vtgtint  = tgint;
    retvalue = Standard_True;
  }
  else
  {
    retvalue = Standard_False;
  }
  return retvalue;
}

void ProcessSegments(const IntPatch_SequenceOfSegmentOfTheSOnBounds& listedg,
                     IntPatch_SequenceOfLine&                        slin,
                     const IntSurf_Quadric&                          Quad1,
                     const IntSurf_Quadric&                          Quad2,
                     const Standard_Boolean                          OnFirst,
                     const Standard_Real                             TolArc)
{
  Standard_Integer i, j, k;
  Standard_Integer nbedg = listedg.Length();
  Standard_Integer Nblines, Nbpts;

  Handle(Adaptor2d_Curve2d) arcRef;
  IntPatch_Point            ptvtx, newptvtx;

  Handle(IntPatch_RLine) rline; //-- On fait rline = new ... par la suite

  IntPatch_TheSegmentOfTheSOnBounds   thesegsol;
  IntPatch_ThePathPointOfTheSOnBounds PStartf, PStartl;
  Standard_Boolean                    dofirst, dolast, procf, procl;

  Standard_Real paramf = 0., paraml = 0., U1 = 0., V1 = 0., U2 = 0., V2 = 0.;

  IntPatch_IType     typ;
  IntSurf_TypeTrans  trans1, trans2;
  IntSurf_Transition TRest, TArc;
  gp_Vec             tgline, norm1, norm2, tgarc;
  gp_Pnt             valpt;

  gp_Vec   d1u, d1v;
  gp_Pnt2d p2d;
  gp_Vec2d d2d;

  for (i = 1; i <= nbedg; i++)
  {
    Standard_Boolean EdgeDegenere = Standard_False;
    thesegsol                     = listedg.Value(i);
    arcRef                        = thesegsol.Curve();

    rline = new IntPatch_RLine(Standard_False);
    if (OnFirst)
    {
      rline->SetArcOnS1(arcRef);
    }
    else
    {
      rline->SetArcOnS2(arcRef);
    }

    // Traitement des points debut/fin du segment solution.

    dofirst = Standard_False;
    dolast  = Standard_False;
    procf   = Standard_False;
    procl   = Standard_False;

    if (thesegsol.HasFirstPoint())
    {
      dofirst = Standard_True;
      PStartf = thesegsol.FirstPoint();
      paramf  = PStartf.Parameter();
    }
    if (thesegsol.HasLastPoint())
    {
      dolast  = Standard_True;
      PStartl = thesegsol.LastPoint();
      paraml  = PStartl.Parameter();
    }

    if (dofirst && dolast)
    { // determination de la transition de la ligne
      arcRef->D1(0.5 * (paramf + paraml), p2d, d2d);
      if (OnFirst)
      {
        Quad1.D1(p2d.X(), p2d.Y(), valpt, d1u, d1v);
      }
      else
      {
        Quad2.D1(p2d.X(), p2d.Y(), valpt, d1u, d1v);
      }
      tgline.SetLinearForm(d2d.X(), d1u, d2d.Y(), d1v);

      if (d1u.Magnitude() < 1e-7)
      { //-- edge degenere ?
        EdgeDegenere = Standard_True;
        for (Standard_Integer edg = 0; edg <= 10; edg++)
        {
          arcRef->D1(paramf + (paraml - paramf) * edg * 0.1, p2d, d2d);
          if (OnFirst)
          {
            Quad1.D1(p2d.X(), p2d.Y(), valpt, d1u, d1v);
          }
          else
          {
            Quad2.D1(p2d.X(), p2d.Y(), valpt, d1u, d1v);
          }

          if (d1u.Magnitude() > 1e-7)
          {
            EdgeDegenere = Standard_False;
          }
        }
        rline = new IntPatch_RLine(Standard_False);
        if (OnFirst)
        {
          rline->SetArcOnS1(arcRef);
        }
        else
        {
          rline->SetArcOnS2(arcRef);
        }
      }
      else
      {
        norm2 = Quad2.Normale(valpt);
        norm1 = Quad1.Normale(valpt);

        if (tgline.DotCross(norm2, norm1) > 0.000000001)
        {
          trans1 = IntSurf_Out;
          trans2 = IntSurf_In;
        }
        else if (tgline.DotCross(norm2, norm1) < -0.000000001)
        {
          trans1 = IntSurf_In;
          trans2 = IntSurf_Out;
        }
        else
        {
          trans1 = trans2 = IntSurf_Undecided;
        }
        rline = new IntPatch_RLine(Standard_False, trans1, trans2);
        if (OnFirst)
        {
          rline->SetArcOnS1(arcRef);
        }
        else
        {
          rline->SetArcOnS2(arcRef);
        }
      }
    }
    else
    {
      rline = new IntPatch_RLine(Standard_False);
      if (OnFirst)
      {
        rline->SetArcOnS1(arcRef);
      }
      else
      {
        rline->SetArcOnS2(arcRef);
      }
    }

    if (dofirst || dolast)
    {
      Nblines = slin.Length();
      for (j = 1; j <= Nblines; j++)
      {
        const Handle(IntPatch_Line)& slinj = slin(j);
        typ                                = slinj->ArcType();
        if (typ == IntPatch_Analytic)
        {
          Nbpts = Handle(IntPatch_ALine)::DownCast(slinj)->NbVertex();
        }
        else if (typ == IntPatch_Restriction)
        {
          Nbpts = Handle(IntPatch_RLine)::DownCast(slinj)->NbVertex();
        }
        else
        {
          Nbpts = Handle(IntPatch_GLine)::DownCast(slinj)->NbVertex();
        }
        for (k = 1; k <= Nbpts; k++)
        {
          if (typ == IntPatch_Analytic)
          {
            ptvtx = Handle(IntPatch_ALine)::DownCast(slinj)->Vertex(k);
          }
          else if (typ == IntPatch_Restriction)
          {
            ptvtx = Handle(IntPatch_RLine)::DownCast(slinj)->Vertex(k);
          }
          else
          {
            ptvtx = Handle(IntPatch_GLine)::DownCast(slinj)->Vertex(k);
          }

          if (EdgeDegenere == Standard_False && dofirst)
          {
            if (ptvtx.Value().Distance(PStartf.Value()) <= TolArc)
            {
              ptvtx.SetMultiple(Standard_True);
              ptvtx.SetTolerance(TolArc);
              if (typ == IntPatch_Analytic)
              {
                Handle(IntPatch_ALine)::DownCast(slinj)->Replace(k, ptvtx);
              }
              else if (typ == IntPatch_Restriction)
              {
                Handle(IntPatch_RLine)::DownCast(slinj)->Replace(k, ptvtx);
              }
              else
              {
                Handle(IntPatch_GLine)::DownCast(slinj)->Replace(k, ptvtx);
              }
              newptvtx = ptvtx;
              newptvtx.SetParameter(paramf);
              // Recalcul des  transitions si point sur restriction

              arcRef->D1(paramf, p2d, d2d);
              if (OnFirst)
              {
                Quad1.D1(p2d.X(), p2d.Y(), valpt, d1u, d1v);
              }
              else
              {
                Quad2.D1(p2d.X(), p2d.Y(), valpt, d1u, d1v);
              }
              tgline.SetLinearForm(d2d.X(), d1u, d2d.Y(), d1v);
              if (ptvtx.IsOnDomS1())
              {
                const Handle(Adaptor2d_Curve2d)& thearc = ptvtx.ArcOnS1();
                thearc->D1(ptvtx.ParameterOnArc1(), p2d, d2d);
                Quad1.D1(p2d.X(), p2d.Y(), valpt, d1u, d1v);
                tgarc.SetLinearForm(d2d.X(), d1u, d2d.Y(), d1v);
                norm1 = d1u.Crossed(d1v);
                if (norm1.SquareMagnitude() < 1e-16)
                {
                  TRest.SetValue(Standard_True, IntSurf_Undecided);
                  TArc.SetValue(Standard_True, IntSurf_Undecided);
                }
                else
                {
                  IntSurf::MakeTransition(tgline, tgarc, norm1, TRest, TArc);
                }
                newptvtx.SetArc(Standard_True, thearc, ptvtx.ParameterOnArc1(), TRest, TArc);
              }
              if (ptvtx.IsOnDomS2())
              {
                const Handle(Adaptor2d_Curve2d)& thearc = ptvtx.ArcOnS2();
                thearc->D1(ptvtx.ParameterOnArc2(), p2d, d2d);
                Quad2.D1(p2d.X(), p2d.Y(), valpt, d1u, d1v);
                tgarc.SetLinearForm(d2d.X(), d1u, d2d.Y(), d1v);
                norm2 = d1u.Crossed(d1v);
                if (norm2.SquareMagnitude() < 1e-16)
                {
                  TRest.SetValue(Standard_True, IntSurf_Undecided);
                  TArc.SetValue(Standard_True, IntSurf_Undecided);
                }
                else
                {
                  IntSurf::MakeTransition(tgline, tgarc, norm2, TRest, TArc);
                }
                newptvtx.SetArc(Standard_False, thearc, ptvtx.ParameterOnArc2(), TRest, TArc);
              }

              rline->AddVertex(newptvtx);
              if (!procf)
              {
                procf = Standard_True;
                rline->SetFirstPoint(rline->NbVertex());
              }
            }
          }
          if (EdgeDegenere == Standard_False && dolast)
          {
            if (ptvtx.Value().Distance(PStartl.Value()) <= TolArc)
            {
              ptvtx.SetMultiple(Standard_True);
              ptvtx.SetTolerance(TolArc);
              if (typ == IntPatch_Analytic)
              {
                Handle(IntPatch_ALine)::DownCast(slinj)->Replace(k, ptvtx);
              }
              else if (typ == IntPatch_Restriction)
              {
                Handle(IntPatch_RLine)::DownCast(slinj)->Replace(k, ptvtx);
              }
              else
              {
                Handle(IntPatch_GLine)::DownCast(slinj)->Replace(k, ptvtx);
              }

              newptvtx = ptvtx;
              newptvtx.SetParameter(paraml);
              // Recalcul des  transitions si point sur restriction

              arcRef->D1(paraml, p2d, d2d);
              if (OnFirst)
              {
                Quad1.D1(p2d.X(), p2d.Y(), valpt, d1u, d1v);
              }
              else
              {
                Quad2.D1(p2d.X(), p2d.Y(), valpt, d1u, d1v);
              }
              tgline.SetLinearForm(d2d.X(), d1u, d2d.Y(), d1v);
              if (ptvtx.IsOnDomS1())
              {
                const Handle(Adaptor2d_Curve2d)& thearc = ptvtx.ArcOnS1();
                thearc->D1(ptvtx.ParameterOnArc1(), p2d, d2d);
                Quad1.D1(p2d.X(), p2d.Y(), valpt, d1u, d1v);
                tgarc.SetLinearForm(d2d.X(), d1u, d2d.Y(), d1v);
                norm1 = d1u.Crossed(d1v);
                if (norm1.SquareMagnitude() < 1e-16)
                {
                  TRest.SetValue(Standard_True, IntSurf_Undecided);
                  TArc.SetValue(Standard_True, IntSurf_Undecided);
                }
                else
                {
                  IntSurf::MakeTransition(tgline, tgarc, norm1, TRest, TArc);
                }
                newptvtx.SetArc(Standard_True, thearc, ptvtx.ParameterOnArc1(), TRest, TArc);
              }
              if (ptvtx.IsOnDomS2())
              {
                const Handle(Adaptor2d_Curve2d)& thearc = ptvtx.ArcOnS2();
                thearc->D1(ptvtx.ParameterOnArc2(), p2d, d2d);
                Quad2.D1(p2d.X(), p2d.Y(), valpt, d1u, d1v);
                tgarc.SetLinearForm(d2d.X(), d1u, d2d.Y(), d1v);
                norm2 = d1u.Crossed(d1v);
                if (norm2.SquareMagnitude() < 1e-16)
                {
                  TRest.SetValue(Standard_True, IntSurf_Undecided);
                  TArc.SetValue(Standard_True, IntSurf_Undecided);
                }
                else
                {
                  IntSurf::MakeTransition(tgline, tgarc, norm2, TRest, TArc);
                }
                newptvtx.SetArc(Standard_False, thearc, ptvtx.ParameterOnArc2(), TRest, TArc);
              }

              rline->AddVertex(newptvtx);
              if (!procl)
              {
                procl = Standard_True;
                rline->SetLastPoint(rline->NbVertex());
              }
            }
          }
        }
        // Si on a traite le pt debut et/ou fin, on ne doit pas recommencer si
        // il (ils) correspond(ent) a un point multiple.

        if (procf)
        {
          dofirst = Standard_False;
        }
        if (procl)
        {
          dolast = Standard_False;
        }
      }
    }
    // Si on n a pas trouve le point debut et./ou fin sur une des lignes
    // d intersection, il faut quand-meme le placer sur la restriction solution

    if (dofirst)
    {
      ptvtx.SetValue(PStartf.Value(), PStartf.Tolerance(), Standard_False);
      Quad1.Parameters(PStartf.Value(), U1, V1);
      Quad2.Parameters(PStartf.Value(), U2, V2);
      ptvtx.SetParameters(U1, V1, U2, V2);
      ptvtx.SetParameter(paramf);
      if (!PStartf.IsNew())
      {
        IntSurf_Transition Transline;
        IntSurf_Transition Transarc;
        ptvtx.SetVertex(OnFirst, PStartf.Vertex());
        ptvtx.SetArc(OnFirst, PStartf.Arc(), PStartf.Parameter(), Transline, Transarc);
      }

      rline->AddVertex(ptvtx);
      rline->SetFirstPoint(rline->NbVertex());
    }
    if (dolast)
    {
      ptvtx.SetValue(PStartl.Value(), PStartl.Tolerance(), Standard_False);
      Quad1.Parameters(PStartl.Value(), U1, V1);
      Quad2.Parameters(PStartl.Value(), U2, V2);
      ptvtx.SetParameters(U1, V1, U2, V2);
      ptvtx.SetParameter(paraml);
      if (!PStartl.IsNew())
      {
        IntSurf_Transition Transline;
        IntSurf_Transition Transarc;

        ptvtx.SetVertex(OnFirst, PStartl.Vertex());
        ptvtx.SetArc(OnFirst, PStartl.Arc(), PStartl.Parameter(), Transline, Transarc);
      }

      rline->AddVertex(ptvtx);
      rline->SetLastPoint(rline->NbVertex());
    }
    slin.Append(rline);
  }
}

inline const gp_Pnt& PointValue(const Handle(IntPatch_RLine) theRLine,
                                const Standard_Integer       theIndex)
{
  return theRLine->Point(theIndex).Value();
}

inline const gp_Pnt& VertexValue(const Handle(IntPatch_RLine) theRLine,
                                 const Standard_Integer       theIndex)
{
  return theRLine->Vertex(theIndex).Value();
}

static Standard_Real SquareDistance(const Handle(IntPatch_GLine)& theGLine,
                                    const gp_Pnt&                 theP,
                                    Extrema_ExtPC&                theExtr)
{
  Standard_Real aSQDist = RealLast();
  switch (theGLine->ArcType())
  {
    case IntPatch_Lin:
      aSQDist = theGLine->Line().SquareDistance(theP);
      break;
    case IntPatch_Circle:
      aSQDist = theGLine->Circle().SquareDistance(theP);
      break;
    default:
      theExtr.Perform(theP);
      if (!theExtr.IsDone() || !theExtr.NbExt())
      {
        // Lines are not overlapped
        return aSQDist;
      }

      aSQDist                        = theExtr.SquareDistance(1);
      const Standard_Integer aNbExtr = theExtr.NbExt();
      for (Standard_Integer i = 2; i <= aNbExtr; i++)
      {
        const Standard_Real aSQD = theExtr.SquareDistance(i);
        if (aSQD < aSQDist)
        {
          aSQDist = aSQD;
        }
      }
  }

  return aSQDist;
}

static Standard_Boolean IsRLineGood(const IntSurf_Quadric&       Quad1,
                                    const IntSurf_Quadric&       Quad2,
                                    const Handle(IntPatch_GLine) theGLine,
                                    const Handle(IntPatch_RLine) theRLine,
                                    const Standard_Real          theTol)
{
  const Standard_Real  aSQTol    = theTol * theTol;
  const IntPatch_IType aGType    = theGLine->ArcType();
  Standard_Integer     aNbPntsM1 = 0;

  const gp_Pnt& (*Value)(const Handle(IntPatch_RLine), const Standard_Integer);

  if (theRLine->HasPolygon())
  {
    aNbPntsM1 = theRLine->NbPnts() - 1;
    Value     = PointValue;
  }
  else
  {
    aNbPntsM1 = theRLine->NbVertex() - 1;
    Value     = VertexValue;
  }

  if (aNbPntsM1 < 1)
    return Standard_False;

  Extrema_ExtPC      anExtr;
  GeomAdaptor_Curve  anAC;
  Handle(Geom_Curve) aCurv;

  if (aGType == IntPatch_Ellipse)
    aCurv = new Geom_Ellipse(theGLine->Ellipse());
  else if (aGType == IntPatch_Parabola)
    aCurv = new Geom_Parabola(theGLine->Parabola());
  else if (aGType == IntPatch_Hyperbola)
    aCurv = new Geom_Hyperbola(theGLine->Hyperbola());

  if (!aCurv.IsNull())
  {
    const Standard_Real anUinf = aCurv->FirstParameter(), anUsup = aCurv->LastParameter();
    anAC.Load(aCurv, anUinf, anUsup);
    anExtr.Initialize(anAC, anUinf, anUsup);
  }

  if (aNbPntsM1 == 1)
  {
    gp_Pnt aP1(Value(theRLine, 1)), aP2(Value(theRLine, 2));

    if (aP1.SquareDistance(aP2) < aSQTol)
    {
      // RLine is degenerated
      return Standard_False;
    }

    gp_Pnt aPMid;
    if (theRLine->IsArcOnS1())
    {
      const Handle(Adaptor2d_Curve2d)& anAC2d = theRLine->ArcOnS1();
      const Standard_Real aParF = anAC2d->FirstParameter(), aParL = anAC2d->LastParameter();
      gp_Pnt2d            aP2d(anAC2d->Value(0.5 * (aParF + aParL)));
      aPMid = Quad1.Value(aP2d.X(), aP2d.Y());
    }
    else
    {
      const Handle(Adaptor2d_Curve2d)& anAC2d = theRLine->ArcOnS2();
      const Standard_Real aParF = anAC2d->FirstParameter(), aParL = anAC2d->LastParameter();
      gp_Pnt2d            aP2d(anAC2d->Value(0.5 * (aParF + aParL)));
      aPMid = Quad2.Value(aP2d.X(), aP2d.Y());
    }

    const Standard_Real aSQDist = SquareDistance(theGLine, aPMid, anExtr);
    return (aSQDist > aSQTol);
  }

  for (Standard_Integer i = 2; i <= aNbPntsM1; i++)
  {
    const gp_Pnt        aP(Value(theRLine, i));
    const Standard_Real aSQDist = SquareDistance(theGLine, aP, anExtr);

    if (aSQDist > aSQTol)
      return Standard_True;
  }

  return Standard_False;
}

void ProcessRLine(IntPatch_SequenceOfLine& slin,
                  //		   const Handle(Adaptor3d_Surface)& Surf1,
                  //		   const Handle(Adaptor3d_Surface)& Surf2,
                  const IntSurf_Quadric& Quad1,
                  const IntSurf_Quadric& Quad2,
                  const Standard_Real    _TolArc,
                  const Standard_Boolean theIsReqToKeepRLine)
{
  // On cherche a placer sur les restrictions solutions les points "multiples"
  // des autres lignes d intersection
  // Pas forcemment le plus efficace : on rique de projeter plusieurs fois
  // le meme point sur la meme restriction...

  Standard_Real TolArc = 100.0 * _TolArc;
  if (TolArc > 0.1)
    TolArc = 0.1;

  Standard_Integer i, j, k;
  Standard_Integer Nblin, Nbvtx, Nbpt;

  Standard_Boolean OnFirst = Standard_False, project = Standard_False, keeppoint = Standard_False;

  Handle(Adaptor2d_Curve2d) arcref;
  Standard_Real             paramproj, paramf, paraml;

  TColgp_SequenceOfPnt   seq_Pnt3d;
  TColStd_SequenceOfReal seq_Real;

  gp_Pnt ptproj, toproj, valpt;

  gp_Pnt2d           p2d;
  gp_Vec2d           d2d;
  gp_Vec             d1u, d1v, tgrest, tgarc, norm;
  IntSurf_Transition TRest, TArc;
#ifndef OCCT_DEBUG
  Standard_Real U = 0., V = 0.;
#else
  Standard_Real U, V;
#endif
  IntPatch_Point Ptvtx, newptvtx;

  IntPatch_IType typ1, typ2;

  Nblin = slin.Length();
  for (i = 1; i <= Nblin; i++)
  {
    const Handle(IntPatch_Line)& slini = slin(i);
    typ1                               = slini->ArcType();

    Standard_Boolean HasToDeleteRLine = Standard_False;
    if (typ1 == IntPatch_Restriction)
    {
      seq_Pnt3d.Clear();
      seq_Real.Clear();

      for (j = 1; j <= Nblin; j++)
      {
        const Handle(IntPatch_Line)& slinj = slin(j);
        Nbpt                               = seq_Pnt3d.Length(); // important que ce soit ici
        typ2                               = slinj->ArcType();
        if (typ2 != IntPatch_Restriction)
        {
          //-- arcref = Handle(IntPatch_RLine)::DownCast (slini)->Arc();
          //-- OnFirst = Handle(IntPatch_RLine)::DownCast (slini)->IsOnFirstSurface();

          //-- DES CHOSES A FAIRE ICI
          if (Handle(IntPatch_RLine)::DownCast(slini)->IsArcOnS1())
          {
            OnFirst = Standard_True;
            arcref  = Handle(IntPatch_RLine)::DownCast(slini)->ArcOnS1();
          }
          else if (Handle(IntPatch_RLine)::DownCast(slini)->IsArcOnS2())
          {
            arcref  = Handle(IntPatch_RLine)::DownCast(slini)->ArcOnS2();
            OnFirst = Standard_False;
          }
          if (Handle(IntPatch_RLine)::DownCast(slini)->HasFirstPoint())
          {
            paramf = Handle(IntPatch_RLine)::DownCast(slini)->FirstPoint().ParameterOnLine();
          }
          else
          {
            // cout << "Pas de param debut sur rst solution" << endl;
            paramf = RealFirst();
          }
          if (Handle(IntPatch_RLine)::DownCast(slini)->HasLastPoint())
          {
            paraml = Handle(IntPatch_RLine)::DownCast(slini)->LastPoint().ParameterOnLine();
          }
          else
          {
            // cout << "Pas de param debut sur rst solution" << endl;
            paraml = RealLast();
          }

          if (typ2 == IntPatch_Analytic)
          {
            Nbvtx = Handle(IntPatch_ALine)::DownCast(slinj)->NbVertex();
          }
          else
          {
            Nbvtx = Handle(IntPatch_GLine)::DownCast(slinj)->NbVertex();
          }

          Standard_Boolean EdgeDegenere = Standard_True;
          for (Standard_Integer edg = 0; EdgeDegenere && edg <= 10; edg++)
          {
            arcref->D1(paramf + (paraml - paramf) * edg * 0.1, p2d, d2d);
            if (OnFirst)
            {
              Quad1.D1(p2d.X(), p2d.Y(), valpt, d1u, d1v);
            }
            else
            {
              Quad2.D1(p2d.X(), p2d.Y(), valpt, d1u, d1v);
            }
            if (d1u.Magnitude() > 1e-7)
            {
              EdgeDegenere = Standard_False;
            }
          }

          for (k = 1; EdgeDegenere == Standard_False && k <= Nbvtx; k++)
          {
            if (typ2 == IntPatch_Analytic)
            {
              Ptvtx = Handle(IntPatch_ALine)::DownCast(slinj)->Vertex(k);
            }
            else
            {
              Ptvtx = Handle(IntPatch_GLine)::DownCast(slinj)->Vertex(k);
            }
            if ((OnFirst && !Ptvtx.IsOnDomS1()) || (!OnFirst && !Ptvtx.IsOnDomS2()))
            {
              // Si OnFirst && OnDomS1, c est qu on est a une extremite
              // ca doit etre traite par Process Segment...
              project   = Standard_True;
              keeppoint = Standard_False;
              toproj    = Ptvtx.Value();

              Standard_Integer jj;
              for (jj = 1; jj <= Nbpt; jj++)
              {
                // for (Standard_Integer jj = 1; jj <= Nbpt; jj++) {
                if (toproj.Distance(seq_Pnt3d(jj)) < _TolArc)
                {
                  project = Standard_False;
                  break;
                }
              }
              if (project)
              { //-- il faut projeter pour trouver le point sur la rline.
                if (OnFirst)
                {
                  Ptvtx.ParametersOnS1(U, V);
                }
                else
                {
                  Ptvtx.ParametersOnS2(U, V);
                }

                project = IntPatch_HInterTool::Project(arcref, gp_Pnt2d(U, V), paramproj, p2d);

                if (project)
                {
                  if (OnFirst)
                  {
                    ptproj = Quad1.Value(p2d.X(), p2d.Y());
                  }
                  else
                  {
                    ptproj = Quad2.Value(p2d.X(), p2d.Y());
                  }
                  if ((toproj.Distance(ptproj) <= 100 * TolArc) && (paramproj >= paramf)
                      && (paramproj <= paraml))
                  {
                    newptvtx = Ptvtx;
                    newptvtx.SetParameter(paramproj);
                    keeppoint = Standard_True;
                    seq_Pnt3d.Append(toproj);
                    seq_Real.Append(paramproj);

                    //-- verifier que si la restriction arcref est trouvee, elle porte ce vertex
                    for (int ri = 1; ri <= Nblin; ri++)
                    {
                      const Handle(IntPatch_Line)& slinri = slin(ri);
                      if (slinri->ArcType() == IntPatch_Restriction)
                      {
                        if (OnFirst && Handle(IntPatch_RLine)::DownCast(slinri)->IsArcOnS1())
                        {
                          if (arcref == Handle(IntPatch_RLine)::DownCast(slinri)->ArcOnS1())
                          {
                            Handle(IntPatch_RLine)::DownCast(slinri)->AddVertex(newptvtx);
                            // printf("\n ImpImpIntersection_0.gxx CAS1 \n");
                          }
                        }
                        else if (OnFirst == Standard_False
                                 && Handle(IntPatch_RLine)::DownCast(slinri)->IsArcOnS2())
                        {
                          if (arcref == Handle(IntPatch_RLine)::DownCast(slinri)->ArcOnS2())
                          {
                            Handle(IntPatch_RLine)::DownCast(slinri)->AddVertex(newptvtx);
                            // printf("\n ImpImpIntersection_0.gxx CAS2 \n");
                          }
                        }
                      }
                    }
                    // -- --------------------------------------------------
                  }
                }
              }
              else
              {
                keeppoint = Standard_True;
                newptvtx  = Ptvtx;
                newptvtx.SetParameter(seq_Real(jj));
              }
              if (keeppoint)
              {
                Ptvtx.SetMultiple(Standard_True);
                Ptvtx.SetTolerance(_TolArc);
                newptvtx.SetMultiple(Standard_True);

                if (typ2 == IntPatch_Analytic)
                {
                  Handle(IntPatch_ALine)::DownCast(slinj)->Replace(k, Ptvtx);
                }
                else
                {
                  Handle(IntPatch_GLine)::DownCast(slinj)->Replace(k, Ptvtx);
                }

                if (Ptvtx.IsOnDomS1() || Ptvtx.IsOnDomS2())
                {
                  arcref->D1(newptvtx.ParameterOnLine(), p2d, d2d);

                  if (OnFirst)
                  { // donc OnDomS2
                    Quad1.D1(p2d.X(), p2d.Y(), valpt, d1u, d1v);
                    tgrest.SetLinearForm(d2d.X(), d1u, d2d.Y(), d1v);

                    const Handle(Adaptor2d_Curve2d)& thearc = Ptvtx.ArcOnS2();
                    thearc->D1(Ptvtx.ParameterOnArc2(), p2d, d2d);
                    Quad2.D1(p2d.X(), p2d.Y(), valpt, d1u, d1v);
                    tgarc.SetLinearForm(d2d.X(), d1u, d2d.Y(), d1v);
                    norm = d1u.Crossed(d1v); // Quad2.Normale(valpt);
                    if (norm.SquareMagnitude() < 1e-16)
                    {
                      TRest.SetValue(Standard_True, IntSurf_Undecided);
                      TArc.SetValue(Standard_True, IntSurf_Undecided);
                    }
                    else
                    {
                      IntSurf::MakeTransition(tgrest, tgarc, norm, TRest, TArc);
                    }
                    newptvtx.SetArc(Standard_False, thearc, Ptvtx.ParameterOnArc2(), TRest, TArc);
                  }
                  else
                  { // donc OnDomS1
                    Quad2.D1(p2d.X(), p2d.Y(), valpt, d1u, d1v);
                    tgrest.SetLinearForm(d2d.X(), d1u, d2d.Y(), d1v);

                    const Handle(Adaptor2d_Curve2d)& thearc = Ptvtx.ArcOnS1();
                    thearc->D1(Ptvtx.ParameterOnArc1(), p2d, d2d);
                    Quad1.D1(p2d.X(), p2d.Y(), valpt, d1u, d1v);
                    tgarc.SetLinearForm(d2d.X(), d1u, d2d.Y(), d1v);
                    norm = d1u.Crossed(d1v); // Quad1.Normale(valpt);
                    if (norm.SquareMagnitude() < 1e-16)
                    {
                      TRest.SetValue(Standard_True, IntSurf_Undecided);
                      TArc.SetValue(Standard_True, IntSurf_Undecided);
                    }
                    else
                    {
                      IntSurf::MakeTransition(tgrest, tgarc, norm, TRest, TArc);
                    }
                    newptvtx.SetArc(Standard_True, thearc, Ptvtx.ParameterOnArc1(), TRest, TArc);
                  }
                } //-- if (Ptvtx.IsOnDomS1() || Ptvtx.IsOnDomS2())

                Handle(IntPatch_RLine)::DownCast(slini)->AddVertex(newptvtx);

              } //-- if (keeppoint)
            } //-- if ((OnFirst && !Ptvtx.IsOnDomS1())||(!OnFirst && !Ptvtx.IsOnDomS2()))
          } //-- boucle sur les vertex

          if (!theIsReqToKeepRLine)
          {
            Handle(IntPatch_GLine) aGL = Handle(IntPatch_GLine)::DownCast(slinj);

            if (!aGL.IsNull())
            {
              HasToDeleteRLine =
                !IsRLineGood(Quad1, Quad2, aGL, Handle(IntPatch_RLine)::DownCast(slini), TolArc);
            }

            if (HasToDeleteRLine)
            {
              break;
            }
          }
        } //-- if (typ2 != IntPatch_Restriction)
      } //-- for (j=1; j<=Nblin; j++)
    } //-- if (typ1 == IntPatch_Restriction)

    if (HasToDeleteRLine)
    {
      slin.Remove(i);
      i--;
      Nblin = slin.Length();
      continue;
    }
  } //-- for (i=1; i<=Nblin; i++)
}

static Standard_Boolean IntPP(const IntSurf_Quadric&,
                              const IntSurf_Quadric&,
                              const Standard_Real,
                              const Standard_Real,
                              Standard_Boolean&,
                              IntPatch_SequenceOfLine&);

static Standard_Boolean IntPCy(const IntSurf_Quadric&,
                               const IntSurf_Quadric&,
                               const Standard_Real,
                               const Standard_Real,
                               const Standard_Boolean,
                               Standard_Boolean&,
                               IntPatch_SequenceOfLine&,
                               const Standard_Real H = 0.);

static Standard_Boolean IntPSp(const IntSurf_Quadric&,
                               const IntSurf_Quadric&,
                               // modified by NIZNHY-PKV Tue Sep 20 08:59:56 2011t
                               const Standard_Real,
                               // modified by NIZNHY-PKV Tue Sep 20 08:59:52 2011t
                               const Standard_Real,
                               const Standard_Boolean,
                               Standard_Boolean&,
                               IntPatch_SequenceOfLine&,
                               IntPatch_SequenceOfPoint&);

static Standard_Boolean IntPCo(const IntSurf_Quadric&,
                               const IntSurf_Quadric&,
                               const Standard_Real,
                               const Standard_Real,
                               const Standard_Boolean,
                               Standard_Boolean&,
                               Standard_Boolean&,
                               IntPatch_SequenceOfLine&,
                               IntPatch_SequenceOfPoint&);

static void ProcessBounds(const Handle(IntPatch_ALine)&,
                          const IntPatch_SequenceOfLine&,
                          const IntSurf_Quadric&,
                          const IntSurf_Quadric&,
                          Standard_Boolean&,
                          const gp_Pnt&,
                          const Standard_Real,
                          Standard_Boolean&,
                          const gp_Pnt&,
                          const Standard_Real,
                          Standard_Boolean&,
                          const Standard_Real);

static IntPatch_ImpImpIntersection::IntStatus IntCyCy(const IntSurf_Quadric&    theQuad1,
                                                      const IntSurf_Quadric&    theQuad2,
                                                      const Standard_Real       theTol3D,
                                                      const Standard_Real       theTol2D,
                                                      const Bnd_Box2d&          theUVSurf1,
                                                      const Bnd_Box2d&          theUVSurf2,
                                                      Standard_Boolean&         isTheEmpty,
                                                      Standard_Boolean&         isTheSameSurface,
                                                      Standard_Boolean&         isTheMultiplePoint,
                                                      IntPatch_SequenceOfLine&  theSlin,
                                                      IntPatch_SequenceOfPoint& theSPnt);

static Standard_Boolean IntCySp(const IntSurf_Quadric&,
                                const IntSurf_Quadric&,
                                const Standard_Real,
                                const Standard_Boolean,
                                Standard_Boolean&,
                                Standard_Boolean&,
                                IntPatch_SequenceOfLine&,
                                IntPatch_SequenceOfPoint&);

static Standard_Boolean IntCyCo(const IntSurf_Quadric&,
                                const IntSurf_Quadric&,
                                const Standard_Real,
                                const Standard_Boolean,
                                Standard_Boolean&,
                                Standard_Boolean&,
                                IntPatch_SequenceOfLine&,
                                IntPatch_SequenceOfPoint&);

static Standard_Boolean IntSpSp(const IntSurf_Quadric&,
                                const IntSurf_Quadric&,
                                const Standard_Real,
                                Standard_Boolean&,
                                Standard_Boolean&,
                                IntPatch_SequenceOfLine&,
                                IntPatch_SequenceOfPoint&);

static Standard_Boolean IntCoSp(const IntSurf_Quadric&,
                                const IntSurf_Quadric&,
                                const Standard_Real,
                                const Standard_Boolean,
                                Standard_Boolean&,
                                Standard_Boolean&,
                                IntPatch_SequenceOfLine&,
                                IntPatch_SequenceOfPoint&);

static Standard_Boolean IntCoCo(const IntSurf_Quadric&,
                                const IntSurf_Quadric&,
                                const Standard_Real,
                                Standard_Boolean&,
                                Standard_Boolean&,
                                Standard_Boolean&,
                                IntPatch_SequenceOfLine&,
                                IntPatch_SequenceOfPoint&);

// torus
static Standard_Boolean IntPTo(const IntSurf_Quadric&,
                               const IntSurf_Quadric&,
                               const Standard_Real,
                               const Standard_Boolean,
                               Standard_Boolean&,
                               IntPatch_SequenceOfLine&);

static Standard_Boolean IntCyTo(const IntSurf_Quadric&,
                                const IntSurf_Quadric&,
                                const Standard_Real,
                                const Standard_Boolean,
                                Standard_Boolean&,
                                IntPatch_SequenceOfLine&);

static Standard_Boolean IntCoTo(const IntSurf_Quadric&,
                                const IntSurf_Quadric&,
                                const Standard_Real,
                                const Standard_Boolean,
                                Standard_Boolean&,
                                IntPatch_SequenceOfLine&);

static Standard_Boolean IntSpTo(const IntSurf_Quadric&,
                                const IntSurf_Quadric&,
                                const Standard_Real,
                                const Standard_Boolean,
                                Standard_Boolean&,
                                IntPatch_SequenceOfLine&);

static Standard_Boolean IntToTo(const IntSurf_Quadric&,
                                const IntSurf_Quadric&,
                                const Standard_Real,
                                Standard_Boolean&,
                                Standard_Boolean&,
                                IntPatch_SequenceOfLine&);

static Standard_Integer SetQuad(const Handle(Adaptor3d_Surface)& theS,
                                GeomAbs_SurfaceType&             theTS,
                                IntSurf_Quadric&                 theQuad);

//=================================================================================================

IntPatch_ImpImpIntersection::IntPatch_ImpImpIntersection()
    : myDone(IntStatus_Fail),
      empt(Standard_True),
      tgte(Standard_False),
      oppo(Standard_False)
{
}

//=================================================================================================

IntPatch_ImpImpIntersection::IntPatch_ImpImpIntersection(const Handle(Adaptor3d_Surface)&   S1,
                                                         const Handle(Adaptor3d_TopolTool)& D1,
                                                         const Handle(Adaptor3d_Surface)&   S2,
                                                         const Handle(Adaptor3d_TopolTool)& D2,
                                                         const Standard_Real                TolArc,
                                                         const Standard_Real                TolTang,
                                                         const Standard_Boolean theIsReqToKeepRLine)
{
  Perform(S1, D1, S2, D2, TolArc, TolTang, theIsReqToKeepRLine);
}

//=================================================================================================

void IntPatch_ImpImpIntersection::Perform(const Handle(Adaptor3d_Surface)&   S1,
                                          const Handle(Adaptor3d_TopolTool)& D1,
                                          const Handle(Adaptor3d_Surface)&   S2,
                                          const Handle(Adaptor3d_TopolTool)& D2,
                                          const Standard_Real                TolArc,
                                          const Standard_Real                TolTang,
                                          const Standard_Boolean             theIsReqToKeepRLine)
{
  myDone = IntStatus_Fail;
  spnt.Clear();
  slin.Clear();

  Standard_Boolean isPostProcessingRequired = Standard_True;

  empt = Standard_True;
  tgte = Standard_False;
  oppo = Standard_False;

  Standard_Boolean all1      = Standard_False;
  Standard_Boolean all2      = Standard_False;
  Standard_Boolean SameSurf  = Standard_False;
  Standard_Boolean multpoint = Standard_False;

  Standard_Boolean nosolonS1 = Standard_False;
  // indique s il y a des points sur restriction du carreau 1
  Standard_Boolean nosolonS2 = Standard_False;
  // indique s il y a des points sur restriction du carreau 2
  Standard_Integer                           i, nbpt, nbseg;
  IntPatch_SequenceOfSegmentOfTheSOnBounds   edg1, edg2;
  IntPatch_SequenceOfPathPointOfTheSOnBounds pnt1, pnt2;
  //
  // On commence par intersecter les supports des surfaces
  IntSurf_Quadric      quad1, quad2;
  IntPatch_ArcFunction AFunc;
  const Standard_Real  Tolang = 1.e-8;
  GeomAbs_SurfaceType  typs1, typs2;
  Standard_Boolean     bEmpty = Standard_False;
  //
  const Standard_Integer iT1 = SetQuad(S1, typs1, quad1);
  const Standard_Integer iT2 = SetQuad(S2, typs2, quad2);
  //
  if (!iT1 || !iT2)
  {
    throw Standard_ConstructionError();
    return;
  }
  //
  const Standard_Boolean bReverse = iT1 > iT2;
  const Standard_Integer iTT      = iT1 * 10 + iT2;
  //
  switch (iTT)
  {
    case 11: { // Plane/Plane
      if (!IntPP(quad1, quad2, Tolang, TolTang, SameSurf, slin))
      {
        return;
      }
      break;
    }
    //
    case 12:
    case 21: { // Plane/Cylinder
      Standard_Real VMin, VMax, H;
      //
      const Handle(Adaptor3d_Surface)& aSCyl = bReverse ? S1 : S2;
      VMin                                   = aSCyl->FirstVParameter();
      VMax                                   = aSCyl->LastVParameter();
      H = (Precision::IsNegativeInfinite(VMin) || Precision::IsPositiveInfinite(VMax))
            ? 0
            : (VMax - VMin);
      //
      if (!IntPCy(quad1, quad2, Tolang, TolTang, bReverse, empt, slin, H))
      {
        return;
      }
      bEmpty = empt;
      break;
    }
    //
    case 13:
    case 31: { // Plane/Cone
      if (!IntPCo(quad1, quad2, Tolang, TolTang, bReverse, empt, multpoint, slin, spnt))
      {
        return;
      }
      bEmpty = empt;
      break;
    }
    //
    case 14:
    case 41: { // Plane/Sphere
      if (!IntPSp(quad1, quad2, Tolang, TolTang, bReverse, empt, slin, spnt))
      {
        return;
      }
      bEmpty = empt;
      break;
    }
    //
    case 15:
    case 51: { // Plane/Torus
      if (!IntPTo(quad1, quad2, TolTang, bReverse, empt, slin))
      {
        return;
      }
      bEmpty = empt;
      break;
    }
    //
    case 22: { // Cylinder/Cylinder
      Bnd_Box2d aBox1, aBox2;

      const Standard_Real aU1f = S1->FirstUParameter();
      Standard_Real       aU1l = S1->LastUParameter();
      const Standard_Real aU2f = S2->FirstUParameter();
      Standard_Real       aU2l = S2->LastUParameter();

      const Standard_Real anUperiod = 2.0 * M_PI;

      if (aU1l - aU1f > anUperiod)
        aU1l = aU1f + anUperiod;

      if (aU2l - aU2f > anUperiod)
        aU2l = aU2f + anUperiod;

      aBox1.Add(gp_Pnt2d(aU1f, S1->FirstVParameter()));
      aBox1.Add(gp_Pnt2d(aU1l, S1->LastVParameter()));
      aBox2.Add(gp_Pnt2d(aU2f, S2->FirstVParameter()));
      aBox2.Add(gp_Pnt2d(aU2l, S2->LastVParameter()));

      // Resolution is too big if the cylinder radius is
      // too small. Therefore, we shall bind its value above.
      // Here, we use simple constant.
      const Standard_Real a2DTol =
        Min(1.0e-4, Min(S1->UResolution(TolTang), S2->UResolution(TolTang)));

      myDone =
        IntCyCy(quad1, quad2, TolTang, a2DTol, aBox1, aBox2, empt, SameSurf, multpoint, slin, spnt);

      if (myDone == IntPatch_ImpImpIntersection::IntStatus_Fail)
      {
        return;
      }

      bEmpty = empt;
      if (!slin.IsEmpty())
      {
        const Handle(IntPatch_WLine)& aWLine = Handle(IntPatch_WLine)::DownCast(slin.Value(1));

        if (!aWLine.IsNull())
        { // No geometric solution
          isPostProcessingRequired = Standard_False;
        }
      }

      break;
    }
    //
    case 23:
    case 32: { // Cylinder/Cone
      if (!IntCyCo(quad1, quad2, TolTang, bReverse, empt, multpoint, slin, spnt))
      {
        return;
      }
      bEmpty = empt;
      break;
    }
    //
    case 24:
    case 42: { // Cylinder/Sphere
      if (!IntCySp(quad1, quad2, TolTang, bReverse, empt, multpoint, slin, spnt))
      {
        return;
      }
      bEmpty = empt;
      break;
    }
    //
    case 25:
    case 52: { // Cylinder/Torus
      if (!IntCyTo(quad1, quad2, TolTang, bReverse, empt, slin))
      {
        return;
      }
      bEmpty = empt;
      break;
    }
    //
    case 33: { // Cone/Cone
      if (!IntCoCo(quad1, quad2, TolTang, empt, SameSurf, multpoint, slin, spnt))
      {
        return;
      }
      bEmpty = empt;
      break;
    }
    //
    case 34:
    case 43: { // Cone/Sphere
      if (!IntCoSp(quad1, quad2, TolTang, bReverse, empt, multpoint, slin, spnt))
      {
        return;
      }
      bEmpty = empt;
      break;
    }
    //
    case 35:
    case 53: { // Cone/Torus
      if (!IntCoTo(quad1, quad2, TolTang, bReverse, empt, slin))
      {
        return;
      }
      break;
    }
    //
    case 44: { // Sphere/Sphere
      if (!IntSpSp(quad1, quad2, TolTang, empt, SameSurf, slin, spnt))
      {
        return;
      }
      bEmpty = empt;
      break;
    }
    //
    case 45:
    case 54: { // Sphere/Torus
      if (!IntSpTo(quad1, quad2, TolTang, bReverse, empt, slin))
      {
        return;
      }
      bEmpty = empt;
      break;
    }
    //
    case 55: { // Torus/Torus
      if (!IntToTo(quad1, quad2, TolTang, SameSurf, empt, slin))
      {
        return;
      }
      bEmpty = empt;
      break;
    }
    //
    default: {
      throw Standard_ConstructionError();
      break;
    }
  }
  //
  if (bEmpty)
  {
    if (myDone == IntStatus_Fail)
      myDone = IntStatus_OK;

    return;
  }
  //

  if (isPostProcessingRequired)
  {
    if (!SameSurf)
    {
      AFunc.SetQuadric(quad2);
      AFunc.Set(S1);

      solrst.Perform(AFunc, D1, TolArc, TolTang);
      if (!solrst.IsDone())
      {
        return;
      }

      if (solrst.AllArcSolution() && typs1 == typs2)
      {
        all1 = Standard_True;
      }
      nbpt  = solrst.NbPoints();
      nbseg = solrst.NbSegments();
      for (i = 1; i <= nbpt; i++)
      {
        const IntPatch_ThePathPointOfTheSOnBounds& aPt = solrst.Point(i);
        pnt1.Append(aPt);
      }
      for (i = 1; i <= nbseg; i++)
      {
        const IntPatch_TheSegmentOfTheSOnBounds& aSegm = solrst.Segment(i);
        edg1.Append(aSegm);
      }
      nosolonS1 = (nbpt == 0) && (nbseg == 0);

      if (nosolonS1 && all1)
      { // cas de face sans restrictions
        all1 = Standard_False;
      }
    } // if (!SameSurf) {
    else
    {
      nosolonS1 = Standard_True;
    }

    if (!SameSurf)
    {
      AFunc.SetQuadric(quad1);
      AFunc.Set(S2);

      solrst.Perform(AFunc, D2, TolArc, TolTang);
      if (!solrst.IsDone())
      {
        return;
      }

      if (solrst.AllArcSolution() && typs1 == typs2)
      {
        all2 = Standard_True;
      }

      nbpt  = solrst.NbPoints();
      nbseg = solrst.NbSegments();
      for (i = 1; i <= nbpt; i++)
      {
        const IntPatch_ThePathPointOfTheSOnBounds& aPt = solrst.Point(i);
        pnt2.Append(aPt);
      }

      for (i = 1; i <= nbseg; i++)
      {
        const IntPatch_TheSegmentOfTheSOnBounds& aSegm = solrst.Segment(i);
        edg2.Append(aSegm);
      }

      nosolonS2 = (nbpt == 0) && (nbseg == 0);

      if (nosolonS2 && all2)
      { // cas de face sans restrictions
        all2 = Standard_False;
      }
    } // if (!SameSurf) {
    else
    {
      nosolonS2 = Standard_True;
    }
    //
    if (SameSurf || (all1 && all2))
    {
      // faces "paralleles" parfaites
      empt = Standard_False;
      tgte = Standard_True;
      slin.Clear();
      spnt.Clear();

      gp_Pnt Ptreference;

      switch (typs1)
      {
        case GeomAbs_Plane: {
          Ptreference = (S1->Plane()).Location();
        }
        break;
        case GeomAbs_Cylinder: {
          Ptreference = ElSLib::Value(0., 0., S1->Cylinder());
        }
        break;
        case GeomAbs_Sphere: {
          Ptreference = ElSLib::Value(M_PI / 4., M_PI / 4., S1->Sphere());
        }
        break;
        case GeomAbs_Cone: {
          Ptreference = ElSLib::Value(0., 10., S1->Cone());
        }
        break;
        case GeomAbs_Torus: {
          Ptreference = ElSLib::Value(0., 0., S1->Torus());
        }
        break;
        default:
          break;
      }
      //
      oppo   = quad1.Normale(Ptreference).Dot(quad2.Normale(Ptreference)) < 0.0;
      myDone = IntStatus_OK;
      return;
    } // if (SameSurf || (all1 && all2)) {

    if (!nosolonS1 || !nosolonS2)
    {
      empt = Standard_False;
      // C est la qu il faut commencer a bosser...
      PutPointsOnLine(S1, S2, pnt1, slin, Standard_True, D1, quad1, quad2, multpoint, TolArc);

      PutPointsOnLine(S1, S2, pnt2, slin, Standard_False, D2, quad2, quad1, multpoint, TolArc);

      if (edg1.Length() != 0)
      {
        ProcessSegments(edg1, slin, quad1, quad2, Standard_True, TolArc);
      }

      if (edg2.Length() != 0)
      {
        ProcessSegments(edg2, slin, quad1, quad2, Standard_False, TolArc);
      }

      if (edg1.Length() != 0 || edg2.Length() != 0)
      {
        //      ProcessRLine(slin,S1,S2,TolArc);
        ProcessRLine(slin, quad1, quad2, TolArc, theIsReqToKeepRLine);
      }
    } // if (!nosolonS1 || !nosolonS2) {
    else
    {
      empt = ((slin.Length() == 0) && (spnt.Length() == 0));
    }
  }

  Standard_Integer nblin = slin.Length(), aNbPnt = spnt.Length();
  //
  // modified by NIZNHY-PKV Tue Sep 06 10:03:35 2011f
  if (aNbPnt)
  {
    IntPatch_SequenceOfPoint aSIP;
    //
    for (i = 1; i <= aNbPnt; ++i)
    {
      Standard_Real aU1, aV1, aU2, aV2;
      gp_Pnt2d      aP2D;
      TopAbs_State  aState1, aState2;
      //
      const IntPatch_Point& aIP = spnt(i);
      aIP.Parameters(aU1, aV1, aU2, aV2);
      //
      aP2D.SetCoord(aU1, aV1);
      aState1 = D1->Classify(aP2D, TolArc);
      //
      aP2D.SetCoord(aU2, aV2);
      aState2 = D2->Classify(aP2D, TolArc);
      //
      if (aState1 != TopAbs_OUT && aState2 != TopAbs_OUT)
      {
        aSIP.Append(aIP);
      }
    }
    //
    spnt.Clear();
    //
    aNbPnt = aSIP.Length();
    for (i = 1; i <= aNbPnt; ++i)
    {
      const IntPatch_Point& aIP = aSIP(i);
      spnt.Append(aIP);
    }
    //
  } //  if (aNbPnt) {
  // modified by NIZNHY-PKV Tue Sep 06 10:18:20 2011t
  //
  for (i = 1; i <= nblin; i++)
  {
    IntPatch_IType thetype = slin.Value(i)->ArcType();
    if ((thetype == IntPatch_Ellipse) || (thetype == IntPatch_Circle) || (thetype == IntPatch_Lin)
        || (thetype == IntPatch_Parabola) || (thetype == IntPatch_Hyperbola))
    {
      Handle(IntPatch_GLine)& glin = *((Handle(IntPatch_GLine)*)&slin.Value(i));
      glin->ComputeVertexParameters(TolArc);
    }
    else if (thetype == IntPatch_Analytic)
    {
      Handle(IntPatch_ALine)& aligold = *((Handle(IntPatch_ALine)*)&slin.Value(i));
      aligold->ComputeVertexParameters(TolArc);
    }
    else if (thetype == IntPatch_Restriction)
    {
      Handle(IntPatch_RLine)& rlig = *((Handle(IntPatch_RLine)*)&slin.Value(i));
      rlig->ComputeVertexParameters(TolArc);
    }
  }
  //
  //----------------------------------------------------------------
  //-- On place 2 vertex sur les courbes de GLine qui n en
  //-- contiennent pas.
  for (i = 1; i <= nblin; i++)
  {
    gp_Pnt         P;
    IntPatch_Point point;
    Standard_Real  u1, v1, u2, v2;
    if (slin.Value(i)->ArcType() == IntPatch_Circle)
    {
      const Handle(IntPatch_GLine)& glin = *((Handle(IntPatch_GLine)*)&slin.Value(i));
      if (glin->NbVertex() == 0)
      {
        gp_Circ Circ = glin->Circle();
        P            = ElCLib::Value(0.0, Circ);
        quad1.Parameters(P, u1, v1);
        quad2.Parameters(P, u2, v2);
        point.SetValue(P, TolArc, Standard_False);
        point.SetParameters(u1, v1, u2, v2);
        point.SetParameter(0.0);
        glin->AddVertex(point);

        P = ElCLib::Value(0.0, Circ);
        quad1.Parameters(P, u1, v1);
        quad2.Parameters(P, u2, v2);
        point.SetValue(P, TolArc, Standard_False);
        point.SetParameters(u1, v1, u2, v2);
        point.SetParameter(M_PI + M_PI);
        glin->AddVertex(point);
      }
    }

    else if (slin.Value(i)->ArcType() == IntPatch_Ellipse)
    {
      const Handle(IntPatch_GLine)& glin = *((Handle(IntPatch_GLine)*)&slin.Value(i));
      if (glin->NbVertex() == 0)
      {
        gp_Elips Elips = glin->Ellipse();
        P              = ElCLib::Value(0.0, Elips);
        quad1.Parameters(P, u1, v1);
        quad2.Parameters(P, u2, v2);
        point.SetValue(P, TolArc, Standard_False);
        point.SetParameters(u1, v1, u2, v2);
        point.SetParameter(0.0);
        glin->AddVertex(point);

        P = ElCLib::Value(0.0, Elips);
        quad1.Parameters(P, u1, v1);
        quad2.Parameters(P, u2, v2);
        point.SetValue(P, TolArc, Standard_False);
        point.SetParameters(u1, v1, u2, v2);
        point.SetParameter(M_PI + M_PI);
        glin->AddVertex(point);
      }
    }
  }
  myDone = IntStatus_OK;
}

//=================================================================================================

Standard_Integer SetQuad(const Handle(Adaptor3d_Surface)& theS,
                         GeomAbs_SurfaceType&             theTS,
                         IntSurf_Quadric&                 theQuad)
{
  theTS                 = theS->GetType();
  Standard_Integer iRet = 0;
  switch (theTS)
  {
    case GeomAbs_Plane:
      theQuad.SetValue(theS->Plane());
      iRet = 1;
      break;
    case GeomAbs_Cylinder:
      theQuad.SetValue(theS->Cylinder());
      iRet = 2;
      break;
    case GeomAbs_Cone:
      theQuad.SetValue(theS->Cone());
      iRet = 3;
      break;
    case GeomAbs_Sphere:
      theQuad.SetValue(theS->Sphere());
      iRet = 4;
      break;
    case GeomAbs_Torus:
      theQuad.SetValue(theS->Torus());
      iRet = 5;
      break;
    default:
      break;
  }
  //
  return iRet;
}

static void SeamPosition(const gp_Pnt& aPLoc, const gp_Ax3& aPos, gp_Ax2& aSeamPos);
static void AdjustToSeam(const gp_Cylinder& aQuad, gp_Circ& aCirc);
static void AdjustToSeam(const gp_Sphere& aQuad, gp_Circ& aCirc, const Standard_Real aTolAng);
static void AdjustToSeam(const gp_Cone& aQuad, gp_Circ& aCirc);
static void AdjustToSeam(const gp_Torus& aQuad, gp_Circ& aCirc);

// modified by NIZNHY-PKV Thu Sep 15 11:09:13 2011

//=======================================================================
// function : IntPP
// purpose  :
// Traitement du cas Plan/Plan
//=======================================================================
Standard_Boolean IntPP(const IntSurf_Quadric&   Quad1,
                       const IntSurf_Quadric&   Quad2,
                       const Standard_Real      Tolang,
                       const Standard_Real      TolTang,
                       Standard_Boolean&        Same,
                       IntPatch_SequenceOfLine& slin)

{
  IntSurf_TypeTrans trans1, trans2;
  IntAna_ResultType typint;
  gp_Pln            pl1(Quad1.Plane());
  gp_Pln            pl2(Quad2.Plane());

  IntAna_QuadQuadGeo inter(pl1, pl2, Tolang, TolTang);
  if (!inter.IsDone())
  {
    return Standard_False;
  }
  Same   = Standard_False;
  typint = inter.TypeInter();
  if (typint == IntAna_Same)
  { // cas faces confondues
    Same = Standard_True;
  }
  else if (typint != IntAna_Empty)
  { // on a une ligne
    gp_Lin        linsol = inter.Line(1);
    Standard_Real discri = linsol.Direction().DotCross(Quad2.Normale(linsol.Location()),
                                                       Quad1.Normale(linsol.Location()));

    if (discri > 0.0)
    {
      trans1 = IntSurf_Out;
      trans2 = IntSurf_In;
    }
    else
    {
      trans1 = IntSurf_In;
      trans2 = IntSurf_Out;
    }
    Handle(IntPatch_GLine) glig = new IntPatch_GLine(linsol, Standard_False, trans1, trans2);
    slin.Append(glig);
  }
  return Standard_True;
}

//=======================================================================
// function : IntPCy
// purpose  :
// Traitement du cas Plan/Cylindre et reciproquement
//=======================================================================
Standard_Boolean IntPCy(const IntSurf_Quadric&   Quad1,
                        const IntSurf_Quadric&   Quad2,
                        const Standard_Real      Tolang,
                        const Standard_Real      TolTang,
                        const Standard_Boolean   Reversed,
                        Standard_Boolean&        Empty,
                        IntPatch_SequenceOfLine& slin,
                        const Standard_Real      H)

{
  gp_Pln      Pl;
  gp_Cylinder Cy;

  IntSurf_TypeTrans trans1, trans2;
  IntAna_ResultType typint;

  IntAna_QuadQuadGeo inter;
  if (!Reversed)
  {
    Pl = Quad1.Plane();
    Cy = Quad2.Cylinder();
  }
  else
  {
    Pl = Quad2.Plane();
    Cy = Quad1.Cylinder();
  }
  inter.Perform(Pl, Cy, Tolang, TolTang, H);
  if (!inter.IsDone())
  {
    return Standard_False;
  }
  typint                 = inter.TypeInter();
  Standard_Integer NbSol = inter.NbSolutions();
  Empty                  = Standard_False;

  switch (typint)
  {
    case IntAna_Empty: {
      Empty = Standard_True;
    }
    break;

    case IntAna_Line: {
      gp_Lin linsol = inter.Line(1);
      gp_Pnt orig(linsol.Location());
      if (NbSol == 1)
      { // ligne de tangence
        gp_Vec TestCurvature(orig, Cy.Location());
        gp_Vec Normp, Normcyl;
        if (!Reversed)
        {
          Normp   = Quad1.Normale(orig);
          Normcyl = Quad2.Normale(orig);
        }
        else
        {
          Normp   = Quad2.Normale(orig);
          Normcyl = Quad1.Normale(orig);
        }

        IntSurf_Situation situcyl;
        IntSurf_Situation situp;

        if (Normp.Dot(TestCurvature) > 0.)
        {
          situcyl = IntSurf_Outside;
          if (Normp.Dot(Normcyl) > 0.)
          {
            situp = IntSurf_Inside;
          }
          else
          {
            situp = IntSurf_Outside;
          }
        }
        else
        {
          situcyl = IntSurf_Inside;
          if (Normp.Dot(Normcyl) > 0.)
          {
            situp = IntSurf_Outside;
          }
          else
          {
            situp = IntSurf_Inside;
          }
        }
        Handle(IntPatch_GLine) glig;
        if (!Reversed)
        {
          glig = new IntPatch_GLine(linsol, Standard_True, situp, situcyl);
        }
        else
        {
          glig = new IntPatch_GLine(linsol, Standard_True, situcyl, situp);
        }
        slin.Append(glig);
      }
      else
      {
        // on a 2 droites. Il faut determiner les transitions
        // de chacune.

        if (linsol.Direction().DotCross(Quad2.Normale(orig), Quad1.Normale(orig)) > 0.)
        {
          trans1 = IntSurf_Out;
          trans2 = IntSurf_In;
        }
        else
        {
          trans1 = IntSurf_In;
          trans2 = IntSurf_Out;
        }
        Handle(IntPatch_GLine) glig = new IntPatch_GLine(linsol, Standard_False, trans1, trans2);
        slin.Append(glig);

        linsol = inter.Line(2);
        orig   = linsol.Location();

        if (linsol.Direction().DotCross(Quad2.Normale(orig), Quad1.Normale(orig)) > 0.)
        {
          trans1 = IntSurf_Out;
          trans2 = IntSurf_In;
        }
        else
        {
          trans1 = IntSurf_In;
          trans2 = IntSurf_Out;
        }
        glig = new IntPatch_GLine(linsol, Standard_False, trans1, trans2);
        slin.Append(glig);
      }
    }
    break;
      //
    case IntAna_Circle: {
      gp_Circ cirsol;
      gp_Pnt  ptref;
      gp_Vec  Tgt;
      //
      cirsol = inter.Circle(1);
      // modified by NIZNHY-PKV Thu Sep 15 11:30:03 2011f
      AdjustToSeam(Cy, cirsol);
      // modified by NIZNHY-PKV Thu Sep 15 11:30:15 2011t
      ElCLib::D1(0., cirsol, ptref, Tgt);

      if (Tgt.DotCross(Quad2.Normale(ptref), Quad1.Normale(ptref)) > 0.0)
      {
        trans1 = IntSurf_Out;
        trans2 = IntSurf_In;
      }
      else
      {
        trans1 = IntSurf_In;
        trans2 = IntSurf_Out;
      }
      Handle(IntPatch_GLine) glig = new IntPatch_GLine(cirsol, Standard_False, trans1, trans2);
      slin.Append(glig);
    }
    break;
      //
    case IntAna_Ellipse: {
      gp_Elips elipsol = inter.Ellipse(1);
      gp_Pnt   ptref;
      gp_Vec   Tgt;
      ElCLib::D1(0., elipsol, ptref, Tgt);

      if (Tgt.DotCross(Quad2.Normale(ptref), Quad1.Normale(ptref)) > 0.0)
      {
        trans1 = IntSurf_Out;
        trans2 = IntSurf_In;
      }
      else
      {
        trans1 = IntSurf_In;
        trans2 = IntSurf_Out;
      }
      Handle(IntPatch_GLine) glig = new IntPatch_GLine(elipsol, Standard_False, trans1, trans2);
      slin.Append(glig);
    }
    break;
      //
    default: {
      return Standard_False; // on ne doit pas passer ici
    }
  }
  return Standard_True;
}

//=======================================================================
// function : IntPSp
// purpose  :
// Traitement du cas Plan/Sphere et reciproquement
//=======================================================================
Standard_Boolean IntPSp(const IntSurf_Quadric& Quad1,
                        const IntSurf_Quadric& Quad2,
                        // modified by NIZNHY-PKV Tue Sep 20 08:59:36 2011f
                        const Standard_Real Tolang,
                        // modified by NIZNHY-PKV Tue Sep 20 08:59:39 2011t
                        const Standard_Real       TolTang,
                        const Standard_Boolean    Reversed,
                        Standard_Boolean&         Empty,
                        IntPatch_SequenceOfLine&  slin,
                        IntPatch_SequenceOfPoint& spnt)

{
  gp_Circ           cirsol;
  gp_Pln            Pl;
  gp_Sphere         Sp;
  IntSurf_TypeTrans trans1, trans2;
  IntAna_ResultType typint;

  IntAna_QuadQuadGeo inter;
  if (!Reversed)
  {
    Pl = Quad1.Plane();
    Sp = Quad2.Sphere();
  }
  else
  {
    Pl = Quad2.Plane();
    Sp = Quad1.Sphere();
  }
  inter.Perform(Pl, Sp);

  if (!inter.IsDone())
  {
    return Standard_False;
  }

  typint = inter.TypeInter();
  Empty  = Standard_False;

  switch (typint)
  {
    case IntAna_Empty: {
      Empty = Standard_True;
    }
    break;
      //
    case IntAna_Point: {
      gp_Pnt        psol = inter.Point(1);
      Standard_Real U1, V1, U2, V2;
      Quad1.Parameters(psol, U1, V1);
      Quad2.Parameters(psol, U2, V2);
      IntPatch_Point ptsol;
      ptsol.SetValue(psol, TolTang, Standard_True);
      ptsol.SetParameters(U1, V1, U2, V2);
      spnt.Append(ptsol);
    }
    break;
      //
    case IntAna_Circle: {
      cirsol = inter.Circle(1);
      // modified by NIZNHY-PKV Thu Sep 15 11:30:03 2011f
      AdjustToSeam(Sp, cirsol, Tolang);
      // modified by NIZNHY-PKV Thu Sep 15 11:30:15 2011t
      gp_Pnt ptref;
      gp_Vec Tgt;
      ElCLib::D1(0., cirsol, ptref, Tgt);

      if (Tgt.DotCross(Quad2.Normale(ptref), Quad1.Normale(ptref)) > 0.)
      {
        trans1 = IntSurf_Out;
        trans2 = IntSurf_In;
      }
      else
      {
        trans1 = IntSurf_In;
        trans2 = IntSurf_Out;
      }
      Handle(IntPatch_GLine) glig = new IntPatch_GLine(cirsol, Standard_False, trans1, trans2);
      slin.Append(glig);
    }
    break;

    default: {
      return Standard_False; // on ne doit pas passer ici
    }
  }
  return Standard_True;
}

//=======================================================================
// function : IntPCo
// purpose  :
// Traitement du cas Plan/Cone et reciproquement
//=======================================================================
Standard_Boolean IntPCo(const IntSurf_Quadric&    Quad1,
                        const IntSurf_Quadric&    Quad2,
                        const Standard_Real       Tolang,
                        const Standard_Real       TolTang,
                        const Standard_Boolean    Reversed,
                        Standard_Boolean&         Empty,
                        Standard_Boolean&         Multpoint,
                        IntPatch_SequenceOfLine&  slin,
                        IntPatch_SequenceOfPoint& spnt)

{
  gp_Pnt apex;

  gp_Pln  Pl;
  gp_Cone Co;

  IntSurf_TypeTrans trans1, trans2;
  IntAna_ResultType typint;

  IntAna_QuadQuadGeo inter;
  if (!Reversed)
  {
    Pl   = Quad1.Plane();
    Co   = Quad2.Cone();
    apex = Co.Apex();
  }
  else
  {
    Pl   = Quad2.Plane();
    Co   = Quad1.Cone();
    apex = Co.Apex();
  }

  inter.Perform(Pl, Co, Tolang, TolTang);
  if (!inter.IsDone())
  {
    return Standard_False;
  }
  //
  typint                 = inter.TypeInter();
  Standard_Integer NbSol = inter.NbSolutions();
  Empty                  = Standard_False;

  switch (typint)
  {
    case IntAna_Point: {
      gp_Pnt        psol = inter.Point(1);
      Standard_Real U1, V1, U2, V2;
      Quad1.Parameters(psol, U1, V1);
      Quad2.Parameters(psol, U2, V2);
      IntPatch_Point ptsol;
      ptsol.SetValue(psol, TolTang, Standard_False);
      ptsol.SetParameters(U1, V1, U2, V2);
      spnt.Append(ptsol);
    }
    break;

    case IntAna_Line: {
      gp_Lin linsol = inter.Line(1);
      if (linsol.Direction().Dot(Co.Axis().Direction()) < 0.)
      {
        linsol.SetDirection(linsol.Direction().Reversed());
      }
      Standard_Real para = ElCLib::Parameter(linsol, apex);
      gp_Pnt        ptbid(ElCLib::Value(para + 5., linsol));
      Standard_Real U1, V1, U2, V2;
      Quad1.Parameters(apex, U1, V1);
      Quad2.Parameters(apex, U2, V2);

      if (NbSol == 1)
      { // ligne de tangence
        IntPatch_Point ptsol;
        ptsol.SetValue(apex, TolTang, Standard_False);
        ptsol.SetParameters(U1, V1, U2, V2);
        ptsol.SetParameter(para);
        gp_Pnt ptbid2(apex.XYZ() + 5. * Co.Axis().Direction().XYZ());
        gp_Vec TestCurvature(ptbid, ptbid2);
        gp_Vec Normp, Normco;
        if (!Reversed)
        {
          Normp  = Quad1.Normale(ptbid);
          Normco = Quad2.Normale(ptbid);
        }
        else
        {
          Normp  = Quad2.Normale(ptbid);
          Normco = Quad1.Normale(ptbid);
        }
        IntSurf_Situation situco, situco_otherside;
        IntSurf_Situation situp, situp_otherside;

        if (Normp.Dot(TestCurvature) > 0.)
        {
          situco           = IntSurf_Outside;
          situco_otherside = IntSurf_Inside;
          if (Normp.Dot(Normco) > 0.)
          {
            situp           = IntSurf_Inside;
            situp_otherside = IntSurf_Outside;
          }
          else
          {
            situp           = IntSurf_Outside;
            situp_otherside = IntSurf_Inside;
          }
        }
        else
        {
          situco           = IntSurf_Inside;
          situco_otherside = IntSurf_Outside;
          if (Normp.Dot(Normco) > 0.)
          {
            situp           = IntSurf_Outside;
            situp_otherside = IntSurf_Inside;
          }
          else
          {
            situp           = IntSurf_Inside;
            situp_otherside = IntSurf_Outside;
          }
        }
        //----------------------------------------------------------
        //--              Apex ---> Cone.Direction
        //--
        Handle(IntPatch_GLine) glig;
        if (!Reversed)
        {
          glig = new IntPatch_GLine(linsol, Standard_True, situp, situco);
        }
        else
        {
          glig = new IntPatch_GLine(linsol, Standard_True, situco, situp);
        }
        glig->AddVertex(ptsol);
        glig->SetFirstPoint(1);
        slin.Append(glig);
        //----------------------------------------------------------
        //--   -Cone.Direction <------- Apex
        //--
        linsol.SetDirection(linsol.Direction().Reversed());
        if (!Reversed)
        {
          glig = new IntPatch_GLine(linsol, Standard_True, situp_otherside, situco_otherside);
        }
        else
        {
          glig = new IntPatch_GLine(linsol, Standard_True, situco_otherside, situp_otherside);
        }
        glig->AddVertex(ptsol);
        glig->SetFirstPoint(1);
        slin.Append(glig);
      }
      else
      {
        // on a 2 droites. Il faut determiner les transitions
        // de chacune. On oriente chaque ligne dans le sens
        // de l axe du cone. Les transitions de chaque ligne seront
        // inverses l une de l autre => on ne fait le calcul que sur
        // la premiere.
        if (linsol.Direction().DotCross(Quad2.Normale(ptbid), Quad1.Normale(ptbid)) > 0.)
        {
          trans1 = IntSurf_Out;
          trans2 = IntSurf_In;
        }
        else
        {
          trans1 = IntSurf_In;
          trans2 = IntSurf_Out;
        }

        Multpoint = Standard_True;
        //------------------------------------------- Ligne 1 -------
        IntPatch_Point ptsol;
        ptsol.SetValue(apex, TolTang, Standard_False);
        ptsol.SetParameters(U1, V1, U2, V2);
        ptsol.SetParameter(para);
        ptsol.SetMultiple(Standard_True);
        Handle(IntPatch_GLine) glig;
        glig = new IntPatch_GLine(linsol, Standard_False, trans1, trans2);
        glig->AddVertex(ptsol);
        glig->SetFirstPoint(1);
        slin.Append(glig);
        //-----------------------------------------------------------
        //-- Other Side : Les transitions restent les memes
        //--    linsol -> -linsol   et Quad1(2).N -> -Quad1(2).N
        //--
        linsol.SetDirection(linsol.Direction().Reversed());
        glig = new IntPatch_GLine(linsol, Standard_False, trans1, trans2);
        para = ElCLib::Parameter(linsol, apex);
        ptsol.SetParameter(para);
        glig->AddVertex(ptsol);
        glig->SetFirstPoint(1);
        slin.Append(glig);

        //------------------------------------------- Ligne 2 -------
        linsol = inter.Line(2);
        if (linsol.Direction().Dot(Co.Axis().Direction()) < 0.)
        {
          linsol.SetDirection(linsol.Direction().Reversed());
        }
        para  = ElCLib::Parameter(linsol, apex);
        ptbid = ElCLib::Value(para + 5., linsol);
        if (linsol.Direction().DotCross(Quad2.Normale(ptbid), Quad1.Normale(ptbid)) > 0.)
        {
          trans1 = IntSurf_Out;
          trans2 = IntSurf_In;
        }
        else
        {
          trans1 = IntSurf_In;
          trans2 = IntSurf_Out;
        }
        ptsol.SetParameter(para);
        glig = new IntPatch_GLine(linsol, Standard_False, trans1, trans2);
        para = ElCLib::Parameter(linsol, apex);
        ptsol.SetParameter(para);
        glig->AddVertex(ptsol);
        glig->SetFirstPoint(1);
        slin.Append(glig);
        //-----------------------------------------------------------
        //-- Other Side : Les transitions restent les memes
        //--    linsol -> -linsol   et Quad1(2).N -> -Quad1(2).N
        //--
        linsol.SetDirection(linsol.Direction().Reversed());
        glig = new IntPatch_GLine(linsol, Standard_False, trans1, trans2);
        para = ElCLib::Parameter(linsol, apex);
        ptsol.SetParameter(para);
        glig->AddVertex(ptsol);
        glig->SetFirstPoint(1);
        slin.Append(glig);
      }
    }
    break;

    case IntAna_Circle: {
      gp_Circ cirsol = inter.Circle(1);
      // modified by NIZNHY-PKV Thu Sep 15 11:34:04 2011f
      AdjustToSeam(Co, cirsol);
      // modified by NIZNHY-PKV Thu Sep 15 11:36:08 2011t
      gp_Pnt ptref;
      gp_Vec Tgt;
      ElCLib::D1(0., cirsol, ptref, Tgt);

      if (Tgt.DotCross(Quad2.Normale(ptref), Quad1.Normale(ptref)) > 0.)
      {
        trans1 = IntSurf_Out;
        trans2 = IntSurf_In;
      }
      else
      {
        trans1 = IntSurf_In;
        trans2 = IntSurf_Out;
      }
      Handle(IntPatch_GLine) glig = new IntPatch_GLine(cirsol, Standard_False, trans1, trans2);
      slin.Append(glig);
    }
    break;

    case IntAna_Ellipse: {
      gp_Elips elipsol = inter.Ellipse(1);
      gp_Pnt   ptref;
      gp_Vec   Tgt;
      ElCLib::D1(0., elipsol, ptref, Tgt);

      if (Tgt.DotCross(Quad2.Normale(ptref), Quad1.Normale(ptref)) > 0.)
      {
        trans1 = IntSurf_Out;
        trans2 = IntSurf_In;
      }
      else
      {
        trans1 = IntSurf_In;
        trans2 = IntSurf_Out;
      }
      Handle(IntPatch_GLine) glig = new IntPatch_GLine(elipsol, Standard_False, trans1, trans2);
      slin.Append(glig);
    }
    break;

    case IntAna_Parabola: {
      gp_Parab parabsol = inter.Parabola(1);

      gp_Vec        Tgtorig(parabsol.YAxis().Direction());
      Standard_Real ptran =
        Tgtorig.DotCross(Quad2.Normale(parabsol.Location()), Quad1.Normale(parabsol.Location()));
      if (ptran > 0.00000001)
      {
        trans1 = IntSurf_Out;
        trans2 = IntSurf_In;
      }
      else if (ptran < -0.00000001)
      {
        trans1 = IntSurf_In;
        trans2 = IntSurf_Out;
      }
      else
      {
        trans1 = trans2 = IntSurf_Undecided;
      }
      Handle(IntPatch_GLine) glig = new IntPatch_GLine(parabsol, Standard_False, trans1, trans2);
      slin.Append(glig);
    }
    break;

    case IntAna_Hyperbola: {
      gp_Pnt tophypr;
      gp_Vec Tgttop;

      for (Standard_Integer i = 1; i <= 2; i++)
      {
        gp_Hypr hyprsol   = inter.Hyperbola(i);
        tophypr           = ElCLib::Value(hyprsol.MajorRadius(), hyprsol.XAxis());
        Tgttop            = hyprsol.YAxis().Direction();
        Standard_Real qwe = Tgttop.DotCross(Quad2.Normale(tophypr), Quad1.Normale(tophypr));

        if (qwe > 0.00000001)
        {
          trans1 = IntSurf_Out;
          trans2 = IntSurf_In;
        }
        else if (qwe < -0.00000001)
        {
          trans1 = IntSurf_In;
          trans2 = IntSurf_Out;
        }
        else
        {
          trans1 = trans2 = IntSurf_Undecided;
        }
        Handle(IntPatch_GLine) glig = new IntPatch_GLine(hyprsol, Standard_False, trans1, trans2);
        slin.Append(glig);
      }
    }
    break;

    default: {
      return Standard_False;
    }
  }
  return Standard_True;
}

//=================================================================================================

Standard_Boolean IntPTo(const IntSurf_Quadric&   theQuad1,
                        const IntSurf_Quadric&   theQuad2,
                        const Standard_Real      theTolTang,
                        const Standard_Boolean   bReversed,
                        Standard_Boolean&        bEmpty,
                        IntPatch_SequenceOfLine& theSeqLin)
{
  const gp_Pln   aPln   = bReversed ? theQuad2.Plane() : theQuad1.Plane();
  const gp_Torus aTorus = bReversed ? theQuad1.Torus() : theQuad2.Torus();
  //
  IntAna_QuadQuadGeo inter(aPln, aTorus, theTolTang);
  Standard_Boolean   bRet = inter.IsDone();
  //
  if (!bRet)
  {
    return bRet;
  }
  //
  IntAna_ResultType typint = inter.TypeInter();
  Standard_Integer  NbSol  = inter.NbSolutions();
  bEmpty                   = Standard_False;
  //
  switch (typint)
  {
    case IntAna_Empty:
      bEmpty = Standard_True;
      break;
    //
    case IntAna_Circle: {
      Standard_Integer  i;
      IntSurf_TypeTrans trans1, trans2;
      gp_Pnt            ptref;
      gp_Vec            Tgt;
      //
      for (i = 1; i <= NbSol; ++i)
      {
        gp_Circ aC = inter.Circle(i);
        if (!aPln.Axis().IsNormal(aTorus.Axis(), Precision::Angular()))
        {
          AdjustToSeam(aTorus, aC);
        }
        ElCLib::D1(0., aC, ptref, Tgt);
        //
        if (Tgt.DotCross(theQuad2.Normale(ptref), theQuad1.Normale(ptref)) > 0.0)
        {
          trans1 = IntSurf_Out;
          trans2 = IntSurf_In;
        }
        else
        {
          trans1 = IntSurf_In;
          trans2 = IntSurf_Out;
        }
        //
        Handle(IntPatch_GLine) glig = new IntPatch_GLine(aC, Standard_False, trans1, trans2);
        theSeqLin.Append(glig);
      }
    }
    break;
    //
    case IntAna_NoGeometricSolution:
    default:
      bRet = Standard_False;
      break;
  }
  //
  return bRet;
}

//
// modified by NIZNHY-PKV Thu Sep 15 10:53:39 2011f
//=================================================================================================

void AdjustToSeam(const gp_Cone& aQuad, gp_Circ& aCirc)
{
  gp_Ax2 aAx2;
  //
  const gp_Pnt& aPLoc = aCirc.Location();
  const gp_Ax3& aAx3  = aQuad.Position();
  SeamPosition(aPLoc, aAx3, aAx2);
  aCirc.SetPosition(aAx2);
}

//=================================================================================================

void AdjustToSeam(const gp_Sphere& aQuad, gp_Circ& aCirc, const Standard_Real aTolAng)
{
  gp_Ax2 aAx2;
  //
  const gp_Ax1& aAx1C = aCirc.Axis();
  const gp_Ax3& aAx3  = aQuad.Position();
  const gp_Ax1& aAx1Q = aAx3.Axis();
  //
  const gp_Dir& aDirC = aAx1C.Direction();
  const gp_Dir& aDirQ = aAx1Q.Direction();
  if (aDirC.IsParallel(aDirQ, aTolAng))
  {
    const gp_Pnt& aPLoc = aCirc.Location();
    SeamPosition(aPLoc, aAx3, aAx2);
    aCirc.SetPosition(aAx2);
  }
}

//=================================================================================================

void AdjustToSeam(const gp_Cylinder& aQuad, gp_Circ& aCirc)
{
  gp_Ax2 aAx2;
  //
  const gp_Pnt& aPLoc = aCirc.Location();
  const gp_Ax3& aAx3  = aQuad.Position();
  SeamPosition(aPLoc, aAx3, aAx2);
  aCirc.SetPosition(aAx2);
}

//=================================================================================================

void AdjustToSeam(const gp_Torus& aQuad, gp_Circ& aCirc)
{
  gp_Ax2 aAx2;
  //
  const gp_Pnt& aPLoc = aCirc.Location();
  const gp_Ax3& aAx3  = aQuad.Position();
  SeamPosition(aPLoc, aAx3, aAx2);
  aCirc.SetPosition(aAx2);
}

//=================================================================================================

void SeamPosition(const gp_Pnt& aPLoc, const gp_Ax3& aPos, gp_Ax2& aSeamPos)
{
  const gp_Dir& aDZ = aPos.Direction();
  const gp_Dir& aDX = aPos.XDirection();
  gp_Ax2        aAx2(aPLoc, aDZ, aDX);
  aSeamPos = aAx2;
}

// modified by NIZNHY-PKV Thu Sep 15 10:53:41 2011t

// If Abs(a) <= aNulValue then it is considered that a = 0.
static const Standard_Real aNulValue = 1.0e-11;

static void ShortCosForm(const Standard_Real theCosFactor,
                         const Standard_Real theSinFactor,
                         Standard_Real&      theCoeff,
                         Standard_Real&      theAngle);
//
static Standard_Boolean ExploreCurve(const gp_Cone&      theCo,
                                     IntAna_Curve&       aC,
                                     const Standard_Real aTol,
                                     IntAna_ListOfCurve& aLC);

static Standard_Boolean InscribePoint(const Standard_Real    theUfTarget,
                                      const Standard_Real    theUlTarget,
                                      Standard_Real&         theUGiven,
                                      const Standard_Real    theTol2D,
                                      const Standard_Real    thePeriod,
                                      const Standard_Boolean theFlForce);

class ComputationMethods
{
  // Every cylinder can be represented by the following equation in parametric form:
  //     S(U,V) = L + R*cos(U)*Xd+R*sin(U)*Yd+V*Zd,
  // where location L, directions Xd, Yd and Zd have type gp_XYZ.

  // Intersection points between two cylinders can be found from the following system:
  //     S1(U1, V1) = S2(U2, V2)
  // or
  //     {X01 + R1*cos(U1)*Xx1 + R1*sin(U1)*Yx1 + V1*Zx1 = X02 + R2*cos(U2)*Xx2 + R2*sin(U2)*Yx2 +
  //     V2*Zx2 {Y01 + R1*cos(U1)*Xy1 + R1*sin(U1)*Yy1 + V1*Zy1 = Y02 + R2*cos(U2)*Xy2 +
  //     R2*sin(U2)*Yy2 + V2*Zy2   (1) {Z01 + R1*cos(U1)*Xz1 + R1*sin(U1)*Yz1 + V1*Zz1 = Z02 +
  //     R2*cos(U2)*Xz2 + R2*sin(U2)*Yz2 + V2*Zz2

  // The formula (1) can be rewritten as follows
  //     {C11*V1+C21*V2=A11*cos(U1)+B11*sin(U1)+A21*cos(U2)+B21*sin(U2)+D1
  //     {C12*V1+C22*V2=A12*cos(U1)+B12*sin(U1)+A22*cos(U2)+B22*sin(U2)+D2 (2)
  //     {C13*V1+C23*V2=A13*cos(U1)+B13*sin(U1)+A23*cos(U2)+B23*sin(U2)+D3

  // Hereafter we consider that in system
  //     {C11*V1+C21*V2=A11*cos(U1)+B11*sin(U1)+A21*cos(U2)+B21*sin(U2)+D1 (3)
  //     {C12*V1+C22*V2=A12*cos(U1)+B12*sin(U1)+A22*cos(U2)+B22*sin(U2)+D2
  // variables V1 and V2 can be found unambiguously, i.e. determinant
  //             |C11 C21|
  //             |       | != 0
  //             |C12 C22|
  //
  // In this case, variables V1 and V2 can be found as follows:
  //     {V1 = K11*sin(U1)+K21*sin(U2)+L11*cos(U1)+L21*cos(U2)+M1 =
  //     K1*cos(U1-FIV1)+L1*cos(U2-PSIV1)+M1      (4) {V2 =
  //     K12*sin(U1)+K22*sin(U2)+L12*cos(U1)+L22*cos(U2)+M2 = K2*cos(U2-FIV2)+L2*cos(U2-PSIV2)+M2

  // Having substituted result of (4) to the 3rd equation of (2), we will obtain equation
  //     cos(U2-FI2) = B*cos(U1-FI1)+C. (5)

  // I.e. when U1 is taken different given values (from domain), correspond U2 value can be computed
  // from equation (5). After that, V1 and V2 can be computed from the system (4) (see
  // CylCylComputeParameters(...) methods).

  // It is important to remark that equation (5) (in general) has two solutions: U2=FI2 +/- f(U1).
  // Therefore, we are getting here two intersection lines.

public:
  // Stores equations coefficients
  struct stCoeffsValue
  {
    stCoeffsValue(const gp_Cylinder&, const gp_Cylinder&);

    math_Vector mVecA1;
    math_Vector mVecA2;
    math_Vector mVecB1;
    math_Vector mVecB2;
    math_Vector mVecC1;
    math_Vector mVecC2;
    math_Vector mVecD;

    Standard_Real mK21; // sinU2
    Standard_Real mK11; // sinU1
    Standard_Real mL21; // cosU2
    Standard_Real mL11; // cosU1
    Standard_Real mM1;  // Free member

    Standard_Real mK22; // sinU2
    Standard_Real mK12; // sinU1
    Standard_Real mL22; // cosU2
    Standard_Real mL12; // cosU1
    Standard_Real mM2;  // Free member

    Standard_Real mK1;
    Standard_Real mL1;
    Standard_Real mK2;
    Standard_Real mL2;

    Standard_Real mFIV1;
    Standard_Real mPSIV1;
    Standard_Real mFIV2;
    Standard_Real mPSIV2;

    Standard_Real mB;
    Standard_Real mC;
    Standard_Real mFI1;
    Standard_Real mFI2;
  };

  //! Determines, if U2(U1) function is increasing.
  static Standard_Boolean CylCylMonotonicity(const Standard_Real    theU1par,
                                             const Standard_Integer theWLIndex,
                                             const stCoeffsValue&   theCoeffs,
                                             const Standard_Real    thePeriod,
                                             Standard_Boolean&      theIsIncreasing);

  //! Computes U2 (U-parameter of the 2nd cylinder) and, if theDelta != 0,
  //! esimates the tolerance of U2-computing (estimation result is
  //! assigned to *theDelta value).
  static Standard_Boolean CylCylComputeParameters(const Standard_Real    theU1par,
                                                  const Standard_Integer theWLIndex,
                                                  const stCoeffsValue&   theCoeffs,
                                                  Standard_Real&         theU2,
                                                  Standard_Real* const   theDelta = 0);

  static Standard_Boolean CylCylComputeParameters(const Standard_Real  theU1,
                                                  const Standard_Real  theU2,
                                                  const stCoeffsValue& theCoeffs,
                                                  Standard_Real&       theV1,
                                                  Standard_Real&       theV2);

  static Standard_Boolean CylCylComputeParameters(const Standard_Real    theU1par,
                                                  const Standard_Integer theWLIndex,
                                                  const stCoeffsValue&   theCoeffs,
                                                  Standard_Real&         theU2,
                                                  Standard_Real&         theV1,
                                                  Standard_Real&         theV2);
};

ComputationMethods::stCoeffsValue::stCoeffsValue(const gp_Cylinder& theCyl1,
                                                 const gp_Cylinder& theCyl2)
    : mVecA1(-theCyl1.Radius() * theCyl1.XAxis().Direction().XYZ()),
      mVecA2(theCyl2.Radius() * theCyl2.XAxis().Direction().XYZ()),
      mVecB1(-theCyl1.Radius() * theCyl1.YAxis().Direction().XYZ()),
      mVecB2(theCyl2.Radius() * theCyl2.YAxis().Direction().XYZ()),
      mVecC1(theCyl1.Axis().Direction().XYZ()),
      mVecC2(theCyl2.Axis().Direction().XYZ().Reversed()),
      mVecD(theCyl2.Location().XYZ() - theCyl1.Location().XYZ())
{
  enum CoupleOfEquation
  {
    COENONE = 0,
    COE12   = 1,
    COE23   = 2,
    COE13   = 3
  } aFoundCouple = COENONE;

  Standard_Real aDetV1V2 = 0.0;

  const Standard_Real aDelta1 = mVecC1(1) * mVecC2(2) - mVecC1(2) * mVecC2(1); // 1-2
  const Standard_Real aDelta2 = mVecC1(2) * mVecC2(3) - mVecC1(3) * mVecC2(2); // 2-3
  const Standard_Real aDelta3 = mVecC1(1) * mVecC2(3) - mVecC1(3) * mVecC2(1); // 1-3
  const Standard_Real anAbsD1 = Abs(aDelta1);                                  // 1-2
  const Standard_Real anAbsD2 = Abs(aDelta2);                                  // 2-3
  const Standard_Real anAbsD3 = Abs(aDelta3);                                  // 1-3

  if (anAbsD1 >= anAbsD2)
  {
    if (anAbsD3 > anAbsD1)
    {
      aFoundCouple = COE13;
      aDetV1V2     = aDelta3;
    }
    else
    {
      aFoundCouple = COE12;
      aDetV1V2     = aDelta1;
    }
  }
  else
  {
    if (anAbsD3 > anAbsD2)
    {
      aFoundCouple = COE13;
      aDetV1V2     = aDelta3;
    }
    else
    {
      aFoundCouple = COE23;
      aDetV1V2     = aDelta2;
    }
  }

  // In point of fact, every determinant (aDelta1, aDelta2 and aDelta3) is
  // cross-product between directions (i.e. sine of angle).
  // If sine is too small then sine is (approx.) equal to angle itself.
  // Therefore, in this case we should compare sine with angular tolerance.
  // This constant is used for check if axes are parallel (see constructor
  // AxeOperator::AxeOperator(...) in IntAna_QuadQuadGeo.cxx file).
  if (Abs(aDetV1V2) < Precision::Angular())
  {
    throw Standard_Failure("Error. Exception in divide by zerro (IntCyCyTrim)!!!!");
  }

  switch (aFoundCouple)
  {
    case COE12:
      break;
    case COE23: {
      math_Vector aVTemp(mVecA1);
      mVecA1(1) = aVTemp(2);
      mVecA1(2) = aVTemp(3);
      mVecA1(3) = aVTemp(1);

      aVTemp    = mVecA2;
      mVecA2(1) = aVTemp(2);
      mVecA2(2) = aVTemp(3);
      mVecA2(3) = aVTemp(1);

      aVTemp    = mVecB1;
      mVecB1(1) = aVTemp(2);
      mVecB1(2) = aVTemp(3);
      mVecB1(3) = aVTemp(1);

      aVTemp    = mVecB2;
      mVecB2(1) = aVTemp(2);
      mVecB2(2) = aVTemp(3);
      mVecB2(3) = aVTemp(1);

      aVTemp    = mVecC1;
      mVecC1(1) = aVTemp(2);
      mVecC1(2) = aVTemp(3);
      mVecC1(3) = aVTemp(1);

      aVTemp    = mVecC2;
      mVecC2(1) = aVTemp(2);
      mVecC2(2) = aVTemp(3);
      mVecC2(3) = aVTemp(1);

      aVTemp   = mVecD;
      mVecD(1) = aVTemp(2);
      mVecD(2) = aVTemp(3);
      mVecD(3) = aVTemp(1);

      break;
    }
    case COE13: {
      math_Vector aVTemp = mVecA1;
      mVecA1(2)          = aVTemp(3);
      mVecA1(3)          = aVTemp(2);

      aVTemp    = mVecA2;
      mVecA2(2) = aVTemp(3);
      mVecA2(3) = aVTemp(2);

      aVTemp    = mVecB1;
      mVecB1(2) = aVTemp(3);
      mVecB1(3) = aVTemp(2);

      aVTemp    = mVecB2;
      mVecB2(2) = aVTemp(3);
      mVecB2(3) = aVTemp(2);

      aVTemp    = mVecC1;
      mVecC1(2) = aVTemp(3);
      mVecC1(3) = aVTemp(2);

      aVTemp    = mVecC2;
      mVecC2(2) = aVTemp(3);
      mVecC2(3) = aVTemp(2);

      aVTemp   = mVecD;
      mVecD(2) = aVTemp(3);
      mVecD(3) = aVTemp(2);

      break;
    }
    default:
      break;
  }

  //------- For V1 (begin)
  // sinU2
  mK21 = (mVecC2(2) * mVecB2(1) - mVecC2(1) * mVecB2(2)) / aDetV1V2;
  // sinU1
  mK11 = (mVecC2(2) * mVecB1(1) - mVecC2(1) * mVecB1(2)) / aDetV1V2;
  // cosU2
  mL21 = (mVecC2(2) * mVecA2(1) - mVecC2(1) * mVecA2(2)) / aDetV1V2;
  // cosU1
  mL11 = (mVecC2(2) * mVecA1(1) - mVecC2(1) * mVecA1(2)) / aDetV1V2;
  // Free member
  mM1 = (mVecC2(2) * mVecD(1) - mVecC2(1) * mVecD(2)) / aDetV1V2;
  //------- For V1 (end)

  //------- For V2 (begin)
  // sinU2
  mK22 = (mVecC1(1) * mVecB2(2) - mVecC1(2) * mVecB2(1)) / aDetV1V2;
  // sinU1
  mK12 = (mVecC1(1) * mVecB1(2) - mVecC1(2) * mVecB1(1)) / aDetV1V2;
  // cosU2
  mL22 = (mVecC1(1) * mVecA2(2) - mVecC1(2) * mVecA2(1)) / aDetV1V2;
  // cosU1
  mL12 = (mVecC1(1) * mVecA1(2) - mVecC1(2) * mVecA1(1)) / aDetV1V2;
  // Free member
  mM2 = (mVecC1(1) * mVecD(2) - mVecC1(2) * mVecD(1)) / aDetV1V2;
  //------- For V1 (end)

  ShortCosForm(mL11, mK11, mK1, mFIV1);
  ShortCosForm(mL21, mK21, mL1, mPSIV1);
  ShortCosForm(mL12, mK12, mK2, mFIV2);
  ShortCosForm(mL22, mK22, mL2, mPSIV2);

  const Standard_Real aA1 = mVecC1(3) * mK21 + mVecC2(3) * mK22 - mVecB2(3), // sinU2
    aA2                   = mVecC1(3) * mL21 + mVecC2(3) * mL22 - mVecA2(3), // cosU2
    aB1                   = mVecB1(3) - mVecC1(3) * mK11 - mVecC2(3) * mK12, // sinU1
    aB2                   = mVecA1(3) - mVecC1(3) * mL11 - mVecC2(3) * mL12; // cosU1

  mC = mVecD(3) - mVecC1(3) * mM1 - mVecC2(3) * mM2; // Free

  Standard_Real aA = 0.0;

  ShortCosForm(aB2, aB1, mB, mFI1);
  ShortCosForm(aA2, aA1, aA, mFI2);

  mB /= aA;
  mC /= aA;
}

class WorkWithBoundaries
{
public:
  enum SearchBoundType
  {
    SearchNONE = 0,
    SearchV1   = 1,
    SearchV2   = 2
  };

  struct StPInfo
  {
    StPInfo()
    {
      mySurfID = 0;
      myU1     = RealLast();
      myV1     = RealLast();
      myU2     = RealLast();
      myV2     = RealLast();
    }

    // Equal to 0 for 1st surface non-zero for 2nd one.
    Standard_Integer mySurfID;

    Standard_Real myU1;
    Standard_Real myV1;
    Standard_Real myU2;
    Standard_Real myV2;

    bool operator>(const StPInfo& theOther) const { return myU1 > theOther.myU1; }

    bool operator<(const StPInfo& theOther) const { return myU1 < theOther.myU1; }

    bool operator==(const StPInfo& theOther) const { return myU1 == theOther.myU1; }
  };

  WorkWithBoundaries(const IntSurf_Quadric&                   theQuad1,
                     const IntSurf_Quadric&                   theQuad2,
                     const ComputationMethods::stCoeffsValue& theCoeffs,
                     const Bnd_Box2d&                         theUVSurf1,
                     const Bnd_Box2d&                         theUVSurf2,
                     const Standard_Integer                   theNbWLines,
                     const Standard_Real                      thePeriod,
                     const Standard_Real                      theTol3D,
                     const Standard_Real                      theTol2D,
                     const Standard_Boolean                   isTheReverse)
      : myQuad1(theQuad1),
        myQuad2(theQuad2),
        myCoeffs(theCoeffs),
        myUVSurf1(theUVSurf1),
        myUVSurf2(theUVSurf2),
        myNbWLines(theNbWLines),
        myPeriod(thePeriod),
        myTol3D(theTol3D),
        myTol2D(theTol2D),
        myIsReverse(isTheReverse) {};

  // Returns parameters of system solved while finding
  // intersection line
  const ComputationMethods::stCoeffsValue& SICoeffs() const { return myCoeffs; }

  // Returns quadric correspond to the index theIdx.
  const IntSurf_Quadric& GetQSurface(const Standard_Integer theIdx) const
  {
    if (theIdx <= 1)
      return myQuad1;

    return myQuad2;
  }

  // Returns TRUE in case of reverting surfaces
  Standard_Boolean IsReversed() const { return myIsReverse; }

  // Returns 2D-tolerance
  Standard_Real Get2dTolerance() const { return myTol2D; }

  // Returns 3D-tolerance
  Standard_Real Get3dTolerance() const { return myTol3D; }

  // Returns UV-bounds of 1st surface
  const Bnd_Box2d& UVS1() const { return myUVSurf1; }

  // Returns UV-bounds of 2nd surface
  const Bnd_Box2d& UVS2() const { return myUVSurf2; }

  void AddBoundaryPoint(const Handle(IntPatch_WLine)& theWL,
                        const Standard_Real           theU1,
                        const Standard_Real           theU1Min,
                        const Standard_Real           theU2,
                        const Standard_Real           theV1,
                        const Standard_Real           theV1Prev,
                        const Standard_Real           theV2,
                        const Standard_Real           theV2Prev,
                        const Standard_Integer        theWLIndex,
                        const Standard_Boolean        theFlForce,
                        Standard_Boolean&             isTheFound1,
                        Standard_Boolean&             isTheFound2) const;

  static Standard_Boolean BoundariesComputing(const ComputationMethods::stCoeffsValue& theCoeffs,
                                              const Standard_Real                      thePeriod,
                                              Bnd_Range                                theURange[]);

  void BoundaryEstimation(const gp_Cylinder& theCy1,
                          const gp_Cylinder& theCy2,
                          Bnd_Range&         theOutBoxS1,
                          Bnd_Range&         theOutBoxS2) const;

protected:
  // Solves equation (2) (see declaration of ComputationMethods class) in case,
  // when V1 or V2 (is set by theSBType argument) is known (corresponds to the boundary
  // and equal to theVzad) but U1 is unknown. Computation is made by numeric methods and
  // requires initial values (theVInit, theInitU2 and theInitMainVar).
  Standard_Boolean SearchOnVBounds(const SearchBoundType theSBType,
                                   const Standard_Real   theVzad,
                                   const Standard_Real   theVInit,
                                   const Standard_Real   theInitU2,
                                   const Standard_Real   theInitMainVar,
                                   Standard_Real&        theMainVariableValue) const;

  const WorkWithBoundaries& operator=(const WorkWithBoundaries&);

private:
  friend class ComputationMethods;

  const IntSurf_Quadric&                   myQuad1;
  const IntSurf_Quadric&                   myQuad2;
  const ComputationMethods::stCoeffsValue& myCoeffs;
  const Bnd_Box2d&                         myUVSurf1;
  const Bnd_Box2d&                         myUVSurf2;
  const Standard_Integer                   myNbWLines;
  const Standard_Real                      myPeriod;
  const Standard_Real                      myTol3D;
  const Standard_Real                      myTol2D;
  const Standard_Boolean                   myIsReverse;
};

static void SeekAdditionalPoints(const IntSurf_Quadric&                   theQuad1,
                                 const IntSurf_Quadric&                   theQuad2,
                                 const Handle(IntSurf_LineOn2S)&          theLine,
                                 const ComputationMethods::stCoeffsValue& theCoeffs,
                                 const Standard_Integer                   theWLIndex,
                                 const Standard_Integer                   theMinNbPoints,
                                 const Standard_Integer                   theStartPointOnLine,
                                 const Standard_Integer                   theEndPointOnLine,
                                 const Standard_Real                      theTol2D,
                                 const Standard_Real                      thePeriodOfSurf2,
                                 const Standard_Boolean                   isTheReverse);

//=======================================================================
// function : MinMax
// purpose  : Replaces theParMIN = MIN(theParMIN, theParMAX),
//                    theParMAX = MAX(theParMIN, theParMAX).
//=======================================================================
static inline void MinMax(Standard_Real& theParMIN, Standard_Real& theParMAX)
{
  if (theParMIN > theParMAX)
  {
    const Standard_Real aux = theParMAX;
    theParMAX               = theParMIN;
    theParMIN               = aux;
  }
}

//=======================================================================
// function : ExtremaLineLine
// purpose  : Computes extrema between the given lines. Returns parameters
//          on correspond curve (see correspond method for Extrema_ExtElC class).
//=======================================================================
static inline void ExtremaLineLine(const gp_Ax1&       theC1,
                                   const gp_Ax1&       theC2,
                                   const Standard_Real theCosA,
                                   const Standard_Real theSqSinA,
                                   Standard_Real&      thePar1,
                                   Standard_Real&      thePar2)
{
  const gp_Dir &aD1 = theC1.Direction(), &aD2 = theC2.Direction();

  const gp_XYZ        aL1L2 = theC2.Location().XYZ() - theC1.Location().XYZ();
  const Standard_Real aD1L = aD1.XYZ().Dot(aL1L2), aD2L = aD2.XYZ().Dot(aL1L2);

  thePar1 = (aD1L - theCosA * aD2L) / theSqSinA;
  thePar2 = (theCosA * aD1L - aD2L) / theSqSinA;
}

//=======================================================================
// function : VBoundaryPrecise
// purpose  : By default, we shall consider, that V1 and V2 will be increased
//            if U1 is increased. But if it is not, new V1set and/or V2set
//            must be computed as [V1current - DeltaV1] (analogically
//            for V2). This function processes this case.
//=======================================================================
static void VBoundaryPrecise(const math_Matrix&  theMatr,
                             const Standard_Real theV1AfterDecrByDelta,
                             const Standard_Real theV2AfterDecrByDelta,
                             Standard_Real&      theV1Set,
                             Standard_Real&      theV2Set)
{
  // Now we are going to define if V1 (and V2) increases
  //(or decreases) when U1 will increase.
  const Standard_Integer aNbDim = 3;
  math_Matrix            aSyst(1, aNbDim, 1, aNbDim);

  aSyst.SetCol(1, theMatr.Col(1));
  aSyst.SetCol(2, theMatr.Col(2));
  aSyst.SetCol(3, theMatr.Col(4));

  // We have the system (see comment to StepComputing(...) function)
  //     {a11*dV1 + a12*dV2 + a14*dU2 = -a13*dU1
  //     {a21*dV1 + a22*dV2 + a24*dU2 = -a23*dU1
  //     {a31*dV1 + a32*dV2 + a34*dU2 = -a33*dU1

  const Standard_Real aDet = aSyst.Determinant();

  aSyst.SetCol(1, theMatr.Col(3));
  const Standard_Real aDet1 = aSyst.Determinant();

  aSyst.SetCol(1, theMatr.Col(1));
  aSyst.SetCol(2, theMatr.Col(3));

  const Standard_Real aDet2 = aSyst.Determinant();

  // Now,
  //     dV1 = -dU1*aDet1/aDet
  //     dV2 = -dU1*aDet2/aDet

  // If U1 is increased then dU1 > 0.
  // If (aDet1/aDet > 0) then dV1 < 0 and
  // V1 will be decreased after increasing U1.

  // We have analogical situation with V2-parameter.

  if (aDet * aDet1 > 0.0)
  {
    theV1Set = theV1AfterDecrByDelta;
  }

  if (aDet * aDet2 > 0.0)
  {
    theV2Set = theV2AfterDecrByDelta;
  }
}

//=======================================================================
// function : DeltaU1Computing
// purpose  : Computes new step for U1 parameter.
//=======================================================================
static inline Standard_Boolean DeltaU1Computing(const math_Matrix& theSyst,
                                                const math_Vector& theFree,
                                                Standard_Real&     theDeltaU1Found)
{
  Standard_Real aDet = theSyst.Determinant();

  if (Abs(aDet) > aNulValue)
  {
    math_Matrix aSyst1(theSyst);
    aSyst1.SetCol(2, theFree);

    theDeltaU1Found = Abs(aSyst1.Determinant() / aDet);
    return Standard_True;
  }

  return Standard_False;
}

//=======================================================================
// function : StepComputing
// purpose  :
//
// Attention!!!:
//            theMatr must have 3*5-dimension strictly.
//            For system
//                {a11*V1+a12*V2+a13*dU1+a14*dU2=b1;
//                {a21*V1+a22*V2+a23*dU1+a24*dU2=b2;
//                {a31*V1+a32*V2+a33*dU1+a34*dU2=b3;
//            theMatr must be following:
//                (a11 a12 a13 a14 b1)
//                (a21 a22 a23 a24 b2)
//                (a31 a32 a33 a34 b3)
//=======================================================================
static Standard_Boolean StepComputing(const math_Matrix& theMatr,
                                      const Standard_Real theV1Cur,
                                      const Standard_Real theV2Cur,
                                      const Standard_Real theDeltaV1,
                                      const Standard_Real theDeltaV2,
                                      Standard_Real& theDeltaU1Found/*,
                                      Standard_Real& theDeltaU2Found,
                                      Standard_Real& theV1Found,
                                      Standard_Real& theV2Found*/)
{
#ifdef INTPATCH_IMPIMPINTERSECTION_DEBUG
  bool flShow = false;

  if (flShow)
  {
    printf("{%+10.20f*V1 + %+10.20f*V2 + %+10.20f*dU1 + %+10.20f*dU2 = %+10.20f\n",
           theMatr(1, 1),
           theMatr(1, 2),
           theMatr(1, 3),
           theMatr(1, 4),
           theMatr(1, 5));
    printf("{%+10.20f*V1 + %+10.20f*V2 + %+10.20f*dU1 + %+10.20f*dU2 = %+10.20f\n",
           theMatr(2, 1),
           theMatr(2, 2),
           theMatr(2, 3),
           theMatr(2, 4),
           theMatr(2, 5));
    printf("{%+10.20f*V1 + %+10.20f*V2 + %+10.20f*dU1 + %+10.20f*dU2 = %+10.20f\n",
           theMatr(3, 1),
           theMatr(3, 2),
           theMatr(3, 3),
           theMatr(3, 4),
           theMatr(3, 5));
  }
#endif

  Standard_Boolean isSuccess             = Standard_False;
  theDeltaU1Found /* = theDeltaU2Found*/ = RealLast();
  // theV1Found = theV1set;
  // theV2Found = theV2Set;
  const Standard_Integer aNbDim = 3;

  math_Matrix aSyst(1, aNbDim, 1, aNbDim);
  math_Vector aFree(1, aNbDim);

  // By default, increasing V1(U1) and V2(U1) functions is
  // considered
  Standard_Real aV1Set = theV1Cur + theDeltaV1, aV2Set = theV2Cur + theDeltaV2;

  // However, what is indeed?
  VBoundaryPrecise(theMatr, theV1Cur - theDeltaV1, theV2Cur - theDeltaV2, aV1Set, aV2Set);

  aSyst.SetCol(2, theMatr.Col(3));
  aSyst.SetCol(3, theMatr.Col(4));

  for (Standard_Integer i = 0; i < 2; i++)
  {
    if (i == 0)
    { // V1 is known
      aSyst.SetCol(1, theMatr.Col(2));
      aFree.Set(1, aNbDim, theMatr.Col(5) - aV1Set * theMatr.Col(1));
    }
    else
    { // i==1 => V2 is known
      aSyst.SetCol(1, theMatr.Col(1));
      aFree.Set(1, aNbDim, theMatr.Col(5) - aV2Set * theMatr.Col(2));
    }

    Standard_Real aNewDU = theDeltaU1Found;
    if (DeltaU1Computing(aSyst, aFree, aNewDU))
    {
      isSuccess = Standard_True;
      if (aNewDU < theDeltaU1Found)
      {
        theDeltaU1Found = aNewDU;
      }
    }
  }

  if (!isSuccess)
  {
    aFree = theMatr.Col(5) - aV1Set * theMatr.Col(1) - aV2Set * theMatr.Col(2);
    math_Matrix aSyst1(1, aNbDim, 1, 2);
    aSyst1.SetCol(1, aSyst.Col(2));
    aSyst1.SetCol(2, aSyst.Col(3));

    // Now we have overdetermined system.

    const Standard_Real aDet1   = theMatr(1, 3) * theMatr(2, 4) - theMatr(2, 3) * theMatr(1, 4);
    const Standard_Real aDet2   = theMatr(1, 3) * theMatr(3, 4) - theMatr(3, 3) * theMatr(1, 4);
    const Standard_Real aDet3   = theMatr(2, 3) * theMatr(3, 4) - theMatr(3, 3) * theMatr(2, 4);
    const Standard_Real anAbsD1 = Abs(aDet1);
    const Standard_Real anAbsD2 = Abs(aDet2);
    const Standard_Real anAbsD3 = Abs(aDet3);

    if (anAbsD1 >= anAbsD2)
    {
      if (anAbsD1 >= anAbsD3)
      {
        // Det1
        if (anAbsD1 <= aNulValue)
          return isSuccess;

        theDeltaU1Found = Abs(aFree(1) * theMatr(2, 4) - aFree(2) * theMatr(1, 4)) / anAbsD1;
        isSuccess       = Standard_True;
      }
      else
      {
        // Det3
        if (anAbsD3 <= aNulValue)
          return isSuccess;

        theDeltaU1Found = Abs(aFree(2) * theMatr(3, 4) - aFree(3) * theMatr(2, 4)) / anAbsD3;
        isSuccess       = Standard_True;
      }
    }
    else
    {
      if (anAbsD2 >= anAbsD3)
      {
        // Det2
        if (anAbsD2 <= aNulValue)
          return isSuccess;

        theDeltaU1Found = Abs(aFree(1) * theMatr(3, 4) - aFree(3) * theMatr(1, 4)) / anAbsD2;
        isSuccess       = Standard_True;
      }
      else
      {
        // Det3
        if (anAbsD3 <= aNulValue)
          return isSuccess;

        theDeltaU1Found = Abs(aFree(2) * theMatr(3, 4) - aFree(3) * theMatr(2, 4)) / anAbsD3;
        isSuccess       = Standard_True;
      }
    }
  }

  return isSuccess;
}

//=================================================================================================

void ProcessBounds(const Handle(IntPatch_ALine)&  alig, //-- ligne courante
                   const IntPatch_SequenceOfLine& slin,
                   const IntSurf_Quadric&         Quad1,
                   const IntSurf_Quadric&         Quad2,
                   Standard_Boolean&              procf,
                   const gp_Pnt&                  ptf,   //-- Debut Ligne Courante
                   const Standard_Real            first, //-- Paramf
                   Standard_Boolean&              procl,
                   const gp_Pnt&                  ptl,  //-- Fin Ligne courante
                   const Standard_Real            last, //-- Paraml
                   Standard_Boolean&              Multpoint,
                   const Standard_Real            Tol)
{
  Standard_Integer j, k;
  Standard_Real    U1, V1, U2, V2;
  IntPatch_Point   ptsol;
  Standard_Real    d;

  if (procf && procl)
  {
    j = slin.Length() + 1;
  }
  else
  {
    j = 1;
  }

  //-- On prend les lignes deja enregistrees

  while (j <= slin.Length())
  {
    if (slin.Value(j)->ArcType() == IntPatch_Analytic)
    {
      const Handle(IntPatch_ALine)& aligold = *((Handle(IntPatch_ALine)*)&slin.Value(j));
      k                                     = 1;

      //-- On prend les vertex des lignes deja enregistrees

      while (k <= aligold->NbVertex())
      {
        ptsol = aligold->Vertex(k);
        if (!procf)
        {
          d = ptf.Distance(ptsol.Value());
          if (d <= Tol)
          {
            ptsol.SetTolerance(Tol);
            if (!ptsol.IsMultiple())
            {
              //-- le point ptsol (de aligold) est declare multiple sur aligold
              Multpoint = Standard_True;
              ptsol.SetMultiple(Standard_True);
              aligold->Replace(k, ptsol);
            }
            ptsol.SetParameter(first);
            alig->AddVertex(ptsol);
            alig->SetFirstPoint(alig->NbVertex());
            procf = Standard_True;

            //-- On restore le point avec son parametre sur aligold
            ptsol = aligold->Vertex(k);
          }
        }
        if (!procl)
        {
          if (ptl.Distance(ptsol.Value()) <= Tol)
          {
            ptsol.SetTolerance(Tol);
            if (!ptsol.IsMultiple())
            {
              Multpoint = Standard_True;
              ptsol.SetMultiple(Standard_True);
              aligold->Replace(k, ptsol);
            }
            ptsol.SetParameter(last);
            alig->AddVertex(ptsol);
            alig->SetLastPoint(alig->NbVertex());
            procl = Standard_True;

            //-- On restore le point avec son parametre sur aligold
            ptsol = aligold->Vertex(k);
          }
        }
        if (procf && procl)
        {
          k = aligold->NbVertex() + 1;
        }
        else
        {
          k = k + 1;
        }
      }
      if (procf && procl)
      {
        j = slin.Length() + 1;
      }
      else
      {
        j = j + 1;
      }
    }
  }

  ptsol.SetTolerance(Tol);
  if (!procf && !procl)
  {
    Quad1.Parameters(ptf, U1, V1);
    Quad2.Parameters(ptf, U2, V2);
    ptsol.SetValue(ptf, Tol, Standard_False);
    ptsol.SetParameters(U1, V1, U2, V2);
    ptsol.SetParameter(first);
    if (ptf.Distance(ptl) <= Tol)
    {
      ptsol.SetMultiple(Standard_True); // a voir
      Multpoint = Standard_True;        // a voir de meme
      alig->AddVertex(ptsol);
      alig->SetFirstPoint(alig->NbVertex());

      ptsol.SetParameter(last);
      alig->AddVertex(ptsol);
      alig->SetLastPoint(alig->NbVertex());
    }
    else
    {
      alig->AddVertex(ptsol);
      alig->SetFirstPoint(alig->NbVertex());
      Quad1.Parameters(ptl, U1, V1);
      Quad2.Parameters(ptl, U2, V2);
      ptsol.SetValue(ptl, Tol, Standard_False);
      ptsol.SetParameters(U1, V1, U2, V2);
      ptsol.SetParameter(last);
      alig->AddVertex(ptsol);
      alig->SetLastPoint(alig->NbVertex());
    }
  }
  else if (!procf)
  {
    Quad1.Parameters(ptf, U1, V1);
    Quad2.Parameters(ptf, U2, V2);
    ptsol.SetValue(ptf, Tol, Standard_False);
    ptsol.SetParameters(U1, V1, U2, V2);
    ptsol.SetParameter(first);
    alig->AddVertex(ptsol);
    alig->SetFirstPoint(alig->NbVertex());
  }
  else if (!procl)
  {
    Quad1.Parameters(ptl, U1, V1);
    Quad2.Parameters(ptl, U2, V2);
    ptsol.SetValue(ptl, Tol, Standard_False);
    ptsol.SetParameters(U1, V1, U2, V2);
    ptsol.SetParameter(last);
    alig->AddVertex(ptsol);
    alig->SetLastPoint(alig->NbVertex());
  }
}

//=======================================================================
// function : CyCyAnalyticalIntersect
// purpose  : Checks if intersection curve is analytical (line, circle, ellipse)
//            and returns these curves.
//=======================================================================
Standard_Boolean CyCyAnalyticalIntersect(const IntSurf_Quadric&    Quad1,
                                         const IntSurf_Quadric&    Quad2,
                                         const IntAna_QuadQuadGeo& theInter,
                                         const Standard_Real       Tol,
                                         Standard_Boolean&         Empty,
                                         Standard_Boolean&         Same,
                                         Standard_Boolean&         Multpoint,
                                         IntPatch_SequenceOfLine&  slin,
                                         IntPatch_SequenceOfPoint& spnt)
{
  IntPatch_Point ptsol;

  Standard_Integer i;

  IntSurf_TypeTrans trans1, trans2;
  IntAna_ResultType typint;

  gp_Elips elipsol;
  gp_Lin   linsol;

  gp_Cylinder Cy1(Quad1.Cylinder());
  gp_Cylinder Cy2(Quad2.Cylinder());

  typint                 = theInter.TypeInter();
  Standard_Integer NbSol = theInter.NbSolutions();
  Empty                  = Standard_False;
  Same                   = Standard_False;

  switch (typint)
  {
    case IntAna_Empty: {
      Empty = Standard_True;
    }
    break;

    case IntAna_Same: {
      Same = Standard_True;
    }
    break;

    case IntAna_Point: {
      gp_Pnt psol(theInter.Point(1));
      ptsol.SetValue(psol, Tol, Standard_True);

      Standard_Real U1, V1, U2, V2;
      Quad1.Parameters(psol, U1, V1);
      Quad2.Parameters(psol, U2, V2);

      ptsol.SetParameters(U1, V1, U2, V2);
      spnt.Append(ptsol);
    }
    break;

    case IntAna_Line: {
      gp_Pnt ptref;
      if (NbSol == 1)
      { // Cylinders are tangent to each other by line
        linsol = theInter.Line(1);
        ptref  = linsol.Location();

        // Radius-vectors
        gp_Dir crb1(gp_Vec(ptref, Cy1.Location()));
        gp_Dir crb2(gp_Vec(ptref, Cy2.Location()));

        // outer normal lines
        gp_Vec            norm1(Quad1.Normale(ptref));
        gp_Vec            norm2(Quad2.Normale(ptref));
        IntSurf_Situation situcyl1;
        IntSurf_Situation situcyl2;

        if (crb1.Dot(crb2) < 0.)
        { // centre de courbures "opposes"
          // ATTENTION!!!
          //         Normal and Radius-vector of the 1st(!) cylinder
          //         is used for judging what the situation of the 2nd(!)
          //         cylinder is.

          if (norm1.Dot(crb1) > 0.)
          {
            situcyl2 = IntSurf_Inside;
          }
          else
          {
            situcyl2 = IntSurf_Outside;
          }

          if (norm2.Dot(crb2) > 0.)
          {
            situcyl1 = IntSurf_Inside;
          }
          else
          {
            situcyl1 = IntSurf_Outside;
          }
        }
        else
        {
          if (Cy1.Radius() < Cy2.Radius())
          {
            if (norm1.Dot(crb1) > 0.)
            {
              situcyl2 = IntSurf_Inside;
            }
            else
            {
              situcyl2 = IntSurf_Outside;
            }

            if (norm2.Dot(crb2) > 0.)
            {
              situcyl1 = IntSurf_Outside;
            }
            else
            {
              situcyl1 = IntSurf_Inside;
            }
          }
          else
          {
            if (norm1.Dot(crb1) > 0.)
            {
              situcyl2 = IntSurf_Outside;
            }
            else
            {
              situcyl2 = IntSurf_Inside;
            }

            if (norm2.Dot(crb2) > 0.)
            {
              situcyl1 = IntSurf_Inside;
            }
            else
            {
              situcyl1 = IntSurf_Outside;
            }
          }
        }

        Handle(IntPatch_GLine) glig = new IntPatch_GLine(linsol, Standard_True, situcyl1, situcyl2);
        slin.Append(glig);
      }
      else
      {
        for (i = 1; i <= NbSol; i++)
        {
          linsol     = theInter.Line(i);
          ptref      = linsol.Location();
          gp_Vec lsd = linsol.Direction();

          // Theoretically, qwe = +/- 1.0.
          Standard_Real qwe = lsd.DotCross(Quad2.Normale(ptref), Quad1.Normale(ptref));
          if (qwe > 0.00000001)
          {
            trans1 = IntSurf_Out;
            trans2 = IntSurf_In;
          }
          else if (qwe < -0.00000001)
          {
            trans1 = IntSurf_In;
            trans2 = IntSurf_Out;
          }
          else
          {
            trans1 = trans2 = IntSurf_Undecided;
          }

          Handle(IntPatch_GLine) glig = new IntPatch_GLine(linsol, Standard_False, trans1, trans2);
          slin.Append(glig);
        }
      }
    }
    break;

    case IntAna_Ellipse: {
      gp_Vec         Tgt;
      gp_Pnt         ptref;
      IntPatch_Point pmult1, pmult2;

      elipsol = theInter.Ellipse(1);

      gp_Pnt pttang1(ElCLib::Value(0.5 * M_PI, elipsol));
      gp_Pnt pttang2(ElCLib::Value(1.5 * M_PI, elipsol));

      Multpoint = Standard_True;
      pmult1.SetValue(pttang1, Tol, Standard_True);
      pmult2.SetValue(pttang2, Tol, Standard_True);
      pmult1.SetMultiple(Standard_True);
      pmult2.SetMultiple(Standard_True);

      Standard_Real oU1, oV1, oU2, oV2;
      Quad1.Parameters(pttang1, oU1, oV1);
      Quad2.Parameters(pttang1, oU2, oV2);

      pmult1.SetParameters(oU1, oV1, oU2, oV2);
      Quad1.Parameters(pttang2, oU1, oV1);
      Quad2.Parameters(pttang2, oU2, oV2);

      pmult2.SetParameters(oU1, oV1, oU2, oV2);

      // on traite la premiere ellipse

      //-- Calcul de la Transition de la ligne
      ElCLib::D1(0., elipsol, ptref, Tgt);

      // Theoretically, qwe = +/- |Tgt|.
      Standard_Real qwe = Tgt.DotCross(Quad2.Normale(ptref), Quad1.Normale(ptref));
      if (qwe > 0.00000001)
      {
        trans1 = IntSurf_Out;
        trans2 = IntSurf_In;
      }
      else if (qwe < -0.00000001)
      {
        trans1 = IntSurf_In;
        trans2 = IntSurf_Out;
      }
      else
      {
        trans1 = trans2 = IntSurf_Undecided;
      }

      //-- Transition calculee au point 0 -> Trans2 , Trans1
      //-- car ici, on devarit calculer en PI
      Handle(IntPatch_GLine) glig = new IntPatch_GLine(elipsol, Standard_False, trans2, trans1);
      //
      {
        Standard_Real  aU1, aV1, aU2, aV2;
        IntPatch_Point aIP;
        gp_Pnt         aP(ElCLib::Value(0., elipsol));
        //
        aIP.SetValue(aP, Tol, Standard_False);
        aIP.SetMultiple(Standard_False);
        //
        Quad1.Parameters(aP, aU1, aV1);
        Quad2.Parameters(aP, aU2, aV2);

        aIP.SetParameters(aU1, aV1, aU2, aV2);
        //
        aIP.SetParameter(0.);
        glig->AddVertex(aIP);
        glig->SetFirstPoint(1);
        //
        aIP.SetParameter(2. * M_PI);
        glig->AddVertex(aIP);
        glig->SetLastPoint(2);
      }
      //
      pmult1.SetParameter(0.5 * M_PI);
      glig->AddVertex(pmult1);
      //
      pmult2.SetParameter(1.5 * M_PI);
      glig->AddVertex(pmult2);

      //
      slin.Append(glig);

      //-- Transitions calculee au point 0    OK
      //
      // on traite la deuxieme ellipse
      elipsol = theInter.Ellipse(2);

      Standard_Real param1              = ElCLib::Parameter(elipsol, pttang1);
      Standard_Real param2              = ElCLib::Parameter(elipsol, pttang2);
      Standard_Real parampourtransition = 0.0;
      if (param1 < param2)
      {
        pmult1.SetParameter(0.5 * M_PI);
        pmult2.SetParameter(1.5 * M_PI);
        parampourtransition = M_PI;
      }
      else
      {
        pmult1.SetParameter(1.5 * M_PI);
        pmult2.SetParameter(0.5 * M_PI);
        parampourtransition = 0.0;
      }

      //-- Calcul des transitions de ligne pour la premiere ligne
      ElCLib::D1(parampourtransition, elipsol, ptref, Tgt);

      // Theoretically, qwe = +/- |Tgt|.
      qwe = Tgt.DotCross(Quad2.Normale(ptref), Quad1.Normale(ptref));
      if (qwe > 0.00000001)
      {
        trans1 = IntSurf_Out;
        trans2 = IntSurf_In;
      }
      else if (qwe < -0.00000001)
      {
        trans1 = IntSurf_In;
        trans2 = IntSurf_Out;
      }
      else
      {
        trans1 = trans2 = IntSurf_Undecided;
      }

      //-- La transition a ete calculee sur un point de cette ligne
      glig = new IntPatch_GLine(elipsol, Standard_False, trans1, trans2);
      //
      {
        Standard_Real  aU1, aV1, aU2, aV2;
        IntPatch_Point aIP;
        gp_Pnt         aP(ElCLib::Value(0., elipsol));
        //
        aIP.SetValue(aP, Tol, Standard_False);
        aIP.SetMultiple(Standard_False);
        //

        Quad1.Parameters(aP, aU1, aV1);
        Quad2.Parameters(aP, aU2, aV2);

        aIP.SetParameters(aU1, aV1, aU2, aV2);
        //
        aIP.SetParameter(0.);
        glig->AddVertex(aIP);
        glig->SetFirstPoint(1);
        //
        aIP.SetParameter(2. * M_PI);
        glig->AddVertex(aIP);
        glig->SetLastPoint(2);
      }
      //
      glig->AddVertex(pmult1);
      glig->AddVertex(pmult2);
      //
      slin.Append(glig);
    }
    break;

    case IntAna_Parabola:
    case IntAna_Hyperbola:
      throw Standard_Failure("IntCyCy(): Wrong intersection type!");

    case IntAna_Circle:
      // Circle is useful when we will work with trimmed surfaces
      // (two cylinders can be tangent by their basises, e.g. circle)
    case IntAna_NoGeometricSolution:
    default:
      return Standard_False;
  }

  return Standard_True;
}

//=======================================================================
// function : ShortCosForm
// purpose  : Represents theCosFactor*cosA+theSinFactor*sinA as
//            theCoeff*cos(A-theAngle) if it is possibly (all angles are
//            in radians).
//=======================================================================
static void ShortCosForm(const Standard_Real theCosFactor,
                         const Standard_Real theSinFactor,
                         Standard_Real&      theCoeff,
                         Standard_Real&      theAngle)
{
  theCoeff = sqrt(theCosFactor * theCosFactor + theSinFactor * theSinFactor);
  theAngle = 0.0;
  if (IsEqual(theCoeff, 0.0))
  {
    theAngle = 0.0;
    return;
  }

  theAngle = acos(Abs(theCosFactor / theCoeff));

  if (theSinFactor > 0.0)
  {
    if (IsEqual(theCosFactor, 0.0))
    {
      theAngle = M_PI / 2.0;
    }
    else if (theCosFactor < 0.0)
    {
      theAngle = M_PI - theAngle;
    }
  }
  else if (IsEqual(theSinFactor, 0.0))
  {
    if (theCosFactor < 0.0)
    {
      theAngle = M_PI;
    }
  }
  if (theSinFactor < 0.0)
  {
    if (theCosFactor > 0.0)
    {
      theAngle = 2.0 * M_PI - theAngle;
    }
    else if (IsEqual(theCosFactor, 0.0))
    {
      theAngle = 3.0 * M_PI / 2.0;
    }
    else if (theCosFactor < 0.0)
    {
      theAngle = M_PI + theAngle;
    }
  }
}

//=======================================================================
// function : CylCylMonotonicity
// purpose  : Determines, if U2(U1) function is increasing.
//=======================================================================
Standard_Boolean ComputationMethods::CylCylMonotonicity(const Standard_Real    theU1par,
                                                        const Standard_Integer theWLIndex,
                                                        const stCoeffsValue&   theCoeffs,
                                                        const Standard_Real    thePeriod,
                                                        Standard_Boolean&      theIsIncreasing)
{
  // U2(U1) = FI2 + (+/-)acos(B*cos(U1 - FI1) + C);
  // Accordingly,
  // Func. U2(X1) = FI2 + X1;
  // Func. X1(X2) = anArccosFactor*X2;
  // Func. X2(X3) = acos(X3);
  // Func. X3(X4) = B*X4 + C;
  // Func. X4(U1) = cos(U1 - FI1).
  //
  // Consequently,
  // U2(X1) is always increasing.
  // X1(X2) is increasing if anArccosFactor > 0.0 and
  // is decreasing otherwise.
  // X2(X3) is always decreasing.
  // Therefore, U2(X3) is decreasing if anArccosFactor > 0.0 and
  // is increasing otherwise.
  // X3(X4) is increasing if B > 0 and is decreasing otherwise.
  // X4(U1) is increasing if U1 - FI1 in [PI, 2*PI) and
  // is decreasing U1 - FI1 in [0, PI) or if (U1 - FI1 == 2PI).
  // After that, we can predict behaviour of U2(U1) function:
  // if it is increasing or decreasing.

  // For "+/-" sign. If isPlus == TRUE, "+" is chosen, otherwise, we choose "-".
  Standard_Boolean isPlus = Standard_False;

  switch (theWLIndex)
  {
    case 0:
      isPlus = Standard_True;
      break;
    case 1:
      isPlus = Standard_False;
      break;
    default:
      // throw Standard_Failure("Error. Range Error!!!!");
      return Standard_False;
  }

  Standard_Real aU1Temp = theU1par - theCoeffs.mFI1;
  InscribePoint(0, thePeriod, aU1Temp, 0.0, thePeriod, Standard_False);

  theIsIncreasing = Standard_True;

  if (((M_PI - aU1Temp) < RealSmall()) && (aU1Temp < thePeriod))
  {
    theIsIncreasing = Standard_False;
  }

  if (theCoeffs.mB < 0.0)
  {
    theIsIncreasing = !theIsIncreasing;
  }

  if (!isPlus)
  {
    theIsIncreasing = !theIsIncreasing;
  }

  return Standard_True;
}

//=======================================================================
// function : CylCylComputeParameters
// purpose  : Computes U2 (U-parameter of the 2nd cylinder) and, if theDelta != 0,
//            estimates the tolerance of U2-computing (estimation result is
//            assigned to *theDelta value).
//=======================================================================
Standard_Boolean ComputationMethods::CylCylComputeParameters(const Standard_Real    theU1par,
                                                             const Standard_Integer theWLIndex,
                                                             const stCoeffsValue&   theCoeffs,
                                                             Standard_Real&         theU2,
                                                             Standard_Real* const   theDelta)
{
  // This formula is got from some experience and can be changed.
  const Standard_Real aTol0 = Min(10.0 * Epsilon(1.0) * theCoeffs.mB, aNulValue);
  const Standard_Real aTol  = 1.0 - aTol0;

  if (theWLIndex < 0 || theWLIndex > 1)
    return Standard_False;

  const Standard_Real aSign = theWLIndex ? -1.0 : 1.0;

  Standard_Real anArg = cos(theU1par - theCoeffs.mFI1);
  anArg               = theCoeffs.mB * anArg + theCoeffs.mC;

  if (anArg >= aTol)
  {
    if (theDelta)
      *theDelta = 0.0;

    anArg = 1.0;
  }
  else if (anArg <= -aTol)
  {
    if (theDelta)
      *theDelta = 0.0;

    anArg = -1.0;
  }
  else if (theDelta)
  {
    // There is a case, when
    //   const double aPar = 0.99999999999721167;
    //   const double aFI2 = 2.3593296083566181e-006;

    // Then
    //   aPar - cos(aFI2) == -5.10703e-015 ==> cos(aFI2) == aPar.
    // Theoretically, in this case
    //   aFI2 == +/- acos(aPar).
    // However,
    //   acos(aPar) - aFI2 == 2.16362e-009.
    // Error is quite big.

    // This error should be estimated. Let use following way, which is based
    // on expanding to Taylor series.

    //  acos(p)-acos(p+x) = x/sqrt(1-p*p).

    // If p == (1-d) (when p > 0) or p == (-1+d) (when p < 0) then
    //   acos(p)-acos(p+x) = x/sqrt(d*(2-d)).

    // Here always aTol0 <= d <= 1. Max(x) is considered (!) to be equal to aTol0.
    // In this case
    //   8*aTol0 <= acos(p)-acos(p+x) <= sqrt(2/(2-aTol0)-1),
    //                                               because 0 < aTol0 < 1.
    // E.g. when aTol0 = 1.0e-11,
    //    8.0e-11 <= acos(p)-acos(p+x) < 2.24e-6.

    const Standard_Real aDelta = Min(1.0 - anArg, 1.0 + anArg);
    Standard_DivideByZero_Raise_if((aDelta * aDelta < RealSmall()) || (aDelta >= 2.0),
                                   "IntPatch_ImpImpIntersection_4.gxx, CylCylComputeParameters()");
    *theDelta = aTol0 / sqrt(aDelta * (2.0 - aDelta));
  }

  theU2 = acos(anArg);
  theU2 = theCoeffs.mFI2 + aSign * theU2;

  return Standard_True;
}

//=======================================================================
// function : CylCylComputeParameters
// purpose  : Computes V1 and V2 (V-parameters of the 1st and 2nd cylinder respectively).
//=======================================================================
Standard_Boolean ComputationMethods::CylCylComputeParameters(const Standard_Real  theU1,
                                                             const Standard_Real  theU2,
                                                             const stCoeffsValue& theCoeffs,
                                                             Standard_Real&       theV1,
                                                             Standard_Real&       theV2)
{
  theV1 = theCoeffs.mK21 * sin(theU2) + theCoeffs.mK11 * sin(theU1) + theCoeffs.mL21 * cos(theU2)
          + theCoeffs.mL11 * cos(theU1) + theCoeffs.mM1;

  theV2 = theCoeffs.mK22 * sin(theU2) + theCoeffs.mK12 * sin(theU1) + theCoeffs.mL22 * cos(theU2)
          + theCoeffs.mL12 * cos(theU1) + theCoeffs.mM2;

  return Standard_True;
}

//=======================================================================
// function : CylCylComputeParameters
// purpose  : Computes U2 (U-parameter of the 2nd cylinder),
//            V1 and V2 (V-parameters of the 1st and 2nd cylinder respectively).
//=======================================================================
Standard_Boolean ComputationMethods::CylCylComputeParameters(const Standard_Real    theU1par,
                                                             const Standard_Integer theWLIndex,
                                                             const stCoeffsValue&   theCoeffs,
                                                             Standard_Real&         theU2,
                                                             Standard_Real&         theV1,
                                                             Standard_Real&         theV2)
{
  if (!CylCylComputeParameters(theU1par, theWLIndex, theCoeffs, theU2))
    return Standard_False;

  if (!CylCylComputeParameters(theU1par, theU2, theCoeffs, theV1, theV2))
    return Standard_False;

  return Standard_True;
}

//=================================================================================================

Standard_Boolean WorkWithBoundaries::SearchOnVBounds(const SearchBoundType theSBType,
                                                     const Standard_Real   theVzad,
                                                     const Standard_Real   theVInit,
                                                     const Standard_Real   theInitU2,
                                                     const Standard_Real   theInitMainVar,
                                                     Standard_Real& theMainVariableValue) const
{
  const Standard_Integer aNbDim    = 3;
  const Standard_Real    aMaxError = 4.0 * M_PI; // two periods

  theMainVariableValue             = theInitMainVar;
  const Standard_Real aTol2        = 1.0e-18;
  Standard_Real       aMainVarPrev = theInitMainVar, aU2Prev = theInitU2, anOtherVar = theVInit;

  // Structure of aMatr:
  //   C_{1}*U_{1} & C_{2}*U_{2} & C_{3}*V_{*},
  // where C_{1}, C_{2} and C_{3} are math_Vector.
  math_Matrix aMatr(1, aNbDim, 1, aNbDim);

  Standard_Real    anError     = RealLast();
  Standard_Real    anErrorPrev = anError;
  Standard_Integer aNbIter     = 0;
  do
  {
    if (++aNbIter > 1000)
      return Standard_False;

    const Standard_Real aSinU1 = sin(aMainVarPrev), aCosU1 = cos(aMainVarPrev),
                        aSinU2 = sin(aU2Prev), aCosU2 = cos(aU2Prev);

    math_Vector aVecFreeMem = (myCoeffs.mVecA2 * aU2Prev + myCoeffs.mVecB2) * aSinU2
                              - (myCoeffs.mVecB2 * aU2Prev - myCoeffs.mVecA2) * aCosU2
                              + (myCoeffs.mVecA1 * aMainVarPrev + myCoeffs.mVecB1) * aSinU1
                              - (myCoeffs.mVecB1 * aMainVarPrev - myCoeffs.mVecA1) * aCosU1
                              + myCoeffs.mVecD;

    math_Vector aMSum(1, 3);

    switch (theSBType)
    {
      case SearchV1:
        aMatr.SetCol(3, myCoeffs.mVecC2);
        aMSum = myCoeffs.mVecC1 * theVzad;
        aVecFreeMem -= aMSum;
        aMSum += myCoeffs.mVecC2 * anOtherVar;
        break;

      case SearchV2:
        aMatr.SetCol(3, myCoeffs.mVecC1);
        aMSum = myCoeffs.mVecC2 * theVzad;
        aVecFreeMem -= aMSum;
        aMSum += myCoeffs.mVecC1 * anOtherVar;
        break;

      default:
        return Standard_False;
    }

    aMatr.SetCol(1, myCoeffs.mVecA1 * aSinU1 - myCoeffs.mVecB1 * aCosU1);
    aMatr.SetCol(2, myCoeffs.mVecA2 * aSinU2 - myCoeffs.mVecB2 * aCosU2);

    Standard_Real aDetMainSyst = aMatr.Determinant();

    if (Abs(aDetMainSyst) < aNulValue)
    {
      return Standard_False;
    }

    math_Matrix aM1(aMatr), aM2(aMatr), aM3(aMatr);
    aM1.SetCol(1, aVecFreeMem);
    aM2.SetCol(2, aVecFreeMem);
    aM3.SetCol(3, aVecFreeMem);

    const Standard_Real aDetMainVar = aM1.Determinant();
    const Standard_Real aDetVar1    = aM2.Determinant();
    const Standard_Real aDetVar2    = aM3.Determinant();

    Standard_Real aDelta = aDetMainVar / aDetMainSyst - aMainVarPrev;

    if (Abs(aDelta) > aMaxError)
      return Standard_False;

    anError = aDelta * aDelta;
    aMainVarPrev += aDelta;

    ///
    aDelta = aDetVar1 / aDetMainSyst - aU2Prev;

    if (Abs(aDelta) > aMaxError)
      return Standard_False;

    anError += aDelta * aDelta;
    aU2Prev += aDelta;

    ///
    aDelta = aDetVar2 / aDetMainSyst - anOtherVar;
    anError += aDelta * aDelta;
    anOtherVar += aDelta;

    if (anError > anErrorPrev)
    { // Method diverges. Keep the best result
      const Standard_Real aSinU1Last = sin(aMainVarPrev), aCosU1Last = cos(aMainVarPrev),
                          aSinU2Last = sin(aU2Prev), aCosU2Last = cos(aU2Prev);
      aMSum -= (myCoeffs.mVecA1 * aCosU1Last + myCoeffs.mVecB1 * aSinU1Last
                + myCoeffs.mVecA2 * aCosU2Last + myCoeffs.mVecB2 * aSinU2Last + myCoeffs.mVecD);
      const Standard_Real aSQNorm = aMSum.Norm2();
      return (aSQNorm < aTol2);
    }
    else
    {
      theMainVariableValue = aMainVarPrev;
    }

    anErrorPrev = anError;
  } while (anError > aTol2);

  theMainVariableValue = aMainVarPrev;

  return Standard_True;
}

//=======================================================================
// function : InscribePoint
// purpose  : If theFlForce==TRUE theUGiven will be changed forcefully
//            even if theUGiven is already inscibed in the boundary
//            (if it is possible; i.e. if new theUGiven is inscribed
//            in the boundary, too).
//=======================================================================
Standard_Boolean InscribePoint(const Standard_Real    theUfTarget,
                               const Standard_Real    theUlTarget,
                               Standard_Real&         theUGiven,
                               const Standard_Real    theTol2D,
                               const Standard_Real    thePeriod,
                               const Standard_Boolean theFlForce)
{
  if (Precision::IsInfinite(theUGiven))
  {
    return Standard_False;
  }

  if ((theUfTarget - theUGiven <= theTol2D) && (theUGiven - theUlTarget <= theTol2D))
  { // it has already inscribed

    /*
             Utf    U      Utl
              +     *       +
    */

    if (theFlForce)
    {
      Standard_Real anUtemp = theUGiven + thePeriod;
      if ((theUfTarget - anUtemp <= theTol2D) && (anUtemp - theUlTarget <= theTol2D))
      {
        theUGiven = anUtemp;
        return Standard_True;
      }

      anUtemp = theUGiven - thePeriod;
      if ((theUfTarget - anUtemp <= theTol2D) && (anUtemp - theUlTarget <= theTol2D))
      {
        theUGiven = anUtemp;
      }
    }

    return Standard_True;
  }

  const Standard_Real aUf = theUfTarget - theTol2D;
  const Standard_Real aUl = aUf + thePeriod;

  theUGiven = ElCLib::InPeriod(theUGiven, aUf, aUl);

  return ((theUfTarget - theUGiven <= theTol2D) && (theUGiven - theUlTarget <= theTol2D));
}

//=======================================================================
// function : InscribeInterval
// purpose  : Shifts theRange in order to make at least one of its
//            boundary in the range [theUfTarget, theUlTarget]
//=======================================================================
static Standard_Boolean InscribeInterval(const Standard_Real theUfTarget,
                                         const Standard_Real theUlTarget,
                                         Bnd_Range&          theRange,
                                         const Standard_Real theTol2D,
                                         const Standard_Real thePeriod)
{
  Standard_Real anUpar = 0.0;
  if (!theRange.GetMin(anUpar))
  {
    return Standard_False;
  }

  const Standard_Real aDelta = theRange.Delta();
  if (InscribePoint(theUfTarget,
                    theUlTarget,
                    anUpar,
                    theTol2D,
                    thePeriod,
                    (Abs(theUlTarget - anUpar) < theTol2D)))
  {
    theRange.SetVoid();
    theRange.Add(anUpar);
    theRange.Add(anUpar + aDelta);
  }
  else
  {
    if (!theRange.GetMax(anUpar))
    {
      return Standard_False;
    }

    if (InscribePoint(theUfTarget,
                      theUlTarget,
                      anUpar,
                      theTol2D,
                      thePeriod,
                      (Abs(theUfTarget - anUpar) < theTol2D)))
    {
      theRange.SetVoid();
      theRange.Add(anUpar);
      theRange.Add(anUpar - aDelta);
    }
    else
    {
      return Standard_False;
    }
  }

  return Standard_True;
}

//=======================================================================
// function : ExcludeNearElements
// purpose  : Checks if theArr contains two almost equal elements.
//            If it is true then one of equal elements will be excluded
//            (made infinite).
//           Returns TRUE if at least one element of theArr has been changed.
// ATTENTION!!!
//           1. Every not infinite element of theArr is considered to be
//            in [0, T] interval (where T is the period);
//           2. theArr must be sorted in ascending order.
//=======================================================================
static Standard_Boolean ExcludeNearElements(Standard_Real          theArr[],
                                            const Standard_Integer theNOfMembers,
                                            const Standard_Real    theUSurf1f,
                                            const Standard_Real    theUSurf1l,
                                            const Standard_Real    theTol)
{
  Standard_Boolean aRetVal = Standard_False;
  for (Standard_Integer i = 1; i < theNOfMembers; i++)
  {
    Standard_Real &anA = theArr[i], &anB = theArr[i - 1];

    // Here, anA >= anB

    if (Precision::IsInfinite(anA))
      break;

    if ((anA - anB) < theTol)
    {
      if ((anB != 0.0) && (anB != theUSurf1f) && (anB != theUSurf1l))
        anA = (anA + anB) / 2.0;
      else
        anA = anB;

      // Make this element infinite an forget it
      //(we will not use it in next iterations).
      anB     = Precision::Infinite();
      aRetVal = Standard_True;
    }
  }

  return aRetVal;
}

//=======================================================================
// function : AddPointIntoWL
// purpose  : Surf1 is a surface, whose U-par is variable.
//           If theFlBefore == TRUE then we enable the U1-parameter
//            of the added point to be less than U1-parameter of
//           previously added point (in general U1-parameter is
//           always increased; therefore, this situation is abnormal).
//           If theOnlyCheck==TRUE then no point will be added to theLine.
//=======================================================================
static Standard_Boolean AddPointIntoWL(const IntSurf_Quadric&                   theQuad1,
                                       const IntSurf_Quadric&                   theQuad2,
                                       const ComputationMethods::stCoeffsValue& theCoeffs,
                                       const Standard_Boolean                   isTheReverse,
                                       const Standard_Boolean                   isThePrecise,
                                       const gp_Pnt2d&                          thePntOnSurf1,
                                       const gp_Pnt2d&                          thePntOnSurf2,
                                       const Standard_Real                      theUfSurf1,
                                       const Standard_Real                      theUlSurf1,
                                       const Standard_Real                      theUfSurf2,
                                       const Standard_Real                      theUlSurf2,
                                       const Standard_Real                      theVfSurf1,
                                       const Standard_Real                      theVlSurf1,
                                       const Standard_Real                      theVfSurf2,
                                       const Standard_Real                      theVlSurf2,
                                       const Standard_Real                      thePeriodOfSurf1,
                                       const Handle(IntSurf_LineOn2S)&          theLine,
                                       const Standard_Integer                   theWLIndex,
                                       const Standard_Real                      theTol3D,
                                       const Standard_Real                      theTol2D,
                                       const Standard_Boolean theFlBefore  = Standard_False,
                                       const Standard_Boolean theOnlyCheck = Standard_False)
{
  // Check if the point is in the domain or can be inscribed in the domain after adjusting.

  gp_Pnt aPt1(theQuad1.Value(thePntOnSurf1.X(), thePntOnSurf1.Y())),
    aPt2(theQuad2.Value(thePntOnSurf2.X(), thePntOnSurf2.Y()));

  Standard_Real aU1par = thePntOnSurf1.X();

  // aU1par always increases. Therefore, we must reduce its
  // value in order to continue creation of WLine.
  if (!InscribePoint(theUfSurf1,
                     theUlSurf1,
                     aU1par,
                     theTol2D,
                     thePeriodOfSurf1,
                     aU1par > 0.5 * (theUfSurf1 + theUlSurf1)))
    return Standard_False;

  if ((theLine->NbPoints() > 0) && ((theUlSurf1 - theUfSurf1) >= (thePeriodOfSurf1 - theTol2D))
      && (((aU1par + thePeriodOfSurf1 - theUlSurf1) <= theTol2D)
          || ((aU1par - thePeriodOfSurf1 - theUfSurf1) >= theTol2D)))
  {
    // aU1par can be adjusted to both theUlSurf1 and theUfSurf1
    // with equal possibilities. This code fragment allows choosing
    // correct parameter from these two variants.

    Standard_Real aU1 = 0.0, aV1 = 0.0;
    if (isTheReverse)
    {
      theLine->Value(theLine->NbPoints()).ParametersOnS2(aU1, aV1);
    }
    else
    {
      theLine->Value(theLine->NbPoints()).ParametersOnS1(aU1, aV1);
    }

    const Standard_Real aDelta = aU1 - aU1par;
    if (2.0 * Abs(aDelta) > thePeriodOfSurf1)
    {
      aU1par += Sign(thePeriodOfSurf1, aDelta);
    }
  }

  Standard_Real aU2par = thePntOnSurf2.X();
  if (!InscribePoint(theUfSurf2, theUlSurf2, aU2par, theTol2D, thePeriodOfSurf1, Standard_False))
    return Standard_False;

  Standard_Real aV1par = thePntOnSurf1.Y();
  if ((aV1par - theVlSurf1 > theTol2D) || (theVfSurf1 - aV1par > theTol2D))
    return Standard_False;

  Standard_Real aV2par = thePntOnSurf2.Y();
  if ((aV2par - theVlSurf2 > theTol2D) || (theVfSurf2 - aV2par > theTol2D))
    return Standard_False;

  // Get intersection point and add it in the WL
  IntSurf_PntOn2S aPnt;

  if (isTheReverse)
  {
    aPnt.SetValue((aPt1.XYZ() + aPt2.XYZ()) / 2.0, aU2par, aV2par, aU1par, aV1par);
  }
  else
  {
    aPnt.SetValue((aPt1.XYZ() + aPt2.XYZ()) / 2.0, aU1par, aV1par, aU2par, aV2par);
  }

  Standard_Integer aNbPnts = theLine->NbPoints();
  if (aNbPnts > 0)
  {
    Standard_Real         aUl = 0.0, aVl = 0.0;
    const IntSurf_PntOn2S aPlast = theLine->Value(aNbPnts);
    if (isTheReverse)
      aPlast.ParametersOnS2(aUl, aVl);
    else
      aPlast.ParametersOnS1(aUl, aVl);

    if (!theFlBefore && (aU1par <= aUl))
    {
      // Parameter value must be increased if theFlBefore == FALSE.

      aU1par += thePeriodOfSurf1;

      // The condition is as same as in
      // InscribePoint(...) function
      if ((theUfSurf1 - aU1par > theTol2D) || (aU1par - theUlSurf1 > theTol2D))
      {
        // New aU1par is out of target interval.
        // Go back to old value.

        return Standard_False;
      }
    }

    if (theOnlyCheck)
      return Standard_True;

    // theTol2D is minimal step along parameter changed.
    // Therefore, if we apply this minimal step two
    // neighbour points will be always "same". Consequently,
    // we should reduce tolerance for IsSame checking.
    const Standard_Real aDTol = 1.0 - Epsilon(1.0);
    if (aPnt.IsSame(aPlast, theTol3D * aDTol, theTol2D * aDTol))
    {
      theLine->RemovePoint(aNbPnts);
    }
  }

  if (theOnlyCheck)
    return Standard_True;

  theLine->Add(aPnt);

  if (!isThePrecise)
    return Standard_True;

  // Try to precise existing WLine
  aNbPnts = theLine->NbPoints();
  if (aNbPnts >= 3)
  {
    Standard_Real aU1 = 0.0, aU2 = 0.0, aU3 = 0.0, aV = 0.0;
    if (isTheReverse)
    {
      theLine->Value(aNbPnts).ParametersOnS2(aU3, aV);
      theLine->Value(aNbPnts - 1).ParametersOnS2(aU2, aV);
      theLine->Value(aNbPnts - 2).ParametersOnS2(aU1, aV);
    }
    else
    {
      theLine->Value(aNbPnts).ParametersOnS1(aU3, aV);
      theLine->Value(aNbPnts - 1).ParametersOnS1(aU2, aV);
      theLine->Value(aNbPnts - 2).ParametersOnS1(aU1, aV);
    }

    const Standard_Real aStepPrev = aU2 - aU1;
    const Standard_Real aStep     = aU3 - aU2;

    const Standard_Integer aDeltaStep = RealToInt(aStepPrev / aStep);

    if ((1 < aDeltaStep) && (aDeltaStep < 2000))
    {
      // Add new points in case of non-uniform distribution of existing points
      SeekAdditionalPoints(theQuad1,
                           theQuad2,
                           theLine,
                           theCoeffs,
                           theWLIndex,
                           aDeltaStep,
                           aNbPnts - 2,
                           aNbPnts - 1,
                           theTol2D,
                           thePeriodOfSurf1,
                           isTheReverse);
    }
  }

  return Standard_True;
}

//=======================================================================
// function : AddBoundaryPoint
// purpose  : Find intersection point on V-boundary.
//=======================================================================
void WorkWithBoundaries::AddBoundaryPoint(const Handle(IntPatch_WLine)& theWL,
                                          const Standard_Real           theU1,
                                          const Standard_Real           theU1Min,
                                          const Standard_Real           theU2,
                                          const Standard_Real           theV1,
                                          const Standard_Real           theV1Prev,
                                          const Standard_Real           theV2,
                                          const Standard_Real           theV2Prev,
                                          const Standard_Integer        theWLIndex,
                                          const Standard_Boolean        theFlForce,
                                          Standard_Boolean&             isTheFound1,
                                          Standard_Boolean&             isTheFound2) const
{
  Standard_Real aUSurf1f = 0.0, // const
    aUSurf1l = 0.0, aVSurf1f = 0.0, aVSurf1l = 0.0;
  Standard_Real aUSurf2f = 0.0, // const
    aUSurf2l = 0.0, aVSurf2f = 0.0, aVSurf2l = 0.0;

  myUVSurf1.Get(aUSurf1f, aVSurf1f, aUSurf1l, aVSurf1l);
  myUVSurf2.Get(aUSurf2f, aVSurf2f, aUSurf2l, aVSurf2l);

  const Standard_Integer aSize            = 4;
  const Standard_Real    anArrVzad[aSize] = {aVSurf1f, aVSurf1l, aVSurf2f, aVSurf2l};

  StPInfo aUVPoint[aSize];

  for (Standard_Integer anIDSurf = 0; anIDSurf < 4; anIDSurf += 2)
  {
    const Standard_Real aVf = (anIDSurf == 0) ? theV1 : theV2,
                        aVl = (anIDSurf == 0) ? theV1Prev : theV2Prev;

    const SearchBoundType aTS = (anIDSurf == 0) ? SearchV1 : SearchV2;

    for (Standard_Integer anIDBound = 0; anIDBound < 2; anIDBound++)
    {
      const Standard_Integer anIndex = anIDSurf + anIDBound;

      aUVPoint[anIndex].mySurfID = anIDSurf;

      if ((Abs(aVf - anArrVzad[anIndex]) > myTol2D)
          && ((aVf - anArrVzad[anIndex]) * (aVl - anArrVzad[anIndex]) > 0.0))
      {
        continue;
      }

      // Segment [aVf, aVl] intersects at least one V-boundary (first or last)
      //  (in general, case is possible, when aVf > aVl).

      // Precise intersection point
      const Standard_Boolean aRes = SearchOnVBounds(aTS,
                                                    anArrVzad[anIndex],
                                                    (anIDSurf == 0) ? theV2 : theV1,
                                                    theU2,
                                                    theU1,
                                                    aUVPoint[anIndex].myU1);

      // aUVPoint[anIndex].myU1 is considered to be nearer to theU1 than
      // to theU1+/-Period
      if (!aRes || (aUVPoint[anIndex].myU1 >= theU1) || (aUVPoint[anIndex].myU1 < theU1Min))
      {
        // Intersection point is not found or out of the domain
        aUVPoint[anIndex].myU1 = RealLast();
        continue;
      }
      else
      {
        // intersection point is found

        Standard_Real &aU1 = aUVPoint[anIndex].myU1, &aU2 = aUVPoint[anIndex].myU2,
                      &aV1 = aUVPoint[anIndex].myV1, &aV2 = aUVPoint[anIndex].myV2;
        aU2 = theU2;
        aV1 = theV1;
        aV2 = theV2;

        if (!ComputationMethods::CylCylComputeParameters(aU1, theWLIndex, myCoeffs, aU2, aV1, aV2))
        {
          // Found point is wrong
          aU1 = RealLast();
          continue;
        }

        // Point on true V-boundary.
        if (aTS == SearchV1)
          aV1 = anArrVzad[anIndex];
        else // if(aTS[anIndex] == SearchV2)
          aV2 = anArrVzad[anIndex];
      }
    }
  }

  // Sort with acceding U1-parameter.
  std::sort(aUVPoint, aUVPoint + aSize);

  isTheFound1 = isTheFound2 = Standard_False;

  // Adding found points on boundary in the WLine.
  for (Standard_Integer i = 0; i < aSize; i++)
  {
    if (aUVPoint[i].myU1 == RealLast())
      break;

    if (!AddPointIntoWL(myQuad1,
                        myQuad2,
                        myCoeffs,
                        myIsReverse,
                        Standard_False,
                        gp_Pnt2d(aUVPoint[i].myU1, aUVPoint[i].myV1),
                        gp_Pnt2d(aUVPoint[i].myU2, aUVPoint[i].myV2),
                        aUSurf1f,
                        aUSurf1l,
                        aUSurf2f,
                        aUSurf2l,
                        aVSurf1f,
                        aVSurf1l,
                        aVSurf2f,
                        aVSurf2l,
                        myPeriod,
                        theWL->Curve(),
                        theWLIndex,
                        myTol3D,
                        myTol2D,
                        theFlForce))
    {
      continue;
    }

    if (aUVPoint[i].mySurfID == 0)
    {
      isTheFound1 = Standard_True;
    }
    else
    {
      isTheFound2 = Standard_True;
    }
  }
}

//=======================================================================
// function : SeekAdditionalPoints
// purpose  : Inserts additional intersection points between neighbor points.
//            This action is bone with several iterations. During every iteration,
//          new point is inserted in middle of every interval.
//            The process will be finished if NbPoints >= theMinNbPoints.
//=======================================================================
static void SeekAdditionalPoints(const IntSurf_Quadric&                   theQuad1,
                                 const IntSurf_Quadric&                   theQuad2,
                                 const Handle(IntSurf_LineOn2S)&          theLine,
                                 const ComputationMethods::stCoeffsValue& theCoeffs,
                                 const Standard_Integer                   theWLIndex,
                                 const Standard_Integer                   theMinNbPoints,
                                 const Standard_Integer                   theStartPointOnLine,
                                 const Standard_Integer                   theEndPointOnLine,
                                 const Standard_Real                      theTol2D,
                                 const Standard_Real                      thePeriodOfSurf2,
                                 const Standard_Boolean                   isTheReverse)
{
  if (theLine.IsNull())
    return;

  Standard_Integer aNbPoints = theEndPointOnLine - theStartPointOnLine + 1;

  Standard_Real aMinDeltaParam = theTol2D;

  {
    Standard_Real u1 = 0.0, v1 = 0.0, u2 = 0.0, v2 = 0.0;

    if (isTheReverse)
    {
      theLine->Value(theStartPointOnLine).ParametersOnS2(u1, v1);
      theLine->Value(theEndPointOnLine).ParametersOnS2(u2, v2);
    }
    else
    {
      theLine->Value(theStartPointOnLine).ParametersOnS1(u1, v1);
      theLine->Value(theEndPointOnLine).ParametersOnS1(u2, v2);
    }

    aMinDeltaParam = Max(Abs(u2 - u1) / IntToReal(theMinNbPoints), aMinDeltaParam);
  }

  Standard_Integer aLastPointIndex = theEndPointOnLine;
  Standard_Real    U1prec = 0.0, V1prec = 0.0, U2prec = 0.0, V2prec = 0.0;

  Standard_Integer aNbPointsPrev = 0;
  do
  {
    aNbPointsPrev = aNbPoints;
    for (Standard_Integer fp = theStartPointOnLine, lp = 0; fp < aLastPointIndex; fp = lp + 1)
    {
      Standard_Real U1f = 0.0, V1f = 0.0; // first point in 1st suraface
      Standard_Real U1l = 0.0, V1l = 0.0; // last  point in 1st suraface

      Standard_Real U2f = 0.0, V2f = 0.0; // first point in 2nd suraface
      Standard_Real U2l = 0.0, V2l = 0.0; // last  point in 2nd suraface

      lp = fp + 1;

      if (isTheReverse)
      {
        theLine->Value(fp).ParametersOnS2(U1f, V1f);
        theLine->Value(lp).ParametersOnS2(U1l, V1l);

        theLine->Value(fp).ParametersOnS1(U2f, V2f);
        theLine->Value(lp).ParametersOnS1(U2l, V2l);
      }
      else
      {
        theLine->Value(fp).ParametersOnS1(U1f, V1f);
        theLine->Value(lp).ParametersOnS1(U1l, V1l);

        theLine->Value(fp).ParametersOnS2(U2f, V2f);
        theLine->Value(lp).ParametersOnS2(U2l, V2l);
      }

      if (Abs(U1l - U1f) <= aMinDeltaParam)
      {
        // Step is minimal. It is not necessary to divide it.
        continue;
      }

      U1prec = 0.5 * (U1f + U1l);

      if (!ComputationMethods::CylCylComputeParameters(U1prec,
                                                       theWLIndex,
                                                       theCoeffs,
                                                       U2prec,
                                                       V1prec,
                                                       V2prec))
      {
        continue;
      }

      MinMax(U2f, U2l);
      if (!InscribePoint(U2f, U2l, U2prec, theTol2D, thePeriodOfSurf2, Standard_False))
      {
        continue;
      }

      const gp_Pnt aP1(theQuad1.Value(U1prec, V1prec)), aP2(theQuad2.Value(U2prec, V2prec));
      const gp_Pnt aPInt(0.5 * (aP1.XYZ() + aP2.XYZ()));

#ifdef INTPATCH_IMPIMPINTERSECTION_DEBUG
      std::cout << "|P1Pi| = " << aP1.SquareDistance(aPInt)
                << "; |P2Pi| = " << aP2.SquareDistance(aPInt) << std::endl;
#endif

      IntSurf_PntOn2S anIP;
      if (isTheReverse)
      {
        anIP.SetValue(aPInt, U2prec, V2prec, U1prec, V1prec);
      }
      else
      {
        anIP.SetValue(aPInt, U1prec, V1prec, U2prec, V2prec);
      }

      theLine->InsertBefore(lp, anIP);

      aNbPoints++;
      aLastPointIndex++;
    }

    if (aNbPoints >= theMinNbPoints)
    {
      return;
    }
  } while (aNbPoints < theMinNbPoints && (aNbPoints != aNbPointsPrev));
}

//=======================================================================
// function : BoundariesComputing
// purpose  : Computes true domain of future intersection curve (allows
//            avoiding excess iterations)
//=======================================================================
Standard_Boolean WorkWithBoundaries::BoundariesComputing(
  const ComputationMethods::stCoeffsValue& theCoeffs,
  const Standard_Real                      thePeriod,
  Bnd_Range                                theURange[])
{
  // All comments to this method is related to the comment
  // to ComputationMethods class

  // So, we have the equation
  //     cos(U2-FI2)=B*cos(U1-FI1)+C
  // Evidently,
  //     -1 <= B*cos(U1-FI1)+C <= 1

  if (theCoeffs.mB > 0.0)
  {
    // -(1+C)/B <= cos(U1-FI1) <= (1-C)/B

    if (theCoeffs.mB + Abs(theCoeffs.mC) < -1.0)
    {
      //(1-C)/B < -1 or -(1+C)/B > 1  ==> No solution

      return Standard_False;
    }
    else if (theCoeffs.mB + Abs(theCoeffs.mC) <= 1.0)
    {
      //(1-C)/B >= 1 and -(1+C)/B <= -1 ==> U=[0;2*PI]+aFI1
      theURange[0].Add(theCoeffs.mFI1);
      theURange[0].Add(thePeriod + theCoeffs.mFI1);
    }
    else if ((1 + theCoeffs.mC <= theCoeffs.mB) && (theCoeffs.mB <= 1 - theCoeffs.mC))
    {
      //(1-C)/B >= 1 and -(1+C)/B >= -1 ==>
      //(U=[0;aDAngle]+aFI1) || (U=[2*PI-aDAngle;2*PI]+aFI1),
      // where aDAngle = acos(-(myCoeffs.mC + 1) / myCoeffs.mB)

      Standard_Real anArg = -(theCoeffs.mC + 1) / theCoeffs.mB;
      if (anArg > 1.0)
        anArg = 1.0;
      if (anArg < -1.0)
        anArg = -1.0;

      const Standard_Real aDAngle = acos(anArg);
      theURange[0].Add(theCoeffs.mFI1);
      theURange[0].Add(aDAngle + theCoeffs.mFI1);
      theURange[1].Add(thePeriod - aDAngle + theCoeffs.mFI1);
      theURange[1].Add(thePeriod + theCoeffs.mFI1);
    }
    else if ((1 - theCoeffs.mC <= theCoeffs.mB) && (theCoeffs.mB <= 1 + theCoeffs.mC))
    {
      //(1-C)/B <= 1 and -(1+C)/B <= -1 ==> U=[aDAngle;2*PI-aDAngle]+aFI1
      // where aDAngle = acos((1 - myCoeffs.mC) / myCoeffs.mB)

      Standard_Real anArg = (1 - theCoeffs.mC) / theCoeffs.mB;
      if (anArg > 1.0)
        anArg = 1.0;
      if (anArg < -1.0)
        anArg = -1.0;

      const Standard_Real aDAngle = acos(anArg);
      theURange[0].Add(aDAngle + theCoeffs.mFI1);
      theURange[0].Add(thePeriod - aDAngle + theCoeffs.mFI1);
    }
    else if (theCoeffs.mB - Abs(theCoeffs.mC) >= 1.0)
    {
      //(1-C)/B <= 1 and -(1+C)/B >= -1 ==>
      //(U=[aDAngle1;aDAngle2]+aFI1) ||
      //(U=[2*PI-aDAngle2;2*PI-aDAngle1]+aFI1)
      // where aDAngle1 = acos((1 - myCoeffs.mC) / myCoeffs.mB),
      //      aDAngle2 = acos(-(myCoeffs.mC + 1) / myCoeffs.mB).

      Standard_Real anArg1 = (1 - theCoeffs.mC) / theCoeffs.mB,
                    anArg2 = -(theCoeffs.mC + 1) / theCoeffs.mB;
      if (anArg1 > 1.0)
        anArg1 = 1.0;
      if (anArg1 < -1.0)
        anArg1 = -1.0;

      if (anArg2 > 1.0)
        anArg2 = 1.0;
      if (anArg2 < -1.0)
        anArg2 = -1.0;

      const Standard_Real aDAngle1 = acos(anArg1), aDAngle2 = acos(anArg2);
      //(U=[aDAngle1;aDAngle2]+aFI1) ||
      //(U=[2*PI-aDAngle2;2*PI-aDAngle1]+aFI1)
      theURange[0].Add(aDAngle1 + theCoeffs.mFI1);
      theURange[0].Add(aDAngle2 + theCoeffs.mFI1);
      theURange[1].Add(thePeriod - aDAngle2 + theCoeffs.mFI1);
      theURange[1].Add(thePeriod - aDAngle1 + theCoeffs.mFI1);
    }
    else
    {
      return Standard_False;
    }
  }
  else if (theCoeffs.mB < 0.0)
  {
    // (1-C)/B <= cos(U1-FI1) <= -(1+C)/B

    if (theCoeffs.mB + Abs(theCoeffs.mC) > 1.0)
    {
      // -(1+C)/B < -1 or (1-C)/B > 1 ==> No solutions
      return Standard_False;
    }
    else if (-theCoeffs.mB + Abs(theCoeffs.mC) <= 1.0)
    {
      //  -(1+C)/B >= 1 and (1-C)/B <= -1 ==> U=[0;2*PI]+aFI1
      theURange[0].Add(theCoeffs.mFI1);
      theURange[0].Add(thePeriod + theCoeffs.mFI1);
    }
    else if ((-theCoeffs.mC - 1 <= theCoeffs.mB) && (theCoeffs.mB <= theCoeffs.mC - 1))
    {
      //  -(1+C)/B >= 1 and (1-C)/B >= -1 ==>
      //(U=[0;aDAngle]+aFI1) || (U=[2*PI-aDAngle;2*PI]+aFI1),
      // where aDAngle = acos((1 - myCoeffs.mC) / myCoeffs.mB)

      Standard_Real anArg = (1 - theCoeffs.mC) / theCoeffs.mB;
      if (anArg > 1.0)
        anArg = 1.0;
      if (anArg < -1.0)
        anArg = -1.0;

      const Standard_Real aDAngle = acos(anArg);
      theURange[0].Add(theCoeffs.mFI1);
      theURange[0].Add(aDAngle + theCoeffs.mFI1);
      theURange[1].Add(thePeriod - aDAngle + theCoeffs.mFI1);
      theURange[1].Add(thePeriod + theCoeffs.mFI1);
    }
    else if ((theCoeffs.mC - 1 <= theCoeffs.mB) && (theCoeffs.mB <= -theCoeffs.mB - 1))
    {
      //  -(1+C)/B <= 1 and (1-C)/B <= -1 ==> U=[aDAngle;2*PI-aDAngle]+aFI1,
      // where aDAngle = acos(-(myCoeffs.mC + 1) / myCoeffs.mB).

      Standard_Real anArg = -(theCoeffs.mC + 1) / theCoeffs.mB;
      if (anArg > 1.0)
        anArg = 1.0;
      if (anArg < -1.0)
        anArg = -1.0;

      const Standard_Real aDAngle = acos(anArg);
      theURange[0].Add(aDAngle + theCoeffs.mFI1);
      theURange[0].Add(thePeriod - aDAngle + theCoeffs.mFI1);
    }
    else if (-theCoeffs.mB - Abs(theCoeffs.mC) >= 1.0)
    {
      //  -(1+C)/B <= 1 and (1-C)/B >= -1 ==>
      //(U=[aDAngle1;aDAngle2]+aFI1) || (U=[2*PI-aDAngle2;2*PI-aDAngle1]+aFI1),
      // where aDAngle1 = acos(-(myCoeffs.mC + 1) / myCoeffs.mB),
      //      aDAngle2 = acos((1 - myCoeffs.mC) / myCoeffs.mB)

      Standard_Real anArg1 = -(theCoeffs.mC + 1) / theCoeffs.mB,
                    anArg2 = (1 - theCoeffs.mC) / theCoeffs.mB;
      if (anArg1 > 1.0)
        anArg1 = 1.0;
      if (anArg1 < -1.0)
        anArg1 = -1.0;

      if (anArg2 > 1.0)
        anArg2 = 1.0;
      if (anArg2 < -1.0)
        anArg2 = -1.0;

      const Standard_Real aDAngle1 = acos(anArg1), aDAngle2 = acos(anArg2);
      theURange[0].Add(aDAngle1 + theCoeffs.mFI1);
      theURange[0].Add(aDAngle2 + theCoeffs.mFI1);
      theURange[1].Add(thePeriod - aDAngle2 + theCoeffs.mFI1);
      theURange[1].Add(thePeriod - aDAngle1 + theCoeffs.mFI1);
    }
    else
    {
      return Standard_False;
    }
  }
  else
  {
    return Standard_False;
  }

  return Standard_True;
}

//=======================================================================
// function : CriticalPointsComputing
// purpose  : theNbCritPointsMax contains true number of critical points.
//            It must be initialized correctly before function calling
//=======================================================================
static void CriticalPointsComputing(const ComputationMethods::stCoeffsValue& theCoeffs,
                                    const Standard_Real                      theUSurf1f,
                                    const Standard_Real                      theUSurf1l,
                                    const Standard_Real                      theUSurf2f,
                                    const Standard_Real                      theUSurf2l,
                                    const Standard_Real                      thePeriod,
                                    const Standard_Real                      theTol2D,
                                    Standard_Integer&                        theNbCritPointsMax,
                                    Standard_Real                            theU1crit[])
{
  //[0...1] - in these points parameter U1 goes through
  //          the seam-edge of the first cylinder.
  //[2...3] - First and last U1 parameter.
  //[4...5] - in these points parameter U2 goes through
  //          the seam-edge of the second cylinder.
  //[6...9] - in these points an intersection line goes through
  //          U-boundaries of the second surface.
  //[10...11] - Boundary of monotonicity interval of U2(U1) function
  //            (see CylCylMonotonicity() function)

  theU1crit[0] = 0.0;
  theU1crit[1] = thePeriod;
  theU1crit[2] = theUSurf1f;
  theU1crit[3] = theUSurf1l;

  const Standard_Real aCOS = cos(theCoeffs.mFI2);
  const Standard_Real aBSB = Abs(theCoeffs.mB);
  if ((theCoeffs.mC - aBSB <= aCOS) && (aCOS <= theCoeffs.mC + aBSB))
  {
    Standard_Real anArg = (aCOS - theCoeffs.mC) / theCoeffs.mB;
    if (anArg > 1.0)
      anArg = 1.0;
    if (anArg < -1.0)
      anArg = -1.0;

    theU1crit[4] = -acos(anArg) + theCoeffs.mFI1;
    theU1crit[5] = acos(anArg) + theCoeffs.mFI1;
  }

  Standard_Real aSf = cos(theUSurf2f - theCoeffs.mFI2);
  Standard_Real aSl = cos(theUSurf2l - theCoeffs.mFI2);
  MinMax(aSf, aSl);

  // In accorance with pure mathematic, theU1crit[6] and [8]
  // must be -Precision::Infinite() instead of used +Precision::Infinite()
  theU1crit[6] = Abs((aSl - theCoeffs.mC) / theCoeffs.mB) < 1.0
                   ? -acos((aSl - theCoeffs.mC) / theCoeffs.mB) + theCoeffs.mFI1
                   : Precision::Infinite();
  theU1crit[7] = Abs((aSf - theCoeffs.mC) / theCoeffs.mB) < 1.0
                   ? -acos((aSf - theCoeffs.mC) / theCoeffs.mB) + theCoeffs.mFI1
                   : Precision::Infinite();
  theU1crit[8] = Abs((aSf - theCoeffs.mC) / theCoeffs.mB) < 1.0
                   ? acos((aSf - theCoeffs.mC) / theCoeffs.mB) + theCoeffs.mFI1
                   : Precision::Infinite();
  theU1crit[9] = Abs((aSl - theCoeffs.mC) / theCoeffs.mB) < 1.0
                   ? acos((aSl - theCoeffs.mC) / theCoeffs.mB) + theCoeffs.mFI1
                   : Precision::Infinite();

  theU1crit[10] = theCoeffs.mFI1;
  theU1crit[11] = M_PI + theCoeffs.mFI1;

  // preparative treatment of array. This array must have faled to contain negative
  // infinity number

  for (Standard_Integer i = 0; i < theNbCritPointsMax; i++)
  {
    if (Precision::IsInfinite(theU1crit[i]))
    {
      continue;
    }

    theU1crit[i] = fmod(theU1crit[i], thePeriod);
    if (theU1crit[i] < 0.0)
      theU1crit[i] += thePeriod;
  }

  // Here all not infinite elements of theU1crit are in [0, thePeriod) range

  do
  {
    std::sort(theU1crit, theU1crit + theNbCritPointsMax);
  } while (ExcludeNearElements(theU1crit, theNbCritPointsMax, theUSurf1f, theUSurf1l, theTol2D));

  // Here all not infinite elements in theU1crit are different and sorted.

  while (theNbCritPointsMax > 0)
  {
    Standard_Real& anB = theU1crit[theNbCritPointsMax - 1];
    if (Precision::IsInfinite(anB))
    {
      theNbCritPointsMax--;
      continue;
    }

    // 1st not infinte element is found

    if (theNbCritPointsMax == 1)
      break;

    // Here theNbCritPointsMax > 1

    Standard_Real& anA = theU1crit[0];

    // Compare 1st and last significant elements of theU1crit
    // They may still differs by period.

    if (Abs(anB - anA - thePeriod) < theTol2D)
    { // E.g. anA == 2.0e-17, anB == (thePeriod-1.0e-18)
      anA = (anA + anB - thePeriod) / 2.0;
      anB = Precision::Infinite();
      theNbCritPointsMax--;
    }

    // Out of "while(theNbCritPointsMax > 0)" cycle.
    break;
  }

  // Attention! Here theU1crit may be unsorted.
}

//=======================================================================
// function : BoundaryEstimation
// purpose  : Rough estimation of the parameter range.
//=======================================================================
void WorkWithBoundaries::BoundaryEstimation(const gp_Cylinder& theCy1,
                                            const gp_Cylinder& theCy2,
                                            Bnd_Range&         theOutBoxS1,
                                            Bnd_Range&         theOutBoxS2) const
{
  const gp_Dir &      aD1 = theCy1.Axis().Direction(), &aD2 = theCy2.Axis().Direction();
  const Standard_Real aR1 = theCy1.Radius(), aR2 = theCy2.Radius();

  // Let consider a parallelogram. Its edges are parallel to aD1 and aD2.
  // Its altitudes are equal to 2*aR1 and 2*aR2 (diameters of the cylinders).
  // In fact, this parallelogram is a projection of the cylinders to the plane
  // created by the intersected axes aD1 and aD2 (if the axes are skewed then
  // one axis can be translated by parallel shifting till intersection).

  const Standard_Real aCosA   = aD1.Dot(aD2);
  const Standard_Real aSqSinA = aD1.XYZ().CrossSquareMagnitude(aD2.XYZ());

  // If sine is small then it can be compared with angle.
  if (aSqSinA < Precision::Angular() * Precision::Angular())
    return;

  // Half of delta V. Delta V is a distance between
  // projections of two opposite parallelogram vertices
  //(joined by the maximal diagonal) to the cylinder axis.
  const Standard_Real aSinA     = sqrt(aSqSinA);
  const Standard_Real anAbsCosA = Abs(aCosA);
  const Standard_Real aHDV1     = (aR1 * anAbsCosA + aR2) / aSinA,
                      aHDV2     = (aR2 * anAbsCosA + aR1) / aSinA;

#ifdef INTPATCH_IMPIMPINTERSECTION_DEBUG
  // The code in this block is created for test only.It is stupidly to create
  // OCCT-test for the method, which will be changed possibly never.
  std::cout << "Reference values: aHDV1 = " << aHDV1 << "; aHDV2 = " << aHDV2 << std::endl;
#endif

  // V-parameters of intersection point of the axes (in case of skewed axes,
  // see comment above).
  Standard_Real aV01 = 0.0, aV02 = 0.0;
  ExtremaLineLine(theCy1.Axis(), theCy2.Axis(), aCosA, aSqSinA, aV01, aV02);

  theOutBoxS1.Add(aV01 - aHDV1);
  theOutBoxS1.Add(aV01 + aHDV1);

  theOutBoxS2.Add(aV02 - aHDV2);
  theOutBoxS2.Add(aV02 + aHDV2);

  theOutBoxS1.Enlarge(Precision::Confusion());
  theOutBoxS2.Enlarge(Precision::Confusion());

  Standard_Real aU1 = 0.0, aV1 = 0.0, aU2 = 0.0, aV2 = 0.0;
  myUVSurf1.Get(aU1, aV1, aU2, aV2);
  theOutBoxS1.Common(Bnd_Range(aV1, aV2));

  myUVSurf2.Get(aU1, aV1, aU2, aV2);
  theOutBoxS2.Common(Bnd_Range(aV1, aV2));
}

//=================================================================================================

static IntPatch_ImpImpIntersection::IntStatus CyCyNoGeometric(
  const gp_Cylinder&        theCyl1,
  const gp_Cylinder&        theCyl2,
  const WorkWithBoundaries& theBW,
  Bnd_Range                 theRange[],
  const Standard_Integer    theNbOfRanges /*=2*/,
  Standard_Boolean&         isTheEmpty,
  IntPatch_SequenceOfLine&  theSlin,
  IntPatch_SequenceOfPoint& theSPnt)
{
  Standard_Real aUSurf1f = 0.0, aUSurf1l = 0.0, aUSurf2f = 0.0, aUSurf2l = 0.0, aVSurf1f = 0.0,
                aVSurf1l = 0.0, aVSurf2f = 0.0, aVSurf2l = 0.0;

  theBW.UVS1().Get(aUSurf1f, aVSurf1f, aUSurf1l, aVSurf1l);
  theBW.UVS2().Get(aUSurf2f, aVSurf2f, aUSurf2l, aVSurf2l);

  Bnd_Range aRangeS1, aRangeS2;
  theBW.BoundaryEstimation(theCyl1, theCyl2, aRangeS1, aRangeS2);
  if (aRangeS1.IsVoid() || aRangeS2.IsVoid())
    return IntPatch_ImpImpIntersection::IntStatus_OK;

  {
    // Quotation of the message from issue #26894 (author MSV):
    //"We should return fail status from intersector if the result should be an
    // infinite curve of non-analytical type... I propose to define the limit for the
    // extent as the radius divided by 1e+2 and multiplied by 1e+7.
    // Thus, taking into account the number of valuable digits (15), we provide reliable
    // computations with an error not exceeding R/100."
    const Standard_Real aF          = 1.0e+5;
    const Standard_Real aMaxV1Range = aF * theCyl1.Radius(), aMaxV2Range = aF * theCyl2.Radius();
    if ((aRangeS1.Delta() > aMaxV1Range) || (aRangeS2.Delta() > aMaxV2Range))
      return IntPatch_ImpImpIntersection::IntStatus_InfiniteSectionCurve;
  }
  //
  Standard_Boolean isGoodIntersection = Standard_False;
  Standard_Real    anOptdu            = 0.;
  for (;;)
  {
    // Checking parameters of cylinders in order to define "good intersection"
    //"Good intersection" means that axes of cylinders are almost perpendicular and
    //  one radius is much smaller than the other and small cylinder is "inside" big one.
    const Standard_Real aToMuchCoeff = 3.;
    const Standard_Real aCritAngle   = M_PI / 18.; // 10 degree
    Standard_Real       anR1         = theCyl1.Radius();
    Standard_Real       anR2         = theCyl2.Radius();
    Standard_Real       anRmin = 0., anRmax = 0.;
    // Radius criterion
    if (anR1 > aToMuchCoeff * anR2)
    {
      anRmax = anR1;
      anRmin = anR2;
    }
    else if (anR2 > aToMuchCoeff * anR1)
    {
      anRmax = anR2;
      anRmin = anR1;
    }
    else
    {
      break;
    }
    // Angle criterion
    const gp_Ax1& anAx1 = theCyl1.Axis();
    const gp_Ax1& anAx2 = theCyl2.Axis();
    if (!anAx1.IsNormal(anAx2, aCritAngle))
    {
      break;
    }
    // Placement criterion
    gp_Lin        anL1(anAx1), anL2(anAx2);
    Standard_Real aDist = anL1.Distance(anL2);
    if (aDist > anRmax / 2.)
    {
      break;
    }

    isGoodIntersection = Standard_True;
    // Estimation of "optimal" du
    // Relative deflection, absolut deflection is Rmin*aDeflection
    Standard_Real    aDeflection = 0.001;
    Standard_Integer aNbP        = 3;
    if (anRmin * aDeflection > 1.e-3)
    {
      Standard_Real anAngle = 1.0e0 - aDeflection;
      anAngle               = 2.0e0 * ACos(anAngle);
      aNbP                  = (Standard_Integer)(2. * M_PI / anAngle) + 1;
    }
    anOptdu = 2. * M_PI_2 / (Standard_Real)(aNbP - 1);
    break;
  }
  //
  const ComputationMethods::stCoeffsValue& anEquationCoeffs = theBW.SICoeffs();
  const IntSurf_Quadric&                   aQuad1           = theBW.GetQSurface(1);
  const IntSurf_Quadric&                   aQuad2           = theBW.GetQSurface(2);
  const Standard_Boolean                   isReversed       = theBW.IsReversed();
  const Standard_Real                      aTol2D           = theBW.Get2dTolerance();
  const Standard_Real                      aTol3D           = theBW.Get3dTolerance();
  const Standard_Real                      aPeriod          = 2.0 * M_PI;
  Standard_Integer                         aNbMaxPoints     = 1000;
  Standard_Integer                         aNbMinPoints     = 200;
  Standard_Real                            du;
  if (isGoodIntersection)
  {
    du           = anOptdu;
    aNbMaxPoints = 200;
    aNbMinPoints = 50;
  }
  else
  {
    du = 2. * M_PI / aNbMaxPoints;
  }
  Standard_Integer aNbPts =
    Min(RealToInt((aUSurf1l - aUSurf1f) / du) + 1, RealToInt(20.0 * theCyl1.Radius()));
  const Standard_Integer aNbPoints = Min(Max(aNbMinPoints, aNbPts), aNbMaxPoints);
  const Standard_Real    aStepMin  = Max(aTol2D, Precision::PConfusion()),
                      aStepMax     = (aUSurf1l - aUSurf1f > M_PI / 100.0)
                                       ? (aUSurf1l - aUSurf1f) / IntToReal(aNbPoints)
                                       : aUSurf1l - aUSurf1f;

  // The main idea of the algorithm is to change U1-parameter
  //(U-parameter of theCyl1) from aU1f to aU1l with some step
  //(step is adaptive) and to obtain set of intersection points.

  for (Standard_Integer i = 0; i < theNbOfRanges; i++)
  {
    if (theRange[i].IsVoid())
      continue;

    InscribeInterval(aUSurf1f, aUSurf1l, theRange[i], aTol2D, aPeriod);
  }

  if (theRange[0].Union(theRange[1]))
  {
    // Works only if (theNbOfRanges == 2).
    theRange[1].SetVoid();
  }

  // Critical points are the value of U1-parameter in the points
  // where WL must be decomposed.

  // When U1 goes through critical points its value is set up to this
  // parameter forcefully and the intersection point is added in the line.
  // After that, the WL is broken (next U1 value will be correspond to the new WL).

  // See CriticalPointsComputing(...) function to get detail information about this array.
  const Standard_Integer aNbCritPointsMax           = 12;
  Standard_Real          anU1crit[aNbCritPointsMax] = {Precision::Infinite(),
                                                       Precision::Infinite(),
                                                       Precision::Infinite(),
                                                       Precision::Infinite(),
                                                       Precision::Infinite(),
                                                       Precision::Infinite(),
                                                       Precision::Infinite(),
                                                       Precision::Infinite(),
                                                       Precision::Infinite(),
                                                       Precision::Infinite(),
                                                       Precision::Infinite(),
                                                       Precision::Infinite()};

  // This list of critical points is not full because it does not contain any points
  // which intersection line goes through V-bounds of cylinders in.
  // They are computed by numerical methods on - line (during algorithm working).
  // The moment is caught, when intersection line goes through V-bounds of any cylinder.

  Standard_Integer aNbCritPoints = aNbCritPointsMax;
  CriticalPointsComputing(anEquationCoeffs,
                          aUSurf1f,
                          aUSurf1l,
                          aUSurf2f,
                          aUSurf2l,
                          aPeriod,
                          aTol2D,
                          aNbCritPoints,
                          anU1crit);

  // Getting Walking-line

  enum WLFStatus
  {
    // No points have been added in WL
    WLFStatus_Absent = 0,
    // WL contains at least one point
    WLFStatus_Exist = 1,
    // WL has been finished in some critical point
    // We should start new line
    WLFStatus_Broken = 2
  };

  const Standard_Integer aNbWLines = 2;
  for (Standard_Integer aCurInterval = 0; aCurInterval < theNbOfRanges; aCurInterval++)
  {
    // Process every continuous region
    Standard_Boolean isAddedIntoWL[aNbWLines];
    for (Standard_Integer i = 0; i < aNbWLines; i++)
      isAddedIntoWL[i] = Standard_False;

    Standard_Real anUf = 1.0, anUl = 0.0;
    if (!theRange[aCurInterval].GetBounds(anUf, anUl))
      continue;

    const Standard_Boolean isDeltaPeriod = IsEqual(anUl - anUf, aPeriod);

    // Inscribe and sort critical points
    for (Standard_Integer i = 0; i < aNbCritPoints; i++)
    {
      InscribePoint(anUf, anUl, anU1crit[i], 0.0, aPeriod, Standard_False);
    }

    std::sort(anU1crit, anU1crit + aNbCritPoints);

    while (anUf < anUl)
    {
      // Change value of U-parameter on the 1st surface from anUf to anUl
      //(anUf will be modified in the cycle body).
      // Step is computed adaptively (see comments below).

      Standard_Real    aU2[aNbWLines], aV1[aNbWLines], aV2[aNbWLines];
      WLFStatus        aWLFindStatus[aNbWLines];
      Standard_Real    aV1Prev[aNbWLines], aV2Prev[aNbWLines];
      Standard_Real    anUexpect[aNbWLines];
      Standard_Boolean isAddingWLEnabled[aNbWLines];

      Handle(IntSurf_LineOn2S) aL2S[aNbWLines];
      Handle(IntPatch_WLine)   aWLine[aNbWLines];
      for (Standard_Integer i = 0; i < aNbWLines; i++)
      {
        aL2S[i]   = new IntSurf_LineOn2S();
        aWLine[i] = new IntPatch_WLine(aL2S[i], Standard_False);
        aWLine[i]->SetCreatingWayInfo(IntPatch_WLine::IntPatch_WLImpImp);
        aWLFindStatus[i]     = WLFStatus_Absent;
        isAddingWLEnabled[i] = Standard_True;
        aU2[i] = aV1[i] = aV2[i] = 0.0;
        aV1Prev[i] = aV2Prev[i] = 0.0;
        anUexpect[i]            = anUf;
      }

      Standard_Real aCriticalDelta[aNbCritPointsMax] = {0};
      for (Standard_Integer aCritPID = 0; aCritPID < aNbCritPoints; aCritPID++)
      { // We are not interested in elements of aCriticalDelta array
        // if their index is greater than or equal to aNbCritPoints

        aCriticalDelta[aCritPID] = anUf - anU1crit[aCritPID];
      }

      Standard_Real    anU1 = anUf, aMinCriticalParam = anUf;
      Standard_Boolean isFirst = Standard_True;

      while (anU1 <= anUl)
      {
        // Change value of U-parameter on the 1st surface from anUf to anUl
        //(anUf will be modified in the cycle body). However, this cycle
        // can be broken if WL goes though some critical point.
        // Step is computed adaptively (see comments below).

        for (Standard_Integer i = 0; i < aNbCritPoints; i++)
        {
          if ((anU1 - anU1crit[i]) * aCriticalDelta[i] < 0.0)
          {
            // WL has gone through i-th critical point
            anU1 = anU1crit[i];

            for (Standard_Integer j = 0; j < aNbWLines; j++)
            {
              aWLFindStatus[j] = WLFStatus_Broken;
              anUexpect[j]     = anU1;
            }

            break;
          }
        }

        if (IsEqual(anU1, anUl))
        {
          for (Standard_Integer i = 0; i < aNbWLines; i++)
          {
            aWLFindStatus[i] = WLFStatus_Broken;
            anUexpect[i]     = anU1;

            if (isDeltaPeriod)
            {
              // if isAddedIntoWL[i] == TRUE WLine contains only one point
              //(which was end point of previous WLine). If we will
              // add point found on the current step WLine will contain only
              // two points. At that both these points will be equal to the
              // points found earlier. Therefore, new WLine will repeat
              // already existing WLine. Consequently, it is necessary
              // to forbid building new line in this case.

              isAddingWLEnabled[i] = (!isAddedIntoWL[i]);
            }
            else
            {
              isAddingWLEnabled[i] =
                ((aTol2D >= (anUexpect[i] - anU1)) || (aWLFindStatus[i] == WLFStatus_Absent));
            }
          } // for(Standard_Integer i = 0; i < aNbWLines; i++)
        }
        else
        {
          for (Standard_Integer i = 0; i < aNbWLines; i++)
          {
            isAddingWLEnabled[i] =
              ((aTol2D >= (anUexpect[i] - anU1)) || (aWLFindStatus[i] == WLFStatus_Absent));
          } // for(Standard_Integer i = 0; i < aNbWLines; i++)
        }

        for (Standard_Integer i = 0; i < aNbWLines; i++)
        {
          const Standard_Integer aNbPntsWL =
            aWLine[i].IsNull() ? 0 : aWLine[i]->Curve()->NbPoints();

          if ((aWLFindStatus[i] == WLFStatus_Broken) || (aWLFindStatus[i] == WLFStatus_Absent))
          { // Begin and end of WLine must be on boundary point
            // or on seam-edge strictly (if it is possible).

            Standard_Real aTol = aTol2D;
            ComputationMethods::CylCylComputeParameters(anU1, i, anEquationCoeffs, aU2[i], &aTol);
            InscribePoint(aUSurf2f, aUSurf2l, aU2[i], aTol2D, aPeriod, Standard_False);

            aTol = Max(aTol, aTol2D);

            if (Abs(aU2[i]) <= aTol)
              aU2[i] = 0.0;
            else if (Abs(aU2[i] - aPeriod) <= aTol)
              aU2[i] = aPeriod;
            else if (Abs(aU2[i] - aUSurf2f) <= aTol)
              aU2[i] = aUSurf2f;
            else if (Abs(aU2[i] - aUSurf2l) <= aTol)
              aU2[i] = aUSurf2l;
          }
          else
          {
            ComputationMethods::CylCylComputeParameters(anU1, i, anEquationCoeffs, aU2[i]);
            InscribePoint(aUSurf2f, aUSurf2l, aU2[i], aTol2D, aPeriod, Standard_False);
          }

          if (aNbPntsWL == 0)
          { // the line has not contained any points yet
            if (((aUSurf2f + aPeriod - aUSurf2l) <= 2.0 * aTol2D)
                && ((Abs(aU2[i] - aUSurf2f) < aTol2D) || (Abs(aU2[i] - aUSurf2l) < aTol2D)))
            {
              // In this case aU2[i] can have two values: current aU2[i] or
              // aU2[i]+aPeriod (aU2[i]-aPeriod). It is necessary to choose
              // correct value.

              Standard_Boolean isIncreasing = Standard_True;
              ComputationMethods::CylCylMonotonicity(anU1 + aStepMin,
                                                     i,
                                                     anEquationCoeffs,
                                                     aPeriod,
                                                     isIncreasing);

              // If U2(U1) is increasing and U2 is considered to be equal aUSurf2l
              // then after the next step (when U1 will be increased) U2 will be
              // increased too. And we will go out of surface boundary.
              // Therefore, If U2(U1) is increasing then U2 must be equal aUSurf2f.
              // Analogically, if U2(U1) is decreasing.
              if (isIncreasing)
              {
                aU2[i] = aUSurf2f;
              }
              else
              {
                aU2[i] = aUSurf2l;
              }
            }
          }
          else
          { // aNbPntsWL > 0
            if (((aUSurf2l - aUSurf2f) >= aPeriod)
                && ((Abs(aU2[i] - aUSurf2f) < aTol2D) || (Abs(aU2[i] - aUSurf2l) < aTol2D)))
            { // end of the line
              Standard_Real aU2prev = 0.0, aV2prev = 0.0;
              if (isReversed)
                aWLine[i]->Curve()->Value(aNbPntsWL).ParametersOnS1(aU2prev, aV2prev);
              else
                aWLine[i]->Curve()->Value(aNbPntsWL).ParametersOnS2(aU2prev, aV2prev);

              if (2.0 * Abs(aU2prev - aU2[i]) > aPeriod)
              {
                if (aU2prev > aU2[i])
                  aU2[i] += aPeriod;
                else
                  aU2[i] -= aPeriod;
              }
            }
          }

          ComputationMethods::CylCylComputeParameters(anU1,
                                                      aU2[i],
                                                      anEquationCoeffs,
                                                      aV1[i],
                                                      aV2[i]);

          if (isFirst)
          {
            aV1Prev[i] = aV1[i];
            aV2Prev[i] = aV2[i];
          }
        } // for(Standard_Integer i = 0; i < aNbWLines; i++)

        isFirst = Standard_False;

        // Looking for points into WLine
        Standard_Boolean isBroken = Standard_False;
        for (Standard_Integer i = 0; i < aNbWLines; i++)
        {
          if (!isAddingWLEnabled[i])
          {
            Standard_Boolean isBoundIntersect = Standard_False;
            if ((Abs(aV1[i] - aVSurf1f) <= aTol2D)
                || ((aV1[i] - aVSurf1f) * (aV1Prev[i] - aVSurf1f) < 0.0))
            {
              isBoundIntersect = Standard_True;
            }
            else if ((Abs(aV1[i] - aVSurf1l) <= aTol2D)
                     || ((aV1[i] - aVSurf1l) * (aV1Prev[i] - aVSurf1l) < 0.0))
            {
              isBoundIntersect = Standard_True;
            }
            else if ((Abs(aV2[i] - aVSurf2f) <= aTol2D)
                     || ((aV2[i] - aVSurf2f) * (aV2Prev[i] - aVSurf2f) < 0.0))
            {
              isBoundIntersect = Standard_True;
            }
            else if ((Abs(aV2[i] - aVSurf2l) <= aTol2D)
                     || ((aV2[i] - aVSurf2l) * (aV2Prev[i] - aVSurf2l) < 0.0))
            {
              isBoundIntersect = Standard_True;
            }

            if (aWLFindStatus[i] == WLFStatus_Broken)
              isBroken = Standard_True;

            if (!isBoundIntersect)
            {
              continue;
            }
            else
            {
              anUexpect[i] = anU1;
            }
          }

          // True if the current point already in the domain
          const Standard_Boolean isInscribe =
            ((aUSurf2f - aU2[i]) <= aTol2D) && ((aU2[i] - aUSurf2l) <= aTol2D)
            && ((aVSurf1f - aV1[i]) <= aTol2D) && ((aV1[i] - aVSurf1l) <= aTol2D)
            && ((aVSurf2f - aV2[i]) <= aTol2D) && ((aV2[i] - aVSurf2l) <= aTol2D);

          // isVIntersect == TRUE if intersection line intersects two (!)
          // V-bounds of cylinder (1st or 2nd - no matter)
          const Standard_Boolean isVIntersect =
            (((aVSurf1f - aV1[i]) * (aVSurf1f - aV1Prev[i]) < RealSmall())
             && ((aVSurf1l - aV1[i]) * (aVSurf1l - aV1Prev[i]) < RealSmall()))
            || (((aVSurf2f - aV2[i]) * (aVSurf2f - aV2Prev[i]) < RealSmall())
                && ((aVSurf2l - aV2[i]) * (aVSurf2l - aV2Prev[i]) < RealSmall()));

          // isFound1 == TRUE if intersection line intersects V-bounds
          //   (First or Last - no matter) of the 1st cylynder
          // isFound2 == TRUE if intersection line intersects V-bounds
          //   (First or Last - no matter) of the 2nd cylynder
          Standard_Boolean isFound1 = Standard_False, isFound2 = Standard_False;
          Standard_Boolean isForce = Standard_False;

          if (aWLFindStatus[i] == WLFStatus_Absent)
          {
            if (((aUSurf2l - aUSurf2f) >= aPeriod) && (Abs(anU1 - aUSurf1l) < aTol2D))
            {
              isForce = Standard_True;
            }
          }

          theBW.AddBoundaryPoint(aWLine[i],
                                 anU1,
                                 aMinCriticalParam,
                                 aU2[i],
                                 aV1[i],
                                 aV1Prev[i],
                                 aV2[i],
                                 aV2Prev[i],
                                 i,
                                 isForce,
                                 isFound1,
                                 isFound2);

          const Standard_Boolean isPrevVBound =
            !isVIntersect
            && ((Abs(aV1Prev[i] - aVSurf1f) <= aTol2D) || (Abs(aV1Prev[i] - aVSurf1l) <= aTol2D)
                || (Abs(aV2Prev[i] - aVSurf2f) <= aTol2D)
                || (Abs(aV2Prev[i] - aVSurf2l) <= aTol2D));

          aV1Prev[i] = aV1[i];
          aV2Prev[i] = aV2[i];

          if ((aWLFindStatus[i] == WLFStatus_Exist) && (isFound1 || isFound2) && !isPrevVBound)
          {
            aWLFindStatus[i] = WLFStatus_Broken; // start a new line
          }
          else if (isInscribe)
          {
            if ((aWLFindStatus[i] == WLFStatus_Absent) && (isFound1 || isFound2))
            {
              aWLFindStatus[i] = WLFStatus_Exist;
            }

            if ((aWLFindStatus[i] != WLFStatus_Broken) || (aWLine[i]->NbPnts() >= 1)
                || IsEqual(anU1, anUl))
            {
              if (aWLine[i]->NbPnts() > 0)
              {
                Standard_Real aU2p = 0.0, aV2p = 0.0;
                if (isReversed)
                  aWLine[i]->Point(aWLine[i]->NbPnts()).ParametersOnS1(aU2p, aV2p);
                else
                  aWLine[i]->Point(aWLine[i]->NbPnts()).ParametersOnS2(aU2p, aV2p);

                const Standard_Real aDelta = aU2[i] - aU2p;

                if (2.0 * Abs(aDelta) > aPeriod)
                {
                  if (aDelta > 0.0)
                  {
                    aU2[i] -= aPeriod;
                  }
                  else
                  {
                    aU2[i] += aPeriod;
                  }
                }
              }

              if (AddPointIntoWL(aQuad1,
                                 aQuad2,
                                 anEquationCoeffs,
                                 isReversed,
                                 Standard_True,
                                 gp_Pnt2d(anU1, aV1[i]),
                                 gp_Pnt2d(aU2[i], aV2[i]),
                                 aUSurf1f,
                                 aUSurf1l,
                                 aUSurf2f,
                                 aUSurf2l,
                                 aVSurf1f,
                                 aVSurf1l,
                                 aVSurf2f,
                                 aVSurf2l,
                                 aPeriod,
                                 aWLine[i]->Curve(),
                                 i,
                                 aTol3D,
                                 aTol2D,
                                 isForce))
              {
                if (aWLFindStatus[i] == WLFStatus_Absent)
                {
                  aWLFindStatus[i] = WLFStatus_Exist;
                }
              }
              else if (!isFound1 && !isFound2)
              { // We do not add any point while doing this iteration
                if (aWLFindStatus[i] == WLFStatus_Exist)
                {
                  aWLFindStatus[i] = WLFStatus_Broken;
                }
              }
            }
          }
          else
          { // We do not add any point while doing this iteration
            if (aWLFindStatus[i] == WLFStatus_Exist)
            {
              aWLFindStatus[i] = WLFStatus_Broken;
            }
          }

          if (aWLFindStatus[i] == WLFStatus_Broken)
            isBroken = Standard_True;
        } // for(Standard_Integer i = 0; i < aNbWLines; i++)

        if (isBroken)
        { // current lines are filled. Go to the next lines
          anUf = anU1;

          Standard_Boolean isAdded = Standard_True;

          for (Standard_Integer i = 0; i < aNbWLines; i++)
          {
            if (isAddingWLEnabled[i])
            {
              continue;
            }

            isAdded = Standard_False;

            Standard_Boolean isFound1 = Standard_False, isFound2 = Standard_False;

            theBW.AddBoundaryPoint(aWLine[i],
                                   anU1,
                                   aMinCriticalParam,
                                   aU2[i],
                                   aV1[i],
                                   aV1Prev[i],
                                   aV2[i],
                                   aV2Prev[i],
                                   i,
                                   Standard_False,
                                   isFound1,
                                   isFound2);

            if (isFound1 || isFound2)
            {
              isAdded = Standard_True;
            }

            if (aWLine[i]->NbPnts() > 0)
            {
              Standard_Real aU2p = 0.0, aV2p = 0.0;
              if (isReversed)
                aWLine[i]->Point(aWLine[i]->NbPnts()).ParametersOnS1(aU2p, aV2p);
              else
                aWLine[i]->Point(aWLine[i]->NbPnts()).ParametersOnS2(aU2p, aV2p);

              const Standard_Real aDelta = aU2[i] - aU2p;

              if (2 * Abs(aDelta) > aPeriod)
              {
                if (aDelta > 0.0)
                {
                  aU2[i] -= aPeriod;
                }
                else
                {
                  aU2[i] += aPeriod;
                }
              }
            }

            if (AddPointIntoWL(aQuad1,
                               aQuad2,
                               anEquationCoeffs,
                               isReversed,
                               Standard_True,
                               gp_Pnt2d(anU1, aV1[i]),
                               gp_Pnt2d(aU2[i], aV2[i]),
                               aUSurf1f,
                               aUSurf1l,
                               aUSurf2f,
                               aUSurf2l,
                               aVSurf1f,
                               aVSurf1l,
                               aVSurf2f,
                               aVSurf2l,
                               aPeriod,
                               aWLine[i]->Curve(),
                               i,
                               aTol3D,
                               aTol2D,
                               Standard_False))
            {
              isAdded = Standard_True;
            }
          }

          if (!isAdded)
          {
            // Before breaking WL, we must complete it correctly
            //(e.g. to prolong to the surface boundary).
            // Therefore, we take the point last added in some WL
            //(have maximal U1-parameter) and try to add it in
            // the current WL.
            Standard_Real anUmaxAdded = RealFirst();

            {
              Standard_Boolean isChanged = Standard_False;
              for (Standard_Integer i = 0; i < aNbWLines; i++)
              {
                if ((aWLFindStatus[i] == WLFStatus_Absent) || (aWLine[i]->NbPnts() == 0))
                  continue;

                Standard_Real aU1c = 0.0, aV1c = 0.0;
                if (isReversed)
                  aWLine[i]->Curve()->Value(aWLine[i]->NbPnts()).ParametersOnS2(aU1c, aV1c);
                else
                  aWLine[i]->Curve()->Value(aWLine[i]->NbPnts()).ParametersOnS1(aU1c, aV1c);

                anUmaxAdded = Max(anUmaxAdded, aU1c);
                isChanged   = Standard_True;
              }

              if (!isChanged)
              { // If anUmaxAdded were not changed in previous cycle then
                // we would break existing WLines.
                break;
              }
            }

            for (Standard_Integer i = 0; i < aNbWLines; i++)
            {
              if (isAddingWLEnabled[i])
              {
                continue;
              }

              ComputationMethods::CylCylComputeParameters(anUmaxAdded,
                                                          i,
                                                          anEquationCoeffs,
                                                          aU2[i],
                                                          aV1[i],
                                                          aV2[i]);

              AddPointIntoWL(aQuad1,
                             aQuad2,
                             anEquationCoeffs,
                             isReversed,
                             Standard_True,
                             gp_Pnt2d(anUmaxAdded, aV1[i]),
                             gp_Pnt2d(aU2[i], aV2[i]),
                             aUSurf1f,
                             aUSurf1l,
                             aUSurf2f,
                             aUSurf2l,
                             aVSurf1f,
                             aVSurf1l,
                             aVSurf2f,
                             aVSurf2l,
                             aPeriod,
                             aWLine[i]->Curve(),
                             i,
                             aTol3D,
                             aTol2D,
                             Standard_False);
            }
          }

          break;
        }

        // Step computing

        {
          // Step of aU1-parameter is computed adaptively. The algorithm
          // aims to provide given aDeltaV1 and aDeltaV2 values (if it is
          // possible because the intersection line can go along V-isoline)
          // in every iteration. It allows avoiding "flying" intersection
          // points too far each from other (see issue #24915).

          const Standard_Real aDeltaV1 = aRangeS1.Delta() / IntToReal(aNbPoints),
                              aDeltaV2 = aRangeS2.Delta() / IntToReal(aNbPoints);

          math_Matrix aMatr(1, 3, 1, 5);

          Standard_Real aMinUexp = RealLast();

          for (Standard_Integer i = 0; i < aNbWLines; i++)
          {
            if (aTol2D < (anUexpect[i] - anU1))
            {
              continue;
            }

            if (aWLFindStatus[i] == WLFStatus_Absent)
            {
              anUexpect[i] += aStepMax;
              aMinUexp = Min(aMinUexp, anUexpect[i]);
              continue;
            }
            //
            if (isGoodIntersection)
            {
              // Use constant step
              anUexpect[i] += aStepMax;
              aMinUexp = Min(aMinUexp, anUexpect[i]);

              continue;
            }
            //

            Standard_Real aStepTmp = aStepMax;

            const Standard_Real aSinU1 = sin(anU1), aCosU1 = cos(anU1), aSinU2 = sin(aU2[i]),
                                aCosU2 = cos(aU2[i]);

            aMatr.SetCol(1, anEquationCoeffs.mVecC1);
            aMatr.SetCol(2, anEquationCoeffs.mVecC2);
            aMatr.SetCol(3, anEquationCoeffs.mVecA1 * aSinU1 - anEquationCoeffs.mVecB1 * aCosU1);
            aMatr.SetCol(4, anEquationCoeffs.mVecA2 * aSinU2 - anEquationCoeffs.mVecB2 * aCosU2);
            aMatr.SetCol(5,
                         anEquationCoeffs.mVecA1 * aCosU1 + anEquationCoeffs.mVecB1 * aSinU1
                           + anEquationCoeffs.mVecA2 * aCosU2 + anEquationCoeffs.mVecB2 * aSinU2
                           + anEquationCoeffs.mVecD);

            // The main idea is in solving of linearized system (2)
            //(see description to ComputationMethods class) in order to find new U1-value
            // to provide new value V1 or V2, which differs from current one by aDeltaV1 or
            // aDeltaV2 respectively.

            // While linearizing, following Taylor formulas are used:
            //     cos(x0+dx) = cos(x0) - sin(x0)*dx
            //     sin(x0+dx) = sin(x0) + cos(x0)*dx

            // Consequently cos(U1), cos(U2), sin(U1) and sin(U2) in the system (2)
            // must be substituted by corresponding values.

            // ATTENTION!!!
            // The solution is approximate. More over, all requirements to the
            // linearization must be satisfied in order to obtain quality result.

            if (!StepComputing(aMatr, aV1[i], aV2[i], aDeltaV1, aDeltaV2, aStepTmp))
            {
              // To avoid cycling-up
              anUexpect[i] += aStepMax;
              aMinUexp = Min(aMinUexp, anUexpect[i]);

              continue;
            }

            if (aStepTmp < aStepMin)
              aStepTmp = aStepMin;

            if (aStepTmp > aStepMax)
              aStepTmp = aStepMax;

            anUexpect[i] = anU1 + aStepTmp;
            aMinUexp     = Min(aMinUexp, anUexpect[i]);
          }

          anU1 = aMinUexp;
        }

        if (Precision::PConfusion() >= (anUl - anU1))
          anU1 = anUl;

        anUf = anU1;

        for (Standard_Integer i = 0; i < aNbWLines; i++)
        {
          if (aWLine[i]->NbPnts() != 1)
            isAddedIntoWL[i] = Standard_False;

          if (anU1 == anUl)
          { // strictly equal. Tolerance is considered above.
            anUexpect[i] = anUl;
          }
        }
      }

      for (Standard_Integer i = 0; i < aNbWLines; i++)
      {
        if ((aWLine[i]->NbPnts() == 1) && (!isAddedIntoWL[i]))
        {
          isTheEmpty = Standard_False;
          Standard_Real u1, v1, u2, v2;
          aWLine[i]->Point(1).Parameters(u1, v1, u2, v2);
          IntPatch_Point aP;
          aP.SetParameter(u1);
          aP.SetParameters(u1, v1, u2, v2);
          aP.SetTolerance(aTol3D);
          aP.SetValue(aWLine[i]->Point(1).Value());

          // Check whether the added point exists.
          // It is enough to check the last point.
          if (theSPnt.IsEmpty()
              || !theSPnt.Last().PntOn2S().IsSame(aP.PntOn2S(), Precision::Confusion()))
          {
            theSPnt.Append(aP);
          }
        }
        else if (aWLine[i]->NbPnts() > 1)
        {
          Standard_Boolean isGood = Standard_True;

          if (aWLine[i]->NbPnts() == 2)
          {
            const IntSurf_PntOn2S& aPf = aWLine[i]->Point(1);
            const IntSurf_PntOn2S& aPl = aWLine[i]->Point(2);

            if (aPf.IsSame(aPl, Precision::Confusion()))
              isGood = Standard_False;
          }
          else if (aWLine[i]->NbPnts() > 2)
          {
            // Sometimes points of the WLine are distributed
            // linearly and uniformly. However, such position
            // of the points does not always describe the real intersection
            // curve. I.e. real tangents at the ends of the intersection
            // curve can significantly deviate from this "line" direction.
            // Here we are processing this case by inserting additional points
            // to the beginning/end of the WLine to make it more precise.
            // See description to the issue #30082.

            const Standard_Real aSqTol3D = aTol3D * aTol3D;
            for (Standard_Integer j = 0; j < 2; j++)
            {
              // If j == 0 ==> add point at begin of WLine.
              // If j == 1 ==> add point at end of WLine.

              for (;;)
              {
                if (aWLine[i]->NbPnts() >= aNbMaxPoints)
                {
                  break;
                }

                // Take 1st and 2nd point to compute the "line" direction.
                // For our convenience, we make 2nd point be the ends of the WLine
                // because it will be used for computation of the normals
                // to the surfaces.
                const Standard_Integer anIdx1 = j ? aWLine[i]->NbPnts() - 1 : 2;
                const Standard_Integer anIdx2 = j ? aWLine[i]->NbPnts() : 1;

                const gp_Pnt& aP1 = aWLine[i]->Point(anIdx1).Value();
                const gp_Pnt& aP2 = aWLine[i]->Point(anIdx2).Value();

                const gp_Vec aDir(aP1, aP2);

                if (aDir.SquareMagnitude() < aSqTol3D)
                {
                  break;
                }

                // Compute tangent in first/last point of the WLine.
                // We do not take into account the flag "isReversed"
                // because strict direction of the tangent is not
                // important here (we are interested in the tangent
                // line itself and nothing to fear if its direction
                // is reversed).
                const gp_Vec aN1 = aQuad1.Normale(aP2);
                const gp_Vec aN2 = aQuad2.Normale(aP2);
                const gp_Vec aTg(aN1.Crossed(aN2));

                if (aTg.SquareMagnitude() < Precision::SquareConfusion())
                {
                  // Tangent zone
                  break;
                }

                // Check of the bending
                Standard_Real anAngle = aDir.Angle(aTg);

                if (anAngle > M_PI_2)
                  anAngle -= M_PI;

                if (Abs(anAngle) > 0.25) // ~ 14deg.
                {
                  const Standard_Integer aNbPntsPrev = aWLine[i]->NbPnts();
                  SeekAdditionalPoints(aQuad1,
                                       aQuad2,
                                       aWLine[i]->Curve(),
                                       anEquationCoeffs,
                                       i,
                                       3,
                                       anIdx1,
                                       anIdx2,
                                       aTol2D,
                                       aPeriod,
                                       isReversed);

                  if (aWLine[i]->NbPnts() == aNbPntsPrev)
                  {
                    // No points have been added. ==> Exit from a loop.
                    break;
                  }
                }
                else
                {
                  // Good result has been achieved. ==> Exit from a loop.
                  break;
                }
              } // for (;;)
            }
          }

          if (isGood)
          {
            isTheEmpty       = Standard_False;
            isAddedIntoWL[i] = Standard_True;
            SeekAdditionalPoints(aQuad1,
                                 aQuad2,
                                 aWLine[i]->Curve(),
                                 anEquationCoeffs,
                                 i,
                                 aNbPoints,
                                 1,
                                 aWLine[i]->NbPnts(),
                                 aTol2D,
                                 aPeriod,
                                 isReversed);

            aWLine[i]->ComputeVertexParameters(aTol3D);
            theSlin.Append(aWLine[i]);
          }
        }
        else
        {
          isAddedIntoWL[i] = Standard_False;
        }

#ifdef INTPATCH_IMPIMPINTERSECTION_DEBUG
        aWLine[i]->Dump(0);
#endif
      }
    }
  }

  // Delete the points in theSPnt, which
  // lie at least in one of the line in theSlin.
  for (Standard_Integer aNbPnt = 1; aNbPnt <= theSPnt.Length(); aNbPnt++)
  {
    for (Standard_Integer aNbLin = 1; aNbLin <= theSlin.Length(); aNbLin++)
    {
      Handle(IntPatch_WLine) aWLine1(Handle(IntPatch_WLine)::DownCast(theSlin.Value(aNbLin)));

      const IntSurf_PntOn2S& aPntFWL1 = aWLine1->Point(1);
      const IntSurf_PntOn2S& aPntLWL1 = aWLine1->Point(aWLine1->NbPnts());

      const IntSurf_PntOn2S aPntCur = theSPnt.Value(aNbPnt).PntOn2S();
      if (aPntCur.IsSame(aPntFWL1, aTol3D) || aPntCur.IsSame(aPntLWL1, aTol3D))
      {
        theSPnt.Remove(aNbPnt);
        aNbPnt--;
        break;
      }
    }
  }

  // Try to add new points in the neighborhood of existing point
  for (Standard_Integer aNbPnt = 1; aNbPnt <= theSPnt.Length(); aNbPnt++)
  {
    // Standard algorithm (implemented above) could not find any
    // continuous curve in neighborhood of aPnt2S (e.g. because
    // this curve is too small; see tests\bugs\modalg_5\bug25292_35 and _36).
    // Here, we will try to find several new points nearer to aPnt2S.

    // The algorithm below tries to find two points in every
    // intervals [u1 - aStepMax, u1] and [u1, u1 + aStepMax]
    // and every new point will be in maximal distance from
    // u1. If these two points exist they will be joined
    // by the intersection curve.

    const IntPatch_Point& aPnt2S = theSPnt.Value(aNbPnt);

    Standard_Real u1, v1, u2, v2;
    aPnt2S.Parameters(u1, v1, u2, v2);

    Handle(IntSurf_LineOn2S) aL2S   = new IntSurf_LineOn2S();
    Handle(IntPatch_WLine)   aWLine = new IntPatch_WLine(aL2S, Standard_False);
    aWLine->SetCreatingWayInfo(IntPatch_WLine::IntPatch_WLImpImp);

    // Define the index of WLine, which lies the point aPnt2S in.
    Standard_Integer anIndex = 0;

    Standard_Real anUf = 0.0, anUl = 0.0, aCurU2 = 0.0;
    if (isReversed)
    {
      anUf   = Max(u2 - aStepMax, aUSurf1f);
      anUl   = Min(u2 + aStepMax, aUSurf1l);
      aCurU2 = u1;
    }
    else
    {
      anUf   = Max(u1 - aStepMax, aUSurf1f);
      anUl   = Min(u1 + aStepMax, aUSurf1l);
      aCurU2 = u2;
    }

    const Standard_Real anUinf = anUf, anUsup = anUl, anUmid = 0.5 * (anUf + anUl);

    {
      // Find the value of anIndex variable.
      Standard_Real aDelta = RealLast();
      for (Standard_Integer i = 0; i < aNbWLines; i++)
      {
        Standard_Real anU2t = 0.0;
        if (!ComputationMethods::CylCylComputeParameters(anUmid, i, anEquationCoeffs, anU2t))
          continue;

        Standard_Real aDU2 = fmod(Abs(anU2t - aCurU2), aPeriod);
        aDU2               = Min(aDU2, Abs(aDU2 - aPeriod));
        if (aDU2 < aDelta)
        {
          aDelta  = aDU2;
          anIndex = i;
        }
      }
    }

    // Bisection method is used in order to find every new point.
    // I.e. if we need to find intersection point in the interval [anUinf, anUmid]
    // we check the point anUC = 0.5*(anUinf+anUmid). If it is an intersection point
    // we try to find another point in the interval [anUinf, anUC] (because we find the point in
    // maximal distance from anUmid). If it is not then we try to find another point in the
    // interval [anUC, anUmid]. Next iterations will be made analogically.
    // When we find intersection point in the interval [anUmid, anUsup] we try to find
    // another point in the interval [anUC, anUsup] if anUC is intersection point and
    // in the interval [anUmid, anUC], otherwise.

    Standard_Real anAddedPar[2] = {isReversed ? u2 : u1, isReversed ? u2 : u1};

    for (Standard_Integer aParID = 0; aParID < 2; aParID++)
    {
      if (aParID == 0)
      {
        anUf = anUinf;
        anUl = anUmid;
      }
      else // if(aParID == 1)
      {
        anUf = anUmid;
        anUl = anUsup;
      }

      Standard_Real &aPar1 = (aParID == 0) ? anUf : anUl, &aPar2 = (aParID == 0) ? anUl : anUf;

      while (Abs(aPar2 - aPar1) > aStepMin)
      {
        Standard_Real    anUC = 0.5 * (anUf + anUl);
        Standard_Real    aU2 = 0.0, aV1 = 0.0, aV2 = 0.0;
        Standard_Boolean isDone = ComputationMethods::CylCylComputeParameters(anUC,
                                                                              anIndex,
                                                                              anEquationCoeffs,
                                                                              aU2,
                                                                              aV1,
                                                                              aV2);

        if (isDone)
        {
          if (Abs(aV1 - aVSurf1f) <= aTol2D)
            aV1 = aVSurf1f;

          if (Abs(aV1 - aVSurf1l) <= aTol2D)
            aV1 = aVSurf1l;

          if (Abs(aV2 - aVSurf2f) <= aTol2D)
            aV2 = aVSurf2f;

          if (Abs(aV2 - aVSurf2l) <= aTol2D)
            aV2 = aVSurf2l;

          isDone = AddPointIntoWL(aQuad1,
                                  aQuad2,
                                  anEquationCoeffs,
                                  isReversed,
                                  Standard_True,
                                  gp_Pnt2d(anUC, aV1),
                                  gp_Pnt2d(aU2, aV2),
                                  aUSurf1f,
                                  aUSurf1l,
                                  aUSurf2f,
                                  aUSurf2l,
                                  aVSurf1f,
                                  aVSurf1l,
                                  aVSurf2f,
                                  aVSurf2l,
                                  aPeriod,
                                  aWLine->Curve(),
                                  anIndex,
                                  aTol3D,
                                  aTol2D,
                                  Standard_False,
                                  Standard_True);
        }

        if (isDone)
        {
          anAddedPar[0] = Min(anAddedPar[0], anUC);
          anAddedPar[1] = Max(anAddedPar[1], anUC);
          aPar2         = anUC;
        }
        else
        {
          aPar1 = anUC;
        }
      }
    }

    // Fill aWLine by additional points
    if (anAddedPar[1] - anAddedPar[0] > aStepMin)
    {
      for (Standard_Integer aParID = 0; aParID < 2; aParID++)
      {
        Standard_Real aU2 = 0.0, aV1 = 0.0, aV2 = 0.0;
        ComputationMethods::CylCylComputeParameters(anAddedPar[aParID],
                                                    anIndex,
                                                    anEquationCoeffs,
                                                    aU2,
                                                    aV1,
                                                    aV2);

        AddPointIntoWL(aQuad1,
                       aQuad2,
                       anEquationCoeffs,
                       isReversed,
                       Standard_True,
                       gp_Pnt2d(anAddedPar[aParID], aV1),
                       gp_Pnt2d(aU2, aV2),
                       aUSurf1f,
                       aUSurf1l,
                       aUSurf2f,
                       aUSurf2l,
                       aVSurf1f,
                       aVSurf1l,
                       aVSurf2f,
                       aVSurf2l,
                       aPeriod,
                       aWLine->Curve(),
                       anIndex,
                       aTol3D,
                       aTol2D,
                       Standard_False,
                       Standard_False);
      }

      SeekAdditionalPoints(aQuad1,
                           aQuad2,
                           aWLine->Curve(),
                           anEquationCoeffs,
                           anIndex,
                           aNbMinPoints,
                           1,
                           aWLine->NbPnts(),
                           aTol2D,
                           aPeriod,
                           isReversed);

      aWLine->ComputeVertexParameters(aTol3D);
      theSlin.Append(aWLine);

      theSPnt.Remove(aNbPnt);
      aNbPnt--;
    }
  }

  return IntPatch_ImpImpIntersection::IntStatus_OK;
}

//=================================================================================================

IntPatch_ImpImpIntersection::IntStatus IntCyCy(const IntSurf_Quadric&    theQuad1,
                                               const IntSurf_Quadric&    theQuad2,
                                               const Standard_Real       theTol3D,
                                               const Standard_Real       theTol2D,
                                               const Bnd_Box2d&          theUVSurf1,
                                               const Bnd_Box2d&          theUVSurf2,
                                               Standard_Boolean&         isTheEmpty,
                                               Standard_Boolean&         isTheSameSurface,
                                               Standard_Boolean&         isTheMultiplePoint,
                                               IntPatch_SequenceOfLine&  theSlin,
                                               IntPatch_SequenceOfPoint& theSPnt)
{
  isTheEmpty         = Standard_True;
  isTheSameSurface   = Standard_False;
  isTheMultiplePoint = Standard_False;
  theSlin.Clear();
  theSPnt.Clear();

  const gp_Cylinder aCyl1 = theQuad1.Cylinder(), aCyl2 = theQuad2.Cylinder();

  IntAna_QuadQuadGeo anInter(aCyl1, aCyl2, theTol3D);

  if (!anInter.IsDone())
  {
    return IntPatch_ImpImpIntersection::IntStatus_Fail;
  }

  if (anInter.TypeInter() != IntAna_NoGeometricSolution)
  {
    if (CyCyAnalyticalIntersect(theQuad1,
                                theQuad2,
                                anInter,
                                theTol3D,
                                isTheEmpty,
                                isTheSameSurface,
                                isTheMultiplePoint,
                                theSlin,
                                theSPnt))
    {
      return IntPatch_ImpImpIntersection::IntStatus_OK;
    }
  }

  // Here, intersection line is not an analytical curve(line, circle, ellipsis etc.)

  Standard_Real aUSBou[2][2], aVSBou[2][2]; // const

  theUVSurf1.Get(aUSBou[0][0], aVSBou[0][0], aUSBou[0][1], aVSBou[0][1]);
  theUVSurf2.Get(aUSBou[1][0], aVSBou[1][0], aUSBou[1][1], aVSBou[1][1]);

  const Standard_Real    aPeriod   = 2.0 * M_PI;
  const Standard_Integer aNbWLines = 2;

  const ComputationMethods::stCoeffsValue anEquationCoeffs1(aCyl1, aCyl2);
  const ComputationMethods::stCoeffsValue anEquationCoeffs2(aCyl2, aCyl1);

  // Boundaries.
  // Intersection result can include two non-connected regions
  //(see WorkWithBoundaries::BoundariesComputing(...) method).
  const Standard_Integer aNbOfBoundaries = 2;
  Bnd_Range              anURange[2][aNbOfBoundaries]; // const

  if (!WorkWithBoundaries::BoundariesComputing(anEquationCoeffs1, aPeriod, anURange[0]))
    return IntPatch_ImpImpIntersection::IntStatus_OK;

  if (!WorkWithBoundaries::BoundariesComputing(anEquationCoeffs2, aPeriod, anURange[1]))
    return IntPatch_ImpImpIntersection::IntStatus_OK;

  // anURange[*] can be in different periodic regions in
  // compare with First-Last surface. E.g. the surface
  // is full cylinder [0, 2*PI] but anURange is [5, 7].
  // Trivial common range computation returns [5, 2*PI] and
  // its summary length is 2*PI-5 == 1.28... only. That is wrong.
  // This problem can be solved by the following
  // algorithm:
  //  1. split anURange[*] by the surface boundary;
  //  2. shift every new range in order to inscribe it
  //       in [Ufirst, Ulast] of cylinder;
  //  3. consider only common ranges between [Ufirst, Ulast]
  //     and new ranges.
  //
  //  In above example, we obtain following:
  //  1. two ranges: [5, 2*PI] and [2*PI, 7];
  //  2. after shifting: [5, 2*PI] and [0, 7-2*PI];
  //  3. Common ranges: ([5, 2*PI] and [0, 2*PI]) == [5, 2*PI],
  //                    ([0, 7-2*PI] and [0, 2*PI]) == [0, 7-2*PI];
  //  4. Their summary length is (2*PI-5)+(7-2*PI-0)==7-5==2 ==> GOOD.

  Standard_Real                    aSumRange[2] = {0.0, 0.0};
  Handle(NCollection_IncAllocator) anAlloc      = new NCollection_IncAllocator;
  for (Standard_Integer aCID = 0; aCID < 2; aCID++)
  {
    anAlloc->Reset(false);
    NCollection_List<Bnd_Range> aListOfRng(anAlloc);

    aListOfRng.Append(anURange[aCID][0]);
    aListOfRng.Append(anURange[aCID][1]);

    const Standard_Real aSplitArr[3] = {aUSBou[aCID][0], aUSBou[aCID][1], 0.0};

    NCollection_List<Bnd_Range>::Iterator anITrRng;
    for (Standard_Integer aSInd = 0; aSInd < 3; aSInd++)
    {
      NCollection_List<Bnd_Range> aLstTemp(aListOfRng);
      aListOfRng.Clear();
      for (anITrRng.Init(aLstTemp); anITrRng.More(); anITrRng.Next())
      {
        Bnd_Range& aRng = anITrRng.ChangeValue();
        aRng.Split(aSplitArr[aSInd], aListOfRng, aPeriod);
      }
    }

    anITrRng.Init(aListOfRng);
    for (; anITrRng.More(); anITrRng.Next())
    {
      Bnd_Range& aCurrRange = anITrRng.ChangeValue();

      Bnd_Range aBoundR;
      aBoundR.Add(aUSBou[aCID][0]);
      aBoundR.Add(aUSBou[aCID][1]);

      if (!InscribeInterval(aUSBou[aCID][0], aUSBou[aCID][1], aCurrRange, theTol2D, aPeriod))
      {
        // If aCurrRange does not have common block with
        //[Ufirst, Ulast] of cylinder then we will try
        // to inscribe [Ufirst, Ulast] in the boundaries of aCurrRange.
        Standard_Real aF = 1.0, aL = 0.0;
        if (!aCurrRange.GetBounds(aF, aL))
          continue;

        if ((aL < aUSBou[aCID][0]))
        {
          aCurrRange.Shift(aPeriod);
        }
        else if (aF > aUSBou[aCID][1])
        {
          aCurrRange.Shift(-aPeriod);
        }
      }

      aBoundR.Common(aCurrRange);

      const Standard_Real aDelta = aBoundR.Delta();

      if (aDelta > 0.0)
      {
        aSumRange[aCID] += aDelta;
      }
    }
  }

  // The bigger range the bigger number of points in Walking-line (WLine)
  // we will be able to add and consequently we will obtain more
  // precise intersection line.
  // Every point of WLine is determined as function from U1-parameter,
  // where U1 is U-parameter on 1st quadric.
  // Therefore, we should use quadric with bigger range as 1st parameter
  // in IntCyCy() function.
  // On the other hand, there is no point in reversing in case of
  // analytical intersection (when result is line, ellipse, point...).
  // This result is independent of the arguments order.
  const Standard_Boolean isToReverse = (aSumRange[1] > aSumRange[0]);

  if (isToReverse)
  {
    const WorkWithBoundaries aBoundWork(theQuad2,
                                        theQuad1,
                                        anEquationCoeffs2,
                                        theUVSurf2,
                                        theUVSurf1,
                                        aNbWLines,
                                        aPeriod,
                                        theTol3D,
                                        theTol2D,
                                        Standard_True);

    return CyCyNoGeometric(aCyl2,
                           aCyl1,
                           aBoundWork,
                           anURange[1],
                           aNbOfBoundaries,
                           isTheEmpty,
                           theSlin,
                           theSPnt);
  }
  else
  {
    const WorkWithBoundaries aBoundWork(theQuad1,
                                        theQuad2,
                                        anEquationCoeffs1,
                                        theUVSurf1,
                                        theUVSurf2,
                                        aNbWLines,
                                        aPeriod,
                                        theTol3D,
                                        theTol2D,
                                        Standard_False);

    return CyCyNoGeometric(aCyl1,
                           aCyl2,
                           aBoundWork,
                           anURange[0],
                           aNbOfBoundaries,
                           isTheEmpty,
                           theSlin,
                           theSPnt);
  }
}

//=================================================================================================

Standard_Boolean IntCySp(const IntSurf_Quadric&    Quad1,
                         const IntSurf_Quadric&    Quad2,
                         const Standard_Real       Tol,
                         const Standard_Boolean    Reversed,
                         Standard_Boolean&         Empty,
                         Standard_Boolean&         Multpoint,
                         IntPatch_SequenceOfLine&  slin,
                         IntPatch_SequenceOfPoint& spnt)

{
  Standard_Integer i;

  IntSurf_TypeTrans trans1, trans2;
  IntAna_ResultType typint;
  IntPatch_Point    ptsol;
  gp_Circ           cirsol;

  gp_Cylinder Cy;
  gp_Sphere   Sp;

  if (!Reversed)
  {
    Cy = Quad1.Cylinder();
    Sp = Quad2.Sphere();
  }
  else
  {
    Cy = Quad2.Cylinder();
    Sp = Quad1.Sphere();
  }
  IntAna_QuadQuadGeo inter(Cy, Sp, Tol);

  if (!inter.IsDone())
  {
    return Standard_False;
  }

  typint                 = inter.TypeInter();
  Standard_Integer NbSol = inter.NbSolutions();
  Empty                  = Standard_False;

  switch (typint)
  {
    case IntAna_Empty: {
      Empty = Standard_True;
    }
    break;

    case IntAna_Point: {
      gp_Pnt        psol(inter.Point(1));
      Standard_Real U1, V1, U2, V2;
      Quad1.Parameters(psol, U1, V1);
      Quad2.Parameters(psol, U2, V2);
      ptsol.SetValue(psol, Tol, Standard_True);
      ptsol.SetParameters(U1, V1, U2, V2);
      spnt.Append(ptsol);
    }
    break;

    case IntAna_Circle: {
      cirsol = inter.Circle(1);
      gp_Vec Tgt;
      gp_Pnt ptref;
      ElCLib::D1(0., cirsol, ptref, Tgt);

      if (NbSol == 1)
      {
        gp_Vec TestCurvature(ptref, Sp.Location());
        gp_Vec Normsp, Normcyl;
        if (!Reversed)
        {
          Normcyl = Quad1.Normale(ptref);
          Normsp  = Quad2.Normale(ptref);
        }
        else
        {
          Normcyl = Quad2.Normale(ptref);
          Normsp  = Quad1.Normale(ptref);
        }

        IntSurf_Situation situcyl;
        IntSurf_Situation situsp;

        if (Normcyl.Dot(TestCurvature) > 0.)
        {
          situsp = IntSurf_Outside;
          if (Normsp.Dot(Normcyl) > 0.)
          {
            situcyl = IntSurf_Inside;
          }
          else
          {
            situcyl = IntSurf_Outside;
          }
        }
        else
        {
          situsp = IntSurf_Inside;
          if (Normsp.Dot(Normcyl) > 0.)
          {
            situcyl = IntSurf_Outside;
          }
          else
          {
            situcyl = IntSurf_Inside;
          }
        }
        Handle(IntPatch_GLine) glig;
        if (!Reversed)
        {
          glig = new IntPatch_GLine(cirsol, Standard_True, situcyl, situsp);
        }
        else
        {
          glig = new IntPatch_GLine(cirsol, Standard_True, situsp, situcyl);
        }
        slin.Append(glig);
      }
      else
      {
        if (Tgt.DotCross(Quad2.Normale(ptref), Quad1.Normale(ptref)) > 0.0)
        {
          trans1 = IntSurf_Out;
          trans2 = IntSurf_In;
        }
        else
        {
          trans1 = IntSurf_In;
          trans2 = IntSurf_Out;
        }
        Handle(IntPatch_GLine) glig = new IntPatch_GLine(cirsol, Standard_False, trans1, trans2);
        slin.Append(glig);

        cirsol = inter.Circle(2);
        ElCLib::D1(0., cirsol, ptref, Tgt);
        Standard_Real qwe = Tgt.DotCross(Quad2.Normale(ptref), Quad1.Normale(ptref));
        if (qwe > 0.0000001)
        {
          trans1 = IntSurf_Out;
          trans2 = IntSurf_In;
        }
        else if (qwe < -0.0000001)
        {
          trans1 = IntSurf_In;
          trans2 = IntSurf_Out;
        }
        else
        {
          trans1 = trans2 = IntSurf_Undecided;
        }
        glig = new IntPatch_GLine(cirsol, Standard_False, trans1, trans2);
        slin.Append(glig);
      }
    }
    break;

    case IntAna_NoGeometricSolution: {
      gp_Pnt             psol;
      Standard_Real      U1, V1, U2, V2;
      IntAna_IntQuadQuad anaint(Cy, Sp, Tol);
      if (!anaint.IsDone())
      {
        return Standard_False;
      }

      if (anaint.NbPnt() == 0 && anaint.NbCurve() == 0)
      {
        Empty = Standard_True;
      }
      else
      {
        NbSol = anaint.NbPnt();
        for (i = 1; i <= NbSol; i++)
        {
          psol = anaint.Point(i);
          Quad1.Parameters(psol, U1, V1);
          Quad2.Parameters(psol, U2, V2);
          ptsol.SetValue(psol, Tol, Standard_True);
          ptsol.SetParameters(U1, V1, U2, V2);
          spnt.Append(ptsol);
        }

        gp_Pnt           ptvalid, ptf, ptl;
        gp_Vec           tgvalid;
        Standard_Real    first, last, para;
        IntAna_Curve     curvsol;
        Standard_Boolean tgfound;
        Standard_Integer kount;

        NbSol = anaint.NbCurve();
        for (i = 1; i <= NbSol; i++)
        {
          curvsol = anaint.Curve(i);
          curvsol.Domain(first, last);
          ptf = curvsol.Value(first);
          ptl = curvsol.Value(last);

          para    = last;
          kount   = 1;
          tgfound = Standard_False;

          while (!tgfound)
          {
            para    = (1.123 * first + para) / 2.123;
            tgfound = curvsol.D1u(para, ptvalid, tgvalid);
            if (!tgfound)
            {
              kount++;
              tgfound = kount > 5;
            }
          }
          Handle(IntPatch_ALine) alig;
          if (kount <= 5)
          {
            Standard_Real qwe = tgvalid.DotCross(Quad2.Normale(ptvalid), Quad1.Normale(ptvalid));
            if (qwe > 0.00000001)
            {
              trans1 = IntSurf_Out;
              trans2 = IntSurf_In;
            }
            else if (qwe < -0.00000001)
            {
              trans1 = IntSurf_In;
              trans2 = IntSurf_Out;
            }
            else
            {
              trans1 = trans2 = IntSurf_Undecided;
            }
            alig = new IntPatch_ALine(curvsol, Standard_False, trans1, trans2);
          }
          else
          {
            alig = new IntPatch_ALine(curvsol, Standard_False);
          }
          Standard_Boolean TempFalse1a = Standard_False;
          Standard_Boolean TempFalse2a = Standard_False;

          //-- ptf et ptl : points debut et fin de alig

          ProcessBounds(alig,
                        slin,
                        Quad1,
                        Quad2,
                        TempFalse1a,
                        ptf,
                        first,
                        TempFalse2a,
                        ptl,
                        last,
                        Multpoint,
                        Tol);
          slin.Append(alig);
        } //-- boucle sur les lignes
      } //-- solution avec au moins une lihne
    }
    break;

    default: {
      return Standard_False;
    }
  }
  return Standard_True;
}

//=================================================================================================

Standard_Boolean IntCyCo(const IntSurf_Quadric&    Quad1,
                         const IntSurf_Quadric&    Quad2,
                         const Standard_Real       Tol,
                         const Standard_Boolean    Reversed,
                         Standard_Boolean&         Empty,
                         Standard_Boolean&         Multpoint,
                         IntPatch_SequenceOfLine&  slin,
                         IntPatch_SequenceOfPoint& spnt)

{
  IntPatch_Point ptsol;

  Standard_Integer i;

  IntSurf_TypeTrans trans1, trans2;
  IntAna_ResultType typint;
  gp_Circ           cirsol;

  gp_Cylinder Cy;
  gp_Cone     Co;

  if (!Reversed)
  {
    Cy = Quad1.Cylinder();
    Co = Quad2.Cone();
  }
  else
  {
    Cy = Quad2.Cylinder();
    Co = Quad1.Cone();
  }
  IntAna_QuadQuadGeo inter(Cy, Co, Tol);

  if (!inter.IsDone())
  {
    return Standard_False;
  }

  typint                 = inter.TypeInter();
  Standard_Integer NbSol = inter.NbSolutions();
  Empty                  = Standard_False;

  switch (typint)
  {
    case IntAna_Empty: {
      Empty = Standard_True;
    }
    break;

    case IntAna_Point: {
      gp_Pnt        psol(inter.Point(1));
      Standard_Real U1, V1, U2, V2;
      Quad1.Parameters(psol, U1, V1);
      Quad1.Parameters(psol, U2, V2);
      ptsol.SetValue(psol, Tol, Standard_True);
      ptsol.SetParameters(U1, V1, U2, V2);
      spnt.Append(ptsol);
    }
    break;

    case IntAna_Circle: {
      gp_Vec           Tgt;
      gp_Pnt           ptref;
      Standard_Integer j;
      Standard_Real    qwe;
      //
      for (j = 1; j <= 2; ++j)
      {
        cirsol = inter.Circle(j);
        ElCLib::D1(0., cirsol, ptref, Tgt);
        qwe = Tgt.DotCross(Quad2.Normale(ptref), Quad1.Normale(ptref));
        if (qwe > 0.00000001)
        {
          trans1 = IntSurf_Out;
          trans2 = IntSurf_In;
        }
        else if (qwe < -0.00000001)
        {
          trans1 = IntSurf_In;
          trans2 = IntSurf_Out;
        }
        else
        {
          trans1 = trans2 = IntSurf_Undecided;
        }
        Handle(IntPatch_GLine) glig = new IntPatch_GLine(cirsol, Standard_False, trans1, trans2);
        slin.Append(glig);
      }
    }
    break;

    case IntAna_NoGeometricSolution: {
      gp_Pnt             psol;
      Standard_Real      U1, V1, U2, V2;
      IntAna_IntQuadQuad anaint(Cy, Co, Tol);
      if (!anaint.IsDone())
      {
        return Standard_False;
      }

      if (anaint.NbPnt() == 0 && anaint.NbCurve() == 0)
      {
        Empty = Standard_True;
      }
      else
      {
        NbSol = anaint.NbPnt();
        for (i = 1; i <= NbSol; i++)
        {
          psol = anaint.Point(i);
          Quad1.Parameters(psol, U1, V1);
          Quad2.Parameters(psol, U2, V2);
          ptsol.SetValue(psol, Tol, Standard_True);
          ptsol.SetParameters(U1, V1, U2, V2);
          spnt.Append(ptsol);
        }

        gp_Pnt ptvalid, ptf, ptl;
        gp_Vec tgvalid;
        //
        Standard_Real    first, last, para;
        Standard_Boolean tgfound, firstp, lastp, kept;
        Standard_Integer kount;
        //
        //
        // IntAna_Curve curvsol;
        IntAna_Curve                     aC;
        IntAna_ListOfCurve               aLC;
        IntAna_ListIteratorOfListOfCurve aIt;

        //
        NbSol = anaint.NbCurve();
        for (i = 1; i <= NbSol; ++i)
        {
          kept = Standard_False;
          // curvsol = anaint.Curve(i);
          aC = anaint.Curve(i);
          aLC.Clear();
          ExploreCurve(Co, aC, 10. * Tol, aLC);
          //
          aIt.Initialize(aLC);
          for (; aIt.More(); aIt.Next())
          {
            IntAna_Curve& curvsol = aIt.ChangeValue();
            //
            curvsol.Domain(first, last);
            firstp = !curvsol.IsFirstOpen();
            lastp  = !curvsol.IsLastOpen();
            if (firstp)
            {
              ptf = curvsol.Value(first);
            }
            if (lastp)
            {
              ptl = curvsol.Value(last);
            }
            para    = last;
            kount   = 1;
            tgfound = Standard_False;

            while (!tgfound)
            {
              para    = (1.123 * first + para) / 2.123;
              tgfound = curvsol.D1u(para, ptvalid, tgvalid);
              if (!tgfound)
              {
                kount++;
                tgfound = kount > 5;
              }
            }
            Handle(IntPatch_ALine) alig;
            if (kount <= 5)
            {
              Standard_Real qwe = tgvalid.DotCross(Quad2.Normale(ptvalid), Quad1.Normale(ptvalid));
              if (qwe > 0.00000001)
              {
                trans1 = IntSurf_Out;
                trans2 = IntSurf_In;
              }
              else if (qwe < -0.00000001)
              {
                trans1 = IntSurf_In;
                trans2 = IntSurf_Out;
              }
              else
              {
                trans1 = trans2 = IntSurf_Undecided;
              }
              alig = new IntPatch_ALine(curvsol, Standard_False, trans1, trans2);
              kept = Standard_True;
            }
            else
            {
              ptvalid = curvsol.Value(para);
              alig    = new IntPatch_ALine(curvsol, Standard_False);
              kept    = Standard_True;
              //-- std::cout << "Transition indeterminee" << std::endl;
            }
            if (kept)
            {
              Standard_Boolean Nfirstp = !firstp;
              Standard_Boolean Nlastp  = !lastp;
              ProcessBounds(alig,
                            slin,
                            Quad1,
                            Quad2,
                            Nfirstp,
                            ptf,
                            first,
                            Nlastp,
                            ptl,
                            last,
                            Multpoint,
                            Tol);
              slin.Append(alig);
            }
          } // for (; aIt.More(); aIt.Next())
        } // for (i = 1; i <= NbSol; ++i)
      }
    }
    break;

    default:
      return Standard_False;

  } // switch (typint)

  return Standard_True;
}

//=======================================================================
// function : ExploreCurve
// purpose  : Splits aC on several curves in the cone apex points.
//=======================================================================
Standard_Boolean ExploreCurve(const gp_Cone&      theCo,
                              IntAna_Curve&       theCrv,
                              const Standard_Real theTol,
                              IntAna_ListOfCurve& theLC)
{
  const Standard_Real aSqTol = theTol * theTol;
  const gp_Pnt        aPapx(theCo.Apex());

  Standard_Real aT1, aT2;
  theCrv.Domain(aT1, aT2);

  theLC.Clear();
  //
  TColStd_ListOfReal aLParams;
  theCrv.FindParameter(aPapx, aLParams);
  if (aLParams.IsEmpty())
  {
    theLC.Append(theCrv);
    return Standard_False;
  }

  for (TColStd_ListIteratorOfListOfReal anItr(aLParams); anItr.More(); anItr.Next())
  {
    Standard_Real aPrm = anItr.Value();

    if ((aPrm - aT1) < Precision::PConfusion())
      continue;

    Standard_Boolean isLast = Standard_False;
    if ((aT2 - aPrm) < Precision::PConfusion())
    {
      aPrm   = aT2;
      isLast = Standard_True;
    }

    const gp_Pnt        aP   = theCrv.Value(aPrm);
    const Standard_Real aSqD = aP.SquareDistance(aPapx);
    if (aSqD < aSqTol)
    {
      IntAna_Curve aC1 = theCrv;
      aC1.SetDomain(aT1, aPrm);
      aT1 = aPrm;
      theLC.Append(aC1);
    }

    if (isLast)
      break;
  }

  if (theLC.IsEmpty())
  {
    theLC.Append(theCrv);
    return Standard_False;
  }

  if ((aT2 - aT1) > Precision::PConfusion())
  {
    IntAna_Curve aC1 = theCrv;
    aC1.SetDomain(aT1, aT2);
    theLC.Append(aC1);
  }

  return Standard_True;
}

//=================================================================================================

Standard_Boolean IntCoCo(const IntSurf_Quadric&    Quad1,
                         const IntSurf_Quadric&    Quad2,
                         const Standard_Real       Tol,
                         Standard_Boolean&         Empty,
                         Standard_Boolean&         Same,
                         Standard_Boolean&         Multpoint,
                         IntPatch_SequenceOfLine&  slin,
                         IntPatch_SequenceOfPoint& spnt)

{
  Standard_Integer  i, NbSol;
  Standard_Real     U1, V1, U2, V2;
  IntSurf_TypeTrans trans1, trans2;
  IntAna_ResultType typint;
  //
  gp_Cone Co1(Quad1.Cone());
  gp_Cone Co2(Quad2.Cone());
  //
  IntAna_QuadQuadGeo inter(Co1, Co2, Tol);
  if (!inter.IsDone())
  {
    return Standard_False;
  }
  //
  typint = inter.TypeInter();
  NbSol  = inter.NbSolutions();
  Empty  = Standard_False;
  Same   = Standard_False;

  switch (typint)
  {
    case IntAna_Empty: {
      Empty = Standard_True;
    }
    break;

    case IntAna_Same: {
      Same = Standard_True;
    }
    break;

      // modified by NIZNHY-PKV Wed Nov 30 12:56:06 2005f
    case IntAna_Line: {
      Standard_Real          para, aDot;
      gp_Pnt                 aPApex1, aPApex2, ptbid;
      gp_Lin                 linsol;
      gp_Vec                 NormC1, NormC2;
      IntPatch_Point         aPtsol;
      Handle(IntPatch_GLine) glig;
      //
      aPApex1 = Co1.Apex();
      aPApex2 = Co2.Apex();
      //
      if (NbSol == 1)
      {
        IntSurf_Situation situC1, situC2;
        //
        linsol = inter.Line(1);
        para   = ElCLib::Parameter(linsol, aPApex1);
        ptbid  = ElCLib::Value(para + 5., linsol);
        Quad1.Parameters(aPApex1, U1, V1);
        Quad2.Parameters(aPApex1, U2, V2);
        //
        aPtsol.SetValue(aPApex1, Tol, Standard_False);
        aPtsol.SetParameters(U1, V1, U2, V2);
        aPtsol.SetParameter(para);
        //
        NormC1 = Quad1.Normale(ptbid);
        NormC2 = Quad2.Normale(ptbid);
        aDot   = NormC1.Dot(NormC2);
        if (aDot < 0.)
        {
          situC1 = IntSurf_Outside;
          situC2 = IntSurf_Outside;
        }
        else
        {
          Standard_Real aR1, aR2;
          gp_Lin        aLAx1(aPApex1, Co1.Axis().Direction());
          gp_Lin        aLAx2(aPApex2, Co2.Axis().Direction());
          //
          aR1 = aLAx1.Distance(ptbid);
          aR2 = aLAx2.Distance(ptbid);
          //
          situC1 = IntSurf_Inside;
          situC2 = IntSurf_Outside;
          if (aR1 > aR2)
          {                          // Intersection line parametrizes from Apex1 to Apex2,
                                     // clang-format off
            situC1 = IntSurf_Outside; // So the distance between ptbid and aLAx1 is greater than the
                                     // clang-format on
            situC2 = IntSurf_Inside; // distance between ptbid and aLAx2 and in that case Cone2
                                     // is inside Cone 1
          }
        }
        // 1
        glig = new IntPatch_GLine(linsol, Standard_True, situC1, situC2);
        glig->AddVertex(aPtsol);
        glig->SetFirstPoint(1);
        slin.Append(glig);
        // 2
        linsol.SetDirection(linsol.Direction().Reversed());
        para = ElCLib::Parameter(linsol, aPApex1);
        aPtsol.SetParameter(para);

        glig = new IntPatch_GLine(linsol, Standard_True, situC2, situC1);
        glig->AddVertex(aPtsol);
        glig->SetFirstPoint(1);
        slin.Append(glig);
      } // if (NbSol==1) {
      //////////////////////
      else if (NbSol == 2)
      {
        //
        for (i = 1; i <= 2; ++i)
        {
          linsol = inter.Line(i);
          para   = ElCLib::Parameter(linsol, aPApex1);
          ptbid  = ElCLib::Value(para + 5., linsol);
          Quad1.Parameters(aPApex1, U1, V1);
          Quad2.Parameters(aPApex1, U2, V2);
          //
          trans1 = IntSurf_In;
          trans2 = IntSurf_Out;
          if (linsol.Direction().DotCross(Quad2.Normale(ptbid), Quad1.Normale(ptbid)) > 0.)
          {
            trans1 = IntSurf_Out;
            trans2 = IntSurf_In;
          }
          //
          Multpoint = Standard_True;
          // 1,3
          aPtsol.SetValue(aPApex1, Tol, Standard_False);
          aPtsol.SetParameters(U1, V1, U2, V2);
          aPtsol.SetParameter(para);
          aPtsol.SetMultiple(Standard_True);

          glig = new IntPatch_GLine(linsol, Standard_False, trans1, trans2);
          glig->AddVertex(aPtsol);
          glig->SetFirstPoint(1);
          slin.Append(glig);
          // 2,4
          linsol.SetDirection(linsol.Direction().Reversed());
          para = ElCLib::Parameter(linsol, aPApex1);
          aPtsol.SetParameter(para);
          glig = new IntPatch_GLine(linsol, Standard_False, trans1, trans2);
          glig->AddVertex(aPtsol);
          glig->SetFirstPoint(1);
          slin.Append(glig);
          //
        } // for (i=1; i<=2; ++i)
      } // else if (NbSol==2)
    }
    break;
      // modified by NIZNHY-PKV Wed Nov 30 12:56:10 2005t

    case IntAna_Point: {
      gp_Pnt        ptcontact;
      gp_Pnt        apex1(Co1.Apex());
      gp_Pnt        apex2(Co2.Apex());
      Standard_Real param1, param2;
      Standard_Real paramapex1 = ElCLib::LineParameter(Co1.Axis(), apex1);
      Standard_Real paramapex2 = ElCLib::LineParameter(Co2.Axis(), apex2);
      for (i = 1; i <= NbSol; i++)
      {
        ptcontact = inter.Point(i);
        param1    = ElCLib::LineParameter(Co1.Axis(), ptcontact);
        param2    = ElCLib::LineParameter(Co2.Axis(), ptcontact);

        Quad1.Parameters(ptcontact, U1, V1);
        Quad2.Parameters(ptcontact, U2, V2);

        if (apex1.Distance(ptcontact) <= Tol && apex2.Distance(ptcontact) <= Tol)
        {
          IntPatch_Point ptsol;
          ptsol.SetValue(ptcontact, Tol, Standard_False);
          ptsol.SetParameters(U1, V1, U2, V2);
          spnt.Append(ptsol);
        }
        else if (param1 >= paramapex1 && param2 >= paramapex2)
        {
          IntPatch_Point ptsol;
          ptsol.SetValue(ptcontact, Tol, Standard_True);
          ptsol.SetParameters(U1, V1, U2, V2);
          spnt.Append(ptsol);
        }
      }
    }
    break;

    case IntAna_Circle: {
      IntPatch_Point aPtsol;
      gp_Vec         Tgt;
      gp_Pnt         ptref;
      for (i = 1; i <= NbSol; i++)
      {
        gp_Circ cirsol = inter.Circle(i);
        ElCLib::D1(0., cirsol, ptref, Tgt);
        Standard_Real qwe = Tgt.DotCross(Quad2.Normale(ptref), Quad1.Normale(ptref));
        if (qwe > 0.00000001)
        {
          trans1 = IntSurf_Out;
          trans2 = IntSurf_In;
        }
        else if (qwe < -0.00000001)
        {
          trans1 = IntSurf_In;
          trans2 = IntSurf_Out;
        }
        else
        {
          trans1 = trans2 = IntSurf_Undecided;
        }
        Handle(IntPatch_GLine) glig = new IntPatch_GLine(cirsol, Standard_False, trans1, trans2);
        if (inter.HasCommonGen())
        {
          const gp_Pnt& aPChar = inter.PChar();
          Quad1.Parameters(aPChar, U1, V1);
          Quad2.Parameters(aPChar, U2, V2);
          aPtsol.SetValue(aPChar, Tol, Standard_False);
          aPtsol.SetParameters(U1, V1, U2, V2);
          aPtsol.SetParameter(0.);
          glig->AddVertex(aPtsol);
        }
        slin.Append(glig);
      }
    }
    break;

    case IntAna_Ellipse: {
      IntPatch_Point aPtsol;
      gp_Elips       elipsol = inter.Ellipse(1);

      gp_Vec Tgt;
      gp_Pnt ptref;
      ElCLib::D1(0., elipsol, ptref, Tgt);

      Standard_Real qwe = Tgt.DotCross(Quad2.Normale(ptref), Quad1.Normale(ptref));
      if (qwe > 0.00000001)
      {
        trans1 = IntSurf_Out;
        trans2 = IntSurf_In;
      }
      else if (qwe < -0.00000001)
      {
        trans1 = IntSurf_In;
        trans2 = IntSurf_Out;
      }
      else
      {
        trans1 = trans2 = IntSurf_Undecided;
      }
      Handle(IntPatch_GLine) glig = new IntPatch_GLine(elipsol, Standard_False, trans1, trans2);
      if (inter.HasCommonGen())
      {
        const gp_Pnt& aPChar = inter.PChar();
        Quad1.Parameters(aPChar, U1, V1);
        Quad2.Parameters(aPChar, U2, V2);
        aPtsol.SetValue(aPChar, Tol, Standard_False);
        aPtsol.SetParameters(U1, V1, U2, V2);
        aPtsol.SetParameter(0.);
        glig->AddVertex(aPtsol);
      }
      slin.Append(glig);
    }
    break;

    case IntAna_Hyperbola: {
      IntPatch_Point aPtsol;
      gp_Vec         Tgt;
      gp_Pnt         ptref;
      for (i = 1; i <= 2; i++)
      {
        gp_Hypr hyprsol = inter.Hyperbola(i);
        ElCLib::D1(0., hyprsol, ptref, Tgt);
        Standard_Real qwe = Tgt.DotCross(Quad2.Normale(ptref), Quad1.Normale(ptref));
        if (qwe > 0.00000001)
        {
          trans1 = IntSurf_Out;
          trans2 = IntSurf_In;
        }
        else if (qwe < -0.00000001)
        {
          trans1 = IntSurf_In;
          trans2 = IntSurf_Out;
        }
        else
        {
          trans1 = trans2 = IntSurf_Undecided;
        }
        Handle(IntPatch_GLine) glig = new IntPatch_GLine(hyprsol, Standard_False, trans1, trans2);
        if (inter.HasCommonGen())
        {
          const gp_Pnt& aPChar = inter.PChar();
          Quad1.Parameters(aPChar, U1, V1);
          Quad2.Parameters(aPChar, U2, V2);
          aPtsol.SetValue(aPChar, Tol, Standard_False);
          aPtsol.SetParameters(U1, V1, U2, V2);
          aPtsol.SetParameter(0.);
          glig->AddVertex(aPtsol);
        }
        slin.Append(glig);
      }
    }
    break;

    case IntAna_Parabola: {
      IntPatch_Point aPtsol;
      gp_Parab       parabsol = inter.Parabola(1);

      gp_Vec        Tgtorig(parabsol.YAxis().Direction());
      Standard_Real ptran =
        Tgtorig.DotCross(Quad2.Normale(parabsol.Location()), Quad1.Normale(parabsol.Location()));
      if (ptran > 0.00000001)
      {
        trans1 = IntSurf_Out;
        trans2 = IntSurf_In;
      }
      else if (ptran < -0.00000001)
      {
        trans1 = IntSurf_In;
        trans2 = IntSurf_Out;
      }
      else
      {
        trans1 = trans2 = IntSurf_Undecided;
      }

      Handle(IntPatch_GLine) glig = new IntPatch_GLine(parabsol, Standard_False, trans1, trans2);
      if (inter.HasCommonGen())
      {
        const gp_Pnt& aPChar = inter.PChar();
        Quad1.Parameters(aPChar, U1, V1);
        Quad2.Parameters(aPChar, U2, V2);
        aPtsol.SetValue(aPChar, Tol, Standard_False);
        aPtsol.SetParameters(U1, V1, U2, V2);
        aPtsol.SetParameter(0.);
        glig->AddVertex(aPtsol);
      }
      slin.Append(glig);
    }
    break;

    case IntAna_NoGeometricSolution: {
      gp_Pnt             psol;
      IntAna_IntQuadQuad anaint(Co1, Co2, Tol);
      if (!anaint.IsDone())
      {
        return Standard_False;
      }

      if (anaint.NbPnt() == 0 && anaint.NbCurve() == 0)
      {
        Empty = Standard_True;
      }
      else
      {
        NbSol = anaint.NbPnt();
        for (i = 1; i <= NbSol; i++)
        {
          psol = anaint.Point(i);
          Quad1.Parameters(psol, U1, V1);
          Quad2.Parameters(psol, U2, V2);
          IntPatch_Point ptsol;
          ptsol.SetValue(psol, Tol, Standard_True);
          ptsol.SetParameters(U1, V1, U2, V2);
          spnt.Append(ptsol);
        }

        gp_Pnt ptvalid, ptf, ptl;
        gp_Vec tgvalid;

        Standard_Real    first, last, para;
        Standard_Boolean tgfound, firstp, lastp, kept;
        Standard_Integer kount;

        NbSol = anaint.NbCurve();
        for (i = 1; i <= NbSol; i++)
        {
          Handle(IntPatch_ALine) alig;
          kept                 = Standard_False;
          IntAna_Curve curvsol = anaint.Curve(i);
          curvsol.Domain(first, last);
          firstp = !curvsol.IsFirstOpen();
          lastp  = !curvsol.IsLastOpen();
          if (firstp)
          {
            ptf = curvsol.Value(first);
          }
          if (lastp)
          {
            ptl = curvsol.Value(last);
          }
          para    = last;
          kount   = 1;
          tgfound = Standard_False;

          while (!tgfound)
          {
            para    = (1.123 * first + para) / 2.123;
            tgfound = curvsol.D1u(para, ptvalid, tgvalid);
            if (tgvalid.SquareMagnitude() < 1e-14)
            {
              //-- on se trouve ds un cas ou les normales n'auront pas de sens
              tgfound = Standard_False;
            }

            if (!tgfound)
            {
              kount++;
              tgfound = kount > 5;
            }
          }
          if (kount <= 5)
          {
            Standard_Real qwe = tgvalid.DotCross(Quad2.Normale(ptvalid), Quad1.Normale(ptvalid));
            if (qwe > 0.000000001)
            {
              trans1 = IntSurf_Out;
              trans2 = IntSurf_In;
            }
            else if (qwe < -0.000000001)
            {
              trans1 = IntSurf_In;
              trans2 = IntSurf_Out;
            }
            else
            {
              trans1 = trans2 = IntSurf_Undecided;
            }
            alig = new IntPatch_ALine(curvsol, Standard_False, trans1, trans2);
            kept = Standard_True;
          }
          else
          {
            ptvalid = curvsol.Value(para);
            alig    = new IntPatch_ALine(curvsol, Standard_False);
            kept    = Standard_True;
            //-- cout << "Transition indeterminee" << endl;
          }
          if (kept)
          {
            Standard_Boolean Nfirstp = !firstp;
            Standard_Boolean Nlastp  = !lastp;
            ProcessBounds(alig,
                          slin,
                          Quad1,
                          Quad2,
                          Nfirstp,
                          ptf,
                          first,
                          Nlastp,
                          ptl,
                          last,
                          Multpoint,
                          Tol);
            slin.Append(alig);
          }
        }
      }
    }
    break;

    default: {
      return Standard_False;
    }
  }

  // When two cones have common generatrix passing through apexes
  // it is necessary to add it is solution
  if (inter.HasCommonGen())
  {
    Standard_Real  para;
    IntPatch_Point aPtsol;
    gp_Pnt         aPApex1, aPApex2;
    aPApex1 = Co1.Apex();
    aPApex2 = Co2.Apex();
    // common generatrix of cones
    gce_MakeLin            aMkLin(aPApex1, aPApex2);
    const gp_Lin&          linsol = aMkLin.Value();
    Handle(IntPatch_GLine) glig =
      new IntPatch_GLine(linsol, Standard_True, IntSurf_Undecided, IntSurf_Undecided);

    const gp_Pnt& aPChar = inter.PChar();
    Quad1.Parameters(aPChar, U1, V1);
    Quad2.Parameters(aPChar, U2, V2);
    aPtsol.SetValue(aPChar, Tol, Standard_False);
    aPtsol.SetParameters(U1, V1, U2, V2);
    para = ElCLib::Parameter(linsol, aPChar);
    aPtsol.SetParameter(para);
    glig->AddVertex(aPtsol);

    slin.Append(glig);
  }

  return Standard_True;
}

//=================================================================================================

Standard_Boolean IntCoSp(const IntSurf_Quadric&    Quad1,
                         const IntSurf_Quadric&    Quad2,
                         const Standard_Real       Tol,
                         const Standard_Boolean    Reversed,
                         Standard_Boolean&         Empty,
                         Standard_Boolean&         Multpoint,
                         IntPatch_SequenceOfLine&  slin,
                         IntPatch_SequenceOfPoint& spnt)

{
  Standard_Integer i;

  IntSurf_TypeTrans trans1, trans2;
  IntAna_ResultType typint;

  gp_Sphere     Sp;
  gp_Cone       Co;
  Standard_Real U1, V1, U2, V2;

  if (!Reversed)
  {
    Co = Quad1.Cone();
    Sp = Quad2.Sphere();
  }
  else
  {
    Co = Quad2.Cone();
    Sp = Quad1.Sphere();
  }
  IntAna_QuadQuadGeo inter(Sp, Co, Tol);

  if (!inter.IsDone())
  {
    return Standard_False;
  }

  typint                 = inter.TypeInter();
  Standard_Integer NbSol = inter.NbSolutions();
  Empty                  = Standard_False;

  switch (typint)
  {
    case IntAna_Empty: {
      Empty = Standard_True;
    }
    break;

    case IntAna_Point: {
      gp_Pnt        ptcontact;
      gp_Pnt        apex(Co.Apex());
      Standard_Real param;
      Standard_Real paramapex = ElCLib::LineParameter(Co.Axis(), apex);
      for (i = 1; i <= NbSol; i++)
      {
        ptcontact = inter.Point(i);
        param     = ElCLib::LineParameter(Co.Axis(), ptcontact);
        Quad1.Parameters(ptcontact, U1, V1);
        Quad2.Parameters(ptcontact, U2, V2);

        if (apex.Distance(ptcontact) <= Tol)
        {
          IntPatch_Point ptsol;
          ptsol.SetValue(ptcontact, Tol, Standard_False);
          ptsol.SetParameters(U1, V1, U2, V2);
          spnt.Append(ptsol);
        }
        else if (param >= paramapex)
        {
          IntPatch_Point ptsol;
          ptsol.SetValue(ptcontact, Tol, Standard_True);
          ptsol.SetParameters(U1, V1, U2, V2);
          spnt.Append(ptsol);
        }
      }
    }
    break;

    case IntAna_Circle: {
      gp_Vec Tgt;
      gp_Pnt ptref;

      for (i = 1; i <= NbSol; i++)
      {
        gp_Circ cirsol = inter.Circle(i);
        //-- param = ElCLib::LineParameter(Co.Axis(),
        //-- 			      cirsol.Location());
        //-- if (param >= paramapex) {

        ElCLib::D1(0., cirsol, ptref, Tgt);
        Standard_Real qwe = Tgt.DotCross(Quad2.Normale(ptref), Quad1.Normale(ptref));
        if (qwe > 0.00000001)
        {
          trans1 = IntSurf_Out;
          trans2 = IntSurf_In;
        }
        else if (qwe < -0.00000001)
        {
          trans1 = IntSurf_In;
          trans2 = IntSurf_Out;
        }
        else
        {
          trans1 = trans2 = IntSurf_Undecided;
        }
        Handle(IntPatch_GLine) glig = new IntPatch_GLine(cirsol, Standard_False, trans1, trans2);
        slin.Append(glig);
        //-- }
      }
    }
    break;

    case IntAna_PointAndCircle: {
      gp_Vec        Tgt;
      gp_Pnt        ptref;
      gp_Pnt        apex(Co.Apex());
      Standard_Real param;
      Standard_Real paramapex = ElCLib::LineParameter(Co.Axis(), apex);

      // le point est necessairement l apex
      Quad1.Parameters(apex, U1, V1);
      Quad2.Parameters(apex, U2, V2);
      IntPatch_Point ptsol;
      ptsol.SetValue(apex, Tol, Standard_False);
      ptsol.SetParameters(U1, V1, U2, V2);
      spnt.Append(ptsol);

      gp_Circ cirsol = inter.Circle(1);
      param          = ElCLib::LineParameter(Co.Axis(), cirsol.Location());
      ElCLib::D1(0., cirsol, ptref, Tgt);
      Standard_Real qwe = Tgt.DotCross(Quad2.Normale(ptref), Quad1.Normale(ptref));

      if (param >= paramapex)
      {
        if (qwe > Precision::PConfusion())
        {
          trans1 = IntSurf_Out;
          trans2 = IntSurf_In;
        }
        else if (qwe < -Precision::PConfusion())
        {
          trans1 = IntSurf_In;
          trans2 = IntSurf_Out;
        }
        else
        {
          trans1 = trans2 = IntSurf_Undecided;
        }
      }
      else
      {
        if (qwe < -Precision::PConfusion())
        {
          trans1 = IntSurf_Out;
          trans2 = IntSurf_In;
        }
        else if (qwe > Precision::PConfusion())
        {
          trans1 = IntSurf_In;
          trans2 = IntSurf_Out;
        }
        else
        {
          trans1 = trans2 = IntSurf_Undecided;
        }
      }
      Handle(IntPatch_GLine) glig = new IntPatch_GLine(cirsol, Standard_False, trans1, trans2);
      slin.Append(glig);
    }
    break;

    case IntAna_NoGeometricSolution: {
      gp_Pnt             psol;
      IntAna_IntQuadQuad anaint(Co, Sp, Tol);
      if (!anaint.IsDone())
      {
        return Standard_False;
      }

      if (anaint.NbPnt() == 0 && anaint.NbCurve() == 0)
      {
        Empty = Standard_True;
      }
      else
      {
        NbSol = anaint.NbPnt();
        for (i = 1; i <= NbSol; i++)
        {
          psol = anaint.Point(i);
          Quad1.Parameters(psol, U1, V1);
          Quad2.Parameters(psol, U2, V2);
          IntPatch_Point ptsol;
          ptsol.SetValue(psol, Tol, Standard_True);
          ptsol.SetParameters(U1, V1, U2, V2);
          spnt.Append(ptsol);
        }

        gp_Pnt           ptvalid, ptf, ptl;
        gp_Vec           tgvalid;
        Standard_Real    first, last, para;
        Standard_Boolean tgfound, firstp, lastp, kept;
        Standard_Integer kount;

        NbSol = anaint.NbCurve();
        for (i = 1; i <= NbSol; i++)
        {
          Handle(IntPatch_ALine) alig;
          kept                 = Standard_False;
          IntAna_Curve curvsol = anaint.Curve(i);
          curvsol.Domain(first, last);
          firstp = !curvsol.IsFirstOpen();
          lastp  = !curvsol.IsLastOpen();
          if (firstp)
          {
            ptf = curvsol.Value(first);
          }
          if (lastp)
          {
            ptl = curvsol.Value(last);
          }
          para    = last;
          kount   = 1;
          tgfound = Standard_False;

          while (!tgfound)
          {
            para    = (1.123 * first + para) / 2.123;
            tgfound = curvsol.D1u(para, ptvalid, tgvalid);
            if (!tgfound)
            {
              kount++;
              tgfound = kount > 5;
            }
          }
          if (kount <= 5)
          {
            para              = ElCLib::LineParameter(Co.Axis(), ptvalid);
            Standard_Real qwe = tgvalid.DotCross(Quad2.Normale(ptvalid), Quad1.Normale(ptvalid));
            if (qwe > 0.000000001)
            {
              trans1 = IntSurf_Out;
              trans2 = IntSurf_In;
            }
            else if (qwe < -0.000000001)
            {
              trans1 = IntSurf_In;
              trans2 = IntSurf_Out;
            }
            else
            {
              trans1 = trans2 = IntSurf_Undecided;
            }
            alig = new IntPatch_ALine(curvsol, Standard_False, trans1, trans2);
            kept = Standard_True;
          }
          else
          {
            ptvalid = curvsol.Value(para);
            para    = ElCLib::LineParameter(Co.Axis(), ptvalid);
            alig    = new IntPatch_ALine(curvsol, Standard_False);
            kept    = Standard_True;
            //-- cout << "Transition indeterminee" << endl;
          }
          if (kept)
          {
            Standard_Boolean Nfirstp = !firstp;
            Standard_Boolean Nlastp  = !lastp;
            ProcessBounds(alig,
                          slin,
                          Quad1,
                          Quad2,
                          Nfirstp,
                          ptf,
                          first,
                          Nlastp,
                          ptl,
                          last,
                          Multpoint,
                          Tol);
            slin.Append(alig);
          }
        }
      }
    }
    break;

    default: {
      return Standard_False;
    }
  }

  return Standard_True;
}
//=================================================================================================

Standard_Boolean IntSpSp(const IntSurf_Quadric&    Quad1,
                         const IntSurf_Quadric&    Quad2,
                         const Standard_Real       Tol,
                         Standard_Boolean&         Empty,
                         Standard_Boolean&         Same,
                         IntPatch_SequenceOfLine&  slin,
                         IntPatch_SequenceOfPoint& spnt)

// Traitement du cas Sphere/Sphere

{
  IntSurf_TypeTrans trans1, trans2;
  IntAna_ResultType typint;
  gp_Sphere         sph1(Quad1.Sphere());
  gp_Sphere         sph2(Quad2.Sphere());

  IntAna_QuadQuadGeo inter(sph1, sph2, Tol);
  if (!inter.IsDone())
  {
    return Standard_False;
  }

  typint = inter.TypeInter();
  Empty  = Standard_False;
  Same   = Standard_False;

  switch (typint)
  {
    case IntAna_Empty: {
      Empty = Standard_True;
    }
    break;

    case IntAna_Same: {
      Same = Standard_True;
    }
    break;

    case IntAna_Point: {
      gp_Pnt        psol(inter.Point(1));
      Standard_Real U1, V1, U2, V2;
      Quad1.Parameters(psol, U1, V1);
      Quad2.Parameters(psol, U2, V2);
      IntPatch_Point ptsol;
      ptsol.SetValue(psol, Tol, Standard_True);
      ptsol.SetParameters(U1, V1, U2, V2);
      spnt.Append(ptsol);
    }
    break;

    case IntAna_Circle: {
      gp_Circ cirsol = inter.Circle(1);
      gp_Pnt  ptref;
      gp_Vec  Tgt;
      ElCLib::D1(0., cirsol, ptref, Tgt);

      Standard_Real qwe = Tgt.DotCross(Quad2.Normale(ptref), Quad1.Normale(ptref));
      if (qwe > 0.00000001)
      {
        trans1 = IntSurf_Out;
        trans2 = IntSurf_In;
      }
      else if (qwe < -0.00000001)
      {
        trans1 = IntSurf_In;
        trans2 = IntSurf_Out;
      }
      else
      {
        trans1 = trans2 = IntSurf_Undecided;
      }
      Handle(IntPatch_GLine) glig = new IntPatch_GLine(cirsol, Standard_False, trans1, trans2);
      slin.Append(glig);
    }
    break;

    default: {
      return Standard_False; // on ne doit pas passer ici
    }
  }
  return Standard_True;
}

static Standard_Boolean TreatResultTorus(const IntSurf_Quadric&    theQuad1,
                                         const IntSurf_Quadric&    theQuad2,
                                         const IntAna_QuadQuadGeo& anInt,
                                         Standard_Boolean&         bEmpty,
                                         IntPatch_SequenceOfLine&  theSeqLin);

//=================================================================================================

Standard_Boolean IntCyTo(const IntSurf_Quadric&   theQuad1,
                         const IntSurf_Quadric&   theQuad2,
                         const Standard_Real      theTolTang,
                         const Standard_Boolean   bReversed,
                         Standard_Boolean&        bEmpty,
                         IntPatch_SequenceOfLine& theSeqLin)
{
  const gp_Cylinder aCyl   = bReversed ? theQuad2.Cylinder() : theQuad1.Cylinder();
  const gp_Torus    aTorus = bReversed ? theQuad1.Torus() : theQuad2.Torus();
  //
  IntAna_QuadQuadGeo anInt(aCyl, aTorus, theTolTang);
  Standard_Boolean   bRet = TreatResultTorus(theQuad1, theQuad2, anInt, bEmpty, theSeqLin);
  //
  return bRet;
}

//=================================================================================================

Standard_Boolean IntCoTo(const IntSurf_Quadric&   theQuad1,
                         const IntSurf_Quadric&   theQuad2,
                         const Standard_Real      theTolTang,
                         const Standard_Boolean   bReversed,
                         Standard_Boolean&        bEmpty,
                         IntPatch_SequenceOfLine& theSeqLin)
{
  const gp_Cone  aCone  = bReversed ? theQuad2.Cone() : theQuad1.Cone();
  const gp_Torus aTorus = bReversed ? theQuad1.Torus() : theQuad2.Torus();
  //
  IntAna_QuadQuadGeo anInt(aCone, aTorus, theTolTang);
  Standard_Boolean   bRet = TreatResultTorus(theQuad1, theQuad2, anInt, bEmpty, theSeqLin);
  //
  return bRet;
}

//=================================================================================================

Standard_Boolean IntSpTo(const IntSurf_Quadric&   theQuad1,
                         const IntSurf_Quadric&   theQuad2,
                         const Standard_Real      theTolTang,
                         const Standard_Boolean   bReversed,
                         Standard_Boolean&        bEmpty,
                         IntPatch_SequenceOfLine& theSeqLin)
{
  const gp_Sphere aSphere = bReversed ? theQuad2.Sphere() : theQuad1.Sphere();
  const gp_Torus  aTorus  = bReversed ? theQuad1.Torus() : theQuad2.Torus();
  //
  IntAna_QuadQuadGeo anInt(aSphere, aTorus, theTolTang);
  Standard_Boolean   bRet = TreatResultTorus(theQuad1, theQuad2, anInt, bEmpty, theSeqLin);
  //
  return bRet;
}

//=================================================================================================

Standard_Boolean IntToTo(const IntSurf_Quadric&   theQuad1,
                         const IntSurf_Quadric&   theQuad2,
                         const Standard_Real      theTolTang,
                         Standard_Boolean&        bSameSurf,
                         Standard_Boolean&        bEmpty,
                         IntPatch_SequenceOfLine& theSeqLin)
{
  const gp_Torus aTorus1 = theQuad1.Torus();
  const gp_Torus aTorus2 = theQuad2.Torus();
  //
  IntAna_QuadQuadGeo anInt(aTorus1, aTorus2, theTolTang);
  Standard_Boolean   bRet = anInt.IsDone();
  if (bRet)
  {
    if (anInt.TypeInter() == IntAna_Same)
    {
      bEmpty    = Standard_False;
      bSameSurf = Standard_True;
    }
    else
    {
      bRet = TreatResultTorus(theQuad1, theQuad2, anInt, bEmpty, theSeqLin);
    }
  }
  //
  return bRet;
}

//=================================================================================================

static Standard_Boolean TreatResultTorus(const IntSurf_Quadric&    theQuad1,
                                         const IntSurf_Quadric&    theQuad2,
                                         const IntAna_QuadQuadGeo& anInt,
                                         Standard_Boolean&         bEmpty,
                                         IntPatch_SequenceOfLine&  theSeqLin)
{
  Standard_Boolean bRet = anInt.IsDone();
  //
  if (!bRet)
  {
    return bRet;
  }
  //
  IntAna_ResultType typint = anInt.TypeInter();
  Standard_Integer  NbSol  = anInt.NbSolutions();
  bEmpty                   = Standard_False;
  //
  switch (typint)
  {
    case IntAna_Empty:
      bEmpty = Standard_True;
      break;
    //
    case IntAna_Circle: {
      Standard_Integer  i;
      IntSurf_TypeTrans trans1, trans2;
      gp_Vec            Tgt;
      gp_Pnt            ptref;
      //
      for (i = 1; i <= NbSol; ++i)
      {
        gp_Circ aC = anInt.Circle(i);
        if (theQuad1.TypeQuadric() == theQuad2.TypeQuadric())
        {
          AdjustToSeam(theQuad1.Torus(), aC);
        }
        ElCLib::D1(0., aC, ptref, Tgt);
        Standard_Real qwe = Tgt.DotCross(theQuad2.Normale(ptref), theQuad1.Normale(ptref));
        if (qwe > 0.00000001)
        {
          trans1 = IntSurf_Out;
          trans2 = IntSurf_In;
        }
        else if (qwe < -0.00000001)
        {
          trans1 = IntSurf_In;
          trans2 = IntSurf_Out;
        }
        else
        {
          trans1 = trans2 = IntSurf_Undecided;
        }
        //
        Handle(IntPatch_GLine) glig = new IntPatch_GLine(aC, Standard_False, trans1, trans2);
        theSeqLin.Append(glig);
      }
    }
    break;
    //
    case IntAna_NoGeometricSolution:
    default:
      bRet = Standard_False;
      break;
  }
  //
  return bRet;
}
