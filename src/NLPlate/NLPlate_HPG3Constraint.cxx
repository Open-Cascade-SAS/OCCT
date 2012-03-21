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
