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

#ifndef VInspector_ToolBar_H
#define VInspector_ToolBar_H

#include <Standard.hxx>
#include <Standard_Macro.hxx>
#include <inspector/VInspector_ToolActionType.hxx>

#ifdef _MSC_VER
  #pragma warning(disable : 4127 4718) // conditional expression is constant, recursive call has no side effects
#endif
#include <QMap>
#include <QObject>

class QWidget;
class QPushButton;

//! \class VInspector_ToolBar
//! Container of View tool bar actions
class VInspector_ToolBar : public QObject
{
  Q_OBJECT

public:

  //! Constructor
  Standard_EXPORT VInspector_ToolBar (QWidget* theParent);

  //! Destructor
  virtual ~VInspector_ToolBar() Standard_OVERRIDE {}

  //! Returns main control
  QWidget* GetControl() const { return myMainWindow; }

  //! Returns tool button by action index
  //! \param theActionId index of action
  Standard_EXPORT QPushButton* GetToolButton (const VInspector_ToolActionType& theActionId) const;

signals:

  //! Signal about action click
  //! \param theActionId an action index
  void actionClicked (int theActionId);

private slots:

  //! Provides switch for action. Emits signal about action click
  void onActionClicked();

private:

  QWidget* myMainWindow; //!< the main control
  QMap<VInspector_ToolActionType, QPushButton*> myActionsMap; //!< container of type into button
};

#endif
