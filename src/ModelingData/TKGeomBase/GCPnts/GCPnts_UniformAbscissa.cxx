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

#include <GCPnts_UniformAbscissa.hxx>

#include <GCPnts_AbscissaPoint.hxx>
#include <GCPnts_AbscissaType.hxx>
#include <GCPnts_TCurveTypes.hxx>
#include <Standard_ConstructionError.hxx>

//=================================================================================================

template <class TheCurve>
static Standard_Real GetParameterLengthRatio(const TheCurve& theC)
{
  switch (theC.GetType())
  {
    case GeomAbs_Circle: {
      return theC.Circle().Radius();
    }
    case GeomAbs_Line: {
      return 1.0;
    }
    case GeomAbs_BezierCurve:
    case GeomAbs_BSplineCurve: {
      if (!theC.IsRational())
      {
        return theC.DN(0.0, 1).Magnitude();
      }
      return RealLast();
    }
    default: {
      return RealLast();
    }
  }
}

//=================================================================================================

template <class TheCurve>
static GCPnts_AbscissaType GetAbsType(const TheCurve& theC)
{
  if (theC.NbIntervals(GeomAbs_C1) > 1)
  {
    return GCPnts_AbsComposite;
  }

  switch (theC.GetType())
  {
    case GeomAbs_Line:
    case GeomAbs_Circle: {
      return GCPnts_LengthParametrized;
    }
    case GeomAbs_BezierCurve: {
      Handle(typename GCPnts_TCurveTypes<TheCurve>::BezierCurve) aBZ = theC.Bezier();
      if (aBZ->NbPoles() == 2 && !aBZ->IsRational())
      {
        return GCPnts_LengthParametrized;
      }
      return GCPnts_Parametrized;
    }
    case GeomAbs_BSplineCurve: {
      Handle(typename GCPnts_TCurveTypes<TheCurve>::BSplineCurve) aBS = theC.BSpline();
      if (aBS->NbPoles() == 2 && !aBS->IsRational())
      {
        return GCPnts_LengthParametrized;
      }
      return GCPnts_Parametrized;
    }
    default: {
      return GCPnts_Parametrized;
    }
  }
}

//=================================================================================================

template <class TheCurve>
static Standard_Boolean Perform(TColStd_Array1OfReal& theParameters,
                                const TheCurve&       theC,
                                const Standard_Real   theAbscissa,
                                const Standard_Real   theU1,
                                const Standard_Real   theU2,
                                const Standard_Real   theTotalLength,
                                Standard_Integer&     theNbPoints,
                                const Standard_Real   theEPSILON)
{
  Standard_Boolean isLocalDone = Standard_True;
  Standard_Real    aUU1 = Min(theU1, theU2), aUU2 = Max(theU1, theU2);
  theNbPoints = 0;

  // this initialization avoids the computation of the Length of the curve
  Standard_Real    aDelta  = (theAbscissa / theTotalLength) * (aUU2 - aUU1);
  Standard_Integer anIndex = 1;
  theParameters.SetValue(anIndex, aUU1);
  for (Standard_Boolean isNotDone = Standard_True; isNotDone;)
  {
    Standard_Real aUi = theParameters.Value(anIndex) + aDelta;
    if (aUi > aUU2)
    {
      // MSV 21.04.2004: OCC5739 (GCPnts_UniformAbscissa gives incorrect distribution of points)
      // if (aUU2 - theParameters.Value (anIndex) > 0.01 * aDelta) { Index += 1; }
      // theParameters.SetValue (anIndex, aUU2);
      // isNotDone = Standard_False;
      // break;
      aUi = aUU2;
    }
    GCPnts_AbscissaPoint anAbscissaFinder(theC,
                                          theAbscissa,
                                          theParameters.Value(anIndex),
                                          aUi,
                                          theEPSILON);
    if (anAbscissaFinder.IsDone())
    {
      anIndex += 1;
      aUi = anAbscissaFinder.Parameter();
      if (Abs(aUi - aUU2) <= theEPSILON)
      {
        theParameters.SetValue(anIndex, aUU2);
        isNotDone = Standard_False;
      }
      else if (aUi < aUU2)
      {
        theParameters.SetValue(anIndex, aUi);
      }
      else
      {
        theParameters.SetValue(anIndex, aUU2);
        isNotDone = Standard_False;
      }
      isNotDone = isNotDone && (anIndex + 1 <= theParameters.Length());
    }
    else
    {
      isLocalDone = Standard_False;
      isNotDone   = Standard_True;
      aDelta -= aDelta / 10;
      if (aDelta <= Precision::PConfusion())
      {
        break;
      }
    }
  }
  theNbPoints = anIndex;
  return isLocalDone;
}

