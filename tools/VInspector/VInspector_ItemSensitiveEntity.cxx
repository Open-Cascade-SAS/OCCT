// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
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


#include <inspector/VInspector_ItemSensitiveEntity.hxx>

#include <AIS_ListOfInteractive.hxx>
#include <SelectBasics_SensitiveEntity.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <SelectMgr_SensitiveEntity.hxx>
#include <Standard_Version.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <TopoDS_Shape.hxx>
#include <inspector/VInspector_ItemContext.hxx>
#include <inspector/VInspector_ItemEntityOwner.hxx>
#include <inspector/VInspector_ItemSelection.hxx>
#include <inspector/VInspector_Tools.hxx>

#include <QStringList>
#include <QColor>

// =======================================================================
// function : GetSensitiveEntity
// purpose :
// =======================================================================
Handle(SelectMgr_SensitiveEntity) VInspector_ItemSensitiveEntity::GetSensitiveEntity() const
{
  initItem();
  return myEntity;
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemSensitiveEntity::initValue (int theItemRole) const
{
  Handle(SelectMgr_SensitiveEntity) aBase = GetSensitiveEntity();
  Handle(SelectBasics_EntityOwner) anOwner = aBase->BaseSensitive()->OwnerId();

  switch (theItemRole)
  {
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Qt::ToolTipRole:
    {
      switch (Column())
      {
        case 0:
          return myEntity->DynamicType()->Name();
        case 2:
        {
          return theItemRole == Qt::ToolTipRole ? "Owner pointer" : VInspector_Tools::GetPointerInfo (
                             GetSensitiveEntity()->BaseSensitive()->OwnerId(), true).ToCString();
        }
        case 3:
        {
          if (theItemRole == Qt::ToolTipRole)
            return "Owner Shape type";
          else
          {
            Handle(StdSelect_BRepOwner) BROwnr = Handle(StdSelect_BRepOwner)::DownCast (anOwner);
            if (!BROwnr.IsNull())
            {
              const TopoDS_Shape& aShape = BROwnr->Shape();
              if (!aShape.IsNull())
                return VInspector_Tools::GetShapeTypeInfo (aShape.ShapeType()).ToCString();
            }
          }
          break;
        }
        case 4: return theItemRole == Qt::ToolTipRole ? "IsActiveForSelection"
#if OCC_VERSION_HEX <= 0x060901
                           : ("none");
#else
                           : myEntity->IsActiveForSelection() ? QString ("true") : QString ("false");
#endif
        case 5: return theItemRole == Qt::ToolTipRole ? "SelectBasics_SensitiveEntity"
                                                      : getEntityOwner()->DynamicType()->Name();
        case 6: return theItemRole == Qt::ToolTipRole ? "SensitivityFactor"
                       : QString::number (GetSensitiveEntity()->BaseSensitive()->SensitivityFactor());
        case 7: return theItemRole == Qt::ToolTipRole ? "NbSubElements"
                           : QString::number (GetSensitiveEntity()->BaseSensitive()->NbSubElements());
        case 8:
        {
          if (theItemRole == Qt::ToolTipRole)
            return "Owner priority";
          else
          {
            Handle(StdSelect_BRepOwner) BROwnr = Handle(StdSelect_BRepOwner)::DownCast (anOwner);
            if (!BROwnr.IsNull())
              return anOwner->Priority();
          }
          break;
        }
        default:
          break;
      }
      break;
    }
    case Qt::BackgroundRole:
    case Qt::ForegroundRole:
    {
      if (Column() == 2)
      {
        Handle(AIS_InteractiveContext) aContext = GetContext();
        if (!aContext.IsNull())
        {
          if (VInspector_Tools::IsOwnerSelected(aContext, getEntityOwner()))
            return QVariant ((theItemRole == Qt::BackgroundRole) ? QColor (Qt::darkBlue) : QColor (Qt::white));
        }
      }
      VInspector_ItemSelectionPtr aParentItem = itemDynamicCast<VInspector_ItemSelection>(Parent());
      if (aParentItem)
        return aParentItem->data(QModelIndex(), theItemRole);
      break;
    }
    default:
    break;
  }
  return QVariant();
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr VInspector_ItemSensitiveEntity::createChild (int theRow, int theColumn)
{
  return VInspector_ItemEntityOwner::CreateItem (currentItem(), theRow, theColumn);
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void VInspector_ItemSensitiveEntity::Init()
{
  VInspector_ItemSelectionPtr aParentItem = itemDynamicCast<VInspector_ItemSelection>(Parent());

  Handle(SelectMgr_Selection) aSelection = aParentItem->getSelection();

  int aRowId = Row();
  int aCurrentId = 0;
  for (aSelection->Init(); aSelection->More(); aSelection->Next(), aCurrentId++)
  {
    if (aCurrentId != aRowId)
      continue;
    myEntity = aSelection->Sensitive();
    break;
  }
  TreeModel_ItemBase::Init();
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================
void VInspector_ItemSensitiveEntity::Reset()
{
  // an empty method to don't clear the main label, otherwise the model will be empty
  TreeModel_ItemBase::Reset();
  myEntity = NULL;
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void VInspector_ItemSensitiveEntity::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemSensitiveEntity*>(this)->Init();
}

// =======================================================================
// function : getEntityOwner
// purpose :
// =======================================================================
Handle(SelectBasics_EntityOwner) VInspector_ItemSensitiveEntity::getEntityOwner() const
{
  initItem();

  Handle(SelectBasics_EntityOwner) anOwner;
  const Handle(SelectBasics_SensitiveEntity)& aBase = myEntity->BaseSensitive();
  if (aBase.IsNull())
    return anOwner;
  return aBase->OwnerId();
}
