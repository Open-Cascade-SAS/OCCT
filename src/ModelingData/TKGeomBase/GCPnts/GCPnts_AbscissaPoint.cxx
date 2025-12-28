// Created on: 1995-05-05
// Created by: Modelistation
// Copyright (c) 1995-1999 Matra Datavision
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

#include <GCPnts_AbscissaPoint.hxx>

#include <GCPnts_AbscissaType.hxx>
#include <GCPnts_TCurveTypes.hxx>
#include <Standard_ConstructionError.hxx>

//! Dimension independent used to implement GCPnts_AbscissaPoint
//! compute the type and the length ratio if GCPnts_LengthParametrized.
template <class TheCurve>
static GCPnts_AbscissaType computeType(const TheCurve& theC, double& theRatio)
{
  if (theC.NbIntervals(GeomAbs_CN) > 1)
  {
    return GCPnts_AbsComposite;
  }

  switch (theC.GetType())
  {
    case GeomAbs_Line: {
      theRatio = 1.0;
      return GCPnts_LengthParametrized;
    }
    case GeomAbs_Circle: {
      theRatio = theC.Circle().Radius();
      return GCPnts_LengthParametrized;
    }
    case GeomAbs_BezierCurve: {
      Handle(typename GCPnts_TCurveTypes<TheCurve>::BezierCurve) aBz = theC.Bezier();
      if (aBz->NbPoles() == 2 && !aBz->IsRational())
      {
        theRatio = aBz->DN(0, 1).Magnitude();
        return GCPnts_LengthParametrized;
      }
      return GCPnts_Parametrized;
    }
    case GeomAbs_BSplineCurve: {
      Handle(typename GCPnts_TCurveTypes<TheCurve>::BSplineCurve) aBs = theC.BSpline();
      if (aBs->NbPoles() == 2 && !aBs->IsRational())
      {
        theRatio = aBs->DN(aBs->FirstParameter(), 1).Magnitude();
        return GCPnts_LengthParametrized;
      }
      return GCPnts_Parametrized;
    }
    default: {
      return GCPnts_Parametrized;
    }
  }
}

//! Compute a point at distance theAbscis from parameter theU0 using theUi as initial guess
template <class TheCurve>
static void Compute(CPnts_AbscissaPoint& theComputer,
                    const TheCurve&      theC,
                    double&              theAbscis,
                    double&              theU0,
                    double&              theUi,
                    const double         theEPSILON)
{
  // test for easy solution
  if (std::abs(theAbscis) <= Precision::Confusion())
  {
    theComputer.SetParameter(theU0);
    return;
  }

  double                    aRatio = 1.0;
  const GCPnts_AbscissaType aType  = computeType(theC, aRatio);
  switch (aType)
  {
    case GCPnts_LengthParametrized: {
      theComputer.SetParameter(theU0 + theAbscis / aRatio);
      return;
    }
    case GCPnts_Parametrized: {
      theComputer.Init(theC);
      theComputer.Perform(theAbscis, theU0, theUi, theEPSILON);
      return;
    }
    case GCPnts_AbsComposite: {
      const int                  aNbIntervals = theC.NbIntervals(GeomAbs_CN);
      NCollection_Array1<double> aTI(1, aNbIntervals + 1);
      theC.Intervals(aTI, GeomAbs_CN);
      double aL = 0.0, aSign = 1.0;
      int    anIndex = 1;
      BSplCLib::Hunt(aTI, theU0, anIndex);
      int aDirection = 1;
      if (theAbscis < 0)
      {
        aDirection = 0;
        theAbscis  = -theAbscis;
        aSign      = -1.0;
      }

      while (anIndex >= 1 && anIndex <= aNbIntervals)
      {
        aL = CPnts_AbscissaPoint::Length(theC, theU0, aTI(anIndex + aDirection));
        if (std::abs(aL - theAbscis) <= Precision::Confusion())
        {
          theComputer.SetParameter(aTI(anIndex + aDirection));
          return;
        }

        if (aL > theAbscis)
        {
          if (theUi < aTI(anIndex) || theUi > aTI(anIndex + 1))
          {
            theUi = (theAbscis / aL) * (aTI(anIndex + 1) - theU0);
            if (aDirection)
            {
              theUi = theU0 + theUi;
            }
            else
            {
              theUi = theU0 - theUi;
            }
          }
          theComputer.Init(theC, aTI(anIndex), aTI(anIndex + 1));
          theComputer.Perform(aSign * theAbscis, theU0, theUi, theEPSILON);
          return;
        }
        else
        {
          theU0 = aTI(anIndex + aDirection);
          theAbscis -= aL;
        }
        if (aDirection)
        {
          ++anIndex;
        }
        else
        {
          --anIndex;
        }
      }

      // Push a little bit outside the limits (hairy !!!)
      theUi = theU0 + 0.1;
      theComputer.Init(theC, theU0, theU0 + 0.2);
      theComputer.Perform(aSign * theAbscis, theU0, theUi, theEPSILON);
      return;
    }
    break;
  }
}

