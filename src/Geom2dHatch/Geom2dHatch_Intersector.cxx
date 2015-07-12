// Created on: 1994-03-23
// Created by: Jean Marc LACHAUME
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <ElCLib.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dHatch_Intersector.hxx>
#include <Geom2dLProp_CLProps2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <Precision.hxx>

//=======================================================================
//function : Geom2dHatch_Intersector
//purpose  : 
//=======================================================================
Geom2dHatch_Intersector::Geom2dHatch_Intersector() :
myConfusionTolerance(0.0),
myTangencyTolerance(0.0)
{
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void  Geom2dHatch_Intersector::Perform(const gp_Lin2d& L, 
				       const Standard_Real P, 
				       const Standard_Real Tol, 
				       const Geom2dAdaptor_Curve& C)
{
  
//Standard_Real pfbid,plbid;
  IntRes2d_Domain DL;
  if(P!=RealLast()) 
    DL.SetValues(L.Location(),0.,Tol,ElCLib::Value(P,L),P,Tol);
  else 
    DL.SetValues(L.Location(),0.,Tol,Standard_True);
  
  IntRes2d_Domain DE(C.Value(C.FirstParameter()),
		     C.FirstParameter(),Precision::PIntersection(),
		     C.Value(C.LastParameter()),
		     C.LastParameter(),Precision::PIntersection());
  
  Handle(Geom2d_Line) GL= new Geom2d_Line(L);
  Geom2dAdaptor_Curve CGA(GL);
  void *ptrpoureviterlesproblemesdeconst = (void *)(&C);

  Geom2dInt_GInter Inter(CGA,
			 DL,
			 *((Geom2dAdaptor_Curve *)ptrpoureviterlesproblemesdeconst),
			 DE,
			 Precision::PConfusion(),
			 Precision::PIntersection());
  this->SetValues(Inter);
}

//=======================================================================
//function : LocalGeometry
//purpose  : 
//=======================================================================

void  Geom2dHatch_Intersector::LocalGeometry(const Geom2dAdaptor_Curve& E, 
					     const Standard_Real U, 
					     gp_Dir2d& Tang, 
					     gp_Dir2d& Norm, 
					     Standard_Real& C) const 
{
  //Standard_Real f,l;
  Geom2dLProp_CLProps2d Prop(E.Curve(),U,2,Precision::PConfusion());

  if(!Prop.IsTangentDefined()) return;

  Prop.Tangent(Tang);
  C = Prop.Curvature();
  if (C > Precision::PConfusion() && C<RealLast())
    Prop.Normal(Norm);
  else
    Norm.SetCoord(Tang.Y(),-Tang.X());
}






