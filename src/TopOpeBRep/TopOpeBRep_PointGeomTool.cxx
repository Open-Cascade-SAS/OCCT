// File:	TopOpeBRep_PointGeomTool.cxx
// Created:	Tue Oct 25 18:27:21 1994
// Author:	Jean Yves LEBEY
//		<jyl@bravox>

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
