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
#include <gp_XYZ.hxx>
#include <NLPlate_HPG0Constraint.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(NLPlate_HPG0Constraint,NLPlate_HGPPConstraint)

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
