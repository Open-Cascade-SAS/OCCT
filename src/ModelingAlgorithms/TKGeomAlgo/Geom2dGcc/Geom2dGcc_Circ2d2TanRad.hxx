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

#ifndef _Geom2dGcc_Circ2d2TanRad_HeaderFile
#define _Geom2dGcc_Circ2d2TanRad_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColgp_Array1OfCirc2d.hxx>
#include <Standard_Integer.hxx>
#include <GccEnt_Array1OfPosition.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <GccEnt_Position.hxx>
class Geom2dGcc_QualifiedCurve;
class Geom2d_Point;
class GccAna_Circ2d2TanRad;
class Geom2dGcc_Circ2d2TanRadGeo;
class gp_Circ2d;
class gp_Pnt2d;

//! This class implements the algorithms used to
//! create 2d circles tangent to one curve and a
//! point/line/circle/curv and with a given radius.
//! For each construction methods arguments are:
//! - Two Qualified elements for tangency constrains.
//! (for example EnclosedCirc if we want the
//! solution inside the argument EnclosedCirc).
//! - Two Reals. One (Radius) for the radius and the
//! other (Tolerance) for the tolerance.
//! Tolerance is only used for the limit cases.
//! For example :
//! We want to create a circle inside a circle C1 and
//! inside a curve Cu2 with a radius Radius and a
//! tolerance Tolerance.
//! If we did not used Tolerance it is impossible to
//! find a solution in the following case : Cu2 is
//! inside C1 and there is no intersection point
//! between the two elements.
//! with Tolerance we will give a solution if the
//! lowest distance between C1 and Cu2 is lower than or
//! equal Tolerance.
class Geom2dGcc_Circ2d2TanRad
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT Geom2dGcc_Circ2d2TanRad(const Geom2dGcc_QualifiedCurve& Qualified1,
                                          const Geom2dGcc_QualifiedCurve& Qualified2,
                                          const Standard_Real             Radius,
                                          const Standard_Real             Tolerance);

  Standard_EXPORT Geom2dGcc_Circ2d2TanRad(const Geom2dGcc_QualifiedCurve& Qualified1,
                                          const Handle(Geom2d_Point)&     Point,
                                          const Standard_Real             Radius,
                                          const Standard_Real             Tolerance);

  //! These constructors create one or more 2D circles of radius Radius either
  //! -   tangential to the 2 curves Qualified1 and Qualified2,   or
  //! -   tangential to the curve Qualified1 and passing through the point Point, or
  //! -   passing through two points Point1 and Point2.
  //! Tolerance is a tolerance criterion used by the algorithm
  //! to find a solution when, mathematically, the problem
  //! posed does not have a solution, but where there is
  //! numeric uncertainty attached to the arguments.
  //! For example, take two circles C1 and C2, such that C2
  //! is inside C1, and almost tangential to C1. There is, in
  //! fact, no point of intersection between C1 and C2. You
  //! now want to find a circle of radius R (smaller than the
  //! radius of C2), which is tangential to C1 and C2, and
  //! inside these two circles: a pure mathematical resolution
  //! will not find a solution. This is where the tolerance
  //! criterion is used: the algorithm considers that C1 and
  //! C2 are tangential if the shortest distance between these
  //! two circles is less than or equal to Tolerance. Thus, a
  //! solution is found by the algorithm.
  //! Exceptions
  //! GccEnt_BadQualifier if a qualifier is inconsistent with
  //! the argument it qualifies (for example, enclosing for a line).
  //! Standard_NegativeValue if Radius is negative.
  Standard_EXPORT Geom2dGcc_Circ2d2TanRad(const Handle(Geom2d_Point)& Point1,
                                          const Handle(Geom2d_Point)& Point2,
                                          const Standard_Real         Radius,
                                          const Standard_Real         Tolerance);

  Standard_EXPORT void Results(const GccAna_Circ2d2TanRad& Circ);

  Standard_EXPORT void Results(const Geom2dGcc_Circ2d2TanRadGeo& Circ);

  //! This method returns True if the algorithm succeeded.
  //! Note: IsDone protects against a failure arising from a
  //! more internal intersection algorithm, which has reached its numeric limits.
  Standard_EXPORT Standard_Boolean IsDone() const;

  //! This method returns the number of solutions.
  //! NotDone is raised if the algorithm failed.
  //! Exceptions
  //! StdFail_NotDone if the construction fails.
  Standard_EXPORT Standard_Integer NbSolutions() const;

  //! Returns the solution number Index and raises OutOfRange
  //! exception if Index is greater than the number of solutions.
  //! Be careful: the Index is only a way to get all the
  //! solutions, but is not associated to these outside the context of the algorithm-object.
  //! Warning
  //! This indexing simply provides a means of consulting the
  //! solutions. The index values are not associated with
  //! these solutions outside the context of the algorithm object.
  //! Exceptions
  //! Standard_OutOfRange if Index is less than zero or
  //! greater than the number of solutions computed by this algorithm.
  //! StdFail_NotDone if the construction fails.
  Standard_EXPORT gp_Circ2d ThisSolution(const Standard_Integer Index) const;

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
  //! -   GccEnt_noqualifier if the tangency argument is a point, or
  //! -   GccEnt_unqualified in certain limit cases where it
  //! is impossible to qualify the solution as enclosed, enclosing or outside.
  //! Exceptions
  //! Standard_OutOfRange if Index is less than zero or
  //! greater than the number of solutions computed by this algorithm.
  //! StdFail_NotDone if the construction fails.
  Standard_EXPORT void WhichQualifier(const Standard_Integer Index,
                                      GccEnt_Position&       Qualif1,
                                      GccEnt_Position&       Qualif2) const;

  //! Returns information about the tangency point between the
  //! result number Index and the first argument.
  //! ParSol is the intrinsic parameter of the point PntSol on the solution curv.
  //! ParArg is the intrinsic parameter of the point PntSol on the argument curv.
  //! OutOfRange is raised if Index is greater than the number of solutions.
  //! notDone is raised if the construction algorithm did not succeed.
  Standard_EXPORT void Tangency1(const Standard_Integer Index,
                                 Standard_Real&         ParSol,
                                 Standard_Real&         ParArg,
                                 gp_Pnt2d&              PntSol) const;

  //! Returns information about the tangency point between the
  //! result number Index and the second argument.
  //! ParSol is the intrinsic parameter of the point PntSol on the solution curv.
  //! ParArg is the intrinsic parameter of the point PntSol on the argument curv.
  //! OutOfRange is raised if Index is greater than the number of solutions.
  //! notDone is raised if the construction algorithm did not succeed.
  Standard_EXPORT void Tangency2(const Standard_Integer Index,
                                 Standard_Real&         ParSol,
                                 Standard_Real&         ParArg,
                                 gp_Pnt2d&              PntSol) const;

  //! Returns true if the solution of index Index and,
  //! respectively, the first or second argument of this
  //! algorithm are the same (i.e. there are 2 identical circles).
  //! If Rarg is the radius of the first or second argument,
  //! Rsol is the radius of the solution and dist is the
  //! distance between the two centers, we consider the two
  //! circles to be identical if |Rarg - Rsol| and dist
  //! are less than or equal to the tolerance criterion given at
  //! the time of construction of this algorithm.
  //! OutOfRange is raised if Index is greater than the number of solutions.
  //! notDone is raised if the construction algorithm did not succeed.
  Standard_EXPORT Standard_Boolean IsTheSame1(const Standard_Integer Index) const;

  //! Returns true if the solution of index Index and,
  //! respectively, the first or second argument of this
  //! algorithm are the same (i.e. there are 2 identical circles).
  //! If Rarg is the radius of the first or second argument,
  //! Rsol is the radius of the solution and dist is the
  //! distance between the two centers, we consider the two
  //! circles to be identical if |Rarg - Rsol| and dist
  //! are less than or equal to the tolerance criterion given at
  //! the time of construction of this algorithm.
  //! OutOfRange is raised if Index is greater than the number of solutions.
  //! notDone is raised if the construction algorithm did not succeed.
  Standard_EXPORT Standard_Boolean IsTheSame2(const Standard_Integer Index) const;

protected:
private:
  Standard_Boolean        WellDone;
  TColgp_Array1OfCirc2d   cirsol;
  Standard_Integer        NbrSol;
  GccEnt_Array1OfPosition qualifier1;
  GccEnt_Array1OfPosition qualifier2;
  TColStd_Array1OfInteger TheSame1;
  TColStd_Array1OfInteger TheSame2;
  TColgp_Array1OfPnt2d    pnttg1sol;
  TColgp_Array1OfPnt2d    pnttg2sol;
  TColStd_Array1OfReal    par1sol;
  TColStd_Array1OfReal    par2sol;
  TColStd_Array1OfReal    pararg1;
  TColStd_Array1OfReal    pararg2;
  Standard_Boolean        Invert;
};

#endif // _Geom2dGcc_Circ2d2TanRad_HeaderFile
