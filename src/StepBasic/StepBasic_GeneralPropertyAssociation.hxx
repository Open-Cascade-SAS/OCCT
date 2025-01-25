// Copyright (c) 2024 OPEN CASCADE SAS
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

#ifndef _StepBasic_GeneralPropertyAssociation_HeaderFile
#define _StepBasic_GeneralPropertyAssociation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>

class TCollection_HAsciiString;
class StepBasic_GeneralProperty;
class StepRepr_PropertyDefinition;

DEFINE_STANDARD_HANDLE(StepBasic_GeneralPropertyAssociation, Standard_Transient)

//! Representation of STEP entity GeneralPropertyAssociation
class StepBasic_GeneralPropertyAssociation : public Standard_Transient
{

public:
  //! Empty constructor
  Standard_EXPORT StepBasic_GeneralPropertyAssociation();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)&    aName,
                            const Handle(TCollection_HAsciiString)&    aDescription,
                            const Handle(StepBasic_GeneralProperty)&   aGeneralProperty,
                            const Handle(StepRepr_PropertyDefinition)& aPropertyDefinition);

  //! Returns field Name
  Standard_EXPORT Handle(TCollection_HAsciiString) Name() const;

  //! Set field Name
  Standard_EXPORT void SetName(const Handle(TCollection_HAsciiString)& Name);

  //! Returns field Description
  Standard_EXPORT Handle(TCollection_HAsciiString) Description() const;

  //! Set field Description
  Standard_EXPORT void SetDescription(const Handle(TCollection_HAsciiString)& Description);

  //! Returns field GeneralProperty
  Standard_EXPORT Handle(StepBasic_GeneralProperty) GeneralProperty() const;

  //! Set field GeneralProperty
  Standard_EXPORT void SetGeneralProperty(const Handle(StepBasic_GeneralProperty)& GeneralProperty);

  //! Returns field PropertyDefinition
  Standard_EXPORT Handle(StepRepr_PropertyDefinition) PropertyDefinition() const;

  //! Set field PropertyDefinition
  Standard_EXPORT void SetPropertyDefinition(
    const Handle(StepRepr_PropertyDefinition)& PropertyDefinition);

  DEFINE_STANDARD_RTTIEXT(StepBasic_GeneralPropertyAssociation, Standard_Transient)

protected:
private:
  Handle(TCollection_HAsciiString)    theName;
  Handle(TCollection_HAsciiString)    theDescription;
  Handle(StepBasic_GeneralProperty)   theGeneralProperty;
  Handle(StepRepr_PropertyDefinition) thePropertyDefinition;
};

#endif // _StepBasic_GeneralPropertyAssociation_HeaderFile
