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

class StdFail_NotDone;
class Standard_ConstructionError;
class Adaptor3d_Curve;
class Adaptor2d_Curve2d;

//! Provides an algorithm to compute a point on a curve
//! situated at a given distance from another point on the
//! curve, the distance being measured along the curve
//! (curvilinear abscissa on the curve).
//! This algorithm is also used to compute the length of a curve.
//! An AbscissaPoint object provides a framework for:
//! -   defining the point to compute
//! -   implementing the construction algorithm
//! -   consulting the result.
class GCPnts_AbscissaPoint 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Computes the length of the Curve <C>.
  Standard_EXPORT static Standard_Real Length (const Adaptor3d_Curve& C);
  
  //! Computes the length of the Curve <C>.
  Standard_EXPORT static Standard_Real Length (const Adaptor2d_Curve2d& C);
  
  //! Computes the length of the Curve <C> with the given tolerance.
  Standard_EXPORT static Standard_Real Length (const Adaptor3d_Curve& C, const Standard_Real Tol);
  
  //! Computes the length of the Curve <C> with the given tolerance.
  Standard_EXPORT static Standard_Real Length (const Adaptor2d_Curve2d& C, const Standard_Real Tol);
  
  //! Computes the length of the Curve <C>.
  Standard_EXPORT static Standard_Real Length (const Adaptor3d_Curve& C, const Standard_Real U1, const Standard_Real U2);
  
  //! Computes the length of the Curve <C>.
  Standard_EXPORT static Standard_Real Length (const Adaptor2d_Curve2d& C, const Standard_Real U1, const Standard_Real U2);
  
  //! Computes the length of the Curve <C> with the given tolerance.
  Standard_EXPORT static Standard_Real Length (const Adaptor3d_Curve& C, const Standard_Real U1, const Standard_Real U2, const Standard_Real Tol);
  
  //! Computes the length of the Curve <C> with the given tolerance.
  //! Constructs an empty algorithm. This function is used
  //! only for initializing a framework to compute the length
  //! of a curve (or a series of curves).
  //! Warning
  //! The function IsDone will return the value false after the use of this function.
  Standard_EXPORT static Standard_Real Length (const Adaptor2d_Curve2d& C, const Standard_Real U1, const Standard_Real U2, const Standard_Real Tol);
  
  Standard_EXPORT GCPnts_AbscissaPoint();
  
  //! the algorithm computes a point on a curve <Curve> at the
  //! distance <Abscissa> from the point of parameter <U0>.
  Standard_EXPORT GCPnts_AbscissaPoint(const Adaptor3d_Curve& C, const Standard_Real Abscissa, const Standard_Real U0);
  
  //! the  algorithm computes a point on  a curve <Curve> at
  //! the distance  <Abscissa> from the  point of parameter
  //! <U0> with the given  tolerance.
  Standard_EXPORT GCPnts_AbscissaPoint(const Standard_Real Tol, const Adaptor3d_Curve& C, const Standard_Real Abscissa, const Standard_Real U0);
  
  //! the  algorithm computes a point on  a curve <Curve> at
  //! the distance  <Abscissa> from the  point of parameter
  //! <U0> with the given  tolerance.
  Standard_EXPORT GCPnts_AbscissaPoint(const Standard_Real Tol, const Adaptor2d_Curve2d& C, const Standard_Real Abscissa, const Standard_Real U0);
  
  //! the algorithm computes a point on a curve <Curve> at the
  //! distance <Abscissa> from the point of parameter <U0>.
  Standard_EXPORT GCPnts_AbscissaPoint(const Adaptor2d_Curve2d& C, const Standard_Real Abscissa, const Standard_Real U0);
  
  //! the algorithm computes a point on a curve <Curve> at the
  //! distance <Abscissa> from the point of parameter <U0>.
  //! <Ui> is the starting value used in the iterative process
  //! which find the solution, it must be close to the final
  //! solution
  Standard_EXPORT GCPnts_AbscissaPoint(const Adaptor3d_Curve& C, const Standard_Real Abscissa, const Standard_Real U0, const Standard_Real Ui);
  
  //! the algorithm computes a point on a curve <Curve> at the
  //! distance <Abscissa> from the point of parameter <U0>.
  //! <Ui> is the starting value used in the iterative process
  //! which find the solution, it must be closed to the final
  //! solution
  Standard_EXPORT GCPnts_AbscissaPoint(const Adaptor2d_Curve2d& C, const Standard_Real Abscissa, const Standard_Real U0, const Standard_Real Ui);
  
  //! the algorithm computes a point on a curve <Curve> at the
  //! distance <Abscissa> from the point of parameter <U0>.
  //! <Ui> is the starting value used in the iterative process
  //! which find the solution, it must be close to the final
  //! solution
  Standard_EXPORT GCPnts_AbscissaPoint(const Adaptor3d_Curve& C, const Standard_Real Abscissa, const Standard_Real U0, const Standard_Real Ui, const Standard_Real Tol);
  
  //! the algorithm computes a point on a curve <Curve> at the
  //! distance <Abscissa> from the point of parameter <U0>.
  //! <Ui> is the starting value used in the iterative process
  //! which find the solution, it must be close to the final
  //! solution
  Standard_EXPORT GCPnts_AbscissaPoint(const Adaptor2d_Curve2d& C, const Standard_Real Abscissa, const Standard_Real U0, const Standard_Real Ui, const Standard_Real Tol);
  
  //! True if the computation was successful, False otherwise.
  //! IsDone is a protection against:
  //! -   non-convergence of the algorithm
  //! -   querying the results before computation.
  Standard_Boolean IsDone () const
  {
    return myComputer.IsDone ();
  }
  
  //! Returns the parameter on the curve of the point
  //! solution of this algorithm.
  //! Exceptions
  //! StdFail_NotDone if the computation was not
  //! successful, or was not done.
  Standard_Real Parameter () const
  {
    return myComputer.Parameter ();
  }

private:
  CPnts_AbscissaPoint myComputer;
};

#endif // _GCPnts_AbscissaPoint_HeaderFile
