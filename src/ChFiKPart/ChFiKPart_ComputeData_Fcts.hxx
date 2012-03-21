// Created on: 1995-05-22
// Created by: Laurent BOURESCHE
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



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
