// File:	BRepBlend_AppFuncRst.cxx
// Created:	Fri Jul 25 14:18:14 1997
// Author:	Jerome LEMONIER
//		<jlr@sgi64>


#include <BRepBlend_AppFuncRst.ixx>

BRepBlend_AppFuncRst::BRepBlend_AppFuncRst (Handle(BRepBlend_Line)& Line,
					    Blend_SurfRstFunction& Func,
					    const Standard_Real Tol3d,
					    const Standard_Real Tol2d)
:BRepBlend_AppFuncRoot(Line,Func,Tol3d,Tol2d)
{
}

void BRepBlend_AppFuncRst::Point(const Blend_AppFunction& Func,
				 const Standard_Real Param,
				 const math_Vector& Sol,
				 Blend_Point& Pnt)const
{
  Pnt.SetValue(Func.Pnt1(), Func.Pnt2(),
	       Param,
	       Sol(1), Sol(2), Sol(3));
}

void BRepBlend_AppFuncRst::Vec(math_Vector& Sol,
			       const Blend_Point& Pnt)const
{
  Pnt.ParametersOnS(Sol(1),Sol(2));
  Sol(3) = Pnt.ParameterOnC();
}

