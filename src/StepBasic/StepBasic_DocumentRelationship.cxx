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
#include <StepBasic_DocumentRelationship.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_DocumentRelationship,Standard_Transient)

StepBasic_DocumentRelationship::StepBasic_DocumentRelationship  ()    {  }

void  StepBasic_DocumentRelationship::Init
  (const Handle(TCollection_HAsciiString)& aName,
   const Handle(TCollection_HAsciiString)& aDescription,
   const Handle(StepBasic_Document)& aRelating,
   const Handle(StepBasic_Document)& aRelated)
{
  theName = aName;
  theDescription = aDescription;
  theRelating = aRelating;
  theRelated  = aRelated;
}

Handle(TCollection_HAsciiString)  StepBasic_DocumentRelationship::Name () const
{  return theName;  }

void  StepBasic_DocumentRelationship::SetName (const Handle(TCollection_HAsciiString)& aName)
{  theName = aName;  }

Handle(TCollection_HAsciiString)  StepBasic_DocumentRelationship::Description () const
{  return theDescription;  }

void  StepBasic_DocumentRelationship::SetDescription (const Handle(TCollection_HAsciiString)& aDescription)
{  theDescription = aDescription;  }

Handle(StepBasic_Document)  StepBasic_DocumentRelationship::RelatingDocument () const
{  return theRelating;  }

void  StepBasic_DocumentRelationship::SetRelatingDocument (const Handle(StepBasic_Document)& aRelating)
{  theRelating = aRelating;  }

Handle(StepBasic_Document)  StepBasic_DocumentRelationship::RelatedDocument () const
{  return theRelated;  }

void  StepBasic_DocumentRelationship::SetRelatedDocument (const Handle(StepBasic_Document)& aRelated)
{  theRelated = aRelated;  }
