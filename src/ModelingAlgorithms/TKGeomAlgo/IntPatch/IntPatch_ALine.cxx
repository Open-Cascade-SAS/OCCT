// Created on: 1992-04-06
// Created by: Jacques GOUSSARD
// Copyright (c) 1992-1999 Matra Datavision
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

#include <gp_Pnt.hxx>
#include <IntPatch_ALine.hxx>
#include <IntPatch_Point.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IntPatch_ALine, IntPatch_Line)

#define DEBUG 0

IntPatch_ALine::IntPatch_ALine(const IntAna_Curve&     C,
                               const bool              Tang,
                               const IntSurf_TypeTrans Trans1,
                               const IntSurf_TypeTrans Trans2)
    : IntPatch_Line(Tang, Trans1, Trans2),
      fipt(false),
      lapt(false),
      indf(0),
      indl(0)
{
  typ  = IntPatch_Analytic;
  curv = C;
}

IntPatch_ALine::IntPatch_ALine(const IntAna_Curve&     C,
                               const bool              Tang,
                               const IntSurf_Situation Situ1,
                               const IntSurf_Situation Situ2)
    : IntPatch_Line(Tang, Situ1, Situ2),
      fipt(false),
      lapt(false),
      indf(0),
      indl(0)
{
  typ  = IntPatch_Analytic;
  curv = C;
}

IntPatch_ALine::IntPatch_ALine(const IntAna_Curve& C, const bool Tang)
    : IntPatch_Line(Tang),
      fipt(false),
      lapt(false),
      indf(0),
      indl(0)
{
  typ  = IntPatch_Analytic;
  curv = C;
}

const IntAna_Curve& IntPatch_ALine::Curve() const
{
  return (curv);
}

#define PCONFUSION 0.00001

void IntPatch_ALine::AddVertex(const IntPatch_Point& VTXj)
{
  svtx.Append(VTXj);
}

