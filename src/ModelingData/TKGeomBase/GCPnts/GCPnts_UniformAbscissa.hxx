// Created on: 1991-02-27
// Created by: Jean Claude Vauthier
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _GCPnts_UniformAbscissa_HeaderFile
#define _GCPnts_UniformAbscissa_HeaderFile

#include <StdFail_NotDone.hxx>
#include <TColStd_HArray1OfReal.hxx>

class Adaptor3d_Curve;
class Adaptor2d_Curve2d;

//! This class allows to compute a uniform distribution of points
//! on a curve (i.e. the points will all be equally distant).
class GCPnts_UniformAbscissa
{
public:
  DEFINE_STANDARD_ALLOC

  //! creation of a indefinite UniformAbscissa
  Standard_EXPORT GCPnts_UniformAbscissa();

  //! Computes a uniform abscissa distribution of points on the 3D curve.
  //! @param[in] theC  input curve
  //! @param[in] theAbscissa  abscissa (distance between two consecutive points)
  //! @param[in] theToler  used for more precise calculation of curve length
  //!                      (Precision::Confusion() by default)
  Standard_EXPORT GCPnts_UniformAbscissa(const Adaptor3d_Curve& theC,
                                         const Standard_Real    theAbscissa,
                                         const Standard_Real    theToler = -1);

  //! Computes a Uniform abscissa distribution of points on a part of the 3D Curve.
  //! @param[in] theC  input curve
  //! @param[in] theAbscissa  abscissa (distance between two consecutive points)
  //! @param[in] theU1  first parameter on curve
  //! @param[in] theU2  last  parameter on curve
  //! @param[in] theToler  used for more precise calculation of curve length
  //!                      (Precision::Confusion() by default)
  Standard_EXPORT GCPnts_UniformAbscissa(const Adaptor3d_Curve& theC,
                                         const Standard_Real    theAbscissa,
                                         const Standard_Real    theU1,
                                         const Standard_Real    theU2,
                                         const Standard_Real    theToler = -1);

  //! Computes a uniform abscissa distribution of points on the 3D Curve.
  //! @param[in] theC  input curve
  //! @param[in] theNbPoints  defines the number of desired points
  //! @param[in] theToler  used for more precise calculation of curve length
  //!                      (Precision::Confusion() by default)
  Standard_EXPORT GCPnts_UniformAbscissa(const Adaptor3d_Curve& theC,
                                         const Standard_Integer theNbPoints,
                                         const Standard_Real    theToler = -1);

  //! Computes a Uniform abscissa distribution of points on a part of the 3D Curve.
  //! @param[in] theC  input curve
  //! @param[in] theNbPoints  defines the number of desired points
  //! @param[in] theU1  first parameter on curve
  //! @param[in] theU2  last  parameter on curve
  //! @param[in] theToler  used for more precise calculation of curve length
  //!                      (Precision::Confusion() by default)
  Standard_EXPORT GCPnts_UniformAbscissa(const Adaptor3d_Curve& theC,
                                         const Standard_Integer theNbPoints,
                                         const Standard_Real    theU1,
                                         const Standard_Real    theU2,
                                         const Standard_Real    theToler = -1);

  //! Initialize the algorithms with 3D curve, Abscissa, and Tolerance.
  //! @param[in] theC  input curve
  //! @param[in] theAbscissa  abscissa (distance between two consecutive points)
  //! @param[in] theToler  used for more precise calculation of curve length
  //!                      (Precision::Confusion() by default)
  Standard_EXPORT void Initialize(const Adaptor3d_Curve& theC,
                                  const Standard_Real    theAbscissa,
                                  const Standard_Real    theToler = -1);

