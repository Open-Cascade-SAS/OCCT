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

#ifndef _StepRepr_ConstructiveGeometryRepresentationRelationship_HeaderFile
#define _StepRepr_ConstructiveGeometryRepresentationRelationship_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_RepresentationRelationship.hxx>

class StepRepr_ConstructiveGeometryRepresentationRelationship;
DEFINE_STANDARD_HANDLE(StepRepr_ConstructiveGeometryRepresentationRelationship,
                       StepRepr_RepresentationRelationship)

class StepRepr_ConstructiveGeometryRepresentationRelationship
    : public StepRepr_RepresentationRelationship
{
public:
  Standard_EXPORT StepRepr_ConstructiveGeometryRepresentationRelationship();

  DEFINE_STANDARD_RTTIEXT(StepRepr_ConstructiveGeometryRepresentationRelationship,
                          StepRepr_RepresentationRelationship)
};
#endif // _StepRepr_ConstructiveGeometryRepresentationRelationship_HeaderFile
