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

#include <StepRepr_ItemDefinedTransformation.ixx>

StepRepr_ItemDefinedTransformation::StepRepr_ItemDefinedTransformation ()    {  }

void  StepRepr_ItemDefinedTransformation::Init
(const Handle(TCollection_HAsciiString)& aName,
 const Handle(TCollection_HAsciiString)& aDescription,
 const Handle(StepRepr_RepresentationItem)& aTransformItem1,
 const Handle(StepRepr_RepresentationItem)& aTransformItem2)
{  theName = aName;  theDescription = aDescription;
  theTransformItem1 = aTransformItem1;   theTransformItem2 = aTransformItem2; }

void  StepRepr_ItemDefinedTransformation::SetName (const Handle(TCollection_HAsciiString)& aName)
{  theName = aName;  }

Handle(TCollection_HAsciiString)  StepRepr_ItemDefinedTransformation::Name () const
{  return theName;  }

void  StepRepr_ItemDefinedTransformation::SetDescription (const Handle(TCollection_HAsciiString)& aDescription)
{  theDescription = aDescription;  }

Handle(TCollection_HAsciiString)  StepRepr_ItemDefinedTransformation::Description () const
{  return theDescription;  }

void  StepRepr_ItemDefinedTransformation::SetTransformItem1 (const Handle(StepRepr_RepresentationItem)& aTransformItem1)
{  theTransformItem1 = aTransformItem1;  }

Handle(StepRepr_RepresentationItem)  StepRepr_ItemDefinedTransformation::TransformItem1 () const
{  return theTransformItem1;  }

void  StepRepr_ItemDefinedTransformation::SetTransformItem2 (const Handle(StepRepr_RepresentationItem)& aTransformItem2)
{  theTransformItem2 = aTransformItem2;  }

Handle(StepRepr_RepresentationItem)  StepRepr_ItemDefinedTransformation::TransformItem2 () const
{  return theTransformItem2;  }
