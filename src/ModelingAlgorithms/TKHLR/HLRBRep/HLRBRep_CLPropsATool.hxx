// Created on: 1993-04-20
// Created by: Modelistation
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _HLRBRep_CLPropsATool_HeaderFile
#define _HLRBRep_CLPropsATool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
class gp_Pnt2d;
class gp_Vec2d;

class HLRBRep_CLPropsATool
{
public:
  DEFINE_STANDARD_ALLOC

  //! Computes the  point <P> of  parameter <U>   on the
  //! Curve from HLRBRep <C>.
  static void Value(const HLRBRep_Curve* A, const Standard_Real U, gp_Pnt2d& P);

  //! Computes the point <P>  and  first derivative <V1>
  //! of parameter <U> on the curve <C>.
  static void D1(const HLRBRep_Curve* A, const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1);

  //! Computes the point <P>,  the first derivative <V1>
  //! and second derivative <V2> of parameter <U> on the
  //! curve <C>.
  static void D2(const HLRBRep_Curve* A,
                 const Standard_Real  U,
                 gp_Pnt2d&            P,
                 gp_Vec2d&            V1,
                 gp_Vec2d&            V2);

  //! Computes the point <P>, the first derivative <V1>,
  //! the second derivative  <V2>   and third derivative
  //! <V3> of parameter <U> on the curve <C>.
  static void D3(const HLRBRep_Curve* A,
                 const Standard_Real  U,
                 gp_Pnt2d&            P,
                 gp_Vec2d&            V1,
                 gp_Vec2d&            V2,
                 gp_Vec2d&            V3);

  //! returns the order  of continuity of the curve <C>.
  //! returns 1 :  first  derivative only is  computable
  //! returns 2  : first and  second derivative only are
  //! computable.  returns  3 : first,  second and third
  //! are computable.
  static Standard_Integer Continuity(const HLRBRep_Curve* A);

  //! returns the first parameter bound of the curve.
  static Standard_Real FirstParameter(const HLRBRep_Curve* A);

  //! returns the  last  parameter bound  of  the curve.
  //! FirstParameter must be less than LastParamenter.
  static Standard_Real LastParameter(const HLRBRep_Curve* A);

protected:
private:
};

#include <HLRBRep_CLPropsATool.lxx>

#endif // _HLRBRep_CLPropsATool_HeaderFile
