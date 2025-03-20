// Created on: 1991-03-29
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

#ifndef _Geom2dGcc_Circ2d2TanRadGeo_HeaderFile
#define _Geom2dGcc_Circ2d2TanRadGeo_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Standard_Integer.hxx>
#include <TColgp_Array1OfCirc2d.hxx>
#include <GccEnt_Array1OfPosition.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <GccEnt_Position.hxx>
class GccEnt_QualifiedCirc;
class Geom2dGcc_QCurve;
class GccEnt_QualifiedLin;
class gp_Pnt2d;
class gp_Circ2d;

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
//! With Tolerance we will get a solution if the
//! lowest distance between C1 and Cu2 is lower than or
//! equal Tolerance.
class Geom2dGcc_Circ2d2TanRadGeo
{
public:
  DEFINE_STANDARD_ALLOC

  //! This method implements the algorithms used to
  //! create 2d circles TANgent to a 2d circle and a curve
  //! with a radius of Radius.
  //! It raises NegativeValue if Radius is lower than zero.
  Standard_EXPORT Geom2dGcc_Circ2d2TanRadGeo(const GccEnt_QualifiedCirc& Qualified1,
                                             const Geom2dGcc_QCurve&     Qualified2,
                                             const Standard_Real         Radius,
                                             const Standard_Real         Tolerance);

  //! This method implements the algorithms used to
  //! create 2d circles TANgent to a 2d line and a curve
  //! with a radius of Radius.
  //! It raises NegativeValue if Radius is lower than zero.
  Standard_EXPORT Geom2dGcc_Circ2d2TanRadGeo(const GccEnt_QualifiedLin& Qualified1,
                                             const Geom2dGcc_QCurve&    Qualified2,
                                             const Standard_Real        Radius,
                                             const Standard_Real        Tolerance);

  //! This method implements the algorithms used to
  //! create 2d circles TANgent to two curves with
  //! a radius of Radius.
  //! It raises NegativeValue if Radius is lower than zero.
  Standard_EXPORT Geom2dGcc_Circ2d2TanRadGeo(const Geom2dGcc_QCurve& Qualified1,
                                             const Geom2dGcc_QCurve& Qualified2,
                                             const Standard_Real     Radius,
                                             const Standard_Real     Tolerance);

  //! This method implements the algorithms used to
  //! create 2d circles TANgent to a curve and a point
  //! with a radius of Radius.
  //! It raises NegativeValue if Radius is lower than zero.
  Standard_EXPORT Geom2dGcc_Circ2d2TanRadGeo(const Geom2dGcc_QCurve& Qualified1,
                                             const gp_Pnt2d&         Point2,
                                             const Standard_Real     Radius,
                                             const Standard_Real     Tolerance);

  //! This method returns True if the algorithm succeeded.
  Standard_EXPORT Standard_Boolean IsDone() const;

  //! This method returns the number of solutions.
  //! It raises NotDone if the algorithm failed.
  Standard_EXPORT Standard_Integer NbSolutions() const;

  //! Returns the solution number Index.
  //! Be careful: the Index is only a way to get all the
  //! solutions, but is not associated to those outside the context
  //! of the algorithm-object.
  //! It raises OutOfRange exception if Index is greater
  //! than the number of solutions.
  //! It raises NotDone if the construction algorithm did not
  //! succeed.
  Standard_EXPORT gp_Circ2d ThisSolution(const Standard_Integer Index) const;

  //! It returns the information about the qualifiers of
  //! the tangency arguments concerning the solution number Index.
  //! It returns the real qualifiers (the qualifiers given to the
  //! constructor method in case of enclosed, enclosing and outside
  //! and the qualifiers computedin case of unqualified).
  Standard_EXPORT void WhichQualifier(const Standard_Integer Index,
                                      GccEnt_Position&       Qualif1,
                                      GccEnt_Position&       Qualif2) const;

  //! Returns information about the tangency point between the
  //! result number Index and the first argument.
  //! ParSol is the intrinsic parameter of the point PntSol on the solution.
  //! ParArg is the intrinsic parameter of the point PntSol on the first
  //! argument.
  //! It raises OutOfRange if Index is greater than the number
  //! of solutions.
  //! It raises NotDone if the construction algorithm did not
  //! succeed.
  Standard_EXPORT void Tangency1(const Standard_Integer Index,
                                 Standard_Real&         ParSol,
                                 Standard_Real&         ParArg,
                                 gp_Pnt2d&              PntSol) const;

  //! Returns information about the tangency point between the
  //! result number Index and the second argument.
  //! ParSol is the intrinsic parameter of the point PntSol on
  //! the solution.
  //! ParArg is the intrinsic parameter of the point PntArg on
  //! the second argument.
  //! It raises OutOfRange if Index is greater than the number
  //! of solutions.
  //! It raises NotDone if the construction algorithm did not
  //! succeed.
  Standard_EXPORT void Tangency2(const Standard_Integer Index,
                                 Standard_Real&         ParSol,
                                 Standard_Real&         ParArg,
                                 gp_Pnt2d&              PntSol) const;

  //! Returns True if the solution number Index is equal to
  //! the first argument.
  //! It raises OutOfRange if Index is greater than the number
  //! of solutions.
  //! It raises NotDone if the construction algorithm did not
  //! succeed.
  Standard_EXPORT Standard_Boolean IsTheSame1(const Standard_Integer Index) const;

  //! Returns True if the solution number Index is equal to
  //! the second argument.
  //! It raises OutOfRange if Index is greater than the number
  //! of solutions.
  //! It raises NotDone if the construction algorithm did not
  //! succeed.
  Standard_EXPORT Standard_Boolean IsTheSame2(const Standard_Integer Index) const;

protected:
private:
  Standard_Boolean        WellDone;
  Standard_Integer        NbrSol;
  TColgp_Array1OfCirc2d   cirsol;
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
};

#endif // _Geom2dGcc_Circ2d2TanRadGeo_HeaderFile
