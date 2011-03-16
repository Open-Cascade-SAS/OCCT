// File:	Extrema_CurveTool.cxx
// Created:	Wed Jul 19 09:00:02 1995
// Author:	Modelistation
//		<model@metrox>


#include <Extrema_CurveTool.ixx>

//=======================================================================
//function : IsPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Extrema_CurveTool::IsPeriodic(const Adaptor3d_Curve& C)
{
  GeomAbs_CurveType aType = GetType(C);
  if (aType == GeomAbs_Circle ||
      aType == GeomAbs_Ellipse)
    return Standard_True;
  else
    return C.IsPeriodic();
}


