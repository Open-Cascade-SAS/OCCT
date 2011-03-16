// File:	NLPlate_HPG0G3Constraint.cxx
// Created:	Fri Apr 17 19:19:39 1998
// Author:	Andre LIEUTIER
//		<alr@sgi63>

#include <Plate_D1.hxx>
#include <Plate_D2.hxx>
#include <Plate_D3.hxx>

#include <NLPlate_HPG0G3Constraint.ixx>

NLPlate_HPG0G3Constraint::NLPlate_HPG0G3Constraint(const gp_XY& UV,const gp_XYZ& Value,const Plate_D1& D1T,const Plate_D2& D2T,const Plate_D3& D3T)
: NLPlate_HPG0G2Constraint(UV,Value,D1T,D2T), myG3Target(D3T)
{
  SetActiveOrder(3);
}
Standard_Integer NLPlate_HPG0G3Constraint::ActiveOrder() const
{
  if (myActiveOrder<3) return myActiveOrder;
  else return 3;
}
const Plate_D3& NLPlate_HPG0G3Constraint::G3Target() const
{
  return myG3Target;
}
