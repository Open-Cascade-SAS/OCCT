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

#include <DFBrowserPane_TableView.hxx>
#include <DFBrowserPane_Tools.hxx>

#include <QWidget>
#include <QTableView>
#include <QAbstractTableModel>
#include <QHeaderView>
#include <QHBoxLayout>

static const int DEFAULT_ROW_HEIGHT = 30;

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowserPane_TableView::DFBrowserPane_TableView (QWidget* theParent,
                                                  const QMap<int, int>& theDefaultColumnWidths)
: QWidget (theParent)
{
  QHBoxLayout* aLay = new QHBoxLayout (this);
  aLay->setContentsMargins (0, 0, 0, 0);

  myTableView = new QTableView (theParent);
  myTableView->setShowGrid (false);

  QHeaderView* aVHeader = myTableView->verticalHeader();
  aVHeader->setVisible (false);
  aVHeader->setDefaultSectionSize (aVHeader->minimumSectionSize() + DFBrowserPane_Tools::HeaderSectionMargin());

  myTableView->horizontalHeader()->setStretchLastSection (true);
  aLay->addWidget (myTableView);
  myDefaultColumnWidths = theDefaultColumnWidths;
}

// =======================================================================
// function : SetModel
// purpose :
// =======================================================================
void DFBrowserPane_TableView::SetModel (QAbstractTableModel* theModel)
{
  myTableView->setModel (theModel);

  for (int aColumnId = 0, aCount = theModel->columnCount(); aColumnId < aCount; aColumnId++)
    myTableView->setColumnWidth (aColumnId, myDefaultColumnWidths.contains (aColumnId) ?
            myDefaultColumnWidths[aColumnId] : DFBrowserPane_Tools::DefaultPanelColumnWidth (aColumnId));
}

// =======================================================================
// function : SetFixedRowCount
// purpose :
// =======================================================================
void DFBrowserPane_TableView::SetFixedRowCount (const int theCount, QTableView* theView)
{
  theView->setMaximumHeight (DEFAULT_ROW_HEIGHT*(theCount + 1/*header row*/));
}

// =======================================================================
// function : GetSelectedColumnValues
// purpose :
// =======================================================================
QStringList DFBrowserPane_TableView::GetSelectedColumnValues (QTableView* theTableView, const int theColumnId)
{
  QAbstractItemModel* aModel = theTableView->model();
  QModelIndexList aSelectedIndices = theTableView->selectionModel()->selectedIndexes();

  QStringList aSelectedEntries;
  for (QModelIndexList::const_iterator aSelectedIt = aSelectedIndices.begin();
       aSelectedIt != aSelectedIndices.end(); aSelectedIt++)
  {
    QModelIndex anIndex = *aSelectedIt;
    aSelectedEntries.append (aModel->data (aModel->index (anIndex.row(), theColumnId, anIndex.parent()), Qt::DisplayRole).toString());
  }
  return aSelectedEntries;
}
