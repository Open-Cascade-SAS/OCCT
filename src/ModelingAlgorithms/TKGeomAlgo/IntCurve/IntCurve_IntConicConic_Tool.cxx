// Created on: 1992-05-06
// Created by: Laurent BUCHARD
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

#include <IntCurve_IntConicConic_Tool.hxx>
#include <gp.hxx>

#define TOLERANCE_ANGULAIRE 0.00000001

//======================================================================
//===         R e s s o u r c e s        G e n e r a l e s           ===
//======================================================================

void Determine_Transition_LC(const IntRes2d_Position Pos1,
                             gp_Vec2d&               Tan1,
                             const gp_Vec2d&         Norm1,
                             IntRes2d_Transition&    T1,
                             const IntRes2d_Position Pos2,
                             gp_Vec2d&               Tan2,
                             const gp_Vec2d&         Norm2,
                             IntRes2d_Transition&    T2,
                             const double)
{

  double sgn  = Tan1.Crossed(Tan2);
  double norm = Tan1.Magnitude() * Tan2.Magnitude();

  if (std::abs(sgn) <= TOLERANCE_ANGULAIRE * norm)
  { // Transition TOUCH #########
    bool opos = (Tan1.Dot(Tan2)) < 0;

    gp_Vec2d Norm;
    //  Modified by Sergey KHROMOV - Thu Nov  2 17:57:15 2000 Begin
    Tan1.Normalize();
    //  Modified by Sergey KHROMOV - Thu Nov  2 17:57:16 2000 End
    Norm.SetCoord(-Tan1.Y(), Tan1.X());

    double Val1 = Norm.Dot(Norm1);
    double Val2 = Norm.Dot(Norm2);

    if (std::abs(Val1 - Val2) <= gp::Resolution())
    {
      T1.SetValue(true, Pos1, IntRes2d_Unknown, opos);
      T2.SetValue(true, Pos2, IntRes2d_Unknown, opos);
    }
    else if (Val2 > Val1)
    {
      T2.SetValue(true, Pos2, IntRes2d_Inside, opos);
      if (opos)
      {
        T1.SetValue(true, Pos1, IntRes2d_Inside, opos);
      }
      else
      {
        T1.SetValue(true, Pos1, IntRes2d_Outside, opos);
      }
    }
    else
    { // Val1 > Val2
      T2.SetValue(true, Pos2, IntRes2d_Outside, opos);
      if (opos)
      {
        T1.SetValue(true, Pos1, IntRes2d_Outside, opos);
      }
      else
      {
        T1.SetValue(true, Pos1, IntRes2d_Inside, opos);
      }
    }
  }
  else if (sgn < 0)
  {
    T1.SetValue(false, Pos1, IntRes2d_In);
    T2.SetValue(false, Pos2, IntRes2d_Out);
  }
  else
  {
    T1.SetValue(false, Pos1, IntRes2d_Out);
    T2.SetValue(false, Pos2, IntRes2d_In);
  }
}

//----------------------------------------------------------------------
double NormalizeOnCircleDomain(const double _Param, const IntRes2d_Domain& TheDomain)
{
  double Param = _Param;
  while (Param < TheDomain.FirstParameter())
  {
    Param += PIpPI;
  }
  while (Param > TheDomain.LastParameter())
  {
    Param -= PIpPI;
  }
  return (Param);
}

