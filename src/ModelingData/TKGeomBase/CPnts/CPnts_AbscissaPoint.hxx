// Created on: 1991-07-15
// Created by: Isabelle GRIGNON
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

#ifndef _CPnts_AbscissaPoint_HeaderFile
#define _CPnts_AbscissaPoint_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <CPnts_MyRootFunction.hxx>
class Adaptor3d_Curve;
class Adaptor2d_Curve2d;

//! the algorithm computes a point on a curve at a given
//! distance from another point on the curve
//!
//! We can instantiates with
//! Curve from Adaptor3d, Pnt from gp, Vec from gp
//!
//! or
//! Curve2d from Adaptor2d, Pnt2d from gp, Vec2d from gp
class CPnts_AbscissaPoint
{
public:
  DEFINE_STANDARD_ALLOC

  //! Computes the length of the Curve <C>.
  Standard_EXPORT static Standard_Real Length(const Adaptor3d_Curve& C);

  //! Computes the length of the Curve <C>.
  Standard_EXPORT static Standard_Real Length(const Adaptor2d_Curve2d& C);

  //! Computes the length of the Curve <C> with the given tolerance.
  Standard_EXPORT static Standard_Real Length(const Adaptor3d_Curve& C, const Standard_Real Tol);

  //! Computes the length of the Curve <C> with the given tolerance.
  Standard_EXPORT static Standard_Real Length(const Adaptor2d_Curve2d& C, const Standard_Real Tol);

  //! Computes the length of the Curve <C> between <U1> and <U2>.
  Standard_EXPORT static Standard_Real Length(const Adaptor3d_Curve& C,
                                              const Standard_Real    U1,
                                              const Standard_Real    U2);

  //! Computes the length of the Curve <C> between <U1> and <U2>.
  Standard_EXPORT static Standard_Real Length(const Adaptor2d_Curve2d& C,
                                              const Standard_Real      U1,
                                              const Standard_Real      U2);

  //! Computes the length of the Curve <C> between <U1> and <U2> with the given tolerance.
  Standard_EXPORT static Standard_Real Length(const Adaptor3d_Curve& C,
                                              const Standard_Real    U1,
                                              const Standard_Real    U2,
                                              const Standard_Real    Tol);

  //! Computes the length of the Curve <C> between <U1> and <U2> with the given tolerance.
  //! creation of a indefinite AbscissaPoint.
  Standard_EXPORT static Standard_Real Length(const Adaptor2d_Curve2d& C,
                                              const Standard_Real      U1,
                                              const Standard_Real      U2,
                                              const Standard_Real      Tol);

  Standard_EXPORT CPnts_AbscissaPoint();

  //! the algorithm computes a point on a curve <Curve> at the
  //! distance <Abscissa> from the point of parameter <U0>.
  //! <Resolution> is the error allowed in the computation.
  //! The computed point can be outside of the curve 's bounds.
  Standard_EXPORT CPnts_AbscissaPoint(const Adaptor3d_Curve& C,
                                      const Standard_Real    Abscissa,
                                      const Standard_Real    U0,
                                      const Standard_Real    Resolution);

  //! the algorithm computes a point on a curve <Curve> at the
  //! distance <Abscissa> from the point of parameter <U0>.
  //! <Resolution> is the error allowed in the computation.
  //! The computed point can be outside of the curve 's bounds.
  Standard_EXPORT CPnts_AbscissaPoint(const Adaptor2d_Curve2d& C,
                                      const Standard_Real      Abscissa,
                                      const Standard_Real      U0,
                                      const Standard_Real      Resolution);

  //! the algorithm computes a point on a curve <Curve> at the
  //! distance <Abscissa> from the point of parameter <U0>.
  //! <Ui> is the starting value used in the iterative process
  //! which find the solution, it must be closed to the final
  //! solution
  //! <Resolution> is the error allowed in the computation.
  //! The computed point can be outside of the curve 's bounds.
  Standard_EXPORT CPnts_AbscissaPoint(const Adaptor3d_Curve& C,
                                      const Standard_Real    Abscissa,
                                      const Standard_Real    U0,
                                      const Standard_Real    Ui,
                                      const Standard_Real    Resolution);

