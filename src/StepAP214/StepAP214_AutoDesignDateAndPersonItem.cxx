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


#include <Interface_Macros.hxx>
#include <Standard_Transient.hxx>
#include <StepAP214_AutoDesignDateAndPersonItem.hxx>
#include <StepAP214_AutoDesignDocumentReference.hxx>
#include <StepAP214_AutoDesignOrganizationAssignment.hxx>
#include <StepBasic_Product.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <StepBasic_ProductDefinitionFormation.hxx>
#include <StepBasic_ProductDefinitionRelationship.hxx>
#include <StepBasic_ProductDefinitionWithAssociatedDocuments.hxx>
#include <StepRepr_ExternallyDefinedRepresentation.hxx>
#include <StepRepr_Representation.hxx>

StepAP214_AutoDesignDateAndPersonItem::StepAP214_AutoDesignDateAndPersonItem () {  }

Standard_Integer StepAP214_AutoDesignDateAndPersonItem::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepAP214_AutoDesignOrganizationAssignment))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepBasic_Product))) return 2;
	if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinition))) return 3;
	if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionFormation))) return 4;
	if (ent->IsKind(STANDARD_TYPE(StepRepr_Representation))) return 5;
	if (ent->IsKind(STANDARD_TYPE(StepAP214_AutoDesignDocumentReference))) return 6;
	if (ent->IsKind(STANDARD_TYPE(StepRepr_ExternallyDefinedRepresentation))) return 7;
	if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionRelationship))) return 8;
	if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionWithAssociatedDocuments))) return 9;
	return 0;
}

Handle(StepAP214_AutoDesignOrganizationAssignment)  StepAP214_AutoDesignDateAndPersonItem::AutoDesignOrganizationAssignment () const
{  return GetCasted(StepAP214_AutoDesignOrganizationAssignment,Value());  }

Handle(StepBasic_Product)  StepAP214_AutoDesignDateAndPersonItem::Product () const
{  return GetCasted(StepBasic_Product,Value());  }

Handle(StepBasic_ProductDefinition)  StepAP214_AutoDesignDateAndPersonItem::ProductDefinition () const
{  return GetCasted(StepBasic_ProductDefinition,Value());  }

Handle(StepBasic_ProductDefinitionFormation)  StepAP214_AutoDesignDateAndPersonItem::ProductDefinitionFormation () const
{  return GetCasted(StepBasic_ProductDefinitionFormation,Value());  }

Handle(StepRepr_Representation)  StepAP214_AutoDesignDateAndPersonItem::Representation () const
{  return GetCasted(StepRepr_Representation,Value());  }

Handle(StepAP214_AutoDesignDocumentReference)  StepAP214_AutoDesignDateAndPersonItem::AutoDesignDocumentReference () const
{  return GetCasted(StepAP214_AutoDesignDocumentReference,Value());  }

Handle(StepRepr_ExternallyDefinedRepresentation)  StepAP214_AutoDesignDateAndPersonItem::ExternallyDefinedRepresentation () const
{  return GetCasted(StepRepr_ExternallyDefinedRepresentation,Value());  }

Handle(StepBasic_ProductDefinitionRelationship)  StepAP214_AutoDesignDateAndPersonItem::ProductDefinitionRelationship () const
{  return GetCasted(StepBasic_ProductDefinitionRelationship,Value());  }

Handle(StepBasic_ProductDefinitionWithAssociatedDocuments)  StepAP214_AutoDesignDateAndPersonItem::ProductDefinitionWithAssociatedDocuments () const
{  return GetCasted(StepBasic_ProductDefinitionWithAssociatedDocuments,Value());  }
