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

#ifndef MessageModel_Actions_H
#define MessageModel_Actions_H

#include <Standard.hxx>
#include <Standard_Transient.hxx>

#include <inspector/MessageModel_ActionType.hxx>
#include <inspector/TInspectorAPI_PluginParameters.hxx>

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

//! \class MessageModel_Actions
//! \brief This is a listener of popup context menu items and selection change in message model
class MessageModel_Actions : public QObject
{
  Q_OBJECT

public:
  //! Constructor
  Standard_EXPORT MessageModel_Actions(QWidget*                theParent,
                                       MessageModel_TreeModel* theTreeModel,
                                       QItemSelectionModel*    theModel);

  //! Destructor
  virtual ~MessageModel_Actions() Standard_OVERRIDE {}

  //! Returns action by the type
  //! \param theType an action type
  //! \return an action instance if it exists
  Standard_EXPORT QAction* GetAction(const MessageModel_ActionType& theType);

  //! Fills popup menu with actions depending on the current selection
  //! \param theSelectedIndices tree model selected indices
  //! \param theMenu menu to be filled
  Standard_EXPORT void AddMenuActions(const QModelIndexList& theSelectedIndices, QMenu* theMenu);

  //! Sets parameters container, it should be used when the plugin is initialized or in update
  //! content \param theParameters a parameters container
  void SetParameters(const Handle(TInspectorAPI_PluginParameters)& theParameters)
  {
    myParameters = theParameters;
  }

public slots:
  //! Set selected report active
  void OnActivateReport();

  //! Set selected report not active
  void OnDeactivateReport();

  //! Clears container of alerts of selected report
  void OnClearReport();

  //! Exports the first selected shape into ShapeViewer plugin.
  void OnExportToShapeView();

protected:
  //! Returns report of selected tree view item if a report item is selected
  //! \param theReportIndex tree model index of the found report
  //! \return report instance or NULL
  Handle(Message_Report) getSelectedReport(QModelIndex& theReportIndex) const;

protected:
  MessageModel_TreeModel*                 myTreeModel;      //< tree model
  QItemSelectionModel*                    mySelectionModel; //< selection model
  Handle(TInspectorAPI_PluginParameters)  myParameters;     //!< plugins parameters container
  QMap<MessageModel_ActionType, QAction*> myActions;        //!< container of all actions
};

#endif
