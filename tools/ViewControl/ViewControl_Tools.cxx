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

#include <inspector/ViewControl_Tools.hxx>

#include <inspector/ViewControl_TableModel.hxx>
#include <inspector/TreeModel_ModelBase.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QAction>
#include <QHeaderView>
#include <QObject>
#include <QPalette>
#include <QTableView>
#include <QWidget>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : CreateAction
// purpose :
// =======================================================================
QAction* ViewControl_Tools::CreateAction (const QString& theText, const char* theSlot, QObject* theParent, QObject* theContext)
{
  QAction* anAction = new QAction (theText, theParent);
  QObject::connect (anAction, SIGNAL (triggered (bool)), theContext, theSlot);
  return anAction;
}

// =======================================================================
// function : SetWhiteBackground
// purpose :
// =======================================================================
void ViewControl_Tools::SetWhiteBackground (QWidget* theControl)
{
  QPalette aPalette = theControl->palette();
  aPalette.setColor (QPalette::All, QPalette::Foreground, Qt::white);
  theControl->setPalette (aPalette);
}

// =======================================================================
// function : SetDefaultHeaderSections
// purpose :
// =======================================================================
void ViewControl_Tools::SetDefaultHeaderSections(QTableView* theTableView, const Qt::Orientation theOrientation)
{
  ViewControl_TableModel * aTableModel = dynamic_cast<ViewControl_TableModel*> (theTableView->model());
  ViewControl_TableModelValues* aModelValues = aTableModel->ModelValues();
  if (!aModelValues)
    return;

  int aSectionSize;
  if (aModelValues->DefaultSectionSize (Qt::Horizontal, aSectionSize) )
    theTableView->horizontalHeader()->setDefaultSectionSize (aSectionSize);
  else {
    bool isStretchLastSection = true;
    for (int aColumnId = 0, aNbColumns = aTableModel->columnCount(); aColumnId < aNbColumns; aColumnId++)
    {
      TreeModel_HeaderSection aSection = aModelValues->HeaderItem (theOrientation, aColumnId);
      if (aSection.IsEmpty())
        continue;

      int aColumnWidth = aSection.GetWidth();
      if (aColumnWidth > 0)
      {
        theTableView->setColumnWidth (aColumnId, aColumnWidth);
        if (aColumnId == aNbColumns - 1)
          isStretchLastSection = false;
      }
      theTableView->setColumnHidden (aColumnId, aSection.IsHidden());
    }
    if (isStretchLastSection != theTableView->horizontalHeader()->stretchLastSection())
      theTableView->horizontalHeader()->setStretchLastSection (isStretchLastSection);
  }
}

// =======================================================================
// function : CreateTableModelValues
// purpose :
// =======================================================================
ViewControl_TableModelValues* ViewControl_Tools::CreateTableModelValues (QItemSelectionModel* theSelectionModel)
{
  ViewControl_TableModelValues* aTableValues = 0;

  QModelIndex anIndex = TreeModel_ModelBase::SingleSelected (theSelectionModel->selectedIndexes(), 0);
  TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex (anIndex);
  if (!anItemBase)
    return aTableValues;

  const Handle(TreeModel_ItemProperties)& anItemProperties = anItemBase->Properties();
  if (anItemProperties.IsNull())
    return aTableValues;

  aTableValues = new ViewControl_TableModelValues();
  aTableValues->SetProperties (anItemProperties);
  return aTableValues;
}
