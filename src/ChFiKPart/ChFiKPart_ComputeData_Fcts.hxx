// File:	ChFiKPart_ComputeData_Fcts.hxx
// Created:	Mon May 22 13:52:09 1995
// Author:	Laurent BOURESCHE
//		<lbo@phylox>


#ifndef _ChFiKPart_ComputeData_Fcts_HeaderFile
#define _ChFiKPart_ComputeData_Fcts_HeaderFile

#include <gp_Pnt2d.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <TopOpeBRepDS_DataStructure.hxx>
		      
Standard_Real  ChFiKPart_InPeriod(const Standard_Real U, 
				  const Standard_Real UFirst, 
				  const Standard_Real ULast,
				  const Standard_Real Eps);

Handle(Geom2d_BSplineCurve) ChFiKPart_PCurve(const gp_Pnt2d& UV1,
					     const gp_Pnt2d& UV2,
					     const Standard_Real Pardeb,
					     const Standard_Real Parfin);

void ChFiKPart_ProjPC(const GeomAdaptor_Curve& Cg, 
		      const GeomAdaptor_Surface& Sg, 
		      Handle(Geom2d_Curve)& Pcurv);
					     
Standard_EXPORT Standard_Integer ChFiKPart_IndexCurveInDS(const Handle(Geom_Curve)& C,
							  TopOpeBRepDS_DataStructure& DStr); 
					  

Standard_EXPORT Standard_Integer ChFiKPart_IndexSurfaceInDS(const Handle(Geom_Surface)& S,
							    TopOpeBRepDS_DataStructure& DStr); 


#endif
