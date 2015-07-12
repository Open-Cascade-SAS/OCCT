// Created on: 1991-04-24
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

#ifndef _Geom2dGcc_Lin2d2TanIter_HeaderFile
#define _Geom2dGcc_Lin2d2TanIter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <gp_Lin2d.hxx>
#include <GccEnt_Position.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_Real.hxx>
class GccEnt_BadQualifier;
class StdFail_NotDone;
class Geom2dGcc_QCurve;
class gp_Pnt2d;
class GccEnt_QualifiedCirc;
class gp_Lin2d;


//! This class implements the algorithms used to
//! create 2d lines tangent to 2 other elements which
//! can be circles, curves or points.
//! More than one argument must be a curve.
//!
//! Note: Some constructors may check the type of the qualified argument
//! and raise BadQualifier Error in case of incorrect couple (qualifier,
//! curv).
//! For example: "EnclosedCirc".
class Geom2dGcc_Lin2d2TanIter 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! This class implements the algorithms used to create 2d
  //! lines passing thrue a point and tangent to a curve.
  //! Tolang is used to determine the tolerance for the
  //! tangency points.
  //! Param2 is used for the initial guess on the curve.
  Standard_EXPORT Geom2dGcc_Lin2d2TanIter(const Geom2dGcc_QCurve& Qualified1, const gp_Pnt2d& ThePoint, const Standard_Real Param1, const Standard_Real Tolang);
  
  //! This class implements the algorithms used to create 2d
  //! line tangent to a circle and to a cuve.
  //! Tolang is used to determine the tolerance for the
  //! tangency points.
  //! Param2 is used for the initial guess on the curve.
  //! Exception BadQualifier is raised in the case of
  //! EnclosedCirc
  Standard_EXPORT Geom2dGcc_Lin2d2TanIter(const GccEnt_QualifiedCirc& Qualified1, const Geom2dGcc_QCurve& Qualified2, const Standard_Real Param2, const Standard_Real Tolang);
  
  //! This class implements the algorithms used to create 2d
  //! line tangent to two curves.
  //! Tolang is used to determine the tolerance for the
  //! tangency points.
  //! Param1 is used for the initial guess on the first curve.
  //! Param2 is used for the initial guess on the second curve.
  Standard_EXPORT Geom2dGcc_Lin2d2TanIter(const Geom2dGcc_QCurve& Qualified1, const Geom2dGcc_QCurve& Qualified2, const Standard_Real Param1, const Standard_Real Param2, const Standard_Real Tolang);
  
  //! This methode returns true when there is a solution
  //! and false in the other cases.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns the solution.
  Standard_EXPORT gp_Lin2d ThisSolution() const;
  
  Standard_EXPORT void WhichQualifier (GccEnt_Position& Qualif1, GccEnt_Position& Qualif2) const;
  
  //! Returns informations about the tangency point between the
  //! result and the first argument.
  //! ParSol is the intrinsic parameter of the point PntSol on
  //! the solution curv.
  //! ParArg is the intrinsic parameter of the point PntSol on
  //! the argument curv.
  Standard_EXPORT void Tangency1 (Standard_Real& ParSol, Standard_Real& ParArg, gp_Pnt2d& PntSol) const;
  
  Standard_EXPORT void Tangency2 (Standard_Real& ParSol, Standard_Real& ParArg, gp_Pnt2d& PntSol) const;




protected:





private:



  Standard_Boolean WellDone;
  gp_Lin2d linsol;
  GccEnt_Position qualifier1;
  GccEnt_Position qualifier2;
  gp_Pnt2d pnttg1sol;
  gp_Pnt2d pnttg2sol;
  Standard_Real par1sol;
  Standard_Real par2sol;
  Standard_Real pararg1;
  Standard_Real pararg2;


};







#endif // _Geom2dGcc_Lin2d2TanIter_HeaderFile
