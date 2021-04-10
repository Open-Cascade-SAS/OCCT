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

#include <GCPnts_QuasiUniformDeflection.hxx>

#include <GCPnts_DeflectionType.hxx>
#include <GCPnts_TCurveTypes.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <StdFail_NotDone.hxx>

static const Standard_Integer MyMaxQuasiFleshe = 2000;

// mask the return of a Adaptor2d_Curve2d as a gp_Pnt 
static gp_Pnt Value (const Adaptor3d_Curve& theC,
                     const Standard_Real theParameter)
{
  return theC.Value (theParameter);
}

static gp_Pnt Value (const Adaptor2d_Curve2d& theC,
                     const Standard_Real theParameter)
{
  gp_Pnt aPoint;
  gp_Pnt2d a2dPoint (theC.Value (theParameter));
  aPoint.SetCoord (a2dPoint.X(), a2dPoint.Y(), 0.0);
  return aPoint;
}

static void D1 (const Adaptor3d_Curve& theC,
                const Standard_Real theParameter,
                gp_Pnt& theP,
                gp_Vec& theV)
{
  theC.D1 (theParameter, theP, theV);
}

static void D1 (const Adaptor2d_Curve2d& theC,
                const Standard_Real theParameter,
                gp_Pnt& theP,
                gp_Vec& theV)
{
  gp_Pnt2d a2dPoint;
  gp_Vec2d a2dVec;
  theC.D1 (theParameter, a2dPoint, a2dVec);
  theP.SetCoord (a2dPoint.X(), a2dPoint.Y(), 0.0);
  theV.SetCoord (a2dVec.X(), a2dVec.Y(), 0.0);
}

//=======================================================================
//function : QuasiFleche
//purpose  :
//=======================================================================
template<class TheCurve>
static void QuasiFleche (const TheCurve& theC,
                         const Standard_Real theDeflection2,
                         const Standard_Real theUdeb,
                         const gp_Pnt& thePdeb,
                         const gp_Vec& theVdeb,
                         const Standard_Real theUfin,
                         const gp_Pnt& thePfin,
                         const gp_Vec& theVfin,
                         const Standard_Integer theNbmin,
                         const Standard_Real theEps,
                         TColStd_SequenceOfReal& theParameters,
                         TColgp_SequenceOfPnt& thePoints,
                         Standard_Integer& theNbCalls)
{
  ++theNbCalls;
  if (theNbCalls >= MyMaxQuasiFleshe)
  {
    return;
  }

  const Standard_Integer aPtslength = thePoints.Length();
  if (theNbCalls > 100 && aPtslength < 2)
  {
    return;
  }

  Standard_Real aUdelta = theUfin - theUdeb;
  gp_Pnt aPdelta;
  gp_Vec aVdelta;
  if (theNbmin > 2)
  {
    aUdelta /= (theNbmin - 1);
    D1 (theC, theUdeb + aUdelta, aPdelta, aVdelta);
  }
  else
  {
    aPdelta = thePfin;
    aVdelta = theVfin;
  }

  const Standard_Real aNorme = gp_Vec (thePdeb, aPdelta).SquareMagnitude();
  Standard_Real aFleche = 0.0;
  Standard_Boolean isFlecheOk = Standard_False;
  if (aNorme > theEps)
  {
    // Evaluation de la fleche par interpolation . Voir IntWalk_IWalking_5.gxx
    Standard_Real N1 = theVdeb.SquareMagnitude();
    Standard_Real N2 = aVdelta.SquareMagnitude();
    if (N1 > theEps && N2 > theEps)
    {
      Standard_Real aNormediff = (theVdeb.Normalized().XYZ() - aVdelta.Normalized().XYZ()).SquareModulus();
      if (aNormediff > theEps)
      {
        aFleche = aNormediff * aNorme / 64.0;
        isFlecheOk = Standard_True;
      }
    }
  }
  if (!isFlecheOk)
  {
    gp_Pnt aPmid ((thePdeb.XYZ() + aPdelta.XYZ()) * 0.5);
    gp_Pnt aPverif (Value (theC, theUdeb + aUdelta * 0.5));
    aFleche = aPmid.SquareDistance (aPverif);
  }

  if (aFleche < theDeflection2)
  {
    theParameters.Append (theUdeb + aUdelta);
    thePoints.Append (aPdelta);
  }
  else
  {
    QuasiFleche (theC, theDeflection2, theUdeb, thePdeb,
                 theVdeb,
                 theUdeb + aUdelta, aPdelta,
                 aVdelta,
                 3,
                 theEps,
                 theParameters, thePoints, theNbCalls);
  }

  if (theNbmin > 2)
  {
    QuasiFleche (theC, theDeflection2, theUdeb + aUdelta, aPdelta,
                 aVdelta,
                 theUfin, thePfin,
                 theVfin,
                 theNbmin - (thePoints.Length() - aPtslength),
                 theEps,
                 theParameters, thePoints, theNbCalls);
  }
  --theNbCalls;
}

