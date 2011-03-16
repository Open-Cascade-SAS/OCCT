#include <Blend_AppFunction.ixx>


//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

Standard_Real Blend_AppFunction::Parameter(const Blend_Point& P) const
{
  return P.Parameter();
}



