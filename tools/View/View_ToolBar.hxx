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

#ifndef View_ToolBar_H
#define View_ToolBar_H

#include <AIS_InteractiveContext.hxx>
#include <Standard.hxx>
#include <inspector/View_ContextType.hxx>
#include <inspector/View_ToolActionType.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QObject>
#include <QMap>
#include <Standard_WarningsRestore.hxx>

class QComboBox;
class QToolButton;
class QWidget;

//! \class View_ToolBar
//! \brief This is a container of the next view actions:
//! - selection of interactive context
//! - actions of View_ToolActionType enumeration
//!
//! It contains container of possible interactive contexts where the presentations may be visualized
//! and choice control to select an active context.
//! If action is clicked, a signal about is sent.
class View_ToolBar : public QObject
{
  Q_OBJECT

public:
  //! Constructor
  Standard_EXPORT View_ToolBar(QWidget* theParent, const bool isUseKeepView = true);

  //! Destructor
  virtual ~View_ToolBar() {}

  //! \returns parent widget of actions
  QWidget* GetControl() const { return myMainWindow; }

  //! Appends context for the given type
  //! \param theType a context type
  //! \param theContext a context
  Standard_EXPORT void SetContext(View_ContextType                      theType,
                                  const Handle(AIS_InteractiveContext)& theContext);

  //! \returns type of active item of context selector
  Standard_EXPORT View_ContextType CurrentContextType() const;

  //! Sets current context type
  //! \param theType a context type
  Standard_EXPORT void SetCurrentContextType(View_ContextType theType);

  //! Sets default context type
  //! \param theType a context type
  void SetDefaultContextType(const View_ContextType theType)
  {
    myDefaultContextType = (int)theType;
  }

  //! \returns an active context of context selector
  Standard_EXPORT Handle(AIS_InteractiveContext) CurrentContext() const;

  //! \returns whether the action is checked(toggled). Acceptable only if the action is checkable.
  Standard_EXPORT bool IsActionChecked(const int theActionId) const;

  //! Saves state of tool bar actions
  //! \param theToolBar a view instance
  //! \param[out] theItems  properties
  //! \param thePrefix preference item prefix
  Standard_EXPORT static void SaveState(View_ToolBar*           theToolBar,
                                        QMap<QString, QString>& theItems,
                                        const QString&          thePrefix = QString());
  //! Restores state of tool bar actions
  //! \param theToolBar a view instance
  //! \param theKey property key
  //! \param theValue property value
  //! \param thePrefix preference item prefix
  //! \return boolean value whether the property is applied to the tree view
  Standard_EXPORT static bool RestoreState(View_ToolBar*  theToolBar,
                                           const QString& theKey,
                                           const QString& theValue,
                                           const QString& thePrefix = QString());

signals:

  //! Signal about click on action of View_ToolActionType enumeration
  void actionClicked(int theActionId);

  //! Signal about selection of context
  void contextChanged();

private slots:

  //! Processes clicked action and emit signal with action identifier
  void onActionClicked();

private:
  QWidget*   myMainWindow;         //!< tool bar parent widget
  QComboBox* myViewSelector;       //!< container of possible contexts
  int        myDefaultContextType; //!< type of context read from preferences

  QMap<View_ToolActionType, QToolButton*>                myActionsMap;       //!< tool actions
  QMap<View_ContextType, Handle(AIS_InteractiveContext)> myViewContexts;     //!< contexts
  QMap<View_ContextType, QString>                        myViewContextNames; //!< names of contexts
};

#endif
