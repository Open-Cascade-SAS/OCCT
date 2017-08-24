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

#include <inspector/VInspector_ItemEntityOwner.hxx>
#include <inspector/VInspector_ItemSensitiveEntity.hxx>
#include <inspector/VInspector_ItemPresentableObject.hxx>
#include <inspector/VInspector_Tools.hxx>

#include <SelectMgr_SensitiveEntity.hxx>
#include <SelectBasics_SensitiveEntity.hxx>
#include <Standard_Version.hxx>
#include <StdSelect_BRepOwner.hxx>

#include <QItemSelectionModel>
#include <QColor>

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemEntityOwner::initValue(int theItemRole) const
{
  switch (theItemRole)
  { 
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Qt::ToolTipRole:
    {
      Handle(SelectBasics_EntityOwner) anOwner = getEntityOwner();
      if (anOwner.IsNull())
        return QVariant();

      switch (Column())
      {
        case 0: return anOwner->DynamicType()->Name();
        case 2: return theItemRole == Qt::ToolTipRole ? "Owner pointer"
                                    : VInspector_Tools::GetPointerInfo (anOwner, true).ToCString();
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
        case 5:
        {
          if (theItemRole == Qt::ToolTipRole)
            return "TShape pointer";
          else
          {
            Handle(StdSelect_BRepOwner) BROwnr = Handle(StdSelect_BRepOwner)::DownCast (anOwner);
            if (!BROwnr.IsNull())
            {
              const TopoDS_Shape& aShape = BROwnr->Shape();
              if (!aShape.IsNull())
                return VInspector_Tools::GetPointerInfo (aShape.TShape(), true).ToCString();
            }
          }
          break;
        }
        case 8:
        {
          if (theItemRole == Qt::ToolTipRole)
            return "Shape Location : Shape Orientation";
          else
          {
            Handle(StdSelect_BRepOwner) aBROwnr = Handle(StdSelect_BRepOwner)::DownCast (anOwner);
            if (!aBROwnr.IsNull())
            {
              const TopoDS_Shape& aShape = aBROwnr->Shape();
              if (!aShape.IsNull())
                return  QString ("%1 : %2")
                        .arg (VInspector_Tools::LocationToName(aShape.Location()).ToCString())
                        .arg (VInspector_Tools::OrientationToName(aShape.Orientation()).ToCString());
              return VInspector_Tools::GetShapeTypeInfo (aShape.ShapeType()).ToCString();
            }
          }
          break;
        }
        default: break;
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
            return (theItemRole == Qt::BackgroundRole) ? QColor (Qt::darkBlue) : QColor (Qt::white);
        }
      }
      VInspector_ItemSensitiveEntityPtr aParentItem = itemDynamicCast<VInspector_ItemSensitiveEntity>(Parent());
      if (aParentItem)
        return aParentItem->data(QModelIndex(), theItemRole);
      break;
    }
  }
  return QVariant();
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void VInspector_ItemEntityOwner::Init()
{
  Handle(SelectBasics_EntityOwner) anOwner;

  VInspector_ItemSensitiveEntityPtr aParentItem = itemDynamicCast<VInspector_ItemSensitiveEntity>(Parent());
  if (aParentItem)
  {
    Handle(SelectMgr_SensitiveEntity) anEntity = aParentItem->GetSensitiveEntity();
    anOwner = anEntity->BaseSensitive()->OwnerId();
  }
  else
  {
    VInspector_ItemPresentableObjectPtr aPOItem = itemDynamicCast<VInspector_ItemPresentableObject>(Parent());
    if (aPOItem)
    {
      Handle(AIS_InteractiveObject) anIO = aPOItem->GetInteractiveObject();

      int aRowId = Row();
      int aCurrentIndex = 0;
      for (anIO->Init(); anIO->More() && anOwner.IsNull(); anIO->Next())
      {
        const Handle(SelectMgr_Selection)& aSelection = anIO->CurrentSelection();
        for (aSelection->Init(); aSelection->More() && anOwner.IsNull(); aSelection->Next())
        {
          Handle(SelectMgr_SensitiveEntity) anEntity = aSelection->Sensitive();
          const Handle(SelectBasics_SensitiveEntity)& aBase = anEntity->BaseSensitive();
          if (!aBase.IsNull())
          {
            if (aRowId == aCurrentIndex)
              anOwner = aBase->OwnerId();
            aCurrentIndex++;
          }
        }
      }
    }
  }
  myOwner = anOwner;
  TreeModel_ItemBase::Init();
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================
void VInspector_ItemEntityOwner::Reset()
{
  VInspector_ItemBase::Reset();
  SetContext (NULL);
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void VInspector_ItemEntityOwner::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemEntityOwner*>(this)->Init();
}

// =======================================================================
// function : getEntityOwner
// purpose :
// =======================================================================
Handle(SelectBasics_EntityOwner) VInspector_ItemEntityOwner::getEntityOwner() const
{
  initItem();
  return myOwner;
}
