// Created on: 1998-06-30
// Created by: Christian CAILLET
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

#ifndef _StepRepr_RepresentationRelationshipWithTransformation_HeaderFile
#define _StepRepr_RepresentationRelationshipWithTransformation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_Transformation.hxx>
#include <StepRepr_ShapeRepresentationRelationship.hxx>
class TCollection_HAsciiString;
class StepRepr_Representation;

class StepRepr_RepresentationRelationshipWithTransformation
    : public StepRepr_ShapeRepresentationRelationship
{

public:
  Standard_EXPORT StepRepr_RepresentationRelationshipWithTransformation();

  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>& aName,
                            const occ::handle<TCollection_HAsciiString>& aDescription,
                            const occ::handle<StepRepr_Representation>&  aRep1,
                            const occ::handle<StepRepr_Representation>&  aRep2,
                            const StepRepr_Transformation&          aTransf);

  Standard_EXPORT StepRepr_Transformation TransformationOperator() const;

  Standard_EXPORT void SetTransformationOperator(const StepRepr_Transformation& aTrans);

  DEFINE_STANDARD_RTTIEXT(StepRepr_RepresentationRelationshipWithTransformation,
                          StepRepr_ShapeRepresentationRelationship)

private:
  StepRepr_Transformation theTrans;
};

#endif // _StepRepr_RepresentationRelationshipWithTransformation_HeaderFile