//=======================================================================
//function : QuasiFleche
//purpose  :
//=======================================================================
template<class TheCurve>
static void QuasiFleche (const TheCurve& theC,
                         const Standard_Real theDeflection2,
                         const Standard_Real theUdeb,
                         const gp_Pnt& thePdeb,
                         const Standard_Real theUfin,
                         const gp_Pnt& thePfin,
                         const Standard_Integer theNbmin,
                         TColStd_SequenceOfReal& theParameters,
                         TColgp_SequenceOfPnt& thePoints,
                         Standard_Integer& theNbCalls)
{
  ++theNbCalls;
  if (theNbCalls >= MyMaxQuasiFleshe)
  {
    return;
  }
  const Standard_Integer aPtslength = thePoints.Length();
  if (theNbCalls > 100 && aPtslength < 2)
  {
    return;
  }

  Standard_Real aUdelta = theUfin - theUdeb;
  gp_Pnt aPdelta;
  if (theNbmin > 2)
  {
    aUdelta /= (theNbmin - 1);
    aPdelta = Value (theC, theUdeb + aUdelta);
  }
  else
  {
    aPdelta = thePfin;
  }

  const gp_Pnt aPmid ((thePdeb.XYZ() + aPdelta.XYZ()) * 0.5);
  const gp_Pnt aPverif (Value (theC, theUdeb + aUdelta * 0.5));
  const Standard_Real aFleche = aPmid.SquareDistance (aPverif);
  if (aFleche < theDeflection2)
  {
    theParameters.Append (theUdeb + aUdelta);
    thePoints.Append (aPdelta);
  }
  else
  {
    QuasiFleche (theC, theDeflection2, theUdeb, thePdeb,
                 theUdeb + aUdelta * 0.5, aPverif,
                 2,
                 theParameters, thePoints, theNbCalls);

    QuasiFleche (theC, theDeflection2, theUdeb + aUdelta * 0.5, aPverif,
                 theUdeb + aUdelta, aPdelta,
                 2,
                 theParameters, thePoints, theNbCalls);
  }

  if (theNbmin > 2)
  {
    QuasiFleche (theC, theDeflection2, theUdeb + aUdelta, aPdelta,
                 theUfin, thePfin,
                 theNbmin - (thePoints.Length() - aPtslength),
                 theParameters, thePoints, theNbCalls);
  }
  --theNbCalls;
}


//=======================================================================
//function : PerformLinear
//purpose  :
//=======================================================================
template<class TheCurve>
static Standard_Boolean PerformLinear (const TheCurve& theC,
                                       TColStd_SequenceOfReal& theParameters,
                                       TColgp_SequenceOfPnt& thePoints,
                                       const Standard_Real theU1,
                                       const Standard_Real theU2)
{
  theParameters.Append (theU1);
  gp_Pnt aPoint = Value (theC, theU1);
  thePoints.Append (aPoint);

  theParameters.Append (theU2);
  aPoint = Value (theC, theU2);
  thePoints.Append (aPoint);
  return Standard_True;
}

