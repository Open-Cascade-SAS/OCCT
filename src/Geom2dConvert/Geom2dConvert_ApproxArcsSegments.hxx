// Created: 2009-01-20
//
// Copyright (c) 2009-2013 OPEN CASCADE SAS
//
// This file is part of commercial software by OPEN CASCADE SAS,
// furnished in accordance with the terms and conditions of the contract
// and with the inclusion of this copyright notice.
// This file or any part thereof may not be provided or otherwise
// made available to any third party.
//
// No ownership title to the software is transferred hereby.
//
// OPEN CASCADE SAS makes no representation or warranties with respect to the
// performance of this software, and specifically disclaims any responsibility
// for any damages, special or consequential, connected with its use.

#ifndef _Geom2dConvert_ApproxArcsSegments_HeaderFile
#define _Geom2dConvert_ApproxArcsSegments_HeaderFile

#include <TColGeom2d_SequenceOfCurve.hxx>
#include <Geom2dConvert_PPoint.hxx>
#include <Geom2dConvert_SequenceOfPPoint.hxx>
#include <Geom2d_TrimmedCurve.hxx>

//! Approximation of a free-form curve by a sequence of arcs+segments.
class Geom2dConvert_ApproxArcsSegments
{
public:
  // ---------- PUBLIC METHODS ----------

  enum Status
  {
    StatusOK = 0,
    StatusNotDone,
    StatusError
  };

  //! Constructor.
  Standard_EXPORT Geom2dConvert_ApproxArcsSegments(const Adaptor2d_Curve2d& theCurve,
                                                   const Standard_Real      theTolerance,
                                                   const Standard_Real      theAngleTol);

  //! Get the result curve after approximation.
  const TColGeom2d_SequenceOfCurve& GetResult() const { return mySeqCurves; }

private:
  //! Create arc of circle by three points (knowing that myCurve is circle).
  Handle(Geom2d_Curve) makeCircle(const Geom2dConvert_PPoint& theFirst,
                                  const Geom2dConvert_PPoint& theLast) const;

  //! Create an arc of circle using 2 points and a derivative in the first point.
  Standard_Boolean makeArc(const Geom2dConvert_PPoint&  theParam1,
                           Geom2dConvert_PPoint&        theParam2,
                           const Standard_Boolean       isFirst,
                           Handle(Geom2d_TrimmedCurve)& theCurve) const;

  //! Make a line from myCurve in the limits by parameter from theFirst to theLast
  Handle(Geom2d_TrimmedCurve) makeLine(Geom2dConvert_PPoint&  theFirst,
                                       Geom2dConvert_PPoint&  theLast,
                                       const Standard_Boolean isCheck) const;

  //! Create a sequence of elementary curves from a free-form adaptor curve.
  Standard_Boolean makeFreeform();

  //! Obtain the linear intervals on the curve using as criteria
  //! curvature tolerance (indicating either linear part or inflection)
  void getLinearParts(Geom2dConvert_SequenceOfPPoint& theSeqParam);

  //! Dichotomic search of the boundary of inflection interval, between
  //! two parameters on the Curve
  Geom2dConvert_PPoint findInflection(const Geom2dConvert_PPoint& theParamIsIn,
                                      const Geom2dConvert_PPoint& theParamNoIn) const;

  //! Make approximation non-linear part of the other curve.
  Standard_Boolean makeApproximation(Geom2dConvert_PPoint& theFirstParam,
                                     Geom2dConvert_PPoint& theLastParam);

  //! Method for calculation of a biarc.
  Standard_Boolean calculateBiArcs(Geom2dConvert_PPoint& theFirstParam,
                                   Geom2dConvert_PPoint& theLastParam);

  //! Method for calculation of a linear interpolation.
  Standard_Boolean calculateLines(Geom2dConvert_PPoint& theFirstParam,
                                  Geom2dConvert_PPoint& theLastParam);

  //! Checking max deflection Geom curve from Adaptor Curve
  Standard_Boolean checkCurve(const Handle(Geom2d_Curve)& aCurve,
                              const Standard_Real         theFirstParam,
                              const Standard_Real         theLastParam) const;

private:
  // ---------- PRIVATE FIELDS ----------

  const Adaptor2d_Curve2d& myCurve;
  Geom2dConvert_PPoint     myExt[2];

  Handle(NCollection_BaseAllocator) myAlloc;
  Standard_Real                     myTolerance;
  Standard_Real                     myAngleTolerance;

  Geom2dConvert_SequenceOfPPoint mySeqParams;
  TColGeom2d_SequenceOfCurve     mySeqCurves;
  Status                         myStatus;

  //! Protection against compiler warning
  void operator=(const Geom2dConvert_ApproxArcsSegments&);
};

#endif
