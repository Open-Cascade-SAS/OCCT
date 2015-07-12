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

#ifndef _Geom2dGcc_Circ2d2TanOnGeo_HeaderFile
#define _Geom2dGcc_Circ2d2TanOnGeo_HeaderFile

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
class StdFail_NotDone;
class GccEnt_BadQualifier;
class Standard_OutOfRange;
class GccEnt_QualifiedCirc;
class Geom2dAdaptor_Curve;
class GccEnt_QualifiedLin;
class gp_Pnt2d;
class gp_Circ2d;


//! This class implements the algorithms used to
//! create 2d circles TANgent to 2 entities and
//! having the center ON a curve.
//! The order of the tangency argument is always
//! QualifiedCirc, QualifiedLin, QualifiedCurv, Pnt2d.
//! the arguments are :
//! - The two tangency arguments (lines, circles or points).
//! - The center line (a curve).
//! - The parameter for each tangency argument which
//! is a curve.
//! - The tolerance.
class Geom2dGcc_Circ2d2TanOnGeo 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! This method implements the algorithms used to
  //! create 2d circles TANgent to two 2d circles and
  //! having the center ON a curve.
  Standard_EXPORT Geom2dGcc_Circ2d2TanOnGeo(const GccEnt_QualifiedCirc& Qualified1, const GccEnt_QualifiedCirc& Qualified2, const Geom2dAdaptor_Curve& OnCurv, const Standard_Real Tolerance);
  
  //! This method implements the algorithms used to
  //! create 2d circles TANgent to a 2d circle and a 2d line
  //! having the center ON a curve.
  Standard_EXPORT Geom2dGcc_Circ2d2TanOnGeo(const GccEnt_QualifiedCirc& Qualified1, const GccEnt_QualifiedLin& Qualified2, const Geom2dAdaptor_Curve& OnCurv, const Standard_Real Tolerance);
  
  //! This method implements the algorithms used to
  //! create 2d circles TANgent to a 2d circle and a point
  //! having the center ON a curve.
  Standard_EXPORT Geom2dGcc_Circ2d2TanOnGeo(const GccEnt_QualifiedCirc& Qualified1, const gp_Pnt2d& Point2, const Geom2dAdaptor_Curve& OnCurv, const Standard_Real Tolerance);
  
  //! This method implements the algorithms used to
  //! create 2d circles TANgent to two 2d lines
  //! having the center ON a curve.
  Standard_EXPORT Geom2dGcc_Circ2d2TanOnGeo(const GccEnt_QualifiedLin& Qualified1, const GccEnt_QualifiedLin& Qualified2, const Geom2dAdaptor_Curve& OnCurv, const Standard_Real Tolerance);
  
  //! This method implements the algorithms used to
  //! create 2d circles TANgent to a 2d line and a point
  //! having the center ON a 2d line.
  Standard_EXPORT Geom2dGcc_Circ2d2TanOnGeo(const GccEnt_QualifiedLin& Qualified1, const gp_Pnt2d& Qualified2, const Geom2dAdaptor_Curve& OnCurv, const Standard_Real Tolerance);
  
  //! This method implements the algorithms used to
  //! create 2d circles TANgent to two points
  //! having the center ON a 2d line.
  Standard_EXPORT Geom2dGcc_Circ2d2TanOnGeo(const gp_Pnt2d& Point1, const gp_Pnt2d& Point2, const Geom2dAdaptor_Curve& OnCurv, const Standard_Real Tolerance);
  
  //! This method returns True if the construction
  //! algorithm succeeded.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! This method returns the number of solutions.
  //! It raises NotDone if the construction algorithm
  //! didn't succeed.
  Standard_EXPORT Standard_Integer NbSolutions() const;
  
  //! Returns the solution number Index and raises OutOfRange
  //! exception if Index is greater than the number of solutions.
  //! Be careful: the Index is only a way to get all the
  //! solutions, but is not associated to those outside the
  //! context of the algorithm-object.
  //! It raises NotDone if the construction algorithm
  //! didn't succeed.
  //! It raises OutOfRange if Index is greater than the
  //! number of solutions.
  Standard_EXPORT gp_Circ2d ThisSolution (const Standard_Integer Index) const;
  
  //! It returns the informations about the qualifiers of
  //! the tangency
  //! arguments concerning the solution number Index.
  //! It returns the real qualifiers (the qualifiers given to the
  //! constructor method in case of enclosed, enclosing and outside
  //! and the qualifiers computedin case of unqualified).
  Standard_EXPORT void WhichQualifier (const Standard_Integer Index, GccEnt_Position& Qualif1, GccEnt_Position& Qualif2) const;
  
  //! Returns informations about the tangency point between the
  //! result number Index and the first argument.
  //! ParSol is the intrinsic parameter of the point on the
  //! solution curv.
  //! ParArg is the intrinsic parameter of the point on the
  //! argument curv.
  //! PntSol is the tangency point on the solution curv.
  //! PntArg is the tangency point on the argument curv.
  //! It raises NotDone if the construction algorithm
  //! didn't succeed.
  //! It raises OutOfRange if Index is greater than the
  //! number of solutions.
  Standard_EXPORT void Tangency1 (const Standard_Integer Index, Standard_Real& ParSol, Standard_Real& ParArg, gp_Pnt2d& PntSol) const;
  
  //! Returns informations about the tangency point between the
  //! result number Index and the second argument.
  //! ParSol is the intrinsic parameter of the point on the
  //! solution curv.
  //! ParArg is the intrinsic parameter of the point on the
  //! argument curv.
  //! PntSol is the tangency point on the solution curv.
  //! PntArg is the tangency point on the argument curv.
  //! It raises NotDone if the construction algorithm
  //! didn't succeed.
  //! It raises OutOfRange if Index is greater than the
  //! number of solutions.
  Standard_EXPORT void Tangency2 (const Standard_Integer Index, Standard_Real& ParSol, Standard_Real& ParArg, gp_Pnt2d& PntSol) const;
  
  //! Returns informations about the center (on the curv)
  //! of the result.
  //! ParArg is the intrinsic parameter of the point on
  //! the argument curv.
  //! PntSol is the center point of the solution curv.
  //! It raises NotDone if the construction algorithm
  //! didn't succeed.
  //! It raises OutOfRange if Index is greater than the
  //! number of solutions.
  Standard_EXPORT void CenterOn3 (const Standard_Integer Index, Standard_Real& ParArg, gp_Pnt2d& PntSol) const;
  
  //! Returns True if the solution number Index is equal to
  //! the first argument and False in the other cases.
  //! It raises NotDone if the construction algorithm
  //! didn't succeed.
  //! It raises OutOfRange if Index is greater than the
  //! number of solutions.
  Standard_EXPORT Standard_Boolean IsTheSame1 (const Standard_Integer Index) const;
  
  //! Returns True if the solution number Index is equal to
  //! the second argument and False in the other cases.
  //! It raises NotDone if the construction algorithm
  //! didn't succeed.
  //! It raises OutOfRange if Index is greater than the
  //! number of solutions.
  Standard_EXPORT Standard_Boolean IsTheSame2 (const Standard_Integer Index) const;




protected:





private:



  Standard_Boolean WellDone;
  Standard_Integer NbrSol;
  TColgp_Array1OfCirc2d cirsol;
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


};







#endif // _Geom2dGcc_Circ2d2TanOnGeo_HeaderFile
