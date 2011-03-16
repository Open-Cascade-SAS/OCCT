// File:	NLPlate_HPG0G2Constraint.cxx
// Created:	Fri Apr 17 19:18:03 1998
// Author:	Andre LIEUTIER
//		<alr@sgi63>

#include <Plate_D1.hxx>
#include <Plate_D2.hxx>

#include <NLPlate_HPG0G2Constraint.ixx>

NLPlate_HPG0G2Constraint::NLPlate_HPG0G2Constraint(const gp_XY& UV, const gp_XYZ& Value, const Plate_D1& D1T, const Plate_D2& D2T)
: NLPlate_HPG0G1Constraint(UV,Value,D1T), myG2Target(D2T)
{
  SetActiveOrder(2);
}
Standard_Integer NLPlate_HPG0G2Constraint::ActiveOrder() const
{
  if (myActiveOrder<2) return myActiveOrder;
  else return 2;
}
const Plate_D2& NLPlate_HPG0G2Constraint::G2Target() const
{
  return myG2Target;
}
