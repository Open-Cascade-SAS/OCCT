// File:	GeomLProp_CurveTool.cxx
// Created:	Tue Aug 18 15:40:26 1992
// Author:	Herve LEGRAND
//		<hl@bravox>

#include <GeomLProp_CurveTool.ixx>
#include <Geom_Curve.hxx>
#include <GeomAbs_Shape.hxx>

void  GeomLProp_CurveTool::Value(const Handle_Geom_Curve& C, 
	    const Standard_Real U, gp_Pnt& P)
{
  P = C->Value(U);
}

void  GeomLProp_CurveTool::D1(const Handle_Geom_Curve& C, 
	 const Standard_Real U, gp_Pnt& P, gp_Vec& V1)
{
  C->D1(U, P, V1);
}

void  GeomLProp_CurveTool::D2(const Handle_Geom_Curve& C, 
	 const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2)
{
  C->D2(U, P, V1, V2);
}

void  GeomLProp_CurveTool::D3(const Handle_Geom_Curve& C, 
	 const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2, gp_Vec& V3)
{
  C->D3(U, P, V1, V2, V3);
}

Standard_Integer  GeomLProp_CurveTool::Continuity(const Handle_Geom_Curve& C)
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

Standard_Real  GeomLProp_CurveTool::FirstParameter(const Handle_Geom_Curve& C)
{
  return C->FirstParameter();
}

Standard_Real  GeomLProp_CurveTool::LastParameter(const Handle_Geom_Curve& C)
{
  return C->LastParameter();
}



