// File:	NLPlate_HPG0G1Constraint.cxx
// Created:	Fri Apr 17 18:58:06 1998
// Author:	Andre LIEUTIER
//		<alr@sgi63>

#include <Plate_D1.hxx>

#include <NLPlate_HPG0G1Constraint.ixx>

NLPlate_HPG0G1Constraint::NLPlate_HPG0G1Constraint(const gp_XY& UV,const gp_XYZ& Value,const Plate_D1& D1T)
: NLPlate_HPG0Constraint(UV,Value),myG1Target(D1T)
{
  SetActiveOrder(1);
  myOrientation = 0;
}

void NLPlate_HPG0G1Constraint::SetOrientation(const Standard_Integer Orient) 
{
  myOrientation = Orient;
}

Standard_Integer NLPlate_HPG0G1Constraint::ActiveOrder() const
{
  if (myActiveOrder<1) return myActiveOrder;
  else return 1;
}

Standard_Integer NLPlate_HPG0G1Constraint::Orientation() 
{
  return myOrientation;
}

const Plate_D1& NLPlate_HPG0G1Constraint::G1Target() const
{
  return myG1Target;
}
