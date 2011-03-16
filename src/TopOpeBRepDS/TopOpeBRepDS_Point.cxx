// File:	TopOpeBRepDS_Point.cxx
// Created:	Wed Jun 23 19:08:33 1993
// Author:	Jean Yves LEBEY
//		<jyl@zerox>

#include <TopOpeBRepDS_Point.ixx>
#include <TopOpeBRepTool_ShapeTool.hxx>

//=======================================================================
//function : TopOpeBRepDS_Point
//purpose  : 
//=======================================================================

TopOpeBRepDS_Point::TopOpeBRepDS_Point()
: myKeep(Standard_True)
{
}


//=======================================================================
//function : TopOpeBRepDS_Point
//purpose  : 
//=======================================================================

TopOpeBRepDS_Point::TopOpeBRepDS_Point(const gp_Pnt& P, 
				       const Standard_Real T)
: myPoint(P),
  myTolerance(T),
  myKeep(Standard_True)
{
}


//=======================================================================
//function : TopOpeBRepDS_Point
//purpose  : 
//=======================================================================

TopOpeBRepDS_Point::TopOpeBRepDS_Point(const TopoDS_Shape& S)
{
  myPoint     = TopOpeBRepTool_ShapeTool::Pnt(S);
  myTolerance = TopOpeBRepTool_ShapeTool::Tolerance(S);
}


//=======================================================================
//function : IsEqual
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_Point::IsEqual(const TopOpeBRepDS_Point& P) const 
{
  Standard_Real    t = Max(myTolerance,P.Tolerance());
  Standard_Boolean b = myPoint.IsEqual(P.Point(),t);
  return b;
}


//=======================================================================
//function : Point
//purpose  : 
//=======================================================================

const gp_Pnt& TopOpeBRepDS_Point::Point()const 
{
  return myPoint;
}

//=======================================================================
//function : ChangePoint
//purpose  : 
//=======================================================================

gp_Pnt& TopOpeBRepDS_Point::ChangePoint() 
{
  return myPoint;
}


//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================

Standard_Real  TopOpeBRepDS_Point::Tolerance()const 
{
  return myTolerance;
}

//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================

void TopOpeBRepDS_Point::Tolerance(const Standard_Real Tol)
{
  myTolerance = Tol;
}

//=======================================================================
//function : Keep
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_Point::Keep() const
{
  return myKeep;
}
//=======================================================================
//function : ChangeKeep
//purpose  : 
//=======================================================================

void TopOpeBRepDS_Point::ChangeKeep(const Standard_Boolean b)
{
  myKeep = b;
}
