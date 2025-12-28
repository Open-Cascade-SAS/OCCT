// Created on: 1997-07-18
// Created by: Philippe MANGIN
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _GeomFill_PolynomialConvertor_HeaderFile
#define _GeomFill_PolynomialConvertor_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <math_Matrix.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Vec.hxx>
#include <NCollection_Array1.hxx>
class gp_Pnt;
class gp_Vec;

//! To convert circular section in polynome
class GeomFill_PolynomialConvertor
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT GeomFill_PolynomialConvertor();

  //! say if <me> is Initialized
  Standard_EXPORT bool Initialized() const;

  Standard_EXPORT void Init();

  Standard_EXPORT void Section(const gp_Pnt&       FirstPnt,
                               const gp_Pnt&       Center,
                               const gp_Vec&       Dir,
                               const double Angle,
                               NCollection_Array1<gp_Pnt>& Poles) const;

  Standard_EXPORT void Section(const gp_Pnt&       FirstPnt,
                               const gp_Vec&       DFirstPnt,
                               const gp_Pnt&       Center,
                               const gp_Vec&       DCenter,
                               const gp_Vec&       Dir,
                               const gp_Vec&       DDir,
                               const double Angle,
                               const double DAngle,
                               NCollection_Array1<gp_Pnt>& Poles,
                               NCollection_Array1<gp_Vec>& DPoles) const;

  Standard_EXPORT void Section(const gp_Pnt&       FirstPnt,
                               const gp_Vec&       DFirstPnt,
                               const gp_Vec&       D2FirstPnt,
                               const gp_Pnt&       Center,
                               const gp_Vec&       DCenter,
                               const gp_Vec&       D2Center,
                               const gp_Vec&       Dir,
                               const gp_Vec&       DDir,
                               const gp_Vec&       D2Dir,
                               const double Angle,
                               const double DAngle,
                               const double D2Angle,
                               NCollection_Array1<gp_Pnt>& Poles,
                               NCollection_Array1<gp_Vec>& DPoles,
                               NCollection_Array1<gp_Vec>& D2Poles) const;

private:
  int Ordre;
  bool myinit;
  math_Matrix      BH;
};

#endif // _GeomFill_PolynomialConvertor_HeaderFile
