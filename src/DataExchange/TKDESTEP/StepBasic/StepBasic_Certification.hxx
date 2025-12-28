// Created on: 1999-11-26
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
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

#ifndef _StepBasic_Certification_HeaderFile
#define _StepBasic_Certification_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class TCollection_HAsciiString;
class StepBasic_CertificationType;

//! Representation of STEP entity Certification
class StepBasic_Certification : public Standard_Transient
{

public:
  //! Empty constructor
  Standard_EXPORT StepBasic_Certification();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>&    aName,
                            const occ::handle<TCollection_HAsciiString>&    aPurpose,
                            const occ::handle<StepBasic_CertificationType>& aKind);

  //! Returns field Name
  Standard_EXPORT occ::handle<TCollection_HAsciiString> Name() const;

  //! Set field Name
  Standard_EXPORT void SetName(const occ::handle<TCollection_HAsciiString>& Name);

  //! Returns field Purpose
  Standard_EXPORT occ::handle<TCollection_HAsciiString> Purpose() const;

  //! Set field Purpose
  Standard_EXPORT void SetPurpose(const occ::handle<TCollection_HAsciiString>& Purpose);

  //! Returns field Kind
  Standard_EXPORT occ::handle<StepBasic_CertificationType> Kind() const;

  //! Set field Kind
  Standard_EXPORT void SetKind(const occ::handle<StepBasic_CertificationType>& Kind);

  DEFINE_STANDARD_RTTIEXT(StepBasic_Certification, Standard_Transient)

private:
  occ::handle<TCollection_HAsciiString>    theName;
  occ::handle<TCollection_HAsciiString>    thePurpose;
  occ::handle<StepBasic_CertificationType> theKind;
};

#endif // _StepBasic_Certification_HeaderFile
