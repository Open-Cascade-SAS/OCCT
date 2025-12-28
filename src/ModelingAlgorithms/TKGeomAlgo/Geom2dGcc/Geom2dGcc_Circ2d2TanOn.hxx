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

#ifndef _Geom2dGcc_Circ2d2TanOn_HeaderFile
#define _Geom2dGcc_Circ2d2TanOn_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Circ2d.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <GccEnt_Position.hxx>
#include <gp_Pnt2d.hxx>
class Geom2dGcc_QualifiedCurve;
class Geom2dAdaptor_Curve;
class Geom2d_Point;
class GccAna_Circ2d2TanOn;
class Geom2dGcc_Circ2d2TanOnGeo;
class gp_Circ2d;
class gp_Pnt2d;

//! This class implements the algorithms used to
//! create 2d circles TANgent to 2 entities and
//! having the center ON a curve.
//! The order of the tangency argument is always
//! QualifiedCirc, QualifiedLin, QualifiedCurv, Pnt2d.
//! the arguments are :
//! - The two tangency arguments.
//! - The center line.
//! - The parameter for each tangency argument which
//! is a curve.
//! - The tolerance.
class Geom2dGcc_Circ2d2TanOn
{
public:
  DEFINE_STANDARD_ALLOC

  //! This method implements the algorithms used to
  //! create 2d circles TANgent to two curves and
  //! having the center ON a 2d curve.
  //! Param1 is the initial guess on the first curve QualifiedCurv.
  //! Param1 is the initial guess on the second curve QualifiedCurv.
  //! ParamOn is the initial guess on the center curve OnCurv.
  //! Tolerance is used for the limit cases.
  Standard_EXPORT Geom2dGcc_Circ2d2TanOn(const Geom2dGcc_QualifiedCurve& Qualified1,
                                         const Geom2dGcc_QualifiedCurve& Qualified2,
                                         const Geom2dAdaptor_Curve&      OnCurve,
                                         const double                    Tolerance,
                                         const double                    Param1,
                                         const double                    Param2,
                                         const double                    ParamOn);

  //! This method implements the algorithms used to
  //! create 2d circles TANgent to one curve and one point and
  //! having the center ON a 2d curve.
  //! Param1 is the initial guess on the first curve QualifiedCurv.
  //! ParamOn is the initial guess on the center curve OnCurv.
  //! Tolerance is used for the limit cases.
  Standard_EXPORT Geom2dGcc_Circ2d2TanOn(const Geom2dGcc_QualifiedCurve&  Qualified1,
                                         const occ::handle<Geom2d_Point>& Point,
                                         const Geom2dAdaptor_Curve&       OnCurve,
                                         const double                     Tolerance,
                                         const double                     Param1,
                                         const double                     ParamOn);

  //! This method implements the algorithms used to
  //! create 2d circles TANgent to two points and
  //! having the center ON a 2d curve.
  //! Tolerance is used for the limit cases.
  Standard_EXPORT Geom2dGcc_Circ2d2TanOn(const occ::handle<Geom2d_Point>& Point1,
                                         const occ::handle<Geom2d_Point>& Point2,
                                         const Geom2dAdaptor_Curve&       OnCurve,
                                         const double                     Tolerance);

  Standard_EXPORT void Results(const GccAna_Circ2d2TanOn& Circ);

  Standard_EXPORT void Results(const Geom2dGcc_Circ2d2TanOnGeo& Circ);

  //! Returns true if the construction algorithm does not fail
  //! (even if it finds no solution).
  //! Note: IsDone protects against a failure arising from a
  //! more internal intersection algorithm, which has
  //! reached its numeric limits.
  Standard_EXPORT bool IsDone() const;

  //! This method returns the number of solutions.
  //! NotDone is raised if the algorithm failed.
  Standard_EXPORT int NbSolutions() const;

  //! Returns the solution number Index and raises OutOfRange
  //! exception if Index is greater than the number of solutions.
  //! Be careful: the Index is only a way to get all the
  //! solutions, but is not associated to these outside the context
  //! of the algorithm-object.
  //! Exceptions
  //! Standard_OutOfRange if Index is less than or equal
  //! to zero or greater than the number of solutions
  //! computed by this algorithm.
  //! StdFail_NotDone if the construction fails.
  Standard_EXPORT gp_Circ2d ThisSolution(const int Index) const;

