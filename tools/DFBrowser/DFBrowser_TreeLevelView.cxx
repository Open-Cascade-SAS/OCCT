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

#include <inspector/DFBrowser_TreeLevelView.hxx>

#include <inspector/DFBrowser_Item.hxx>
#include <inspector/DFBrowser_TreeLevelViewModel.hxx>
#include <inspector/DFBrowser_Window.hxx>
#include <inspector/DFBrowserPane_Tools.hxx>
#include <inspector/TreeModel_ModelBase.hxx>

#include <QItemSelectionModel>

#include <QGridLayout>
#include <QHeaderView>
#include <QTableView>
#include <QWidget>

const int LABEL_OR_ATTRIBUTECOLUMN_WIDTH = 160;

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowser_TreeLevelView::DFBrowser_TreeLevelView (QWidget* theParent)
: QObject (theParent)
{
  myMainWindow = new QWidget (theParent);
  QGridLayout* aLayout = new QGridLayout (myMainWindow);
  aLayout->setContentsMargins (0, 0, 0, 0);

  myTableView = new QTableView (myMainWindow);
  myTableView->setModel (new DFBrowser_TreeLevelViewModel (myTableView));
  myTableView->setColumnWidth (0, LABEL_OR_ATTRIBUTECOLUMN_WIDTH);
  myTableView->setEditTriggers (QAbstractItemView::DoubleClicked);
  myTableView->horizontalHeader()->setVisible (false);
  QHeaderView* aVHeader = myTableView->verticalHeader();
  aVHeader->setVisible (false);
  aVHeader->setDefaultSectionSize (aVHeader->minimumSectionSize());
  myTableView->horizontalHeader()->setStretchLastSection (true);
  aLayout->addWidget (myTableView);

  QItemSelectionModel* aSelectionModel = new QItemSelectionModel (myTableView->model());
  myTableView->setSelectionMode (QAbstractItemView::SingleSelection);
  myTableView->setSelectionModel (aSelectionModel);
  myTableView->setSelectionBehavior (QAbstractItemView::SelectRows);
  connect (aSelectionModel, SIGNAL (selectionChanged (const QItemSelection&, const QItemSelection&)),
           this, SLOT (onTableSelectionChanged (const QItemSelection&, const QItemSelection&)));
  connect (myTableView, SIGNAL (doubleClicked (const QModelIndex&)),
           this, SLOT (onTableDoubleClicked (const QModelIndex&)));

  DFBrowser_Window::SetWhiteBackground (myTableView);
  myTableView->setGridStyle (Qt::NoPen);
}

// =======================================================================
// function : clearSelection
// purpose :
// =======================================================================
void DFBrowser_TreeLevelView::ClearSelection()
{
  myTableView->selectionModel()->clearSelection();
}

// =======================================================================
// function : ProcessItem
// purpose :
// =======================================================================
bool DFBrowser_TreeLevelView::ProcessItem (const QModelIndex& theIndex)
{
  bool aResult = false;
  TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex (theIndex);
  if (anItemBase) {
    // use this view for attribute/document/label items
    DFBrowser_ItemPtr anItem = itemDynamicCast<DFBrowser_Item> (anItemBase);
    if (anItem)
      aResult = anItem && !anItem->HasAttribute();
    else
      aResult = true; // attribute or document item
  }
  return aResult;
}

// =======================================================================
// function : UpdateByTreeSelectionChanged
// purpose :
// =======================================================================
void DFBrowser_TreeLevelView::UpdateByTreeSelectionChanged (const QItemSelection& theSelected,
                                                            const QItemSelection&)
{
  QModelIndexList aSelectedIndices = theSelected.indexes();
  QModelIndexList aFirstColumnSelectedIndices;
  for (QModelIndexList::const_iterator aSelIt = aSelectedIndices.begin(); aSelIt != aSelectedIndices.end(); aSelIt++)
  {
    QModelIndex anIndex = *aSelIt;
    if (anIndex.column() == 0)
      aFirstColumnSelectedIndices.append (anIndex);
  }

  if (aFirstColumnSelectedIndices.size() != 1)
    return;

  DFBrowser_TreeLevelViewModel* aModel = dynamic_cast<DFBrowser_TreeLevelViewModel*> (myTableView->model());
  const QModelIndex& anIndex = aFirstColumnSelectedIndices.first();
  if (DFBrowser_TreeLevelView::ProcessItem(anIndex)) // to Init
    aModel->Init (anIndex);
  else
    aModel->Reset();
}

// =======================================================================
// function : onTableSelectionChanged
// purpose :
// =======================================================================
void DFBrowser_TreeLevelView::onTableSelectionChanged (const QItemSelection& theSelected,
                                                       const QItemSelection&)
{
  QModelIndexList aSelectedIndices = theSelected.indexes();
  QModelIndex aSelectedIndex = DFBrowser_Window::SingleSelected (aSelectedIndices, 0);

  DFBrowser_TreeLevelViewModel* aTableModel = dynamic_cast<DFBrowser_TreeLevelViewModel*> (myTableView->model());
  if (aTableModel && aTableModel->IsInitialized())
  {
    const QModelIndex& aTreeViewIndex = aTableModel->GetTreeViewIndex (aSelectedIndex);
    if (aTreeViewIndex.isValid())
      emit indexSelected (aTreeViewIndex);
  }
}

// =======================================================================
// function : onTableDoubleClicked
// purpose :
// =======================================================================
void DFBrowser_TreeLevelView::onTableDoubleClicked (const QModelIndex& theIndex)
{
  DFBrowser_TreeLevelViewModel* aTableModel = dynamic_cast<DFBrowser_TreeLevelViewModel*> (myTableView->model());
  if (!aTableModel)
    return;

  const QModelIndex& aTreeViewIndex = aTableModel->GetTreeViewIndex (theIndex);
  if (aTreeViewIndex.isValid())
    emit indexDoubleClicked (aTreeViewIndex);
}
