// Created on: 1991-03-22
// Created by: Remy GILET
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

#ifndef _GccAna_Circ2d2TanOn_HeaderFile
#define _GccAna_Circ2d2TanOn_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Standard_Integer.hxx>
#include <gp_Circ2d.hxx>
#include <NCollection_Array1.hxx>
#include <GccEnt_Position.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array1.hxx>
#include <GccEnt_Position.hxx>
class GccEnt_QualifiedCirc;
class gp_Lin2d;
class GccEnt_QualifiedLin;
class gp_Pnt2d;
class gp_Circ2d;

//! Describes functions for building a 2D circle
//! -   tangential to 2 curves, or
//! -   tangential to a curve and passing through a point, or
//! -   passing through 2 points,
//! and with its center on a curve. For these analytic
//! algorithms, curves are circles or lines.
//! A Circ2d2TanOn object provides a framework for:
//! -   defining the construction of 2D circles(s),
//! -   implementing the construction algorithm, and
//! -   consulting the result(s).
class GccAna_Circ2d2TanOn
{
public:
  DEFINE_STANDARD_ALLOC

  //! This method implements the algorithms used to
  //! create 2d circles TANgent to two 2d circles and
  //! having the center ON a 2d line.
  Standard_EXPORT GccAna_Circ2d2TanOn(const GccEnt_QualifiedCirc& Qualified1,
                                      const GccEnt_QualifiedCirc& Qualified2,
                                      const gp_Lin2d&             OnLine,
                                      const double         Tolerance);

  //! This method implements the algorithms used to
  //! create 2d circles TANgent to a 2d circle and a 2d line
  //! having the center ON a 2d line.
  Standard_EXPORT GccAna_Circ2d2TanOn(const GccEnt_QualifiedCirc& Qualified1,
                                      const GccEnt_QualifiedLin&  Qualified2,
                                      const gp_Lin2d&             OnLine,
                                      const double         Tolerance);

  //! This method implements the algorithms used to
  //! create 2d circles TANgent to two 2d lines
  //! having the center ON a 2d line.
  Standard_EXPORT GccAna_Circ2d2TanOn(const GccEnt_QualifiedLin& Qualified1,
                                      const GccEnt_QualifiedLin& Qualified2,
                                      const gp_Lin2d&            OnLine,
                                      const double        Tolerance);

  //! This method implements the algorithms used to
  //! create 2d circles TANgent to a 2d circle and a point
  //! having the center ON a 2d line.
  Standard_EXPORT GccAna_Circ2d2TanOn(const GccEnt_QualifiedCirc& Qualified1,
                                      const gp_Pnt2d&             Point2,
                                      const gp_Lin2d&             OnLine,
                                      const double         Tolerance);

  //! This method implements the algorithms used to
  //! create 2d circles TANgent to a 2d line and a point
  //! having the center ON a 2d line.
  Standard_EXPORT GccAna_Circ2d2TanOn(const GccEnt_QualifiedLin& Qualified1,
                                      const gp_Pnt2d&            Point2,
                                      const gp_Lin2d&            OnLine,
                                      const double        Tolerance);

  //! This method implements the algorithms used to
  //! create 2d circles TANgent to two points
  //! having the center ON a 2d line.
  Standard_EXPORT GccAna_Circ2d2TanOn(const gp_Pnt2d&     Point1,
                                      const gp_Pnt2d&     Point2,
                                      const gp_Lin2d&     OnLine,
                                      const double Tolerance);

  //! This method implements the algorithms used to
  //! create 2d circles TANgent to two 2d circles and
  //! having the center ON a 2d circle.
  Standard_EXPORT GccAna_Circ2d2TanOn(const GccEnt_QualifiedCirc& Qualified1,
                                      const GccEnt_QualifiedCirc& Qualified2,
                                      const gp_Circ2d&            OnCirc,
                                      const double         Tolerance);

  //! This method implements the algorithms used to
  //! create 2d circles TANgent to a circle and a line
  //! having the center ON a 2d circle.
  Standard_EXPORT GccAna_Circ2d2TanOn(const GccEnt_QualifiedCirc& Qualified1,
                                      const GccEnt_QualifiedLin&  Qualified2,
                                      const gp_Circ2d&            OnCirc,
                                      const double         Tolerance);

  //! This method implements the algorithms used to
  //! create 2d circles TANgent to a circle and a point
  //! having the center ON a 2d circle.
  Standard_EXPORT GccAna_Circ2d2TanOn(const GccEnt_QualifiedCirc& Qualified1,
                                      const gp_Pnt2d&             Point2,
                                      const gp_Circ2d&            OnCirc,
                                      const double         Tolerance);

  //! This method implements the algorithms used to
  //! create 2d circles TANgent to two 2d lines
  //! having the center ON a 2d circle.
  Standard_EXPORT GccAna_Circ2d2TanOn(const GccEnt_QualifiedLin& Qualified1,
                                      const GccEnt_QualifiedLin& Qualified2,
                                      const gp_Circ2d&           OnCirc,
                                      const double        Tolerance);

  //! This method implements the algorithms used to
  //! create 2d circles TANgent to a line and a point
  //! having the center ON a 2d circle.
  Standard_EXPORT GccAna_Circ2d2TanOn(const GccEnt_QualifiedLin& Qualified1,
                                      const gp_Pnt2d&            Point2,
                                      const gp_Circ2d&           OnCirc,
                                      const double        Tolerance);

