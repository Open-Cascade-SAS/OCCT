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

#ifndef _StepRepr_ShapeAspectRelationship_HeaderFile
#define _StepRepr_ShapeAspectRelationship_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class TCollection_HAsciiString;
class StepRepr_ShapeAspect;

//! Representation of STEP entity ShapeAspectRelationship
class StepRepr_ShapeAspectRelationship : public Standard_Transient
{

public:
  //! Empty constructor
  Standard_EXPORT StepRepr_ShapeAspectRelationship();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>& aName,
                            const bool                  hasDescription,
                            const occ::handle<TCollection_HAsciiString>& aDescription,
                            const occ::handle<StepRepr_ShapeAspect>&     aRelatingShapeAspect,
                            const occ::handle<StepRepr_ShapeAspect>&     aRelatedShapeAspect);

  //! Returns field Name
  Standard_EXPORT occ::handle<TCollection_HAsciiString> Name() const;

  //! Set field Name
  Standard_EXPORT void SetName(const occ::handle<TCollection_HAsciiString>& Name);

  //! Returns field Description
  Standard_EXPORT occ::handle<TCollection_HAsciiString> Description() const;

  //! Set field Description
  Standard_EXPORT void SetDescription(const occ::handle<TCollection_HAsciiString>& Description);

  //! Returns True if optional field Description is defined
  Standard_EXPORT bool HasDescription() const;

  //! Returns field RelatingShapeAspect
  Standard_EXPORT occ::handle<StepRepr_ShapeAspect> RelatingShapeAspect() const;

  //! Set field RelatingShapeAspect
  Standard_EXPORT void SetRelatingShapeAspect(
    const occ::handle<StepRepr_ShapeAspect>& RelatingShapeAspect);

  //! Returns field RelatedShapeAspect
  Standard_EXPORT occ::handle<StepRepr_ShapeAspect> RelatedShapeAspect() const;

  //! Set field RelatedShapeAspect
  Standard_EXPORT void SetRelatedShapeAspect(
    const occ::handle<StepRepr_ShapeAspect>& RelatedShapeAspect);

  DEFINE_STANDARD_RTTIEXT(StepRepr_ShapeAspectRelationship, Standard_Transient)

private:
  occ::handle<TCollection_HAsciiString> theName;
  occ::handle<TCollection_HAsciiString> theDescription;
  occ::handle<StepRepr_ShapeAspect>     theRelatingShapeAspect;
  occ::handle<StepRepr_ShapeAspect>     theRelatedShapeAspect;
  bool                 defDescription;
};

#endif // _StepRepr_ShapeAspectRelationship_HeaderFile
