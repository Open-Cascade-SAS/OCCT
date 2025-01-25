// Created on: 2021-04-27
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef MessageView_ActionsTest_H
#define MessageView_ActionsTest_H

#include <Standard.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Transient.hxx>

#include <inspector/MessageModel_ActionType.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QObject>
#include <QMap>
#include <QModelIndexList>
#include <QPoint>
#include <QString>
#include <Standard_WarningsRestore.hxx>

class Message_Report;
class MessageModel_TreeModel;

class QAction;
class QItemSelectionModel;
class QMenu;
class QWidget;

//! \class MessageView_ActionsTest
//! Window that unites all MessageView controls.
class MessageView_ActionsTest : public QObject
{
  Q_OBJECT
public:
  //! Constructor
  MessageView_ActionsTest(QWidget*                theParent,
                          MessageModel_TreeModel* theTreeModel,
                          QItemSelectionModel*    theModel);

  //! Destructor
  virtual ~MessageView_ActionsTest() {}

  //! Fills popup menu with actions depending on the current selection
  //! \param theSelectedIndices tree model selected indices
  //! \param theMenu menu to be filled
  Standard_EXPORT void AddMenuActions(const QModelIndexList& theSelectedIndices, QMenu* theMenu);

public slots:
  //! Sending several alerts to check metric of message-alert-tool mechanizm
  void OnTestMetric();

  //! Sending several alerts to check property panel/presentations of messenger-alert-tool mechanizm
  void OnTestMessenger();

  //! Check tree of alerts
  void OnTestReportTree();

protected:
  //! Returns report of selected tree view item if a report item is selected
  //! \param theReportIndex tree model index of the found report
  //! \return report instance or NULL
  Handle(Message_Report) getSelectedReport(QModelIndex& theReportIndex) const;

protected:
  MessageModel_TreeModel*                 myTreeModel;      //< tree model
  QItemSelectionModel*                    mySelectionModel; //< selection model
  QMap<MessageModel_ActionType, QAction*> myActions;        //!< container of all actions
};

#endif
