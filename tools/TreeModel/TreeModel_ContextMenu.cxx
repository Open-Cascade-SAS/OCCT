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

#include <inspector/TreeModel_ContextMenu.hxx>
#include <inspector/TreeModel_ModelBase.hxx>
#include <inspector/TreeModel_Tools.hxx>

#include <inspector/ViewControl_Tools.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QHeaderView>
#include <QMenu>
#include <QTreeView>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
TreeModel_ContextMenu::TreeModel_ContextMenu(QTreeView* theTreeView)
    : QObject(theTreeView),
      myTreeView(theTreeView)
{
  myTreeView->header()->setContextMenuPolicy(Qt::CustomContextMenu);
#if QT_VERSION >= 0x050000
  myTreeView->header()->setSectionsClickable(true);
#endif
  myTreeView->header()->setHighlightSections(true);
  connect(myTreeView->header(),
          SIGNAL(customContextMenuRequested(const QPoint&)),
          this,
          SLOT(onTreeViewHeaderContextMenuRequested(const QPoint&)));
}

// =======================================================================
// function : onTreeViewHeaderContextMenuRequested
// purpose :
// =======================================================================
void TreeModel_ContextMenu::onTreeViewHeaderContextMenuRequested(const QPoint& thePosition)
{
  TreeModel_ModelBase* aModel = dynamic_cast<TreeModel_ModelBase*>(myTreeView->model());
  if (!aModel)
    return;

  QMenu* aMenu       = new QMenu(myTreeView);
  int    aNbSections = aModel->columnCount();
  for (int aColumnId = 0; aColumnId < aNbSections; aColumnId++)
  {
    QAction* anAction =
      ViewControl_Tools::CreateAction(aModel->ChangeHeaderItem(aColumnId)->GetName(),
                                      SLOT(onColumnVisibilityChanged()),
                                      myTreeView,
                                      this);
    anAction->setCheckable(true);
    anAction->setChecked(!myTreeView->isColumnHidden(aColumnId));
    anAction->setData(aColumnId);
    aMenu->addAction(anAction);
  }

  if (aMenu->actions().isEmpty())
    return;

  QPoint aPoint = myTreeView->mapToGlobal(thePosition);
  aMenu->exec(aPoint);
}

// =======================================================================
// function : onColumnVisibilityChanged
// purpose :
// =======================================================================
void TreeModel_ContextMenu::onColumnVisibilityChanged()
{
  QAction* aClickedAction = (QAction*)sender();

  myTreeView->setColumnHidden(aClickedAction->data().toInt(), !aClickedAction->isChecked());
}