//! Introduced by rbv for curvilinear parametrization
//! performs more appropriate tolerance management.
template <class TheCurve>
static void AdvCompute(CPnts_AbscissaPoint& theComputer,
                       const TheCurve&      theC,
                       double&              theAbscis,
                       double&              theU0,
                       double&              theUi,
                       const double         theEPSILON)
{
  double                    aRatio = 1.0;
  const GCPnts_AbscissaType aType  = computeType(theC, aRatio);
  switch (aType)
  {
    case GCPnts_LengthParametrized: {
      theComputer.SetParameter(theU0 + theAbscis / aRatio);
      return;
    }
    case GCPnts_Parametrized: {
      // theComputer.Init (theC);
      theComputer.Init(theC, theEPSILON); // rbv's modification
      theComputer.AdvPerform(theAbscis, theU0, theUi, theEPSILON);
      return;
    }
    case GCPnts_AbsComposite: {
      const int                  aNbIntervals = theC.NbIntervals(GeomAbs_CN);
      NCollection_Array1<double> aTI(1, aNbIntervals + 1);
      theC.Intervals(aTI, GeomAbs_CN);
      double aL = 0.0, aSign = 1.0;
      int    anIndex = 1;
      BSplCLib::Hunt(aTI, theU0, anIndex);

      int aDirection = 1;
      if (theAbscis < 0)
      {
        aDirection = 0;
        theAbscis  = -theAbscis;
        aSign      = -1.0;
      }

      if (anIndex == 0 && aDirection > 0)
      {
        aL = CPnts_AbscissaPoint::Length(theC, theU0, aTI(anIndex + aDirection), theEPSILON);
        if (std::abs(aL - theAbscis) <= /*Precision::Confusion()*/ theEPSILON)
        {
          theComputer.SetParameter(aTI(anIndex + aDirection));
          return;
        }

        if (aL > theAbscis)
        {
          if (theUi > aTI(anIndex + 1))
          {
            theUi = (theAbscis / aL) * (aTI(anIndex + 1) - theU0);
            theUi = theU0 + theUi;
          }
          theComputer.Init(theC, theU0, aTI(anIndex + 1), theEPSILON);
          theComputer.AdvPerform(aSign * theAbscis, theU0, theUi, theEPSILON);
          return;
        }
        else
        {
          theU0 = aTI(anIndex + aDirection);
          theAbscis -= aL;
        }
        ++anIndex;
      }

      while (anIndex >= 1 && anIndex <= aNbIntervals)
      {
        aL = CPnts_AbscissaPoint::Length(theC, theU0, aTI(anIndex + aDirection), theEPSILON);
        if (std::abs(aL - theAbscis) <= Precision::PConfusion())
        {
          theComputer.SetParameter(aTI(anIndex + aDirection));
          return;
        }

        if (aL > theAbscis)
        {
          if (theUi < aTI(anIndex) || theUi > aTI(anIndex + 1))
          {
            theUi = (theAbscis / aL) * (aTI(anIndex + 1) - theU0);
            if (aDirection)
            {
              theUi = theU0 + theUi;
            }
            else
            {
              theUi = theU0 - theUi;
            }
          }
          theComputer.Init(theC, aTI(anIndex), aTI(anIndex + 1), theEPSILON);
          theComputer.AdvPerform(aSign * theAbscis, theU0, theUi, theEPSILON);
          return;
        }
        else
        {
          theU0 = aTI(anIndex + aDirection);
          theAbscis -= aL;
        }
        if (aDirection)
        {
          ++anIndex;
        }
        else
        {
          --anIndex;
        }
      }

      // Push a little bit outside the limits (hairy !!!)
      const bool isNonPeriodic = !theC.IsPeriodic();
      theUi                    = theU0 + aSign * 0.1;
      double aU1               = theU0 + aSign * 0.2;
      if (isNonPeriodic)
      {
        if (aSign > 0)
        {
          theUi = std::min(theUi, theC.LastParameter());
          aU1   = std::min(aU1, theC.LastParameter());
        }
        else
        {
          theUi = std::max(theUi, theC.FirstParameter());
          aU1   = std::max(aU1, theC.FirstParameter());
        }
      }

      theComputer.Init(theC, theU0, aU1, theEPSILON);
      theComputer.AdvPerform(aSign * theAbscis, theU0, theUi, theEPSILON);
      return;
    }
    break;
  }
}

