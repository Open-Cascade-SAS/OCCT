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

#include <IntPatch_RLine.hxx>
#include <IntSurf_LineOn2S.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IntPatch_RLine, IntPatch_PointLine)

IntPatch_RLine::IntPatch_RLine(const bool              Tang,
                               const IntSurf_TypeTrans Trans1,
                               const IntSurf_TypeTrans Trans2)
    : IntPatch_PointLine(Tang, Trans1, Trans2),
      ParamInf1(0.0),
      ParamSup1(0.0),
      ParamInf2(0.0),
      ParamSup2(0.0),
      fipt(false),
      lapt(false),
      indf(0),
      indl(0)
{
  typ  = IntPatch_Restriction;
  onS2 = false;
  onS1 = false;
}

IntPatch_RLine::IntPatch_RLine(const bool              Tang,
                               const IntSurf_Situation Situ1,
                               const IntSurf_Situation Situ2)
    : IntPatch_PointLine(Tang, Situ1, Situ2),
      ParamInf1(0.0),
      ParamSup1(0.0),
      ParamInf2(0.0),
      ParamSup2(0.0),
      fipt(false),
      lapt(false),
      indf(0),
      indl(0)
{
  typ  = IntPatch_Restriction;
  onS2 = false;
  onS1 = false;
}

IntPatch_RLine::IntPatch_RLine(const bool Tang)
    : IntPatch_PointLine(Tang),
      ParamInf1(0.0),
      ParamSup1(0.0),
      ParamInf2(0.0),
      ParamSup2(0.0),
      fipt(false),
      lapt(false),
      indf(0),
      indl(0)
{
  typ  = IntPatch_Restriction;
  onS2 = false;
  onS1 = false;
}

void IntPatch_RLine::ParamOnS1(double& a, double& b) const
{
  if (onS1)
  {
    a = RealLast();
    b = -a;
    for (int i = svtx.Length(); i >= 1; i--)
    {
      double p = svtx(i).ParameterOnLine();
      if (p < a)
        a = p;
      if (p > b)
        b = p;
    }
  }
  else
  {
    a = b = 0.0;
  }
}

void IntPatch_RLine::ParamOnS2(double& a, double& b) const
{
  if (onS2)
  {
    a = RealLast();
    b = -a;
    for (int i = svtx.Length(); i >= 1; i--)
    {
      double p = svtx(i).ParameterOnLine();
      if (p < a)
        a = p;
      if (p > b)
        b = p;
    }
  }
  else
  {
    a = b = 0.0;
  }
}

void IntPatch_RLine::SetArcOnS1(const occ::handle<Adaptor2d_Curve2d>& A)
{
  theArcOnS1 = A;
  onS1       = true;
}

void IntPatch_RLine::SetArcOnS2(const occ::handle<Adaptor2d_Curve2d>& A)
{
  theArcOnS2 = A;
  onS2       = true;
}

void IntPatch_RLine::SetPoint(const int Index, const IntPatch_Point& thepoint)
{
  curv->Value(Index, thepoint.PntOn2S());
}

// void IntPatch_RLine::ComputeVertexParameters(const double Tol)
void IntPatch_RLine::ComputeVertexParameters(const double)
{
  int i, j, nbvtx; // k;

  bool APointDeleted = false;
  //----------------------------------------------------------
  //--     F i l t r e   s u r   r e s t r i c t i o n s   --
  //----------------------------------------------------------
  //-- deux vertex sur la meme restriction et seulement
  //-- sur celle ci ne doivent pas avoir le meme parametre
  //--
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
              if (VTXi.ParameterOnLine() == VTXj.ParameterOnLine())
              {
                if (VTXi.ArcOnS1() == VTXj.ArcOnS1())
                {
                  if (VTXi.ParameterOnArc1() == VTXj.ParameterOnArc1())
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
                  if (VTXi.ParameterOnArc2() == VTXj.ParameterOnArc2())
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
  } while (APointDeleted);

  nbvtx = NbVertex();

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

  do
  {
    APointDeleted = false;
    bool restrdiff;
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
            restrdiff = false;
            if (VTXM1.IsOnDomS1() && VTX.IsOnDomS1())
            { //-- OnS1    OnS1
              if (VTXM1.ArcOnS1() == VTX.ArcOnS1())
              { //-- OnS1 == OnS1
                if (VTX.ParameterOnArc1() == VTXM1.ParameterOnArc1())
                {
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
                        if (VTX.ParameterOnArc2() == VTXM1.ParameterOnArc2())
                        {
                          kill = true;
                        }
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
              {
                restrdiff = true;
              }
            }

            if ((!restrdiff) && (!(kill || killm1)))
            {
              if (VTXM1.IsOnDomS2() && VTX.IsOnDomS2())
              { //-- OnS2    OnS2
                if (VTXM1.ArcOnS2() == VTX.ArcOnS2())
                { //-- OnS2 == OnS2
                  if (VTX.ParameterOnArc2() == VTXM1.ParameterOnArc2())
                  {
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
                          if (VTX.ParameterOnArc1() == VTXM1.ParameterOnArc1())
                          {
                            kill = true;
                          }
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
                {
                  restrdiff = true;
                }
              }
            }
            if (!restrdiff)
            {
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

  //----------------------------------------------------
  //-- On trie les Vertex ( Cas Bizarre )
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

  SetFirstPoint(1);
  SetLastPoint(nbvtx);
}

void IntPatch_RLine::Dump(const int theMode) const
{
  std::cout << " ----------- D u m p    I n t P a t c h  _  R L i n e  -(begin)------" << std::endl;
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
