// Created on: 1992-06-10
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

#include <IntImpParGen.hxx>

#include <IntImpParGen_Tool.hxx>
#include <IntRes2d_Domain.hxx>
#include <IntRes2d_Position.hxx>
#include <IntRes2d_Transition.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>

#include <algorithm>
#include <cmath>

namespace
{
// A nonzero derivative defines a direction regardless of parameter speed.
// Scale before normalizing to avoid overflow and underflow.
//==================================================================================================

bool normalizedDirection(const gp_Vec2d& theVector, gp_Vec2d& theDirection)
{
  const double aScale = std::max(std::abs(theVector.X()), std::abs(theVector.Y()));
  if (aScale == 0.0)
  {
    return false;
  }
  theDirection.SetCoord(theVector.X() / aScale, theVector.Y() / aScale);
  theDirection.Normalize();
  return true;
}
} // namespace

#define TOLERANCE_ANGULAIRE 0.00000001

//----------------------------------------------------------------------
double IntImpParGen::NormalizeOnDomain(double& Param, const IntRes2d_Domain& TheDomain)
{
  double modParam = Param;
  if (TheDomain.IsClosed())
  {
    double Periode, t;
    TheDomain.EquivalentParameters(t, Periode);
    Periode -= t;
    while (modParam < TheDomain.FirstParameter() && modParam + Periode < TheDomain.LastParameter())
    {
      modParam += Periode;
    }
    while (modParam > TheDomain.LastParameter() && modParam - Periode > TheDomain.FirstParameter())
    {
      modParam -= Periode;
    }
  }
  return (modParam);
}

//----------------------------------------------------------------------
void IntImpParGen::DeterminePosition(IntRes2d_Position&     Pos1,
                                     const IntRes2d_Domain& TheDomain,
                                     const gp_Pnt2d&        Pnt1,
                                     const double           Param1)
{

  Pos1 = IntRes2d_Middle;

  if (TheDomain.HasFirstPoint())
  {
    if (Pnt1.Distance(TheDomain.FirstPoint()) <= TheDomain.FirstTolerance())
    {
      Pos1 = IntRes2d_Head;
    }
  }

  if (TheDomain.HasLastPoint())
  {
    if (Pnt1.Distance(TheDomain.LastPoint()) <= TheDomain.LastTolerance())
    {
      if (Pos1 == IntRes2d_Head)
      {
        if (std::abs(Param1 - TheDomain.LastParameter())
            < std::abs(Param1 - TheDomain.FirstParameter()))
        {
          Pos1 = IntRes2d_End;
        }
      }
      else
      {
        Pos1 = IntRes2d_End;
      }
    }
  }
}

//----------------------------------------------------------------------
void IntImpParGen::DetermineTransition(const IntRes2d_Position Pos1,
                                       gp_Vec2d&               Tan1,
                                       const gp_Vec2d&         Norm1,
                                       IntRes2d_Transition&    T1,
                                       const IntRes2d_Position Pos2,
                                       gp_Vec2d&               Tan2,
                                       const gp_Vec2d&         Norm2,
                                       IntRes2d_Transition&    T2,
                                       const double)
{

  bool courbure1 = true;
  bool courbure2 = true;
  bool decide    = true;

  T1.SetPosition(Pos1);
  T2.SetPosition(Pos2);

  gp_Vec2d aDirection1, aDirection2;
  // At an endpoint, D2 defines the tangent when D1 is within parameter resolution of zero.
  if ((Pos1 != IntRes2d_Middle
       && std::hypot(Tan1.X(), Tan1.Y())
            <= Precision::PConfusion() * std::hypot(Norm1.X(), Norm1.Y()))
      || !normalizedDirection(Tan1, aDirection1))
  {
    Tan1      = Norm1;
    courbure1 = false;
    if (!normalizedDirection(Tan1, aDirection1))
    { // transition undecided
      decide = false;
    }
  }

  if ((Pos2 != IntRes2d_Middle
       && std::hypot(Tan2.X(), Tan2.Y())
            <= Precision::PConfusion() * std::hypot(Norm2.X(), Norm2.Y()))
      || !normalizedDirection(Tan2, aDirection2))
  {
    Tan2      = Norm2;
    courbure2 = false;
    if (!normalizedDirection(Tan2, aDirection2))
    { // transition undecided
      decide = false;
    }
  }

  if (!decide)
  {
    T1.SetValue(Pos1);
    T2.SetValue(Pos2);
  }
  else
  {
    const double sgn = aDirection1.Crossed(aDirection2);

    if (std::abs(sgn) <= TOLERANCE_ANGULAIRE)
    { // Transition TOUCH #########
      bool opos = (aDirection1.Dot(aDirection2)) < 0;
      if (!(courbure1 || courbure2))
      {
        T1.SetValue(true, Pos1, IntRes2d_Unknown, opos);
        T2.SetValue(true, Pos2, IntRes2d_Unknown, opos);
      }
      else
      {
        gp_Vec2d Norm;
        Norm.SetCoord(-Tan1.Y(), Tan1.X());
        double Val1, Val2;
        if (!courbure1)
        {
          Val1 = 0.0;
        }
        else
        {
          Val1 = Norm.Dot(Norm1);
        }
        if (!courbure2)
        {
          Val2 = 0.0;
        }
        else
        {
          Val2 = Norm.Dot(Norm2);
        }

        if (std::abs(Val1 - Val2) <= TOLERANCE_ANGULAIRE)
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
    }
    else if (sgn < 0)
    {
      T1.SetValue(false, Pos1, IntRes2d_In);
      T2.SetValue(false, Pos2, IntRes2d_Out);
    }
    else
    { // sgn>0
      T1.SetValue(false, Pos1, IntRes2d_Out);
      T2.SetValue(false, Pos2, IntRes2d_In);
    }
  }
}

//----------------------------------------------------------------------
bool IntImpParGen::DetermineTransition(const IntRes2d_Position Pos1,
                                       gp_Vec2d&               Tan1,
                                       IntRes2d_Transition&    T1,
                                       const IntRes2d_Position Pos2,
                                       gp_Vec2d&               Tan2,
                                       IntRes2d_Transition&    T2,
                                       const double)
{

  T1.SetPosition(Pos1);
  T2.SetPosition(Pos2);

  gp_Vec2d aDirection1, aDirection2;
  if (!normalizedDirection(Tan1, aDirection1) || !normalizedDirection(Tan2, aDirection2))
  {
    return false;
  }

  const double sgn = aDirection1.Crossed(aDirection2);

  if (std::abs(sgn) <= TOLERANCE_ANGULAIRE)
  { // Transition TOUCH #########
    return (false);
  }
  else if (sgn < 0)
  {
    T1.SetValue(false, Pos1, IntRes2d_In);
    T2.SetValue(false, Pos2, IntRes2d_Out);
  }
  else
  { // sgn>0
    T1.SetValue(false, Pos1, IntRes2d_Out);
    T2.SetValue(false, Pos2, IntRes2d_In);
  }
  return (true);
}
