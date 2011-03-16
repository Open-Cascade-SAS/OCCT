#include <IntSurf_PathPoint.ixx>


IntSurf_PathPoint::IntSurf_PathPoint ():
      ispass(Standard_True), istgt(Standard_True) 
{
}


IntSurf_PathPoint::IntSurf_PathPoint (const gp_Pnt& P,
				      const Standard_Real U,
				      const Standard_Real V):
       pt(P),ispass(Standard_True),istgt(Standard_True) 
{
  sequv = new TColgp_HSequenceOfXY ();
  sequv->Append(gp_XY(U,V));
}


void IntSurf_PathPoint::SetValue (const gp_Pnt& P,
				  const Standard_Real U,
				  const Standard_Real V) 
{
  pt = P;
  sequv = new TColgp_HSequenceOfXY ();
  sequv->Append(gp_XY(U,V));
}
  
