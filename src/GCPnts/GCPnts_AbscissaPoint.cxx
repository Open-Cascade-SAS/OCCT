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
//! compute the type  and the length ratio if GCPnts_LengthParametrized.
template<class TheCurve>
static GCPnts_AbscissaType computeType (const TheCurve& theC,
                                        Standard_Real& theRatio)
{
  if (theC.NbIntervals (GeomAbs_CN) > 1)
  {
    return GCPnts_AbsComposite;
  }

  switch (theC.GetType())
  {
    case GeomAbs_Line:
    {
      theRatio = 1.0;
      return GCPnts_LengthParametrized;
    }
    case GeomAbs_Circle:
    {
      theRatio = theC.Circle().Radius();
      return GCPnts_LengthParametrized;
    }
    case GeomAbs_BezierCurve:
    {
      Handle(typename GCPnts_TCurveTypes<TheCurve>::BezierCurve) aBz = theC.Bezier();
      if (aBz->NbPoles() == 2
      && !aBz->IsRational())
      {
        theRatio = aBz->DN (0, 1).Magnitude();
        return GCPnts_LengthParametrized;
      }
      return GCPnts_Parametrized;
    }
    case GeomAbs_BSplineCurve:
    {
      Handle(typename GCPnts_TCurveTypes<TheCurve>::BSplineCurve) aBs = theC.BSpline();
      if (aBs->NbPoles() == 2
      && !aBs->IsRational())
      {
        theRatio = aBs->DN (aBs->FirstParameter(), 1).Magnitude();
        return GCPnts_LengthParametrized;
      }
      return GCPnts_Parametrized;
    }
    default:
    {
      return GCPnts_Parametrized;
    }
  }
}

