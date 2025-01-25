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

#include <inspector/TreeModel_ModelBase.hxx>

#include <inspector/TreeModel_ItemBase.hxx>
#include <inspector/TreeModel_ItemProperties.hxx>
#include <inspector/TreeModel_Tools.hxx>
#include <inspector/TreeModel_VisibilityState.hxx>

#include <Standard_Transient.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QIcon>
#include <Standard_WarningsRestore.hxx>

const int COLUMN_NAME_WIDTH = 260;
const int COLUMN_SIZE_WIDTH = 30;

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
TreeModel_ModelBase::TreeModel_ModelBase(QObject* theParent)
    : QAbstractItemModel(theParent),
      m_pUseVisibilityColumn(false),
      myVisibilityState(0)
{
  myVisibleIcon   = QIcon(":/icons/item_visible.png");
  myInvisibleIcon = QIcon(":/icons/item_invisible.png");
}

// =======================================================================
// function :  InitColumns
// purpose :
// =======================================================================
void TreeModel_ModelBase::InitColumns()
{
  setHeaderItem(TreeModel_ColumnType_Name, TreeModel_HeaderSection("Name", COLUMN_NAME_WIDTH));
  setHeaderItem(
    TreeModel_ColumnType_Visibility,
    TreeModel_HeaderSection("Visibility", TreeModel_ModelBase::ColumnVisibilityWidth()));
  setHeaderItem(TreeModel_ColumnType_Row, TreeModel_HeaderSection("Row", COLUMN_SIZE_WIDTH));
}

// =======================================================================
// function :  GetItemByIndex
// purpose :
// =======================================================================
TreeModel_ItemBasePtr TreeModel_ModelBase::GetItemByIndex(const QModelIndex& theIndex)
{
  TreeModel_ItemBase* anItem = (TreeModel_ItemBase*)theIndex.internalPointer();
  return TreeModel_ItemBasePtr(anItem);
}

// =======================================================================
// function :  reset
// purpose :
// =======================================================================
void TreeModel_ModelBase::Reset()
{
  for (int aColId = 0, aNbColumns = columnCount(); aColId < aNbColumns; aColId++)
  {
    TreeModel_ItemBasePtr aRootItem = RootItem(aColId);
    if (aRootItem)
      aRootItem->Reset();
  }
}

// =======================================================================
// function :  index
// purpose :
// =======================================================================
QModelIndex TreeModel_ModelBase::index(int                theRow,
                                       int                theColumn,
                                       const QModelIndex& theParent) const
{
  if (!hasIndex(theRow, theColumn, theParent))
    return QModelIndex();

  // to create index on the root item
  if (!theParent.isValid())
    return createIndex(theRow, theColumn, getIndexValue(RootItem(theColumn)));

  TreeModel_ItemBasePtr aParentItem;
  if (!theParent.isValid())
    aParentItem = RootItem(theColumn);
  else
    aParentItem = GetItemByIndex(theParent);

  TreeModel_ItemBasePtr aChildItem = aParentItem->Child(theRow, theColumn);
  return aChildItem ? createIndex(theRow, theColumn, getIndexValue(aChildItem)) : QModelIndex();
}

// =======================================================================
// function :  data
// purpose :
// =======================================================================
QVariant TreeModel_ModelBase::data(const QModelIndex& theIndex, int theRole) const
{
  if (!theIndex.isValid())
    return QVariant("undefined");

  if (IsUseVisibilityColumn() && theIndex.column() == TreeModel_ColumnType_Visibility)
  {
    if (theRole != Qt::DecorationRole)
      return QVariant();

    TreeModel_ItemBasePtr anItem = GetItemByIndex(theIndex);
    if (!anItem->data(theIndex, theRole).isNull()) // value is already in cache
      return anItem->data(theIndex, theRole);

    if (!anItem->IsInitialized())
      anItem->Init();

    if (!myVisibilityState || !myVisibilityState->CanBeVisible(theIndex))
      return QVariant();

    QVariant aValue = myVisibilityState->IsVisible(theIndex) ? myVisibleIcon : myInvisibleIcon;
    anItem->SetCustomData(aValue, theRole);
    return aValue;
  }

  TreeModel_ItemBasePtr anItem     = GetItemByIndex(theIndex);
  QVariant              anItemData = anItem->data(theIndex, theRole);

  if (anItemData.isNull() && theRole == Qt::BackgroundRole
      && myHighlightedIndices.contains(theIndex))
    anItemData = TreeModel_Tools::LightHighlightColor();

  return anItemData;
}

// =======================================================================
// function :  parent
// purpose :
// =======================================================================
QModelIndex TreeModel_ModelBase::parent(const QModelIndex& theIndex) const
{
  if (!theIndex.isValid())
    return QModelIndex();

  TreeModel_ItemBasePtr aChildItem  = GetItemByIndex(theIndex);
  TreeModel_ItemBasePtr aParentItem = aChildItem ? aChildItem->Parent() : TreeModel_ItemBasePtr();

  if (!aParentItem)
    return QModelIndex();

  return createIndex(aParentItem->Row(), aParentItem->Column(), getIndexValue(aParentItem));
}

