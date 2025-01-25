// Created on: 2000-05-10
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

#ifndef _StepBasic_DocumentRepresentationType_HeaderFile
#define _StepBasic_DocumentRepresentationType_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class TCollection_HAsciiString;
class StepBasic_Document;

class StepBasic_DocumentRepresentationType;
DEFINE_STANDARD_HANDLE(StepBasic_DocumentRepresentationType, Standard_Transient)

//! Representation of STEP entity DocumentRepresentationType
class StepBasic_DocumentRepresentationType : public Standard_Transient
{

public:
  //! Empty constructor
  Standard_EXPORT StepBasic_DocumentRepresentationType();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& aName,
                            const Handle(StepBasic_Document)&       aRepresentedDocument);

  //! Returns field Name
  Standard_EXPORT Handle(TCollection_HAsciiString) Name() const;

  //! Set field Name
  Standard_EXPORT void SetName(const Handle(TCollection_HAsciiString)& Name);

  //! Returns field RepresentedDocument
  Standard_EXPORT Handle(StepBasic_Document) RepresentedDocument() const;

  //! Set field RepresentedDocument
  Standard_EXPORT void SetRepresentedDocument(
    const Handle(StepBasic_Document)& RepresentedDocument);

  DEFINE_STANDARD_RTTIEXT(StepBasic_DocumentRepresentationType, Standard_Transient)

protected:
private:
  Handle(TCollection_HAsciiString) theName;
  Handle(StepBasic_Document)       theRepresentedDocument;
};

#endif // _StepBasic_DocumentRepresentationType_HeaderFile