//=======================================================================
//function : PerformCircular
//purpose  :
//=======================================================================
template<class TheCurve>
static Standard_Boolean PerformCircular (const TheCurve& theC,
                                         TColStd_SequenceOfReal& theParameters,
                                         TColgp_SequenceOfPnt& thePoints,
                                         const Standard_Real theDeflection,
                                         const Standard_Real theU1,
                                         const Standard_Real theU2)
{
  Standard_Real anAngle = Max (1.0 - (theDeflection / theC.Circle().Radius()), 0.0);
  anAngle = 2.0 * ACos (anAngle);
  Standard_Integer aNbPoints = (Standard_Integer )((theU2 - theU1) / anAngle);
  aNbPoints += 2;
  anAngle = (theU2 - theU1) / (Standard_Real) (aNbPoints - 1);
  Standard_Real U = theU1;
  for (Standard_Integer i = 1; i <= aNbPoints; ++i)
  {
    theParameters.Append (U);
    const gp_Pnt aPoint = Value (theC, U);
    thePoints.Append (aPoint);
    U += anAngle;
  }
  return Standard_True;
}

//=======================================================================
//function : GetDefType
//purpose  :
//=======================================================================
template<class TheCurve>
static GCPnts_DeflectionType GetDefType (const TheCurve& theC)
{
  if (theC.NbIntervals (GeomAbs_C1) > 1)
  {
    return GCPnts_DefComposite;
  }

  // pour forcer les decoupages aux cassures.
  // G1 devrait marcher, mais donne des exceptions...
  switch (theC.GetType())
  {
    case GeomAbs_Line:   return GCPnts_Linear;
    case GeomAbs_Circle: return GCPnts_Circular;
    case GeomAbs_BSplineCurve:
    {
      Handle(typename GCPnts_TCurveTypes<TheCurve>::BSplineCurve) aBS = theC.BSpline();
      return (aBS->NbPoles() == 2) ? GCPnts_Linear : GCPnts_Curved;
    }
    case GeomAbs_BezierCurve:
    {
      Handle(typename GCPnts_TCurveTypes<TheCurve>::BezierCurve) aBZ = theC.Bezier();
      return (aBZ->NbPoles() == 2) ? GCPnts_Linear : GCPnts_Curved;
    }
    default: return GCPnts_Curved;
  }
}

//=======================================================================
//function : PerformCurve
//purpose  :
//=======================================================================
template<class TheCurve>
static Standard_Boolean PerformCurve (TColStd_SequenceOfReal& theParameters,
                                      TColgp_SequenceOfPnt& thePoints,
                                      const TheCurve& theC,
                                      const Standard_Real theDeflection,
                                      const Standard_Real theU1,
                                      const Standard_Real theU2,
                                      const Standard_Real theEPSILON,
                                      const GeomAbs_Shape theContinuity)
{
  Standard_Integer aNbmin = 2;
  Standard_Integer aNbCallQF = 0;

  gp_Pnt aPdeb;
  if (theContinuity <= GeomAbs_G1)
  {
    aPdeb = Value (theC, theU1);
    theParameters.Append (theU1);
    thePoints.Append (aPdeb);

    gp_Pnt aPfin (Value (theC, theU2));
    QuasiFleche (theC, theDeflection * theDeflection,
                 theU1, aPdeb,
                 theU2, aPfin,
                 aNbmin,
                 theParameters, thePoints, aNbCallQF);
  }
  else
  {
    gp_Pnt aPfin;
    gp_Vec aDdeb, aDfin;
    D1 (theC, theU1, aPdeb, aDdeb);
    theParameters.Append (theU1);
    thePoints.Append (aPdeb);

    const Standard_Real aDecreasedU2 = theU2 - Epsilon (theU2) * 10.0;
    D1 (theC, aDecreasedU2, aPfin, aDfin);
    QuasiFleche (theC, theDeflection * theDeflection,
                 theU1, aPdeb,
                 aDdeb,
                 theU2, aPfin,
                 aDfin,
                 aNbmin,
                 theEPSILON * theEPSILON,
                 theParameters, thePoints, aNbCallQF);
  }
  //  cout << "Nb de pts: " << Points.Length()<< endl;
  return Standard_True;
}

