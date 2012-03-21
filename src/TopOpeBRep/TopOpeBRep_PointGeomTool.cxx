// Created on: 1994-10-25
// Created by: Jean Yves LEBEY
// Copyright (c) 1994-1999 Matra Datavision
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


#include <TopOpeBRep_PointGeomTool.ixx>
#include <TopOpeBRep_Point2d.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <BRep_Tool.hxx>

//=======================================================================
//function : TopOpeBRep_PointGeomTool
//purpose  : 
//=======================================================================
TopOpeBRep_PointGeomTool::TopOpeBRep_PointGeomTool()
{
}

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
