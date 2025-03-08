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

#include <GCPnts_UniformDeflection.hxx>

#include <CPnts_UniformDeflection.hxx>
#include <GCPnts_DeflectionType.hxx>
#include <GCPnts_TCurveTypes.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_NotImplemented.hxx>
#include <StdFail_NotDone.hxx>

// mask the return of a Adaptor2d_Curve2d as a gp_Pnt
static gp_Pnt Value(const Adaptor3d_Curve& theC, const Standard_Real theParameter)
{
  return theC.Value(theParameter);
}

static gp_Pnt Value(const Adaptor2d_Curve2d& theC, const Standard_Real theParameter)
{
  const gp_Pnt2d a2dPoint = theC.Value(theParameter);
  return gp_Pnt(a2dPoint.X(), a2dPoint.Y(), 0.0);
}

//=================================================================================================

GCPnts_UniformDeflection::GCPnts_UniformDeflection()
    : myDone(Standard_False),
      myDeflection(0.0)
{
  //
}

//=================================================================================================

GCPnts_UniformDeflection::GCPnts_UniformDeflection(const Adaptor3d_Curve& theC,
                                                   const Standard_Real    theDeflection,
                                                   const Standard_Real    theU1,
                                                   const Standard_Real    theU2,
                                                   const Standard_Boolean theWithControl)
    : myDone(Standard_False),
      myDeflection(theDeflection)
{
  Initialize(theC, theDeflection, theU1, theU2, theWithControl);
}

//=================================================================================================

GCPnts_UniformDeflection::GCPnts_UniformDeflection(const Adaptor3d_Curve& theC,
                                                   const Standard_Real    theDeflection,
                                                   const Standard_Boolean theWithControl)
    : myDone(Standard_False),
      myDeflection(theDeflection)
{
  Initialize(theC, theDeflection, theWithControl);
}

//=================================================================================================

GCPnts_UniformDeflection::GCPnts_UniformDeflection(const Adaptor2d_Curve2d& theC,
                                                   const Standard_Real      theDeflection,
                                                   const Standard_Real      theU1,
                                                   const Standard_Real      theU2,
                                                   const Standard_Boolean   theWithControl)
    : myDone(Standard_False),
      myDeflection(theDeflection)
{
  Initialize(theC, theDeflection, theU1, theU2, theWithControl);
}

//=================================================================================================

GCPnts_UniformDeflection::GCPnts_UniformDeflection(const Adaptor2d_Curve2d& theC,
                                                   const Standard_Real      theDeflection,
                                                   const Standard_Boolean   theWithControl)
    : myDone(Standard_False),
      myDeflection(theDeflection)
{
  Initialize(theC, theDeflection, theWithControl);
}

//=================================================================================================

void GCPnts_UniformDeflection::Initialize(const Adaptor3d_Curve& theC,
                                          const Standard_Real    theDeflection,
                                          const Standard_Boolean theWithControl)
{
  Initialize(theC, theDeflection, theC.FirstParameter(), theC.LastParameter(), theWithControl);
}

//=================================================================================================

void GCPnts_UniformDeflection::Initialize(const Adaptor2d_Curve2d& theC,
                                          const Standard_Real      theDeflection,
                                          const Standard_Boolean   theWithControl)
{
  Initialize(theC, theDeflection, theC.FirstParameter(), theC.LastParameter(), theWithControl);
}

//=================================================================================================

void GCPnts_UniformDeflection::Initialize(const Adaptor3d_Curve& theC,
                                          const Standard_Real    theDeflection,
                                          const Standard_Real    theU1,
                                          const Standard_Real    theU2,
                                          const Standard_Boolean theWithControl)
{
  initialize(theC, theDeflection, theU1, theU2, theWithControl);
}

//=================================================================================================

void GCPnts_UniformDeflection::Initialize(const Adaptor2d_Curve2d& theC,
                                          const Standard_Real      theDeflection,
                                          const Standard_Real      theU1,
                                          const Standard_Real      theU2,
                                          const Standard_Boolean   theWithControl)
{
  initialize(theC, theDeflection, theU1, theU2, theWithControl);
}