//=================================================================================================

GCPnts_AbscissaPoint::GCPnts_AbscissaPoint()
{
  //
}

//=================================================================================================

double GCPnts_AbscissaPoint::Length(const Adaptor3d_Curve& theC)
{
  return GCPnts_AbscissaPoint::Length(theC, theC.FirstParameter(), theC.LastParameter());
}

//=================================================================================================

double GCPnts_AbscissaPoint::Length(const Adaptor2d_Curve2d& theC)
{
  return GCPnts_AbscissaPoint::Length(theC, theC.FirstParameter(), theC.LastParameter());
}

//=================================================================================================

double GCPnts_AbscissaPoint::Length(const Adaptor3d_Curve& theC, const double theTol)
{
  return GCPnts_AbscissaPoint::Length(theC, theC.FirstParameter(), theC.LastParameter(), theTol);
}

//=================================================================================================

double GCPnts_AbscissaPoint::Length(const Adaptor2d_Curve2d& theC, const double theTol)
{
  return GCPnts_AbscissaPoint::Length(theC, theC.FirstParameter(), theC.LastParameter(), theTol);
}

//=================================================================================================

double GCPnts_AbscissaPoint::Length(const Adaptor3d_Curve& theC,
                                    const double           theU1,
                                    const double           theU2)
{
  return length(theC, theU1, theU2, nullptr);
}

//=================================================================================================

double GCPnts_AbscissaPoint::Length(const Adaptor2d_Curve2d& theC,
                                    const double             theU1,
                                    const double             theU2)
{
  return length(theC, theU1, theU2, nullptr);
}

//=================================================================================================

double GCPnts_AbscissaPoint::Length(const Adaptor3d_Curve& theC,
                                    const double           theU1,
                                    const double           theU2,
                                    const double           theTol)
{
  return length(theC, theU1, theU2, &theTol);
}

//=================================================================================================

double GCPnts_AbscissaPoint::Length(const Adaptor2d_Curve2d& theC,
                                    const double             theU1,
                                    const double             theU2,
                                    const double             theTol)
{
  return length(theC, theU1, theU2, &theTol);
}

//=================================================================================================

template <class TheCurve>
double GCPnts_AbscissaPoint::length(const TheCurve& theC,
                                    const double    theU1,
                                    const double    theU2,
                                    const double*   theTol)
{
  double                    aRatio = 1.0;
  const GCPnts_AbscissaType aType  = computeType(theC, aRatio);
  switch (aType)
  {
    case GCPnts_LengthParametrized: {
      return std::abs(theU2 - theU1) * aRatio;
    }
    case GCPnts_Parametrized: {
      return theTol != nullptr ? CPnts_AbscissaPoint::Length(theC, theU1, theU2, *theTol)
                            : CPnts_AbscissaPoint::Length(theC, theU1, theU2);
    }
    case GCPnts_AbsComposite: {
      const int                  aNbIntervals = theC.NbIntervals(GeomAbs_CN);
      NCollection_Array1<double> aTI(1, aNbIntervals + 1);
      theC.Intervals(aTI, GeomAbs_CN);
      const double aUU1 = std::min(theU1, theU2);
      const double aUU2 = std::max(theU1, theU2);
      double       aL   = 0.0;
      for (int anIndex = 1; anIndex <= aNbIntervals; ++anIndex)
      {
        if (aTI(anIndex) > aUU2)
        {
          break;
        }
        if (aTI(anIndex + 1) < aUU1)
        {
          continue;
        }
        if (theTol != nullptr)
        {
          aL += CPnts_AbscissaPoint::Length(theC,
                                            std::max(aTI(anIndex), aUU1),
                                            std::min(aTI(anIndex + 1), aUU2),
                                            *theTol);
        }
        else
        {
          aL += CPnts_AbscissaPoint::Length(theC,
                                            std::max(aTI(anIndex), aUU1),
                                            std::min(aTI(anIndex + 1), aUU2));
        }
      }
      return aL;
    }
  }
  return RealLast();
}

//=================================================================================================

