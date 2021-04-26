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

#ifndef ViewControl_TableItemDelegate_H
#define ViewControl_TableItemDelegate_H

#include <Standard_Macro.hxx>
#include <inspector/ViewControl_EditType.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QItemDelegate>
#include <Standard_WarningsRestore.hxx>

class ViewControl_TableModelValues;

//! \class ViewControl_TableItemDelegate
//! \brief This is an implementation for table cell editor
class ViewControl_TableItemDelegate : public QItemDelegate
{
public:

  //! Constructor
  //! \param theParent parent object
  Standard_EXPORT ViewControl_TableItemDelegate (QObject* theParent = 0);

  //! Destructor
  virtual ~ViewControl_TableItemDelegate() Standard_OVERRIDE {}

  //! Sets table model values
  //! \param theModelValues instance of model values
  void SetModelValues (ViewControl_TableModelValues* theModelValues) { myModelValues = theModelValues; }

  //! Creates widget editor: spin box, combo box or line edit
  //! \param theParent parent widget
  //! \param theOption style option
  //! \param theIndex index of requested widget
  virtual QWidget* createEditor (QWidget* theParent, const QStyleOptionViewItem& theOption,
                                 const QModelIndex& theIndex) const Standard_OVERRIDE;

  //! Sets the data to be displayed and edited by the editor from the data model item specified by the model index
  //! \param theEditor editor to be filled
  //! \param theIndex index of requested widget, contains information about model
  virtual void setEditorData (QWidget* theEditor, const QModelIndex& theIndex) const Standard_OVERRIDE;

  //! Gets data from the editor widget and stores it in the specified model at the item index.
  //! \param theEditor editor to be filled
  //! \param theModel data model
  //! \param theIndex index of requested widget, contains information about model
  virtual void setModelData (QWidget* theEditor, QAbstractItemModel* theModel,
                             const QModelIndex& theIndex) const Standard_OVERRIDE;

private:
  //! Creates an editor
  //! \param theParent parent widget
  //! \param theEditType edition control type
  //! \return edit control
  static QWidget* createEditorControl (QWidget* theParent, const ViewControl_EditType theEditType);

  //! Inits an editor by model values parameters
  //! \param theEditor editor
  //! \param theEditType edition control type
  //! \param theModelValues custom implementation to provide parameters
  //! \param theRow a model index row
  //! \param theColumn a model index column
  //! \return edit control
  static void initEditorParameters (QWidget* theEditor, const ViewControl_EditType theEditType,
                                    ViewControl_TableModelValues* theModelValues,
                                    const int theRow, const int theColumn);

  //! Sets editor value
  //! \param theEditor editor
  //! \param theEditType editor typ
  //! \param theValue new value
  void setEditorValue (QWidget* theEditor, const ViewControl_EditType theEditType, const QVariant& theValue) const;

  //! Returns value of spin box editor
  //! \param theEditor editor
  //! \param theEditType editor typ
  //! \return current value
  static QVariant getEditorValue (QWidget* theEditor, const ViewControl_EditType theEditType);

private:
  ViewControl_TableModelValues* myModelValues; //!< table model values
};

#endif
