// Created on: 2016-04-26
// Created by: Irina KRYLOVA
// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef _StepRepr_ConstructiveGeometryRepresentation_HeaderFile
#define _StepRepr_ConstructiveGeometryRepresentation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_Representation.hxx>


class StepRepr_ConstructiveGeometryRepresentation;
DEFINE_STANDARD_HANDLE(StepRepr_ConstructiveGeometryRepresentation, StepRepr_Representation)


class StepRepr_ConstructiveGeometryRepresentation : public StepRepr_Representation
{
public:
  
  //! Returns a ConstructiveGeometryRepresentation
  Standard_EXPORT StepRepr_ConstructiveGeometryRepresentation();

  DEFINE_STANDARD_RTTIEXT(StepRepr_ConstructiveGeometryRepresentation,StepRepr_Representation)
};

#endif // _StepRepr_ConstructiveGeometryRepresentation_HeaderFile
