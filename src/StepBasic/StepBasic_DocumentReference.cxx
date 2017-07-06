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


#include <Standard_Type.hxx>
#include <StepBasic_Document.hxx>
#include <StepBasic_DocumentReference.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_DocumentReference,Standard_Transient)

void  StepBasic_DocumentReference::Init0
  (const Handle(StepBasic_Document)& aAssignedDocument,
   const Handle(TCollection_HAsciiString)& aSource)
{
  theAssignedDocument = aAssignedDocument;
  theSource = aSource;
}

Handle(StepBasic_Document)  StepBasic_DocumentReference::AssignedDocument () const
{  return theAssignedDocument;  }

void  StepBasic_DocumentReference::SetAssignedDocument (const Handle(StepBasic_Document)& aAssignedDocument)
{  theAssignedDocument = aAssignedDocument;  }

Handle(TCollection_HAsciiString)  StepBasic_DocumentReference::Source () const
{  return theSource;  }

void  StepBasic_DocumentReference::SetSource (const Handle(TCollection_HAsciiString)& aSource)
{  theSource = aSource;  }
