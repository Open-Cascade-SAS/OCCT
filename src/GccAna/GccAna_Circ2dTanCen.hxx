// Created on: 1991-03-18
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

#ifndef _GccAna_Circ2dTanCen_HeaderFile
#define _GccAna_Circ2dTanCen_HeaderFile

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
class gp_Pnt2d;
class gp_Lin2d;
class gp_Circ2d;


//! This class implements the algorithms used to
//! create 2d circles tangent to an entity and
//! centered on a point.
//! The arguments of all construction methods are :
//! - The qualified element for the tangency constrains
//! (QualifiedCirc, Line, Point).
//! - The center point Pcenter.
//! - A real Tolerance.
//! Tolerance is only used in the limits cases.
//! For example :
//! We want to create a circle tangent to an EnclosedCirc C1
//! with a tolerance Tolerance.
//! If we did not used Tolerance it is impossible to
//! find a solution in the following case : Pcenter is
//! outside C1.
//! With Tolerance we will give a solution if the distance
//! between C1 and Pcenter is lower than or equal Tolerance.
class GccAna_Circ2dTanCen 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! This method implements the algorithms used to
  //! create 2d circles tangent to a circle and
  //! centered on a point.
  Standard_EXPORT GccAna_Circ2dTanCen(const GccEnt_QualifiedCirc& Qualified1, const gp_Pnt2d& Pcenter, const Standard_Real Tolerance);
  
  //! This method implements the algorithms used to
  //! create 2d circles tangent to a line and
  //! centered on a point.
  Standard_EXPORT GccAna_Circ2dTanCen(const gp_Lin2d& Linetan, const gp_Pnt2d& Pcenter);
  
  //! This method implements the algorithms used to
  //! create 2d circles passing through a point and
  //! centered on a point.
  //! Tolerance is a tolerance criterion used by the algorithm
  //! to find a solution when, mathematically, the problem
  //! posed does not have a solution, but where there is
  //! numeric uncertainty attached to the arguments.
  //! In these algorithms Tolerance is only used in very
  //! specific cases where the center of the solution is very
  //! close to the circle to which it is tangential, and where the
  //! solution is therefore a very small circle.
  //! Exceptions
  //! GccEnt_BadQualifier if a qualifier is inconsistent with
  //! the argument it qualifies (for example, enclosing for a line).
  Standard_EXPORT GccAna_Circ2dTanCen(const gp_Pnt2d& Point1, const gp_Pnt2d& Pcenter);
  
  //! This method returns True if the construction
  //! algorithm succeeded.
  //! Note: IsDone protects against a failure arising from a
  //! more internal intersection algorithm, which has reached
  //! its numeric limits.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns the number of circles, representing solutions
  //! computed by this algorithm and raises NotDone
  //! exception if the algorithm didn't succeed.
  Standard_EXPORT Standard_Integer NbSolutions() const;
  
  //! Returns the circle, representing the solution number Index and raises OutOfRange
  //! exception if Index is greater than the number of solutions.
  //! Be careful: the Index is only a way to get all the
  //! solutions, but is not associated to these outside the
  //! context of the algorithm-object.
  //! Raises NotDone if the construction algorithm didn't succeed.
  //! It raises OutOfRange if Index is greater than the
  //! number of solutions or less than zer
  Standard_EXPORT gp_Circ2d ThisSolution (const Standard_Integer Index) const;
  
  //! Returns the qualifier Qualif1 of the tangency argument
  //! for the solution of index Index computed by this algorithm.
  //! The returned qualifier is:
  //! -   that specified at the start of construction when the
  //! solutions are defined as enclosed, enclosing or
  //! It returns the real qualifiers (the qualifiers given to the
  //! constructor method in case of enclosed, enclosing and outside
  //! and the qualifiers computedin case of unqualified).
  Standard_EXPORT void WhichQualifier (const Standard_Integer Index, GccEnt_Position& Qualif1) const;
  
  //! Returns information about the tangency point between the
  //! result number Index and the first argument.
  //! ParSol is the intrinsic parameter of the point PntSol
  //! on the solution curv.
  //! ParArg is the intrinsic parameter of the point PntArg
  //! on the argument curv.
  //! It raises NotDone if the construction algorithm
  //! didn't succeed.
  //! It raises OutOfRange if Index is greater than the
  //! number of solutions or less than zero.
  Standard_EXPORT void Tangency1 (const Standard_Integer Index, Standard_Real& ParSol, Standard_Real& ParArg, gp_Pnt2d& PntSol) const;
  
  //! Returns True if the solution number Index is equal to
  //! the first argument.
  //! It raises NotDone if the construction algorithm
  //! didn't succeed.
  //! It raises OutOfRange if Index is greater than the
  //! number of solutions or less than zero.
  Standard_EXPORT Standard_Boolean IsTheSame1 (const Standard_Integer Index) const;




protected:





private:



  Standard_Boolean WellDone;
  Standard_Integer NbrSol;
  TColgp_Array1OfCirc2d cirsol;
  GccEnt_Array1OfPosition qualifier1;
  TColStd_Array1OfInteger TheSame1;
  TColgp_Array1OfPnt2d pnttg1sol;
  TColStd_Array1OfReal par1sol;
  TColStd_Array1OfReal pararg1;


};







#endif // _GccAna_Circ2dTanCen_HeaderFile
