// Created on: 1998-04-17
// Created by: Andre LIEUTIER
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



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
