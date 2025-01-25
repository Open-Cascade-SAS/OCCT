// Created on: 1992-10-20
// Created by: Remi GILET
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

#ifndef _Geom2dGcc_Lin2dTanObl_HeaderFile
#define _Geom2dGcc_Lin2dTanObl_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Standard_Integer.hxx>
#include <TColgp_Array1OfLin2d.hxx>
#include <GccEnt_Array1OfPosition.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <GccEnt_Position.hxx>
class Geom2dGcc_QualifiedCurve;
class gp_Lin2d;
class gp_Pnt2d;
class Geom2dGcc_Lin2dTanOblIter;
class Geom2dAdaptor_Curve;

//! This class implements the algorithms used to
//! create 2d line tangent to a curve QualifiedCurv and
//! doing an angle Angle with a line TheLin.
//! The angle must be in Radian.
//! Describes functions for building a 2D line making a given
//! angle with a line and tangential to a curve.
//! A Lin2dTanObl object provides a framework for:
//! -   defining the construction of 2D line(s),
//! -   implementing the construction algorithm, and
//! -   consulting the result(s).
class Geom2dGcc_Lin2dTanObl
{
public:
  DEFINE_STANDARD_ALLOC

  //! This class implements the algorithm used to
  //! create 2d line tangent to a curve and doing an
  //! angle Angle with the line TheLin.
  //! Angle must be in Radian.
  //! Tolang is the angular tolerance.
  Standard_EXPORT Geom2dGcc_Lin2dTanObl(const Geom2dGcc_QualifiedCurve& Qualified1,
                                        const gp_Lin2d&                 TheLin,
                                        const Standard_Real             TolAng,
                                        const Standard_Real             Angle);

  //! This class implements the algorithm used to
  //! create 2d line tangent to a curve and doing an
  //! angle Angle with the line TheLin.
  //! Angle must be in Radian.
  //! Param2 is the initial guess on the curve QualifiedCurv.
  //! Tolang is the angular tolerance.
  //! Warning
  //! An iterative algorithm is used if Qualified1 is more
  //! complex than a line or a circle. In such cases, the
  //! algorithm constructs only one solution.
  //! Exceptions
  //! GccEnt_BadQualifier if a qualifier is inconsistent with
  //! the argument it qualifies (for example, enclosed for a circle).
  Standard_EXPORT Geom2dGcc_Lin2dTanObl(const Geom2dGcc_QualifiedCurve& Qualified1,
                                        const gp_Lin2d&                 TheLin,
                                        const Standard_Real             TolAng,
                                        const Standard_Real             Param1,
                                        const Standard_Real             Angle);

  //! Returns true if the construction algorithm does not fail
  //! (even if it finds no solution).
  //! Note: IsDone protects against a failure arising from a
  //! more internal intersection algorithm, which has reached its numeric limits.
  Standard_EXPORT Standard_Boolean IsDone() const;

  //! Returns the number of lines, representing solutions computed by this algorithm.
  //! Exceptions
  //! StdFail_NotDone if the construction fails.
  Standard_EXPORT Standard_Integer NbSolutions() const;

  //! Returns a line, representing the solution of index Index
  //! computed by this algorithm.
  //! Exceptions
  //! Standard_OutOfRange if Index is less than zero or
  //! greater than the number of solutions computed by this algorithm.
  //! StdFail_NotDone if the construction fails.
  Standard_EXPORT gp_Lin2d ThisSolution(const Standard_Integer Index) const;

  //! Returns the qualifier Qualif1 of the tangency argument
  //! for the solution of index Index computed by this algorithm.
  //! The returned qualifier is:
  //! -   that specified at the start of construction when the
  //! solutions are defined as enclosing or outside with
  //! respect to the argument, or
  //! -   that computed during construction (i.e. enclosing or
  //! outside) when the solutions are defined as unqualified
  //! with respect to the argument, or
  //! -   GccEnt_noqualifier if the tangency argument is a point.
  //! Exceptions
  //! Standard_OutOfRange if Index is less than zero or
  //! greater than the number of solutions computed by this algorithm.
  //! StdFail_NotDone if the construction fails.
  Standard_EXPORT void WhichQualifier(const Standard_Integer Index, GccEnt_Position& Qualif1) const;

  //! Returns information about the tangency point between the
  //! result and the first argument.
  //! ParSol is the intrinsic parameter of the point PntSol on
  //! the solution curv.
  //! ParArg is the intrinsic parameter of the point PntSol on
  //! the argument curv.
  Standard_EXPORT void Tangency1(const Standard_Integer Index,
                                 Standard_Real&         ParSol,
                                 Standard_Real&         ParArg,
                                 gp_Pnt2d&              PntSol) const;

  //! Returns the point of intersection PntSol between the
  //! solution of index Index and the second argument (the line) of this algorithm.
  //! ParSol is the parameter of the point PntSol on the
  //! solution. ParArg is the parameter of the point PntSol on the second argument (the line).
  //! Exceptions
  //! StdFail_NotDone if the construction fails.
  //! Geom2dGcc_IsParallel if the solution and the second
  //! argument (the line) are parallel.
  //! Standard_OutOfRange if Index is less than zero or
  //! greater than the number of solutions computed by this algorithm.
  Standard_EXPORT void Intersection2(const Standard_Integer Index,
                                     Standard_Real&         ParSol,
                                     Standard_Real&         ParArg,
                                     gp_Pnt2d&              PntSol) const;

  //! Returns true if the line and the solution are parallel. This
  //! is the case when the angle given at the time of
  //! construction is equal to 0 or Pi.
  //! Exceptions StdFail_NotDone if the construction fails.
  Standard_EXPORT Standard_Boolean IsParallel2() const;

protected:
private:
  Standard_EXPORT Standard_Boolean Add(const Standard_Integer           theIndex,
                                       const Geom2dGcc_Lin2dTanOblIter& theLin,
                                       const Standard_Real              theTol,
                                       const Geom2dAdaptor_Curve&       theC1);

  Standard_Boolean        WellDone;
  Standard_Boolean        Paral2;
  Standard_Integer        NbrSol;
  TColgp_Array1OfLin2d    linsol;
  GccEnt_Array1OfPosition qualifier1;
  TColgp_Array1OfPnt2d    pnttg1sol;
  TColgp_Array1OfPnt2d    pntint2sol;
  TColStd_Array1OfReal    par1sol;
  TColStd_Array1OfReal    par2sol;
  TColStd_Array1OfReal    pararg1;
  TColStd_Array1OfReal    pararg2;
};

#endif // _Geom2dGcc_Lin2dTanObl_HeaderFile
