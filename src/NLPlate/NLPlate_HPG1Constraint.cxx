// Created on: 1998-04-17
// Created by: Andre LIEUTIER
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <gp_XY.hxx>
#include <NLPlate_HPG1Constraint.hxx>
#include <Plate_D1.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(NLPlate_HPG1Constraint,NLPlate_HGPPConstraint)

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
