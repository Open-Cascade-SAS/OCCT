// Created on: 2021-04-27
// Created by: Svetlana SHUTINA
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

#pragma once

#include <inspector/TreeModel_ItemBase.hxx>

#include <Message_Alert.hxx>
#include <Message_MetricType.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QAbstractTableModel>
#include <Standard_WarningsRestore.hxx>

//! @class MessageView_MetricStatisticModel
//! Table model that sums for parameter alert the number of calls and
//! metric time spent on the alert and its children.
//! It visualizes a table with statistic information:
//! the 1st column is alert name, the 2nd column is a counter of the name appearance,
//! the 3rd column is the cummulative time.
//! Tables rows are sorted by descending time.
class MessageView_MetricStatisticModel : public QAbstractTableModel
{
private:
  // Struct to describe a row of the table
  struct RowValues
  {
    QString myName; //!< string values
    int myCounter;  //!< count of the values
    double myTime;  //!< total time
  };

public:
  //! Constructor
  MessageView_MetricStatisticModel (const Message_MetricType& theType, QObject* theParent = 0)
   : QAbstractTableModel (theParent), myMetricType (theType) {}

  //! Destructor
  virtual ~MessageView_MetricStatisticModel() {}

  //! Fills map of the fields values
  //! \param theItemBase a parent item.
  Standard_EXPORT void Init (const TreeModel_ItemBasePtr theItemBase);

  //! Returns content of the model index for the given role,
  //! it is obtained from internal container of values.
  //! It returns value only for DisplayRole
  //! \param theIndex a model index
  //! \param theRole a view role
  //! \return value intepreted depending on the given role
  Standard_EXPORT virtual QVariant data (const QModelIndex& theIndex, int theRole = Qt::DisplayRole) const Standard_OVERRIDE;

  //! Returns number of rows
  //! \param theParent an index of the parent item
  //! \return an integer value
  Standard_EXPORT virtual int rowCount (const QModelIndex& theParent = QModelIndex()) const Standard_OVERRIDE
  { (void)theParent; return myValues.size(); }

  //! Returns number of columns
  //! \param theParent an index of the parent item
  //! \return an integer value
  Standard_EXPORT virtual int columnCount (const QModelIndex& theParent = QModelIndex()) const Standard_OVERRIDE
  { (void)theParent; return 3; }

private:

  //! Sorts values and fills map of the fields values depends on unique text identifier. It's recursive.
  //! \param theAlert unique text identifier. The alert should have attribute of the metric type
  void appendAlert (const Handle(Message_Alert)& theAlert);

  //! Adds theValues in the map to position theIndex
  //! If theIndex is -1, the element will be added in the end of the map
  //! \param theIndex the serial number in the map
  //! \param theValues the field values
  void setValueByIndex (const int theIndex, const RowValues theValues);

private:
  Message_MetricType myMetricType;             //!< current metric type
  QMap<QString, QPair<int, double> > myValues; //!< map of fields values
  QMap<int, RowValues> mySortValues;           //!< sorted map of fields values
};
