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

#ifndef DFBrowserPane_AttributePaneSelector_H
#define DFBrowserPane_AttributePaneSelector_H

#include <Standard.hxx>

#ifdef _MSC_VER
#pragma warning(disable : 4127) // conditional expression is constant
#endif

#include <QObject>
#include <QItemSelection>
#include <list>

class QItemSelectionModel;

//! \class DFBrowserPane_AttributePaneSelector
//! \brief Container of active selection models. It connects to selection changed signal of the models and
//! emits one signal for any selection.
class DFBrowserPane_AttributePaneSelector : public QObject
{
  Q_OBJECT
public:

  //! Constructor
  Standard_EXPORT DFBrowserPane_AttributePaneSelector(QObject* theParent);

  //! Destructor
  Standard_EXPORT virtual ~DFBrowserPane_AttributePaneSelector();

  //! Fills the pane selection by the given models. Disconnect it from the previous model and connect to new models
  //! \param theModels a list of selection models
  Standard_EXPORT void  SetCurrentSelectionModels(const std::list<QItemSelectionModel*>& theModels);

  //! Clears selection in all selection models using block for selection changed flag
  Standard_EXPORT void ClearSelected();

signals:

  //! Signal about selection changed in the model
  //! \param theSelected selected items
  //! \param theDeselected deselected items
  //! \param theModel a selection model where the selection happens
  void tableSelectionChanged (const QItemSelection& theSelected, const QItemSelection& theDeselected,
                              QItemSelectionModel* theModel);
protected slots:

  //! Listens selectionChanged() of the model and emits signal tableSelectionChanged filled with the selection model
  //! \param theSelected selected items
  //! \param theDeselected deselected items
  void onTableSelectionChanged (const QItemSelection& theSelected, const QItemSelection& theDeselected);

private:

  std::list<QItemSelectionModel*> mySelectionModels; //!< container of selection models
  bool mySendSelectionChangeBlocked; //!< flag is selection processing should not performed, avoid cyclic dependency
};

#endif