//=================================================================================================

template <class TheCurve>
static Standard_Boolean PerformLengthParametrized(TColStd_Array1OfReal& theParameters,
                                                  const TheCurve&       theC,
                                                  const Standard_Real   theAbscissa,
                                                  const Standard_Real   theU1,
                                                  const Standard_Real   theU2,
                                                  const Standard_Real   theTotalLength,
                                                  Standard_Integer&     theNbPoints,
                                                  const Standard_Real   theEPSILON)
{
  Standard_Real aUU1 = Min(theU1, theU2);
  Standard_Real aUU2 = Max(theU1, theU2);

  // Ratio is defined as dl = Ratio * du
  // for a circle of gp Ratio is equal to the radius of the circle.
  // for a line of gp ratio is equal to 1.0
  const Standard_Real aRatio = GetParameterLengthRatio(theC);
  if (theAbscissa < 0.0e0)
  {
    aUU2 = Min(theU1, theU2);
    aUU1 = Max(theU1, theU2);
  }

  const Standard_Real aDelta  = (theAbscissa / theTotalLength) * (aUU2 - aUU1);
  Standard_Integer    anIndex = 1;
  theNbPoints                 = 0;
  theParameters.SetValue(anIndex, aUU1);
  for (Standard_Boolean isNotDone = Standard_True; isNotDone;)
  {
    anIndex += 1;
    const Standard_Real aUi = theParameters.Value(anIndex - 1) + aDelta;
    if (Abs(aUi - aUU2) <= theEPSILON)
    {
      theParameters.SetValue(anIndex, aUU2);
      isNotDone = Standard_False;
    }
    else if (aUi < aUU2)
    {
      theParameters.SetValue(anIndex, aUi);
    }
    else
    {
      isNotDone = Standard_False;
      if (Abs(theParameters.Value(anIndex - 1) - aUU2) * aRatio / theAbscissa < 0.1)
      {
        theParameters.SetValue(anIndex - 1, aUU2);
        anIndex -= 1;
      }
      else
      {
        theParameters.SetValue(anIndex, aUU2);
      }
    }
    isNotDone = (anIndex + 1 <= theParameters.Length()) && isNotDone;
  }

  theNbPoints = anIndex;
  return Standard_True;
}

//=================================================================================================

GCPnts_UniformAbscissa::GCPnts_UniformAbscissa()
    : myDone(Standard_False),
      myNbPoints(0),
      myAbscissa(0.0)
{
  //
}

//=================================================================================================

GCPnts_UniformAbscissa::GCPnts_UniformAbscissa(const Adaptor3d_Curve& theC,
                                               const Standard_Real    theAbscissa,
                                               const Standard_Real    theTol)
    : myDone(Standard_False),
      myNbPoints(0),
      myAbscissa(0.0)
{
  Initialize(theC, theAbscissa, theTol);
}

//=================================================================================================

GCPnts_UniformAbscissa::GCPnts_UniformAbscissa(const Adaptor2d_Curve2d& theC,
                                               const Standard_Real      theAbscissa,
                                               const Standard_Real      theTol)
    : myDone(Standard_False),
      myNbPoints(0),
      myAbscissa(0.0)
{
  Initialize(theC, theAbscissa, theTol);
}

//=================================================================================================

GCPnts_UniformAbscissa::GCPnts_UniformAbscissa(const Adaptor3d_Curve& theC,
                                               const Standard_Real    theAbscissa,
                                               const Standard_Real    theU1,
                                               const Standard_Real    theU2,
                                               const Standard_Real    theTol)
    : myDone(Standard_False),
      myNbPoints(0),
      myAbscissa(0.0)
{
  Initialize(theC, theAbscissa, theU1, theU2, theTol);
}

//=================================================================================================

GCPnts_UniformAbscissa::GCPnts_UniformAbscissa(const Adaptor2d_Curve2d& theC,
                                               const Standard_Real      theAbscissa,
                                               const Standard_Real      theU1,
                                               const Standard_Real      theU2,
                                               const Standard_Real      theTol)
    : myDone(Standard_False),
      myNbPoints(0),
      myAbscissa(0.0)
{
  Initialize(theC, theAbscissa, theU1, theU2, theTol);
}

