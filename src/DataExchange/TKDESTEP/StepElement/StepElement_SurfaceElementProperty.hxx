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

#ifndef _StepElement_SurfaceElementProperty_HeaderFile
#define _StepElement_SurfaceElementProperty_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class TCollection_HAsciiString;
class StepElement_SurfaceSectionField;

class StepElement_SurfaceElementProperty;
DEFINE_STANDARD_HANDLE(StepElement_SurfaceElementProperty, Standard_Transient)

//! Representation of STEP entity SurfaceElementProperty
class StepElement_SurfaceElementProperty : public Standard_Transient
{

public:
  //! Empty constructor
  Standard_EXPORT StepElement_SurfaceElementProperty();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)&        aPropertyId,
                            const Handle(TCollection_HAsciiString)&        aDescription,
                            const Handle(StepElement_SurfaceSectionField)& aSection);

  //! Returns field PropertyId
  Standard_EXPORT Handle(TCollection_HAsciiString) PropertyId() const;

  //! Set field PropertyId
  Standard_EXPORT void SetPropertyId(const Handle(TCollection_HAsciiString)& PropertyId);

  //! Returns field Description
  Standard_EXPORT Handle(TCollection_HAsciiString) Description() const;

  //! Set field Description
  Standard_EXPORT void SetDescription(const Handle(TCollection_HAsciiString)& Description);

  //! Returns field Section
  Standard_EXPORT Handle(StepElement_SurfaceSectionField) Section() const;

  //! Set field Section
  Standard_EXPORT void SetSection(const Handle(StepElement_SurfaceSectionField)& Section);

  DEFINE_STANDARD_RTTIEXT(StepElement_SurfaceElementProperty, Standard_Transient)

protected:
private:
  Handle(TCollection_HAsciiString)        thePropertyId;
  Handle(TCollection_HAsciiString)        theDescription;
  Handle(StepElement_SurfaceSectionField) theSection;
};

#endif // _StepElement_SurfaceElementProperty_HeaderFile
