// File:	GeomFill_DegeneratedBound.cxx
// Created:	Tue Dec  5 17:52:55 1995
// Author:	Laurent BOURESCHE
//		<lbo@phylox>


#include <GeomFill_DegeneratedBound.ixx>

//=======================================================================
//function : GeomFill_DegeneratedBound
//purpose  : 
//=======================================================================

GeomFill_DegeneratedBound::GeomFill_DegeneratedBound
(const gp_Pnt& Point, 
 const Standard_Real First, 
 const Standard_Real Last, 
 const Standard_Real Tol3d, 
 const Standard_Real Tolang) :
 GeomFill_Boundary(Tol3d,Tolang),
 myPoint(Point),myFirst(First),myLast(Last)
{
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

//gp_Pnt GeomFill_DegeneratedBound::Value(const Standard_Real U) const 
gp_Pnt GeomFill_DegeneratedBound::Value(const Standard_Real ) const 
{
  return myPoint;
}


//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

//void GeomFill_DegeneratedBound::D1(const Standard_Real U, 
void GeomFill_DegeneratedBound::D1(const Standard_Real , 
				   gp_Pnt& P, 
				   gp_Vec& V) const 
{
  P = myPoint;
  V.SetCoord(0.,0.,0.);
}


//=======================================================================
//function : Reparametrize
//purpose  : 
//=======================================================================

void GeomFill_DegeneratedBound::Reparametrize(const Standard_Real First, 
					      const Standard_Real Last, 
					      const Standard_Boolean , 
					      const Standard_Boolean , 
					      const Standard_Real , 
					      const Standard_Real , 
					      const Standard_Boolean )
{
  myFirst = First;
  myLast  = Last;
}


//=======================================================================
//function : Bounds
//purpose  : 
//=======================================================================

void GeomFill_DegeneratedBound::Bounds(Standard_Real& First, 
				       Standard_Real& Last) const 
{
  First = myFirst;
  Last  = myLast;
}


//=======================================================================
//function : IsDegenerated
//purpose  : 
//=======================================================================

Standard_Boolean GeomFill_DegeneratedBound::IsDegenerated() const 
{
  return Standard_True;
}