//=================================================================================================

GCPnts_UniformAbscissa::GCPnts_UniformAbscissa(const Adaptor3d_Curve& theC,
                                               const Standard_Integer theNbPoints,
                                               const Standard_Real    theTol)
    : myDone(Standard_False),
      myNbPoints(0),
      myAbscissa(0.0)
{
  Initialize(theC, theNbPoints, theTol);
}

//=================================================================================================

GCPnts_UniformAbscissa::GCPnts_UniformAbscissa(const Adaptor2d_Curve2d& theC,
                                               const Standard_Integer   theNbPoints,
                                               const Standard_Real      theTol)
    : myDone(Standard_False),
      myNbPoints(0),
      myAbscissa(0.0)
{
  Initialize(theC, theNbPoints, theTol);
}

//=================================================================================================

GCPnts_UniformAbscissa::GCPnts_UniformAbscissa(const Adaptor3d_Curve& theC,
                                               const Standard_Integer theNbPoints,
                                               const Standard_Real    theU1,
                                               const Standard_Real    theU2,
                                               const Standard_Real    theTol)
    : myDone(Standard_False),
      myNbPoints(0),
      myAbscissa(0.0)
{
  Initialize(theC, theNbPoints, theU1, theU2, theTol);
}

//=================================================================================================

GCPnts_UniformAbscissa::GCPnts_UniformAbscissa(const Adaptor2d_Curve2d& theC,
                                               const Standard_Integer   theNbPoints,
                                               const Standard_Real      theU1,
                                               const Standard_Real      theU2,
                                               const Standard_Real      theTol)
    : myDone(Standard_False),
      myNbPoints(0),
      myAbscissa(0.0)
{
  Initialize(theC, theNbPoints, theU1, theU2, theTol);
}

//=================================================================================================

void GCPnts_UniformAbscissa::Initialize(const Adaptor3d_Curve& theC,
                                        const Standard_Real    theAbscissa,
                                        const Standard_Real    theTol)
{
  Initialize(theC, theAbscissa, theC.FirstParameter(), theC.LastParameter(), theTol);
}

//=================================================================================================

void GCPnts_UniformAbscissa::Initialize(const Adaptor2d_Curve2d& theC,
                                        const Standard_Real      theAbscissa,
                                        const Standard_Real      theTol)
{
  Initialize(theC, theAbscissa, theC.FirstParameter(), theC.LastParameter(), theTol);
}

//=================================================================================================

void GCPnts_UniformAbscissa::Initialize(const Adaptor3d_Curve& theC,
                                        const Standard_Real    theAbscissa,
                                        const Standard_Real    theU1,
                                        const Standard_Real    theU2,
                                        const Standard_Real    theTol)
{
  initialize(theC, theAbscissa, theU1, theU2, theTol);
}

//=================================================================================================

void GCPnts_UniformAbscissa::Initialize(const Adaptor2d_Curve2d& theC,
                                        const Standard_Real      theAbscissa,
                                        const Standard_Real      theU1,
                                        const Standard_Real      theU2,
                                        const Standard_Real      theTol)
{
  initialize(theC, theAbscissa, theU1, theU2, theTol);
}

//=================================================================================================

template <class TheCurve>
void GCPnts_UniformAbscissa::initialize(const TheCurve&     theC,
                                        const Standard_Real theAbscissa,
                                        const Standard_Real theU1,
                                        const Standard_Real theU2,
                                        const Standard_Real theTol)
{
  myAbscissa = theAbscissa;
  myNbPoints = 0;
  myDone     = Standard_False;

  const Standard_Real anEPSILON = theC.Resolution(Max(theTol, Precision::Confusion()));
  const Standard_Real aL        = GCPnts_AbscissaPoint::Length(theC, theU1, theU2, anEPSILON);
  if (aL <= Precision::Confusion())
  {
    return;
  }

  // compute the total Length here so that we can guess
  // the number of points instead of letting the constructor
  // of CPnts_AbscissaPoint do that and lose the information
  const Standard_Real aSizeR = aL / Abs(theAbscissa) + 5;
  if (aSizeR >= IntegerLast()) // modified by Igor Motchalov 23/04/2001
  {
    return;
  }

  const Standard_Integer aSize = (Standard_Integer)aSizeR;
  if (!myParams.IsNull())
  {
    if (myParams->Length() < aSize)
    {
      myParams.Nullify();
      myParams = new TColStd_HArray1OfReal(1, aSize);
    }
  }
  else
  {
    myParams = new TColStd_HArray1OfReal(1, aSize);
  }

  const GCPnts_AbscissaType aType = GetAbsType(theC);
  switch (aType)
  {
    case GCPnts_LengthParametrized: {
      myDone = PerformLengthParametrized(myParams->ChangeArray1(),
                                         theC,
                                         theAbscissa,
                                         theU1,
                                         theU2,
                                         aL,
                                         myNbPoints,
                                         anEPSILON);
      break;
    }
    case GCPnts_Parametrized:
    case GCPnts_AbsComposite: {
      myDone = Perform(myParams->ChangeArray1(),
                       theC,
                       theAbscissa,
                       theU1,
                       theU2,
                       aL,
                       myNbPoints,
                       anEPSILON);
      break;
    }
  }
}

