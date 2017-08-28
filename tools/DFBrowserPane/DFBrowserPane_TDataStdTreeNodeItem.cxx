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

#include <inspector/DFBrowserPane_TDataStdTreeNodeItem.hxx>
#include <inspector/DFBrowserPane_Tools.hxx>

#include <inspector/DFBrowser_Module.hxx>
#include <inspector/DFBrowser_Tools.hxx>

#include <inspector/DFBrowserPane_AttributePane.hxx>
#include <inspector/DFBrowserPane_Tools.hxx>

#include <TDataStd_TreeNode.hxx>
#include <TDataStd_ChildNodeIterator.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_ChildIterator.hxx>

#include <QIcon>
#include <QObject>

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void DFBrowserPane_TDataStdTreeNodeItem::Init()
{
  DFBrowserPane_TDataStdTreeNodeItemPtr aParentItem = itemDynamicCast<DFBrowserPane_TDataStdTreeNodeItem> (Parent());
  // if aParentItem is empty, it is the root item, an attribute field is already filled by SetAttribute method
  if (aParentItem)
    myAttribute = aParentItem->getChildAttribute (Row());

  Handle(TDataStd_TreeNode) aTreeNode = Handle(TDataStd_TreeNode)::DownCast (myAttribute);
  if (aTreeNode.IsNull())
    return;

  TreeModel_ItemBase::Init();

  myRowCount = aTreeNode->NbChildren (false);
  myLabelName = QString (DFBrowserPane_Tools::GetEntry (aTreeNode->Label()).ToCString());
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================
void DFBrowserPane_TDataStdTreeNodeItem::Reset()
{
  DFBrowserPane_TDataStdTreeNodeItemPtr aParentItem = itemDynamicCast<DFBrowserPane_TDataStdTreeNodeItem> (Parent());
  if (aParentItem)
  {
    Handle(TDF_Attribute) anAttribute;
    SetAttribute (anAttribute);
    myRowCount = 0;
    myLabelName = QString();
  }
  myIsCurrentItem = false;
  TreeModel_ItemBase::Reset();
}

// =======================================================================
// function : data
// purpose :
// =======================================================================
QVariant DFBrowserPane_TDataStdTreeNodeItem::initValue (const int theRole) const
{
  if (Column() != 0)
    return QVariant();

  switch (theRole)
  {
    case Qt::DisplayRole:
    case Qt::ToolTipRole:    return getName();
    case Qt::ForegroundRole: return myIsCurrentItem ? QColor (Qt::darkBlue) : QColor (Qt::black);
    case Qt::BackgroundRole: return myIsCurrentItem ? DFBrowserPane_Tools::LightHighlightColor() : QVariant();
    default: break;
  }
  return QVariant();
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr DFBrowserPane_TDataStdTreeNodeItem::createChild(int theRow, int theColumn)
{
  return DFBrowserPane_TDataStdTreeNodeItem::CreateItem(currentItem(), theRow, theColumn);
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void DFBrowserPane_TDataStdTreeNodeItem::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<DFBrowserPane_TDataStdTreeNodeItem*>(this)->Init();
}

// =======================================================================
// function : getChildAttribute
// purpose :
// =======================================================================
Handle(TDF_Attribute) DFBrowserPane_TDataStdTreeNodeItem::getChildAttribute (const int theChildRow) const
{
  Handle(TDF_Attribute) aResult;
  
  Handle(TDataStd_TreeNode) aTreeNode = Handle(TDataStd_TreeNode)::DownCast (myAttribute);
  int aChildNodeId = 0;
  for (TDataStd_ChildNodeIterator aChildIt (aTreeNode); aChildIt.More(); aChildIt.Next(), aChildNodeId++)
  {
    if (aChildNodeId != theChildRow)
      continue;
    aResult = aChildIt.Value();
    break;
  }
  return aResult;
}

// =======================================================================
// function : getRowCount
// purpose :
// =======================================================================
int DFBrowserPane_TDataStdTreeNodeItem::getRowCount() const
{
  initItem();
  return myRowCount;
}
