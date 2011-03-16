#include <STEPConstruct_PointHasher.ixx>

//=======================================================================
//function : IsEqual
//purpose  : 
//=======================================================================
Standard_Boolean STEPConstruct_PointHasher::IsEqual(const gp_Pnt& point1, 
                                                    const gp_Pnt& point2)
{
  if(Abs(point1.X()-point2.X()) > Epsilon(point1.X())) return Standard_False;
  if(Abs(point1.Y()-point2.Y()) > Epsilon(point1.Y())) return Standard_False;
  if(Abs(point1.Z()-point2.Z()) > Epsilon(point1.Z())) return Standard_False;
  return Standard_True;
}
