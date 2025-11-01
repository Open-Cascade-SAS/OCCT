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

#ifndef _NLPlate_HPG3Constraint_HeaderFile
#define _NLPlate_HPG3Constraint_HeaderFile

#include <Standard.hxx>

#include <Plate_D3.hxx>
#include <NLPlate_HPG2Constraint.hxx>
#include <Standard_Integer.hxx>
class gp_XY;
class Plate_D1;
class Plate_D2;

class NLPlate_HPG3Constraint;
DEFINE_STANDARD_HANDLE(NLPlate_HPG3Constraint, NLPlate_HPG2Constraint)

//! define a PinPoint (no G0)  G3 Constraint used to load a Non
//! Linear Plate
class NLPlate_HPG3Constraint : public NLPlate_HPG2Constraint
{

public:
  Standard_EXPORT NLPlate_HPG3Constraint(const gp_XY&    UV,
                                         const Plate_D1& D1T,
                                         const Plate_D2& D2T,
                                         const Plate_D3& D3T);

  Standard_EXPORT virtual Standard_Integer ActiveOrder() const Standard_OVERRIDE;

  Standard_EXPORT virtual const Plate_D3& G3Target() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(NLPlate_HPG3Constraint, NLPlate_HPG2Constraint)

protected:
private:
  Plate_D3 myG3Target;
};

#endif // _NLPlate_HPG3Constraint_HeaderFile
