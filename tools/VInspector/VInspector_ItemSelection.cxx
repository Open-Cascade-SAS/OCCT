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

#include <inspector/VInspector_ItemSelection.hxx>

#include <AIS_ListOfInteractive.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <SelectMgr_SensitiveEntity.hxx>
#include <Standard_Version.hxx>
#include <inspector/VInspector_ItemContext.hxx>
#include <inspector/VInspector_ItemPresentableObject.hxx>
#include <inspector/VInspector_ItemSensitiveEntity.hxx>
#include <inspector/VInspector_Tools.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QStringList>

#include <QColor>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : getSelection
// purpose :
// =======================================================================
Handle(SelectMgr_Selection) VInspector_ItemSelection::getSelection() const
{
  initItem();
  return mySelection;
}

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int VInspector_ItemSelection::initRowCount() const
{
  Handle(SelectMgr_Selection) aSelection = getSelection();
#if OCC_VERSION_HEX < 0x070201
  int aRows = 0;
  for (aSelection->Init(); aSelection->More(); aSelection->Next())
    aRows++;
  return aRows;
#else
  return aSelection->Entities().Size();
#endif
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemSelection::initValue (int theItemRole) const
{
  switch (theItemRole)
  {
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Qt::ToolTipRole:
    {
      switch (Column())
      {
        case 0: return getSelection()->DynamicType()->Name();
        case 1: return rowCount();
        case 3:
        {
          if (theItemRole == Qt::ToolTipRole)
            return "Mode";
          else
          {
            VInspector_ItemPresentableObjectPtr aParentItem = itemDynamicCast<VInspector_ItemPresentableObject>(Parent());
            return VInspector_Tools::SelectionModeToName(getSelection()->Mode(), aParentItem->GetInteractiveObject()).ToCString();
          }
        }
        case 4:
        {
          if (theItemRole == Qt::ToolTipRole)
            return "SelectMgr_StateOfSelection";
          else {
            int aNbSelected = 0;
            SelectMgr_StateOfSelection aState = getSelection()->GetSelectionState();
            if (aState == SelectMgr_SOS_Activated || aState == SelectMgr_SOS_Any)
            {
              Handle(AIS_InteractiveContext) aContext = GetContext();
#if OCC_VERSION_HEX < 0x070201
              for (mySelection->Init(); mySelection->More(); mySelection->Next())
              {
                const Handle(SelectBasics_EntityOwner)& anOwner = mySelection->Sensitive()->BaseSensitive()->OwnerId();
#else
              for (NCollection_Vector<Handle(SelectMgr_SensitiveEntity)>::Iterator aSelEntIter (mySelection->Entities()); aSelEntIter.More(); aSelEntIter.Next())
              {
                const Handle(SelectBasics_EntityOwner)& anOwner = aSelEntIter.Value()->BaseSensitive()->OwnerId();
#endif
                if (VInspector_Tools::IsOwnerSelected(aContext, anOwner))
                  aNbSelected++;
              }
            }
            return aNbSelected > 0 ? QString::number (aNbSelected) : "";
          }
        }
        case 9:
        {
          SelectMgr_StateOfSelection aState = getSelection()->GetSelectionState();
          return VInspector_Tools::ToName (VInspector_SelectionType_StateOfSelection, aState).ToCString();
        }
        case 10: return QString::number (getSelection()->Sensitivity());
        case 11:
          return VInspector_Tools::ToName (VInspector_SelectionType_TypeOfUpdate,
                                           getSelection()->UpdateStatus()).ToCString();
        case 12:
          return VInspector_Tools::ToName (VInspector_SelectionType_TypeOfBVHUpdate,
                                           getSelection()->BVHUpdateStatus()).ToCString();
        default:
          break;
      }
      break;
    }
    case Qt::ForegroundRole:
    {
      SelectMgr_StateOfSelection aState = getSelection()->GetSelectionState();
      return QVariant (aState == SelectMgr_SOS_Activated ? QColor (Qt::black) : QColor (Qt::darkGray));
    }
  }
  return QVariant();
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr VInspector_ItemSelection::createChild (int theRow, int theColumn)
{
  return VInspector_ItemSensitiveEntity::CreateItem (currentItem(), theRow, theColumn);
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void VInspector_ItemSelection::Init()
{
  VInspector_ItemPresentableObjectPtr aParentItem = itemDynamicCast<VInspector_ItemPresentableObject>(Parent());

  Handle(AIS_InteractiveObject) anIO = aParentItem->GetInteractiveObject();

  int aRowId = Row();
  int aCurrentId = 0;
#if OCC_VERSION_HEX < 0x070201
  for (anIO->Init(); anIO->More(); anIO->Next(), aCurrentId++)
#else
  for (SelectMgr_SequenceOfSelection::Iterator aSelIter (anIO->Selections()); aSelIter.More(); aSelIter.Next(), aCurrentId++)
#endif
  {
    if (aCurrentId != aRowId)
      continue;
#if OCC_VERSION_HEX < 0x070201
    mySelection = anIO->CurrentSelection();
#else
    mySelection = aSelIter.Value();
#endif
    break;
  }
  TreeModel_ItemBase::Init();
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================
void VInspector_ItemSelection::Reset()
{
  // an empty method to don't clear the main label, otherwise the model will be empty
  TreeModel_ItemBase::Reset();

  mySelection = NULL;
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void VInspector_ItemSelection::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemSelection*>(this)->Init();
  // an empty method to don't initialize the main label, as it was not cleared in Reset()
}
