// File:	BRepBlend_AppFuncRstRst.cxx
// Created:	Wed May 13 09:50:59 1998
// Author:	Philippe NOUAILLE
//		<pne@cleox.paris1.matra-dtv.fr>


#include <BRepBlend_AppFuncRstRst.ixx>

BRepBlend_AppFuncRstRst::BRepBlend_AppFuncRstRst (Handle(BRepBlend_Line)& Line,
						  Blend_RstRstFunction& Func,
						  const Standard_Real Tol3d,
						  const Standard_Real Tol2d)
:BRepBlend_AppFuncRoot(Line,Func,Tol3d,Tol2d)
{
}

void BRepBlend_AppFuncRstRst::Point(const Blend_AppFunction& Func,
				    const Standard_Real Param,
				    const math_Vector& Sol,
				    Blend_Point& Pnt)const
{
  Pnt.SetValue(Func.Pnt1(), Func.Pnt2(),
	       Param,
	       Sol(1), Sol(2));
}

void BRepBlend_AppFuncRstRst::Vec(math_Vector& Sol,
				  const Blend_Point& Pnt)const
{
  Sol(1) = Pnt.ParameterOnC1();
  Sol(2) = Pnt.ParameterOnC2();
}

