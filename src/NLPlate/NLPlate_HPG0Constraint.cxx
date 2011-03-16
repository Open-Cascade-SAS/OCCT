// File:	NLPlate_HPG0Constraint.cxx
// Created:	Fri Apr 17 17:52:28 1998
// Author:	Andre LIEUTIER
//		<alr@sgi63>


#include <NLPlate_HPG0Constraint.ixx>

NLPlate_HPG0Constraint::NLPlate_HPG0Constraint(const gp_XY& UV,const gp_XYZ& Value)
:myXYZTarget(Value)
{
  SetUV(UV);
  SetActiveOrder(0);
  UVIsFree = Standard_False;
  IncrementalLoadingAllowed = Standard_False;
}
void NLPlate_HPG0Constraint::SetUVFreeSliding(const Standard_Boolean UVFree) 
{
  UVIsFree = UVFree;
}
void NLPlate_HPG0Constraint::SetIncrementalLoadAllowed(const Standard_Boolean ILA) 
{
  IncrementalLoadingAllowed = ILA;
}
Standard_Boolean NLPlate_HPG0Constraint::UVFreeSliding() const
{
  return UVIsFree;
}
Standard_Boolean NLPlate_HPG0Constraint::IncrementalLoadAllowed() const
{
  return IncrementalLoadingAllowed;
}
Standard_Integer NLPlate_HPG0Constraint::ActiveOrder() const
{
  if (myActiveOrder<0) return myActiveOrder;
  else return 0;
}
Standard_Boolean NLPlate_HPG0Constraint::IsG0() const
{
  return Standard_True;
}
const gp_XYZ& NLPlate_HPG0Constraint::G0Target() const
{
  return myXYZTarget;
}
