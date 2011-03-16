// File:	NLPlate_HPG1Constraint.cxx
// Created:	Fri Apr 17 19:21:01 1998
// Author:	Andre LIEUTIER
//		<alr@sgi63>


#include <NLPlate_HPG1Constraint.ixx>
NLPlate_HPG1Constraint::NLPlate_HPG1Constraint(const gp_XY& UV,const Plate_D1& D1T)
:myG1Target(D1T)
{
  SetUV(UV);
  SetActiveOrder(1);
  IncrementalLoadingAllowed = Standard_False;
  myOrientation = 0;
}
void NLPlate_HPG1Constraint::SetIncrementalLoadAllowed(const Standard_Boolean ILA) 
{
  IncrementalLoadingAllowed = ILA;
}
void NLPlate_HPG1Constraint::SetOrientation(const Standard_Integer Orient) 
{
  myOrientation = Orient;
}

Standard_Boolean NLPlate_HPG1Constraint::IncrementalLoadAllowed() const
{
  return IncrementalLoadingAllowed;
}
Standard_Integer NLPlate_HPG1Constraint::ActiveOrder() const
{
  if (myActiveOrder<1) return myActiveOrder;
  else return 1;
}
Standard_Boolean NLPlate_HPG1Constraint::IsG0() const
{
  return Standard_False;
}
Standard_Integer NLPlate_HPG1Constraint::Orientation() 
{
  return myOrientation;
}
const Plate_D1& NLPlate_HPG1Constraint::G1Target() const
{
  return myG1Target;
}
