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

#include <StepBasic_ProductDefinitionWithAssociatedDocuments.ixx>

StepBasic_ProductDefinitionWithAssociatedDocuments::StepBasic_ProductDefinitionWithAssociatedDocuments  ()    {  }

void  StepBasic_ProductDefinitionWithAssociatedDocuments::Init
  (const Handle(TCollection_HAsciiString)& aId,
   const Handle(TCollection_HAsciiString)& aDescription,
   const Handle(StepBasic_ProductDefinitionFormation)& aFormation,
   const Handle(StepBasic_ProductDefinitionContext)& aFrame,
   const Handle(StepBasic_HArray1OfDocument)& aDocIds)
{
  StepBasic_ProductDefinition::Init (aId,aDescription,aFormation,aFrame);
  theDocIds = aDocIds;
}

Handle(StepBasic_HArray1OfDocument)  StepBasic_ProductDefinitionWithAssociatedDocuments::DocIds () const
{  return theDocIds;  }

void  StepBasic_ProductDefinitionWithAssociatedDocuments::SetDocIds (const Handle(StepBasic_HArray1OfDocument)& aDocIds)
{  theDocIds = aDocIds;  }

Standard_Integer  StepBasic_ProductDefinitionWithAssociatedDocuments::NbDocIds () const
{  return (theDocIds.IsNull() ? 0 : theDocIds->Length());  }

Handle(StepBasic_Document)  StepBasic_ProductDefinitionWithAssociatedDocuments::DocIdsValue (const Standard_Integer num) const
{  return theDocIds->Value(num);  }

void  StepBasic_ProductDefinitionWithAssociatedDocuments::SetDocIdsValue (const Standard_Integer num, const Handle(StepBasic_Document)& adoc)
{  theDocIds->SetValue (num,adoc);  }