  //! the algorithm computes a point on a curve <Curve> at the
  //! distance <Abscissa> from the point of parameter <U0>.
  //! <Ui> is the starting value used in the iterative process
  //! which find the solution, it must be closed to the final
  //! solution
  //! <Resolution> is the error allowed in the computation.
  //! The computed point can be outside of the curve 's bounds.
  Standard_EXPORT CPnts_AbscissaPoint(const Adaptor2d_Curve2d& C,
                                      const Standard_Real      Abscissa,
                                      const Standard_Real      U0,
                                      const Standard_Real      Ui,
                                      const Standard_Real      Resolution);

  //! Initializes the resolution function with <C>.
  Standard_EXPORT void Init(const Adaptor3d_Curve& C);

  //! Initializes the resolution function with <C>.
  Standard_EXPORT void Init(const Adaptor2d_Curve2d& C);

  //! Initializes the resolution function with <C>.
  Standard_EXPORT void Init(const Adaptor3d_Curve& C, const Standard_Real Tol);

  //! Initializes the resolution function with <C>.
  Standard_EXPORT void Init(const Adaptor2d_Curve2d& C, const Standard_Real Tol);

  //! Initializes the resolution function with <C>
  //! between U1 and U2.
  Standard_EXPORT void Init(const Adaptor3d_Curve& C,
                            const Standard_Real    U1,
                            const Standard_Real    U2);

  //! Initializes the resolution function with <C>
  //! between U1 and U2.
  Standard_EXPORT void Init(const Adaptor2d_Curve2d& C,
                            const Standard_Real      U1,
                            const Standard_Real      U2);

  //! Initializes the resolution function with <C>
  //! between U1 and U2.
  Standard_EXPORT void Init(const Adaptor3d_Curve& C,
                            const Standard_Real    U1,
                            const Standard_Real    U2,
                            const Standard_Real    Tol);

  //! Initializes the resolution function with <C>
  //! between U1 and U2.
  Standard_EXPORT void Init(const Adaptor2d_Curve2d& C,
                            const Standard_Real      U1,
                            const Standard_Real      U2,
                            const Standard_Real      Tol);

  //! Computes the point at the distance <Abscissa> of
  //! the curve.
  //! U0 is the parameter of the point from which the distance
  //! is measured.
  Standard_EXPORT void Perform(const Standard_Real Abscissa,
                               const Standard_Real U0,
                               const Standard_Real Resolution);

  //! Computes the point at the distance <Abscissa> of
  //! the curve.
  //! U0 is the parameter of the point from which the distance
  //! is measured and Ui is the starting value for the iterative
  //! process (should be close to the final solution).
  Standard_EXPORT void Perform(const Standard_Real Abscissa,
                               const Standard_Real U0,
                               const Standard_Real Ui,
                               const Standard_Real Resolution);

  //! Computes the point at the distance <Abscissa> of
  //! the curve; performs more appropriate tolerance management;
  //! to use this method in right way it is necessary to call
  //! empty constructor. then call method Init with
  //! Tolerance = Resolution, then call AdvPermorm.
  //! U0 is the parameter of the point from which the distance
  //! is measured and Ui is the starting value for the iterative
  //! process (should be close to the final solution).
  Standard_EXPORT void AdvPerform(const Standard_Real Abscissa,
                                  const Standard_Real U0,
                                  const Standard_Real Ui,
                                  const Standard_Real Resolution);

  //! True if the computation was successful, False otherwise.
  Standard_Boolean IsDone() const;

  //! Returns the parameter of the solution.
  Standard_Real Parameter() const;

  //! Enforce the solution, used by GCPnts.
  void SetParameter(const Standard_Real P);

protected:
private:
  Standard_Boolean     myDone;
  Standard_Real        myL;
  Standard_Real        myParam;
  Standard_Real        myUMin;
  Standard_Real        myUMax;
  CPnts_MyRootFunction myF;
};

#include <CPnts_AbscissaPoint.lxx>

#endif // _CPnts_AbscissaPoint_HeaderFile
