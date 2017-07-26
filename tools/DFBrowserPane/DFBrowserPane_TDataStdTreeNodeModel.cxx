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

#include <DFBrowserPane_TDataStdTreeNodeModel.hxx>
#include <DFBrowserPane_TDataStdTreeNodeItem.hxx>

#include <TDataStd_TreeNode.hxx>

#include <QAbstractItemModel>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowserPane_TDataStdTreeNodeModel::DFBrowserPane_TDataStdTreeNodeModel (QObject* theParent)
: TreeModel_ModelBase (theParent)
{
  m_pRootItem = DFBrowserPane_TDataStdTreeNodeItem::CreateItem (TreeModel_ItemBasePtr(), 0, 0);
}

// =======================================================================
// function : SetAttribute
// purpose :
// =======================================================================
void DFBrowserPane_TDataStdTreeNodeModel::SetAttribute (const Handle(TDF_Attribute)& theAttribute)
{
  DFBrowserPane_TDataStdTreeNodeItemPtr aRootItem = itemDynamicCast<DFBrowserPane_TDataStdTreeNodeItem>(m_pRootItem);
  Reset();
  aRootItem->SetAttribute (theAttribute);
  EmitLayoutChanged();
}
