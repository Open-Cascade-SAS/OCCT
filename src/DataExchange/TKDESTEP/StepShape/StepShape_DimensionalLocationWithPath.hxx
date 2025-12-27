// Created on: 2000-04-18
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

#ifndef _StepShape_DimensionalLocationWithPath_HeaderFile
#define _StepShape_DimensionalLocationWithPath_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepShape_DimensionalLocation.hxx>
class StepRepr_ShapeAspect;
class TCollection_HAsciiString;

//! Representation of STEP entity DimensionalLocationWithPath
class StepShape_DimensionalLocationWithPath : public StepShape_DimensionalLocation
{

public:
  //! Empty constructor
  Standard_EXPORT StepShape_DimensionalLocationWithPath();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>& aShapeAspectRelationship_Name,
    const bool                  hasShapeAspectRelationship_Description,
    const occ::handle<TCollection_HAsciiString>& aShapeAspectRelationship_Description,
    const occ::handle<StepRepr_ShapeAspect>&     aShapeAspectRelationship_RelatingShapeAspect,
    const occ::handle<StepRepr_ShapeAspect>&     aShapeAspectRelationship_RelatedShapeAspect,
    const occ::handle<StepRepr_ShapeAspect>&     aPath);

  //! Returns field Path
  Standard_EXPORT occ::handle<StepRepr_ShapeAspect> Path() const;

  //! Set field Path
  Standard_EXPORT void SetPath(const occ::handle<StepRepr_ShapeAspect>& Path);

  DEFINE_STANDARD_RTTIEXT(StepShape_DimensionalLocationWithPath, StepShape_DimensionalLocation)

private:
  occ::handle<StepRepr_ShapeAspect> thePath;
};

#endif // _StepShape_DimensionalLocationWithPath_HeaderFile
