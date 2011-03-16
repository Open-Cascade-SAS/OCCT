// File:	BRepBlend_AppFunc.cxx
// Created:	Tue Nov 26 09:53:06 1996
// Author:	Philippe MANGIN
//		<pmn@sgi29>

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