//=================================================================================================

gp_Pnt GCPnts_UniformDeflection::Value(const Standard_Integer theIndex) const
{
  StdFail_NotDone_Raise_if(!myDone, "GCPnts_UniformAbscissa::Parameter()");
  return myPoints.Value(theIndex);
}

//! Control of the last points.
template <class TheCurve>
static void Controle(const TheCurve&         theC,
                     TColStd_SequenceOfReal& theParameters,
                     TColgp_SequenceOfPnt&   thePoints,
                     const Standard_Real     theU2)
{
  const Standard_Integer aNbPnts = thePoints.Length();
  if (aNbPnts > 2)
  {
    const Standard_Real aUa = theParameters(aNbPnts - 2);
    const Standard_Real aUb = theParameters(aNbPnts - 1);
    if (theU2 - aUb < 0.33 * (theU2 - aUa))
    {
      const Standard_Real aUc    = (theU2 + aUa) * 0.5;
      theParameters(aNbPnts - 1) = aUc;
      thePoints(aNbPnts - 1)     = Value(theC, aUc);
    }
  }
}

//=================================================================================================

template <class TheCurve>
static Standard_Boolean PerformLinear(const TheCurve&         theC,
                                      TColStd_SequenceOfReal& theParameters,
                                      TColgp_SequenceOfPnt&   thePoints,
                                      const Standard_Real     theU1,
                                      const Standard_Real     theU2)
{
  theParameters.Append(theU1);
  gp_Pnt aPoint = Value(theC, theU1);
  thePoints.Append(aPoint);

  theParameters.Append(theU2);
  aPoint = Value(theC, theU2);
  thePoints.Append(aPoint);
  return Standard_True;
}

//=================================================================================================

template <class TheCurve>
static Standard_Boolean PerformCircular(const TheCurve&         theC,
                                        TColStd_SequenceOfReal& theParameters,
                                        TColgp_SequenceOfPnt&   thePoints,
                                        const Standard_Real     theDeflection,
                                        const Standard_Real     theU1,
                                        const Standard_Real     theU2)
{
  gp_Pnt        aPoint;
  Standard_Real anAngle      = Max(1.0 - (theDeflection / theC.Circle().Radius()), 0.0);
  anAngle                    = 2.0e0 * ACos(anAngle);
  Standard_Integer aNbPoints = (Standard_Integer)((theU2 - theU1) / anAngle);
  aNbPoints += 2;
  anAngle          = (theU2 - theU1) / (Standard_Real)(aNbPoints - 1);
  Standard_Real aU = theU1;
  for (Standard_Integer i = 1; i <= aNbPoints; ++i)
  {
    theParameters.Append(aU);
    aPoint = Value(theC, aU);
    thePoints.Append(aPoint);
    aU += anAngle;
  }
  return Standard_True;
}

//=================================================================================================

template <class TheCurve>
static GCPnts_DeflectionType GetDefType(const TheCurve& theC)
{
  if (theC.NbIntervals(GeomAbs_C2) > 1)
  {
    return GCPnts_DefComposite;
  }

  switch (theC.GetType())
  {
    case GeomAbs_Line:
      return GCPnts_Linear;
    case GeomAbs_Circle:
      return GCPnts_Circular;
    case GeomAbs_BSplineCurve: {
      Handle(typename GCPnts_TCurveTypes<TheCurve>::BSplineCurve) aBSpline = theC.BSpline();
      return (aBSpline->NbPoles() == 2) ? GCPnts_Linear : GCPnts_Curved;
    }
    case GeomAbs_BezierCurve: {
      Handle(typename GCPnts_TCurveTypes<TheCurve>::BezierCurve) aBezier = theC.Bezier();
      return (aBezier->NbPoles() == 2) ? GCPnts_Linear : GCPnts_Curved;
    }
    default: {
      return GCPnts_Curved;
    }
  }
}

