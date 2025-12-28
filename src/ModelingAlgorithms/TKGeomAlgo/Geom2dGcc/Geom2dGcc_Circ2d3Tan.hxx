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

#ifndef _Geom2dGcc_Circ2d3Tan_HeaderFile
#define _Geom2dGcc_Circ2d3Tan_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Circ2d.hxx>
#include <NCollection_Array1.hxx>
#include <GccEnt_Position.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <GccEnt_Position.hxx>
class Geom2dGcc_QualifiedCurve;
class Geom2d_Point;
class GccAna_Circ2d3Tan;
class gp_Circ2d;
class gp_Pnt2d;

//! This class implements the algorithms used to
//! create 2d circles tangent to 3 points/lines/circles/
//! curves with one curve or more.
//! The arguments of all construction methods are :
//! - The three qualifiied elements for the
//! tangency constrains (QualifiedCirc, QualifiedLine,
//! Qualifiedcurv, Points).
//! - A parameter for each QualifiedCurv.
//! Describes functions for building a 2D circle:
//! -   tangential to 3 curves, or
//! -   tangential to 2 curves and passing through a point, or
//! -   tangential to a curve and passing through 2 points, or
//! -   passing through 3 points.
//! A Circ2d3Tan object provides a framework for:
//! -   defining the construction of 2D circles(s),
//! -   implementing the construction algorithm, and
//! -   consulting the result(s).
class Geom2dGcc_Circ2d3Tan
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructs one or more 2D circles
  //! tangential to three curves Qualified1, Qualified2 and
  //! Qualified3, where Param1, Param2 and Param3 are
  //! used, respectively, as the initial values of the
  //! parameters on Qualified1, Qualified2 and Qualified3
  //! of the tangency point between these arguments and
  //! the solution sought, if the algorithm chooses an
  //! iterative method to find the solution (i.e. if either
  //! Qualified1, Qualified2 or Qualified3 is more complex
  //! than a line or a circle).
  Standard_EXPORT Geom2dGcc_Circ2d3Tan(const Geom2dGcc_QualifiedCurve& Qualified1,
                                       const Geom2dGcc_QualifiedCurve& Qualified2,
                                       const Geom2dGcc_QualifiedCurve& Qualified3,
                                       const double             Tolerance,
                                       const double             Param1,
                                       const double             Param2,
                                       const double             Param3);

  //! Constructs one or more 2D circles
  //! tangential to two curves Qualified1 and Qualified2
  //! and passing through the point Point, where Param1
  //! and Param2 are used, respectively, as the initial
  //! values of the parameters on Qualified1 and
  //! Qualified2 of the tangency point between this
  //! argument and the solution sought, if the algorithm
  //! chooses an iterative method to find the solution (i.e. if
  //! either Qualified1 or Qualified2 is more complex than
  //! a line or a circle).
  Standard_EXPORT Geom2dGcc_Circ2d3Tan(const Geom2dGcc_QualifiedCurve& Qualified1,
                                       const Geom2dGcc_QualifiedCurve& Qualified2,
                                       const occ::handle<Geom2d_Point>&     Point,
                                       const double             Tolerance,
                                       const double             Param1,
                                       const double             Param2);

  //! Constructs one or more 2D circles tangential to the curve Qualified1 and passing
  //! through two points Point1 and Point2, where Param1
  //! is used as the initial value of the parameter on
  //! Qualified1 of the tangency point between this
  //! argument and the solution sought, if the algorithm
  //! chooses an iterative method to find the solution (i.e. if
  //! Qualified1 is more complex than a line or a circle)
  Standard_EXPORT Geom2dGcc_Circ2d3Tan(const Geom2dGcc_QualifiedCurve& Qualified1,
                                       const occ::handle<Geom2d_Point>&     Point1,
                                       const occ::handle<Geom2d_Point>&     Point2,
                                       const double             Tolerance,
                                       const double             Param1);

  //! Constructs one or more 2D circles passing through three points Point1, Point2 and Point3.
  //! Tolerance is a tolerance criterion used by the algorithm
  //! to find a solution when, mathematically, the problem
  //! posed does not have a solution, but where there is
  //! numeric uncertainty attached to the arguments.
  //! For example, take:
  //! -   two circles C1 and C2, such that C2 is inside C1,
  //! and almost tangential to C1; there is in fact no point
  //! of intersection between C1 and C2; and
  //! -   a circle C3 outside C1.
  //! You now want to find a circle which is tangential to C1,
  //! C2 and C3: a pure mathematical resolution will not find
  //! a solution. This is where the tolerance criterion is used:
  //! the algorithm considers that C1 and C2 are tangential if
  //! the shortest distance between these two circles is less
  //! than or equal to Tolerance. Thus, the algorithm finds a solution.
  //! Warning
  //! An iterative algorithm is used if Qualified1, Qualified2 or
  //! Qualified3 is more complex than a line or a circle. In
  //! such cases, the algorithm constructs only one solution.
  //! Exceptions
  //! GccEnt_BadQualifier if a qualifier is inconsistent with
  //! the argument it qualifies (for example, enclosing for a line).
  Standard_EXPORT Geom2dGcc_Circ2d3Tan(const occ::handle<Geom2d_Point>& Point1,
                                       const occ::handle<Geom2d_Point>& Point2,
                                       const occ::handle<Geom2d_Point>& Point3,
                                       const double         Tolerance);

  Standard_EXPORT void Results(const GccAna_Circ2d3Tan& Circ,
                               const int   Rank1,
                               const int   Rank2,
                               const int   Rank3);

  //! Returns true if the construction algorithm does not fail (even if it finds no solution).
  //! Note: IsDone protects against a failure arising from a
  //! more internal intersection algorithm, which has reached its numeric limits.
  Standard_EXPORT bool IsDone() const;

  //! This method returns the number of solutions.
  //! NotDone is raised if the algorithm failed.
  Standard_EXPORT int NbSolutions() const;

  //! Returns the solution number Index and raises OutOfRange
  //! exception if Index is greater than the number of solutions.
  //! Be careful: the Index is only a way to get all the
  //! solutions, but is not associated to these outside the context
  //! of the algorithm-object.
  Standard_EXPORT gp_Circ2d ThisSolution(const int Index) const;

  //! It returns the information about the qualifiers of the tangency
  //! arguments concerning the solution number Index.
  //! It returns the real qualifiers (the qualifiers given to the
  //! constructor method in case of enclosed, enclosing and outside
  //! and the qualifiers computedin case of unqualified).
  Standard_EXPORT void WhichQualifier(const int Index,
                                      GccEnt_Position&       Qualif1,
                                      GccEnt_Position&       Qualif2,
                                      GccEnt_Position&       Qualif3) const;

  //! Returns information about the tangency point between the
  //! result and the first argument.
  //! ParSol is the intrinsic parameter of the point PntSol on the solution curv.
  //! ParArg is the intrinsic parameter of the point PntSol on the argument curv.
  Standard_EXPORT void Tangency1(const int Index,
                                 double&         ParSol,
                                 double&         ParArg,
                                 gp_Pnt2d&              PntSol) const;

  //! Returns information about the tangency point between the
  //! result and the second argument.
  //! ParSol is the intrinsic parameter of the point PntSol on the solution curv.
  //! ParArg is the intrinsic parameter of the point PntSol on the argument curv.
  Standard_EXPORT void Tangency2(const int Index,
                                 double&         ParSol,
                                 double&         ParArg,
                                 gp_Pnt2d&              PntSol) const;

  //! Returns information about the tangency point between the
  //! result and the third argument.
  //! ParSol is the intrinsic parameter of the point PntSol on the solution curv.
  //! ParArg is the intrinsic parameter of the point PntSol on the argument curv.
  Standard_EXPORT void Tangency3(const int Index,
                                 double&         ParSol,
                                 double&         ParArg,
                                 gp_Pnt2d&              PntSol) const;

  //! Returns True if the solution is equal to the first argument.
  Standard_EXPORT bool IsTheSame1(const int Index) const;

  //! Returns True if the solution is equal to the second argument.
  Standard_EXPORT bool IsTheSame2(const int Index) const;

  //! Returns True if the solution is equal to the third argument.
  //! If Rarg is the radius of the first, second or third
  //! argument, Rsol is the radius of the solution and dist
  //! is the distance between the two centers, we consider
  //! the two circles to be identical if |Rarg - Rsol| and
  //! dist are less than or equal to the tolerance criterion
  //! given at the time of construction of this algorithm.
  //! Exceptions
  //! Standard_OutOfRange if Index is less than zero or
  //! greater than the number of solutions computed by this algorithm.
  //! StdFail_NotDone if the construction fails.
  Standard_EXPORT bool IsTheSame3(const int Index) const;

private:
  NCollection_Array1<gp_Circ2d>   cirsol;
  double           NbrSol;
  bool        WellDone;
  NCollection_Array1<GccEnt_Position> qualifier1;
  NCollection_Array1<GccEnt_Position> qualifier2;
  NCollection_Array1<GccEnt_Position> qualifier3;
  NCollection_Array1<int> TheSame1;
  NCollection_Array1<int> TheSame2;
  NCollection_Array1<int> TheSame3;
  NCollection_Array1<gp_Pnt2d>    pnttg1sol;
  NCollection_Array1<gp_Pnt2d>    pnttg2sol;
  NCollection_Array1<gp_Pnt2d>    pnttg3sol;
  NCollection_Array1<double>    par1sol;
  NCollection_Array1<double>    par2sol;
  NCollection_Array1<double>    par3sol;
  NCollection_Array1<double>    pararg1;
  NCollection_Array1<double>    pararg2;
  NCollection_Array1<double>    pararg3;
};

#endif // _Geom2dGcc_Circ2d3Tan_HeaderFile
