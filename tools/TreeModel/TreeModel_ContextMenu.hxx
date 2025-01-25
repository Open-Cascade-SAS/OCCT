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

#ifndef TreeModel_ContextMenu_H
#define TreeModel_ContextMenu_H

#include <Standard.hxx>
#include <Standard_Macro.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QObject>
#include <QPoint>
#include <Standard_WarningsRestore.hxx>

class QTreeView;

//! \class TreeModel_ContextMenu
//! \brief Creates actions to show/hide tree view columns
class TreeModel_ContextMenu : public QObject
{
  Q_OBJECT
public:
  //! Constructor
  Standard_EXPORT TreeModel_ContextMenu(QTreeView* theTreeView);

  //! Destructor
  ~TreeModel_ContextMenu() {}

protected slots:
  //! Shows context menu for tree view header. It contains actions to change columns visibility.
  //! \param thePosition a clicked point
  void onTreeViewHeaderContextMenuRequested(const QPoint& thePosition);

  //! Changes clicked column visibility
  void onColumnVisibilityChanged();

private:
  QTreeView* myTreeView; //!< current tree view
};

#endif
