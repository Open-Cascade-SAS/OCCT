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

#ifndef _StepShape_DimensionalSize_HeaderFile
#define _StepShape_DimensionalSize_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class StepRepr_ShapeAspect;
class TCollection_HAsciiString;

//! Representation of STEP entity DimensionalSize
class StepShape_DimensionalSize : public Standard_Transient
{

public:
  //! Empty constructor
  Standard_EXPORT StepShape_DimensionalSize();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const occ::handle<StepRepr_ShapeAspect>&     aAppliesTo,
                            const occ::handle<TCollection_HAsciiString>& aName);

  //! Returns field AppliesTo
  Standard_EXPORT occ::handle<StepRepr_ShapeAspect> AppliesTo() const;

  //! Set field AppliesTo
  Standard_EXPORT void SetAppliesTo(const occ::handle<StepRepr_ShapeAspect>& AppliesTo);

  //! Returns field Name
  Standard_EXPORT occ::handle<TCollection_HAsciiString> Name() const;

  //! Set field Name
  Standard_EXPORT void SetName(const occ::handle<TCollection_HAsciiString>& Name);

  DEFINE_STANDARD_RTTIEXT(StepShape_DimensionalSize, Standard_Transient)

private:
  occ::handle<StepRepr_ShapeAspect>     theAppliesTo;
  occ::handle<TCollection_HAsciiString> theName;
};

#endif // _StepShape_DimensionalSize_HeaderFile
