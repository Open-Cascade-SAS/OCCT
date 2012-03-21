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


#include <StepAP214_AutoDesignGeneralOrgItem.ixx>
#include <Interface_Macros.hxx>

StepAP214_AutoDesignGeneralOrgItem::StepAP214_AutoDesignGeneralOrgItem () {  }

Standard_Integer StepAP214_AutoDesignGeneralOrgItem::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepBasic_Product))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinition))) return 2;
	if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionFormation))) return 3;
	if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionRelationship))) return 4;
	if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionWithAssociatedDocuments))) return 5;
	if (ent->IsKind(STANDARD_TYPE(StepRepr_Representation))) return 6;
	if (ent->IsKind(STANDARD_TYPE(StepRepr_ExternallyDefinedRepresentation))) return 7;
	if (ent->IsKind(STANDARD_TYPE(StepAP214_AutoDesignDocumentReference))) return 8;
	return 0;
}

Handle(StepBasic_Product) StepAP214_AutoDesignGeneralOrgItem::Product () const
{
	return GetCasted(StepBasic_Product,Value());
}

Handle(StepBasic_ProductDefinition) StepAP214_AutoDesignGeneralOrgItem::ProductDefinition () const
{
	return GetCasted(StepBasic_ProductDefinition,Value());
}

Handle(StepBasic_ProductDefinitionFormation) StepAP214_AutoDesignGeneralOrgItem::ProductDefinitionFormation () const
{
	return GetCasted(StepBasic_ProductDefinitionFormation,Value());
}

Handle(StepBasic_ProductDefinitionRelationship) StepAP214_AutoDesignGeneralOrgItem::ProductDefinitionRelationship () const
{
	return GetCasted(StepBasic_ProductDefinitionRelationship,Value());
}

Handle(StepBasic_ProductDefinitionWithAssociatedDocuments) StepAP214_AutoDesignGeneralOrgItem::ProductDefinitionWithAssociatedDocuments () const
{
	return GetCasted(StepBasic_ProductDefinitionWithAssociatedDocuments,Value());
}

Handle(StepRepr_Representation) StepAP214_AutoDesignGeneralOrgItem::Representation () const
{
	return GetCasted(StepRepr_Representation,Value());
}

Handle(StepRepr_ExternallyDefinedRepresentation) StepAP214_AutoDesignGeneralOrgItem::ExternallyDefinedRepresentation () const
{
	return GetCasted(StepRepr_ExternallyDefinedRepresentation,Value());
}


Handle(StepAP214_AutoDesignDocumentReference) StepAP214_AutoDesignGeneralOrgItem:: AutoDesignDocumentReference () const
{
	return GetCasted(StepAP214_AutoDesignDocumentReference,Value());
}
