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

#ifndef _Geom2dGcc_Lin2dTanOblIter_HeaderFile
#define _Geom2dGcc_Lin2dTanOblIter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <gp_Lin2d.hxx>
#include <GccEnt_Position.hxx>
#include <gp_Pnt2d.hxx>
class Geom2dGcc_QCurve;

//! This class implements the algorithms used to
//! create 2d line tangent to a curve QualifiedCurv and
//! doing an angle Angle with a line TheLin.
//! The angle must be in Radian.
class Geom2dGcc_Lin2dTanOblIter
{
public:
  DEFINE_STANDARD_ALLOC

  //! This class implements the algorithm used to
  //! create 2d line tangent to a curve and doing an
  //! angle Angle with the line TheLin.
  //! Angle must be in Radian.
  //! Param2 is the initial guess on the curve QualifiedCurv.
  //! Tolang is the angular tolerance.
  Standard_EXPORT Geom2dGcc_Lin2dTanOblIter(const Geom2dGcc_QCurve& Qualified1,
                                            const gp_Lin2d&         TheLin,
                                            const double     Param1,
                                            const double     TolAng,
                                            const double     Angle = 0);

  //! This method returns true when there is a solution
  //! and false in the other cases.
  Standard_EXPORT bool IsDone() const;

  Standard_EXPORT gp_Lin2d ThisSolution() const;

  Standard_EXPORT void WhichQualifier(GccEnt_Position& Qualif1) const;

  Standard_EXPORT void Tangency1(double& ParSol,
                                 double& ParArg,
                                 gp_Pnt2d&      PntSol) const;

  Standard_EXPORT void Intersection2(double& ParSol,
                                     double& ParArg,
                                     gp_Pnt2d&      PntSol) const;

  Standard_EXPORT bool IsParallel2() const;

private:
  bool WellDone;
  bool Paral2;
  gp_Lin2d         linsol;
  GccEnt_Position  qualifier1;
  gp_Pnt2d         pnttg1sol;
  gp_Pnt2d         pntint2sol;
  double    par1sol;
  double    par2sol;
  double    pararg1;
  double    pararg2;
};

#endif // _Geom2dGcc_Lin2dTanOblIter_HeaderFile
