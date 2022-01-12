// Created on: 1991-03-18
// Created by: Remy GILET
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

#ifndef _GccAna_Circ2d3Tan_HeaderFile
#define _GccAna_Circ2d3Tan_HeaderFile

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
class GccEnt_QualifiedLin;
class gp_Pnt2d;
class gp_Circ2d;


//! This class implements the algorithms used to
//! create 2d circles tangent to 3 points/lines/circles.
//! The arguments of all construction methods are :
//! - The three qualified elements for the
//! tangency constraints (QualifiedCirc, QualifiedLine,
//! Points).
//! - A real Tolerance.
//! Tolerance is only used in the limit cases.
//! For example :
//! We want to create a circle tangent to an UnqualifiedCirc
//! C1 and an UnqualifiedCirc C2 and an UnqualifiedCirc C3
//! with a tolerance Tolerance.
//! If we do not use Tolerance it is impossible to find
//! a solution in the following case : C2 is inside C1
//! and there is no intersection point between the two
//! circles, and C3 is completely outside C1.
//! With Tolerance we will find a solution if the
//! lowest distance between C1 and C2 is lower than or
//! equal Tolerance.
class GccAna_Circ2d3Tan 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! This method implements the algorithms used to
  //! create 2d circles tangent to 3 circles.
  //! ConstructionError is raised if there is a problem during
  //! the computation.
  Standard_EXPORT GccAna_Circ2d3Tan(const GccEnt_QualifiedCirc& Qualified1, const GccEnt_QualifiedCirc& Qualified2, const GccEnt_QualifiedCirc& Qualified3, const Standard_Real Tolerance);
  
  //! This method implements the algorithms used to
  //! create 2d circles tangent to 2 circles and 1 line.
  //! ConstructionError is raised if there is a problem during
  //! the computation.
  Standard_EXPORT GccAna_Circ2d3Tan(const GccEnt_QualifiedCirc& Qualified1, const GccEnt_QualifiedCirc& Qualified2, const GccEnt_QualifiedLin& Qualified3, const Standard_Real Tolerance);
  
  //! This method implements the algorithms used to
  //! create 2d circles tangent to 1 circle and 2 lines.
  //! ConstructionError is raised if there is a problem during
  //! the computation.
  Standard_EXPORT GccAna_Circ2d3Tan(const GccEnt_QualifiedCirc& Qualified1, const GccEnt_QualifiedLin& Qualified2, const GccEnt_QualifiedLin& Qualified3, const Standard_Real Tolerance);
  
  //! This method implements the algorithms used to
  //! create 2d circles tangent to 3 lines.
  //! ConstructionError is raised if there is a problem during
  //! the computation.
  Standard_EXPORT GccAna_Circ2d3Tan(const GccEnt_QualifiedLin& Qualified1, const GccEnt_QualifiedLin& Qualified2, const GccEnt_QualifiedLin& Qualified3, const Standard_Real Tolerance);
  
  //! This method implements the algorithms used to
  //! create 2d circles tangent to 2 circles and 1 Point.
  //! ConstructionError is raised if there is a problem during
  //! the computation.
  Standard_EXPORT GccAna_Circ2d3Tan(const GccEnt_QualifiedCirc& Qualified1, const GccEnt_QualifiedCirc& Qualified2, const gp_Pnt2d& Point3, const Standard_Real Tolerance);
  
  //! This method implements the algorithms used to
  //! create 2d circles tangent to a circle and a line and
  //! 1 Point.
  //! ConstructionError is raised if there is a problem during
  //! the computation.
  Standard_EXPORT GccAna_Circ2d3Tan(const GccEnt_QualifiedCirc& Qualified1, const GccEnt_QualifiedLin& Qualified2, const gp_Pnt2d& Point3, const Standard_Real Tolerance);
  
  //! This method implements the algorithms used to
  //! create 2d circles tangent to 2 lines and 1 Point.
  //! ConstructionError is raised if there is a problem during
  //! the computation.
  Standard_EXPORT GccAna_Circ2d3Tan(const GccEnt_QualifiedLin& Qualified1, const GccEnt_QualifiedLin& Qualified2, const gp_Pnt2d& Point3, const Standard_Real Tolerance);
  
  //! This method implements the algorithms used to
  //! create 2d circles tangent to a circle and passing
  //! through 2 Points.
  //! ConstructionError is raised if there is a problem during
  //! the computation.
  Standard_EXPORT GccAna_Circ2d3Tan(const GccEnt_QualifiedCirc& Qualified1, const gp_Pnt2d& Point2, const gp_Pnt2d& Point3, const Standard_Real Tolerance);
  
  //! This method implements the algorithms used to
  //! create 2d circles tangent to a line and passing
  //! through 2 Points.
  //! ConstructionError is raised if there is a problem during
  //! the computation.
  Standard_EXPORT GccAna_Circ2d3Tan(const GccEnt_QualifiedLin& Qualified1, const gp_Pnt2d& Point2, const gp_Pnt2d& Point3, const Standard_Real Tolerance);
  
  //! This method implements the algorithms used to
  //! create 2d circles passing through 3 Points.
  //! ConstructionError is raised if there is a problem during
  //! the computation.
  Standard_EXPORT GccAna_Circ2d3Tan(const gp_Pnt2d& Point1, const gp_Pnt2d& Point2, const gp_Pnt2d& Point3, const Standard_Real Tolerance);
  
  //! This method returns True if the construction
  //! algorithm succeeded.
  //! Note: IsDone protects against a failure arising from a
  //! more internal intersection algorithm, which has
  //! reached its numeric limits.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! This method returns the number of solutions.
  //! Raises NotDone if the construction algorithm didn't succeed.
  Standard_EXPORT Standard_Integer NbSolutions() const;
  
  //! Returns the solution number Index and raises OutOfRange
  //! exception if Index is greater than the number of
  //! solutions.
  //! Be careful: the Index is only a way to get all the
  //! solutions, but is not associated to those outside the
  //! context of the algorithm-object.
  //! Raises OutOfRange if Index is greater than
  //! the number of solutions.
  //! It raises NotDone if the algorithm failed.
  Standard_EXPORT gp_Circ2d ThisSolution (const Standard_Integer Index) const;
  
  //! Returns the information about the qualifiers of the
  //! tangency
  //! arguments concerning the solution number Index.
  //! It returns the real qualifiers (the qualifiers given to the
  //! constructor method in case of enclosed, enclosing and outside
  //! and the qualifiers computedin case of unqualified).
  Standard_EXPORT void WhichQualifier (const Standard_Integer Index, GccEnt_Position& Qualif1, GccEnt_Position& Qualif2, GccEnt_Position& Qualif3) const;
  
  //! Returns information about the tangency point between the
  //! result number Index and the first argument.
  //! ParSol is the intrinsic parameter of the point PntSol
  //! on the solution curv.
  //! ParArg is the intrinsic parameter of the point PntArg
  //! on the argument curv. Raises OutOfRange if Index is greater than
  //! the number of solutions.
  //! It raises NotDone if the algorithm failed.
  Standard_EXPORT void Tangency1 (const Standard_Integer Index, Standard_Real& ParSol, Standard_Real& ParArg, gp_Pnt2d& PntSol) const;
  
  //! Returns information about the tangency point between the
  //! result number Index and the first argument.
  //! ParSol is the intrinsic parameter of the point PntSol
  //! on the solution curv.
  //! ParArg is the intrinsic parameter of the point Pntsol
  //! on the argument curv. Raises OutOfRange if Index is greater than
  //! the number of solutions.
  //! It raises NotDone if the algorithm failed.
  Standard_EXPORT void Tangency2 (const Standard_Integer Index, Standard_Real& ParSol, Standard_Real& ParArg, gp_Pnt2d& PntSol) const;
  
  //! Returns information about the tangency point between the
  //! result number Index and the first argument.
  //! ParSol is the intrinsic parameter of the point PntSol
  //! on the solution curv.
  //! ParArg is the intrinsic parameter of the point Pntsol
  //! on the argument curv. Raises OutOfRange if Index is greater than
  //! the number of solutions.
  //! It raises NotDone if the algorithm failed.
  Standard_EXPORT void Tangency3 (const Standard_Integer Index, Standard_Real& ParSol, Standard_Real& ParArg, gp_Pnt2d& PntSol) const;
  
  //! Returns True if the solution number Index is equal to
  //! the first argument. Raises OutOfRange if Index is greater than
  //! the number of solutions.
  //! It raises NotDone if the algorithm failed.
  Standard_EXPORT Standard_Boolean IsTheSame1 (const Standard_Integer Index) const;
  
  //! Returns True if the solution number Index is equal to
  //! the second argument. Raises OutOfRange Index is greater than
  //! the number of solutions.
  //! It raises NotDone if the algorithm failed.
  Standard_EXPORT Standard_Boolean IsTheSame2 (const Standard_Integer Index) const;
  
  //! Returns True if the solution number Index is equal to
  //! the third argument. Raises OutOfRange if Index is greater than
  //! the number of solutions.
  //! It raises NotDone if the algorithm failed.
  Standard_EXPORT Standard_Boolean IsTheSame3 (const Standard_Integer Index) const;




protected:





private:



  Standard_Boolean WellDone;
  Standard_Integer NbrSol;
  TColgp_Array1OfCirc2d cirsol;
  GccEnt_Array1OfPosition qualifier1;
  GccEnt_Array1OfPosition qualifier2;
  GccEnt_Array1OfPosition qualifier3;
  TColStd_Array1OfInteger TheSame1;
  TColStd_Array1OfInteger TheSame2;
  TColStd_Array1OfInteger TheSame3;
  TColgp_Array1OfPnt2d pnttg1sol;
  TColgp_Array1OfPnt2d pnttg2sol;
  TColgp_Array1OfPnt2d pnttg3sol;
  TColStd_Array1OfReal par1sol;
  TColStd_Array1OfReal par2sol;
  TColStd_Array1OfReal par3sol;
  TColStd_Array1OfReal pararg1;
  TColStd_Array1OfReal pararg2;
  TColStd_Array1OfReal pararg3;


};







#endif // _GccAna_Circ2d3Tan_HeaderFile
