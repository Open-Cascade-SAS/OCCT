// Created on: 1994-09-06
// Created by: Yves FRICAUD
// Copyright (c) 1994-1999 Matra Datavision
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

#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dLProp_CurAndInf2d.hxx>
#include <Geom2dLProp_NumericCurInf2d.pxx>
#include <LProp_AnalyticCurInf.hxx>
#include <NCollection_Array1.hxx>

//=================================================================================================

Geom2dLProp_CurAndInf2d::Geom2dLProp_CurAndInf2d()
    : isDone(false)
{
}

//=================================================================================================

void Geom2dLProp_CurAndInf2d::Perform(const occ::handle<Geom2d_Curve>& C)
{
  Clear();
  isDone = true;
  performCurExt(C);
  performInf(C);
}

//=================================================================================================

void Geom2dLProp_CurAndInf2d::PerformCurExt(const occ::handle<Geom2d_Curve>& C)
{
  Clear();
  isDone = true;
  performCurExt(C);
}

//=================================================================================================

void Geom2dLProp_CurAndInf2d::performCurExt(const occ::handle<Geom2d_Curve>& theCurve)
{
  Geom2dAdaptor_Curve     anAdaptor(theCurve);
  LProp_AnalyticCurInf    anAnalyticInf;
  Geom2dLProp_NumericCurInf2d aNumericInf;
  const GeomAbs_CurveType aCurveType = anAdaptor.GetType();

  switch (aCurveType)
  {
    case GeomAbs_Line:
      break;
    case GeomAbs_Circle:
      break;
    case GeomAbs_Ellipse:
      anAnalyticInf.Perform(aCurveType,
                            anAdaptor.FirstParameter(),
                            anAdaptor.LastParameter(),
                            *this);
      break;
    case GeomAbs_Hyperbola:
      anAnalyticInf.Perform(aCurveType,
                            anAdaptor.FirstParameter(),
                            anAdaptor.LastParameter(),
                            *this);
      break;
    case GeomAbs_Parabola:
      anAnalyticInf.Perform(aCurveType,
                            anAdaptor.FirstParameter(),
                            anAdaptor.LastParameter(),
                            *this);
      break;
    case GeomAbs_BSplineCurve:
      if (anAdaptor.Continuity() >= GeomAbs_C3)
      {
        aNumericInf.PerformCurExt(theCurve, *this);
        isDone = aNumericInf.IsDone();
      }
      else
      {
        isDone                                  = true;
        const int                  aNbIntervals = anAdaptor.NbIntervals(GeomAbs_C3);
        NCollection_Array1<double> aParams(1, aNbIntervals + 1);
        anAdaptor.Intervals(aParams, GeomAbs_C3);
        for (int i = 1; i <= aNbIntervals; i++)
        {
          aNumericInf.PerformCurExt(theCurve, aParams(i), aParams(i + 1), *this);
          if (!aNumericInf.IsDone())
          {
            isDone = false;
          }
        }
      }
      break;

    default: {
      aNumericInf.PerformCurExt(theCurve, *this);
      isDone = aNumericInf.IsDone();
    }
    break;
  }
}

//=================================================================================================

void Geom2dLProp_CurAndInf2d::PerformInf(const occ::handle<Geom2d_Curve>& C)
{
  Clear();
  isDone = true;
  performInf(C);
}

//=================================================================================================

void Geom2dLProp_CurAndInf2d::performInf(const occ::handle<Geom2d_Curve>& theCurve)
{
  Geom2dAdaptor_Curve     anAdaptor(theCurve);
  const GeomAbs_CurveType aCurveType = anAdaptor.GetType();
  Geom2dLProp_NumericCurInf2d aNumericInf;

  switch (aCurveType)
  {
    case GeomAbs_Line:
      break;
    case GeomAbs_Circle:
      break;
    case GeomAbs_Ellipse:
      break;
    case GeomAbs_Hyperbola:
      break;
    case GeomAbs_Parabola:
      break;
    case GeomAbs_BSplineCurve:
      if (anAdaptor.Continuity() >= GeomAbs_C3)
      {
        aNumericInf.PerformInf(theCurve, *this);
        isDone = aNumericInf.IsDone();
      }
      else
      {
        isDone                                  = true;
        const int                  aNbIntervals = anAdaptor.NbIntervals(GeomAbs_C3);
        NCollection_Array1<double> aParams(1, aNbIntervals + 1);
        anAdaptor.Intervals(aParams, GeomAbs_C3);

        for (int i = 1; i <= aNbIntervals; i++)
        {
          aNumericInf.PerformInf(theCurve, aParams(i), aParams(i + 1), *this);
          if (!aNumericInf.IsDone())
          {
            isDone = false;
          }
        }
      }
      break;

    default: {
      aNumericInf.PerformInf(theCurve, *this);
      isDone = aNumericInf.IsDone();
    }
    break;
  }
}

//=================================================================================================

bool Geom2dLProp_CurAndInf2d::IsDone() const
{
  return isDone;
}
