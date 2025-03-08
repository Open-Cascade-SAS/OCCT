// Created on: 2000-05-11
// Created by: data exchange team
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

#ifndef _StepBasic_Document_HeaderFile
#define _StepBasic_Document_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Transient.hxx>
class TCollection_HAsciiString;
class StepBasic_DocumentType;

class StepBasic_Document;
DEFINE_STANDARD_HANDLE(StepBasic_Document, Standard_Transient)

//! Representation of STEP entity Document
class StepBasic_Document : public Standard_Transient
{

public:
  //! Empty constructor
  Standard_EXPORT StepBasic_Document();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& aId,
                            const Handle(TCollection_HAsciiString)& aName,
                            const Standard_Boolean                  hasDescription,
                            const Handle(TCollection_HAsciiString)& aDescription,
                            const Handle(StepBasic_DocumentType)&   aKind);

  //! Returns field Id
  Standard_EXPORT Handle(TCollection_HAsciiString) Id() const;

  //! Set field Id
  Standard_EXPORT void SetId(const Handle(TCollection_HAsciiString)& Id);

  //! Returns field Name
  Standard_EXPORT Handle(TCollection_HAsciiString) Name() const;

  //! Set field Name
  Standard_EXPORT void SetName(const Handle(TCollection_HAsciiString)& Name);

  //! Returns field Description
  Standard_EXPORT Handle(TCollection_HAsciiString) Description() const;

  //! Set field Description
  Standard_EXPORT void SetDescription(const Handle(TCollection_HAsciiString)& Description);

  //! Returns True if optional field Description is defined
  Standard_EXPORT Standard_Boolean HasDescription() const;

  //! Returns field Kind
  Standard_EXPORT Handle(StepBasic_DocumentType) Kind() const;

  //! Set field Kind
  Standard_EXPORT void SetKind(const Handle(StepBasic_DocumentType)& Kind);

  DEFINE_STANDARD_RTTIEXT(StepBasic_Document, Standard_Transient)

protected:
private:
  Handle(TCollection_HAsciiString) theId;
  Handle(TCollection_HAsciiString) theName;
  Handle(TCollection_HAsciiString) theDescription;
  Handle(StepBasic_DocumentType)   theKind;
  Standard_Boolean                 defDescription;
};

#endif // _StepBasic_Document_HeaderFile