//! Compute a point at distance theAbscis from parameter theU0 using theUi as initial guess
template<class TheCurve>
static void Compute (CPnts_AbscissaPoint& theComputer,
                     const TheCurve& theC,
                     Standard_Real& theAbscis,
                     Standard_Real& theU0,
                     Standard_Real& theUi,
                     const Standard_Real theEPSILON)
{
  // test for easy solution
  if (Abs (theAbscis) <= Precision::Confusion())
  {
    theComputer.SetParameter (theU0);
    return;
  }

  Standard_Real aRatio = 1.0;
  const GCPnts_AbscissaType aType = computeType (theC, aRatio);
  switch (aType)
  {
    case GCPnts_LengthParametrized:
    {
      theComputer.SetParameter (theU0 + theAbscis / aRatio);
      return;
    }
    case GCPnts_Parametrized:
    {
      theComputer.Init (theC);
      theComputer.Perform (theAbscis, theU0, theUi, theEPSILON);
      return;
    }
    case GCPnts_AbsComposite:
    {
      const Standard_Integer aNbIntervals = theC.NbIntervals (GeomAbs_CN);
      TColStd_Array1OfReal aTI (1, aNbIntervals + 1);
      theC.Intervals (aTI, GeomAbs_CN);
      Standard_Real aL = 0.0, aSign = 1.0;
      Standard_Integer anIndex = 1;
      BSplCLib::Hunt (aTI, theU0, anIndex);
      Standard_Integer aDirection = 1;
      if (theAbscis < 0)
      {
        aDirection = 0;
        theAbscis = -theAbscis;
        aSign = -1.0;
      }

      while (anIndex >= 1
          && anIndex <= aNbIntervals)
      {
        aL = CPnts_AbscissaPoint::Length (theC, theU0, aTI (anIndex + aDirection));
        if (Abs (aL - theAbscis) <= Precision::Confusion())
        {
          theComputer.SetParameter (aTI (anIndex + aDirection));
          return;
        }

        if (aL > theAbscis)
        {
          if (theUi < aTI (anIndex)
           || theUi > aTI (anIndex + 1))
          {
            theUi = (theAbscis / aL) * (aTI (anIndex + 1) - theU0);
            if (aDirection)
            {
              theUi = theU0 + theUi;
            }
            else
            {
              theUi = theU0 - theUi;
            }
          }
          theComputer.Init (theC, aTI (anIndex), aTI (anIndex + 1));
          theComputer.Perform (aSign * theAbscis, theU0, theUi, theEPSILON);
          return;
        }
        else
        {
          theU0 = aTI (anIndex + aDirection);
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
      theComputer.Init (theC, theU0, theU0 + 0.2);
      theComputer.Perform (aSign * theAbscis, theU0, theUi, theEPSILON);
      return;
    }
    break;
  }
}

//! Introduced by rbv for curvilinear parametrization
//! performs more appropriate tolerance management.
template<class TheCurve>
static void AdvCompute (CPnts_AbscissaPoint& theComputer,
                        const TheCurve& theC,
                        Standard_Real& theAbscis,
                        Standard_Real& theU0,
                        Standard_Real& theUi,
                        const Standard_Real theEPSILON)
{
  Standard_Real aRatio = 1.0;
  const GCPnts_AbscissaType aType = computeType (theC, aRatio);
  switch (aType)
  {
    case GCPnts_LengthParametrized:
    {
      theComputer.SetParameter (theU0 + theAbscis / aRatio);
      return;
    }
    case GCPnts_Parametrized:
    {
      // theComputer.Init (theC);
      theComputer.Init (theC, theEPSILON); //rbv's modification
      theComputer.AdvPerform (theAbscis, theU0, theUi, theEPSILON);
      return;
    }
    case GCPnts_AbsComposite:
    {
      const Standard_Integer aNbIntervals = theC.NbIntervals (GeomAbs_CN);
      TColStd_Array1OfReal aTI (1, aNbIntervals + 1);
      theC.Intervals (aTI, GeomAbs_CN);
      Standard_Real aL = 0.0, aSign = 1.0;
      Standard_Integer anIndex = 1;
      BSplCLib::Hunt (aTI, theU0, anIndex);

      Standard_Integer aDirection = 1;
      if (theAbscis < 0)
      {
        aDirection = 0;
        theAbscis = -theAbscis;
        aSign = -1.0;
      }

      if (anIndex == 0 && aDirection > 0)
      {
        aL = CPnts_AbscissaPoint::Length (theC, theU0, aTI (anIndex + aDirection), theEPSILON);
        if (Abs (aL - theAbscis) <= /*Precision::Confusion()*/theEPSILON)
        {
          theComputer.SetParameter (aTI (anIndex + aDirection));
          return;
        }

        if (aL > theAbscis)
        {
          if (theUi > aTI (anIndex + 1))
          {
            theUi = (theAbscis / aL) * (aTI (anIndex + 1) - theU0);
            theUi = theU0 + theUi;
          }
          theComputer.Init (theC, theU0, aTI (anIndex + 1), theEPSILON);
          theComputer.AdvPerform (aSign * theAbscis, theU0, theUi, theEPSILON);
          return;
        }
        else
        {
          theU0 = aTI (anIndex + aDirection);
          theAbscis -= aL;
        }
        ++anIndex;
      }

      while (anIndex >= 1
          && anIndex <= aNbIntervals)
      {
        aL = CPnts_AbscissaPoint::Length (theC, theU0, aTI (anIndex + aDirection), theEPSILON);
        if (Abs (aL - theAbscis) <= Precision::PConfusion())
        {
          theComputer.SetParameter (aTI (anIndex + aDirection));
          return;
        }

        if (aL > theAbscis)
        {
          if (theUi < aTI (anIndex)
           || theUi > aTI (anIndex + 1))
          {
            theUi = (theAbscis / aL) * (aTI (anIndex + 1) - theU0);
            if (aDirection)
            {
              theUi = theU0 + theUi;
            }
            else
            {
              theUi = theU0 - theUi;
            }
          }
          theComputer.Init (theC, aTI (anIndex), aTI (anIndex + 1), theEPSILON);
          theComputer.AdvPerform (aSign * theAbscis, theU0, theUi, theEPSILON);
          return;
        }
        else
        {
          theU0 = aTI (anIndex + aDirection);
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
      const Standard_Boolean isNonPeriodic = !theC.IsPeriodic();
      theUi = theU0 + aSign * 0.1;
      Standard_Real aU1 = theU0 + aSign * 0.2;
      if (isNonPeriodic)
      {
        if (aSign > 0)
        {
          theUi = Min (theUi, theC.LastParameter());
          aU1   = Min (aU1,   theC.LastParameter());
        }
        else
        {
          theUi = Max (theUi, theC.FirstParameter());
          aU1   = Max (aU1,   theC.FirstParameter());
        }
      }

      theComputer.Init (theC, theU0, aU1, theEPSILON);
      theComputer.AdvPerform (aSign * theAbscis, theU0, theUi, theEPSILON);
      return;
    }
    break;
  }
}

//=======================================================================
//function : GCPnts_AbscissaPoint
//purpose  :
//=======================================================================
GCPnts_AbscissaPoint::GCPnts_AbscissaPoint() 
{
  //
}

//=======================================================================
//function : Length
//purpose  :
//=======================================================================
Standard_Real GCPnts_AbscissaPoint::Length (const Adaptor3d_Curve& theC)
{
  return GCPnts_AbscissaPoint::Length (theC, theC.FirstParameter(), theC.LastParameter());
}

//=======================================================================
//function : Length
//purpose  :
//=======================================================================
Standard_Real GCPnts_AbscissaPoint::Length (const Adaptor2d_Curve2d& theC)
{
  return GCPnts_AbscissaPoint::Length (theC, theC.FirstParameter(), theC.LastParameter());
}

//=======================================================================
//function : Length
//purpose  :
//=======================================================================
Standard_Real GCPnts_AbscissaPoint::Length (const Adaptor3d_Curve& theC,
                                            const Standard_Real theTol)
{
  return GCPnts_AbscissaPoint::Length (theC, theC.FirstParameter(), theC.LastParameter(), theTol);
}

//=======================================================================
//function : Length
//purpose  :
//=======================================================================
Standard_Real GCPnts_AbscissaPoint::Length (const Adaptor2d_Curve2d& theC,
                                            const Standard_Real theTol)
{
  return GCPnts_AbscissaPoint::Length (theC, theC.FirstParameter(), theC.LastParameter(), theTol);
}

//=======================================================================
//function : Length
//purpose  :
//=======================================================================
Standard_Real GCPnts_AbscissaPoint::Length (const Adaptor3d_Curve& theC,
                                            const Standard_Real theU1, const Standard_Real theU2)
{
  return length (theC, theU1, theU2, NULL);
}

//=======================================================================
//function : Length
//purpose  :
//=======================================================================
Standard_Real GCPnts_AbscissaPoint::Length (const Adaptor2d_Curve2d& theC,
                                            const Standard_Real theU1, const Standard_Real theU2)
{
  return length (theC, theU1, theU2, NULL);
}

//=======================================================================
//function : Length
//purpose  :
//=======================================================================
Standard_Real GCPnts_AbscissaPoint::Length (const Adaptor3d_Curve& theC,
                                            const Standard_Real theU1, const Standard_Real theU2,
                                            const Standard_Real theTol)
{
  return length (theC, theU1, theU2, &theTol);
}

//=======================================================================
//function : Length
//purpose  :
//=======================================================================
Standard_Real GCPnts_AbscissaPoint::Length (const Adaptor2d_Curve2d& theC,
                                            const Standard_Real theU1, const Standard_Real theU2,
                                            const Standard_Real theTol)
{
  return length (theC, theU1, theU2, &theTol);
}

//=======================================================================
//function : length
//purpose  :
//=======================================================================
template<class TheCurve>
Standard_Real GCPnts_AbscissaPoint::length (const TheCurve& theC,
                                            const Standard_Real theU1, const Standard_Real theU2,
                                            const Standard_Real* theTol)
{
  Standard_Real aRatio = 1.0;
  const GCPnts_AbscissaType aType = computeType (theC, aRatio);
  switch (aType)
  {
    case GCPnts_LengthParametrized:
    {
      return Abs (theU2 - theU1) * aRatio;
    }
    case GCPnts_Parametrized:
    {
      return theTol != NULL
           ? CPnts_AbscissaPoint::Length (theC, theU1, theU2, *theTol)
           : CPnts_AbscissaPoint::Length (theC, theU1, theU2);
    }
    case GCPnts_AbsComposite:
    {
      const Standard_Integer aNbIntervals = theC.NbIntervals (GeomAbs_CN);
      TColStd_Array1OfReal aTI (1, aNbIntervals + 1);
      theC.Intervals (aTI, GeomAbs_CN);
      const Standard_Real aUU1 = Min (theU1, theU2);
      const Standard_Real aUU2 = Max (theU1, theU2);
      Standard_Real aL = 0.0;
      for (Standard_Integer anIndex = 1; anIndex <= aNbIntervals; ++anIndex)
      {
        if (aTI (anIndex)     > aUU2) { break; }
        if (aTI (anIndex + 1) < aUU1) { continue; }
        if (theTol != NULL)
        {
          aL += CPnts_AbscissaPoint::Length (theC,
                                             Max (aTI (anIndex),     aUU1),
                                             Min (aTI (anIndex + 1), aUU2),
                                             *theTol);
        }
        else
        {
          aL += CPnts_AbscissaPoint::Length (theC,
                                             Max (aTI (anIndex),     aUU1),
                                             Min (aTI (anIndex + 1), aUU2));
        }
      }
      return aL;
    }
  }
  return RealLast();
}

//=======================================================================
//function : compute
//purpose  :
//=======================================================================
template<class TheCurve>
void GCPnts_AbscissaPoint::compute (const TheCurve& theC,
                                    const Standard_Real theAbscissa,
                                    const Standard_Real theU0)
{
  const Standard_Real aL = GCPnts_AbscissaPoint::Length (theC);
  if (aL < Precision::Confusion())
  {
    throw Standard_ConstructionError();
  }

  Standard_Real anAbscis = theAbscissa;
  Standard_Real aUU0 = theU0;
  Standard_Real aUUi = theU0 + (anAbscis / aL) * (theC.LastParameter() - theC.FirstParameter());
  Compute (myComputer, theC, anAbscis, aUU0, aUUi,
           theC.Resolution (Precision::Confusion()));
}

//=======================================================================
//function : GCPnts_AbscissaPoint
//purpose  :
//=======================================================================
GCPnts_AbscissaPoint::GCPnts_AbscissaPoint (const Adaptor3d_Curve& theC,
                                            const Standard_Real theAbscissa,
                                            const Standard_Real theU0)
{
  compute (theC, theAbscissa, theU0);
}

//=======================================================================
//function : GCPnts_AbscissaPoint
//purpose  :
//=======================================================================
GCPnts_AbscissaPoint::GCPnts_AbscissaPoint (const Adaptor2d_Curve2d& theC,
                                            const Standard_Real theAbscissa,
                                            const Standard_Real theU0)
{
  compute (theC, theAbscissa, theU0);
}

//=======================================================================
//function : advCompute
//purpose  :
//=======================================================================
template<class TheCurve>
void GCPnts_AbscissaPoint::advCompute (const Standard_Real theTol,
                                       const TheCurve& theC,
                                       const Standard_Real theAbscissa,
                                       const Standard_Real theU0)
{
  const Standard_Real aL = GCPnts_AbscissaPoint::Length (theC, theTol);
  /*if (aL < Precision::Confusion())
  {
    throw Standard_ConstructionError ("GCPnts_AbscissaPoint::GCPnts_AbscissaPoint");
  }*/
  Standard_Real anAbscis = theAbscissa;
  Standard_Real aUU0 = theU0;
  Standard_Real aUUi = 0.0;
  if (aL >= Precision::Confusion())
  {
    aUUi= theU0 + (anAbscis / aL) * (theC.LastParameter() - theC.FirstParameter());
  }
  else
  {
    aUUi = theU0;
  }
  AdvCompute (myComputer, theC, anAbscis, aUU0, aUUi, theTol);
}

//=======================================================================
//function : GCPnts_AbscissaPoint
//purpose  :
//=======================================================================
GCPnts_AbscissaPoint::GCPnts_AbscissaPoint (const Standard_Real theTol,
                                            const Adaptor3d_Curve& theC,
                                            const Standard_Real theAbscissa,
                                            const Standard_Real theU0)
{
  advCompute (theTol, theC, theAbscissa, theU0);
}

//=======================================================================
//function : GCPnts_AbscissaPoint
//purpose  :
//=======================================================================
GCPnts_AbscissaPoint::GCPnts_AbscissaPoint (const Standard_Real theTol,
                                            const Adaptor2d_Curve2d& theC,
                                            const Standard_Real theAbscissa,
                                            const Standard_Real theU0)
{
  advCompute (theTol, theC, theAbscissa, theU0);
}

//=======================================================================
//function : GCPnts_AbscissaPoint
//purpose  :
//=======================================================================
GCPnts_AbscissaPoint::GCPnts_AbscissaPoint (const Adaptor3d_Curve& theC,
                                            const Standard_Real theAbscissa,
                                            const Standard_Real theU0, const Standard_Real theUi)
{
  Standard_Real anAbscis = theAbscissa, aUU0 = theU0, aUUi = theUi;
  Compute (myComputer, theC, anAbscis, aUU0, aUUi, theC.Resolution (Precision::Confusion()));
}

//=======================================================================
//function : GCPnts_AbscissaPoint
//purpose  :
//=======================================================================
GCPnts_AbscissaPoint::GCPnts_AbscissaPoint (const Adaptor2d_Curve2d& theC,
                                            const Standard_Real theAbscissa,
                                            const Standard_Real theU0, const Standard_Real theUi)
{
  Standard_Real anAbscis = theAbscissa, aUU0 = theU0, aUUi = theUi;
  Compute (myComputer, theC, anAbscis, aUU0, aUUi, theC.Resolution (Precision::Confusion()));
}

//=======================================================================
//function : GCPnts_AbscissaPoint
//purpose  :
//=======================================================================
GCPnts_AbscissaPoint::GCPnts_AbscissaPoint (const Adaptor3d_Curve& theC,
                                            const Standard_Real theAbscissa,
                                            const Standard_Real theU0, const Standard_Real theUi,
                                            const Standard_Real theTol)
{
  Standard_Real anAbscis = theAbscissa, aUU0 = theU0, aUUi = theUi;
  AdvCompute (myComputer, theC, anAbscis, aUU0, aUUi, theTol);
}

//=======================================================================
//function : GCPnts_AbscissaPoint
//purpose  :
//=======================================================================
GCPnts_AbscissaPoint::GCPnts_AbscissaPoint (const Adaptor2d_Curve2d& theC,
                                            const Standard_Real theAbscissa,
                                            const Standard_Real theU0, const Standard_Real theUi,
                                            const Standard_Real theTol)
{
  Standard_Real anAbscis = theAbscissa, aUU0 = theU0, aUUi = theUi;
  AdvCompute (myComputer, theC, anAbscis, aUU0, aUUi, theTol);
}
