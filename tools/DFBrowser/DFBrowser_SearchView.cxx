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

#include <inspector/DFBrowser_SearchView.hxx>

#include <inspector/DFBrowser_HighlightDelegate.hxx>
#include <inspector/DFBrowser_SearchLine.hxx>
#include <inspector/DFBrowser_SearchLineModel.hxx>
#include <inspector/DFBrowser_Window.hxx>

#include <inspector/DFBrowserPane_Tools.hxx>

#include <QAbstractProxyModel>
#include <QGridLayout>
#include <QHeaderView>
#include <QTableView>
#include <QWidget>

const int DEFAULT_COLUMN_WIDTH = 500;
const int DEFAULT_ICON_SIZE = 40;

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowser_SearchView::DFBrowser_SearchView (QWidget* theParent)
: QObject (theParent)
{
  myMainWindow = new QWidget (theParent);
  QGridLayout* aLayout = new QGridLayout (myMainWindow);
  aLayout->setContentsMargins (0, 0, 0, 0);

  myTableView = new QTableView (myMainWindow);
  myTableView->verticalHeader()->setVisible (false);
  myTableView->verticalHeader()->setDefaultSectionSize (DEFAULT_ICON_SIZE + DFBrowserPane_Tools::HeaderSectionMargin());
  myTableView->setIconSize (QSize (DEFAULT_ICON_SIZE, DEFAULT_ICON_SIZE));
  myTableView->horizontalHeader()->setVisible (false);
  myTableView->horizontalHeader()->setStretchLastSection (true);

  myTableView->viewport()->setAttribute (Qt::WA_Hover);
  myTableView->setItemDelegate (new DFBrowser_HighlightDelegate (myTableView));

  aLayout->addWidget (myTableView);

  DFBrowser_Window::SetWhiteBackground (myTableView);
  myTableView->setGridStyle (Qt::NoPen);
}

// =======================================================================
// function : InitModels
// purpose :
// =======================================================================
void DFBrowser_SearchView::InitModels()
{
  QAbstractItemModel* aModel = mySearchLine->GetCompletionModel();
  myTableView->setModel (aModel);
  myTableView->setColumnWidth (0, 0); // to hide column
  myTableView->setColumnWidth (1, DEFAULT_ICON_SIZE + DFBrowserPane_Tools::HeaderSectionMargin());

  QItemSelectionModel* aSelectionModel = new QItemSelectionModel (aModel);
  myTableView->setSelectionMode (QAbstractItemView::SingleSelection);
  myTableView->setSelectionModel (aSelectionModel);
  connect (aSelectionModel, SIGNAL (selectionChanged (const QItemSelection&, const QItemSelection&)),
          this, SLOT (onTableSelectionChanged (const QItemSelection&, const QItemSelection&)));
  connect (myTableView, SIGNAL (doubleClicked (const QModelIndex&)),
          this, SLOT (onTableDoubleClicked (const QModelIndex&)));
}

// =======================================================================
// function : onTableSelectionChanged
// purpose :
// =======================================================================
void DFBrowser_SearchView::onTableSelectionChanged (const QItemSelection&,
                                                   const QItemSelection&)
{
  QItemSelectionModel* aSelectionModel = myTableView->selectionModel();
  QModelIndexList aSelectedIndices = aSelectionModel->selectedIndexes();
  QModelIndex aSelectedIndex = DFBrowser_Window::SingleSelected (aSelectedIndices, 2);
  if (!aSelectedIndex.isValid())
    return;
  QAbstractProxyModel* aTableModel = dynamic_cast<QAbstractProxyModel*> (myTableView->model());
  if (!aTableModel)
    return;
  DFBrowser_SearchLineModel* aSourceModel = dynamic_cast<DFBrowser_SearchLineModel*> (aTableModel->sourceModel());
  if (!aSourceModel)
    return;

  QModelIndex aSourceSelectedIndex = aTableModel->mapToSource(aSelectedIndex);
  emit pathSelected(aSourceModel->GetPath (aSourceSelectedIndex), aSourceModel->GetValue (aSourceSelectedIndex));
}

// =======================================================================
// function : onTableDoubleClicked
// purpose :
// =======================================================================
void DFBrowser_SearchView::onTableDoubleClicked (const QModelIndex& theIndex)
{
  QAbstractProxyModel* aTableModel = dynamic_cast<QAbstractProxyModel*> (myTableView->model());
  if (!aTableModel)
    return;
  DFBrowser_SearchLineModel* aSourceModel = dynamic_cast<DFBrowser_SearchLineModel*> (aTableModel->sourceModel());
  if (!aSourceModel)
    return;

  QModelIndex aSourceSelectedIndex = aTableModel->mapToSource(theIndex);
  emit pathDoubleClicked (aSourceModel->GetPath (aSourceSelectedIndex), aSourceModel->GetValue (aSourceSelectedIndex));
}
