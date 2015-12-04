// Created on: 1997-07-25
// Created by: Jerome LEMONIER
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


#include <Blend_AppFunction.hxx>
#include <Blend_Point.hxx>
#include <Blend_SurfRstFunction.hxx>
#include <BRepBlend_AppFuncRst.hxx>
#include <BRepBlend_Line.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepBlend_AppFuncRst,BRepBlend_AppFuncRoot)

BRepBlend_AppFuncRst::BRepBlend_AppFuncRst (Handle(BRepBlend_Line)& Line,
					    Blend_SurfRstFunction& Func,
					    const Standard_Real Tol3d,
					    const Standard_Real Tol2d)
:BRepBlend_AppFuncRoot(Line,Func,Tol3d,Tol2d)
{
}

void BRepBlend_AppFuncRst::Point(const Blend_AppFunction& Func,
				 const Standard_Real Param,
				 const math_Vector& theSol,
				 Blend_Point& Pnt)const
{
  Pnt.SetValue(Func.Pnt1(), Func.Pnt2(),
	       Param,
	       theSol(1), theSol(2), theSol(3));
}

void BRepBlend_AppFuncRst::Vec(math_Vector& theSol,
			       const Blend_Point& Pnt)const
{
  Pnt.ParametersOnS(theSol(1),theSol(2));
  theSol(3) = Pnt.ParameterOnC();
}