  //! It returns the information about the qualifiers of
  //! the tangency
  //! arguments concerning the solution number Index.
  //! It returns the real qualifiers (the qualifiers given to the
  //! constructor method in case of enclosed, enclosing and outside
  //! and the qualifiers computedin case of unqualified).
  //! Exceptions
  //! Standard_OutOfRange if Index is less than zero or
  //! greater than the number of solutions computed by this algorithm.
  //! StdFail_NotDone if the construction fails.
  Standard_EXPORT void WhichQualifier(const int        Index,
                                      GccEnt_Position& Qualif1,
                                      GccEnt_Position& Qualif2) const;

  //! Returns information about the tangency point between the
  //! result and the first argument.
  //! ParSol is the intrinsic parameter of the point PntSol on the solution curv.
  //! ParArg is the intrinsic parameter of the point PntSol on the argument curv.
  Standard_EXPORT void Tangency1(const int Index,
                                 double&   ParSol,
                                 double&   ParArg,
                                 gp_Pnt2d& PntSol) const;

  //! Returns information about the tangency point between the
  //! result and the second argument.
  //! ParSol is the intrinsic parameter of the point PntSol on the solution curv.
  //! ParArg is the intrinsic parameter of the point PntSol on the argument curv.
  Standard_EXPORT void Tangency2(const int Index,
                                 double&   ParSol,
                                 double&   ParArg,
                                 gp_Pnt2d& PntSol) const;

  //! Returns the center PntSol of the solution of index Index
  //! computed by this algorithm.
  //! ParArg is the parameter of the point PntSol on the third argument.
  //! Exceptions
  //! Standard_OutOfRange if Index is less than zero or
  //! greater than the number of solutions computed by this algorithm.
  //! StdFail_NotDone if the construction fails.
  Standard_EXPORT void CenterOn3(const int Index, double& ParArg, gp_Pnt2d& PntSol) const;

  //! Returns true if the solution of index Index and,
  //! respectively, the first or second argument of this
  //! algorithm are the same (i.e. there are 2 identical circles).
  //! If Rarg is the radius of the first or second argument,
  //! Rsol is the radius of the solution and dist is the
  //! distance between the two centers, we consider the two
  //! circles to be identical if |Rarg - Rsol| and dist
  //! are less than or equal to the tolerance criterion given at
  //! the time of construction of this algorithm.
  //! Exceptions
  //! Standard_OutOfRange if Index is less than zero or
  //! greater than the number of solutions computed by this algorithm.
  //! StdFail_NotDone if the construction fails.
  Standard_EXPORT bool IsTheSame1(const int Index) const;

  //! Returns true if the solution of index Index and,
  //! respectively, the first or second argument of this
  //! algorithm are the same (i.e. there are 2 identical circles).
  //! If Rarg is the radius of the first or second argument,
  //! Rsol is the radius of the solution and dist is the
  //! distance between the two centers, we consider the two
  //! circles to be identical if |Rarg - Rsol| and dist
  //! are less than or equal to the tolerance criterion given at
  //! the time of construction of this algorithm.
  //! Exceptions
  //! Standard_OutOfRange if Index is less than zero or
  //! greater than the number of solutions computed by this algorithm.
  //! StdFail_NotDone if the construction fails.
  Standard_EXPORT bool IsTheSame2(const int Index) const;

private:
  bool                                WellDone;
  NCollection_Array1<gp_Circ2d>       cirsol;
  int                                 NbrSol;
  NCollection_Array1<GccEnt_Position> qualifier1;
  NCollection_Array1<GccEnt_Position> qualifier2;
  NCollection_Array1<int>             TheSame1;
  NCollection_Array1<int>             TheSame2;
  NCollection_Array1<gp_Pnt2d>        pnttg1sol;
  NCollection_Array1<gp_Pnt2d>        pnttg2sol;
  NCollection_Array1<gp_Pnt2d>        pntcen;
  NCollection_Array1<double>          par1sol;
  NCollection_Array1<double>          par2sol;
  NCollection_Array1<double>          pararg1;
  NCollection_Array1<double>          pararg2;
  NCollection_Array1<double>          parcen3;
  bool                                Invert;
};

#endif // _Geom2dGcc_Circ2d2TanOn_HeaderFile
