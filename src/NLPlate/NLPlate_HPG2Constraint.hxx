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

#ifndef _NLPlate_HPG2Constraint_HeaderFile
#define _NLPlate_HPG2Constraint_HeaderFile

#include <Standard.hxx>

#include <Plate_D2.hxx>
#include <NLPlate_HPG1Constraint.hxx>
#include <Standard_Integer.hxx>
class gp_XY;
class Plate_D1;


class NLPlate_HPG2Constraint;
DEFINE_STANDARD_HANDLE(NLPlate_HPG2Constraint, NLPlate_HPG1Constraint)

//! define a PinPoint (no G0)  G2 Constraint used to load a Non
//! Linear Plate
class NLPlate_HPG2Constraint : public NLPlate_HPG1Constraint
{

public:

  
  Standard_EXPORT NLPlate_HPG2Constraint(const gp_XY& UV, const Plate_D1& D1T, const Plate_D2& D2T);
  
  Standard_EXPORT virtual Standard_Integer ActiveOrder() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual const Plate_D2& G2Target() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(NLPlate_HPG2Constraint,NLPlate_HPG1Constraint)

protected:




private:


  Plate_D2 myG2Target;


};







#endif // _NLPlate_HPG2Constraint_HeaderFile
