// Created on: 1993-06-23
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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


#include <Geom2d_Curve.hxx>
#include <Standard_Type.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_SurfaceCurveInterference.hxx>
#include <TopOpeBRepDS_Transition.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TopOpeBRepDS_SurfaceCurveInterference,TopOpeBRepDS_Interference)

//=======================================================================
//function : TopOpeBRepDS_SurfaceCurveInterference
//purpose  : 
//=======================================================================

TopOpeBRepDS_SurfaceCurveInterference::TopOpeBRepDS_SurfaceCurveInterference()
{
}

//=======================================================================
//function : TopOpeBRepDS_SurfaceCurveInterference
//purpose  : 
//=======================================================================

TopOpeBRepDS_SurfaceCurveInterference::TopOpeBRepDS_SurfaceCurveInterference
  (const TopOpeBRepDS_Transition& T,
   const TopOpeBRepDS_Kind ST, 
   const Standard_Integer S, 
   const TopOpeBRepDS_Kind GT, 
   const Standard_Integer G, 
   const Handle(Geom2d_Curve)& PC) :
  TopOpeBRepDS_Interference(T,ST,S,GT,G),
  myPCurve(PC)
{
}

//=======================================================================
//function : TopOpeBRepDS_SurfaceCurveInterference
//purpose  : 
//=======================================================================

TopOpeBRepDS_SurfaceCurveInterference::TopOpeBRepDS_SurfaceCurveInterference
  (const Handle(TopOpeBRepDS_Interference)& I) :
  TopOpeBRepDS_Interference(I)
{
}


//=======================================================================
//function : PCurve
//purpose  : 
//=======================================================================

const Handle(Geom2d_Curve)& TopOpeBRepDS_SurfaceCurveInterference::PCurve
       ()const 
{
  return myPCurve;
}

//=======================================================================
//function : PCurve
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_SurfaceCurveInterference::PCurve
  (const Handle(Geom2d_Curve)& PC)
{
  myPCurve = PC;
}
