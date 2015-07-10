// File:	Functions.hxx
// Created:	Tue Oct 07 09:23:55 1997
// Author:	Stephane ROUTELOUS
//		<s-routelous@muenchen.matra-dtv.fr>


#include "StdAfx.h"

#include <TopoDS_Compound.hxx>
#include <BRep_Builder.hxx>
#include <BRepAlgoAPI_Common.hxx>

#include <TColgp_SequenceOfPnt.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard.hxx>
#include <Precision.hxx>
#include <Standard_Boolean.hxx>
#include <Geom_BSplineSurface.hxx>
#include <TColStd_HArray2OfReal.hxx>
#include "GeomAPI_PointsToBSplineSurface.hxx"
#include <stdio.h>
#include <TColStd_HArray2OfReal.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <Geom_CartesianPoint.hxx>
#include <AIS_Point.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <Geom_BSplineCurve.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <TopoDS_Solid.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepTools.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <AIS_InteractiveContext.hxx>


Standard_Boolean grid2surf(CString ShapeName, Handle(Geom_BSplineSurface)& S);
Handle(Geom_BSplineCurve) SetTransfo(Handle(Geom_BSplineCurve) aBSCurve);
Handle(Geom_BSplineCurve) BuildBSplineCurve(Standard_Boolean DisplayPoints = Standard_True);
Handle(TColStd_HArray2OfReal) ReadRegularGrid(CString FileName,
					      Standard_Real& xmin,
					      Standard_Real& ymin,
					      Standard_Real& Deltax,
					      Standard_Real& Deltay);