template <class TheCurve>
void GCPnts_AbscissaPoint::compute(const TheCurve& theC,
                                   const double    theAbscissa,
                                   const double    theU0)
{
  const double aL = GCPnts_AbscissaPoint::Length(theC);
  if (aL < Precision::Confusion())
  {
    throw Standard_ConstructionError();
  }

  double anAbscis = theAbscissa;
  double aUU0     = theU0;
  double aUUi     = theU0 + (anAbscis / aL) * (theC.LastParameter() - theC.FirstParameter());
  Compute(myComputer, theC, anAbscis, aUU0, aUUi, theC.Resolution(Precision::Confusion()));
}

//=================================================================================================

GCPnts_AbscissaPoint::GCPnts_AbscissaPoint(const Adaptor3d_Curve& theC,
                                           const double           theAbscissa,
                                           const double           theU0)
{
  compute(theC, theAbscissa, theU0);
}

//=================================================================================================

GCPnts_AbscissaPoint::GCPnts_AbscissaPoint(const Adaptor2d_Curve2d& theC,
                                           const double             theAbscissa,
                                           const double             theU0)
{
  compute(theC, theAbscissa, theU0);
}

//=================================================================================================

template <class TheCurve>
void GCPnts_AbscissaPoint::advCompute(const double    theTol,
                                      const TheCurve& theC,
                                      const double    theAbscissa,
                                      const double    theU0)
{
  const double aL = GCPnts_AbscissaPoint::Length(theC, theTol);
  /*if (aL < Precision::Confusion())
  {
    throw Standard_ConstructionError ("GCPnts_AbscissaPoint::GCPnts_AbscissaPoint");
  }*/
  double anAbscis = theAbscissa;
  double aUU0     = theU0;
  double aUUi     = 0.0;
  if (aL >= Precision::Confusion())
  {
    aUUi = theU0 + (anAbscis / aL) * (theC.LastParameter() - theC.FirstParameter());
  }
  else
  {
    aUUi = theU0;
  }
  AdvCompute(myComputer, theC, anAbscis, aUU0, aUUi, theTol);
}

//=================================================================================================

GCPnts_AbscissaPoint::GCPnts_AbscissaPoint(const double           theTol,
                                           const Adaptor3d_Curve& theC,
                                           const double           theAbscissa,
                                           const double           theU0)
{
  advCompute(theTol, theC, theAbscissa, theU0);
}

//=================================================================================================

GCPnts_AbscissaPoint::GCPnts_AbscissaPoint(const double             theTol,
                                           const Adaptor2d_Curve2d& theC,
                                           const double             theAbscissa,
                                           const double             theU0)
{
  advCompute(theTol, theC, theAbscissa, theU0);
}

//=================================================================================================

GCPnts_AbscissaPoint::GCPnts_AbscissaPoint(const Adaptor3d_Curve& theC,
                                           const double           theAbscissa,
                                           const double           theU0,
                                           const double           theUi)
{
  double anAbscis = theAbscissa, aUU0 = theU0, aUUi = theUi;
  Compute(myComputer, theC, anAbscis, aUU0, aUUi, theC.Resolution(Precision::Confusion()));
}

//=================================================================================================

GCPnts_AbscissaPoint::GCPnts_AbscissaPoint(const Adaptor2d_Curve2d& theC,
                                           const double             theAbscissa,
                                           const double             theU0,
                                           const double             theUi)
{
  double anAbscis = theAbscissa, aUU0 = theU0, aUUi = theUi;
  Compute(myComputer, theC, anAbscis, aUU0, aUUi, theC.Resolution(Precision::Confusion()));
}

//=================================================================================================

GCPnts_AbscissaPoint::GCPnts_AbscissaPoint(const Adaptor3d_Curve& theC,
                                           const double           theAbscissa,
                                           const double           theU0,
                                           const double           theUi,
                                           const double           theTol)
{
  double anAbscis = theAbscissa, aUU0 = theU0, aUUi = theUi;
  AdvCompute(myComputer, theC, anAbscis, aUU0, aUUi, theTol);
}

//=================================================================================================

GCPnts_AbscissaPoint::GCPnts_AbscissaPoint(const Adaptor2d_Curve2d& theC,
                                           const double             theAbscissa,
                                           const double             theU0,
                                           const double             theUi,
                                           const double             theTol)
{
  double anAbscis = theAbscissa, aUU0 = theU0, aUUi = theUi;
  AdvCompute(myComputer, theC, anAbscis, aUU0, aUUi, theTol);
}
