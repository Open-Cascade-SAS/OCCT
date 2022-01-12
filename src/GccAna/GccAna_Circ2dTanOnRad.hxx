// Created on: 1991-03-22
// Created by: Philippe DAUTRY
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

#ifndef _GccAna_Circ2dTanOnRad_HeaderFile
#define _GccAna_Circ2dTanOnRad_HeaderFile

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
class gp_Lin2d;
class GccEnt_QualifiedLin;
class gp_Pnt2d;
class gp_Circ2d;


//! This class implements the algorithms used to
//! create a 2d circle tangent to a 2d entity,
//! centered on a curv and with a given radius.
//! The arguments of all construction methods are :
//! - The qualified element for the tangency constrains
//! (QualifiedCirc, QualifiedLin, Points).
//! - The Center element (circle, line).
//! - A real Tolerance.
//! Tolerance is only used in the limits cases.
//! For example :
//! We want to create a circle tangent to an OutsideCirc C1
//! centered on a line OnLine with a radius Radius and with
//! a tolerance Tolerance.
//! If we did not use Tolerance it is impossible to
//! find a solution in the following case : OnLine is
//! outside C1. There is no intersection point between C1
//! and OnLine. The distance between the line and the
//! circle is greater than Radius.
//! With Tolerance we will give a solution if the
//! distance between C1 and OnLine is lower than or
//! equal Tolerance.
class GccAna_Circ2dTanOnRad 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! This methods implements the algorithms used to create
  //! 2d Circles tangent to a circle and centered on a 2d Line
  //! with a given radius.
  //! Tolerance is used to find solution in every limit cases.
  //! For example Tolerance is used in the case of EnclosedCirc when
  //! Radius-R1+dist is greater Tolerance (dist is the distance
  //! between the line and the location of the circ, R1 is the
  //! radius of the circ) because there is no solution.
  //! raises NegativeValue in case of NegativeRadius.
  Standard_EXPORT GccAna_Circ2dTanOnRad(const GccEnt_QualifiedCirc& Qualified1, const gp_Lin2d& OnLine, const Standard_Real Radius, const Standard_Real Tolerance);
  
  //! This methods implements the algorithms used to create
  //! 2d Circles tangent to a 2d Line and centered on a 2d Line
  //! with a given radius.
  //! Tolerance is used to find solution in every limit cases.
  //! raises NegativeValue in case of NegativeRadius.
  Standard_EXPORT GccAna_Circ2dTanOnRad(const GccEnt_QualifiedLin& Qualified1, const gp_Lin2d& OnLine, const Standard_Real Radius, const Standard_Real Tolerance);
  
  //! This methods implements the algorithms used to create
  //! 2d Circles passing through a 2d Point and centered on a
  //! 2d Line with a given radius.
  //! Tolerance is used to find solution in every limit cases.
  Standard_EXPORT GccAna_Circ2dTanOnRad(const gp_Pnt2d& Point1, const gp_Lin2d& OnLine, const Standard_Real Radius, const Standard_Real Tolerance);
  
  //! This methods implements the algorithms used to create
  //! 2d Circles tangent to a circle and centered on a 2d Circle
  //! with a given radius.
  //! Tolerance is used to find solution in every limit cases.
  //! raises NegativeValue in case of NegativeRadius.
  Standard_EXPORT GccAna_Circ2dTanOnRad(const GccEnt_QualifiedCirc& Qualified1, const gp_Circ2d& OnCirc, const Standard_Real Radius, const Standard_Real Tolerance);
  
  //! This methods implements the algorithms used to create
  //! 2d Circles tangent to a 2d Line and centered on a 2d Line
  //! with a given radius.
  //! Tolerance is used to find solution in every limit cases.
  //! raises NegativeValue in case of NegativeRadius.
  Standard_EXPORT GccAna_Circ2dTanOnRad(const GccEnt_QualifiedLin& Qualified1, const gp_Circ2d& OnCirc, const Standard_Real Radius, const Standard_Real Tolerance);
  
  //! This methods implements the algorithms used to create
  //! 2d Circles passing through a 2d Point and centered on a
  //! 2d Line with a given radius.
  //! Tolerance is used to find solution in every limit cases.
  //! raises NegativeValue in case of NegativeRadius.
  Standard_EXPORT GccAna_Circ2dTanOnRad(const gp_Pnt2d& Point1, const gp_Circ2d& OnCirc, const Standard_Real Radius, const Standard_Real Tolerance);
  
  //! Returns true if the construction algorithm does not fail
  //! (even if it finds no solution).
  //! Note: IsDone protects against a failure arising from a
  //! more internal intersection algorithm, which has
  //! reached its numeric limits.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! This method returns the number of circles, representing solutions.
  //! Raises NotDone if the construction algorithm didn't succeed.
  Standard_EXPORT Standard_Integer NbSolutions() const;
  
  //! Returns the solution number Index and raises OutOfRange
  //! exception if Index is greater than the number of solutions.
  //! Be careful: the Index is only a way to get all the
  //! solutions, but is not associated to these outside the
  //! context of the algorithm-object.
  //! Raises NotDone if the construction algorithm  didn't succeed.
  //! It raises OutOfRange if Index is greater than the
  //! number of solutions
  Standard_EXPORT gp_Circ2d ThisSolution (const Standard_Integer Index) const;
  
  //! Returns the qualifier Qualif1 of the tangency argument
  //! for the solution of index Index computed by this algorithm.
  //! The returned qualifier is:
  //! -   that specified at the start of construction when the
  //! solutions are defined as enclosed, enclosing or
  //! outside with respect to the argument, or
  //! -   that computed during construction (i.e. enclosed,
  //! enclosing or outside) when the solutions are defined
  //! as unqualified with respect to the argument, or
  //! -   GccEnt_noqualifier if the tangency argument is a point.
  //! Exceptions
  //! Standard_OutOfRange if Index is less than zero or
  //! greater than the number of solutions computed by this algorithm.
  //! StdFail_NotDone if the construction fails.
  Standard_EXPORT void WhichQualifier (const Standard_Integer Index, GccEnt_Position& Qualif1) const;
  
  //! Returns information about the tangency point between the
  //! result number Index and the first argument.
  //! ParSol is the intrinsic parameter of the point on the
  //! solution curv.
  //! ParArg is the intrinsic parameter of the point on the
  //! argument curv.
  //! PntSol is the tangency point on the solution curv.
  //! PntArg is the tangency point on the argument curv.
  //! Raises NotDone if the construction algorithm didn't succeed.
  //! It raises OutOfRange if Index is greater than the
  //! number of solutions.
  Standard_EXPORT void Tangency1 (const Standard_Integer Index, Standard_Real& ParSol, Standard_Real& ParArg, gp_Pnt2d& PntSol) const;
  
  //! Returns information about the center (on the curv)
  //! of the result.
  //! ParArg is the intrinsic parameter of the point on
  //! the argument curv.
  //! PntSol is the center point of the solution curv.
  //! Raises NotDone if the construction algorithm  didn't succeed.
  //! It raises OutOfRange if Index is greater than the
  //! number of solutions.
  Standard_EXPORT void CenterOn3 (const Standard_Integer Index, Standard_Real& ParArg, gp_Pnt2d& PntSol) const;
  
  //! Returns True if the solution number Index is equal to
  //! the first argument and False in the other cases.
  //! Raises NotDone if the construction algorithm  didn't succeed.
  //! It raises OutOfRange if Index is greater than the
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
  TColgp_Array1OfPnt2d pntcen3;
  TColStd_Array1OfReal par1sol;
  TColStd_Array1OfReal pararg1;
  TColStd_Array1OfReal parcen3;


};







#endif // _GccAna_Circ2dTanOnRad_HeaderFile
