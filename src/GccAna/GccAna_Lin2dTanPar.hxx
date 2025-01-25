// Created on: 1991-04-03
// Created by: Remi GILET
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

#ifndef _GccAna_Lin2dTanPar_HeaderFile
#define _GccAna_Lin2dTanPar_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Standard_Integer.hxx>
#include <TColgp_Array1OfLin2d.hxx>
#include <GccEnt_Array1OfPosition.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <GccEnt_Position.hxx>
class gp_Pnt2d;
class gp_Lin2d;
class GccEnt_QualifiedCirc;

//! This class implements the algorithms used to create 2d
//! line tangent to a circle or a point and parallel to
//! another line.
//! The solution has the same orientation as the
//! second argument.
//! Describes functions for building a 2D line parallel to a line and:
//! -   tangential to a circle, or
//! -   passing through a point.
//! A Lin2dTanPar object provides a framework for:
//! -   defining the construction of 2D line(s),
//! -   implementing the construction algorithm, and consulting the result(s).
class GccAna_Lin2dTanPar
{
public:
  DEFINE_STANDARD_ALLOC

  //! This method implements the algorithms used to create a 2d
  //! line passing through a point and parallel to
  //! another line.
  Standard_EXPORT GccAna_Lin2dTanPar(const gp_Pnt2d& ThePoint, const gp_Lin2d& Lin1);

  //! This method implements the algorithms used to create a 2d
  //! line tangent to a circle and parallel to another line.
  //! It raises BadQualifier in case of EnclosedCirc.
  //! Exceptions
  //! GccEnt_BadQualifier if a qualifier is inconsistent with
  //! the argument it qualifies (for example, enclosed for a circle).
  Standard_EXPORT GccAna_Lin2dTanPar(const GccEnt_QualifiedCirc& Qualified1, const gp_Lin2d& Lin1);

  //! Returns True if the algorithm succeeded.
  Standard_EXPORT Standard_Boolean IsDone() const;

  //! Returns the number of solutions.
  //! Raises NotDone if the construction algorithm  didn't succeed.
  Standard_EXPORT Standard_Integer NbSolutions() const;

  //! Returns the solution number Index and raises OutOfRange
  //! exception if Index is greater than the number of solutions.
  //! Be careful: the Index is only a way to get all the
  //! solutions, but is not associated to those outside the
  //! context of the algorithm-object.
  //! raises NotDone if the construction algorithm
  //! didn't succeed.
  //! It raises OutOfRange if Index is greater than the
  //! number of solutions.
  Standard_EXPORT gp_Lin2d ThisSolution(const Standard_Integer Index) const;

  //! Returns the information about the qualifiers of the
  //! tangency arguments concerning the solution number Index.
  //! It returns the real qualifiers (the qualifiers given to the
  //! constructor method in case of enclosed, enclosing and outside
  //! and the qualifiers computed in case of unqualified).
  //! Raises NotDone if the construction algorithm
  //! didn't succeed.
  //! It raises OutOfRange if Index is greater than the
  //! number of solutions.
  Standard_EXPORT void WhichQualifier(const Standard_Integer Index, GccEnt_Position& Qualif1) const;

  //! Returns information about the tangency point between the
  //! result number Index and the first argument.
  //! ParSol is the intrinsic parameter of the point on the
  //! solution curv.
  //! ParArg is the intrinsic parameter of the point on the
  //! argument curv.
  //! ParArg is equal 0 when the solution is passing through
  //! a point. Raises NotDone if the construction algorithm
  //! didn't succeed.
  //! It raises OutOfRange if Index is greater than the
  //! number of solutions.
  Standard_EXPORT void Tangency1(const Standard_Integer Index,
                                 Standard_Real&         ParSol,
                                 Standard_Real&         ParArg,
                                 gp_Pnt2d&              Pnt) const;

protected:
private:
  Standard_Boolean        WellDone;
  Standard_Integer        NbrSol;
  TColgp_Array1OfLin2d    linsol;
  GccEnt_Array1OfPosition qualifier1;
  TColgp_Array1OfPnt2d    pnttg1sol;
  TColStd_Array1OfReal    par1sol;
  TColStd_Array1OfReal    pararg1;
};

#endif // _GccAna_Lin2dTanPar_HeaderFile
