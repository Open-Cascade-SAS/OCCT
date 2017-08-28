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

#ifndef DFBrowserPane_AttributePaneModel_H
#define DFBrowserPane_AttributePaneModel_H

#include <Standard.hxx>
#include <TDocStd_Document.hxx>

#ifdef _MSC_VER
#pragma warning(disable : 4127) // conditional expression is constant
#endif
#include <QAbstractTableModel>
#include <QList>
#include <QVariant>

//! \class DFBrowserPane_AttributePaneModel
//! \brief This is an extension of table model to visualize a container of values
//! It is possible to:
//! - set orientation to interpretate the values.
//! - set table view header values.
//! Items of the view are enabled and selectable.
class DFBrowserPane_AttributePaneModel : public QAbstractTableModel
{
public:

  //! Constructor
  Standard_EXPORT DFBrowserPane_AttributePaneModel(QObject* theParent = 0);

  //! Destructor
  virtual ~DFBrowserPane_AttributePaneModel() {}

  //! Sets direction of the values applying, whether it should be placed by rows or by columns
  //! \param theOrientation if horizontal, the values are applyed by rows, otherwise by columns
  void SetOrientation (const Qt::Orientation& theOrientation) { myOrientation = theOrientation; }

  //! Returns table orientation for setting data values
  //! \return thye current orientation
  Qt::Orientation GetOrientation() const { return myOrientation; }

  //! Sets number of columns
  //! \param theColumnCount a column count
  void SetColumnCount (const int theColumnCount) { myColumnCount = theColumnCount; }

  //! Fills the model with the values. Store the values in a cache.
  //! \param theValues a container of values
  Standard_EXPORT void Init (const QList<QVariant>& theValues);

  //! Fills the model header values for orientation.
  //! \param theValues a container of header text values
  //! \param theOrientation an orientation of header
  Standard_EXPORT void SetHeaderValues (const QList<QVariant>& theValues, Qt::Orientation theOrientation);

  //! Returns header values for orientation.
  //! \param theValues a container of header text values
  //! \param theOrientation an orientation of header
  const QList<QVariant>& HeaderValues (Qt::Orientation theOrientation)
    { return theOrientation == Qt::Horizontal ? myHorizontalHeaderValues : myVerticalHeaderValues; }

  //! Returns indices of italic columns
  //! \return indices of columns
  const QList<int>& GetItalicColumns() const { return myItalicColumns; }

  //! Sets indices of italic columns
  //! \param theValues indices of columns
  void SetItalicColumns (const QList<int>& theValues) { myItalicColumns = theValues; }

  //! Returns number of columns, depending on orientation: myColumnCount or size of values container
  //! \param theParent an index of the parent item
  //! \return an integer value
  Standard_EXPORT virtual int columnCount (const QModelIndex& theParent = QModelIndex()) const Standard_OVERRIDE;

  //! Returns number of rows, depending on orientation: myColumnCount or size of values container
  //! \param theParent an index of the parent item
  //! \return an integer value
  Standard_EXPORT virtual int rowCount (const QModelIndex& theParent = QModelIndex()) const Standard_OVERRIDE;

  //! Returns content of the model index for the given role, it is obtained from internal container of values
  //! It returns value only for DisplayRole.
  //! \param theIndex a model index
  //! \param theRole a view role
  //! \return value intepreted depending on the given role
  Standard_EXPORT virtual QVariant data (const QModelIndex& theIndex, int theRole = Qt::DisplayRole) const Standard_OVERRIDE;

  //! Returns content of the model index for the given role, it is obtainer from internal container of header values
  //! It returns value only for DisplayRole.
  //! \param theSection an index of value in the container 
  //! \param theIndex a model index
  //! \param theRole a view role
  //! \return value intepreted depending on the given role
  Standard_EXPORT virtual QVariant headerData (int theSection, Qt::Orientation theOrientation, int theRole = Qt::DisplayRole) const Standard_OVERRIDE;

  //! Returns flags for the item: ItemIsEnabled | Qt::ItemIsSelectable
  //! \param theIndex a model index
  //! \return flags
  Qt::ItemFlags flags (const QModelIndex& theIndex) const
  { return theIndex.isValid() ? Qt::ItemIsEnabled | Qt::ItemIsSelectable : Qt::NoItemFlags; }

private:

  Qt::Orientation myOrientation; //!< orientation how the values should fill the current table view
  int myColumnCount; //!< number of table columns
  QMap< int, QList<QVariant> > myValuesMap; //!< container of values, filled in Init(), used in data()
  QList<QVariant> myHorizontalHeaderValues; //!< table horizontal header values
  QList<QVariant> myVerticalHeaderValues; //!< table vertical header values
  QList<int> myItalicColumns; //!< indices of columns that should be visualized in gray and italic

};

#endif
