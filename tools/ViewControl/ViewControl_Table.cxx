// Created on: 2020-01-25
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2020 OPEN CASCADE SAS
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

#include <inspector/ViewControl_Table.hxx>
#include <inspector/ViewControl_TableModel.hxx>
#include <inspector/ViewControl_Tools.hxx>

#include <inspector/TreeModel_ItemProperties.hxx>
#include <inspector/TreeModel_ItemStream.hxx>
#include <inspector/TreeModel_Tools.hxx>

#include <Standard_Dump.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QAction>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QTableView>
#include <QWidget>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
ViewControl_Table::ViewControl_Table (QWidget* theParent)
: QObject (theParent), myIsUseProperty (false)
{
  myMainWidget = new QWidget (theParent);
  QGridLayout* aLayout = new QGridLayout (myMainWidget);
  aLayout->setContentsMargins (0, 0, 0, 0);

  myTableView = new QTableView (myMainWidget);
  myTableView->setVerticalScrollMode (QAbstractItemView::ScrollPerPixel);

  QHeaderView* aVHeader = myTableView->verticalHeader();
  int aDefCellSize = aVHeader->minimumSectionSize();
  aVHeader->setDefaultSectionSize (aDefCellSize);

  connect (myTableView->horizontalHeader(), SIGNAL (sectionResized (int, int, int)),
           this, SLOT(onHeaderResized (int, int, int)));

  aLayout->addWidget (myTableView);
}

// =======================================================================
// function : SetModel
// purpose :
// =======================================================================
void ViewControl_Table::SetModel (QAbstractTableModel* theModel)
{
  myTableView->setModel (theModel);

  myTableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
  QItemSelectionModel* aSelectionModel = new QItemSelectionModel(theModel);
  myTableView->setSelectionModel (aSelectionModel);
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void ViewControl_Table::Init (ViewControl_TableModelValues* theModelValues)
{
  myTableView->selectionModel()->clearSelection();

  ViewControl_TableModel* aModel = dynamic_cast<ViewControl_TableModel*> (myTableView->model());
  aModel->SetModelValues (theModelValues);

  myTableView->horizontalHeader()->setVisible (theModelValues->IsHeaderVisible (Qt::Horizontal));

  myTableView->verticalHeader()->setVisible (theModelValues->IsHeaderVisible (Qt::Vertical));
  int aSectionSize;
  if (theModelValues->DefaultSectionSize (Qt::Vertical, aSectionSize) )
  {
    myTableView->verticalHeader()->setDefaultSectionSize (aSectionSize);
  }
  else
    myTableView->verticalHeader()->setDefaultSectionSize (myTableView->verticalHeader()->minimumSectionSize());

  aModel->EmitLayoutChanged();
}

// =======================================================================
// function : SelectedIndices
// purpose :
// =======================================================================
void ViewControl_Table::SelectedIndices (QMap<int, QList<int>>& theSelectedIndices) const
{
  QModelIndexList aSelected = myTableView->selectionModel()->selectedIndexes();

  int aRow, aColumn;
  for (QModelIndexList::const_iterator anIt = aSelected.begin(); anIt != aSelected.end(); anIt++)
  {
    QModelIndex anIndex = *anIt;
    aRow = anIndex.row();
    aColumn = anIndex.column();
    if (!theSelectedIndices.contains (aRow))
      theSelectedIndices.insert (aRow, QList<int>());
    theSelectedIndices[aRow].append (aColumn);
  }
}

// =======================================================================
// function : SeparatorData
// purpose :
// =======================================================================
QString ViewControl_Table::SeparatorData()
{
  return ViewControl_Tools::TableSeparator();
}

// =======================================================================
// function : SelectedPointers
// purpose :
// =======================================================================
void ViewControl_Table::SelectedPointers (QStringList& thePointers) const
{
  QMap<int, QList<int>> aSelectedIndices;
  SelectedIndices (aSelectedIndices);

  ViewControl_TableModel* aTableModel = dynamic_cast<ViewControl_TableModel*>(TableView()->model());
  ViewControl_TableModelValues* aTableValues = aTableModel->ModelValues();

  for (QMap<int, QList<int>>::const_iterator aSelIt = aSelectedIndices.begin(); aSelIt != aSelectedIndices.end(); aSelIt++)
  {
    int aRowId = aSelIt.key();
    QList<int> aColIds = aSelIt.value();
    for (int aColId = 0; aColId < aColIds.size(); aColId++)
    {
      int aSelectedColId = aColIds[aColId];
      if (aSelectedColId != 1)
        continue;

      QString aData = aTableValues->Data (aRowId, aSelectedColId, Qt::DisplayRole).toString();
      if (aData.contains (Standard_Dump::GetPointerPrefix().ToCString()))
        thePointers.append (aData);
    }
  }
}

// =======================================================================
// function : onHeaderResized
// purpose :
// =======================================================================
void ViewControl_Table::onHeaderResized (int theSectionId, int, int)
{
  if (theSectionId != 0)
    return;

  myTableView->horizontalHeader()->setDefaultSectionSize (myTableView->columnWidth (theSectionId));
}
