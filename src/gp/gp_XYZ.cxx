// File gp_XYZ.cxx, JCV 05/12/90

#include <gp_XYZ.ixx>

Standard_Boolean gp_XYZ::IsEqual (const gp_XYZ& Other,
				  const Standard_Real Tolerance) const {
  Standard_Real val;
  val = x - Other.x;
  if (val < 0) val = - val;
  if (val > Tolerance) return Standard_False;
  val = y - Other.y;
  if (val < 0) val = - val;
  if (val > Tolerance) return Standard_False;
  val = z - Other.z;
  if (val < 0) val = - val;
  if (val > Tolerance) return Standard_False;
  return Standard_True;
}