//=======================================================================
//function : PerformComposite
//purpose  :
//=======================================================================
template<class TheCurve>
static Standard_Boolean PerformComposite (TColStd_SequenceOfReal& theParameters,
                                          TColgp_SequenceOfPnt& thePoints,
                                          const TheCurve& theC,
                                          const Standard_Real theDeflection,
                                          const Standard_Real theU1,
                                          const Standard_Real theU2,
                                          const Standard_Real theEPSILON,
                                          const GeomAbs_Shape theContinuity)
{
  //
  //  coherence avec Intervals
  //
  const Standard_Integer aNbIntervals = theC.NbIntervals (GeomAbs_C2);
  Standard_Integer aPIndex = 0;
  TColStd_Array1OfReal aTI (1, aNbIntervals + 1);
  theC.Intervals (aTI, GeomAbs_C2);
  BSplCLib::Hunt (aTI, theU1, aPIndex);

  // iterate by continuous segments
  Standard_Real aUa = theU1;
  for (Standard_Integer anIndex = aPIndex;;)
  {
    Standard_Real aUb = anIndex + 1 <= aTI.Upper()
                      ? Min (theU2, aTI (anIndex + 1))
                      : theU2;
    if (!PerformCurve (theParameters, thePoints, theC, theDeflection,
                       aUa, aUb, theEPSILON, theContinuity))
    {
      return Standard_False;
    }

    ++anIndex;
    if (anIndex > aNbIntervals || theU2 < aTI (anIndex))
    {
      return Standard_True;
    }

    // remove last point to avoid duplication
    theParameters.Remove (theParameters.Length());
    thePoints.Remove (thePoints.Length());

    aUa = aUb;
  }
}

//=======================================================================
//function : Value
//purpose  :
//=======================================================================
gp_Pnt GCPnts_QuasiUniformDeflection::Value (const Standard_Integer theIndex) const
{
  StdFail_NotDone_Raise_if(!myDone, "GCPnts_QuasiUniformAbscissa::Parameter()");
  return myPoints.Value (theIndex);
}

//=======================================================================
//function : GCPnts_QuasiUniformDeflection
//purpose  :
//=======================================================================
GCPnts_QuasiUniformDeflection::GCPnts_QuasiUniformDeflection()
: myDone (Standard_False),
  myDeflection (0.0),
  myCont (GeomAbs_C1)
{
  //
}

//=======================================================================
//function : GCPnts_QuasiUniformDeflection
//purpose  :
//=======================================================================
GCPnts_QuasiUniformDeflection::GCPnts_QuasiUniformDeflection (const Adaptor3d_Curve& theC,
                                                              const Standard_Real theDeflection,
                                                              const Standard_Real theU1, const Standard_Real theU2,
                                                              const GeomAbs_Shape theContinuity)
: myDone (Standard_False),
  myDeflection (theDeflection),
  myCont (GeomAbs_C1)
{
  Initialize (theC, theDeflection, theU1, theU2, theContinuity);
}

//=======================================================================
//function : GCPnts_QuasiUniformDeflection
//purpose  :
//=======================================================================
GCPnts_QuasiUniformDeflection::GCPnts_QuasiUniformDeflection (const Adaptor2d_Curve2d& theC,
                                                              const Standard_Real theDeflection,
                                                              const Standard_Real theU1, const Standard_Real theU2,
                                                              const GeomAbs_Shape theContinuity)
: myDone (Standard_False),
  myDeflection (theDeflection),
  myCont (GeomAbs_C1)
{
  Initialize (theC, theDeflection, theU1, theU2, theContinuity);
}

//=======================================================================
//function : GCPnts_QuasiUniformDeflection
//purpose  :
//=======================================================================
GCPnts_QuasiUniformDeflection::GCPnts_QuasiUniformDeflection (const Adaptor3d_Curve& theC,
                                                              const Standard_Real theDeflection,
                                                              const GeomAbs_Shape theContinuity)
: myDone (Standard_False),
  myDeflection (theDeflection),
  myCont (GeomAbs_C1)
{
  Initialize (theC, theDeflection, theContinuity);
}

