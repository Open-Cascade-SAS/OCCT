// Created on: 1996-11-26
// Created by: Philippe MANGIN
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <BRepBlend_AppFunc.ixx>

BRepBlend_AppFunc::BRepBlend_AppFunc(Handle(BRepBlend_Line)& Line,
				     Blend_Function& Func,
				     const Standard_Real Tol3d,
				     const Standard_Real Tol2d)
:BRepBlend_AppFuncRoot(Line,Func,Tol3d,Tol2d)
{
}

void BRepBlend_AppFunc::Point(const Blend_AppFunction& Func,
			      const Standard_Real Param,
			      const math_Vector& Sol,
			      Blend_Point& Pnt)const
{
  Pnt.SetValue(Func.Pnt1(), Func.Pnt2(),
	       Param,
	       Sol(1), Sol(2), Sol(3), Sol(4));
}

void BRepBlend_AppFunc::Vec(math_Vector& Sol,
			    const Blend_Point& Pnt)const
{
  Pnt.ParametersOnS1(Sol(1),Sol(2));
  Pnt.ParametersOnS2(Sol(3),Sol(4));
}

