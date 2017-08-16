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

#include <DFBrowserPane_TDataStdTreeNode.hxx>

#include <DFBrowserPane_AttributePaneModel.hxx>
#include <DFBrowserPane_TableView.hxx>
#include <DFBrowserPane_TDataStdTreeNodeItem.hxx>
#include <DFBrowserPane_TDataStdTreeNodeModel.hxx>
#include <DFBrowserPane_Tools.hxx>

#include <TDataStd_TreeNode.hxx>

#include <QVariant>
#include <QTableView>
#include <QTreeView>
#include <QVBoxLayout>
#include <QWidget>
#include <QItemSelectionModel>

//#define REQUIRE_OCAF_REVIEW:11
// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowserPane_TDataStdTreeNode::DFBrowserPane_TDataStdTreeNode()
: DFBrowserPane_AttributePane()
{
  myModel = new DFBrowserPane_TDataStdTreeNodeModel (0);
  mySelectionModels.clear(); // do not use selection model of parent pane
  mySelectionModels.push_back (new QItemSelectionModel (myModel));
}

// =======================================================================
// function : CreateWidget
// purpose :
// =======================================================================
QWidget* DFBrowserPane_TDataStdTreeNode::CreateWidget (QWidget* theParent)
{
  QWidget* aMainWidget = new QWidget (theParent);
  aMainWidget->setVisible (false);

  myTableView = new DFBrowserPane_TableView (aMainWidget, getTableColumnWidths());
  DFBrowserPane_TableView::SetFixedRowCount (1, myTableView->GetTableView());
  myTableView->SetModel (myPaneModel);
  QTableView* aTableView = myTableView->GetTableView();
  aTableView->setSelectionModel (mySelectionModels.front());
  aTableView->setSelectionBehavior (QAbstractItemView::SelectRows);

  QVBoxLayout* aLay = new QVBoxLayout (aMainWidget);
  aLay->setContentsMargins (0, 0, 0, 0);
  aLay->addWidget (myTableView);

  myTreeNodeView = new QTreeView (theParent);
  myTreeNodeView->setModel (myModel);
  myTreeNodeView->setSelectionModel (mySelectionModels.front());
  myTreeNodeView->setSelectionBehavior (QAbstractItemView::SelectRows);

  aLay->addWidget (myTreeNodeView);
  return aMainWidget;
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void DFBrowserPane_TDataStdTreeNode::Init (const Handle(TDF_Attribute)& theAttribute)
{
  Handle(TDataStd_TreeNode) aTreeNode = Handle(TDataStd_TreeNode)::DownCast (theAttribute);

  bool aDefaultGUID = aTreeNode->ID() != aTreeNode->GetDefaultTreeID();
  myTableView->setVisible (!aDefaultGUID);
  if (!aDefaultGUID) {
    QList<QVariant> aValues;
    char aStr[256];
    aTreeNode->ID().ToCString (aStr);
    TCollection_AsciiString aString(aStr);
    aValues.append (DFBrowserPane_Tools::ToString(aString));
    getPaneModel()->Init (aValues);
  }


  DFBrowserPane_TDataStdTreeNodeModel* aModel = dynamic_cast<DFBrowserPane_TDataStdTreeNodeModel*> (myModel);
  aModel->Reset();

  if (!aTreeNode.IsNull())
  {
    Handle(TDataStd_TreeNode) aRootItem = aTreeNode->Root();
    aModel->SetAttribute (aRootItem);

    QModelIndex anIndex = aModel->FindIndex (theAttribute);
    if (anIndex.isValid())
    {
      myTreeNodeView->setExpanded (anIndex.parent(), true);
      myTreeNodeView->scrollTo (anIndex);

      TreeModel_ItemBasePtr anAttributeItem = TreeModel_ModelBase::GetItemByIndex (anIndex);
      DFBrowserPane_TDataStdTreeNodeItemPtr anAttributeNodeItem =
              itemDynamicCast<DFBrowserPane_TDataStdTreeNodeItem>(anAttributeItem);
      anAttributeNodeItem->setCurrentAttribute (true);
    }
  }
  aModel->EmitLayoutChanged();
}

// =======================================================================
// function : GetShortAttributeInfo
// purpose :
// =======================================================================
void DFBrowserPane_TDataStdTreeNode::GetShortAttributeInfo (const Handle(TDF_Attribute)& theAttribute,
                                                            QList<QVariant>& theValues)
{
  Handle(TDataStd_TreeNode) aTNAttribute = Handle(TDataStd_TreeNode)::DownCast (theAttribute);
  bool aDefaultGUID = aTNAttribute->ID() != aTNAttribute->GetDefaultTreeID();
  QString aGUIDPrefix;
  if (!aDefaultGUID) {
    QList<QVariant> aValues;
    char aStr[256];
    aTNAttribute->ID().ToCString (aStr);
    TCollection_AsciiString aString(aStr);
    aGUIDPrefix = DFBrowserPane_Tools::ToString(aString);
  }

  if (aTNAttribute->HasFather())
  {
    TDF_Label aLabel = aTNAttribute->Father()->Label();
    theValues.append (QString ("%1 ==> %2").arg(aGUIDPrefix).arg (DFBrowserPane_Tools::GetEntry (aLabel).ToCString()));
  }
  else 
  {
    Handle(TDataStd_TreeNode) aFirstChild = aTNAttribute->First();
    QStringList aRefs;
    while (! aFirstChild.IsNull() ) 
    {
      TDF_Label aLabel = aFirstChild->Label();
      aRefs.append (DFBrowserPane_Tools::GetEntry (aLabel).ToCString());
      aFirstChild = aFirstChild->Next();
    }
    theValues.append (QString ("%1 <== (%2)").arg(aGUIDPrefix).arg (aRefs.join (", ")));
  }
}

// =======================================================================
// function : GetReferences
// purpose :
// =======================================================================
void DFBrowserPane_TDataStdTreeNode::GetReferences (const Handle(TDF_Attribute)& theAttribute,
                                                    NCollection_List<TDF_Label>& theRefLabels,
                                                    Handle(Standard_Transient)&)
{
  Handle(TDataStd_TreeNode) anAttribute = Handle(TDataStd_TreeNode)::DownCast (theAttribute);
  if (anAttribute.IsNull())
    return;

  theRefLabels.Append (anAttribute->Label());
}