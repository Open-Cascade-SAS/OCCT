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

#ifndef _StepBasic_DocumentReference_HeaderFile
#define _StepBasic_DocumentReference_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class StepBasic_Document;
class TCollection_HAsciiString;

class StepBasic_DocumentReference;
DEFINE_STANDARD_HANDLE(StepBasic_DocumentReference, Standard_Transient)

class StepBasic_DocumentReference : public Standard_Transient
{

public:
  Standard_EXPORT void Init0(const Handle(StepBasic_Document)&       aAssignedDocument,
                             const Handle(TCollection_HAsciiString)& aSource);

  Standard_EXPORT Handle(StepBasic_Document) AssignedDocument() const;

  Standard_EXPORT void SetAssignedDocument(const Handle(StepBasic_Document)& aAssignedDocument);

  Standard_EXPORT Handle(TCollection_HAsciiString) Source() const;

  Standard_EXPORT void SetSource(const Handle(TCollection_HAsciiString)& aSource);

  DEFINE_STANDARD_RTTIEXT(StepBasic_DocumentReference, Standard_Transient)

protected:
private:
  Handle(StepBasic_Document)       theAssignedDocument;
  Handle(TCollection_HAsciiString) theSource;
};

#endif // _StepBasic_DocumentReference_HeaderFile