//=================================================================================================

void GCPnts_UniformAbscissa::Initialize(const Adaptor3d_Curve& theC,
                                        const Standard_Integer theNbPoints,
                                        const Standard_Real    theTol)
{
  Initialize(theC, theNbPoints, theC.FirstParameter(), theC.LastParameter(), theTol);
}

//=================================================================================================

void GCPnts_UniformAbscissa::Initialize(const Adaptor2d_Curve2d& theC,
                                        const Standard_Integer   theNbPoints,
                                        const Standard_Real      theTol)
{
  Initialize(theC, theNbPoints, theC.FirstParameter(), theC.LastParameter(), theTol);
}

//=================================================================================================

void GCPnts_UniformAbscissa::Initialize(const Adaptor3d_Curve& theC,
                                        const Standard_Integer theNbPoints,
                                        const Standard_Real    theU1,
                                        const Standard_Real    theU2,
                                        const Standard_Real    theTol)
{
  initialize(theC, theNbPoints, theU1, theU2, theTol);
}

//=================================================================================================

void GCPnts_UniformAbscissa::Initialize(const Adaptor2d_Curve2d& theC,
                                        const Standard_Integer   theNbPoints,
                                        const Standard_Real      theU1,
                                        const Standard_Real      theU2,
                                        const Standard_Real      theTol)
{
  initialize(theC, theNbPoints, theU1, theU2, theTol);
}

//=================================================================================================

template <class TheCurve>
void GCPnts_UniformAbscissa::initialize(const TheCurve&        theC,
                                        const Standard_Integer theNbPoints,
                                        const Standard_Real    theU1,
                                        const Standard_Real    theU2,
                                        const Standard_Real    theTol)
{
  Standard_ConstructionError_Raise_if(
    theNbPoints <= 1,
    "GCPnts_UniformAbscissa::Initialize() - number of points should be >= 2");
  myNbPoints = 0;
  myDone     = Standard_False;

  const Standard_Real anEPSILON = theC.Resolution(Max(theTol, Precision::Confusion()));
  // although very similar to Initialize with Abscissa this avoid
  // the computation of the total length of the curve twice
  const Standard_Real aL = GCPnts_AbscissaPoint::Length(theC, theU1, theU2, anEPSILON);
  if (aL <= Precision::Confusion())
  {
    return;
  }

  const Standard_Real anAbscissa = myAbscissa = aL / (theNbPoints - 1);

  // compute the total Length here so that we can guess
  // the number of points instead of letting the constructor
  // of CPnts_AbscissaPoint do that and lose the information
  const Standard_Integer aSize = theNbPoints + 5;
  if (!myParams.IsNull())
  {
    if (myParams->Length() < aSize)
    {
      myParams.Nullify();
      myParams = new TColStd_HArray1OfReal(1, aSize);
    }
  }
  else
  {
    myParams = new TColStd_HArray1OfReal(1, aSize);
  }

  myNbPoints                      = 0;
  const GCPnts_AbscissaType aType = GetAbsType(theC);
  switch (aType)
  {
    case GCPnts_LengthParametrized: {
      myDone = PerformLengthParametrized(myParams->ChangeArray1(),
                                         theC,
                                         anAbscissa,
                                         theU1,
                                         theU2,
                                         aL,
                                         myNbPoints,
                                         anEPSILON);
      break;
    }
    case GCPnts_Parametrized:
    case GCPnts_AbsComposite: {
      myDone = Perform(myParams->ChangeArray1(),
                       theC,
                       anAbscissa,
                       theU1,
                       theU2,
                       aL,
                       myNbPoints,
                       anEPSILON);
      break;
    }
  }
}
