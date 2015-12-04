// Created on: 2001-12-28
// Created by: Andrey BETENEV
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#ifndef _StepShape_ConnectedFaceShapeRepresentation_HeaderFile
#define _StepShape_ConnectedFaceShapeRepresentation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_Representation.hxx>


class StepShape_ConnectedFaceShapeRepresentation;
DEFINE_STANDARD_HANDLE(StepShape_ConnectedFaceShapeRepresentation, StepRepr_Representation)

//! Representation of STEP entity ConnectedFaceShapeRepresentation
class StepShape_ConnectedFaceShapeRepresentation : public StepRepr_Representation
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepShape_ConnectedFaceShapeRepresentation();




  DEFINE_STANDARD_RTTIEXT(StepShape_ConnectedFaceShapeRepresentation,StepRepr_Representation)

protected:




private:




};







#endif // _StepShape_ConnectedFaceShapeRepresentation_HeaderFile