  //! Initialize the algorithms with 3D curve, Abscissa, Tolerance, and parameter range.
  //! @param[in] theC  input curve
  //! @param[in] theAbscissa  abscissa (distance between two consecutive points)
  //! @param[in] theU1  first parameter on curve
  //! @param[in] theU2  last  parameter on curve
  //! @param[in] theToler  used for more precise calculation of curve length
  //!                      (Precision::Confusion() by default)
  Standard_EXPORT void Initialize(const Adaptor3d_Curve& theC,
                                  const Standard_Real    theAbscissa,
                                  const Standard_Real    theU1,
                                  const Standard_Real    theU2,
                                  const Standard_Real    theToler = -1);

  //! Initialize the algorithms with 3D curve, number of points, and Tolerance.
  //! @param[in] theC  input curve
  //! @param[in] theNbPoints  defines the number of desired points
  //! @param[in] theToler  used for more precise calculation of curve length
  //!                      (Precision::Confusion() by default)
  Standard_EXPORT void Initialize(const Adaptor3d_Curve& theC,
                                  const Standard_Integer theNbPoints,
                                  const Standard_Real    theToler = -1);

  //! Initialize the algorithms with 3D curve, number of points, Tolerance, and parameter range.
  //! @param[in] theC  input curve
  //! @param[in] theNbPoints  defines the number of desired points
  //! @param[in] theU1  first parameter on curve
  //! @param[in] theU2  last  parameter on curve
  //! @param[in] theToler  used for more precise calculation of curve length
  //!                      (Precision::Confusion() by default)
  Standard_EXPORT void Initialize(const Adaptor3d_Curve& theC,
                                  const Standard_Integer theNbPoints,
                                  const Standard_Real    theU1,
                                  const Standard_Real    theU2,
                                  const Standard_Real    theToler = -1);

public:
  //! Computes a uniform abscissa distribution of points on the 2D curve.
  //! @param[in] theC  input curve
  //! @param[in] theAbscissa  abscissa (distance between two consecutive points)
  //! @param[in] theToler  used for more precise calculation of curve length
  //!                      (Precision::Confusion() by default)
  Standard_EXPORT GCPnts_UniformAbscissa(const Adaptor2d_Curve2d& theC,
                                         const Standard_Real      theAbscissa,
                                         const Standard_Real      theToler = -1);

  //! Computes a Uniform abscissa distribution of points on a part of the 2D Curve.
  //! @param[in] theC  input curve
  //! @param[in] theAbscissa  abscissa (distance between two consecutive points)
  //! @param[in] theU1  first parameter on curve
  //! @param[in] theU2  last  parameter on curve
  //! @param[in] theToler  used for more precise calculation of curve length
  //!                      (Precision::Confusion() by default)
  Standard_EXPORT GCPnts_UniformAbscissa(const Adaptor2d_Curve2d& theC,
                                         const Standard_Real      theAbscissa,
                                         const Standard_Real      theU1,
                                         const Standard_Real      theU2,
                                         const Standard_Real      theToler = -1);

  //! Computes a uniform abscissa distribution of points on the 2D Curve.
  //! @param[in] theC  input curve
  //! @param[in] theNbPoints  defines the number of desired points
  //! @param[in] theToler  used for more precise calculation of curve length
  //!                      (Precision::Confusion() by default)
  Standard_EXPORT GCPnts_UniformAbscissa(const Adaptor2d_Curve2d& theC,
                                         const Standard_Integer   theNbPoints,
                                         const Standard_Real      theToler = -1);

  //! Computes a Uniform abscissa distribution of points on a part of the 2D Curve.
  //! @param[in] theC  input curve
  //! @param[in] theNbPoints  defines the number of desired points
  //! @param[in] theU1  first parameter on curve
  //! @param[in] theU2  last  parameter on curve
  //! @param[in] theToler  used for more precise calculation of curve length
  //!                      (Precision::Confusion() by default)
  Standard_EXPORT GCPnts_UniformAbscissa(const Adaptor2d_Curve2d& theC,
                                         const Standard_Integer   theNbPoints,
                                         const Standard_Real      theU1,
                                         const Standard_Real      theU2,
                                         const Standard_Real      theToler = -1);

