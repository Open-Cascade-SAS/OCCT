// File gp_XY.cxx, JCV 08/01/91

#include <gp_XY.ixx>

Standard_Boolean gp_XY::IsEqual (const gp_XY& Other,
				 const Standard_Real Tolerance) const
{
  Standard_Real val;
  val = x - Other.x;
  if (val < 0) val = - val;
  if (val > Tolerance) return Standard_False;
  val = y - Other.y;
  if (val < 0) val = - val;
  if (val > Tolerance) return Standard_False;
  return Standard_True;
}

