// File:	ChFiKPart_ComputeData_CS.hxx
// Created:	Tue Jun 28 10:52:20 1994
// Author:	Laurent BOURESCHE
//		<lbo@phylox>

#include <Adaptor3d_HSurface.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Circ.hxx>

#ifndef ChFiKPart_ComputeData_CS_HeaderFile
#define ChFiKPart_ComputeData_CS_HeaderFile


void ChFiKPart_CornerSpine(const Handle(Adaptor3d_HSurface)& S1, 
			   const Handle(Adaptor3d_HSurface)& S2,
			   const gp_Pnt2d& P1S1,
			   const gp_Pnt2d& P2S1,
			   const gp_Pnt2d& P1S2,
			   const gp_Pnt2d& P2S2,
			   const Standard_Real R,
			   gp_Cylinder& cyl,
			   gp_Circ& circ,
			   Standard_Real& First,
			   Standard_Real& Last);


#endif
