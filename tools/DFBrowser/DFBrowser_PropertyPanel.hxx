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

#ifndef DFBrowser_PropertyPanel_H
#define DFBrowser_PropertyPanel_H

#include <inspector/TreeModel_ItemBase.hxx>

#include <Standard.hxx>

#include <QItemSelection>
#include <QObject>

class DFBrowser_AttributePaneStack;

class QAbstractItemModel;
class QWidget;

//! \class DFBrowser_PropertyPanel
//! Control that contains attribute pane stack
class DFBrowser_PropertyPanel : public QObject
{
  Q_OBJECT
public:

  //! Constructor
  Standard_EXPORT DFBrowser_PropertyPanel (QWidget* theParent);

  //! Destructor
  virtual ~DFBrowser_PropertyPanel() Standard_OVERRIDE {}

  //! Returns main control
  QWidget* GetControl() const { return myMainWindow; }

  //! Returns instance of attributes stack
  //! \return stack
  DFBrowser_AttributePaneStack* GetAttributesStack() { return myAttributesStack; }

  //! Fills attributes stack by selected index
  //! \param theSelected selected items
  //! \param theDeselected deselected items
  Standard_EXPORT void UpdateBySelectionChanged (const QItemSelection& theSelected,
                                                 const QItemSelection& theDeselected);
private:

  QWidget* myMainWindow; //! < parent of attribute stack control
  DFBrowser_AttributePaneStack* myAttributesStack; //! < panes stack
};
#endif
