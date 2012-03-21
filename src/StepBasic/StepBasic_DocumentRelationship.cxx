// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <StepBasic_DocumentRelationship.ixx>

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
