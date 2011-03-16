// File:	BRepOffset_Interval.cxx
// Created:	Fri Oct 20 18:01:32 1995
// Author:	Yves FRICAUD
//		<yfr@stylox>


#include <BRepOffset_Interval.ixx>

//=======================================================================
//function : BRepOffset_Interval
//purpose  : 
//=======================================================================

BRepOffset_Interval::BRepOffset_Interval()
{
}


//=======================================================================
//function : BRepOffset_Interval
//purpose  : 
//=======================================================================

BRepOffset_Interval::BRepOffset_Interval(const Standard_Real   U1, 
					 const Standard_Real   U2, 
					 const BRepOffset_Type Type):
f(U1),
l(U2),
type(Type)
{
}