void IntPatch_ALine::ComputeVertexParameters(const double Tol)
{
  bool   SortIsOK, APointDeleted;
  bool   SortAgain = true;
  int    nbvtx, i, j;
  double ParamMinOnLine, ParamMaxOnLine;
  bool   OpenFirst, OpenLast;

  ParamMinOnLine = FirstParameter(OpenFirst);
  ParamMaxOnLine = LastParameter(OpenLast);

  //----------------------------------------------------------
  //--     F i l t r e   s u r   r e s t r i c t i o n s   --
  //----------------------------------------------------------
  //-- deux vertex sur la meme restriction et seulement
  //-- sur celle ci ne doivent pas avoir le meme parametre
  //--
  //--

  //-- Le tri est necessaire si suppression du first ou du last point
  nbvtx = NbVertex();

  //-- On verifie qu un vertex a bien toute ses representations :
  //-- Cas tres rare : point de tangence sur un debut de ligne
  //-- et la ligne fait 2 * 2 PI de parametrage.

  for (i = 1; i <= nbvtx; i++)
  {
    IntPatch_Point& VTX     = svtx.ChangeValue(i);
    double          p       = VTX.ParameterOnLine();
    double          pmpimpi = p - M_PI - M_PI;
    if (pmpimpi >= ParamMinOnLine)
    {
      gp_Pnt P1 = Value(pmpimpi);
      double d1 = P1.Distance(VTX.Value());
      if (d1 < Tol)
      {
        IntPatch_Point OVTX(VTX);
        OVTX.SetParameter(pmpimpi);
        svtx.Append(OVTX);
      }
    }
    pmpimpi = p + M_PI + M_PI;
    if (pmpimpi <= ParamMaxOnLine)
    {
      gp_Pnt P1 = Value(pmpimpi);
      double d1 = P1.Distance(VTX.Value());
      if (d1 < Tol)
      {
        IntPatch_Point OVTX(VTX);
        OVTX.SetParameter(pmpimpi);
        svtx.Append(OVTX);
      }
    }
  }

  nbvtx = NbVertex();

  if (nbvtx <= 0)
    return;
  do
  {
    SortIsOK = true;
    for (i = 2; i <= nbvtx; i++)
    {
      if (svtx.Value(i - 1).ParameterOnLine() > svtx.Value(i).ParameterOnLine())
      {
        SortIsOK = false;
        svtx.Exchange(i - 1, i);
        if (fipt)
        {
          if (indf == i)
            indf = i - 1;
          else if (indf == i - 1)
            indf = i;
        }
        if (lapt)
        {
          if (indl == i)
            indl = i - 1;
          else if (indl == i - 1)
            indl = i;
        }
      }
    }
  } while (!SortIsOK);

  do
  {
    APointDeleted = false;
    for (i = 1; (i <= nbvtx) && (APointDeleted == false); i++)
    {
      const IntPatch_Point& VTXi = svtx.Value(i);
      if ((VTXi.IsOnDomS1() == true) && (VTXi.IsOnDomS2() == false))
      {
        for (j = 1; (j <= nbvtx) && (APointDeleted == false); j++)
        {
          if (i != j)
          {
            const IntPatch_Point& VTXj = svtx.Value(j);
            if ((VTXj.IsOnDomS1() == true) && (VTXj.IsOnDomS2() == false))
            {
              if (std::abs(VTXi.ParameterOnArc1() - VTXj.ParameterOnArc1()) <= PCONFUSION)
              {
                if (std::abs(VTXi.ParameterOnLine() - VTXj.ParameterOnLine()) <= PCONFUSION)
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
    }
  } while (APointDeleted == true);

  do
  {
    APointDeleted = false;
    for (i = 1; (i <= nbvtx) && (APointDeleted == false); i++)
    {
      const IntPatch_Point& VTXi = svtx.Value(i);
      if ((VTXi.IsOnDomS2() == true) && (VTXi.IsOnDomS1() == false))
      {
        for (j = 1; (j <= nbvtx) && (APointDeleted == false); j++)
        {
          if (i != j)
          {
            const IntPatch_Point& VTXj = svtx.Value(j);
            if ((VTXj.IsOnDomS2() == true) && (VTXj.IsOnDomS1() == false))
            {
              if (std::abs(VTXi.ParameterOnArc2() - VTXj.ParameterOnArc2()) <= PCONFUSION)
              {
                if (std::abs(VTXi.ParameterOnLine() - VTXj.ParameterOnLine()) <= PCONFUSION)
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
    }
  } while (APointDeleted == true);

  //----------------------------------------------------------
  //-- Tri des vertex et suppression des Vtx superflus
  //--
  do
  {
    nbvtx = NbVertex();
    if (SortAgain)
    {
      do
      {
        SortIsOK = true;
        for (i = 2; i <= nbvtx; i++)
        {
          if (svtx.Value(i - 1).ParameterOnLine() > svtx.Value(i).ParameterOnLine())
          {
            SortIsOK = false;
            svtx.Exchange(i - 1, i);
            if (fipt)
            {
              if (indf == i)
                indf = i - 1;
              else if (indf == i - 1)
                indf = i;
            }
            if (lapt)
            {
              if (indl == i)
                indl = i - 1;
              else if (indl == i - 1)
                indl = i;
            }
          }
        }
      } while (!SortIsOK);
    }
    SortAgain = false;
    SortIsOK  = true;
    for (i = 2; i <= nbvtx && SortIsOK; i++)
    {
      IntPatch_Point& VTX = svtx.ChangeValue(i);
      for (j = 1; j < i && SortIsOK; j++)
      {
        IntPatch_Point& VTXM1  = svtx.ChangeValue(j);
        bool            kill   = false;
        bool            killm1 = false;
        if (std::abs(VTXM1.ParameterOnLine() - VTX.ParameterOnLine()) < PCONFUSION)
        {
          if (VTXM1.IsOnDomS1() && VTX.IsOnDomS1())
          { //-- OnS1    OnS1
            if (VTXM1.ArcOnS1() == VTX.ArcOnS1())
            { //-- OnS1 == OnS1
              if (VTXM1.IsOnDomS2())
              { //-- OnS1 == OnS1  OnS2
                if (VTX.IsOnDomS2() == false)
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
          else
          { //-- Pas OnS1  et  OnS1
            if (VTXM1.IsOnDomS2() == false && VTX.IsOnDomS2() == false)
            {
              if (VTXM1.IsOnDomS1() && VTX.IsOnDomS1() == false)
              {
                kill = true;
              }
              else if (VTX.IsOnDomS1() && VTXM1.IsOnDomS1() == false)
              {
                killm1 = true;
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
                  if (VTX.IsOnDomS1() == false)
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
            else
            { //-- Pas OnS2  et  OnS2
              if (VTXM1.IsOnDomS1() == false && VTX.IsOnDomS1() == false)
              {
                if (VTXM1.IsOnDomS2() && VTX.IsOnDomS2() == false)
                {
                  kill = true;
                }
                else if (VTX.IsOnDomS2() && VTXM1.IsOnDomS2() == false)
                {
                  killm1 = true;
                }
              }
            }
          }
          //-- On a j < i
          if (kill)
          {
            SortIsOK = false;
            if (lapt)
            {
              if (indl > i)
                indl--;
              else if (indl == i)
                indl = j;
            }
            if (fipt)
            {
              if (indf > i)
                indf--;
              else if (indf == i)
                indf = j;
            }
            svtx.Remove(i);
            nbvtx--;
          }
          else if (killm1)
          {
            SortIsOK = false;
            if (lapt)
            {
              if (indl > j)
                indl--;
              else if (indl == j)
                indl = i - 1;
            }
            if (fipt)
            {
              if (indf > j)
                indf--;
              else if (indf == j)
                indf = i - 1;
            }
            svtx.Remove(j);
            nbvtx--;
          }
        }
      }
    }
  } while (!SortIsOK);

  //----------------------------------------------------------
  //--   Traitement des lignes periodiques                  --
  //----------------------------------------------------------
  if (OpenFirst == false && OpenLast == false)
  {
    nbvtx = NbVertex();

    IntPatch_Point& VTX0 = svtx.ChangeValue(1);
    IntPatch_Point& VTXN = svtx.ChangeValue(nbvtx);
    if (VTX0.ParameterOnLine() == ParamMinOnLine)
    {
      if (VTXN.ParameterOnLine() != ParamMaxOnLine)
      {
        gp_Pnt PN = Value(ParamMaxOnLine);
        double d  = PN.Distance(VTX0.Value());
        if (d <= Tol)
        {
          IntPatch_Point OVTX(VTX0);
          OVTX.SetParameter(ParamMaxOnLine);
          svtx.Append(OVTX);
        }
      }
      else
      {
        if (VTXN.ParameterOnLine() == ParamMaxOnLine)
        {
          if (VTX0.ParameterOnLine() != ParamMinOnLine)
          {
            gp_Pnt P0 = Value(ParamMinOnLine);
            double d  = P0.Distance(VTX0.Value());
            if (d <= Tol)
            {
              IntPatch_Point OVTX(VTXN);
              OVTX.SetParameter(ParamMinOnLine);
              svtx.Prepend(OVTX);
            }
          }
        }
      }
    }
  }
  //---------------------------------------------------------
  //-- Faut il supprimer le premier et le dernier point
  //--
  nbvtx = NbVertex();
  if (nbvtx > 1)
  {
    IntPatch_Point& VTX0 = svtx.ChangeValue(1);
    if ((VTX0.IsOnDomS1() == false) && (VTX0.IsOnDomS2() == false))
    {
      svtx.Remove(1);
      nbvtx--;
      if (lapt)
      {
        indl--;
      }
    }
  }
  if (nbvtx > 1)
  {
    IntPatch_Point& VTX0 = svtx.ChangeValue(nbvtx);
    if ((VTX0.IsOnDomS1() == false) && (VTX0.IsOnDomS2() == false))
    {
      svtx.Remove(nbvtx);
      if (lapt)
      {
        indl--;
      }
    }
  }

  //-- Si 2 vertex ont le meme parametre   on identifie le p3d
  nbvtx = NbVertex();
  do
  {
    SortIsOK = true;
    for (i = 2; i <= nbvtx; i++)
    {
      IntPatch_Point& VTX   = svtx.ChangeValue(i);
      IntPatch_Point& VTXm1 = svtx.ChangeValue(i - 1);
      if (std::abs(VTX.ParameterOnLine() - VTXm1.ParameterOnLine()) < PCONFUSION)
      {
        if (VTX.IsOnDomS1() && VTXm1.IsOnDomS1() == false)
        {
          VTXm1.SetArc(true,
                       VTX.ArcOnS1(),
                       VTX.ParameterOnArc1(),
                       VTX.TransitionLineArc1(),
                       VTX.TransitionOnS1());
        }
        else if (VTXm1.IsOnDomS1() && VTX.IsOnDomS1() == false)
        {
          VTX.SetArc(true,
                     VTXm1.ArcOnS1(),
                     VTXm1.ParameterOnArc1(),
                     VTXm1.TransitionLineArc1(),
                     VTXm1.TransitionOnS1());
        }
        if (VTX.IsVertexOnS1() && VTXm1.IsVertexOnS1() == false)
        {
          VTXm1.SetVertex(true, VTX.VertexOnS1());
          VTXm1.SetArc(true,
                       VTX.ArcOnS1(),
                       VTX.ParameterOnArc1(),
                       VTX.TransitionLineArc1(),
                       VTX.TransitionOnS1());
        }
        else if (VTXm1.IsVertexOnS1() && VTX.IsVertexOnS1() == false)
        {
          VTX.SetVertex(true, VTXm1.VertexOnS1());
          VTX.SetArc(true,
                     VTXm1.ArcOnS1(),
                     VTXm1.ParameterOnArc1(),
                     VTXm1.TransitionLineArc1(),
                     VTXm1.TransitionOnS1());
        }

        if (VTX.IsOnDomS2() && VTXm1.IsOnDomS2() == false)
        {
          VTXm1.SetArc(false,
                       VTX.ArcOnS2(),
                       VTX.ParameterOnArc2(),
                       VTX.TransitionLineArc2(),
                       VTX.TransitionOnS2());
        }
        else if (VTXm1.IsOnDomS2() && VTX.IsOnDomS2() == false)
        {
          VTX.SetArc(false,
                     VTXm1.ArcOnS2(),
                     VTXm1.ParameterOnArc2(),
                     VTXm1.TransitionLineArc2(),
                     VTXm1.TransitionOnS2());
        }
        if (VTX.IsVertexOnS2() && VTXm1.IsVertexOnS2() == false)
        {
          VTXm1.SetVertex(false, VTX.VertexOnS2());
          VTXm1.SetArc(false,
                       VTX.ArcOnS2(),
                       VTX.ParameterOnArc2(),
                       VTX.TransitionLineArc2(),
                       VTX.TransitionOnS2());
        }
        else if (VTXm1.IsVertexOnS2() && VTX.IsVertexOnS2() == false)
        {
          VTX.SetVertex(false, VTXm1.VertexOnS2());
          VTX.SetArc(false,
                     VTXm1.ArcOnS2(),
                     VTXm1.ParameterOnArc2(),
                     VTXm1.TransitionLineArc2(),
                     VTXm1.TransitionOnS2());
        }

        if (VTX.Value().SquareDistance(VTXm1.Value()) > 1e-12)
        {
          IntPatch_Point CopyVtx = VTXm1;
          VTXm1.SetParameter(VTX.ParameterOnLine());
          VTXm1.SetValue(VTX.Value(), VTX.Tolerance(), VTX.IsTangencyPoint());
          double u1, v1, u2, v2;
          VTX.Parameters(u1, v1, u2, v2);
          VTXm1.SetParameters(u1, v1, u2, v2);
          if (CopyVtx.IsOnDomS1())
          {
            VTXm1.SetArc(true,
                         CopyVtx.ArcOnS1(),
                         CopyVtx.ParameterOnArc1(),
                         CopyVtx.TransitionLineArc1(),
                         CopyVtx.TransitionOnS1());
          }
          if (CopyVtx.IsOnDomS2())
          {
            VTXm1.SetArc(false,
                         CopyVtx.ArcOnS2(),
                         CopyVtx.ParameterOnArc2(),
                         CopyVtx.TransitionLineArc2(),
                         CopyVtx.TransitionOnS2());
          }
          if (CopyVtx.IsVertexOnS1())
          {
            VTXm1.SetVertex(true, CopyVtx.VertexOnS1());
            VTXm1.SetArc(true,
                         CopyVtx.ArcOnS1(),
                         CopyVtx.ParameterOnArc1(),
                         CopyVtx.TransitionLineArc1(),
                         CopyVtx.TransitionOnS1());
          }
          if (CopyVtx.IsVertexOnS2())
          {
            VTXm1.SetVertex(false, CopyVtx.VertexOnS2());
            VTXm1.SetArc(false,
                         CopyVtx.ArcOnS2(),
                         CopyVtx.ParameterOnArc2(),
                         CopyVtx.TransitionLineArc2(),
                         CopyVtx.TransitionOnS2());
          }

          SortIsOK = false;
          //-- std::cout<<" IntPatch_ALine : ComputeVertexParameters : Ajust "<<std::endl;
        }
      }
    }
  } while (!SortIsOK);

  /*nbvtx     = NbVertex();
  for(int opopo = 1; opopo<=nbvtx; opopo++) {
    svtx.Value(opopo).Dump();
  }*/
}
