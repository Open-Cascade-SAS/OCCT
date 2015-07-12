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

#ifndef _Geom2dGcc_Circ2dTanCen_HeaderFile
#define _Geom2dGcc_Circ2dTanCen_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <TColgp_Array1OfCirc2d.hxx>
#include <GccEnt_Array1OfPosition.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <Standard_Real.hxx>
#include <GccEnt_Position.hxx>
class Standard_OutOfRange;
class GccEnt_BadQualifier;
class StdFail_NotDone;
class Geom2dGcc_QualifiedCurve;
class Geom2d_Point;
class gp_Circ2d;
class gp_Pnt2d;


//! This class implements the algorithms used to
//! create 2d circles tangent to a curve and
//! centered on a point.
//! The arguments of all construction methods are :
//! - The qualified element for the tangency constrains
//! (QualifiedCurv).
//! -The center point Pcenter.
//! - A real Tolerance.
//! Tolerance is only used in the limits cases.
//! For example :
//! We want to create a circle tangent to an EnclosedCurv C1
//! with a tolerance Tolerance.
//! If we did not used Tolerance it is impossible to
//! find a solution in the the following case : Pcenter is
//! outside C1.
//! With Tolerance we will give a solution if the distance
//! between C1 and Pcenter is lower than or equal Tolerance/2.
class Geom2dGcc_Circ2dTanCen 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs one or more 2D circles tangential to the
  //! curve Qualified1 and centered on the point Pcenter.
  //! Tolerance is a tolerance criterion used by the algorithm
  //! to find a solution when, mathematically, the problem
  //! posed does not have a solution, but where there is
  //! numeric uncertainty attached to the arguments.
  //! Tolerance is only used in these algorithms in very
  //! specific cases where the center of the solution is very
  //! close to the circle to which it is tangential, and where the
  //! solution is thus a very small circle.
  //! Exceptions
  //! GccEnt_BadQualifier if a qualifier is inconsistent with
  //! the argument it qualifies (for example, enclosing for a line).
  Standard_EXPORT Geom2dGcc_Circ2dTanCen(const Geom2dGcc_QualifiedCurve& Qualified1, const Handle(Geom2d_Point)& Pcenter, const Standard_Real Tolerance);
  
  //! Returns true if the construction algorithm does not fail
  //! (even if it finds no solution).
  //! Note: IsDone protects against a failure arising from a
  //! more internal intersection algorithm, which has reached
  //! its numeric limits.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns the number of circles, representing solutions
  //! computed by this algorithm.
  //! Exceptions
  //! StdFail_NotDone if the construction fails.
  Standard_EXPORT Standard_Integer NbSolutions() const;
  
  //! Returns a circle, representing the solution of index
  //! Index computed by this algorithm.
  //! Warning
  //! This indexing simply provides a means of consulting the
  //! solutions. The index values are not associated with
  //! these solutions outside the context of the algorithm object.
  //! Exceptions
  //! Standard_OutOfRange if Index is less than zero or
  //! greater than the number of solutions computed by this algorithm.
  //! StdFail_NotDone if the construction fails
  Standard_EXPORT gp_Circ2d ThisSolution (const Standard_Integer Index) const;
  
  //! Returns the qualifier Qualif1 of the tangency argument
  //! for the solution of index Index computed by this algorithm.
  //! The returned qualifier is:
  //! -   that specified at the start of construction when the
  //! solutions are defined as enclosed, enclosing or
  //! outside with respect to the argument, or
  //! -   that computed during construction (i.e. enclosed,
  //! enclosing or outside) when the solutions are defined
  //! as unqualified with respect to the argument.
  //! Exceptions
  //! Standard_OutOfRange if Index is less than zero or
  //! greater than the number of solutions computed by this algorithm.
  //! StdFail_NotDone if the construction fails.
  Standard_EXPORT void WhichQualifier (const Standard_Integer Index, GccEnt_Position& Qualif1) const;
  
  //! Returns informations about the tangency point between the
  //! result number Index and the first argument.
  //! ParSol is the intrinsic parameter of the point PntSol on the solution curv.
  //! ParArg is the intrinsic parameter of the point PntSol on the argument curv.
  //! Exceptions
  //! Standard_OutOfRange if Index is less than zero or
  //! greater than the number of solutions computed by this algorithm.
  //! StdFail_NotDone if the construction fails.
  Standard_EXPORT void Tangency1 (const Standard_Integer Index, Standard_Real& ParSol, Standard_Real& ParArg, gp_Pnt2d& PntSol) const;
  
  //! Returns true if the solution of index Index and the first
  //! argument of this algorithm are the same (i.e. there are 2
  //! identical circles).
  //! If Rarg is the radius of the first argument, Rsol is the
  //! radius of the solution and dist is the distance between
  //! the two centers, we consider the two circles to be
  //! identical if |Rarg - Rsol| and dist are less than
  //! or equal to the tolerance criterion given at the time of
  //! construction of this algorithm.
  //! NotDone is raised if the construction algorithm didn't succeed.
  //! OutOfRange is raised if Index is greater than the
  //! number of solutions.
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







#endif // _Geom2dGcc_Circ2dTanCen_HeaderFile
