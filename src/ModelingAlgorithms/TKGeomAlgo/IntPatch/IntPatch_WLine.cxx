// Created on: 1991-05-27
// Created by: Isabelle GRIGNON
// Copyright (c) 1991-1999 Matra Datavision
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

#include <IntPatch_WLine.hxx>
#include <IntSurf_LineOn2S.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IntPatch_WLine, IntPatch_PointLine)

#define DEBUG 0
#define DEBUGV 0

#include <Precision.hxx>
#include <cstdio>

IntPatch_WLine::IntPatch_WLine(const occ::handle<IntSurf_LineOn2S>& Line,
                               const bool                           Tang,
                               const IntSurf_TypeTrans              Trans1,
                               const IntSurf_TypeTrans              Trans2)
    : IntPatch_PointLine(Tang, Trans1, Trans2),
      fipt(false),
      lapt(false),
      hasArcOnS1(false),
      hasArcOnS2(false),
      myIsPurgerAllowed(true),
      myCreationWay(IntPatch_WLUnknown)
{
  typ      = IntPatch_Walking;
  curv     = Line;
  u1period = v1period = u2period = v2period = 0.0;
}

IntPatch_WLine::IntPatch_WLine(const occ::handle<IntSurf_LineOn2S>& Line,
                               const bool                           Tang,
                               const IntSurf_Situation              Situ1,
                               const IntSurf_Situation              Situ2)
    : IntPatch_PointLine(Tang, Situ1, Situ2),
      fipt(false),
      lapt(false),
      hasArcOnS1(false),
      hasArcOnS2(false),
      myIsPurgerAllowed(true),
      myCreationWay(IntPatch_WLUnknown)
{
  typ      = IntPatch_Walking;
  curv     = Line;
  u1period = v1period = u2period = v2period = 0.0;
}

IntPatch_WLine::IntPatch_WLine(const occ::handle<IntSurf_LineOn2S>& Line, const bool Tang)
    : IntPatch_PointLine(Tang),
      fipt(false),
      lapt(false),
      hasArcOnS1(false),
      hasArcOnS2(false),
      myIsPurgerAllowed(true),
      myCreationWay(IntPatch_WLUnknown)
{
  typ      = IntPatch_Walking;
  curv     = Line;
  u1period = v1period = u2period = v2period = 0.0;
}

void IntPatch_WLine::SetPoint(const int Index, const IntPatch_Point& thepoint)
{
  curv->Value(Index, thepoint.PntOn2S());
}

occ::handle<IntSurf_LineOn2S> IntPatch_WLine::Curve() const
{
  return (curv);
}

static void RecadreMemePeriode(double&      u1,
                               double&      v1,
                               double&      u2,
                               double&      v2,
                               const double anu1,
                               const double anv1,
                               const double anu2,
                               const double anv2,
                               const double U1Period,
                               const double V1Period,
                               const double U2Period,
                               const double V2Period)
{
  if (U1Period)
  {
    while (anu1 - u1 > 0.8 * U1Period)
    {
      u1 += U1Period;
    }
    while (u1 - anu1 > 0.8 * U1Period)
    {
      u1 -= U1Period;
    }
  }
  if (U2Period)
  {
    while (anu2 - u2 > 0.8 * U2Period)
    {
      u2 += U2Period;
    }
    while (u2 - anu2 > 0.8 * U2Period)
    {
      u2 -= U2Period;
    }
  }
  if (V1Period)
  {
    while (anv1 - v1 > 0.8 * V1Period)
    {
      v1 += V1Period;
    }
    while (v1 - anv1 > 0.8 * V1Period)
    {
      v1 -= V1Period;
    }
  }
  if (V2Period)
  {
    while (anv2 - v2 > 0.8 * V2Period)
    {
      v2 += V2Period;
    }
    while (v2 - anv2 > 0.8 * V2Period)
    {
      v2 -= V2Period;
    }
  }
}

static void RecadreMemePeriode(IntSurf_PntOn2S&       POn2S,
                               const IntSurf_PntOn2S& RefPOn2S,
                               const double           up1,
                               const double           vp1,
                               const double           up2,
                               const double           vp2)
{
  double u1, v1, u2, v2, pu1, pv1, pu2, pv2;
  POn2S.Parameters(u1, v1, u2, v2);
  RefPOn2S.Parameters(pu1, pv1, pu2, pv2);
  RecadreMemePeriode(u1, v1, u2, v2, pu1, pv1, pu2, pv2, up1, vp1, up2, vp2);
  POn2S.SetValue(u1, v1, u2, v2);
}

static bool CompareVertexAndPoint(const gp_Pnt& V, const gp_Pnt& P, const double& Tol)
{
  const double aSQDist = V.SquareDistance(P);
  const double aSQTol  = Tol * Tol;
  return (aSQDist <= aSQTol);
}

void IntPatch_WLine::SetPeriod(const double pu1,
                               const double pv1,
                               const double pu2,
                               const double pv2)
{
  u1period = pu1;
  v1period = pv1;
  u2period = pu2;
  v2period = pv2;
}

