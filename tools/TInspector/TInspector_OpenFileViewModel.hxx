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

#ifndef TInspectorEXE_OpenFileViewModel_H
#define TInspectorEXE_OpenFileViewModel_H

#include <Standard_Macro.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QAbstractTableModel>
#include <QStringList>
#include <QItemDelegate>
#include <Standard_WarningsRestore.hxx>

class QObject;
class QPainter;

//! \class TInspectorEXE_OpenFileItemDelegate
//! Draws large(40x40) icons in cell. The icon background in colored in highlight when mouse is over button
class TInspectorEXE_OpenFileItemDelegate : public QItemDelegate
{

public:

  //! Constructor
  TInspectorEXE_OpenFileItemDelegate (QObject* theParent, const QColor& theHighlightColor)
  : QItemDelegate (theParent), myColor(theHighlightColor) {}

  //! Destructor
  virtual ~TInspectorEXE_OpenFileItemDelegate() {}

  //! Draws an icon in the cell
  //! \param thePainter a painter
  //! \param theOption a paint options
  //! \param theIndex a view index
  virtual void paint (QPainter* thePainter, const QStyleOptionViewItem& theOption,
                      const QModelIndex& theIndex) const Standard_OVERRIDE;

private:

  QColor myColor; //!< highlight color
};

//! \class TInspector_OpenFileViewModel
//! Table model that visualizes container of string values (file names)
//! Table orientation is horizontal, it has 1 row, number of columns equals to number of values
class TInspector_OpenFileViewModel : public QAbstractTableModel
{

public:

  //! Constructor
  TInspector_OpenFileViewModel (QObject* theParent = 0) : QAbstractTableModel (theParent) {}

  //! Destructor
  virtual ~TInspector_OpenFileViewModel() {}

  //! Store values
  //! \param theValues a container of values to fill model
  void Init (const QStringList& theValues);

  //! Returns content of the model index for the given role, it is obtained from internal container of values
  //! It returns value only for DisplayRole.
  //! \param theIndex a model index
  //! \param theRole a view role
  //! \return value interpreted depending on the given role
  virtual QVariant data (const QModelIndex& theIndex, int theRole = Qt::DisplayRole) const Standard_OVERRIDE;

  //! Returns number of rows
  //! \param theParent an index of the parent item
  //! \return an integer value
  virtual int rowCount (const QModelIndex& theParent = QModelIndex()) const Standard_OVERRIDE
  { (void)theParent; return 1; }

  //! Returns number of columns
  //! \param theParent an index of the parent item
  //! \return an integer value
  virtual int columnCount (const QModelIndex& theParent = QModelIndex()) const Standard_OVERRIDE
  { (void)theParent; return myValues.size(); }

private:

  QStringList myValues; //!< file names
};

#endif
