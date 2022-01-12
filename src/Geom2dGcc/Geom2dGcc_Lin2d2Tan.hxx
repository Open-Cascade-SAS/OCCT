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

#ifndef _Geom2dGcc_Lin2d2Tan_HeaderFile
#define _Geom2dGcc_Lin2d2Tan_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Standard_Integer.hxx>
#include <TColgp_Array1OfLin2d.hxx>
#include <GccEnt_Array1OfPosition.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <GccEnt_Position.hxx>
class Geom2dGcc_QualifiedCurve;
class gp_Pnt2d;
class gp_Lin2d;
class Geom2dGcc_Lin2d2TanIter;
class Geom2dAdaptor_Curve;


//! This class implements the algorithms used to
//! create 2d lines tangent to 2 other elements which
//! can be circles, curves or points.
//! More than one argument must be a curve.
//! Describes functions for building a 2D line:
//! -   tangential to 2 curves, or
//! -   tangential to a curve and passing through a point.
//! A Lin2d2Tan object provides a framework for:
//! -   defining the construction of 2D line(s),
//! -   implementing the construction algorithm, and
//! -   consulting the result(s).
//!
//! Note: Some constructors may check the type of the qualified argument
//! and raise BadQualifier Error in case of incorrect couple (qualifier, curv).
class Geom2dGcc_Lin2d2Tan 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! This class implements the algorithms used to create 2d
  //! line tangent to two curves.
  //! Tolang is used to determine the tolerance for the tangency points.
  Standard_EXPORT Geom2dGcc_Lin2d2Tan(const Geom2dGcc_QualifiedCurve& Qualified1, const Geom2dGcc_QualifiedCurve& Qualified2, const Standard_Real Tolang);
  
  //! This class implements the algorithms used to create 2d
  //! lines passing through a point and tangent to a curve.
  //! Tolang is used to determine the tolerance for the tangency points.
  Standard_EXPORT Geom2dGcc_Lin2d2Tan(const Geom2dGcc_QualifiedCurve& Qualified1, const gp_Pnt2d& ThePoint, const Standard_Real Tolang);
  
  //! This class implements the algorithms used to create 2d
  //! line tangent to two curves.
  //! Tolang is used to determine the tolerance for the tangency points.
  //! Param1 is used for the initial guess on the first curve.
  //! Param2 is used for the initial guess on the second curve.
  Standard_EXPORT Geom2dGcc_Lin2d2Tan(const Geom2dGcc_QualifiedCurve& Qualified1, const Geom2dGcc_QualifiedCurve& Qualified2, const Standard_Real Tolang, const Standard_Real Param1, const Standard_Real Param2);
  
  //! This class implements the algorithms used to create 2d
  //! lines passing through a point and tangent to a curve.
  //! Tolang is used to determine the tolerance for the tangency points.
  //! Param2 is used for the initial guess on the curve.
  Standard_EXPORT Geom2dGcc_Lin2d2Tan(const Geom2dGcc_QualifiedCurve& Qualified1, const gp_Pnt2d& ThePoint, const Standard_Real Tolang, const Standard_Real Param1);
  
  //! Returns true if the construction algorithm does not fail
  //! (even if it finds no solution).
  //! Note: IsDone protects against a failure arising from a
  //! more internal intersection algorithm, which has
  //! reached its numeric limits.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns the number of lines, representing solutions computed by this algorithm.
  //! Exceptions StdFail_NotDone if the construction fails.R
  Standard_EXPORT Standard_Integer NbSolutions() const;
  
  //! Returns a line, representing the solution of index Index computed by this algorithm.
  //! Warning
  //! This indexing simply provides a means of consulting the
  //! solutions. The index values are not associated with
  //! these solutions outside the context of the algorithm object.
  //! Exceptions
  //! Standard_OutOfRange if Index is less than zero or
  //! greater than the number of solutions computed by this algorithm.
  //! StdFail_NotDone if the construction fails.
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
  //! -   GccEnt_noqualifier if the tangency argument is a   point.
  //! Exceptions
  //! Standard_OutOfRange if Index is less than zero or
  //! greater than the number of solutions computed by this algorithm.
  //! StdFail_NotDone if the construction fails.
  Standard_EXPORT void WhichQualifier (const Standard_Integer Index, GccEnt_Position& Qualif1, GccEnt_Position& Qualif2) const;
  
  //! Returns information about the tangency point between the
  //! result and the first argument.
  //! ParSol is the intrinsic parameter of the point PntSol on
  //! the solution curv.
  //! ParArg is the intrinsic parameter of the point PntSol on the argument curv.
  //! Exceptions
  //! Standard_OutOfRange if Index is less than zero or
  //! greater than the number of solutions computed by this algorithm.
  //! StdFail_NotDone if the construction fails.
  Standard_EXPORT void Tangency1 (const Standard_Integer Index, Standard_Real& ParSol, Standard_Real& ParArg, gp_Pnt2d& PntSol) const;
  
  //! Returns information about the tangency point between the
  //! result and the first argument.
  //! ParSol is the intrinsic parameter of the point PntSol on the solution curv.
  //! ParArg is the intrinsic parameter of the point PntSol on the argument curv.
  //! Exceptions
  //! Standard_OutOfRange if Index is less than zero or
  //! greater than the number of solutions computed by this algorithm.
  //! StdFail_NotDone if the construction fails.
  Standard_EXPORT void Tangency2 (const Standard_Integer Index, Standard_Real& ParSol, Standard_Real& ParArg, gp_Pnt2d& PntSol) const;




protected:





private:

  
  Standard_EXPORT Standard_Boolean Add (const Standard_Integer theIndex, const Geom2dGcc_Lin2d2TanIter& theLin, const Standard_Real theTol, const Geom2dAdaptor_Curve& theC1, const Geom2dAdaptor_Curve& theC2);


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







#endif // _Geom2dGcc_Lin2d2Tan_HeaderFile
