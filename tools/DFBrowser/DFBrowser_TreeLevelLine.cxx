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

#include <inspector/DFBrowser_TreeLevelLine.hxx>

#include <inspector/DFBrowser_SearchLine.hxx>
#include <inspector/DFBrowser_Window.hxx>
#include <inspector/DFBrowser_TreeLevelLineDelegate.hxx>
#include <inspector/DFBrowser_TreeLevelLineModel.hxx>

#include <inspector/DFBrowserPane_Tools.hxx>

#include <QAbstractItemModel>
#include <QFrame>
#include <QGridLayout>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QPainter>
#include <QScrollBar>
#include <QTableView>
#include <QToolButton>
#include <QWidget>

const int HISTORY_SIZE = 10;
const int MARGIN_SIZE = 2;

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowser_TreeLevelLine::DFBrowser_TreeLevelLine (QWidget* theParent)
: QObject (theParent), mySelectionProcessingBlocked (false), myCurrentHistoryIndex (-1)
{
  myMainWindow = new QWidget (theParent);
  QGridLayout* aLayout = new QGridLayout (myMainWindow);
  aLayout->setContentsMargins (MARGIN_SIZE, MARGIN_SIZE, MARGIN_SIZE, MARGIN_SIZE);

  myBackwardButton = new QToolButton (myMainWindow);
  myBackwardButton->setIcon (QIcon (":/icons/treeline_backward.png"));
  myBackwardButton->setToolTip (tr ("Backward"));
  connect (myBackwardButton, SIGNAL (clicked()), this, SLOT (onActionClicked()));
  aLayout->addWidget (myBackwardButton, 0, 0);

  myForwardButton = new QToolButton (myMainWindow);
  myForwardButton->setIcon (QIcon (":/icons/treeline_forward.png"));
  myForwardButton->setToolTip (tr ("Forward"));
  connect (myForwardButton, SIGNAL (clicked()), this, SLOT (onActionClicked()));
  aLayout->addWidget (myForwardButton, 0, 1);

  myTableView = new QTableView (myMainWindow);
  myTableView->horizontalHeader()->setVisible (false);
  QHeaderView* aVHeader = myTableView->verticalHeader();
  aVHeader->setVisible (false);
  int aDefCellSize = aVHeader->minimumSectionSize() + DFBrowserPane_Tools::HeaderSectionMargin();
  aVHeader->setDefaultSectionSize (aDefCellSize);
  aLayout->addWidget (myTableView, 0, 2);

  myTableView->setFixedHeight (aDefCellSize);
  myTableView->horizontalHeader()->setMinimumSectionSize (5); // it will be resized by context
  myTableView->setHorizontalScrollMode (QAbstractItemView::ScrollPerItem);
  myTableView->setHorizontalScrollBarPolicy (Qt::ScrollBarAlwaysOff); //! TEMPORARY
  myTableView->setShowGrid (false);

  DFBrowser_TreeLevelLineModel* aHModel = new DFBrowser_TreeLevelLineModel (myTableView);
  myTableView->setModel (aHModel);

  QItemSelectionModel* aSelectionModel = new QItemSelectionModel (aHModel);
  myTableView->setSelectionMode (QAbstractItemView::SingleSelection);
  myTableView->setSelectionModel (aSelectionModel);
  connect (aSelectionModel, SIGNAL (selectionChanged (const QItemSelection&, const QItemSelection&)),
           this, SLOT (onTableSelectionChanged (const QItemSelection&, const QItemSelection&)));

  // highlight for items
  myTableView->viewport()->setAttribute (Qt::WA_Hover);
  myTableView->setItemDelegate (new DFBrowser_TreeLevelLineDelegate (myTableView));

  aLayout->setColumnStretch (2, 1);

  myUpdateButton = new QToolButton (myMainWindow);
  myUpdateButton->setIcon (QIcon (":/icons/treeline_update.png"));
  myUpdateButton->setToolTip (tr ("Update Tree Model"));
  connect (myUpdateButton, SIGNAL (clicked()), this, SLOT (onActionClicked()));
  aLayout->addWidget (myUpdateButton, 0, 3);

  mySearchLine = new DFBrowser_SearchLine (myMainWindow);
  aLayout->addWidget (mySearchLine, 0, 4);

  updateActionsState();
}

// =======================================================================
// function : clear
// purpose :
// =======================================================================
void DFBrowser_TreeLevelLine::ClearHistory()
{
  myHistoryIndices.clear();
  setCurrentHistoryIndex (-1);
}