double IntPatch_WLine::U1Period() const
{
  return (u1period);
}

double IntPatch_WLine::V1Period() const
{
  return (v1period);
}

double IntPatch_WLine::U2Period() const
{
  return (u2period);
}

double IntPatch_WLine::V2Period() const
{
  return (v2period);
}

//------------------------------------------------------------------------
//--  En Entree : Une ligne de cheminement     +    Une Liste de Vetex
//--
//--   LineOn2S   :       1------2-------3-------4-----5---- ----nbp
//--
//--   Vertex     :   a     b c          d    e      f
//--
//--
//--  En Sortie
//--
//--                  1--2-3-4--5--------6----7--8--9--10--------
//--
//--  avec   a de parametre 1
//--         b              3
//--
//--   etc ...
//--
//--
//-- !!!!!!!!!!!!!!! On considere que deux vertex ne peuvent pas etre
//-- !!!!!!!!!!!!!!!  a une distance inferieure a Tol
//------------------------------------------------------------------------
//--
//-- On Teste si la LineOn2S contient des points confondus.
//-- Dans ce cas, on remove ces points.
//--
//------------------------------------------------------------------------

bool SameVtxRst(const IntPatch_Point& vtx1, const IntPatch_Point& vtx2)
{
  if (vtx1.IsOnDomS1())
  {
    if (vtx2.IsOnDomS1())
    {
      if (vtx1.ArcOnS1() == vtx2.ArcOnS1())
      {
        if (vtx1.ParameterOnArc1() == vtx2.ParameterOnArc1())
        {
        }
        else
        {
          return (false);
        }
      }
      else
      {
        return (false);
      }
    }
    else
    {
      return (false);
    }
  }
  else
  {
    if (vtx2.IsOnDomS1())
    {
      return (false);
    }
  }
  if (vtx1.IsOnDomS2())
  {
    if (vtx2.IsOnDomS2())
    {
      if (vtx1.ArcOnS2() == vtx2.ArcOnS2())
      {
        if (vtx1.ParameterOnArc2() == vtx2.ParameterOnArc2())
        {
        }
        else
        {
          return (false);
        }
      }
      else
      {
        return (false);
      }
    }
    else
    {
      return (false);
    }
  }
  else
  {
    if (vtx2.IsOnDomS2())
    {
      return (false);
    }
  }
  return (true);
}

static bool CompareVerticesOnSurf(const IntPatch_Point& vtx1,
                                  const IntPatch_Point& vtx2,
                                  const bool            onFirst)
{
  double u1, v1, u2, v2, tolU, tolV;
  if (onFirst)
  {
    vtx1.ParametersOnS1(u1, v1);
    vtx2.ParametersOnS1(u2, v2);
  }
  else
  {
    vtx1.ParametersOnS2(u1, v1);
    vtx2.ParametersOnS2(u2, v2);
  }
  tolU = Precision::PConfusion();
  tolV = Precision::PConfusion();
  return (std::abs(u1 - u2) <= tolU && std::abs(v1 - v2) <= tolV);
}

inline bool CompareVerticesOnS1(const IntPatch_Point& vtx1, const IntPatch_Point& vtx2)
{
  return CompareVerticesOnSurf(vtx1, vtx2, true);
}

inline bool CompareVerticesOnS2(const IntPatch_Point& vtx1, const IntPatch_Point& vtx2)
{
  return CompareVerticesOnSurf(vtx1, vtx2, false);
}

