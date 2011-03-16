#include <StepAP214_AutoDesignReferencingItem.ixx>
#include <Interface_Macros.hxx>

StepAP214_AutoDesignReferencingItem::StepAP214_AutoDesignReferencingItem  ()    {  }

Standard_Integer  StepAP214_AutoDesignReferencingItem::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;

  if (ent->IsKind(STANDARD_TYPE(StepBasic_Approval))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_DocumentRelationship))) return 2;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ExternallyDefinedRepresentation))) return 3;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_MappedItem))) return 4;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_MaterialDesignation))) return 5;
  if (ent->IsKind(STANDARD_TYPE(StepVisual_PresentationArea))) return 6;
  if (ent->IsKind(STANDARD_TYPE(StepVisual_PresentationView))) return 7;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductCategory))) return 8;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinition))) return 9;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionRelationship))) return 10;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_PropertyDefinition))) return 11;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_Representation))) return 12;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_RepresentationRelationship))) return 13;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ShapeAspect))) return 14;
  return 0;
}


Handle(StepBasic_Approval)  StepAP214_AutoDesignReferencingItem::Approval () const
{  return GetCasted(StepBasic_Approval,Value());  }

Handle(StepBasic_DocumentRelationship)  StepAP214_AutoDesignReferencingItem::DocumentRelationship () const
{  return GetCasted(StepBasic_DocumentRelationship,Value());  }

Handle(StepRepr_ExternallyDefinedRepresentation)  StepAP214_AutoDesignReferencingItem::ExternallyDefinedRepresentation () const
{  return GetCasted(StepRepr_ExternallyDefinedRepresentation,Value());  }

Handle(StepRepr_MappedItem)  StepAP214_AutoDesignReferencingItem::MappedItem () const
{  return GetCasted(StepRepr_MappedItem,Value());  }

Handle(StepRepr_MaterialDesignation)  StepAP214_AutoDesignReferencingItem::MaterialDesignation () const
{  return GetCasted(StepRepr_MaterialDesignation,Value());  }

Handle(StepVisual_PresentationArea)  StepAP214_AutoDesignReferencingItem::PresentationArea () const
{  return GetCasted(StepVisual_PresentationArea,Value());  }

Handle(StepVisual_PresentationView)  StepAP214_AutoDesignReferencingItem::PresentationView () const
{  return GetCasted(StepVisual_PresentationView,Value());  }

Handle(StepBasic_ProductCategory)  StepAP214_AutoDesignReferencingItem::ProductCategory () const
{  return GetCasted(StepBasic_ProductCategory,Value());  }

Handle(StepBasic_ProductDefinition)  StepAP214_AutoDesignReferencingItem::ProductDefinition () const
{  return GetCasted(StepBasic_ProductDefinition,Value());  }

Handle(StepBasic_ProductDefinitionRelationship)  StepAP214_AutoDesignReferencingItem::ProductDefinitionRelationship () const
{  return GetCasted(StepBasic_ProductDefinitionRelationship,Value());  }

Handle(StepRepr_PropertyDefinition)  StepAP214_AutoDesignReferencingItem::PropertyDefinition () const
{  return GetCasted(StepRepr_PropertyDefinition,Value());  }

Handle(StepRepr_Representation)  StepAP214_AutoDesignReferencingItem::Representation () const
{  return GetCasted(StepRepr_Representation,Value());  }

Handle(StepRepr_RepresentationRelationship)  StepAP214_AutoDesignReferencingItem::RepresentationRelationship () const
{  return GetCasted(StepRepr_RepresentationRelationship,Value());  }

Handle(StepRepr_ShapeAspect)  StepAP214_AutoDesignReferencingItem::ShapeAspect () const
{  return GetCasted(StepRepr_ShapeAspect,Value());  }