  //! Initialize the algorithms with 2D curve, Abscissa, and Tolerance.
  //! @param[in] theC  input curve
  //! @param[in] theAbscissa  abscissa (distance between two consecutive points)
  //! @param[in] theToler  used for more precise calculation of curve length
  //!                      (Precision::Confusion() by default)
  Standard_EXPORT void Initialize(const Adaptor2d_Curve2d& theC,
                                  const Standard_Real      theAbscissa,
                                  const Standard_Real      theToler = -1);

  //! Initialize the algorithms with 2D curve, Abscissa, Tolerance, and parameter range.
  //! @param[in] theC  input curve
  //! @param[in] theAbscissa  abscissa (distance between two consecutive points)
  //! @param[in] theU1  first parameter on curve
  //! @param[in] theU2  last  parameter on curve
  //! @param[in] theToler  used for more precise calculation of curve length
  //!                      (Precision::Confusion() by default)
  Standard_EXPORT void Initialize(const Adaptor2d_Curve2d& theC,
                                  const Standard_Real      theAbscissa,
                                  const Standard_Real      theU1,
                                  const Standard_Real      theU2,
                                  const Standard_Real      theToler = -1);

  //! Initialize the algorithms with 2D curve, number of points, and Tolerance.
  //! @param[in] theC  input curve
  //! @param[in] theNbPoints  defines the number of desired points
  //! @param[in] theToler  used for more precise calculation of curve length
  //!                      (Precision::Confusion() by default)
  Standard_EXPORT void Initialize(const Adaptor2d_Curve2d& theC,
                                  const Standard_Integer   theNbPoints,
                                  const Standard_Real      theToler = -1);

  //! Initialize the algorithms with 2D curve, number of points, Tolerance, and parameter range.
  //! @param[in] theC  input curve
  //! @param[in] theNbPoints  defines the number of desired points
  //! @param[in] theU1  first parameter on curve
  //! @param[in] theU2  last  parameter on curve
  //! @param[in] theToler  used for more precise calculation of curve length
  //!                      (Precision::Confusion() by default)
  Standard_EXPORT void Initialize(const Adaptor2d_Curve2d& theC,
                                  const Standard_Integer   theNbPoints,
                                  const Standard_Real      theU1,
                                  const Standard_Real      theU2,
                                  const Standard_Real      theToler = -1);

  Standard_Boolean IsDone() const { return myDone; }

  Standard_Integer NbPoints() const
  {
    StdFail_NotDone_Raise_if(!myDone, "GCPnts_UniformAbscissa::NbPoints()");
    return myNbPoints;
  }

  //! returns the computed Parameter of index <Index>.
  Standard_Real Parameter(const Standard_Integer Index) const
  {
    StdFail_NotDone_Raise_if(!myDone, "GCPnts_UniformAbscissa::Parameter()");
    return myParams->Value(Index);
  }

  //! Returns the current abscissa, i.e. the distance between two consecutive points.
  Standard_Real Abscissa() const
  {
    StdFail_NotDone_Raise_if(!myDone, "GCPnts_UniformAbscissa::Abscissa()");
    return myAbscissa;
  }

private:
  //! Initializes algorithm.
  template <class TheCurve>
  void initialize(const TheCurve&     theC,
                  const Standard_Real theAbscissa,
                  const Standard_Real theU1,
                  const Standard_Real theU2,
                  const Standard_Real theTol);

  //! Initializes algorithm.
  template <class TheCurve>
  void initialize(const TheCurve&        theC,
                  const Standard_Integer theNbPoints,
                  const Standard_Real    theU1,
                  const Standard_Real    theU2,
                  const Standard_Real    theTol);

private:
  Standard_Boolean              myDone;
  Standard_Integer              myNbPoints;
  Standard_Real                 myAbscissa;
  Handle(TColStd_HArray1OfReal) myParams;
};

#endif // _GCPnts_UniformAbscissa_HeaderFile
