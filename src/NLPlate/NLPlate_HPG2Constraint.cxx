// File:	NLPlate_HPG2Constraint.cxx
// Created:	Fri Apr 17 19:28:03 1998
// Author:	Andre LIEUTIER
//		<alr@sgi63>


#include <NLPlate_HPG2Constraint.ixx>

NLPlate_HPG2Constraint::NLPlate_HPG2Constraint(const gp_XY& UV,const Plate_D1& D1T,const Plate_D2& D2T)
:NLPlate_HPG1Constraint(UV,D1T),myG2Target(D2T)
{
  SetActiveOrder(2);
}
Standard_Integer NLPlate_HPG2Constraint::ActiveOrder() const
{
  if (myActiveOrder<2) return myActiveOrder;
  else return 2;
}
const Plate_D2& NLPlate_HPG2Constraint::G2Target() const
{
  return myG2Target;
}