  //! This method implements the algorithms used to create
  //! 2d circles TANgent to two points having the center ON
  //! a 2d circle.
  Standard_EXPORT GccAna_Circ2d2TanOn(const gp_Pnt2d&     Point1,
                                      const gp_Pnt2d&     Point2,
                                      const gp_Circ2d&    OnCirc,
                                      const double Tolerance);

  //! Returns true if the construction algorithm does not fail
  //! (even if it finds no solution).
  //! Note: IsDone protects against a failure arising from a
  //! more internal intersection algorithm, which has reached its numeric limits.
  Standard_EXPORT bool IsDone() const;

  //! Returns the number of circles, representing solutions
  //! computed by this algorithm.
  //! Exceptions
  //! StdFail_NotDone if the construction fails.
  Standard_EXPORT int NbSolutions() const;

  //! Returns the solution number Index and raises OutOfRange
  //! exception if Index is greater than the number of solutions.
  //! Be careful: the Index is only a way to get all the
  //! solutions, but is not associated to those outside the context
  //! of the algorithm-object.
  //! Exceptions
  //! Standard_OutOfRange if Index is less than zero or
  //! greater than the number of solutions computed by this algorithm.
  //! StdFail_NotDone if the construction fails.
  Standard_EXPORT gp_Circ2d ThisSolution(const int Index) const;

  //! Returns the qualifiers Qualif1 and Qualif2 of the
  //! tangency arguments for the solution of index Index
  //! computed by this algorithm.
  //! The returned qualifiers are:
  //! -   those specified at the start of construction when the
  //! solutions are defined as enclosed, enclosing or
  //! outside with respect to the arguments, or
  //! -   those computed during construction (i.e. enclosed,
  //! enclosing or outside) when the solutions are defined
  //! as unqualified with respect to the arguments, or
  //! -   GccEnt_noqualifier if the tangency argument is a point.
  //! Exceptions
  //! Standard_OutOfRange if Index is less than zero or
  //! greater than the number of solutions computed by this algorithm.
  //! StdFail_NotDone if the construction fails.
  Standard_EXPORT void WhichQualifier(const int Index,
                                      GccEnt_Position&       Qualif1,
                                      GccEnt_Position&       Qualif2) const;

  //! Returns the information about the tangency point between the
  //! result number Index and the first argument.
  //! ParSol is the intrinsic parameter of the point PntSol on
  //! the solution
  //! ParArg is the intrinsic parameter of the point PntSol on
  //! the first argument. Raises OutOfRange if Index is greater than the number
  //! of solutions and NotDone if IsDone returns false.
  Standard_EXPORT void Tangency1(const int Index,
                                 double&         ParSol,
                                 double&         ParArg,
                                 gp_Pnt2d&              PntSol) const;

  //! Returns the information about the tangency point between the
  //! result number Index and the second argument.
  //! ParSol is the intrinsic parameter of the point PntSol on
  //! the solution.
  //! ParArg is the intrinsic parameter of the point PntSol on
  //! the second argument. Raises OutOfRange if Index is greater than the number
  //! of solutions and NotDone if IsDone returns false.
  Standard_EXPORT void Tangency2(const int Index,
                                 double&         ParSol,
                                 double&         ParArg,
                                 gp_Pnt2d&              PntSol) const;

  //! Returns the information about the center (on the curv) of
  //! the result number Index and the third argument.
  //! ParArg is the intrinsic parameter of the point PntArg on
  //! the third argument.
  //! Exceptions
  //! Standard_OutOfRange if Index is less than zero or
  //! greater than the number of solutions computed by this algorithm.
  //! StdFail_NotDone if the construction fails.
  Standard_EXPORT void CenterOn3(const int Index,
                                 double&         ParArg,
                                 gp_Pnt2d&              PntArg) const;

  //! True if the solution and the first argument are the same
  //! (2 circles).
  //! If R1 is the radius of the first argument and Rsol the radius
  //! of the solution and dist the distance between the two centers,
  //! we consider the two circles are identical if R1+dist-Rsol is
  //! less than Tolerance.
  //! False in the other cases.
  //! Raises OutOfRange if Index is greater than the number
  //! of solutions and NotDone if IsDone returns false.
  Standard_EXPORT bool IsTheSame1(const int Index) const;

  //! True if the solution and the second argument are the same
  //! (2 circles).
  //! If R2 is the radius of the second argument and Rsol the radius
  //! of the solution and dist the distance between the two centers,
  //! we consider the two circles are identical if R2+dist-Rsol is
  //! less than Tolerance.
  //! False in the other cases.
  //! Raises OutOfRange if Index is greater than the number
  //! of solutions and NotDone if IsDone returns false.
  Standard_EXPORT bool IsTheSame2(const int Index) const;

private:
  bool        WellDone;
  int        NbrSol;
  NCollection_Array1<gp_Circ2d>   cirsol;
  NCollection_Array1<GccEnt_Position> qualifier1;
  NCollection_Array1<GccEnt_Position> qualifier2;
  NCollection_Array1<int> TheSame1;
  NCollection_Array1<int> TheSame2;
  NCollection_Array1<gp_Pnt2d>    pnttg1sol;
  NCollection_Array1<gp_Pnt2d>    pnttg2sol;
  NCollection_Array1<gp_Pnt2d>    pntcen;
  NCollection_Array1<double>    par1sol;
  NCollection_Array1<double>    par2sol;
  NCollection_Array1<double>    pararg1;
  NCollection_Array1<double>    pararg2;
  NCollection_Array1<double>    parcen3;
};

#endif // _GccAna_Circ2d2TanOn_HeaderFile
