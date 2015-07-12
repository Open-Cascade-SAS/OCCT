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

#include <Standard_Boolean.hxx>
#include <TColgp_Array1OfCirc2d.hxx>
#include <Standard_Integer.hxx>
#include <GccEnt_Array1OfPosition.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <Standard_Real.hxx>
#include <GccEnt_Position.hxx>
class StdFail_NotDone;
class GccEnt_BadQualifier;
class Standard_OutOfRange;
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
  Standard_EXPORT Geom2dGcc_Circ2d2TanOn(const Geom2dGcc_QualifiedCurve& Qualified1, const Geom2dGcc_QualifiedCurve& Qualified2, const Geom2dAdaptor_Curve& OnCurve, const Standard_Real Tolerance, const Standard_Real Param1, const Standard_Real Param2, const Standard_Real ParamOn);
  
  //! This method implements the algorithms used to
  //! create 2d circles TANgent to one curve and one point and
  //! having the center ON a 2d curve.
  //! Param1 is the initial guess on the first curve QualifiedCurv.
  //! ParamOn is the initial guess on the center curve OnCurv.
  //! Tolerance is used for the limit cases.
  Standard_EXPORT Geom2dGcc_Circ2d2TanOn(const Geom2dGcc_QualifiedCurve& Qualified1, const Handle(Geom2d_Point)& Point, const Geom2dAdaptor_Curve& OnCurve, const Standard_Real Tolerance, const Standard_Real Param1, const Standard_Real ParamOn);
  
  //! This method implements the algorithms used to
  //! create 2d circles TANgent to two points and
  //! having the center ON a 2d curve.
  //! Tolerance is used for the limit cases.
  Standard_EXPORT Geom2dGcc_Circ2d2TanOn(const Handle(Geom2d_Point)& Point1, const Handle(Geom2d_Point)& Point2, const Geom2dAdaptor_Curve& OnCurve, const Standard_Real Tolerance);
  
  Standard_EXPORT void Results (const GccAna_Circ2d2TanOn& Circ);
  
  Standard_EXPORT void Results (const Geom2dGcc_Circ2d2TanOnGeo& Circ);
  
  //! Returns true if the construction algorithm does not fail
  //! (even if it finds no solution).
  //! Note: IsDone protects against a failure arising from a
  //! more internal intersection algorithm, which has
  //! reached its numeric limits.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! This method returns the number of solutions.
  //! NotDone is raised if the algorithm failed.
  Standard_EXPORT Standard_Integer NbSolutions() const;
  
  //! Returns the solution number Index and raises OutOfRange
  //! exception if Index is greater than the number of solutions.
  //! Be carefull: the Index is only a way to get all the
  //! solutions, but is not associated to theses outside the context
  //! of the algorithm-object.
  //! Exceptions
  //! Standard_OutOfRange if Index is less than or equal
  //! to zero or greater than the number of solutions
  //! computed by this algorithm.
  //! StdFail_NotDone if the construction fails.
  Standard_EXPORT gp_Circ2d ThisSolution (const Standard_Integer Index) const;
  
  //! It returns the informations about the qualifiers of
  //! the tangency
  //! arguments concerning the solution number Index.
  //! It returns the real qualifiers (the qualifiers given to the
  //! constructor method in case of enclosed, enclosing and outside
  //! and the qualifiers computedin case of unqualified).
  //! Exceptions
  //! Standard_OutOfRange if Index is less than zero or
  //! greater than the number of solutions computed by this algorithm.
  //! StdFail_NotDone if the construction fails.
  Standard_EXPORT void WhichQualifier (const Standard_Integer Index, GccEnt_Position& Qualif1, GccEnt_Position& Qualif2) const;
  
  //! Returns informations about the tangency point between the
  //! result and the first argument.
  //! ParSol is the intrinsic parameter of the point PntSol on the solution curv.
  //! ParArg is the intrinsic parameter of the point PntSol on the argument curv.
  Standard_EXPORT void Tangency1 (const Standard_Integer Index, Standard_Real& ParSol, Standard_Real& ParArg, gp_Pnt2d& PntSol) const;
  
  //! Returns informations about the tangency point between the
  //! result and the second argument.
  //! ParSol is the intrinsic parameter of the point PntSol on the solution curv.
  //! ParArg is the intrinsic parameter of the point PntSol on the argument curv.
  Standard_EXPORT void Tangency2 (const Standard_Integer Index, Standard_Real& ParSol, Standard_Real& ParArg, gp_Pnt2d& PntSol) const;
  
  //! Returns the center PntSol of the solution of index Index
  //! computed by this algorithm.
  //! ParArg is the parameter of the point PntSol on the third argument.
  //! Exceptions
  //! Standard_OutOfRange if Index is less than zero or
  //! greater than the number of solutions computed by this algorithm.
  //! StdFail_NotDone if the construction fails.
  Standard_EXPORT void CenterOn3 (const Standard_Integer Index, Standard_Real& ParArg, gp_Pnt2d& PntSol) const;
  
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
  Standard_EXPORT Standard_Boolean IsTheSame1 (const Standard_Integer Index) const;
  
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
  Standard_EXPORT Standard_Boolean IsTheSame2 (const Standard_Integer Index) const;




protected:





private:



  Standard_Boolean WellDone;
  TColgp_Array1OfCirc2d cirsol;
  Standard_Integer NbrSol;
  GccEnt_Array1OfPosition qualifier1;
  GccEnt_Array1OfPosition qualifier2;
  TColStd_Array1OfInteger TheSame1;
  TColStd_Array1OfInteger TheSame2;
  TColgp_Array1OfPnt2d pnttg1sol;
  TColgp_Array1OfPnt2d pnttg2sol;
  TColgp_Array1OfPnt2d pntcen;
  TColStd_Array1OfReal par1sol;
  TColStd_Array1OfReal par2sol;
  TColStd_Array1OfReal pararg1;
  TColStd_Array1OfReal pararg2;
  TColStd_Array1OfReal parcen3;
  Standard_Boolean Invert;


};







#endif // _Geom2dGcc_Circ2d2TanOn_HeaderFile
