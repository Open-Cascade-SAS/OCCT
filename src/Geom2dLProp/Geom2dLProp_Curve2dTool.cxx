// File:	Geom2dLProp_Curve2dTool.cxx
// Created:	Tue Aug 18 15:40:26 1992
// Author:	Herve LEGRAND
//		<hl@bravox>

#include <Geom2dLProp_Curve2dTool.ixx>
#include <Geom2d_Curve.hxx>
#include <GeomAbs_Shape.hxx>

void  Geom2dLProp_Curve2dTool::Value(const Handle_Geom2d_Curve& C, 
	    const Standard_Real U, gp_Pnt2d& P)
{
  P = C->Value(U);
}

void  Geom2dLProp_Curve2dTool::D1(const Handle_Geom2d_Curve& C, 
	 const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1)
{
  C->D1(U, P, V1);
}

void  Geom2dLProp_Curve2dTool::D2(const Handle_Geom2d_Curve& C, 
	 const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2)
{
  C->D2(U, P, V1, V2);
}

void  Geom2dLProp_Curve2dTool::D3(const Handle_Geom2d_Curve& C, 
	 const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2, gp_Vec2d& V3)
{
  C->D3(U, P, V1, V2, V3);
}

Standard_Integer  Geom2dLProp_Curve2dTool::Continuity(const Handle_Geom2d_Curve& C)
{
  GeomAbs_Shape s = C->Continuity();
  switch (s) {
  case GeomAbs_C0:
    return 0;
  case GeomAbs_C1:
    return 1;
  case GeomAbs_C2:
    return 2;
  case GeomAbs_C3:
    return 3;
  case GeomAbs_G1:
    return 0;
  case GeomAbs_G2:
    return 0;
  case GeomAbs_CN:
    return 3;
  };
  return 0;
}

Standard_Real  Geom2dLProp_Curve2dTool::FirstParameter(const Handle_Geom2d_Curve& C)
{
  return C->FirstParameter();
}

Standard_Real  Geom2dLProp_Curve2dTool::LastParameter(const Handle_Geom2d_Curve& C)
{
  return C->LastParameter();
}