//=================================================================================================

template <class TheCurve>
static Standard_Boolean PerformCurve(TColStd_SequenceOfReal& theParameters,
                                     TColgp_SequenceOfPnt&   thePoints,
                                     const TheCurve&         theC,
                                     const Standard_Real     theDeflection,
                                     const Standard_Real     theU1,
                                     const Standard_Real     theU2,
                                     const Standard_Real     theEPSILON,
                                     const Standard_Boolean  theWithControl)
{
  CPnts_UniformDeflection anIterator(theC, theDeflection, theU1, theU2, theEPSILON, theWithControl);
  for (; anIterator.More(); anIterator.Next())
  {
    theParameters.Append(anIterator.Value());
    thePoints.Append(anIterator.Point());
  }
  return anIterator.IsAllDone();
}

//=================================================================================================

template <class TheCurve>
static Standard_Boolean PerformComposite(TColStd_SequenceOfReal& theParameters,
                                         TColgp_SequenceOfPnt&   thePoints,
                                         const TheCurve&         theC,
                                         const Standard_Real     theDeflection,
                                         const Standard_Real     theU1,
                                         const Standard_Real     theU2,
                                         const Standard_Real     theEPSILON,
                                         const Standard_Boolean  theWithControl)
{
  const Standard_Integer aNbIntervals = theC.NbIntervals(GeomAbs_C2);
  Standard_Integer       aPIndex      = 0;

  TColStd_Array1OfReal aTI(1, aNbIntervals + 1);
  theC.Intervals(aTI, GeomAbs_C2);
  BSplCLib::Hunt(aTI, theU1, aPIndex);

  // iterate by continuous segments
  Standard_Real aUa = theU1;
  for (Standard_Integer anIndex = aPIndex;;)
  {
    Standard_Real aUb = anIndex + 1 <= aTI.Upper() ? Min(theU2, aTI(anIndex + 1)) : theU2;
    if (!PerformCurve(theParameters,
                      thePoints,
                      theC,
                      theDeflection,
                      aUa,
                      aUb,
                      theEPSILON,
                      theWithControl))
    {
      return Standard_False;
    }
    ++anIndex;
    if (anIndex > aNbIntervals || theU2 < aTI(anIndex))
    {
      return Standard_True;
    }

    // remove last point to avoid duplication
    theParameters.Remove(theParameters.Length());
    thePoints.Remove(thePoints.Length());

    aUa = aUb;
  }
}

//=================================================================================================

template <class TheCurve>
void GCPnts_UniformDeflection::initialize(const TheCurve&        theC,
                                          const Standard_Real    theDeflection,
                                          const Standard_Real    theU1,
                                          const Standard_Real    theU2,
                                          const Standard_Boolean theWithControl)
{
  const Standard_Real anEPSILON = theC.Resolution(Precision::Confusion());
  myDeflection                  = theDeflection;
  myDone                        = Standard_False;
  myParams.Clear();
  myPoints.Clear();

  const Standard_Real         aU1   = Min(theU1, theU2);
  const Standard_Real         aU2   = Max(theU1, theU2);
  const GCPnts_DeflectionType aType = GetDefType(theC);
  switch (aType)
  {
    case GCPnts_Linear:
      myDone = PerformLinear(theC, myParams, myPoints, aU1, aU2);
      break;
    case GCPnts_Circular:
      myDone = PerformCircular(theC, myParams, myPoints, theDeflection, aU1, aU2);
      break;
    case GCPnts_Curved:
      myDone =
        PerformCurve(myParams, myPoints, theC, theDeflection, aU1, aU2, anEPSILON, theWithControl);
      break;
    case GCPnts_DefComposite:
      myDone = PerformComposite(myParams,
                                myPoints,
                                theC,
                                theDeflection,
                                aU1,
                                aU2,
                                anEPSILON,
                                theWithControl);
      break;
  }

  // control of the last points
  Controle(theC, myParams, myPoints, aU2);
}
