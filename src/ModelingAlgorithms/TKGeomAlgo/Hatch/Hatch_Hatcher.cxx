// Created on: 1992-08-19
// Created by: Modelistation
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

#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <Hatch_Hatcher.hxx>
#include <Hatch_Line.hxx>
#include <Hatch_Parameter.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <Standard_OutOfRange.hxx>

//=================================================================================================

Hatch_Hatcher::Hatch_Hatcher(const double Tol, const bool Oriented)
    : myToler(Tol),
      myOrient(Oriented)
{
}

//=================================================================================================

void Hatch_Hatcher::AddLine(const gp_Lin2d& L, const Hatch_LineForm T)
{
  Hatch_Line HL(L, T);
  myLines.Append(HL);
}

//=================================================================================================

void Hatch_Hatcher::AddLine(const gp_Dir2d& D, const double Dist)
{
  double X = D.X();
  double Y = D.Y();
  gp_Pnt2d      O(-Y * Dist, X * Dist);
  gp_Lin2d      L(O, D);
  AddLine(L, Hatch_ANYLINE);
}

//=================================================================================================

void Hatch_Hatcher::AddXLine(const double X)
{
  gp_Pnt2d O(X, 0);
  gp_Dir2d D(gp_Dir2d::D::Y);
  gp_Lin2d L(O, D);
  AddLine(L, Hatch_XLINE);
}

//=================================================================================================

void Hatch_Hatcher::AddYLine(const double Y)
{
  gp_Pnt2d O(0, Y);
  gp_Dir2d D(gp_Dir2d::D::X);
  gp_Lin2d L(O, D);
  AddLine(L, Hatch_YLINE);
}

//=================================================================================================

void Hatch_Hatcher::Trim(const gp_Lin2d& L, const int Index)
{
  Trim(L, RealFirst(), RealLast(), Index);
}

//=================================================================================================

void Hatch_Hatcher::Trim(const gp_Lin2d&        L,
                         const double    Start,
                         const double    End,
                         const int Index)
{
  IntAna2d_IntPoint        Pinter;
  IntAna2d_AnaIntersection Inters;
  int         iLine;
  for (iLine = 1; iLine <= myLines.Length(); iLine++)
  {
    Inters.Perform(myLines(iLine).myLin, L);
    if (Inters.IsDone())
    {
      if (!Inters.IdenticalElements() && !Inters.ParallelElements())
      {
        // we have got something
        Pinter                = Inters.Point(1);
        double linePar = Pinter.ParamOnSecond();
        if (linePar - Start < -myToler)
          continue;
        if (linePar - End > myToler)
          continue;
        double norm = L.Direction() ^ myLines(iLine).myLin.Direction();
        if (linePar - Start < myToler)
        {
          // on the limit of the trimming segment
          // accept if the other extremity is on the left
          if (norm < 0)
            continue;
        }
        if (linePar - End > -myToler)
        {
          // on the limit of the trimming segment
          // accept if the other extremity is on the left
          if (norm > 0)
            continue;
        }
        // insert the parameter
        myLines(iLine).AddIntersection(Pinter.ParamOnFirst(),
                                       norm > 0,
                                       Index,
                                       Pinter.ParamOnSecond(),
                                       myToler);
      }
    }
  }
}

//=================================================================================================

void Hatch_Hatcher::Trim(const gp_Pnt2d& P1, const gp_Pnt2d& P2, const int Index)
{
  gp_Vec2d V(P1, P2);
  if (std::abs(V.X()) > .9 * RealLast())
    V.Multiply(1 / V.X());
  else if (std::abs(V.Y()) > .9 * RealLast())
    V.Multiply(1 / V.Y());
  if (V.Magnitude() > myToler)
  {
    gp_Dir2d D(V);
    gp_Lin2d L(P1, D);
    Trim(L, 0, P1.Distance(P2), Index);
  }
}

//=================================================================================================

int Hatch_Hatcher::NbIntervals() const
{
  int i, nb = 0;
  for (i = 1; i <= myLines.Length(); i++)
    nb += NbIntervals(i);
  return nb;
}

//=================================================================================================

int Hatch_Hatcher::NbLines() const
{
  return myLines.Length();
}

//=================================================================================================

const gp_Lin2d& Hatch_Hatcher::Line(const int I) const
{
  return myLines(I).myLin;
}

//=================================================================================================

Hatch_LineForm Hatch_Hatcher::LineForm(const int I) const
{
  return myLines(I).myForm;
}

//=================================================================================================

double Hatch_Hatcher::Coordinate(const int I) const
{
  switch (myLines(I).myForm)
  {

    case Hatch_XLINE:
      return myLines(I).myLin.Location().X();

    case Hatch_YLINE:
      return myLines(I).myLin.Location().Y();

    case Hatch_ANYLINE:
      throw Standard_OutOfRange("Hatcher : not an X or Y line");
  }

  return 0.;
}

//=================================================================================================

int Hatch_Hatcher::NbIntervals(const int I) const
{
  int l = myLines(I).myInters.Length();
  if (l == 0)
    l = myOrient ? 1 : 0;
  else
  {
    l = l / 2;
    if (myOrient)
      if (!myLines(I).myInters(1).myStart)
        l++;
  }
  return l;
}

//=================================================================================================

double Hatch_Hatcher::Start(const int I, const int J) const
{
  if (myLines(I).myInters.IsEmpty())
  {
    if (J != 1 || !myOrient)
      throw Standard_OutOfRange();
    return RealFirst();
  }
  else
  {
    int jj = 2 * J - 1;
    if (!myLines(I).myInters(1).myStart && myOrient)
      jj--;
    if (jj == 0)
      return RealFirst();
    return myLines(I).myInters(jj).myPar1;
  }
}

//=================================================================================================

void Hatch_Hatcher::StartIndex(const int I,
                               const int J,
                               int&      Index,
                               double&         Par2) const
{
  if (myLines(I).myInters.IsEmpty())
  {
    if (J != 1)
      throw Standard_OutOfRange();
    Index = 0;
    Par2  = 0;
  }
  else
  {
    int jj = 2 * J - 1;
    if (!myLines(I).myInters(1).myStart && myOrient)
      jj--;
    if (jj == 0)
    {
      Index = 0;
      Par2  = 0;
    }
    else
    {
      Index = myLines(I).myInters(jj).myIndex;
      Par2  = myLines(I).myInters(jj).myPar2;
    }
  }
}

//=================================================================================================

double Hatch_Hatcher::End(const int I, const int J) const
{
  if (myLines(I).myInters.IsEmpty())
  {
    if (J != 1 || !myOrient)
      throw Standard_OutOfRange();
    return RealLast();
  }
  else
  {
    int jj = 2 * J;
    if (!myLines(I).myInters(1).myStart && myOrient)
      jj--;
    if (jj > myLines(I).myInters.Length())
      return RealLast();
    return myLines(I).myInters(jj).myPar1;
  }
}

//=================================================================================================

void Hatch_Hatcher::EndIndex(const int I,
                             const int J,
                             int&      Index,
                             double&         Par2) const
{
  if (myLines(I).myInters.IsEmpty())
  {
    if (J != 1)
      throw Standard_OutOfRange();
    Index = 0;
    Par2  = 0;
  }
  else
  {
    int jj = 2 * J;
    if (!myLines(I).myInters(1).myStart && myOrient)
      jj--;
    if (jj > myLines(I).myInters.Length())
    {
      Index = 0;
      Par2  = 0;
    }
    else
    {
      Index = myLines(I).myInters(jj).myIndex;
      Par2  = myLines(I).myInters(jj).myPar2;
    }
  }
}
