// Created on: 1994-10-25
// Created by: Jean Yves LEBEY
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


#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRep_FaceEdgeIntersector.hxx>
#include <TopOpeBRep_Point2d.hxx>
#include <TopOpeBRep_PointGeomTool.hxx>
#include <TopOpeBRep_VPointInter.hxx>
#include <TopOpeBRepDS_Point.hxx>

//=======================================================================
//function : MakePoint
//purpose  : 
//=======================================================================
TopOpeBRepDS_Point TopOpeBRep_PointGeomTool::MakePoint
(const TopOpeBRep_VPointInter& IP)
{
  Standard_Real tolip = IP.Tolerance();
  Standard_Real tolout = tolip;
  Standard_Integer si = IP.ShapeIndex();
  if ( si == 0 ) {
    tolout = tolip;
  } else if ( si == 1 ) {
    const TopoDS_Edge& E = TopoDS::Edge(IP.Edge(si));
    tolout = BRep_Tool::Tolerance(E);      
  } else if ( si == 2 ) {
    const TopoDS_Edge& E = TopoDS::Edge(IP.Edge(si));
    tolout = BRep_Tool::Tolerance(E);      
  } else if ( si == 3 ) {
    const TopoDS_Edge& E1 = TopoDS::Edge(IP.Edge(1));
    const TopoDS_Edge& E2 = TopoDS::Edge(IP.Edge(2));
    Standard_Real t1 = BRep_Tool::Tolerance(E1);
    Standard_Real t2 = BRep_Tool::Tolerance(E2);
    //    tolout = Max(t1,t2);
    if (t1 > 0.9) t1 = 0.9;
    if (t1 > 0.9) t1 = 0.9;
    tolout = 2.5*(t1+t2);
  }
  tolout = Max(tolout,tolip);
  return TopOpeBRepDS_Point(IP.Value(),tolout);
}

//=======================================================================
//function : MakePoint
//purpose  : 
//=======================================================================
TopOpeBRepDS_Point TopOpeBRep_PointGeomTool::MakePoint
(const TopOpeBRep_Point2d& P2D)
{
  Standard_Real tol = P2D.Tolerance();
  return TopOpeBRepDS_Point(P2D.Value(),tol);
}

//=======================================================================
//function : MakePoint
//purpose  : 
//=======================================================================
TopOpeBRepDS_Point TopOpeBRep_PointGeomTool::MakePoint
(const TopOpeBRep_FaceEdgeIntersector& FEI)
{
  Standard_Real tol = FEI.Tolerance();
  return TopOpeBRepDS_Point(FEI.Value(),tol);
}

//=======================================================================
//function : MakePoint
//purpose  : 
//=======================================================================
TopOpeBRepDS_Point TopOpeBRep_PointGeomTool::MakePoint
(const TopoDS_Shape& S)
{
  return TopOpeBRepDS_Point(S);
}

//=======================================================================
//function : IsEqual
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRep_PointGeomTool::IsEqual
(const TopOpeBRepDS_Point& DSP1, 
 const TopOpeBRepDS_Point& DSP2)
{
  Standard_Boolean b = DSP1.IsEqual(DSP2);
  return b;
}
