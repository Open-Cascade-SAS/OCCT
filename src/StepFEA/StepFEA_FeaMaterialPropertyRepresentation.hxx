// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _StepFEA_FeaMaterialPropertyRepresentation_HeaderFile
#define _StepFEA_FeaMaterialPropertyRepresentation_HeaderFile

#include <Standard.hxx>

#include <StepRepr_MaterialPropertyRepresentation.hxx>


class StepFEA_FeaMaterialPropertyRepresentation;
DEFINE_STANDARD_HANDLE(StepFEA_FeaMaterialPropertyRepresentation, StepRepr_MaterialPropertyRepresentation)

//! Representation of STEP entity FeaMaterialPropertyRepresentation
class StepFEA_FeaMaterialPropertyRepresentation : public StepRepr_MaterialPropertyRepresentation
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepFEA_FeaMaterialPropertyRepresentation();




  DEFINE_STANDARD_RTTIEXT(StepFEA_FeaMaterialPropertyRepresentation,StepRepr_MaterialPropertyRepresentation)

protected:




private:




};







#endif // _StepFEA_FeaMaterialPropertyRepresentation_HeaderFile