//=======================================================================
//function : GCPnts_QuasiUniformDeflection
//purpose  :
//=======================================================================
GCPnts_QuasiUniformDeflection::GCPnts_QuasiUniformDeflection (const Adaptor2d_Curve2d& theC,
                                                              const Standard_Real theDeflection,
                                                              const GeomAbs_Shape theContinuity)
: myDone (Standard_False),
  myDeflection (theDeflection),
  myCont (GeomAbs_C1)
{
  Initialize (theC, theDeflection, theContinuity);
}

//=======================================================================
//function : Initialize
//purpose  :
//=======================================================================
void GCPnts_QuasiUniformDeflection::Initialize (const Adaptor3d_Curve& theC,
                                                const Standard_Real theDeflection,
                                                const GeomAbs_Shape theContinuity)
{
  Initialize (theC, theDeflection, theC.FirstParameter(), theC.LastParameter(), theContinuity);
}

//=======================================================================
//function : Initialize
//purpose  :
//=======================================================================
void GCPnts_QuasiUniformDeflection::Initialize (const Adaptor2d_Curve2d& theC,
                                                const Standard_Real theDeflection,
                                                const GeomAbs_Shape theContinuity)
{
  Initialize (theC, theDeflection, theC.FirstParameter(), theC.LastParameter(), theContinuity);
}

//=======================================================================
//function : Initialize
//purpose  :
//=======================================================================
void GCPnts_QuasiUniformDeflection::Initialize (const Adaptor3d_Curve& theC,
                                                const Standard_Real theDeflection,
                                                const Standard_Real theU1, const Standard_Real theU2,
                                                const GeomAbs_Shape theContinuity)
{
  initialize (theC, theDeflection, theU1, theU2, theContinuity);
}

//=======================================================================
//function : Initialize
//purpose  :
//=======================================================================
void GCPnts_QuasiUniformDeflection::Initialize (const Adaptor2d_Curve2d& theC,
                                                const Standard_Real theDeflection,
                                                const Standard_Real theU1, const Standard_Real theU2,
                                                const GeomAbs_Shape theContinuity)
{
  initialize (theC, theDeflection, theU1, theU2, theContinuity);
}

//=======================================================================
//function : initialize
//purpose  :
//=======================================================================
template<class TheCurve>
void GCPnts_QuasiUniformDeflection::initialize (const TheCurve& theC,
                                                const Standard_Real theDeflection,
                                                const Standard_Real theU1, const Standard_Real theU2,
                                                const GeomAbs_Shape theContinuity)
{
  myCont = (theContinuity > GeomAbs_G1) ? GeomAbs_C1 : GeomAbs_C0;
  myDeflection = theDeflection;
  myDone = Standard_False;
  myParams.Clear();
  myPoints.Clear();

  const Standard_Real anEPSILON = Min (theC.Resolution (Precision::Confusion()), 1.e50);
  const GCPnts_DeflectionType aType = GetDefType (theC);
  const Standard_Real aU1 = Min (theU1, theU2);
  const Standard_Real aU2 = Max (theU1, theU2);
  if (aType == GCPnts_Curved
   || aType == GCPnts_DefComposite)
  {
    if (theC.GetType() == GeomAbs_BSplineCurve
     || theC.GetType() == GeomAbs_BezierCurve)
    {
      const Standard_Real aMaxPar = Max (Abs (theC.FirstParameter()), Abs (theC.LastParameter()));
      if (anEPSILON < Epsilon (aMaxPar))
      {
        return;
      }
    }
  }

  switch (aType)
  {
    case GCPnts_Linear:
    {
      myDone = PerformLinear (theC, myParams, myPoints, aU1, aU2);
      break;
    }
    case GCPnts_Circular:
    {
      myDone = PerformCircular (theC, myParams, myPoints, theDeflection, aU1, aU2);
      break;
    }
    case GCPnts_Curved:
    {
      myDone = PerformCurve (myParams, myPoints, theC, theDeflection,
                             aU1, aU2, anEPSILON, myCont);
      break;
    }
    case GCPnts_DefComposite:
    {
      myDone = PerformComposite (myParams, myPoints, theC, theDeflection,
                                 aU1, aU2, anEPSILON, myCont);
      break;
    }
  }
}
