// Created on: 2000-07-06
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _StepShape_DefinitionalRepresentationAndShapeRepresentation_HeaderFile
#define _StepShape_DefinitionalRepresentationAndShapeRepresentation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_DefinitionalRepresentation.hxx>


class StepShape_DefinitionalRepresentationAndShapeRepresentation;
DEFINE_STANDARD_HANDLE(StepShape_DefinitionalRepresentationAndShapeRepresentation, StepRepr_DefinitionalRepresentation)

//! Implements complex type
//! (DEFINITIONAL_REPRESENTATION,REPRESENTATION,SHAPE_REPRESENTATION)
class StepShape_DefinitionalRepresentationAndShapeRepresentation : public StepRepr_DefinitionalRepresentation
{

public:

  
  Standard_EXPORT StepShape_DefinitionalRepresentationAndShapeRepresentation();




  DEFINE_STANDARD_RTTIEXT(StepShape_DefinitionalRepresentationAndShapeRepresentation,StepRepr_DefinitionalRepresentation)

protected:




private:




};







#endif // _StepShape_DefinitionalRepresentationAndShapeRepresentation_HeaderFile