//----------------------------------------------------------------------
PeriodicInterval PeriodicInterval::FirstIntersection(PeriodicInterval& PInter)
{
  double a, b;
  if (PInter.isnull || isnull)
  {
    PeriodicInterval PourSGI;
    return (PourSGI);
  }
  else
  {
    if (Length() >= PIpPI)
      return (PeriodicInterval(PInter.Binf, PInter.Bsup));
    if (PInter.Length() >= PIpPI)
      return (PeriodicInterval(Binf, Bsup));
    if (PInter.Bsup <= Binf)
    {
      while (PInter.Binf <= Binf && PInter.Bsup <= Binf)
      {
        PInter.Binf += PIpPI;
        PInter.Bsup += PIpPI;
      }
    }
    if (PInter.Binf >= Bsup)
    {
      while (PInter.Binf >= Bsup && PInter.Bsup >= Bsup)
      {
        PInter.Binf -= PIpPI;
        PInter.Bsup -= PIpPI;
      }
    }
    if ((PInter.Bsup < Binf) || (PInter.Binf > Bsup))
    {
      PeriodicInterval PourSGI;
      return (PourSGI);
    }
  }

  a = (PInter.Binf > Binf) ? PInter.Binf : Binf;
  b = (PInter.Bsup < Bsup) ? PInter.Bsup : Bsup;

  return (PeriodicInterval(a, b));
}

//----------------------------------------------------------------------
PeriodicInterval PeriodicInterval::SecondIntersection(PeriodicInterval& PInter)
{
  double a, b;

  if (PInter.isnull || isnull || this->Length() >= PIpPI || PInter.Length() >= PIpPI)
  {
    PeriodicInterval PourSGI;
    return (PourSGI);
  }

  double PInter_inf = PInter.Binf + PIpPI;
  double PInter_sup = PInter.Bsup + PIpPI;
  if (PInter_inf > Bsup)
  {
    PInter_inf = PInter.Binf - PIpPI;
    PInter_sup = PInter.Bsup - PIpPI;
  }
  if ((PInter_sup < Binf) || (PInter_inf > Bsup))
  {
    PeriodicInterval PourSGI;
    return (PourSGI);
  }
  else
  {
    a = (PInter_inf > Binf) ? PInter_inf : Binf;
    b = (PInter_sup < Bsup) ? PInter_sup : Bsup;
  }
  return (PeriodicInterval(a, b));
}

//----------------------------------------------------------------------
Interval::Interval()
    : Binf(0.),
      Bsup(0.),
      HasFirstBound(false),
      HasLastBound(false)
{
  IsNull = true;
}

Interval::Interval(const double a, const double b)
{
  HasFirstBound = HasLastBound = true;
  if (a < b)
  {
    Binf = a;
    Bsup = b;
  }
  else
  {
    Binf = b;
    Bsup = a;
  }
  IsNull = false;
}

Interval::Interval(const IntRes2d_Domain& Domain)
    : Binf(0.0),
      Bsup(0.0)
{
  IsNull = false;
  if (Domain.HasFirstPoint())
  {
    HasFirstBound = true;
    Binf          = Domain.FirstParameter() - Domain.FirstTolerance();
  }
  else
    HasFirstBound = false;
  if (Domain.HasLastPoint())
  {
    HasLastBound = true;
    Bsup         = Domain.LastParameter() + Domain.LastTolerance();
  }
  else
    HasLastBound = false;
}

Interval::Interval(const double a, const bool hf, const double b, const bool hl)
{
  Binf          = a;
  Bsup          = b;
  IsNull        = false;
  HasFirstBound = hf;
  HasLastBound  = hl;
}

double Interval::Length()
{
  return ((IsNull) ? -1.0 : std::abs(Bsup - Binf));
}

Interval Interval::IntersectionWithBounded(const Interval& Inter)
{
  if (IsNull || Inter.IsNull)
  {
    Interval PourSGI;
    return (PourSGI);
  }
  if (!(HasFirstBound || HasLastBound))
    return (Interval(Inter.Binf, Inter.Bsup));
  double a, b;
  if (HasFirstBound)
  {
    if (Inter.Bsup < Binf)
    {
      Interval PourSGI;
      return (PourSGI);
    }
    a = (Inter.Binf < Binf) ? Binf : Inter.Binf;
  }
  else
  {
    a = Inter.Binf;
  }

  if (HasLastBound)
  {
    if (Inter.Binf > Bsup)
    {
      Interval PourSGI;
      return (PourSGI);
    }
    b = (Inter.Bsup > Bsup) ? Bsup : Inter.Bsup;
  }
  else
  {
    b = Inter.Bsup;
  }
  return (Interval(a, b));
}
