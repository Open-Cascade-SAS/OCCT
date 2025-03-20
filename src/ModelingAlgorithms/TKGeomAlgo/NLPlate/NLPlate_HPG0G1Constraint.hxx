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

#ifndef _NLPlate_HPG0G1Constraint_HeaderFile
#define _NLPlate_HPG0G1Constraint_HeaderFile

#include <Standard.hxx>

#include <Plate_D1.hxx>
#include <Standard_Integer.hxx>
#include <NLPlate_HPG0Constraint.hxx>
class gp_XY;
class gp_XYZ;

class NLPlate_HPG0G1Constraint;
DEFINE_STANDARD_HANDLE(NLPlate_HPG0G1Constraint, NLPlate_HPG0Constraint)

//! define a PinPoint G0+G1  Constraint  used to load a Non Linear
//! Plate
class NLPlate_HPG0G1Constraint : public NLPlate_HPG0Constraint
{

public:
  Standard_EXPORT NLPlate_HPG0G1Constraint(const gp_XY&    UV,
                                           const gp_XYZ&   Value,
                                           const Plate_D1& D1T);

  Standard_EXPORT virtual void SetOrientation(const Standard_Integer Orient = 0) Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_Integer ActiveOrder() const Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_Integer Orientation() Standard_OVERRIDE;

  Standard_EXPORT virtual const Plate_D1& G1Target() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(NLPlate_HPG0G1Constraint, NLPlate_HPG0Constraint)

protected:
private:
  Plate_D1         myG1Target;
  Standard_Integer myOrientation;
};

#endif // _NLPlate_HPG0G1Constraint_HeaderFile
