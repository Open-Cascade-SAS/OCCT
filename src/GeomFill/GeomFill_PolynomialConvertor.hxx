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
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfVec.hxx>
class gp_Pnt;
class gp_Vec;

//! To convert circular section in polynome
class GeomFill_PolynomialConvertor
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT GeomFill_PolynomialConvertor();

  //! say if <me> is Initialized
  Standard_EXPORT Standard_Boolean Initialized() const;

  Standard_EXPORT void Init();

  Standard_EXPORT void Section(const gp_Pnt&       FirstPnt,
                               const gp_Pnt&       Center,
                               const gp_Vec&       Dir,
                               const Standard_Real Angle,
                               TColgp_Array1OfPnt& Poles) const;

  Standard_EXPORT void Section(const gp_Pnt&       FirstPnt,
                               const gp_Vec&       DFirstPnt,
                               const gp_Pnt&       Center,
                               const gp_Vec&       DCenter,
                               const gp_Vec&       Dir,
                               const gp_Vec&       DDir,
                               const Standard_Real Angle,
                               const Standard_Real DAngle,
                               TColgp_Array1OfPnt& Poles,
                               TColgp_Array1OfVec& DPoles) const;

  Standard_EXPORT void Section(const gp_Pnt&       FirstPnt,
                               const gp_Vec&       DFirstPnt,
                               const gp_Vec&       D2FirstPnt,
                               const gp_Pnt&       Center,
                               const gp_Vec&       DCenter,
                               const gp_Vec&       D2Center,
                               const gp_Vec&       Dir,
                               const gp_Vec&       DDir,
                               const gp_Vec&       D2Dir,
                               const Standard_Real Angle,
                               const Standard_Real DAngle,
                               const Standard_Real D2Angle,
                               TColgp_Array1OfPnt& Poles,
                               TColgp_Array1OfVec& DPoles,
                               TColgp_Array1OfVec& D2Poles) const;

protected:
private:
  Standard_Integer Ordre;
  Standard_Boolean myinit;
  math_Matrix      BH;
};

#endif // _GeomFill_PolynomialConvertor_HeaderFile
