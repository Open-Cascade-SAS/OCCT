// File:	GCPnts_AbscissaPoint.cxx
// Created:	Fri May  5 09:47:41 1995
// Author:	Modelistation
//		<model@fuegox>


#include <GCPnts_AbscissaPoint.ixx>
#include <Standard_ConstructionError.hxx>

//=======================================================================
//function : GCPnts_AbscissaPoint
//purpose  : 
//=======================================================================

GCPnts_AbscissaPoint::GCPnts_AbscissaPoint() 
{
}

#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>

#define TheCurve                 Adaptor3d_Curve
#define Handle_TheBezierCurve   Handle(Geom_BezierCurve)
#define Handle_TheBSplineCurve  Handle(Geom_BSplineCurve)

#include <GCPnts_AbscissaPoint.gxx>

#undef TheCurve
#undef Handle_TheBezierCurve
#undef Handle_TheBSplineCurve

#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>

#define TheCurve                 Adaptor2d_Curve2d
#define Handle_TheBezierCurve   Handle(Geom2d_BezierCurve)
#define Handle_TheBSplineCurve  Handle(Geom2d_BSplineCurve)

#include <GCPnts_AbscissaPoint.gxx>

