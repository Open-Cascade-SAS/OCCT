// Created on: 1992-03-25
// Created by: Herve LEGRAND
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

#ifndef _GCPnts_AbscissaPoint_HeaderFile
#define _GCPnts_AbscissaPoint_HeaderFile

#include <CPnts_AbscissaPoint.hxx>

class Adaptor3d_Curve;
class Adaptor2d_Curve2d;

//! Provides an algorithm to compute a point on a curve
//! situated at a given distance from another point on the curve,
//! the distance being measured along the curve (curvilinear abscissa on the curve).
//! This algorithm is also used to compute the length of a curve.
//! An AbscissaPoint object provides a framework for:
//! -   defining the point to compute
//! -   implementing the construction algorithm
//! -   consulting the result.
class GCPnts_AbscissaPoint
{
public:
  DEFINE_STANDARD_ALLOC

  //! Computes the length of the 3D Curve.
  Standard_EXPORT static Standard_Real Length(const Adaptor3d_Curve& theC);

  //! Computes the length of the 2D Curve.
  Standard_EXPORT static Standard_Real Length(const Adaptor2d_Curve2d& theC);

  //! Computes the length of the 3D Curve with the given tolerance.
  Standard_EXPORT static Standard_Real Length(const Adaptor3d_Curve& theC,
                                              const Standard_Real    theTol);

  //! Computes the length of the 2D Curve with the given tolerance.
  Standard_EXPORT static Standard_Real Length(const Adaptor2d_Curve2d& theC,
                                              const Standard_Real      theTol);

  //! Computes the length of the 3D Curve.
  Standard_EXPORT static Standard_Real Length(const Adaptor3d_Curve& theC,
                                              const Standard_Real    theU1,
                                              const Standard_Real    theU2);

  //! Computes the length of the 2D Curve.
  Standard_EXPORT static Standard_Real Length(const Adaptor2d_Curve2d& theC,
                                              const Standard_Real      theU1,
                                              const Standard_Real      theU2);

  //! Computes the length of the 3D Curve with the given tolerance.
  Standard_EXPORT static Standard_Real Length(const Adaptor3d_Curve& theC,
                                              const Standard_Real    theU1,
                                              const Standard_Real    theU2,
                                              const Standard_Real    theTol);

  //! Computes the length of the Curve with the given tolerance.
  Standard_EXPORT static Standard_Real Length(const Adaptor2d_Curve2d& theC,
                                              const Standard_Real      theU1,
                                              const Standard_Real      theU2,
                                              const Standard_Real      theTol);

public:
  //! Empty constructor.
  Standard_EXPORT GCPnts_AbscissaPoint();

  //! The algorithm computes a point on a curve at the
  //! distance theAbscissa from the point of parameter theU0.
  Standard_EXPORT GCPnts_AbscissaPoint(const Adaptor3d_Curve& theC,
                                       const Standard_Real    theAbscissa,
                                       const Standard_Real    theU0);

  //! The algorithm computes a point on a curve at
  //! the distance theAbscissa from the point of parameter
  //! theU0 with the given tolerance.
  Standard_EXPORT GCPnts_AbscissaPoint(const Standard_Real    theTol,
                                       const Adaptor3d_Curve& theC,
                                       const Standard_Real    theAbscissa,
                                       const Standard_Real    theU0);

  //! The algorithm computes a point on a curve at
  //! the distance theAbscissa from the point of parameter
  //! theU0 with the given tolerance.
  Standard_EXPORT GCPnts_AbscissaPoint(const Standard_Real      theTol,
                                       const Adaptor2d_Curve2d& theC,
                                       const Standard_Real      theAbscissa,
                                       const Standard_Real      theU0);

  //! The algorithm computes a point on a curve at the
  //! distance theAbscissa from the point of parameter theU0.
  Standard_EXPORT GCPnts_AbscissaPoint(const Adaptor2d_Curve2d& theC,
                                       const Standard_Real      theAbscissa,
                                       const Standard_Real      theU0);

  //! The algorithm computes a point on a curve at the
  //! distance theAbscissa from the point of parameter theU0.
  //! theUi is the starting value used in the iterative process
  //! which find the solution, it must be close to the final solution.
  Standard_EXPORT GCPnts_AbscissaPoint(const Adaptor3d_Curve& theC,
                                       const Standard_Real    theAbscissa,
                                       const Standard_Real    theU0,
                                       const Standard_Real    theUi);

  //! The algorithm computes a point on a curve at the
  //! distance theAbscissa from the point of parameter theU0.
  //! theUi is the starting value used in the iterative process
  //! which find the solution, it must be closed to the final solution
  Standard_EXPORT GCPnts_AbscissaPoint(const Adaptor2d_Curve2d& theC,
                                       const Standard_Real      theAbscissa,
                                       const Standard_Real      theU0,
                                       const Standard_Real      theUi);

  //! The algorithm computes a point on a curve at the
  //! distance theAbscissa from the point of parameter theU0.
  //! theUi is the starting value used in the iterative process
  //! which find the solution, it must be close to the final solution
  Standard_EXPORT GCPnts_AbscissaPoint(const Adaptor3d_Curve& theC,
                                       const Standard_Real    theAbscissa,
                                       const Standard_Real    theU0,
                                       const Standard_Real    theUi,
                                       const Standard_Real    theTol);

  //! The algorithm computes a point on a curve at the
  //! distance theAbscissa from the point of parameter theU0.
  //! theUi is the starting value used in the iterative process
  //! which find the solution, it must be close to the final solution
  Standard_EXPORT GCPnts_AbscissaPoint(const Adaptor2d_Curve2d& theC,
                                       const Standard_Real      theAbscissa,
                                       const Standard_Real      theU0,
                                       const Standard_Real      theUi,
                                       const Standard_Real      theTol);

  //! True if the computation was successful, False otherwise.
  //! IsDone is a protection against:
  //! -   non-convergence of the algorithm
  //! -   querying the results before computation.
  Standard_Boolean IsDone() const { return myComputer.IsDone(); }

  //! Returns the parameter on the curve of the point
  //! solution of this algorithm.
  //! Exceptions
  //! StdFail_NotDone if the computation was not
  //! successful, or was not done.
  Standard_Real Parameter() const { return myComputer.Parameter(); }

private:
  //! Computes the length of the Curve with the optional tolerance.
  template <class TheCurve>
  static Standard_Real length(const TheCurve&      theC,
                              const Standard_Real  theU1,
                              const Standard_Real  theU2,
                              const Standard_Real* theTol);

  //! Performs algorithm from the point of parameter.
  template <class TheCurve>
  void compute(const TheCurve& theC, const Standard_Real theAbscissa, const Standard_Real theU0);

  //! Performs algorithm from the point of parameter with the given tolerance.
  template <class TheCurve>
  void advCompute(const Standard_Real theTol,
                  const TheCurve&     theC,
                  const Standard_Real theAbscissa,
                  const Standard_Real theU0);

private:
  CPnts_AbscissaPoint myComputer;
};

#endif // _GCPnts_AbscissaPoint_HeaderFile
