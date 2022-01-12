// Created on: 1991-03-28
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

#ifndef _Geom2dGcc_Circ2dTanCenGeo_HeaderFile
#define _Geom2dGcc_Circ2dTanCenGeo_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Standard_Integer.hxx>
#include <TColgp_Array1OfCirc2d.hxx>
#include <GccEnt_Array1OfPosition.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <GccEnt_Position.hxx>
class Geom2dGcc_QCurve;
class gp_Pnt2d;
class gp_Circ2d;


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
//! If we did not use Tolerance it is impossible to
//! find a solution in the following case : Pcenter is
//! outside C1.
//! With Tolerance we will give a solution if the distance
//! between C1 and Pcenter is lower than or equal Tolerance/2.
class Geom2dGcc_Circ2dTanCenGeo 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! This method implements the algorithms used to
  //! create 2d circles tangent to a circle and
  //! centered on a point.
  Standard_EXPORT Geom2dGcc_Circ2dTanCenGeo(const Geom2dGcc_QCurve& Qualified1, const gp_Pnt2d& Pcenter, const Standard_Real Tolerance);
  
  //! This method returns True if the construction
  //! algorithm succeeded.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns the number of solutions and raises NotDone
  //! exception if the algorithm didn't succeed.
  //! It raises NotDone if the construction algorithm
  //! didn't succeed.
  Standard_EXPORT Standard_Integer NbSolutions() const;
  
  //! Returns the solution number Index and raises OutOfRange
  //! exception if Index is greater than the number of solutions.
  //! Be careful: the Index is only a way to get all the
  //! solutions, but is not associated to these outside the
  //! context of the algorithm-object.
  //! It raises NotDone if the construction algorithm
  //! didn't succeed.
  //! It raises OutOfRange if Index is greater than the
  //! number of solutions or less than zero.
  Standard_EXPORT gp_Circ2d ThisSolution (const Standard_Integer Index) const;
  
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




protected:





private:



  Standard_Boolean WellDone;
  Standard_Integer NbrSol;
  TColgp_Array1OfCirc2d cirsol;
  GccEnt_Array1OfPosition qualifier1;
  TColgp_Array1OfPnt2d pnttg1sol;
  TColStd_Array1OfReal par1sol;
  TColStd_Array1OfReal pararg1;


};







#endif // _Geom2dGcc_Circ2dTanCenGeo_HeaderFile
