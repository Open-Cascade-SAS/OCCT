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
#include <NLPlate_HPG3Constraint.hxx>
#include <Plate_D1.hxx>
#include <Plate_D2.hxx>
#include <Plate_D3.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(NLPlate_HPG3Constraint,NLPlate_HPG2Constraint)

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
