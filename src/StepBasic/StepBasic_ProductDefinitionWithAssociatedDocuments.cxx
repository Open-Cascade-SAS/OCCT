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
#include <StepBasic_ProductDefinitionContext.hxx>
#include <StepBasic_ProductDefinitionFormation.hxx>
#include <StepBasic_ProductDefinitionWithAssociatedDocuments.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_ProductDefinitionWithAssociatedDocuments,StepBasic_ProductDefinition)

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
