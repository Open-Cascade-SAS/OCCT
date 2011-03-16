// File:	NLPlate_HPG3Constraint.cxx
// Created:	Fri Apr 17 19:30:33 1998
// Author:	Andre LIEUTIER
//		<alr@sgi63>


#include <NLPlate_HPG3Constraint.ixx>

NLPlate_HPG3Constraint::NLPlate_HPG3Constraint(const gp_XY& UV,const Plate_D1& D1T,const Plate_D2& D2T,const Plate_D3& D3T)
:NLPlate_HPG2Constraint(UV,D1T,D2T),myG3Target(D3T)
{
  SetActiveOrder(3);
}
Standard_Integer NLPlate_HPG3Constraint::ActiveOrder() const
{
  if (myActiveOrder<3) return myActiveOrder;
  else return 3;
}
const Plate_D3& NLPlate_HPG3Constraint::G3Target() const
{
  return myG3Target;
}