void IntPatch_WLine::ComputeVertexParameters(const double RTol)
{
  // MSV Oct 15, 2001: use tolerance of vertex instead of RTol where
  //                   it is possible

  int    i, j, k, nbvtx, nbponline;
  int    indicevertexonline;
  double indicevertex;

  bool APointDeleted = false;
  //----------------------------------------------------------
  //--     F i l t r e   s u r   r e s t r i c t i o n s   --
  //----------------------------------------------------------
  //-- deux vertex sur la meme restriction et seulement
  //-- sur celle ci ne doivent pas avoir le meme parametre
  //--
  double Tol = RTol;
  nbvtx      = NbVertex();

#if DEBUGV
  std::cout << "\n----------- avant ComputeVertexParameters -------------" << std::endl;
  for (i = 1; i <= nbvtx; i++)
  {
    Vertex(i).Dump();
    double polr = Vertex(i).ParameterOnLine();
    double pol  = (int)polr;
    if (pol >= 1 && pol <= nbvtx)
    {
      std::cout << "----> IntSurf_PntOn2S : " << polr << "  Pnt (" << Vertex(pol).Value().X() << ","
                << Vertex(pol).Value().Y() << "," << Vertex(pol).Value().Z() << ")" << std::endl;
    }
  }
  std::cout << "\n----------------------------------------------------------" << std::endl;
#endif

  //-- ----------------------------------------------------------------------
  //-- Traitement des aretes de couture : On duplique les points situes
  //-- sur des restrictions differentes
  //--
  //-- Phase Creation de nouveaux points sur S1
  bool encoreunefois;
  do
  {
    nbvtx         = NbVertex();
    encoreunefois = false;
    for (i = 1; i <= nbvtx && !encoreunefois; i++)
    {
      IntPatch_Point& VTXi = svtx.ChangeValue(i);
      for (j = 1; j <= nbvtx && !encoreunefois; j++)
      {
        if (i != j)
        {
          IntPatch_Point& VTXj = svtx.ChangeValue(j);
          if (VTXi.ParameterOnLine() != VTXj.ParameterOnLine())
          {
            double d      = VTXi.Value().Distance(VTXj.Value());
            double toli   = VTXi.Tolerance();
            double tolj   = VTXj.Tolerance();
            double maxtol = std::max(toli, tolj);
            // MSV Oct 30, 2001: compare in 2D space also;
            //                   increase tolerances
            if (d < maxtol || CompareVerticesOnS1(VTXi, VTXj) || CompareVerticesOnS2(VTXi, VTXj))
            {
              //-- Creation Vtx (REF:S1(i)  S2(j))    (On Garde S1(i))
              double newtoli = std::max(toli, tolj + d * 1.01);
              double newtolj = std::max(tolj, toli + d * 1.01);
              bool   acreer  = false;
              if (VTXi.IsOnDomS1())
              {
                if (VTXj.IsOnDomS1())
                {
                  if (VTXj.ArcOnS1() != VTXi.ArcOnS1())
                  {
                    acreer = true;
                  }
                }
                else
                {
                  acreer = true;
                }
              }
              if (acreer)
              {
                IntPatch_Point vtx;
                vtx = VTXj;
                vtx.SetArc(true,
                           VTXi.ArcOnS1(),
                           VTXi.ParameterOnArc1(),
                           VTXi.TransitionLineArc1(),
                           VTXi.TransitionOnS1());
                for (k = 1; !encoreunefois && k <= nbvtx; k++)
                {
                  const IntPatch_Point& VTXk = svtx.Value(k);
                  if (SameVtxRst(VTXk, vtx))
                  {
                    encoreunefois = true;
                  }
                }
                if (!encoreunefois)
                {
                  VTXi.SetTolerance(newtoli);
                  VTXj.SetTolerance(newtolj);
                  vtx.SetTolerance(newtolj);
                  svtx.Append(vtx);
                  encoreunefois = true;
                }
                else
                {
                  encoreunefois = false;
                }
              }
              //-- -----------------------------------------------------
              //-- Creation Vtx (REF:S2(i)  S1(j))    (On Garde S2(i))
              acreer = false;
              if (VTXi.IsOnDomS2())
              {
                if (VTXj.IsOnDomS2())
                {
                  if (VTXj.ArcOnS2() != VTXi.ArcOnS2())
                  {
                    acreer = true;
                  }
                }
                else
                {
                  acreer = true;
                }
              }
              if (acreer)
              {
                IntPatch_Point vtx;
                vtx = VTXj;
                vtx.SetArc(false,
                           VTXi.ArcOnS2(),
                           VTXi.ParameterOnArc2(),
                           VTXi.TransitionLineArc2(),
                           VTXi.TransitionOnS2());
                for (k = 1; !encoreunefois && k <= nbvtx; k++)
                {
                  const IntPatch_Point& VTXk = svtx.Value(k);
                  if (SameVtxRst(VTXk, vtx))
                  {
                    encoreunefois = true;
                  }
                }
                if (!encoreunefois)
                {
                  VTXi.SetTolerance(newtoli);
                  VTXj.SetTolerance(newtolj);
                  vtx.SetTolerance(newtolj);
                  svtx.Append(vtx);
                  encoreunefois = true;
                }
                else
                {
                  encoreunefois = false;
                }
              }
            }
          }
        }
      }
    }
  } while (encoreunefois);

  //-- ----------------------------------------------------------------------

  do
  {
    APointDeleted = false;
    for (i = 1; (i <= nbvtx) && (!APointDeleted); i++)
    {
      const IntPatch_Point& VTXi = svtx.Value(i);
      if (VTXi.Tolerance() > Tol)
        Tol = VTXi.Tolerance(); //-- 9 oct 97
      if ((VTXi.IsOnDomS1()) && (!VTXi.IsOnDomS2()))
      {
        for (j = 1; (j <= nbvtx) && (!APointDeleted); j++)
        {
          if (i != j)
          {
            const IntPatch_Point& VTXj = svtx.Value(j);
            if ((VTXj.IsOnDomS1()) && (!VTXj.IsOnDomS2()))
            {
              if (VTXi.ParameterOnLine() == VTXj.ParameterOnLine())
              {
                if (VTXi.ArcOnS1() == VTXj.ArcOnS1())
                {
                  svtx.Remove(j);
                  nbvtx--;
                  if (lapt)
                  {
                    if (indl >= j)
                      indl--;
                  }
                  if (fipt)
                  {
                    if (indf >= j)
                      indf--;
                  }
                  APointDeleted = true;
                }
              }
            }
          }
        }
      }
    }
  } while (APointDeleted);

  do
  {
    APointDeleted = false;
    for (i = 1; (i <= nbvtx) && (!APointDeleted); i++)
    {
      const IntPatch_Point& VTXi = svtx.Value(i);
      if ((VTXi.IsOnDomS2()) && (!VTXi.IsOnDomS1()))
      {
        for (j = 1; (j <= nbvtx) && (!APointDeleted); j++)
        {
          if (i != j)
          {
            const IntPatch_Point& VTXj = svtx.Value(j);
            if ((VTXj.IsOnDomS2()) && (!VTXj.IsOnDomS1()))
            {
              if (VTXi.ParameterOnLine() == VTXj.ParameterOnLine())
              {
                if (VTXi.ArcOnS2() == VTXj.ArcOnS2())
                {
                  svtx.Remove(j);
                  nbvtx--;
                  if (lapt)
                  {
                    if (indl >= j)
                      indl--;
                  }
                  if (fipt)
                  {
                    if (indf >= j)
                      indf--;
                  }
                  APointDeleted = true;
                }
              }
            }
          }
        }
      }
    }
  } while (APointDeleted);

  nbvtx     = NbVertex();
  nbponline = NbPnts();

  //----------------------------------------------------
  //-- On trie les Vertex
  bool SortIsOK;
  do
  {
    SortIsOK = true;
    for (i = 2; i <= nbvtx; i++)
    {
      if (svtx.Value(i - 1).ParameterOnLine() > svtx.Value(i).ParameterOnLine())
      {
        SortIsOK = false;
        svtx.Exchange(i - 1, i);
      }
    }
  } while (!SortIsOK);

  //----------------------------------------------------
  //-- On detecte les points confondus dans la LineOn2S
  constexpr double dmini = Precision::SquareConfusion();
  for (i = 2; (i <= nbponline) && (nbponline > 2); i++)
  {
    const IntSurf_PntOn2S& aPnt1 = curv->Value(i - 1);
    const IntSurf_PntOn2S& aPnt2 = curv->Value(i);
    double                 d     = (aPnt1.Value()).SquareDistance((aPnt2.Value()));
    if (d < dmini)
    {
      curv->RemovePoint(i);
      nbponline--;
      //----------------------------------------------
      //-- On recadre les Vertex si besoin
      //--
      for (j = 1; j <= nbvtx; j++)
      {
        indicevertex = svtx.Value(j).ParameterOnLine();
        if (indicevertex >= i)
        {
          svtx.ChangeValue(j).SetParameter(indicevertex - 1.0);
        }
      }
      // modified by NIZNHY-PKV Mon Feb 11 09:28:02 2002 f
      i--;
      // modified by NIZNHY-PKV Mon Feb 11 09:28:04 2002 t
    }
  }
  //----------------------------------------------------
  for (i = 1; i <= nbvtx; i++)
  {
    const gp_Pnt& P    = svtx.Value(i).Value();
    double        vTol = svtx.Value(i).Tolerance();

    indicevertex       = svtx.Value(i).ParameterOnLine();
    indicevertexonline = (int)indicevertex;
    //--------------------------------------------------
    //-- On Compare le vertex avec les points de la ligne
    //-- d indice   indicevertexOnLine-1
    //--            indicevertexOnLine
    //--            indicevertexOnLine+1
    //--------------------------------------------------
    if (indicevertexonline < 1)
    {
      if (CompareVertexAndPoint(P, curv->Value(1).Value(), vTol))
      {
        //-------------------------------------------------------
        //-- On remplace le point cheminement(1) par vertex(i)
        //-- et   vertex(i) prend pour parametre 1
        //-------------------------------------------------------

        IntSurf_PntOn2S POn2S = svtx.Value(i).PntOn2S();
        RecadreMemePeriode(POn2S, curv->Value(1), U1Period(), V1Period(), U2Period(), V2Period());
        if (myCreationWay == IntPatch_WLImpImp)
        {
          // Adjust first point of curve to corresponding vertex the following way:
          // set 3D point as the point of the vertex and 2D points as the points of the point on
          // curve.
          curv->SetPoint(1, POn2S.Value());
          double mu1, mv1, mu2, mv2;
          curv->Value(1).Parameters(mu1, mv1, mu2, mv2);
          svtx.ChangeValue(i).SetParameter(1);
          svtx.ChangeValue(i).SetParameters(mu1, mv1, mu2, mv2);
        }
        else
        {
          curv->Value(1, POn2S);
        }

        //--curv->Value(1,svtx.Value(i).PntOn2S());
        svtx.ChangeValue(i).SetParameter(1.0);
      }
      else
      {
        //-------------------------------------------------------
        //-- On insere le point de cheminement Vertex(i)
        //-- On recadre les parametres des autres vertex
        //-------------------------------------------------------
        IntSurf_PntOn2S POn2S = svtx.Value(i).PntOn2S();
        RecadreMemePeriode(POn2S, curv->Value(1), U1Period(), V1Period(), U2Period(), V2Period());
        curv->InsertBefore(1, POn2S);

        //-- curv->InsertBefore(1,svtx.Value(i).PntOn2S());
        svtx.ChangeValue(i).SetParameter(1.0);
        nbponline++;
        for (j = 1; j <= nbvtx; j++)
        {
          if (j != 1)
          {
            double t = svtx.Value(j).ParameterOnLine();
            if (t > 1.0)
            {
              svtx.ChangeValue(j).SetParameter(t + 1.0);
            }
          }
        }
      }
    } //--- fin : if(indicevertexonline<1)
    else
    {
      //---------------------------------------------------------
      //-- vertex(i)   ==   cheminement (indicevertexonline-1)
      //-- vertex(i)   ==   cheminement (indicevertexonline)
      //-- vertex(i)   ==   cheminement (indicevertexonline+1)
      //---------------------------------------------------------
      bool Substitution = false;
      //-- for(k=indicevertexonline+1; !Substitution && k>=indicevertexonline-1;k--) {   avant le 9
      // oct 97
      double mu1, mv1, mu2, mv2;
      curv->Value(indicevertexonline).Parameters(mu1, mv1, mu2, mv2);

      for (k = indicevertexonline + 1; k >= indicevertexonline - 1; k--)
      {
        if (k > 0 && k <= nbponline)
        {
          if (CompareVertexAndPoint(P, curv->Value(k).Value(), vTol))
          {
            //-------------------------------------------------------
            //-- On remplace le point cheminement(k)
            //-- par vertex(i)  et vertex(i) prend pour parametre k
            //-------------------------------------------------------
            IntSurf_PntOn2S POn2S = svtx.Value(i).PntOn2S();
            RecadreMemePeriode(POn2S,
                               curv->Value(k),
                               U1Period(),
                               V1Period(),
                               U2Period(),
                               V2Period());

            if (myCreationWay == IntPatch_WLImpImp)
            {
              // Adjust a point of curve to corresponding vertex the following way:
              // set 3D point as the point of the vertex and 2D points as the points
              // of the point on curve with index <indicevertexonline>
              curv->SetPoint(k, POn2S.Value());
              curv->SetUV(k, true, mu1, mv1);
              curv->SetUV(k, false, mu2, mv2);
            }
            else
            {
              curv->Value(k, POn2S);
              POn2S.Parameters(mu1, mv1, mu2, mv2);
            }
            svtx.ChangeValue(i).SetParameter(k);
            svtx.ChangeValue(i).SetParameters(mu1, mv1, mu2, mv2);
            Substitution = true;
          }
        }
      }

      // Remove duplicating points
      if (Substitution)
      {
        int ind_point;
        for (ind_point = 2; (ind_point <= nbponline && nbponline > 1); ind_point++)
        {
          double d =
            (curv->Value(ind_point - 1).Value()).SquareDistance((curv->Value(ind_point).Value()));
          if (d < dmini)
          {
            curv->RemovePoint(ind_point);
            nbponline--;
            //----------------------------------------------
            //-- On recadre les Vertex si besoin
            //--
            for (j = 1; j <= nbvtx; j++)
            {
              indicevertex = svtx.Value(j).ParameterOnLine();
              if (indicevertex >= ind_point)
              {
                svtx.ChangeValue(j).SetParameter(indicevertex - 1.0);
              }
            }
            // modified by NIZNHY-PKV Mon Feb 11 09:28:02 2002 f
            ind_point--;
            // modified by NIZNHY-PKV Mon Feb 11 09:28:04 2002 t
          }
        }
      }

      //--static int deb6nov98=1;    Ne resout rien (a part partiellement BUC60409)
      //--if(deb6nov98) {
      //--Substitution=true;
      //-- }

      if (!Substitution)
      {
        //-------------------------------------------------------
        //-- On insere le point de cheminement Vertex(i)
        //-- On recadre les parametres des autres vertex
        //-------------------------------------------------------
        IntSurf_PntOn2S POn2S = svtx.Value(i).PntOn2S();
        if (indicevertexonline >= nbponline)
        {
          RecadreMemePeriode(POn2S,
                             curv->Value(nbponline),
                             U1Period(),
                             V1Period(),
                             U2Period(),
                             V2Period());
          curv->Add(POn2S);
        }
        else
        {
          RecadreMemePeriode(POn2S,
                             curv->Value(indicevertexonline + 1),
                             U1Period(),
                             V1Period(),
                             U2Period(),
                             V2Period());
          curv->InsertBefore(indicevertexonline + 1, POn2S);
        }
        //-- curv->InsertBefore(indicevertexonline+1,svtx.Value(i).PntOn2S());
        svtx.ChangeValue(i).SetParameter(indicevertexonline + 1);
        nbponline++;
        for (j = 1; j <= nbvtx; j++)
        {
          if (j != i)
          {
            double t = svtx.Value(j).ParameterOnLine();
            if (t > (double)indicevertexonline)
            {
              svtx.ChangeValue(j).SetParameter(t + 1.0);
            }
          }
        }
      } //-- Substitution
    } //-- indicevertexonline>=1

  } //-- boucle i sur vertex

  do
  {
    APointDeleted = false;
    for (i = 1; i <= nbvtx && (!APointDeleted); i++)
    {
      const IntPatch_Point& VTX = svtx.Value(i);
      for (j = 1; j <= nbvtx && (!APointDeleted); j++)
      {
        if (i != j)
        {
          const IntPatch_Point& VTXM1 = svtx.Value(j);

          bool kill   = false;
          bool killm1 = false;
          if (VTXM1.ParameterOnLine() == VTX.ParameterOnLine())
          {
            if (VTXM1.IsOnDomS1() && VTX.IsOnDomS1())
            { //-- OnS1    OnS1
              if (VTXM1.ArcOnS1() == VTX.ArcOnS1())
              { //-- OnS1 == OnS1
                if (VTXM1.IsOnDomS2())
                { //-- OnS1 == OnS1  OnS2
                  if (!VTX.IsOnDomS2())
                  { //-- OnS1 == OnS1  OnS2 PasOnS2
                    kill = true;
                  }
                  else
                  {
                    if (VTXM1.ArcOnS2() == VTX.ArcOnS2())
                    { //-- OnS1 == OnS1  OnS2 == OnS2
                      kill = true;
                    }
                  }
                }
                else
                { //-- OnS1 == OnS1  PasOnS2
                  if (VTX.IsOnDomS2())
                  { //-- OnS1 == OnS1  PasOnS2  OnS2
                    killm1 = true;
                  }
                }
              }
            }

            if (!(kill || killm1))
            {
              if (VTXM1.IsOnDomS2() && VTX.IsOnDomS2())
              { //-- OnS2    OnS2
                if (VTXM1.ArcOnS2() == VTX.ArcOnS2())
                { //-- OnS2 == OnS2
                  if (VTXM1.IsOnDomS1())
                  { //-- OnS2 == OnS2  OnS1
                    if (!VTX.IsOnDomS1())
                    { //-- OnS2 == OnS2  OnS1 PasOnS1
                      kill = true;
                    }
                    else
                    {
                      if (VTXM1.ArcOnS1() == VTX.ArcOnS1())
                      { //-- OnS2 == OnS2  OnS1 == OnS1
                        kill = true;
                      }
                    }
                  }
                  else
                  { //-- OnS2 == OnS2  PasOnS1
                    if (VTX.IsOnDomS1())
                    { //-- OnS2 == OnS2  PasOnS1  OnS1
                      killm1 = true;
                    }
                  }
                }
              }
            }
            if (kill)
            {
              APointDeleted = true;
              svtx.Remove(i);
              nbvtx--;
            }
            else if (killm1)
            {
              APointDeleted = true;
              svtx.Remove(j);
              nbvtx--;
            }
          }
        }
      }
    }
  } while (APointDeleted);

  do
  {
    SortIsOK = true;
    for (i = 2; i <= nbvtx && SortIsOK; i++)
    {
      const IntPatch_Point& Pim1 = svtx.Value(i - 1);
      const IntPatch_Point& Pii  = svtx.Value(i);
      if (Pim1.ParameterOnLine() == Pii.ParameterOnLine())
      {
        if ((!Pii.IsOnDomS1()) && (!Pii.IsOnDomS2()))
        {
          SortIsOK = false;
          svtx.Remove(i);
          nbvtx--;
        }
        else
        {
          if ((!Pim1.IsOnDomS1()) && (!Pim1.IsOnDomS2()))
          {
            SortIsOK = false;
            svtx.Remove(i - 1);
            nbvtx--;
          }
        }
      }
    }
  } while (!SortIsOK);
  //-- ----------------------------------------------------------------------------
  //-- On ajoute les vertex de debut et de fin de ligne s il ne sont pas presents.
  //--
  //-- Existe t il un vertex de debut de ligne, de fin .
  //--
  //-- Si Besoin : il faudra dedoubler les points de debut et de fin sur les periodiques ??????

  bool bFirst = false;
  bool bLast  = false;
  nbponline   = NbPnts();
  for (i = 1; i <= nbvtx; i++)
  {
    double pol = svtx.Value(i).ParameterOnLine();
    if (pol == 1.0)
    {
      bFirst = fipt = true;
      indf          = i;
    }
    if (pol == nbponline)
    {
      bLast = lapt = true;
      indl         = i;
    }
  }
  if (!bFirst)
  {
    double         pu1, pv1, pu2, pv2;
    bool           vtxfound = false;
    IntPatch_Point vtx;
    curv->Value(1).Parameters(pu1, pv1, pu2, pv2);
    for (i = 1; (!vtxfound) && (i <= nbvtx); i++)
    {
      const IntPatch_Point& V = svtx.Value(i);
      // jgv: to avoid loops
      // double vTol = V.Tolerance();
      if (CompareVertexAndPoint(V.Value(), curv->Value(1).Value(), Precision::Confusion() /*vTol*/))
      {
        vtx = V;
        vtx.SetParameters(pu1, pv1, pu2, pv2);
        vtxfound = true;
      }
    }
    if (!vtxfound)
    {
      vtx.SetValue(curv->Value(1).Value(), Tol, false);
      vtx.SetParameters(pu1, pv1, pu2, pv2);
    }
    vtx.SetParameter(1);
    svtx.Prepend(vtx);
    nbvtx++;
    fipt = true;
    indf = 1;
  }
  if (!bLast)
  {
    double         pu1, pv1, pu2, pv2;
    bool           vtxfound = false;
    IntPatch_Point vtx;
    curv->Value(nbponline).Parameters(pu1, pv1, pu2, pv2);
    for (i = 1; (!vtxfound) && (i <= nbvtx); i++)
    {
      const IntPatch_Point& V = svtx.Value(i);
      // jgv: to avoid loops
      // double vTol = V.Tolerance();
      if (CompareVertexAndPoint(V.Value(),
                                curv->Value(nbponline).Value(),
                                Precision::Confusion() /*vTol*/))
      {
        vtx = V;
        vtx.SetParameters(pu1, pv1, pu2, pv2);
        vtxfound = true;
      }
    }
    if (!vtxfound)
    {
      vtx.SetValue(curv->Value(nbponline).Value(), Tol, false);
      vtx.SetParameters(pu1, pv1, pu2, pv2);
    }
    vtx.SetParameter(nbponline);
    svtx.Append(vtx);
    nbvtx++;
    lapt = true;
    indl = nbvtx;
  }

  //--------------------------------------------------------------
  //-- ** Detection de points trouves sur une meme restriction
  //--    avec la meme transition et avec des params on line
  //--    voisins.
  //-- ** Dans ce cas (-> donnerait un baillemenmt) on supprime
  //--    le point 'intermediaire'.
  //-- ** (exemple Vtx(1)  .....  Vtx(204) Vtx(205))
  //--    on supprime le Vtx(204)
  //-- ** (exemple Vtx(1) Vtx(2)  .....  Vtx(205))
  //--    on supprime le Vtx(2)
  //-- ** (exemple Vtx(1)  ...  Vtx(100) Vtx(101) ... Vtx(205))
  //--    on supprime le Vtx(100)  (Vtx(100)et101 sur m restr)

  //--------------------------------------------------------------
  nbvtx = NbVertex();
  do
  {
    APointDeleted = false;
    for (i = 1; (i <= nbvtx) && (!APointDeleted); i++)
    {
      const IntPatch_Point& VTXi = svtx.Value(i);
      if ((VTXi.IsOnDomS1()) && (!VTXi.IsOnDomS2()))
      {
        for (j = 1; (j <= nbvtx) && (!APointDeleted); j++)
        {
          if (i != j)
          {
            const IntPatch_Point& VTXj = svtx.Value(j);
            if ((VTXj.IsOnDomS1()) && (!VTXj.IsOnDomS2()))
            {
              if ((VTXi.ParameterOnLine() == VTXj.ParameterOnLine() + 1)
                  || (VTXi.ParameterOnLine() == VTXj.ParameterOnLine() - 1))
              {
                if (VTXi.ArcOnS1() == VTXj.ArcOnS1())
                {
                  IntSurf_Transition t1 = VTXi.TransitionLineArc1();
                  IntSurf_Transition t2 = VTXj.TransitionLineArc1();
                  if (t1.TransitionType() == t2.TransitionType())
                  {
                    if ((VTXj.ParameterOnLine() != 1) && (VTXj.ParameterOnLine() != NbPnts()))
                    {
                      svtx.Remove(j);
                      nbvtx--;
                      if (lapt)
                      {
                        if (indl >= j)
                          indl--;
                      }
                      if (fipt)
                      {
                        if (indf >= j)
                          indf--;
                      }
                      APointDeleted = true;
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

  //-- meme traitement sur les restrictions du second shape

  while (APointDeleted);
  nbvtx = NbVertex();
  do
  {
    APointDeleted = false;
    for (i = 1; (i <= nbvtx) && (!APointDeleted); i++)
    {
      const IntPatch_Point& VTXi = svtx.Value(i);
      if ((!VTXi.IsOnDomS1()) && (VTXi.IsOnDomS2()))
      {
        for (j = 1; (j <= nbvtx) && (!APointDeleted); j++)
        {
          if (i != j)
          {
            const IntPatch_Point& VTXj = svtx.Value(j);
            if ((!VTXj.IsOnDomS1()) && (VTXj.IsOnDomS2()))
            {
              if ((VTXi.ParameterOnLine() == VTXj.ParameterOnLine() + 1)
                  || (VTXi.ParameterOnLine() == VTXj.ParameterOnLine() - 1))
              {
                if (VTXi.ArcOnS2() == VTXj.ArcOnS2())
                {
                  IntSurf_Transition t1 = VTXi.TransitionLineArc2();
                  IntSurf_Transition t2 = VTXj.TransitionLineArc2();
                  if (t1.TransitionType() == t2.TransitionType())
                  {
                    if ((VTXj.ParameterOnLine() != 1) && (VTXj.ParameterOnLine() != NbPnts()))
                    {
                      svtx.Remove(j);
                      nbvtx--;
                      if (lapt)
                      {
                        if (indl >= j)
                          indl--;
                      }
                      if (fipt)
                      {
                        if (indf >= j)
                          indf--;
                      }
                      APointDeleted = true;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  } while (APointDeleted);
  //--------------------------------------------------------------

  //--------------------------------------------------------------
  //-- dans le cas de lignes periodiques du type :
  //--    Un point sur restriction R1 de param p1  -> P3d1   Vtx1
  //--    Un point sur restriction R2 de param p2  -> P3d1   Vtx2
  //--
  //--    Un point sur restriction R1 de param p1  -> P3d1   Vtx3
  //--    pas de point sur R2
  //--
  //-- On doit dans ce cas creer un nouveau Vtx4 = Vtx3 sur la
  //-- restriction R2
  //--
  //-- Ce cas se produit qd on a corrige un baillement avec le filtre
  //-- precedent
  //--

  nbvtx = NbVertex();
  do
  {
    SortIsOK = true;
    for (i = 2; i <= nbvtx; i++)
    {
      if (svtx.Value(i - 1).ParameterOnLine() > svtx.Value(i).ParameterOnLine())
      {
        SortIsOK = false;
        svtx.Exchange(i - 1, i);
      }
    }
  } while (!SortIsOK);

  //-- Dump();

#if DEBUGV
  std::cout << "\n----------- apres ComputeVertexParameters -------------" << std::endl;
  for (i = 1; i <= nbvtx; i++)
  {
    Vertex(i).Dump();
    double polr = Vertex(i).ParameterOnLine();
    double pol  = (int)polr;
    if (pol >= 1 && pol <= nbvtx)
    {
      std::cout << "----> IntSurf_PntOn2S : " << polr << "  Pnt (" << Vertex(pol).Value().X() << ","
                << Vertex(pol).Value().Y() << "," << Vertex(pol).Value().Z() << ")" << std::endl;
    }
  }
  std::cout << "\n----------------------------------------------------------" << std::endl;
#endif
}

bool IntPatch_WLine::HasArcOnS1() const
{
  return (hasArcOnS1);
}

void IntPatch_WLine::SetArcOnS1(const occ::handle<Adaptor2d_Curve2d>& A)
{
  hasArcOnS1 = true;
  theArcOnS1 = A;
}

const occ::handle<Adaptor2d_Curve2d>& IntPatch_WLine::GetArcOnS1() const
{
  return (theArcOnS1);
}

bool IntPatch_WLine::HasArcOnS2() const
{
  return (hasArcOnS2);
}

void IntPatch_WLine::SetArcOnS2(const occ::handle<Adaptor2d_Curve2d>& A)
{
  hasArcOnS2 = true;
  theArcOnS2 = A;
}

const occ::handle<Adaptor2d_Curve2d>& IntPatch_WLine::GetArcOnS2() const
{
  return (theArcOnS2);
}

void IntPatch_WLine::Dump(const int theMode) const
{
  std::cout << " ----------- D u m p    I n t P a t c h  _  W L i n e  -(begin)------" << std::endl;
  const int aNbPoints = NbPnts();
  const int aNbVertex = NbVertex();

  switch (theMode)
  {
    case 0:
      printf("Num    [X  Y  Z]     [U1  V1]   [U2  V2]\n");
      for (int i = 1; i <= aNbPoints; i++)
      {
        double u1, v1, u2, v2;
        Point(i).Parameters(u1, v1, u2, v2);
        printf("%4d  [%+10.20f %+10.20f %+10.20f]  [%+10.20f %+10.20f]  [%+10.20f %+10.20f]\n",
               i,
               Point(i).Value().X(),
               Point(i).Value().Y(),
               Point(i).Value().Z(),
               u1,
               v1,
               u2,
               v2);
      }

      for (int i = 1; i <= aNbVertex; i++)
      {
        Vertex(i).Dump();
        double polr = Vertex(i).ParameterOnLine();
        int    pol  = static_cast<int>(polr);

        if (pol >= 1 && pol <= aNbVertex)
        {
          std::cout << "----> IntSurf_PntOn2S : " << polr << ", Pnt (" << Vertex(pol).Value().X()
                    << "," << Vertex(pol).Value().Y() << "," << Vertex(pol).Value().Z() << ")"
                    << std::endl;
        }
      }

      break;
    case 1:
      for (int i = 1; i <= aNbPoints; i++)
      {
        double u1, v1, u2, v2;
        Point(i).Parameters(u1, v1, u2, v2);
        printf("point p%d %+10.20f %+10.20f %+10.20f\n",
               i,
               Point(i).Value().X(),
               Point(i).Value().Y(),
               Point(i).Value().Z());
      }

      break;
    case 2:
      for (int i = 1; i <= aNbPoints; i++)
      {
        double u1, v1, u2, v2;
        Point(i).Parameters(u1, v1, u2, v2);
        printf("point p%d %+10.20f %+10.20f\n", i, u1, v1);
      }

      break;
    default:
      for (int i = 1; i <= aNbPoints; i++)
      {
        double u1, v1, u2, v2;
        Point(i).Parameters(u1, v1, u2, v2);
        printf("point p%d %+10.20f %+10.20f\n", i, u2, v2);
      }

      break;
  }
  std::cout << "\n--------------------------------------------------- (end) -------" << std::endl;
}