// =======================================================================
// function : onSelectionChanged
// purpose :
// =======================================================================
void DFBrowser_TreeLevelLine::OnTreeViewSelectionChanged (const QItemSelection& theSelected,
                                                          const QItemSelection&)
{
  QModelIndexList aSelectedIndices = theSelected.indexes();
  QModelIndex aSelectedIndex = DFBrowser_Window::SingleSelected (aSelectedIndices, 0);

  if (!mySelectionProcessingBlocked) // we're processing action click
    setForwardIndex (aSelectedIndex);

  bool isBlocked = mySelectionProcessingBlocked;
  // block selection processing in order to avoid circling by processing table selection changing
  mySelectionProcessingBlocked = true;
  DFBrowser_TreeLevelLineModel* aModel = dynamic_cast<DFBrowser_TreeLevelLineModel*> (myTableView->model());
  aModel->Init (aSelectedIndex);
  myTableView->selectionModel()->clearSelection();
  myTableView->resizeColumnsToContents();

  myTableView->scrollTo (myTableView->model()->index (0, myTableView->model()->columnCount()-1));

  mySelectionProcessingBlocked = isBlocked;
}

// =======================================================================
// function : onTableSelectionChanged
// purpose :
// =======================================================================
void DFBrowser_TreeLevelLine::onTableSelectionChanged (const QItemSelection& theSelected,
                                                       const QItemSelection&)
{
  if (mySelectionProcessingBlocked)
    return;

  DFBrowser_TreeLevelLineModel* aTableModel = dynamic_cast<DFBrowser_TreeLevelLineModel*> (myTableView->model());
  if (!aTableModel)
    return;

  QModelIndex aSelectedIndex = DFBrowser_Window::SingleSelected (theSelected.indexes(), 0, Qt::Vertical);
  emit indexSelected (aTableModel->GetTreeViewIndex (aSelectedIndex));
}

// =======================================================================
// function : onActionClicked
// purpose :
// =======================================================================
void DFBrowser_TreeLevelLine::onActionClicked()
{
  QToolButton* aSender = (QToolButton*)sender();
  if (aSender == myBackwardButton || aSender == myForwardButton)
  {
    bool aBlocked = mySelectionProcessingBlocked;
    mySelectionProcessingBlocked = true;
    QModelIndex anIndex;
    if (aSender == myBackwardButton)
    {
      anIndex = getBackwardIndex();
      if (anIndex.isValid())
        setCurrentHistoryIndex (myCurrentHistoryIndex - 1);
    }
    else
    {
      anIndex = getForwardIndex();
      if (anIndex.isValid())
        setCurrentHistoryIndex (myCurrentHistoryIndex + 1);
    }
    if (anIndex.isValid())
      emit indexSelected (anIndex);
    mySelectionProcessingBlocked = aBlocked;
  }
  else if (aSender == myUpdateButton)
    emit updateClicked();
}

// =======================================================================
// function : getBackwardIndex
// purpose :
// =======================================================================
QModelIndex DFBrowser_TreeLevelLine::getBackwardIndex()
{
  return myCurrentHistoryIndex > 0 ? myHistoryIndices[myCurrentHistoryIndex-1] : QModelIndex();
}

// =======================================================================
// function : getForwardIndex
// purpose :
// =======================================================================
QModelIndex DFBrowser_TreeLevelLine::getForwardIndex()
{
  return (myCurrentHistoryIndex >= 0 && myCurrentHistoryIndex + 1 < myHistoryIndices.count())
     ? myHistoryIndices[myCurrentHistoryIndex + 1] : QModelIndex();
}

// =======================================================================
// function : setForwardIndex
// purpose :
// =======================================================================
void DFBrowser_TreeLevelLine::setForwardIndex (const QModelIndex& theIndex)
{
  while (myCurrentHistoryIndex != myHistoryIndices.count() - 1)
    myHistoryIndices.removeLast();

  myHistoryIndices.append (theIndex);
  if (myHistoryIndices.size() > HISTORY_SIZE)
    myHistoryIndices.removeFirst();

  setCurrentHistoryIndex (myHistoryIndices.count() - 1);
}

// =======================================================================
// function : setCurrentHistoryIndex
// purpose :
// =======================================================================
void DFBrowser_TreeLevelLine::setCurrentHistoryIndex (const int theIndexId)
{
  myCurrentHistoryIndex = theIndexId;
  updateActionsState();
}

// =======================================================================
// function : updateActionsState
// purpose :
// =======================================================================
void DFBrowser_TreeLevelLine::updateActionsState()
{
  if (myCurrentHistoryIndex < 0)
  {
    myBackwardButton->setEnabled (false);
    myForwardButton->setEnabled (false);
  }
  else
  {
    myBackwardButton->setEnabled (myCurrentHistoryIndex > 0);
    myForwardButton->setEnabled (myCurrentHistoryIndex < myHistoryIndices.size() - 1);
  }
}