// =======================================================================
// function :  flags
// purpose :
// =======================================================================
Qt::ItemFlags TreeModel_ModelBase::flags(const QModelIndex& theIndex) const
{
  if (!theIndex.isValid())
  {
    return Qt::ItemFlags();
  }
  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

// =======================================================================
// function : headerData
// purpose :
// =======================================================================
QVariant TreeModel_ModelBase::headerData(int             theSection,
                                         Qt::Orientation theOrientation,
                                         int             theRole) const
{
  if (theOrientation != Qt::Horizontal || theRole != Qt::DisplayRole)
    return QVariant();

  if (IsUseVisibilityColumn() && theSection == TreeModel_ColumnType_Visibility)
    return QVariant();

  return myHeaderValues[theSection].GetName();
}

// =======================================================================
// function :  rowCount
// purpose :
// =======================================================================
int TreeModel_ModelBase::rowCount(const QModelIndex& theParent) const
{
  // to create index on the root item
  if (!theParent.isValid())
    return 1;

  TreeModel_ItemBasePtr aParentItem;
  if (!theParent.isValid())
    aParentItem = RootItem(0);
  else
    aParentItem = GetItemByIndex(theParent);

  if (!aParentItem)
    return 0;

  return aParentItem ? aParentItem->rowCount() : 0;
}

// =======================================================================
// function : EmitLayoutChanged
// purpose :
// =======================================================================
void TreeModel_ModelBase::EmitLayoutChanged()
{
  emit layoutChanged();
}

// =======================================================================
// function : EmitLayoutChanged
// purpose :
// =======================================================================
void TreeModel_ModelBase::EmitDataChanged(const QModelIndex&  theTopLeft,
                                          const QModelIndex&  theBottomRight,
                                          const QVector<int>& theRoles,
                                          const bool          isResetItem)
{
  TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex(theTopLeft);
  if (anItemBase && isResetItem)
    anItemBase->Reset();

#if QT_VERSION < 0x050000
  (void)theRoles;
  emit dataChanged(theTopLeft, theBottomRight);
#else
  emit dataChanged(theTopLeft, theBottomRight, theRoles);
#endif
}

// =======================================================================
// function : Selected
// purpose :
// =======================================================================
QModelIndexList TreeModel_ModelBase::Selected(const QModelIndexList& theIndices,
                                              const int              theCellId,
                                              const Qt::Orientation  theOrientation)
{
  QModelIndexList aSelected;
  for (QModelIndexList::const_iterator anIndicesIt = theIndices.begin();
       anIndicesIt != theIndices.end();
       anIndicesIt++)
  {
    QModelIndex anIndex = *anIndicesIt;
    if ((theOrientation == Qt::Horizontal && anIndex.column() == theCellId)
        || (theOrientation == Qt::Vertical && anIndex.row() == theCellId))
      aSelected.append(anIndex);
  }
  return aSelected;
}

// =======================================================================
// function : SingleSelected
// purpose :
// =======================================================================
QModelIndex TreeModel_ModelBase::SingleSelected(const QModelIndexList& theIndices,
                                                const int              theCellId,
                                                const Qt::Orientation  theOrientation)
{
  QModelIndexList aSelected = Selected(theIndices, theCellId, theOrientation);
  return aSelected.size() == 1 ? aSelected.first() : QModelIndex();
}

// =======================================================================
// function :  SelectedItems
// purpose :
// =======================================================================
QList<TreeModel_ItemBasePtr> TreeModel_ModelBase::SelectedItems(const QModelIndexList& theIndices)
{
  QList<TreeModel_ItemBasePtr> anItems;

  for (QModelIndexList::const_iterator anIndicesIt = theIndices.begin();
       anIndicesIt != theIndices.end();
       anIndicesIt++)
  {
    TreeModel_ItemBasePtr anItem = TreeModel_ModelBase::GetItemByIndex(*anIndicesIt);
    if (!anItem || anItems.contains(anItem))
      continue;
    anItems.append(anItem);
  }
  return anItems;
}

// =======================================================================
// function : SubItemsPresentations
// purpose :
// =======================================================================
void TreeModel_ModelBase::SubItemsPresentations(
  const QModelIndexList&                        theIndices,
  NCollection_List<Handle(Standard_Transient)>& thePresentations)
{
  QList<TreeModel_ItemBasePtr> anItems;

  for (QModelIndexList::const_iterator anIndicesIt = theIndices.begin();
       anIndicesIt != theIndices.end();
       anIndicesIt++)
  {
    TreeModel_ItemBasePtr anItem = TreeModel_ModelBase::GetItemByIndex(*anIndicesIt);
    if (!anItem || anItems.contains(anItem))
      continue;
    subItemsPresentations(anItem, thePresentations);
  }
}

// =======================================================================
// function : subItemPresentations
// purpose :
// =======================================================================
void TreeModel_ModelBase::subItemsPresentations(
  const TreeModel_ItemBasePtr&                  theItem,
  NCollection_List<Handle(Standard_Transient)>& thePresentations)
{
  theItem->Presentations(thePresentations);

  QList<TreeModel_ItemBasePtr> anItems;
  for (int aRowId = 0; aRowId < theItem->rowCount(); aRowId++)
  {
    subItemsPresentations(theItem->Child(aRowId, theItem->Column()), thePresentations);
  }
}

// =======================================================================
// function : createRoot
// purpose :
// =======================================================================
void TreeModel_ModelBase::createRoot(const int theColumnId)
{
  myRootItems.insert(theColumnId, createRootItem(theColumnId));
}

// =======================================================================
// function : setHeaderItem
// purpose :
// =======================================================================
void TreeModel_ModelBase::setHeaderItem(const int                      theColumnId,
                                        const TreeModel_HeaderSection& theSection)
{
  if (theSection.IsEmpty())
  {
    // remove section
    myHeaderValues.remove(theColumnId);
    myRootItems.remove(theColumnId);
  }

  myHeaderValues[theColumnId] = theSection;
  createRoot(theColumnId);
}

// =======================================================================
// function :  getIndexValue
// purpose :
// =======================================================================
void* TreeModel_ModelBase::getIndexValue(const TreeModel_ItemBasePtr& theItem)
{
  return theItem.data();
}
