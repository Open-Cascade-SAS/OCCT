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

#ifndef IntCurve_IntConicConic_Tool_HeaderFile
#define IntCurve_IntConicConic_Tool_HeaderFile

#include <IntCurve_IntImpConicParConic.hxx>
// #include <IntCurve_IConicPConicTool.hxx>
#include <IntRes2d_Domain.hxx>
#include <IntRes2d_Transition.hxx>
#include <IntRes2d_Position.hxx>

static double PIpPI = M_PI + M_PI;

//======================================================================
//==========          P R O T O T Y P E S                   ============
//======================================================================

void Determine_Transition_LC(const IntRes2d_Position,
                             gp_Vec2d&,
                             const gp_Vec2d&,
                             IntRes2d_Transition&,
                             const IntRes2d_Position,
                             gp_Vec2d&,
                             const gp_Vec2d&,
                             IntRes2d_Transition&,
                             const double);
//======================================================================
double NormalizeOnCircleDomain(const double Param, const IntRes2d_Domain& Domain);

//=====================================================================
//====   C l a s s e     I n t e r v a l      !! Specifique !! ========
//=====================================================================
class Interval
{
public:
  double    Binf;
  double    Bsup;
  bool HasFirstBound;
  bool HasLastBound;
  bool IsNull;

  Interval();
  Interval(const double a, const double b);
  Interval(const IntRes2d_Domain& Domain);
  Interval(const double    a,
           const bool hf,
           const double    b,
           const bool hl);
  double Length();
  Interval      IntersectionWithBounded(const Interval& Inter);
};

//======================================================================
//==  C L A S S E    P E R I O D I C    I N T E R V A L  (Specifique)
//======================================================================
class PeriodicInterval
{
public:
  double    Binf;
  double    Bsup;
  bool isnull;

  void SetNull() { isnull = true; }

  bool IsNull() { return (isnull); }

  void Complement()
  {
    if (!isnull)
    {
      double t = Binf;
      Binf            = Bsup;
      Bsup            = t + PIpPI;
      if (Binf > PIpPI)
      {
        Binf -= PIpPI;
        Bsup -= PIpPI;
      }
    }
  }

  double Length() { return ((isnull) ? -100.0 : std::abs(Bsup - Binf)); }

  PeriodicInterval(const IntRes2d_Domain& Domain)
  {
    isnull = false;
    if (Domain.HasFirstPoint())
      Binf = Domain.FirstParameter();
    else
      Binf = -1.0;
    if (Domain.HasLastPoint())
      Bsup = Domain.LastParameter();
    else
      Bsup = 20.0;
  }

  PeriodicInterval()
  {
    isnull = true;
    Binf = Bsup = 0.0;
  }

  PeriodicInterval(const double a, const double b)
  {
    isnull = false;
    Binf   = a;
    Bsup   = b;
    if ((b - a) < PIpPI)
      this->Normalize();
  }

  void SetValues(const double a, const double b)
  {
    isnull = false;
    Binf   = a;
    Bsup   = b;
    if ((b - a) < PIpPI)
      this->Normalize();
  }

  void Normalize()
  {
    if (!isnull)
    {
      while (Binf > PIpPI)
        Binf -= PIpPI;
      while (Binf < 0.0)
        Binf += PIpPI;
      while (Bsup < Binf)
        Bsup += PIpPI;
      while (Bsup >= (Binf + PIpPI))
        Bsup -= PIpPI;
    }
  }

  PeriodicInterval FirstIntersection(PeriodicInterval& I1);
  PeriodicInterval SecondIntersection(PeriodicInterval& I2);
};

#endif
