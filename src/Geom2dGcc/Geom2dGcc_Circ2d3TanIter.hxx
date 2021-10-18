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

#ifndef _Geom2dGcc_Circ2d3TanIter_HeaderFile
#define _Geom2dGcc_Circ2d3TanIter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <gp_Circ2d.hxx>
#include <GccEnt_Position.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_Real.hxx>
class GccEnt_QualifiedCirc;
class Geom2dGcc_QCurve;
class GccEnt_QualifiedLin;


//! This class implements the algorithms used to
//! create 2d circles tangent to 3 points/lines/circles/
//! curves with one curve or more.
//! The arguments of all construction methods are :
//! - The three qualifiied elements for the
//! tangency constrains (QualifiedCirc, QualifiedLine,
//! Qualifiedcurv, Points).
//! - A parameter for each QualifiedCurv.
class Geom2dGcc_Circ2d3TanIter 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! This method implements the algorithms used to
  //! create 2d circles tangent to 2 circles and a curve.
  Standard_EXPORT Geom2dGcc_Circ2d3TanIter(const GccEnt_QualifiedCirc& Qualified1, const GccEnt_QualifiedCirc& Qualified2, const Geom2dGcc_QCurve& Qualified3, const Standard_Real Param1, const Standard_Real Param2, const Standard_Real Param3, const Standard_Real Tolerance);
  
  //! This method implements the algorithms used to
  //! create 2d circles tangent to a circle and 2 curves.
  Standard_EXPORT Geom2dGcc_Circ2d3TanIter(const GccEnt_QualifiedCirc& Qualified1, const Geom2dGcc_QCurve& Qualified2, const Geom2dGcc_QCurve& Qualified3, const Standard_Real Param1, const Standard_Real Param2, const Standard_Real Param3, const Standard_Real Tolerance);
  
  //! This method implements the algorithms used to
  //! create 2d circles tangent to a circle and a line and
  //! a curve.
  Standard_EXPORT Geom2dGcc_Circ2d3TanIter(const GccEnt_QualifiedCirc& Qualified1, const GccEnt_QualifiedLin& Qualified2, const Geom2dGcc_QCurve& Qualified3, const Standard_Real Param1, const Standard_Real Param2, const Standard_Real Param3, const Standard_Real Tolerance);
  
  //! This method implements the algorithms used to
  //! create 2d circles tangent to a circle and a point and
  //! a curve.
  Standard_EXPORT Geom2dGcc_Circ2d3TanIter(const GccEnt_QualifiedCirc& Qualified1, const Geom2dGcc_QCurve& Qualified2, const gp_Pnt2d& Point3, const Standard_Real Param1, const Standard_Real Param2, const Standard_Real Tolerance);
  
  //! This method implements the algorithms used to
  //! create 2d circles tangent to 2 lines and a curve.
  Standard_EXPORT Geom2dGcc_Circ2d3TanIter(const GccEnt_QualifiedLin& Qualified1, const GccEnt_QualifiedLin& Qualified2, const Geom2dGcc_QCurve& Qualified3, const Standard_Real Param1, const Standard_Real Param2, const Standard_Real Param3, const Standard_Real Tolerance);
  
  //! This method implements the algorithms used to
  //! create 2d circles tangent to a line and 2 curves.
  Standard_EXPORT Geom2dGcc_Circ2d3TanIter(const GccEnt_QualifiedLin& Qualified1, const Geom2dGcc_QCurve& Qualified2, const Geom2dGcc_QCurve& Qualified3, const Standard_Real Param1, const Standard_Real Param2, const Standard_Real Param3, const Standard_Real Tolerance);
  
  //! This method implements the algorithms used to
  //! create 2d circles tangent to a line and a curve
  //! and a point.
  Standard_EXPORT Geom2dGcc_Circ2d3TanIter(const GccEnt_QualifiedLin& Qualified1, const Geom2dGcc_QCurve& Qualified2, const gp_Pnt2d& Point3, const Standard_Real Param1, const Standard_Real Param2, const Standard_Real Tolerance);
  
  //! This method implements the algorithms used to
  //! create 2d circles tangent to a curve and 2 points.
  Standard_EXPORT Geom2dGcc_Circ2d3TanIter(const Geom2dGcc_QCurve& Qualified1, const gp_Pnt2d& Point1, const gp_Pnt2d& Point2, const Standard_Real Param1, const Standard_Real Tolerance);
  
  //! This method implements the algorithms used to
  //! create 2d circles tangent to 2 curves and a point.
  Standard_EXPORT Geom2dGcc_Circ2d3TanIter(const Geom2dGcc_QCurve& Qualified1, const Geom2dGcc_QCurve& Qualified2, const gp_Pnt2d& Point2, const Standard_Real Param1, const Standard_Real Param2, const Standard_Real Tolerance);
  
  //! This method implements the algorithms used to
  //! create 2d circles tangent to 3 curves.
  Standard_EXPORT Geom2dGcc_Circ2d3TanIter(const Geom2dGcc_QCurve& Qualified1, const Geom2dGcc_QCurve& Qualified2, const Geom2dGcc_QCurve& Qualified3, const Standard_Real Param1, const Standard_Real Param2, const Standard_Real Param3, const Standard_Real Tolerance);
  
  //! This method returns True if the construction
  //! algorithm succeeded.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns the solution.
  //! It raises NotDone if the construction algorithm
  //! didn't succeed.
  Standard_EXPORT gp_Circ2d ThisSolution() const;
  
  Standard_EXPORT void WhichQualifier (GccEnt_Position& Qualif1, GccEnt_Position& Qualif2, GccEnt_Position& Qualif3) const;
  
  //! Returns information about the tangency point between
  //! the result and the first argument.
  //! ParSol is the intrinsic parameter of the point PntSol
  //! on the solution curv.
  //! ParArg is the intrinsic parameter of the point PntSol
  //! on the argument curv.
  //! It raises NotDone if the construction algorithm
  //! didn't succeed.
  Standard_EXPORT void Tangency1 (Standard_Real& ParSol, Standard_Real& ParArg, gp_Pnt2d& PntSol) const;
  
  //! Returns information about the tangency point between
  //! the result and the second argument.
  //! ParSol is the intrinsic parameter of the point PntSol
  //! on the solution curv.
  //! ParArg is the intrinsic parameter of the point PntSol
  //! on the argument curv.
  //! It raises NotDone if the construction algorithm
  //! didn't succeed.
  Standard_EXPORT void Tangency2 (Standard_Real& ParSol, Standard_Real& ParArg, gp_Pnt2d& PntSol) const;
  
  //! Returns information about the tangency point between
  //! the result and the third argument.
  //! ParSol is the intrinsic parameter of the point PntSol
  //! on the solution curv.
  //! ParArg is the intrinsic parameter of the point PntSol
  //! on the argument curv.
  //! It raises NotDone if the construction algorithm
  //! didn't succeed.
  Standard_EXPORT void Tangency3 (Standard_Real& ParSol, Standard_Real& ParArg, gp_Pnt2d& PntSol) const;
  
  //! It raises NotDone if the construction algorithm
  //! didn't succeed.
  Standard_EXPORT Standard_Boolean IsTheSame1() const;
  
  //! It raises NotDone if the construction algorithm
  //! didn't succeed.
  Standard_EXPORT Standard_Boolean IsTheSame2() const;
  
  //! It raises NotDone if the construction algorithm
  //! didn't succeed.
  Standard_EXPORT Standard_Boolean IsTheSame3() const;




protected:





private:



  Standard_Boolean WellDone;
  gp_Circ2d cirsol;
  GccEnt_Position qualifier1;
  GccEnt_Position qualifier2;
  GccEnt_Position qualifier3;
  Standard_Boolean TheSame1;
  Standard_Boolean TheSame2;
  Standard_Boolean TheSame3;
  gp_Pnt2d pnttg1sol;
  gp_Pnt2d pnttg2sol;
  gp_Pnt2d pnttg3sol;
  Standard_Real par1sol;
  Standard_Real par2sol;
  Standard_Real par3sol;
  Standard_Real pararg1;
  Standard_Real pararg2;
  Standard_Real pararg3;


};







#endif // _Geom2dGcc_Circ2d3TanIter_HeaderFile
