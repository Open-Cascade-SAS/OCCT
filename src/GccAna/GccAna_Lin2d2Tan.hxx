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

#ifndef _GccAna_Lin2d2Tan_HeaderFile
#define _GccAna_Lin2d2Tan_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Standard_Integer.hxx>
#include <TColgp_Array1OfLin2d.hxx>
#include <GccEnt_Array1OfPosition.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <GccEnt_Position.hxx>
class gp_Pnt2d;
class GccEnt_QualifiedCirc;
class gp_Lin2d;


//! This class implements the algorithms used to
//! create 2d lines tangent to 2 other elements which
//! can be circles or points.
//! Describes functions for building a 2D line:
//! -   tangential to 2 circles, or
//! -   tangential to a circle and passing through a point, or
//! -   passing through 2 points.
//! A Lin2d2Tan object provides a framework for:
//! -   defining the construction of 2D line(s),
//! -   implementing the construction algorithm, and
//! consulting the result(s).
//! Some constructors may check the type of the qualified argument
//! and raise BadQualifier Error in case of incorrect couple (qualifier,
//! curv).
//! For example: "EnclosedCirc".
class GccAna_Lin2d2Tan 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! This methods implements the algorithms used to
  //! create 2d lines passing through 2 points.
  //! Tolerance is used because we can't create a line
  //! when the distance between the two points is too small.
  Standard_EXPORT GccAna_Lin2d2Tan(const gp_Pnt2d& ThePoint1, const gp_Pnt2d& ThePoint2, const Standard_Real Tolerance);
  
  //! This methods implements the algorithms used to
  //! create 2d lines tangent to one circle and passing
  //! through a point.
  //! Exception BadQualifier is raised in the case of
  //! EnclosedCirc
  //! Tolerance is used because there is no solution
  //! when the point is inside the solution according to
  //! the tolerance.
  Standard_EXPORT GccAna_Lin2d2Tan(const GccEnt_QualifiedCirc& Qualified1, const gp_Pnt2d& ThePoint, const Standard_Real Tolerance);
  
  //! This methods implements the algorithms used to
  //! create 2d lines tangent to 2 circles.
  //! Exception BadQualifier is raised in the case of
  //! EnclosedCirc
  Standard_EXPORT GccAna_Lin2d2Tan(const GccEnt_QualifiedCirc& Qualified1, const GccEnt_QualifiedCirc& Qualified2, const Standard_Real Tolerance);
  
  //! This method returns true when there is a solution
  //! and false in the other cases.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! This method returns the number of solutions.
  //! Raises NotDone if the construction algorithm didn't succeed.
  Standard_EXPORT Standard_Integer NbSolutions() const;
  
  //! Returns the solution number Index and raises OutOfRange
  //! exception if Index is greater than the number of solutions.
  //! Be careful: the Index is only a way to get all the
  //! solutions, but is not associated to these outside the
  //! context of the algorithm-object. Raises OutOfRange is raised if Index is greater than
  //! the number of solutions.
  //! It raises NotDone if the algorithm failed.
  Standard_EXPORT gp_Lin2d ThisSolution (const Standard_Integer Index) const;
  
  //! Returns the qualifiers Qualif1 and Qualif2 of the
  //! tangency arguments for the solution of index Index
  //! computed by this algorithm.
  //! The returned qualifiers are:
  //! -   those specified at the start of construction when the
  //! solutions are defined as enclosing or outside with
  //! respect to the arguments, or
  //! -   those computed during construction (i.e. enclosing or
  //! outside) when the solutions are defined as unqualified
  //! with respect to the arguments, or
  //! -   GccEnt_noqualifier if the tangency argument is a point.
  //! Exceptions
  //! Standard_OutOfRange if Index is less than zero or
  //! greater than the number of solutions computed by this algorithm.
  //! StdFail_NotDone if the construction fails.
  Standard_EXPORT void WhichQualifier (const Standard_Integer Index, GccEnt_Position& Qualif1, GccEnt_Position& Qualif2) const;
  
  //! Returns information about the tangency point between the
  //! result number Index and the first argument.
  //! ParSol is the intrinsic parameter of the point PntSol on
  //! the solution curv.
  //! ParArg is the intrinsic parameter of the point PntSol on
  //! the argument curv. Raises OutOfRange is raised if Index is greater than
  //! the number of solutions.
  //! It raises NotDone if the algorithm failed.
  Standard_EXPORT void Tangency1 (const Standard_Integer Index, Standard_Real& ParSol, Standard_Real& ParArg, gp_Pnt2d& PntSol) const;
  
  //! Returns information about the tangency point between the
  //! result number Index and the second argument.
  //! ParSol is the intrinsic parameter of the point ParSol on
  //! the solution curv.
  //! ParArg is the intrinsic parameter of the point PntSol on
  //! the argument curv. Raises OutOfRange is raised if Index is greater than
  //! the number of solutions.
  //! It raises NotDone if the algorithm failed.
  Standard_EXPORT void Tangency2 (const Standard_Integer Index, Standard_Real& ParSol, Standard_Real& ParArg, gp_Pnt2d& PntSol) const;




protected:





private:



  Standard_Boolean WellDone;
  Standard_Integer NbrSol;
  TColgp_Array1OfLin2d linsol;
  GccEnt_Array1OfPosition qualifier1;
  GccEnt_Array1OfPosition qualifier2;
  TColgp_Array1OfPnt2d pnttg1sol;
  TColgp_Array1OfPnt2d pnttg2sol;
  TColStd_Array1OfReal par1sol;
  TColStd_Array1OfReal par2sol;
  TColStd_Array1OfReal pararg1;
  TColStd_Array1OfReal pararg2;


};







#endif // _GccAna_Lin2d2Tan_HeaderFile
