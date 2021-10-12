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

#ifndef _GccAna_Lin2dTanPer_HeaderFile
#define _GccAna_Lin2dTanPer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <TColgp_Array1OfLin2d.hxx>
#include <GccEnt_Array1OfPosition.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <GccEnt_Position.hxx>
#include <Standard_Real.hxx>
class gp_Pnt2d;
class gp_Lin2d;
class gp_Circ2d;
class GccEnt_QualifiedCirc;


//! This class implements the algorithms used to
//! create 2d lines tangent to a circle or a point and
//! perpendicular to a line or a circle.
//! Describes functions for building a 2D line perpendicular
//! to a line and:
//! -   tangential to a circle, or
//! -   passing through a point.
//! A Lin2dTanPer object provides a framework for:
//! -   defining the construction of 2D line(s),
//! -   implementing the construction algorithm, and
//! -   consulting the result(s).
class GccAna_Lin2dTanPer 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! This method implements the algorithms used to
  //! create 2d lines passing through a point and
  //! perpendicular to a line.
  Standard_EXPORT GccAna_Lin2dTanPer(const gp_Pnt2d& ThePnt, const gp_Lin2d& TheLin);
  
  //! This method implements the algorithms used to
  //! create 2d lines passing through a point and
  //! perpendicular to a circle.
  Standard_EXPORT GccAna_Lin2dTanPer(const gp_Pnt2d& ThePnt, const gp_Circ2d& TheCircle);
  
  //! This method implements the algorithms used to
  //! create 2d lines tangent to a circle and
  //! perpendicular to a line.
  Standard_EXPORT GccAna_Lin2dTanPer(const GccEnt_QualifiedCirc& Qualified1, const gp_Lin2d& TheLin);
  
  //! This method implements the algorithms used to
  //! create 2d lines tangent to a circle and
  //! perpendicular to a circle.
  Standard_EXPORT GccAna_Lin2dTanPer(const GccEnt_QualifiedCirc& Qualified1, const gp_Circ2d& TheCircle);
  
  //! Returns True if the algorithm succeeded.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns the number of solutions.
  //! Raises NotDone if the construction algorithm didn't succeed.
  Standard_EXPORT Standard_Integer NbSolutions() const;
  
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
  Standard_EXPORT void WhichQualifier (const Standard_Integer Index, GccEnt_Position& Qualif1) const;
  
  //! Returns the solution number Index and raises OutOfRange
  //! exception if Index is greater than the number of solutions.
  //! Be careful: the Index is only a way to get all the
  //! solutions, but is not associated to those outside the
  //! context of the algorithm-object.
  //! Raises NotDone if the construction algorithm
  //! didn't succeed.
  //! It raises OutOfRange if Index is greater than the
  //! number of solutions.
  Standard_EXPORT gp_Lin2d ThisSolution (const Standard_Integer Index) const;
  
  //! Returns information about the tangency point between the
  //! result number Index and the first argument.
  //! ParSol is the intrinsic parameter of the point on the
  //! solution curv.
  //! ParArg is the intrinsic parameter of the point on the
  //! argument curv.
  //! If the first argument is a point ParArg is equal zero.
  //! raises NotDone if the construction algorithm didn't succeed.
  //! It raises OutOfRange if Index is greater than the
  //! number of solutions.
  Standard_EXPORT void Tangency1 (const Standard_Integer Index, Standard_Real& ParSol, Standard_Real& ParArg, gp_Pnt2d& Pnt) const;
  
  //! Returns information about the intersection between the
  //! solution number Index and the second argument.
  //! It returns the first intersection in a case of
  //! Lin2dTanPer which is perpendicular to a circle .
  //! ParSol is the intrinsic parameter of the point on the
  //! solution curv.
  //! ParArg is the intrinsic parameter of the point on the
  //! argument curv. Raises NotDone if the construction algorithm
  //! didn't succeed.
  //! It raises OutOfRange if Index is greater than the
  //! number of solutions.
  Standard_EXPORT void Intersection2 (const Standard_Integer Index, Standard_Real& ParSol, Standard_Real& ParArg, gp_Pnt2d& PntSol) const;




protected:





private:



  Standard_Boolean WellDone;
  Standard_Integer NbrSol;
  TColgp_Array1OfLin2d linsol;
  GccEnt_Array1OfPosition qualifier1;
  TColgp_Array1OfPnt2d pnttg1sol;
  TColgp_Array1OfPnt2d pntint2sol;
  TColStd_Array1OfReal par1sol;
  TColStd_Array1OfReal par2sol;
  TColStd_Array1OfReal pararg1;
  TColStd_Array1OfReal pararg2;


};







#endif // _GccAna_Lin2dTanPer_HeaderFile
